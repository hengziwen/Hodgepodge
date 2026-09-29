# AbilitySystem 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAbilityTask_PlayTimeline.cpp

驱动 HodgeAbilityTimeline 的唯一 AbilityTask：初始化与 Tick 共用 CollectNodes + SortNodes 统一 Scheduler，推进逻辑时间，维护 WindowTag 与 GE 两个账本，派发 Point 与系统事件。窗口 GE 施加/移除、Point 与 Timeline.End 派发、中途取消清理已实测通过；重入类时序、NetPolicy 跨端、时钟倒退未验证。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)

定义候选（多行签名仅展示首行）：

- L29: `UHodgeAbilityTask_PlayTimeline::UHodgeAbilityTask_PlayTimeline(const FObjectInitializer& ObjectInitializer)`
- L36: `UHodgeAbilityTask_PlayTimeline* UHodgeAbilityTask_PlayTimeline::PlayTimeline(`
- L49: `void UHodgeAbilityTask_PlayTimeline::Activate()`
- L109: `void UHodgeAbilityTask_PlayTimeline::TickTask(float DeltaTime)`
- L168: `void UHodgeAbilityTask_PlayTimeline::CollectNodes(float PreviousTime, float CurrentTime,`
- L176: `void UHodgeAbilityTask_PlayTimeline::SortNodes(TArray<FHodgeTimelineNode>& Nodes) const`
- L181: `void UHodgeAbilityTask_PlayTimeline::InitializeTimeline(float InStartOffset)`
- L217: `void UHodgeAbilityTask_PlayTimeline::AdvanceTimeline(float PreviousTime, float CurrentTime)`
- L268: `bool UHodgeAbilityTask_PlayTimeline::HasAuthorityOnAvatar() const`
- L274: `void UHodgeAbilityTask_PlayTimeline::EnterWindow(int32 EventIndex)`
- L334: `void UHodgeAbilityTask_PlayTimeline::ExitWindow(int32 EventIndex)`
- L383: `FActiveGameplayEffectHandle UHodgeAbilityTask_PlayTimeline::ApplyTimelineEffect(`
- L407: `void UHodgeAbilityTask_PlayTimeline::FirePointEvent(const FHodgeTimelineEvent& Event)`
- L471: `void UHodgeAbilityTask_PlayTimeline::FireSystemEvent(const FGameplayTag& EventTag)`
- L484: `void UHodgeAbilityTask_PlayTimeline::ClearAllWindowState()`
- L527: `void UHodgeAbilityTask_PlayTimeline::StopTimeline(EHodgeTimelineStopReason Reason)`
- L560: `void UHodgeAbilityTask_PlayTimeline::OnDestroy(bool bInOwnerFinished)`
- L572: `UHodgeAbilityTask_PlayTimeline* UHodgeAbilityTask_PlayTimeline::PlayMontageTimeline(`
- L583: `FGameplayTagContainer UHodgeAbilityTask_PlayTimeline::GetActiveWindowTags() const`
- L604: `void UHodgeAbilityTask_PlayTimeline::RefreshMontageClock()`

## HodgeAbilityTask_WaitMoveCancel.cpp

把“取消窗口（Timeline 授权）”与“移动意图（输入层）”两个变化驱动信号合流的 AbilityTask：同时成立时广播 OnMoveCancel 一次后自结束。本地控制端语义；AI/模拟代理找不到 HeroComponent 时永不成立。工作区新增，未编译、未 PIE。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)

定义候选（多行签名仅展示首行）：

- L14: `UHodgeAbilityTask_WaitMoveCancel::UHodgeAbilityTask_WaitMoveCancel(const FObjectInitializer& ObjectInitializer)`
- L20: `void UHodgeAbilityTask_WaitMoveCancel::TickTask(float DeltaTime)`
- L26: `UHodgeAbilityTask_WaitMoveCancel* UHodgeAbilityTask_WaitMoveCancel::WaitMoveCancel(`
- L35: `void UHodgeAbilityTask_WaitMoveCancel::Activate()`
- L70: `void UHodgeAbilityTask_WaitMoveCancel::OnDestroy(bool bInOwnerFinished)`
- L93: `void UHodgeAbilityTask_WaitMoveCancel::Evaluate(const TCHAR* Source)`
- L136: `void UHodgeAbilityTask_WaitMoveCancel::HandleMoveIntentChanged(bool                   )`
- L142: `void UHodgeAbilityTask_WaitMoveCancel::HandleWindowTagChanged(FGameplayTag        , int32 NewCount)`

## HodgeGameplayAbility.cpp

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。（PreloadPrimaryAssetsOnGrant 已随未提交改动回退，当前不存在。）

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/Abilities/HodgeAbilityCost.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h)、[Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Interface/HodgeAbilitySourceInterface.h](../../../Source/Hodgepodge/Public/Interface/HodgeAbilitySourceInterface.h)

定义候选（多行签名仅展示首行）：

- L36: `UHodgeGameplayAbility::UHodgeGameplayAbility(const FObjectInitializer& ObjectInitializer)`
- L65: `UHodgeAbilitySystemComponent* UHodgeGameplayAbility::GetHodgeAbilitySystemComponentFromActorInfo() const`
- L74: `AHodgePlayerController* UHodgeGameplayAbility::GetHodgePlayerControllerFromActorInfo() const`
- L81: `AController* UHodgeGameplayAbility::GetControllerFromActorInfo() const`
- L119: `AHodgeCombatCharacter* UHodgeGameplayAbility::GetHodgeCharacterFromActorInfo() const`
- L125: `UHodgeHeroComponent* UHodgeGameplayAbility::GetHeroComponentFromActorInfo() const`
- L132: `void UHodgeGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const`
- L185: `bool UHodgeGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,`
- L226: `void UHodgeGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)`
- L245: `void UHodgeGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)`
- L258: `void UHodgeGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,`
- L269: `void UHodgeGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,`
- L279: `void UHodgeGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,`
- L292: `bool UHodgeGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,`
- L322: `void UHodgeGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,`
- L405: `FGameplayEffectContextHandle UHodgeGameplayAbility::MakeEffectContext(const FGameplayAbilitySpecHandle Handle,`
- L452: `void UHodgeGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,`
- L471: `bool UHodgeGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,`
- L627: `void UHodgeGameplayAbility::OnPawnAvatarSet()`
- L634: `void UHodgeGameplayAbility::GetAbilitySource(FGameplayAbilitySpecHandle Handle,`
- L660: `void UHodgeGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo,`
- L702: `bool UHodgeGameplayAbility::CanChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup) const`
- L740: `bool UHodgeGameplayAbility::ChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup)`
- L773: `void UHodgeGameplayAbility::SetCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode)`
- L787: `void UHodgeGameplayAbility::ClearCameraMode()`

