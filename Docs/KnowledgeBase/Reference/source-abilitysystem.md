# AbilitySystem 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAbilityCost_ResonanceEnergy.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.h)、[AbilitySystem/Effects/HodgeResonanceEnergyEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h)、[AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L9: `UHodgeAbilityCost_ResonanceEnergy::UHodgeAbilityCost_ResonanceEnergy()`
- L14: `bool UHodgeAbilityCost_ResonanceEnergy::ValidateEffect() const`
- L23: `bool UHodgeAbilityCost_ResonanceEnergy::CheckCost(const UHodgeGameplayAbility* Ability, FGameplayAbilitySpecHandle Handle,`
- L33: `void UHodgeAbilityCost_ResonanceEnergy::ApplyCost(const UHodgeGameplayAbility* Ability, FGameplayAbilitySpecHandle Handle,`

## HodgeAbilityTask_WaitHitResults.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Component/HodgeHitDetectionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHitDetectionComponent.h)

定义候选（多行签名仅展示首行）：

- L15: `void FHodgeHitResultsTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType,`
- L23: `FString FHodgeHitResultsTickFunction::DiagnosticMessage()`
- L29: `UHodgeAbilityTask_WaitHitResults* UHodgeAbilityTask_WaitHitResults::WaitHitResults(`
- L43: `bool UHodgeAbilityTask_WaitHitResults::IsRunningForExecution(const FGuid& InExecutionId) const`
- L56: `void UHodgeAbilityTask_WaitHitResults::Activate()`
- L73: `uint64 UHodgeAbilityTask_WaitHitResults::CreateWindow(int32 OccurrenceId, const FHodgeHitDetectionRequest& Request)`
- L88: `bool UHodgeAbilityTask_WaitHitResults::IsBatchCurrent(const FHodgeHitDetectionBatch& Batch) const`
- L96: `bool UHodgeAbilityTask_WaitHitResults::PrepareSourcePose(uint64 Handle)`
- L102: `void UHodgeAbilityTask_WaitHitResults::SampleWindow(int32 OccurrenceId)`
- L133: `void UHodgeAbilityTask_WaitHitResults::CloseWindow(int32 OccurrenceId, bool bSampleFinal)`
- L146: `void UHodgeAbilityTask_WaitHitResults::TickDetection()`
- L173: `void UHodgeAbilityTask_WaitHitResults::UpdateTickState()`
- L181: `void UHodgeAbilityTask_WaitHitResults::Pause()`
- L188: `void UHodgeAbilityTask_WaitHitResults::Resume()`
- L207: `void UHodgeAbilityTask_WaitHitResults::ReleaseSessions()`
- L221: `void UHodgeAbilityTask_WaitHitResults::OnDestroy(bool bInOwnerFinished)`
- L228: `void UHodgeAbilityTask_WaitHitResults::BeginDestroy()`

## HodgeAbilityTask_WaitMoveCancel.cpp

合流取消窗口与本地移动意图，广播 OnMoveCancel 后结束；已有取消验证，当前 Combat 主链与旧 BasicAttack 消费方需区分。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)

定义候选（多行签名仅展示首行）：

- L18: `UHodgeAbilityTask_WaitMoveCancel::UHodgeAbilityTask_WaitMoveCancel(const FObjectInitializer& ObjectInitializer)`
- L25: `void UHodgeAbilityTask_WaitMoveCancel::TickTask(float DeltaTime)`
- L32: `UHodgeAbilityTask_WaitMoveCancel* UHodgeAbilityTask_WaitMoveCancel::WaitMoveCancel(`
- L42: `void UHodgeAbilityTask_WaitMoveCancel::Activate()`
- L79: `void UHodgeAbilityTask_WaitMoveCancel::OnDestroy(bool bInOwnerFinished)`
- L105: `void UHodgeAbilityTask_WaitMoveCancel::Evaluate(const TCHAR* Source)`
- L150: `void UHodgeAbilityTask_WaitMoveCancel::HandleMoveIntentChanged(bool                   )`
- L157: `void UHodgeAbilityTask_WaitMoveCancel::HandleWindowTagChanged(FGameplayTag        , int32 NewCount)`

## HodgeGameplayAbility.cpp

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。（PreloadPrimaryAssetsOnGrant 已随未提交改动回退，当前不存在。）

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Character/HodgeHeroCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/Abilities/HodgeAbilityCost.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)、[Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Interface/HodgeAbilitySourceInterface.h](../../../Source/Hodgepodge/Public/Interface/HodgeAbilitySourceInterface.h)

定义候选（多行签名仅展示首行）：

- L45: `UHodgeGameplayAbility::UHodgeGameplayAbility(const FObjectInitializer& ObjectInitializer)`
- L74: `UHodgeAbilitySystemComponent* UHodgeGameplayAbility::GetHodgeAbilitySystemComponentFromActorInfo() const`
- L83: `AHodgePlayerController* UHodgeGameplayAbility::GetHodgePlayerControllerFromActorInfo() const`
- L90: `AController* UHodgeGameplayAbility::GetControllerFromActorInfo() const`
- L128: `AHodgeCombatCharacter* UHodgeGameplayAbility::GetHodgeCharacterFromActorInfo() const`
- L135: `UHodgeHeroComponent* UHodgeGameplayAbility::GetHeroComponentFromActorInfo() const`
- L142: `void UHodgeGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const`
- L195: `bool UHodgeGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,`
- L256: `void UHodgeGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)`
- L275: `void UHodgeGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)`
- L288: `void UHodgeGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,`
- L299: `void UHodgeGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,`
- L309: `void UHodgeGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,`
- L328: `bool UHodgeGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,`
- L358: `void UHodgeGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,`
- L441: `FGameplayEffectContextHandle UHodgeGameplayAbility::MakeEffectContext(const FGameplayAbilitySpecHandle Handle,`
- L488: `void UHodgeGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,`
- L507: `bool UHodgeGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,`
- L663: `void UHodgeGameplayAbility::OnPawnAvatarSet()`
- L670: `void UHodgeGameplayAbility::GetAbilitySource(FGameplayAbilitySpecHandle Handle,`
- L696: `void UHodgeGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo,`
- L738: `bool UHodgeGameplayAbility::CanChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup) const`
- L776: `bool UHodgeGameplayAbility::ChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup)`
- L809: `void UHodgeGameplayAbility::SetCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode)`
- L823: `void UHodgeGameplayAbility::ClearCameraMode()`

## HodgeGameplayAbility_Dash.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Component/HodgeLocomotionPolicyComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Component/HodgeLockOnComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h)、[Component/HodgeDefenseComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeDefenseComponent.h)、[Data/HodgeSprintAbilityProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h)

定义候选（多行签名仅展示首行）：

- L22: `UHodgeGameplayAbility_Dash::UHodgeGameplayAbility_Dash()`
- L25: `bool UHodgeGameplayAbility_Dash::IsExecutionMovementModeValid(const UCharacterMovementComponent* Movement) const`
- L30: `bool UHodgeGameplayAbility_Dash::CheckCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayTagContainer* Tags) const`
- L37: `void UHodgeGameplayAbility_Dash::BeginMovement(const FHodgeMoveIntentSnapshot& Intent)`
- L61: `void UHodgeGameplayAbility_Dash::OnCommitRequested()`
- L71: `void UHodgeGameplayAbility_Dash::CommitDash()`
- L95: `void UHodgeGameplayAbility_Dash::UpdateMovement(float DeltaTime)`
- L155: `void UHodgeGameplayAbility_Dash::CompleteHandoff()`
- L166: `void UHodgeGameplayAbility_Dash::OnMontageInterrupted() { if (IsActive() && !bEnding) { Finish(); } }`
- L168: `void UHodgeGameplayAbility_Dash::PrepareLocomotionExit(float MaximumSpeed)`
- L182: `void UHodgeGameplayAbility_Dash::OnMontageBlendOut()`
- L192: `void UHodgeGameplayAbility_Dash::OnMontageCompleted() { if (IsActive() && !bEnding) { Finish(false); } }`
- L194: `void UHodgeGameplayAbility_Dash::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`

## HodgeGameplayAbility_Death.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Death.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Death.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_Death.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Death.h)、[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L17: `UHodgeGameplayAbility_Death::UHodgeGameplayAbility_Death(const FObjectInitializer& ObjectInitializer)`
- L38: `void UHodgeGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle,`
- L73: `void UHodgeGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle,`
- L88: `void UHodgeGameplayAbility_Death::StartDeath()`
- L101: `void UHodgeGameplayAbility_Death::FinishDeath()`

## HodgeGameplayAbility_Definition.cpp

执行单段 Definition、本次 Montage 时钟 Timeline 与手持窗口，按执行清理；Melee 子类扩展命中。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[Combat/HodgeAttackRule.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackRule.h)、[Component/HodgeLockOnComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Component/HodgeWingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeWingComponent.h)、[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)、[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Component/HodgeAttackMotionWarpingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeAttackMotionWarpingComponent.h)

定义候选（多行签名仅展示首行）：

- L30: `UHodgeGameplayAbility_Definition::UHodgeGameplayAbility_Definition(const FObjectInitializer& Initializer)`
- L41: `const UHodgeAbilityDefinition* UHodgeGameplayAbility_Definition::GetDefinition() const`
- L49: `bool UHodgeGameplayAbility_Definition::IsComboCoordinated() const`
- L57: `void UHodgeGameplayAbility_Definition::ActivateConfirmedDefinition(FGameplayAbilitySpecHandle Handle,`
- L74: `bool UHodgeGameplayAbility_Definition::CanActivateAbility(FGameplayAbilitySpecHandle Handle,`
- L96: `void UHodgeGameplayAbility_Definition::ActivateAbility(FGameplayAbilitySpecHandle Handle,`
- L144: `void UHodgeGameplayAbility_Definition::StartExecutionMontage()`
- L177: `bool UHodgeGameplayAbility_Definition::BuildAttackFacingData(const FGameplayAbilityActorInfo* Info, FHodgeAttackFacingTargetData& Out)`
- L206: `void UHodgeGameplayAbility_Definition::InitializeAttackFacing(const FGameplayEventData* Payload)`
- L231: `void UHodgeGameplayAbility_Definition::OnAttackFacingData(const FGameplayAbilityTargetDataHandle& Handle, FGameplayTag, FGuid ForExecution)`
- L244: `bool UHodgeGameplayAbility_Definition::ApplyAttackFacing(const FHodgeAttackFacingTargetData& Target)`
- L266: `void UHodgeGameplayAbility_Definition::OnAttackFacingTimeout(FGuid ForExecution)`
- L268: `void UHodgeGameplayAbility_Definition::UpdateAttackFacingLifecycle()`
- L280: `void UHodgeGameplayAbility_Definition::ReleaseAttackFacing()`
- L292: `void UHodgeGameplayAbility_Definition::FinishExecution(bool bCancelled, bool bReplicate)`
- L302: `bool UHodgeGameplayAbility_Definition::BeginPredictedMoveCancel()`
- L323: `void UHodgeGameplayAbility_Definition::ResolvePredictedMoveCancel(bool bEnd, float ServerPosition, float ServerTime, float ServerPlayRate)`
- L362: `void UHodgeGameplayAbility_Definition::EndAbility(FGameplayAbilitySpecHandle Handle,`
- L443: `FGameplayTagContainer UHodgeGameplayAbility_Definition::GetExecutionWindows() const`
- L450: `bool UHodgeGameplayAbility_Definition::GetCinematicSource(UAnimMontage*& OutMontage, int32& OutInstanceId, float& OutPosition) const`
- L464: `bool UHodgeGameplayAbility_Definition::AcceptsNotify(const FBranchingPointNotifyPayload& Payload) const`
- L473: `int32 UHodgeGameplayAbility_Definition::AllocateNotifyOccurrence()`
- L479: `int32 UHodgeGameplayAbility_Definition::BeginNotifyResource(const FBranchingPointNotifyPayload& Payload)`
- L489: `void UHodgeGameplayAbility_Definition::EndNotifyResource(const FBranchingPointNotifyPayload& Payload)`
- L509: `void UHodgeGameplayAbility_Definition::AcquireNotifyTag(int32 OccurrenceId, FGameplayTag Tag)`
- L526: `void UHodgeGameplayAbility_Definition::AcquireNotifyWeapon(int32 OccurrenceId, EHodgeWeaponHandEndBehavior EndBehavior)`
- L542: `void UHodgeGameplayAbility_Definition::NotifyWindowsChanged()`
- L549: `void UHodgeGameplayAbility_Definition::SendExecutionEvent(FGameplayTag Tag)`
- L567: `void UHodgeGameplayAbility_Definition::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted, FGuid Execution, int32 InstanceId)`
- L577: `void UHodgeGameplayAbility_Definition::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, FGuid Execution, int32 InstanceId)`
- L584: `void UHodgeGameplayAbility_Definition::ValidateExecutionConfiguration(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const`
- L593: `void UHodgeGameplayAbility_Definition::PublishAttackSettlement(const FHodgeAttackSettlement& Settlement)`
- L605: `void UHodgeGameplayAbility_Definition::DispatchAttackRuleEvent(FGameplayTag Tag)`

## HodgeGameplayAbility_EnemyDeath.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)

定义候选（多行签名仅展示首行）：

- L14: `void UHodgeGameplayAbility_EnemyDeath::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`
- L34: `void UHodgeGameplayAbility_EnemyDeath::CheckHeldDeathPose()`
- L46: `void UHodgeGameplayAbility_EnemyDeath::FinishMontage()`
- L53: `void UHodgeGameplayAbility_EnemyDeath::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`

## HodgeGameplayAbility_HavocRules.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)、[AbilitySystem/Effects/HodgeHavocEffects.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeHavocEffects.h)、[AbilitySystem/Effects/HodgeResonanceEnergyEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)

定义候选（多行签名仅展示首行）：

- L16: `UHodgeGameplayAbility_HavocRules::UHodgeGameplayAbility_HavocRules()`
- L26: `UHodgeGameplayAbility_HavocRules* UHodgeGameplayAbility_HavocRules::Find(UAbilitySystemComponent* ASC)`
- L39: `bool UHodgeGameplayAbility_HavocRules::ValidateEffects() const`
- L52: `bool UHodgeGameplayAbility_HavocRules::IsCurrentSource(const UHodgeGameplayAbility_Definition* Source) const`
- L62: `bool UHodgeGameplayAbility_HavocRules::CanEnterDarkSurge(const AActor* Avatar) const`
- L73: `bool UHodgeGameplayAbility_HavocRules::TryEnterDarkSurge(UHodgeGameplayAbility_Definition* Source)`
- L79: `bool UHodgeGameplayAbility_HavocRules::BeginSession(FGuid EntryExecutionId)`
- L102: `void UHodgeGameplayAbility_HavocRules::EndDarkSurge(FGuid Id)`
- L114: `float UHodgeGameplayAbility_HavocRules::ApplyDarkFlowGain(const UHodgeGameplayAbility_Definition* Source, float Amount)`
- L126: `void UHodgeGameplayAbility_HavocRules::HandleEnhancedNormalHit(const UHodgeGameplayAbility_Definition* Source, float AcceptedDamage)`
- L137: `float UHodgeGameplayAbility_HavocRules::SettleFuel(const FGameplayEffectSpec& Spec)`
- L148: `void UHodgeGameplayAbility_HavocRules::OnFuelChanged(const FOnAttributeChangeData& Data)`
- L153: `void UHodgeGameplayAbility_HavocRules::OnEffectRemoved(const FActiveGameplayEffect& Effect)`
- L162: `void UHodgeGameplayAbility_HavocRules::OnDeathTagChanged(FGameplayTag Tag,int32 Count)`
- L167: `void UHodgeGameplayAbility_HavocRules::ClearDarkFlow()`
- L181: `void UHodgeGameplayAbility_HavocRules::OnAvatarUninitialized(TWeakObjectPtr<AActor> ForAvatar)`
- L187: `void UHodgeGameplayAbility_HavocRules::BindAvatar()`
- L198: `void UHodgeGameplayAbility_HavocRules::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`
- L214: `void UHodgeGameplayAbility_HavocRules::OnPawnAvatarSet()`
- L225: `void UHodgeGameplayAbility_HavocRules::Unbind()`
- L238: `void UHodgeGameplayAbility_HavocRules::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`

## HodgeGameplayAbility_HitReaction.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h)、[Component/HodgeHitReactionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHitReactionComponent.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L18: `UHodgeGameplayAbility_HitReaction::UHodgeGameplayAbility_HitReaction(const FObjectInitializer& ObjectInitializer)`
- L30: `bool UHodgeGameplayAbility_HitReaction::CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`
- L39: `void UHodgeGameplayAbility_HitReaction::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`
- L68: `bool UHodgeGameplayAbility_HitReaction::Synchronize(const FHodgeHitReactionState& State)`
- L134: `void UHodgeGameplayAbility_HitReaction::ClearMotion()`
- L154: `void UHodgeGameplayAbility_HitReaction::FinishReaction(bool bCancelled)`
- L162: `void UHodgeGameplayAbility_HitReaction::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`

## HodgeGameplayAbility_Jump.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h)、[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)

定义候选（多行签名仅展示首行）：

- L17: `UHodgeGameplayAbility_Jump::UHodgeGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)`
- L25: `bool UHodgeGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,`
- L52: `void UHodgeGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle,`
- L64: `void UHodgeGameplayAbility_Jump::CharacterJumpStart()`
- L77: `void UHodgeGameplayAbility_Jump::CharacterJumpStop()`

## HodgeGameplayAbility_LockOn.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h)、[Component/HodgeLockOnComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h)、[Targeting/HodgeLockOnTypes.h](../../../Source/Hodgepodge/Public/Targeting/HodgeLockOnTypes.h)、[Data/HodgeLockOnProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeLockOnProfile.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

定义候选（多行签名仅展示首行）：

- L11: `UHodgeGameplayAbility_LockOn::UHodgeGameplayAbility_LockOn()`
- L21: `bool UHodgeGameplayAbility_LockOn::CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`
- L27: `void UHodgeGameplayAbility_LockOn::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`
- L53: `void UHodgeGameplayAbility_LockOn::OnTargetData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag)`
- L64: `void UHodgeGameplayAbility_LockOn::OnTimeout()`
- L66: `void UHodgeGameplayAbility_LockOn::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`

