> 2026-10-05 文件组织更新：原 `_Montage.cpp`、`_Validation.cpp` 已合入对应主 `.cpp`；下文保留原快照片段标题，源码链接指向合并后的文件。历史 snapshot.json 的路径/hash 不重写。

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

## HodgeAbilityEditorTests.cpp

[Source/HodgeAbilityEditor/Private/HodgeAbilityEditorTests.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeAbilityEditorTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeAbilityEditorTest::RunTest` — L11

## HodgeAbilityEditorToolkit.cpp

[Source/HodgeAbilityEditor/Private/HodgeAbilityEditorToolkit.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeAbilityEditorToolkit.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeAbilityEditorToolkit::~FHodgeAbilityEditorToolkit` — L33
- `FHodgeAbilityEditorToolkit::GetBaseToolkitName` — L40
- `FHodgeAbilityEditorToolkit::GetToolkitName` — L41
- `FHodgeAbilityEditorToolkit::GetToolkitToolTipText` — L42
- `FHodgeAbilityEditorToolkit::Init` — L43
- `FHodgeAbilityEditorToolkit::RegisterTabSpawners` — L76
- `FHodgeAbilityEditorToolkit::UnregisterTabSpawners` — L88
- `FHodgeAbilityEditorToolkit::SpawnMain` — L93
- `FHodgeAbilityEditorToolkit::AddReferencedObjects` — L145
- `FHodgeAbilityEditorToolkit::GetSaveableObjects` — L151
- `FHodgeAbilityEditorToolkit::SaveAsset_Execute` — L157
- `FHodgeAbilityEditorToolkit::Duration` — L163
- `FHodgeAbilityEditorToolkit::Snap` — L164
- `FHodgeAbilityEditorToolkit::SelectedIndex` — L165
- `FHodgeAbilityEditorToolkit::Select` — L169
- `FHodgeAbilityEditorToolkit::RebuildEventDetails` — L175
- `FHodgeAbilityEditorToolkit::EventEdited` — L186
- `FHodgeAbilityEditorToolkit::CommitTimeline` — L196
- `FHodgeAbilityEditorToolkit::SyncTimeline` — L205
- `FHodgeAbilityEditorToolkit::ObjectChanged` — L215
- `FHodgeAbilityEditorToolkit::Refresh` — L219
- `FHodgeAbilityEditorToolkit::PostUndo` — L229
- `FHodgeAbilityEditorToolkit::Seek` — L230
- `FHodgeAbilityEditorToolkit::TogglePlay` — L238
- `FHodgeAbilityEditorToolkit::TickPreview` — L256
- `FHodgeAbilityEditorToolkit::Validate` — L274
- `FHodgeAbilityEditorToolkit::StatusText` — L282
- `FHodgeAbilityEditorToolkit::AddEvent` — L286
- `FHodgeAbilityEditorToolkit::DuplicateEvent` — L303
- `FHodgeAbilityEditorToolkit::DeleteEvent` — L317
- `FHodgeAbilityEditorToolkit::MakeUniqueTimeline` — L327

## HodgeAbilityEditorToolkit.h

[Source/HodgeAbilityEditor/Private/HodgeAbilityEditorToolkit.h](../../../Source/HodgeAbilityEditor/Private/HodgeAbilityEditorToolkit.h)

模块定义或基础代码；请查看对应文件。

## SHodgeAbilityPreview.cpp

[Source/HodgeAbilityEditor/Private/SHodgeAbilityPreview.cpp](../../../Source/HodgeAbilityEditor/Private/SHodgeAbilityPreview.cpp)

模块定义或基础代码；请查看对应文件。

- `SHodgeAbilityPreview::Construct` — L31
- `SHodgeAbilityPreview::~SHodgeAbilityPreview` — L41
- `SHodgeAbilityPreview::MakeEditorViewportClient` — L46
- `SHodgeAbilityPreview::SetMesh` — L51
- `SHodgeAbilityPreview::SetMontage` — L64
- `SHodgeAbilityPreview::SetTime` — L81
- `SHodgeAbilityPreview::SetPlaying` — L99
- `SHodgeAbilityPreview::GetMeshPath` — L104

## SHodgeAbilityPreview.h

[Source/HodgeAbilityEditor/Private/SHodgeAbilityPreview.h](../../../Source/HodgeAbilityEditor/Private/SHodgeAbilityPreview.h)

模块定义或基础代码；请查看对应文件。

## SHodgeAbilityTimeline.cpp

[Source/HodgeAbilityEditor/Private/SHodgeAbilityTimeline.cpp](../../../Source/HodgeAbilityEditor/Private/SHodgeAbilityTimeline.cpp)

模块定义或基础代码；请查看对应文件。

- `SHodgeAbilityTimeline::ComputeDesiredSize` — L13
- `SHodgeAbilityTimeline::TimeAt` — L18
- `SHodgeAbilityTimeline::XAt` — L23
- `SHodgeAbilityTimeline::OnPaint` — L29
- `SHodgeAbilityTimeline::OnMouseButtonDown` — L87
- `SHodgeAbilityTimeline::OnMouseMove` — L115
- `SHodgeAbilityTimeline::FinishDrag` — L136
- `SHodgeAbilityTimeline::OnMouseButtonUp` — L141
- `SHodgeAbilityTimeline::OnMouseCaptureLost` — L147
- `SHodgeAbilityTimeline::OnMouseWheel` — L148
- `SHodgeAbilityTimeline::OnKeyDown` — L157

## SHodgeAbilityTimeline.h

[Source/HodgeAbilityEditor/Private/SHodgeAbilityTimeline.h](../../../Source/HodgeAbilityEditor/Private/SHodgeAbilityTimeline.h)

模块定义或基础代码；请查看对应文件。

## Hodgepodge.Build.cs

[Source/Hodgepodge/Hodgepodge.Build.cs](../../../Source/Hodgepodge/Hodgepodge.Build.cs)

模块定义或基础代码；请查看对应文件。

## Hodgepodge.cpp

[Source/Hodgepodge/Hodgepodge.cpp](../../../Source/Hodgepodge/Hodgepodge.cpp)

模块定义或基础代码；请查看对应文件。

## Hodgepodge.h

[Source/Hodgepodge/Hodgepodge.h](../../../Source/Hodgepodge/Hodgepodge.h)

模块定义或基础代码；请查看对应文件。

## HodgeAbilityTask_PlayTimeline.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.cpp)

驱动 HodgeAbilityTimeline 的唯一 AbilityTask：初始化与 Tick 共用 CollectNodes + SortNodes 统一 Scheduler，推进逻辑时间，维护 WindowTag 与 GE 两个账本，派发 Point 与系统事件。窗口 GE 施加/移除、Point 与 Timeline.End 派发、中途取消清理已实测通过；重入类时序、NetPolicy 跨端、时钟倒退未验证。

- `UHodgeAbilityTask_PlayTimeline::UHodgeAbilityTask_PlayTimeline` — L29
- `UHodgeAbilityTask_PlayTimeline::PlayTimeline` — L36
- `UHodgeAbilityTask_PlayTimeline::Activate` — L49
- `UHodgeAbilityTask_PlayTimeline::TickTask` — L109
- `UHodgeAbilityTask_PlayTimeline::CollectNodes` — L168
- `UHodgeAbilityTask_PlayTimeline::SortNodes` — L176
- `UHodgeAbilityTask_PlayTimeline::InitializeTimeline` — L181
- `UHodgeAbilityTask_PlayTimeline::AdvanceTimeline` — L217
- `UHodgeAbilityTask_PlayTimeline::HasAuthorityOnAvatar` — L268
- `UHodgeAbilityTask_PlayTimeline::EnterWindow` — L274
- `UHodgeAbilityTask_PlayTimeline::ExitWindow` — L334
- `UHodgeAbilityTask_PlayTimeline::ApplyTimelineEffect` — L383
- `UHodgeAbilityTask_PlayTimeline::FirePointEvent` — L407
- `UHodgeAbilityTask_PlayTimeline::FireSystemEvent` — L471
- `UHodgeAbilityTask_PlayTimeline::ClearAllWindowState` — L484
- `UHodgeAbilityTask_PlayTimeline::StopTimeline` — L527
- `UHodgeAbilityTask_PlayTimeline::OnDestroy` — L560
- `UHodgeAbilityTask_PlayTimeline::PlayMontageTimeline` — L572
- `UHodgeAbilityTask_PlayTimeline::GetActiveWindowTags` — L583
- `UHodgeAbilityTask_PlayTimeline::RefreshMontageClock` — L604

## HodgeAbilityTask_WaitMoveCancel.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.cpp)

把“取消窗口（Timeline 授权）”与“移动意图（输入层）”两个变化驱动信号合流的 AbilityTask：同时成立时广播 OnMoveCancel 一次后自结束。本地控制端语义；AI/模拟代理找不到 HeroComponent 时永不成立。工作区新增，未编译、未 PIE。

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

- `UHodgeGameplayAbility::UHodgeGameplayAbility` — L36
- `UHodgeGameplayAbility::GetHodgeAbilitySystemComponentFromActorInfo` — L65
- `UHodgeGameplayAbility::GetHodgePlayerControllerFromActorInfo` — L74
- `UHodgeGameplayAbility::GetControllerFromActorInfo` — L81
- `UHodgeGameplayAbility::GetHodgeCharacterFromActorInfo` — L119
- `UHodgeGameplayAbility::GetHeroComponentFromActorInfo` — L125
- `UHodgeGameplayAbility::NativeOnAbilityFailedToActivate` — L132
- `UHodgeGameplayAbility::CanActivateAbility` — L185
- `UHodgeGameplayAbility::SetCanBeCanceled` — L226
- `UHodgeGameplayAbility::OnGiveAbility` — L245
- `UHodgeGameplayAbility::OnRemoveAbility` — L258
- `UHodgeGameplayAbility::ActivateAbility` — L269
- `UHodgeGameplayAbility::EndAbility` — L279
- `UHodgeGameplayAbility::CheckCost` — L292
- `UHodgeGameplayAbility::ApplyCost` — L322
- `UHodgeGameplayAbility::MakeEffectContext` — L405
- `UHodgeGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec` — L452
- `UHodgeGameplayAbility::DoesAbilitySatisfyTagRequirements` — L471
- `UHodgeGameplayAbility::OnPawnAvatarSet` — L627
- `UHodgeGameplayAbility::GetAbilitySource` — L634
- `UHodgeGameplayAbility::TryActivateAbilityOnSpawn` — L660
- `UHodgeGameplayAbility::CanChangeActivationGroup` — L702
- `UHodgeGameplayAbility::ChangeActivationGroup` — L740
- `UHodgeGameplayAbility::SetCameraMode` — L773
- `UHodgeGameplayAbility::ClearCameraMode` — L787

## HodgeGameplayAbility_BasicAttack.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_BasicAttack.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_BasicAttack.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeGameplayAbility_BasicAttack::UHodgeGameplayAbility_BasicAttack` — L16
- `UHodgeGameplayAbility_BasicAttack::ActivateAbility` — L25
- `UHodgeGameplayAbility_BasicAttack::StartStep` — L64
- `UHodgeGameplayAbility_BasicAttack::OnAttackPressed` — L90
- `UHodgeGameplayAbility_BasicAttack::OnComboWindowChanged` — L96
- `UHodgeGameplayAbility_BasicAttack::TryAdvance` — L101
- `UHodgeGameplayAbility_BasicAttack::OnTimelineEnded` — L116
- `UHodgeGameplayAbility_BasicAttack::OnCompleted` — L121
- `UHodgeGameplayAbility_BasicAttack::OnInterrupted` — L129
- `UHodgeGameplayAbility_BasicAttack::ClearStep` — L137
- `UHodgeGameplayAbility_BasicAttack::EndAbility` — L168

## HodgeGameplayAbility_Definition.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeGameplayAbility_Definition::UHodgeGameplayAbility_Definition` — L10
- `UHodgeGameplayAbility_Definition::GetDefinition` — L19
- `UHodgeGameplayAbility_Definition::ActivateConfirmedDefinition` — L25
- `UHodgeGameplayAbility_Definition::CanActivateAbility` — L35
- `UHodgeGameplayAbility_Definition::ActivateAbility` — L45
- `UHodgeGameplayAbility_Definition::FinishExecution` — L75
- `UHodgeGameplayAbility_Definition::EndAbility` — L80
- `UHodgeGameplayAbility_Definition::GetExecutionWindows` — L102
- `UHodgeGameplayAbility_Definition::RefreshExecutionClock` — L106
- `UHodgeGameplayAbility_Definition::OnTimelineFinished` — L110
- `UHodgeGameplayAbility_Definition::OnWindowsChanged` — L116
- `UHodgeGameplayAbility_Definition::OnPoint` — L120

## HodgeAttributeSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp)

项目 AttributeSet 基础和 ASC 访问。

- `UHodgeAttributeSet::UHodgeAttributeSet` — L23
- `UHodgeAttributeSet::GetWorld` — L28
- `UHodgeAttributeSet::GetHodgeAbilitySystemComponent` — L41

## HodgeCombatSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeCombatSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeCombatSet.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeCombatSet::UHodgeCombatSet` — L11
- `UHodgeCombatSet::GetLifetimeReplicatedProps` — L20
- `UHodgeCombatSet::OnRep_BaseDamage` — L33
- `UHodgeCombatSet::OnRep_BaseHeal` — L40

## HodgeHealthSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp)

Health/MaxHealth、BaseDamage/BaseHeal 和 Damage/Healing 元属性；有效结算、夹取、免疫和耗尽广播。

- `UHodgeHealthSet::UHodgeHealthSet` — L33
- `UHodgeHealthSet::GetLifetimeReplicatedProps` — L50
- `UHodgeHealthSet::OnRep_Health` — L63
- `UHodgeHealthSet::OnRep_MaxHealth` — L99
- `UHodgeHealthSet::PreGameplayEffectExecute` — L116
- `UHodgeHealthSet::PostGameplayEffectExecute` — L176
- `UHodgeHealthSet::PreAttributeBaseChange` — L298
- `UHodgeHealthSet::PreAttributeChange` — L308
- `UHodgeHealthSet::PostAttributeChange` — L318
- `UHodgeHealthSet::ClampAttribute` — L352

## HodgeDamageExecution.cpp

[Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeDamageExecution.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeDamageExecution.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeDamageExecution::UHodgeDamageExecution` — L47
- `UHodgeDamageExecution::Execute_Implementation` — L54

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

Tag 输入缓存、激活组、关系映射、全局注册、失败通知与动态 Tag GE。

- `UHodgeAbilitySystemComponent::UHodgeAbilitySystemComponent` — L32
- `UHodgeAbilitySystemComponent::EndPlay` — L48
- `UHodgeAbilitySystemComponent::InitAbilityActorInfo` — L60
- `UHodgeAbilitySystemComponent::TryActivateAbilitiesOnSpawn` — L136
- `UHodgeAbilitySystemComponent::CancelAbilitiesByFunc` — L153
- `UHodgeAbilitySystemComponent::CancelInputActivatedAbilities` — L221
- `UHodgeAbilitySystemComponent::AbilitySpecInputPressed` — L238
- `UHodgeAbilitySystemComponent::AbilitySpecInputReleased` — L265
- `UHodgeAbilitySystemComponent::AbilityInputTagPressed` — L292
- `UHodgeAbilitySystemComponent::AbilityInputTagReleased` — L317
- `UHodgeAbilitySystemComponent::ProcessAbilityInput` — L338
- `UHodgeAbilitySystemComponent::ClearAbilityInput` — L468
- `UHodgeAbilitySystemComponent::NotifyAbilityActivated` — L481
- `UHodgeAbilitySystemComponent::NotifyAbilityFailed` — L495
- `UHodgeAbilitySystemComponent::NotifyAbilityEnded` — L517
- `UHodgeAbilitySystemComponent::ApplyAbilityBlockAndCancelTags` — L531
- `UHodgeAbilitySystemComponent::HandleChangeAbilityCanBeCanceled` — L560
- `UHodgeAbilitySystemComponent::GetAdditionalActivationTagRequirements` — L571
- `UHodgeAbilitySystemComponent::SetTagRelationshipMapping` — L584
- `UHodgeAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation` — L590
- `UHodgeAbilitySystemComponent::HandleAbilityFailed` — L597
- `UHodgeAbilitySystemComponent::IsActivationGroupBlocked` — L610
- `UHodgeAbilitySystemComponent::AddAbilityToActivationGroup` — L641
- `UHodgeAbilitySystemComponent::RemoveAbilityFromActivationGroup` — L689
- `UHodgeAbilitySystemComponent::CancelActivationGroupAbilities` — L702
- `UHodgeAbilitySystemComponent::AddDynamicTagGameplayEffect` — L718
- `UHodgeAbilitySystemComponent::RemoveDynamicTagGameplayEffect` — L754
- `UHodgeAbilitySystemComponent::GetAbilityTargetData` — L779
- `UHodgeAbilitySystemComponent::GetLifetimeReplicatedProps` — L794
- `UHodgeAbilitySystemComponent::GiveAbilityDefinition` — L801
- `UHodgeAbilitySystemComponent::FindAbilityDefinition` — L829
- `UHodgeAbilitySystemComponent::FindDefinitionAbility` — L839
- `UHodgeAbilitySystemComponent::OnRemoveAbility` — L854
- `UHodgeAbilitySystemComponent::InternalServerTryActivateAbility` — L860
- `UHodgeAbilitySystemComponent::ClientActivateAbilitySucceedWithEventData_Implementation` — L885

## HodgeAbilitySystemComponent_Montage.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeAbilitySystemComponent::PlayMontage` — L6
- `UHodgeAbilitySystemComponent::PlayMontageSimulated` — L21
- `UHodgeAbilitySystemComponent::ApplyDefinitionMontageSettings` — L28
- `UHodgeAbilitySystemComponent::OnRep_DefinitionMontage` — L42
- `UHodgeAbilitySystemComponent::OnRep_ReplicatedAnimMontage` — L51
- `UHodgeAbilitySystemComponent::StopDefinitionMontage` — L57
- `UHodgeAbilitySystemComponent::CurrentMontageStop` — L64

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

## HodgeTimelineEvaluator.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeTimelineEvaluator.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeTimelineEvaluator.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeTimelineEvaluator::WindowEnd` — L3
- `FHodgeTimelineEvaluator::Collect` — L9
- `FHodgeTimelineEvaluator::Sort` — L41
- `FHodgeTimelineEvaluator::EvaluateRange` — L53
- `FHodgeTimelineEvaluator::EvaluateAt` — L60

## HodgeActorBase.cpp

[Source/Hodgepodge/Private/Actor/HodgeActorBase.cpp](../../../Source/Hodgepodge/Private/Actor/HodgeActorBase.cpp)

项目 Actor 基类扩展。

- `AHodgeActorBase::AHodgeActorBase` — L21
- `AHodgeActorBase::BeginPlay` — L34
- `AHodgeActorBase::Tick` — L45

## HodgeAnimInstance.cpp

[Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp)

ASC GameplayTag 属性映射和 GroundDistance 更新。

- `UHodgeAnimInstance::UHodgeAnimInstance` — L13
- `UHodgeAnimInstance::InitializeWithAbilitySystem` — L18
- `UHodgeAnimInstance::IsDataValid` — L29
- `UHodgeAnimInstance::NativeInitializeAnimation` — L43
- `UHodgeAnimInstance::NativeUpdateAnimation` — L61

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

- `AHodgeCharacterBase::AHodgeCharacterBase` — L29
- `AHodgeCharacterBase::PreInitializeComponents` — L41
- `AHodgeCharacterBase::EndPlay` — L55
- `AHodgeCharacterBase::BeginPlay` — L68

## HodgeCombatCharacter.cpp

[Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp)

PawnExtension、相机、ASC 查询、移动标签、复制与死亡占位逻辑。

- `AHodgeCombatCharacter::AHodgeCombatCharacter` — L37
- `AHodgeCombatCharacter::PreInitializeComponents` — L154
- `AHodgeCombatCharacter::BeginPlay` — L161
- `AHodgeCombatCharacter::EndPlay` — L184
- `AHodgeCombatCharacter::Reset` — L207
- `AHodgeCombatCharacter::GetLifetimeReplicatedProps` — L220
- `AHodgeCombatCharacter::PreReplication` — L232
- `AHodgeCombatCharacter::NotifyControllerChanged` — L259
- `AHodgeCombatCharacter::GetHodgePlayerController` — L279
- `AHodgeCombatCharacter::GetHodgePlayerState` — L286
- `AHodgeCombatCharacter::GetHodgeAbilitySystemComponent` — L293
- `AHodgeCombatCharacter::GetAbilitySystemComponent` — L300
- `AHodgeCombatCharacter::OnAbilitySystemInitialized` — L312
- `AHodgeCombatCharacter::OnAbilitySystemUninitialized` — L326
- `AHodgeCombatCharacter::PossessedBy` — L333
- `AHodgeCombatCharacter::UnPossessed` — L359
- `AHodgeCombatCharacter::OnRep_Controller` — L385
- `AHodgeCombatCharacter::OnRep_PlayerState` — L394
- `AHodgeCombatCharacter::SetupPlayerInputComponent` — L403
- `AHodgeCombatCharacter::InitializeGameplayTags` — L414
- `AHodgeCombatCharacter::GetOwnedGameplayTags` — L449
- `AHodgeCombatCharacter::HasMatchingGameplayTag` — L459
- `AHodgeCombatCharacter::HasAllMatchingGameplayTags` — L471
- `AHodgeCombatCharacter::HasAnyMatchingGameplayTags` — L483
- `AHodgeCombatCharacter::FellOutOfWorld` — L495
- `AHodgeCombatCharacter::OnDeathStarted` — L502
- `AHodgeCombatCharacter::OnDeathFinished` — L509
- `AHodgeCombatCharacter::DisableMovementAndCollision` — L516
- `AHodgeCombatCharacter::DestroyDueToDeath` — L546
- `AHodgeCombatCharacter::UninitAndDestroy` — L556
- `AHodgeCombatCharacter::OnMovementModeChanged` — L584
- `AHodgeCombatCharacter::SetMovementModeTag` — L600
- `AHodgeCombatCharacter::ToggleCrouch` — L629
- `AHodgeCombatCharacter::OnStartCrouch` — L648
- `AHodgeCombatCharacter::OnEndCrouch` — L662
- `AHodgeCombatCharacter::CanJumpInternal_Implementation` — L676
- `AHodgeCombatCharacter::OnRep_ReplicatedAcceleration` — L683
- `AHodgeCombatCharacter::OnControllerChangedTeam` — L713
- `AHodgeCombatCharacter::OnRep_MyTeamID` — L726
- `AHodgeCombatCharacter::UpdateSharedReplication` — L733
- `AHodgeCombatCharacter::FastSharedReplication_Implementation` — L766
- `FSharedRepMovement::FSharedRepMovement` — L808
- `FSharedRepMovement::FillForCharacter` — L815
- `FSharedRepMovement::Equals` — L862
- `FSharedRepMovement::NetSerialize` — L905

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

CharacterMovement 扩展、地面距离、加速度和能力系统相关移动入口。

- `UHodgeCharacterMovementComponent::UHodgeCharacterMovementComponent` — L27
- `UHodgeCharacterMovementComponent::SimulateMovement` — L33
- `UHodgeCharacterMovementComponent::CanAttemptJump` — L54
- `UHodgeCharacterMovementComponent::InitializeComponent` — L63
- `UHodgeCharacterMovementComponent::GetGroundInfo` — L70
- `UHodgeCharacterMovementComponent::SetReplicatedAcceleration` — L151
- `UHodgeCharacterMovementComponent::GetDeltaRotation` — L161
- `UHodgeCharacterMovementComponent::GetMaxSpeed` — L179

## HodgeCombatComponentBase.cpp

[Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp)

战斗组件占位，当前构造关闭 Tick，尚无完整命中或连招实现。

- `UHodgeCombatComponentBase::UHodgeCombatComponentBase` — L23

## HodgeComboComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeComboComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeComboComponent.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeComboComponent::UHodgeComboComponent` — L14
- `UHodgeComboComponent::Configure` — L20
- `UHodgeComboComponent::Shutdown` — L31
- `UHodgeComboComponent::ClearInput` — L40
- `UHodgeComboComponent::InputPressed` — L46
- `UHodgeComboComponent::SelectTransition` — L65
- `UHodgeComboComponent::IsAuthorized` — L82
- `UHodgeComboComponent::ExecutionKey` — L87
- `UHodgeComboComponent::PrepareTransition` — L94
- `UHodgeComboComponent::TryTransition` — L116
- `UHodgeComboComponent::PrepareServerActivation` — L153
- `UHodgeComboComponent::PrepareConfirmedActivation` — L174
- `UHodgeComboComponent::RejectServerActivation` — L194
- `UHodgeComboComponent::CompleteServerActivation` — L202
- `UHodgeComboComponent::ExecutionStarted` — L211
- `UHodgeComboComponent::ExecutionEnded` — L218
- `UHodgeComboComponent::WindowsChanged` — L225
- `UHodgeComboComponent::TimelineEvent` — L234
- `UHodgeComboComponent::DrainEvents` — L241
- `UHodgeComboComponent::SetNode` — L259
- `UHodgeComboComponent::ResetSession` — L273
- `UHodgeComboComponent::TickComponent` — L284
- `UHodgeComboComponent::ServerMoveCancel_Implementation` — L310
- `UHodgeComboComponent::ClientMoveCancelResult_Implementation` — L322
- `UHodgeComboComponent::ServerReturnToEntry_Implementation` — L324
- `UHodgeComboComponent::GetLifetimeReplicatedProps` — L336
- `UHodgeComboComponent::OnRep_ObserverTags` — L342
- `UHodgeComboComponent::EndPlay` — L351

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
- `UHodgeHealthComponent::UninitializeFromAbilitySystem` — L142
- `UHodgeHealthComponent::ClearGameplayTags` — L168
- `UHodgeHealthComponent::GetHealth` — L182
- `UHodgeHealthComponent::GetMaxHealth` — L189
- `UHodgeHealthComponent::GetHealthNormalized` — L196
- `UHodgeHealthComponent::HandleHealthChanged` — L216
- `UHodgeHealthComponent::HandleMaxHealthChanged` — L225
- `UHodgeHealthComponent::HandleOutOfHealth` — L234
- `UHodgeHealthComponent::OnRep_DeathState` — L308
- `UHodgeHealthComponent::StartDeath` — L380
- `UHodgeHealthComponent::FinishDeath` — L412
- `UHodgeHealthComponent::DamageSelfDestruct` — L444

## HodgeHeroComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp)

玩家 Init State 协调、ASC 接入、输入与相机；额外输入句柄持久化并在移除/EndPlay 解绑。新增“移动意图”信号（HasMoveIntent / GetMoveIntent / OnMoveIntentChanged），记 Input_Move 原始输入量、Completed/Canceled 清零，供移动取消后摇消费。

- `UHodgeHeroComponent::NAME_BindInputsNow` — L81
- `UHodgeHeroComponent::NAME_ActorFeatureName` — L84
- `UHodgeHeroComponent::UHodgeHeroComponent` — L87
- `UHodgeHeroComponent::OnRegister` — L98
- `UHodgeHeroComponent::CanChangeInitState` — L142
- `UHodgeHeroComponent::HandleChangeInitState` — L262
- `UHodgeHeroComponent::OnActorInitStateChanged` — L329
- `UHodgeHeroComponent::CheckDefaultInitialization` — L345
- `UHodgeHeroComponent::BeginPlay` — L365
- `UHodgeHeroComponent::EndPlay` — L389
- `UHodgeHeroComponent::InitializePlayerInput` — L413
- `UHodgeHeroComponent::AddAdditionalInputConfig` — L639
- `UHodgeHeroComponent::RemoveAdditionalInputConfig` — L730
- `UHodgeHeroComponent::IsReadyToBindInputs` — L759
- `UHodgeHeroComponent::Input_AbilityInputTagPressed` — L766
- `UHodgeHeroComponent::Input_AbilityInputTagReleased` — L789
- `UHodgeHeroComponent::Input_Move` — L817
- `UHodgeHeroComponent::Input_MoveStopped` — L890
- `UHodgeHeroComponent::HasMoveIntent` — L898
- `UHodgeHeroComponent::SetMoveIntent` — L905
- `UHodgeHeroComponent::RefreshMoveIntent` — L912
- `UHodgeHeroComponent::Input_LookMouse` — L926
- `UHodgeHeroComponent::Input_LookStick` — L965
- `UHodgeHeroComponent::Input_Crouch` — L1012
- `UHodgeHeroComponent::Input_AutoRun` — L1025
- `UHodgeHeroComponent::DetermineCameraMode` — L1045
- `UHodgeHeroComponent::SetAbilityCameraMode` — L1078
- `UHodgeHeroComponent::ClearAbilityCameraMode` — L1093

## HodgeInteractionComponentBase.cpp

[Source/Hodgepodge/Private/Component/HodgeInteractionComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeInteractionComponentBase.cpp)

交互组件基础占位；完整扫描、交互规则与 UI 需另行实现。

## HodgeMovementComponentBase.cpp

[Source/Hodgepodge/Private/Component/HodgeMovementComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeMovementComponentBase.cpp)

通用移动组件基础占位，与 CharacterMovement 派生类需区分。

## HodgePawnExtensionComponent.cpp

[Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp)

PawnData 复制、Init State、ASC 关联/解除、TagRelationshipMapping 与 ClearAbilityInput；需验证退出顺序。

- `UHodgePawnExtensionComponent::NAME_ActorFeatureName` — L19
- `UHodgePawnExtensionComponent::UHodgePawnExtensionComponent` — L21
- `UHodgePawnExtensionComponent::GetLifetimeReplicatedProps` — L35
- `UHodgePawnExtensionComponent::OnRegister` — L43
- `UHodgePawnExtensionComponent::BeginPlay` — L62
- `UHodgePawnExtensionComponent::EndPlay` — L76
- `UHodgePawnExtensionComponent::SetPawnData` — L87
- `UHodgePawnExtensionComponent::OnRep_PawnData` — L117
- `UHodgePawnExtensionComponent::InitializeAbilitySystem` — L123
- `UHodgePawnExtensionComponent::UninitializeAbilitySystem` — L187
- `UHodgePawnExtensionComponent::HandleControllerChanged` — L236
- `UHodgePawnExtensionComponent::HandlePlayerStateReplicated` — L260
- `UHodgePawnExtensionComponent::SetupPlayerInputComponent` — L266
- `UHodgePawnExtensionComponent::CheckDefaultInitialization` — L272
- `UHodgePawnExtensionComponent::CanChangeInitState` — L287
- `UHodgePawnExtensionComponent::HandleChangeInitState` — L347
- `UHodgePawnExtensionComponent::OnActorInitStateChanged` — L357
- `UHodgePawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall` — L369
- `UHodgePawnExtensionComponent::OnAbilitySystemUninitialized_Register` — L385

## HodgeGameInstanceBase.cpp

[Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp](../../../Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp)

注册 Init State 顺序、主控制器访问和全局生命周期扩展。

- `UHodgeGameInstanceBase::UHodgeGameInstanceBase` — L17
- `UHodgeGameInstanceBase::GetPrimaryPlayerController` — L22
- `UHodgeGameInstanceBase::Shutdown` — L28
- `UHodgeGameInstanceBase::Init` — L34

## HodgeGameModeBase.cpp

[Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp](../../../Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp)

服务器选择玩法、等待加载、生成 Pawn，并在 FinishSpawning 前注入 PawnData；选用 HodgePlayerController。

- `AHodgeGameModeBase::AHodgeGameModeBase` — L21
- `AHodgeGameModeBase::GetPawnDataForController` — L46
- `AHodgeGameModeBase::InitGame` — L87
- `AHodgeGameModeBase::HandleMatchAssignmentIfNotExpectingOne` — L96
- `AHodgeGameModeBase::TryDedicatedServerLogin` — L193
- `AHodgeGameModeBase::OnMatchAssignmentGiven` — L337
- `AHodgeGameModeBase::OnExperienceLoaded` — L361
- `AHodgeGameModeBase::IsExperienceLoaded` — L382
- `AHodgeGameModeBase::GetDefaultPawnClassForController_Implementation` — L396
- `AHodgeGameModeBase::SpawnDefaultPawnAtTransform_Implementation` — L412
- `AHodgeGameModeBase::ShouldSpawnAtStartSpot` — L469
- `AHodgeGameModeBase::HandleStartingNewPlayer_Implementation` — L475
- `AHodgeGameModeBase::ChoosePlayerStart_Implementation` — L485
- `AHodgeGameModeBase::FinishRestartPlayer` — L498
- `AHodgeGameModeBase::PlayerCanRestart_Implementation` — L511
- `AHodgeGameModeBase::ControllerCanRestart` — L517
- `AHodgeGameModeBase::InitGameState` — L546
- `AHodgeGameModeBase::GenericPlayerInitialization` — L561
- `AHodgeGameModeBase::RequestPlayerRestartNextFrame` — L570
- `AHodgeGameModeBase::UpdatePlayerStartSpot` — L591
- `AHodgeGameModeBase::FailedToRestartPlayer` — L597

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

项目 HUD 类：GameFrameworkComponent 接收器注册与 GAS 调试 Actor 列表；不代表 CommonUI 已完成。

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

- `AHodgePlayerController::AHodgePlayerController` — L96
- `AHodgePlayerController::PreInitializeComponents` — L109
- `AHodgePlayerController::BeginPlay` — L116
- `AHodgePlayerController::EndPlay` — L141
- `AHodgePlayerController::GetLifetimeReplicatedProps` — L148
- `AHodgePlayerController::ReceivedPlayer` — L171
- `AHodgePlayerController::PlayerTick` — L178
- `AHodgePlayerController::GetHodgePlayerState` — L243
- `AHodgePlayerController::GetHodgeAbilitySystemComponent` — L250
- `AHodgePlayerController::GetHodgeHUD` — L260
- `AHodgePlayerController::TryToRecordClientReplay` — L267
- `AHodgePlayerController::ShouldRecordClientReplay` — L296
- `AHodgePlayerController::OnPlayerStateChangedTeam` — L363
- `AHodgePlayerController::OnPlayerStateChanged` — L370
- `AHodgePlayerController::BroadcastOnPlayerStateChanged` — L377
- `AHodgePlayerController::InitPlayerState` — L422
- `AHodgePlayerController::CleanupPlayerState` — L432
- `AHodgePlayerController::OnRep_PlayerState` — L442
- `AHodgePlayerController::SetPlayer` — L452
- `AHodgePlayerController::AddCheats` — L475
- `AHodgePlayerController::ServerCheat_Implementation` — L486
- `AHodgePlayerController::ServerCheat_Validate` — L499
- `AHodgePlayerController::ServerCheatAll_Implementation` — L506
- `AHodgePlayerController::ServerCheatAll_Validate` — L526
- `AHodgePlayerController::PreProcessInput` — L533
- `AHodgePlayerController::PostProcessInput` — L540
- `AHodgePlayerController::OnCameraPenetratingTarget` — L554
- `AHodgePlayerController::OnPossess` — L561
- `AHodgePlayerController::SetIsAutoRunning` — L586
- `AHodgePlayerController::GetIsAutoRunning` — L609
- `AHodgePlayerController::OnStartAutoRun` — L626
- `AHodgePlayerController::OnEndAutoRun` — L640
- `AHodgePlayerController::UpdateForceFeedback` — L654
- `AHodgePlayerController::UpdateHiddenComponents` — L677
- `AHodgePlayerController::OnUnPossess` — L787
- `AHodgeReplayPlayerController::Tick` — L816
- `AHodgeReplayPlayerController::SmoothTargetViewRotation` — L854
- `AHodgeReplayPlayerController::ShouldRecordClientReplay` — L864
- `AHodgeReplayPlayerController::RecorderPlayerStateUpdated` — L871
- `AHodgeReplayPlayerController::OnPlayerStatePawnSet` — L891

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

- `AHodgePlayerState::NAME_HodgeAbilityReady` — L35
- `AHodgePlayerState::AHodgePlayerState` — L38
- `AHodgePlayerState::GetHodgePlayerController` — L68
- `AHodgePlayerState::GetAbilitySystemComponent` — L75
- `AHodgePlayerState::SetPawnData` — L82
- `AHodgePlayerState::PreInitializeComponents` — L127
- `AHodgePlayerState::PostInitializeComponents` — L165
- `AHodgePlayerState::Reset` — L172
- `AHodgePlayerState::ClientInitialize` — L179
- `AHodgePlayerState::CopyProperties` — L192
- `AHodgePlayerState::GetLifetimeReplicatedProps` — L202
- `AHodgePlayerState::OnDeactivated` — L236
- `AHodgePlayerState::OnReactivated` — L272
- `AHodgePlayerState::SetPlayerConnectionType` — L286
- `AHodgePlayerState::SetSquadID` — L296
- `AHodgePlayerState::AddStatTagStack` — L310
- `AHodgePlayerState::RemoveStatTagStack` — L317
- `AHodgePlayerState::GetStatTagStackCount` — L324
- `AHodgePlayerState::HasStatTag` — L331
- `AHodgePlayerState::GetReplicatedViewRotation` — L338
- `AHodgePlayerState::SetReplicatedViewRotation` — L345
- `AHodgePlayerState::OnExperienceLoaded` — L359
- `AHodgePlayerState::OnRep_PawnData` — L382
- `AHodgePlayerState::OnRep_MyTeamID` — L388
- `AHodgePlayerState::OnRep_MySquadID` — L395

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

模块定义或基础代码；请查看对应文件。

- `FHodgeAbilityBlendSettings::MakeBlend` — L11
- `FHodgeAbilityBlendSettings::IsValid` — L19
- `UHodgeAbilityDefinition::GetDuration` — L25
- `UHodgeAbilityDefinition::ValidateDefinition` — L29
- `UHodgeAbilityDefinition::IsDataValid` — L65

## HodgeAbilitySet.cpp

[Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp)

权威端批量授予属性集、技能和 GE，并用句柄集合撤销。

- `FHodgeAbilitySet_GrantedHandles::AddAbilitySpecHandle` — L11
- `FHodgeAbilitySet_GrantedHandles::AddGameplayEffectHandle` — L20
- `FHodgeAbilitySet_GrantedHandles::AddAttributeSet` — L29
- `FHodgeAbilitySet_GrantedHandles::TakeFromAbilitySystem` — L35
- `UHodgeAbilitySet::UHodgeAbilitySet` — L76
- `UHodgeAbilitySet::GiveToAbilitySystem` — L82

## HodgeAbilityTimeline.cpp

[Source/Hodgepodge/Private/Data/HodgeAbilityTimeline.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAbilityTimeline.cpp)

技能逻辑时间轴数据资产（统一事件模型：单一 Events[]，Kind = Window / Point）。只描述“何时发生什么”，不含业务判断；没有 Montage 字段、不做 Bundle 收集。

- `UHodgeAbilityTimeline::ValidateForPlayback` — L27
- `UHodgeAbilityTimeline::ValidateEntries` — L38
- `UHodgeAbilityTimeline::IsDataValid` — L240
- `UHodgeAbilityTimeline::PostEditChangeProperty` — L350

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

## HodgeComboDefinition.cpp

[Source/Hodgepodge/Private/Data/HodgeComboDefinition.cpp](../../../Source/Hodgepodge/Private/Data/HodgeComboDefinition.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeComboDefinition::FindNode` — L7
- `UHodgeComboDefinition::ValidateDefinition` — L12
- `UHodgeComboDefinition::IsDataValid` — L49

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

PawnClass、AbilitySets、TagRelationshipMapping、InputConfig、DefaultCameraMode 配置。

- `UHodgePawnData::UHodgePawnData` — L8

## HodgePawnData_Validation.cpp

[Source/Hodgepodge/Private/Data/HodgePawnData.cpp](../../../Source/Hodgepodge/Private/Data/HodgePawnData.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgePawnData::IsDataValid` — L10

## HodgeEquipmentDefinition.cpp

[Source/Hodgepodge/Private/Equipment/HodgeEquipmentDefinition.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentDefinition.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeEquipmentDefinition::UHodgeEquipmentDefinition` — L11

## HodgeEquipmentInstance.cpp

[Source/Hodgepodge/Private/Equipment/HodgeEquipmentInstance.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentInstance.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeEquipmentInstance::UHodgeEquipmentInstance` — L31
- `UHodgeEquipmentInstance::GetWorld` — L37
- `UHodgeEquipmentInstance::GetLifetimeReplicatedProps` — L53
- `UHodgeEquipmentInstance::RegisterReplicationFragments` — L68
- `UHodgeEquipmentInstance::GetPawn` — L82
- `UHodgeEquipmentInstance::GetTypedPawn` — L88
- `UHodgeEquipmentInstance::SpawnEquipmentActors` — L109
- `UHodgeEquipmentInstance::DestroyEquipmentActors` — L147
- `UHodgeEquipmentInstance::OnEquipped` — L161
- `UHodgeEquipmentInstance::OnUnequipped` — L168
- `UHodgeEquipmentInstance::OnRep_Instigator` — L175

## HodgeEquipmentManagerComponent.cpp

[Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeAppliedEquipmentEntry::GetDebugString` — L32
- `FHodgeEquipmentList::PreReplicatedRemove` — L41
- `FHodgeEquipmentList::PostReplicatedAdd` — L58
- `FHodgeEquipmentList::PostReplicatedChange` — L75
- `FHodgeEquipmentList::GetAbilitySystemComponent` — L85
- `FHodgeEquipmentList::AddEntry` — L98
- `FHodgeEquipmentList::RemoveEntry` — L165
- `UHodgeEquipmentManagerComponent::UHodgeEquipmentManagerComponent` — L200
- `UHodgeEquipmentManagerComponent::GetLifetimeReplicatedProps` — L212
- `UHodgeEquipmentManagerComponent::EquipItem` — L222
- `UHodgeEquipmentManagerComponent::UnequipItem` — L253
- `UHodgeEquipmentManagerComponent::ReplicateSubobjects` — L273
- `UHodgeEquipmentManagerComponent::InitializeComponent` — L298
- `UHodgeEquipmentManagerComponent::UninitializeComponent` — L305
- `UHodgeEquipmentManagerComponent::ReadyForReplication` — L329
- `UHodgeEquipmentManagerComponent::GetFirstInstanceOfType` — L356
- `UHodgeEquipmentManagerComponent::GetEquipmentInstancesOfType` — L379

## HodgeWeaponInstance.cpp

[Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeWeaponInstance::UHodgeWeaponInstance` — L32
- `UHodgeWeaponInstance::OnEquipped` — L57
- `UHodgeWeaponInstance::OnUnequipped` — L76
- `UHodgeWeaponInstance::UpdateFiringTime` — L86
- `UHodgeWeaponInstance::GetTimeSinceLastInteractedWith` — L99
- `UHodgeWeaponInstance::PickBestAnimLayer` — L128
- `UHodgeWeaponInstance::GetOwningUserId` — L139
- `UHodgeWeaponInstance::ApplyDeviceProperties` — L153
- `UHodgeWeaponInstance::RemoveDeviceProperties` — L191
- `UHodgeWeaponInstance::OnDeathStarted` — L214

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

- `UGameFeatureAction_AddWidgets::OnGameFeatureDeactivating` — L46
- `UGameFeatureAction_AddWidgets::AddAdditionalAssetBundleData` — L65
- `UGameFeatureAction_AddWidgets::IsDataValid` — L80
- `UGameFeatureAction_AddWidgets::AddToWorld` — L155
- `UGameFeatureAction_AddWidgets::Reset` — L189
- `UGameFeatureAction_AddWidgets::HandleActorExtension` — L211
- `UGameFeatureAction_AddWidgets::AddWidgets` — L230
- `UGameFeatureAction_AddWidgets::RemoveWidgets` — L279

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
- `FHodgeComboSessionTest::RunTest` — L95
- `FHodgeMontageDurationTest::RunTest` — L177

## HodgeAbilityTimelineTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeAbilityTimelineTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAbilityTimelineTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeTimelineValidationTest::RunTest` — L77
- `FHodgeTimelineRejectTest::RunTest` — L115
- `FHodgeTimelineCleanupTest::RunTest` — L157

## HodgeTimelineEvaluatorTests.cpp

[Source/Hodgepodge/Private/Tests/HodgeTimelineEvaluatorTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeTimelineEvaluatorTests.cpp)

模块定义或基础代码；请查看对应文件。

- `FHodgeEvaluatorTest::RunTest` — L8

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

## UIExtensionPointWidget.cpp

[Source/Hodgepodge/Private/UI/Extension/UIExtensionPointWidget.cpp](../../../Source/Hodgepodge/Private/UI/Extension/UIExtensionPointWidget.cpp)

模块定义或基础代码；请查看对应文件。

- `UUIExtensionPointWidget::UUIExtensionPointWidget` — L39
- `UUIExtensionPointWidget::ReleaseSlateResources` — L45
- `UUIExtensionPointWidget::RebuildWidget` — L56
- `UUIExtensionPointWidget::ResetExtensionPoint` — L123
- `UUIExtensionPointWidget::RegisterExtensionPoint` — L142
- `UUIExtensionPointWidget::RegisterExtensionPointForPlayerState` — L186
- `UUIExtensionPointWidget::OnAddOrRemoveExtension` — L214
- `UUIExtensionPointWidget::ValidateCompiledDefaults` — L297

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

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

## HodgeHUDLayout.cpp

[Source/Hodgepodge/Private/UI/HodgeHUDLayout.cpp](../../../Source/Hodgepodge/Private/UI/HodgeHUDLayout.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeHUDLayout::UHodgeHUDLayout` — L77
- `UHodgeHUDLayout::NativeOnInitialized` — L90
- `UHodgeHUDLayout::NativeDestruct` — L139
- `UHodgeHUDLayout::EnsureMenuLayerStack` — L169
- `UHodgeHUDLayout::HandleEscapeAction` — L199
- `UHodgeHUDLayout::HandleInputDeviceConnectionChanged` — L247
- `UHodgeHUDLayout::HandleInputDevicePairingChanged` — L271
- `UHodgeHUDLayout::ShouldPlatformDisplayControllerDisconnectScreen` — L295
- `UHodgeHUDLayout::NotifyControllerStateChangeForDisconnectScreen` — L328
- `UHodgeHUDLayout::ProcessControllerDevicesHavingChangedForDisconnectScreen` — L369
- `UHodgeHUDLayout::DisplayControllerDisconnectedMenu_Implementation` — L442
- `UHodgeHUDLayout::HideControllerDisconnectedMenu_Implementation` — L481

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

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

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

## HodgeAbilityTask_PlayTimeline.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h)

驱动 HodgeAbilityTimeline 的唯一 AbilityTask：初始化与 Tick 共用 CollectNodes + SortNodes 统一 Scheduler，推进逻辑时间，维护 WindowTag 与 GE 两个账本，派发 Point 与系统事件。窗口 GE 施加/移除、Point 与 Timeline.End 派发、中途取消清理已实测通过；重入类时序、NetPolicy 跨端、时钟倒退未验证。

## HodgeAbilityTask_WaitMoveCancel.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h)

把“取消窗口（Timeline 授权）”与“移动意图（输入层）”两个变化驱动信号合流的 AbilityTask：同时成立时广播 OnMoveCancel 一次后自结束。本地控制端语义；AI/模拟代理找不到 HeroComponent 时永不成立。工作区新增，未编译、未 PIE。

## HodgeGameplayAbility.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。（PreloadPrimaryAssetsOnGrant 已随未提交改动回退，当前不存在。）

## HodgeGameplayAbility_BasicAttack.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_BasicAttack.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_BasicAttack.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayAbility_Definition.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)

模块定义或基础代码；请查看对应文件。

## HodgeAttributeSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)

项目 AttributeSet 基础和 ASC 访问。

## HodgeCombatSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)

模块定义或基础代码；请查看对应文件。

## HodgeHealthSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)

Health/MaxHealth、BaseDamage/BaseHeal 和 Damage/Healing 元属性；有效结算、夹取、免疫和耗尽广播。

## HodgeDamageExecution.h

[Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeDamageExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeDamageExecution.h)

模块定义或基础代码；请查看对应文件。

## HodgeHealExecution.h

[Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeHealExecution.h](../../../Source/Hodgepodge/Public/AbilitySystem/Executions/HodgeHealExecution.h)

模块定义或基础代码；请查看对应文件。

## GameplayTagStack.h

[Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h)

带计数的标签栈及复制数据结构，区别于只判断有无的 TagContainer。

## HodgeAbilitySystemComponent.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

Tag 输入缓存、激活组、关系映射、全局注册、失败通知与动态 Tag GE。

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

## HodgeTimelineEvaluator.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeTimelineEvaluator.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeTimelineEvaluator.h)

模块定义或基础代码；请查看对应文件。

## HodgeActorBase.h

[Source/Hodgepodge/Public/Actor/HodgeActorBase.h](../../../Source/Hodgepodge/Public/Actor/HodgeActorBase.h)

项目 Actor 基类扩展。

## HodgeAnimInstance.h

[Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)

ASC GameplayTag 属性映射和 GroundDistance 更新。

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

PawnExtension、相机、ASC 查询、移动标签、复制与死亡占位逻辑。

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

## HodgeActorComponentBase.h

[Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)

通用 ActorComponent 基础访问与扩展。

## HodgeCharacterMovementComponent.h

[Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)

CharacterMovement 扩展、地面距离、加速度和能力系统相关移动入口。

## HodgeCombatComponentBase.h

[Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)

战斗组件占位，当前构造关闭 Tick，尚无完整命中或连招实现。

## HodgeComboComponent.h

[Source/Hodgepodge/Public/Component/HodgeComboComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeComboComponent.h)

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

项目 HUD 类：GameFrameworkComponent 接收器注册与 GAS 调试 Actor 列表；不代表 CommonUI 已完成。

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

模块定义或基础代码；请查看对应文件。

## HodgeAbilitySet.h

[Source/Hodgepodge/Public/Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)

权威端批量授予属性集、技能和 GE，并用句柄集合撤销。

## HodgeAbilityTimeline.h

[Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)

技能逻辑时间轴数据资产（统一事件模型：单一 Events[]，Kind = Window / Point）。只描述“何时发生什么”，不含业务判断；没有 Montage 字段、不做 Bundle 收集。

## HodgeAssetManager.h

[Source/Hodgepodge/Public/Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)

资产入口、GameData 缓存、启动任务、同步加载、加载进度。Cue 初始化钩子仍需接通。（PreloadPrimaryAssetBundles 已随未提交改动回退，当前不存在。）

- `UHodgeAssetManager::GetAsset` — L139
- `UHodgeAssetManager::GetSubclass` — L170

## HodgeAssetManagerStartupJob.h

[Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h)

封装启动任务与进度权重，供 AssetManager 执行启动工作。

## HodgeComboDefinition.h

[Source/Hodgepodge/Public/Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)

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

PawnClass、AbilitySets、TagRelationshipMapping、InputConfig、DefaultCameraMode 配置。

## HodgeEquipmentDefinition.h

[Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)

模块定义或基础代码；请查看对应文件。

## HodgeEquipmentInstance.h

[Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)

模块定义或基础代码；请查看对应文件。

## HodgeEquipmentManagerComponent.h

[Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)

模块定义或基础代码；请查看对应文件。

## HodgeWeaponInstance.h

[Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)

模块定义或基础代码；请查看对应文件。

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

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

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

模块定义或基础代码；请查看对应文件。

状态：文件无有效非注释内容。

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