## HodgeGameplayAbility_BasicAttack.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_BasicAttack.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_BasicAttack.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_BasicAttack.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_BasicAttack.h)、[AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h)、[AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)

定义候选（多行签名仅展示首行）：

- L16: `UHodgeGameplayAbility_BasicAttack::UHodgeGameplayAbility_BasicAttack(const FObjectInitializer& ObjectInitializer)`
- L25: `void UHodgeGameplayAbility_BasicAttack::ActivateAbility(FGameplayAbilitySpecHandle Handle,`
- L64: `void UHodgeGameplayAbility_BasicAttack::StartStep()`
- L90: `void UHodgeGameplayAbility_BasicAttack::OnAttackPressed(float TimeWaited)`
- L96: `void UHodgeGameplayAbility_BasicAttack::OnComboWindowChanged(FGameplayTag Tag, int32 NewCount)`
- L101: `void UHodgeGameplayAbility_BasicAttack::TryAdvance()`
- L116: `void UHodgeGameplayAbility_BasicAttack::OnTimelineEnded(const FGameplayEventData* Payload)`
- L121: `void UHodgeGameplayAbility_BasicAttack::OnCompleted()`
- L129: `void UHodgeGameplayAbility_BasicAttack::OnInterrupted()`
- L137: `void UHodgeGameplayAbility_BasicAttack::ClearStep()`
- L168: `void UHodgeGameplayAbility_BasicAttack::EndAbility(FGameplayAbilitySpecHandle Handle,`

## HodgeGameplayAbility_Definition.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Component/HodgeComboComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeComboComponent.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)

定义候选（多行签名仅展示首行）：

- L10: `UHodgeGameplayAbility_Definition::UHodgeGameplayAbility_Definition(const FObjectInitializer& Initializer)`
- L19: `const UHodgeAbilityDefinition* UHodgeGameplayAbility_Definition::GetDefinition() const`
- L25: `void UHodgeGameplayAbility_Definition::ActivateConfirmedDefinition(FGameplayAbilitySpecHandle Handle,`
- L35: `bool UHodgeGameplayAbility_Definition::CanActivateAbility(FGameplayAbilitySpecHandle Handle,`
- L45: `void UHodgeGameplayAbility_Definition::ActivateAbility(FGameplayAbilitySpecHandle Handle,`
- L75: `void UHodgeGameplayAbility_Definition::FinishExecution(bool bCancelled, bool bReplicate)`
- L80: `void UHodgeGameplayAbility_Definition::EndAbility(FGameplayAbilitySpecHandle Handle,`
- L102: `FGameplayTagContainer UHodgeGameplayAbility_Definition::GetExecutionWindows() const`
- L106: `void UHodgeGameplayAbility_Definition::RefreshExecutionClock()`
- L110: `void UHodgeGameplayAbility_Definition::OnTimelineFinished(EHodgeTimelineStopReason Reason)`
- L116: `void UHodgeGameplayAbility_Definition::OnWindowsChanged()`
- L120: `void UHodgeGameplayAbility_Definition::OnPoint(FGameplayTag Tag)`

## HodgeAttributeSet.cpp

项目 AttributeSet 基础和 ASC 访问。

源码：[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

定义候选（多行签名仅展示首行）：

- L23: `UHodgeAttributeSet::UHodgeAttributeSet()`
- L28: `UWorld* UHodgeAttributeSet::GetWorld() const`
- L41: `UHodgeAbilitySystemComponent* UHodgeAttributeSet::GetHodgeAbilitySystemComponent() const`

## HodgeCombatSet.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeCombatSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeCombatSet.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)

定义候选（多行签名仅展示首行）：

- L11: `UHodgeCombatSet::UHodgeCombatSet()`
- L20: `void UHodgeCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L33: `void UHodgeCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)`
- L40: `void UHodgeCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)`

## HodgeHealthSet.cpp

Health/MaxHealth、BaseDamage/BaseHeal 和 Damage/Healing 元属性；有效结算、夹取、免疫和耗尽广播。

源码：[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L33: `UHodgeHealthSet::UHodgeHealthSet()`
- L50: `void UHodgeHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L63: `void UHodgeHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)`
- L99: `void UHodgeHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)`
- L116: `bool UHodgeHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)`
- L176: `void UHodgeHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)`
- L298: `void UHodgeHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const`
- L308: `void UHodgeHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)`
- L318: `void UHodgeHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)`
- L352: `void UHodgeHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const`

## HodgeDamageExecution.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeDamageExecution.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeDamageExecution.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Executions/HodgeDamageExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeDamageExecution.h)、[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)

定义候选（多行签名仅展示首行）：

- L47: `UHodgeDamageExecution::UHodgeDamageExecution()`
- L54: `void UHodgeDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,`

## HodgeHealExecution.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeHealExecution.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeHealExecution.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Executions/HodgeHealExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeHealExecution.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)

定义候选（多行签名仅展示首行）：

- L44: `UHodgeHealExecution::UHodgeHealExecution()`
- L51: `void UHodgeHealExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,`

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

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeAbilityTagRelationshipMapping.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h)、[AbilitySystem/HodgeGlobalAbilitySystem.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h)、[Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)、[Data/HodgeGameData.h](../../../Source/Hodgepodge/Public/Data/HodgeGameData.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Component/HodgeComboComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeComboComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)

定义候选（多行签名仅展示首行）：

