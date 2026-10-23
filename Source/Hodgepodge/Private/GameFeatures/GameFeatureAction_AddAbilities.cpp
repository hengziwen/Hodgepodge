/** @file GameFeatureAction_AddAbilities.cpp
 * @brief 实现GameFeature能力注入动作相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

// GameFeature：动态给指定 Actor 添加 Ability、AttributeSet、AbilitySet 的实现。
#include "GameFeatures/GameFeatureAction_AddAbilities.h"

// GameInstance：用于获取 GameFrameworkComponentManager 子系统。
#include "Engine/GameInstance.h"

// GameFrameworkComponentManager：负责监听 Actor 扩展生命周期，并动态添加组件。
#include "Components/GameFrameworkComponentManager.h"

// 项目自定义的 AbilitySystemComponent。
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "Engine/DataTable.h"
#include "Misc/ScopeExit.h"
#include "UObject/StrongObjectPtr.h"
#include "GameFramework/Pawn.h"
#include "Component/HodgePawnExtensionComponent.h"

// World：用于判断当前是否为有效的游戏世界。
#include "Engine/World.h"

// PlayerState：用于使用项目自定义的 AbilityReady 扩展事件名。
#include "Core/PlayState/HodgePlayerState.h" //@TODO: For the fname

// GameFeatureAction_WorldActionBase：当前 Action 的父类实现。
#include "GameFeatures/GameFeatureAction_WorldActionBase.h"

#if WITH_EDITOR
// 编辑器下的数据验证功能。
#include "Misc/DataValidation.h"
#endif

// 使用当前 CPP 对应的内联生成代码。
#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddAbilities)

// GameFeature 模块使用的本地化文本命名空间。
#define LOCTEXT_NAMESPACE "GameFeatures"

//////////////////////////////////////////////////////////////////////
// UGameFeatureAction_AddAbilities

// GameFeature 开始激活时调用。
void UGameFeatureAction_AddAbilities::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
    const auto Previous = ContextData.FindRef(Context);
    const auto ActiveData = MakeShared<FPerContextData>();
    ContextData.Add(Context, ActiveData);
    if (Previous) { Reset(*Previous); }
    if (ContextData.FindRef(Context) == ActiveData && ActiveData->bActive)
    { Super::OnGameFeatureActivating(Context); }
}

/** 先停止接受本上下文回调，再撤销其来源资源。 */
void UGameFeatureAction_AddAbilities::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
    Super::OnGameFeatureDeactivating(Context);
    const auto ActiveData = ContextData.FindRef(Context);
    ContextData.Remove(Context);
    if (ActiveData) { Reset(*ActiveData); }
}

#if WITH_EDITOR