## HodgeGameplayAbility_Melee.cpp

接收服务器 Window/Point 命中结果，准备锚点/目标、逐段/共享去重，构建独立 GE Spec/Context 并施加。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[Component/HodgeDefenseComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeDefenseComponent.h)、[AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[AbilitySystem/Executions/HodgeDamageExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeDamageExecution.h)、[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Combat/HodgeDamageRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeDamageRules.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Component/HodgeHitReactionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHitReactionComponent.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)

定义候选（多行签名仅展示首行）：

- L28: `UHodgeGameplayAbility_Melee::UHodgeGameplayAbility_Melee()`
- L34: `bool FHodgeMeleeHitHistory::CanHit(UAbilitySystemComponent* Target, double Time, float RepeatInterval) const`
- L43: `void FHodgeMeleeHitHistory::RecordHit(UAbilitySystemComponent* Target, double Time)`
- L49: `void UHodgeGameplayAbility_Melee::ValidateExecutionConfiguration(`
- L95: `bool UHodgeGameplayAbility_Melee::PrepareHitExecutionContext_Implementation() { return true; }`
- L98: `bool UHodgeGameplayAbility_Melee::SetHitAnchor(FName Key, const FTransform& WorldTransform)`
- L113: `bool UHodgeGameplayAbility_Melee::SetHitTarget(FName Key, AActor* Target)`
- L124: `void UHodgeGameplayAbility_Melee::ResetHitGeometryHistory()`
- L135: `void UHodgeGameplayAbility_Melee::OnExecutionReady()`
- L164: `void UHodgeGameplayAbility_Melee::OnExecutionEnding(const FGuid& EndingExecutionId)`
- L182: `bool UHodgeGameplayAbility_Melee::BeginNotifyHit(int32 OccurrenceId, const FHodgeAnimHitConfig& Config, USkeletalMeshComponent* Mesh, bool bSingle)`
- L251: `bool UHodgeGameplayAbility_Melee::OpenHit(int32 OccurrenceId, const FHodgeHitEffectConfig& Binding, const FHodgeHitDetectionRequest& Request)`
- L275: `void UHodgeGameplayAbility_Melee::OnNotifyResourceEnded(int32 OccurrenceId)`
- L281: `bool UHodgeGameplayAbility_Melee::IsMeleeBatchCurrent(const FHodgeHitDetectionBatch& Batch) const`
- L289: `void UHodgeGameplayAbility_Melee::OnHitResults(const FHodgeHitDetectionBatch& Batch)`
- L300: `bool UHodgeGameplayAbility_Melee::CanApplyMeleeHit_Implementation(AActor* Target, const FHitResult& Hit,`
- L307: `FGameplayEffectContextHandle UHodgeGameplayAbility_Melee::MakeMeleeHitContext(`
- L329: `FGameplayEffectSpecHandle UHodgeGameplayAbility_Melee::BuildMeleeHitSpec(const FHodgeHitDetectionBatch& Batch,`
- L353: `void UHodgeGameplayAbility_Melee::ProcessMeleeHitResults_Implementation(const FHodgeHitDetectionBatch& Batch)`
- L388: `void UHodgeGameplayAbility_Melee::SubmitMeleeHitEffects(const FHodgeHitDetectionBatch& Batch,`
- L441: `void UHodgeGameplayAbility_Melee::ApplyMeleeHitEffects_Implementation(const FHodgeHitDetectionBatch& Batch,`

## HodgeGameplayAbility_MovementAction.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Component/HodgeLocomotionPolicyComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)、[Data/HodgeSprintAbilityProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h)

定义候选（多行签名仅展示首行）：

- L18: `bool FHodgeMovementActionTargetData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bSuccess)`
- L23: `UHodgeGameplayAbility_MovementAction::UHodgeGameplayAbility_MovementAction()`
- L31: `const UHodgeSprintAbilityProfile* UHodgeGameplayAbility_MovementAction::ResolveProfile(const FGameplayAbilityActorInfo* Info) const`
- L38: `bool UHodgeGameplayAbility_MovementAction::CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`
- L53: `void UHodgeGameplayAbility_MovementAction::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`
- L88: `void UHodgeGameplayAbility_MovementAction::OnTargetData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag Tag)`
- L113: `bool UHodgeGameplayAbility_MovementAction::IsMovementValid() const`
- L124: `bool UHodgeGameplayAbility_MovementAction::IsExecutionMovementModeValid(const UCharacterMovementComponent* Movement) const`
- L129: `void UHodgeGameplayAbility_MovementAction::TickMovement()`
- L138: `void UHodgeGameplayAbility_MovementAction::SetMovementTag(FGameplayTag Tag, bool bAdd)`
- L152: `void UHodgeGameplayAbility_MovementAction::StartCue(FGameplayTag Tag)`
- L160: `void UHodgeGameplayAbility_MovementAction::StopCue()`
- L166: `void UHodgeGameplayAbility_MovementAction::Finish(bool bCancelled)`
- L169: `void UHodgeGameplayAbility_MovementAction::OnConstraintChanged(FGameplayTag Tag, int32 Count) { if (Count > 0) { Finish(); } }`
- L171: `void UHodgeGameplayAbility_MovementAction::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`

## HodgeGameplayAbility_ResonanceCircuit.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_ResonanceCircuit.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_ResonanceCircuit.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_ResonanceCircuit.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_ResonanceCircuit.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h)、[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)

定义候选（多行签名仅展示首行）：

- L9: `bool UHodgeGameplayAbility_ResonanceCircuit::CanActivateAbility(FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* Info,`
- L24: `void UHodgeGameplayAbility_ResonanceCircuit::OnExecutionReady()`
- L29: `bool UHodgeGameplayAbility_ResonanceCircuit::CommitDarkSurge(const FBranchingPointNotifyPayload& Payload,const FHodgeAnimHitConfig& Burst)`
- L44: `void UHodgeGameplayAbility_ResonanceCircuit::ProcessMeleeHitResults_Implementation(const FHodgeHitDetectionBatch& Batch)`

## HodgeGameplayAbility_Sprint.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Component/HodgeLocomotionPolicyComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Data/HodgeSprintAbilityProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h)

定义候选（多行签名仅展示首行）：

- L26: `UHodgeGameplayAbility_Sprint::UHodgeGameplayAbility_Sprint()`
- L30: `bool FHodgeSprintPivotTargetData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bSuccess)`
- L33: `bool UHodgeGameplayAbility_Sprint::QualifiesPivot(FVector Velocity, FVector Desired, float MinimumSpeed, float Angle)`
- L42: `bool UHodgeGameplayAbility_Sprint::IsPivotRecoveryComplete(float Position, float EndTime)`
- L45: `void UHodgeGameplayAbility_Sprint::ProcessMovementIntent(FVector Desired)`
- L55: `void UHodgeGameplayAbility_Sprint::RequestPivot(FVector Velocity, FVector Desired)`
- L69: `void UHodgeGameplayAbility_Sprint::HandleMovementFollowUp(const FGameplayAbilityTargetDataHandle& Data)`
- L95: `void UHodgeGameplayAbility_Sprint::StartPivot(float StartingYaw)`
- L117: `void UHodgeGameplayAbility_Sprint::ReleasePivotTask()`
- L125: `void UHodgeGameplayAbility_Sprint::UpdatePivotRecovery()`
- L140: `void UHodgeGameplayAbility_Sprint::OnPivotFinished()`
- L146: `void UHodgeGameplayAbility_Sprint::BeginMovement(const FHodgeMoveIntentSnapshot& Intent)`
- L191: `void UHodgeGameplayAbility_Sprint::UpdateMovement(float DeltaTime)`
- L217: `void UHodgeGameplayAbility_Sprint::OnAuthorityEnded()`
- L223: `void UHodgeGameplayAbility_Sprint::OnAttackStarted(FGameplayTag Tag, int32 Count) { if (Count > 0) { Finish(); } }`
- L225: `void UHodgeGameplayAbility_Sprint::OnDamage(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* Spec, float Magnitude, float Old, float New)`
- L228: `void UHodgeGameplayAbility_Sprint::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`

## HodgeAttributeSet.cpp

项目 AttributeSet 基础和 ASC 访问。

源码：[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

定义候选（多行签名仅展示首行）：

- L26: `UHodgeAttributeSet::UHodgeAttributeSet()`
- L31: `UWorld* UHodgeAttributeSet::GetWorld() const`
- L44: `UHodgeAbilitySystemComponent* UHodgeAttributeSet::GetHodgeAbilitySystemComponent() const`

## HodgeCombatSet.cpp

BaseDamage/BaseHeal 战斗属性，OwnerOnly 复制，供伤害/治疗 Execution 捕获。

源码：[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeCombatSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeCombatSet.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)

定义候选（多行签名仅展示首行）：

- L15: `UHodgeCombatSet::UHodgeCombatSet()`
- L25: `void UHodgeCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L38: `void UHodgeCombatSet::OnRep_HavocDamageBonus(const FGameplayAttributeData& OldValue)`
- L44: `void UHodgeCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)`
- L51: `void UHodgeCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)`

## HodgeHavocSet.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHavocSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHavocSet.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)

定义候选（多行签名仅展示首行）：

- L16: `UHodgeHavocSet::UHodgeHavocSet() : DarkFlow(0.f), MaxDarkFlow(0.f) {}`
- L18: `bool UHodgeHavocSet::IsResourceReady() const`
- L23: `void UHodgeHavocSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L30: `void UHodgeHavocSet::Clamp(const FGameplayAttribute& Attribute, float& Value) const`
- L37: `void UHodgeHavocSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const`
- L42: `void UHodgeHavocSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)`
- L47: `void UHodgeHavocSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)`
- L53: `void UHodgeHavocSet::OnRep_DarkFlow(const FGameplayAttributeData& Old)`
- L58: `void UHodgeHavocSet::OnRep_MaxDarkFlow(const FGameplayAttributeData& Old)`

## HodgeHealthSet.cpp

Health/MaxHealth、Damage/Healing 元属性、结算夹取/免疫/耗尽广播；BaseDamage/BaseHeal 位于 CombatSet。

源码：[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)、[Component/HodgeHitReactionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHitReactionComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L52: `UHodgeHealthSet::UHodgeHealthSet()`
- L69: `void UHodgeHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L82: `void UHodgeHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)`
- L118: `void UHodgeHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)`
- L135: `bool UHodgeHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)`
- L199: `void UHodgeHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)`
- L334: `void UHodgeHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const`
- L344: `void UHodgeHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)`
- L354: `void UHodgeHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)`
- L388: `void UHodgeHealthSet::BeginAttributeRebuild()`
- L399: `float UHodgeHealthSet::EndAttributeRebuild(bool& bReachedZero)`
- L408: `float UHodgeHealthSet::GetResourceClampMax() const`
- L414: `bool UHodgeHealthSet::SetHealthForAttributeCommit(float NewHealth)`
- L430: `void UHodgeHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const`

## HodgeResonanceSet.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeResonanceSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeResonanceSet.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)

定义候选（多行签名仅展示首行）：

- L15: `UHodgeResonanceSet::UHodgeResonanceSet() : ResonanceEnergy(0.f), MaxResonanceEnergy(0.f) {}`
- L17: `bool UHodgeResonanceSet::IsResourceReady() const`
- L22: `void UHodgeResonanceSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L29: `void UHodgeResonanceSet::Clamp(const FGameplayAttribute& Attribute, float& Value) const`
- L36: `void UHodgeResonanceSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const`
- L41: `void UHodgeResonanceSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)`
- L46: `void UHodgeResonanceSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)`
- L52: `void UHodgeResonanceSet::OnRep_ResonanceEnergy(const FGameplayAttributeData& Old)`
- L57: `void UHodgeResonanceSet::OnRep_MaxResonanceEnergy(const FGameplayAttributeData& Old)`

## HodgeHavocEffects.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Effects/HodgeHavocEffects.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Effects/HodgeHavocEffects.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Effects/HodgeHavocEffects.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeHavocEffects.h)、[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h)

定义候选（多行签名仅展示首行）：

- L9: `UHodgeDarkFlowEffect::UHodgeDarkFlowEffect()`
- L18: `UHodgeDarkFlowInitialization::UHodgeDarkFlowInitialization()`
- L26: `UHodgeDarkSurgeEffect::UHodgeDarkSurgeEffect()`
- L37: `UHodgeDarkFlowDrainEffect::UHodgeDarkFlowDrainEffect()`
- L45: `void UHodgeDarkFlowDrainExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& Parameters,`

## HodgeResonanceEnergyEffect.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Effects/HodgeResonanceEnergyEffect.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Effects/HodgeResonanceEnergyEffect.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Effects/HodgeResonanceEnergyEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h)、[AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)

定义候选（多行签名仅展示首行）：

- L6: `UHodgeResonanceEnergyEffect::UHodgeResonanceEnergyEffect()`
- L15: `UHodgeResonanceEnergyInitialization::UHodgeResonanceEnergyInitialization()`

## HodgeDamageExecution.cpp

BaseDamage × SetByCaller 倍率 × 距离/材质衰减 × 共享目标规则；不再恒零。

源码：[Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeDamageExecution.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeDamageExecution.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Executions/HodgeDamageExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeDamageExecution.h)、[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Combat/HodgeDamageRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeDamageRules.h)、[Combat/HodgeHavocRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeHavocRules.h)、[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)

定义候选（多行签名仅展示首行）：

- L59: `UHodgeDamageExecution::UHodgeDamageExecution()`
- L67: `void UHodgeDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,`

## HodgeHealExecution.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeHealExecution.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeHealExecution.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Executions/HodgeHealExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeHealExecution.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)

定义候选（多行签名仅展示首行）：

- L48: `UHodgeHealExecution::UHodgeHealExecution()`
- L55: `void UHodgeHealExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,`

## HodgeGameplayCue_Movement.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/GameplayCues/HodgeGameplayCue_Movement.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/GameplayCues/HodgeGameplayCue_Movement.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/GameplayCues/HodgeGameplayCue_Movement.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayCues/HodgeGameplayCue_Movement.h)

定义候选（多行签名仅展示首行）：

- L11: `AHodgeGameplayCue_Movement::AHodgeGameplayCue_Movement()`
- L16: `bool AHodgeGameplayCue_Movement::OnActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)`
- L19: `bool AHodgeGameplayCue_Movement::WhileActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)`
- L31: `bool AHodgeGameplayCue_Movement::OnRemove_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)`
- L38: `bool AHodgeGameplayCue_Movement::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)`

## GameplayTagStack.cpp

带计数的标签栈及复制数据结构，区别于只判断有无的 TagContainer。

源码：[Source/Hodgepodge/Private/AbilitySystem/GameplayTagStack.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/GameplayTagStack.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/GameplayTagStack.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h)

定义候选（多行签名仅展示首行）：

- L16: `FString FGameplayTagStack::GetDebugString() const`
- L25: `void FGameplayTagStackContainer::AddStack(FGameplayTag Tag, int32 StackCount)`
- L72: `void FGameplayTagStackContainer::RemoveStack(FGameplayTag Tag, int32 StackCount)`
- L127: `void FGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)`
- L141: `void FGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)`
- L155: `void FGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)`

## HodgeAbilitySystemComponent.cpp

Tag 输入、激活组、关系映射、Montage 复制和全局注册；失败时恢复武器预测与连段记忆。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h)、[Component/HodgeLockOnComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h)、[AbilitySystem/HodgeAbilityTagRelationshipMapping.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/HodgeGlobalAbilitySystem.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h)、[Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)、[Data/HodgeGameData.h](../../../Source/Hodgepodge/Public/Data/HodgeGameData.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)

定义候选（多行签名仅展示首行）：

