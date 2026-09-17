# AbilitySystem 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeGameplayAbility.cpp

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展；PreloadPrimaryAssetsOnGrant 在授予时预加载 Bundle。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/Abilities/HodgeAbilityCost.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h)、[Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)、[Interface/HodgeAbilitySourceInterface.h](../../../Source/Hodgepodge/Public/Interface/HodgeAbilitySourceInterface.h)

定义候选（多行签名仅展示首行）：

- L37: `UHodgeGameplayAbility::UHodgeGameplayAbility(const FObjectInitializer& ObjectInitializer)`
- L66: `UHodgeAbilitySystemComponent* UHodgeGameplayAbility::GetHodgeAbilitySystemComponentFromActorInfo() const`
- L75: `AHodgePlayerController* UHodgeGameplayAbility::GetHodgePlayerControllerFromActorInfo() const`
- L82: `AController* UHodgeGameplayAbility::GetControllerFromActorInfo() const`
- L120: `AHodgeCombatCharacter* UHodgeGameplayAbility::GetHodgeCharacterFromActorInfo() const`
- L126: `UHodgeHeroComponent* UHodgeGameplayAbility::GetHeroComponentFromActorInfo() const`
- L133: `void UHodgeGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const`
- L186: `bool UHodgeGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,`
- L227: `void UHodgeGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)`
- L246: `void UHodgeGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)`
- L264: `void UHodgeGameplayAbility::PreloadConfiguredPrimaryAssets()`
- L306: `void UHodgeGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,`
- L317: `void UHodgeGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,`
- L327: `void UHodgeGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,`
- L340: `bool UHodgeGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,`
- L370: `void UHodgeGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,`
- L453: `FGameplayEffectContextHandle UHodgeGameplayAbility::MakeEffectContext(const FGameplayAbilitySpecHandle Handle,`
- L500: `void UHodgeGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,`
- L519: `bool UHodgeGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,`
- L675: `void UHodgeGameplayAbility::OnPawnAvatarSet()`
- L682: `void UHodgeGameplayAbility::GetAbilitySource(FGameplayAbilitySpecHandle Handle,`
- L708: `void UHodgeGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo,`
- L750: `bool UHodgeGameplayAbility::CanChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup) const`
- L788: `bool UHodgeGameplayAbility::ChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup)`
- L821: `void UHodgeGameplayAbility::SetCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode)`
- L835: `void UHodgeGameplayAbility::ClearCameraMode()`

## HodgeAttributeSet.cpp

项目 AttributeSet 基础和 ASC 访问。

源码：[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

定义候选（多行签名仅展示首行）：

- L23: `UHodgeAttributeSet::UHodgeAttributeSet()`
- L28: `UWorld* UHodgeAttributeSet::GetWorld() const`
- L41: `UHodgeAbilitySystemComponent* UHodgeAttributeSet::GetHodgeAbilitySystemComponent() const`

## HodgeHealthSet.cpp

Health/MaxHealth、BaseDamage/BaseHeal 和 Damage/Healing 元属性；有效结算、夹取、免疫和耗尽广播。

源码：[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L37: `UHodgeHealthSet::UHodgeHealthSet() : Health(100.0f)`
- L53: `void UHodgeHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L72: `void UHodgeHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)`
- L108: `void UHodgeHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)`
- L125: `void UHodgeHealthSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)`
- L132: `void UHodgeHealthSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)`
- L139: `bool UHodgeHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)`
- L197: `void UHodgeHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)`
- L334: `void UHodgeHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const`
- L344: `void UHodgeHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)`
- L354: `void UHodgeHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)`
- L386: `void UHodgeHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const`

## GameplayTagStack.cpp

带计数的标签栈及复制数据结构，区别于只判断有无的 TagContainer。

源码：[Source/Hodgepodge/Private/AbilitySystem/GameplayTagStack.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/GameplayTagStack.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/GameplayTagStack.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h)

定义候选（多行签名仅展示首行）：

- L13: `FString FGameplayTagStack::GetDebugString() const`
- L22: `void FGameplayTagStackContainer::AddStack(FGameplayTag Tag, int32 StackCount)`
- L69: `void FGameplayTagStackContainer::RemoveStack(FGameplayTag Tag, int32 StackCount)`
- L124: `void FGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)`
- L138: `void FGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)`
- L152: `void FGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)`

## HodgeAbilitySystemComponent.cpp

Tag 输入缓存、激活组、关系映射、全局注册、失败通知与动态 Tag GE。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeAbilityTagRelationshipMapping.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h)、[AbilitySystem/HodgeGlobalAbilitySystem.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h)、[Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)、[Data/HodgeGameData.h](../../../Source/Hodgepodge/Public/Data/HodgeGameData.h)

定义候选（多行签名仅展示首行）：

- L27: `UHodgeAbilitySystemComponent::UHodgeAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)`
- L43: `void UHodgeAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L55: `void UHodgeAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)`
- L126: `void UHodgeAbilitySystemComponent::TryActivateAbilitiesOnSpawn()`
- L143: `void UHodgeAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc,`
- L211: `void UHodgeAbilitySystemComponent::CancelInputActivatedAbilities(bool bReplicateCancelAbility)`
- L228: `void UHodgeAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)`
- L255: `void UHodgeAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)`
- L282: `void UHodgeAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)`
- L303: `void UHodgeAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)`
- L324: `void UHodgeAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)`
- L454: `void UHodgeAbilitySystemComponent::ClearAbilityInput()`
- L466: `void UHodgeAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle,`
- L480: `void UHodgeAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle,`
- L502: `void UHodgeAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,`
- L516: `void UHodgeAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,`
- L545: `void UHodgeAbilitySystemComponent::HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags,`
- L556: `void UHodgeAbilitySystemComponent::GetAdditionalActivationTagRequirements(`
- L569: `void UHodgeAbilitySystemComponent::SetTagRelationshipMapping(UHodgeAbilityTagRelationshipMapping* NewMapping)`
- L575: `void UHodgeAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(`
- L582: `void UHodgeAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability,`
- L595: `bool UHodgeAbilitySystemComponent::IsActivationGroupBlocked(EHodgeAbilityActivationGroup Group) const`
- L626: `void UHodgeAbilitySystemComponent::AddAbilityToActivationGroup(EHodgeAbilityActivationGroup Group,`
- L674: `void UHodgeAbilitySystemComponent::RemoveAbilityFromActivationGroup(EHodgeAbilityActivationGroup Group,`
- L687: `void UHodgeAbilitySystemComponent::CancelActivationGroupAbilities(EHodgeAbilityActivationGroup Group,`
- L703: `void UHodgeAbilitySystemComponent::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)`
- L739: `void UHodgeAbilitySystemComponent::RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag)`
- L764: `void UHodgeAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle,`

## HodgeAbilitySystemGlobals.cpp

分配 FHodgeGameplayEffectContext 的 GAS 全局类；已加入项目配置。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemGlobals.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemGlobals.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemGlobals.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemGlobals.h)、[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)

定义候选（多行签名仅展示首行）：

- L15: `UHodgeAbilitySystemGlobals::UHodgeAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)`
- L22: `FGameplayEffectContext* UHodgeAbilitySystemGlobals::AllocGameplayEffectContext() const`

