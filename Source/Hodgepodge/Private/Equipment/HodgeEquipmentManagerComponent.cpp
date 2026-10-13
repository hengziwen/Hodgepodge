/** @file HodgeEquipmentManagerComponent.cpp
 * @brief 实现装备管理组件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/HodgeEquipmentManagerComponent.h"
#include "Character/HodgeEnemyCharacter.h"
#include "UObject/StrongObjectPtr.h"
#include "AbilitySystem/Stats/HodgeAttributeCoordinator.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Data/HodgeEquipmentStatProfile.h"
#include "Character/HodgeCombatCharacter.h"

// Hodge 自定义 ASC，用于装备时授予和卸下 AbilitySet。
#include "AbilitySystem/HodgeAbilitySystemComponent.h"

// 提供从 Actor 获取 AbilitySystemComponent 的统一接口。
#include "AbilitySystemGlobals.h"

// 提供 UActorChannel，用于传统网络复制路径下手动复制 EquipmentInstance 子对象。
#include "Engine/ActorChannel.h"

// 提供装备静态定义以及 FHodgeEquipmentActorToSpawn 等配置。
#include "Equipment/HodgeEquipmentDefinition.h"

// 提供装备运行时实例。
#include "Equipment/HodgeEquipmentInstance.h"
#include "Equipment/HodgeWeaponInstance.h"

// 提供 DOREPLIFETIME 等 UE 网络复制功能。
#include "Net/UnrealNetwork.h"

// [HODGE-DBG] 提供 GEngine，用于屏幕调试消息。
#include "Engine/Engine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEquipmentManagerComponent)

class FLifetimeProperty;

/** 保存装备MutationScope的数据和接口契约。 */
struct FHodgeEquipmentMutationScope
{
	// 管理器的数据或对象引用。
	UHodgeEquipmentManagerComponent& Manager;
	/** 构造装备MutationScope并初始化其默认状态。
	 * @param InManager 管理器，由调用方传入。
	 */
	explicit FHodgeEquipmentMutationScope(UHodgeEquipmentManagerComponent& InManager) : Manager(InManager) { Manager.bEquipmentMutation = true; }
	/** 销毁该对象，执行当前类型的析构收尾。 */
	~FHodgeEquipmentMutationScope()
	{
		Manager.bEquipmentMutation = false;
		if (Manager.bPendingUninitialize) { Manager.bPendingUninitialize = false; Manager.UninitializeComponent(); }
	}
};
struct FReplicationFlags;

//////////////////////////////////////////////////////////////////////
// FHodgeAppliedEquipmentEntry

// 返回当前已装备条目的调试字符串，包含运行时 Instance 和 EquipmentDefinition 名称。
FString FHodgeAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}

//////////////////////////////////////////////////////////////////////
// FHodgeEquipmentList

// 客户端收到 FastArray 条目删除通知时，在条目真正从本地数组移除前触发卸装生命周期。
void FHodgeEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	// 遍历本次网络复制需要删除的装备条目索引。
	for (int32 Index : RemovedIndices)
	{
		// 获取即将被删除的装备条目。
		const FHodgeAppliedEquipmentEntry& Entry = Entries[Index];

		// 运行时装备实例有效时通知它执行卸装逻辑。
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnUnequipped();
		}
	}
}

// 客户端收到 FastArray 新增条目后触发装备生命周期。
void FHodgeEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	// 遍历本次网络复制新增的装备条目索引。
	for (int32 Index : AddedIndices)
	{
		// 获取刚刚同步到客户端的装备条目。
		const FHodgeAppliedEquipmentEntry& Entry = Entries[Index];

		// 运行时装备实例有效时通知它执行装备逻辑。
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnEquipped();
		}
	}
}

// 客户端收到已有 FastArray 条目内容变化后调用，目前没有额外处理逻辑。
void FHodgeEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	// 延迟映射的实例可能错过首次新增回调，装备入口应能重复初始化。
	for (int32 Index : ChangedIndices)
	{
		if (auto* Weapon = Cast<UHodgeWeaponInstance>(Entries[Index].Instance)) { Weapon->OnEquipped(); }
	}
	// 	for (int32 Index : ChangedIndices)
	// 	{
	// 		const FGameplayTagStack& Stack = Stacks[Index];
	// 		TagToCountMap[Stack.Tag] = Stack.StackCount;
	// 	}
}