// 编辑器中用于检查 GameFeature 配置是否合法。
EDataValidationResult UGameFeatureAction_AddAbilities::IsDataValid(FDataValidationContext& Context) const
{
	// 先执行父类的数据验证，再合并当前 Action 自己的验证结果。
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context),
	                                                            EDataValidationResult::Valid);

	// 当前遍历到的 AbilitiesList 下标。
	int32 EntryIndex = 0;

	// 遍历所有 Actor → Ability/Attribute/AbilitySet 配置。
	for (const FGameFeatureAbilitiesEntry& Entry : AbilitiesList)
	{
		// 每一项必须指定目标 Actor 类型。
		if (Entry.ActorClass.IsNull())
		{
			// 当前配置无效。
			Result = EDataValidationResult::Invalid;

			// 告诉编辑器具体是哪一个 Entry 缺少 ActorClass。
			Context.AddError(FText::Format(
				LOCTEXT("EntryHasNullActor", "Null ActorClass at index {0} in AbilitiesList"),
				FText::AsNumber(EntryIndex)));
		}

		// 如果 Ability、Attribute 和 AbilitySet 全部为空，则这条配置实际上什么都不会做。
		if (Entry.GrantedAbilities.IsEmpty() && Entry.GrantedAttributes.IsEmpty() && Entry.GrantedAbilitySets.IsEmpty())
		{
			// 当前配置无效。
			Result = EDataValidationResult::Invalid;

			// 告诉编辑器具体是哪一个 Entry 没有任何可添加内容。
			Context.AddError(FText::Format(LOCTEXT("EntryHasNoAddOns",
			                                       "Index {0} in AbilitiesList will do nothing (no granted abilities, attributes, or ability sets)"), FText::AsNumber(
				                               EntryIndex)));
		}

		// 当前 Entry 中遍历到的 Ability 下标。
		int32 AbilityIndex = 0;

		// 检查当前 Entry 中配置的所有 GameplayAbility。
		for (const FHodgeAbilityGrant& Ability : Entry.GrantedAbilities)
		{
			// Ability 必须指定具体的 Ability 类。
			if (Ability.AbilityType.IsNull())
			{
				// 当前配置无效。
				Result = EDataValidationResult::Invalid;

				// 告诉编辑器具体是哪一个 Ability 配置为空。
				Context.AddError(FText::Format(
					LOCTEXT("EntryHasNullAbility",
					        "Null AbilityType at index {0} in AbilitiesList[{1}].GrantedAbilities"),
					FText::AsNumber(AbilityIndex), FText::AsNumber(EntryIndex)));
			}

			// 移动到下一个 Ability 配置。
			++AbilityIndex;
		}

		// 当前 Entry 中遍历到的 AttributeSet 下标。
		int32 AttributesIndex = 0;

		// 检查当前 Entry 中配置的所有 AttributeSet。
		for (const FHodgeAttributeSetGrant& Attributes : Entry.GrantedAttributes)
		{
			// AttributeSet 必须指定具体的类。
			if (Attributes.AttributeSetType.IsNull())
			{
				// 当前配置无效。
				Result = EDataValidationResult::Invalid;

				// 告诉编辑器具体是哪一个 AttributeSet 配置为空。
				Context.AddError(FText::Format(
					LOCTEXT("EntryHasNullAttributeSet",
					        "Null AttributeSetType at index {0} in AbilitiesList[{1}].GrantedAttributes"),
					FText::AsNumber(AttributesIndex), FText::AsNumber(EntryIndex)));
			}

			// 移动到下一个 AttributeSet 配置。
			++AttributesIndex;
		}

		// 当前 Entry 中遍历到的 AbilitySet 下标。
		int32 AttributeSetIndex = 0;

		// 检查当前 Entry 中配置的所有 AbilitySet。
		for (const TSoftObjectPtr<const UHodgeAbilitySet>& AttributeSetPtr : Entry.GrantedAbilitySets)
		{
			// AbilitySet 必须指定有效的资源引用。
			if (AttributeSetPtr.IsNull())
			{
				// 当前配置无效。
				Result = EDataValidationResult::Invalid;

				// 告诉编辑器具体是哪一个 AbilitySet 配置为空。
				Context.AddError(FText::Format(
					LOCTEXT("EntryHasNullAttributeSet",
					        "Null AbilitySet at index {0} in AbilitiesList[{1}].GrantedAbilitySets"),
					FText::AsNumber(AttributeSetIndex), FText::AsNumber(EntryIndex)));
			}

			// 移动到下一个 AbilitySet 配置。
			++AttributeSetIndex;
		}

		// 移动到下一个 Actor 配置。
		++EntryIndex;
	}

	// 返回最终的数据验证结果。
	return Result;

}
#endif

// GameFeature 被加入某个 World 时调用。
// 这里主要负责向 GameFrameworkComponentManager 注册 Actor 扩展监听。
void UGameFeatureAction_AddAbilities::AddToWorld(const FWorldContext& WorldContext,
    const FGameFeatureStateChangeContext& ChangeContext)
{
    const auto ActiveData = ContextData.FindRef(ChangeContext);
    UWorld* World = WorldContext.World();
    UGameInstance* Instance = WorldContext.OwningGameInstance;
    if (!ActiveData || !ActiveData->bActive || !World || !World->IsGameWorld() || !Instance) { return; }
    auto* Manager = Instance->GetSubsystem<UGameFrameworkComponentManager>();
    if (!Manager) { return; }
    // 下标始终对应原数组，非法空项不会令后续条目错位。
    for (int32 Index = 0; Index < AbilitiesList.Num() && ActiveData->bActive; ++Index)
    {
        if (AbilitiesList[Index].ActorClass.IsNull()) { continue; }
        TWeakPtr<FPerContextData> WeakContext = ActiveData;
        const auto Delegate = UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateWeakLambda(this,
            [this, WeakContext, Index, ChangeContext](AActor* Actor, FName Event)
            {
                const auto Context = WeakContext.Pin();
                if (Context && ContextData.FindRef(ChangeContext) == Context)
                { HandleActorExtension(Actor, Event, Index, ChangeContext); }
            });
        auto Request = Manager->AddExtensionHandler(AbilitiesList[Index].ActorClass, Delegate);
        if (ActiveData->bActive) { ActiveData->ComponentRequests.Add(MoveTemp(Request)); }
    }
}