## HodgeAbilityTagRelationshipMapping.cpp

数据驱动的能力阻断、取消与激活条件关系。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilityTagRelationshipMapping.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilityTagRelationshipMapping.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilityTagRelationshipMapping.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h)

定义候选（多行签名仅展示首行）：

- L9: `void UHodgeAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(`
- L39: `void UHodgeAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(`
- L69: `bool UHodgeAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags,`

## HodgeGameplayCueManager.cpp

项目 Cue 管理类已配置；启动预加载及 Feature Cue 观察者生命周期仍未完整接通。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayCueManager.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayCueManager.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeGameplayCueManager.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayCueManager.h)

定义候选（多行签名仅展示首行）：

- L68: `UHodgeGameplayCueManager::UHodgeGameplayCueManager(const FObjectInitializer& ObjectInitializer)`
- L74: `UHodgeGameplayCueManager* UHodgeGameplayCueManager::Get()`
- L80: `void UHodgeGameplayCueManager::OnCreated()`
- L90: `void UHodgeGameplayCueManager::LoadAlwaysLoadedCues()`
- L124: `bool UHodgeGameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries() const`
- L153: `bool UHodgeGameplayCueManager::ShouldSyncLoadMissingGameplayCues() const`
- L160: `bool UHodgeGameplayCueManager::ShouldAsyncLoadMissingGameplayCues() const`
- L167: `void UHodgeGameplayCueManager::DumpGameplayCues(const TArray<FString>& Args)`
- L262: `void UHodgeGameplayCueManager::OnGameplayTagLoaded(const FGameplayTag& Tag)`
- L309: `void UHodgeGameplayCueManager::HandlePostGarbageCollect()`
- L322: `void UHodgeGameplayCueManager::ProcessLoadedTags()`
- L373: `void UHodgeGameplayCueManager::ProcessTagToPreload(const FGameplayTag& Tag, UObject* OwningObject)`
- L438: `void UHodgeGameplayCueManager::OnPreloadCueComplete(FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject,`
- L454: `void UHodgeGameplayCueManager::RegisterPreloadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject)`
- L490: `void UHodgeGameplayCueManager::HandlePostLoadMap(UWorld* NewWorld)`
- L537: `void UHodgeGameplayCueManager::UpdateDelayLoadDelegateListeners()`
- L581: `bool UHodgeGameplayCueManager::ShouldDelayLoadGameplayCues() const`
- L600: `void UHodgeGameplayCueManager::RefreshGameplayCuePrimaryAsset()`

## HodgeGameplayEffectContext.cpp

项目 GE 上下文与序列化扩展，已有 HodgeAbilitySystemGlobals 分配配套。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayEffectContext.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayEffectContext.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)

定义候选（多行签名仅展示首行）：

- L18: `FHodgeGameplayEffectContext* FHodgeGameplayEffectContext::ExtractEffectContext(`
- L37: `bool FHodgeGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)`
- L61: `void FHodgeGameplayEffectContext::SetAbilitySource(const IHodgeAbilitySourceInterface* InObject, float InSourceLevel)`
- L71: `const IHodgeAbilitySourceInterface* FHodgeGameplayEffectContext::GetAbilitySource() const`
- L78: `const UPhysicalMaterial* FHodgeGameplayEffectContext::GetPhysicalMaterial() const`

## HodgeGameplayTags.cpp

原生 GameplayTag 注册和移动状态标签映射。标签存在不等于对应玩法实现。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayTags.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayTags.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：


## HodgeGlobalAbilitySystem.cpp

世界级全局能力/效果授予及 ASC 注册表。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeGlobalAbilitySystem.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGlobalAbilitySystem.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeGlobalAbilitySystem.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

定义候选（多行签名仅展示首行）：

- L10: `void FGlobalAppliedAbilityList::AddToASC(TSubclassOf<UGameplayAbility> Ability, UHodgeAbilitySystemComponent* ASC)`
- L32: `void FGlobalAppliedAbilityList::RemoveFromASC(UHodgeAbilitySystemComponent* ASC)`
- L46: `void FGlobalAppliedAbilityList::RemoveFromAll()`
- L65: `void FGlobalAppliedEffectList::AddToASC(TSubclassOf<UGameplayEffect> Effect, UHodgeAbilitySystemComponent* ASC)`
- L85: `void FGlobalAppliedEffectList::RemoveFromASC(UHodgeAbilitySystemComponent* ASC)`
- L99: `void FGlobalAppliedEffectList::RemoveFromAll()`
- L116: `UHodgeGlobalAbilitySystem::UHodgeGlobalAbilitySystem()`
- L121: `void UHodgeGlobalAbilitySystem::ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability)`
- L138: `void UHodgeGlobalAbilitySystem::ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect)`
- L155: `void UHodgeGlobalAbilitySystem::RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability)`
- L172: `void UHodgeGlobalAbilitySystem::RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect)`
- L189: `void UHodgeGlobalAbilitySystem::RegisterASC(UHodgeAbilitySystemComponent* ASC)`
- L210: `void UHodgeGlobalAbilitySystem::UnregisterASC(UHodgeAbilitySystemComponent* ASC)`

## HodgeAbilityTask_PlayTimeline.cpp

驱动时间轴的 AbilityTask：逻辑时间推进、阶段 loose tag、GameplayEvent 分派与自动 EndTask。