// 获取当前装备列表所属 Actor 的 Hodge AbilitySystemComponent。
UHodgeAbilitySystemComponent* FHodgeEquipmentList::GetAbilitySystemComponent() const
{
	// EquipmentList 必须已经绑定有效的 OwnerComponent。
	check(OwnerComponent);

	// 通过管理组件取得真正拥有该组件的 Actor。
	AActor* OwningActor = OwnerComponent->GetOwner();

	// 通过 AbilitySystemGlobals 从所属 Actor 查询 ASC，并转换为 Hodge 自定义 ASC。
	return Cast<UHodgeAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

// 在服务器上根据 EquipmentDefinition 创建并添加一条新的已装备记录。
UHodgeEquipmentInstance* FHodgeEquipmentList::AddEntry(TSubclassOf<UHodgeEquipmentDefinition> Definition, FGuid Id, int32 Level)
{
	auto* Manager = CastChecked<UHodgeEquipmentManagerComponent>(OwnerComponent);
	Manager->LastEquipError.Reset(); Manager->bEquipIrreversibleEffects = false;
	// 后续GAS操作使用的能力系统组件。
	auto* ASC = GetAbilitySystemComponent();
	TArray<FText> Errors;
	const bool bBootstrap = OwnerComponent->GetOwner()->IsA<AHodgeEnemyCharacter>();
	// 按服务器权限区分本次处理分支。
	if (!OwnerComponent->GetOwner()->HasAuthority() ||
		!UHodgeEquipmentManagerComponent::ValidateEquipmentForGrant(Definition, ASC, Level, Errors, bBootstrap))
	{ Manager->LastEquipError = Errors.IsEmpty() ? TEXT("Equipment requires authority") : Errors[0].ToString(); return nullptr; }
	const auto* CDO = GetDefault<UHodgeEquipmentDefinition>(Definition);
	UClass* Type = CDO->InstanceType ? CDO->InstanceType.Get() : UHodgeEquipmentInstance::StaticClass();
	TStrongObjectPtr<UHodgeEquipmentInstance> Instance(NewObject<UHodgeEquipmentInstance>(OwnerComponent->GetOwner(), Type));
	FHodgeAppliedEquipmentEntry Pending;
	Pending.EquipmentDefinition = Definition; Pending.Instance = Instance.Get(); Pending.GrantedAbilitySystem = ASC;
	Instance->SetStatIdentity(Id, Level, CDO->StatProfile);
	TWeakObjectPtr<AActor> Avatar = ASC ? ASC->GetAvatarActor() : nullptr;
	auto Current = [&]() { return ASC && Avatar.IsValid() && !Avatar->IsActorBeingDestroyed() && Manager->IsRegistered() &&
		!Manager->bPendingUninitialize && ASC->GetAvatarActor() == Avatar.Get() && GetAbilitySystemComponent() == ASC; };
	auto Fail = [&](const FString& Error) -> UHodgeEquipmentInstance*
	{
		Manager->LastEquipError = Error;
		if (ASC)
		{
			Pending.GrantedHandles.TakeFromAbilitySystem(ASC);
			if (Pending.AttributeHandle.IsValid()) { ASC->RemoveActiveGameplayEffect(Pending.AttributeHandle); }
		}
		Instance->DestroyEquipmentActors();
		UE_LOG(LogTemp, Warning, TEXT("Equipment grant failed: %s (%s)"), *GetNameSafe(Definition), *Error);
		return nullptr;
	};
	if (!Current()) { return Fail(TEXT("Equipment ASC is not bound to the current Pawn")); }
	// 将构造的效果规格提交给GAS执行属性更新。
	if (CDO->StatProfile)
	{
		float HealthBonus, DamageBonus; CDO->StatProfile->Evaluate(Level, HealthBonus, DamageBonus);
		auto Context = ASC->MakeEffectContext(); Context.AddSourceObject(Instance.Get());
		auto Spec = ASC->MakeOutgoingSpec(CDO->StatProfile->AttributeEffect, Level, Context);
		if (!Spec.IsValid()) { return Fail(TEXT("Equipment attribute spec is invalid")); }
		Spec.Data->SetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_Stat_MaxHealth, HealthBonus);
		Spec.Data->SetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_Stat_BaseDamage, DamageBonus);
		Pending.AttributeHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		if (!Pending.AttributeHandle.IsValid() || !Current()) { return Fail(TEXT("Equipment attribute application failed")); }
	}
	for (const UHodgeAbilitySet* Set : CDO->AbilitySetsToGrant)
	{
		const auto Result = Set->TryGiveToAbilitySystem(ASC, Pending.GrantedHandles, Instance.Get(), bBootstrap);
		Manager->bEquipIrreversibleEffects |= Result.bHasIrreversibleEffects;
		if (!Result.bSucceeded || !Current()) { return Fail(Result.Error.IsEmpty() ? TEXT("Equipment binding changed during grant") : Result.Error); }
	}
	if (!Instance->TrySpawnEquipmentActors(CDO->ActorsToSpawn) || !Current() || !Pending.GrantedHandles.AreValidFor(ASC))
	{ return Fail(TEXT("Equipment actors or granted resources are invalid")); }
	// 只提交完整记录，失败候选从未进入 FastArray 或复制子对象列表。
	const int32 Index = Entries.Add(MoveTemp(Pending));
	MarkItemDirty(Entries[Index]);
	return Instance.Get();
}

