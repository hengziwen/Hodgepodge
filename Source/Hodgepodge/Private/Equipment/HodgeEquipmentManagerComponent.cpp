// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/HodgeEquipmentManagerComponent.h"

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
UHodgeEquipmentInstance* FHodgeEquipmentList::AddEntry(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition)
{
	// 保存最终创建出来的运行时装备实例。
	UHodgeEquipmentInstance* Result = nullptr;

	// EquipmentDefinition 必须有效。
	check(EquipmentDefinition != nullptr);

	// EquipmentList 必须已经绑定有效的 OwnerComponent。
	check(OwnerComponent);

	// 装备列表的新增只能由服务器权威端执行。
	check(OwnerComponent->GetOwner()->HasAuthority());

	// 获取 EquipmentDefinition 的类默认对象，直接读取这类装备的静态配置。
	const UHodgeEquipmentDefinition* EquipmentCDO = GetDefault<UHodgeEquipmentDefinition>(EquipmentDefinition);

	// 从装备定义中取得需要创建的运行时 EquipmentInstance 类型。
	TSubclassOf<UHodgeEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;

	// 没有指定特殊 InstanceType 时退回基础 UHodgeEquipmentInstance。
	if (InstanceType == nullptr)
	{
		InstanceType = UHodgeEquipmentInstance::StaticClass();
	}

	// 在装备列表中创建一条新的默认 Entry，并取得其引用。
	FHodgeAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();

	// 记录当前 Entry 对应的装备定义类型。
	NewEntry.EquipmentDefinition = EquipmentDefinition;

	// 创建这件装备对应的运行时实例，并将 Owner Actor 作为该 UObject 的 Outer。
	NewEntry.Instance = NewObject<UHodgeEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);

	//@TODO: Using the actor instead of component as the outer due to UE-127172

	// 保存新创建的运行时装备实例作为返回值。
	Result = NewEntry.Instance;

	// 获取当前 Pawn 使用的 ASC，装备授予 GAS 内容需要由 ASC 完成。
	if (UHodgeAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		NewEntry.GrantedAbilitySystem = ASC;

		// 遍历当前 EquipmentDefinition 配置的所有 AbilitySet。
		for (const TObjectPtr<const UHodgeAbilitySet>& AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			// 将 AbilitySet 授予 ASC，并把授予句柄记录到当前 Entry 供卸装时精确撤销。
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
		}
	}
	else
	{
		//@TODO: Warning logging?
	}

	// 根据 EquipmentDefinition 配置生成并挂接这件装备需要的表现 Actor。
	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);


	// 标记当前 FastArray Entry 已发生变化，使其能够通过增量复制同步到客户端。
	MarkItemDirty(NewEntry);

	// 返回新创建的运行时装备实例。
	return Result;
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
UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::EquipItem(
	TSubclassOf<UHodgeEquipmentDefinition> EquipmentClass)
{
	// 保存最终创建出来的运行时装备实例。
	UHodgeEquipmentInstance* Result = nullptr;

	// 只有传入有效 EquipmentDefinition 时才执行装备流程。
	if (EquipmentClass != nullptr)
	{
		// 向装备列表添加 Entry，并创建 Instance、授予 AbilitySet、生成装备 Actor。
		Result = EquipmentList.AddEntry(EquipmentClass);

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

	// 返回新创建并装备完成的运行时装备实例。
	return Result;
}

// 在服务器上卸下指定的运行时装备实例。
void UHodgeEquipmentManagerComponent::UnequipItem(UHodgeEquipmentInstance* ItemInstance)
{
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
