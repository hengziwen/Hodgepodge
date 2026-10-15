# 源码文件与有效定义索引

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

索引排除第三方插件和构建产物；行号为生成时位置。注释已排除，但没有求值预处理条件。

## HodgeAbilityEditor.Build.cs

[Source/HodgeAbilityEditor/HodgeAbilityEditor.Build.cs](../../../Source/HodgeAbilityEditor/HodgeAbilityEditor.Build.cs)

模块定义或基础代码；请查看对应文件。

## HodgeAbilityEditorModule.cpp

[Source/HodgeAbilityEditor/Private/HodgeAbilityEditorModule.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeAbilityEditorModule.cpp)

模块定义或基础代码；请查看对应文件。

## HodgeAnimationAuthoringLibrary.cpp

[Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.cpp)

Editor 动画文本导入、属性/引用修正、编译诊断与实验 PIE 配置。

- `UHodgeAnimationAuthoringLibrary::ConfigureWingBoxHit` — L104
- `UHodgeAnimationAuthoringLibrary::ConfigureWingActionWindow` — L324
- `UHodgeAnimationAuthoringLibrary::ConfigureWingMontageGraph` — L353
- `UHodgeAnimationAuthoringLibrary::ConfigureWingMontageAsset` — L379
- `UHodgeAnimationAuthoringLibrary::ConfigureHavocMontage` — L406
- `UHodgeAnimationAuthoringLibrary::ConfigureHavocCastCommit` — L457
- `UHodgeAnimationAuthoringLibrary::ConfigureLiberationMontage` — L472
- `UHodgeAnimationAuthoringLibrary::ConfigureWingAttackPreview` — L512
- `UHodgeAnimationAuthoringLibrary::ConfigureWingAnimation` — L549
- `UHodgeAnimationAuthoringLibrary::ConfigureWingExperience` — L598
- `UHodgeAnimationAuthoringLibrary::ConfigureWingFinisher` — L624
- `UHodgeAnimationAuthoringLibrary::ImportAnimationBlueprintText` — L669
- `UHodgeAnimationAuthoringLibrary::SetAnimationDefault` — L737
- `UHodgeAnimationAuthoringLibrary::SetAnimationNodeProperty` — L747
- `UHodgeAnimationAuthoringLibrary::RemapAnimationReferences` — L759
- `UHodgeAnimationAuthoringLibrary::CompileAnimationBlueprint` — L783
- `UHodgeAnimationAuthoringLibrary::GetSkeletonBoneNames` — L807
- `UHodgeAnimationAuthoringLibrary::SetSkeletonSlotGroup` — L818
- `UHodgeAnimationAuthoringLibrary::GetSkeletonSlotGroup` — L830
- `UHodgeAnimationAuthoringLibrary::SetHeroReactionMontageSlot` — L836
- `UHodgeAnimationAuthoringLibrary::SetHeroReactionSkeleton` — L848
- `UHodgeAnimationAuthoringLibrary::ConfigureHeroHitStunDuration` — L859
- `UHodgeAnimationAuthoringLibrary::ConfigureHeroHitReactionGraph` — L877
- `UHodgeAnimationAuthoringLibrary::ConfigureHeroDashFootIK` — L939
- `UHodgeAnimationAuthoringLibrary::ConfigureHeroMovementMontageSync` — L967
- `UHodgeAnimationAuthoringLibrary::ConfigureHeroSprintSync` — L976
- `UHodgeAnimationAuthoringLibrary::ConfigureHeroFacingModes` — L1016
- `UHodgeAnimationAuthoringLibrary::ConfigureHeroSprintAnimations` — L1100
- `UHodgeAnimationAuthoringLibrary::ConfigureHeroSprintPivotTiming` — L1175
- `UHodgeAnimationAuthoringLibrary::ConfigureAnimationLabPIE` — L1245
- `UHodgeAnimationAuthoringLibrary::ConfigureAnimationLabGameMode` — L1256
- `UHodgeAnimationAuthoringLibrary::SetEnemyMeshSkeleton` — L1308
- `UHodgeAnimationAuthoringLibrary::ConfigureEnemyBehaviorTree` — L1315
- `UHodgeAnimationAuthoringLibrary::ConfigureEnemyAnimation` — L1382
- `UHodgeAnimationAuthoringLibrary::ConfigureEnemyAnimationAsset` — L1426
- `UHodgeAnimationAuthoringLibrary::ConfigureEnemyNavigationBounds` — L1442
- `UHodgeAnimationAuthoringLibrary::ConfigureEnemyExperienceComponents` — L1463
- `UHodgeAnimationAuthoringLibrary::ConfigureAttackFacingWarp` — L1486

## HodgeAnimationAuthoringLibrary.h

[Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.h](../../../Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.h)

Editor 动画文本导入、属性/引用修正、编译诊断与实验 PIE 配置。

## HodgeCombatValidationLibrary.cpp

[Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeCombatValidationLibrary::QueueDarkFlowDelta` — L42
- `UHodgeCombatValidationLibrary::InspectDarkFlow` — L56
- `UHodgeCombatValidationLibrary::QueueDarkSurgeDispel` — L63
- `UHodgeCombatValidationLibrary::QueueResonanceEnergyDelta` — L81
- `UHodgeCombatValidationLibrary::InspectResonanceEnergy` — L96
- `UHodgeCombatValidationLibrary::QueueCinematicSourceProbe` — L103
- `UHodgeCombatValidationLibrary::QueueCombatInput` — L124
- `UHodgeCombatValidationLibrary::QueueGameplayEvent` — L137
- `UHodgeCombatValidationLibrary::InspectFacingSequences` — L153
- `UHodgeCombatValidationLibrary::InspectPivotTimes` — L176
- `UHodgeCombatValidationLibrary::InspectLocomotionSync` — L201
- `UHodgeCombatValidationLibrary::SetFacingValidationControllerPermission` — L228
- `UHodgeCombatValidationLibrary::QueueFacingMode` — L241
- `UHodgeCombatValidationLibrary::QueueActionFacing` — L257
- `UHodgeCombatValidationLibrary::QueueAbilityAction` — L275
- `UHodgeCombatValidationLibrary::AddHitNotify` — L347
- `UHodgeCombatValidationLibrary::AddStateNotify` — L362
- `UHodgeCombatValidationLibrary::AddWeaponHandNotify` — L372
- `UHodgeCombatValidationLibrary::InspectAbilityWindows` — L379
- `UHodgeCombatValidationLibrary::InspectHitSessions` — L384
- `UHodgeCombatValidationLibrary::InspectPoseLeases` — L391
- `UHodgeCombatValidationLibrary::InspectMontageState` — L399
- `UHodgeCombatValidationLibrary::ConfigureValidationPIE` — L425
- `UHodgeCombatValidationLibrary::ConfigureValidationSections` — L436
- `UHodgeCombatValidationLibrary::NormalizeCombatNotifies` — L455
- `UHodgeCombatValidationLibrary::ConfigureHitReactionValidationMontage` — L472
- `UHodgeCombatValidationLibrary::SpawnHitReactionValidationActor` — L484
- `UHodgeCombatValidationLibrary::InspectEnemyBrain` — L502
- `UHodgeCombatValidationLibrary::QueueCombatAction` — L517
- `UHodgeCombatValidationLibrary::QueueAttackFacingProbe` — L532

## HodgeCombatValidationLibrary.h

[Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.h](../../../Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.h)

模块定义或基础代码；请查看对应文件。

## HodgeUIAuthoringLibrary.cpp

[Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.cpp)

仅 Editor 的 Main UI 资产制作、原生 PIE 命令调度、Slate 截图与只读检查入口。

- `UHodgeUIAuthoringLibrary::CreateFoundationAssets` — L166
- `UHodgeUIAuthoringLibrary::InspectPlayerUI` — L233
- `UHodgeUIAuthoringLibrary::QueueUIAction` — L375
- `UHodgeUIAuthoringLibrary::MigrateDesignerAssets` — L1200
- `UHodgeUIAuthoringLibrary::ConfigureResonanceHUD` — L1296
- `UHodgeUIAuthoringLibrary::ConfigureAbilityIconHUD` — L1310
- `UHodgeUIAuthoringLibrary::ConfigureAbilityIconDesigner` — L1341
- `UHodgeUIAuthoringLibrary::ConfigureReferenceHUD` — L1377
- `UHodgeUIAuthoringLibrary::InspectDesignerAssets` — L1480
- `UHodgeUIAuthoringLibrary::ConfigureDesignerPreviews` — L1496
- `UHodgeUIAuthoringLibrary::ConfigureLockOnHUD` — L1547
- `UHodgeUIAuthoringLibrary::ConfigureLockOnIndicator` — L1561
- `UHodgeUIAuthoringLibrary::ConfigureDarkFlowHUD` — L1580

## HodgeUIAuthoringLibrary.h

[Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.h](../../../Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.h)

仅 Editor 的 Main UI 资产制作、原生 PIE 命令调度、Slate 截图与只读检查入口。

## Hodgepodge.Build.cs

[Source/Hodgepodge/Hodgepodge.Build.cs](../../../Source/Hodgepodge/Hodgepodge.Build.cs)

模块定义或基础代码；请查看对应文件。

## Hodgepodge.cpp

[Source/Hodgepodge/Hodgepodge.cpp](../../../Source/Hodgepodge/Hodgepodge.cpp)

模块定义或基础代码；请查看对应文件。

## Hodgepodge.h

[Source/Hodgepodge/Hodgepodge.h](../../../Source/Hodgepodge/Hodgepodge.h)

模块定义或基础代码；请查看对应文件。

## HodgeAbilityCost_ResonanceEnergy.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeAbilityCost_ResonanceEnergy::UHodgeAbilityCost_ResonanceEnergy` — L9
- `UHodgeAbilityCost_ResonanceEnergy::ValidateEffect` — L14
- `UHodgeAbilityCost_ResonanceEnergy::CheckCost` — L23
- `UHodgeAbilityCost_ResonanceEnergy::ApplyCost` — L33

## HodgeAbilityTask_WaitHitResults.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeHitResultsTickFunction::ExecuteTick` — L15
- `FHodgeHitResultsTickFunction::DiagnosticMessage` — L23
- `UHodgeAbilityTask_WaitHitResults::WaitHitResults` — L29
- `UHodgeAbilityTask_WaitHitResults::IsRunningForExecution` — L42
- `UHodgeAbilityTask_WaitHitResults::Activate` — L55
- `UHodgeAbilityTask_WaitHitResults::CreateWindow` — L75
- `UHodgeAbilityTask_WaitHitResults::IsBatchCurrent` — L98
- `UHodgeAbilityTask_WaitHitResults::PrepareSourcePose` — L106
- `UHodgeAbilityTask_WaitHitResults::SampleWindow` — L119
- `UHodgeAbilityTask_WaitHitResults::CloseWindow` — L150
- `UHodgeAbilityTask_WaitHitResults::TickDetection` — L163
- `UHodgeAbilityTask_WaitHitResults::UpdateTickState` — L190
- `UHodgeAbilityTask_WaitHitResults::Pause` — L198
- `UHodgeAbilityTask_WaitHitResults::Resume` — L205
- `UHodgeAbilityTask_WaitHitResults::ReleaseSessions` — L224
- `UHodgeAbilityTask_WaitHitResults::OnDestroy` — L238
- `UHodgeAbilityTask_WaitHitResults::BeginDestroy` — L245

## HodgeAbilityTask_WaitMoveCancel.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.cpp)

合流取消窗口与本地移动意图，广播 OnMoveCancel 后结束；已有取消验证，当前 Combat 主链与旧 BasicAttack 消费方需区分。

- `UHodgeAbilityTask_WaitMoveCancel::UHodgeAbilityTask_WaitMoveCancel` — L18
- `UHodgeAbilityTask_WaitMoveCancel::TickTask` — L25
- `UHodgeAbilityTask_WaitMoveCancel::WaitMoveCancel` — L32
- `UHodgeAbilityTask_WaitMoveCancel::Activate` — L42
- `UHodgeAbilityTask_WaitMoveCancel::OnDestroy` — L79
- `UHodgeAbilityTask_WaitMoveCancel::Evaluate` — L105
- `UHodgeAbilityTask_WaitMoveCancel::HandleMoveIntentChanged` — L150
- `UHodgeAbilityTask_WaitMoveCancel::HandleWindowTagChanged` — L157

## HodgeGameplayAbility.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp)

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。（PreloadPrimaryAssetsOnGrant 已随未提交改动回退，当前不存在。）

- `UHodgeGameplayAbility::UHodgeGameplayAbility` — L45
- `UHodgeGameplayAbility::GetHodgeAbilitySystemComponentFromActorInfo` — L74
- `UHodgeGameplayAbility::GetHodgePlayerControllerFromActorInfo` — L83
- `UHodgeGameplayAbility::GetControllerFromActorInfo` — L90
- `UHodgeGameplayAbility::GetHodgeCharacterFromActorInfo` — L128
- `UHodgeGameplayAbility::GetHeroComponentFromActorInfo` — L135
- `UHodgeGameplayAbility::NativeOnAbilityFailedToActivate` — L142
- `UHodgeGameplayAbility::CanActivateAbility` — L195
- `UHodgeGameplayAbility::SetCanBeCanceled` — L256
- `UHodgeGameplayAbility::OnGiveAbility` — L275
- `UHodgeGameplayAbility::OnRemoveAbility` — L288
- `UHodgeGameplayAbility::ActivateAbility` — L299
- `UHodgeGameplayAbility::EndAbility` — L309
- `UHodgeGameplayAbility::CheckCost` — L328
- `UHodgeGameplayAbility::ApplyCost` — L358
- `UHodgeGameplayAbility::MakeEffectContext` — L441
- `UHodgeGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec` — L488
- `UHodgeGameplayAbility::DoesAbilitySatisfyTagRequirements` — L507
- `UHodgeGameplayAbility::OnPawnAvatarSet` — L663
- `UHodgeGameplayAbility::GetAbilitySource` — L670
- `UHodgeGameplayAbility::TryActivateAbilityOnSpawn` — L696
- `UHodgeGameplayAbility::CanChangeActivationGroup` — L738
- `UHodgeGameplayAbility::ChangeActivationGroup` — L776
- `UHodgeGameplayAbility::SetCameraMode` — L809
- `UHodgeGameplayAbility::ClearCameraMode` — L823

## HodgeGameplayAbility_Dash.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeGameplayAbility_Dash::UHodgeGameplayAbility_Dash` — L22
- `UHodgeGameplayAbility_Dash::IsExecutionMovementModeValid` — L25
- `UHodgeGameplayAbility_Dash::CheckCost` — L30
- `UHodgeGameplayAbility_Dash::BeginMovement` — L37
- `UHodgeGameplayAbility_Dash::OnCommitRequested` — L61
- `UHodgeGameplayAbility_Dash::CommitDash` — L71
- `UHodgeGameplayAbility_Dash::UpdateMovement` — L95
- `UHodgeGameplayAbility_Dash::CompleteHandoff` — L155
- `UHodgeGameplayAbility_Dash::OnMontageInterrupted` — L166
- `UHodgeGameplayAbility_Dash::PrepareLocomotionExit` — L168
- `UHodgeGameplayAbility_Dash::OnMontageBlendOut` — L182
- `UHodgeGameplayAbility_Dash::OnMontageCompleted` — L192
- `UHodgeGameplayAbility_Dash::EndAbility` — L194

## HodgeGameplayAbility_Death.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Death.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Death.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeGameplayAbility_Death::UHodgeGameplayAbility_Death` — L17
- `UHodgeGameplayAbility_Death::ActivateAbility` — L38
- `UHodgeGameplayAbility_Death::EndAbility` — L73
- `UHodgeGameplayAbility_Death::StartDeath` — L88
- `UHodgeGameplayAbility_Death::FinishDeath` — L101

## HodgeGameplayAbility_Definition.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp)

执行单段 Definition、本次 Montage 时钟 Timeline 与手持窗口，按执行清理；Melee 子类扩展命中。

- `UHodgeGameplayAbility_Definition::UHodgeGameplayAbility_Definition` — L28
- `UHodgeGameplayAbility_Definition::GetDefinition` — L39
- `UHodgeGameplayAbility_Definition::IsComboCoordinated` — L47
- `UHodgeGameplayAbility_Definition::ActivateConfirmedDefinition` — L55
- `UHodgeGameplayAbility_Definition::CanActivateAbility` — L72
- `UHodgeGameplayAbility_Definition::ActivateAbility` — L94
- `UHodgeGameplayAbility_Definition::StartExecutionMontage` — L142
- `UHodgeGameplayAbility_Definition::BuildAttackFacingData` — L173
- `UHodgeGameplayAbility_Definition::InitializeAttackFacing` — L202
- `UHodgeGameplayAbility_Definition::OnAttackFacingData` — L227
- `UHodgeGameplayAbility_Definition::ApplyAttackFacing` — L240
- `UHodgeGameplayAbility_Definition::OnAttackFacingTimeout` — L262
- `UHodgeGameplayAbility_Definition::UpdateAttackFacingLifecycle` — L264
- `UHodgeGameplayAbility_Definition::ReleaseAttackFacing` — L276
- `UHodgeGameplayAbility_Definition::FinishExecution` — L288
- `UHodgeGameplayAbility_Definition::BeginPredictedMoveCancel` — L298
- `UHodgeGameplayAbility_Definition::ResolvePredictedMoveCancel` — L319
- `UHodgeGameplayAbility_Definition::EndAbility` — L358
- `UHodgeGameplayAbility_Definition::GetExecutionWindows` — L438
- `UHodgeGameplayAbility_Definition::GetCinematicSource` — L445
- `UHodgeGameplayAbility_Definition::AcceptsNotify` — L459
- `UHodgeGameplayAbility_Definition::AllocateNotifyOccurrence` — L468
- `UHodgeGameplayAbility_Definition::BeginNotifyResource` — L474
- `UHodgeGameplayAbility_Definition::EndNotifyResource` — L484
- `UHodgeGameplayAbility_Definition::AcquireNotifyTag` — L504
- `UHodgeGameplayAbility_Definition::AcquireNotifyWeapon` — L521
- `UHodgeGameplayAbility_Definition::NotifyWindowsChanged` — L537
- `UHodgeGameplayAbility_Definition::SendExecutionEvent` — L544
- `UHodgeGameplayAbility_Definition::OnMontageBlendingOut` — L560
- `UHodgeGameplayAbility_Definition::OnMontageEnded` — L570
- `UHodgeGameplayAbility_Definition::ValidateExecutionConfiguration` — L577

## HodgeGameplayAbility_EnemyDeath.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeGameplayAbility_EnemyDeath::ActivateAbility` — L14
- `UHodgeGameplayAbility_EnemyDeath::CheckHeldDeathPose` — L34
- `UHodgeGameplayAbility_EnemyDeath::FinishMontage` — L46
- `UHodgeGameplayAbility_EnemyDeath::EndAbility` — L53

## HodgeGameplayAbility_HavocRules.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeGameplayAbility_HavocRules::UHodgeGameplayAbility_HavocRules` — L16
- `UHodgeGameplayAbility_HavocRules::Find` — L26
- `UHodgeGameplayAbility_HavocRules::ValidateEffects` — L39
- `UHodgeGameplayAbility_HavocRules::IsCurrentSource` — L52
- `UHodgeGameplayAbility_HavocRules::CanEnterDarkSurge` — L62
- `UHodgeGameplayAbility_HavocRules::TryEnterDarkSurge` — L73
- `UHodgeGameplayAbility_HavocRules::BeginSession` — L79
- `UHodgeGameplayAbility_HavocRules::EndDarkSurge` — L102
- `UHodgeGameplayAbility_HavocRules::ApplyDarkFlowGain` — L114
- `UHodgeGameplayAbility_HavocRules::HandleEnhancedNormalHit` — L126
- `UHodgeGameplayAbility_HavocRules::SettleFuel` — L137
- `UHodgeGameplayAbility_HavocRules::OnFuelChanged` — L148
- `UHodgeGameplayAbility_HavocRules::OnEffectRemoved` — L153
- `UHodgeGameplayAbility_HavocRules::OnDeathTagChanged` — L162
- `UHodgeGameplayAbility_HavocRules::ClearDarkFlow` — L167
- `UHodgeGameplayAbility_HavocRules::OnAvatarUninitialized` — L181
- `UHodgeGameplayAbility_HavocRules::BindAvatar` — L187
- `UHodgeGameplayAbility_HavocRules::ActivateAbility` — L198
- `UHodgeGameplayAbility_HavocRules::OnPawnAvatarSet` — L214
- `UHodgeGameplayAbility_HavocRules::Unbind` — L225
- `UHodgeGameplayAbility_HavocRules::EndAbility` — L238

## HodgeGameplayAbility_HitReaction.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeGameplayAbility_HitReaction::UHodgeGameplayAbility_HitReaction` — L18
- `UHodgeGameplayAbility_HitReaction::CanActivateAbility` — L30
- `UHodgeGameplayAbility_HitReaction::ActivateAbility` — L39
- `UHodgeGameplayAbility_HitReaction::Synchronize` — L68
- `UHodgeGameplayAbility_HitReaction::ClearMotion` — L134
- `UHodgeGameplayAbility_HitReaction::FinishReaction` — L154
- `UHodgeGameplayAbility_HitReaction::EndAbility` — L162

## HodgeGameplayAbility_Jump.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeGameplayAbility_Jump::UHodgeGameplayAbility_Jump` — L17
- `UHodgeGameplayAbility_Jump::CanActivateAbility` — L25
- `UHodgeGameplayAbility_Jump::EndAbility` — L52
- `UHodgeGameplayAbility_Jump::CharacterJumpStart` — L64
- `UHodgeGameplayAbility_Jump::CharacterJumpStop` — L77

## HodgeGameplayAbility_LockOn.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeGameplayAbility_LockOn::UHodgeGameplayAbility_LockOn` — L11
- `UHodgeGameplayAbility_LockOn::CanActivateAbility` — L21
- `UHodgeGameplayAbility_LockOn::ActivateAbility` — L27
- `UHodgeGameplayAbility_LockOn::OnTargetData` — L53
- `UHodgeGameplayAbility_LockOn::OnTimeout` — L64
- `UHodgeGameplayAbility_LockOn::EndAbility` — L66

## HodgeGameplayAbility_Melee.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp)

接收服务器 Window/Point 命中结果，准备锚点/目标、逐段/共享去重，构建独立 GE Spec/Context 并施加。

- `UHodgeGameplayAbility_Melee::UHodgeGameplayAbility_Melee` — L31
- `FHodgeMeleeHitHistory::CanHit` — L37
- `FHodgeMeleeHitHistory::RecordHit` — L46
- `UHodgeGameplayAbility_Melee::ValidateExecutionConfiguration` — L52
- `UHodgeGameplayAbility_Melee::PrepareHitExecutionContext_Implementation` — L98
- `UHodgeGameplayAbility_Melee::SetHitAnchor` — L101
- `UHodgeGameplayAbility_Melee::SetHitTarget` — L116
- `UHodgeGameplayAbility_Melee::ResetHitGeometryHistory` — L127
- `UHodgeGameplayAbility_Melee::OnExecutionReady` — L138
- `UHodgeGameplayAbility_Melee::OnExecutionEnding` — L169
- `UHodgeGameplayAbility_Melee::BeginNotifyHit` — L188
- `UHodgeGameplayAbility_Melee::OpenHit` — L257
- `UHodgeGameplayAbility_Melee::OnNotifyResourceEnded` — L281
- `UHodgeGameplayAbility_Melee::IsMeleeBatchCurrent` — L287
- `UHodgeGameplayAbility_Melee::OnHitResults` — L295
- `UHodgeGameplayAbility_Melee::CanApplyMeleeHit_Implementation` — L306
- `UHodgeGameplayAbility_Melee::MakeMeleeHitContext` — L313
- `UHodgeGameplayAbility_Melee::BuildMeleeHitSpec` — L335
- `UHodgeGameplayAbility_Melee::ProcessMeleeHitResults_Implementation` — L359
- `UHodgeGameplayAbility_Melee::SubmitMeleeHitEffects` — L394
- `FHodgeResonancePhaseLedger::TryReserve` — L448
- `UHodgeGameplayAbility_Melee::TryGrantResonanceEnergy` — L456
- `UHodgeGameplayAbility_Melee::ApplyMeleeHitEffects_Implementation` — L474
- `UHodgeGameplayAbility_Melee::OnExecutionEvent` — L492

## HodgeGameplayAbility_MovementAction.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeMovementActionTargetData::NetSerialize` — L18
- `UHodgeGameplayAbility_MovementAction::UHodgeGameplayAbility_MovementAction` — L23
- `UHodgeGameplayAbility_MovementAction::ResolveProfile` — L31
- `UHodgeGameplayAbility_MovementAction::CanActivateAbility` — L38
- `UHodgeGameplayAbility_MovementAction::ActivateAbility` — L53
- `UHodgeGameplayAbility_MovementAction::OnTargetData` — L88
- `UHodgeGameplayAbility_MovementAction::IsMovementValid` — L113
- `UHodgeGameplayAbility_MovementAction::IsExecutionMovementModeValid` — L124
- `UHodgeGameplayAbility_MovementAction::TickMovement` — L129
- `UHodgeGameplayAbility_MovementAction::SetMovementTag` — L138
- `UHodgeGameplayAbility_MovementAction::StartCue` — L152
- `UHodgeGameplayAbility_MovementAction::StopCue` — L160
- `UHodgeGameplayAbility_MovementAction::Finish` — L166
- `UHodgeGameplayAbility_MovementAction::OnConstraintChanged` — L169
- `UHodgeGameplayAbility_MovementAction::EndAbility` — L171

## HodgeGameplayAbility_ResonanceCircuit.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_ResonanceCircuit.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_ResonanceCircuit.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeGameplayAbility_ResonanceCircuit::CanActivateAbility` — L9
- `UHodgeGameplayAbility_ResonanceCircuit::OnExecutionReady` — L24
- `UHodgeGameplayAbility_ResonanceCircuit::CommitDarkSurge` — L29
- `UHodgeGameplayAbility_ResonanceCircuit::ProcessMeleeHitResults_Implementation` — L44

## HodgeGameplayAbility_Sprint.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeGameplayAbility_Sprint::UHodgeGameplayAbility_Sprint` — L26
- `FHodgeSprintPivotTargetData::NetSerialize` — L30
- `UHodgeGameplayAbility_Sprint::QualifiesPivot` — L33
- `UHodgeGameplayAbility_Sprint::IsPivotRecoveryComplete` — L42
- `UHodgeGameplayAbility_Sprint::ProcessMovementIntent` — L45
- `UHodgeGameplayAbility_Sprint::RequestPivot` — L55
- `UHodgeGameplayAbility_Sprint::HandleMovementFollowUp` — L69
- `UHodgeGameplayAbility_Sprint::StartPivot` — L95
- `UHodgeGameplayAbility_Sprint::ReleasePivotTask` — L117
- `UHodgeGameplayAbility_Sprint::UpdatePivotRecovery` — L125
- `UHodgeGameplayAbility_Sprint::OnPivotFinished` — L140
- `UHodgeGameplayAbility_Sprint::BeginMovement` — L146
- `UHodgeGameplayAbility_Sprint::UpdateMovement` — L191
- `UHodgeGameplayAbility_Sprint::OnAuthorityEnded` — L217
- `UHodgeGameplayAbility_Sprint::OnAttackStarted` — L223
- `UHodgeGameplayAbility_Sprint::OnDamage` — L225
- `UHodgeGameplayAbility_Sprint::EndAbility` — L228

## HodgeAttributeSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp)

项目 AttributeSet 基础和 ASC 访问。

- `UHodgeAttributeSet::UHodgeAttributeSet` — L26
- `UHodgeAttributeSet::GetWorld` — L31
- `UHodgeAttributeSet::GetHodgeAbilitySystemComponent` — L44

## HodgeCombatSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeCombatSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeCombatSet.cpp)

BaseDamage/BaseHeal 战斗属性，OwnerOnly 复制，供伤害/治疗 Execution 捕获。

- `UHodgeCombatSet::UHodgeCombatSet` — L15
- `UHodgeCombatSet::GetLifetimeReplicatedProps` — L25
- `UHodgeCombatSet::OnRep_HavocDamageBonus` — L38
- `UHodgeCombatSet::OnRep_BaseDamage` — L44
- `UHodgeCombatSet::OnRep_BaseHeal` — L51

## HodgeHavocSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHavocSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHavocSet.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeHavocSet::UHodgeHavocSet` — L16
- `UHodgeHavocSet::IsResourceReady` — L18
- `UHodgeHavocSet::GetLifetimeReplicatedProps` — L23
- `UHodgeHavocSet::Clamp` — L30
- `UHodgeHavocSet::PreAttributeBaseChange` — L37
- `UHodgeHavocSet::PreAttributeChange` — L42
- `UHodgeHavocSet::PostAttributeChange` — L47
- `UHodgeHavocSet::OnRep_DarkFlow` — L53
- `UHodgeHavocSet::OnRep_MaxDarkFlow` — L58

## HodgeHealthSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp)

Health/MaxHealth、Damage/Healing 元属性、结算夹取/免疫/耗尽广播；BaseDamage/BaseHeal 位于 CombatSet。

- `UHodgeHealthSet::UHodgeHealthSet` — L52
- `UHodgeHealthSet::GetLifetimeReplicatedProps` — L69
- `UHodgeHealthSet::OnRep_Health` — L82
- `UHodgeHealthSet::OnRep_MaxHealth` — L118
- `UHodgeHealthSet::PreGameplayEffectExecute` — L135
- `UHodgeHealthSet::PostGameplayEffectExecute` — L199
- `UHodgeHealthSet::PreAttributeBaseChange` — L334
- `UHodgeHealthSet::PreAttributeChange` — L344
- `UHodgeHealthSet::PostAttributeChange` — L354
- `UHodgeHealthSet::BeginAttributeRebuild` — L388
- `UHodgeHealthSet::EndAttributeRebuild` — L399
- `UHodgeHealthSet::GetResourceClampMax` — L408
- `UHodgeHealthSet::SetHealthForAttributeCommit` — L414
- `UHodgeHealthSet::ClampAttribute` — L430

## HodgeResonanceSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeResonanceSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeResonanceSet.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeResonanceSet::UHodgeResonanceSet` — L15
- `UHodgeResonanceSet::IsResourceReady` — L17
- `UHodgeResonanceSet::GetLifetimeReplicatedProps` — L22
- `UHodgeResonanceSet::Clamp` — L29
- `UHodgeResonanceSet::PreAttributeBaseChange` — L36
- `UHodgeResonanceSet::PreAttributeChange` — L41
- `UHodgeResonanceSet::PostAttributeChange` — L46
- `UHodgeResonanceSet::OnRep_ResonanceEnergy` — L52
- `UHodgeResonanceSet::OnRep_MaxResonanceEnergy` — L57

## HodgeHavocEffects.cpp

[Source/Hodgepodge/Private/AbilitySystem/Effects/HodgeHavocEffects.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Effects/HodgeHavocEffects.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeDarkFlowEffect::UHodgeDarkFlowEffect` — L9
- `UHodgeDarkFlowInitialization::UHodgeDarkFlowInitialization` — L18
- `UHodgeDarkSurgeEffect::UHodgeDarkSurgeEffect` — L26
- `UHodgeDarkFlowDrainEffect::UHodgeDarkFlowDrainEffect` — L37
- `UHodgeDarkFlowDrainExecution::Execute_Implementation` — L45

## HodgeResonanceEnergyEffect.cpp

[Source/Hodgepodge/Private/AbilitySystem/Effects/HodgeResonanceEnergyEffect.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Effects/HodgeResonanceEnergyEffect.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeResonanceEnergyEffect::UHodgeResonanceEnergyEffect` — L6
- `UHodgeResonanceEnergyInitialization::UHodgeResonanceEnergyInitialization` — L15

## HodgeDamageExecution.cpp

[Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeDamageExecution.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeDamageExecution.cpp)

BaseDamage × SetByCaller 倍率 × 距离/材质衰减 × 共享目标规则；不再恒零。

- `UHodgeDamageExecution::UHodgeDamageExecution` — L59
- `UHodgeDamageExecution::Execute_Implementation` — L67

## HodgeHealExecution.cpp

[Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeHealExecution.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeHealExecution.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeHealExecution::UHodgeHealExecution` — L48
- `UHodgeHealExecution::Execute_Implementation` — L55

## HodgeGameplayCue_Movement.cpp

[Source/Hodgepodge/Private/AbilitySystem/GameplayCues/HodgeGameplayCue_Movement.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/GameplayCues/HodgeGameplayCue_Movement.cpp)

模块定义或基础代码；请查看对应文件。

- `AHodgeGameplayCue_Movement::AHodgeGameplayCue_Movement` — L11
- `AHodgeGameplayCue_Movement::OnActive_Implementation` — L16
- `AHodgeGameplayCue_Movement::WhileActive_Implementation` — L19
- `AHodgeGameplayCue_Movement::OnRemove_Implementation` — L31
- `AHodgeGameplayCue_Movement::OnExecute_Implementation` — L38

## GameplayTagStack.cpp

[Source/Hodgepodge/Private/AbilitySystem/GameplayTagStack.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/GameplayTagStack.cpp)

带计数的标签栈及复制数据结构，区别于只判断有无的 TagContainer。

- `FGameplayTagStack::GetDebugString` — L16
- `FGameplayTagStackContainer::AddStack` — L25
- `FGameplayTagStackContainer::RemoveStack` — L72
- `FGameplayTagStackContainer::PreReplicatedRemove` — L127
- `FGameplayTagStackContainer::PostReplicatedAdd` — L141
- `FGameplayTagStackContainer::PostReplicatedChange` — L155

## HodgeAbilitySystemComponent.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp)

Tag 输入、激活组、关系映射、Montage 复制和全局注册；失败时恢复武器预测与连段记忆。

- `UHodgeAbilitySystemComponent::UHodgeAbilitySystemComponent` — L43
- `UHodgeAbilitySystemComponent::EndPlay` — L60
- `UHodgeAbilitySystemComponent::InitAbilityActorInfo` — L75
- `UHodgeAbilitySystemComponent::TryActivateAbilitiesOnSpawn` — L154
- `UHodgeAbilitySystemComponent::CancelAbilitiesByFunc` — L172
- `UHodgeAbilitySystemComponent::CancelInputActivatedAbilities` — L241
- `UHodgeAbilitySystemComponent::AbilitySpecInputPressed` — L259
- `UHodgeAbilitySystemComponent::AbilitySpecInputReleased` — L287
- `UHodgeAbilitySystemComponent::AbilityInputTagPressed` — L315
- `UHodgeAbilitySystemComponent::AbilityInputTagReleased` — L344
- `UHodgeAbilitySystemComponent::ProcessAbilityInput` — L366
- `UHodgeAbilitySystemComponent::ClearAbilityInput` — L501
- `UHodgeAbilitySystemComponent::NotifyAbilityActivated` — L515
- `UHodgeAbilitySystemComponent::NotifyAbilityFailed` — L539
- `UHodgeAbilitySystemComponent::NotifyAbilityEnded` — L562
- `UHodgeAbilitySystemComponent::ApplyAbilityBlockAndCancelTags` — L579
- `UHodgeAbilitySystemComponent::HandleChangeAbilityCanBeCanceled` — L627
- `UHodgeAbilitySystemComponent::GetAdditionalActivationTagRequirements` — L639
- `UHodgeAbilitySystemComponent::SetTagRelationshipMapping` — L653
- `UHodgeAbilitySystemComponent::HasActiveAbilityTag` — L660
- `UHodgeAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation` — L670
- `UHodgeAbilitySystemComponent::HandleAbilityFailed` — L678
- `UHodgeAbilitySystemComponent::IsActivationGroupBlocked` — L692
- `UHodgeAbilitySystemComponent::AddAbilityToActivationGroup` — L724
- `UHodgeAbilitySystemComponent::RemoveAbilityFromActivationGroup` — L773
- `UHodgeAbilitySystemComponent::CancelActivationGroupAbilities` — L787
- `UHodgeAbilitySystemComponent::AddDynamicTagGameplayEffect` — L804
- `UHodgeAbilitySystemComponent::RemoveDynamicTagGameplayEffect` — L841
- `UHodgeAbilitySystemComponent::GetAbilityTargetData` — L867
- `UHodgeAbilitySystemComponent::GetLifetimeReplicatedProps` — L883
- `UHodgeAbilitySystemComponent::GiveAbilityDefinition` — L891
- `UHodgeAbilitySystemComponent::FindAbilityDefinition` — L925
- `UHodgeAbilitySystemComponent::FindDefinitionAbility` — L936
- `UHodgeAbilitySystemComponent::OnRemoveAbility` — L952
- `UHodgeAbilitySystemComponent::ProcessDeferredComboRequest` — L959
- `UHodgeAbilitySystemComponent::InternalServerTryActivateAbility` — L995
- `UHodgeAbilitySystemComponent::ClientConfirmComboTiming_Implementation` — L1050
- `UHodgeAbilitySystemComponent::ClientActivateAbilitySucceedWithEventData_Implementation` — L1074
- `UHodgeAbilitySystemComponent::ClientActivateAbilityFailed_Implementation` — L1098
- `UHodgeAbilitySystemComponent::PlayMontage` — L1118
- `UHodgeAbilitySystemComponent::PlayMontageSimulated` — L1136
- `UHodgeAbilitySystemComponent::ApplyDefinitionMontageSettings` — L1144
- `UHodgeAbilitySystemComponent::OnRep_DefinitionMontage` — L1162
- `UHodgeAbilitySystemComponent::OnRep_ReplicatedAnimMontage` — L1172
- `UHodgeAbilitySystemComponent::StopDefinitionMontage` — L1179
- `UHodgeAbilitySystemComponent::CurrentMontageStop` — L1187
- `UHodgeAbilitySystemComponent::ReleaseAbilityInput` — L1214
- `UHodgeAbilitySystemComponent::ServerSetReplicatedTargetData_Implementation` — L1230
- `UHodgeAbilitySystemComponent::RetireLockOnCommand` — L1256
- `UHodgeAbilitySystemComponent::RetireAttackFacingData` — L1264
- `UHodgeAbilitySystemComponent::ResolveAbilityVariant` — L1272

## HodgeAbilitySystemGlobals.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemGlobals.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemGlobals.cpp)

分配 FHodgeGameplayEffectContext 的 GAS 全局类；已加入项目配置。

- `UHodgeAbilitySystemGlobals::UHodgeAbilitySystemGlobals` — L18
- `UHodgeAbilitySystemGlobals::AllocGameplayEffectContext` — L25

## HodgeAbilityTagRelationshipMapping.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilityTagRelationshipMapping.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilityTagRelationshipMapping.cpp)

数据驱动的能力阻断、取消与激活条件关系。

- `UHodgeAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel` — L13
- `UHodgeAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags` — L43
- `UHodgeAbilityTagRelationshipMapping::IsAbilityCancelledByTag` — L75
- `UHodgeAbilityTagRelationshipMapping::IsCancellationProtected` — L79
- `UHodgeAbilityTagRelationshipMapping::GetAbilityCancelExceptions` — L84
- `UHodgeAbilityTagRelationshipMapping::IsAbilityCancelledByTags` — L91
- `UHodgeAbilityTagRelationshipMapping::IsForcedCancellation` — L99

## HodgeGameplayCueManager.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayCueManager.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayCueManager.cpp)

项目 Cue 管理类已配置；启动预加载及 Feature Cue 观察者生命周期仍未完整接通。

- `UHodgeGameplayCueManager::UHodgeGameplayCueManager` — L71
- `UHodgeGameplayCueManager::Get` — L77
- `UHodgeGameplayCueManager::OnCreated` — L83
- `UHodgeGameplayCueManager::LoadAlwaysLoadedCues` — L93
- `UHodgeGameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries` — L127
- `UHodgeGameplayCueManager::ShouldSyncLoadMissingGameplayCues` — L156
- `UHodgeGameplayCueManager::ShouldAsyncLoadMissingGameplayCues` — L163
- `UHodgeGameplayCueManager::DumpGameplayCues` — L170
- `UHodgeGameplayCueManager::OnGameplayTagLoaded` — L265
- `UHodgeGameplayCueManager::HandlePostGarbageCollect` — L312
- `UHodgeGameplayCueManager::ProcessLoadedTags` — L325
- `UHodgeGameplayCueManager::ProcessTagToPreload` — L376
- `UHodgeGameplayCueManager::OnPreloadCueComplete` — L441
- `UHodgeGameplayCueManager::RegisterPreloadedCue` — L457
- `UHodgeGameplayCueManager::HandlePostLoadMap` — L493
- `UHodgeGameplayCueManager::UpdateDelayLoadDelegateListeners` — L540
- `UHodgeGameplayCueManager::ShouldDelayLoadGameplayCues` — L584
- `UHodgeGameplayCueManager::RefreshGameplayCuePrimaryAsset` — L603

## HodgeGameplayEffectContext.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayEffectContext.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayEffectContext.cpp)

项目 GE 上下文与序列化扩展，已有 HodgeAbilitySystemGlobals 分配配套。

- `FHodgeGameplayEffectContext::ExtractEffectContext` — L21
- `FHodgeGameplayEffectContext::NetSerialize` — L40
- `FHodgeGameplayEffectContext::SetAbilitySource` — L64
- `FHodgeGameplayEffectContext::GetAbilitySource` — L74
- `FHodgeGameplayEffectContext::GetPhysicalMaterial` — L81

## HodgeGameplayTags.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayTags.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayTags.cpp)

原生 GameplayTag 注册和移动状态标签映射；含攻击时间轴依赖的 Status.Attack.*（阶段 + 取消窗口 .Cancel.*）与 GameplayEvent.Attack.*。标签存在不等于对应玩法实现。

## HodgeGlobalAbilitySystem.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeGlobalAbilitySystem.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGlobalAbilitySystem.cpp)

世界级全局能力/效果授予及 ASC 注册表。

- `FGlobalAppliedAbilityList::AddToASC` — L13
- `FGlobalAppliedAbilityList::RemoveFromASC` — L35
- `FGlobalAppliedAbilityList::RemoveFromAll` — L49
- `FGlobalAppliedEffectList::AddToASC` — L68
- `FGlobalAppliedEffectList::RemoveFromASC` — L88
- `FGlobalAppliedEffectList::RemoveFromAll` — L102
- `UHodgeGlobalAbilitySystem::UHodgeGlobalAbilitySystem` — L120
- `UHodgeGlobalAbilitySystem::ApplyAbilityToAll` — L125
- `UHodgeGlobalAbilitySystem::ApplyEffectToAll` — L142
- `UHodgeGlobalAbilitySystem::RemoveAbilityFromAll` — L159
- `UHodgeGlobalAbilitySystem::RemoveEffectFromAll` — L176
- `UHodgeGlobalAbilitySystem::RegisterASC` — L193
- `UHodgeGlobalAbilitySystem::UnregisterASC` — L214

## HodgeLockOnStateEffect.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeLockOnStateEffect.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeLockOnStateEffect.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeLockOnStateEffect::UHodgeLockOnStateEffect` — L6

## HodgeAttributeCoordinator.cpp

[Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeAttributeCoordinator.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeAttributeCoordinator.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeAttributeCoordinator::GetPlayerState` — L21
- `UHodgeAttributeCoordinator::IsBoundTo` — L23
- `UHodgeAttributeCoordinator::IsCurrentAvatar` — L25
- `UHodgeAttributeCoordinator::IsReadyFor` — L27
- `UHodgeAttributeCoordinator::ValidateCoreSets` — L35
- `UHodgeAttributeCoordinator::PublishReady` — L49
- `UHodgeAttributeCoordinator::BeginUpdate` — L67
- `UHodgeAttributeCoordinator::ApplyCharacterBase` — L84
- `UHodgeAttributeCoordinator::PrepareAvatar` — L101
- `UHodgeAttributeCoordinator::CompleteAvatarInitialization` — L136
- `UHodgeAttributeCoordinator::CommitUpdate` — L151
- `UHodgeAttributeCoordinator::DetachAvatar` — L196
- `UHodgeAttributeCoordinator::SetCharacterLevel` — L211
- `UHodgeAttributeCoordinator::RestoreHealth` — L230
- `UHodgeAttributeCoordinator::BeginEquipmentUpdate` — L241
- `UHodgeAttributeCoordinator::FinishEquipmentUpdate` — L243
- `UHodgeAttributeCoordinator::ExpectRespawn` — L245
- `UHodgeAttributeCoordinator::SetInitialSavedHealth` — L248
- `UHodgeAttributeCoordinator::GetOrCreateDefaultEquipment` — L258
- `UHodgeAttributeCoordinator::RememberEquipment` — L276

## HodgeCharacterBaseStatEffect.cpp

[Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeCharacterBaseStatEffect.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeCharacterBaseStatEffect.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeCharacterBaseStatEffect::UHodgeCharacterBaseStatEffect` — L11

## HodgeEquipmentStatEffect.cpp

[Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeEquipmentStatEffect.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeEquipmentStatEffect.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeEquipmentStatEffect::UHodgeEquipmentStatEffect` — L12

## HodgeActorBase.cpp

[Source/Hodgepodge/Private/Actor/HodgeActorBase.cpp](../../../Source/Hodgepodge/Private/Actor/HodgeActorBase.cpp)

项目 Actor 基类扩展。

- `AHodgeActorBase::AHodgeActorBase` — L24
- `AHodgeActorBase::BeginPlay` — L37
- `AHodgeActorBase::Tick` — L48

## HodgeBTTask_ExecuteAbility.cpp

[Source/Hodgepodge/Private/AI/HodgeBTTask_ExecuteAbility.cpp](../../../Source/Hodgepodge/Private/AI/HodgeBTTask_ExecuteAbility.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeBTTask_ExecuteAbility::UHodgeBTTask_ExecuteAbility` — L21
- `UHodgeBTTask_ExecuteAbility::InitializeFromAsset` — L30
- `UHodgeBTTask_ExecuteAbility::IsCurrent` — L50
- `UHodgeBTTask_ExecuteAbility::ExecuteTask` — L57
- `UHodgeBTTask_ExecuteAbility::TickTask` — L89
- `UHodgeBTTask_ExecuteAbility::HandleRequestChanged` — L112
- `UHodgeBTTask_ExecuteAbility::Cleanup` — L124
- `UHodgeBTTask_ExecuteAbility::AbortTask` — L137
- `UHodgeBTTask_ExecuteAbility::OnTaskFinished` — L141

## HodgeEnemyAIController.cpp

[Source/Hodgepodge/Private/AI/HodgeEnemyAIController.cpp](../../../Source/Hodgepodge/Private/AI/HodgeEnemyAIController.cpp)

模块定义或基础代码；请查看对应文件。

- `AHodgeEnemyAIController::AHodgeEnemyAIController` — L25
- `AHodgeEnemyAIController::PreInitializeComponents` — L41
- `AHodgeEnemyAIController::BeginPlay` — L48
- `AHodgeEnemyAIController::OnPossess` — L55
- `AHodgeEnemyAIController::IsPossessCurrent` — L72
- `AHodgeEnemyAIController::HandleReadiness` — L76
- `AHodgeEnemyAIController::TryStartBrain` — L85
- `AHodgeEnemyAIController::IsValidEnemyTarget` — L121
- `AHodgeEnemyAIController::HandlePerception` — L129
- `AHodgeEnemyAIController::UpdateDecision` — L140
- `AHodgeEnemyAIController::HandleConstraint` — L180
- `AHodgeEnemyAIController::StopCycle` — L187
- `AHodgeEnemyAIController::OnUnPossess` — L204
- `AHodgeEnemyAIController::EndPlay` — L206

## HodgeAnimInstance.cpp

[Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp)

ASC 属性映射、GroundDistance 与旋转策略快照；FullBody 权重协调蓝图程序性根 Yaw。

- `UHodgeAnimInstance::UHodgeAnimInstance` — L24
- `UHodgeAnimInstance::InitializeWithAbilitySystem` — L29
- `UHodgeAnimInstance::IsDataValid` — L40
- `UHodgeAnimInstance::NativeInitializeAnimation` — L54
- `UHodgeAnimInstance::NativeUpdateAnimation` — L72
- `UHodgeAnimInstance::ResolveFootIKAlpha` — L142
- `UHodgeAnimInstance::ResolveCycleSyncGroup` — L149
- `UHodgeAnimInstance::AdvanceTurnTime` — L153
- `UHodgeAnimInstance::MatchCycleRate` — L160
- `UHodgeAnimInstance::UpdateSprintPivot` — L167
- `UHodgeAnimInstance::IsTurnComplete` — L184
- `UHodgeAnimInstance::UpdateSprintCycle` — L190
- `UHodgeAnimInstance::ShouldExitSprintPivot` — L202

## HodgeAnimNotify_DarkSurgeCommit.cpp

[Source/Hodgepodge/Private/Animation/HodgeAnimNotify_DarkSurgeCommit.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeAnimNotify_DarkSurgeCommit.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeAnimNotify_DarkSurgeCommit::UHodgeAnimNotify_DarkSurgeCommit` — L9
- `UHodgeAnimNotify_DarkSurgeCommit::BranchingPointNotify` — L17

## HodgeAnimNotifyState_CinematicCamera.cpp

[Source/Hodgepodge/Private/Animation/HodgeAnimNotifyState_CinematicCamera.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeAnimNotifyState_CinematicCamera.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeAnimNotifyState_CinematicCamera::GetNotifyName_Implementation` — L6

## HodgeAnimNotifyState_WingAction.cpp

[Source/Hodgepodge/Private/Animation/HodgeAnimNotifyState_WingAction.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeAnimNotifyState_WingAction.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeAnimNotifyState_WingAction::UHodgeAnimNotifyState_WingAction` — L12
- `UHodgeAnimNotifyState_WingAction::Validate` — L17
- `UHodgeAnimNotifyState_WingAction::EvaluateWingTime` — L30
- `UHodgeAnimNotifyState_WingAction::GetNotifyName_Implementation` — L38
- `UHodgeAnimNotifyState_WingAction::RefreshOwner` — L43
- `UHodgeAnimNotifyState_WingAction::NotifyBegin` — L53
- `UHodgeAnimNotifyState_WingAction::NotifyEnd` — L58
- `UHodgeAnimNotifyState_WingAction::BranchingPointNotifyBegin` — L63
- `UHodgeAnimNotifyState_WingAction::BranchingPointNotifyEnd` — L68

## HodgeCombatAnimNotifies.cpp

[Source/Hodgepodge/Private/Animation/HodgeCombatAnimNotifies.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeCombatAnimNotifies.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeAnimHitConfig::FHodgeAnimHitConfig` — L39
- `FHodgeAnimHitConfig::Validate` — L45
- `UHodgeAnimNotifyState_HitCheck::UHodgeAnimNotifyState_HitCheck` — L86
- `UHodgeAnimNotify_Hit::UHodgeAnimNotify_Hit` — L89
- `UHodgeAnimNotifyState_GameplayTag::UHodgeAnimNotifyState_GameplayTag` — L96
- `UHodgeAnimNotifyState_WeaponHand::UHodgeAnimNotifyState_WeaponHand` — L98
- `UHodgeAnimNotify_GameplayEvent::UHodgeAnimNotify_GameplayEvent` — L100
- `UHodgeAnimNotifyState_HitCheck::BranchingPointNotifyBegin` — L103
- `UHodgeAnimNotifyState_HitCheck::BranchingPointNotifyEnd` — L113
- `UHodgeAnimNotify_Hit::BranchingPointNotify` — L119
- `UHodgeAnimNotifyState_GameplayTag::BranchingPointNotifyBegin` — L137
- `UHodgeAnimNotifyState_GameplayTag::BranchingPointNotifyEnd` — L147
- `UHodgeAnimNotifyState_WeaponHand::BranchingPointNotifyBegin` — L153
- `UHodgeAnimNotifyState_WeaponHand::BranchingPointNotifyEnd` — L163
- `UHodgeAnimNotify_GameplayEvent::BranchingPointNotify` — L169
- `UHodgeAnimNotifyState_HitCheck::NotifyBegin` — L192
- `UHodgeAnimNotifyState_HitCheck::NotifyEnd` — L200
- `UHodgeAnimNotifyState_GameplayTag::NotifyBegin` — L208
- `UHodgeAnimNotifyState_GameplayTag::NotifyEnd` — L216
- `UHodgeAnimNotifyState_WeaponHand::NotifyBegin` — L224
- `UHodgeAnimNotifyState_WeaponHand::NotifyEnd` — L232
- `UHodgeAnimNotify_Hit::Notify` — L240
- `UHodgeAnimNotify_GameplayEvent::Notify` — L248
- `UHodgeAnimNotifyState_HitCheck::OnAnimNotifyCreatedInEditor` — L275
- `UHodgeAnimNotify_Hit::OnAnimNotifyCreatedInEditor` — L281
- `UHodgeAnimNotifyState_GameplayTag::OnAnimNotifyCreatedInEditor` — L284
- `UHodgeAnimNotifyState_WeaponHand::OnAnimNotifyCreatedInEditor` — L290
- `UHodgeAnimNotify_GameplayEvent::OnAnimNotifyCreatedInEditor` — L296

## HodgeEnemyAnimInstance.cpp

[Source/Hodgepodge/Private/Animation/HodgeEnemyAnimInstance.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeEnemyAnimInstance.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeEnemyAnimInstance::NativeUpdateAnimation` — L9

## HodgeRootMotionModifier_AttackFacing.cpp

[Source/Hodgepodge/Private/Animation/HodgeRootMotionModifier_AttackFacing.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeRootMotionModifier_AttackFacing.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeRootMotionModifier_AttackFacing::Update` — L8

## HodgeWingAnimInstance.cpp

[Source/Hodgepodge/Private/Animation/HodgeWingAnimInstance.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeWingAnimInstance.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeWingAnimInstance::NativeUpdateAnimation` — L10

## HodgeWingProfile.cpp

[Source/Hodgepodge/Private/Animation/HodgeWingProfile.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeWingProfile.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeWingFadeState::Show` — L17
- `FHodgeWingFadeState::BeginFade` — L23
- `FHodgeWingFadeState::Advance` — L33
- `FHodgeWingFadeState::Reset` — L43
- `FHodgeWingPresentationState::Begin` — L48
- `FHodgeWingPresentationState::End` — L64
- `FHodgeWingPresentationState::Reset` — L75
- `FHodgeWingPose::Blend` — L85
- `UHodgeWingProfile::HasValidTiming` — L96
- `UHodgeWingProfile::Evaluate` — L104
- `UHodgeWingProfile::Validate` — L120
- `UHodgeWingProfile::IsDataValid` — L179

## HodgeCameraComponent.cpp

[Source/Hodgepodge/Private/Camera/HodgeCameraComponent.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraComponent.cpp)

相机模式栈宿主与最终视图输出，依赖默认模式委托。

- `UHodgeCameraComponent::UHodgeCameraComponent` — L21
- `UHodgeCameraComponent::OnRegister` — L32
- `UHodgeCameraComponent::GetCameraView` — L47
- `UHodgeCameraComponent::GetActiveCameraModeClass` — L133
- `UHodgeCameraComponent::GetLastCameraModeView` — L137
- `UHodgeCameraComponent::QueueCinematicReturn` — L143
- `UHodgeCameraComponent::GetGameplayReturnRotation` — L149
- `UHodgeCameraComponent::EndCinematicPresentation` — L156
- `UHodgeCameraComponent::UpdateCameraModes` — L165
- `UHodgeCameraComponent::DrawDebug` — L188
- `UHodgeCameraComponent::GetBlendInfo` — L225

## HodgeCameraMode.cpp

[Source/Hodgepodge/Private/Camera/HodgeCameraMode.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraMode.cpp)

相机视图、模式实例、混合和模式栈。

- `FHodgeCameraModeView::FHodgeCameraModeView` — L23
- `FHodgeCameraModeView::Blend` — L32
- `UHodgeCameraMode::UHodgeCameraMode` — L71
- `UHodgeCameraMode::GetHodgeCameraComponent` — L99
- `UHodgeCameraMode::GetWorld` — L106
- `UHodgeCameraMode::GetTargetActor` — L113
- `UHodgeCameraMode::GetPivotLocation` — L123
- `UHodgeCameraMode::GetPivotRotation` — L169
- `UHodgeCameraMode::UpdateCameraMode` — L187
- `UHodgeCameraMode::UpdateView` — L197
- `UHodgeCameraMode::SetBlendWeight` — L222
- `UHodgeCameraMode::UpdateBlending` — L261
- `UHodgeCameraMode::DrawDebug` — L312
- `UHodgeCameraModeStack::UHodgeCameraModeStack` — L333
- `UHodgeCameraModeStack::ActivateStack` — L340
- `UHodgeCameraModeStack::DeactivateStack` — L361
- `UHodgeCameraModeStack::PushCameraMode` — L382
- `UHodgeCameraModeStack::EvaluateStack` — L472
- `UHodgeCameraModeStack::FindExistingMode` — L491
- `UHodgeCameraModeStack::GetCameraModeInstance` — L497
- `UHodgeCameraModeStack::UpdateStack` — L524
- `UHodgeCameraModeStack::BlendStack` — L583
- `UHodgeCameraModeStack::DrawDebug` — L614
- `UHodgeCameraModeStack::GetBlendInfo` — L642
- `UHodgeCameraModeStack::GetTopCameraModeClass` — L669

## HodgeCameraMode_Cinematic.cpp

[Source/Hodgepodge/Private/Camera/HodgeCameraMode_Cinematic.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraMode_Cinematic.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeCameraMode_Cinematic::UHodgeCameraMode_Cinematic` — L42
- `UHodgeCameraMode_Cinematic::KeepUnderlyingModes` — L47
- `UHodgeCameraMode_Cinematic::UpdateView` — L53
- `UHodgeCameraMode_Cinematic::UpdateBlending` — L129
- `UHodgeCameraMode_Cinematic::ReleaseLocalEffects` — L134
- `UHodgeCameraMode_Cinematic::OnDeactivation` — L141
- `UHodgeCameraMode_Cinematic::EndPresentation` — L146

## HodgeCameraMode_LockOn.cpp

[Source/Hodgepodge/Private/Camera/HodgeCameraMode_LockOn.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraMode_LockOn.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeCameraMode_LockOn::OnBecomeTop` — L14
- `UHodgeCameraMode_LockOn::GetPivotRotation` — L34
- `UHodgeCameraMode_LockOn::UpdateView` — L35

## HodgeCameraMode_ThirdPerson.cpp

[Source/Hodgepodge/Private/Camera/HodgeCameraMode_ThirdPerson.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraMode_ThirdPerson.cpp)

第三人称偏移与防穿透逻辑；C++ 不设默认偏移曲线，改由蓝图/PawnData 提供。

- `UHodgeCameraMode_ThirdPerson::UHodgeCameraMode_ThirdPerson` — L24
- `UHodgeCameraMode_ThirdPerson::UpdateView` — L59
- `UHodgeCameraMode_ThirdPerson::UpdateForTarget` — L115
- `UHodgeCameraMode_ThirdPerson::DrawDebug` — L142
- `UHodgeCameraMode_ThirdPerson::UpdatePreventPenetration` — L166
- `UHodgeCameraMode_ThirdPerson::PreventCameraPenetration` — L258
- `UHodgeCameraMode_ThirdPerson::SetTargetCrouchOffset` — L521
- `UHodgeCameraMode_ThirdPerson::UpdateCrouchOffset` — L534
- `UHodgeCameraMode_ThirdPerson::CopyViewSettingsFrom` — L557

## HodgePlayerCameraManager.cpp

[Source/Hodgepodge/Private/Camera/HodgePlayerCameraManager.cpp](../../../Source/Hodgepodge/Private/Camera/HodgePlayerCameraManager.cpp)

由 HodgePlayerController 构造选用的项目相机管理器；运行效果待验收。

- `AHodgePlayerCameraManager::AHodgePlayerCameraManager` — L21
- `AHodgePlayerCameraManager::GetUICameraComponent` — L38
- `AHodgePlayerCameraManager::UpdateViewTarget` — L45
- `AHodgePlayerCameraManager::DisplayDebug` — L65

## HodgeUICameraManagerComponent.cpp

[Source/Hodgepodge/Private/Camera/HodgeUICameraManagerComponent.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeUICameraManagerComponent.cpp)

UI 相机管理扩展，不代表 UI 系统已接入。

- `UHodgeUICameraManagerComponent::GetComponent` — L18
- `UHodgeUICameraManagerComponent::UHodgeUICameraManagerComponent` — L35
- `UHodgeUICameraManagerComponent::InitializeComponent` — L52
- `UHodgeUICameraManagerComponent::SetViewTarget` — L59
- `UHodgeUICameraManagerComponent::NeedsToUpdateViewTarget` — L72
- `UHodgeUICameraManagerComponent::UpdateViewTarget` — L79
- `UHodgeUICameraManagerComponent::OnShowDebugInfo` — L85

## HodgeCharacterBase.cpp

[Source/Hodgepodge/Private/Character/HodgeCharacterBase.cpp](../../../Source/Hodgepodge/Private/Character/HodgeCharacterBase.cpp)

原生 Character 基础、替换移动组件；Receiver 在 PreInit 注册、EndPlay 成对移除。

- `AHodgeCharacterBase::AHodgeCharacterBase` — L34
- `AHodgeCharacterBase::PreInitializeComponents` — L46
- `AHodgeCharacterBase::EndPlay` — L65
- `AHodgeCharacterBase::BeginPlay` — L78

## HodgeCombatCharacter.cpp

[Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp)

PawnExtension、相机、HealthComponent、原生 RotationComponent；ASC、移动/旋转约束、复制与死亡清理。

- `AHodgeCombatCharacter::AHodgeCombatCharacter` — L55
- `AHodgeCombatCharacter::PreInitializeComponents` — L177
- `AHodgeCombatCharacter::BeginPlay` — L184
- `AHodgeCombatCharacter::EndPlay` — L207
- `AHodgeCombatCharacter::Reset` — L245
- `AHodgeCombatCharacter::GetLifetimeReplicatedProps` — L258
- `AHodgeCombatCharacter::PreReplication` — L270
- `AHodgeCombatCharacter::NotifyControllerChanged` — L297
- `AHodgeCombatCharacter::GetHodgePlayerController` — L317
- `AHodgeCombatCharacter::GetHodgePlayerState` — L324
- `AHodgeCombatCharacter::GetHodgeAbilitySystemComponent` — L331
- `AHodgeCombatCharacter::GetAbilitySystemComponent` — L338
- `AHodgeCombatCharacter::OnAbilitySystemInitialized` — L350
- `AHodgeCombatCharacter::OnAbilitySystemUninitialized` — L370
- `AHodgeCombatCharacter::FaceRotation` — L388
- `AHodgeCombatCharacter::InitializeDefaultEquipment` — L401
- `AHodgeCombatCharacter::UninitializeDefaultEquipment` — L469
- `AHodgeCombatCharacter::PossessedBy` — L492
- `AHodgeCombatCharacter::UnPossessed` — L518
- `AHodgeCombatCharacter::OnRep_Controller` — L544
- `AHodgeCombatCharacter::OnRep_PlayerState` — L553
- `AHodgeCombatCharacter::SetupPlayerInputComponent` — L562
- `AHodgeCombatCharacter::InitializeGameplayTags` — L573
- `AHodgeCombatCharacter::GetOwnedGameplayTags` — L608
- `AHodgeCombatCharacter::HasMatchingGameplayTag` — L618
- `AHodgeCombatCharacter::HasAllMatchingGameplayTags` — L630
- `AHodgeCombatCharacter::HasAnyMatchingGameplayTags` — L642
- `AHodgeCombatCharacter::FellOutOfWorld` — L654
- `AHodgeCombatCharacter::OnDeathStarted` — L661
- `AHodgeCombatCharacter::OnDeathFinished` — L670
- `AHodgeCombatCharacter::DisableMovementAndCollision` — L677
- `AHodgeCombatCharacter::DestroyDueToDeath` — L707
- `AHodgeCombatCharacter::UninitAndDestroy` — L717
- `AHodgeCombatCharacter::OnMovementModeChanged` — L745
- `AHodgeCombatCharacter::SetMovementModeTag` — L761
- `AHodgeCombatCharacter::ToggleCrouch` — L790
- `AHodgeCombatCharacter::OnStartCrouch` — L809
- `AHodgeCombatCharacter::OnEndCrouch` — L823
- `AHodgeCombatCharacter::CanJumpInternal_Implementation` — L837
- `AHodgeCombatCharacter::OnRep_ReplicatedAcceleration` — L844
- `AHodgeCombatCharacter::OnControllerChangedTeam` — L874
- `AHodgeCombatCharacter::OnRep_MyTeamID` — L887
- `AHodgeCombatCharacter::UpdateSharedReplication` — L894
- `AHodgeCombatCharacter::FastSharedReplication_Implementation` — L927
- `FSharedRepMovement::FSharedRepMovement` — L969
- `FSharedRepMovement::FillForCharacter` — L976
- `FSharedRepMovement::Equals` — L1023
- `FSharedRepMovement::NetSerialize` — L1066
- `AHodgeCombatCharacter::TryInitializeAttributesAndEquipment` — L1104
- `AHodgeCombatCharacter::PrepareDefaultEquipment` — L1126
- `AHodgeCombatCharacter::IsDefaultEquipmentReady` — L1133

## HodgeEnemyCharacter.cpp

[Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp)

仅设置 AI 自动控制；旋转/移动参数继承 Combat 基类，尚未完成敌人 ASC 初始化。

- `AHodgeEnemyCharacter::AHodgeEnemyCharacter` — L27
- `AHodgeEnemyCharacter::BeginPlay` — L66
- `AHodgeEnemyCharacter::PossessedBy` — L73
- `AHodgeEnemyCharacter::GetAbilitySystemComponent` — L80
- `AHodgeEnemyCharacter::InitializeEnemyAbilitySystem` — L88
- `AHodgeEnemyCharacter::OnRep_EnemyPawnData` — L94
- `AHodgeEnemyCharacter::EndPlay` — L97
- `AHodgeEnemyCharacter::GetLifetimeReplicatedProps` — L103
- `AHodgeEnemyCharacter::PostEditChangeProperty` — L111
- `AHodgeEnemyCharacter::CanBeLockedBy_Implementation` — L138
- `AHodgeEnemyCharacter::GetLockPoint_Implementation` — L144
- `AHodgeEnemyCharacter::GetLockPriority_Implementation` — L152

## HodgeHeroCharacter.cpp

[Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp)

构造挂载 HeroComponent；PossessedBy/OnRep_PlayerState 只调用 Super，ASC 接入已收敛。

- `AHodgeHeroCharacter::FaceRotation` — L22
- `AHodgeHeroCharacter::AHodgeHeroCharacter` — L33
- `AHodgeHeroCharacter::PossessedBy` — L50
- `AHodgeHeroCharacter::OnRep_PlayerState` — L66

## HodgeALSLocomotion.cpp

[Source/Hodgepodge/Private/CodexText/HodgeALSLocomotion.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeALSLocomotion.cpp)

CodexText 实验：6 向地面运动动画实例，不属于主 Hero 动画链。

- `UHodgeALSLocomotion::GetLabActiveState` — L32
- `UHodgeALSLocomotion::NativeUpdateAnimation` — L39
- `UHodgeALSAuthoring::BuildLocomotionGraph` — L113

## HodgeGroundedAuthoring.cpp

[Source/Hodgepodge/Private/CodexText/HodgeGroundedAuthoring.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeGroundedAuthoring.cpp)

CodexText 实验：编辑器辅助库，含 Grounded 图层与停步/转身精修、步幅、地形、战斗图层等编辑期构建函数。

- `UHodgeGroundedAuthoring::RepairGroundedLegIK` — L69
- `UHodgeGroundedAuthoring::RefineGroundedTransitions` — L120
- `UHodgeGroundedAuthoring::AddStrideLayer` — L184
- `UHodgeGroundedAuthoring::AddTerrainLayer` — L244
- `UHodgeGroundedAuthoring::AddCombatLayer` — L307
- `UHodgeGroundedAuthoring::AddGroundedLayer` — L364

## HodgeGroundedLocomotion.cpp

[Source/Hodgepodge/Private/CodexText/HodgeGroundedLocomotion.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeGroundedLocomotion.cpp)

CodexText 实验：在 ALS 基础动画上增加平地起停/转身/脚锁，并扩展上半身 Overlay、左手握持、步幅与地形 IK。

- `UHodgeGroundedLocomotion::NativeInitializeAnimation` — L14
- `UHodgeGroundedLocomotion::GetGroundedState` — L35
- `UHodgeGroundedLocomotion::SetLeftHandGrip` — L42
- `UHodgeGroundedLocomotion::NativeUninitializeAnimation` — L48
- `UHodgeGroundedLocomotion::CacheFinalizedFootPose` — L59
- `UHodgeGroundedLocomotion::NativeUpdateAnimation` — L72

## HodgeLocomotionLab.cpp

[Source/Hodgepodge/Private/CodexText/HodgeLocomotionLab.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeLocomotionLab.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeLocomotionLabComponent::UHodgeLocomotionLabComponent` — L31
- `UHodgeLocomotionLabComponent::BeginPlay` — L38
- `UHodgeLocomotionLabComponent::SetCombatFacing` — L53
- `UHodgeLocomotionLabComponent::SetWalking` — L67
- `UHodgeLocomotionLabComponent::TickComponent` — L77
- `AHodgeLocomotionLabMode::InitGame` — L121
- `UHodgeLocomotionLabAuthoring::RemapCopy` — L129
- `UHodgeLocomotionLabAuthoring::ConfigureGroundBlend` — L167

## HodgeSurvivor.cpp

[Source/Hodgepodge/Private/CodexText/HodgeSurvivor.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeSurvivor.cpp)

模块定义或基础代码；请查看对应文件。

- `AHodgeSurvivorHero::AHodgeSurvivorHero` — L26
- `AHodgeSurvivorHero::SetupPlayerInputComponent` — L54
- `AHodgeSurvivorHero::Forward` — L77
- `AHodgeSurvivorHero::Right` — L83
- `AHodgeSurvivorHero::Dash` — L89
- `AHodgeSurvivorHero::Tick` — L99
- `AHodgeSurvivorHero::EndPlay` — L117
- `AHodgeSurvivorMode::AHodgeSurvivorMode` — L126
- `AHodgeSurvivorMode::StartPlay` — L137
- `AHodgeSurvivorMode::MakeShape` — L152
- `AHodgeSurvivorMode::SpawnEnemy` — L168
- `AHodgeSurvivorMode::HitEnemy` — L190
- `AHodgeSurvivorMode::DamageHero` — L208
- `AHodgeSurvivorMode::GrantExperience` — L216
- `AHodgeSurvivorMode::GetUpgradeText` — L229
- `AHodgeSurvivorMode::ChooseUpgrade` — L235
- `AHodgeSurvivorMode::SetRunState` — L251
- `AHodgeSurvivorMode::TogglePause` — L272
- `AHodgeSurvivorMode::RestartRun` — L278
- `AHodgeSurvivorMode::ReturnToMenu` — L283
- `AHodgeSurvivorMode::Tick` — L288
- `AHodgeSurvivorMode::EndPlay` — L374

## HodgeSurvivorHUD.cpp

[Source/Hodgepodge/Private/CodexText/HodgeSurvivorHUD.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeSurvivorHUD.cpp)

CodexText 实验：代码构建的 Survivor HUD UserWidget。

- `UHodgeSurvivorHUD::Mode` — L41
- `UHodgeSurvivorHUD::Text` — L43
- `UHodgeSurvivorHUD::AddButton` — L48
- `UHodgeSurvivorHUD::NativeConstruct` — L63
- `UHodgeSurvivorHUD::Refresh` — L134
- `UHodgeSurvivorHUD::NativeDestruct` — L174
- `UHodgeSurvivorHUD::ChooseOne` — L184
- `UHodgeSurvivorHUD::ChooseTwo` — L186
- `UHodgeSurvivorHUD::ChooseThree` — L188
- `UHodgeSurvivorHUD::Retry` — L190
- `UHodgeSurvivorHUD::Menu` — L192
- `UHodgeSurvivorHUD::Pause` — L194

## HodgeAttackFacingTypes.cpp

[Source/Hodgepodge/Private/Combat/HodgeAttackFacingTypes.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeAttackFacingTypes.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeAttackFacingConfig::Validate` — L7
- `HodgeAttackFacing::ResolveYaw` — L16
- `FHodgeAttackFacingTargetData::NetSerialize` — L21

## HodgeCharacterFacingTypes.cpp

[Source/Hodgepodge/Private/Combat/HodgeCharacterFacingTypes.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeCharacterFacingTypes.cpp)

模块定义或基础代码；请查看对应文件。

- `HodgeFacing::NormalizeDirection` — L8
- `HodgeFacing::MakeMoveIntent` — L17
- `HodgeFacing::IsBaseDriver` — L33

## HodgeDamageRules.cpp

[Source/Hodgepodge/Private/Combat/HodgeDamageRules.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeDamageRules.cpp)

检测和 Execution 共用目标、ASC、死亡/Health、自伤/友伤及队伍规则。

- `FHodgeDamageRules::CanDamage` — L31

## HodgeHavocRules.cpp

[Source/Hodgepodge/Private/Combat/HodgeHavocRules.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeHavocRules.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeHavocFuelClock::Settle` — L4
- `FHodgeHavocProcGate::TryReserve` — L12
- `FHodgeHavocDamageRules::GetMultiplier` — L18
- `FHodgeHavocHoldSession::Begin` — L23
- `FHodgeHavocHoldSession::IsReady` — L29
- `FHodgeHavocHoldSession::HasExpired` — L34

## HodgeHitDetection.cpp

[Source/Hodgepodge/Private/Combat/HodgeHitDetection.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeHitDetection.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeHitVolumeConfig::Validate` — L162
- `UHodgeHitDetectionProfile::UHodgeHitDetectionProfile` — L194
- `UHodgeHitDetectionProfile::Validate` — L200
- `UHodgeHitDetectionProfile::IsDataValid` — L226
- `UHodgeSocketSweepStrategy::Capture` — L239
- `UHodgeSocketSweepStrategy::Detect` — L274
- `UHodgeBoxSweepStrategy::Capture` — L302
- `UHodgeBoxSweepStrategy::Detect` — L316
- `UHodgeShapeQueryStrategy::Capture` — L348
- `UHodgeShapeQueryStrategy::Detect` — L355

## HodgeHitReactionTypes.cpp

[Source/Hodgepodge/Private/Combat/HodgeHitReactionTypes.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeHitReactionTypes.cpp)

模块定义或基础代码；请查看对应文件。

- `HodgeHitReaction::BodyRank` — L25
- `HodgeHitReaction::JudgementRank` — L36
- `HodgeHitReaction::ResolveBody` — L47
- `HodgeHitReaction::CanImpact` — L64
- `HodgeHitReaction::CanUseMontage` — L70
- `FHodgeImpactSpec::IsMovement` — L77
- `FHodgeImpactSpec::Validate` — L84
- `FHodgeHitReactionConfig::IsConfigured` — L99
- `FHodgeHitReactionConfig::Validate` — L105

## HodgeMovementActionTypes.cpp

[Source/Hodgepodge/Private/Combat/HodgeMovementActionTypes.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeMovementActionTypes.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeSprintInputSession::Begin` — L19
- `FHodgeSprintInputSession::Release` — L28
- `FHodgeSprintInputSession::Consume` — L30
- `FHodgeSprintInputSession::Qualifies` — L33
- `FHodgeDashDirections::Select` — L40

## HodgeActorComponentBase.cpp

[Source/Hodgepodge/Private/Component/HodgeActorComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeActorComponentBase.cpp)

通用 ActorComponent 基础访问与扩展。

- `UHodgeActorComponentBase::UHodgeActorComponentBase` — L24
- `UHodgeActorComponentBase::BeginPlay` — L40
- `UHodgeActorComponentBase::TickComponent` — L58
- `UHodgeActorComponentBase::GetOwnerCharacter` — L70
- `UHodgeActorComponentBase::InitializeComponent` — L81
- `UHodgeActorComponentBase::OnReady` — L92

## HodgeAttackMotionWarpingComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeAttackMotionWarpingComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeAttackMotionWarpingComponent.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeAttackMotionWarpingComponent::InitializeComponent` — L15
- `UHodgeAttackMotionWarpingComponent::ClearAttackWarp` — L19
- `UHodgeAttackMotionWarpingComponent::PrepareAttackEvaluation` — L26
- `UHodgeAttackMotionWarpingComponent::PrepareAttackTarget` — L47

## HodgeCharacterMovementComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeCharacterMovementComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCharacterMovementComponent.cpp)

地面距离/加速度、最终旋转过滤、SavedMove 旋转策略重放与权威校正。

- `UHodgeCharacterMovementComponent::UHodgeCharacterMovementComponent` — L200
- `UHodgeCharacterMovementComponent::PerformMovement` — L208
- `UHodgeCharacterMovementComponent::ClientHandleMoveResponse` — L238
- `UHodgeCharacterMovementComponent::ClientAdjustPosition_Implementation` — L265
- `UHodgeCharacterMovementComponent::SimulateMovement` — L275
- `UHodgeCharacterMovementComponent::CanAttemptJump` — L296
- `UHodgeCharacterMovementComponent::InitializeComponent` — L306
- `UHodgeCharacterMovementComponent::GetGroundInfo` — L313
- `UHodgeCharacterMovementComponent::SetReplicatedAcceleration` — L394
- `UHodgeCharacterMovementComponent::GetDeltaRotation` — L404
- `UHodgeCharacterMovementComponent::PhysicsRotation` — L433
- `UHodgeCharacterMovementComponent::ServerMove_PerformMovement` — L489
- `UHodgeCharacterMovementComponent::MoveUpdatedComponentImpl` — L505
- `UHodgeCharacterMovementComponent::GetPredictionData_Client` — L525
- `UHodgeCharacterMovementComponent::ClientUpdatePositionAfterServerUpdate` — L537
- `UHodgeCharacterMovementComponent::SmoothCorrection` — L552
- `UHodgeCharacterMovementComponent::GetMaxSpeed` — L560
- `UHodgeCharacterMovementComponent::GetMaxAcceleration` — L580
- `UHodgeCharacterMovementComponent::GetMaxBrakingDeceleration` — L587
- `UHodgeCharacterMovementComponent::ConstrainInputAcceleration` — L594
- `UHodgeCharacterMovementComponent::IsHitReactionControlled` — L608
- `UHodgeCharacterMovementComponent::ApplyRequestedMove` — L617

## HodgeCharacterRotationComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeCharacterRotationComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCharacterRotationComponent.cpp)

ASC 旋转标签约束、锁定 Yaw、恢复、权威复制与移动重放状态。

- `UHodgeCharacterRotationComponent::UHodgeCharacterRotationComponent` — L22
- `UHodgeCharacterRotationComponent::InitializeWithAbilitySystem` — L32
- `UHodgeCharacterRotationComponent::UninitializeFromAbilitySystem` — L64
- `UHodgeCharacterRotationComponent::HandleConstraintTagChanged` — L112
- `UHodgeCharacterRotationComponent::RefreshLocalState` — L118
- `UHodgeCharacterRotationComponent::PublishAuthorityState` — L143
- `UHodgeCharacterRotationComponent::IsReplayingMove` — L163
- `UHodgeCharacterRotationComponent::GetResolvedState` — L170
- `UHodgeCharacterRotationComponent::IsYawLocked` — L182
- `UHodgeCharacterRotationComponent::IsRecoveringFacing` — L184
- `UHodgeCharacterRotationComponent::GetLockedYaw` — L186
- `UHodgeCharacterRotationComponent::FilterControlRotation` — L189
- `UHodgeCharacterRotationComponent::NotifyFacingApplied` — L206
- `UHodgeCharacterRotationComponent::SetMoveReplayState` — L225
- `UHodgeCharacterRotationComponent::ClearMoveReplayState` — L233
- `UHodgeCharacterRotationComponent::OnRep_RotationState` — L241
- `UHodgeCharacterRotationComponent::GetLifetimeReplicatedProps` — L254
- `UHodgeCharacterRotationComponent::EndPlay` — L261
- `UHodgeCharacterRotationComponent::IsFacingSystemReady` — L268
- `UHodgeCharacterRotationComponent::IsValidSource` — L275
- `UHodgeCharacterRotationComponent::HasPendingPrediction` — L288
- `UHodgeCharacterRotationComponent::IsAuthorityActionCurrent` — L296
- `UHodgeCharacterRotationComponent::AcquireBaseFacingMode` — L314
- `UHodgeCharacterRotationComponent::AcquireLockOnFacing` — L328
- `UHodgeCharacterRotationComponent::RequestActionFacing` — L337
- `UHodgeCharacterRotationComponent::RequestMontageWarpFacing` — L352
- `UHodgeCharacterRotationComponent::SetFacingRequestSuspended` — L365
- `UHodgeCharacterRotationComponent::IsFacingRequestSelected` — L371
- `UHodgeCharacterRotationComponent::AddRequest` — L378
- `UHodgeCharacterRotationComponent::ReleaseFacingRequest` — L404
- `UHodgeCharacterRotationComponent::ReleaseRequestsForSource` — L421
- `UHodgeCharacterRotationComponent::GetFacingRequestStatus` — L430
- `UHodgeCharacterRotationComponent::IsActionFacingApplied` — L440
- `UHodgeCharacterRotationComponent::SetRequestStatus` — L453
- `UHodgeCharacterRotationComponent::ResolveRequests` — L464
- `UHodgeCharacterRotationComponent::ApplyResolvedMode` — L530
- `UHodgeCharacterRotationComponent::GetFacingPresentationSnapshot` — L543
- `UHodgeCharacterRotationComponent::TickComponent` — L552
- `UHodgeCharacterRotationComponent::SendRequest` — L585
- `UHodgeCharacterRotationComponent::ServerFacingRequest_Implementation` — L598
- `UHodgeCharacterRotationComponent::ClientFacingResult_Implementation` — L652
- `UHodgeCharacterRotationComponent::BeginServerMove` — L664
- `UHodgeCharacterRotationComponent::EndServerMove` — L688
- `UHodgeCharacterRotationComponent::IsDataValid` — L696

## HodgeCombatComponentBase.cpp

[Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp)

Experience 注入 Pawn 的统一战斗协调者；连段输入/记忆/预测校正、来源/配置检测体、独立会话与过滤；GA 负责目标去重及效果施加。

- `UHodgeCombatComponentBase::UHodgeCombatComponentBase` — L61
- `UHodgeCombatComponentBase::AcquirePoseLease` — L70
- `UHodgeCombatComponentBase::ReleasePoseLease` — L88
- `UHodgeCombatComponentBase::EndPlay` — L101
- `UHodgeCombatComponentBase::ResolveSource` — L110
- `UHodgeCombatComponentBase::IsSourceValid` — L146
- `UHodgeCombatComponentBase::CaptureDetectionGeometry` — L176
- `UHodgeCombatComponentBase::CreateDetectionSession` — L222
- `UHodgeCombatComponentBase::IsDetectionSessionValid` — L283
- `UHodgeCombatComponentBase::ResetDetectionHistory` — L290
- `UHodgeCombatComponentBase::GetDetectionSourceComponent` — L303
- `UHodgeCombatComponentBase::EndDetectionSession` — L312
- `UHodgeCombatComponentBase::EndDetectionSessionsForExecution` — L319
- `UHodgeCombatComponentBase::EndAllDetectionSessions` — L328
- `UHodgeCombatComponentBase::UpdateDetectionAnchor` — L334
- `UHodgeCombatComponentBase::SampleDetection` — L352
- `UHodgeCombatComponentBase::Configure` — L476
- `UHodgeCombatComponentBase::Shutdown` — L499
- `UHodgeCombatComponentBase::ConsumeBufferedInput` — L541
- `UHodgeCombatComponentBase::ClearInput` — L547
- `UHodgeCombatComponentBase::InputPressed` — L555
- `UHodgeCombatComponentBase::SelectTransition` — L568
- `UHodgeCombatComponentBase::IsAuthorized` — L591
- `UHodgeCombatComponentBase::IsMoveCancelPredicted` — L597
- `UHodgeCombatComponentBase::GetMoveCancelAbility` — L607
- `UHodgeCombatComponentBase::GetMoveCancelPolicy` — L618
- `UHodgeCombatComponentBase::ClearMoveCancelRequest` — L638
- `UHodgeCombatComponentBase::UpdateMoveCancel` — L645
- `UHodgeCombatComponentBase::ExecutionKey` — L673
- `UHodgeCombatComponentBase::PrepareTransition` — L682
- `UHodgeCombatComponentBase::TryTransition` — L721
- `UHodgeCombatComponentBase::GetServerInputBufferSeconds` — L773
- `UHodgeCombatComponentBase::ValidateServerRequestIdentity` — L779
- `UHodgeCombatComponentBase::CanBufferServerActivation` — L790
- `UHodgeCombatComponentBase::PrepareServerActivation` — L811
- `UHodgeCombatComponentBase::PrepareConfirmedActivation` — L825
- `UHodgeCombatComponentBase::RejectServerActivation` — L850
- `UHodgeCombatComponentBase::CompleteServerActivation` — L857
- `UHodgeCombatComponentBase::ExecutionStarted` — L884
- `UHodgeCombatComponentBase::ExecutionEnded` — L897
- `UHodgeCombatComponentBase::WindowsChanged` — L914
- `UHodgeCombatComponentBase::ExecutionEvent` — L926
- `UHodgeCombatComponentBase::DrainEvents` — L935
- `UHodgeCombatComponentBase::SetNode` — L954
- `UHodgeCombatComponentBase::ResetSession` — L971
- `UHodgeCombatComponentBase::ComboTime` — L987
- `UHodgeCombatComponentBase::GetRememberedComboTag` — L996
- `UHodgeCombatComponentBase::GetComboMemoryRemainingTime` — L1004
- `UHodgeCombatComponentBase::TransitionSourceNode` — L1012
- `UHodgeCombatComponentBase::HasResumeTransition` — L1020
- `UHodgeCombatComponentBase::RetainComboMemory` — L1030
- `UHodgeCombatComponentBase::ExpireComboMemory` — L1042
- `UHodgeCombatComponentBase::PublishComboMemory` — L1054
- `UHodgeCombatComponentBase::EndCurrentExecution` — L1065
- `UHodgeCombatComponentBase::OnRep_ComboMemory` — L1072
- `UHodgeCombatComponentBase::HandlePredictionRejected` — L1082
- `UHodgeCombatComponentBase::ServerSynchronizeComboMemory_Implementation` — L1092
- `UHodgeCombatComponentBase::ClientCorrectComboMemory_Implementation` — L1099
- `UHodgeCombatComponentBase::TickComponent` — L1106
- `UHodgeCombatComponentBase::ServerMoveCancel_Implementation` — L1129
- `UHodgeCombatComponentBase::ProcessServerMoveCancel` — L1142
- `UHodgeCombatComponentBase::ClientMoveCancelResult_Implementation` — L1165
- `UHodgeCombatComponentBase::ServerReturnToEntry_Implementation` — L1188
- `UHodgeCombatComponentBase::GetLifetimeReplicatedProps` — L1201
- `UHodgeCombatComponentBase::OnRep_ObserverTags` — L1209
- `UHodgeCombatComponentBase::FindCombatComponent` — L1222
- `UHodgeCombatComponentBase::IsComboReady` — L1228
- `UHodgeCombatComponentBase::BindPawnExtension` — L1237
- `UHodgeCombatComponentBase::HandleAbilitySystemInitialized` — L1249
- `UHodgeCombatComponentBase::HandleAbilitySystemUninitialized` — L1259
- `UHodgeCombatComponentBase::OnRegister` — L1265
- `UHodgeCombatComponentBase::BeginPlay` — L1273
- `UHodgeCombatComponentBase::OnUnregister` — L1281
- `UHodgeCombatComponentBase::IsCombatReadyForActions` — L1292
- `UHodgeCombatComponentBase::AddActionRequest` — L1310
- `UHodgeCombatComponentBase::GetCombatActionRequest` — L1331
- `UHodgeCombatComponentBase::SetActionRequestStatus` — L1338
- `UHodgeCombatComponentBase::RequestStandaloneAbility` — L1364
- `UHodgeCombatComponentBase::SubmitCombatIntent` — L1393
- `UHodgeCombatComponentBase::BindActionRequest` — L1429
- `UHodgeCombatComponentBase::ConfirmActionRequest` — L1440
- `UHodgeCombatComponentBase::HandleActionAbilityEnded` — L1455
- `UHodgeCombatComponentBase::CancelCombatActionRequest` — L1472
- `UHodgeCombatComponentBase::CloseActionRequests` — L1492
- `UHodgeCombatComponentBase::InvalidateModeContinuation` — L1505
- `UHodgeCombatComponentBase::HandleCombatModeChanged` — L1511
- `UHodgeCombatComponentBase::ClearModeContinuation` — L1519

## HodgeDefenseComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeDefenseComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeDefenseComponent.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeDefenseComponent::InitializeWithAbilitySystem` — L15
- `UHodgeDefenseComponent::UninitializeFromAbilitySystem` — L22
- `UHodgeDefenseComponent::RegisterDodgeWindow` — L24
- `UHodgeDefenseComponent::UnregisterDodgeWindow` — L35
- `UHodgeDefenseComponent::EvaluateWindow` — L37
- `UHodgeDefenseComponent::ResolveIncomingHit` — L43
- `UHodgeDefenseComponent::EndPlay` — L83

## HodgeEnemyInitComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeEnemyInitComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeEnemyInitComponent.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeEnemyInitComponent::NAME_ActorFeatureName` — L23
- `UHodgeEnemyInitComponent::UHodgeEnemyInitComponent` — L26
- `UHodgeEnemyInitComponent::OnRegister` — L33
- `UHodgeEnemyInitComponent::OnUnregister` — L40
- `UHodgeEnemyInitComponent::BeginPlay` — L53
- `UHodgeEnemyInitComponent::BindDependencies` — L64
- `UHodgeEnemyInitComponent::HandleGameStateSet` — L87
- `UHodgeEnemyInitComponent::HandleExperienceLoaded` — L89
- `UHodgeEnemyInitComponent::CheckDefaultInitialization` — L95
- `UHodgeEnemyInitComponent::OnActorInitStateChanged` — L97
- `UHodgeEnemyInitComponent::CanChangeInitState` — L100
- `UHodgeEnemyInitComponent::TryInitialize` — L115
- `UHodgeEnemyInitComponent::InitializeStep` — L132
- `UHodgeEnemyInitComponent::IsBindingCurrent` — L239
- `UHodgeEnemyInitComponent::IsGameplayReady` — L247
- `UHodgeEnemyInitComponent::HasValidCombatDependencies` — L255
- `UHodgeEnemyInitComponent::CanExecuteAIAction` — L265
- `UHodgeEnemyInitComponent::Wait` — L275
- `UHodgeEnemyInitComponent::Fail` — L279
- `UHodgeEnemyInitComponent::HandleASCUninitialized` — L288
- `UHodgeEnemyInitComponent::Cleanup` — L296
- `UHodgeEnemyInitComponent::EndPlay` — L309

## HodgeExperienceManagerComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeExperienceManagerComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeExperienceManagerComponent.cpp)

GameState 上的 Experience 复制、资源加载、插件激活、Action 执行与 Loaded 委托。

- `UHodgeExperienceManagerComponent::UHodgeExperienceManagerComponent` — L66
- `UHodgeExperienceManagerComponent::SetCurrentExperience` — L73
- `UHodgeExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_HighPriority` — L104
- `UHodgeExperienceManagerComponent::CallOrRegister_OnExperienceLoaded` — L120
- `UHodgeExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_LowPriority` — L135
- `UHodgeExperienceManagerComponent::GetCurrentExperienceChecked` — L151
- `UHodgeExperienceManagerComponent::IsExperienceLoaded` — L164
- `UHodgeExperienceManagerComponent::OnRep_CurrentExperience` — L171
- `UHodgeExperienceManagerComponent::StartExperienceLoad` — L178
- `UHodgeExperienceManagerComponent::OnExperienceLoadComplete` — L317
- `UHodgeExperienceManagerComponent::OnGameFeaturePluginLoadComplete` — L402
- `UHodgeExperienceManagerComponent::OnExperienceFullLoadCompleted` — L415
- `UHodgeExperienceManagerComponent::OnActionDeactivationCompleted` — L514
- `UHodgeExperienceManagerComponent::GetLifetimeReplicatedProps` — L530
- `UHodgeExperienceManagerComponent::EndPlay` — L539
- `UHodgeExperienceManagerComponent::ShouldShowLoadingScreen` — L630
- `UHodgeExperienceManagerComponent::OnAllActionsDeactivated` — L648

## HodgeHealthComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeHealthComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeHealthComponent.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeHealthComponent::UHodgeHealthComponent` — L35
- `UHodgeHealthComponent::GetLifetimeReplicatedProps` — L58
- `UHodgeHealthComponent::OnUnregister` — L68
- `UHodgeHealthComponent::InitializeWithAbilitySystem` — L78
- `UHodgeHealthComponent::ResetForSpawn` — L141
- `UHodgeHealthComponent::UninitializeFromAbilitySystem` — L151
- `UHodgeHealthComponent::ClearGameplayTags` — L176
- `UHodgeHealthComponent::GetHealth` — L190
- `UHodgeHealthComponent::GetMaxHealth` — L197
- `UHodgeHealthComponent::GetHealthNormalized` — L204
- `UHodgeHealthComponent::HandleHealthChanged` — L224
- `UHodgeHealthComponent::HandleMaxHealthChanged` — L233
- `UHodgeHealthComponent::HandleOutOfHealth` — L242
- `UHodgeHealthComponent::OnRep_DeathState` — L316
- `UHodgeHealthComponent::StartDeath` — L388
- `UHodgeHealthComponent::FinishDeath` — L429
- `UHodgeHealthComponent::DamageSelfDestruct` — L461

## HodgeHeroComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp)

玩家 Init State 协调、ASC 接入、输入与相机；额外输入句柄持久化并在移除/EndPlay 解绑。新增“移动意图”信号（HasMoveIntent / GetMoveIntent / OnMoveIntentChanged），记 Input_Move 原始输入量、Completed/Canceled 清零，供移动取消后摇消费。

- `UHodgeHeroComponent::NAME_BindInputsNow` — L104
- `UHodgeHeroComponent::NAME_ActorFeatureName` — L107
- `UHodgeHeroComponent::UHodgeHeroComponent` — L110
- `UHodgeHeroComponent::OnRegister` — L121
- `UHodgeHeroComponent::CanChangeInitState` — L165
- `UHodgeHeroComponent::HandleChangeInitState` — L284
- `UHodgeHeroComponent::OnActorInitStateChanged` — L351
- `UHodgeHeroComponent::CheckDefaultInitialization` — L367
- `UHodgeHeroComponent::BeginPlay` — L387
- `UHodgeHeroComponent::EndPlay` — L411
- `UHodgeHeroComponent::InitializePlayerInput` — L438
- `UHodgeHeroComponent::AddAdditionalInputConfig` — L664
- `UHodgeHeroComponent::RemoveAdditionalInputConfig` — L755
- `UHodgeHeroComponent::IsReadyToBindInputs` — L784
- `UHodgeHeroComponent::Input_AbilityInputTagPressed` — L791
- `UHodgeHeroComponent::Input_AbilityInputTagReleased` — L820
- `UHodgeHeroComponent::Input_Move` — L852
- `UHodgeHeroComponent::Input_MoveStopped` — L934
- `UHodgeHeroComponent::HasMoveIntent` — L942
- `UHodgeHeroComponent::SetMoveIntent` — L949
- `UHodgeHeroComponent::RefreshMoveIntent` — L965
- `UHodgeHeroComponent::Input_LookMouse` — L980
- `UHodgeHeroComponent::Input_LookStick` — L1034
- `UHodgeHeroComponent::Input_Crouch` — L1083
- `UHodgeHeroComponent::Input_AutoRun` — L1097
- `UHodgeHeroComponent::DetermineCameraMode` — L1118
- `UHodgeHeroComponent::SetAbilityCameraMode` — L1155
- `UHodgeHeroComponent::ClearAbilityCameraMode` — L1180
- `UHodgeHeroComponent::ClearAbilityCameraMode` — L1186
- `UHodgeHeroComponent::EndCinematicCameraForExecution` — L1204
- `UHodgeHeroComponent::AbortCinematicCamera` — L1209
- `UHodgeHeroComponent::RefreshCinematicCamera` — L1217
- `UHodgeHeroComponent::ResetGameplayInput` — L1267
- `UHodgeHeroComponent::BeginSprintInput` — L1276
- `UHodgeHeroComponent::EndSprintInput` — L1301
- `UHodgeHeroComponent::NotifyDashCommitted` — L1322
- `UHodgeHeroComponent::NotifySprintCommitted` — L1328
- `UHodgeHeroComponent::NotifyMovementActionEnded` — L1334
- `UHodgeHeroComponent::RequestSprintHandoff` — L1339
- `UHodgeHeroComponent::BeginCircuitHold` — L1357
- `UHodgeHeroComponent::EndCircuitHold` — L1363
- `UHodgeHeroComponent::UpdateCircuitHold` — L1370

## HodgeHitReactionComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeHitReactionComponent::UHodgeHitReactionComponent` — L27
- `UHodgeHitReactionComponent::IsBound` — L36
- `UHodgeHitReactionComponent::InitializeWithAbilitySystem` — L42
- `UHodgeHitReactionComponent::UninitializeFromAbilitySystem` — L52
- `UHodgeHitReactionComponent::RefreshConfiguration` — L71
- `UHodgeHitReactionComponent::BeginHit` — L87
- `UHodgeHitReactionComponent::RecordDamage` — L103
- `UHodgeHitReactionComponent::FinishHit` — L116
- `UHodgeHitReactionComponent::BuildPlan` — L170
- `UHodgeHitReactionComponent::SubmitPlan` — L259
- `UHodgeHitReactionComponent::SetControl` — L314
- `UHodgeHitReactionComponent::PublishState` — L325
- `UHodgeHitReactionComponent::OnRep_State` — L332
- `UHodgeHitReactionComponent::OnRep_Profile` — L350
- `UHodgeHitReactionComponent::AttachAbility` — L353
- `UHodgeHitReactionComponent::DetachAbility` — L360
- `UHodgeHitReactionComponent::CompletePlan` — L369
- `UHodgeHitReactionComponent::CancelReaction` — L383
- `UHodgeHitReactionComponent::ChangePhase` — L390
- `UHodgeHitReactionComponent::RequestGetUp` — L400
- `UHodgeHitReactionComponent::TickPlan` — L410
- `UHodgeHitReactionComponent::GetServerTime` — L453
- `UHodgeHitReactionComponent::TickComponent` — L461
- `UHodgeHitReactionComponent::MulticastLightFeedback_Implementation` — L485
- `UHodgeHitReactionComponent::GetLifetimeReplicatedProps` — L502
- `UHodgeHitReactionComponent::EndPlay` — L510

## HodgeInteractionComponentBase.cpp

[Source/Hodgepodge/Private/Component/HodgeInteractionComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeInteractionComponentBase.cpp)

交互组件基础占位；完整扫描、交互规则与 UI 需另行实现。

## HodgeLockOnComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeLockOnComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeLockOnComponent.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeLockOnComponent::NAME_ActorFeatureName` — L27
- `UHodgeLockOnComponent::UHodgeLockOnComponent` — L28
- `UHodgeLockOnComponent::OnRegister` — L33
- `UHodgeLockOnComponent::BeginPlay` — L34
- `UHodgeLockOnComponent::CanChangeInitState` — L46
- `UHodgeLockOnComponent::OnActorInitStateChanged` — L59
- `UHodgeLockOnComponent::CheckDefaultInitialization` — L60
- `UHodgeLockOnComponent::InitializeFromExtension` — L75
- `UHodgeLockOnComponent::IsReady` — L95
- `UHodgeLockOnComponent::Uninitialize` — L97
- `UHodgeLockOnComponent::EndPlay` — L116
- `UHodgeLockOnComponent::OnUnregister` — L118
- `UHodgeLockOnComponent::GetLifetimeReplicatedProps` — L124
- `UHodgeLockOnComponent::GetPresentationTarget` — L126
- `UHodgeLockOnComponent::GetPresentationLockPoint` — L136
- `UHodgeLockOnComponent::IsConfirmedTargetValid` — L142
- `UHodgeLockOnComponent::HasSight` — L147
- `UHodgeLockOnComponent::IsTargetValid` — L156
- `UHodgeLockOnComponent::SelectCandidate` — L168
- `UHodgeLockOnComponent::ToggleLock` — L205
- `UHodgeLockOnComponent::TryLockTarget` — L210
- `UHodgeLockOnComponent::SwitchTarget` — L215
- `UHodgeLockOnComponent::ClearLock` — L220
- `UHodgeLockOnComponent::SubmitCommand` — L227
- `UHodgeLockOnComponent::DispatchQueuedCommand` — L236
- `UHodgeLockOnComponent::TakeCommand` — L252
- `UHodgeLockOnComponent::ProcessAuthorityCommand` — L258
- `UHodgeLockOnComponent::PublishTarget` — L278
- `UHodgeLockOnComponent::ClientCommandResult_Implementation` — L292
- `UHodgeLockOnComponent::OnRep_ConfirmedState` — L293
- `UHodgeLockOnComponent::ApplyResult` — L294
- `UHodgeLockOnComponent::NotifyCommandEnded` — L305
- `UHodgeLockOnComponent::RejectPrediction` — L317
- `UHodgeLockOnComponent::CanOwnControllerFacing` — L322
- `UHodgeLockOnComponent::RefreshFacing` — L331
- `UHodgeLockOnComponent::ResolveControlRotation` — L338
- `UHodgeLockOnComponent::SeedCameraView` — L346
- `UHodgeLockOnComponent::UpdateCameraView` — L350
- `UHodgeLockOnComponent::SubmitLookIntent` — L356
- `UHodgeLockOnComponent::SynchronizePresentation` — L367
- `UHodgeLockOnComponent::HandleConstraint` — L403
- `UHodgeLockOnComponent::HandleTargetDeath` — L405
- `UHodgeLockOnComponent::HandleTargetDestroyed` — L407
- `UHodgeLockOnComponent::CheckTargetValidity` — L409
- `UHodgeLockOnComponent::TickComponent` — L430

## HodgeLocomotionPolicyComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeLocomotionPolicyComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeLocomotionPolicyComponent.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeLocomotionPolicyComponent::UHodgeLocomotionPolicyComponent` — L17
- `UHodgeLocomotionPolicyComponent::InitializeWithAbilitySystem` — L22
- `UHodgeLocomotionPolicyComponent::UninitializeFromAbilitySystem` — L30
- `UHodgeLocomotionPolicyComponent::GetProfile` — L39
- `UHodgeLocomotionPolicyComponent::AcquireSprint` — L47
- `UHodgeLocomotionPolicyComponent::ReleasePolicy` — L60
- `UHodgeLocomotionPolicyComponent::GetResolvedPolicy` — L70
- `UHodgeLocomotionPolicyComponent::AcquireSpeedModifier` — L83
- `UHodgeLocomotionPolicyComponent::ReleaseSpeedModifier` — L93
- `UHodgeLocomotionPolicyComponent::RefreshSpeedScale` — L96
- `UHodgeLocomotionPolicyComponent::SetMoveReplayState` — L109
- `UHodgeLocomotionPolicyComponent::ClearMoveReplayState` — L111
- `UHodgeLocomotionPolicyComponent::BeginServerMove` — L113
- `UHodgeLocomotionPolicyComponent::EndServerMove` — L119
- `UHodgeLocomotionPolicyComponent::TickComponent` — L121
- `UHodgeLocomotionPolicyComponent::SetSprintInput` — L134
- `UHodgeLocomotionPolicyComponent::SetInputInternal` — L144
- `UHodgeLocomotionPolicyComponent::ServerSetSprintInput_Implementation` — L152
- `UHodgeLocomotionPolicyComponent::IsSessionHeld` — L155
- `UHodgeLocomotionPolicyComponent::SessionHeldTime` — L157
- `UHodgeLocomotionPolicyComponent::AuthorizeHandoff` — L160
- `UHodgeLocomotionPolicyComponent::CanHandoff` — L166
- `UHodgeLocomotionPolicyComponent::ConsumeHandoff` — L169
- `UHodgeLocomotionPolicyComponent::RecordDashExitVelocity` — L171
- `UHodgeLocomotionPolicyComponent::GetDashExitVelocity` — L180
- `UHodgeLocomotionPolicyComponent::RecordDashMotionVelocity` — L187
- `UHodgeLocomotionPolicyComponent::GetDashFinishVelocity` — L194
- `UHodgeLocomotionPolicyComponent::OnRep_State` — L204
- `UHodgeLocomotionPolicyComponent::GetLifetimeReplicatedProps` — L211
- `UHodgeLocomotionPolicyComponent::EndPlay` — L214

## HodgeMovementComponentBase.cpp

[Source/Hodgepodge/Private/Component/HodgeMovementComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeMovementComponentBase.cpp)

通用移动组件基础占位，与 CharacterMovement 派生类需区分。

## HodgePawnExtensionComponent.cpp

[Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp)

PawnData 复制、Init State、ASC 关联/解除、TagRelationshipMapping 与 ClearAbilityInput；需验证退出顺序。

- `UHodgePawnExtensionComponent::NAME_ActorFeatureName` — L21
- `UHodgePawnExtensionComponent::UHodgePawnExtensionComponent` — L24
- `UHodgePawnExtensionComponent::GetLifetimeReplicatedProps` — L39
- `UHodgePawnExtensionComponent::OnRegister` — L48
- `UHodgePawnExtensionComponent::BeginPlay` — L68
- `UHodgePawnExtensionComponent::EndPlay` — L83
- `UHodgePawnExtensionComponent::SetPawnData` — L95
- `UHodgePawnExtensionComponent::OnRep_PawnData` — L129
- `UHodgePawnExtensionComponent::InitializeAbilitySystem` — L138
- `UHodgePawnExtensionComponent::UninitializeAbilitySystem` — L200
- `UHodgePawnExtensionComponent::HandleControllerChanged` — L246
- `UHodgePawnExtensionComponent::HandlePlayerStateReplicated` — L271
- `UHodgePawnExtensionComponent::SetupPlayerInputComponent` — L278
- `UHodgePawnExtensionComponent::CheckDefaultInitialization` — L285
- `UHodgePawnExtensionComponent::CanChangeInitState` — L301
- `UHodgePawnExtensionComponent::HandleChangeInitState` — L363
- `UHodgePawnExtensionComponent::OnActorInitStateChanged` — L374
- `UHodgePawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall` — L387
- `UHodgePawnExtensionComponent::OnAbilitySystemUninitialized_Register` — L404
- `UHodgePawnExtensionComponent::UnregisterAbilitySystemDelegates` — L414

## HodgeWingComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeWingComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeWingComponent.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeWingComponent::UHodgeWingComponent` — L20
- `UHodgeWingComponent::BeginPlay` — L29
- `UHodgeWingComponent::InitializeWing` — L36
- `UHodgeWingComponent::SetWingVisible` — L135
- `UHodgeWingComponent::ApplyFadeVisibility` — L147
- `UHodgeWingComponent::BeginFadeOut` — L156
- `UHodgeWingComponent::IsOwnerPresentationBlocked` — L162
- `UHodgeWingComponent::ResolveSource` — L168
- `UHodgeWingComponent::BeginWingAction` — L234
- `UHodgeWingComponent::EndWingAction` — L240
- `UHodgeWingComponent::EndWingForExecution` — L252
- `UHodgeWingComponent::ResetWingPresentation` — L257
- `UHodgeWingComponent::EvaluateWindowPose` — L272
- `UHodgeWingComponent::PrepareAuthorityHitPose` — L302
- `UHodgeWingComponent::RequestPresentationRefresh` — L336
- `UHodgeWingComponent::RefreshPresentation` — L344
- `UHodgeWingComponent::UpdatePose` — L398
- `UHodgeWingComponent::TickComponent` — L422
- `UHodgeWingComponent::EndPlay` — L444

## HodgeGameInstanceBase.cpp

[Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp](../../../Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp)

注册 Init State 顺序、主控制器访问和全局生命周期扩展。

- `UHodgeGameInstanceBase::UHodgeGameInstanceBase` — L22
- `UHodgeGameInstanceBase::GetPrimaryPlayerController` — L27
- `UHodgeGameInstanceBase::Shutdown` — L33
- `UHodgeGameInstanceBase::Init` — L39
- `UHodgeGameInstanceBase::AddLocalPlayer` — L89
- `UHodgeGameInstanceBase::RemoveLocalPlayer` — L100

## HodgeGameModeBase.cpp

[Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp](../../../Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp)

服务器选择玩法、等待加载、生成 Pawn，并在 FinishSpawning 前注入 PawnData；选用 HodgePlayerController。

- `AHodgeGameModeBase::AHodgeGameModeBase` — L25
- `AHodgeGameModeBase::GetPawnDataForController` — L51
- `AHodgeGameModeBase::InitGame` — L93
- `AHodgeGameModeBase::HandleMatchAssignmentIfNotExpectingOne` — L103
- `AHodgeGameModeBase::TryDedicatedServerLogin` — L201
- `AHodgeGameModeBase::OnMatchAssignmentGiven` — L346
- `AHodgeGameModeBase::OnExperienceLoaded` — L371
- `AHodgeGameModeBase::IsExperienceLoaded` — L393
- `AHodgeGameModeBase::GetDefaultPawnClassForController_Implementation` — L408
- `AHodgeGameModeBase::SpawnDefaultPawnAtTransform_Implementation` — L425
- `AHodgeGameModeBase::ShouldSpawnAtStartSpot` — L483
- `AHodgeGameModeBase::HandleStartingNewPlayer_Implementation` — L490
- `AHodgeGameModeBase::ChoosePlayerStart_Implementation` — L501
- `AHodgeGameModeBase::FinishRestartPlayer` — L515
- `AHodgeGameModeBase::PlayerCanRestart_Implementation` — L529
- `AHodgeGameModeBase::ControllerCanRestart` — L536
- `AHodgeGameModeBase::InitGameState` — L566
- `AHodgeGameModeBase::GenericPlayerInitialization` — L582
- `AHodgeGameModeBase::RequestPlayerRestartNextFrame` — L592
- `AHodgeGameModeBase::UpdatePlayerStartSpot` — L614
- `AHodgeGameModeBase::FailedToRestartPlayer` — L621
- `AHodgeGameModeBase::RestartPlayer` — L661

## HodgeGameState.cpp

[Source/Hodgepodge/Private/Core/GameState/HodgeGameState.cpp](../../../Source/Hodgepodge/Private/Core/GameState/HodgeGameState.cpp)

创建 ExperienceManager 和世界状态 ASC，处理游戏状态复制/扩展。

- `AHodgeGameState::AHodgeGameState` — L29
- `AHodgeGameState::PreInitializeComponents` — L55
- `AHodgeGameState::PostInitializeComponents` — L62
- `AHodgeGameState::GetAbilitySystemComponent` — L74
- `AHodgeGameState::EndPlay` — L81
- `AHodgeGameState::AddPlayerState` — L88
- `AHodgeGameState::RemovePlayerState` — L95
- `AHodgeGameState::SeamlessTravelTransitionCheckpoint` — L103
- `AHodgeGameState::GetLifetimeReplicatedProps` — L120
- `AHodgeGameState::Tick` — L133
- `AHodgeGameState::GetServerFPS` — L160
- `AHodgeGameState::SetRecorderPlayerState` — L167
- `AHodgeGameState::GetRecorderPlayerState` — L187
- `AHodgeGameState::OnRep_RecorderPlayerState` — L194

## HodgeGameStateBase.cpp

[Source/Hodgepodge/Private/Core/GameState/HodgeGameStateBase.cpp](../../../Source/Hodgepodge/Private/Core/GameState/HodgeGameStateBase.cpp)

GameState 基础扩展生命周期。

- `AHodgeGameStateBase::AHodgeGameStateBase` — L17
- `AHodgeGameStateBase::PreInitializeComponents` — L23
- `AHodgeGameStateBase::BeginPlay` — L32
- `AHodgeGameStateBase::EndPlay` — L42

## HodgeHUD.cpp

[Source/Hodgepodge/Private/Core/HUD/HodgeHUD.cpp](../../../Source/Hodgepodge/Private/Core/HUD/HodgeHUD.cpp)

HUD Receiver 提供 Experience AddWidgets 的真实注入／撤销入口，同时保留 GAS 调试 Actor 列表。

- `AHodgeHUD::AHodgeHUD` — L40
- `AHodgeHUD::PreInitializeComponents` — L49
- `AHodgeHUD::BeginPlay` — L62
- `AHodgeHUD::EndPlay` — L76
- `AHodgeHUD::GetDebugActorList` — L88

## HodgeLocalPlayerBase.cpp

[Source/Hodgepodge/Private/Core/LocalPlayer/HodgeLocalPlayerBase.cpp](../../../Source/Hodgepodge/Private/Core/LocalPlayer/HodgeLocalPlayerBase.cpp)

本地玩家对象及控制器、PlayerState、Pawn 就绪事件桥。

- `UHodgeLocalPlayerBase::UHodgeLocalPlayerBase` — L23
- `UHodgeLocalPlayerBase::CallAndRegister_OnPlayerControllerSet` — L40
- `UHodgeLocalPlayerBase::CallAndRegister_OnPlayerStateSet` — L67
- `UHodgeLocalPlayerBase::CallAndRegister_OnPlayerPawnSet` — L94
- `UHodgeLocalPlayerBase::GetProjectionData` — L128

## HodgePlayerController.cpp

[Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerController.cpp](../../../Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerController.cpp)

具体控制器：每帧消费 ASC 输入、相机管理、AutoRun、UnPossess Avatar 清理及 Replay 扩展。

- `AHodgePlayerController::AHodgePlayerController` — L107
- `AHodgePlayerController::PreInitializeComponents` — L121
- `AHodgePlayerController::BeginPlay` — L128
- `AHodgePlayerController::EndPlay` — L153
- `AHodgePlayerController::GetLifetimeReplicatedProps` — L160
- `AHodgePlayerController::ReceivedPlayer` — L183
- `AHodgePlayerController::PlayerTick` — L190
- `AHodgePlayerController::GetHodgePlayerState` — L255
- `AHodgePlayerController::GetHodgeAbilitySystemComponent` — L262
- `AHodgePlayerController::GetHodgeHUD` — L272
- `AHodgePlayerController::TryToRecordClientReplay` — L279
- `AHodgePlayerController::ShouldRecordClientReplay` — L308
- `AHodgePlayerController::OnPlayerStateChangedTeam` — L375
- `AHodgePlayerController::OnPlayerStateChanged` — L382
- `AHodgePlayerController::BroadcastOnPlayerStateChanged` — L389
- `AHodgePlayerController::InitPlayerState` — L434
- `AHodgePlayerController::CleanupPlayerState` — L444
- `AHodgePlayerController::OnRep_PlayerState` — L454
- `AHodgePlayerController::SetPlayer` — L464
- `AHodgePlayerController::AddCheats` — L487
- `AHodgePlayerController::ServerCheat_Implementation` — L498
- `AHodgePlayerController::HodgeFillResonance` — L507
- `AHodgePlayerController::HodgeFillDarkFlow` — L536
- `AHodgePlayerController::ServerCheat_Validate` — L567
- `AHodgePlayerController::ServerCheatAll_Implementation` — L574
- `AHodgePlayerController::ServerCheatAll_Validate` — L594
- `AHodgePlayerController::PreProcessInput` — L601
- `AHodgePlayerController::PostProcessInput` — L608
- `AHodgePlayerController::OnCameraPenetratingTarget` — L623
- `AHodgePlayerController::OnPossess` — L630
- `AHodgePlayerController::SetIsAutoRunning` — L655
- `AHodgePlayerController::GetIsAutoRunning` — L678
- `AHodgePlayerController::OnStartAutoRun` — L695
- `AHodgePlayerController::OnEndAutoRun` — L709
- `AHodgePlayerController::UpdateForceFeedback` — L723
- `AHodgePlayerController::UpdateHiddenComponents` — L746
- `AHodgePlayerController::OnUnPossess` — L856
- `AHodgeReplayPlayerController::Tick` — L885
- `AHodgeReplayPlayerController::SmoothTargetViewRotation` — L923
- `AHodgeReplayPlayerController::ShouldRecordClientReplay` — L933
- `AHodgeReplayPlayerController::RecorderPlayerStateUpdated` — L940
- `AHodgeReplayPlayerController::OnPlayerStatePawnSet` — L960

## HodgePlayerControllerBase.cpp

[Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerControllerBase.cpp](../../../Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerControllerBase.cpp)

控制器和 Pawn 生命周期桥接到 LocalPlayer 委托；具体输入消费在派生 HodgePlayerController。

- `AHodgePlayerControllerBase::AHodgePlayerControllerBase` — L25
- `AHodgePlayerControllerBase::ReceivedPlayer` — L53
- `AHodgePlayerControllerBase::SetPawn` — L85
- `AHodgePlayerControllerBase::OnPossess` — L121
- `AHodgePlayerControllerBase::OnUnPossess` — L153
- `AHodgePlayerControllerBase::OnRep_PlayerState` — L192

## HodgePlayerState.cpp

[Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp](../../../Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp)

玩家 ASC、HealthSet、PawnData、阵营/标签栈等持有者；SetPawnData 在权威端授予 AbilitySets（未记录句柄）。

- `AHodgePlayerState::NAME_HodgeAbilityReady` — L43
- `AHodgePlayerState::AHodgePlayerState` — L46
- `AHodgePlayerState::GetHodgePlayerController` — L80
- `AHodgePlayerState::GetAbilitySystemComponent` — L87
- `AHodgePlayerState::SetPawnData` — L94
- `AHodgePlayerState::PreInitializeComponents` — L139
- `AHodgePlayerState::PostInitializeComponents` — L146
- `AHodgePlayerState::Reset` — L182
- `AHodgePlayerState::ClientInitialize` — L189
- `AHodgePlayerState::CopyProperties` — L202
- `AHodgePlayerState::GetLifetimeReplicatedProps` — L212
- `AHodgePlayerState::OnDeactivated` — L249
- `AHodgePlayerState::OnReactivated` — L285
- `AHodgePlayerState::SetPlayerConnectionType` — L299
- `AHodgePlayerState::SetSquadID` — L309
- `AHodgePlayerState::AddStatTagStack` — L323
- `AHodgePlayerState::RemoveStatTagStack` — L330
- `AHodgePlayerState::GetStatTagStackCount` — L337
- `AHodgePlayerState::HasStatTag` — L344
- `AHodgePlayerState::GetReplicatedViewRotation` — L351
- `AHodgePlayerState::SetReplicatedViewRotation` — L358
- `AHodgePlayerState::OnExperienceLoaded` — L372
- `AHodgePlayerState::OnRep_PawnData` — L395
- `AHodgePlayerState::OnRep_MyTeamID` — L401
- `AHodgePlayerState::OnRep_MySquadID` — L408
- `AHodgePlayerState::SetCharacterLevel` — L415
- `AHodgePlayerState::RestoreCharacterHealth` — L421
- `AHodgePlayerState::InitializeCharacterProgression` — L427
- `AHodgePlayerState::AreAttributesReadyFor` — L440
- `AHodgePlayerState::NotifyAttributeReadiness` — L447
- `AHodgePlayerState::OnRep_AttributeReadyState` — L458

## HodgePlayerStateBase.cpp

[Source/Hodgepodge/Private/Core/PlayState/HodgePlayerStateBase.cpp](../../../Source/Hodgepodge/Private/Core/PlayState/HodgePlayerStateBase.cpp)

PlayerState ModularGameplay Receiver 注册、注销及组件 Reset/CopyProperties。

- `AHodgePlayerStateBase::PreInitializeComponents` — L13
- `AHodgePlayerStateBase::BeginPlay` — L22
- `AHodgePlayerStateBase::EndPlay` — L32
- `AHodgePlayerStateBase::Reset` — L41
- `AHodgePlayerStateBase::CopyProperties` — L58

## HodgeAbilityDefinition.cpp

[Source/Hodgepodge/Private/Data/HodgeAbilityDefinition.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAbilityDefinition.cpp)

Montage/Blend/Timeline、HitWindows/HitPoints、Volume、独立/连段路由及输入；校验几何、权威消息与手持覆盖。

- `UHodgeAbilityDefinition::UHodgeAbilityDefinition` — L23
- `FHodgeAbilityBlendSettings::MakeBlend` — L30
- `FHodgeAbilityBlendSettings::IsValid` — L40
- `UHodgeAbilityDefinition::GetDuration` — L48
- `UHodgeAbilityDefinition::GetFacingWarpWindow` — L54
- `UHodgeAbilityDefinition::ValidateDefinition` — L65
- `UHodgeAbilityDefinition::IsDataValid` — L167

## HodgeAbilitySet.cpp

[Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp)

权威端批量授予属性集、技能和 GE，并用句柄集合撤销。

- `FHodgeAbilitySet_GrantedHandles::AddAbilitySpecHandle` — L18
- `FHodgeAbilitySet_GrantedHandles::AddGameplayEffectHandle` — L28
- `FHodgeAbilitySet_GrantedHandles::AddAttributeSet` — L38
- `FHodgeAbilitySet_GrantedHandles::TakeFromAbilitySystem` — L44
- `UHodgeAbilitySet::UHodgeAbilitySet` — L86
- `UHodgeAbilitySet::GrantInternal` — L92
- `FHodgeAbilitySet_GrantedHandles::Append` — L243
- `FHodgeAbilitySet_GrantedHandles::AreValidFor` — L252
- `UHodgeAbilitySet::GiveToAbilitySystem` — L263
- `UHodgeAbilitySet::TryGiveToAbilitySystem` — L271
- `UHodgeAbilitySet::ValidateForGrant` — L287
- `UHodgeAbilitySet::ValidateBatchForGrant` — L335

## HodgeAssetManager.cpp

[Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp)

资产入口、GameData 缓存、启动任务、同步加载、加载进度。Cue 初始化钩子仍需接通。（PreloadPrimaryAssetBundles 已随未提交改动回退，当前不存在。）

- `FHodgeBundles::Equipped` — L13
- `UHodgeAssetManager::UHodgeAssetManager` — L48
- `UHodgeAssetManager::Get` — L55
- `UHodgeAssetManager::DumpLoadedAssets` — L75
- `UHodgeAssetManager::GetGameData` — L91
- `UHodgeAssetManager::GetDefaultPawnData` — L98
- `UHodgeAssetManager::SynchronousLoadAsset` — L105
- `UHodgeAssetManager::ShouldLogAssetLoads` — L141
- `UHodgeAssetManager::AddLoadedAsset` — L153
- `UHodgeAssetManager::StartInitialLoading` — L167
- `UHodgeAssetManager::LoadGameDataOfClass` — L189
- `UHodgeAssetManager::DoAllStartupJobs` — L292
- `UHodgeAssetManager::InitializeGameplayCueManager` — L382
- `UHodgeAssetManager::UpdateInitialGameContentLoadPercent` — L394
- `UHodgeAssetManager::PreBeginPIE` — L403

## HodgeAssetManagerStartupJob.cpp

[Source/Hodgepodge/Private/Data/HodgeAssetManagerStartupJob.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAssetManagerStartupJob.cpp)

封装启动任务与进度权重，供 AssetManager 执行启动工作。

- `FHodgeAssetManagerStartupJob::DoJob` — L16

## HodgeCharacterStatProfile.cpp

[Source/Hodgepodge/Private/Data/HodgeCharacterStatProfile.cpp](../../../Source/Hodgepodge/Private/Data/HodgeCharacterStatProfile.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeCharacterStatProfile::UHodgeCharacterStatProfile` — L15
- `UHodgeCharacterStatProfile::Evaluate` — L21
- `UHodgeCharacterStatProfile::Validate` — L31
- `UHodgeCharacterStatProfile::IsDataValid` — L60

## HodgeCinematicCameraProfile.cpp

[Source/Hodgepodge/Private/Data/HodgeCinematicCameraProfile.cpp](../../../Source/Hodgepodge/Private/Data/HodgeCinematicCameraProfile.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeCinematicCameraProfile::Validate` — L27
- `UHodgeCinematicCameraProfile::ValidateMesh` — L67
- `UHodgeCinematicCameraProfile::Sample` — L82
- `UHodgeCinematicCameraProfile::EvaluateWeight` — L97
- `UHodgeCinematicCameraProfile::IsDataValid` — L106

## HodgeComboDefinition.cpp

[Source/Hodgepodge/Private/Data/HodgeComboDefinition.cpp](../../../Source/Hodgepodge/Private/Data/HodgeComboDefinition.cpp)

跳转 DataTable、输入缓存、结束后连段记忆与 bAllowAfterExecutionEnded 续段许可。

- `UHodgeComboDefinition::FindNode` — L11
- `UHodgeComboDefinition::ValidateDefinition` — L19
- `UHodgeComboDefinition::IsDataValid` — L100

## HodgeEquipmentStatProfile.cpp

[Source/Hodgepodge/Private/Data/HodgeEquipmentStatProfile.cpp](../../../Source/Hodgepodge/Private/Data/HodgeEquipmentStatProfile.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeEquipmentStatProfile::UHodgeEquipmentStatProfile` — L15
- `UHodgeEquipmentStatProfile::Evaluate` — L21
- `UHodgeEquipmentStatProfile::Validate` — L31
- `UHodgeEquipmentStatProfile::IsDataValid` — L60

## HodgeExperienceActionSet.cpp

[Source/Hodgepodge/Private/Data/HodgeExperienceActionSet.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceActionSet.cpp)

复用 GameFeature 插件和动作配置的数据资产。

- `UHodgeExperienceActionSet::UHodgeExperienceActionSet` — L19
- `UHodgeExperienceActionSet::IsDataValid` — L26
- `UHodgeExperienceActionSet::UpdateAssetBundleData` — L65

## HodgeExperienceDefinition.cpp

[Source/Hodgepodge/Private/Data/HodgeExperienceDefinition.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceDefinition.cpp)

声明玩法所需插件、默认 PawnData、直接 Actions 和组合 ActionSets。

- `UHodgeExperienceDefinition::UHodgeExperienceDefinition` — L19
- `UHodgeExperienceDefinition::IsDataValid` — L26
- `UHodgeExperienceDefinition::UpdateAssetBundleData` — L100

## HodgeExperienceManager.cpp

[Source/Hodgepodge/Private/Data/HodgeExperienceManager.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceManager.cpp)

管理编辑器等场景的 GameFeature 使用/停用协调，不是挂载在 GameState 的组件。

- `UHodgeExperienceManager::OnPlayInEditorBegun` — L15
- `UHodgeExperienceManager::NotifyOfPluginActivation` — L25
- `UHodgeExperienceManager::RequestToDeactivatePlugin` — L45

## HodgeGameData.cpp

[Source/Hodgepodge/Private/Data/HodgeGameData.cpp](../../../Source/Hodgepodge/Private/Data/HodgeGameData.cpp)

全局伤害、治疗、动态 Tag GE 的软类引用配置；需编辑器核对实际赋值。

- `UHodgeGameData::UHodgeGameData` — L12
- `UHodgeGameData::Get` — L17

## HodgeHitReactionProfile.cpp

[Source/Hodgepodge/Private/Data/HodgeHitReactionProfile.cpp](../../../Source/Hodgepodge/Private/Data/HodgeHitReactionProfile.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeHitReactionProfile::FindAnimation` — L12
- `UHodgeHitReactionProfile::Validate` — L18
- `UHodgeHitReactionProfile::IsDataValid` — L54

## HodgeLockOnProfile.cpp

[Source/Hodgepodge/Private/Data/HodgeLockOnProfile.cpp](../../../Source/Hodgepodge/Private/Data/HodgeLockOnProfile.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeLockOnProfile::UHodgeLockOnProfile` — L7
- `UHodgeLockOnProfile::Validate` — L9
- `UHodgeLockOnProfile::IsDataValid` — L26

## HodgePawnData.cpp

[Source/Hodgepodge/Private/Data/HodgePawnData.cpp](../../../Source/Hodgepodge/Private/Data/HodgePawnData.cpp)

PawnClass、AbilitySets、ComboDefinition、DefaultWeaponDefinition、输入/相机/关系映射配置，编辑器校验统一在主 cpp。

- `UHodgePawnData::UHodgePawnData` — L23
- `UHodgePawnData::IsDataValid` — L34

## HodgeSprintAbilityProfile.cpp

[Source/Hodgepodge/Private/Data/HodgeSprintAbilityProfile.cpp](../../../Source/Hodgepodge/Private/Data/HodgeSprintAbilityProfile.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeSprintAbilityProfile::Validate` — L11
- `UHodgeSprintAbilityProfile::IsHandoffWindow` — L57
- `UHodgeSprintAbilityProfile::IsAuthorityHandoffWindow` — L60
- `UHodgeSprintAbilityProfile::CanMoveCancel` — L63
- `UHodgeSprintAbilityProfile::IsDataValid` — L67

## HodgeEquipmentDefinition.cpp

[Source/Hodgepodge/Private/Equipment/HodgeEquipmentDefinition.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentDefinition.cpp)

装备实例类、Actor 挂接与 AbilitySets 配置。

- `UHodgeEquipmentDefinition::UHodgeEquipmentDefinition` — L14

## HodgeEquipmentInstance.cpp

[Source/Hodgepodge/Private/Equipment/HodgeEquipmentInstance.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentInstance.cpp)

Pawn 所属复制 UObject、SpawnedActors RepNotify 和装备/卸装生命周期。

- `UHodgeEquipmentInstance::UHodgeEquipmentInstance` — L35
- `UHodgeEquipmentInstance::GetWorld` — L41
- `UHodgeEquipmentInstance::GetLifetimeReplicatedProps` — L57
- `UHodgeEquipmentInstance::RegisterReplicationFragments` — L75
- `UHodgeEquipmentInstance::GetPawn` — L89
- `UHodgeEquipmentInstance::GetTypedPawn` — L95
- `UHodgeEquipmentInstance::SpawnEquipmentActors` — L116
- `UHodgeEquipmentInstance::TrySpawnEquipmentActors` — L122
- `UHodgeEquipmentInstance::DestroyEquipmentActors` — L168
- `UHodgeEquipmentInstance::OnRep_SpawnedActors` — L184
- `UHodgeEquipmentInstance::OnEquipped` — L190
- `UHodgeEquipmentInstance::OnUnequipped` — L197
- `UHodgeEquipmentInstance::OnRep_Instigator` — L204
- `UHodgeEquipmentInstance::SetStatIdentity` — L209

## HodgeEquipmentManagerComponent.cpp

[Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp)

Experience 注入 Pawn，ASC 就绪后装备默认剑，来源授予句柄精确撤销与客户端迟到绑定。

- `FHodgeAppliedEquipmentEntry::GetDebugString` — L63
- `FHodgeEquipmentList::PreReplicatedRemove` — L72
- `FHodgeEquipmentList::PostReplicatedAdd` — L89
- `FHodgeEquipmentList::PostReplicatedChange` — L106
- `FHodgeEquipmentList::GetAbilitySystemComponent` — L121
- `FHodgeEquipmentList::AddEntry` — L134
- `FHodgeEquipmentList::RemoveEntry` — L195
- `UHodgeEquipmentManagerComponent::UHodgeEquipmentManagerComponent` — L231
- `UHodgeEquipmentManagerComponent::GetLifetimeReplicatedProps` — L243
- `UHodgeEquipmentManagerComponent::EquipItem` — L253
- `UHodgeEquipmentManagerComponent::EquipItemWithState` — L262
- `UHodgeEquipmentManagerComponent::UnequipItem` — L310
- `UHodgeEquipmentManagerComponent::ReplicateSubobjects` — L344
- `UHodgeEquipmentManagerComponent::InitializeComponent` — L369
- `UHodgeEquipmentManagerComponent::UninitializeComponent` — L386
- `UHodgeEquipmentManagerComponent::ReadyForReplication` — L411
- `UHodgeEquipmentManagerComponent::GetFirstInstanceOfType` — L438
- `UHodgeEquipmentManagerComponent::GetEquipmentInstancesOfType` — L461
- `UHodgeEquipmentManagerComponent::FindInstanceOfDefinition` — L487
- `UHodgeEquipmentManagerComponent::SetEquipmentLevel` — L494
- `UHodgeEquipmentManagerComponent::ValidateEquipmentForGrant` — L549
- `UHodgeEquipmentManagerComponent::IsEquipmentReady` — L577

## HodgeWeaponInstance.cpp

[Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp)

手持请求、显现阶段、计时器、复制/拥有者预测及拒绝恢复，不新增角色常驻武器表现组件。

- `UHodgeWeaponInstance::UHodgeWeaponInstance` — L46
- `UHodgeWeaponInstance::OnEquipped` — L71
- `UHodgeWeaponInstance::OnUnequipped` — L93
- `UHodgeWeaponInstance::UpdateFiringTime` — L104
- `UHodgeWeaponInstance::GetTimeSinceLastInteractedWith` — L117
- `UHodgeWeaponInstance::PickBestAnimLayer` — L146
- `UHodgeWeaponInstance::GetOwningUserId` — L157
- `UHodgeWeaponInstance::ApplyDeviceProperties` — L171
- `UHodgeWeaponInstance::RemoveDeviceProperties` — L209
- `UHodgeWeaponInstance::OnDeathStarted` — L232
- `UHodgeWeaponInstance::GetLifetimeReplicatedProps` — L248
- `UHodgeWeaponInstance::GetPresentationTime` — L255
- `UHodgeWeaponInstance::CanDrivePresentation` — L264
- `UHodgeWeaponInstance::ResolvePresentationWeapon` — L274
- `UHodgeWeaponInstance::InitializePresentation` — L302
- `UHodgeWeaponInstance::ClearPresentationTimer` — L325
- `UHodgeWeaponInstance::ShutdownPresentation` — L331
- `UHodgeWeaponInstance::AcquireHandUse` — L348
- `UHodgeWeaponInstance::ReleaseHandUse` — L381
- `UHodgeWeaponInstance::ReleaseHandUsesForExecution` — L390
- `UHodgeWeaponInstance::BeginIdlePresentation` — L399
- `UHodgeWeaponInstance::SchedulePresentationPhase` — L410
- `UHodgeWeaponInstance::SetPresentationPhase` — L429
- `UHodgeWeaponInstance::GetPresentationState` — L485
- `UHodgeWeaponInstance::OnRep_PresentationState` — L494
- `UHodgeWeaponInstance::RejectPredictedHandUse` — L516
- `UHodgeWeaponInstance::RefreshPresentationActors` — L536
- `UHodgeWeaponInstance::OnSpawnedActorsChanged` — L546
- `UHodgeWeaponInstance::BeginDestroy` — L553
- `UHodgeWeaponInstance::UpdateOwnerPosePolicy` — L561

## HodgeWeaponPresentationActor.cpp

[Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationActor.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationActor.cpp)

检测 Mesh 留手，可见 Mesh 回背/悬浮/消隐挂 BackSocket，手持时挂回检测 Mesh。

- `AHodgeWeaponPresentationActor::AHodgeWeaponPresentationActor` — L18
- `AHodgeWeaponPresentationActor::BeginPlay` — L41
- `AHodgeWeaponPresentationActor::TryBindWeapon` — L51
- `AHodgeWeaponPresentationActor::ConfigureProfile` — L65
- `AHodgeWeaponPresentationActor::BindWeapon` — L83
- `AHodgeWeaponPresentationActor::RefreshPresentation` — L108
- `AHodgeWeaponPresentationActor::GetVisualTransform` — L118
- `AHodgeWeaponPresentationActor::EvaluatePresentation` — L124
- `AHodgeWeaponPresentationActor::Tick` — L200
- `AHodgeWeaponPresentationActor::EndPlay` — L207

## HodgeWeaponPresentationProfile.cpp

[Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationProfile.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationProfile.cpp)

模型/材质、手背插槽/偏移、曲线/时间；BackSocket 默认 WeaponOnBack，BackTransform 为插槽内偏移。

- `UHodgeWeaponPresentationProfile::Validate` — L12
- `UHodgeWeaponPresentationProfile::IsDataValid` — L30

## GameFeatureAction_AddAbilities.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddAbilities.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddAbilities.cpp)

面向配置 Actor 授予能力、属性与 AbilitySet，维护撤销句柄。

- `UGameFeatureAction_AddAbilities::OnGameFeatureActivating` — L42
- `UGameFeatureAction_AddAbilities::OnGameFeatureDeactivating` — L60
- `UGameFeatureAction_AddAbilities::IsDataValid` — L79
- `UGameFeatureAction_AddAbilities::AddToWorld` — L198
- `UGameFeatureAction_AddAbilities::Reset` — L247
- `UGameFeatureAction_AddAbilities::HandleActorExtension` — L264
- `UGameFeatureAction_AddAbilities::AddActorAbilities` — L295
- `UGameFeatureAction_AddAbilities::RemoveActorAbilities` — L413
- `UGameFeatureAction_AddAbilities::FindOrAddComponentForActor` — L452

## GameFeatureAction_AddGameplayCuePath.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddGameplayCuePath.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddGameplayCuePath.cpp)

声明和校验 Cue 路径；Policy 添加路径主体已有，但观察者注册和注销清理仍缺。

- `UGameFeatureAction_AddGameplayCuePath::UGameFeatureAction_AddGameplayCuePath` — L21
- `UGameFeatureAction_AddGameplayCuePath::IsDataValid` — L30

## GameFeatureAction_AddInputBinding.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputBinding.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputBinding.cpp)

有效扩展事件添加/移除额外 InputConfig；Hero 侧移除已实现并与 EndPlay 清理配套。

- `UGameFeatureAction_AddInputBinding::OnGameFeatureActivating` — L55
- `UGameFeatureAction_AddInputBinding::OnGameFeatureDeactivating` — L73
- `UGameFeatureAction_AddInputBinding::IsDataValid` — L91
- `UGameFeatureAction_AddInputBinding::AddToWorld` — L123
- `UGameFeatureAction_AddInputBinding::Reset` — L158
- `UGameFeatureAction_AddInputBinding::HandlePawnExtension` — L184
- `UGameFeatureAction_AddInputBinding::AddInputMappingForPlayer` — L210
- `UGameFeatureAction_AddInputBinding::RemoveInputMapping` — L255

## GameFeatureAction_AddInputContextMapping.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputContextMapping.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputContextMapping.cpp)

有效 Controller 扩展添加 IMC，包含设置注册与诊断日志；记录/撤销需验收。

- `UGameFeatureAction_AddInputContextMapping::OnGameFeatureRegistering` — L44
- `UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating` — L54
- `UGameFeatureAction_AddInputContextMapping::OnGameFeatureDeactivating` — L76
- `UGameFeatureAction_AddInputContextMapping::OnGameFeatureUnregistering` — L93
- `UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContexts` — L103
- `UGameFeatureAction_AddInputContextMapping::RegisterInputContextMappingsForGameInstance` — L122
- `UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer` — L146
- `UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContexts` — L184
- `UGameFeatureAction_AddInputContextMapping::UnregisterInputContextMappingsForGameInstance` — L205
- `UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer` — L228
- `UGameFeatureAction_AddInputContextMapping::IsDataValid` — L266
- `UGameFeatureAction_AddInputContextMapping::AddToWorld` — L298
- `UGameFeatureAction_AddInputContextMapping::Reset` — L333
- `UGameFeatureAction_AddInputContextMapping::HandleControllerExtension` — L358
- `UGameFeatureAction_AddInputContextMapping::AddInputMappingForPlayer` — L390
- `UGameFeatureAction_AddInputContextMapping::RemoveInputMapping` — L438

## GameFeatureAction_AddWidget.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddWidget.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddWidget.cpp)

Widget 注入迁移草稿，当前实现停用。

- `UGameFeatureAction_AddWidgets::OnGameFeatureDeactivating` — L54
- `UGameFeatureAction_AddWidgets::AddAdditionalAssetBundleData` — L73
- `UGameFeatureAction_AddWidgets::IsDataValid` — L88
- `UGameFeatureAction_AddWidgets::AddToWorld` — L163
- `UGameFeatureAction_AddWidgets::ClearActorContents` — L197
- `UGameFeatureAction_AddWidgets::Reset` — L210
- `UGameFeatureAction_AddWidgets::HandleActorExtension` — L223
- `UGameFeatureAction_AddWidgets::AddWidgets` — L242
- `UGameFeatureAction_AddWidgets::RemoveWidgets` — L293

## GameFeatureAction_SplitscreenConfig.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_SplitscreenConfig.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_SplitscreenConfig.cpp)

GameFeature 激活期间的分屏策略调整。

- `UGameFeatureAction_SplitscreenConfig::OnGameFeatureDeactivating` — L24
- `UGameFeatureAction_SplitscreenConfig::AddToWorld` — L81

## GameFeatureAction_WorldActionBase.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_WorldActionBase.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_WorldActionBase.cpp)

按游戏世界和激活上下文组织 Action 生命周期。

- `UGameFeatureAction_WorldActionBase::OnGameFeatureActivating` — L14
- `UGameFeatureAction_WorldActionBase::OnGameFeatureDeactivating` — L36
- `UGameFeatureAction_WorldActionBase::HandleGameInstanceStart` — L50

## HodgeGameFeaturePolicy.cpp

[Source/Hodgepodge/Private/GameFeatures/HodgeGameFeaturePolicy.cpp](../../../Source/Hodgepodge/Private/GameFeatures/HodgeGameFeaturePolicy.cpp)

已配置的 GameFeature 策略；Hotfix 观察者注册，Cue 路径观察者创建仍注释。

- `UHodgeGameFeaturePolicy::UHodgeGameFeaturePolicy` — L18
- `UHodgeGameFeaturePolicy::Get` — L27
- `UHodgeGameFeaturePolicy::InitGameFeatureManager` — L37
- `UHodgeGameFeaturePolicy::ShutdownGameFeatureManager` — L62
- `UHodgeGameFeaturePolicy::GetPreloadAssetListForGameFeature` — L82
- `UHodgeGameFeaturePolicy::GetPreloadBundleStateForGameFeature` — L95
- `UHodgeGameFeaturePolicy::GetGameFeatureLoadingMode` — L105
- `UHodgeGameFeaturePolicy::IsPluginAllowed` — L119
- `UHodgeGameFeature_HotfixManager::OnGameFeatureLoading` — L131
- `UHodgeGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering` — L159
- `UHodgeGameFeature_AddGameplayCuePaths::OnGameFeatureUnregistering` — L243

## HodgeAimSensitivityData.cpp

[Source/Hodgepodge/Private/Input/HodgeAimSensitivityData.cpp](../../../Source/Hodgepodge/Private/Input/HodgeAimSensitivityData.cpp)

瞄准灵敏度数据映射。

- `UHodgeAimSensitivityData::UHodgeAimSensitivityData` — L15

## HodgeInputComponent.cpp

[Source/Hodgepodge/Private/Input/HodgeInputComponent.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputComponent.cpp)

基于 Tag 的 Native/Ability Action 绑定和句柄移除；映射辅助函数仍占位。

- `UHodgeInputComponent::UHodgeInputComponent` — L26
- `UHodgeInputComponent::AddInputMappings` — L42
- `UHodgeInputComponent::RemoveInputMappings` — L62
- `UHodgeInputComponent::RemoveBinds` — L84

## HodgeInputConfig.cpp

[Source/Hodgepodge/Private/Input/HodgeInputConfig.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputConfig.cpp)

NativeInputActions / AbilityInputActions 的 IA 与 Tag 数据配置及查询。

- `UHodgeInputConfig::UHodgeInputConfig` — L25
- `UHodgeInputConfig::FindNativeInputActionForTag` — L40
- `UHodgeInputConfig::FindAbilityInputActionForTag` — L71

## HodgeInputModifiers.cpp

[Source/Hodgepodge/Private/Input/HodgeInputModifiers.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputModifiers.cpp)

输入数值处理扩展；具体启用情况由 IA/IMC 资产决定。

- `UHodgeSettingBasedScalar::ModifyRaw_Implementation` — L58
- `UHodgeInputModifierDeadZone::ModifyRaw_Implementation` — L149
- `UHodgeInputModifierDeadZone::GetVisualizationColor_Implementation` — L234
- `UHodgeInputModifierGamepadSensitivity::ModifyRaw_Implementation` — L256
- `UHodgeInputModifierAimInversion::ModifyRaw_Implementation` — L295

## HodgeInputUserSettings.cpp

[Source/Hodgepodge/Private/Input/HodgeInputUserSettings.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputUserSettings.cpp)

Enhanced Input 用户设置派生入口；须核对实际设置类配置。

- `UHodgeInputUserSettings::ApplySettings` — L12

## HodgePlayerMappableKeyProfile.cpp

[Source/Hodgepodge/Private/Input/HodgePlayerMappableKeyProfile.cpp](../../../Source/Hodgepodge/Private/Input/HodgePlayerMappableKeyProfile.cpp)

玩家键位 Profile 扩展。

- `UHodgePlayerMappableKeyProfile::EquipProfile` — L12
- `UHodgePlayerMappableKeyProfile::UnEquipProfile` — L22

## HodgeAbilitySourceInterface.cpp

[Source/Hodgepodge/Private/Interface/HodgeAbilitySourceInterface.cpp](../../../Source/Hodgepodge/Private/Interface/HodgeAbilitySourceInterface.cpp)

技能来源与相关计算契约。

- `UHodgeAbilitySourceInterface::UHodgeAbilitySourceInterface` — L11

## LoadingProcessInterface.cpp

[Source/Hodgepodge/Private/Interface/LoadingProcessInterface.cpp](../../../Source/Hodgepodge/Private/Interface/LoadingProcessInterface.cpp)

加载状态/原因查询契约；不是独立加载界面。

## HodgeLockOnTypes.cpp

[Source/Hodgepodge/Private/Targeting/HodgeLockOnTypes.cpp](../../../Source/Hodgepodge/Private/Targeting/HodgeLockOnTypes.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeLockOnTargetData::NetSerialize` — L13
- `FHodgeLockOnGesture::Reset` — L26
- `FHodgeLockOnGesture::Sample` — L27
- `FHodgeSoftLockCamera::Seed` — L46
- `FHodgeSoftLockCamera::AddLook` — L55
- `FHodgeSoftLockCamera::BeginAcquire` — L61
- `FHodgeSoftLockCamera::Update` — L67
- `FHodgeSoftLockCamera::Reset` — L140

## HodgeAbilityDefinitionTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeAbilityDefinitionTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAbilityDefinitionTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeDefinitionGrantTest::RunTest` — L23
- `FHodgeComboValidationTest::RunTest` — L62
- `FHodgeComboRetentionTest::RunTest` — L176
- `FHodgeComboSessionTest::RunTest` — L298

## HodgeAbilityIconTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeAbilityIconTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAbilityIconTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeAbilityIconCooldownTest::RunTest` — L9
- `FHodgeAbilityIconEnergyTest::RunTest` — L31

## HodgeAbilityRelationshipTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeAbilityRelationshipTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAbilityRelationshipTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeRelationshipRuleTest::RunTest` — L47
- `FHodgeRelationshipRuntimeTest::RunTest` — L72

## HodgeAnimNotifyTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeAnimNotifyTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAnimNotifyTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeNotifyConfigurationTest::RunTest` — L14
- `FHodgeNotifyContextTest::RunTest` — L35

## HodgeAttackWarpTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeAttackWarpTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAttackWarpTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeAttackWarpConfigTest::RunTest` — L70
- `FHodgeAttackWarpYawTest::RunTest` — L80
- `FHodgeAttackWarpWindowTest::RunTest` — L88
- `FHodgeAttackWarpLeaseTest::RunTest` — L97
- `FHodgeAttackWarpRootTest::RunTest` — L109
- `FHodgeAttackWarpProtocolTest::RunTest` — L122
- `FHodgeAttackWarpReplayTest::RunTest` — L133
- `FHodgeAttackWarpValidationTest::RunTest` — L146
- `FHodgeAttackWarpRetiredStateTest::RunTest` — L157

## HodgeAttackWarpTestTypes.cpp

[Source/Hodgepodge/Private/Tests/HodgeAttackWarpTestTypes.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAttackWarpTestTypes.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeAttackWarpTestAbility::CanActivateAbility` — L4
- `UHodgeAttackWarpTestAbility::ActivateAbility` — L7

## HodgeAttackWarpTestTypes.h

[Source/Hodgepodge/Private/Tests/HodgeAttackWarpTestTypes.h](../../../Source/Hodgepodge/Private/Tests/HodgeAttackWarpTestTypes.h)

模块定义或基础代码；请查看对应文件。

## HodgeAttributeGrowthTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeAttributeGrowthTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAttributeGrowthTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeStatProfileTest::RunTest` — L121
- `FHodgeStatGrowthTest::RunTest` — L143
- `FHodgeStatReentryTest::RunTest` — L181
- `FHodgeStatAvatarTest::RunTest` — L211

## HodgeCharacterFacingTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeCharacterFacingTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeCharacterFacingTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeFacingAuthorityActionTest::RunTest` — L92
- `FHodgeFacingInputTest::RunTest` — L117
- `FHodgeFacingModesTest::RunTest` — L137
- `FHodgeFacingActionTest::RunTest` — L164
- `FHodgeFacingLifecycleTest::RunTest` — L198
- `FHodgeFacingActivationPairTest::RunTest` — L231
- `FHodgeFacingReplayTest::RunTest` — L254
- `FHodgeFacingAnimationTest::RunTest` — L286
- `FHodgeDashInstantFacingTest::RunTest` — L304

## HodgeCharacterRotationTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeCharacterRotationTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeCharacterRotationTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeCharacterRotationConstraintsTest::RunTest` — L19

## HodgeCinematicCameraTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeCinematicCameraTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeCinematicCameraTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeCinematicSchemaTest::RunTest` — L32
- `FHodgeCinematicHeroLeaseTest::RunTest` — L61
- `FHodgeCinematicSamplingTest::RunTest` — L79
- `FHodgeCinematicHandleTest::RunTest` — L102

## HodgeCombatActionRequestTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeCombatActionRequestTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeCombatActionRequestTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeActionLifecycleTest::RunTest` — L173
- `FHodgeStandaloneMovePolicyTest::RunTest` — L195
- `FHodgeStandaloneMoveValidationTest::RunTest` — L224
- `FHodgeStandaloneMoveIdentityTest::RunTest` — L254
- `FHodgeStandaloneMoveConfigTest::RunTest` — L267
- `FHodgeActionIntentTest::RunTest` — L283
- `FHodgeActionAIRouteTest::RunTest` — L308
- `FHodgeActionSynchronousTest::RunTest` — L320
- `FHodgeActionFailuresTest::RunTest` — L344
- `FHodgeActionHistoryTest::RunTest` — L361
- `FHodgeActionMoveResolutionTest::RunTest` — L375
- `FHodgeVerifyHeldDeath::Update` — L402
- `FHodgeActionHeldDeathTest::RunTest` — L415

## HodgeDashAirTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeDashAirTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeDashAirTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeDashAirEligibilityTest::RunTest` — L67
- `FHodgeDashAirConstraintsTest::RunTest` — L90
- `FHodgeDashAirFinishPolicyTest::RunTest` — L110
- `FHodgeDashAirPhysicsTest::RunTest` — L136

## HodgeEnemyAITests.cpp

[Source/Hodgepodge/Private/Tests/HodgeEnemyAITests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeEnemyAITests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeEnemyIdempotencyTest::RunTest` — L155
- `FHodgeEnemyASCConflictTest::RunTest` — L173
- `FHodgeEnemyMissingStatsTest::RunTest` — L190
- `FHodgeEnemyInvalidLevelTest::RunTest` — L202
- `FHodgeEnemyLateDependenciesTest::RunTest` — L211
- `FHodgeEnemyBindingInvalidationTest::RunTest` — L228
- `FHodgeEnemyGrantPreflightTest::RunTest` — L242
- `FHodgeEnemyGrantRebindTest::RunTest` — L256
- `FHodgeEnemyEquipmentFailureTest::RunTest` — L268
- `FHodgeEnemyBrainLifecycleTest::RunTest` — L289
- `FHodgeEnemyControlTest::RunTest` — L303
- `FHodgeEnemyLateCallbackTest::RunTest` — L318
- `FHodgeEnemyComponentRemovalTest::RunTest` — L328
- `FHodgeEnemyInstantFailureTest::RunTest` — L342
- `FHodgeEnemyTargetKeyTest::RunTest` — L359
- `FHodgeEnemyClearedIdentityTest::RunTest` — L391

## HodgeEnemyAITestSupport.h

[Source/Hodgepodge/Private/Tests/HodgeEnemyAITestSupport.h](../../../Source/Hodgepodge/Private/Tests/HodgeEnemyAITestSupport.h)

模块定义或基础代码；请查看对应文件。

## HodgeEnemyAITestTypes.cpp

[Source/Hodgepodge/Private/Tests/HodgeEnemyAITestTypes.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeEnemyAITestTypes.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeEnemyTestAbilitySet::AddAbility` — L12
- `UHodgeEnemyTestAbilitySet::AddEffect` — L17
- `UHodgeEnemyTestEquipment::UHodgeEnemyTestEquipment` — L22
- `UHodgeEnemyRebindOnGrantAbility::OnGiveAbility` — L24
- `UHodgeEnemyOnSpawnTestAbility::UHodgeEnemyOnSpawnTestAbility` — L32
- `AHodgeEnemyEquipmentTestActor::AHodgeEnemyEquipmentTestActor` — L34
- `AHodgeEnemyEquipmentFailActor::OnConstruction` — L39
- `UHodgeEnemyInstantTestEffect::UHodgeEnemyInstantTestEffect` — L45

## HodgeEnemyAITestTypes.h

[Source/Hodgepodge/Private/Tests/HodgeEnemyAITestTypes.h](../../../Source/Hodgepodge/Private/Tests/HodgeEnemyAITestTypes.h)

模块定义或基础代码；请查看对应文件。

## HodgeHavocTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeHavocTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeHavocTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeHavocResourceTest::RunTest` — L70
- `FHodgeHavocFuelTest::RunTest` — L87
- `FHodgeHavocProcTest::RunTest` — L103
- `FHodgeHavocEffectTest::RunTest` — L117
- `FHodgeHavocHoldTest::RunTest` — L136
- `FHodgeHavocVariantTest::RunTest` — L152
- `FHodgeHavocSessionTest::RunTest` — L185
- `FHodgeHavocAssetsTest::RunTest` — L257
- `FHodgeHavocDebugFillTest::RunTest` — L292

## HodgeHitDetectionTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeHitDetectionTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeHitDetectionTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeHitGeometryTest::RunTest` — L15
- `FHodgeHitProfileValidationTest::RunTest` — L42

## HodgeHitReactionTestAbility.h

[Source/Hodgepodge/Private/Tests/HodgeHitReactionTestAbility.h](../../../Source/Hodgepodge/Private/Tests/HodgeHitReactionTestAbility.h)

模块定义或基础代码；请查看对应文件。

## HodgeHitReactionTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeHitReactionTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeHitReactionTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeReactionRanksTest::RunTest` — L137
- `FHodgeReactionSettlementTest::RunTest` — L166
- `FHodgeReactionImmuneTest::RunTest` — L190
- `FHodgeReactionLifecycleTest::RunTest` — L206
- `FHodgeReactionFallbackTest::RunTest` — L245
- `FHodgeReactionMotionTest::RunTest` — L266
- `FHodgeReactionCancellationTest::RunTest` — L288
- `FHodgeReactionEnemyTest::RunTest` — L316

## HodgeLockOnIndicatorTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeLockOnIndicatorTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeLockOnIndicatorTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeLockOnIndicatorDesignerTest::RunTest` — L10

## HodgeLockOnTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeLockOnTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeLockOnTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeLockOnGestureTest::RunTest` — L93
- `FHodgeLockOnProfileTest::RunTest` — L109
- `FHodgeLockOnAuthorityTest::RunTest` — L128
- `FHodgeLockOnRecoveryTest::RunTest` — L155
- `FHodgeLockOnMotionTest::RunTest` — L175
- `FHodgeLockOnRangeTest::RunTest` — L194
- `FHodgeLockOnGASTest::RunTest` — L209
- `FHodgeLockOnDeathTest::RunTest` — L219
- `FHodgeLockOnCacheTest::RunTest` — L231
- `FHodgeSoftLockCameraTest::RunTest` — L252
- `FHodgeLockCameraAcquireTest::RunTest` — L282
- `FHodgeLockCameraTrackingTest::RunTest` — L305
- `FHodgeLockCameraRecoveryTest::RunTest` — L327
- `FHodgeLockOnCameraStackTest::RunTest` — L344

## HodgeLockOnTestTypes.cpp

[Source/Hodgepodge/Private/Tests/HodgeLockOnTestTypes.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeLockOnTestTypes.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeCameraMode_LockOnTestFree::UHodgeCameraMode_LockOnTestFree` — L4

## HodgeLockOnTestTypes.h

[Source/Hodgepodge/Private/Tests/HodgeLockOnTestTypes.h](../../../Source/Hodgepodge/Private/Tests/HodgeLockOnTestTypes.h)

模块定义或基础代码；请查看对应文件。

## HodgeMeleeRoutingTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeMeleeRoutingTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeMeleeRoutingTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeDetectionRoutingTest::RunTest` — L109
- `FHodgeMeleeHitHistoryTest::RunTest` — L140
- `FHodgeMeleeSpecContextTest::RunTest` — L165

## HodgeMovementActionTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeMovementActionTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeMovementActionTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeDashDirectionsTest::RunTest` — L41
- `FHodgeSprintSessionTest::RunTest` — L66
- `FHodgeMovementTargetDataTest::RunTest` — L84
- `FHodgeMovementPolicyTest::RunTest` — L99
- `FHodgeMovementTimingTest::RunTest` — L178
- `FHodgeDashFootIKTest::RunTest` — L210
- `FHodgeSprintPivotTest::RunTest` — L225
- `FHodgeMovementDefenseTest::RunTest` — L254

## HodgeRelationshipTestAbility.cpp

[Source/Hodgepodge/Private/Tests/HodgeRelationshipTestAbility.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeRelationshipTestAbility.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeRelationshipTestAbility::UHodgeRelationshipTestAbility` — L8
- `UHodgeRelationshipTestAbility::ActivateAbility` — L11
- `UHodgeFacingPredictionTestAbility::UHodgeFacingPredictionTestAbility` — L16

## HodgeRelationshipTestAbility.h

[Source/Hodgepodge/Private/Tests/HodgeRelationshipTestAbility.h](../../../Source/Hodgepodge/Private/Tests/HodgeRelationshipTestAbility.h)

模块定义或基础代码；请查看对应文件。

## HodgeResonanceEnergyTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeResonanceEnergyTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeResonanceEnergyTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeResonanceCostTest::RunTest` — L50
- `FHodgeResonancePhaseTest::RunTest` — L77
- `FHodgeResonancePlayerStateBootstrapTest::RunTest` — L94
- `FHodgeResonanceDebugFillTest::RunTest` — L121

## HodgeUIDataSourceTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeUIDataSourceTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeUIDataSourceTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeUIInactiveDataTest::RunTest` — L12

## HodgeUIFoundationTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeUIFoundationTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeUIFoundationTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeUIClosedRootTest::RunTest` — L15
- `FHodgeUINonHodgeInstanceTest::RunTest` — L39

## HodgeWeaponPresentationTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeWeaponPresentationTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeWeaponPresentationTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeWeaponRequestsTest::RunTest` — L42
- `FHodgeWeaponGeometryTest::RunTest` — L114
- `FHodgeWeaponEndBehaviorTest::RunTest` — L171

## HodgeWingAttackPresentationTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeWingAttackPresentationTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeWingAttackPresentationTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeWingFadeStateTest::RunTest` — L55
- `FHodgeWingAnimationMappingTest::RunTest` — L85
- `FHodgeWingPresentationSessionTest::RunTest` — L127
- `FHodgeWingAttackIntegrationTest::RunTest` — L150

## HodgeWingBoxHitTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeWingBoxHitTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeWingBoxHitTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeWingBoxHitTest::RunTest` — L18

## HodgeWingTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeWingTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeWingTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeWingEnvelopeTest::RunTest` — L13
- `FHodgeWingCancellationTest::RunTest` — L42
- `FHodgeWingAssetContractTest::RunTest` — L68

## MaterialProgressBar.cpp

[Source/Hodgepodge/Private/UI/Basic/MaterialProgressBar.cpp](../../../Source/Hodgepodge/Private/UI/Basic/MaterialProgressBar.cpp)

模块定义或基础代码；请查看对应文件。

- `UMaterialProgressBar::SynchronizeProperties` — L15
- `UMaterialProgressBar::OnWidgetRebuilt` — L86
- `UMaterialProgressBar::OnAnimationFinished_Implementation` — L146
- `UMaterialProgressBar::SetProgress` — L158
- `UMaterialProgressBar::SetStartProgress` — L167
- `UMaterialProgressBar::SetColorA` — L176
- `UMaterialProgressBar::SetColorB` — L185
- `UMaterialProgressBar::SetColorBackground` — L194
- `UMaterialProgressBar::AnimateProgressFromStart` — L203
- `UMaterialProgressBar::AnimateProgressFromCurrent` — L211
- `UMaterialProgressBar::SetProgress_Internal` — L224
- `UMaterialProgressBar::SetStartProgress_Internal` — L234
- `UMaterialProgressBar::SetColorA_Internal` — L244
- `UMaterialProgressBar::SetColorB_Internal` — L254
- `UMaterialProgressBar::SetColorBackground_Internal` — L264
- `UMaterialProgressBar::GetBarDynamicMaterial` — L274

## HodgeBoundActionButton.cpp

[Source/Hodgepodge/Private/UI/Common/HodgeBoundActionButton.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeBoundActionButton.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeBoundActionButton::NativeConstruct` — L16
- `UHodgeBoundActionButton::HandleInputMethodChanged` — L29

## HodgeListView.cpp

[Source/Hodgepodge/Private/UI/Common/HodgeListView.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeListView.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeListView::UHodgeListView` — L18
- `UHodgeListView::ValidateCompiledDefaults` — L26
- `UHodgeListView::OnGenerateEntryWidgetInternal` — L41

## HodgeTabButtonBase.cpp

[Source/Hodgepodge/Private/UI/Common/HodgeTabButtonBase.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeTabButtonBase.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeTabButtonBase::SetIconFromLazyObject` — L17
- `UHodgeTabButtonBase::SetIconBrush` — L26
- `UHodgeTabButtonBase::SetTabLabelInfo_Implementation` — L35

## HodgeTabListWidgetBase.cpp

[Source/Hodgepodge/Private/UI/Common/HodgeTabListWidgetBase.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeTabListWidgetBase.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeTabListWidgetBase::NativeOnInitialized` — L14
- `UHodgeTabListWidgetBase::NativeConstruct` — L20
- `UHodgeTabListWidgetBase::NativeDestruct` — L28
- `UHodgeTabListWidgetBase::GetPreregisteredTabInfo` — L43
- `UHodgeTabListWidgetBase::SetTabHiddenState` — L61
- `UHodgeTabListWidgetBase::RegisterDynamicTab` — L74
- `UHodgeTabListWidgetBase::HandlePreLinkedSwitcherChanged` — L88
- `UHodgeTabListWidgetBase::HandlePostLinkedSwitcherChanged` — L103
- `UHodgeTabListWidgetBase::HandleTabCreation_Implementation` — L116
- `UHodgeTabListWidgetBase::IsFirstTabActive` — L146
- `UHodgeTabListWidgetBase::IsLastTabActive` — L157
- `UHodgeTabListWidgetBase::IsTabVisible` — L168
- `UHodgeTabListWidgetBase::GetVisibleTabCount` — L182
- `UHodgeTabListWidgetBase::SetupTabs` — L198

## HodgeWidgetFactory.cpp

[Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeWidgetFactory::FindWidgetClassForData_Implementation` — L14

## HodgeWidgetFactory_Class.cpp

[Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory_Class.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory_Class.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeWidgetFactory_Class::FindWidgetClassForData_Implementation` — L12

## HodgeGameplayUIDataSource.cpp

[Source/Hodgepodge/Private/UI/Data/HodgeGameplayUIDataSource.cpp](../../../Source/Hodgepodge/Private/UI/Data/HodgeGameplayUIDataSource.cpp)

每个本地玩家共享的生命订阅、属性就绪与技能状态/输入接入；布局和按钮行为保存在 WBP。

- `UHodgeGameplayUIDataSource::Initialize` — L24
- `UHodgeGameplayUIDataSource::UnbindPawn` — L37
- `UHodgeGameplayUIDataSource::UnbindState` — L50
- `UHodgeGameplayUIDataSource::Shutdown` — L68
- `UHodgeGameplayUIDataSource::StateChanged` — L84
- `UHodgeGameplayUIDataSource::PawnChanged` — L106
- `UHodgeGameplayUIDataSource::Refresh` — L130
- `UHodgeGameplayUIDataSource::ResonanceAttributeChanged` — L165
- `UHodgeGameplayUIDataSource::HavocModeChanged` — L167
- `UHodgeGameplayUIDataSource::AttributeChanged` — L170
- `UHodgeGameplayUIDataSource::DeathChanged` — L172
- `UHodgeGameplayUIDataSource::CanUseGameplay` — L175
- `UHodgeGameplayUIDataSource::GetAbilityDisplayState` — L183
- `UHodgeGameplayUIDataSource::SubmitInput` — L222

## UIExtensionPointWidget.cpp

[Source/Hodgepodge/Private/UI/Extension/UIExtensionPointWidget.cpp](../../../Source/Hodgepodge/Private/UI/Extension/UIExtensionPointWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UUIExtensionPointWidget::UUIExtensionPointWidget` — L42
- `UUIExtensionPointWidget::SetExtensionPointTag` — L48
- `UUIExtensionPointWidget::ReleaseSlateResources` — L55
- `UUIExtensionPointWidget::RebuildWidget` — L66
- `UUIExtensionPointWidget::ResetExtensionPoint` — L137
- `UUIExtensionPointWidget::RegisterExtensionPoint` — L162
- `UUIExtensionPointWidget::RegisterExtensionPointForPlayerState` — L206
- `UUIExtensionPointWidget::OnAddOrRemoveExtension` — L236
- `UUIExtensionPointWidget::ValidateCompiledDefaults` — L319

## UIExtensionSystem.cpp

[Source/Hodgepodge/Private/UI/Extension/UIExtensionSystem.cpp](../../../Source/Hodgepodge/Private/UI/Extension/UIExtensionSystem.cpp)

模块定义或基础代码；请查看对应文件。

- `FUIExtensionPointHandle::Unregister` — L27
- `FUIExtensionHandle::Unregister` — L41
- `FUIExtensionPoint::DoesExtensionPassContract` — L55
- `UUIExtensionSubsystem::AddReferencedObjects` — L104
- `UUIExtensionSubsystem::Initialize` — L139
- `UUIExtensionSubsystem::Deinitialize` — L146
- `UUIExtensionSubsystem::RegisterExtensionPoint` — L153
- `UUIExtensionSubsystem::RegisterExtensionPointForContext` — L165
- `UUIExtensionSubsystem::RegisterExtensionAsWidget` — L228
- `UUIExtensionSubsystem::RegisterExtensionAsWidgetForContext` — L238
- `UUIExtensionSubsystem::RegisterExtensionAsData` — L247
- `UUIExtensionSubsystem::NotifyExtensionPointOfExtensions` — L310
- `UUIExtensionSubsystem::NotifyExtensionPointsOfExtension` — L360
- `UUIExtensionSubsystem::UnregisterExtension` — L415
- `UUIExtensionSubsystem::UnregisterExtensionPoint` — L469
- `UUIExtensionSubsystem::CreateExtensionRequest` — L505
- `UUIExtensionSubsystem::K2_RegisterExtensionPoint` — L529
- `UUIExtensionSubsystem::K2_RegisterExtensionAsWidget` — L547
- `UUIExtensionSubsystem::K2_RegisterExtensionAsWidgetForContext` — L556
- `UUIExtensionSubsystem::K2_RegisterExtensionAsData` — L575
- `UUIExtensionSubsystem::K2_RegisterExtensionAsDataForContext` — L583
- `UUIExtensionHandleFunctions::Unregister` — L603
- `UUIExtensionHandleFunctions::IsValid` — L609
- `UUIExtensionPointHandleFunctions::Unregister` — L617
- `UUIExtensionPointHandleFunctions::IsValid` — L623

## HodgeActionWidget.cpp

[Source/Hodgepodge/Private/UI/Foundation/HodgeActionWidget.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeActionWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeActionWidget::GetIcon` — L14
- `UHodgeActionWidget::GetEnhancedInputSubsystem` — L40

## HodgeButtonBase.cpp

[Source/Hodgepodge/Private/UI/Foundation/HodgeButtonBase.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeButtonBase.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeButtonBase::NativePreConstruct` — L12
- `UHodgeButtonBase::UpdateInputActionWidget` — L21
- `UHodgeButtonBase::SetButtonText` — L30
- `UHodgeButtonBase::RefreshButtonText` — L38
- `UHodgeButtonBase::OnInputMethodChanged` — L57

## HodgeConfirmationScreen.cpp

[Source/Hodgepodge/Private/UI/Foundation/HodgeConfirmationScreen.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeConfirmationScreen.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeControllerDisconnectedScreen.cpp

[Source/Hodgepodge/Private/UI/Foundation/HodgeControllerDisconnectedScreen.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeControllerDisconnectedScreen.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeControllerDisconnectedScreen::UHodgeControllerDisconnectedScreen` — L27
- `UHodgeControllerDisconnectedScreen::NativeOnActivated` — L35
- `UHodgeControllerDisconnectedScreen::ShouldDisplayChangeUserButton` — L71
- `UHodgeControllerDisconnectedScreen::HandleChangeUserClicked` — L87
- `UHodgeControllerDisconnectedScreen::HandleChangeUserCompleted` — L106

## HodgeLoadingScreenSubsystem.cpp

[Source/Hodgepodge/Private/UI/Foundation/HodgeLoadingScreenSubsystem.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeLoadingScreenSubsystem.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeLoadingScreenSubsystem::UHodgeLoadingScreenSubsystem` — L18
- `UHodgeLoadingScreenSubsystem::SetLoadingScreenContentWidget` — L23
- `UHodgeLoadingScreenSubsystem::GetLoadingScreenContentWidget` — L35

## HodgePrimaryGameLayout.cpp

[Source/Hodgepodge/Private/UI/Foundation/HodgePrimaryGameLayout.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgePrimaryGameLayout.cpp)

绑定 Designer 四个容器的本地玩家根布局，Push/Pop、异步加载取消与令牌恢复；不再生成运行时控件树。

- `UHodgePrimaryGameLayout::NativeOnInitialized` — L17
- `UHodgePrimaryGameLayout::RegisterLayer` — L28
- `UHodgePrimaryGameLayout::GetLayer` — L35
- `UHodgePrimaryGameLayout::Push` — L42
- `UHodgePrimaryGameLayout::Pop` — L52
- `UHodgePrimaryGameLayout::HasBlockingPage` — L62
- `UHodgePrimaryGameLayout::PushAsync` — L77
- `UHodgePrimaryGameLayout::FinishPush` — L98
- `UHodgePrimaryGameLayout::CancelPush` — L110
- `UHodgePrimaryGameLayout::ReleaseLayout` — L121
- `UHodgePrimaryGameLayout::NativeDestruct` — L136

## ApplyFrontendPerfSettingsAction.cpp

[Source/Hodgepodge/Private/UI/Frontend/ApplyFrontendPerfSettingsAction.cpp](../../../Source/Hodgepodge/Private/UI/Frontend/ApplyFrontendPerfSettingsAction.cpp)

模块定义或基础代码；请查看对应文件。

- `UApplyFrontendPerfSettingsAction::OnGameFeatureActivating` — L27
- `UApplyFrontendPerfSettingsAction::OnGameFeatureDeactivating` — L37

## HodgeFrontendStateComponent.cpp

[Source/Hodgepodge/Private/UI/Frontend/HodgeFrontendStateComponent.cpp](../../../Source/Hodgepodge/Private/UI/Frontend/HodgeFrontendStateComponent.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeLobbyBackground.cpp

[Source/Hodgepodge/Private/UI/Frontend/HodgeLobbyBackground.cpp](../../../Source/Hodgepodge/Private/UI/Frontend/HodgeLobbyBackground.cpp)

模块定义或基础代码；请查看对应文件。

## HodgeAbilityIconWidget.cpp

[Source/Hodgepodge/Private/UI/HodgeAbilityIconWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeAbilityIconWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeAbilityIconWidget::EvaluateState` — L12
- `UHodgeAbilityIconWidget::NativeConstruct` — L24
- `UHodgeAbilityIconWidget::SynchronizeProperties` — L31
- `UHodgeAbilityIconWidget::ApplyStyle` — L36
- `UHodgeAbilityIconWidget::NativeTick` — L54
- `UHodgeAbilityIconWidget::RefreshDisplay` — L59
- `UHodgeAbilityIconWidget::NativeDestruct` — L89
- `UHodgeAbilityIconWidget::RequestAbility` — L95

## HodgeActivatableWidget.cpp

[Source/Hodgepodge/Private/UI/HodgeActivatableWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeActivatableWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeActivatableWidget::UHodgeActivatableWidget` — L20
- `UHodgeActivatableWidget::GetDesiredInputConfig` — L27
- `UHodgeActivatableWidget::ValidateCompiledWidgetTree` — L65

## HodgeGameViewportClient.cpp

[Source/Hodgepodge/Private/UI/HodgeGameViewportClient.cpp](../../../Source/Hodgepodge/Private/UI/HodgeGameViewportClient.cpp)

引擎 CommonUI 视口路由，已在 DefaultEngine.ini 启用。

## HodgeHUDLayout.cpp

[Source/Hodgepodge/Private/UI/HodgeHUDLayout.cpp](../../../Source/Hodgepodge/Private/UI/HodgeHUDLayout.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeHUDLayout::UHodgeHUDLayout` — L76
- `UHodgeHUDLayout::NativeOnInitialized` — L89
- `UHodgeHUDLayout::NativeDestruct` — L140
- `UHodgeHUDLayout::CloseOwnedMenu` — L172
- `UHodgeHUDLayout::NativeOnDeactivated` — L181
- `UHodgeHUDLayout::EnsureMenuLayerStack` — L189
- `UHodgeHUDLayout::ToggleHUDMenu` — L198
- `UHodgeHUDLayout::HandleEscapeAction` — L204
- `UHodgeHUDLayout::HandleInputDeviceConnectionChanged` — L222
- `UHodgeHUDLayout::HandleInputDevicePairingChanged` — L246
- `UHodgeHUDLayout::ShouldPlatformDisplayControllerDisconnectScreen` — L270
- `UHodgeHUDLayout::NotifyControllerStateChangeForDisconnectScreen` — L303
- `UHodgeHUDLayout::ProcessControllerDevicesHavingChangedForDisconnectScreen` — L344
- `UHodgeHUDLayout::DisplayControllerDisconnectedMenu_Implementation` — L417
- `UHodgeHUDLayout::HideControllerDisconnectedMenu_Implementation` — L456

## HodgeJoystickWidget.cpp

[Source/Hodgepodge/Private/UI/HodgeJoystickWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeJoystickWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeJoystickWidget::UHodgeJoystickWidget` — L16
- `UHodgeJoystickWidget::NativeOnTouchStarted` — L23
- `UHodgeJoystickWidget::NativeOnTouchMoved` — L38
- `UHodgeJoystickWidget::NativeOnTouchEnded` — L52
- `UHodgeJoystickWidget::NativeOnMouseLeave` — L59
- `UHodgeJoystickWidget::NativeTick` — L66
- `UHodgeJoystickWidget::HandleTouchDelta` — L86
- `UHodgeJoystickWidget::StopInputSimulation` — L112

## HodgeResonanceEnergyWidget.cpp

[Source/Hodgepodge/Private/UI/HodgeResonanceEnergyWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeResonanceEnergyWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeResonanceEnergyWidget::UHodgeResonanceEnergyWidget` — L6

## HodgeSettingScreen.cpp

[Source/Hodgepodge/Private/UI/HodgeSettingScreen.cpp](../../../Source/Hodgepodge/Private/UI/HodgeSettingScreen.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeSimulatedInputWidget.cpp

[Source/Hodgepodge/Private/UI/HodgeSimulatedInputWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeSimulatedInputWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeSimulatedInputWidget::UHodgeSimulatedInputWidget` — L15
- `UHodgeSimulatedInputWidget::GetPaletteCategory` — L23
- `UHodgeSimulatedInputWidget::NativeConstruct` — L30
- `UHodgeSimulatedInputWidget::NativeDestruct` — L45
- `UHodgeSimulatedInputWidget::NativeOnTouchEnded` — L57
- `UHodgeSimulatedInputWidget::GetEnhancedInputSubsystem` — L65
- `UHodgeSimulatedInputWidget::GetPlayerInput` — L78
- `UHodgeSimulatedInputWidget::InputKeyValue` — L88
- `UHodgeSimulatedInputWidget::InputKeyValue2D` — L124
- `UHodgeSimulatedInputWidget::FlushSimulatedInput` — L130
- `UHodgeSimulatedInputWidget::QueryKeyToSimulate` — L139
- `UHodgeSimulatedInputWidget::OnControlMappingsRebuilt` — L157

## HodgeTaggedWidget.cpp

[Source/Hodgepodge/Private/UI/HodgeTaggedWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeTaggedWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeTaggedWidget::UHodgeTaggedWidget` — L16
- `UHodgeTaggedWidget::NativeConstruct` — L22
- `UHodgeTaggedWidget::NativeDestruct` — L47
- `UHodgeTaggedWidget::SetVisibility` — L64
- `UHodgeTaggedWidget::OnWatchedTagsChanged` — L123

## HodgeTouchRegion.cpp

[Source/Hodgepodge/Private/UI/HodgeTouchRegion.cpp](../../../Source/Hodgepodge/Private/UI/HodgeTouchRegion.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeTouchRegion::NativeOnTouchStarted` — L15
- `UHodgeTouchRegion::NativeOnTouchMoved` — L22
- `UHodgeTouchRegion::NativeOnTouchEnded` — L32
- `UHodgeTouchRegion::NativeTick` — L39

## HodgeIndicatorManagerComponent.cpp

[Source/Hodgepodge/Private/UI/IndicatorSystem/HodgeIndicatorManagerComponent.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/HodgeIndicatorManagerComponent.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeIndicatorManagerComponent::UHodgeIndicatorManagerComponent` — L17
- `UHodgeIndicatorManagerComponent::GetComponent` — L31
- `UHodgeIndicatorManagerComponent::AddIndicator` — L46
- `UHodgeIndicatorManagerComponent::RemoveIndicator` — L61

## IndicatorDescriptor.cpp

[Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorDescriptor.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorDescriptor.cpp)

模块定义或基础代码；请查看对应文件。

- `FIndicatorProjection::Project` — L24
- `UIndicatorDescriptor::SetIndicatorManagerComponent` — L257
- `UIndicatorDescriptor::UnregisterIndicator` — L270

## IndicatorLayer.cpp

[Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLayer.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLayer.cpp)

模块定义或基础代码；请查看对应文件。

- `UIndicatorLayer::UIndicatorLayer` — L27
- `UIndicatorLayer::ReleaseSlateResources` — L40
- `UIndicatorLayer::RebuildWidget` — L51

## IndicatorLibrary.cpp

[Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLibrary.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLibrary.cpp)

模块定义或基础代码；请查看对应文件。

- `UIndicatorLibrary::UIndicatorLibrary` — L19
- `UIndicatorLibrary::GetIndicatorManagerComponent` — L24

## SActorCanvas.cpp

[Source/Hodgepodge/Private/UI/IndicatorSystem/SActorCanvas.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/SActorCanvas.cpp)

模块定义或基础代码；请查看对应文件。

- `SActorCanvas::Construct` — L211
- `SActorCanvas::UpdateCanvas` — L257
- `SActorCanvas::SetShowAnyIndicators` — L488
- `SActorCanvas::OnArrangeChildren` — L510
- `SActorCanvas::OnPaint` — L800
- `SActorCanvas::~SActorCanvas` — L862
- `SActorCanvas::GetReferencerName` — L880
- `SActorCanvas::AddReferencedObjects` — L886
- `SActorCanvas::OnIndicatorAdded` — L894
- `SActorCanvas::OnIndicatorRemoved` — L910
- `SActorCanvas::AddIndicatorForEntry` — L924
- `SActorCanvas::OnIndicatorClassLoaded` — L961
- `SActorCanvas::RemoveIndicatorForEntry` — L1020
- `SActorCanvas::AddActorSlot` — L1054
- `SActorCanvas::RemoveActorSlot` — L1075
- `SActorCanvas::GetOffsetAndSize` — L1100
- `SActorCanvas::UpdateActiveTimer` — L1178

## HodgePerfStatContainerBase.cpp

[Source/Hodgepodge/Private/UI/PerformanceStats/HodgePerfStatContainerBase.cpp](../../../Source/Hodgepodge/Private/UI/PerformanceStats/HodgePerfStatContainerBase.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgePerfStatWidgetBase.cpp

[Source/Hodgepodge/Private/UI/PerformanceStats/HodgePerfStatWidgetBase.cpp](../../../Source/Hodgepodge/Private/UI/PerformanceStats/HodgePerfStatWidgetBase.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeUIManagerSubsystem.cpp

[Source/Hodgepodge/Private/UI/Subsystem/HodgeUIManagerSubsystem.cpp](../../../Source/Hodgepodge/Private/UI/Subsystem/HodgeUIManagerSubsystem.cpp)

GameInstance 所属的本地玩家 UI 管理器：根布局、Controller 绑定、输入挂起和精确撤销；专服不创建。

- `UHodgeUIManagerSubsystem::ShouldCreateSubsystem` — L20
- `UHodgeUIManagerSubsystem::Initialize` — L27
- `UHodgeUIManagerSubsystem::PlayerAdded` — L34
- `UHodgeUIManagerSubsystem::ControllerChanged` — L52
- `UHodgeUIManagerSubsystem::CreateRoot` — L74
- `UHodgeUIManagerSubsystem::DestroyRoot` — L89
- `UHodgeUIManagerSubsystem::PlayerRemoved` — L112
- `UHodgeUIManagerSubsystem::Deinitialize` — L123
- `UHodgeUIManagerSubsystem::GetRootLayout` — L133
- `UHodgeUIManagerSubsystem::IsGameInputAllowed` — L140
- `UHodgeUIManagerSubsystem::AllowsGameplayInput` — L149
- `UHodgeUIManagerSubsystem::ClearGameplayInput` — L158
- `UHodgeUIManagerSubsystem::RefreshInputState` — L170
- `UHodgeUIManagerSubsystem::SuspendInput` — L176
- `UHodgeUIManagerSubsystem::ResumeInput` — L190
- `UHodgeUIManagerSubsystem::GetUIManager` — L202
- `UHodgeUIManagerSubsystem::GetRootLayoutForController` — L208
- `UHodgeUIManagerSubsystem::GetGameplayDataForController` — L216

## HodgeUIMessaging.cpp

[Source/Hodgepodge/Private/UI/Subsystem/HodgeUIMessaging.cpp](../../../Source/Hodgepodge/Private/UI/Subsystem/HodgeUIMessaging.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeLockOnIndicatorWidget.cpp

[Source/Hodgepodge/Private/UI/Targeting/HodgeLockOnIndicatorWidget.cpp](../../../Source/Hodgepodge/Private/UI/Targeting/HodgeLockOnIndicatorWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeLockOnIndicatorWidget::RebuildWidget` — L7

## CircumferenceMarkerWidget.cpp

[Source/Hodgepodge/Private/UI/Weapons/CircumferenceMarkerWidget.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/CircumferenceMarkerWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UCircumferenceMarkerWidget::UCircumferenceMarkerWidget` — L14
- `UCircumferenceMarkerWidget::ReleaseSlateResources` — L22
- `UCircumferenceMarkerWidget::RebuildWidget` — L30
- `UCircumferenceMarkerWidget::SynchronizeProperties` — L41
- `UCircumferenceMarkerWidget::SetRadius` — L50

## HitMarkerConfirmationWidget.cpp

[Source/Hodgepodge/Private/UI/Weapons/HitMarkerConfirmationWidget.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/HitMarkerConfirmationWidget.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeReticleWidgetBase.cpp

[Source/Hodgepodge/Private/UI/Weapons/HodgeReticleWidgetBase.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/HodgeReticleWidgetBase.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeWeaponUserInterface.cpp

[Source/Hodgepodge/Private/UI/Weapons/HodgeWeaponUserInterface.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/HodgeWeaponUserInterface.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## SCircumferenceMarkerWidget.cpp

[Source/Hodgepodge/Private/UI/Weapons/SCircumferenceMarkerWidget.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/SCircumferenceMarkerWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `SCircumferenceMarkerWidget::SCircumferenceMarkerWidget` — L17
- `SCircumferenceMarkerWidget::Construct` — L22
- `SCircumferenceMarkerWidget::GetMarkerRenderTransform` — L32
- `SCircumferenceMarkerWidget::OnPaint` — L62
- `SCircumferenceMarkerWidget::ComputeDesiredSize` — L100
- `SCircumferenceMarkerWidget::SetRadius` — L109
- `SCircumferenceMarkerWidget::SetMarkerList` — L119

## SHitMarkerConfirmationWidget.cpp

[Source/Hodgepodge/Private/UI/Weapons/SHitMarkerConfirmationWidget.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/SHitMarkerConfirmationWidget.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeAbilityCost.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h)

自定义额外能力消耗的扩展契约。

## HodgeAbilityCost_ResonanceEnergy.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.h)

模块定义或基础代码；请查看对应文件。

## HodgeAbilityTask_WaitHitResults.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h)

模块定义或基础代码；请查看对应文件。

## HodgeAbilityTask_WaitMoveCancel.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h)

合流取消窗口与本地移动意图，广播 OnMoveCancel 后结束；已有取消验证，当前 Combat 主链与旧 BasicAttack 消费方需区分。

## HodgeGameplayAbility.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。（PreloadPrimaryAssetsOnGrant 已随未提交改动回退，当前不存在。）

## HodgeGameplayAbility_Dash.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayAbility_Death.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Death.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Death.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayAbility_Definition.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)

执行单段 Definition、本次 Montage 时钟 Timeline 与手持窗口，按执行清理；Melee 子类扩展命中。

## HodgeGameplayAbility_EnemyDeath.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayAbility_HavocRules.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayAbility_HitReaction.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayAbility_Jump.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayAbility_LockOn.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayAbility_Melee.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h)

接收服务器 Window/Point 命中结果，准备锚点/目标、逐段/共享去重，构建独立 GE Spec/Context 并施加。

## HodgeGameplayAbility_MovementAction.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayAbility_ResonanceCircuit.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_ResonanceCircuit.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_ResonanceCircuit.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayAbility_Sprint.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h)

模块定义或基础代码；请查看对应文件。

## HodgeAttributeSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)

项目 AttributeSet 基础和 ASC 访问。

## HodgeCombatSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)

BaseDamage/BaseHeal 战斗属性，OwnerOnly 复制，供伤害/治疗 Execution 捕获。

## HodgeHavocSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)

模块定义或基础代码；请查看对应文件。

## HodgeHealthSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)

Health/MaxHealth、Damage/Healing 元属性、结算夹取/免疫/耗尽广播；BaseDamage/BaseHeal 位于 CombatSet。

## HodgeResonanceSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)

模块定义或基础代码；请查看对应文件。

## HodgeHavocEffects.h

[Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeHavocEffects.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeHavocEffects.h)

模块定义或基础代码；请查看对应文件。

## HodgeResonanceEnergyEffect.h

[Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h)

模块定义或基础代码；请查看对应文件。

## HodgeDamageExecution.h

[Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeDamageExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeDamageExecution.h)

BaseDamage × SetByCaller 倍率 × 距离/材质衰减 × 共享目标规则；不再恒零。

## HodgeHealExecution.h

[Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeHealExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeHealExecution.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayCue_Movement.h

[Source/Hodgepodge/Public/AbilitySystem/GameplayCues/HodgeGameplayCue_Movement.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayCues/HodgeGameplayCue_Movement.h)

模块定义或基础代码；请查看对应文件。

## GameplayTagStack.h

[Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h)

带计数的标签栈及复制数据结构，区别于只判断有无的 TagContainer。

## HodgeAbilitySystemComponent.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

Tag 输入、激活组、关系映射、Montage 复制和全局注册；失败时恢复武器预测与连段记忆。

## HodgeAbilitySystemGlobals.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemGlobals.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemGlobals.h)

分配 FHodgeGameplayEffectContext 的 GAS 全局类；已加入项目配置。

## HodgeAbilityTagRelationshipMapping.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h)

数据驱动的能力阻断、取消与激活条件关系。

## HodgeGameplayCueManager.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayCueManager.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayCueManager.h)

项目 Cue 管理类已配置；启动预加载及 Feature Cue 观察者生命周期仍未完整接通。

## HodgeGameplayEffectContext.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)

项目 GE 上下文与序列化扩展，已有 HodgeAbilitySystemGlobals 分配配套。

## HodgeGameplayTags.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

原生 GameplayTag 注册和移动状态标签映射；含攻击时间轴依赖的 Status.Attack.*（阶段 + 取消窗口 .Cancel.*）与 GameplayEvent.Attack.*。标签存在不等于对应玩法实现。

## HodgeGlobalAbilitySystem.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h)

世界级全局能力/效果授予及 ASC 注册表。

## HodgeLockOnStateEffect.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeLockOnStateEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeLockOnStateEffect.h)

模块定义或基础代码；请查看对应文件。

## HodgeAttributeCoordinator.h

[Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeCoordinator.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeCoordinator.h)

模块定义或基础代码；请查看对应文件。

## HodgeAttributeTypes.h

[Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeTypes.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeTypes.h)

模块定义或基础代码；请查看对应文件。

## HodgeCharacterBaseStatEffect.h

[Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeCharacterBaseStatEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeCharacterBaseStatEffect.h)

模块定义或基础代码；请查看对应文件。

## HodgeEquipmentStatEffect.h

[Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeEquipmentStatEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeEquipmentStatEffect.h)

模块定义或基础代码；请查看对应文件。

## HodgeActorBase.h

[Source/Hodgepodge/Public/Actor/HodgeActorBase.h](../../../Source/Hodgepodge/Public/Actor/HodgeActorBase.h)

项目 Actor 基类扩展。

## HodgeBTTask_ExecuteAbility.h

[Source/Hodgepodge/Public/AI/HodgeBTTask_ExecuteAbility.h](../../../Source/Hodgepodge/Public/AI/HodgeBTTask_ExecuteAbility.h)

模块定义或基础代码；请查看对应文件。

## HodgeEnemyAIController.h

[Source/Hodgepodge/Public/AI/HodgeEnemyAIController.h](../../../Source/Hodgepodge/Public/AI/HodgeEnemyAIController.h)

模块定义或基础代码；请查看对应文件。

## HodgeAnimInstance.h

[Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)

ASC 属性映射、GroundDistance 与旋转策略快照；FullBody 权重协调蓝图程序性根 Yaw。

## HodgeAnimNotify_DarkSurgeCommit.h

[Source/Hodgepodge/Public/Animation/HodgeAnimNotify_DarkSurgeCommit.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotify_DarkSurgeCommit.h)

模块定义或基础代码；请查看对应文件。

## HodgeAnimNotifyState_CinematicCamera.h

[Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_CinematicCamera.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_CinematicCamera.h)

模块定义或基础代码；请查看对应文件。

## HodgeAnimNotifyState_WingAction.h

[Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_WingAction.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_WingAction.h)

模块定义或基础代码；请查看对应文件。

## HodgeCombatAnimNotifies.h

[Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)

模块定义或基础代码；请查看对应文件。

## HodgeEnemyAnimInstance.h

[Source/Hodgepodge/Public/Animation/HodgeEnemyAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeEnemyAnimInstance.h)

模块定义或基础代码；请查看对应文件。

## HodgeRootMotionModifier_AttackFacing.h

[Source/Hodgepodge/Public/Animation/HodgeRootMotionModifier_AttackFacing.h](../../../Source/Hodgepodge/Public/Animation/HodgeRootMotionModifier_AttackFacing.h)

模块定义或基础代码；请查看对应文件。

## HodgeWingAnimInstance.h

[Source/Hodgepodge/Public/Animation/HodgeWingAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeWingAnimInstance.h)

模块定义或基础代码；请查看对应文件。

## HodgeWingProfile.h

[Source/Hodgepodge/Public/Animation/HodgeWingProfile.h](../../../Source/Hodgepodge/Public/Animation/HodgeWingProfile.h)

模块定义或基础代码；请查看对应文件。

## HodgeCameraAssistInterface.h

[Source/Hodgepodge/Public/Camera/HodgeCameraAssistInterface.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraAssistInterface.h)

相机辅助接口契约。

## HodgeCameraComponent.h

[Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)

相机模式栈宿主与最终视图输出，依赖默认模式委托。

## HodgeCameraMode.h

[Source/Hodgepodge/Public/Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)

相机视图、模式实例、混合和模式栈。

## HodgeCameraMode_Cinematic.h

[Source/Hodgepodge/Public/Camera/HodgeCameraMode_Cinematic.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_Cinematic.h)

模块定义或基础代码；请查看对应文件。

## HodgeCameraMode_LockOn.h

[Source/Hodgepodge/Public/Camera/HodgeCameraMode_LockOn.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_LockOn.h)

模块定义或基础代码；请查看对应文件。

## HodgeCameraMode_ThirdPerson.h

[Source/Hodgepodge/Public/Camera/HodgeCameraMode_ThirdPerson.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_ThirdPerson.h)

第三人称偏移与防穿透逻辑；C++ 不设默认偏移曲线，改由蓝图/PawnData 提供。

## HodgeCinematicCameraTypes.h

[Source/Hodgepodge/Public/Camera/HodgeCinematicCameraTypes.h](../../../Source/Hodgepodge/Public/Camera/HodgeCinematicCameraTypes.h)

模块定义或基础代码；请查看对应文件。

## HodgePenetrationAvoidanceFeeler.h

[Source/Hodgepodge/Public/Camera/HodgePenetrationAvoidanceFeeler.h](../../../Source/Hodgepodge/Public/Camera/HodgePenetrationAvoidanceFeeler.h)

相机防穿透探测参数结构。

## HodgePlayerCameraManager.h

[Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h](../../../Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h)

由 HodgePlayerController 构造选用的项目相机管理器；运行效果待验收。

## HodgeUICameraManagerComponent.h

[Source/Hodgepodge/Public/Camera/HodgeUICameraManagerComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeUICameraManagerComponent.h)

UI 相机管理扩展，不代表 UI 系统已接入。

## HodgeCharacterBase.h

[Source/Hodgepodge/Public/Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)

原生 Character 基础、替换移动组件；Receiver 在 PreInit 注册、EndPlay 成对移除。

## HodgeCombatCharacter.h

[Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)

PawnExtension、相机、HealthComponent、原生 RotationComponent；ASC、移动/旋转约束、复制与死亡清理。

## HodgeEnemyCharacter.h

[Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)

仅设置 AI 自动控制；旋转/移动参数继承 Combat 基类，尚未完成敌人 ASC 初始化。

## HodgeHeroCharacter.h

[Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h)

构造挂载 HeroComponent；PossessedBy/OnRep_PlayerState 只调用 Super，ASC 接入已收敛。

## HodgeALSLocomotion.h

[Source/Hodgepodge/Public/CodexText/HodgeALSLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeALSLocomotion.h)

CodexText 实验：6 向地面运动动画实例，不属于主 Hero 动画链。

## HodgeGroundedLocomotion.h

[Source/Hodgepodge/Public/CodexText/HodgeGroundedLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeGroundedLocomotion.h)

CodexText 实验：在 ALS 基础动画上增加平地起停/转身/脚锁，并扩展上半身 Overlay、左手握持、步幅与地形 IK。

## HodgeLocomotionLab.h

[Source/Hodgepodge/Public/CodexText/HodgeLocomotionLab.h](../../../Source/Hodgepodge/Public/CodexText/HodgeLocomotionLab.h)

模块定义或基础代码；请查看对应文件。

## HodgeSurvivor.h

[Source/Hodgepodge/Public/CodexText/HodgeSurvivor.h](../../../Source/Hodgepodge/Public/CodexText/HodgeSurvivor.h)

模块定义或基础代码；请查看对应文件。

## HodgeAttackFacingTypes.h

[Source/Hodgepodge/Public/Combat/HodgeAttackFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackFacingTypes.h)

模块定义或基础代码；请查看对应文件。

## HodgeCharacterFacingTypes.h

[Source/Hodgepodge/Public/Combat/HodgeCharacterFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeCharacterFacingTypes.h)

模块定义或基础代码；请查看对应文件。

## HodgeCombatActionRequest.h

[Source/Hodgepodge/Public/Combat/HodgeCombatActionRequest.h](../../../Source/Hodgepodge/Public/Combat/HodgeCombatActionRequest.h)

模块定义或基础代码；请查看对应文件。

## HodgeDamageRules.h

[Source/Hodgepodge/Public/Combat/HodgeDamageRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeDamageRules.h)

检测和 Execution 共用目标、ASC、死亡/Health、自伤/友伤及队伍规则。

## HodgeHavocRules.h

[Source/Hodgepodge/Public/Combat/HodgeHavocRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeHavocRules.h)

模块定义或基础代码；请查看对应文件。

## HodgeHitDetection.h

[Source/Hodgepodge/Public/Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)

模块定义或基础代码；请查看对应文件。

## HodgeHitReactionTypes.h

[Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)

模块定义或基础代码；请查看对应文件。

## HodgeMovementActionTypes.h

[Source/Hodgepodge/Public/Combat/HodgeMovementActionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeMovementActionTypes.h)

模块定义或基础代码；请查看对应文件。

## HodgeActorComponentBase.h

[Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)

通用 ActorComponent 基础访问与扩展。

## HodgeAttackMotionWarpingComponent.h

[Source/Hodgepodge/Public/Component/HodgeAttackMotionWarpingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeAttackMotionWarpingComponent.h)

模块定义或基础代码；请查看对应文件。

## HodgeCharacterMovementComponent.h

[Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)

地面距离/加速度、最终旋转过滤、SavedMove 旋转策略重放与权威校正。

## HodgeCharacterRotationComponent.h

[Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)

ASC 旋转标签约束、锁定 Yaw、恢复、权威复制与移动重放状态。

## HodgeCombatComponentBase.h

[Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)

Experience 注入 Pawn 的统一战斗协调者；连段输入/记忆/预测校正、来源/配置检测体、独立会话与过滤；GA 负责目标去重及效果施加。

## HodgeDefenseComponent.h

[Source/Hodgepodge/Public/Component/HodgeDefenseComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeDefenseComponent.h)

模块定义或基础代码；请查看对应文件。

## HodgeEnemyInitComponent.h

[Source/Hodgepodge/Public/Component/HodgeEnemyInitComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeEnemyInitComponent.h)

模块定义或基础代码；请查看对应文件。

## HodgeExperienceManagerComponent.h

[Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)

GameState 上的 Experience 复制、资源加载、插件激活、Action 执行与 Loaded 委托。

## HodgeHealthComponent.h

[Source/Hodgepodge/Public/Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)

模块定义或基础代码；请查看对应文件。

## HodgeHeroComponent.h

[Source/Hodgepodge/Public/Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)

玩家 Init State 协调、ASC 接入、输入与相机；额外输入句柄持久化并在移除/EndPlay 解绑。新增“移动意图”信号（HasMoveIntent / GetMoveIntent / OnMoveIntentChanged），记 Input_Move 原始输入量、Completed/Canceled 清零，供移动取消后摇消费。

## HodgeHitReactionComponent.h

[Source/Hodgepodge/Public/Component/HodgeHitReactionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHitReactionComponent.h)

模块定义或基础代码；请查看对应文件。

## HodgeInteractionComponentBase.h

[Source/Hodgepodge/Public/Component/HodgeInteractionComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeInteractionComponentBase.h)

交互组件基础占位；完整扫描、交互规则与 UI 需另行实现。

## HodgeLockOnComponent.h

[Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h)

模块定义或基础代码；请查看对应文件。

## HodgeLocomotionPolicyComponent.h

[Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h)

模块定义或基础代码；请查看对应文件。

## HodgeMovementComponentBase.h

[Source/Hodgepodge/Public/Component/HodgeMovementComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeMovementComponentBase.h)

通用移动组件基础占位，与 CharacterMovement 派生类需区分。

## HodgePawnExtensionComponent.h

[Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)

PawnData 复制、Init State、ASC 关联/解除、TagRelationshipMapping 与 ClearAbilityInput；需验证退出顺序。

## HodgeWingComponent.h

[Source/Hodgepodge/Public/Component/HodgeWingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeWingComponent.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameInstanceBase.h

[Source/Hodgepodge/Public/Core/GameInstance/HodgeGameInstanceBase.h](../../../Source/Hodgepodge/Public/Core/GameInstance/HodgeGameInstanceBase.h)

注册 Init State 顺序、主控制器访问和全局生命周期扩展。

## HodgeGameModeBase.h

[Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h](../../../Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h)

服务器选择玩法、等待加载、生成 Pawn，并在 FinishSpawning 前注入 PawnData；选用 HodgePlayerController。

## HodgeGameState.h

[Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h)

创建 ExperienceManager 和世界状态 ASC，处理游戏状态复制/扩展。

## HodgeGameStateBase.h

[Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h)

GameState 基础扩展生命周期。

## HodgeHUD.h

[Source/Hodgepodge/Public/Core/HUD/HodgeHUD.h](../../../Source/Hodgepodge/Public/Core/HUD/HodgeHUD.h)

HUD Receiver 提供 Experience AddWidgets 的真实注入／撤销入口，同时保留 GAS 调试 Actor 列表。

## HodgeLocalPlayerBase.h

[Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)

本地玩家对象及控制器、PlayerState、Pawn 就绪事件桥。

## HodgePlayerController.h

[Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)

具体控制器：每帧消费 ASC 输入、相机管理、AutoRun、UnPossess Avatar 清理及 Replay 扩展。

## HodgePlayerControllerBase.h

[Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h)

控制器和 Pawn 生命周期桥接到 LocalPlayer 委托；具体输入消费在派生 HodgePlayerController。

## HodgePlayerState.h

[Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)

玩家 ASC、HealthSet、PawnData、阵营/标签栈等持有者；SetPawnData 在权威端授予 AbilitySets（未记录句柄）。

## HodgePlayerStateBase.h

[Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h)

PlayerState ModularGameplay Receiver 注册、注销及组件 Reset/CopyProperties。

## HodgeAbilityDefinition.h

[Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)

Montage/Blend/Timeline、HitWindows/HitPoints、Volume、独立/连段路由及输入；校验几何、权威消息与手持覆盖。

## HodgeAbilitySet.h

[Source/Hodgepodge/Public/Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)

权威端批量授予属性集、技能和 GE，并用句柄集合撤销。

## HodgeAssetManager.h

[Source/Hodgepodge/Public/Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)

资产入口、GameData 缓存、启动任务、同步加载、加载进度。Cue 初始化钩子仍需接通。（PreloadPrimaryAssetBundles 已随未提交改动回退，当前不存在。）

- `UHodgeAssetManager::GetAsset` — L198
- `UHodgeAssetManager::GetSubclass` — L235

## HodgeAssetManagerStartupJob.h

[Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h)

封装启动任务与进度权重，供 AssetManager 执行启动工作。

## HodgeCharacterStatProfile.h

[Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h)

模块定义或基础代码；请查看对应文件。

## HodgeCinematicCameraProfile.h

[Source/Hodgepodge/Public/Data/HodgeCinematicCameraProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCinematicCameraProfile.h)

模块定义或基础代码；请查看对应文件。

## HodgeComboDefinition.h

[Source/Hodgepodge/Public/Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)

跳转 DataTable、输入缓存、结束后连段记忆与 bAllowAfterExecutionEnded 续段许可。

## HodgeEquipmentStatProfile.h

[Source/Hodgepodge/Public/Data/HodgeEquipmentStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeEquipmentStatProfile.h)

模块定义或基础代码；请查看对应文件。

## HodgeExperienceActionSet.h

[Source/Hodgepodge/Public/Data/HodgeExperienceActionSet.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceActionSet.h)

复用 GameFeature 插件和动作配置的数据资产。

## HodgeExperienceDefinition.h

[Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)

声明玩法所需插件、默认 PawnData、直接 Actions 和组合 ActionSets。

## HodgeExperienceManager.h

[Source/Hodgepodge/Public/Data/HodgeExperienceManager.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceManager.h)

管理编辑器等场景的 GameFeature 使用/停用协调，不是挂载在 GameState 的组件。

## HodgeGameData.h

[Source/Hodgepodge/Public/Data/HodgeGameData.h](../../../Source/Hodgepodge/Public/Data/HodgeGameData.h)

全局伤害、治疗、动态 Tag GE 的软类引用配置；需编辑器核对实际赋值。

## HodgeHitReactionProfile.h

[Source/Hodgepodge/Public/Data/HodgeHitReactionProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeHitReactionProfile.h)

模块定义或基础代码；请查看对应文件。

## HodgeLockOnProfile.h

[Source/Hodgepodge/Public/Data/HodgeLockOnProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeLockOnProfile.h)

模块定义或基础代码；请查看对应文件。

## HodgePawnData.h

[Source/Hodgepodge/Public/Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)

PawnClass、AbilitySets、ComboDefinition、DefaultWeaponDefinition、输入/相机/关系映射配置，编辑器校验统一在主 cpp。

## HodgeSprintAbilityProfile.h

[Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h)

模块定义或基础代码；请查看对应文件。

## HodgeEquipmentDefinition.h

[Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)

装备实例类、Actor 挂接与 AbilitySets 配置。

## HodgeEquipmentInstance.h

[Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)

Pawn 所属复制 UObject、SpawnedActors RepNotify 和装备/卸装生命周期。

## HodgeEquipmentManagerComponent.h

[Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)

Experience 注入 Pawn，ASC 就绪后装备默认剑，来源授予句柄精确撤销与客户端迟到绑定。

## HodgeWeaponInstance.h

[Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)

手持请求、显现阶段、计时器、复制/拥有者预测及拒绝恢复，不新增角色常驻武器表现组件。

## HodgeWeaponPresentationActor.h

[Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationActor.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationActor.h)

检测 Mesh 留手，可见 Mesh 回背/悬浮/消隐挂 BackSocket，手持时挂回检测 Mesh。

## HodgeWeaponPresentationProfile.h

[Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h)

模型/材质、手背插槽/偏移、曲线/时间；BackSocket 默认 WeaponOnBack，BackTransform 为插槽内偏移。

## HodgeWeaponPresentationTypes.h

[Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationTypes.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationTypes.h)

表现阶段与复制快照：服务器时间、起始变换/可见度、版本和激活身份。

## GameFeatureAction_AddAbilities.h

[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddAbilities.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddAbilities.h)

面向配置 Actor 授予能力、属性与 AbilitySet，维护撤销句柄。

## GameFeatureAction_AddGameplayCuePath.h

[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddGameplayCuePath.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddGameplayCuePath.h)

声明和校验 Cue 路径；Policy 添加路径主体已有，但观察者注册和注销清理仍缺。

## GameFeatureAction_AddInputBinding.h

[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputBinding.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputBinding.h)

有效扩展事件添加/移除额外 InputConfig；Hero 侧移除已实现并与 EndPlay 清理配套。

## GameFeatureAction_AddInputContextMapping.h

[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputContextMapping.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputContextMapping.h)

有效 Controller 扩展添加 IMC，包含设置注册与诊断日志；记录/撤销需验收。

## GameFeatureAction_AddWidget.h

[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddWidget.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddWidget.h)

Widget 注入迁移草稿，当前实现停用。

## GameFeatureAction_SplitscreenConfig.h

[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_SplitscreenConfig.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_SplitscreenConfig.h)

GameFeature 激活期间的分屏策略调整。

## GameFeatureAction_WorldActionBase.h

[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)

按游戏世界和激活上下文组织 Action 生命周期。

## HodgeGameFeaturePolicy.h

[Source/Hodgepodge/Public/GameFeatures/HodgeGameFeaturePolicy.h](../../../Source/Hodgepodge/Public/GameFeatures/HodgeGameFeaturePolicy.h)

已配置的 GameFeature 策略；Hotfix 观察者注册，Cue 路径观察者创建仍注释。

## HodgeAimSensitivityData.h

[Source/Hodgepodge/Public/Input/HodgeAimSensitivityData.h](../../../Source/Hodgepodge/Public/Input/HodgeAimSensitivityData.h)

瞄准灵敏度数据映射。

## HodgeInputComponent.h

[Source/Hodgepodge/Public/Input/HodgeInputComponent.h](../../../Source/Hodgepodge/Public/Input/HodgeInputComponent.h)

基于 Tag 的 Native/Ability Action 绑定和句柄移除；映射辅助函数仍占位。

- `UHodgeInputComponent::BindNativeAction` — L176
- `UHodgeInputComponent::BindAbilityActions` — L195

## HodgeInputConfig.h

[Source/Hodgepodge/Public/Input/HodgeInputConfig.h](../../../Source/Hodgepodge/Public/Input/HodgeInputConfig.h)

NativeInputActions / AbilityInputActions 的 IA 与 Tag 数据配置及查询。

## HodgeInputModifiers.h

[Source/Hodgepodge/Public/Input/HodgeInputModifiers.h](../../../Source/Hodgepodge/Public/Input/HodgeInputModifiers.h)

输入数值处理扩展；具体启用情况由 IA/IMC 资产决定。

## HodgeInputUserSettings.h

[Source/Hodgepodge/Public/Input/HodgeInputUserSettings.h](../../../Source/Hodgepodge/Public/Input/HodgeInputUserSettings.h)

Enhanced Input 用户设置派生入口；须核对实际设置类配置。

## HodgePlayerMappableKeyProfile.h

[Source/Hodgepodge/Public/Input/HodgePlayerMappableKeyProfile.h](../../../Source/Hodgepodge/Public/Input/HodgePlayerMappableKeyProfile.h)

玩家键位 Profile 扩展。

## HodgeAbilitySourceInterface.h

[Source/Hodgepodge/Public/Interface/HodgeAbilitySourceInterface.h](../../../Source/Hodgepodge/Public/Interface/HodgeAbilitySourceInterface.h)

技能来源与相关计算契约。

## HodgeLockOnTargetInterface.h

[Source/Hodgepodge/Public/Interface/HodgeLockOnTargetInterface.h](../../../Source/Hodgepodge/Public/Interface/HodgeLockOnTargetInterface.h)

模块定义或基础代码；请查看对应文件。

## LoadingProcessInterface.h

[Source/Hodgepodge/Public/Interface/LoadingProcessInterface.h](../../../Source/Hodgepodge/Public/Interface/LoadingProcessInterface.h)

加载状态/原因查询契约；不是独立加载界面。

## HodgeLockOnTypes.h

[Source/Hodgepodge/Public/Targeting/HodgeLockOnTypes.h](../../../Source/Hodgepodge/Public/Targeting/HodgeLockOnTypes.h)

模块定义或基础代码；请查看对应文件。

## MaterialProgressBar.h

[Source/Hodgepodge/Public/UI/Basic/MaterialProgressBar.h](../../../Source/Hodgepodge/Public/UI/Basic/MaterialProgressBar.h)

模块定义或基础代码；请查看对应文件。

## HodgeBoundActionButton.h

[Source/Hodgepodge/Public/UI/Common/HodgeBoundActionButton.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeBoundActionButton.h)

模块定义或基础代码；请查看对应文件。

## HodgeListView.h

[Source/Hodgepodge/Public/UI/Common/HodgeListView.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeListView.h)

模块定义或基础代码；请查看对应文件。

## HodgeTabButtonBase.h

[Source/Hodgepodge/Public/UI/Common/HodgeTabButtonBase.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeTabButtonBase.h)

模块定义或基础代码；请查看对应文件。

## HodgeTabListWidgetBase.h

[Source/Hodgepodge/Public/UI/Common/HodgeTabListWidgetBase.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeTabListWidgetBase.h)

模块定义或基础代码；请查看对应文件。

## HodgeWidgetFactory.h

[Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory.h)

模块定义或基础代码；请查看对应文件。

## HodgeWidgetFactory_Class.h

[Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory_Class.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory_Class.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayUIDataSource.h

[Source/Hodgepodge/Public/UI/Data/HodgeGameplayUIDataSource.h](../../../Source/Hodgepodge/Public/UI/Data/HodgeGameplayUIDataSource.h)

每个本地玩家共享的生命订阅、属性就绪与技能状态/输入接入；布局和按钮行为保存在 WBP。

## UIExtensionPointWidget.h

[Source/Hodgepodge/Public/UI/Extension/UIExtensionPointWidget.h](../../../Source/Hodgepodge/Public/UI/Extension/UIExtensionPointWidget.h)

模块定义或基础代码；请查看对应文件。

## UIExtensionSystem.h

[Source/Hodgepodge/Public/UI/Extension/UIExtensionSystem.h](../../../Source/Hodgepodge/Public/UI/Extension/UIExtensionSystem.h)

模块定义或基础代码；请查看对应文件。

## HodgeActionWidget.h

[Source/Hodgepodge/Public/UI/Foundation/HodgeActionWidget.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeActionWidget.h)

模块定义或基础代码；请查看对应文件。

## HodgeButtonBase.h

[Source/Hodgepodge/Public/UI/Foundation/HodgeButtonBase.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeButtonBase.h)

模块定义或基础代码；请查看对应文件。

## HodgeConfirmationScreen.h

[Source/Hodgepodge/Public/UI/Foundation/HodgeConfirmationScreen.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeConfirmationScreen.h)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeControllerDisconnectedScreen.h

[Source/Hodgepodge/Public/UI/Foundation/HodgeControllerDisconnectedScreen.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeControllerDisconnectedScreen.h)

模块定义或基础代码；请查看对应文件。

## HodgeLoadingScreenSubsystem.h

[Source/Hodgepodge/Public/UI/Foundation/HodgeLoadingScreenSubsystem.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeLoadingScreenSubsystem.h)

模块定义或基础代码；请查看对应文件。

## HodgePrimaryGameLayout.h

[Source/Hodgepodge/Public/UI/Foundation/HodgePrimaryGameLayout.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgePrimaryGameLayout.h)

绑定 Designer 四个容器的本地玩家根布局，Push/Pop、异步加载取消与令牌恢复；不再生成运行时控件树。

## ApplyFrontendPerfSettingsAction.h

[Source/Hodgepodge/Public/UI/Frontend/ApplyFrontendPerfSettingsAction.h](../../../Source/Hodgepodge/Public/UI/Frontend/ApplyFrontendPerfSettingsAction.h)

模块定义或基础代码；请查看对应文件。

## HodgeFrontendStateComponent.h

[Source/Hodgepodge/Public/UI/Frontend/HodgeFrontendStateComponent.h](../../../Source/Hodgepodge/Public/UI/Frontend/HodgeFrontendStateComponent.h)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeLobbyBackground.h

[Source/Hodgepodge/Public/UI/Frontend/HodgeLobbyBackground.h](../../../Source/Hodgepodge/Public/UI/Frontend/HodgeLobbyBackground.h)

模块定义或基础代码；请查看对应文件。

## HodgeAbilityIconWidget.h

[Source/Hodgepodge/Public/UI/HodgeAbilityIconWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeAbilityIconWidget.h)

模块定义或基础代码；请查看对应文件。

## HodgeActivatableWidget.h

[Source/Hodgepodge/Public/UI/HodgeActivatableWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeActivatableWidget.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameViewportClient.h

[Source/Hodgepodge/Public/UI/HodgeGameViewportClient.h](../../../Source/Hodgepodge/Public/UI/HodgeGameViewportClient.h)

引擎 CommonUI 视口路由，已在 DefaultEngine.ini 启用。

## HodgeHUDLayout.h

[Source/Hodgepodge/Public/UI/HodgeHUDLayout.h](../../../Source/Hodgepodge/Public/UI/HodgeHUDLayout.h)

模块定义或基础代码；请查看对应文件。

## HodgeJoystickWidget.h

[Source/Hodgepodge/Public/UI/HodgeJoystickWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeJoystickWidget.h)

模块定义或基础代码；请查看对应文件。

## HodgeResonanceEnergyWidget.h

[Source/Hodgepodge/Public/UI/HodgeResonanceEnergyWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeResonanceEnergyWidget.h)

模块定义或基础代码；请查看对应文件。

## HodgeSettingScreen.h

[Source/Hodgepodge/Public/UI/HodgeSettingScreen.h](../../../Source/Hodgepodge/Public/UI/HodgeSettingScreen.h)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeSimulatedInputWidget.h

[Source/Hodgepodge/Public/UI/HodgeSimulatedInputWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeSimulatedInputWidget.h)

模块定义或基础代码；请查看对应文件。

## HodgeTaggedWidget.h

[Source/Hodgepodge/Public/UI/HodgeTaggedWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeTaggedWidget.h)

模块定义或基础代码；请查看对应文件。

## HodgeTouchRegion.h

[Source/Hodgepodge/Public/UI/HodgeTouchRegion.h](../../../Source/Hodgepodge/Public/UI/HodgeTouchRegion.h)

模块定义或基础代码；请查看对应文件。

## HodgeIndicatorManagerComponent.h

[Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h)

模块定义或基础代码；请查看对应文件。

## IActorIndicatorWidget.h

[Source/Hodgepodge/Public/UI/IndicatorSystem/IActorIndicatorWidget.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IActorIndicatorWidget.h)

模块定义或基础代码；请查看对应文件。

## IndicatorDescriptor.h

[Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h)

模块定义或基础代码；请查看对应文件。

## IndicatorLayer.h

[Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLayer.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLayer.h)

模块定义或基础代码；请查看对应文件。

## IndicatorLibrary.h

[Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLibrary.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLibrary.h)

模块定义或基础代码；请查看对应文件。

## SActorCanvas.h

[Source/Hodgepodge/Public/UI/IndicatorSystem/SActorCanvas.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/SActorCanvas.h)

模块定义或基础代码；请查看对应文件。

## HodgePerfStatContainerBase.h

[Source/Hodgepodge/Public/UI/PerformanceStats/HodgePerfStatContainerBase.h](../../../Source/Hodgepodge/Public/UI/PerformanceStats/HodgePerfStatContainerBase.h)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgePerfStatWidgetBase.h

[Source/Hodgepodge/Public/UI/PerformanceStats/HodgePerfStatWidgetBase.h](../../../Source/Hodgepodge/Public/UI/PerformanceStats/HodgePerfStatWidgetBase.h)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeUIManagerSubsystem.h

[Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)

GameInstance 所属的本地玩家 UI 管理器：根布局、Controller 绑定、输入挂起和精确撤销；专服不创建。

## HodgeUIMessaging.h

[Source/Hodgepodge/Public/UI/Subsystem/HodgeUIMessaging.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIMessaging.h)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeLockOnIndicatorWidget.h

[Source/Hodgepodge/Public/UI/Targeting/HodgeLockOnIndicatorWidget.h](../../../Source/Hodgepodge/Public/UI/Targeting/HodgeLockOnIndicatorWidget.h)

模块定义或基础代码；请查看对应文件。

## CircumferenceMarkerWidget.h

[Source/Hodgepodge/Public/UI/Weapons/CircumferenceMarkerWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/CircumferenceMarkerWidget.h)

模块定义或基础代码；请查看对应文件。

## HitMarkerConfirmationWidget.h

[Source/Hodgepodge/Public/UI/Weapons/HitMarkerConfirmationWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/HitMarkerConfirmationWidget.h)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeReticleWidgetBase.h

[Source/Hodgepodge/Public/UI/Weapons/HodgeReticleWidgetBase.h](../../../Source/Hodgepodge/Public/UI/Weapons/HodgeReticleWidgetBase.h)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeWeaponUserInterface.h

[Source/Hodgepodge/Public/UI/Weapons/HodgeWeaponUserInterface.h](../../../Source/Hodgepodge/Public/UI/Weapons/HodgeWeaponUserInterface.h)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## SCircumferenceMarkerWidget.h

[Source/Hodgepodge/Public/UI/Weapons/SCircumferenceMarkerWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/SCircumferenceMarkerWidget.h)

模块定义或基础代码；请查看对应文件。

## SHitMarkerConfirmationWidget.h

[Source/Hodgepodge/Public/UI/Weapons/SHitMarkerConfirmationWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/SHitMarkerConfirmationWidget.h)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## Hodgepodge.Target.cs

[Source/Hodgepodge.Target.cs](../../../Source/Hodgepodge.Target.cs)

模块定义或基础代码；请查看对应文件。

## HodgepodgeEditor.Target.cs

[Source/HodgepodgeEditor.Target.cs](../../../Source/HodgepodgeEditor.Target.cs)

模块定义或基础代码；请查看对应文件。