// 在服务器上根据 EquipmentInstance 移除对应的已装备记录。
void FHodgeEquipmentList::RemoveEntry(UHodgeEquipmentInstance* Instance)
{
	// 使用迭代器遍历装备列表，便于找到目标后直接删除当前元素。
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		// 获取当前正在检查的装备 Entry。
		FHodgeAppliedEquipmentEntry& Entry = *EntryIt;

		// 找到与目标运行时装备实例对应的 Entry。
		if (Entry.Instance == Instance)
		{
			// 使用授予时的 ASC，避免 Pawn 解绑或换绑后漏撤销或撤销到其他 ASC。
			if (UHodgeAbilitySystemComponent* ASC = Entry.GrantedAbilitySystem.Get())
			{
				// 根据装备时保存的 GrantedHandles 精确撤销 AbilitySet 授予的内容。
                if (Entry.AttributeHandle.IsValid()) { ASC->RemoveActiveGameplayEffect(Entry.AttributeHandle); }
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			// 销毁这件装备实例生成并挂接到 Pawn 身上的装备 Actor。
			Instance->DestroyEquipmentActors();


			// 从服务器装备列表中删除当前 Entry。
			EntryIt.RemoveCurrent();

			// FastArray 发生元素删除，标记整个数组状态已发生变化以同步删除操作。
			MarkArrayDirty();
		}
	}
}

//////////////////////////////////////////////////////////////////////
// UHodgeEquipmentManagerComponent

// 构造 Pawn 的装备管理组件，并将 EquipmentList 绑定到当前组件。
UHodgeEquipmentManagerComponent::UHodgeEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	  , EquipmentList(this)
{
	// 默认启用该组件的网络复制。
	SetIsReplicatedByDefault(true);

	// 要求 UE 调用该组件的 InitializeComponent 生命周期。
	bWantsInitializeComponent = true;
}

// 注册当前组件需要通过网络复制的属性。
void UHodgeEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 保留父类的网络复制属性。
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 注册 EquipmentList，由 FastArray 负责装备列表的增量复制。
	DOREPLIFETIME(ThisClass, EquipmentList);
}