- L32: `UHodgeAbilitySystemComponent::UHodgeAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)`
- L48: `void UHodgeAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L60: `void UHodgeAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)`
- L136: `void UHodgeAbilitySystemComponent::TryActivateAbilitiesOnSpawn()`
- L153: `void UHodgeAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc,`
- L221: `void UHodgeAbilitySystemComponent::CancelInputActivatedAbilities(bool bReplicateCancelAbility)`
- L238: `void UHodgeAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)`
- L265: `void UHodgeAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)`
- L292: `void UHodgeAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)`
- L317: `void UHodgeAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)`
- L338: `void UHodgeAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)`
- L468: `void UHodgeAbilitySystemComponent::ClearAbilityInput()`
- L481: `void UHodgeAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle,`
- L495: `void UHodgeAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle,`
- L517: `void UHodgeAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,`
- L531: `void UHodgeAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,`
- L560: `void UHodgeAbilitySystemComponent::HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags,`
- L571: `void UHodgeAbilitySystemComponent::GetAdditionalActivationTagRequirements(`
- L584: `void UHodgeAbilitySystemComponent::SetTagRelationshipMapping(UHodgeAbilityTagRelationshipMapping* NewMapping)`
- L590: `void UHodgeAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(`
- L597: `void UHodgeAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability,`
- L610: `bool UHodgeAbilitySystemComponent::IsActivationGroupBlocked(EHodgeAbilityActivationGroup Group) const`
- L641: `void UHodgeAbilitySystemComponent::AddAbilityToActivationGroup(EHodgeAbilityActivationGroup Group,`
- L689: `void UHodgeAbilitySystemComponent::RemoveAbilityFromActivationGroup(EHodgeAbilityActivationGroup Group,`
- L702: `void UHodgeAbilitySystemComponent::CancelActivationGroupAbilities(EHodgeAbilityActivationGroup Group,`
- L718: `void UHodgeAbilitySystemComponent::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)`
- L754: `void UHodgeAbilitySystemComponent::RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag)`
- L779: `void UHodgeAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle,`
- L794: `void UHodgeAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L801: `FGameplayAbilitySpecHandle UHodgeAbilitySystemComponent::GiveAbilityDefinition(`
- L829: `const UHodgeAbilityDefinition* UHodgeAbilitySystemComponent::FindAbilityDefinition(`
- L839: `FGameplayAbilitySpecHandle UHodgeAbilitySystemComponent::FindDefinitionAbility(FGameplayTag AbilityTag) const`
- L854: `void UHodgeAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)`
- L860: `void UHodgeAbilitySystemComponent::InternalServerTryActivateAbility(FGameplayAbilitySpecHandle Handle,`
- L885: `void UHodgeAbilitySystemComponent::ClientActivateAbilitySucceedWithEventData_Implementation(`

## HodgeAbilitySystemComponent_Montage.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent_Montage.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent_Montage.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)

定义候选（多行签名仅展示首行）：

- L6: `float UHodgeAbilitySystemComponent::PlayMontage(UGameplayAbility* Ability, FGameplayAbilityActivationInfo Info,`
- L21: `float UHodgeAbilitySystemComponent::PlayMontageSimulated(UAnimMontage* Montage, float Rate, FName Section)`
- L28: `void UHodgeAbilitySystemComponent::ApplyDefinitionMontageSettings()`
- L42: `void UHodgeAbilitySystemComponent::OnRep_DefinitionMontage()`
- L51: `void UHodgeAbilitySystemComponent::OnRep_ReplicatedAnimMontage()`
- L57: `void UHodgeAbilitySystemComponent::StopDefinitionMontage(UGameplayAbility* Ability, bool bNatural)`
- L64: `void UHodgeAbilitySystemComponent::CurrentMontageStop(float OverrideBlendOutTime)`

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

原生 GameplayTag 注册和移动状态标签映射；含攻击时间轴依赖的 Status.Attack.*（阶段 + 取消窗口 .Cancel.*）与 GameplayEvent.Attack.*。标签存在不等于对应玩法实现。

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

## HodgeTimelineEvaluator.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeTimelineEvaluator.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeTimelineEvaluator.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeTimelineEvaluator.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeTimelineEvaluator.h)

定义候选（多行签名仅展示首行）：

- L3: `float FHodgeTimelineEvaluator::WindowEnd(const FHodgeTimelineEvent& Event, float Duration)`
- L9: `void FHodgeTimelineEvaluator::Collect(TConstArrayView<FHodgeTimelineEvent> Events, float Duration,`
- L41: `void FHodgeTimelineEvaluator::Sort(TConstArrayView<FHodgeTimelineEvent> Events, TArray<FHodgeTimelineNode>& Nodes)`
- L53: `void FHodgeTimelineEvaluator::EvaluateRange(TConstArrayView<FHodgeTimelineEvent> Events, float Duration,`
- L60: `void FHodgeTimelineEvaluator::EvaluateAt(TConstArrayView<FHodgeTimelineEvent> Events, float Duration,`

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

## HodgeAbilityTask_PlayTimeline.h

驱动 HodgeAbilityTimeline 的唯一 AbilityTask：初始化与 Tick 共用 CollectNodes + SortNodes 统一 Scheduler，推进逻辑时间，维护 WindowTag 与 GE 两个账本，派发 Point 与系统事件。窗口 GE 施加/移除、Point 与 Timeline.End 派发、中途取消清理已实测通过；重入类时序、NetPolicy 跨端、时钟倒退未验证。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeTimelineEvaluator.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeTimelineEvaluator.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  21: #pragma once
  23: #include "CoreMinimal.h"
  24: #include "Abilities/Tasks/AbilityTask.h"
  25: #include "GameplayEffectTypes.h"
  26: #include "GameplayTagContainer.h"
  27: #include "AbilitySystem/HodgeTimelineEvaluator.h"
  28: #include "Templates/SubclassOf.h"
  30: #include "HodgeAbilityTask_PlayTimeline.generated.h"
  32: class UGameplayEffect;
  33: class UAnimInstance;
  34: class UAnimMontage;
  35: class UHodgeAbilityTimeline;
  36: struct FHodgeTimelineEvent;
  43: UENUM(BlueprintType)
  44: enum class EHodgeTimelineStopReason : uint8
  45: {
  47: 	None,
  50: 	NaturalEnd,
  53: 	Interrupted,
  56: 	AbilityCancelled
  57: };
  62: UCLASS()
  63: class HODGEPODGE_API UHodgeAbilityTask_PlayTimeline : public UAbilityTask
  64: {
  65: 	GENERATED_BODY()
  67: public:
  68: 	UHodgeAbilityTask_PlayTimeline(const FObjectInitializer& ObjectInitializer);
  77: 	UFUNCTION(BlueprintCallable, Category="Hodge|Ability|Tasks",
  78: 		meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility",
  79: 			BlueprintInternalUseOnly="TRUE"))
  80: 	static UHodgeAbilityTask_PlayTimeline* PlayTimeline(
  81: 		UGameplayAbility* OwningAbility,
  82: 		UHodgeAbilityTimeline* Timeline,
  83: 		float StartOffset = 0.f,
  84: 		float InitialPlayRate = 1.f);
  90: 	UFUNCTION(BlueprintCallable, Category="Hodge|Ability|Tasks")
  91: 	void StopTimeline(EHodgeTimelineStopReason Reason);
  93: 	static UHodgeAbilityTask_PlayTimeline* PlayMontageTimeline(UGameplayAbility* OwningAbility,
  94: 	                                                           UHodgeAbilityTimeline* Timeline,
  95: 	                                                           UAnimInstance* AnimInstance, UAnimMontage* Montage,
  96: 	                                                           int32 InstanceID);
  97: 	FGameplayTagContainer GetActiveWindowTags() const;
  98: 	DECLARE_MULTICAST_DELEGATE(FWindowsChanged);
  99: 	DECLARE_MULTICAST_DELEGATE_OneParam(FFinished, EHodgeTimelineStopReason);
 100: 	DECLARE_MULTICAST_DELEGATE_OneParam(FPoint, FGameplayTag);
 101: 	FWindowsChanged OnWindowsChanged;
 102: 	FFinished OnFinished;
 103: 	FPoint OnPoint;
 104: 	void RefreshMontageClock();
 107: 	UFUNCTION(BlueprintPure, Category="Hodge|Ability|Tasks")
 108: 	bool IsTimelineStopped() const { return bStopped; }
 110: protected:
 111: 	virtual void Activate() override;
 112: 	virtual void TickTask(float DeltaTime) override;
 113: 	virtual void OnDestroy(bool bInOwnerFinished) override;
 118: 	void InitializeTimeline(float InStartOffset);
 121: 	void AdvanceTimeline(float PreviousTime, float CurrentTime);
 125: 	void CollectNodes(float PreviousTime, float CurrentTime, bool bInitializing,
 126: 	                  TArray<FHodgeTimelineNode>& OutNodes) const;
 129: 	void SortNodes(TArray<FHodgeTimelineNode>& Nodes) const;
 132: 	void EnterWindow(int32 EventIndex);
 135: 	void ExitWindow(int32 EventIndex);
 138: 	void FirePointEvent(const FHodgeTimelineEvent& Event);
 141: 	void FireSystemEvent(const FGameplayTag& EventTag);
 144: 	void ClearAllWindowState();
 148: 	FActiveGameplayEffectHandle ApplyTimelineEffect(UAbilitySystemComponent* ASC,
 149: 	                                                TSubclassOf<UGameplayEffect> EffectClass);
 152: 	bool HasAuthorityOnAvatar() const;
 154: private:
 155: 	friend struct FHodgeTimelineTestAccess;
 156: 	TWeakObjectPtr<UAnimInstance> ClockAnimInstance;
 157: 	UPROPERTY()
 158: 	TObjectPtr<UAnimMontage> ClockMontage;
 159: 	int32 ClockInstanceID = INDEX_NONE;
 160: 	float EffectiveDuration = 0.f;
 161: 	bool bAdvancingMontage = false;
 163: 	UPROPERTY()
 164: 	TObjectPtr<UHodgeAbilityTimeline> TimelineAsset;
 167: 	float StartOffset = 0.f;
 170: 	float InitialPlayRate = 1.f;
 173: 	float LastUpdateWorldTime = 0.f;
 176: 	float LogicalElapsed = 0.f;
 179: 	float ElapsedTime = 0.f;
 182: 	TArray<int32> ActiveWindowIndices;
 185: 	TMap<int32, FActiveGameplayEffectHandle> WindowEffectHandles;
 188: 	bool bStopped = false;
 192: 	bool bCleanedUp = false;
 193: };
```

## HodgeAbilityTask_WaitMoveCancel.h

把“取消窗口（Timeline 授权）”与“移动意图（输入层）”两个变化驱动信号合流的 AbilityTask：同时成立时广播 OnMoveCancel 一次后自结束。本地控制端语义；AI/模拟代理找不到 HeroComponent 时永不成立。工作区新增，未编译、未 PIE。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  27: #pragma once
  29: #include "CoreMinimal.h"
  30: #include "Abilities/Tasks/AbilityTask.h"
  31: #include "GameplayTagContainer.h"
  33: #include "HodgeAbilityTask_WaitMoveCancel.generated.h"
  35: class UAbilitySystemComponent;
  36: class UHodgeHeroComponent;
  39: DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHodgeMoveCancelDelegate);
  41: UCLASS()
  42: class HODGEPODGE_API UHodgeAbilityTask_WaitMoveCancel : public UAbilityTask
  43: {
  44: 	GENERATED_BODY()
  46: public:
  47: 	UHodgeAbilityTask_WaitMoveCancel(const FObjectInitializer& ObjectInitializer);
  49: 	UPROPERTY(BlueprintAssignable)
  50: 	FHodgeMoveCancelDelegate OnMoveCancel;
  58: 	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
  59: 		meta = (DisplayName = "Wait Move Cancel", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility",
  60: 			BlueprintInternalUseOnly = "TRUE"))
  61: 	static UHodgeAbilityTask_WaitMoveCancel* WaitMoveCancel(UGameplayAbility* OwningAbility,
  62: 	                                                        FGameplayTag CancelWindowTag,
  63: 	                                                        float MoveIntentThreshold = 0.1f);
  65: protected:
  66: 	virtual void Activate() override;
  67: 	virtual void TickTask(float DeltaTime) override;
  68: 	virtual void OnDestroy(bool bInOwnerFinished) override;
  70: private:
  72: 	void Evaluate(const TCHAR* Source);
  75: 	UFUNCTION()
  76: 	void HandleMoveIntentChanged(bool bHasMoveIntent);
  79: 	void HandleWindowTagChanged(FGameplayTag Tag, int32 NewCount);
  82: 	FGameplayTag WindowTag;
  85: 	float IntentThreshold = 0.1f;
  88: 	bool bSucceeded = false;
  91: 	bool bInOnDestroy = false;
  94: 	TWeakObjectPtr<UAbilitySystemComponent> SubscribedASC;
  95: 	TWeakObjectPtr<UHodgeHeroComponent> SubscribedHeroComponent;
  98: 	FDelegateHandle WindowTagDelegateHandle;
  99: };
```

## HodgeGameplayAbility.h

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。（PreloadPrimaryAssetsOnGrant 已随未提交改动回退，当前不存在。）

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
  21: UENUM(BlueprintType)
  22: enum class EHodgeAbilityActivationPolicy : uint8
  23: {
  25: 	OnInputTriggered,
  28: 	WhileInputActive,
  31: 	OnSpawn
  32: };
  40: UENUM(BlueprintType)
  41: enum class EHodgeAbilityActivationGroup : uint8
  42: {
  44: 	Independent,
  47: 	Exclusive_Replaceable,
  50: 	Exclusive_Blocking,
  53: 	MAX UMETA(Hidden)
  54: };
  57: USTRUCT(BlueprintType)
  58: struct FHodgeAbilityMontageFailureMessage
  59: {
  60: 	GENERATED_BODY()
  62: public:
  64: 	UPROPERTY(BlueprintReadWrite)
  65: 	TObjectPtr<APlayerController> PlayerController = nullptr;
  68: 	UPROPERTY(BlueprintReadWrite)
  69: 	TObjectPtr<AActor> AvatarActor = nullptr;
  72: 	UPROPERTY(BlueprintReadWrite)
  73: 	FGameplayTagContainer FailureTags;
  76: 	UPROPERTY(BlueprintReadWrite)
  77: 	TObjectPtr<UAnimMontage> FailureMontage = nullptr;
  78: };
  92: UCLASS(Abstract, HideCategories = Input,
  93: 	Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
  94: class HODGEPODGE_API UHodgeGameplayAbility : public UGameplayAbility
  95: {
  96: 	GENERATED_BODY()
  99: 	friend class UHodgeAbilitySystemComponent;
 101: public:
 103: 	UHodgeGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
 106: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 107: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponentFromActorInfo() const;
 110: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 111: 	AHodgePlayerController* GetHodgePlayerControllerFromActorInfo() const;
 114: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 115: 	AController* GetControllerFromActorInfo() const;
 118: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 119: 	AHodgeCombatCharacter* GetHodgeCharacterFromActorInfo() const;
 122: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 123: 	UHodgeHeroComponent* GetHeroComponentFromActorInfo() const;
 126: 	EHodgeAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
 129: 	EHodgeAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }
 132: 	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
 136: 	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Ability",
 137: 		Meta = (ExpandBoolAsExecs = "ReturnValue"))
 138: 	bool CanChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup) const;
 142: 	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Ability",
 143: 		Meta = (ExpandBoolAsExecs = "ReturnValue"))
 144: 	bool ChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup);
 147: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 148: 	void SetCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode);
 151: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 152: 	void ClearCameraMode();
 155: 	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
 156: 	{
 158: 		NativeOnAbilityFailedToActivate(FailedReason);
 161: 		ScriptOnAbilityFailedToActivate(FailedReason);
 162: 	}
 164: protected:
 166: 	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;
 169: 	UFUNCTION(BlueprintImplementableEvent)
 170: 	void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;
 174: 	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 175: 	                                const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
 176: 	                                FGameplayTagContainer* OptionalRelevantTags) const override;
 179: 	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
 182: 	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
 185: 	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
 188: 	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 189: 	                             const FGameplayAbilityActivationInfo ActivationInfo,
 190: 	                             const FGameplayEventData* TriggerEventData) override;
 193: 	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 194: 	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
 195: 	                        bool bWasCancelled) override;
 198: 	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 199: 	                       OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
 202: 	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 203: 	                       const FGameplayAbilityActivationInfo ActivationInfo) const override;
 206: 	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle,
 207: 	                                                       const FGameplayAbilityActorInfo* ActorInfo) const override;
 210: 	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,
 211: 	                                                  FGameplayAbilitySpec* AbilitySpec) const override;
 214: 	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
 215: 	                                               const FGameplayTagContainer* SourceTags = nullptr,
 216: 	                                               const FGameplayTagContainer* TargetTags = nullptr,
 217: 	                                               OUT FGameplayTagContainer* OptionalRelevantTags = nullptr)
 218: 	const override;
 222: 	virtual void OnPawnAvatarSet();
 225: 	virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 226: 	                              float& OutSourceLevel, const IHodgeAbilitySourceInterface*& OutAbilitySource,
 227: 	                              AActor*& OutEffectCauser) const;
 231: 	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
 232: 	void K2_OnAbilityAdded();
 236: 	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
 237: 	void K2_OnAbilityRemoved();
 241: 	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
 242: 	void K2_OnPawnAvatarSet();
 244: protected:
 246: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Ability Activation")
 247: 	EHodgeAbilityActivationPolicy ActivationPolicy;
 250: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Ability Activation")
 251: 	EHodgeAbilityActivationGroup ActivationGroup;
 254: 	UPROPERTY(EditDefaultsOnly, Instanced, Category = Costs)
 255: 	TArray<TObjectPtr<UHodgeAbilityCost>> AdditionalCosts;
 258: 	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
 259: 	TMap<FGameplayTag, FText> FailureTagToUserFacingMessages;
 262: 	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
 263: 	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> FailureTagToAnimMontage;
 266: 	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
 267: 	bool bLogCancelation;
 270: 	TSubclassOf<UHodgeCameraMode> ActiveCameraMode;
 271: };