/** 上下文共享记录在同步撤销回调期间仍存活，已开始的提交随后失败关闭。 */
void UGameFeatureAction_AddAbilities::Reset(FPerContextData& ActiveData)
{
    ActiveData.bActive = false;
    auto Extensions = MoveTemp(ActiveData.ActiveExtensions);
    for (const auto& Actor : Extensions)
    { for (const auto& Entry : Actor.Value) { RetireExtension(Entry.Value.ToSharedRef()); } }
    ActiveData.ComponentRequests.Reset();
}

/** Actor 和配置条目共同确定一次扩展的归属。 */
void UGameFeatureAction_AddAbilities::HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex,
    FGameFeatureStateChangeContext ChangeContext)
{
    const auto ActiveData = ContextData.FindRef(ChangeContext);
    if (!ActiveData || !ActiveData->bActive || !AbilitiesList.IsValidIndex(EntryIndex) || !IsValid(Actor)) { return; }
    if (EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved ||
        EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved ||
        EventName == UHodgePawnExtensionComponent::NAME_AbilitySystemUninitialized)
    { RemoveActorAbilities(Actor, EntryIndex, *ActiveData, EventName == UHodgePawnExtensionComponent::NAME_AbilitySystemUninitialized); }
    else if (EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded ||
        EventName == AHodgePlayerState::NAME_HodgeAbilityReady ||
        EventName == UHodgePawnExtensionComponent::NAME_AbilitySystemInitialized)
    { AddActorAbilities(Actor, EntryIndex, ActiveData.ToSharedRef()); }
}