- L43: `UHodgeAbilitySystemComponent::UHodgeAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)`
- L60: `void UHodgeAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L75: `void UHodgeAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)`
- L154: `void UHodgeAbilitySystemComponent::TryActivateAbilitiesOnSpawn()`
- L172: `void UHodgeAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc,`
- L241: `void UHodgeAbilitySystemComponent::CancelInputActivatedAbilities(bool bReplicateCancelAbility)`
- L259: `void UHodgeAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)`
- L287: `void UHodgeAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)`
- L315: `void UHodgeAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)`
- L344: `void UHodgeAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)`
- L366: `void UHodgeAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)`
- L501: `void UHodgeAbilitySystemComponent::ClearAbilityInput()`
- L515: `void UHodgeAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle,`
- L539: `void UHodgeAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle,`
- L562: `void UHodgeAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,`
- L579: `void UHodgeAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,`
- L627: `void UHodgeAbilitySystemComponent::HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags,`
- L639: `void UHodgeAbilitySystemComponent::GetAdditionalActivationTagRequirements(`
- L653: `void UHodgeAbilitySystemComponent::SetTagRelationshipMapping(UHodgeAbilityTagRelationshipMapping* NewMapping)`
- L660: `bool UHodgeAbilitySystemComponent::HasActiveAbilityTag(FGameplayTag Tag) const`
- L670: `void UHodgeAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(`
- L678: `void UHodgeAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability,`
- L692: `bool UHodgeAbilitySystemComponent::IsActivationGroupBlocked(EHodgeAbilityActivationGroup Group) const`
- L724: `void UHodgeAbilitySystemComponent::AddAbilityToActivationGroup(EHodgeAbilityActivationGroup Group,`
- L773: `void UHodgeAbilitySystemComponent::RemoveAbilityFromActivationGroup(EHodgeAbilityActivationGroup Group,`
- L787: `void UHodgeAbilitySystemComponent::CancelActivationGroupAbilities(EHodgeAbilityActivationGroup Group,`
- L804: `void UHodgeAbilitySystemComponent::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)`
- L841: `void UHodgeAbilitySystemComponent::RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag)`
- L867: `void UHodgeAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle,`
- L883: `void UHodgeAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L891: `FGameplayAbilitySpecHandle UHodgeAbilitySystemComponent::GiveAbilityDefinition(`
- L925: `const UHodgeAbilityDefinition* UHodgeAbilitySystemComponent::FindAbilityDefinition(`
- L936: `FGameplayAbilitySpecHandle UHodgeAbilitySystemComponent::FindDefinitionAbility(FGameplayTag AbilityTag) const`
- L952: `void UHodgeAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)`
- L959: `void UHodgeAbilitySystemComponent::ProcessDeferredComboRequest(uint64 RequestId, TWeakObjectPtr<AActor> Avatar,`
- L995: `void UHodgeAbilitySystemComponent::InternalServerTryActivateAbility(FGameplayAbilitySpecHandle Handle,`
- L1050: `void UHodgeAbilitySystemComponent::ClientConfirmComboTiming_Implementation(AActor* Avatar,`
- L1074: `void UHodgeAbilitySystemComponent::ClientActivateAbilitySucceedWithEventData_Implementation(`
- L1098: `void UHodgeAbilitySystemComponent::ClientActivateAbilityFailed_Implementation(FGameplayAbilitySpecHandle Handle, int16 Key)`
- L1118: `float UHodgeAbilitySystemComponent::PlayMontage(UGameplayAbility* Ability, FGameplayAbilityActivationInfo Info,`
- L1136: `float UHodgeAbilitySystemComponent::PlayMontageSimulated(UAnimMontage* Montage, float Rate, FName Section)`
- L1144: `void UHodgeAbilitySystemComponent::ApplyDefinitionMontageSettings()`
- L1162: `void UHodgeAbilitySystemComponent::OnRep_DefinitionMontage()`
- L1172: `void UHodgeAbilitySystemComponent::OnRep_ReplicatedAnimMontage()`
- L1179: `void UHodgeAbilitySystemComponent::StopDefinitionMontage(UGameplayAbility* Ability, bool bNatural)`
- L1187: `void UHodgeAbilitySystemComponent::CurrentMontageStop(float OverrideBlendOutTime)`
- L1214: `void UHodgeAbilitySystemComponent::ReleaseAbilityInput()`
- L1230: `void UHodgeAbilitySystemComponent::ServerSetReplicatedTargetData_Implementation(FGameplayAbilitySpecHandle Handle,`
- L1256: `void UHodgeAbilitySystemComponent::RetireLockOnCommand(FGameplayAbilitySpecHandle Handle, FPredictionKey Key)`
- L1264: `void UHodgeAbilitySystemComponent::RetireAttackFacingData(FGameplayAbilitySpecHandle Handle, FPredictionKey Key)`
- L1272: `FGameplayAbilitySpecHandle UHodgeAbilitySystemComponent::ResolveAbilityVariant(FGameplayTag InputTag, FString* OutFailureReason) const`

## HodgeAbilitySystemGlobals.cpp

分配 FHodgeGameplayEffectContext 的 GAS 全局类；已加入项目配置。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemGlobals.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemGlobals.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemGlobals.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemGlobals.h)、[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)

定义候选（多行签名仅展示首行）：

- L18: `UHodgeAbilitySystemGlobals::UHodgeAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)`
- L25: `FGameplayEffectContext* UHodgeAbilitySystemGlobals::AllocGameplayEffectContext() const`

## HodgeAbilityTagRelationshipMapping.cpp

数据驱动的能力阻断、取消与激活条件关系。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilityTagRelationshipMapping.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilityTagRelationshipMapping.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilityTagRelationshipMapping.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L13: `void UHodgeAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(`
- L43: `void UHodgeAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(`
- L75: `bool UHodgeAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& TargetTags, const FGameplayTag& ActionTag) const`
- L79: `bool UHodgeAbilityTagRelationshipMapping::IsCancellationProtected(const FGameplayTagContainer& TargetTags)`
- L84: `void UHodgeAbilityTagRelationshipMapping::GetAbilityCancelExceptions(const FGameplayTagContainer& SourceTags, FGameplayTagContainer& OutExceptions) const`
- L91: `bool UHodgeAbilityTagRelationshipMapping::IsAbilityCancelledByTags(const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags) const`
- L99: `bool UHodgeAbilityTagRelationshipMapping::IsForcedCancellation(const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags) const`

## HodgeGameplayCueManager.cpp

项目 Cue 管理类已配置；启动预加载及 Feature Cue 观察者生命周期仍未完整接通。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayCueManager.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayCueManager.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeGameplayCueManager.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayCueManager.h)

定义候选（多行签名仅展示首行）：

- L71: `UHodgeGameplayCueManager::UHodgeGameplayCueManager(const FObjectInitializer& ObjectInitializer)`
- L77: `UHodgeGameplayCueManager* UHodgeGameplayCueManager::Get()`
- L83: `void UHodgeGameplayCueManager::OnCreated()`
- L93: `void UHodgeGameplayCueManager::LoadAlwaysLoadedCues()`
- L127: `bool UHodgeGameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries() const`
- L156: `bool UHodgeGameplayCueManager::ShouldSyncLoadMissingGameplayCues() const`
- L163: `bool UHodgeGameplayCueManager::ShouldAsyncLoadMissingGameplayCues() const`
- L170: `void UHodgeGameplayCueManager::DumpGameplayCues(const TArray<FString>& Args)`
- L265: `void UHodgeGameplayCueManager::OnGameplayTagLoaded(const FGameplayTag& Tag)`
- L312: `void UHodgeGameplayCueManager::HandlePostGarbageCollect()`
- L325: `void UHodgeGameplayCueManager::ProcessLoadedTags()`
- L376: `void UHodgeGameplayCueManager::ProcessTagToPreload(const FGameplayTag& Tag, UObject* OwningObject)`
- L441: `void UHodgeGameplayCueManager::OnPreloadCueComplete(FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject,`
- L457: `void UHodgeGameplayCueManager::RegisterPreloadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject)`
- L493: `void UHodgeGameplayCueManager::HandlePostLoadMap(UWorld* NewWorld)`
- L540: `void UHodgeGameplayCueManager::UpdateDelayLoadDelegateListeners()`
- L584: `bool UHodgeGameplayCueManager::ShouldDelayLoadGameplayCues() const`
- L603: `void UHodgeGameplayCueManager::RefreshGameplayCuePrimaryAsset()`

## HodgeGameplayEffectContext.cpp

项目 GE 上下文与序列化扩展，已有 HodgeAbilitySystemGlobals 分配配套。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayEffectContext.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayEffectContext.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)

定义候选（多行签名仅展示首行）：

- L21: `FHodgeGameplayEffectContext* FHodgeGameplayEffectContext::ExtractEffectContext(`
- L40: `bool FHodgeGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)`
- L64: `void FHodgeGameplayEffectContext::SetAbilitySource(const IHodgeAbilitySourceInterface* InObject, float InSourceLevel)`
- L74: `const IHodgeAbilitySourceInterface* FHodgeGameplayEffectContext::GetAbilitySource() const`
- L81: `const UPhysicalMaterial* FHodgeGameplayEffectContext::GetPhysicalMaterial() const`

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

- L13: `void FGlobalAppliedAbilityList::AddToASC(TSubclassOf<UGameplayAbility> Ability, UHodgeAbilitySystemComponent* ASC)`
- L35: `void FGlobalAppliedAbilityList::RemoveFromASC(UHodgeAbilitySystemComponent* ASC)`
- L49: `void FGlobalAppliedAbilityList::RemoveFromAll()`
- L68: `void FGlobalAppliedEffectList::AddToASC(TSubclassOf<UGameplayEffect> Effect, UHodgeAbilitySystemComponent* ASC)`
- L88: `void FGlobalAppliedEffectList::RemoveFromASC(UHodgeAbilitySystemComponent* ASC)`
- L102: `void FGlobalAppliedEffectList::RemoveFromAll()`
- L120: `UHodgeGlobalAbilitySystem::UHodgeGlobalAbilitySystem()`
- L125: `void UHodgeGlobalAbilitySystem::ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability)`
- L142: `void UHodgeGlobalAbilitySystem::ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect)`
- L159: `void UHodgeGlobalAbilitySystem::RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability)`
- L176: `void UHodgeGlobalAbilitySystem::RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect)`
- L193: `void UHodgeGlobalAbilitySystem::RegisterASC(UHodgeAbilitySystemComponent* ASC)`
- L214: `void UHodgeGlobalAbilitySystem::UnregisterASC(UHodgeAbilitySystemComponent* ASC)`

## HodgeLockOnStateEffect.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeLockOnStateEffect.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeLockOnStateEffect.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeLockOnStateEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeLockOnStateEffect.h)、[Targeting/HodgeLockOnTypes.h](../../../Source/Hodgepodge/Public/Targeting/HodgeLockOnTypes.h)

定义候选（多行签名仅展示首行）：

- L6: `UHodgeLockOnStateEffect::UHodgeLockOnStateEffect()`

## HodgeAttributeCoordinator.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeAttributeCoordinator.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeAttributeCoordinator.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Stats/HodgeAttributeCoordinator.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeCoordinator.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[Data/HodgeCharacterStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Data/HodgeEquipmentStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeEquipmentStatProfile.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)、[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)

定义候选（多行签名仅展示首行）：

- L21: `AHodgePlayerState* UHodgeAttributeCoordinator::GetPlayerState() const { return GetTypedOuter<AHodgePlayerState>(); }`
- L23: `bool UHodgeAttributeCoordinator::IsBoundTo(const APawn* Avatar) const { return Avatar && BoundAvatar.Get() == Avatar; }`
- L25: `bool UHodgeAttributeCoordinator::IsCurrentAvatar() const { return BoundAvatar.IsValid() && AbilitySystem.IsValid() && AbilitySystem->GetAvatarActor() == BoundAvatar.Get(); }`
- L27: `bool UHodgeAttributeCoordinator::IsReadyFor(const APawn* Avatar) const`
- L35: `bool UHodgeAttributeCoordinator::ValidateCoreSets() const`
- L49: `void UHodgeAttributeCoordinator::PublishReady(bool bReady)`
- L67: `bool UHodgeAttributeCoordinator::BeginUpdate(EHodgeAttributeUpdateReason Reason)`
- L84: `bool UHodgeAttributeCoordinator::ApplyCharacterBase(int32 Level)`
- L101: `bool UHodgeAttributeCoordinator::PrepareAvatar(APawn* Avatar, const UHodgePawnData* Data)`
- L136: `bool UHodgeAttributeCoordinator::CompleteAvatarInitialization()`
- L151: `bool UHodgeAttributeCoordinator::CommitUpdate(bool bSuccess)`
- L196: `void UHodgeAttributeCoordinator::DetachAvatar(APawn* Avatar)`
- L211: `bool UHodgeAttributeCoordinator::SetCharacterLevel(int32 Level)`
- L230: `bool UHodgeAttributeCoordinator::RestoreHealth(float Health)`
- L241: `bool UHodgeAttributeCoordinator::BeginEquipmentUpdate() { return IsReadyFor(BoundAvatar.Get()) && BeginUpdate(EHodgeAttributeUpdateReason::EquipmentChange); }`
- L243: `void UHodgeAttributeCoordinator::FinishEquipmentUpdate(bool bSuccess) { CommitUpdate(bSuccess); }`
- L245: `void UHodgeAttributeCoordinator::ExpectRespawn() { if (GetPlayerState() && GetPlayerState()->HasAuthority()) { bExpectRespawn = LifeGeneration > 0; } }`
- L248: `bool UHodgeAttributeCoordinator::SetInitialSavedHealth(float Health)`
- L258: `FHodgeOwnedEquipmentState UHodgeAttributeCoordinator::GetOrCreateDefaultEquipment(TSubclassOf<UHodgeEquipmentDefinition> Definition)`
- L276: `void UHodgeAttributeCoordinator::RememberEquipment(FGuid Id, TSubclassOf<UHodgeEquipmentDefinition> Definition, int32 Level)`

## HodgeCharacterBaseStatEffect.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeCharacterBaseStatEffect.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeCharacterBaseStatEffect.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Stats/HodgeCharacterBaseStatEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeCharacterBaseStatEffect.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)

定义候选（多行签名仅展示首行）：

- L11: `UHodgeCharacterBaseStatEffect::UHodgeCharacterBaseStatEffect()`

## HodgeEquipmentStatEffect.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeEquipmentStatEffect.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeEquipmentStatEffect.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Stats/HodgeEquipmentStatEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeEquipmentStatEffect.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)

定义候选（多行签名仅展示首行）：

- L12: `UHodgeEquipmentStatEffect::UHodgeEquipmentStatEffect()`

## HodgeAbilityCost.h

自定义额外能力消耗的扩展契约。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "GameplayAbilitySpec.h"
  10: #include "Abilities/GameplayAbility.h"
  11: #include "HodgeAbilityCost.generated.h"
  13: class UHodgeGameplayAbility;
  28: UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
  29: class HODGEPODGE_API UHodgeAbilityCost : public UObject
  30: {
  31: 	GENERATED_BODY()
  33: public:
  36: 	UHodgeAbilityCost()
  37: 	{
  38: 	}
  61: 	virtual bool CheckCost(const UHodgeGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
  62: 	                       const FGameplayAbilityActorInfo* ActorInfo,
  63: 	                       FGameplayTagContainer* OptionalRelevantTags) const
  64: 	{
  66: 		return true;
  67: 	}
  88: 	virtual void ApplyCost(const UHodgeGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
  89: 	                       const FGameplayAbilityActorInfo* ActorInfo,
  90: 	                       const FGameplayAbilityActivationInfo ActivationInfo)
  91: 	{
  93: 	}
  99: 	bool ShouldOnlyApplyCostOnHit() const { return bOnlyApplyCostOnHit; }
 101: protected:
 103: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
 104: 	bool bOnlyApplyCostOnHit = false;
 105: };
```

## HodgeAbilityCost_ResonanceEnergy.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeAbilityCost.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "AbilitySystem/Abilities/HodgeAbilityCost.h"
   4: #include "HodgeAbilityCost_ResonanceEnergy.generated.h"
   5: class UGameplayEffect;
   8: UCLASS(EditInlineNew, DefaultToInstanced)
   9: class HODGEPODGE_API UHodgeAbilityCost_ResonanceEnergy : public UHodgeAbilityCost
  10: {
  11: 	GENERATED_BODY()
  12: public:
  14: 	UHodgeAbilityCost_ResonanceEnergy();
  15: 	UPROPERTY(EditAnywhere, Category="Costs") TSubclassOf<UGameplayEffect> CostGameplayEffectClass;
  17: 	virtual bool CheckCost(const UHodgeGameplayAbility* Ability, FGameplayAbilitySpecHandle Handle,
  18: 		const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* FailureTags) const override;
  20: 	virtual void ApplyCost(const UHodgeGameplayAbility* Ability, FGameplayAbilitySpecHandle Handle,
  21: 		const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo) override;
  23: 	bool ValidateEffect() const;
  24: };
```

## HodgeAbilityTask_WaitHitResults.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "Abilities/Tasks/AbilityTask.h"
   7: #include "Combat/HodgeHitDetection.h"
   8: #include "Engine/EngineBaseTypes.h"
   9: #include "HodgeAbilityTask_WaitHitResults.generated.h"
  11: class UHodgeGameplayAbility_Definition;
  12: class UHodgeCombatComponentBase;
  13: class UHodgeHitDetectionComponent;
  14: class UHodgeAbilityTask_WaitHitResults;
  17: struct FHodgeHitResultsTickFunction final : FTickFunction
  18: {
  20: 	TWeakObjectPtr<UHodgeAbilityTask_WaitHitResults> Target;
  27: 	virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread,
  28: 		const FGraphEventRef& CompletionEvent) override;
  32: 	virtual FString DiagnosticMessage() override;
  33: };
  36: UCLASS()
  37: class HODGEPODGE_API UHodgeAbilityTask_WaitHitResults : public UAbilityTask
  38: {
  39: 	GENERATED_BODY()
  40: public:
  47: 	static UHodgeAbilityTask_WaitHitResults* WaitHitResults(UHodgeGameplayAbility_Definition* OwningAbility,
  48: 		UHodgeCombatComponentBase* Combat, const FGuid& ExecutionId);
  50: 	DECLARE_MULTICAST_DELEGATE_OneParam(FHitResults, const FHodgeHitDetectionBatch&);
  52: 	FHitResults OnHitResults;
  60: 	uint64 CreateWindow(int32 OccurrenceId, const FHodgeHitDetectionRequest& Request);
  64: 	void SampleWindow(int32 OccurrenceId);
  69: 	void CloseWindow(int32 OccurrenceId, bool bSampleFinal);
  74: 	bool IsRunningForExecution(const FGuid& ExecutionId) const;
  79: 	bool IsBatchCurrent(const FHodgeHitDetectionBatch& Batch) const;
  81: 	virtual void Pause() override;
  83: 	virtual void Resume() override;
  85: 	virtual void BeginDestroy() override;
  87: protected:
  89: 	virtual void Activate() override;
  93: 	virtual void OnDestroy(bool bInOwnerFinished) override;
  95: private:
  97: 	UPROPERTY() TWeakObjectPtr<UHodgeHitDetectionComponent> DetectionComponent;
  98: 	friend struct FHodgeHitResultsTickFunction;
  99: 	friend struct FHodgeMeleeTestAccess;
 101: 	struct FWindow
 102: 	{
 104: 		uint64 Handle = 0;
 106: 		bool bClosing = false;
 108: 		bool bSampled = false;
 110: 		bool bOnce = false;
 111: 	};
 113: 	void TickDetection();
 115: 	bool PrepareSourcePose(uint64 Handle);
 117: 	void ReleaseSessions();
 119: 	void UpdateTickState();
 122: 	FGuid ExecutionId;
 124: 	TMap<int32, FWindow> Windows;
 126: 	FHodgeHitResultsTickFunction DetectionTick;
 128: 	UPROPERTY() TWeakObjectPtr<UHodgeGameplayAbility_Definition> ExecutionAbility;
 130: 	UPROPERTY() TWeakObjectPtr<UHodgeCombatComponentBase> CombatComponent;
 132: 	UPROPERTY() TWeakObjectPtr<AActor> Avatar;
 134: 	bool bStopped = false;
 136: 	bool bSampling = false;
 138: 	bool bTickingDetection = false;
 139: };