```

## HodgeGameplayAbility_BasicAttack.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_BasicAttack.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_BasicAttack.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
   5: #include "HodgeGameplayAbility_BasicAttack.generated.h"
   7: class UHodgeAbilityTimeline;
   8: class UHodgeAbilityTask_PlayTimeline;
   9: class UHodgeAbilityTask_WaitMoveCancel;
  10: class UAbilityTask_PlayMontageAndWait;
  11: class UAbilityTask_WaitInputPress;
  13: USTRUCT(BlueprintType)
  14: struct FHodgeBasicAttackStep
  15: {
  16: 	GENERATED_BODY()
  18: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  19: 	TObjectPtr<UAnimMontage> Montage;
  21: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  22: 	TObjectPtr<UHodgeAbilityTimeline> Timeline;
  23: };
  26: UCLASS(Blueprintable)
  27: class HODGEPODGE_API UHodgeGameplayAbility_BasicAttack : public UHodgeGameplayAbility
  28: {
  29: 	GENERATED_BODY()
  31: public:
  32: 	UHodgeGameplayAbility_BasicAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  34: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
  35: 	TArray<FHodgeBasicAttackStep> AttackSteps;
  37: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack", meta=(ClampMin="0.01"))
  38: 	float PlayRate = 1.f;
  40: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack", meta=(ClampMin="0.0"))
  41: 	float CancelBlendOutTime = 0.1f;
  43: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack", meta=(ClampMin="0.0"))
  44: 	float MoveIntentThreshold = 0.1f;
  47: 	UPROPERTY(BlueprintReadOnly, Transient, Category="Attack")
  48: 	int32 CurrentAttackStep = 0;
  50: protected:
  51: 	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
  52: 		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
  53: 	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
  54: 		FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
  56: private:
  57: 	void StartStep();
  58: 	void ClearStep();
  59: 	void TryAdvance();
  60: 	void OnComboWindowChanged(FGameplayTag Tag, int32 NewCount);
  61: 	void OnTimelineEnded(const FGameplayEventData* Payload);
  62: 	UFUNCTION() void OnAttackPressed(float TimeWaited);
  63: 	UFUNCTION() void OnCompleted();
  64: 	UFUNCTION() void OnInterrupted();
  66: 	UPROPERTY(Transient) TObjectPtr<UHodgeAbilityTask_PlayTimeline> TimelineTask;
  67: 	UPROPERTY(Transient) TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
  68: 	UPROPERTY(Transient) TObjectPtr<UAbilityTask_WaitInputPress> InputTask;
  69: 	UPROPERTY(Transient) TObjectPtr<UHodgeAbilityTask_WaitMoveCancel> MoveTask;
  70: 	FDelegateHandle ComboHandle;
  71: 	FDelegateHandle TimelineEndHandle;
  72: 	bool bBufferedAttack = false;
  73: 	bool bChangingStep = false;
  74: 	bool bEndingAttack = false;
  75: };
```

