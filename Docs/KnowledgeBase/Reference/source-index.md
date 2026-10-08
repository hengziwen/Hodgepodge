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

- `UHodgeAnimationAuthoringLibrary::ImportAnimationBlueprintText` — L97
- `UHodgeAnimationAuthoringLibrary::SetAnimationDefault` — L163
- `UHodgeAnimationAuthoringLibrary::SetAnimationNodeProperty` — L172
- `UHodgeAnimationAuthoringLibrary::RemapAnimationReferences` — L183
- `UHodgeAnimationAuthoringLibrary::CompileAnimationBlueprint` — L206
- `UHodgeAnimationAuthoringLibrary::GetSkeletonBoneNames` — L223
- `UHodgeAnimationAuthoringLibrary::ConfigureAnimationLabPIE` — L233
- `UHodgeAnimationAuthoringLibrary::ConfigureAnimationLabGameMode` — L243

## HodgeAnimationAuthoringLibrary.h

[Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.h](../../../Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.h)

Editor 动画文本导入、属性/引用修正、编译诊断与实验 PIE 配置。

## HodgeCombatValidationLibrary.cpp

[Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeCombatValidationLibrary::QueueAbilityAction` — L13
- `UHodgeCombatValidationLibrary::AddHitNotify` — L75
- `UHodgeCombatValidationLibrary::AddStateNotify` — L89
- `UHodgeCombatValidationLibrary::AddWeaponHandNotify` — L97
- `UHodgeCombatValidationLibrary::InspectAbilityWindows` — L103
- `UHodgeCombatValidationLibrary::InspectHitSessions` — L107
- `UHodgeCombatValidationLibrary::InspectPoseLeases` — L112
- `UHodgeCombatValidationLibrary::ConfigureValidationPIE` — L118
- `UHodgeCombatValidationLibrary::ConfigureValidationSections` — L128
- `UHodgeCombatValidationLibrary::NormalizeCombatNotifies` — L146

## HodgeCombatValidationLibrary.h

[Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.h](../../../Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.h)

模块定义或基础代码；请查看对应文件。

## HodgeUIAuthoringLibrary.cpp

[Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.cpp)

仅 Editor 的 Main UI 资产制作、原生 PIE 命令调度、Slate 截图与只读检查入口。

- `UHodgeUIAuthoringLibrary::CreateFoundationAssets` — L125
- `UHodgeUIAuthoringLibrary::InspectPlayerUI` — L190
- `UHodgeUIAuthoringLibrary::QueueUIAction` — L263
- `UHodgeUIAuthoringLibrary::MigrateDesignerAssets` — L822
- `UHodgeUIAuthoringLibrary::InspectDesignerAssets` — L916
- `UHodgeUIAuthoringLibrary::ConfigureDesignerPreviews` — L931

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

## HodgeAbilityTask_WaitHitResults.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeHitResultsTickFunction::ExecuteTick` — L10
- `FHodgeHitResultsTickFunction::DiagnosticMessage` — L16
- `UHodgeAbilityTask_WaitHitResults::WaitHitResults` — L21
- `UHodgeAbilityTask_WaitHitResults::IsRunningForExecution` — L33
- `UHodgeAbilityTask_WaitHitResults::Activate` — L44
- `UHodgeAbilityTask_WaitHitResults::CreateWindow` — L63
- `UHodgeAbilityTask_WaitHitResults::IsBatchCurrent` — L81
- `UHodgeAbilityTask_WaitHitResults::SampleWindow` — L88
- `UHodgeAbilityTask_WaitHitResults::CloseWindow` — L110
- `UHodgeAbilityTask_WaitHitResults::TickDetection` — L121
- `UHodgeAbilityTask_WaitHitResults::UpdateTickState` — L149
- `UHodgeAbilityTask_WaitHitResults::Pause` — L156
- `UHodgeAbilityTask_WaitHitResults::Resume` — L162
- `UHodgeAbilityTask_WaitHitResults::ReleaseSessions` — L177
- `UHodgeAbilityTask_WaitHitResults::OnDestroy` — L189
- `UHodgeAbilityTask_WaitHitResults::BeginDestroy` — L195

## HodgeAbilityTask_WaitMoveCancel.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.cpp)

合流取消窗口与本地移动意图，广播 OnMoveCancel 后结束；已有取消验证，当前 Combat 主链与旧 BasicAttack 消费方需区分。

- `UHodgeAbilityTask_WaitMoveCancel::UHodgeAbilityTask_WaitMoveCancel` — L14
- `UHodgeAbilityTask_WaitMoveCancel::TickTask` — L20
- `UHodgeAbilityTask_WaitMoveCancel::WaitMoveCancel` — L26
- `UHodgeAbilityTask_WaitMoveCancel::Activate` — L35
- `UHodgeAbilityTask_WaitMoveCancel::OnDestroy` — L70
- `UHodgeAbilityTask_WaitMoveCancel::Evaluate` — L93
- `UHodgeAbilityTask_WaitMoveCancel::HandleMoveIntentChanged` — L136
- `UHodgeAbilityTask_WaitMoveCancel::HandleWindowTagChanged` — L142

## HodgeGameplayAbility.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp)

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。（PreloadPrimaryAssetsOnGrant 已随未提交改动回退，当前不存在。）

- `UHodgeGameplayAbility::UHodgeGameplayAbility` — L39
- `UHodgeGameplayAbility::GetHodgeAbilitySystemComponentFromActorInfo` — L68
- `UHodgeGameplayAbility::GetHodgePlayerControllerFromActorInfo` — L77
- `UHodgeGameplayAbility::GetControllerFromActorInfo` — L84
- `UHodgeGameplayAbility::GetHodgeCharacterFromActorInfo` — L122
- `UHodgeGameplayAbility::GetHeroComponentFromActorInfo` — L128
- `UHodgeGameplayAbility::NativeOnAbilityFailedToActivate` — L135
- `UHodgeGameplayAbility::CanActivateAbility` — L188
- `UHodgeGameplayAbility::SetCanBeCanceled` — L244
- `UHodgeGameplayAbility::OnGiveAbility` — L263
- `UHodgeGameplayAbility::OnRemoveAbility` — L276
- `UHodgeGameplayAbility::ActivateAbility` — L287
- `UHodgeGameplayAbility::EndAbility` — L297
- `UHodgeGameplayAbility::CheckCost` — L310
- `UHodgeGameplayAbility::ApplyCost` — L340
- `UHodgeGameplayAbility::MakeEffectContext` — L423
- `UHodgeGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec` — L470
- `UHodgeGameplayAbility::DoesAbilitySatisfyTagRequirements` — L489
- `UHodgeGameplayAbility::OnPawnAvatarSet` — L645
- `UHodgeGameplayAbility::GetAbilitySource` — L652
- `UHodgeGameplayAbility::TryActivateAbilityOnSpawn` — L678
- `UHodgeGameplayAbility::CanChangeActivationGroup` — L720
- `UHodgeGameplayAbility::ChangeActivationGroup` — L758
- `UHodgeGameplayAbility::SetCameraMode` — L791
- `UHodgeGameplayAbility::ClearCameraMode` — L805

## HodgeGameplayAbility_Death.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Death.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Death.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeGameplayAbility_Death::UHodgeGameplayAbility_Death` — L13
- `UHodgeGameplayAbility_Death::ActivateAbility` — L31
- `UHodgeGameplayAbility_Death::EndAbility` — L65
- `UHodgeGameplayAbility_Death::StartDeath` — L79
- `UHodgeGameplayAbility_Death::FinishDeath` — L91

## HodgeGameplayAbility_Definition.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp)

执行单段 Definition、本次 Montage 时钟 Timeline 与手持窗口，按执行清理；Melee 子类扩展命中。

- `UHodgeGameplayAbility_Definition::UHodgeGameplayAbility_Definition` — L15
- `UHodgeGameplayAbility_Definition::GetDefinition` — L25
- `UHodgeGameplayAbility_Definition::IsComboCoordinated` — L31
- `UHodgeGameplayAbility_Definition::ActivateConfirmedDefinition` — L37
- `UHodgeGameplayAbility_Definition::CanActivateAbility` — L52
- `UHodgeGameplayAbility_Definition::ActivateAbility` — L69
- `UHodgeGameplayAbility_Definition::FinishExecution` — L123
- `UHodgeGameplayAbility_Definition::EndAbility` — L131
- `UHodgeGameplayAbility_Definition::GetExecutionWindows` — L183
- `UHodgeGameplayAbility_Definition::AcceptsNotify` — L190
- `UHodgeGameplayAbility_Definition::AllocateNotifyOccurrence` — L198
- `UHodgeGameplayAbility_Definition::BeginNotifyResource` — L203
- `UHodgeGameplayAbility_Definition::EndNotifyResource` — L212
- `UHodgeGameplayAbility_Definition::AcquireNotifyTag` — L230
- `UHodgeGameplayAbility_Definition::AcquireNotifyWeapon` — L245
- `UHodgeGameplayAbility_Definition::NotifyWindowsChanged` — L260
- `UHodgeGameplayAbility_Definition::SendExecutionEvent` — L265
- `UHodgeGameplayAbility_Definition::OnMontageBlendingOut` — L277
- `UHodgeGameplayAbility_Definition::OnMontageEnded` — L285
- `UHodgeGameplayAbility_Definition::ValidateExecutionConfiguration` — L290

## HodgeGameplayAbility_Jump.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeGameplayAbility_Jump::UHodgeGameplayAbility_Jump` — L13
- `UHodgeGameplayAbility_Jump::CanActivateAbility` — L20
- `UHodgeGameplayAbility_Jump::EndAbility` — L45
- `UHodgeGameplayAbility_Jump::CharacterJumpStart` — L56
- `UHodgeGameplayAbility_Jump::CharacterJumpStop` — L68

## HodgeGameplayAbility_Melee.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp)

接收服务器 Window/Point 命中结果，准备锚点/目标、逐段/共享去重，构建独立 GE Spec/Context 并施加。

- `FHodgeMeleeHitHistory::CanHit` — L20
- `FHodgeMeleeHitHistory::RecordHit` — L27
- `UHodgeGameplayAbility_Melee::ValidateExecutionConfiguration` — L32
- `UHodgeGameplayAbility_Melee::PrepareHitExecutionContext_Implementation` — L76
- `UHodgeGameplayAbility_Melee::SetHitAnchor` — L78
- `UHodgeGameplayAbility_Melee::SetHitTarget` — L91
- `UHodgeGameplayAbility_Melee::ResetHitGeometryHistory` — L100
- `UHodgeGameplayAbility_Melee::OnExecutionReady` — L109
- `UHodgeGameplayAbility_Melee::OnExecutionEnding` — L133
- `UHodgeGameplayAbility_Melee::BeginNotifyHit` — L150
- `UHodgeGameplayAbility_Melee::OpenHit` — L215
- `UHodgeGameplayAbility_Melee::OnNotifyResourceEnded` — L238
- `UHodgeGameplayAbility_Melee::IsMeleeBatchCurrent` — L243
- `UHodgeGameplayAbility_Melee::OnHitResults` — L250
- `UHodgeGameplayAbility_Melee::CanApplyMeleeHit_Implementation` — L260
- `UHodgeGameplayAbility_Melee::MakeMeleeHitContext` — L266
- `UHodgeGameplayAbility_Melee::BuildMeleeHitSpec` — L287
- `UHodgeGameplayAbility_Melee::ProcessMeleeHitResults_Implementation` — L306
- `UHodgeGameplayAbility_Melee::ApplyMeleeHitEffects_Implementation` — L340

## HodgeAttributeSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp)

项目 AttributeSet 基础和 ASC 访问。

- `UHodgeAttributeSet::UHodgeAttributeSet` — L23
- `UHodgeAttributeSet::GetWorld` — L28
- `UHodgeAttributeSet::GetHodgeAbilitySystemComponent` — L41

## HodgeCombatSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeCombatSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeCombatSet.cpp)

BaseDamage/BaseHeal 战斗属性，OwnerOnly 复制，供伤害/治疗 Execution 捕获。

- `UHodgeCombatSet::UHodgeCombatSet` — L11
- `UHodgeCombatSet::GetLifetimeReplicatedProps` — L20
- `UHodgeCombatSet::OnRep_BaseDamage` — L33
- `UHodgeCombatSet::OnRep_BaseHeal` — L40

## HodgeHealthSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp)

Health/MaxHealth、Damage/Healing 元属性、结算夹取/免疫/耗尽广播；BaseDamage/BaseHeal 位于 CombatSet。

- `UHodgeHealthSet::UHodgeHealthSet` — L33
- `UHodgeHealthSet::GetLifetimeReplicatedProps` — L50
- `UHodgeHealthSet::OnRep_Health` — L63
- `UHodgeHealthSet::OnRep_MaxHealth` — L99
- `UHodgeHealthSet::PreGameplayEffectExecute` — L116
- `UHodgeHealthSet::PostGameplayEffectExecute` — L177
- `UHodgeHealthSet::PreAttributeBaseChange` — L308
- `UHodgeHealthSet::PreAttributeChange` — L318
- `UHodgeHealthSet::PostAttributeChange` — L328
- `UHodgeHealthSet::BeginAttributeRebuild` — L361
- `UHodgeHealthSet::EndAttributeRebuild` — L371
- `UHodgeHealthSet::GetResourceClampMax` — L379
- `UHodgeHealthSet::SetHealthForAttributeCommit` — L384
- `UHodgeHealthSet::ClampAttribute` — L398

## HodgeDamageExecution.cpp

[Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeDamageExecution.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeDamageExecution.cpp)

BaseDamage × SetByCaller 倍率 × 距离/材质衰减 × 共享目标规则；不再恒零。

- `UHodgeDamageExecution::UHodgeDamageExecution` — L50
- `UHodgeDamageExecution::Execute_Implementation` — L57

## HodgeHealExecution.cpp

[Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeHealExecution.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeHealExecution.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeHealExecution::UHodgeHealExecution` — L44
- `UHodgeHealExecution::Execute_Implementation` — L51

## GameplayTagStack.cpp

[Source/Hodgepodge/Private/AbilitySystem/GameplayTagStack.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/GameplayTagStack.cpp)

带计数的标签栈及复制数据结构，区别于只判断有无的 TagContainer。

- `FGameplayTagStack::GetDebugString` — L13
- `FGameplayTagStackContainer::AddStack` — L22
- `FGameplayTagStackContainer::RemoveStack` — L69
- `FGameplayTagStackContainer::PreReplicatedRemove` — L124
- `FGameplayTagStackContainer::PostReplicatedAdd` — L138
- `FGameplayTagStackContainer::PostReplicatedChange` — L152

## HodgeAbilitySystemComponent.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp)

Tag 输入、激活组、关系映射、Montage 复制和全局注册；失败时恢复武器预测与连段记忆。

- `UHodgeAbilitySystemComponent::UHodgeAbilitySystemComponent` — L37
- `UHodgeAbilitySystemComponent::EndPlay` — L53
- `UHodgeAbilitySystemComponent::InitAbilityActorInfo` — L67
- `UHodgeAbilitySystemComponent::TryActivateAbilitiesOnSpawn` — L145
- `UHodgeAbilitySystemComponent::CancelAbilitiesByFunc` — L162
- `UHodgeAbilitySystemComponent::CancelInputActivatedAbilities` — L230
- `UHodgeAbilitySystemComponent::AbilitySpecInputPressed` — L247
- `UHodgeAbilitySystemComponent::AbilitySpecInputReleased` — L274
- `UHodgeAbilitySystemComponent::AbilityInputTagPressed` — L301
- `UHodgeAbilitySystemComponent::AbilityInputTagReleased` — L326
- `UHodgeAbilitySystemComponent::ProcessAbilityInput` — L347
- `UHodgeAbilitySystemComponent::ClearAbilityInput` — L477
- `UHodgeAbilitySystemComponent::NotifyAbilityActivated` — L490
- `UHodgeAbilitySystemComponent::NotifyAbilityFailed` — L512
- `UHodgeAbilitySystemComponent::NotifyAbilityEnded` — L534
- `UHodgeAbilitySystemComponent::ApplyAbilityBlockAndCancelTags` — L549
- `UHodgeAbilitySystemComponent::HandleChangeAbilityCanBeCanceled` — L578
- `UHodgeAbilitySystemComponent::GetAdditionalActivationTagRequirements` — L589
- `UHodgeAbilitySystemComponent::SetTagRelationshipMapping` — L602
- `UHodgeAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation` — L608
- `UHodgeAbilitySystemComponent::HandleAbilityFailed` — L615
- `UHodgeAbilitySystemComponent::IsActivationGroupBlocked` — L628
- `UHodgeAbilitySystemComponent::AddAbilityToActivationGroup` — L659
- `UHodgeAbilitySystemComponent::RemoveAbilityFromActivationGroup` — L707
- `UHodgeAbilitySystemComponent::CancelActivationGroupAbilities` — L720
- `UHodgeAbilitySystemComponent::AddDynamicTagGameplayEffect` — L736
- `UHodgeAbilitySystemComponent::RemoveDynamicTagGameplayEffect` — L772
- `UHodgeAbilitySystemComponent::GetAbilityTargetData` — L797
- `UHodgeAbilitySystemComponent::GetLifetimeReplicatedProps` — L812
- `UHodgeAbilitySystemComponent::GiveAbilityDefinition` — L819
- `UHodgeAbilitySystemComponent::FindAbilityDefinition` — L851
- `UHodgeAbilitySystemComponent::FindDefinitionAbility` — L861
- `UHodgeAbilitySystemComponent::OnRemoveAbility` — L876
- `UHodgeAbilitySystemComponent::ProcessDeferredComboRequest` — L882
- `UHodgeAbilitySystemComponent::InternalServerTryActivateAbility` — L913
- `UHodgeAbilitySystemComponent::ClientConfirmComboTiming_Implementation` — L965
- `UHodgeAbilitySystemComponent::ClientActivateAbilitySucceedWithEventData_Implementation` — L985
- `UHodgeAbilitySystemComponent::ClientActivateAbilityFailed_Implementation` — L1006
- `UHodgeAbilitySystemComponent::PlayMontage` — L1024
- `UHodgeAbilitySystemComponent::PlayMontageSimulated` — L1039
- `UHodgeAbilitySystemComponent::ApplyDefinitionMontageSettings` — L1046
- `UHodgeAbilitySystemComponent::OnRep_DefinitionMontage` — L1061
- `UHodgeAbilitySystemComponent::OnRep_ReplicatedAnimMontage` — L1070
- `UHodgeAbilitySystemComponent::StopDefinitionMontage` — L1076
- `UHodgeAbilitySystemComponent::CurrentMontageStop` — L1083
- `UHodgeAbilitySystemComponent::ReleaseAbilityInput` — L1104

## HodgeAbilitySystemGlobals.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemGlobals.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemGlobals.cpp)

分配 FHodgeGameplayEffectContext 的 GAS 全局类；已加入项目配置。

- `UHodgeAbilitySystemGlobals::UHodgeAbilitySystemGlobals` — L15
- `UHodgeAbilitySystemGlobals::AllocGameplayEffectContext` — L22

## HodgeAbilityTagRelationshipMapping.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilityTagRelationshipMapping.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilityTagRelationshipMapping.cpp)

数据驱动的能力阻断、取消与激活条件关系。

- `UHodgeAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel` — L9
- `UHodgeAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags` — L39
- `UHodgeAbilityTagRelationshipMapping::IsAbilityCancelledByTag` — L69

## HodgeGameplayCueManager.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayCueManager.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayCueManager.cpp)

项目 Cue 管理类已配置；启动预加载及 Feature Cue 观察者生命周期仍未完整接通。

- `UHodgeGameplayCueManager::UHodgeGameplayCueManager` — L68
- `UHodgeGameplayCueManager::Get` — L74
- `UHodgeGameplayCueManager::OnCreated` — L80
- `UHodgeGameplayCueManager::LoadAlwaysLoadedCues` — L90
- `UHodgeGameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries` — L124
- `UHodgeGameplayCueManager::ShouldSyncLoadMissingGameplayCues` — L153
- `UHodgeGameplayCueManager::ShouldAsyncLoadMissingGameplayCues` — L160
- `UHodgeGameplayCueManager::DumpGameplayCues` — L167
- `UHodgeGameplayCueManager::OnGameplayTagLoaded` — L262
- `UHodgeGameplayCueManager::HandlePostGarbageCollect` — L309
- `UHodgeGameplayCueManager::ProcessLoadedTags` — L322
- `UHodgeGameplayCueManager::ProcessTagToPreload` — L373
- `UHodgeGameplayCueManager::OnPreloadCueComplete` — L438
- `UHodgeGameplayCueManager::RegisterPreloadedCue` — L454
- `UHodgeGameplayCueManager::HandlePostLoadMap` — L490
- `UHodgeGameplayCueManager::UpdateDelayLoadDelegateListeners` — L537
- `UHodgeGameplayCueManager::ShouldDelayLoadGameplayCues` — L581
- `UHodgeGameplayCueManager::RefreshGameplayCuePrimaryAsset` — L600

## HodgeGameplayEffectContext.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayEffectContext.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayEffectContext.cpp)

项目 GE 上下文与序列化扩展，已有 HodgeAbilitySystemGlobals 分配配套。

- `FHodgeGameplayEffectContext::ExtractEffectContext` — L18
- `FHodgeGameplayEffectContext::NetSerialize` — L37
- `FHodgeGameplayEffectContext::SetAbilitySource` — L61
- `FHodgeGameplayEffectContext::GetAbilitySource` — L71
- `FHodgeGameplayEffectContext::GetPhysicalMaterial` — L78

## HodgeGameplayTags.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayTags.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayTags.cpp)

原生 GameplayTag 注册和移动状态标签映射；含攻击时间轴依赖的 Status.Attack.*（阶段 + 取消窗口 .Cancel.*）与 GameplayEvent.Attack.*。标签存在不等于对应玩法实现。

## HodgeGlobalAbilitySystem.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeGlobalAbilitySystem.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGlobalAbilitySystem.cpp)

世界级全局能力/效果授予及 ASC 注册表。

- `FGlobalAppliedAbilityList::AddToASC` — L10
- `FGlobalAppliedAbilityList::RemoveFromASC` — L32
- `FGlobalAppliedAbilityList::RemoveFromAll` — L46
- `FGlobalAppliedEffectList::AddToASC` — L65
- `FGlobalAppliedEffectList::RemoveFromASC` — L85
- `FGlobalAppliedEffectList::RemoveFromAll` — L99
- `UHodgeGlobalAbilitySystem::UHodgeGlobalAbilitySystem` — L116
- `UHodgeGlobalAbilitySystem::ApplyAbilityToAll` — L121
- `UHodgeGlobalAbilitySystem::ApplyEffectToAll` — L138
- `UHodgeGlobalAbilitySystem::RemoveAbilityFromAll` — L155
- `UHodgeGlobalAbilitySystem::RemoveEffectFromAll` — L172
- `UHodgeGlobalAbilitySystem::RegisterASC` — L189
- `UHodgeGlobalAbilitySystem::UnregisterASC` — L210

## HodgeAttributeCoordinator.cpp

[Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeAttributeCoordinator.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeAttributeCoordinator.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeAttributeCoordinator::GetPlayerState` — L18
- `UHodgeAttributeCoordinator::IsBoundTo` — L19
- `UHodgeAttributeCoordinator::IsCurrentAvatar` — L20
- `UHodgeAttributeCoordinator::IsReadyFor` — L21
- `UHodgeAttributeCoordinator::ValidateCoreSets` — L27
- `UHodgeAttributeCoordinator::PublishReady` — L39
- `UHodgeAttributeCoordinator::BeginUpdate` — L54
- `UHodgeAttributeCoordinator::ApplyCharacterBase` — L69
- `UHodgeAttributeCoordinator::PrepareAvatar` — L82
- `UHodgeAttributeCoordinator::CompleteAvatarInitialization` — L113
- `UHodgeAttributeCoordinator::CommitUpdate` — L126
- `UHodgeAttributeCoordinator::DetachAvatar` — L168
- `UHodgeAttributeCoordinator::SetCharacterLevel` — L181
- `UHodgeAttributeCoordinator::RestoreHealth` — L197
- `UHodgeAttributeCoordinator::BeginEquipmentUpdate` — L206
- `UHodgeAttributeCoordinator::FinishEquipmentUpdate` — L207
- `UHodgeAttributeCoordinator::ExpectRespawn` — L208
- `UHodgeAttributeCoordinator::SetInitialSavedHealth` — L210
- `UHodgeAttributeCoordinator::GetOrCreateDefaultEquipment` — L218
- `UHodgeAttributeCoordinator::RememberEquipment` — L233

## HodgeCharacterBaseStatEffect.cpp

[Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeCharacterBaseStatEffect.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeCharacterBaseStatEffect.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeCharacterBaseStatEffect::UHodgeCharacterBaseStatEffect` — L7

## HodgeEquipmentStatEffect.cpp

[Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeEquipmentStatEffect.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Stats/HodgeEquipmentStatEffect.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeEquipmentStatEffect::UHodgeEquipmentStatEffect` — L8

## HodgeActorBase.cpp

[Source/Hodgepodge/Private/Actor/HodgeActorBase.cpp](../../../Source/Hodgepodge/Private/Actor/HodgeActorBase.cpp)

项目 Actor 基类扩展。

- `AHodgeActorBase::AHodgeActorBase` — L21
- `AHodgeActorBase::BeginPlay` — L34
- `AHodgeActorBase::Tick` — L45

## HodgeAnimInstance.cpp

[Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp)

ASC 属性映射、GroundDistance 与旋转策略快照；FullBody 权重协调蓝图程序性根 Yaw。

- `UHodgeAnimInstance::UHodgeAnimInstance` — L15
- `UHodgeAnimInstance::InitializeWithAbilitySystem` — L20
- `UHodgeAnimInstance::IsDataValid` — L31
- `UHodgeAnimInstance::NativeInitializeAnimation` — L45
- `UHodgeAnimInstance::NativeUpdateAnimation` — L63

## HodgeCombatAnimNotifies.cpp

[Source/Hodgepodge/Private/Animation/HodgeCombatAnimNotifies.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeCombatAnimNotifies.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeAnimHitConfig::FHodgeAnimHitConfig` — L29
- `FHodgeAnimHitConfig::Validate` — L34
- `UHodgeAnimNotifyState_HitCheck::UHodgeAnimNotifyState_HitCheck` — L72
- `UHodgeAnimNotify_Hit::UHodgeAnimNotify_Hit` — L74
- `UHodgeAnimNotifyState_GameplayTag::UHodgeAnimNotifyState_GameplayTag` — L80
- `UHodgeAnimNotifyState_WeaponHand::UHodgeAnimNotifyState_WeaponHand` — L81
- `UHodgeAnimNotify_GameplayEvent::UHodgeAnimNotify_GameplayEvent` — L82
- `UHodgeAnimNotifyState_HitCheck::BranchingPointNotifyBegin` — L84
- `UHodgeAnimNotifyState_HitCheck::BranchingPointNotifyEnd` — L93
- `UHodgeAnimNotify_Hit::BranchingPointNotify` — L98
- `UHodgeAnimNotifyState_GameplayTag::BranchingPointNotifyBegin` — L115
- `UHodgeAnimNotifyState_GameplayTag::BranchingPointNotifyEnd` — L124
- `UHodgeAnimNotifyState_WeaponHand::BranchingPointNotifyBegin` — L129
- `UHodgeAnimNotifyState_WeaponHand::BranchingPointNotifyEnd` — L138
- `UHodgeAnimNotify_GameplayEvent::BranchingPointNotify` — L143
- `UHodgeAnimNotifyState_HitCheck::NotifyBegin` — L160
- `UHodgeAnimNotifyState_HitCheck::NotifyEnd` — L167
- `UHodgeAnimNotifyState_GameplayTag::NotifyBegin` — L174
- `UHodgeAnimNotifyState_GameplayTag::NotifyEnd` — L181
- `UHodgeAnimNotifyState_WeaponHand::NotifyBegin` — L188
- `UHodgeAnimNotifyState_WeaponHand::NotifyEnd` — L195
- `UHodgeAnimNotify_Hit::Notify` — L202
- `UHodgeAnimNotify_GameplayEvent::Notify` — L209
- `UHodgeAnimNotifyState_HitCheck::OnAnimNotifyCreatedInEditor` — L232
- `UHodgeAnimNotify_Hit::OnAnimNotifyCreatedInEditor` — L237
- `UHodgeAnimNotifyState_GameplayTag::OnAnimNotifyCreatedInEditor` — L239
- `UHodgeAnimNotifyState_WeaponHand::OnAnimNotifyCreatedInEditor` — L244
- `UHodgeAnimNotify_GameplayEvent::OnAnimNotifyCreatedInEditor` — L249

## HodgeCameraComponent.cpp

[Source/Hodgepodge/Private/Camera/HodgeCameraComponent.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraComponent.cpp)

相机模式栈宿主与最终视图输出，依赖默认模式委托。

- `UHodgeCameraComponent::UHodgeCameraComponent` — L12
- `UHodgeCameraComponent::OnRegister` — L22
- `UHodgeCameraComponent::GetCameraView` — L36
- `UHodgeCameraComponent::UpdateCameraModes` — L111
- `UHodgeCameraComponent::DrawDebug` — L132
- `UHodgeCameraComponent::GetBlendInfo` — L168

## HodgeCameraMode.cpp

[Source/Hodgepodge/Private/Camera/HodgeCameraMode.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraMode.cpp)

相机视图、模式实例、混合和模式栈。

- `FHodgeCameraModeView::FHodgeCameraModeView` — L20
- `FHodgeCameraModeView::Blend` — L29
- `UHodgeCameraMode::UHodgeCameraMode` — L68
- `UHodgeCameraMode::GetHodgeCameraComponent` — L96
- `UHodgeCameraMode::GetWorld` — L103
- `UHodgeCameraMode::GetTargetActor` — L110
- `UHodgeCameraMode::GetPivotLocation` — L120
- `UHodgeCameraMode::GetPivotRotation` — L166
- `UHodgeCameraMode::UpdateCameraMode` — L184
- `UHodgeCameraMode::UpdateView` — L194
- `UHodgeCameraMode::SetBlendWeight` — L219
- `UHodgeCameraMode::UpdateBlending` — L258
- `UHodgeCameraMode::DrawDebug` — L309
- `UHodgeCameraModeStack::UHodgeCameraModeStack` — L330
- `UHodgeCameraModeStack::ActivateStack` — L337
- `UHodgeCameraModeStack::DeactivateStack` — L358
- `UHodgeCameraModeStack::PushCameraMode` — L379
- `UHodgeCameraModeStack::EvaluateStack` — L467
- `UHodgeCameraModeStack::GetCameraModeInstance` — L486
- `UHodgeCameraModeStack::UpdateStack` — L513
- `UHodgeCameraModeStack::BlendStack` — L572
- `UHodgeCameraModeStack::DrawDebug` — L603
- `UHodgeCameraModeStack::GetBlendInfo` — L631

## HodgeCameraMode_ThirdPerson.cpp

[Source/Hodgepodge/Private/Camera/HodgeCameraMode_ThirdPerson.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraMode_ThirdPerson.cpp)

第三人称偏移与防穿透逻辑；C++ 不设默认偏移曲线，改由蓝图/PawnData 提供。

- `UHodgeCameraMode_ThirdPerson::UHodgeCameraMode_ThirdPerson` — L20
- `UHodgeCameraMode_ThirdPerson::UpdateView` — L54
- `UHodgeCameraMode_ThirdPerson::UpdateForTarget` — L109
- `UHodgeCameraMode_ThirdPerson::DrawDebug` — L135
- `UHodgeCameraMode_ThirdPerson::UpdatePreventPenetration` — L158
- `UHodgeCameraMode_ThirdPerson::PreventCameraPenetration` — L249
- `UHodgeCameraMode_ThirdPerson::SetTargetCrouchOffset` — L511
- `UHodgeCameraMode_ThirdPerson::UpdateCrouchOffset` — L523

## HodgePlayerCameraManager.cpp

[Source/Hodgepodge/Private/Camera/HodgePlayerCameraManager.cpp](../../../Source/Hodgepodge/Private/Camera/HodgePlayerCameraManager.cpp)

由 HodgePlayerController 构造选用的项目相机管理器；运行效果待验收。

- `AHodgePlayerCameraManager::AHodgePlayerCameraManager` — L17
- `AHodgePlayerCameraManager::GetUICameraComponent` — L33
- `AHodgePlayerCameraManager::UpdateViewTarget` — L39
- `AHodgePlayerCameraManager::DisplayDebug` — L58

## HodgeUICameraManagerComponent.cpp

[Source/Hodgepodge/Private/Camera/HodgeUICameraManagerComponent.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeUICameraManagerComponent.cpp)

UI 相机管理扩展，不代表 UI 系统已接入。

- `UHodgeUICameraManagerComponent::GetComponent` — L15
- `UHodgeUICameraManagerComponent::UHodgeUICameraManagerComponent` — L31
- `UHodgeUICameraManagerComponent::InitializeComponent` — L48
- `UHodgeUICameraManagerComponent::SetViewTarget` — L55
- `UHodgeUICameraManagerComponent::NeedsToUpdateViewTarget` — L68
- `UHodgeUICameraManagerComponent::UpdateViewTarget` — L75
- `UHodgeUICameraManagerComponent::OnShowDebugInfo` — L81

## HodgeCharacterBase.cpp

[Source/Hodgepodge/Private/Character/HodgeCharacterBase.cpp](../../../Source/Hodgepodge/Private/Character/HodgeCharacterBase.cpp)

原生 Character 基础、替换移动组件；Receiver 在 PreInit 注册、EndPlay 成对移除。

- `AHodgeCharacterBase::AHodgeCharacterBase` — L31
- `AHodgeCharacterBase::PreInitializeComponents` — L43
- `AHodgeCharacterBase::EndPlay` — L62
- `AHodgeCharacterBase::BeginPlay` — L75

## HodgeCombatCharacter.cpp

[Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp)

PawnExtension、相机、HealthComponent、原生 RotationComponent；ASC、移动/旋转约束、复制与死亡清理。

- `AHodgeCombatCharacter::AHodgeCombatCharacter` — L46
- `AHodgeCombatCharacter::PreInitializeComponents` — L164
- `AHodgeCombatCharacter::BeginPlay` — L171
- `AHodgeCombatCharacter::EndPlay` — L194
- `AHodgeCombatCharacter::Reset` — L229
- `AHodgeCombatCharacter::GetLifetimeReplicatedProps` — L242
- `AHodgeCombatCharacter::PreReplication` — L254
- `AHodgeCombatCharacter::NotifyControllerChanged` — L281
- `AHodgeCombatCharacter::GetHodgePlayerController` — L301
- `AHodgeCombatCharacter::GetHodgePlayerState` — L308
- `AHodgeCombatCharacter::GetHodgeAbilitySystemComponent` — L315
- `AHodgeCombatCharacter::GetAbilitySystemComponent` — L322
- `AHodgeCombatCharacter::OnAbilitySystemInitialized` — L334
- `AHodgeCombatCharacter::OnAbilitySystemUninitialized` — L351
- `AHodgeCombatCharacter::FaceRotation` — L365
- `AHodgeCombatCharacter::InitializeDefaultEquipment` — L376
- `AHodgeCombatCharacter::UninitializeDefaultEquipment` — L440
- `AHodgeCombatCharacter::PossessedBy` — L462
- `AHodgeCombatCharacter::UnPossessed` — L488
- `AHodgeCombatCharacter::OnRep_Controller` — L514
- `AHodgeCombatCharacter::OnRep_PlayerState` — L523
- `AHodgeCombatCharacter::SetupPlayerInputComponent` — L532
- `AHodgeCombatCharacter::InitializeGameplayTags` — L543
- `AHodgeCombatCharacter::GetOwnedGameplayTags` — L578
- `AHodgeCombatCharacter::HasMatchingGameplayTag` — L588
- `AHodgeCombatCharacter::HasAllMatchingGameplayTags` — L600
- `AHodgeCombatCharacter::HasAnyMatchingGameplayTags` — L612
- `AHodgeCombatCharacter::FellOutOfWorld` — L624
- `AHodgeCombatCharacter::OnDeathStarted` — L631
- `AHodgeCombatCharacter::OnDeathFinished` — L640
- `AHodgeCombatCharacter::DisableMovementAndCollision` — L647
- `AHodgeCombatCharacter::DestroyDueToDeath` — L677
- `AHodgeCombatCharacter::UninitAndDestroy` — L687
- `AHodgeCombatCharacter::OnMovementModeChanged` — L715
- `AHodgeCombatCharacter::SetMovementModeTag` — L731
- `AHodgeCombatCharacter::ToggleCrouch` — L760
- `AHodgeCombatCharacter::OnStartCrouch` — L779
- `AHodgeCombatCharacter::OnEndCrouch` — L793
- `AHodgeCombatCharacter::CanJumpInternal_Implementation` — L807
- `AHodgeCombatCharacter::OnRep_ReplicatedAcceleration` — L814
- `AHodgeCombatCharacter::OnControllerChangedTeam` — L844
- `AHodgeCombatCharacter::OnRep_MyTeamID` — L857
- `AHodgeCombatCharacter::UpdateSharedReplication` — L864
- `AHodgeCombatCharacter::FastSharedReplication_Implementation` — L897
- `FSharedRepMovement::FSharedRepMovement` — L939
- `FSharedRepMovement::FillForCharacter` — L946
- `FSharedRepMovement::Equals` — L993
- `FSharedRepMovement::NetSerialize` — L1036
- `AHodgeCombatCharacter::TryInitializeAttributesAndEquipment` — L1073

## HodgeEnemyCharacter.cpp

[Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp)

仅设置 AI 自动控制；旋转/移动参数继承 Combat 基类，尚未完成敌人 ASC 初始化。

- `AHodgeEnemyCharacter::AHodgeEnemyCharacter` — L10
- `AHodgeEnemyCharacter::BeginPlay` — L41
- `AHodgeEnemyCharacter::PossessedBy` — L46
- `AHodgeEnemyCharacter::PostEditChangeProperty` — L54

## HodgeHeroCharacter.cpp

[Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp)

构造挂载 HeroComponent；PossessedBy/OnRep_PlayerState 只调用 Super，ASC 接入已收敛。

- `AHodgeHeroCharacter::AHodgeHeroCharacter` — L23
- `AHodgeHeroCharacter::PossessedBy` — L39
- `AHodgeHeroCharacter::OnRep_PlayerState` — L55

## HodgeALSLocomotion.cpp

[Source/Hodgepodge/Private/CodexText/HodgeALSLocomotion.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeALSLocomotion.cpp)

CodexText 实验：6 向地面运动动画实例，不属于主 Hero 动画链。

- `UHodgeALSLocomotion::GetLabActiveState` — L28
- `UHodgeALSLocomotion::NativeUpdateAnimation` — L34
- `UHodgeALSAuthoring::BuildLocomotionGraph` — L107

## HodgeGroundedAuthoring.cpp

[Source/Hodgepodge/Private/CodexText/HodgeGroundedAuthoring.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeGroundedAuthoring.cpp)

CodexText 实验：编辑器辅助库，含 Grounded 图层与停步/转身精修、步幅、地形、战斗图层等编辑期构建函数。

- `UHodgeGroundedAuthoring::RepairGroundedLegIK` — L51
- `UHodgeGroundedAuthoring::RefineGroundedTransitions` — L101
- `UHodgeGroundedAuthoring::AddStrideLayer` — L164
- `UHodgeGroundedAuthoring::AddTerrainLayer` — L223
- `UHodgeGroundedAuthoring::AddCombatLayer` — L285
- `UHodgeGroundedAuthoring::AddGroundedLayer` — L341

## HodgeGroundedLocomotion.cpp

[Source/Hodgepodge/Private/CodexText/HodgeGroundedLocomotion.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeGroundedLocomotion.cpp)

CodexText 实验：在 ALS 基础动画上增加平地起停/转身/脚锁，并扩展上半身 Overlay、左手握持、步幅与地形 IK。

- `UHodgeGroundedLocomotion::NativeInitializeAnimation` — L10
- `UHodgeGroundedLocomotion::GetGroundedState` — L29
- `UHodgeGroundedLocomotion::SetLeftHandGrip` — L35
- `UHodgeGroundedLocomotion::NativeUninitializeAnimation` — L40
- `UHodgeGroundedLocomotion::CacheFinalizedFootPose` — L49
- `UHodgeGroundedLocomotion::NativeUpdateAnimation` — L61

## HodgeLocomotionLab.cpp

[Source/Hodgepodge/Private/CodexText/HodgeLocomotionLab.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeLocomotionLab.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeLocomotionLabComponent::UHodgeLocomotionLabComponent` — L27
- `UHodgeLocomotionLabComponent::BeginPlay` — L33
- `UHodgeLocomotionLabComponent::SetCombatFacing` — L47
- `UHodgeLocomotionLabComponent::SetWalking` — L60
- `UHodgeLocomotionLabComponent::TickComponent` — L69
- `AHodgeLocomotionLabMode::InitGame` — L111
- `UHodgeLocomotionLabAuthoring::RemapCopy` — L118
- `UHodgeLocomotionLabAuthoring::ConfigureGroundBlend` — L155

## HodgeSurvivor.cpp

[Source/Hodgepodge/Private/CodexText/HodgeSurvivor.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeSurvivor.cpp)

模块定义或基础代码；请查看对应文件。

- `AHodgeSurvivorHero::AHodgeSurvivorHero` — L22
- `AHodgeSurvivorHero::SetupPlayerInputComponent` — L49
- `AHodgeSurvivorHero::Forward` — L70
- `AHodgeSurvivorHero::Right` — L75
- `AHodgeSurvivorHero::Dash` — L80
- `AHodgeSurvivorHero::Tick` — L89
- `AHodgeSurvivorHero::EndPlay` — L106
- `AHodgeSurvivorMode::AHodgeSurvivorMode` — L114
- `AHodgeSurvivorMode::StartPlay` — L124
- `AHodgeSurvivorMode::MakeShape` — L137
- `AHodgeSurvivorMode::SpawnEnemy` — L151
- `AHodgeSurvivorMode::HitEnemy` — L172
- `AHodgeSurvivorMode::DamageHero` — L188
- `AHodgeSurvivorMode::GrantExperience` — L195
- `AHodgeSurvivorMode::GetUpgradeText` — L207
- `AHodgeSurvivorMode::ChooseUpgrade` — L212
- `AHodgeSurvivorMode::SetRunState` — L226
- `AHodgeSurvivorMode::TogglePause` — L246
- `AHodgeSurvivorMode::RestartRun` — L251
- `AHodgeSurvivorMode::ReturnToMenu` — L255
- `AHodgeSurvivorMode::Tick` — L259
- `AHodgeSurvivorMode::EndPlay` — L342

## HodgeSurvivorHUD.cpp

[Source/Hodgepodge/Private/CodexText/HodgeSurvivorHUD.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeSurvivorHUD.cpp)

CodexText 实验：代码构建的 Survivor HUD UserWidget。

- `UHodgeSurvivorHUD::Mode` — L27
- `UHodgeSurvivorHUD::Text` — L28
- `UHodgeSurvivorHUD::AddButton` — L32
- `UHodgeSurvivorHUD::NativeConstruct` — L46
- `UHodgeSurvivorHUD::Refresh` — L116
- `UHodgeSurvivorHUD::NativeDestruct` — L155
- `UHodgeSurvivorHUD::ChooseOne` — L163
- `UHodgeSurvivorHUD::ChooseTwo` — L164
- `UHodgeSurvivorHUD::ChooseThree` — L165
- `UHodgeSurvivorHUD::Retry` — L166
- `UHodgeSurvivorHUD::Menu` — L167
- `UHodgeSurvivorHUD::Pause` — L168

## HodgeDamageRules.cpp

[Source/Hodgepodge/Private/Combat/HodgeDamageRules.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeDamageRules.cpp)

检测和 Execution 共用目标、ASC、死亡/Health、自伤/友伤及队伍规则。

- `FHodgeDamageRules::CanDamage` — L23

## HodgeHitDetection.cpp

[Source/Hodgepodge/Private/Combat/HodgeHitDetection.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeHitDetection.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeHitVolumeConfig::Validate` — L122
- `UHodgeHitDetectionProfile::UHodgeHitDetectionProfile` — L153
- `UHodgeHitDetectionProfile::Validate` — L158
- `UHodgeHitDetectionProfile::IsDataValid` — L183
- `UHodgeSocketSweepStrategy::Capture` — L195
- `UHodgeSocketSweepStrategy::Detect` — L228
- `UHodgeBoxSweepStrategy::Capture` — L255
- `UHodgeBoxSweepStrategy::Detect` — L267
- `UHodgeShapeQueryStrategy::Capture` — L298
- `UHodgeShapeQueryStrategy::Detect` — L304

## HodgeActorComponentBase.cpp

[Source/Hodgepodge/Private/Component/HodgeActorComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeActorComponentBase.cpp)

通用 ActorComponent 基础访问与扩展。

- `UHodgeActorComponentBase::UHodgeActorComponentBase` — L21
- `UHodgeActorComponentBase::BeginPlay` — L37
- `UHodgeActorComponentBase::TickComponent` — L55
- `UHodgeActorComponentBase::GetOwnerCharacter` — L67
- `UHodgeActorComponentBase::InitializeComponent` — L78
- `UHodgeActorComponentBase::OnReady` — L89

## HodgeCharacterMovementComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeCharacterMovementComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCharacterMovementComponent.cpp)

地面距离/加速度、最终旋转过滤、SavedMove 旋转策略重放与权威校正。

- `UHodgeCharacterMovementComponent::UHodgeCharacterMovementComponent` — L94
- `UHodgeCharacterMovementComponent::SimulateMovement` — L100
- `UHodgeCharacterMovementComponent::CanAttemptJump` — L121
- `UHodgeCharacterMovementComponent::InitializeComponent` — L130
- `UHodgeCharacterMovementComponent::GetGroundInfo` — L137
- `UHodgeCharacterMovementComponent::SetReplicatedAcceleration` — L218
- `UHodgeCharacterMovementComponent::GetDeltaRotation` — L228
- `UHodgeCharacterMovementComponent::PhysicsRotation` — L253
- `UHodgeCharacterMovementComponent::MoveUpdatedComponentImpl` — L277
- `UHodgeCharacterMovementComponent::GetPredictionData_Client` — L294
- `UHodgeCharacterMovementComponent::ClientUpdatePositionAfterServerUpdate` — L304
- `UHodgeCharacterMovementComponent::SmoothCorrection` — L314
- `UHodgeCharacterMovementComponent::GetMaxSpeed` — L322

## HodgeCharacterRotationComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeCharacterRotationComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCharacterRotationComponent.cpp)

ASC 旋转标签约束、锁定 Yaw、恢复、权威复制与移动重放状态。

- `UHodgeCharacterRotationComponent::UHodgeCharacterRotationComponent` — L11
- `UHodgeCharacterRotationComponent::InitializeWithAbilitySystem` — L18
- `UHodgeCharacterRotationComponent::UninitializeFromAbilitySystem` — L35
- `UHodgeCharacterRotationComponent::HandleConstraintTagChanged` — L58
- `UHodgeCharacterRotationComponent::RefreshLocalState` — L63
- `UHodgeCharacterRotationComponent::PublishAuthorityState` — L77
- `UHodgeCharacterRotationComponent::IsReplayingMove` — L87
- `UHodgeCharacterRotationComponent::GetResolvedState` — L93
- `UHodgeCharacterRotationComponent::IsYawLocked` — L101
- `UHodgeCharacterRotationComponent::IsRecoveringFacing` — L102
- `UHodgeCharacterRotationComponent::GetLockedYaw` — L103
- `UHodgeCharacterRotationComponent::FilterControlRotation` — L105
- `UHodgeCharacterRotationComponent::NotifyFacingApplied` — L118
- `UHodgeCharacterRotationComponent::SetMoveReplayState` — L129
- `UHodgeCharacterRotationComponent::ClearMoveReplayState` — L135
- `UHodgeCharacterRotationComponent::OnRep_RotationState` — L141
- `UHodgeCharacterRotationComponent::GetLifetimeReplicatedProps` — L151
- `UHodgeCharacterRotationComponent::EndPlay` — L157

## HodgeCombatComponentBase.cpp

[Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp)

Experience 注入 Pawn 的统一战斗协调者；连段输入/记忆/预测校正、来源/配置检测体、独立会话与过滤；GA 负责目标去重及效果施加。

- `UHodgeCombatComponentBase::UHodgeCombatComponentBase` — L43
- `UHodgeCombatComponentBase::AcquirePoseLease` — L51
- `UHodgeCombatComponentBase::ReleasePoseLease` — L67
- `UHodgeCombatComponentBase::EndPlay` — L78
- `UHodgeCombatComponentBase::ResolveSource` — L86
- `UHodgeCombatComponentBase::IsSourceValid` — L121
- `UHodgeCombatComponentBase::CaptureDetectionGeometry` — L149
- `UHodgeCombatComponentBase::CreateDetectionSession` — L194
- `UHodgeCombatComponentBase::IsDetectionSessionValid` — L252
- `UHodgeCombatComponentBase::ResetDetectionHistory` — L258
- `UHodgeCombatComponentBase::GetDetectionSourceComponent` — L270
- `UHodgeCombatComponentBase::EndDetectionSession` — L278
- `UHodgeCombatComponentBase::EndDetectionSessionsForExecution` — L284
- `UHodgeCombatComponentBase::EndAllDetectionSessions` — L292
- `UHodgeCombatComponentBase::UpdateDetectionAnchor` — L297
- `UHodgeCombatComponentBase::SampleDetection` — L313
- `UHodgeCombatComponentBase::Configure` — L435
- `UHodgeCombatComponentBase::Shutdown` — L453
- `UHodgeCombatComponentBase::ClearInput` — L486
- `UHodgeCombatComponentBase::InputPressed` — L492
- `UHodgeCombatComponentBase::SelectTransition` — L511
- `UHodgeCombatComponentBase::IsAuthorized` — L531
- `UHodgeCombatComponentBase::ExecutionKey` — L536
- `UHodgeCombatComponentBase::PrepareTransition` — L543
- `UHodgeCombatComponentBase::TryTransition` — L580
- `UHodgeCombatComponentBase::GetServerInputBufferSeconds` — L617
- `UHodgeCombatComponentBase::ValidateServerRequestIdentity` — L622
- `UHodgeCombatComponentBase::CanBufferServerActivation` — L632
- `UHodgeCombatComponentBase::PrepareServerActivation` — L651
- `UHodgeCombatComponentBase::PrepareConfirmedActivation` — L663
- `UHodgeCombatComponentBase::RejectServerActivation` — L687
- `UHodgeCombatComponentBase::CompleteServerActivation` — L693
- `UHodgeCombatComponentBase::ExecutionStarted` — L714
- `UHodgeCombatComponentBase::ExecutionEnded` — L725
- `UHodgeCombatComponentBase::WindowsChanged` — L739
- `UHodgeCombatComponentBase::ExecutionEvent` — L748
- `UHodgeCombatComponentBase::DrainEvents` — L755
- `UHodgeCombatComponentBase::SetNode` — L773
- `UHodgeCombatComponentBase::ResetSession` — L787
- `UHodgeCombatComponentBase::ComboTime` — L801
- `UHodgeCombatComponentBase::GetRememberedComboTag` — L808
- `UHodgeCombatComponentBase::GetComboMemoryRemainingTime` — L815
- `UHodgeCombatComponentBase::TransitionSourceNode` — L822
- `UHodgeCombatComponentBase::HasResumeTransition` — L829
- `UHodgeCombatComponentBase::RetainComboMemory` — L838
- `UHodgeCombatComponentBase::ExpireComboMemory` — L849
- `UHodgeCombatComponentBase::PublishComboMemory` — L859
- `UHodgeCombatComponentBase::EndCurrentExecution` — L868
- `UHodgeCombatComponentBase::OnRep_ComboMemory` — L873
- `UHodgeCombatComponentBase::HandlePredictionRejected` — L882
- `UHodgeCombatComponentBase::ServerSynchronizeComboMemory_Implementation` — L890
- `UHodgeCombatComponentBase::ClientCorrectComboMemory_Implementation` — L896
- `UHodgeCombatComponentBase::TickComponent` — L902
- `UHodgeCombatComponentBase::ServerMoveCancel_Implementation` — L929
- `UHodgeCombatComponentBase::ProcessServerMoveCancel` — L942
- `UHodgeCombatComponentBase::ClientMoveCancelResult_Implementation` — L955
- `UHodgeCombatComponentBase::ServerReturnToEntry_Implementation` — L957
- `UHodgeCombatComponentBase::GetLifetimeReplicatedProps` — L969
- `UHodgeCombatComponentBase::OnRep_ObserverTags` — L976
- `UHodgeCombatComponentBase::FindCombatComponent` — L987
- `UHodgeCombatComponentBase::IsComboReady` — L992
- `UHodgeCombatComponentBase::BindPawnExtension` — L1000
- `UHodgeCombatComponentBase::HandleAbilitySystemInitialized` — L1011
- `UHodgeCombatComponentBase::HandleAbilitySystemUninitialized` — L1018
- `UHodgeCombatComponentBase::OnRegister` — L1023
- `UHodgeCombatComponentBase::BeginPlay` — L1029
- `UHodgeCombatComponentBase::OnUnregister` — L1036

## HodgeExperienceManagerComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeExperienceManagerComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeExperienceManagerComponent.cpp)

GameState 上的 Experience 复制、资源加载、插件激活、Action 执行与 Loaded 委托。

- `UHodgeExperienceManagerComponent::UHodgeExperienceManagerComponent` — L63
- `UHodgeExperienceManagerComponent::SetCurrentExperience` — L70
- `UHodgeExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_HighPriority` — L101
- `UHodgeExperienceManagerComponent::CallOrRegister_OnExperienceLoaded` — L117
- `UHodgeExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_LowPriority` — L132
- `UHodgeExperienceManagerComponent::GetCurrentExperienceChecked` — L148
- `UHodgeExperienceManagerComponent::IsExperienceLoaded` — L161
- `UHodgeExperienceManagerComponent::OnRep_CurrentExperience` — L168
- `UHodgeExperienceManagerComponent::StartExperienceLoad` — L175
- `UHodgeExperienceManagerComponent::OnExperienceLoadComplete` — L314
- `UHodgeExperienceManagerComponent::OnGameFeaturePluginLoadComplete` — L399
- `UHodgeExperienceManagerComponent::OnExperienceFullLoadCompleted` — L412
- `UHodgeExperienceManagerComponent::OnActionDeactivationCompleted` — L511
- `UHodgeExperienceManagerComponent::GetLifetimeReplicatedProps` — L527
- `UHodgeExperienceManagerComponent::EndPlay` — L536
- `UHodgeExperienceManagerComponent::ShouldShowLoadingScreen` — L627
- `UHodgeExperienceManagerComponent::OnAllActionsDeactivated` — L645

## HodgeHealthComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeHealthComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeHealthComponent.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeHealthComponent::UHodgeHealthComponent` — L30
- `UHodgeHealthComponent::GetLifetimeReplicatedProps` — L53
- `UHodgeHealthComponent::OnUnregister` — L63
- `UHodgeHealthComponent::InitializeWithAbilitySystem` — L73
- `UHodgeHealthComponent::ResetForSpawn` — L135
- `UHodgeHealthComponent::UninitializeFromAbilitySystem` — L144
- `UHodgeHealthComponent::ClearGameplayTags` — L169
- `UHodgeHealthComponent::GetHealth` — L183
- `UHodgeHealthComponent::GetMaxHealth` — L190
- `UHodgeHealthComponent::GetHealthNormalized` — L197
- `UHodgeHealthComponent::HandleHealthChanged` — L217
- `UHodgeHealthComponent::HandleMaxHealthChanged` — L226
- `UHodgeHealthComponent::HandleOutOfHealth` — L235
- `UHodgeHealthComponent::OnRep_DeathState` — L309
- `UHodgeHealthComponent::StartDeath` — L381
- `UHodgeHealthComponent::FinishDeath` — L413
- `UHodgeHealthComponent::DamageSelfDestruct` — L445

## HodgeHeroComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp)

玩家 Init State 协调、ASC 接入、输入与相机；额外输入句柄持久化并在移除/EndPlay 解绑。新增“移动意图”信号（HasMoveIntent / GetMoveIntent / OnMoveIntentChanged），记 Input_Move 原始输入量、Completed/Canceled 清零，供移动取消后摇消费。

- `UHodgeHeroComponent::NAME_BindInputsNow` — L83
- `UHodgeHeroComponent::NAME_ActorFeatureName` — L86
- `UHodgeHeroComponent::UHodgeHeroComponent` — L89
- `UHodgeHeroComponent::OnRegister` — L100
- `UHodgeHeroComponent::CanChangeInitState` — L144
- `UHodgeHeroComponent::HandleChangeInitState` — L263
- `UHodgeHeroComponent::OnActorInitStateChanged` — L330
- `UHodgeHeroComponent::CheckDefaultInitialization` — L346
- `UHodgeHeroComponent::BeginPlay` — L366
- `UHodgeHeroComponent::EndPlay` — L390
- `UHodgeHeroComponent::InitializePlayerInput` — L414
- `UHodgeHeroComponent::AddAdditionalInputConfig` — L640
- `UHodgeHeroComponent::RemoveAdditionalInputConfig` — L731
- `UHodgeHeroComponent::IsReadyToBindInputs` — L760
- `UHodgeHeroComponent::Input_AbilityInputTagPressed` — L767
- `UHodgeHeroComponent::Input_AbilityInputTagReleased` — L791
- `UHodgeHeroComponent::Input_Move` — L819
- `UHodgeHeroComponent::Input_MoveStopped` — L893
- `UHodgeHeroComponent::HasMoveIntent` — L901
- `UHodgeHeroComponent::SetMoveIntent` — L908
- `UHodgeHeroComponent::RefreshMoveIntent` — L915
- `UHodgeHeroComponent::Input_LookMouse` — L929
- `UHodgeHeroComponent::Input_LookStick` — L969
- `UHodgeHeroComponent::Input_Crouch` — L1017
- `UHodgeHeroComponent::Input_AutoRun` — L1031
- `UHodgeHeroComponent::DetermineCameraMode` — L1052
- `UHodgeHeroComponent::SetAbilityCameraMode` — L1085
- `UHodgeHeroComponent::ClearAbilityCameraMode` — L1100
- `UHodgeHeroComponent::ResetGameplayInput` — L1113

## HodgeInteractionComponentBase.cpp

[Source/Hodgepodge/Private/Component/HodgeInteractionComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeInteractionComponentBase.cpp)

交互组件基础占位；完整扫描、交互规则与 UI 需另行实现。

## HodgeMovementComponentBase.cpp

[Source/Hodgepodge/Private/Component/HodgeMovementComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeMovementComponentBase.cpp)

通用移动组件基础占位，与 CharacterMovement 派生类需区分。

## HodgePawnExtensionComponent.cpp

[Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp)

PawnData 复制、Init State、ASC 关联/解除、TagRelationshipMapping 与 ClearAbilityInput；需验证退出顺序。

- `UHodgePawnExtensionComponent::NAME_ActorFeatureName` — L18
- `UHodgePawnExtensionComponent::UHodgePawnExtensionComponent` — L20
- `UHodgePawnExtensionComponent::GetLifetimeReplicatedProps` — L34
- `UHodgePawnExtensionComponent::OnRegister` — L42
- `UHodgePawnExtensionComponent::BeginPlay` — L61
- `UHodgePawnExtensionComponent::EndPlay` — L75
- `UHodgePawnExtensionComponent::SetPawnData` — L86
- `UHodgePawnExtensionComponent::OnRep_PawnData` — L116
- `UHodgePawnExtensionComponent::InitializeAbilitySystem` — L122
- `UHodgePawnExtensionComponent::UninitializeAbilitySystem` — L182
- `UHodgePawnExtensionComponent::HandleControllerChanged` — L227
- `UHodgePawnExtensionComponent::HandlePlayerStateReplicated` — L251
- `UHodgePawnExtensionComponent::SetupPlayerInputComponent` — L257
- `UHodgePawnExtensionComponent::CheckDefaultInitialization` — L263
- `UHodgePawnExtensionComponent::CanChangeInitState` — L278
- `UHodgePawnExtensionComponent::HandleChangeInitState` — L338
- `UHodgePawnExtensionComponent::OnActorInitStateChanged` — L348
- `UHodgePawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall` — L360
- `UHodgePawnExtensionComponent::OnAbilitySystemUninitialized_Register` — L376
- `UHodgePawnExtensionComponent::UnregisterAbilitySystemDelegates` — L385

## HodgeGameInstanceBase.cpp

[Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp](../../../Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp)

注册 Init State 顺序、主控制器访问和全局生命周期扩展。

- `UHodgeGameInstanceBase::UHodgeGameInstanceBase` — L19
- `UHodgeGameInstanceBase::GetPrimaryPlayerController` — L24
- `UHodgeGameInstanceBase::Shutdown` — L30
- `UHodgeGameInstanceBase::Init` — L36
- `UHodgeGameInstanceBase::AddLocalPlayer` — L85
- `UHodgeGameInstanceBase::RemoveLocalPlayer` — L95

## HodgeGameModeBase.cpp

[Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp](../../../Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp)

服务器选择玩法、等待加载、生成 Pawn，并在 FinishSpawning 前注入 PawnData；选用 HodgePlayerController。

- `AHodgeGameModeBase::AHodgeGameModeBase` — L22
- `AHodgeGameModeBase::GetPawnDataForController` — L47
- `AHodgeGameModeBase::InitGame` — L88
- `AHodgeGameModeBase::HandleMatchAssignmentIfNotExpectingOne` — L97
- `AHodgeGameModeBase::TryDedicatedServerLogin` — L194
- `AHodgeGameModeBase::OnMatchAssignmentGiven` — L338
- `AHodgeGameModeBase::OnExperienceLoaded` — L362
- `AHodgeGameModeBase::IsExperienceLoaded` — L383
- `AHodgeGameModeBase::GetDefaultPawnClassForController_Implementation` — L397
- `AHodgeGameModeBase::SpawnDefaultPawnAtTransform_Implementation` — L413
- `AHodgeGameModeBase::ShouldSpawnAtStartSpot` — L470
- `AHodgeGameModeBase::HandleStartingNewPlayer_Implementation` — L476
- `AHodgeGameModeBase::ChoosePlayerStart_Implementation` — L486
- `AHodgeGameModeBase::FinishRestartPlayer` — L499
- `AHodgeGameModeBase::PlayerCanRestart_Implementation` — L512
- `AHodgeGameModeBase::ControllerCanRestart` — L518
- `AHodgeGameModeBase::InitGameState` — L547
- `AHodgeGameModeBase::GenericPlayerInitialization` — L562
- `AHodgeGameModeBase::RequestPlayerRestartNextFrame` — L571
- `AHodgeGameModeBase::UpdatePlayerStartSpot` — L592
- `AHodgeGameModeBase::FailedToRestartPlayer` — L598
- `AHodgeGameModeBase::RestartPlayer` — L637

## HodgeGameState.cpp

[Source/Hodgepodge/Private/Core/GameState/HodgeGameState.cpp](../../../Source/Hodgepodge/Private/Core/GameState/HodgeGameState.cpp)

创建 ExperienceManager 和世界状态 ASC，处理游戏状态复制/扩展。

- `AHodgeGameState::AHodgeGameState` — L25
- `AHodgeGameState::PreInitializeComponents` — L50
- `AHodgeGameState::PostInitializeComponents` — L56
- `AHodgeGameState::GetAbilitySystemComponent` — L67
- `AHodgeGameState::EndPlay` — L73
- `AHodgeGameState::AddPlayerState` — L79
- `AHodgeGameState::RemovePlayerState` — L85
- `AHodgeGameState::SeamlessTravelTransitionCheckpoint` — L92
- `AHodgeGameState::GetLifetimeReplicatedProps` — L108
- `AHodgeGameState::Tick` — L120
- `AHodgeGameState::GetServerFPS` — L147
- `AHodgeGameState::SetRecorderPlayerState` — L153
- `AHodgeGameState::GetRecorderPlayerState` — L172
- `AHodgeGameState::OnRep_RecorderPlayerState` — L178

## HodgeGameStateBase.cpp

[Source/Hodgepodge/Private/Core/GameState/HodgeGameStateBase.cpp](../../../Source/Hodgepodge/Private/Core/GameState/HodgeGameStateBase.cpp)

GameState 基础扩展生命周期。

- `AHodgeGameStateBase::AHodgeGameStateBase` — L14
- `AHodgeGameStateBase::PreInitializeComponents` — L19
- `AHodgeGameStateBase::BeginPlay` — L27
- `AHodgeGameStateBase::EndPlay` — L36

## HodgeHUD.cpp

[Source/Hodgepodge/Private/Core/HUD/HodgeHUD.cpp](../../../Source/Hodgepodge/Private/Core/HUD/HodgeHUD.cpp)

HUD Receiver 提供 Experience AddWidgets 的真实注入／撤销入口，同时保留 GAS 调试 Actor 列表。

- `AHodgeHUD::AHodgeHUD` — L37
- `AHodgeHUD::PreInitializeComponents` — L46
- `AHodgeHUD::BeginPlay` — L59
- `AHodgeHUD::EndPlay` — L73
- `AHodgeHUD::GetDebugActorList` — L85

## HodgeLocalPlayerBase.cpp

[Source/Hodgepodge/Private/Core/LocalPlayer/HodgeLocalPlayerBase.cpp](../../../Source/Hodgepodge/Private/Core/LocalPlayer/HodgeLocalPlayerBase.cpp)

本地玩家对象及控制器、PlayerState、Pawn 就绪事件桥。

- `UHodgeLocalPlayerBase::UHodgeLocalPlayerBase` — L20
- `UHodgeLocalPlayerBase::CallAndRegister_OnPlayerControllerSet` — L37
- `UHodgeLocalPlayerBase::CallAndRegister_OnPlayerStateSet` — L63
- `UHodgeLocalPlayerBase::CallAndRegister_OnPlayerPawnSet` — L88
- `UHodgeLocalPlayerBase::GetProjectionData` — L120

## HodgePlayerController.cpp

[Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerController.cpp](../../../Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerController.cpp)

具体控制器：每帧消费 ASC 输入、相机管理、AutoRun、UnPossess Avatar 清理及 Replay 扩展。

- `AHodgePlayerController::AHodgePlayerController` — L98
- `AHodgePlayerController::PreInitializeComponents` — L111
- `AHodgePlayerController::BeginPlay` — L118
- `AHodgePlayerController::EndPlay` — L143
- `AHodgePlayerController::GetLifetimeReplicatedProps` — L150
- `AHodgePlayerController::ReceivedPlayer` — L173
- `AHodgePlayerController::PlayerTick` — L180
- `AHodgePlayerController::GetHodgePlayerState` — L245
- `AHodgePlayerController::GetHodgeAbilitySystemComponent` — L252
- `AHodgePlayerController::GetHodgeHUD` — L262
- `AHodgePlayerController::TryToRecordClientReplay` — L269
- `AHodgePlayerController::ShouldRecordClientReplay` — L298
- `AHodgePlayerController::OnPlayerStateChangedTeam` — L365
- `AHodgePlayerController::OnPlayerStateChanged` — L372
- `AHodgePlayerController::BroadcastOnPlayerStateChanged` — L379
- `AHodgePlayerController::InitPlayerState` — L424
- `AHodgePlayerController::CleanupPlayerState` — L434
- `AHodgePlayerController::OnRep_PlayerState` — L444
- `AHodgePlayerController::SetPlayer` — L454
- `AHodgePlayerController::AddCheats` — L477
- `AHodgePlayerController::ServerCheat_Implementation` — L488
- `AHodgePlayerController::ServerCheat_Validate` — L501
- `AHodgePlayerController::ServerCheatAll_Implementation` — L508
- `AHodgePlayerController::ServerCheatAll_Validate` — L528
- `AHodgePlayerController::PreProcessInput` — L535
- `AHodgePlayerController::PostProcessInput` — L542
- `AHodgePlayerController::OnCameraPenetratingTarget` — L557
- `AHodgePlayerController::OnPossess` — L564
- `AHodgePlayerController::SetIsAutoRunning` — L589
- `AHodgePlayerController::GetIsAutoRunning` — L612
- `AHodgePlayerController::OnStartAutoRun` — L629
- `AHodgePlayerController::OnEndAutoRun` — L643
- `AHodgePlayerController::UpdateForceFeedback` — L657
- `AHodgePlayerController::UpdateHiddenComponents` — L680
- `AHodgePlayerController::OnUnPossess` — L790
- `AHodgeReplayPlayerController::Tick` — L819
- `AHodgeReplayPlayerController::SmoothTargetViewRotation` — L857
- `AHodgeReplayPlayerController::ShouldRecordClientReplay` — L867
- `AHodgeReplayPlayerController::RecorderPlayerStateUpdated` — L874
- `AHodgeReplayPlayerController::OnPlayerStatePawnSet` — L894

## HodgePlayerControllerBase.cpp

[Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerControllerBase.cpp](../../../Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerControllerBase.cpp)

控制器和 Pawn 生命周期桥接到 LocalPlayer 委托；具体输入消费在派生 HodgePlayerController。

- `AHodgePlayerControllerBase::AHodgePlayerControllerBase` — L22
- `AHodgePlayerControllerBase::ReceivedPlayer` — L50
- `AHodgePlayerControllerBase::SetPawn` — L81
- `AHodgePlayerControllerBase::OnPossess` — L116
- `AHodgePlayerControllerBase::OnUnPossess` — L147
- `AHodgePlayerControllerBase::OnRep_PlayerState` — L185

## HodgePlayerState.cpp

[Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp](../../../Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp)

玩家 ASC、HealthSet、PawnData、阵营/标签栈等持有者；SetPawnData 在权威端授予 AbilitySets（未记录句柄）。

- `AHodgePlayerState::NAME_HodgeAbilityReady` — L38
- `AHodgePlayerState::AHodgePlayerState` — L41
- `AHodgePlayerState::GetHodgePlayerController` — L71
- `AHodgePlayerState::GetAbilitySystemComponent` — L78
- `AHodgePlayerState::SetPawnData` — L85
- `AHodgePlayerState::PreInitializeComponents` — L130
- `AHodgePlayerState::PostInitializeComponents` — L168
- `AHodgePlayerState::Reset` — L175
- `AHodgePlayerState::ClientInitialize` — L182
- `AHodgePlayerState::CopyProperties` — L195
- `AHodgePlayerState::GetLifetimeReplicatedProps` — L205
- `AHodgePlayerState::OnDeactivated` — L242
- `AHodgePlayerState::OnReactivated` — L278
- `AHodgePlayerState::SetPlayerConnectionType` — L292
- `AHodgePlayerState::SetSquadID` — L302
- `AHodgePlayerState::AddStatTagStack` — L316
- `AHodgePlayerState::RemoveStatTagStack` — L323
- `AHodgePlayerState::GetStatTagStackCount` — L330
- `AHodgePlayerState::HasStatTag` — L337
- `AHodgePlayerState::GetReplicatedViewRotation` — L344
- `AHodgePlayerState::SetReplicatedViewRotation` — L351
- `AHodgePlayerState::OnExperienceLoaded` — L365
- `AHodgePlayerState::OnRep_PawnData` — L388
- `AHodgePlayerState::OnRep_MyTeamID` — L394
- `AHodgePlayerState::OnRep_MySquadID` — L401
- `AHodgePlayerState::SetCharacterLevel` — L407
- `AHodgePlayerState::RestoreCharacterHealth` — L412
- `AHodgePlayerState::InitializeCharacterProgression` — L417
- `AHodgePlayerState::AreAttributesReadyFor` — L428
- `AHodgePlayerState::NotifyAttributeReadiness` — L434
- `AHodgePlayerState::OnRep_AttributeReadyState` — L443

## HodgePlayerStateBase.cpp

[Source/Hodgepodge/Private/Core/PlayState/HodgePlayerStateBase.cpp](../../../Source/Hodgepodge/Private/Core/PlayState/HodgePlayerStateBase.cpp)

PlayerState ModularGameplay Receiver 注册、注销及组件 Reset/CopyProperties。

- `AHodgePlayerStateBase::PreInitializeComponents` — L10
- `AHodgePlayerStateBase::BeginPlay` — L19
- `AHodgePlayerStateBase::EndPlay` — L29
- `AHodgePlayerStateBase::Reset` — L38
- `AHodgePlayerStateBase::CopyProperties` — L55

## HodgeAbilityDefinition.cpp

[Source/Hodgepodge/Private/Data/HodgeAbilityDefinition.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAbilityDefinition.cpp)

Montage/Blend/Timeline、HitWindows/HitPoints、Volume、独立/连段路由及输入；校验几何、权威消息与手持覆盖。

- `FHodgeAbilityBlendSettings::MakeBlend` — L10
- `FHodgeAbilityBlendSettings::IsValid` — L19
- `UHodgeAbilityDefinition::GetDuration` — L26
- `UHodgeAbilityDefinition::ValidateDefinition` — L31
- `UHodgeAbilityDefinition::IsDataValid` — L90

## HodgeAbilitySet.cpp

[Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp)

权威端批量授予属性集、技能和 GE，并用句柄集合撤销。

- `FHodgeAbilitySet_GrantedHandles::AddAbilitySpecHandle` — L13
- `FHodgeAbilitySet_GrantedHandles::AddGameplayEffectHandle` — L22
- `FHodgeAbilitySet_GrantedHandles::AddAttributeSet` — L31
- `FHodgeAbilitySet_GrantedHandles::TakeFromAbilitySystem` — L37
- `UHodgeAbilitySet::UHodgeAbilitySet` — L78
- `UHodgeAbilitySet::GiveToAbilitySystem` — L84

## HodgeAssetManager.cpp

[Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp)

资产入口、GameData 缓存、启动任务、同步加载、加载进度。Cue 初始化钩子仍需接通。（PreloadPrimaryAssetBundles 已随未提交改动回退，当前不存在。）

- `FHodgeBundles::Equipped` — L10
- `UHodgeAssetManager::UHodgeAssetManager` — L44
- `UHodgeAssetManager::Get` — L50
- `UHodgeAssetManager::DumpLoadedAssets` — L69
- `UHodgeAssetManager::GetGameData` — L84
- `UHodgeAssetManager::GetDefaultPawnData` — L90
- `UHodgeAssetManager::SynchronousLoadAsset` — L96
- `UHodgeAssetManager::ShouldLogAssetLoads` — L130
- `UHodgeAssetManager::AddLoadedAsset` — L141
- `UHodgeAssetManager::StartInitialLoading` — L154
- `UHodgeAssetManager::LoadGameDataOfClass` — L175
- `UHodgeAssetManager::DoAllStartupJobs` — L277
- `UHodgeAssetManager::InitializeGameplayCueManager` — L366
- `UHodgeAssetManager::UpdateInitialGameContentLoadPercent` — L377
- `UHodgeAssetManager::PreBeginPIE` — L385

## HodgeAssetManagerStartupJob.cpp

[Source/Hodgepodge/Private/Data/HodgeAssetManagerStartupJob.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAssetManagerStartupJob.cpp)

封装启动任务与进度权重，供 AssetManager 执行启动工作。

- `FHodgeAssetManagerStartupJob::DoJob` — L13

## HodgeCharacterStatProfile.cpp

[Source/Hodgepodge/Private/Data/HodgeCharacterStatProfile.cpp](../../../Source/Hodgepodge/Private/Data/HodgeCharacterStatProfile.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeCharacterStatProfile::UHodgeCharacterStatProfile` — L11
- `UHodgeCharacterStatProfile::Evaluate` — L16
- `UHodgeCharacterStatProfile::Validate` — L24
- `UHodgeCharacterStatProfile::IsDataValid` — L52

## HodgeComboDefinition.cpp

[Source/Hodgepodge/Private/Data/HodgeComboDefinition.cpp](../../../Source/Hodgepodge/Private/Data/HodgeComboDefinition.cpp)

跳转 DataTable、输入缓存、结束后连段记忆与 bAllowAfterExecutionEnded 续段许可。

- `UHodgeComboDefinition::FindNode` — L7
- `UHodgeComboDefinition::ValidateDefinition` — L14
- `UHodgeComboDefinition::IsDataValid` — L93

## HodgeEquipmentStatProfile.cpp

[Source/Hodgepodge/Private/Data/HodgeEquipmentStatProfile.cpp](../../../Source/Hodgepodge/Private/Data/HodgeEquipmentStatProfile.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeEquipmentStatProfile::UHodgeEquipmentStatProfile` — L11
- `UHodgeEquipmentStatProfile::Evaluate` — L16
- `UHodgeEquipmentStatProfile::Validate` — L24
- `UHodgeEquipmentStatProfile::IsDataValid` — L52

## HodgeExperienceActionSet.cpp

[Source/Hodgepodge/Private/Data/HodgeExperienceActionSet.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceActionSet.cpp)

复用 GameFeature 插件和动作配置的数据资产。

- `UHodgeExperienceActionSet::UHodgeExperienceActionSet` — L16
- `UHodgeExperienceActionSet::IsDataValid` — L23
- `UHodgeExperienceActionSet::UpdateAssetBundleData` — L62

## HodgeExperienceDefinition.cpp

[Source/Hodgepodge/Private/Data/HodgeExperienceDefinition.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceDefinition.cpp)

声明玩法所需插件、默认 PawnData、直接 Actions 和组合 ActionSets。

- `UHodgeExperienceDefinition::UHodgeExperienceDefinition` — L16
- `UHodgeExperienceDefinition::IsDataValid` — L23
- `UHodgeExperienceDefinition::UpdateAssetBundleData` — L97

## HodgeExperienceManager.cpp

[Source/Hodgepodge/Private/Data/HodgeExperienceManager.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceManager.cpp)

管理编辑器等场景的 GameFeature 使用/停用协调，不是挂载在 GameState 的组件。

- `UHodgeExperienceManager::OnPlayInEditorBegun` — L12
- `UHodgeExperienceManager::NotifyOfPluginActivation` — L22
- `UHodgeExperienceManager::RequestToDeactivatePlugin` — L42

## HodgeGameData.cpp

[Source/Hodgepodge/Private/Data/HodgeGameData.cpp](../../../Source/Hodgepodge/Private/Data/HodgeGameData.cpp)

全局伤害、治疗、动态 Tag GE 的软类引用配置；需编辑器核对实际赋值。

- `UHodgeGameData::UHodgeGameData` — L8
- `UHodgeGameData::Get` — L12

## HodgePawnData.cpp

[Source/Hodgepodge/Private/Data/HodgePawnData.cpp](../../../Source/Hodgepodge/Private/Data/HodgePawnData.cpp)

PawnClass、AbilitySets、ComboDefinition、DefaultWeaponDefinition、输入/相机/关系映射配置，编辑器校验统一在主 cpp。

- `UHodgePawnData::UHodgePawnData` — L18
- `UHodgePawnData::IsDataValid` — L28

## HodgeEquipmentDefinition.cpp

[Source/Hodgepodge/Private/Equipment/HodgeEquipmentDefinition.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentDefinition.cpp)

装备实例类、Actor 挂接与 AbilitySets 配置。

- `UHodgeEquipmentDefinition::UHodgeEquipmentDefinition` — L11

## HodgeEquipmentInstance.cpp

[Source/Hodgepodge/Private/Equipment/HodgeEquipmentInstance.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentInstance.cpp)

Pawn 所属复制 UObject、SpawnedActors RepNotify 和装备/卸装生命周期。

- `UHodgeEquipmentInstance::UHodgeEquipmentInstance` — L32
- `UHodgeEquipmentInstance::GetWorld` — L38
- `UHodgeEquipmentInstance::GetLifetimeReplicatedProps` — L54
- `UHodgeEquipmentInstance::RegisterReplicationFragments` — L72
- `UHodgeEquipmentInstance::GetPawn` — L86
- `UHodgeEquipmentInstance::GetTypedPawn` — L92
- `UHodgeEquipmentInstance::SpawnEquipmentActors` — L113
- `UHodgeEquipmentInstance::DestroyEquipmentActors` — L152
- `UHodgeEquipmentInstance::OnRep_SpawnedActors` — L167
- `UHodgeEquipmentInstance::OnEquipped` — L173
- `UHodgeEquipmentInstance::OnUnequipped` — L180
- `UHodgeEquipmentInstance::OnRep_Instigator` — L187
- `UHodgeEquipmentInstance::SetStatIdentity` — L191

## HodgeEquipmentManagerComponent.cpp

[Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp)

Experience 注入 Pawn，ASC 就绪后装备默认剑，来源授予句柄精确撤销与客户端迟到绑定。

- `FHodgeAppliedEquipmentEntry::GetDebugString` — L52
- `FHodgeEquipmentList::PreReplicatedRemove` — L61
- `FHodgeEquipmentList::PostReplicatedAdd` — L78
- `FHodgeEquipmentList::PostReplicatedChange` — L95
- `FHodgeEquipmentList::GetAbilitySystemComponent` — L110
- `FHodgeEquipmentList::AddEntry` — L123
- `FHodgeEquipmentList::RemoveEntry` — L212
- `UHodgeEquipmentManagerComponent::UHodgeEquipmentManagerComponent` — L248
- `UHodgeEquipmentManagerComponent::GetLifetimeReplicatedProps` — L260
- `UHodgeEquipmentManagerComponent::EquipItem` — L270
- `UHodgeEquipmentManagerComponent::EquipItemWithState` — L277
- `UHodgeEquipmentManagerComponent::UnequipItem` — L321
- `UHodgeEquipmentManagerComponent::ReplicateSubobjects` — L352
- `UHodgeEquipmentManagerComponent::InitializeComponent` — L377
- `UHodgeEquipmentManagerComponent::UninitializeComponent` — L394
- `UHodgeEquipmentManagerComponent::ReadyForReplication` — L419
- `UHodgeEquipmentManagerComponent::GetFirstInstanceOfType` — L446
- `UHodgeEquipmentManagerComponent::GetEquipmentInstancesOfType` — L469
- `UHodgeEquipmentManagerComponent::FindInstanceOfDefinition` — L494
- `UHodgeEquipmentManagerComponent::SetEquipmentLevel` — L500

## HodgeWeaponInstance.cpp

[Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp)

手持请求、显现阶段、计时器、复制/拥有者预测及拒绝恢复，不新增角色常驻武器表现组件。

- `UHodgeWeaponInstance::UHodgeWeaponInstance` — L43
- `UHodgeWeaponInstance::OnEquipped` — L68
- `UHodgeWeaponInstance::OnUnequipped` — L90
- `UHodgeWeaponInstance::UpdateFiringTime` — L101
- `UHodgeWeaponInstance::GetTimeSinceLastInteractedWith` — L114
- `UHodgeWeaponInstance::PickBestAnimLayer` — L143
- `UHodgeWeaponInstance::GetOwningUserId` — L154
- `UHodgeWeaponInstance::ApplyDeviceProperties` — L168
- `UHodgeWeaponInstance::RemoveDeviceProperties` — L206
- `UHodgeWeaponInstance::OnDeathStarted` — L229
- `UHodgeWeaponInstance::GetLifetimeReplicatedProps` — L244
- `UHodgeWeaponInstance::GetPresentationTime` — L250
- `UHodgeWeaponInstance::CanDrivePresentation` — L257
- `UHodgeWeaponInstance::ResolvePresentationWeapon` — L265
- `UHodgeWeaponInstance::InitializePresentation` — L291
- `UHodgeWeaponInstance::ClearPresentationTimer` — L311
- `UHodgeWeaponInstance::ShutdownPresentation` — L316
- `UHodgeWeaponInstance::AcquireHandUse` — L331
- `UHodgeWeaponInstance::ReleaseHandUse` — L361
- `UHodgeWeaponInstance::ReleaseHandUsesForExecution` — L368
- `UHodgeWeaponInstance::BeginIdlePresentation` — L375
- `UHodgeWeaponInstance::SchedulePresentationPhase` — L381
- `UHodgeWeaponInstance::SetPresentationPhase` — L399
- `UHodgeWeaponInstance::GetPresentationState` — L443
- `UHodgeWeaponInstance::OnRep_PresentationState` — L451
- `UHodgeWeaponInstance::RejectPredictedHandUse` — L472
- `UHodgeWeaponInstance::RefreshPresentationActors` — L490
- `UHodgeWeaponInstance::OnSpawnedActorsChanged` — L499
- `UHodgeWeaponInstance::BeginDestroy` — L505
- `UHodgeWeaponInstance::UpdateOwnerPosePolicy` — L512

## HodgeWeaponPresentationActor.cpp

[Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationActor.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationActor.cpp)

检测 Mesh 留手，可见 Mesh 回背/悬浮/消隐挂 BackSocket，手持时挂回检测 Mesh。

- `AHodgeWeaponPresentationActor::AHodgeWeaponPresentationActor` — L14
- `AHodgeWeaponPresentationActor::BeginPlay` — L36
- `AHodgeWeaponPresentationActor::TryBindWeapon` — L44
- `AHodgeWeaponPresentationActor::ConfigureProfile` — L56
- `AHodgeWeaponPresentationActor::BindWeapon` — L73
- `AHodgeWeaponPresentationActor::RefreshPresentation` — L97
- `AHodgeWeaponPresentationActor::GetVisualTransform` — L105
- `AHodgeWeaponPresentationActor::EvaluatePresentation` — L110
- `AHodgeWeaponPresentationActor::Tick` — L182
- `AHodgeWeaponPresentationActor::EndPlay` — L188

## HodgeWeaponPresentationProfile.cpp

[Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationProfile.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationProfile.cpp)

模型/材质、手背插槽/偏移、曲线/时间；BackSocket 默认 WeaponOnBack，BackTransform 为插槽内偏移。

- `UHodgeWeaponPresentationProfile::Validate` — L8
- `UHodgeWeaponPresentationProfile::IsDataValid` — L25

## GameFeatureAction_AddAbilities.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddAbilities.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddAbilities.cpp)

面向配置 Actor 授予能力、属性与 AbilitySet，维护撤销句柄。

- `UGameFeatureAction_AddAbilities::OnGameFeatureActivating` — L39
- `UGameFeatureAction_AddAbilities::OnGameFeatureDeactivating` — L57
- `UGameFeatureAction_AddAbilities::IsDataValid` — L76
- `UGameFeatureAction_AddAbilities::AddToWorld` — L195
- `UGameFeatureAction_AddAbilities::Reset` — L244
- `UGameFeatureAction_AddAbilities::HandleActorExtension` — L261
- `UGameFeatureAction_AddAbilities::AddActorAbilities` — L292
- `UGameFeatureAction_AddAbilities::RemoveActorAbilities` — L410
- `UGameFeatureAction_AddAbilities::FindOrAddComponentForActor` — L449

## GameFeatureAction_AddGameplayCuePath.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddGameplayCuePath.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddGameplayCuePath.cpp)

声明和校验 Cue 路径；Policy 添加路径主体已有，但观察者注册和注销清理仍缺。

- `UGameFeatureAction_AddGameplayCuePath::UGameFeatureAction_AddGameplayCuePath` — L18
- `UGameFeatureAction_AddGameplayCuePath::IsDataValid` — L27

## GameFeatureAction_AddInputBinding.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputBinding.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputBinding.cpp)

有效扩展事件添加/移除额外 InputConfig；Hero 侧移除已实现并与 EndPlay 清理配套。

- `UGameFeatureAction_AddInputBinding::OnGameFeatureActivating` — L52
- `UGameFeatureAction_AddInputBinding::OnGameFeatureDeactivating` — L70
- `UGameFeatureAction_AddInputBinding::IsDataValid` — L88
- `UGameFeatureAction_AddInputBinding::AddToWorld` — L120
- `UGameFeatureAction_AddInputBinding::Reset` — L155
- `UGameFeatureAction_AddInputBinding::HandlePawnExtension` — L181
- `UGameFeatureAction_AddInputBinding::AddInputMappingForPlayer` — L207
- `UGameFeatureAction_AddInputBinding::RemoveInputMapping` — L252

## GameFeatureAction_AddInputContextMapping.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputContextMapping.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputContextMapping.cpp)

有效 Controller 扩展添加 IMC，包含设置注册与诊断日志；记录/撤销需验收。

- `UGameFeatureAction_AddInputContextMapping::OnGameFeatureRegistering` — L41
- `UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating` — L51
- `UGameFeatureAction_AddInputContextMapping::OnGameFeatureDeactivating` — L73
- `UGameFeatureAction_AddInputContextMapping::OnGameFeatureUnregistering` — L90
- `UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContexts` — L100
- `UGameFeatureAction_AddInputContextMapping::RegisterInputContextMappingsForGameInstance` — L119
- `UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer` — L143
- `UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContexts` — L181
- `UGameFeatureAction_AddInputContextMapping::UnregisterInputContextMappingsForGameInstance` — L202
- `UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer` — L225
- `UGameFeatureAction_AddInputContextMapping::IsDataValid` — L263
- `UGameFeatureAction_AddInputContextMapping::AddToWorld` — L295
- `UGameFeatureAction_AddInputContextMapping::Reset` — L330
- `UGameFeatureAction_AddInputContextMapping::HandleControllerExtension` — L355
- `UGameFeatureAction_AddInputContextMapping::AddInputMappingForPlayer` — L387
- `UGameFeatureAction_AddInputContextMapping::RemoveInputMapping` — L435

## GameFeatureAction_AddWidget.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddWidget.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddWidget.cpp)

Widget 注入迁移草稿，当前实现停用。

- `UGameFeatureAction_AddWidgets::OnGameFeatureDeactivating` — L51
- `UGameFeatureAction_AddWidgets::AddAdditionalAssetBundleData` — L70
- `UGameFeatureAction_AddWidgets::IsDataValid` — L85
- `UGameFeatureAction_AddWidgets::AddToWorld` — L160
- `UGameFeatureAction_AddWidgets::ClearActorContents` — L194
- `UGameFeatureAction_AddWidgets::Reset` — L205
- `UGameFeatureAction_AddWidgets::HandleActorExtension` — L218
- `UGameFeatureAction_AddWidgets::AddWidgets` — L237
- `UGameFeatureAction_AddWidgets::RemoveWidgets` — L286

## GameFeatureAction_SplitscreenConfig.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_SplitscreenConfig.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_SplitscreenConfig.cpp)

GameFeature 激活期间的分屏策略调整。

- `UGameFeatureAction_SplitscreenConfig::OnGameFeatureDeactivating` — L20
- `UGameFeatureAction_SplitscreenConfig::AddToWorld` — L76

## GameFeatureAction_WorldActionBase.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_WorldActionBase.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_WorldActionBase.cpp)

按游戏世界和激活上下文组织 Action 生命周期。

- `UGameFeatureAction_WorldActionBase::OnGameFeatureActivating` — L10
- `UGameFeatureAction_WorldActionBase::OnGameFeatureDeactivating` — L31
- `UGameFeatureAction_WorldActionBase::HandleGameInstanceStart` — L44

## HodgeGameFeaturePolicy.cpp

[Source/Hodgepodge/Private/GameFeatures/HodgeGameFeaturePolicy.cpp](../../../Source/Hodgepodge/Private/GameFeatures/HodgeGameFeaturePolicy.cpp)

已配置的 GameFeature 策略；Hotfix 观察者注册，Cue 路径观察者创建仍注释。

- `UHodgeGameFeaturePolicy::UHodgeGameFeaturePolicy` — L15
- `UHodgeGameFeaturePolicy::Get` — L24
- `UHodgeGameFeaturePolicy::InitGameFeatureManager` — L34
- `UHodgeGameFeaturePolicy::ShutdownGameFeatureManager` — L59
- `UHodgeGameFeaturePolicy::GetPreloadAssetListForGameFeature` — L79
- `UHodgeGameFeaturePolicy::GetPreloadBundleStateForGameFeature` — L92
- `UHodgeGameFeaturePolicy::GetGameFeatureLoadingMode` — L102
- `UHodgeGameFeaturePolicy::IsPluginAllowed` — L116
- `UHodgeGameFeature_HotfixManager::OnGameFeatureLoading` — L128
- `UHodgeGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering` — L156
- `UHodgeGameFeature_AddGameplayCuePaths::OnGameFeatureUnregistering` — L240

## HodgeAimSensitivityData.cpp

[Source/Hodgepodge/Private/Input/HodgeAimSensitivityData.cpp](../../../Source/Hodgepodge/Private/Input/HodgeAimSensitivityData.cpp)

瞄准灵敏度数据映射。

- `UHodgeAimSensitivityData::UHodgeAimSensitivityData` — L12

## HodgeInputComponent.cpp

[Source/Hodgepodge/Private/Input/HodgeInputComponent.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputComponent.cpp)

基于 Tag 的 Native/Ability Action 绑定和句柄移除；映射辅助函数仍占位。

- `UHodgeInputComponent::UHodgeInputComponent` — L23
- `UHodgeInputComponent::AddInputMappings` — L39
- `UHodgeInputComponent::RemoveInputMappings` — L59
- `UHodgeInputComponent::RemoveBinds` — L81

## HodgeInputConfig.cpp

[Source/Hodgepodge/Private/Input/HodgeInputConfig.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputConfig.cpp)

NativeInputActions / AbilityInputActions 的 IA 与 Tag 数据配置及查询。

- `UHodgeInputConfig::UHodgeInputConfig` — L22
- `UHodgeInputConfig::FindNativeInputActionForTag` — L37
- `UHodgeInputConfig::FindAbilityInputActionForTag` — L68

## HodgeInputModifiers.cpp

[Source/Hodgepodge/Private/Input/HodgeInputModifiers.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputModifiers.cpp)

输入数值处理扩展；具体启用情况由 IA/IMC 资产决定。

- `UHodgeSettingBasedScalar::ModifyRaw_Implementation` — L55
- `UHodgeInputModifierDeadZone::ModifyRaw_Implementation` — L146
- `UHodgeInputModifierDeadZone::GetVisualizationColor_Implementation` — L231
- `UHodgeInputModifierGamepadSensitivity::ModifyRaw_Implementation` — L253
- `UHodgeInputModifierAimInversion::ModifyRaw_Implementation` — L292

## HodgeInputUserSettings.cpp

[Source/Hodgepodge/Private/Input/HodgeInputUserSettings.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputUserSettings.cpp)

Enhanced Input 用户设置派生入口；须核对实际设置类配置。

- `UHodgeInputUserSettings::ApplySettings` — L9

## HodgePlayerMappableKeyProfile.cpp

[Source/Hodgepodge/Private/Input/HodgePlayerMappableKeyProfile.cpp](../../../Source/Hodgepodge/Private/Input/HodgePlayerMappableKeyProfile.cpp)

玩家键位 Profile 扩展。

- `UHodgePlayerMappableKeyProfile::EquipProfile` — L9
- `UHodgePlayerMappableKeyProfile::UnEquipProfile` — L19

## HodgeAbilitySourceInterface.cpp

[Source/Hodgepodge/Private/Interface/HodgeAbilitySourceInterface.cpp](../../../Source/Hodgepodge/Private/Interface/HodgeAbilitySourceInterface.cpp)

技能来源与相关计算契约。

- `UHodgeAbilitySourceInterface::UHodgeAbilitySourceInterface` — L7

## LoadingProcessInterface.cpp

[Source/Hodgepodge/Private/Interface/LoadingProcessInterface.cpp](../../../Source/Hodgepodge/Private/Interface/LoadingProcessInterface.cpp)

加载状态/原因查询契约；不是独立加载界面。

## HodgeAbilityDefinitionTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeAbilityDefinitionTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAbilityDefinitionTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeDefinitionGrantTest::RunTest` — L19
- `FHodgeComboValidationTest::RunTest` — L55
- `FHodgeComboRetentionTest::RunTest` — L128
- `FHodgeComboSessionTest::RunTest` — L247

## HodgeAnimNotifyTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeAnimNotifyTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAnimNotifyTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeNotifyConfigurationTest::RunTest` — L10
- `FHodgeNotifyContextTest::RunTest` — L30

## HodgeAttributeGrowthTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeAttributeGrowthTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAttributeGrowthTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeStatProfileTest::RunTest` — L97
- `FHodgeStatGrowthTest::RunTest` — L117
- `FHodgeStatReentryTest::RunTest` — L153
- `FHodgeStatAvatarTest::RunTest` — L182

## HodgeCharacterRotationTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeCharacterRotationTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeCharacterRotationTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeCharacterRotationConstraintsTest::RunTest` — L14

## HodgeHitDetectionTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeHitDetectionTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeHitDetectionTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeHitGeometryTest::RunTest` — L11
- `FHodgeHitProfileValidationTest::RunTest` — L36

## HodgeMeleeRoutingTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeMeleeRoutingTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeMeleeRoutingTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeDetectionRoutingTest::RunTest` — L82
- `FHodgeMeleeHitHistoryTest::RunTest` — L112
- `FHodgeMeleeSpecContextTest::RunTest` — L135

## HodgeUIDataSourceTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeUIDataSourceTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeUIDataSourceTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeUIInactiveDataTest::RunTest` — L8

## HodgeUIFoundationTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeUIFoundationTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeUIFoundationTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeUIClosedRootTest::RunTest` — L11
- `FHodgeUINonHodgeInstanceTest::RunTest` — L34

## HodgeWeaponPresentationTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeWeaponPresentationTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeWeaponPresentationTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeWeaponRequestsTest::RunTest` — L22
- `FHodgeWeaponGeometryTest::RunTest` — L91

## MaterialProgressBar.cpp

[Source/Hodgepodge/Private/UI/Basic/MaterialProgressBar.cpp](../../../Source/Hodgepodge/Private/UI/Basic/MaterialProgressBar.cpp)

模块定义或基础代码；请查看对应文件。

- `UMaterialProgressBar::SynchronizeProperties` — L11
- `UMaterialProgressBar::OnWidgetRebuilt` — L80
- `UMaterialProgressBar::OnAnimationFinished_Implementation` — L139
- `UMaterialProgressBar::SetProgress` — L149
- `UMaterialProgressBar::SetStartProgress` — L157
- `UMaterialProgressBar::SetColorA` — L165
- `UMaterialProgressBar::SetColorB` — L173
- `UMaterialProgressBar::SetColorBackground` — L181
- `UMaterialProgressBar::AnimateProgressFromStart` — L189
- `UMaterialProgressBar::AnimateProgressFromCurrent` — L196
- `UMaterialProgressBar::SetProgress_Internal` — L208
- `UMaterialProgressBar::SetStartProgress_Internal` — L217
- `UMaterialProgressBar::SetColorA_Internal` — L226
- `UMaterialProgressBar::SetColorB_Internal` — L235
- `UMaterialProgressBar::SetColorBackground_Internal` — L244
- `UMaterialProgressBar::GetBarDynamicMaterial` — L253

## HodgeBoundActionButton.cpp

[Source/Hodgepodge/Private/UI/Common/HodgeBoundActionButton.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeBoundActionButton.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeBoundActionButton::NativeConstruct` — L12
- `UHodgeBoundActionButton::HandleInputMethodChanged` — L23

## HodgeListView.cpp

[Source/Hodgepodge/Private/UI/Common/HodgeListView.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeListView.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeListView::UHodgeListView` — L14
- `UHodgeListView::ValidateCompiledDefaults` — L21
- `UHodgeListView::OnGenerateEntryWidgetInternal` — L35

## HodgeTabButtonBase.cpp

[Source/Hodgepodge/Private/UI/Common/HodgeTabButtonBase.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeTabButtonBase.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeTabButtonBase::SetIconFromLazyObject` — L13
- `UHodgeTabButtonBase::SetIconBrush` — L21
- `UHodgeTabButtonBase::SetTabLabelInfo_Implementation` — L29

## HodgeTabListWidgetBase.cpp

[Source/Hodgepodge/Private/UI/Common/HodgeTabListWidgetBase.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeTabListWidgetBase.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeTabListWidgetBase::NativeOnInitialized` — L10
- `UHodgeTabListWidgetBase::NativeConstruct` — L15
- `UHodgeTabListWidgetBase::NativeDestruct` — L22
- `UHodgeTabListWidgetBase::GetPreregisteredTabInfo` — L36
- `UHodgeTabListWidgetBase::SetTabHiddenState` — L53
- `UHodgeTabListWidgetBase::RegisterDynamicTab` — L65
- `UHodgeTabListWidgetBase::HandlePreLinkedSwitcherChanged` — L78
- `UHodgeTabListWidgetBase::HandlePostLinkedSwitcherChanged` — L92
- `UHodgeTabListWidgetBase::HandleTabCreation_Implementation` — L103
- `UHodgeTabListWidgetBase::IsFirstTabActive` — L132
- `UHodgeTabListWidgetBase::IsLastTabActive` — L142
- `UHodgeTabListWidgetBase::IsTabVisible` — L152
- `UHodgeTabListWidgetBase::GetVisibleTabCount` — L165
- `UHodgeTabListWidgetBase::SetupTabs` — L180

## HodgeWidgetFactory.cpp

[Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeWidgetFactory::FindWidgetClassForData_Implementation` — L10

## HodgeWidgetFactory_Class.cpp

[Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory_Class.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory_Class.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeWidgetFactory_Class::FindWidgetClassForData_Implementation` — L9

## HodgeGameplayUIDataSource.cpp

[Source/Hodgepodge/Private/UI/Data/HodgeGameplayUIDataSource.cpp](../../../Source/Hodgepodge/Private/UI/Data/HodgeGameplayUIDataSource.cpp)

每个本地玩家共享的生命订阅、属性就绪与技能状态/输入接入；布局和按钮行为保存在 WBP。

- `UHodgeGameplayUIDataSource::Initialize` — L18
- `UHodgeGameplayUIDataSource::UnbindPawn` — L30
- `UHodgeGameplayUIDataSource::UnbindState` — L41
- `UHodgeGameplayUIDataSource::Shutdown` — L47
- `UHodgeGameplayUIDataSource::StateChanged` — L60
- `UHodgeGameplayUIDataSource::PawnChanged` — L68
- `UHodgeGameplayUIDataSource::Refresh` — L90
- `UHodgeGameplayUIDataSource::AttributeChanged` — L105
- `UHodgeGameplayUIDataSource::DeathChanged` — L106
- `UHodgeGameplayUIDataSource::CanUseGameplay` — L108
- `UHodgeGameplayUIDataSource::GetAbilityDisplayState` — L115
- `UHodgeGameplayUIDataSource::SubmitInput` — L145

## UIExtensionPointWidget.cpp

[Source/Hodgepodge/Private/UI/Extension/UIExtensionPointWidget.cpp](../../../Source/Hodgepodge/Private/UI/Extension/UIExtensionPointWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UUIExtensionPointWidget::UUIExtensionPointWidget` — L39
- `UUIExtensionPointWidget::SetExtensionPointTag` — L44
- `UUIExtensionPointWidget::ReleaseSlateResources` — L50
- `UUIExtensionPointWidget::RebuildWidget` — L61
- `UUIExtensionPointWidget::ResetExtensionPoint` — L132
- `UUIExtensionPointWidget::RegisterExtensionPoint` — L156
- `UUIExtensionPointWidget::RegisterExtensionPointForPlayerState` — L200
- `UUIExtensionPointWidget::OnAddOrRemoveExtension` — L230
- `UUIExtensionPointWidget::ValidateCompiledDefaults` — L313

## UIExtensionSystem.cpp

[Source/Hodgepodge/Private/UI/Extension/UIExtensionSystem.cpp](../../../Source/Hodgepodge/Private/UI/Extension/UIExtensionSystem.cpp)

模块定义或基础代码；请查看对应文件。

- `FUIExtensionPointHandle::Unregister` — L24
- `FUIExtensionHandle::Unregister` — L38
- `FUIExtensionPoint::DoesExtensionPassContract` — L52
- `UUIExtensionSubsystem::AddReferencedObjects` — L101
- `UUIExtensionSubsystem::Initialize` — L136
- `UUIExtensionSubsystem::Deinitialize` — L143
- `UUIExtensionSubsystem::RegisterExtensionPoint` — L150
- `UUIExtensionSubsystem::RegisterExtensionPointForContext` — L162
- `UUIExtensionSubsystem::RegisterExtensionAsWidget` — L225
- `UUIExtensionSubsystem::RegisterExtensionAsWidgetForContext` — L235
- `UUIExtensionSubsystem::RegisterExtensionAsData` — L244
- `UUIExtensionSubsystem::NotifyExtensionPointOfExtensions` — L307
- `UUIExtensionSubsystem::NotifyExtensionPointsOfExtension` — L357
- `UUIExtensionSubsystem::UnregisterExtension` — L412
- `UUIExtensionSubsystem::UnregisterExtensionPoint` — L466
- `UUIExtensionSubsystem::CreateExtensionRequest` — L502
- `UUIExtensionSubsystem::K2_RegisterExtensionPoint` — L526
- `UUIExtensionSubsystem::K2_RegisterExtensionAsWidget` — L544
- `UUIExtensionSubsystem::K2_RegisterExtensionAsWidgetForContext` — L553
- `UUIExtensionSubsystem::K2_RegisterExtensionAsData` — L572
- `UUIExtensionSubsystem::K2_RegisterExtensionAsDataForContext` — L580
- `UUIExtensionHandleFunctions::Unregister` — L600
- `UUIExtensionHandleFunctions::IsValid` — L606
- `UUIExtensionPointHandleFunctions::Unregister` — L614
- `UUIExtensionPointHandleFunctions::IsValid` — L620

## HodgeActionWidget.cpp

[Source/Hodgepodge/Private/UI/Foundation/HodgeActionWidget.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeActionWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeActionWidget::GetIcon` — L10
- `UHodgeActionWidget::GetEnhancedInputSubsystem` — L35

## HodgeButtonBase.cpp

[Source/Hodgepodge/Private/UI/Foundation/HodgeButtonBase.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeButtonBase.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeButtonBase::NativePreConstruct` — L8
- `UHodgeButtonBase::UpdateInputActionWidget` — L16
- `UHodgeButtonBase::SetButtonText` — L24
- `UHodgeButtonBase::RefreshButtonText` — L31
- `UHodgeButtonBase::OnInputMethodChanged` — L49

## HodgeConfirmationScreen.cpp

[Source/Hodgepodge/Private/UI/Foundation/HodgeConfirmationScreen.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeConfirmationScreen.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeControllerDisconnectedScreen.cpp

[Source/Hodgepodge/Private/UI/Foundation/HodgeControllerDisconnectedScreen.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeControllerDisconnectedScreen.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeControllerDisconnectedScreen::UHodgeControllerDisconnectedScreen` — L23
- `UHodgeControllerDisconnectedScreen::NativeOnActivated` — L30
- `UHodgeControllerDisconnectedScreen::ShouldDisplayChangeUserButton` — L64
- `UHodgeControllerDisconnectedScreen::HandleChangeUserClicked` — L79
- `UHodgeControllerDisconnectedScreen::HandleChangeUserCompleted` — L97

## HodgeLoadingScreenSubsystem.cpp

[Source/Hodgepodge/Private/UI/Foundation/HodgeLoadingScreenSubsystem.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeLoadingScreenSubsystem.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeLoadingScreenSubsystem::UHodgeLoadingScreenSubsystem` — L14
- `UHodgeLoadingScreenSubsystem::SetLoadingScreenContentWidget` — L18
- `UHodgeLoadingScreenSubsystem::GetLoadingScreenContentWidget` — L28

## HodgePrimaryGameLayout.cpp

[Source/Hodgepodge/Private/UI/Foundation/HodgePrimaryGameLayout.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgePrimaryGameLayout.cpp)

绑定 Designer 四个容器的本地玩家根布局，Push/Pop、异步加载取消与令牌恢复；不再生成运行时控件树。

- `UHodgePrimaryGameLayout::NativeOnInitialized` — L13
- `UHodgePrimaryGameLayout::RegisterLayer` — L23
- `UHodgePrimaryGameLayout::GetLayer` — L28
- `UHodgePrimaryGameLayout::Push` — L34
- `UHodgePrimaryGameLayout::Pop` — L43
- `UHodgePrimaryGameLayout::HasBlockingPage` — L52
- `UHodgePrimaryGameLayout::PushAsync` — L66
- `UHodgePrimaryGameLayout::FinishPush` — L85
- `UHodgePrimaryGameLayout::CancelPush` — L96
- `UHodgePrimaryGameLayout::ReleaseLayout` — L106
- `UHodgePrimaryGameLayout::NativeDestruct` — L120

## ApplyFrontendPerfSettingsAction.cpp

[Source/Hodgepodge/Private/UI/Frontend/ApplyFrontendPerfSettingsAction.cpp](../../../Source/Hodgepodge/Private/UI/Frontend/ApplyFrontendPerfSettingsAction.cpp)

模块定义或基础代码；请查看对应文件。

- `UApplyFrontendPerfSettingsAction::OnGameFeatureActivating` — L23
- `UApplyFrontendPerfSettingsAction::OnGameFeatureDeactivating` — L32

## HodgeFrontendStateComponent.cpp

[Source/Hodgepodge/Private/UI/Frontend/HodgeFrontendStateComponent.cpp](../../../Source/Hodgepodge/Private/UI/Frontend/HodgeFrontendStateComponent.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeLobbyBackground.cpp

[Source/Hodgepodge/Private/UI/Frontend/HodgeLobbyBackground.cpp](../../../Source/Hodgepodge/Private/UI/Frontend/HodgeLobbyBackground.cpp)

模块定义或基础代码；请查看对应文件。

## HodgeActivatableWidget.cpp

[Source/Hodgepodge/Private/UI/HodgeActivatableWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeActivatableWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeActivatableWidget::UHodgeActivatableWidget` — L17
- `UHodgeActivatableWidget::GetDesiredInputConfig` — L24
- `UHodgeActivatableWidget::ValidateCompiledWidgetTree` — L62

## HodgeGameViewportClient.cpp

[Source/Hodgepodge/Private/UI/HodgeGameViewportClient.cpp](../../../Source/Hodgepodge/Private/UI/HodgeGameViewportClient.cpp)

引擎 CommonUI 视口路由，已在 DefaultEngine.ini 启用。

## HodgeHUDLayout.cpp

[Source/Hodgepodge/Private/UI/HodgeHUDLayout.cpp](../../../Source/Hodgepodge/Private/UI/HodgeHUDLayout.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeHUDLayout::UHodgeHUDLayout` — L73
- `UHodgeHUDLayout::NativeOnInitialized` — L86
- `UHodgeHUDLayout::NativeDestruct` — L137
- `UHodgeHUDLayout::CloseOwnedMenu` — L168
- `UHodgeHUDLayout::NativeOnDeactivated` — L176
- `UHodgeHUDLayout::EnsureMenuLayerStack` — L184
- `UHodgeHUDLayout::HandleEscapeAction` — L193
- `UHodgeHUDLayout::HandleInputDeviceConnectionChanged` — L210
- `UHodgeHUDLayout::HandleInputDevicePairingChanged` — L234
- `UHodgeHUDLayout::ShouldPlatformDisplayControllerDisconnectScreen` — L258
- `UHodgeHUDLayout::NotifyControllerStateChangeForDisconnectScreen` — L291
- `UHodgeHUDLayout::ProcessControllerDevicesHavingChangedForDisconnectScreen` — L332
- `UHodgeHUDLayout::DisplayControllerDisconnectedMenu_Implementation` — L405
- `UHodgeHUDLayout::HideControllerDisconnectedMenu_Implementation` — L444

## HodgeJoystickWidget.cpp

[Source/Hodgepodge/Private/UI/HodgeJoystickWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeJoystickWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeJoystickWidget::UHodgeJoystickWidget` — L12
- `UHodgeJoystickWidget::NativeOnTouchStarted` — L18
- `UHodgeJoystickWidget::NativeOnTouchMoved` — L32
- `UHodgeJoystickWidget::NativeOnTouchEnded` — L45
- `UHodgeJoystickWidget::NativeOnMouseLeave` — L51
- `UHodgeJoystickWidget::NativeTick` — L57
- `UHodgeJoystickWidget::HandleTouchDelta` — L76
- `UHodgeJoystickWidget::StopInputSimulation` — L101

## HodgeSettingScreen.cpp

[Source/Hodgepodge/Private/UI/HodgeSettingScreen.cpp](../../../Source/Hodgepodge/Private/UI/HodgeSettingScreen.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeSimulatedInputWidget.cpp

[Source/Hodgepodge/Private/UI/HodgeSimulatedInputWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeSimulatedInputWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeSimulatedInputWidget::UHodgeSimulatedInputWidget` — L11
- `UHodgeSimulatedInputWidget::GetPaletteCategory` — L18
- `UHodgeSimulatedInputWidget::NativeConstruct` — L24
- `UHodgeSimulatedInputWidget::NativeDestruct` — L38
- `UHodgeSimulatedInputWidget::NativeOnTouchEnded` — L48
- `UHodgeSimulatedInputWidget::GetEnhancedInputSubsystem` — L55
- `UHodgeSimulatedInputWidget::GetPlayerInput` — L67
- `UHodgeSimulatedInputWidget::InputKeyValue` — L76
- `UHodgeSimulatedInputWidget::InputKeyValue2D` — L111
- `UHodgeSimulatedInputWidget::FlushSimulatedInput` — L116
- `UHodgeSimulatedInputWidget::QueryKeyToSimulate` — L124
- `UHodgeSimulatedInputWidget::OnControlMappingsRebuilt` — L140

## HodgeTaggedWidget.cpp

[Source/Hodgepodge/Private/UI/HodgeTaggedWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeTaggedWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeTaggedWidget::UHodgeTaggedWidget` — L13
- `UHodgeTaggedWidget::NativeConstruct` — L19
- `UHodgeTaggedWidget::NativeDestruct` — L44
- `UHodgeTaggedWidget::SetVisibility` — L61
- `UHodgeTaggedWidget::OnWatchedTagsChanged` — L120

## HodgeTouchRegion.cpp

[Source/Hodgepodge/Private/UI/HodgeTouchRegion.cpp](../../../Source/Hodgepodge/Private/UI/HodgeTouchRegion.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeTouchRegion::NativeOnTouchStarted` — L11
- `UHodgeTouchRegion::NativeOnTouchMoved` — L17
- `UHodgeTouchRegion::NativeOnTouchEnded` — L26
- `UHodgeTouchRegion::NativeTick` — L32

## HodgeIndicatorManagerComponent.cpp

[Source/Hodgepodge/Private/UI/IndicatorSystem/HodgeIndicatorManagerComponent.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/HodgeIndicatorManagerComponent.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeIndicatorManagerComponent::UHodgeIndicatorManagerComponent` — L14
- `UHodgeIndicatorManagerComponent::GetComponent` — L28
- `UHodgeIndicatorManagerComponent::AddIndicator` — L43
- `UHodgeIndicatorManagerComponent::RemoveIndicator` — L58

## IndicatorDescriptor.cpp

[Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorDescriptor.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorDescriptor.cpp)

模块定义或基础代码；请查看对应文件。

- `FIndicatorProjection::Project` — L21
- `UIndicatorDescriptor::SetIndicatorManagerComponent` — L254
- `UIndicatorDescriptor::UnregisterIndicator` — L267

## IndicatorLayer.cpp

[Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLayer.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLayer.cpp)

模块定义或基础代码；请查看对应文件。

- `UIndicatorLayer::UIndicatorLayer` — L24
- `UIndicatorLayer::ReleaseSlateResources` — L37
- `UIndicatorLayer::RebuildWidget` — L48

## IndicatorLibrary.cpp

[Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLibrary.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLibrary.cpp)

模块定义或基础代码；请查看对应文件。

- `UIndicatorLibrary::UIndicatorLibrary` — L16
- `UIndicatorLibrary::GetIndicatorManagerComponent` — L21

## SActorCanvas.cpp

[Source/Hodgepodge/Private/UI/IndicatorSystem/SActorCanvas.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/SActorCanvas.cpp)

模块定义或基础代码；请查看对应文件。

- `SActorCanvas::Construct` — L207
- `SActorCanvas::UpdateCanvas` — L253
- `SActorCanvas::SetShowAnyIndicators` — L484
- `SActorCanvas::OnArrangeChildren` — L506
- `SActorCanvas::OnPaint` — L796
- `SActorCanvas::~SActorCanvas` — L858
- `SActorCanvas::GetReferencerName` — L876
- `SActorCanvas::AddReferencedObjects` — L882
- `SActorCanvas::OnIndicatorAdded` — L890
- `SActorCanvas::OnIndicatorRemoved` — L904
- `SActorCanvas::AddIndicatorForEntry` — L918
- `SActorCanvas::OnIndicatorClassLoaded` — L955
- `SActorCanvas::RemoveIndicatorForEntry` — L1014
- `SActorCanvas::AddActorSlot` — L1047
- `SActorCanvas::RemoveActorSlot` — L1068
- `SActorCanvas::GetOffsetAndSize` — L1093
- `SActorCanvas::UpdateActiveTimer` — L1170

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

- `UHodgeUIManagerSubsystem::ShouldCreateSubsystem` — L16
- `UHodgeUIManagerSubsystem::Initialize` — L22
- `UHodgeUIManagerSubsystem::PlayerAdded` — L28
- `UHodgeUIManagerSubsystem::ControllerChanged` — L45
- `UHodgeUIManagerSubsystem::CreateRoot` — L65
- `UHodgeUIManagerSubsystem::DestroyRoot` — L77
- `UHodgeUIManagerSubsystem::PlayerRemoved` — L98
- `UHodgeUIManagerSubsystem::Deinitialize` — L108
- `UHodgeUIManagerSubsystem::GetRootLayout` — L117
- `UHodgeUIManagerSubsystem::IsGameInputAllowed` — L123
- `UHodgeUIManagerSubsystem::AllowsGameplayInput` — L131
- `UHodgeUIManagerSubsystem::ClearGameplayInput` — L139
- `UHodgeUIManagerSubsystem::RefreshInputState` — L149
- `UHodgeUIManagerSubsystem::SuspendInput` — L154
- `UHodgeUIManagerSubsystem::ResumeInput` — L167
- `UHodgeUIManagerSubsystem::GetUIManager` — L178
- `UHodgeUIManagerSubsystem::GetRootLayoutForController` — L183
- `UHodgeUIManagerSubsystem::GetGameplayDataForController` — L190

## HodgeUIMessaging.cpp

[Source/Hodgepodge/Private/UI/Subsystem/HodgeUIMessaging.cpp](../../../Source/Hodgepodge/Private/UI/Subsystem/HodgeUIMessaging.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## CircumferenceMarkerWidget.cpp

[Source/Hodgepodge/Private/UI/Weapons/CircumferenceMarkerWidget.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/CircumferenceMarkerWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UCircumferenceMarkerWidget::UCircumferenceMarkerWidget` — L10
- `UCircumferenceMarkerWidget::ReleaseSlateResources` — L17
- `UCircumferenceMarkerWidget::RebuildWidget` — L24
- `UCircumferenceMarkerWidget::SynchronizeProperties` — L34
- `UCircumferenceMarkerWidget::SetRadius` — L42

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

- `SCircumferenceMarkerWidget::SCircumferenceMarkerWidget` — L13
- `SCircumferenceMarkerWidget::Construct` — L17
- `SCircumferenceMarkerWidget::GetMarkerRenderTransform` — L26
- `SCircumferenceMarkerWidget::OnPaint` — L55
- `SCircumferenceMarkerWidget::ComputeDesiredSize` — L92
- `SCircumferenceMarkerWidget::SetRadius` — L100
- `SCircumferenceMarkerWidget::SetMarkerList` — L109

## SHitMarkerConfirmationWidget.cpp

[Source/Hodgepodge/Private/UI/Weapons/SHitMarkerConfirmationWidget.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/SHitMarkerConfirmationWidget.cpp)

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeAbilityCost.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h)

自定义额外能力消耗的扩展契约。

## HodgeAbilityTask_WaitHitResults.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h)

模块定义或基础代码；请查看对应文件。

## HodgeAbilityTask_WaitMoveCancel.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h)

合流取消窗口与本地移动意图，广播 OnMoveCancel 后结束；已有取消验证，当前 Combat 主链与旧 BasicAttack 消费方需区分。

## HodgeGameplayAbility.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。（PreloadPrimaryAssetsOnGrant 已随未提交改动回退，当前不存在。）

## HodgeGameplayAbility_Death.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Death.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Death.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayAbility_Definition.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)

执行单段 Definition、本次 Montage 时钟 Timeline 与手持窗口，按执行清理；Melee 子类扩展命中。

## HodgeGameplayAbility_Jump.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayAbility_Melee.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h)

接收服务器 Window/Point 命中结果，准备锚点/目标、逐段/共享去重，构建独立 GE Spec/Context 并施加。

## HodgeAttributeSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)

项目 AttributeSet 基础和 ASC 访问。

## HodgeCombatSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)

BaseDamage/BaseHeal 战斗属性，OwnerOnly 复制，供伤害/治疗 Execution 捕获。

## HodgeHealthSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)

Health/MaxHealth、Damage/Healing 元属性、结算夹取/免疫/耗尽广播；BaseDamage/BaseHeal 位于 CombatSet。

## HodgeDamageExecution.h

[Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeDamageExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeDamageExecution.h)

BaseDamage × SetByCaller 倍率 × 距离/材质衰减 × 共享目标规则；不再恒零。

## HodgeHealExecution.h

[Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeHealExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeHealExecution.h)

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

## HodgeAnimInstance.h

[Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)

ASC 属性映射、GroundDistance 与旋转策略快照；FullBody 权重协调蓝图程序性根 Yaw。

## HodgeCombatAnimNotifies.h

[Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)

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

## HodgeCameraMode_ThirdPerson.h

[Source/Hodgepodge/Public/Camera/HodgeCameraMode_ThirdPerson.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_ThirdPerson.h)

第三人称偏移与防穿透逻辑；C++ 不设默认偏移曲线，改由蓝图/PawnData 提供。

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

## HodgeDamageRules.h

[Source/Hodgepodge/Public/Combat/HodgeDamageRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeDamageRules.h)

检测和 Execution 共用目标、ASC、死亡/Health、自伤/友伤及队伍规则。

## HodgeHitDetection.h

[Source/Hodgepodge/Public/Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)

模块定义或基础代码；请查看对应文件。

## HodgeActorComponentBase.h

[Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)

通用 ActorComponent 基础访问与扩展。

## HodgeCharacterMovementComponent.h

[Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)

地面距离/加速度、最终旋转过滤、SavedMove 旋转策略重放与权威校正。

## HodgeCharacterRotationComponent.h

[Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)

ASC 旋转标签约束、锁定 Yaw、恢复、权威复制与移动重放状态。

## HodgeCombatComponentBase.h

[Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)

Experience 注入 Pawn 的统一战斗协调者；连段输入/记忆/预测校正、来源/配置检测体、独立会话与过滤；GA 负责目标去重及效果施加。

## HodgeExperienceManagerComponent.h

[Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)

GameState 上的 Experience 复制、资源加载、插件激活、Action 执行与 Loaded 委托。

## HodgeHealthComponent.h

[Source/Hodgepodge/Public/Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)

模块定义或基础代码；请查看对应文件。

## HodgeHeroComponent.h

[Source/Hodgepodge/Public/Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)

玩家 Init State 协调、ASC 接入、输入与相机；额外输入句柄持久化并在移除/EndPlay 解绑。新增“移动意图”信号（HasMoveIntent / GetMoveIntent / OnMoveIntentChanged），记 Input_Move 原始输入量、Completed/Canceled 清零，供移动取消后摇消费。

## HodgeInteractionComponentBase.h

[Source/Hodgepodge/Public/Component/HodgeInteractionComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeInteractionComponentBase.h)

交互组件基础占位；完整扫描、交互规则与 UI 需另行实现。

## HodgeMovementComponentBase.h

[Source/Hodgepodge/Public/Component/HodgeMovementComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeMovementComponentBase.h)

通用移动组件基础占位，与 CharacterMovement 派生类需区分。

## HodgePawnExtensionComponent.h

[Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)

PawnData 复制、Init State、ASC 关联/解除、TagRelationshipMapping 与 ClearAbilityInput；需验证退出顺序。

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

- `UHodgeAssetManager::GetAsset` — L139
- `UHodgeAssetManager::GetSubclass` — L170

## HodgeAssetManagerStartupJob.h

[Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h)

封装启动任务与进度权重，供 AssetManager 执行启动工作。

## HodgeCharacterStatProfile.h

[Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h)

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

## HodgePawnData.h

[Source/Hodgepodge/Public/Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)

PawnClass、AbilitySets、ComboDefinition、DefaultWeaponDefinition、输入/相机/关系映射配置，编辑器校验统一在主 cpp。

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

- `UHodgeInputComponent::BindNativeAction` — L138
- `UHodgeInputComponent::BindAbilityActions` — L150

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

## LoadingProcessInterface.h

[Source/Hodgepodge/Public/Interface/LoadingProcessInterface.h](../../../Source/Hodgepodge/Public/Interface/LoadingProcessInterface.h)

加载状态/原因查询契约；不是独立加载界面。

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