源码：[Source/Hodgepodge/Private/AbilitySystem/Timeline/HodgeAbilityTask_PlayTimeline.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Timeline/HodgeAbilityTask_PlayTimeline.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Timeline/HodgeAbilityTask_PlayTimeline.h](../../../Source/Hodgepodge/Public/AbilitySystem/Timeline/HodgeAbilityTask_PlayTimeline.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L17: `UHodgeAbilityTask_PlayTimeline::UHodgeAbilityTask_PlayTimeline(const FObjectInitializer& ObjectInitializer)`
- L24: `UHodgeAbilityTask_PlayTimeline* UHodgeAbilityTask_PlayTimeline::PlayTimeline(`
- L43: `void UHodgeAbilityTask_PlayTimeline::Activate()`
- L111: `void UHodgeAbilityTask_PlayTimeline::InitializeTimeline(float InStartOffset)`
- L141: `void UHodgeAbilityTask_PlayTimeline::TickTask(float DeltaTime)`
- L183: `void UHodgeAbilityTask_PlayTimeline::AdvanceTimeline(float InPreviousTime, float InCurrentTime)`
- L223: `void UHodgeAbilityTask_PlayTimeline::EnterPhase(int32 PhaseIndex)`
- L260: `void UHodgeAbilityTask_PlayTimeline::ExitPhase(int32 PhaseIndex)`
- L297: `void UHodgeAbilityTask_PlayTimeline::ClearAllPhaseTags()`
- L330: `void UHodgeAbilityTask_PlayTimeline::FireEvent(const FHodgeTimelineEvent& Event)`
- L335: `void UHodgeAbilityTask_PlayTimeline::FireEventTag(const FGameplayTag& EventTag, EHodgeTimelineEventNetPolicy NetPolicy, float EventTime)`
- L369: `bool UHodgeAbilityTask_PlayTimeline::ShouldExecuteOnThisEndpoint(EHodgeTimelineEventNetPolicy NetPolicy) const`
- L398: `void UHodgeAbilityTask_PlayTimeline::StopTimeline(EHodgeTimelineStopReason Reason)`
- L434: `void UHodgeAbilityTask_PlayTimeline::OnDestroy(bool bInOwnerFinished)`

## HodgeAbilityTimeline.cpp

攻击时间轴数据资产：阶段区间、时间点事件、Montage 软引用与 Bundle 收集；有实现、无 C++ 调用点。

源码：[Source/Hodgepodge/Private/AbilitySystem/Timeline/HodgeAbilityTimeline.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Timeline/HodgeAbilityTimeline.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Timeline/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/AbilitySystem/Timeline/HodgeAbilityTimeline.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)

定义候选（多行签名仅展示首行）：

- L19: `void UHodgeAbilityTimeline::GetActivePhases(float Time, TArray<int32>& OutPhaseIndices) const`
- L37: `void UHodgeAbilityTimeline::SortPhasesByTime()`
- L45: `void UHodgeAbilityTimeline::SortEventsByTime()`
- L53: `void UHodgeAbilityTimeline::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)`
- L74: `void UHodgeAbilityTimeline::SyncDurationFromMontage()`
- L92: `EDataValidationResult UHodgeAbilityTimeline::IsDataValid(FDataValidationContext& Context) const`
- L271: `void UHodgeAbilityTimeline::UpdateAssetBundleData()`

## HodgeAbilityCost.h

自定义额外能力消耗的扩展契约。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "GameplayAbilitySpec.h"
   7: #include "Abilities/GameplayAbility.h"
   8: #include "HodgeAbilityCost.generated.h"
  10: class UHodgeGameplayAbility;
  25: UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
  26: class HODGEPODGE_API UHodgeAbilityCost : public UObject
  27: {
  28: 	GENERATED_BODY()
  30: public:
  32: 	UHodgeAbilityCost()
  33: 	{
  34: 	}
  50: 	virtual bool CheckCost(const UHodgeGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
  51: 	                       const FGameplayAbilityActorInfo* ActorInfo,
  52: 	                       FGameplayTagContainer* OptionalRelevantTags) const
  53: 	{
  55: 		return true;
  56: 	}
  71: 	virtual void ApplyCost(const UHodgeGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
  72: 	                       const FGameplayAbilityActorInfo* ActorInfo,
  73: 	                       const FGameplayAbilityActivationInfo ActivationInfo)
  74: 	{
  76: 	}
  79: 	bool ShouldOnlyApplyCostOnHit() const { return bOnlyApplyCostOnHit; }
  81: protected:
  83: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
  84: 	bool bOnlyApplyCostOnHit = false;
  85: };
```

## HodgeGameplayAbility.h

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展；PreloadPrimaryAssetsOnGrant 在授予时预加载 Bundle。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)

项目内直接 include（不是运行调用关系）：[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "Abilities/GameplayAbility.h"
   7: #include "Core/PlayerController/HodgePlayerController.h"
   8: #include "HodgeGameplayAbility.generated.h"
  11: class UHodgeHeroComponent;
  12: class UHodgeAbilityCost;
  13: class UHodgeCameraMode;
  14: class IHodgeAbilitySourceInterface;
  15: class AHodgeCombatCharacter;
  16: class UPrimaryDataAsset;
  22: UENUM(BlueprintType)
  23: enum class EHodgeAbilityActivationPolicy : uint8
  24: {
  26: 	OnInputTriggered,
  29: 	WhileInputActive,
  32: 	OnSpawn
  33: };
  41: UENUM(BlueprintType)
  42: enum class EHodgeAbilityActivationGroup : uint8
  43: {
  45: 	Independent,
  48: 	Exclusive_Replaceable,
  51: 	Exclusive_Blocking,
  54: 	MAX UMETA(Hidden)
  55: };
  58: USTRUCT(BlueprintType)
  59: struct FHodgeAbilityMontageFailureMessage
  60: {
  61: 	GENERATED_BODY()
  63: public:
  65: 	UPROPERTY(BlueprintReadWrite)
  66: 	TObjectPtr<APlayerController> PlayerController = nullptr;
  69: 	UPROPERTY(BlueprintReadWrite)
  70: 	TObjectPtr<AActor> AvatarActor = nullptr;
  73: 	UPROPERTY(BlueprintReadWrite)
  74: 	FGameplayTagContainer FailureTags;
  77: 	UPROPERTY(BlueprintReadWrite)
  78: 	TObjectPtr<UAnimMontage> FailureMontage = nullptr;
  79: };
  93: UCLASS(Abstract, HideCategories = Input,
  94: 	Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
  95: class HODGEPODGE_API UHodgeGameplayAbility : public UGameplayAbility
  96: {
  97: 	GENERATED_BODY()
 100: 	friend class UHodgeAbilitySystemComponent;
 102: public:
 104: 	UHodgeGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
 107: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 108: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponentFromActorInfo() const;
 111: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 112: 	AHodgePlayerController* GetHodgePlayerControllerFromActorInfo() const;
 115: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 116: 	AController* GetControllerFromActorInfo() const;
 119: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 120: 	AHodgeCombatCharacter* GetHodgeCharacterFromActorInfo() const;
 123: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 124: 	UHodgeHeroComponent* GetHeroComponentFromActorInfo() const;
 127: 	EHodgeAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
 130: 	EHodgeAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }
 133: 	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
 137: 	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Ability",
 138: 		Meta = (ExpandBoolAsExecs = "ReturnValue"))
 139: 	bool CanChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup) const;
 143: 	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Ability",
 144: 		Meta = (ExpandBoolAsExecs = "ReturnValue"))
 145: 	bool ChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup);
 148: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 149: 	void SetCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode);
 152: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 153: 	void ClearCameraMode();
 156: 	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
 157: 	{
 159: 		NativeOnAbilityFailedToActivate(FailedReason);
 162: 		ScriptOnAbilityFailedToActivate(FailedReason);
 163: 	}
 165: protected:
 167: 	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;
 170: 	UFUNCTION(BlueprintImplementableEvent)
 171: 	void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;
 175: 	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 176: 	                                const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
 177: 	                                FGameplayTagContainer* OptionalRelevantTags) const override;
 180: 	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
 183: 	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
 186: 	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
 189: 	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 190: 	                             const FGameplayAbilityActivationInfo ActivationInfo,
 191: 	                             const FGameplayEventData* TriggerEventData) override;
 194: 	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 195: 	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
 196: 	                        bool bWasCancelled) override;
 199: 	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 200: 	                       OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
 203: 	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 204: 	                       const FGameplayAbilityActivationInfo ActivationInfo) const override;
 207: 	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle,
 208: 	                                                       const FGameplayAbilityActorInfo* ActorInfo) const override;
 211: 	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,
 212: 	                                                  FGameplayAbilitySpec* AbilitySpec) const override;
 215: 	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
 216: 	                                               const FGameplayTagContainer* SourceTags = nullptr,
 217: 	                                               const FGameplayTagContainer* TargetTags = nullptr,
 218: 	                                               OUT FGameplayTagContainer* OptionalRelevantTags = nullptr)
 219: 	const override;
 223: 	virtual void OnPawnAvatarSet();
 226: 	virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 227: 	                              float& OutSourceLevel, const IHodgeAbilitySourceInterface*& OutAbilitySource,
 228: 	                              AActor*& OutEffectCauser) const;
 232: 	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
 233: 	void K2_OnAbilityAdded();
 237: 	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
 238: 	void K2_OnAbilityRemoved();
 242: 	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
 243: 	void K2_OnPawnAvatarSet();
 245: protected:
 247: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Ability Activation")
 248: 	EHodgeAbilityActivationPolicy ActivationPolicy;
 251: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Ability Activation")
 252: 	EHodgeAbilityActivationGroup ActivationGroup;
 255: 	UPROPERTY(EditDefaultsOnly, Instanced, Category = Costs)
 256: 	TArray<TObjectPtr<UHodgeAbilityCost>> AdditionalCosts;
 259: 	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
 260: 	TMap<FGameplayTag, FText> FailureTagToUserFacingMessages;
 263: 	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
 264: 	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> FailureTagToAnimMontage;
 267: 	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
 268: 	bool bLogCancelation;
 271: 	TSubclassOf<UHodgeCameraMode> ActiveCameraMode;
 285: 	UPROPERTY(EditDefaultsOnly, Category = "Hodge|Ability")
 286: 	TArray<TObjectPtr<UPrimaryDataAsset>> PreloadPrimaryAssetsOnGrant;
 289: 	void PreloadConfiguredPrimaryAssets();
 290: };
```

## HodgeAttributeSet.h

项目 AttributeSet 基础和 ASC 访问。

源码：[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   9: #pragma once
  12: #include "CoreMinimal.h"
  15: #include "AttributeSet.h"
  17: #include "HodgeAttributeSet.generated.h"
  64: #define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
  65:                                                                  \
  66: GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
  67:                              \
  68: GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
  69:                                \
  70: GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
  71:                              \
  72: GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
  75: struct FGameplayEffectSpec;
  78: class UHodgeAbilitySystemComponent;
 102: DECLARE_MULTICAST_DELEGATE_SixParams(FHodgeAttributeEvent, AActor*                     , AActor*                 ,
 103:                                      const FGameplayEffectSpec*               , float                    ,
 104:                                      float             , float             );
 114: UCLASS()
 115: class HODGEPODGE_API UHodgeAttributeSet : public UAttributeSet
 116: {
 117: 	GENERATED_BODY()
 119: public:
 121: 	UHodgeAttributeSet();
 124: 	virtual UWorld* GetWorld() const override;
 127: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const;
 128: };
```

## HodgeHealthSet.h

Health/MaxHealth、BaseDamage/BaseHeal 和 Damage/Healing 元属性；有效结算、夹取、免疫和耗尽广播。

源码：[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   6: #include "CoreMinimal.h"
   9: #include "AbilitySystemComponent.h"
  12: #include "AbilitySystem/AttributeSet/HodgeAttributeSet.h"
  14: #include "HodgeHealthSet.generated.h"
  23: UCLASS()
  24: class HODGEPODGE_API UHodgeHealthSet : public UHodgeAttributeSet
  25: {
  26: 	GENERATED_BODY()
  28: public:
  30: 	UHodgeHealthSet();
  33: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Health);
  36: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, MaxHealth);
  39: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Healing);
  42: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Damage);
  45: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, BaseDamage);
  48: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, BaseHeal);
  52: 	mutable FHodgeAttributeEvent OnHealthChanged;
  56: 	mutable FHodgeAttributeEvent OnMaxHealthChanged;
  60: 	mutable FHodgeAttributeEvent OnOutOfHealth;
  62: protected:
  64: 	UFUNCTION()
  65: 	void OnRep_Health(const FGameplayAttributeData& OldValue);
  68: 	UFUNCTION()
  69: 	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
  72: 	UFUNCTION()
  73: 	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);
  76: 	UFUNCTION()
  77: 	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);
  80: 	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
  83: 	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
  86: 	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
  89: 	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
  92: 	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
  95: 	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
  97: private:
 101: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Hodge|Health",
 102: 		Meta = (HideFromModifiers, AllowPrivateAccess = true))
 103: 	FGameplayAttributeData Health;
 107: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Hodge|Health",
 108: 		Meta = (AllowPrivateAccess = true))
 109: 	FGameplayAttributeData MaxHealth;
 113: 	bool bOutOfHealth;
 117: 	float MaxHealthBeforeAttributeChange;
 120: 	float HealthBeforeAttributeChange;
 129: 	UPROPERTY(BlueprintReadOnly, Category="Hodge|Health", Meta=(AllowPrivateAccess=true))
 130: 	FGameplayAttributeData Healing;
 134: 	UPROPERTY(BlueprintReadOnly, Category="Hodge|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
 135: 	FGameplayAttributeData Damage;
 139: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Hodge|Combat",
 140: 		Meta = (AllowPrivateAccess = true))
 141: 	FGameplayAttributeData BaseDamage;
 144: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Hodge|Combat",
 145: 		Meta = (AllowPrivateAccess = true))
 146: 	FGameplayAttributeData BaseHeal;
 147: };
```

## GameplayTagStack.h

带计数的标签栈及复制数据结构，区别于只判断有无的 TagContainer。

源码：[Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "GameplayTagContainer.h"
   6: #include "Net/Serialization/FastArraySerializer.h"
   8: #include "GameplayTagStack.generated.h"
  11: struct FGameplayTagStackContainer;
  14: struct FNetDeltaSerializeInfo;
  20: USTRUCT(BlueprintType)
  21: struct FGameplayTagStack : public FFastArraySerializerItem
  22: {
  23: 	GENERATED_BODY()
  26: 	FGameplayTagStack()
  27: 	{
  28: 	}
  31: 	FGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
  32: 		: Tag(InTag)
  33: 		  , StackCount(InStackCount)
  34: 	{
  35: 	}
  38: 	FString GetDebugString() const;
  40: private:
  42: 	friend FGameplayTagStackContainer;
  45: 	UPROPERTY()
  46: 	FGameplayTag Tag;
  49: 	UPROPERTY()
  50: 	int32 StackCount = 0;
  51: };
  57: USTRUCT(BlueprintType)
  58: struct FGameplayTagStackContainer : public FFastArraySerializer
  59: {
  60: 	GENERATED_BODY()
  63: 	FGameplayTagStackContainer()
  65: 	{
  66: 	}
  68: public:
  70: 	void AddStack(FGameplayTag Tag, int32 StackCount);
  73: 	void RemoveStack(FGameplayTag Tag, int32 StackCount);
  76: 	int32 GetStackCount(FGameplayTag Tag) const
  77: 	{
  78: 		return TagToCountMap.FindRef(Tag);
  79: 	}
  82: 	bool ContainsTag(FGameplayTag Tag) const
  83: 	{
  84: 		return TagToCountMap.Contains(Tag);
  85: 	}
  90: 	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
  93: 	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
  96: 	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
 101: 	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
 102: 	{
 103: 		return FFastArraySerializer::FastArrayDeltaSerialize<FGameplayTagStack, FGameplayTagStackContainer>(
 104: 			Stacks, DeltaParms, *this);
 105: 	}
 107: private:
 109: 	UPROPERTY()
 110: 	TArray<FGameplayTagStack> Stacks;
 113: 	TMap<FGameplayTag, int32> TagToCountMap;
 114: };
 117: template <>
 118: struct TStructOpsTypeTraits<FGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FGameplayTagStackContainer>
 119: {
 120: 	enum
 121: 	{
 123: 		WithNetDeltaSerializer = true,
 124: 	};
 125: };
```

## HodgeAbilitySystemComponent.h

Tag 输入缓存、激活组、关系映射、全局注册、失败通知与动态 Tag GE。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "AbilitySystemComponent.h"
   5: #include "NativeGameplayTags.h"
   6: #include "Abilities/HodgeGameplayAbility.h"
   7: #include "HodgeAbilitySystemComponent.generated.h"
  10: class UHodgeAbilityTagRelationshipMapping;
  13: HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_AbilityInputBlocked);
  27: UCLASS()
  28: class HODGEPODGE_API UHodgeAbilitySystemComponent : public UAbilitySystemComponent
  29: {
  30: 	GENERATED_BODY()
  32: public:
  34: 	UHodgeAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  38: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  42: 	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
  45: 	typedef TFunctionRef<bool(const UHodgeGameplayAbility* HodgeAbility, FGameplayAbilitySpecHandle Handle)>
  46: 	TShouldCancelAbilityFunc;
  49: 	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);
  52: 	void CancelInputActivatedAbilities(bool bReplicateCancelAbility);
  55: 	void AbilityInputTagPressed(const FGameplayTag& InputTag);
  58: 	void AbilityInputTagReleased(const FGameplayTag& InputTag);
  61: 	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
  64: 	void ClearAbilityInput();
  67: 	bool IsActivationGroupBlocked(EHodgeAbilityActivationGroup Group) const;
  70: 	void AddAbilityToActivationGroup(EHodgeAbilityActivationGroup Group, UHodgeGameplayAbility* HodgeAbility);
  73: 	void RemoveAbilityFromActivationGroup(EHodgeAbilityActivationGroup Group, UHodgeGameplayAbility* HodgeAbility);
  76: 	void CancelActivationGroupAbilities(EHodgeAbilityActivationGroup Group, UHodgeGameplayAbility* IgnoreHodgeAbility,
  77: 	                                    bool bReplicateCancelAbility);
  81: 	void AddDynamicTagGameplayEffect(const FGameplayTag& Tag);
  85: 	void RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag);
  89: 	void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle,
  90: 	                          FGameplayAbilityActivationInfo ActivationInfo,
  91: 	                          FGameplayAbilityTargetDataHandle& OutTargetDataHandle);
  95: 	void SetTagRelationshipMapping(UHodgeAbilityTagRelationshipMapping* NewMapping);
  99: 	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags,
 100: 	                                            FGameplayTagContainer& OutActivationRequired,
 101: 	                                            FGameplayTagContainer& OutActivationBlocked) const;
 103: protected:
 105: 	void TryActivateAbilitiesOnSpawn();
 108: 	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
 111: 	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
 114: 	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
 117: 	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,
 118: 	                                 const FGameplayTagContainer& FailureReason) override;
 121: 	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,
 122: 	                                bool bWasCancelled) override;
 125: 	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,
 126: 	                                            UGameplayAbility* RequestingAbility, bool bEnableBlockTags,
 127: 	                                            const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags,
 128: 	                                            const FGameplayTagContainer& CancelTags) override;
 131: 	virtual void HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags,
 132: 	                                              UGameplayAbility* RequestingAbility, bool bCanBeCanceled) override;
 136: 	UFUNCTION(Client, Unreliable)
 137: 	void ClientNotifyAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);
 140: 	void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);
 142: protected:
 146: 	UPROPERTY()
 147: 	TObjectPtr<UHodgeAbilityTagRelationshipMapping> TagRelationshipMapping;
 151: 	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
 155: 	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
 159: 	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
 163: 	int32 ActivationGroupCounts[(uint8)EHodgeAbilityActivationGroup::MAX];
 164: };
```

## HodgeAbilitySystemGlobals.h

分配 FHodgeGameplayEffectContext 的 GAS 全局类；已加入项目配置。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemGlobals.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemGlobals.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   6: #include "AbilitySystemGlobals.h"
   8: #include "HodgeAbilitySystemGlobals.generated.h"
  11: class UObject;
  14: struct FGameplayEffectContext;
  18: UCLASS(Config=Game)
  19: class UHodgeAbilitySystemGlobals : public UAbilitySystemGlobals
  20: {
  21: 	GENERATED_UCLASS_BODY()
  26: 	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
  29: };
```

## HodgeAbilityTagRelationshipMapping.h

数据驱动的能力阻断、取消与激活条件关系。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "Engine/DataAsset.h"
   6: #include "GameplayTagContainer.h"
   8: #include "HodgeAbilityTagRelationshipMapping.generated.h"
  10: class UObject;
  18: USTRUCT()
  19: struct FHodgeAbilityTagRelationship
  20: {
  21: 	GENERATED_BODY()
  24: 	UPROPERTY(EditAnywhere, Category = Ability, meta = (Categories = "Gameplay.Action"))
  25: 	FGameplayTag AbilityTag;
  28: 	UPROPERTY(EditAnywhere, Category = Ability)
  29: 	FGameplayTagContainer AbilityTagsToBlock;
  32: 	UPROPERTY(EditAnywhere, Category = Ability)
  33: 	FGameplayTagContainer AbilityTagsToCancel;
  36: 	UPROPERTY(EditAnywhere, Category = Ability)
  37: 	FGameplayTagContainer ActivationRequiredTags;
  40: 	UPROPERTY(EditAnywhere, Category = Ability)
  41: 	FGameplayTagContainer ActivationBlockedTags;
  42: };
  51: UCLASS()
  52: class UHodgeAbilityTagRelationshipMapping : public UDataAsset
  53: {
  54: 	GENERATED_BODY()
  56: private:
  58: 	UPROPERTY(EditAnywhere, Category = Ability, meta=(TitleProperty="AbilityTag"))
  59: 	TArray<FHodgeAbilityTagRelationship> AbilityTagRelationships;
  61: public:
  63: 	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock,
  64: 	                                    FGameplayTagContainer* OutTagsToCancel) const;
  67: 	void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags,
  68: 	                                         FGameplayTagContainer* OutActivationRequired,
  69: 	                                         FGameplayTagContainer* OutActivationBlocked) const;
  72: 	bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;
  73: };
```

## HodgeGameplayCueManager.h

项目 Cue 管理类已配置；启动预加载及 Feature Cue 观察者生命周期仍未完整接通。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayCueManager.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayCueManager.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "GameplayCueManager.h"
   7: #include "HodgeGameplayCueManager.generated.h"
   9: class FString;
  10: class UClass;
  11: class UObject;
  12: class UWorld;
  13: struct FObjectKey;
  21: UCLASS()
  22: class UHodgeGameplayCueManager : public UGameplayCueManager
  23: {
  24: 	GENERATED_BODY()
  26: public:
  28: 	UHodgeGameplayCueManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  31: 	static UHodgeGameplayCueManager* Get();
  36: 	virtual void OnCreated() override;
  39: 	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override;
  42: 	virtual bool ShouldSyncLoadMissingGameplayCues() const override;
  45: 	virtual bool ShouldAsyncLoadMissingGameplayCues() const override;
  50: 	static void DumpGameplayCues(const TArray<FString>& Args);
  53: 	void LoadAlwaysLoadedCues();
  56: 	void RefreshGameplayCuePrimaryAsset();
  58: private:
  60: 	void OnGameplayTagLoaded(const FGameplayTag& Tag);
  63: 	void HandlePostGarbageCollect();
  66: 	void ProcessLoadedTags();
  69: 	void ProcessTagToPreload(const FGameplayTag& Tag, UObject* OwningObject);
  72: 	void OnPreloadCueComplete(FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject, bool bAlwaysLoadedCue);
  75: 	void RegisterPreloadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject);
  78: 	void HandlePostLoadMap(UWorld* NewWorld);
  81: 	void UpdateDelayLoadDelegateListeners();
  84: 	bool ShouldDelayLoadGameplayCues() const;
  86: private:
  88: 	struct FLoadedGameplayTagToProcessData
  89: 	{
  91: 		FGameplayTag Tag;
  94: 		TWeakObjectPtr<UObject> WeakOwner;
  97: 		FLoadedGameplayTagToProcessData()
  98: 		{
  99: 		}
 102: 		FLoadedGameplayTagToProcessData(const FGameplayTag& InTag,
 103: 		                                const TWeakObjectPtr<UObject>& InWeakOwner) : Tag(InTag), WeakOwner(InWeakOwner)
 104: 		{
 105: 		}
 106: 	};
 108: private:
 110: 	UPROPERTY(transient)
 111: 	TSet<TObjectPtr<UClass>> PreloadedCues;
 114: 	TMap<FObjectKey, TSet<FObjectKey>> PreloadedCueReferencers;
 118: 	UPROPERTY(transient)
 119: 	TSet<TObjectPtr<UClass>> AlwaysLoadedCues;
 122: 	TArray<FLoadedGameplayTagToProcessData> LoadedGameplayTagsToProcess;
 125: 	FCriticalSection LoadedGameplayTagsToProcessCS;
 128: 	bool bProcessLoadedTagsAfterGC = false;
 129: };
```

## HodgeGameplayEffectContext.h

项目 GE 上下文与序列化扩展，已有 HodgeAbilitySystemGlobals 分配配套。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)

项目内直接 include（不是运行调用关系）：[Interface/HodgeAbilitySourceInterface.h](../../../Source/Hodgepodge/Public/Interface/HodgeAbilitySourceInterface.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "GameplayEffectTypes.h"
   6: #include "Interface/HodgeAbilitySourceInterface.h"
   8: #include "HodgeGameplayEffectContext.generated.h"
  10: class AActor;
  11: class FArchive;
  13: class UObject;
  14: class UPhysicalMaterial;
  26: USTRUCT()
  27: struct FHodgeGameplayEffectContext : public FGameplayEffectContext
  28: {
  29: 	GENERATED_BODY()
  32: 	FHodgeGameplayEffectContext()
  33: 		: FGameplayEffectContext()
  34: 	{
  35: 	}
  38: 	FHodgeGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
  39: 		: FGameplayEffectContext(InInstigator, InEffectCauser)
  40: 	{
  41: 	}
  44: 	static HODGEPODGE_API FHodgeGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);
  47: 	void SetAbilitySource(const IHodgeAbilitySourceInterface* InObject, float InSourceLevel);
  50: 	const IHodgeAbilitySourceInterface* GetAbilitySource() const;
  53: 	virtual FGameplayEffectContext* Duplicate() const override
  54: 	{
  55: 		FHodgeGameplayEffectContext* NewContext = new FHodgeGameplayEffectContext();
  56: 		*NewContext = *this;
  57: 		if (GetHitResult())
  58: 		{
  60: 			NewContext->AddHitResult(*GetHitResult(), true);
  61: 		}
  62: 		return NewContext;
  63: 	}
  66: 	virtual UScriptStruct* GetScriptStruct() const override
  67: 	{
  68: 		return FHodgeGameplayEffectContext::StaticStruct();
  69: 	}
  72: 	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
  75: 	const UPhysicalMaterial* GetPhysicalMaterial() const;
  77: public:
  79: 	UPROPERTY()
  80: 	int32 CartridgeID = -1;
  82: protected:
  85: 	UPROPERTY()
  86: 	TWeakObjectPtr<const UObject> AbilitySourceObject;
  87: };
  90: template <>
  91: struct TStructOpsTypeTraits<FHodgeGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FHodgeGameplayEffectContext>
  92: {
  93: 	enum
  94: 	{
  96: 		WithNetSerializer = true,
  99: 		WithCopy = true
 100: 	};
 101: };
```

## HodgeGameplayTags.h

原生 GameplayTag 注册和移动状态标签映射。标签存在不等于对应玩法实现。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "NativeGameplayTags.h"
   5: namespace HodgeGameplayTags
   6: {
   7: 	HODGEPODGE_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
  12: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
  13: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
  14: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
  15: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
  16: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
  17: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
  18: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);
  23: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);
  28: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dash_Duration_Message);
  29: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Grenade_Duration_Message);
  30: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Interaction_Activate);
  31: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Respawn_Completed_Message);
  32: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Respawn_Duration_Message);
  37: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type);
  38: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action);
  39: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_ADS);
  40: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Dash);
  41: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Drop);
  42: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Emote);
  43: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Grenade);
  44: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Jump);
  45: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Melee);
  46: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Reload);
  47: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_WeaponFire);
  48: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Info);
  49: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Info_ShowLeaderboard);
  50: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive);
  51: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_AutoReload);
  52: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_AutoRespawn);
  53: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_ChangeQuickbarSlot);
  54: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange);
  55: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange_Death);
  56: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange_Spawning);
  65: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack);
  66: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Transition);
  67: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Transition_Normal);
  68: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Transition_Heavy);
  69: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Transition_Dodge);
  70: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Transition_HitConfirm);
  71: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Transition_Airborne);
  72: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Transition_AfterSkill);
  73: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Entry);
  74: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Entry_Default);
  75: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Entry_AfterSkill);
  76: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Entry_AfterDodge);
  81: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic);
  82: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_AnimationStyle_Feminine);
  83: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_AnimationStyle_Masculine);
  84: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_BodyStyle_Medium);
  89: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Zone_WeakSpot);
  94: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_DamageTaken);
  95: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Dash);
  96: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Dash_Cooldown);
  97: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Death);
  98: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Heal);
  99: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Melee_Cooldown);
 104: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_Burst);
 105: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_BurstLatent);
 106: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_Looping);
 111: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Grenade_Cooldown);
 112: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Grenade_Detonate);
 113: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Melee_Hit);
 114: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Melee_Impact);
 115: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Pistol_Fire);
 116: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Rifle_Fire);
 117: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Rifle_Impact);
 118: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Shotgun_Fire);
 123: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_World_Launcher_Activate);
 124: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_World_Teleporter_Activate);
 129: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageTrait_Instant);
 130: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageTrait_Periodic);
 131: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Basic);
 132: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Grenade);
 133: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Melee);
 134: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Pistol);
 135: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Rifle);
 136: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Shotgun);
 137: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Heal_Instant);
 138: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Heal_Periodic);
 143: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_MeleeHit);
 144: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
 145: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Reset);
 146: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RequestReset);
 151: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack);
 154: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_ComboWindow_Open);
 155: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_ComboWindow_Close);
 158: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_ComboTransition);
 159: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_HitCheck);
 160: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_CameraShake);
 161: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_JumpSection);
 164: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_Timeline_End);
 165: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_Interrupted);
 168: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_Phase_Enter);
 169: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_Phase_Exit);
 174: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameSettings_Action_EditSafeZone);
 179: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_ExtraEquipment);
 180: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_InfrequentAbilities);
 181: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_LeftSideTouchInputs);
 182: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_LeftSideTouchRegion);
 183: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RespawnTimer);
 184: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RightSideTouchInputs);
 185: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RightSideTouchRegion);
 190: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Dash);
 191: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Heal);
 192: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Melee);
 193: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Quickslot_Drop);
 194: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Jump);
 199: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_ADS);
 200: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Fire);
 201: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_FireAuto);
 202: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Grenade);
 203: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Reload);
 208: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
 209: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
 210: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
 211: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Stick);
 212: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
 213: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_AutoRun);
 221: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Sprint);
 222: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Walk);
 223: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Aim);
 224: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ragdoll);
 225: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Roll);
 226: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_RotationMode);
 227: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_ViewMode);
 228: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_SwitchShoulder);
 233: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
 234: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
 235: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
 236: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);
 241: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_Damage_Taken_Message);
 242: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_HUD_PlayerHUD);
 243: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_HUD_TempTopWidgets);
 244: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_Player);
 249: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_BinauralSettingControlledByOS);
 250: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_CanExitApplication);
 251: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_PrimarlyController);
 252: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_HasStrictControllerPairing);
 253: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_PrimarlyTouchScreen);
 254: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_SupportsMouseAndKeyboard);
 255: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_HardwareCursor);
 256: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsBackgroundAudio);
 257: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsChangingAudioOutputDevice);
 258: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsWindowedMode);
 263: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ShooterGame_GamePhase_MatchBeginCountdown);
 268: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_SpawningIn);
 269: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
 270: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AutoRunning);
 271: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
 272: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
 273: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);
 278: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack);
 279: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Windup);
 280: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Active);
 281: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Recovery);
 282: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_ComboWindow);
 283: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Invincible);
 284: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_SuperArmor);
 288: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AttackMode);
 289: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AttackMode_Normal);
 290: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AttackMode_Enhanced);
 295: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
 296: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);
 301: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_GodMode);
 302: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_UnlimitedHealth);
 307: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Back);
 308: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Game);
 309: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_GameMenu);
 310: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Menu);
 311: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Modal);
 316: 	HODGEPODGE_API extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
 317: 	HODGEPODGE_API extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;
 319: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
 320: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
 321: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
 322: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
 323: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);
 324: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);
 325: };