## HodgeGameplayAbility_Definition.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   2: #include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
   3: #include "AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h"
   4: #include "HodgeGameplayAbility_Definition.generated.h"
   6: class UHodgeAbilityDefinition;
   9: UCLASS(Blueprintable)
  10: class HODGEPODGE_API UHodgeGameplayAbility_Definition : public UHodgeGameplayAbility
  11: {
  12: 	GENERATED_BODY()
  13: public:
  14: 	UHodgeGameplayAbility_Definition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  15: 	UFUNCTION(BlueprintPure, Category="Hodge|Ability") const UHodgeAbilityDefinition* GetDefinition() const;
  16: 	FGameplayTagContainer GetExecutionWindows() const;
  17: 	void FinishExecution(bool bCancelled, bool bReplicate);
  18: 	void RefreshExecutionClock();
  19: 	void ActivateConfirmedDefinition(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
  20: 		const FPredictionKey& Key, const FGameplayEventData& Payload);
  21: 	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
  22: 		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
  23: 		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
  24: protected:
  25: 	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
  26: 		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
  27: 	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
  28: 		FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
  29: private:
  30: 	void OnTimelineFinished(EHodgeTimelineStopReason Reason);
  31: 	void OnWindowsChanged();
  32: 	void OnPoint(FGameplayTag Tag);
  33: 	UPROPERTY(Transient) TObjectPtr<UHodgeAbilityTask_PlayTimeline> TimelineTask;
  34: 	bool bEnding = false;
  35: };
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