// 在服务器上根据 EquipmentDefinition 为当前 Pawn 装备一件物品。
UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::EquipItem(TSubclassOf<UHodgeEquipmentDefinition> EquipmentClass)
{
    // 先检查对象与上下文，避免在依赖失效后继续处理。
    if (!EquipmentClass) { return nullptr; }
    const UHodgeEquipmentStatProfile* Stats = EquipmentClass->GetDefaultObject<UHodgeEquipmentDefinition>()->StatProfile.Get();
    return EquipItemWithState(EquipmentClass, FGuid::NewGuid(), Stats ? Stats->MinLevel : 1);
}

/** 按已有物品身份和等级创建装备并提交其授予。 */
UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::EquipItemWithState(TSubclassOf<UHodgeEquipmentDefinition> EquipmentClass, FGuid Id, int32 Level)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (bEquipmentMutation) { return nullptr; }
	FHodgeEquipmentMutationScope MutationScope(*this);
    // 按服务器权限区分本次处理分支。
    if (!GetOwner()->HasAuthority() || !Id.IsValid()) { return nullptr; }
    if (auto* Existing = EquipmentList.Entries.FindByPredicate([Id](const auto& Entry) { return Entry.Instance && Entry.Instance->GetEquipmentId() == Id; }))
    { return Existing->EquipmentDefinition == EquipmentClass ? Existing->Instance.Get() : nullptr; }
    // 后续GAS操作使用的能力系统组件。
    auto* ASC = Cast<UHodgeAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
    // 玩家状态对象。
    auto* PS = ASC ? Cast<AHodgePlayerState>(ASC->GetOwnerActor()) : nullptr;
    auto* Coordinator = PS ? PS->GetAttributeCoordinator() : nullptr;
    const bool bBound = Coordinator && Coordinator->IsBoundTo(Cast<APawn>(GetOwner()));
    if (bBound && Coordinator->IsUpdating() && !Coordinator->IsInitializing()) { return nullptr; }
    const bool bOwnUpdate = bBound && !Coordinator->IsUpdating();
    if (bOwnUpdate && !Coordinator->BeginEquipmentUpdate()) { return nullptr; }
	// 保存最终创建出来的运行时装备实例。
	UHodgeEquipmentInstance* Result = nullptr;

	// 只有传入有效 EquipmentDefinition 时才执行装备流程。
	if (EquipmentClass != nullptr)
	{
		// 向装备列表添加 Entry，并创建 Instance、授予 AbilitySet、生成装备 Actor。
		Result = EquipmentList.AddEntry(EquipmentClass, Id, Level);

		// EquipmentInstance 创建成功后继续执行装备生命周期。
		if (Result != nullptr)
		{
			// 在服务器端通知运行时实例已经正式装备。
			Result->OnEquipped();

			// 使用 Registered SubObject List 且组件已经可复制时，将新 Instance 注册为复制子对象。
			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}
		}
	}

    if (Result && Coordinator) { Coordinator->RememberEquipment(Id, EquipmentClass, Level); }
    if (bOwnUpdate) { Coordinator->FinishEquipmentUpdate(Result != nullptr); }
	// 返回新创建并装备完成的运行时装备实例。
	return Result;
}

// 在服务器上卸下指定的运行时装备实例。
void UHodgeEquipmentManagerComponent::UnequipItem(UHodgeEquipmentInstance* ItemInstance)
{
	if (bEquipmentMutation) { return; }
	FHodgeEquipmentMutationScope MutationScope(*this);
    // 按服务器权限区分本次处理分支。
    if (!GetOwner()->HasAuthority() || !ItemInstance) { return; }
    // 后续GAS操作使用的能力系统组件。
    auto* ASC = Cast<UHodgeAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
    // 玩家状态对象。
    auto* PS = ASC ? Cast<AHodgePlayerState>(ASC->GetOwnerActor()) : nullptr;
    auto* Coordinator = PS ? PS->GetAttributeCoordinator() : nullptr;
    const bool bBound = Coordinator && Coordinator->IsBoundTo(Cast<APawn>(GetOwner()));
    if (bBound && Coordinator->IsUpdating() && !Coordinator->IsInitializing()) { return; }
    const bool bOwnUpdate = bBound && !Coordinator->IsUpdating();
    if (bOwnUpdate && !Coordinator->BeginEquipmentUpdate()) { return; }
	// 只有传入有效 EquipmentInstance 时才执行卸装流程。
	if (ItemInstance != nullptr)
	{
		// 使用 Registered SubObject List 时先将该 Instance 从复制子对象列表中注销。
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(ItemInstance);
		}

		// 在服务器端通知运行时实例执行卸装生命周期。
		ItemInstance->OnUnequipped();

		// 从装备列表删除对应 Entry，并撤销 AbilitySet、销毁装备 Actor。
		EquipmentList.RemoveEntry(ItemInstance);
        if (bOwnUpdate) { Coordinator->FinishEquipmentUpdate(true); }
	}
}