```

## HodgeAbilityTask_WaitMoveCancel.h

合流取消窗口与本地移动意图，广播 OnMoveCancel 后结束；已有取消验证，当前 Combat 主链与旧 BasicAttack 消费方需区分。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  30: #pragma once
  32: #include "CoreMinimal.h"
  33: #include "Abilities/Tasks/AbilityTask.h"
  34: #include "GameplayTagContainer.h"
  36: #include "HodgeAbilityTask_WaitMoveCancel.generated.h"
  38: class UAbilitySystemComponent;
  39: class UHodgeHeroComponent;
  42: DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHodgeMoveCancelDelegate);
  45: UCLASS()
  46: class HODGEPODGE_API UHodgeAbilityTask_WaitMoveCancel : public UAbilityTask
  47: {
  48: 	GENERATED_BODY()
  50: public:
  54: 	UHodgeAbilityTask_WaitMoveCancel(const FObjectInitializer& ObjectInitializer);
  56: 	UPROPERTY(BlueprintAssignable)
  57: 	FHodgeMoveCancelDelegate OnMoveCancel;
  65: 	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
  66: 		meta = (DisplayName = "Wait Move Cancel", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility",
  67: 			BlueprintInternalUseOnly = "TRUE"))
  74: 	static UHodgeAbilityTask_WaitMoveCancel* WaitMoveCancel(UGameplayAbility* OwningAbility,
  75: 	                                                        FGameplayTag CancelWindowTag,
  76: 	                                                        float MoveIntentThreshold = 0.1f);
  78: protected:
  80: 	virtual void Activate() override;
  84: 	virtual void TickTask(float DeltaTime) override;
  88: 	virtual void OnDestroy(bool bInOwnerFinished) override;
  90: private:
  95: 	void Evaluate(const TCHAR* Source);
 101: 	UFUNCTION()
 102: 	void HandleMoveIntentChanged(bool bHasMoveIntent);
 109: 	void HandleWindowTagChanged(FGameplayTag Tag, int32 NewCount);
 112: 	FGameplayTag WindowTag;
 115: 	float IntentThreshold = 0.1f;
 118: 	bool bSucceeded = false;
 121: 	bool bInOnDestroy = false;
 124: 	TWeakObjectPtr<UAbilitySystemComponent> SubscribedASC;
 126: 	TWeakObjectPtr<UHodgeHeroComponent> SubscribedHeroComponent;
 129: 	FDelegateHandle WindowTagDelegateHandle;
 130: };
```

## HodgeGameplayAbility.h

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。（PreloadPrimaryAssetsOnGrant 已随未提交改动回退，当前不存在。）

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)

项目内直接 include（不是运行调用关系）：[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[Camera/HodgeCinematicCameraTypes.h](../../../Source/Hodgepodge/Public/Camera/HodgeCinematicCameraTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "Abilities/GameplayAbility.h"
  10: #include "Core/PlayerController/HodgePlayerController.h"
  11: #include "Camera/HodgeCinematicCameraTypes.h"
  12: #include "HodgeGameplayAbility.generated.h"
  15: class UHodgeHeroComponent;
  16: class UHodgeAbilityCost;
  17: class UHodgeCameraMode;
  18: class IHodgeAbilitySourceInterface;
  19: class AHodgeCombatCharacter;
  26: UENUM(BlueprintType)
  27: enum class EHodgeAbilityActivationPolicy : uint8
  28: {
  30: 	OnInputTriggered,
  33: 	WhileInputActive,
  36: 	OnSpawn
  37: };
  46: UENUM(BlueprintType)
  47: enum class EHodgeAbilityActivationGroup : uint8
  48: {
  50: 	Independent,
  53: 	Exclusive_Replaceable,
  56: 	Exclusive_Blocking,
  59: 	MAX UMETA(Hidden)
  60: };
  64: USTRUCT(BlueprintType)
  65: struct FHodgeAbilityMontageFailureMessage
  66: {
  67: 	GENERATED_BODY()
  69: public:
  71: 	UPROPERTY(BlueprintReadWrite)
  72: 	TObjectPtr<APlayerController> PlayerController = nullptr;
  75: 	UPROPERTY(BlueprintReadWrite)
  76: 	TObjectPtr<AActor> AvatarActor = nullptr;
  79: 	UPROPERTY(BlueprintReadWrite)
  80: 	FGameplayTagContainer FailureTags;
  83: 	UPROPERTY(BlueprintReadWrite)
  84: 	TObjectPtr<UAnimMontage> FailureMontage = nullptr;
  85: };
  99: UCLASS(Abstract, HideCategories = Input,
 100: 	Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
 102: class HODGEPODGE_API UHodgeGameplayAbility : public UGameplayAbility
 103: {
 104: 	GENERATED_BODY()
 107: 	friend class UHodgeAbilitySystemComponent;
 109: public:
 114: 	UHodgeGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
 116: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Attributes") bool bRequiresInitializedAttributes = false;
 118: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat") bool bAllowWhileHitReacting = false;
 124: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 125: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponentFromActorInfo() const;
 131: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 132: 	AHodgePlayerController* GetHodgePlayerControllerFromActorInfo() const;
 138: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 139: 	AController* GetControllerFromActorInfo() const;
 145: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 146: 	AHodgeCombatCharacter* GetHodgeCharacterFromActorInfo() const;
 152: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 153: 	UHodgeHeroComponent* GetHeroComponentFromActorInfo() const;
 159: 	EHodgeAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
 165: 	EHodgeAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }
 172: 	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
 176: 	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Ability",
 177: 		Meta = (ExpandBoolAsExecs = "ReturnValue"))
 182: 	bool CanChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup) const;
 186: 	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Ability",
 187: 		Meta = (ExpandBoolAsExecs = "ReturnValue"))
 192: 	bool ChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup);
 198: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 199: 	void SetCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode);
 203: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 204: 	void ClearCameraMode();
 210: 	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
 211: 	{
 213: 		NativeOnAbilityFailedToActivate(FailedReason);
 216: 		ScriptOnAbilityFailedToActivate(FailedReason);
 217: 	}
 219: protected:
 224: 	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;
 230: 	UFUNCTION(BlueprintImplementableEvent)
 231: 	void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;
 243: 	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 244: 	                                const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
 245: 	                                FGameplayTagContainer* OptionalRelevantTags) const override;
 251: 	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
 258: 	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
 265: 	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
 274: 	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 275: 	                             const FGameplayAbilityActivationInfo ActivationInfo,
 276: 	                             const FGameplayEventData* TriggerEventData) override;
 286: 	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 287: 	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
 288: 	                        bool bWasCancelled) override;
 297: 	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 298: 	                       OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
 306: 	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 307: 	                       const FGameplayAbilityActivationInfo ActivationInfo) const override;
 315: 	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle,
 316: 	                                                       const FGameplayAbilityActorInfo* ActorInfo) const override;
 323: 	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,
 324: 	                                                  FGameplayAbilitySpec* AbilitySpec) const override;
 334: 	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
 335: 	                                               const FGameplayTagContainer* SourceTags = nullptr,
 336: 	                                               const FGameplayTagContainer* TargetTags = nullptr,
 337: 	                                               OUT FGameplayTagContainer* OptionalRelevantTags = nullptr)
 338: 	const override;
 343: 	virtual void OnPawnAvatarSet();
 353: 	virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 354: 	                              float& OutSourceLevel, const IHodgeAbilitySourceInterface*& OutAbilitySource,
 355: 	                              AActor*& OutEffectCauser) const;
 360: 	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
 361: 	void K2_OnAbilityAdded();
 366: 	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
 367: 	void K2_OnAbilityRemoved();
 372: 	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
 373: 	void K2_OnPawnAvatarSet();
 375: protected:
 377: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Ability Activation")
 378: 	EHodgeAbilityActivationPolicy ActivationPolicy;
 381: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Ability Activation")
 382: 	EHodgeAbilityActivationGroup ActivationGroup;
 385: 	UPROPERTY(EditDefaultsOnly, Instanced, Category = Costs)
 386: 	TArray<TObjectPtr<UHodgeAbilityCost>> AdditionalCosts;
 389: 	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
 390: 	TMap<FGameplayTag, FText> FailureTagToUserFacingMessages;
 393: 	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
 394: 	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> FailureTagToAnimMontage;
 397: 	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
 398: 	bool bLogCancelation;
 401: 	FHodgeAbilityCameraHandle ActiveCameraHandle;
 403: 	TSubclassOf<UHodgeCameraMode> ActiveCameraMode;
 404: };
```

## HodgeGameplayAbility_Dash.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h"
   7: #include "HodgeGameplayAbility_Dash.generated.h"
   8: class UAbilityTask_ApplyRootMotionConstantForce;
   9: class UAbilityTask_PlayMontageAndWait;
  12: UCLASS(Blueprintable)
  13: class HODGEPODGE_API UHodgeGameplayAbility_Dash : public UHodgeGameplayAbility_MovementAction
  14: {
  15: 	GENERATED_BODY()
  16: public:
  18: 	UHodgeGameplayAbility_Dash();
  22: 	UFUNCTION(BlueprintPure) FHodgeDashDirections GetDashDirections() const { return Directions; }
  24: 	void CompleteHandoff();
  31: 	virtual bool CheckCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayTagContainer* Tags = nullptr) const override;
  32: protected:
  34: 	virtual bool IsExecutionMovementModeValid(const UCharacterMovementComponent* Movement) const override;
  38: 	virtual void BeginMovement(const FHodgeMoveIntentSnapshot& Intent) override;
  42: 	virtual void UpdateMovement(float DeltaTime) override;
  50: 	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
  51: private:
  53: 	FHodgeFacingRequestHandle DashBaseFacing;
  55: 	void CommitDash();
  57: 	void OnCommitRequested();
  61: 	void PrepareLocomotionExit(float MaximumSpeed);
  63: 	UFUNCTION() void OnMontageBlendOut();
  65: 	FDelegateHandle CommitRequestedHandle;
  67: 	UFUNCTION() void OnMontageInterrupted();
  69: 	UFUNCTION() void OnMontageCompleted();
  71: 	FHodgeDashDirections Directions;
  73: 	FGuid DefenseHandle;
  75: 	FName MotionInstanceName;
  77: 	bool bHandoffOpened = false;
  79: 	bool bHandedOff = false;
  81: 	bool bHandoffRequested = false;
  83: 	bool bEnding = false;
  85: 	double AnimationEndsAt = 0.;
  87: 	UPROPERTY(Transient) TObjectPtr<UAbilityTask_ApplyRootMotionConstantForce> MotionTask;
  89: 	UPROPERTY(Transient) TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
  90: };
```

## HodgeGameplayAbility_Death.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Death.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Death.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "HodgeGameplayAbility.h"
  10: #include "HodgeGameplayAbility_Death.generated.h"
  12: class UObject;
  13: struct FFrame;
  14: struct FGameplayAbilityActorInfo;
  15: struct FGameplayEventData;
  25: UCLASS(Abstract)
  26: class UHodgeGameplayAbility_Death : public UHodgeGameplayAbility
  27: {
  28: 	GENERATED_BODY()
  30: public:
  34: 	UHodgeGameplayAbility_Death(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  36: protected:
  43: 	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
  44: 	                             const FGameplayAbilityActivationInfo ActivationInfo,
  45: 	                             const FGameplayEventData* TriggerEventData) override;
  53: 	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
  54: 	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
  55: 	                        bool bWasCancelled) override;
  59: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
  60: 	void StartDeath();
  64: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
  65: 	void FinishDeath();
  67: protected:
  70: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Death")
  71: 	bool bAutoStartDeath;
  72: };
```

## HodgeGameplayAbility_Definition.h

执行单段 Definition、本次 Montage 时钟 Timeline 与手持窗口，按执行清理；Melee 子类扩展命中。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[Equipment/HodgeWeaponPresentationTypes.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationTypes.h)、[Combat/HodgeAttackFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackFacingTypes.h)、[Combat/HodgeCharacterFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeCharacterFacingTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   5: #include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
   6: #include "Animation/AnimNotifies/AnimNotify.h"
   7: #include "Equipment/HodgeWeaponPresentationTypes.h"
   8: #include "Combat/HodgeAttackFacingTypes.h"
   9: #include "Combat/HodgeCharacterFacingTypes.h"
  10: #include "TimerManager.h"
  11: #include "HodgeGameplayAbility_Definition.generated.h"
  13: class UHodgeAbilityDefinition;
  14: class UHodgeWeaponInstance;
  15: class UHodgeCombatComponentBase;
  16: class UHodgeAttackRule;
  17: struct FHodgeAttackSettlement;
  18: class UAnimMontage;
  22: UCLASS(Blueprintable)
  23: class HODGEPODGE_API UHodgeGameplayAbility_Definition : public UHodgeGameplayAbility
  24: {
  25: 	GENERATED_BODY()
  27: public:
  29: 	static bool BuildAttackFacingData(const FGameplayAbilityActorInfo* Info, FHodgeAttackFacingTargetData& OutData);
  31: 	void UpdateAttackFacingLifecycle();
  32: 	bool IsFacingExecutionSuspended() const { return bPredictedMoveCancel; }
  33: 	bool IsFacingAuthorityRetired() const { return bAttackFacingRetired; }
  34: 	bool IsAwaitingFacingData() const { return bFacingDataWaiting && !bEnding; }
  35: 	UFUNCTION(BlueprintPure, Category="Hodge|Facing") float GetAttackFacingYaw() const { return FacingTargetYaw; }
  37: 	UFUNCTION(BlueprintPure, Category="Hodge|Targeting") AActor* GetExecutionLockTarget() const { return ExecutionLockTarget.Get(); }
  41: 	UHodgeGameplayAbility_Definition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  45: 	UFUNCTION(BlueprintPure, Category="Hodge|Ability")
  46: 	const UHodgeAbilityDefinition* GetDefinition() const;
  50: 	FGameplayTagContainer GetExecutionWindows() const;
  55: 	void FinishExecution(bool bCancelled, bool bReplicate);
  59: 	bool BeginPredictedMoveCancel();
  66: 	void ResolvePredictedMoveCancel(bool bEnd, float ServerPosition, float ServerTime, float ServerPlayRate);
  71: 	bool AcceptsNotify(const FBranchingPointNotifyPayload& Payload) const;
  75: 	int32 AllocateNotifyOccurrence();
  80: 	int32 BeginNotifyResource(const FBranchingPointNotifyPayload& Payload);
  84: 	void EndNotifyResource(const FBranchingPointNotifyPayload& Payload);
  89: 	void AcquireNotifyTag(int32 OccurrenceId, FGameplayTag Tag);
  93: 	void AcquireNotifyWeapon(int32 OccurrenceId, EHodgeWeaponHandEndBehavior EndBehavior = EHodgeWeaponHandEndBehavior::ReturnToBack);
  97: 	void SendExecutionEvent(FGameplayTag Tag);
 101: 	FGuid GetExecutionId() const { return ExecutionId; }
 103: 	bool GetCinematicSource(UAnimMontage*& OutMontage, int32& OutInstanceId, float& OutPosition) const;
 107: 	bool IsComboCoordinated() const;
 111: 	bool IsExecutionEnding() const { return bEnding; }
 116: 	virtual void ValidateExecutionConfiguration(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const;
 123: 	void ActivateConfirmedDefinition(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
 124: 	                                 const FPredictionKey& Key, const FGameplayEventData& Payload);
 133: 	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 134: 	                                const FGameplayTagContainer* SourceTags = nullptr,
 135: 	                                const FGameplayTagContainer* TargetTags = nullptr,
 136: 	                                FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
 138: protected:
 141: 	virtual void OnExecutionReady() {}
 143: 	virtual void OnExecutionEvent(FGameplayTag Tag) {}
 147: 	virtual void OnExecutionEnding(const FGuid& EndingExecutionId) {}
 151: 	virtual void OnNotifyResourceEnded(int32 OccurrenceId) {}
 158: 	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 159: 	                             FGameplayAbilityActivationInfo ActivationInfo,
 160: 	                             const FGameplayEventData* TriggerEventData) override;
 168: 	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 169: 	                        FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
 170: 	                        bool bWasCancelled) override;
 172: protected:
 174:  void PublishAttackSettlement(const FHodgeAttackSettlement& Settlement);
 175:  bool HasAttackSettlementRules() const { return !AttackRules.IsEmpty(); }
 176: private:
 177: 	friend struct FHodgeAttackRuleTestAccess;
 179:  UPROPERTY(Transient) TArray<TObjectPtr<UHodgeAttackRule>> AttackRules;
 180:  void DispatchAttackRuleEvent(FGameplayTag Tag);
 181: 	friend struct FHodgeAttackWarpTestAccess;
 182: 	void StartExecutionMontage();
 183: 	void InitializeAttackFacing(const FGameplayEventData* Payload);
 184: 	void OnAttackFacingData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ApplicationTag, FGuid ForExecution);
 185: 	bool ApplyAttackFacing(const FHodgeAttackFacingTargetData& Data);
 186: 	void OnAttackFacingTimeout(FGuid ForExecution);
 187: 	void ReleaseAttackFacing();
 188: 	FDelegateHandle FacingDataDelegate;
 189: 	FTimerHandle FacingDataTimer;
 190: 	FHodgeFacingRequestHandle AttackFacingHandle;
 191: 	float FacingTargetYaw = 0.f;
 192: 	bool bFacingDataWaiting = false;
 193: 	bool bFacingDataProcessed = false;
 194: 	bool bActivationReturned = false;
 195: 	bool bAttackFacingRetired = false;
 196: 	friend struct FHodgeCombatActionTestAccess;
 199: 	UPROPERTY(Transient) TWeakObjectPtr<AActor> ExecutionLockTarget;
 200: 	FGuid ExecutionId;
 208: 	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted, FGuid Execution, int32 InstanceId);
 215: 	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, FGuid Execution, int32 InstanceId);
 217: 	void NotifyWindowsChanged();
 219: 	struct FNotifyResource
 220: 	{
 222: 		int32 OccurrenceId = INDEX_NONE;
 224: 		FGameplayTag Tag;
 226: 		FGuid WeaponHandle;
 227: 	};
 229: 	TMap<const UObject*, FNotifyResource> NotifyResources;
 231: 	TMap<FGameplayTag, int32> StateCounts;
 233: 	int32 NextOccurrence = 0;
 235: 	int32 MontageInstanceId = INDEX_NONE;
 237: 	FGuid PoseLease;
 239: 	FGameplayTag ExecutionBodyTag;
 241: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeCombatComponentBase> ExecutionCombat;
 243: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeWeaponInstance> PresentationWeapon;
 245: 	bool bEnding = false;
 247: 	bool bLifecycleEventSent = false;
 249: 	bool bPredictedMoveCancel = false;
 250: };
```