```

## HodgeGlobalAbilitySystem.h

世界级全局能力/效果授予及 ASC 注册表。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "ActiveGameplayEffectHandle.h"
   6: #include "Subsystems/WorldSubsystem.h"
   7: #include "GameplayAbilitySpecHandle.h"
   8: #include "Templates/SubclassOf.h"
  10: #include "HodgeGlobalAbilitySystem.generated.h"
  12: class UGameplayAbility;
  13: class UGameplayEffect;
  14: class UHodgeAbilitySystemComponent;
  15: class UObject;
  16: struct FActiveGameplayEffectHandle;
  17: struct FFrame;
  18: struct FGameplayAbilitySpecHandle;
  24: USTRUCT()
  25: struct FGlobalAppliedAbilityList
  26: {
  27: 	GENERATED_BODY()
  30: 	UPROPERTY()
  31: 	TMap<TObjectPtr<UHodgeAbilitySystemComponent>, FGameplayAbilitySpecHandle> Handles;
  34: 	void AddToASC(TSubclassOf<UGameplayAbility> Ability, UHodgeAbilitySystemComponent* ASC);
  37: 	void RemoveFromASC(UHodgeAbilitySystemComponent* ASC);
  40: 	void RemoveFromAll();
  41: };
  47: USTRUCT()
  48: struct FGlobalAppliedEffectList
  49: {
  50: 	GENERATED_BODY()
  53: 	UPROPERTY()
  54: 	TMap<TObjectPtr<UHodgeAbilitySystemComponent>, FActiveGameplayEffectHandle> Handles;
  57: 	void AddToASC(TSubclassOf<UGameplayEffect> Effect, UHodgeAbilitySystemComponent* ASC);
  60: 	void RemoveFromASC(UHodgeAbilitySystemComponent* ASC);
  63: 	void RemoveFromAll();
  64: };
  72: UCLASS()
  73: class UHodgeGlobalAbilitySystem : public UWorldSubsystem
  74: {
  75: 	GENERATED_BODY()
  77: public:
  78: 	UHodgeGlobalAbilitySystem();
  81: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
  82: 	void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);
  85: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
  86: 	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);
  89: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
  90: 	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);
  93: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
  94: 	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);
  97: 	void RegisterASC(UHodgeAbilitySystemComponent* ASC);
 100: 	void UnregisterASC(UHodgeAbilitySystemComponent* ASC);
 102: private:
 104: 	UPROPERTY()
 105: 	TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;
 108: 	UPROPERTY()
 109: 	TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;
 112: 	UPROPERTY()
 113: 	TArray<TObjectPtr<UHodgeAbilitySystemComponent>> RegisteredASCs;
 114: };
```