/** 预检全部分支，未驻留或非法配置不产生任何授予，也不登记为成功。 */
void UGameFeatureAction_AddAbilities::AddActorAbilities(AActor* Actor, int32 EntryIndex,
    const TSharedRef<FPerContextData>& ActiveData)
{
    if (!IsValid(Actor) || !Actor->HasAuthority() || !ActiveData->bActive || !AbilitiesList.IsValidIndex(EntryIndex)) { return; }
    TStrongObjectPtr<AActor> KeepActor(Actor);
    TStrongObjectPtr<UGameFeatureAction_AddAbilities> KeepAction(this);
    auto& Slot = ActiveData->ActiveExtensions.FindOrAdd(Actor).FindOrAdd(EntryIndex);
    if (!Slot) { Slot = MakeShared<FActorExtensions>(); Slot->Actor = Actor; }
    const auto Entry = Slot;
    if (Entry->bApplying || Entry->State == EGrantState::Failed || Entry->State == EGrantState::Removed) { return; }
    Entry->bApplying = true;
    ON_SCOPE_EXIT
    {
        Entry->bApplying = false;
        if (Entry->State == EGrantState::Failed || Entry->State == EGrantState::Removed)
        { RevokeExtension(*Entry, true); }
    };
    auto Fail = [&](const FString& Error)
    {
        if (Entry->State != EGrantState::Removed)
        {
            Entry->State = EGrantState::Failed;
            UE_LOG(LogGameFeatures, Error, TEXT("Ability extension failed: %s Actor=%s Entry=%d: %s (irreversible=%d)"),
                *GetPathName(), *GetNameSafe(Actor), EntryIndex, *Error, Entry->bHasIrreversibleEffects ? 1 : 0);
        }
    };
    // 监听已有 Init State；注册后本函数立即查询，依赖晚到不依靠 Tick 轮询。
    if (!Entry->InitStateHandle.IsValid())
    {
        if (auto* Instance = Actor->GetGameInstance())
        {
            if (auto* Manager = Instance->GetSubsystem<UGameFrameworkComponentManager>())
            {
                TWeakPtr<FPerContextData> WeakContext = ActiveData;
                TWeakObjectPtr<AActor> WeakActor = Actor;
                const auto Delegate = FActorInitStateChangedDelegate::CreateWeakLambda(this,
                    [this, WeakContext, WeakActor, EntryIndex](const FActorInitStateChangedParams&)
                    {
                        const auto Context = WeakContext.Pin();
                        if (Context && WeakActor.IsValid()) { AddActorAbilities(WeakActor.Get(), EntryIndex, Context.ToSharedRef()); }
                    });
                Entry->InitManager = Manager;
                Entry->InitStateHandle = Manager->RegisterAndCallForActorInitState(Actor, NAME_None,
                    HodgeGameplayTags::InitState_DataInitialized, Delegate, false);
            }
        }
    }
    const auto& Config = AbilitiesList[EntryIndex];
    UAbilitySystemComponent* Existing = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, true);
    // 实现 ASC 接口的玩家 Pawn 可能仍在等 PlayerState，不能为其创建第二份 ASC。
    if (!Existing && !Actor->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
    { Existing = FindOrAddComponentForActor<UHodgeAbilitySystemComponent>(Actor, Config, *ActiveData); }
    if (!ActiveData->bActive || Entry->State == EGrantState::Removed) { return; }
    auto* ASC = Cast<UHodgeAbilitySystemComponent>(Existing);
    if (Existing && !ASC) { Fail(TEXT("Actor requires HodgeAbilitySystemComponent")); return; }
    if (!ASC || !ASC->IsRegistered() || !ASC->GetOwnerActor()) { Entry->State = EGrantState::Waiting; return; }
    if (Actor->IsA<APawn>() && ASC->GetAvatarActor() != Actor) { Entry->State = EGrantState::Waiting; return; }
    if (!ASC->IsOwnerActorAuthoritative()) { Fail(TEXT("ASC owner lacks authority")); return; }
    TStrongObjectPtr<UHodgeAbilitySystemComponent> KeepASC(ASC);
    if (Entry->State == EGrantState::Applied)
    {
        if (Entry->GrantedASC.Get() == ASC) { return; }
        RevokeExtension(*Entry, false);
        if (!ActiveData->bActive || Entry->State == EGrantState::Removed) { return; }
        Entry->bCommitted = false;
    }
    Entry->GrantedASC = ASC;
    Entry->State = EGrantState::Applying;
    const TWeakObjectPtr<AActor> Owner = ASC->GetOwnerActor();
    const TWeakObjectPtr<AActor> Avatar = ASC->GetAvatarActor();
    const bool bHadAvatar = Avatar.IsValid();
    auto Current = [&]()
    {
        return ActiveData->bActive && Entry->State == EGrantState::Applying && IsValid(Actor) &&
            !Actor->IsActorBeingDestroyed() && ASC->IsRegistered() && Owner.IsValid() &&
            !Owner->IsActorBeingDestroyed() && ASC->GetOwnerActor() == Owner.Get() &&
            ASC->GetAvatarActor() == Avatar.Get() && (!bHadAvatar || (Avatar.IsValid() && !Avatar->IsActorBeingDestroyed()));
    };

    TArray<const UHodgeAbilitySet*> Sets;
    TArray<TStrongObjectPtr<UObject>> Resources;
    for (const auto& Reference : Config.GrantedAbilitySets)
    {
        const auto* Set = Reference.Get();
        if (!Set) { Fail(FString::Printf(TEXT("AbilitySet is missing or not resident: %s"), *Reference.ToString())); return; }
        Sets.Add(Set); Resources.Emplace(const_cast<UHodgeAbilitySet*>(Set));
    }
    TArray<FText> Errors;
    if (!UHodgeAbilitySet::ValidateBatchForGrant(Sets, ASC, Errors))
    { Fail(Errors[0].ToString()); return; }
    TArray<UClass*> AttributeTypes;
    TSet<UClass*> AbilityTypes;
    for (const auto* Set : Sets)
    {
        for (const auto& Attribute : Set->GetGrantedAttributes()) { AttributeTypes.Add(Attribute.AttributeSet); }
        for (const auto& Ability : Set->GetGrantedAbilities()) { AbilityTypes.Add(Ability.Ability); }
    }
    for (const auto& Attribute : Config.GrantedAttributes)
    {
        UClass* Type = Attribute.AttributeSetType.Get();
        if (!Type || Type->HasAnyClassFlags(CLASS_Abstract))
        { Fail(FString::Printf(TEXT("AttributeSet is invalid or not resident: %s"), *Attribute.AttributeSetType.ToString())); return; }
        for (const UClass* Other : AttributeTypes)
        {
            if (UHodgeAbilitySet::AreAttributeSetTypesConflicting(Type, Other))
            { Fail(TEXT("Duplicate AttributeSet in extension entry")); return; }
        }
        for (const UAttributeSet* Set : ASC->GetSpawnedAttributes())
        {
            if (Set && UHodgeAbilitySet::AreAttributeSetTypesConflicting(Type, Set->GetClass()))
            { Fail(TEXT("AttributeSet is already present on ASC")); return; }
        }
        AttributeTypes.Add(Type); Resources.Emplace(Type);
        if (!Attribute.InitializationData.IsNull())
        {
            auto* Table = Attribute.InitializationData.Get();
            if (!Table || Table->GetRowStruct() != FAttributeMetaData::StaticStruct())
            { Fail(TEXT("Attribute initialization table is invalid or not resident")); return; }
            Resources.Emplace(Table);
        }
    }
    for (const auto& Ability : Config.GrantedAbilities)
    {
        UClass* Type = Ability.AbilityType.Get();
        if (!Type || Type->HasAnyClassFlags(CLASS_Abstract) || AbilityTypes.Contains(Type))
        { Fail(FString::Printf(TEXT("Ability is invalid, duplicated or not resident: %s"), *Ability.AbilityType.ToString())); return; }
        AbilityTypes.Add(Type); Resources.Emplace(Type);
    }
    if (!Current()) { Fail(TEXT("Binding changed during preflight")); return; }
    for (const auto& Attribute : Config.GrantedAttributes)
    {
        auto* Set = NewObject<UAttributeSet>(ASC->GetOwner(), Attribute.AttributeSetType.Get());
        if (!Attribute.InitializationData.IsNull()) { Set->InitFromMetaDataTable(Attribute.InitializationData.Get()); }
        ASC->AddAttributeSetSubobject(Set); Entry->GrantedHandles.AddAttributeSet(Set);
        if (!Current()) { Fail(TEXT("Binding changed during AttributeSet grant")); return; }
    }
    const auto Context = Actor->IsA<APawn>() ? EHodgeAbilityGrantContext::AvatarBound : EHodgeAbilityGrantContext::OwnerInitialized;
    const auto Result = UHodgeAbilitySet::TryGiveBatchToAbilitySystem(Sets, ASC, Entry->GrantedHandles, nullptr, false, Context);
    Entry->bHasIrreversibleEffects |= Result.bHasIrreversibleEffects;
    if (!Result.bSucceeded || !Current()) { Fail(Result.Error.IsEmpty() ? TEXT("Extension invalidated during AbilitySet grant") : Result.Error); return; }
    for (const auto& Ability : Config.GrantedAbilities)
    {
        const auto* CDO = Cast<UHodgeGameplayAbility>(Ability.AbilityType.Get()->GetDefaultObject());
        Entry->bHasIrreversibleEffects |= !CDO || CDO->GetActivationPolicy() == EHodgeAbilityActivationPolicy::OnSpawn;
        const auto Handle = ASC->GiveAbility(FGameplayAbilitySpec(Ability.AbilityType.Get()));
        Entry->DirectAbilities.Add(Handle);
        if (!Current() || !Handle.IsValid() || !ASC->FindAbilitySpecFromHandle(Handle))
        { Fail(TEXT("Direct ability grant failed or binding changed")); return; }
    }
    if (!Current() || !Entry->GrantedHandles.AreValidFor(ASC))
    { Fail(TEXT("Granted resources disappeared before commit")); return; }
    for (const auto Handle : Entry->DirectAbilities)
    {
        if (!ASC->FindAbilitySpecFromHandle(Handle))
        { Fail(TEXT("Direct ability disappeared before commit")); return; }
    }
    Entry->bCommitted = true;
    Entry->State = EGrantState::Applied;
}