## HodgeGameplayAbility_EnemyDeath.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_Death.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Death.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   5: #include "AbilitySystem/Abilities/HodgeGameplayAbility_Death.h"
   6: #include "HodgeGameplayAbility_EnemyDeath.generated.h"
   8: class UAbilityTask_PlayMontageAndWait;
   9: class UHodgeCombatComponentBase;
  10: class UAnimMontage;
  13: UCLASS(Blueprintable)
  14: class HODGEPODGE_API UHodgeGameplayAbility_EnemyDeath : public UHodgeGameplayAbility_Death
  15: {
  16: 	GENERATED_BODY()
  17: public:
  19: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Death") TObjectPtr<UAnimMontage> DeathMontage;
  20: protected:
  27: 	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
  28: 		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
  36: 	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
  37: 		FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
  38: private:
  40: 	UFUNCTION() void FinishMontage();
  42: 	void CheckHeldDeathPose();
  44: 	FTimerHandle HeldPoseTimer;
  46: 	UPROPERTY() TObjectPtr<UAbilityTask_PlayMontageAndWait> Task;
  48: 	TWeakObjectPtr<UHodgeCombatComponentBase> Combat;
  50: 	FGuid PoseLease;
  51: };
```

## HodgeGameplayAbility_HavocRules.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[Combat/HodgeHavocRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeHavocRules.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
   4: #include "Combat/HodgeHavocRules.h"
   5: #include "HodgeGameplayAbility_HavocRules.generated.h"
   7: class UHodgeGameplayAbility_Definition;
   8: class UHodgePawnExtensionComponent;
  11: struct FHodgeDarkSurgeSession
  12: {
  13: 	FGuid Id;
  14: 	FGuid EntryExecutionId;
  15: 	TWeakObjectPtr<AActor> Avatar;
  16: 	FActiveGameplayEffectHandle State;
  17: 	FActiveGameplayEffectHandle Drain;
  18: 	FHodgeHavocFuelClock Clock;
  19: };
  22: UCLASS(Blueprintable)
  23: class HODGEPODGE_API UHodgeGameplayAbility_HavocRules : public UHodgeGameplayAbility
  24: {
  25: 	GENERATED_BODY()
  26: public:
  27: 	UHodgeGameplayAbility_HavocRules();
  29: 	static UHodgeGameplayAbility_HavocRules* Find(UAbilitySystemComponent* ASC);
  31: 	bool CanEnterDarkSurge(const AActor* Avatar) const;
  33: 	bool TryEnterDarkSurge(UHodgeGameplayAbility_Definition* Source);
  35: 	void EndDarkSurge(FGuid SessionId);
  37: 	float ApplyDarkFlowGain(const UHodgeGameplayAbility_Definition* Source, float Amount);
  39: 	void HandleEnhancedNormalHit(const UHodgeGameplayAbility_Definition* Source, float AcceptedDamage);
  41: 	float SettleFuel(const FGameplayEffectSpec& Spec);
  43: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Havoc", meta=(ClampMin="0.01")) float DrainPerSecond=10.f;
  45: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Havoc", meta=(ClampMin="0.01")) float HoldThreshold=.35f;
  47: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Havoc", meta=(ClampMin="0")) float HoldBufferSeconds=.3f;
  49: 	UPROPERTY(EditDefaultsOnly, Category="Hodge|Havoc") TSubclassOf<UGameplayEffect> StateEffect;
  50: 	UPROPERTY(EditDefaultsOnly, Category="Hodge|Havoc") TSubclassOf<UGameplayEffect> DrainEffect;
  51: 	UPROPERTY(EditDefaultsOnly, Category="Hodge|Havoc") TSubclassOf<UGameplayEffect> GainEffect;
  53: 	FGuid GetModeSessionId() const { return Session.Id; }
  54: protected:
  56: 	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
  57: 		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
  59: 	virtual void OnPawnAvatarSet() override;
  61: 	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
  62: 		FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
  63: private:
  64: 	friend struct FHodgeHavocTestAccess;
  66: 	bool BeginSession(FGuid EntryExecutionId);
  68: 	FHodgeDarkSurgeSession Session;
  69: 	FHodgeHavocProcGate ProcGate;
  70: 	TWeakObjectPtr<AActor> BoundAvatar;
  71: 	TWeakObjectPtr<UAbilitySystemComponent> BoundASC;
  72: 	FDelegateHandle FuelChangedHandle;
  73: 	FDelegateHandle RemovedHandle;
  74: 	FDelegateHandle DeathHandle;
  75: 	TWeakObjectPtr<UHodgePawnExtensionComponent> BoundExtension;
  76: 	bool bEndingSession=false;
  78: 	bool IsCurrentSource(const UHodgeGameplayAbility_Definition* Source) const;
  80: 	bool ValidateEffects() const;
  82: 	void OnFuelChanged(const FOnAttributeChangeData& Data);
  84: 	void OnEffectRemoved(const FActiveGameplayEffect& Effect);
  86: 	void OnDeathTagChanged(FGameplayTag Tag,int32 Count);
  88: 	void ClearDarkFlow();
  90: 	void OnAvatarUninitialized(TWeakObjectPtr<AActor> ForAvatar);
  91: 	void BindAvatar();
  93: 	void Unbind();
  94: };
```

## HodgeGameplayAbility_HitReaction.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
   7: #include "Combat/HodgeHitReactionTypes.h"
   8: #include "HodgeGameplayAbility_HitReaction.generated.h"
  10: class UHodgeHitReactionComponent;
  11: class UHodgeCombatComponentBase;
  12: class USkeletalMeshComponent;
  13: class ACharacter;
  16: UCLASS(Blueprintable)
  17: class HODGEPODGE_API UHodgeGameplayAbility_HitReaction : public UHodgeGameplayAbility
  18: {
  19: 	GENERATED_BODY()
  20: public:
  24: 	UHodgeGameplayAbility_HitReaction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  29: 	bool Synchronize(const FHodgeHitReactionState& State);
  33: 	void FinishReaction(bool bCancelled = false);
  34: protected:
  43: 	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
  44: 		const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
  45: 		FGameplayTagContainer* RelevantTags) const override;
  52: 	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
  53: 		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Payload) override;
  61: 	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
  62: 		FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
  63: private:
  65: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeHitReactionComponent> Reaction;
  67: 	UPROPERTY(Transient) TWeakObjectPtr<UAnimMontage> PlayedMontage;
  69: 	UPROPERTY(Transient) TWeakObjectPtr<ACharacter> ReactionAvatar;
  71: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeCombatComponentBase> PoseCombat;
  73: 	UPROPERTY(Transient) TWeakObjectPtr<USkeletalMeshComponent> PoseMesh;
  75: 	FGuid PoseLease;
  77: 	uint8 SavedPosePolicy = 0;
  79: 	int32 AppliedSequence = 0;
  81: 	EHodgeHitReactionPhase AppliedPhase = EHodgeHitReactionPhase::None;
  83: 	float AppliedStartedAt = -1.f;
  85: 	uint16 MotionSourceId = 0;
  87: 	FVector QueuedLaunchVelocity = FVector::ZeroVector;
  89: 	bool bEnding = false;
  91: 	void ClearMotion();
  92: };
```

## HodgeGameplayAbility_Jump.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "HodgeGameplayAbility.h"
  10: #include "HodgeGameplayAbility_Jump.generated.h"
  12: class UObject;
  13: struct FFrame;
  14: struct FGameplayAbilityActorInfo;
  15: struct FGameplayTagContainer;
  24: UCLASS(Abstract)
  25: class UHodgeGameplayAbility_Jump : public UHodgeGameplayAbility
  26: {
  27: 	GENERATED_BODY()
  29: public:
  33: 	UHodgeGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  35: protected:
  44: 	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
  45: 	                                const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
  46: 	                                FGameplayTagContainer* OptionalRelevantTags) const override;
  54: 	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
  55: 	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
  56: 	                        bool bWasCancelled) override;
  59: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
  60: 	void CharacterJumpStart();
  63: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
  64: 	void CharacterJumpStop();
  65: };
```

## HodgeGameplayAbility_LockOn.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
   4: #include "HodgeGameplayAbility_LockOn.generated.h"
   5: class UHodgeLockOnComponent;
   7: UCLASS()
   8: class HODGEPODGE_API UHodgeGameplayAbility_LockOn : public UHodgeGameplayAbility
   9: {
  10: 	GENERATED_BODY()
  11: public:
  12: 	UHodgeGameplayAbility_LockOn();
  13: 	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
  14: 		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* RelevantTags = nullptr) const override;
  15: protected:
  16: 	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
  17: 	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
  18: private:
  20: 	void OnTargetData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag Tag);
  22: 	void OnTimeout();
  23: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeLockOnComponent> Component;
  24: 	FDelegateHandle DataHandle;
  25: 	FTimerHandle TimeoutHandle;
  26: 	int32 Sequence = 0;
  28: 	int32 ComponentGeneration = 0;
  29: };
```

## HodgeGameplayAbility_Melee.h

接收服务器 Window/Point 命中结果，准备锚点/目标、逐段/共享去重，构建独立 GE Spec/Context 并施加。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)、[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)、[Combat/HodgeAttackRule.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackRule.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
   7: #include "Combat/HodgeHitDetection.h"
   8: #include "Animation/HodgeCombatAnimNotifies.h"
   9: #include "Combat/HodgeAttackRule.h"
  10: #include "HodgeGameplayAbility_Melee.generated.h"
  12: class UHodgeAbilityTask_WaitHitResults;
  13: class UAbilitySystemComponent;
  16: struct FHodgeMeleeHitHistory
  17: {
  19: 	TMap<TWeakObjectPtr<UAbilitySystemComponent>, double> LastHitTimes;
  26: 	bool CanHit(UAbilitySystemComponent* Target, double Time, float RepeatInterval) const;
  31: 	void RecordHit(UAbilitySystemComponent* Target, double Time);
  32: };
  35: USTRUCT()
  36: struct FHodgeMeleeHitState
  37: {
  38: 	GENERATED_BODY()
  40: 	UPROPERTY() FHodgeHitEffectConfig Binding;
  42: 	uint64 SessionHandle = 0;
  44: 	int32 LastSampleSequence = 0;
  46: 	TSharedPtr<FHodgeMeleeHitHistory> History;
  47: };
  50: UCLASS(Blueprintable)
  51: class HODGEPODGE_API UHodgeGameplayAbility_Melee : public UHodgeGameplayAbility_Definition
  52: {
  53: 	GENERATED_BODY()
  54: public:
  56: 	UHodgeGameplayAbility_Melee();
  61: 	virtual void ValidateExecutionConfiguration(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const override;
  67: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Combat")
  68: 	bool SetHitAnchor(FName Key, const FTransform& WorldTransform);
  74: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Combat")
  75: 	bool SetHitTarget(FName Key, AActor* Target);
  77: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Combat")
  78: 	void ResetHitGeometryHistory();
  87: 	bool BeginNotifyHit(int32 OccurrenceId, const FHodgeAnimHitConfig& Config, USkeletalMeshComponent* Mesh, bool bSingle);
  89: protected:
  91: 	virtual void OnExecutionReady() override;
  95: 	virtual void OnExecutionEnding(const FGuid& EndingExecutionId) override;
  99: 	virtual void OnNotifyResourceEnded(int32 OccurrenceId) override;
 103: 	UFUNCTION(BlueprintNativeEvent, Category="Hodge|Combat") bool PrepareHitExecutionContext();
 107: 	virtual bool PrepareHitExecutionContext_Implementation();
 113: 	UFUNCTION(BlueprintNativeEvent, Category="Hodge|Melee")
 114: 	void ProcessMeleeHitResults(const FHodgeHitDetectionBatch& Batch);
 118: 	virtual void ProcessMeleeHitResults_Implementation(const FHodgeHitDetectionBatch& Batch);
 126: 	UFUNCTION(BlueprintNativeEvent, Category="Hodge|Melee")
 127: 	bool CanApplyMeleeHit(AActor* Target, const FHitResult& Hit, const FHodgeHitEffectConfig& Binding) const;
 134: 	virtual bool CanApplyMeleeHit_Implementation(AActor* Target, const FHitResult& Hit,
 135: 		const FHodgeHitEffectConfig& Binding) const;
 145: 	UFUNCTION(BlueprintNativeEvent, Category="Hodge|Melee")
 146: 	void ApplyMeleeHitEffects(const FHodgeHitDetectionBatch& Batch, const FHitResult& Hit,
 147: 		const FHodgeHitEffectConfig& Binding, UAbilitySystemComponent* TargetASC, const FGameplayEffectSpecHandle& Spec);
 155: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Melee")
 156: 	void SubmitMeleeHitEffects(const FHodgeHitDetectionBatch& Batch, const FHitResult& Hit,
 157: 		const FHodgeHitEffectConfig& Binding, UAbilitySystemComponent* TargetASC, const FGameplayEffectSpecHandle& Spec);
 162: 	UFUNCTION(BlueprintImplementableEvent, Category="Hodge|Melee")
 163: 	void OnMeleeReactionResolved(AActor* Target, const FHodgeHitReactionResult& Result);
 171: 	virtual void ApplyMeleeHitEffects_Implementation(const FHodgeHitDetectionBatch& Batch, const FHitResult& Hit,
 172: 		const FHodgeHitEffectConfig& Binding, UAbilitySystemComponent* TargetASC, const FGameplayEffectSpecHandle& Spec);
 179: 	virtual FGameplayEffectContextHandle MakeMeleeHitContext(const FHodgeHitDetectionBatch& Batch, const FHitResult& Hit) const;
 186: 	virtual FGameplayEffectSpecHandle BuildMeleeHitSpec(const FHodgeHitDetectionBatch& Batch,
 187: 		const FHitResult& Hit, const FHodgeHitEffectConfig& Binding) const;
 192: 	bool IsMeleeBatchCurrent(const FHodgeHitDetectionBatch& Batch) const;
 199: 	bool OpenHit(int32 OccurrenceId, const FHodgeHitEffectConfig& Binding, const FHodgeHitDetectionRequest& Request);
 201: private:
 202: 	friend struct FHodgeMeleeTestAccess;
 206: 	void OnHitResults(const FHodgeHitDetectionBatch& Batch);
 208: 	UPROPERTY(Transient) TObjectPtr<UHodgeAbilityTask_WaitHitResults> DetectionTask;
 210: 	UPROPERTY(Transient) TMap<int32, FHodgeMeleeHitState> HitStates;
 212: 	TMap<FHodgeHitGroupKey, TSharedPtr<FHodgeMeleeHitHistory>> HitGroups;
 214: 	UPROPERTY(Transient) TMap<FName, FTransform> HitAnchors;
 216: 	UPROPERTY(Transient) TMap<FName, TWeakObjectPtr<AActor>> HitTargets;
 217: };
```

## HodgeGameplayAbility_MovementAction.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[Combat/HodgeMovementActionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeMovementActionTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
   7: #include "Combat/HodgeMovementActionTypes.h"
   8: #include "Abilities/GameplayAbilityTargetTypes.h"
   9: #include "HodgeGameplayAbility_MovementAction.generated.h"
  11: class UHodgeSprintAbilityProfile;
  12: class UHodgeLocomotionPolicyComponent;
  13: class UHodgeCharacterRotationComponent;
  14: class UCharacterMovementComponent;
  17: USTRUCT()
  18: struct FHodgeMovementActionTargetData : public FGameplayAbilityTargetData
  19: {
  20: 	GENERATED_BODY()
  22: 	FVector2D Input = FVector2D::ZeroVector;
  24: 	float ControlYaw = 0.f;
  26: 	float ActorYawAtStart = 0.f;
  28: 	int32 SessionId = 0;
  32: 	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
  39: 	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bSuccess);
  40: };
  42: template<> struct TStructOpsTypeTraits<FHodgeMovementActionTargetData> : TStructOpsTypeTraitsBase2<FHodgeMovementActionTargetData>
  43: { enum { WithNetSerializer = true, WithCopy = true }; };
  46: UCLASS(Abstract, Blueprintable)
  47: class HODGEPODGE_API UHodgeGameplayAbility_MovementAction : public UHodgeGameplayAbility
  48: {
  49: 	GENERATED_BODY()
  50: public:
  52: 	UHodgeGameplayAbility_MovementAction();
  54: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UHodgeSprintAbilityProfile> Profile;
  58: 	UFUNCTION(BlueprintPure) bool HasCommittedMovement() const { return bCommitted; }
  62: 	UFUNCTION(BlueprintPure) int32 GetInputSessionId() const { return SessionId; }
  66: 	FGuid GetMovementExecutionId() const { return ExecutionId; }
  71: 	const UHodgeSprintAbilityProfile* ResolveProfile(const FGameplayAbilityActorInfo* Info) const;
  78: 	virtual bool CheckCooldown(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayTagContainer* Tags = nullptr) const override { return true; }
  84: 	virtual void ApplyCooldown(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo) const override {}
  91: 	virtual bool CheckCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayTagContainer* Tags = nullptr) const override { return true; }
  97: 	virtual void ApplyCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo) const override {}
 106: 	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
 107: 		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
 108: 		FGameplayTagContainer* RelevantTags = nullptr) const override;
 109: protected:
 110: 	friend struct FHodgeDashAirTestAccess;
 112: 	virtual bool IsExecutionMovementModeValid(const UCharacterMovementComponent* Movement) const;
 119: 	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
 120: 		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
 128: 	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
 129: 		FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
 133: 	virtual void BeginMovement(const FHodgeMoveIntentSnapshot& Intent) PURE_VIRTUAL(UHodgeGameplayAbility_MovementAction::BeginMovement, );
 137: 	virtual void UpdateMovement(float DeltaTime) PURE_VIRTUAL(UHodgeGameplayAbility_MovementAction::UpdateMovement, );
 141: 	virtual void HandleMovementFollowUp(const FGameplayAbilityTargetDataHandle& Data) {}
 145: 	void Finish(bool bCancelled = true);
 149: 	bool IsMovementValid() const;
 154: 	void SetMovementTag(FGameplayTag Tag, bool bAdd);
 158: 	void StartCue(FGameplayTag Tag);
 160: 	void StopCue();
 162: 	UPROPERTY(Transient) TObjectPtr<const UHodgeSprintAbilityProfile> ExecutionProfile;
 164: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeLocomotionPolicyComponent> Policy;
 166: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeCharacterRotationComponent> Rotation;
 168: 	FGuid ExecutionId;
 170: 	int32 SessionId = 0;
 172: 	float ActorYawAtStart = 0.f;
 174: 	bool bCommitted = false;
 176: 	double PreparingAt = 0.;
 178: 	double CommittedAt = 0.;
 180: 	FHodgeFacingRequestHandle FacingHandle;
 182: 	FGameplayTag MovementTag;
 183: private:
 188: 	void OnTargetData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag Tag);
 190: 	void TickMovement();
 195: 	void OnConstraintChanged(FGameplayTag Tag, int32 Count);
 197: 	FDelegateHandle ConstraintHandles[3];
 199: 	FDelegateHandle DataHandle;
 201: 	FTimerHandle TickHandle;
 203: 	FGameplayTag ActiveCue;
 205: 	bool bReceivedData = false;
 206: };