## HodgeAbilityTask_PlayTimeline.h

驱动时间轴的 AbilityTask：逻辑时间推进、阶段 loose tag、GameplayEvent 分派与自动 EndTask。

源码：[Source/Hodgepodge/Public/AbilitySystem/Timeline/HodgeAbilityTask_PlayTimeline.h](../../../Source/Hodgepodge/Public/AbilitySystem/Timeline/HodgeAbilityTask_PlayTimeline.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Timeline/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/AbilitySystem/Timeline/HodgeAbilityTimeline.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  16: #pragma once
  18: #include "CoreMinimal.h"
  19: #include "Abilities/Tasks/AbilityTask.h"
  20: #include "GameplayTagContainer.h"
  21: #include "AbilitySystem/Timeline/HodgeAbilityTimeline.h"
  22: #include "HodgeAbilityTask_PlayTimeline.generated.h"
  34: UENUM(BlueprintType)
  35: enum class EHodgeTimelineStopReason : uint8
  36: {
  38: 	None UMETA(DisplayName = "未声明"),
  41: 	NaturalEnd UMETA(DisplayName = "自然结束"),
  44: 	ComboTransition UMETA(DisplayName = "连击提交"),
  47: 	Interrupted UMETA(DisplayName = "被外力抢占"),
  50: 	AbilityCancelled UMETA(DisplayName = "能力被取消")
  51: };
  54: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeTimelineStoppedDelegate, EHodgeTimelineStopReason, StopReason);
  62: UCLASS()
  63: class HODGEPODGE_API UHodgeAbilityTask_PlayTimeline : public UAbilityTask
  64: {
  65: 	GENERATED_BODY()
  67: public:
  68: 	UHodgeAbilityTask_PlayTimeline(const FObjectInitializer& ObjectInitializer);
  71: 	UPROPERTY(BlueprintAssignable)
  72: 	FHodgeTimelineStoppedDelegate OnTimelineStopped;
  87: 	UFUNCTION(BlueprintCallable, Category="Hodge|Ability|Tasks",
  88: 		meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="TRUE",
  89: 			AdvancedDisplay="StartOffset,InitialPlayRate"))
  90: 	static UHodgeAbilityTask_PlayTimeline* PlayTimeline(
  91: 		UGameplayAbility* OwningAbility,
  92: 		UHodgeAbilityTimeline* Timeline,
  93: 		FGameplayTag AttackID,
  94: 		float StartOffset = 0.f,
  95: 		float InitialPlayRate = 1.f);
 104: 	UFUNCTION(BlueprintCallable, Category="Hodge|Ability|Tasks")
 105: 	void StopTimeline(EHodgeTimelineStopReason Reason);
 112: 	UFUNCTION(BlueprintPure, Category="Hodge|Ability|Tasks")
 113: 	FGameplayTag GetAttackID() const { return AttackID; }
 116: 	UFUNCTION(BlueprintPure, Category="Hodge|Ability|Tasks")
 117: 	bool IsTimelineStopped() const { return bStopped; }
 120: 	UFUNCTION(BlueprintPure, Category="Hodge|Ability|Tasks")
 121: 	float GetElapsedTime() const { return ElapsedTime; }
 123: protected:
 124: 	virtual void Activate() override;
 125: 	virtual void TickTask(float DeltaTime) override;
 126: 	virtual void OnDestroy(bool bInOwnerFinished) override;
 137: 	void InitializeTimeline(float InStartOffset);
 140: 	void AdvanceTimeline(float InPreviousTime, float InCurrentTime);
 142: 	void EnterPhase(int32 PhaseIndex);
 143: 	void ExitPhase(int32 PhaseIndex);
 146: 	void ClearAllPhaseTags();
 149: 	void FireEvent(const FHodgeTimelineEvent& Event);
 152: 	void FireEventTag(const FGameplayTag& EventTag, EHodgeTimelineEventNetPolicy NetPolicy, float EventTime);
 155: 	bool ShouldExecuteOnThisEndpoint(EHodgeTimelineEventNetPolicy NetPolicy) const;
 157: protected:
 158: 	UPROPERTY()
 159: 	TObjectPtr<UHodgeAbilityTimeline> TimelineAsset;
 162: 	FGameplayTag AttackID;
 165: 	float StartOffset = 0.f;
 171: 	float InitialPlayRate = 1.f;
 174: 	float LastUpdateWorldTime = 0.f;
 177: 	float LogicalElapsed = 0.f;
 180: 	float ElapsedTime = 0.f;
 183: 	float PreviousTime = 0.f;
 186: 	int32 NextEventIndex = 0;
 189: 	TArray<int32> ActivePhaseIndices;
 192: 	bool bStopped = false;
 193: };