## HodgeCombatSet.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   6: #include "CoreMinimal.h"
   9: #include "AbilitySystemComponent.h"
  12: #include "HodgeAttributeSet.h"
  14: #include "HodgeCombatSet.generated.h"
  28: UCLASS(BlueprintType)
  29: class UHodgeCombatSet : public UHodgeAttributeSet
  30: {
  31: 	GENERATED_BODY()
  33: public:
  35: 	UHodgeCombatSet();
  38: 	ATTRIBUTE_ACCESSORS(UHodgeCombatSet, BaseDamage);
  41: 	ATTRIBUTE_ACCESSORS(UHodgeCombatSet, BaseHeal);
  43: protected:
  45: 	UFUNCTION()
  46: 	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);
  49: 	UFUNCTION()
  50: 	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);
  52: private:
  55: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Hodge|Combat",
  56: 		Meta = (AllowPrivateAccess = true))
  57: 	FGameplayAttributeData BaseDamage;
  61: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Hodge|Combat",
  62: 		Meta = (AllowPrivateAccess = true))
  63: 	FGameplayAttributeData BaseHeal;
  64: };
```

## HodgeHealthSet.h

Health/MaxHealth、BaseDamage/BaseHeal 和 Damage/Healing 元属性；有效结算、夹取、免疫和耗尽广播。

源码：[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   7: #include "CoreMinimal.h"
  10: #include "AbilitySystemComponent.h"
  13: #include "NativeGameplayTags.h"
  14: #include "AbilitySystem/AttributeSet/HodgeAttributeSet.h"
  16: #include "HodgeHealthSet.generated.h"
  19: class UObject;
  22: struct FFrame;
  25: HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
  28: HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
  31: HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
  34: HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);
  37: struct FGameplayEffectModCallbackData;
  52: UCLASS(BlueprintType)
  53: class HODGEPODGE_API UHodgeHealthSet : public UHodgeAttributeSet
  54: {
  55: 	GENERATED_BODY()
  57: public:
  59: 	UHodgeHealthSet();
  62: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Health);
  65: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, MaxHealth);
  68: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Healing);
  71: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Damage);
  75: 	mutable FHodgeAttributeEvent OnHealthChanged;
  79: 	mutable FHodgeAttributeEvent OnMaxHealthChanged;
  83: 	mutable FHodgeAttributeEvent OnOutOfHealth;
  85: protected:
  87: 	UFUNCTION()
  88: 	void OnRep_Health(const FGameplayAttributeData& OldValue);
  91: 	UFUNCTION()
  92: 	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
  95: 	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
  98: 	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
 101: 	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
 104: 	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
 107: 	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
 110: 	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
 112: private:
 116: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Hodge|Health",
 117: 		Meta = (HideFromModifiers, AllowPrivateAccess = true))
 118: 	FGameplayAttributeData Health;
 122: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Hodge|Health",
 123: 		Meta = (AllowPrivateAccess = true))
 124: 	FGameplayAttributeData MaxHealth;
 128: 	bool bOutOfHealth;
 132: 	float MaxHealthBeforeAttributeChange;
 135: 	float HealthBeforeAttributeChange;
 146: 	UPROPERTY(BlueprintReadOnly, Category="Hodge|Health", Meta=(AllowPrivateAccess=true))
 147: 	FGameplayAttributeData Healing;
 152: 	UPROPERTY(BlueprintReadOnly, Category="Hodge|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
 153: 	FGameplayAttributeData Damage;
 154: };
```

## HodgeDamageExecution.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeDamageExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeDamageExecution.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   6: #include "CoreMinimal.h"
  10: #include "GameplayEffectExecutionCalculation.h"
  12: #include "HodgeDamageExecution.generated.h"
  26: UCLASS()
  27: class HODGEPODGE_API UHodgeDamageExecution : public UGameplayEffectExecutionCalculation
  28: {
  29: 	GENERATED_BODY()
  31: public:
  34: 	UHodgeDamageExecution();
  36: protected:
  40: 	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
  41: 	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
  42: };