```

## HodgeGameplayAbility_ResonanceCircuit.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_ResonanceCircuit.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_ResonanceCircuit.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
   4: #include "HodgeGameplayAbility_ResonanceCircuit.generated.h"
   7: UCLASS(Blueprintable)
   8: class HODGEPODGE_API UHodgeGameplayAbility_ResonanceCircuit : public UHodgeGameplayAbility_Melee
   9: {
  10: 	GENERATED_BODY()
  11: public:
  13: 	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* Info,
  14: 		const FGameplayTagContainer* SourceTags=nullptr,const FGameplayTagContainer* TargetTags=nullptr,FGameplayTagContainer* RelevantTags=nullptr) const override;
  16: 	bool CommitDarkSurge(const FBranchingPointNotifyPayload& Payload,const FHodgeAnimHitConfig& Burst);
  18: 	UPROPERTY(EditDefaultsOnly, Category="Hodge|Havoc") bool bRequireGrounded=true;
  20: 	UPROPERTY(EditDefaultsOnly, Category="Hodge|Havoc") FGameplayTag EntryCancelWindow;
  21: protected:
  23: 	virtual void OnExecutionReady() override;
  25: 	virtual void ProcessMeleeHitResults_Implementation(const FHodgeHitDetectionBatch& Batch) override;
  26: private:
  27: 	bool bCommitted=false;
  28: 	int32 BurstOccurrence=INDEX_NONE;
  29: };
```

## HodgeGameplayAbility_Sprint.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h"
   7: #include "HodgeGameplayAbility_Sprint.generated.h"
   9: class UAbilityTask_PlayMontageAndWait;
  11: USTRUCT()
  12: struct FHodgeSprintPivotTargetData : public FGameplayAbilityTargetData
  13: {
  14: 	GENERATED_BODY()
  16: 	FVector IncomingDirection = FVector::ZeroVector;
  18: 	FVector DesiredDirection = FVector::ZeroVector;
  20: 	float StartingYaw = 0.f;
  22: 	int32 SessionId = 0;
  24: 	int32 SequenceId = 0;
  28: 	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
  35: 	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bSuccess);
  36: };
  38: template<> struct TStructOpsTypeTraits<FHodgeSprintPivotTargetData> : TStructOpsTypeTraitsBase2<FHodgeSprintPivotTargetData>
  39: { enum { WithNetSerializer = true, WithCopy = true }; };
  42: UCLASS(Blueprintable)
  43: class HODGEPODGE_API UHodgeGameplayAbility_Sprint : public UHodgeGameplayAbility_MovementAction
  44: {
  45: 	GENERATED_BODY()
  46: public:
  48: 	UHodgeGameplayAbility_Sprint();
  52: 	UFUNCTION(BlueprintPure) bool IsPivoting() const { return bPivoting; }
  60: 	static bool QualifiesPivot(FVector Velocity, FVector Desired, float MinimumSpeed, float Angle);
  66: 	static bool IsPivotRecoveryComplete(float Position, float EndTime);
  71: 	void ProcessMovementIntent(FVector Desired);
  72: protected:
  76: 	virtual void BeginMovement(const FHodgeMoveIntentSnapshot& Intent) override;
  80: 	virtual void UpdateMovement(float DeltaTime) override;
  84: 	virtual void HandleMovementFollowUp(const FGameplayAbilityTargetDataHandle& Data) override;
  92: 	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
  93: private:
  98: 	void RequestPivot(FVector Velocity, FVector Desired);
 102: 	void StartPivot(float StartingYaw);
 104: 	void UpdatePivotRecovery();
 106: 	void ReleasePivotTask();
 108: 	UFUNCTION() void OnPivotFinished();
 110: 	UPROPERTY(Transient) TObjectPtr<UAbilityTask_PlayMontageAndWait> PivotTask;
 112: 	bool bPivoting = false;
 114: 	bool bEnding = false;
 116: 	int32 PivotSequence = 0;
 118: 	int32 PivotMontageInstanceId = INDEX_NONE;
 120: 	FGameplayAbilityTargetDataHandle DeferredPivotData;
 122: 	double DeferredPivotExpiresAt = 0.;
 124: 	double NextPivotAt = 0.;
 126: 	void OnAuthorityEnded();
 131: 	void OnAttackStarted(FGameplayTag Tag, int32 Count);
 133: 	FDelegateHandle AttackHandle;
 142: 	void OnDamage(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* Spec, float Magnitude, float Old, float New);
 144: 	FGuid PolicyHandle;
 146: 	FDelegateHandle AuthorityHandle;
 148: 	FDelegateHandle DamageHandle;
 150: 	double NoInputAt = 0.;
 152: 	double BlockedAt = 0.;
 154: 	FHodgeMoveIntentSnapshot PendingIntent;
 155: };
```

## HodgeAttributeSet.h

项目 AttributeSet 基础和 ASC 访问。

源码：[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  12: #pragma once
  15: #include "CoreMinimal.h"
  18: #include "AttributeSet.h"
  20: #include "HodgeAttributeSet.generated.h"
  67: #define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
  68:                                                                  \
  69: GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
  70:                              \
  71: GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
  72:                                \
  73: GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
  74:                              \
  75: GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
  78: struct FGameplayEffectSpec;
  81: class UHodgeAbilitySystemComponent;
 105: DECLARE_MULTICAST_DELEGATE_SixParams(FHodgeAttributeEvent, AActor*                     , AActor*                 ,
 106:                                      const FGameplayEffectSpec*               , float                    ,
 107:                                      float             , float             );
 117: UCLASS()
 118: class HODGEPODGE_API UHodgeAttributeSet : public UAttributeSet
 119: {
 120: 	GENERATED_BODY()
 122: public:
 125: 	UHodgeAttributeSet();
 131: 	virtual UWorld* GetWorld() const override;
 137: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const;
 138: };
```

## HodgeCombatSet.h

BaseDamage/BaseHeal 战斗属性，OwnerOnly 复制，供伤害/治疗 Execution 捕获。

源码：[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   9: #include "CoreMinimal.h"
  12: #include "AbilitySystemComponent.h"
  15: #include "HodgeAttributeSet.h"
  17: #include "HodgeCombatSet.generated.h"
  31: UCLASS(BlueprintType)
  32: class UHodgeCombatSet : public UHodgeAttributeSet
  33: {
  34: 	GENERATED_BODY()
  36: public:
  39: 	UHodgeCombatSet();
  42: 	ATTRIBUTE_ACCESSORS(UHodgeCombatSet, BaseDamage);
  45: 	ATTRIBUTE_ACCESSORS(UHodgeCombatSet, BaseHeal);
  47: 	ATTRIBUTE_ACCESSORS(UHodgeCombatSet, HavocDamageBonus);
  49: protected:
  54: 	UFUNCTION()
  55: 	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);
  61: 	UFUNCTION()
  62: 	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);
  64: private:
  66: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_HavocDamageBonus, Category="Hodge|Combat", meta=(AllowPrivateAccess=true))
  67: 	FGameplayAttributeData HavocDamageBonus;
  69: 	UFUNCTION() void OnRep_HavocDamageBonus(const FGameplayAttributeData& OldValue);
  72: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Hodge|Combat",
  73: 		Meta = (AllowPrivateAccess = true))
  75: 	FGameplayAttributeData BaseDamage;
  79: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Hodge|Combat",
  80: 		Meta = (AllowPrivateAccess = true))
  82: 	FGameplayAttributeData BaseHeal;
  83: };
```

## HodgeHavocSet.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "AbilitySystem/AttributeSet/HodgeAttributeSet.h"
   4: #include "AbilitySystemComponent.h"
   5: #include "NativeGameplayTags.h"
   6: #include "HodgeHavocSet.generated.h"
   9: namespace HodgeHavocTags
  10: {
  11: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(DarkFlowDelta);
  12: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(DarkSurge);
  13: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HavocElement);
  14: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CircuitInput);
  15: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CircuitAbility);
  16: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CastCommitted);
  17: }
  20: UCLASS(BlueprintType)
  21: class HODGEPODGE_API UHodgeHavocSet : public UHodgeAttributeSet
  22: {
  23: 	GENERATED_BODY()
  24: public:
  26: 	UHodgeHavocSet();
  27: 	ATTRIBUTE_ACCESSORS(UHodgeHavocSet, DarkFlow);
  28: 	ATTRIBUTE_ACCESSORS(UHodgeHavocSet, MaxDarkFlow);
  30: 	bool IsResourceReady() const;
  32: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
  33: protected:
  35: 	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
  36: 	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
  38: 	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
  39: private:
  41: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_DarkFlow, meta=(AllowPrivateAccess="true")) FGameplayAttributeData DarkFlow;
  43: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxDarkFlow, meta=(AllowPrivateAccess="true")) FGameplayAttributeData MaxDarkFlow;
  45: 	UFUNCTION() void OnRep_DarkFlow(const FGameplayAttributeData& Old);
  46: 	UFUNCTION() void OnRep_MaxDarkFlow(const FGameplayAttributeData& Old);
  48: 	void Clamp(const FGameplayAttribute& Attribute, float& Value) const;
  49: };
```

## HodgeHealthSet.h

Health/MaxHealth、Damage/Healing 元属性、结算夹取/免疫/耗尽广播；BaseDamage/BaseHeal 位于 CombatSet。

源码：[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   7: #pragma once
  10: #include "CoreMinimal.h"
  13: #include "AbilitySystemComponent.h"
  16: #include "NativeGameplayTags.h"
  17: #include "AbilitySystem/AttributeSet/HodgeAttributeSet.h"
  19: #include "HodgeHealthSet.generated.h"
  22: class UObject;
  25: struct FFrame;
  28: HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
  31: HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
  34: HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
  37: HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);
  40: struct FGameplayEffectModCallbackData;
  55: UCLASS(BlueprintType)
  56: class HODGEPODGE_API UHodgeHealthSet : public UHodgeAttributeSet
  57: {
  58: 	GENERATED_BODY()
  60: public:
  62: 	mutable FHodgeAttributeEvent OnDamageAccepted;
  65: 	UHodgeHealthSet();
  68: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Health);
  71: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, MaxHealth);
  74: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Healing);
  77: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Damage);
  79: 	void BeginAttributeRebuild();
  84: 	float EndAttributeRebuild(bool& bReachedZero);
  89: 	bool SetHealthForAttributeCommit(float NewHealth);
  93: 	mutable FHodgeAttributeEvent OnHealthChanged;
  97: 	mutable FHodgeAttributeEvent OnMaxHealthChanged;
 101: 	mutable FHodgeAttributeEvent OnOutOfHealth;
 103: protected:
 108: 	UFUNCTION()
 109: 	void OnRep_Health(const FGameplayAttributeData& OldValue);
 115: 	UFUNCTION()
 116: 	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
 123: 	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
 129: 	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
 136: 	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
 143: 	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
 151: 	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
 155: 	float GetResourceClampMax() const;
 157: 	TArray<FVector2D> GameplayResourceSnapshots;
 159: 	int32 AttributeRebuildDepth = 0;
 161: 	float AttributeRebuildClampMax = 1.f;
 163: 	float AttributeRebuildResourceDelta = 0.f;
 165: 	bool bReachedZeroDuringRebuild = false;
 172: 	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
 174: private:
 178: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Hodge|Health",
 179: 		Meta = (HideFromModifiers, AllowPrivateAccess = true))
 181: 	FGameplayAttributeData Health;
 185: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Hodge|Health",
 186: 		Meta = (AllowPrivateAccess = true))
 188: 	FGameplayAttributeData MaxHealth;
 192: 	bool bOutOfHealth;
 196: 	float MaxHealthBeforeAttributeChange;
 199: 	float HealthBeforeAttributeChange;
 210: 	UPROPERTY(BlueprintReadOnly, Category="Hodge|Health", Meta=(AllowPrivateAccess=true))
 211: 	FGameplayAttributeData Healing;
 216: 	UPROPERTY(BlueprintReadOnly, Category="Hodge|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
 217: 	FGameplayAttributeData Damage;
 218: };
```

## HodgeResonanceSet.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "AbilitySystem/AttributeSet/HodgeAttributeSet.h"
   4: #include "AbilitySystemComponent.h"
   5: #include "NativeGameplayTags.h"
   6: #include "HodgeResonanceSet.generated.h"
   9: namespace HodgeResonanceTags
  10: {
  11: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EnergyDelta);
  12: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability);
  13: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input);
  14: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown);
  15: }
  18: UCLASS(BlueprintType)
  19: class HODGEPODGE_API UHodgeResonanceSet : public UHodgeAttributeSet
  20: {
  21: 	GENERATED_BODY()
  22: public:
  24: 	UHodgeResonanceSet();
  25: 	ATTRIBUTE_ACCESSORS(UHodgeResonanceSet, ResonanceEnergy);
  26: 	ATTRIBUTE_ACCESSORS(UHodgeResonanceSet, MaxResonanceEnergy);
  28: 	bool IsResourceReady() const;
  30: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
  31: protected:
  33: 	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
  35: 	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
  37: 	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
  38: private:
  40: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ResonanceEnergy, Category="Resonance", meta=(AllowPrivateAccess="true")) FGameplayAttributeData ResonanceEnergy;
  42: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxResonanceEnergy, Category="Resonance", meta=(AllowPrivateAccess="true")) FGameplayAttributeData MaxResonanceEnergy;
  44: 	UFUNCTION() void OnRep_ResonanceEnergy(const FGameplayAttributeData& Old);
  45: 	UFUNCTION() void OnRep_MaxResonanceEnergy(const FGameplayAttributeData& Old);
  47: 	void Clamp(const FGameplayAttribute& Attribute, float& Value) const;
  48: };
```

## HodgeHavocEffects.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeHavocEffects.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeHavocEffects.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "GameplayEffect.h"
   4: #include "GameplayEffectExecutionCalculation.h"
   5: #include "HodgeHavocEffects.generated.h"
   8: UCLASS(Blueprintable)
   9: class HODGEPODGE_API UHodgeDarkFlowEffect : public UGameplayEffect
  10: {
  11: 	GENERATED_BODY()
  12: public:
  14: 	UHodgeDarkFlowEffect();
  15: };
  18: UCLASS(Blueprintable)
  19: class HODGEPODGE_API UHodgeDarkFlowInitialization : public UGameplayEffect
  20: {
  21: 	GENERATED_BODY()
  22: public:
  23: 	UHodgeDarkFlowInitialization();
  24: };
  27: UCLASS(Blueprintable)
  28: class HODGEPODGE_API UHodgeDarkSurgeEffect : public UGameplayEffect
  29: {
  30: 	GENERATED_BODY()
  31: public:
  32: 	UHodgeDarkSurgeEffect();
  33: };
  36: UCLASS(Blueprintable)
  37: class HODGEPODGE_API UHodgeDarkFlowDrainEffect : public UGameplayEffect
  38: {
  39: 	GENERATED_BODY()
  40: public:
  41: 	UHodgeDarkFlowDrainEffect();
  42: };
  45: UCLASS()
  46: class HODGEPODGE_API UHodgeDarkFlowDrainExecution : public UGameplayEffectExecutionCalculation
  47: {
  48: 	GENERATED_BODY()
  49: public:
  51: 	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& Parameters,
  52: 		FGameplayEffectCustomExecutionOutput& Output) const override;
  53: };
```

## HodgeResonanceEnergyEffect.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "GameplayEffect.h"
   4: #include "HodgeResonanceEnergyEffect.generated.h"
   7: UCLASS(Blueprintable)
   8: class HODGEPODGE_API UHodgeResonanceEnergyEffect : public UGameplayEffect
   9: {
  10: 	GENERATED_BODY()
  11: public:
  13: 	UHodgeResonanceEnergyEffect();
  14: };
  17: UCLASS(Blueprintable)
  18: class HODGEPODGE_API UHodgeResonanceEnergyInitialization : public UGameplayEffect
  19: {
  20: 	GENERATED_BODY()
  21: public:
  23: 	UHodgeResonanceEnergyInitialization();
  24: };
```

## HodgeDamageExecution.h

BaseDamage × SetByCaller 倍率 × 距离/材质衰减 × 共享目标规则；不再恒零。