```

## HodgeAbilityTimeline.h

攻击时间轴数据资产：阶段区间、时间点事件、Montage 软引用与 Bundle 收集；有实现、无 C++ 调用点。

源码：[Source/Hodgepodge/Public/AbilitySystem/Timeline/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/AbilitySystem/Timeline/HodgeAbilityTimeline.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  13: #pragma once
  15: #include "CoreMinimal.h"
  16: #include "Engine/DataAsset.h"
  17: #include "GameplayTagContainer.h"
  18: #include "HodgeAbilityTimeline.generated.h"
  20: class UAnimMontage;
  31: UENUM(BlueprintType)
  32: enum class EHodgeTimelineEventNetPolicy : uint8
  33: {
  35: 	LocalAndAuthority UMETA(DisplayName = "两端各自执行"),
  38: 	AuthorityOnly UMETA(DisplayName = "仅服务器"),
  41: 	LocallyControlledOnly UMETA(DisplayName = "仅本地控制端")
  42: };
  52: USTRUCT(BlueprintType)
  53: struct HODGEPODGE_API FHodgeTimelinePhase
  54: {
  55: 	GENERATED_BODY()
  58: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Status.Attack"))
  59: 	FGameplayTag PhaseTag;
  62: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.0, UIMin=0.0, Units="s"))
  63: 	float StartTime = 0.f;
  66: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.0, UIMin=0.0, Units="s"))
  67: 	float EndTime = 0.f;
  75: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Status"))
  76: 	FGameplayTagContainer AdditionalGrantedTags;
  77: };
  85: USTRUCT(BlueprintType)
  86: struct HODGEPODGE_API FHodgeTimelineEvent
  87: {
  88: 	GENERATED_BODY()
  91: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.0, UIMin=0.0, Units="s"))
  92: 	float Time = 0.f;
  95: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="GameplayEvent.Attack"))
  96: 	FGameplayTag EventTag;
  99: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
 100: 	EHodgeTimelineEventNetPolicy NetPolicy = EHodgeTimelineEventNetPolicy::LocalAndAuthority;
 101: };
 109: UCLASS(BlueprintType, Const)
 110: class HODGEPODGE_API UHodgeAbilityTimeline : public UPrimaryDataAsset
 111: {
 112: 	GENERATED_BODY()
 114: public:
 121: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.01, Units="s"))
 122: 	float Duration = 1.0f;
 131: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Animation"))
 132: 	TSoftObjectPtr<UAnimMontage> Montage;
 135: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
 136: 	FName MontageSection;
 139: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.0, Units="s"))
 140: 	float LengthMismatchTolerance = 0.05f;
 147: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty=PhaseTag))
 148: 	TArray<FHodgeTimelinePhase> Phases;
 156: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty=EventTag))
 157: 	TArray<FHodgeTimelineEvent> Events;
 160: 	void GetActivePhases(float Time, TArray<int32>& OutPhaseIndices) const;
 163: 	const TSoftObjectPtr<UAnimMontage>& GetMontage() const { return Montage; }
 165: #if WITH_EDITOR
 167: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
 170: 	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
 173: 	UFUNCTION(CallInEditor, Category="Hodge|Timeline", meta=(DisplayName="从 Montage 同步 Duration"))
 174: 	void SyncDurationFromMontage();
 177: 	void SortPhasesByTime();
 180: 	void SortEventsByTime();
 181: #endif
 183: #if WITH_EDITORONLY_DATA
 185: 	virtual void UpdateAssetBundleData() override;
 186: #endif
 187: };
```