/** 仅移除当前条目，其他匹配同 Actor 的来源继续有效。 */
void UGameFeatureAction_AddAbilities::RemoveActorAbilities(AActor* Actor, int32 EntryIndex, FPerContextData& ActiveData, bool bCancelDirect)
{
    auto* Entries = ActiveData.ActiveExtensions.Find(Actor);
    if (!Entries) { return; }
    TSharedPtr<FActorExtensions> Entry;
    Entries->RemoveAndCopyValue(EntryIndex, Entry);
    if (Entries->IsEmpty()) { ActiveData.ActiveExtensions.Remove(Actor); }
    if (Entry) { RetireExtension(Entry.ToSharedRef(), bCancelDirect); }
}

/** 停止依赖回调，提交中的资源由退出作用域统一清理。 */
void UGameFeatureAction_AddAbilities::RetireExtension(const TSharedRef<FActorExtensions>& Entry, bool bCancelDirect)
{
    Entry->State = EGrantState::Removed;
    Entry->bCancelDirectOnRemoval |= bCancelDirect;
    if (Entry->InitManager.IsValid() && Entry->Actor.IsValid() && Entry->InitStateHandle.IsValid())
    { Entry->InitManager->UnregisterActorInitStateDelegate(Entry->Actor.Get(), Entry->InitStateHandle); }
    Entry->InitStateHandle.Reset();
    if (!Entry->bApplying) { RevokeExtension(*Entry, Entry->bCancelDirectOnRemoval || !Entry->bCommitted); }
}