源码：[Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeDamageExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeDamageExecution.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   9: #include "CoreMinimal.h"
  13: #include "GameplayEffectExecutionCalculation.h"
  15: #include "HodgeDamageExecution.generated.h"
  29: UCLASS()
  30: class HODGEPODGE_API UHodgeDamageExecution : public UGameplayEffectExecutionCalculation
  31: {
  32: 	GENERATED_BODY()
  34: public:
  38: 	UHodgeDamageExecution();
  40: protected:
  48: 	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
  49: 	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
  50: };
```

## HodgeHealExecution.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeHealExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeHealExecution.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   9: #include "CoreMinimal.h"
  13: #include "GameplayEffectExecutionCalculation.h"
  15: #include "HodgeHealExecution.generated.h"
  29: UCLASS()
  30: class HODGEPODGE_API UHodgeHealExecution : public UGameplayEffectExecutionCalculation
  31: {
  32: 	GENERATED_BODY()
  34: public:
  38: 	UHodgeHealExecution();
  40: protected:
  48: 	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
  49: 	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
  50: };
```

## HodgeGameplayCue_Movement.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/GameplayCues/HodgeGameplayCue_Movement.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayCues/HodgeGameplayCue_Movement.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "GameplayCueNotify_Actor.h"
   7: #include "HodgeGameplayCue_Movement.generated.h"
   8: class UNiagaraSystem;
   9: class UNiagaraComponent;
  12: UCLASS(Blueprintable)
  13: class HODGEPODGE_API AHodgeGameplayCue_Movement : public AGameplayCueNotify_Actor
  14: {
  15: 	GENERATED_BODY()
  16: public:
  18: 	AHodgeGameplayCue_Movement();
  20: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UNiagaraSystem> Effect;
  26: 	virtual bool OnActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
  32: 	virtual bool WhileActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
  38: 	virtual bool OnRemove_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
  44: 	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
  45: private:
  47: 	UPROPERTY(Transient) TObjectPtr<UNiagaraComponent> ActiveEffect;
  48: };
```

## GameplayTagStack.h

带计数的标签栈及复制数据结构，区别于只判断有无的 TagContainer。

源码：[Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "GameplayTagContainer.h"
   9: #include "Net/Serialization/FastArraySerializer.h"
  11: #include "GameplayTagStack.generated.h"
  14: struct FGameplayTagStackContainer;
  17: struct FNetDeltaSerializeInfo;
  23: USTRUCT(BlueprintType)
  24: struct FGameplayTagStack : public FFastArraySerializerItem
  25: {
  26: 	GENERATED_BODY()
  30: 	FGameplayTagStack()
  31: 	{
  32: 	}
  39: 	FGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
  40: 		: Tag(InTag)
  41: 		  , StackCount(InStackCount)
  42: 	{
  43: 	}
  49: 	FString GetDebugString() const;
  51: private:
  53: 	friend FGameplayTagStackContainer;
  56: 	UPROPERTY()
  57: 	FGameplayTag Tag;
  60: 	UPROPERTY()
  61: 	int32 StackCount = 0;
  62: };
  68: USTRUCT(BlueprintType)
  69: struct FGameplayTagStackContainer : public FFastArraySerializer
  70: {
  71: 	GENERATED_BODY()
  75: 	FGameplayTagStackContainer()
  77: 	{
  78: 	}
  80: public:
  86: 	void AddStack(FGameplayTag Tag, int32 StackCount);
  93: 	void RemoveStack(FGameplayTag Tag, int32 StackCount);
 100: 	int32 GetStackCount(FGameplayTag Tag) const
 101: 	{
 102: 		return TagToCountMap.FindRef(Tag);
 103: 	}
 110: 	bool ContainsTag(FGameplayTag Tag) const
 111: 	{
 112: 		return TagToCountMap.Contains(Tag);
 113: 	}
 122: 	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
 129: 	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
 136: 	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
 145: 	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
 146: 	{
 147: 		return FFastArraySerializer::FastArrayDeltaSerialize<FGameplayTagStack, FGameplayTagStackContainer>(
 148: 			Stacks, DeltaParms, *this);
 149: 	}
 151: private:
 153: 	UPROPERTY()
 154: 	TArray<FGameplayTagStack> Stacks;
 157: 	TMap<FGameplayTag, int32> TagToCountMap;
 158: };
 161: template <>
 163: struct TStructOpsTypeTraits<FGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FGameplayTagStackContainer>
 164: {
 165: 	enum
 166: 	{
 168: 		WithNetDeltaSerializer = true,
 169: 	};
 170: };
```

## HodgeAbilitySystemComponent.h

Tag 输入、激活组、关系映射、Montage 复制和全局注册；失败时恢复武器预测与连段记忆。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "AbilitySystemComponent.h"
   8: #include "NativeGameplayTags.h"
   9: #include "Abilities/HodgeGameplayAbility.h"
  10: #include "HodgeAbilitySystemComponent.generated.h"
  13: class UHodgeAbilityTagRelationshipMapping;
  14: class UHodgeAbilityDefinition;
  17: USTRUCT()
  18: struct FHodgeDefinitionMontageState
  19: {
  20: 	GENERATED_BODY()
  22: 	UPROPERTY()
  23: 	TObjectPtr<const UHodgeAbilityDefinition> Definition;
  25: 	UPROPERTY()
  26: 	uint8 PlayInstanceID = 0;
  28: 	UPROPERTY()
  29: 	bool bNaturalStop = false;
  30: };
  33: USTRUCT()
  34: struct FHodgeGrantedAbilityDefinition
  35: {
  36: 	GENERATED_BODY()
  38: 	UPROPERTY()
  39: 	FGameplayAbilitySpecHandle Handle;
  41: 	UPROPERTY()
  42: 	TObjectPtr<const UHodgeAbilityDefinition> Definition;
  43: };
  46: HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_AbilityInputBlocked);
  60: UCLASS()
  61: class HODGEPODGE_API UHodgeAbilitySystemComponent : public UAbilitySystemComponent
  62: {
  63: 	GENERATED_BODY()
  65: public:
  67: 	virtual void ServerSetReplicatedTargetData_Implementation(FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey, const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ApplicationTag, FPredictionKey CurrentPredictionKey) override;
  69: 	void RetireLockOnCommand(FGameplayAbilitySpecHandle Handle, FPredictionKey Key);
  71: 	void RetireAttackFacingData(FGameplayAbilitySpecHandle Handle, FPredictionKey Key);
  76: 	UHodgeAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  87: 	virtual float PlayMontage(UGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo,
  88: 	                          UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None,
  89: 	                          float StartTimeSeconds = 0.f) override;
  96: 	virtual float
  97: 	PlayMontageSimulated(UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None) override;
 101: 	virtual void CurrentMontageStop(float OverrideBlendOutTime = -1.f) override;
 106: 	void StopDefinitionMontage(UGameplayAbility* Ability, bool bNatural);
 110: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 117: 	FGameplayAbilitySpecHandle GiveAbilityDefinition(const UHodgeAbilityDefinition* Definition, int32 Level,
 118: 	                                                 UObject* SourceObject);
 123: 	UFUNCTION(BlueprintPure, Category="Hodge|Abilities")
 124: 	const UHodgeAbilityDefinition* FindAbilityDefinition(FGameplayAbilitySpecHandle Handle) const;
 129: 	FGameplayAbilitySpecHandle FindDefinitionAbility(FGameplayTag AbilityTag) const;
 136: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 144: 	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
 147: 	typedef TFunctionRef<bool(const UHodgeGameplayAbility* HodgeAbility, FGameplayAbilitySpecHandle Handle)>
 148: 	TShouldCancelAbilityFunc;
 155: 	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);
 161: 	void CancelInputActivatedAbilities(bool bReplicateCancelAbility);
 167: 	void AbilityInputTagPressed(const FGameplayTag& InputTag);
 169: 	FGameplayAbilitySpecHandle ResolveAbilityVariant(FGameplayTag InputTag, FString* OutFailureReason=nullptr) const;
 175: 	void AbilityInputTagReleased(const FGameplayTag& InputTag);
 182: 	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
 186: 	void ClearAbilityInput();
 188: 	void ReleaseAbilityInput();
 195: 	bool IsActivationGroupBlocked(EHodgeAbilityActivationGroup Group) const;
 202: 	void AddAbilityToActivationGroup(EHodgeAbilityActivationGroup Group, UHodgeGameplayAbility* HodgeAbility);
 209: 	void RemoveAbilityFromActivationGroup(EHodgeAbilityActivationGroup Group, UHodgeGameplayAbility* HodgeAbility);
 217: 	void CancelActivationGroupAbilities(EHodgeAbilityActivationGroup Group, UHodgeGameplayAbility* IgnoreHodgeAbility,
 218: 	                                    bool bReplicateCancelAbility);
 225: 	void AddDynamicTagGameplayEffect(const FGameplayTag& Tag);
 232: 	void RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag);
 241: 	void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle,
 242: 	                          FGameplayAbilityActivationInfo ActivationInfo,
 243: 	                          FGameplayAbilityTargetDataHandle& OutTargetDataHandle);
 250: 	void SetTagRelationshipMapping(UHodgeAbilityTagRelationshipMapping* NewMapping);
 255: 	UFUNCTION(BlueprintPure, Category="Hodge|Abilities")
 256: 	bool HasActiveAbilityTag(FGameplayTag Tag) const;
 257: 	friend struct FHodgeRelationshipTestAccess;
 266: 	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags,
 267: 	                                            FGameplayTagContainer& OutActivationRequired,
 268: 	                                            FGameplayTagContainer& OutActivationBlocked) const;
 270: protected:
 272: 	UPROPERTY(ReplicatedUsing=OnRep_DefinitionMontage)
 273: 	FHodgeDefinitionMontageState DefinitionMontage;
 275: 	UFUNCTION()
 276: 	void OnRep_DefinitionMontage();
 278: 	virtual void OnRep_ReplicatedAnimMontage() override;
 280: 	void ApplyDefinitionMontageSettings();
 282: 	int32 ConfiguredMontageInstance = INDEX_NONE;
 288: 	virtual void ClientActivateAbilitySucceedWithEventData_Implementation(
 289: 		FGameplayAbilitySpecHandle Handle, FPredictionKey PredictionKey, FGameplayEventData TriggerEventData) override;
 294: 	virtual void ClientActivateAbilityFailed_Implementation(FGameplayAbilitySpecHandle Handle, int16 PredictionKey) override;
 296: 	TArray<int16> RecentComboPredictionKeys;
 298: 	UPROPERTY(Transient) TMap<uint64, FGameplayEventData> DeferredComboPayloads;
 300: 	uint64 NextDeferredComboRequest = 0;
 302: 	bool bProcessingDeferredComboRequest = false;
 304: 	float DeferredComboWaitSeconds = 0.f;
 306: 	TMap<int16, double> PredictedComboStartTimes;
 313: 	UFUNCTION(Client, Reliable)
 314: 	void ClientConfirmComboTiming(AActor* Avatar, FGameplayAbilitySpecHandle Handle, int16 Key, float DeferredSeconds);
 324: 	void ProcessDeferredComboRequest(uint64 RequestId, TWeakObjectPtr<AActor> Avatar, FGameplayAbilitySpecHandle Handle,
 325: 	                                 bool InputPressed, FPredictionKey PredictionKey, double ReceivedAt, double ExpiresAt);
 333: 	virtual void InternalServerTryActivateAbility(FGameplayAbilitySpecHandle Handle, bool InputPressed,
 334: 	                                              const FPredictionKey& PredictionKey,
 335: 	                                              const FGameplayEventData* TriggerEventData) override;
 339: 	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;
 341: 	UPROPERTY(Replicated)
 342: 	TArray<FHodgeGrantedAbilityDefinition> GrantedDefinitions;
 346: 	void TryActivateAbilitiesOnSpawn();
 352: 	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
 358: 	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
 365: 	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
 373: 	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,
 374: 	                                 const FGameplayTagContainer& FailureReason) override;
 382: 	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,
 383: 	                                bool bWasCancelled) override;
 394: 	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,
 395: 	                                            UGameplayAbility* RequestingAbility, bool bEnableBlockTags,
 396: 	                                            const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags,
 397: 	                                            const FGameplayTagContainer& CancelTags) override;
 405: 	virtual void HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags,
 406: 	                                              UGameplayAbility* RequestingAbility, bool bCanBeCanceled) override;
 414: 	UFUNCTION(Client, Unreliable)
 415: 	void ClientNotifyAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);
 422: 	void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);
 424: protected:
 428: 	UPROPERTY()
 429: 	TObjectPtr<UHodgeAbilityTagRelationshipMapping> TagRelationshipMapping;
 433: 	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
 437: 	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
 441: 	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
 445: 	int32 ActivationGroupCounts[(uint8)EHodgeAbilityActivationGroup::MAX];
 446: private:
 448: 	TArray<FGameplayAbilitySpecHandleAndPredictionKey> RetiredLockOnCommands;
 449: 	TArray<FGameplayAbilitySpecHandleAndPredictionKey> RetiredAttackFacingCommands;
 450: };
```

## HodgeAbilitySystemGlobals.h

分配 FHodgeGameplayEffectContext 的 GAS 全局类；已加入项目配置。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemGlobals.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemGlobals.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   9: #include "AbilitySystemGlobals.h"
  11: #include "HodgeAbilitySystemGlobals.generated.h"
  14: class UObject;
  17: struct FGameplayEffectContext;
  21: UCLASS(Config=Game)
  22: class UHodgeAbilitySystemGlobals : public UAbilitySystemGlobals
  23: {
  24: 	GENERATED_UCLASS_BODY()
  32: 	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
  35: };
```

## HodgeAbilityTagRelationshipMapping.h

数据驱动的能力阻断、取消与激活条件关系。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "Engine/DataAsset.h"
   9: #include "GameplayTagContainer.h"
  11: #include "HodgeAbilityTagRelationshipMapping.generated.h"
  13: class UObject;
  21: USTRUCT(BlueprintType)
  22: struct FHodgeAbilityTagRelationship
  23: {
  24: 	GENERATED_BODY()
  27: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability, meta = (Categories = "Ability"))
  28: 	FGameplayTag AbilityTag;
  31: 	UPROPERTY(EditAnywhere, Category = Ability)
  32: 	FGameplayTagContainer AbilityTagsToBlock;
  35: 	UPROPERTY(EditAnywhere, Category = Ability)
  36: 	FGameplayTagContainer AbilityTagsToCancel;
  38: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
  39: 	FGameplayTagContainer AbilityTagsToCancelExceptions;
  41: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
  42: 	bool bForceCancel = false;
  45: 	UPROPERTY(EditAnywhere, Category = Ability)
  46: 	FGameplayTagContainer ActivationRequiredTags;
  49: 	UPROPERTY(EditAnywhere, Category = Ability)
  50: 	FGameplayTagContainer ActivationBlockedTags;
  51: };
  60: UCLASS(BlueprintType)
  61: class HODGEPODGE_API UHodgeAbilityTagRelationshipMapping : public UDataAsset
  62: {
  63: 	GENERATED_BODY()
  65: private:
  67: 	UPROPERTY(EditAnywhere, Category = Ability, meta=(TitleProperty="AbilityTag"))
  68: 	TArray<FHodgeAbilityTagRelationship> AbilityTagRelationships;
  70: public:
  71: 	friend struct FHodgeRelationshipTestAccess;
  76: 	UFUNCTION(BlueprintPure, Category="Hodge|Ability Relationships")
  77: 	static bool IsCancellationProtected(const FGameplayTagContainer& TargetTags);
  82: 	void GetAbilityCancelExceptions(const FGameplayTagContainer& SourceTags, FGameplayTagContainer& OutExceptions) const;
  88: 	UFUNCTION(BlueprintPure, Category="Hodge|Ability Relationships")
  89: 	bool IsAbilityCancelledByTags(const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags) const;
  95: 	bool IsForcedCancellation(const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags) const;
 102: 	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock,
 103: 	                                    FGameplayTagContainer* OutTagsToCancel) const;
 111: 	void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags,
 112: 	                                         FGameplayTagContainer* OutActivationRequired,
 113: 	                                         FGameplayTagContainer* OutActivationBlocked) const;
 121: 	bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;
 122: };
```

## HodgeGameplayCueManager.h

项目 Cue 管理类已配置；启动预加载及 Feature Cue 观察者生命周期仍未完整接通。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayCueManager.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayCueManager.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "GameplayCueManager.h"
  10: #include "HodgeGameplayCueManager.generated.h"
  12: class FString;
  13: class UClass;
  14: class UObject;
  15: class UWorld;
  16: struct FObjectKey;
  24: UCLASS()
  25: class UHodgeGameplayCueManager : public UGameplayCueManager
  26: {
  27: 	GENERATED_BODY()
  29: public:
  34: 	UHodgeGameplayCueManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  40: 	static UHodgeGameplayCueManager* Get();
  46: 	virtual void OnCreated() override;
  52: 	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override;
  58: 	virtual bool ShouldSyncLoadMissingGameplayCues() const override;
  64: 	virtual bool ShouldAsyncLoadMissingGameplayCues() const override;
  72: 	static void DumpGameplayCues(const TArray<FString>& Args);
  76: 	void LoadAlwaysLoadedCues();
  80: 	void RefreshGameplayCuePrimaryAsset();
  82: private:
  87: 	void OnGameplayTagLoaded(const FGameplayTag& Tag);
  91: 	void HandlePostGarbageCollect();
  95: 	void ProcessLoadedTags();
 102: 	void ProcessTagToPreload(const FGameplayTag& Tag, UObject* OwningObject);
 110: 	void OnPreloadCueComplete(FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject, bool bAlwaysLoadedCue);
 117: 	void RegisterPreloadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject);
 123: 	void HandlePostLoadMap(UWorld* NewWorld);
 127: 	void UpdateDelayLoadDelegateListeners();
 133: 	bool ShouldDelayLoadGameplayCues() const;
 135: private:
 140: 	struct FLoadedGameplayTagToProcessData
 141: 	{
 143: 		FGameplayTag Tag;
 146: 		TWeakObjectPtr<UObject> WeakOwner;
 150: 		FLoadedGameplayTagToProcessData()
 151: 		{
 152: 		}
 159: 		FLoadedGameplayTagToProcessData(const FGameplayTag& InTag,
 160: 		                                const TWeakObjectPtr<UObject>& InWeakOwner) : Tag(InTag), WeakOwner(InWeakOwner)
 161: 		{
 162: 		}
 163: 	};
 165: private:
 167: 	UPROPERTY(transient)
 168: 	TSet<TObjectPtr<UClass>> PreloadedCues;
 171: 	TMap<FObjectKey, TSet<FObjectKey>> PreloadedCueReferencers;
 175: 	UPROPERTY(transient)
 176: 	TSet<TObjectPtr<UClass>> AlwaysLoadedCues;
 179: 	TArray<FLoadedGameplayTagToProcessData> LoadedGameplayTagsToProcess;
 182: 	FCriticalSection LoadedGameplayTagsToProcessCS;
 185: 	bool bProcessLoadedTagsAfterGC = false;
 186: };
```