// 传统 ActorChannel 复制路径下手动复制所有 EquipmentInstance 子对象。
bool UHodgeEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch,
                                                          FReplicationFlags* RepFlags)
{
	// 先让父类复制自己负责的所有 SubObject。
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	// 遍历当前所有已装备条目。
	for (FHodgeAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		// 获取当前 Entry 对应的运行时 EquipmentInstance。
		UHodgeEquipmentInstance* Instance = Entry.Instance;

		// 只复制仍然有效的运行时装备实例。
		if (IsValid(Instance))
		{
			// 通过当前 ActorChannel 将 EquipmentInstance 作为 Replicated SubObject 进行网络复制。
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	// 返回本次调用是否实际写入了任何网络复制数据。
	return WroteSomething;
}

// 初始化装备管理组件。
void UHodgeEquipmentManagerComponent::InitializeComponent()
{
	// 当前没有额外初始化逻辑，仅执行父类实现。
	Super::InitializeComponent();
    if (auto* Character = Cast<AHodgeCombatCharacter>(GetOwner())) { Character->TryInitializeAttributesAndEquipment(); }

	// [HODGE-DBG] 临时诊断：组件实例真正被创建时打印所属世界与角色。
	UE_LOG(LogTemp, Warning, TEXT("[HODGE-DBG] EquipMgr InitializeComponent Owner=%s NetMode=%d Role=%d"),
	       *GetNameSafe(GetOwner()), static_cast<int32>(GetNetMode()),
	       static_cast<int32>(GetOwnerRole()));

	// [HODGE-DBG] 临时诊断：在屏幕上直接确认组件挂到了哪个角色上。
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20.0f, FColor::Green,
	                                 FString::Printf(TEXT("[HODGE-DBG] EquipMgr created on %s"), *GetNameSafe(GetOwner())));
}

// 组件反初始化时卸下并清理当前 Pawn 的全部装备。
void UHodgeEquipmentManagerComponent::UninitializeComponent()
{
	if (bEquipmentMutation) { bPendingUninitialize = true; return; }
	// 临时保存所有运行时装备实例，避免卸装修改 EquipmentList 导致原迭代器失效。
	TArray<UHodgeEquipmentInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator

	// 先将当前所有 EquipmentInstance 收集到临时数组。
	for (const FHodgeAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	// 再逐个走正常 UnequipItem 流程清理所有装备。
	for (UHodgeEquipmentInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipInstance);
	}

	// 最后执行父类组件反初始化逻辑。
	Super::UninitializeComponent();
}

// 组件进入可网络复制状态时注册已经存在的 EquipmentInstance 子对象。
void UHodgeEquipmentManagerComponent::ReadyForReplication()
{
	// 先执行父类的复制准备逻辑。
	Super::ReadyForReplication();

	// Register existing HodgeEquipmentInstances

	// Registered SubObject List 模式下，将此前已经存在的装备实例全部注册为复制子对象。
	if (IsUsingRegisteredSubObjectList())
	{
		// 遍历当前已经装备的所有 Entry。
		for (const FHodgeAppliedEquipmentEntry& Entry : EquipmentList.Entries)
		{
			// 获取当前 Entry 对应的 EquipmentInstance。
			UHodgeEquipmentInstance* Instance = Entry.Instance;

			// 只注册仍然有效的装备实例。
			if (IsValid(Instance))
			{
				// 将 EquipmentInstance 加入组件的 Replicated SubObject List。
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

// 返回第一个属于指定 EquipmentInstance 类型的已装备实例。
UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::GetFirstInstanceOfType(
	TSubclassOf<UHodgeEquipmentInstance> InstanceType)
{
	// 遍历当前所有已装备 Entry。
	for (FHodgeAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		// 获取当前 Entry 对应的运行时实例。
		if (UHodgeEquipmentInstance* Instance = Entry.Instance)
		{
			// 支持指定类型及其派生类型。
			if (Instance->IsA(InstanceType))
			{
				// 找到第一个符合类型要求的装备实例后立即返回。
				return Instance;
			}
		}
	}

	// 没有找到指定类型的装备实例。
	return nullptr;
}

// 返回所有属于指定 EquipmentInstance 类型的已装备实例。
TArray<UHodgeEquipmentInstance*> UHodgeEquipmentManagerComponent::GetEquipmentInstancesOfType(
	TSubclassOf<UHodgeEquipmentInstance> InstanceType) const
{
	// 保存所有符合类型要求的运行时装备实例。
	TArray<UHodgeEquipmentInstance*> Results;

	// 遍历当前所有已装备 Entry。
	for (const FHodgeAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		// 获取当前 Entry 对应的运行时实例。
		if (UHodgeEquipmentInstance* Instance = Entry.Instance)
		{
			// 支持指定类型及其派生类型。
			if (Instance->IsA(InstanceType))
			{
				// 将符合要求的实例加入结果数组。
				Results.Add(Instance);
			}
		}
	}

	// 返回所有符合类型要求的装备实例。
	return Results;
}

/** 查找InstanceOf定义。 */
UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::FindInstanceOfDefinition(TSubclassOf<UHodgeEquipmentDefinition> Definition) const
{
    const auto* Entry = EquipmentList.Entries.FindByPredicate([Definition](const auto& Item) { return Item.EquipmentDefinition == Definition; });
    return Entry ? Entry->Instance.Get() : nullptr;
}

/** 设置装备等级。 */
bool UHodgeEquipmentManagerComponent::SetEquipmentLevel(UHodgeEquipmentInstance* Instance, int32 Level)
{
	if (bEquipmentMutation) { return false; }
	FHodgeEquipmentMutationScope MutationScope(*this);
    // 按服务器权限区分本次处理分支。
    if (!GetOwner()->HasAuthority() || !Instance) { return false; }
    auto* Entry = EquipmentList.Entries.FindByPredicate([Instance](const auto& Item) { return Item.Instance == Instance; });
    // 先检查对象与上下文，避免在依赖失效后继续处理。
    if (!Entry || !Entry->GrantedAbilitySystem.IsValid() || !Instance->GetStatProfile()) { return false; }
    const auto* Stats = Instance->GetStatProfile();
    float HealthBonus, DamageBonus;
    TArray<FText> Errors;
    if (!Stats->Validate(Errors) || !Stats->Evaluate(Level, HealthBonus, DamageBonus)) { return false; }
    if (Instance->GetEquipmentLevel() == Level && Entry->GrantedAbilitySystem->GetActiveGameplayEffect(Entry->AttributeHandle)) { return true; }
    // 玩家状态对象。
    auto* PS = Cast<AHodgePlayerState>(Entry->GrantedAbilitySystem->GetOwnerActor());
    auto* Coordinator = PS ? PS->GetAttributeCoordinator() : nullptr;
    const bool bBound = Coordinator && Coordinator->IsBoundTo(Cast<APawn>(GetOwner()));
    if (bBound && (Coordinator->IsUpdating() || !Coordinator->BeginEquipmentUpdate())) { return false; }
    bool bSuccess = false;
    // 将构造的效果规格提交给GAS执行属性更新。
    if (Entry->GrantedAbilitySystem->GetActiveGameplayEffect(Entry->AttributeHandle))
    {
        TMap<FGameplayTag, float> Values;
        Values.Add(HodgeGameplayTags::SetByCaller_Stat_MaxHealth, HealthBonus);
        Values.Add(HodgeGameplayTags::SetByCaller_Stat_BaseDamage, DamageBonus);
        Entry->GrantedAbilitySystem->UpdateActiveGameplayEffectSetByCallerMagnitudes(Entry->AttributeHandle, Values);
        Entry->GrantedAbilitySystem->SetActiveGameplayEffectLevel(Entry->AttributeHandle, Level);
        bSuccess = true;
    }
    else
    {
        auto Context = Entry->GrantedAbilitySystem->MakeEffectContext();
        Context.AddSourceObject(Instance);
        auto Spec = Entry->GrantedAbilitySystem->MakeOutgoingSpec(Stats->AttributeEffect, Level, Context);
        if (Spec.IsValid())
        {
            Spec.Data->SetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_Stat_MaxHealth, HealthBonus);
            Spec.Data->SetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_Stat_BaseDamage, DamageBonus);
            Entry->AttributeHandle = Entry->GrantedAbilitySystem->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
            bSuccess = Entry->AttributeHandle.IsValid();
        }
    }
    if (bSuccess)
    {
        Instance->SetStatIdentity(Instance->GetEquipmentId(), Level, Stats);
        EquipmentList.MarkItemDirty(*Entry);
        if (Coordinator) { Coordinator->RememberEquipment(Instance->GetEquipmentId(), Entry->EquipmentDefinition, Level); }
    }
    if (bBound) { Coordinator->FinishEquipmentUpdate(bSuccess); }
    return bSuccess;
}


/** 校验装备关联授予。 */
bool UHodgeEquipmentManagerComponent::ValidateEquipmentForGrant(TSubclassOf<UHodgeEquipmentDefinition> Definition,
	const UHodgeAbilitySystemComponent* ASC, int32 Level, TArray<FText>& Errors, bool bForInitialization)
{
	const int32 Before = Errors.Num();
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!ASC || !Definition || Definition->HasAnyClassFlags(CLASS_Abstract) || Level < 1)
	{ Errors.Add(FText::FromString(TEXT("Missing ASC/equipment Definition or invalid level"))); return false; }
	const auto* CDO = GetDefault<UHodgeEquipmentDefinition>(Definition);
	if (CDO->InstanceType && CDO->InstanceType->HasAnyClassFlags(CLASS_Abstract))
	{ Errors.Add(FText::FromString(TEXT("Equipment instance class is abstract"))); }
	if (CDO->StatProfile)
	{
		float Health, Damage;
		if (!CDO->StatProfile->Validate(Errors) || !CDO->StatProfile->Evaluate(Level, Health, Damage))
		{ Errors.Add(FText::FromString(TEXT("Equipment stat configuration is invalid"))); }
	}
	TArray<const UHodgeAbilitySet*> Sets;
	for (const UHodgeAbilitySet* Set : CDO->AbilitySetsToGrant) { Sets.Add(Set); }
	UHodgeAbilitySet::ValidateBatchForGrant(Sets, ASC, Errors, bForInitialization);
	for (const auto& Actor : CDO->ActorsToSpawn)
	{
		if (!Actor.ActorToSpawn || Actor.ActorToSpawn->HasAnyClassFlags(CLASS_Abstract) || !Actor.AttachTransform.IsValid())
		{ Errors.Add(FText::FromString(TEXT("Equipment actor configuration is invalid"))); }
	}
	return Before == Errors.Num();
}

/** 判断装备就绪。 */
bool UHodgeEquipmentManagerComponent::IsEquipmentReady(const UHodgeEquipmentInstance* Instance) const
{
	const auto* Entry = EquipmentList.Entries.FindByPredicate([Instance](const auto& Item) { return Item.Instance == Instance; });
	return IsRegistered() && !bPendingUninitialize && Entry && IsValid(Instance) && Entry->GrantedAbilitySystem.IsValid() &&
		Entry->GrantedHandles.AreValidFor(Entry->GrantedAbilitySystem.Get()) &&
		(!Instance->GetStatProfile() || Entry->GrantedAbilitySystem->GetActiveGameplayEffect(Entry->AttributeHandle));
}