```

## HodgeHealExecution.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeHealExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeHealExecution.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   6: #include "CoreMinimal.h"
  10: #include "GameplayEffectExecutionCalculation.h"
  12: #include "HodgeHealExecution.generated.h"
  26: UCLASS()
  27: class HODGEPODGE_API UHodgeHealExecution : public UGameplayEffectExecutionCalculation
  28: {
  29: 	GENERATED_BODY()
  31: public:
  34: 	UHodgeHealExecution();
  36: protected:
  40: 	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
  41: 	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
  42: };
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
  11: class UHodgeAbilityDefinition;
  13: USTRUCT()
  14: struct FHodgeDefinitionMontageState
  15: {
  16: 	GENERATED_BODY()
  17: 	UPROPERTY()
  18: 	TObjectPtr<const UHodgeAbilityDefinition> Definition;
  19: 	UPROPERTY()
  20: 	uint8 PlayInstanceID = 0;
  21: 	UPROPERTY()
  22: 	bool bNaturalStop = false;
  23: };
  25: USTRUCT()
  26: struct FHodgeGrantedAbilityDefinition
  27: {
  28: 	GENERATED_BODY()
  29: 	UPROPERTY()
  30: 	FGameplayAbilitySpecHandle Handle;
  31: 	UPROPERTY()
  32: 	TObjectPtr<const UHodgeAbilityDefinition> Definition;
  33: };
  36: HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_AbilityInputBlocked);
  50: UCLASS()
  51: class HODGEPODGE_API UHodgeAbilitySystemComponent : public UAbilitySystemComponent
  52: {
  53: 	GENERATED_BODY()
  55: public:
  57: 	UHodgeAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  59: 	virtual float PlayMontage(UGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo,
  60: 	                          UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None,
  61: 	                          float StartTimeSeconds = 0.f) override;
  62: 	virtual float
  63: 	PlayMontageSimulated(UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None) override;
  64: 	virtual void CurrentMontageStop(float OverrideBlendOutTime = -1.f) override;
  65: 	void StopDefinitionMontage(UGameplayAbility* Ability, bool bNatural);
  66: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
  67: 	FGameplayAbilitySpecHandle GiveAbilityDefinition(const UHodgeAbilityDefinition* Definition, int32 Level,
  68: 	                                                 UObject* SourceObject);
  69: 	UFUNCTION(BlueprintPure, Category="Hodge|Abilities")
  70: 	const UHodgeAbilityDefinition* FindAbilityDefinition(FGameplayAbilitySpecHandle Handle) const;
  71: 	FGameplayAbilitySpecHandle FindDefinitionAbility(FGameplayTag AbilityTag) const;
  75: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  79: 	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
  82: 	typedef TFunctionRef<bool(const UHodgeGameplayAbility* HodgeAbility, FGameplayAbilitySpecHandle Handle)>
  83: 	TShouldCancelAbilityFunc;
  86: 	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);
  89: 	void CancelInputActivatedAbilities(bool bReplicateCancelAbility);
  92: 	void AbilityInputTagPressed(const FGameplayTag& InputTag);
  95: 	void AbilityInputTagReleased(const FGameplayTag& InputTag);
  98: 	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
 101: 	void ClearAbilityInput();
 104: 	bool IsActivationGroupBlocked(EHodgeAbilityActivationGroup Group) const;
 107: 	void AddAbilityToActivationGroup(EHodgeAbilityActivationGroup Group, UHodgeGameplayAbility* HodgeAbility);
 110: 	void RemoveAbilityFromActivationGroup(EHodgeAbilityActivationGroup Group, UHodgeGameplayAbility* HodgeAbility);
 113: 	void CancelActivationGroupAbilities(EHodgeAbilityActivationGroup Group, UHodgeGameplayAbility* IgnoreHodgeAbility,
 114: 	                                    bool bReplicateCancelAbility);
 118: 	void AddDynamicTagGameplayEffect(const FGameplayTag& Tag);
 122: 	void RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag);
 126: 	void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle,
 127: 	                          FGameplayAbilityActivationInfo ActivationInfo,
 128: 	                          FGameplayAbilityTargetDataHandle& OutTargetDataHandle);
 132: 	void SetTagRelationshipMapping(UHodgeAbilityTagRelationshipMapping* NewMapping);
 136: 	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags,
 137: 	                                            FGameplayTagContainer& OutActivationRequired,
 138: 	                                            FGameplayTagContainer& OutActivationBlocked) const;
 140: protected:
 141: 	UPROPERTY(ReplicatedUsing=OnRep_DefinitionMontage)
 142: 	FHodgeDefinitionMontageState DefinitionMontage;
 143: 	UFUNCTION()
 144: 	void OnRep_DefinitionMontage();
 145: 	virtual void OnRep_ReplicatedAnimMontage() override;
 146: 	void ApplyDefinitionMontageSettings();
 147: 	int32 ConfiguredMontageInstance = INDEX_NONE;
 148: 	virtual void ClientActivateAbilitySucceedWithEventData_Implementation(
 149: 		FGameplayAbilitySpecHandle Handle, FPredictionKey PredictionKey, FGameplayEventData TriggerEventData) override;
 150: 	TArray<int16> RecentComboPredictionKeys;
 151: 	virtual void InternalServerTryActivateAbility(FGameplayAbilitySpecHandle Handle, bool InputPressed,
 152: 	                                              const FPredictionKey& PredictionKey,
 153: 	                                              const FGameplayEventData* TriggerEventData) override;
 154: 	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;
 155: 	UPROPERTY(Replicated)
 156: 	TArray<FHodgeGrantedAbilityDefinition> GrantedDefinitions;
 159: 	void TryActivateAbilitiesOnSpawn();
 162: 	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
 165: 	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
 168: 	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
 171: 	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,
 172: 	                                 const FGameplayTagContainer& FailureReason) override;
 175: 	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,
 176: 	                                bool bWasCancelled) override;
 179: 	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,
 180: 	                                            UGameplayAbility* RequestingAbility, bool bEnableBlockTags,
 181: 	                                            const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags,
 182: 	                                            const FGameplayTagContainer& CancelTags) override;
 185: 	virtual void HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags,
 186: 	                                              UGameplayAbility* RequestingAbility, bool bCanBeCanceled) override;
 190: 	UFUNCTION(Client, Unreliable)
 191: 	void ClientNotifyAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);
 194: 	void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);
 196: protected:
 200: 	UPROPERTY()
 201: 	TObjectPtr<UHodgeAbilityTagRelationshipMapping> TagRelationshipMapping;
 205: 	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
 209: 	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
 213: 	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
 217: 	int32 ActivationGroupCounts[(uint8)EHodgeAbilityActivationGroup::MAX];
 218: };
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

原生 GameplayTag 注册和移动状态标签映射；含攻击时间轴依赖的 Status.Attack.*（阶段 + 取消窗口 .Cancel.*）与 GameplayEvent.Attack.*。标签存在不等于对应玩法实现。

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
  28: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Weapon_NoFiring);
  33: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dash_Duration_Message);
  34: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Grenade_Duration_Message);
  35: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Interaction_Activate);
  36: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Interaction_Duration_Message);
  37: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Respawn_Completed_Message);
  38: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Respawn_Duration_Message);
  43: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type);
  44: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action);
  45: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_ADS);
  46: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Dash);
  47: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Drop);
  48: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Emote);
  49: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Grenade);
  50: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Jump);
  51: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Melee);
  52: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Reload);
  53: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_WeaponFire);
  54: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Info);
  55: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Info_ShowLeaderboard);
  56: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive);
  57: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_AutoReload);
  58: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_AutoRespawn);
  59: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_ChangeQuickbarSlot);
  60: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange);
  61: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange_Death);
  62: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange_Spawning);
  67: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic);
  68: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_AnimationStyle_Feminine);
  69: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_AnimationStyle_Masculine);
  70: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_BodyStyle_Medium);
  75: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Zone);
  76: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Zone_WeakSpot);
  81: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_DamageTaken);
  82: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Dash);
  83: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Dash_Cooldown);
  84: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Death);
  85: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Heal);
  86: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Melee_Cooldown);
  91: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_Burst);
  92: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_BurstLatent);
  93: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_Looping);
  98: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Grenade_Cooldown);
  99: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Grenade_Detonate);
 100: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Melee_Hit);
 101: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Melee_Impact);
 102: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Pistol_Fire);
 103: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Rifle_Fire);
 104: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Rifle_Impact);
 105: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Shotgun_Fire);
 110: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_World_Launcher_Activate);
 111: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_World_Teleporter_Activate);
 116: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageTrait_Instant);
 117: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageTrait_Periodic);
 118: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Basic);
 119: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Grenade);
 120: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Melee);
 121: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Pistol);
 122: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Rifle);
 123: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Shotgun);
 124: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Heal_Instant);
 125: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Heal_Periodic);
 130: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_MeleeHit);
 131: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
 132: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Reset);
 133: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RequestReset);
 138: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack);
 139: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_Test);
 140: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_Timeline_End);
 141: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_Interrupted);
 146: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameSettings_Action_EditBrightness);
 147: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameSettings_Action_EditSafeZone);
 152: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_ExtraEquipment);
 153: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_InfrequentAbilities);
 154: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_LeftSideTouchInputs);
 155: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_LeftSideTouchRegion);
 156: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RespawnTimer);
 157: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RightSideTouchInputs);
 158: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RightSideTouchRegion);
 163: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Dash);
 164: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Heal);
 165: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Melee);
 166: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Quickslot_Drop);
 167: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Jump);
 172: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_ADS);
 173: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Fire);
 174: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_FireAuto);
 175: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Grenade);
 176: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Reload);
 181: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
 182: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
 183: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
 184: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Stick);
 185: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
 186: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_AutoRun);
 194: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Sprint);
 195: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Walk);
 196: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Aim);
 197: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ragdoll);
 198: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Roll);
 199: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_RotationMode);
 200: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_ViewMode);
 201: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_SwitchShoulder);
 206: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
 207: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
 208: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
 209: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);
 214: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_AddNotification_Message);
 215: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_Assist_Message);
 216: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_Damage_Taken_Message);
 217: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_Elimination_Message);
 218: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_HUD_PlayerHUD);
 219: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_HUD_TempTopWidgets);
 220: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_Inventory_Message_StackChanged);
 221: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_Player);
 222: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_QuickBar_Message_ActiveIndexChanged);
 223: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_QuickBar_Message_SlotsChanged);
 224: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_ShooterGame_Accolade);
 225: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_Weapon_SteadyAimingCamera);
 230: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_BinauralSettingControlledByOS);
 231: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_CanExitApplication);
 232: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_PrimarlyController);
 233: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_HasStrictControllerPairing);
 234: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_PrimarlyTouchScreen);
 235: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_SupportsMouseAndKeyboard);
 236: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_HardwareCursor);
 237: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsBackgroundAudio);
 238: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsChangingAudioOutputDevice);
 239: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsWindowedMode);
 240: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_SupportsGamepad);
 241: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_SupportsTriggerHaptics);
 242: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_NeedsBrightnessAdjustment);
 243: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_ReplaySupport);
 244: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_RequiresStrictControllerMapping);
 245: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SingleOnlineUser);
 250: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SystemMessage_Display);
 251: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SystemMessage_Error);
 252: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SystemMessage_Error_InitializeLocalPlayerFailed);
 253: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SystemMessage_Warning);
 258: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ShooterGame_GamePhase_MatchBeginCountdown);
 263: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_SpawningIn);
 264: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
 265: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AutoRunning);
 266: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
 267: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
 268: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);
 271: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack);
 272: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Windup);
 273: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Active);
 274: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Recovery);
 282: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Cancel);
 283: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Cancel_Move);
 284: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Cancel_NextAttack);
 289: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
 290: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);
 295: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_GodMode);
 296: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_UnlimitedHealth);
 301: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Back);
 302: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Escape);
 303: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Game);
 304: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_GameMenu);
 305: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Menu);
 306: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Modal);
 311: 	HODGEPODGE_API extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
 312: 	HODGEPODGE_API extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;
 314: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
 315: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
 316: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
 317: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
 318: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);
 319: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);
 320: };
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

## HodgeTimelineEvaluator.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeTimelineEvaluator.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeTimelineEvaluator.h)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "Data/HodgeAbilityTimeline.h"
   7: enum class EHodgeTimelineNodeKind : uint8 { WindowEnd, WindowBegin, PointFire };
   8: struct FHodgeTimelineNode
   9: {
  10:     float Time = 0.f;
  11:     EHodgeTimelineNodeKind Kind = EHodgeTimelineNodeKind::PointFire;
  12:     int32 EventIndex = INDEX_NONE;
  13: };
  16: class HODGEPODGE_API FHodgeTimelineEvaluator
  17: {
  18: public:
  19:     static constexpr float WindowEndTolerance = 1.e-3f;
  20:     static float WindowEnd(const FHodgeTimelineEvent& Event, float Duration);
  21:     static void Collect(TConstArrayView<FHodgeTimelineEvent> Events, float Duration,
  22:         float PreviousTime, float CurrentTime, bool bInitialize, TArray<FHodgeTimelineNode>& OutNodes);
  23:     static void Sort(TConstArrayView<FHodgeTimelineEvent> Events, TArray<FHodgeTimelineNode>& Nodes);
  24:     static void EvaluateRange(TConstArrayView<FHodgeTimelineEvent> Events, float Duration,
  25:         float PreviousTime, float CurrentTime, TArray<FHodgeTimelineNode>& OutNodes);
  26:     static void EvaluateAt(TConstArrayView<FHodgeTimelineEvent> Events, float Duration,
  27:         float Time, TArray<int32>& OutActiveWindows);
  28: };
```