## HodgeGameplayEffectContext.h

项目 GE 上下文与序列化扩展，已有 HodgeAbilitySystemGlobals 分配配套。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)

项目内直接 include（不是运行调用关系）：[Interface/HodgeAbilitySourceInterface.h](../../../Source/Hodgepodge/Public/Interface/HodgeAbilitySourceInterface.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "GameplayEffectTypes.h"
   9: #include "Interface/HodgeAbilitySourceInterface.h"
  11: #include "HodgeGameplayEffectContext.generated.h"
  13: class AActor;
  14: class FArchive;
  16: class UObject;
  17: class UPhysicalMaterial;
  29: USTRUCT()
  30: struct FHodgeGameplayEffectContext : public FGameplayEffectContext
  31: {
  32: 	GENERATED_BODY()
  36: 	FHodgeGameplayEffectContext()
  37: 		: FGameplayEffectContext()
  38: 	{
  39: 	}
  46: 	FHodgeGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
  47: 		: FGameplayEffectContext(InInstigator, InEffectCauser)
  48: 	{
  49: 	}
  56: 	static HODGEPODGE_API FHodgeGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);
  63: 	void SetAbilitySource(const IHodgeAbilitySourceInterface* InObject, float InSourceLevel);
  69: 	const IHodgeAbilitySourceInterface* GetAbilitySource() const;
  75: 	virtual FGameplayEffectContext* Duplicate() const override
  76: 	{
  77: 		FHodgeGameplayEffectContext* NewContext = new FHodgeGameplayEffectContext();
  78: 		*NewContext = *this;
  79: 		if (GetHitResult())
  80: 		{
  82: 			NewContext->AddHitResult(*GetHitResult(), true);
  83: 		}
  84: 		return NewContext;
  85: 	}
  91: 	virtual UScriptStruct* GetScriptStruct() const override
  92: 	{
  93: 		return FHodgeGameplayEffectContext::StaticStruct();
  94: 	}
 103: 	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
 109: 	const UPhysicalMaterial* GetPhysicalMaterial() const;
 111: public:
 113: 	UPROPERTY()
 114: 	int32 CartridgeID = -1;
 116: 	UPROPERTY() FGuid HitSettlementId;
 118: protected:
 121: 	UPROPERTY()
 122: 	TWeakObjectPtr<const UObject> AbilitySourceObject;
 123: };
 126: template <>
 128: struct TStructOpsTypeTraits<FHodgeGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FHodgeGameplayEffectContext>
 129: {
 130: 	enum
 131: 	{
 133: 		WithNetSerializer = true,
 136: 		WithCopy = true
 137: 	};
 138: };
```

## HodgeGameplayTags.h

原生 GameplayTag 注册和移动状态标签映射；含攻击时间轴依赖的 Status.Attack.*（阶段 + 取消窗口 .Cancel.*）与 GameplayEvent.Attack.*。标签存在不等于对应玩法实现。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "NativeGameplayTags.h"
   8: namespace HodgeGameplayTags
   9: {
  15: 	HODGEPODGE_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
  20: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
  21: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
  22: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
  23: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
  24: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
  25: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
  26: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);
  31: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);
  36: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Weapon_NoFiring);
  41: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dash_Duration_Message);
  42: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Grenade_Duration_Message);
  43: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Interaction_Activate);
  44: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Interaction_Duration_Message);
  45: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Respawn_Completed_Message);
  46: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Respawn_Duration_Message);
  51: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type);
  52: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action);
  53: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_ADS);
  54: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Dash);
  55: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Drop);
  56: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Emote);
  57: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Grenade);
  58: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Jump);
  59: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Melee);
  60: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Reload);
  61: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_WeaponFire);
  62: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Info);
  63: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Info_ShowLeaderboard);
  64: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive);
  65: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_AutoReload);
  66: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_AutoRespawn);
  67: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_ChangeQuickbarSlot);
  68: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange);
  69: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange_Death);
  70: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange_Spawning);
  75: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic);
  76: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_AnimationStyle_Feminine);
  77: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_AnimationStyle_Masculine);
  78: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_BodyStyle_Medium);
  83: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Zone);
  84: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Zone_WeakSpot);
  89: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_DamageTaken);
  90: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Dash);
  91: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Dash_Cooldown);
  92: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Death);
  93: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Heal);
  94: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Melee_Cooldown);
  99: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_Burst);
 100: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_BurstLatent);
 101: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_Looping);
 106: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Grenade_Cooldown);
 107: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Grenade_Detonate);
 108: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Melee_Hit);
 109: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Melee_Impact);
 110: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Pistol_Fire);
 111: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Rifle_Fire);
 112: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Rifle_Impact);
 113: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Shotgun_Fire);
 118: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_World_Launcher_Activate);
 119: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_World_Teleporter_Activate);
 124: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageTrait_Instant);
 125: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageTrait_Periodic);
 126: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Basic);
 127: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Grenade);
 128: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Melee);
 129: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Pistol);
 130: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Rifle);
 131: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Shotgun);
 132: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Heal_Instant);
 133: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Heal_Periodic);
 138: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_MeleeHit);
 139: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
 140: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Reset);
 141: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RequestReset);
 146: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack);
 147: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_Test);
 148: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_Completed);
 149: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_Interrupted);
 154: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameSettings_Action_EditBrightness);
 155: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameSettings_Action_EditSafeZone);
 160: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_ExtraEquipment);
 161: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_InfrequentAbilities);
 162: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_LeftSideTouchInputs);
 163: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_LeftSideTouchRegion);
 164: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RespawnTimer);
 165: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RightSideTouchInputs);
 166: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RightSideTouchRegion);
 171: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Dash);
 172: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Heal);
 173: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Melee);
 174: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Quickslot_Drop);
 175: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Jump);
 180: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_ADS);
 181: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Fire);
 182: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_FireAuto);
 183: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Grenade);
 184: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Reload);
 189: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
 190: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
 191: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
 192: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Stick);
 193: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
 194: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_AutoRun);
 202: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Sprint);
 203: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Walk);
 204: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Aim);
 205: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ragdoll);
 206: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Roll);
 207: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_RotationMode);
 208: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_ViewMode);
 209: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_SwitchShoulder);
 214: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
 215: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
 216: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
 217: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);
 222: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_AddNotification_Message);
 223: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_Assist_Message);
 224: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_Damage_Taken_Message);
 225: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_Elimination_Message);
 226: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_HUD_PlayerHUD);
 227: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_HUD_TempTopWidgets);
 228: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_Inventory_Message_StackChanged);
 229: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_Player);
 230: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_QuickBar_Message_ActiveIndexChanged);
 231: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_QuickBar_Message_SlotsChanged);
 232: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_ShooterGame_Accolade);
 233: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hodge_Weapon_SteadyAimingCamera);
 238: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_BinauralSettingControlledByOS);
 239: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_CanExitApplication);
 240: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_PrimarlyController);
 241: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_HasStrictControllerPairing);
 242: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_PrimarlyTouchScreen);
 243: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_SupportsMouseAndKeyboard);
 244: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_HardwareCursor);
 245: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsBackgroundAudio);
 246: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsChangingAudioOutputDevice);
 247: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsWindowedMode);
 248: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_SupportsGamepad);
 249: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_SupportsTriggerHaptics);
 250: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_NeedsBrightnessAdjustment);
 251: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_ReplaySupport);
 252: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_RequiresStrictControllerMapping);
 253: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SingleOnlineUser);
 258: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SystemMessage_Display);
 259: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SystemMessage_Error);
 260: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SystemMessage_Error_InitializeLocalPlayerFailed);
 261: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SystemMessage_Warning);
 266: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ShooterGame_GamePhase_MatchBeginCountdown);
 271: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_SpawningIn);
 272: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
 273: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AutoRunning);
 274: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
 275: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
 276: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);
 279: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack);
 280: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Rotation_Locked);
 281: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Weapon_Hand);
 282: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Windup);
 283: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Active);
 284: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Recovery);
 287: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_HitCheck_Weapon);
 288: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_HitCheck_Body);
 289: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_HitCheck_HitBox);
 290: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combat_Source_Weapon_MainHand);
 291: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combat_Source_Body_Origin);
 292: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combat_Source_Body_RightFoot);
 293: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combat_Source_Hitbox_Chest);
 294: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Damage_AllowFriendlyFire);
 295: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_DamageMultiplier);
 296: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Stat_MaxHealth);
 297: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Stat_BaseDamage);
 298: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_AttributesNotReady);
 306: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Cancel);
 307: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Cancel_Move);
 308: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Attack_Cancel_NextAttack);
 313: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
 314: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);
 319: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_GodMode);
 320: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_UnlimitedHealth);
 325: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Back);
 326: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Escape);
 327: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Game);
 328: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_GameMenu);
 329: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Menu);
 330: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Modal);
 335: 	HODGEPODGE_API extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
 336: 	HODGEPODGE_API extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;
 338: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
 339: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
 340: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
 341: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
 342: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);
 343: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);
 344: };
```

## HodgeGlobalAbilitySystem.h

世界级全局能力/效果授予及 ASC 注册表。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "ActiveGameplayEffectHandle.h"
   9: #include "Subsystems/WorldSubsystem.h"
  10: #include "GameplayAbilitySpecHandle.h"
  11: #include "Templates/SubclassOf.h"
  13: #include "HodgeGlobalAbilitySystem.generated.h"
  15: class UGameplayAbility;
  16: class UGameplayEffect;
  17: class UHodgeAbilitySystemComponent;
  18: class UObject;
  19: struct FActiveGameplayEffectHandle;
  20: struct FFrame;
  21: struct FGameplayAbilitySpecHandle;
  27: USTRUCT()
  28: struct FGlobalAppliedAbilityList
  29: {
  30: 	GENERATED_BODY()
  33: 	UPROPERTY()
  34: 	TMap<TObjectPtr<UHodgeAbilitySystemComponent>, FGameplayAbilitySpecHandle> Handles;
  41: 	void AddToASC(TSubclassOf<UGameplayAbility> Ability, UHodgeAbilitySystemComponent* ASC);
  47: 	void RemoveFromASC(UHodgeAbilitySystemComponent* ASC);
  51: 	void RemoveFromAll();
  52: };
  58: USTRUCT()
  59: struct FGlobalAppliedEffectList
  60: {
  61: 	GENERATED_BODY()
  64: 	UPROPERTY()
  65: 	TMap<TObjectPtr<UHodgeAbilitySystemComponent>, FActiveGameplayEffectHandle> Handles;
  72: 	void AddToASC(TSubclassOf<UGameplayEffect> Effect, UHodgeAbilitySystemComponent* ASC);
  78: 	void RemoveFromASC(UHodgeAbilitySystemComponent* ASC);
  82: 	void RemoveFromAll();
  83: };
  91: UCLASS()
  92: class UHodgeGlobalAbilitySystem : public UWorldSubsystem
  93: {
  94: 	GENERATED_BODY()
  96: public:
  98: 	UHodgeGlobalAbilitySystem();
 104: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
 105: 	void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);
 111: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
 112: 	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);
 118: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
 119: 	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);
 125: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
 126: 	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);
 132: 	void RegisterASC(UHodgeAbilitySystemComponent* ASC);
 138: 	void UnregisterASC(UHodgeAbilitySystemComponent* ASC);
 140: private:
 142: 	UPROPERTY()
 143: 	TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;
 146: 	UPROPERTY()
 147: 	TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;
 150: 	UPROPERTY()
 151: 	TArray<TObjectPtr<UHodgeAbilitySystemComponent>> RegisteredASCs;
 152: };
```

## HodgeLockOnStateEffect.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeLockOnStateEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeLockOnStateEffect.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "GameplayEffect.h"
   4: #include "HodgeLockOnStateEffect.generated.h"
   6: UCLASS()
   7: class HODGEPODGE_API UHodgeLockOnStateEffect : public UGameplayEffect
   8: {
   9: 	GENERATED_BODY()
  10: public:
  11: 	UHodgeLockOnStateEffect();
  12: };
```

## HodgeAttributeCoordinator.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeCoordinator.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeCoordinator.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Stats/HodgeAttributeTypes.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "UObject/Object.h"
   8: #include "AbilitySystem/Stats/HodgeAttributeTypes.h"
   9: #include "HodgeAttributeCoordinator.generated.h"
  11: class AHodgePlayerState;
  12: class UHodgeAbilitySystemComponent;
  13: class UHodgeHealthSet;
  14: class UHodgePawnData;
  15: class UHodgeCharacterStatProfile;
  18: UCLASS()
  19: class HODGEPODGE_API UHodgeAttributeCoordinator : public UObject
  20: {
  21: 	GENERATED_BODY()
  22: public:
  28: 	bool PrepareAvatar(APawn* Avatar, const UHodgePawnData* Data);
  32: 	bool CompleteAvatarInitialization();
  36: 	void DetachAvatar(APawn* Avatar);
  41: 	bool SetCharacterLevel(int32 Level);
  46: 	bool RestoreHealth(float Health);
  50: 	bool BeginEquipmentUpdate();
  54: 	void FinishEquipmentUpdate(bool bSuccess);
  56: 	void ExpectRespawn();
  61: 	bool SetInitialSavedHealth(float Health);
  65: 	bool IsUpdating() const { return bUpdating; }
  69: 	bool IsInitializing() const { return bUpdating && bBootstrap; }
  74: 	bool IsBoundTo(const APawn* Avatar) const;
  79: 	bool IsReadyFor(const APawn* Avatar) const;
  84: 	FHodgeOwnedEquipmentState GetOrCreateDefaultEquipment(TSubclassOf<UHodgeEquipmentDefinition> Definition);
  90: 	void RememberEquipment(FGuid Id, TSubclassOf<UHodgeEquipmentDefinition> Definition, int32 Level);
  91: private:
  95: 	AHodgePlayerState* GetPlayerState() const;
  99: 	bool IsCurrentAvatar() const;
 103: 	bool ValidateCoreSets() const;
 108: 	bool ApplyCharacterBase(int32 Level);
 113: 	bool BeginUpdate(EHodgeAttributeUpdateReason Reason);
 118: 	bool CommitUpdate(bool bSuccess);
 122: 	void PublishReady(bool bReady);
 124: 	UPROPERTY(Transient) TWeakObjectPtr<APawn> BoundAvatar;
 126: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> AbilitySystem;
 128: 	UPROPERTY(Transient) TObjectPtr<const UHodgePawnData> PawnData;
 130: 	UPROPERTY(Transient) TObjectPtr<const UHodgeCharacterStatProfile> Profile;
 132: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeHealthSet> HealthSet;
 134: 	bool bUpdating = false;
 136: 	bool bBootstrap = false;
 138: 	bool bWasReady = false;
 140: 	bool bExpectRespawn = false;
 142: 	bool bHasSavedHealth = false;
 144: 	float SavedHealth = 0.f;
 146: 	float OldHealth = 0.f;
 148: 	float OldMaxHealth = 1.f;
 150: 	float OldBaseHealth = 1.f;
 152: 	float OldBaseDamage = 0.f;
 154: 	int32 LifeGeneration = 0;
 156: 	int32 AppliedProfileVersion = 0;
 158: 	EHodgeAttributeUpdateReason UpdateReason = EHodgeAttributeUpdateReason::FirstSpawn;
 159: };
```

## HodgeAttributeTypes.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeTypes.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "HodgeAttributeTypes.generated.h"
   9: class APawn;
  10: class UHodgeEquipmentDefinition;
  13: UENUM(BlueprintType)
  14: enum class EHodgeAttributeUpdateReason : uint8
  15: {
  17: 	FirstSpawn,
  19: 	Respawn,
  21: 	Rebind,
  23: 	LevelUp,
  25: 	EquipmentChange,
  27: 	Restore
  28: };
  31: USTRUCT(BlueprintType)
  32: struct FHodgeCharacterProgression
  33: {
  34: 	GENERATED_BODY()
  36: 	UPROPERTY(BlueprintReadOnly) FGuid CharacterId;
  38: 	UPROPERTY(BlueprintReadOnly) int32 Level = 1;
  40: 	UPROPERTY(BlueprintReadOnly) int32 Revision = 0;
  41: };
  44: USTRUCT(BlueprintType)
  45: struct FHodgeAttributeReadyState
  46: {
  47: 	GENERATED_BODY()
  49: 	UPROPERTY(BlueprintReadOnly) TObjectPtr<APawn> Avatar = nullptr;
  51: 	UPROPERTY(BlueprintReadOnly) int32 LifeGeneration = 0;
  53: 	UPROPERTY(BlueprintReadOnly) int32 Revision = 0;
  55: 	UPROPERTY(BlueprintReadOnly) bool bReady = false;
  56: };
  59: USTRUCT(BlueprintType)
  60: struct FHodgeOwnedEquipmentState
  61: {
  62: 	GENERATED_BODY()
  64: 	UPROPERTY(BlueprintReadOnly) FGuid InstanceId;
  66: 	UPROPERTY(BlueprintReadOnly) TSubclassOf<UHodgeEquipmentDefinition> Definition;
  68: 	UPROPERTY(BlueprintReadOnly) int32 Level = 1;
  69: };
```

## HodgeCharacterBaseStatEffect.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeCharacterBaseStatEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeCharacterBaseStatEffect.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "GameplayEffect.h"
   7: #include "HodgeCharacterBaseStatEffect.generated.h"
  10: UCLASS()
  11: class HODGEPODGE_API UHodgeCharacterBaseStatEffect : public UGameplayEffect
  12: {
  13: 	GENERATED_BODY()
  14: public:
  16: 	UHodgeCharacterBaseStatEffect();
  17: };
```

## HodgeEquipmentStatEffect.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeEquipmentStatEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeEquipmentStatEffect.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "GameplayEffect.h"
   7: #include "HodgeEquipmentStatEffect.generated.h"
  10: UCLASS()
  11: class HODGEPODGE_API UHodgeEquipmentStatEffect : public UGameplayEffect
  12: {
  13: 	GENERATED_BODY()
  14: public:
  16: 	UHodgeEquipmentStatEffect();
  17: };
```