/** 使用授予时 ASC 和来源句柄撤销，回调重入不会再次遍历原账本。 */
void UGameFeatureAction_AddAbilities::RevokeExtension(FActorExtensions& Entry, bool bCancelDirectAbilities)
{
    auto Abilities = MoveTemp(Entry.DirectAbilities);
    if (auto* ASC = Entry.GrantedASC.Get())
    {
        TStrongObjectPtr<UHodgeAbilitySystemComponent> KeepASC(ASC);
        for (const auto Handle : Abilities)
        {
            if (bCancelDirectAbilities) { ASC->ClearAbility(Handle); }
            else { ASC->SetRemoveAbilityOnEnd(Handle); }
        }
        Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
    }
}

// 查找 Actor 上指定类型的组件；如果没有，则通过 GameFrameworkComponentManager 请求创建。
UActorComponent* UGameFeatureAction_AddAbilities::FindOrAddComponentForActor(
	UClass* ComponentType, AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData)
{
	// 首先直接查找 Actor 上是否已经存在目标组件。
	UActorComponent* Component = Actor->FindComponentByClass(ComponentType);

	// 默认情况下，如果没有组件，就需要向 ComponentManager 请求创建。
	bool bMakeComponentRequest = (Component == nullptr);

	// 如果 Actor 已经存在这个组件，则进一步判断它的来源。
	if (Component)
	{
		// Check to see if this component was created from a different `UGameFrameworkComponentManager` request.
		// 检查这个组件是否可能是由其他 GameFrameworkComponentManager 请求创建的。

		// `Native` is what `CreationMethod` defaults to for dynamically added components.
		// 动态创建组件时 CreationMethod 默认可能为 Native，因此不能单纯通过 CreationMethod 判断来源。
		if (Component->CreationMethod == EComponentCreationMethod::Native)
		{
			// Attempt to tell the difference between a true native component and one created by the GameFrameworkComponent system.
			// 尝试区分真正的 Native 组件和 GameFrameworkComponent 系统创建出来的组件。

			// 获取当前组件对应的 Archetype。
			UObject* ComponentArchetype = Component->GetArchetype();

			// 如果 Archetype 是类默认对象，则认为它可能来自 GameFrameworkComponentManager，
			// 因此需要再次创建一个 ComponentRequest；ComponentRequest 本身是引用计数的。
			bMakeComponentRequest = ComponentArchetype->HasAnyFlags(RF_ClassDefaultObject);
		}
	}

	// 如果需要创建组件请求，则向 GameFrameworkComponentManager 请求指定组件。
	if (bMakeComponentRequest)
	{
		// 获取 Actor 所在的 World。
		UWorld* World = Actor->GetWorld();

		// 获取当前 World 对应的 GameInstance。
		UGameInstance* GameInstance = World->GetGameInstance();

		// 获取 GameFrameworkComponentManager 子系统。
		if (UGameFrameworkComponentManager* ComponentMan = UGameInstance::GetSubsystem<
			UGameFrameworkComponentManager>(GameInstance))
		{
			// 请求给 AbilitiesEntry.ActorClass 类型的 Actor 添加指定类型的组件。
			TSharedPtr<FComponentRequestHandle> RequestHandle = ComponentMan->AddComponentRequest(
				AbilitiesEntry.ActorClass, ComponentType);

			// 保存组件请求句柄，之后 GameFeature 停用时进行统一清理。
			ActiveData.ComponentRequests.Add(RequestHandle);
		}

		// 如果之前没有组件，则请求创建后重新查找一次。
		if (!Component)
		{
			// 从 Actor 上重新获取刚刚创建的组件。
			Component = Actor->FindComponentByClass(ComponentType);

			// 确保组件最终成功创建。
			ensureAlways(Component);
		}
	}

	// 返回找到或创建出来的组件。
	return Component;
}

#undef LOCTEXT_NAMESPACE
