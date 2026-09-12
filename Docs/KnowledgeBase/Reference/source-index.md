# 源码文件与有效定义索引

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

索引排除第三方插件和构建产物；行号为生成时位置。注释已排除，但没有求值预处理条件。

## Hodgepodge.Build.cs

[Source/Hodgepodge/Hodgepodge.Build.cs](../../../Source/Hodgepodge/Hodgepodge.Build.cs)

模块定义或基础代码；请查看对应文件。

## Hodgepodge.cpp

[Source/Hodgepodge/Hodgepodge.cpp](../../../Source/Hodgepodge/Hodgepodge.cpp)

模块定义或基础代码；请查看对应文件。

## Hodgepodge.h

[Source/Hodgepodge/Hodgepodge.h](../../../Source/Hodgepodge/Hodgepodge.h)

模块定义或基础代码；请查看对应文件。

## HodgeGameplayAbility.cpp

[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp)

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。

- `UHodgeGameplayAbility::UHodgeGameplayAbility` — L34
- `UHodgeGameplayAbility::GetHodgeAbilitySystemComponentFromActorInfo` — L63
- `UHodgeGameplayAbility::GetHodgePlayerControllerFromActorInfo` — L72
- `UHodgeGameplayAbility::GetControllerFromActorInfo` — L79
- `UHodgeGameplayAbility::GetHodgeCharacterFromActorInfo` — L117
- `UHodgeGameplayAbility::NativeOnAbilityFailedToActivate` — L129
- `UHodgeGameplayAbility::CanActivateAbility` — L182
- `UHodgeGameplayAbility::SetCanBeCanceled` — L223
- `UHodgeGameplayAbility::OnGiveAbility` — L242
- `UHodgeGameplayAbility::OnRemoveAbility` — L255
- `UHodgeGameplayAbility::ActivateAbility` — L266
- `UHodgeGameplayAbility::EndAbility` — L276
- `UHodgeGameplayAbility::CheckCost` — L289
- `UHodgeGameplayAbility::ApplyCost` — L319
- `UHodgeGameplayAbility::MakeEffectContext` — L402
- `UHodgeGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec` — L449
- `UHodgeGameplayAbility::DoesAbilitySatisfyTagRequirements` — L468
- `UHodgeGameplayAbility::OnPawnAvatarSet` — L624
- `UHodgeGameplayAbility::GetAbilitySource` — L631
- `UHodgeGameplayAbility::TryActivateAbilityOnSpawn` — L657
- `UHodgeGameplayAbility::CanChangeActivationGroup` — L699
- `UHodgeGameplayAbility::ChangeActivationGroup` — L737
- `UHodgeGameplayAbility::SetCameraMode` — L770
- `UHodgeGameplayAbility::ClearCameraMode` — L784

## HodgeAttributeSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp)

项目 AttributeSet 基础和 ASC 访问。

- `UHodgeAttributeSet::UHodgeAttributeSet` — L18
- `UHodgeAttributeSet::GetWorld` — L22
- `UHodgeAttributeSet::GetHodgeAbilitySystemComponent` — L29

## HodgeHealthSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp)

Health、MaxHealth、BaseDamage、BaseHeal；复制与夹取有效，GE 结算主体注释。

- `UHodgeHealthSet::UHodgeHealthSet` — L12
- `UHodgeHealthSet::GetLifetimeReplicatedProps` — L22
- `UHodgeHealthSet::OnRep_Health` — L32
- `UHodgeHealthSet::OnRep_MaxHealth` — L54
- `UHodgeHealthSet::OnRep_BaseDamage` — L64
- `UHodgeHealthSet::OnRep_BaseHeal` — L69
- `UHodgeHealthSet::PreGameplayEffectExecute` — L74
- `UHodgeHealthSet::PostGameplayEffectExecute` — L115
- `UHodgeHealthSet::PreAttributeBaseChange` — L192
- `UHodgeHealthSet::PreAttributeChange` — L199
- `UHodgeHealthSet::PostAttributeChange` — L206
- `UHodgeHealthSet::ClampAttribute` — L228

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

## HodgeAbilityTagRelationshipMapping.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilityTagRelationshipMapping.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilityTagRelationshipMapping.cpp)

数据驱动的能力阻断、取消与激活条件关系。

- `UHodgeAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel` — L9
- `UHodgeAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags` — L39
- `UHodgeAbilityTagRelationshipMapping::IsAbilityCancelledByTag` — L69

## HodgeGameplayCueManager.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayCueManager.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayCueManager.cpp)

项目 Cue 资源加载/预加载管理实现；默认管理器配置尚未接通。

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

项目 GE 上下文与序列化扩展；需要 Globals 分配入口配套。

- `FHodgeGameplayEffectContext::ExtractEffectContext` — L18
- `FHodgeGameplayEffectContext::NetSerialize` — L37
- `FHodgeGameplayEffectContext::SetAbilitySource` — L61
- `FHodgeGameplayEffectContext::GetAbilitySource` — L71
- `FHodgeGameplayEffectContext::GetPhysicalMaterial` — L78

## HodgeGameplayTags.cpp

[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayTags.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayTags.cpp)

原生 GameplayTag 注册和移动状态标签映射。标签存在不等于对应玩法实现。

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

## HodgeActorBase.cpp

[Source/Hodgepodge/Private/Actor/HodgeActorBase.cpp](../../../Source/Hodgepodge/Private/Actor/HodgeActorBase.cpp)

项目 Actor 基类扩展。

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

第三人称偏移与防穿透逻辑。

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

项目相机管理器；实际控制器类选择需验证。

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

原生 Character 基础、替换移动组件、Receiver 生命周期。EndPlay 事件配对需修正。

- `AHodgeCharacterBase::AHodgeCharacterBase` — L29
- `AHodgeCharacterBase::PreInitializeComponents` — L41
- `AHodgeCharacterBase::EndPlay` — L55
- `AHodgeCharacterBase::BeginPlay` — L69

## HodgeCombatCharacter.cpp

[Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp)

PawnExtension、相机、ASC 查询、移动标签、复制与死亡占位逻辑。

- `AHodgeCombatCharacter::AHodgeCombatCharacter` — L36
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
- `AHodgeCombatCharacter::UnPossessed` — L357
- `AHodgeCombatCharacter::OnRep_Controller` — L383
- `AHodgeCombatCharacter::OnRep_PlayerState` — L392
- `AHodgeCombatCharacter::SetupPlayerInputComponent` — L401
- `AHodgeCombatCharacter::InitializeGameplayTags` — L410
- `AHodgeCombatCharacter::GetOwnedGameplayTags` — L445
- `AHodgeCombatCharacter::HasMatchingGameplayTag` — L455
- `AHodgeCombatCharacter::HasAllMatchingGameplayTags` — L467
- `AHodgeCombatCharacter::HasAnyMatchingGameplayTags` — L479
- `AHodgeCombatCharacter::FellOutOfWorld` — L491
- `AHodgeCombatCharacter::OnDeathStarted` — L498
- `AHodgeCombatCharacter::OnDeathFinished` — L505
- `AHodgeCombatCharacter::DisableMovementAndCollision` — L512
- `AHodgeCombatCharacter::DestroyDueToDeath` — L542
- `AHodgeCombatCharacter::UninitAndDestroy` — L552
- `AHodgeCombatCharacter::OnMovementModeChanged` — L580
- `AHodgeCombatCharacter::SetMovementModeTag` — L596
- `AHodgeCombatCharacter::ToggleCrouch` — L625
- `AHodgeCombatCharacter::OnStartCrouch` — L644
- `AHodgeCombatCharacter::OnEndCrouch` — L658
- `AHodgeCombatCharacter::CanJumpInternal_Implementation` — L672
- `AHodgeCombatCharacter::OnRep_ReplicatedAcceleration` — L679
- `AHodgeCombatCharacter::OnControllerChangedTeam` — L709
- `AHodgeCombatCharacter::OnRep_MyTeamID` — L722
- `AHodgeCombatCharacter::UpdateSharedReplication` — L729
- `AHodgeCombatCharacter::FastSharedReplication_Implementation` — L762
- `FSharedRepMovement::FSharedRepMovement` — L804
- `FSharedRepMovement::FillForCharacter` — L811
- `FSharedRepMovement::Equals` — L858
- `FSharedRepMovement::NetSerialize` — L901

## HodgeEnemyCharacter.cpp

[Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp)

敌人移动与自动 AI 控制配置，尚未完成敌人 ASC 初始化。

## HodgeHeroCharacter.cpp

[Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp)

玩家角色，当前保留 PossessedBy / OnRep_PlayerState 直接初始化 ASC 的旧入口。

- `AHodgeHeroCharacter::AHodgeHeroCharacter` — L22
- `AHodgeHeroCharacter::PossessedBy` — L37
- `AHodgeHeroCharacter::OnRep_PlayerState` — L58

## HodgeActorComponentBase.cpp

[Source/Hodgepodge/Private/Component/HodgeActorComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeActorComponentBase.cpp)

通用 ActorComponent 基础访问与扩展。

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

## HodgeExperienceManagerComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeExperienceManagerComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeExperienceManagerComponent.cpp)

GameState 上的 Experience 复制、资源加载、插件激活、Action 执行与 Loaded 委托。

## HodgeHeroComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp)

玩家初始化、输入与相机的迁移草稿；当前文件全部注释，不是有效反射类。

状态：文件无有效非注释内容。

## HodgeInteractionComponentBase.cpp

[Source/Hodgepodge/Private/Component/HodgeInteractionComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeInteractionComponentBase.cpp)

交互组件基础占位；完整扫描、交互规则与 UI 需另行实现。

## HodgeMovementComponentBase.cpp

[Source/Hodgepodge/Private/Component/HodgeMovementComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeMovementComponentBase.cpp)

通用移动组件基础占位，与 CharacterMovement 派生类需区分。

## HodgePawnExtensionComponent.cpp

[Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp)

Pawn 数据复制和 Init State 协调、ASC 关联与解除、就绪委托。

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

## HodgeGameInstanceBase.cpp

[Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp](../../../Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp)

注册 Init State 顺序、主控制器访问和全局生命周期扩展。

## HodgeGameModeBase.cpp

[Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp](../../../Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp)

服务器玩法选择、等待 Experience、取得 PawnData、默认 Pawn 生成与重生。数据注入仍注释。

- `AHodgeGameModeBase::AHodgeGameModeBase` — L20
- `AHodgeGameModeBase::GetPawnDataForController` — L45
- `AHodgeGameModeBase::InitGame` — L86
- `AHodgeGameModeBase::HandleMatchAssignmentIfNotExpectingOne` — L95
- `AHodgeGameModeBase::TryDedicatedServerLogin` — L192
- `AHodgeGameModeBase::OnMatchAssignmentGiven` — L336
- `AHodgeGameModeBase::OnExperienceLoaded` — L360
- `AHodgeGameModeBase::IsExperienceLoaded` — L381
- `AHodgeGameModeBase::GetDefaultPawnClassForController_Implementation` — L395
- `AHodgeGameModeBase::SpawnDefaultPawnAtTransform_Implementation` — L411
- `AHodgeGameModeBase::ShouldSpawnAtStartSpot` — L468
- `AHodgeGameModeBase::HandleStartingNewPlayer_Implementation` — L474
- `AHodgeGameModeBase::ChoosePlayerStart_Implementation` — L484
- `AHodgeGameModeBase::FinishRestartPlayer` — L497
- `AHodgeGameModeBase::PlayerCanRestart_Implementation` — L510
- `AHodgeGameModeBase::ControllerCanRestart` — L516
- `AHodgeGameModeBase::InitGameState` — L545
- `AHodgeGameModeBase::GenericPlayerInitialization` — L560
- `AHodgeGameModeBase::RequestPlayerRestartNextFrame` — L569
- `AHodgeGameModeBase::UpdatePlayerStartSpot` — L590
- `AHodgeGameModeBase::FailedToRestartPlayer` — L596

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

## HodgeHUDBase.cpp

[Source/Hodgepodge/Private/Core/HUD/HodgeHUDBase.cpp](../../../Source/Hodgepodge/Private/Core/HUD/HodgeHUDBase.cpp)

项目 HUD 基类扩展入口，不代表 CommonUI 已完成。

## HodgeLocalPlayerBase.cpp

[Source/Hodgepodge/Private/Core/LocalPlayer/HodgeLocalPlayerBase.cpp](../../../Source/Hodgepodge/Private/Core/LocalPlayer/HodgeLocalPlayerBase.cpp)

本地玩家对象及控制器、PlayerState、Pawn 就绪事件桥。

## HodgePlayerControllerBase.cpp

[Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerControllerBase.cpp](../../../Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerControllerBase.cpp)

把控制器和 Pawn 生命周期桥接到 LocalPlayer 委托。技能输入每帧消费尚未接入。

- `AHodgePlayerControllerBase::AHodgePlayerControllerBase` — L22
- `AHodgePlayerControllerBase::ReceivedPlayer` — L49
- `AHodgePlayerControllerBase::SetPawn` — L80
- `AHodgePlayerControllerBase::OnPossess` — L115
- `AHodgePlayerControllerBase::OnUnPossess` — L146
- `AHodgePlayerControllerBase::OnRep_PlayerState` — L184

## HodgePlayerState.cpp

[Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp](../../../Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp)

玩家 ASC、HealthSet、PawnData、阵营/标签栈等持有者。基础能力授予循环仍停用。

- `AHodgePlayerState::NAME_HodgeAbilityReady` — L16
- `AHodgePlayerState::AHodgePlayerState` — L18
- `AHodgePlayerState::GetHodgePlayerController` — L35
- `AHodgePlayerState::GetAbilitySystemComponent` — L40
- `AHodgePlayerState::SetPawnData` — L45
- `AHodgePlayerState::PreInitializeComponents` — L78
- `AHodgePlayerState::PostInitializeComponents` — L97
- `AHodgePlayerState::Reset` — L102
- `AHodgePlayerState::ClientInitialize` — L107
- `AHodgePlayerState::CopyProperties` — L117
- `AHodgePlayerState::GetLifetimeReplicatedProps` — L123
- `AHodgePlayerState::OnDeactivated` — L141
- `AHodgePlayerState::OnReactivated` — L166
- `AHodgePlayerState::SetPlayerConnectionType` — L175
- `AHodgePlayerState::SetSquadID` — L181
- `AHodgePlayerState::AddStatTagStack` — L191
- `AHodgePlayerState::RemoveStatTagStack` — L196
- `AHodgePlayerState::GetStatTagStackCount` — L201
- `AHodgePlayerState::HasStatTag` — L206
- `AHodgePlayerState::GetReplicatedViewRotation` — L211
- `AHodgePlayerState::SetReplicatedViewRotation` — L216
- `AHodgePlayerState::OnExperienceLoaded` — L225
- `AHodgePlayerState::OnRep_PawnData` — L243
- `AHodgePlayerState::OnRep_MyTeamID` — L247
- `AHodgePlayerState::OnRep_MySquadID` — L252

## HodgePlayerStateBase.cpp

[Source/Hodgepodge/Private/Core/PlayState/HodgePlayerStateBase.cpp](../../../Source/Hodgepodge/Private/Core/PlayState/HodgePlayerStateBase.cpp)

PlayerState ModularGameplay Receiver 注册、注销及组件 Reset/CopyProperties。

- `AHodgePlayerStateBase::PreInitializeComponents` — L10
- `AHodgePlayerStateBase::BeginPlay` — L19
- `AHodgePlayerStateBase::EndPlay` — L29
- `AHodgePlayerStateBase::Reset` — L38
- `AHodgePlayerStateBase::CopyProperties` — L55

## HodgeAbilitySet.cpp

[Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp)

权威端批量授予属性集、技能和 GE，并用句柄集合撤销。

- `FHodgeAbilitySet_GrantedHandles::AddAbilitySpecHandle` — L11
- `FHodgeAbilitySet_GrantedHandles::AddGameplayEffectHandle` — L20
- `FHodgeAbilitySet_GrantedHandles::AddAttributeSet` — L29
- `FHodgeAbilitySet_GrantedHandles::TakeFromAbilitySystem` — L35
- `UHodgeAbilitySet::UHodgeAbilitySet` — L76
- `UHodgeAbilitySet::GiveToAbilitySystem` — L82

## HodgeAssetManager.cpp

[Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp)

资产入口、GameData 缓存、启动任务、同步加载和加载进度。Cue 初始化钩子仍需接通。

## HodgeAssetManagerStartupJob.cpp

[Source/Hodgepodge/Private/Data/HodgeAssetManagerStartupJob.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAssetManagerStartupJob.cpp)

封装启动任务与进度权重，供 AssetManager 执行启动工作。

- `FHodgeAssetManagerStartupJob::DoJob` — L13

## HodgeExperienceActionSet.cpp

[Source/Hodgepodge/Private/Data/HodgeExperienceActionSet.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceActionSet.cpp)

复用 GameFeature 插件和动作配置的数据资产。

## HodgeExperienceDefinition.cpp

[Source/Hodgepodge/Private/Data/HodgeExperienceDefinition.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceDefinition.cpp)

声明玩法所需插件、默认 PawnData、直接 Actions 和组合 ActionSets。

## HodgeExperienceManager.cpp

[Source/Hodgepodge/Private/Data/HodgeExperienceManager.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceManager.cpp)

管理编辑器等场景的 GameFeature 使用/停用协调，不是挂载在 GameState 的组件。

## HodgeGameData.cpp

[Source/Hodgepodge/Private/Data/HodgeGameData.cpp](../../../Source/Hodgepodge/Private/Data/HodgeGameData.cpp)

全局伤害、治疗、动态 Tag GE 的软类引用配置；需编辑器核对实际赋值。

## HodgePawnData.cpp

[Source/Hodgepodge/Private/Data/HodgePawnData.cpp](../../../Source/Hodgepodge/Private/Data/HodgePawnData.cpp)

PawnClass、AbilitySets、TagRelationshipMapping、InputConfig、DefaultCameraMode 配置。

- `UHodgePawnData::UHodgePawnData` — L8

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

声明和校验 Cue 路径配置；Policy 内实际增删路径仍注释。

- `UGameFeatureAction_AddGameplayCuePath::UGameFeatureAction_AddGameplayCuePath` — L18
- `UGameFeatureAction_AddGameplayCuePath::IsDataValid` — L27

## GameFeatureAction_AddInputBinding.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputBinding.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputBinding.cpp)

额外 InputConfig 扩展；依赖 Hero 的主体和添加分支仍注释。

- `UGameFeatureAction_AddInputBinding::OnGameFeatureActivating` — L50
- `UGameFeatureAction_AddInputBinding::OnGameFeatureDeactivating` — L68
- `UGameFeatureAction_AddInputBinding::IsDataValid` — L86
- `UGameFeatureAction_AddInputBinding::AddToWorld` — L118
- `UGameFeatureAction_AddInputBinding::Reset` — L153
- `UGameFeatureAction_AddInputBinding::HandlePawnExtension` — L179
- `UGameFeatureAction_AddInputBinding::AddInputMappingForPlayer` — L204
- `UGameFeatureAction_AddInputBinding::RemoveInputMapping` — L249

## GameFeatureAction_AddInputContextMapping.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputContextMapping.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputContextMapping.cpp)

IMC 与本地玩家/设置集成，Controller 扩展添加分支仍注释。

- `UGameFeatureAction_AddInputContextMapping::OnGameFeatureRegistering` — L39
- `UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating` — L49
- `UGameFeatureAction_AddInputContextMapping::OnGameFeatureDeactivating` — L67
- `UGameFeatureAction_AddInputContextMapping::OnGameFeatureUnregistering` — L84
- `UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContexts` — L94
- `UGameFeatureAction_AddInputContextMapping::RegisterInputContextMappingsForGameInstance` — L113
- `UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer` — L137
- `UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContexts` — L175
- `UGameFeatureAction_AddInputContextMapping::UnregisterInputContextMappingsForGameInstance` — L196
- `UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer` — L219
- `UGameFeatureAction_AddInputContextMapping::IsDataValid` — L257
- `UGameFeatureAction_AddInputContextMapping::AddToWorld` — L289
- `UGameFeatureAction_AddInputContextMapping::Reset` — L324
- `UGameFeatureAction_AddInputContextMapping::HandleControllerExtension` — L349
- `UGameFeatureAction_AddInputContextMapping::AddInputMappingForPlayer` — L375
- `UGameFeatureAction_AddInputContextMapping::RemoveInputMapping` — L407

## GameFeatureAction_AddWidget.cpp

[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddWidget.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddWidget.cpp)

Widget 注入迁移草稿，当前实现停用。

状态：文件无有效非注释内容。

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

GameFeature 项目策略和资源加载观察扩展。

- `UHodgeGameFeaturePolicy::UHodgeGameFeaturePolicy` — L14
- `UHodgeGameFeaturePolicy::Get` — L23
- `UHodgeGameFeaturePolicy::InitGameFeatureManager` — L33
- `UHodgeGameFeaturePolicy::ShutdownGameFeatureManager` — L58
- `UHodgeGameFeaturePolicy::GetPreloadAssetListForGameFeature` — L78
- `UHodgeGameFeaturePolicy::GetPreloadBundleStateForGameFeature` — L91
- `UHodgeGameFeaturePolicy::GetGameFeatureLoadingMode` — L101
- `UHodgeGameFeaturePolicy::IsPluginAllowed` — L115
- `UHodgeGameFeature_HotfixManager::OnGameFeatureLoading` — L127
- `UHodgeGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering` — L155
- `UHodgeGameFeature_AddGameplayCuePaths::OnGameFeatureUnregistering` — L239

## HodgeAimSensitivityData.cpp

[Source/Hodgepodge/Private/Input/HodgeAimSensitivityData.cpp](../../../Source/Hodgepodge/Private/Input/HodgeAimSensitivityData.cpp)

瞄准灵敏度数据映射。

- `UHodgeAimSensitivityData::UHodgeAimSensitivityData` — L12

## HodgeInputComponent.cpp

[Source/Hodgepodge/Private/Input/HodgeInputComponent.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputComponent.cpp)

基于 Tag 的 Native/Ability Action 绑定和句柄移除；映射辅助函数仍占位。

## HodgeInputConfig.cpp

[Source/Hodgepodge/Private/Input/HodgeInputConfig.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputConfig.cpp)

NativeInputActions / AbilityInputActions 的 IA 与 Tag 数据配置及查询。

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

## HodgeAbilityCost.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h)

自定义额外能力消耗的扩展契约。

## HodgeGameplayAbility.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。

## HodgeAttributeSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)

项目 AttributeSet 基础和 ASC 访问。

## HodgeHealthSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)

Health、MaxHealth、BaseDamage、BaseHeal；复制与夹取有效，GE 结算主体注释。

## GameplayTagStack.h

[Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h)

带计数的标签栈及复制数据结构，区别于只判断有无的 TagContainer。

## HodgeAbilitySystemComponent.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

Tag 输入缓存、激活组、关系映射、全局注册、失败通知与动态 Tag GE。

## HodgeAbilityTagRelationshipMapping.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h)

数据驱动的能力阻断、取消与激活条件关系。

## HodgeGameplayCueManager.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayCueManager.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayCueManager.h)

项目 Cue 资源加载/预加载管理实现；默认管理器配置尚未接通。

## HodgeGameplayEffectContext.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)

项目 GE 上下文与序列化扩展；需要 Globals 分配入口配套。

## HodgeGameplayTags.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

原生 GameplayTag 注册和移动状态标签映射。标签存在不等于对应玩法实现。

## HodgeGlobalAbilitySystem.h

[Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h)

世界级全局能力/效果授予及 ASC 注册表。

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

第三人称偏移与防穿透逻辑。

## HodgePenetrationAvoidanceFeeler.h

[Source/Hodgepodge/Public/Camera/HodgePenetrationAvoidanceFeeler.h](../../../Source/Hodgepodge/Public/Camera/HodgePenetrationAvoidanceFeeler.h)

相机防穿透探测参数结构。

## HodgePlayerCameraManager.h

[Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h](../../../Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h)

项目相机管理器；实际控制器类选择需验证。

## HodgeUICameraManagerComponent.h

[Source/Hodgepodge/Public/Camera/HodgeUICameraManagerComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeUICameraManagerComponent.h)

UI 相机管理扩展，不代表 UI 系统已接入。

## HodgeCharacterBase.h

[Source/Hodgepodge/Public/Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)

原生 Character 基础、替换移动组件、Receiver 生命周期。EndPlay 事件配对需修正。

## HodgeCombatCharacter.h

[Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)

PawnExtension、相机、ASC 查询、移动标签、复制与死亡占位逻辑。

## HodgeEnemyCharacter.h

[Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)

敌人移动与自动 AI 控制配置，尚未完成敌人 ASC 初始化。

## HodgeHeroCharacter.h

[Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h)

玩家角色，当前保留 PossessedBy / OnRep_PlayerState 直接初始化 ASC 的旧入口。

## HodgeActorComponentBase.h

[Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)

通用 ActorComponent 基础访问与扩展。

## HodgeCharacterMovementComponent.h

[Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)

CharacterMovement 扩展、地面距离、加速度和能力系统相关移动入口。

## HodgeCombatComponentBase.h

[Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)

战斗组件占位，当前构造关闭 Tick，尚无完整命中或连招实现。

## HodgeExperienceManagerComponent.h

[Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)

GameState 上的 Experience 复制、资源加载、插件激活、Action 执行与 Loaded 委托。

## HodgeHeroComponent.h

[Source/Hodgepodge/Public/Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)

玩家初始化、输入与相机的迁移草稿；当前文件全部注释，不是有效反射类。

状态：文件无有效非注释内容。

## HodgeInteractionComponentBase.h

[Source/Hodgepodge/Public/Component/HodgeInteractionComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeInteractionComponentBase.h)

交互组件基础占位；完整扫描、交互规则与 UI 需另行实现。

## HodgeMovementComponentBase.h

[Source/Hodgepodge/Public/Component/HodgeMovementComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeMovementComponentBase.h)

通用移动组件基础占位，与 CharacterMovement 派生类需区分。

## HodgePawnExtensionComponent.h

[Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)

Pawn 数据复制和 Init State 协调、ASC 关联与解除、就绪委托。

## HodgeGameInstanceBase.h

[Source/Hodgepodge/Public/Core/GameInstance/HodgeGameInstanceBase.h](../../../Source/Hodgepodge/Public/Core/GameInstance/HodgeGameInstanceBase.h)

注册 Init State 顺序、主控制器访问和全局生命周期扩展。

## HodgeGameModeBase.h

[Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h](../../../Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h)

服务器玩法选择、等待 Experience、取得 PawnData、默认 Pawn 生成与重生。数据注入仍注释。

## HodgeGameState.h

[Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h)

创建 ExperienceManager 和世界状态 ASC，处理游戏状态复制/扩展。

## HodgeGameStateBase.h

[Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h)

GameState 基础扩展生命周期。

## HodgeHUDBase.h

[Source/Hodgepodge/Public/Core/HUD/HodgeHUDBase.h](../../../Source/Hodgepodge/Public/Core/HUD/HodgeHUDBase.h)

项目 HUD 基类扩展入口，不代表 CommonUI 已完成。

## HodgeLocalPlayerBase.h

[Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)

本地玩家对象及控制器、PlayerState、Pawn 就绪事件桥。

## HodgePlayerControllerBase.h

[Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h)

把控制器和 Pawn 生命周期桥接到 LocalPlayer 委托。技能输入每帧消费尚未接入。

## HodgePlayerState.h

[Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)

玩家 ASC、HealthSet、PawnData、阵营/标签栈等持有者。基础能力授予循环仍停用。

## HodgePlayerStateBase.h

[Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h)

PlayerState ModularGameplay Receiver 注册、注销及组件 Reset/CopyProperties。

## HodgeAbilitySet.h

[Source/Hodgepodge/Public/Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)

权威端批量授予属性集、技能和 GE，并用句柄集合撤销。

## HodgeAssetManager.h

[Source/Hodgepodge/Public/Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)

资产入口、GameData 缓存、启动任务、同步加载和加载进度。Cue 初始化钩子仍需接通。

## HodgeAssetManagerStartupJob.h

[Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h)

封装启动任务与进度权重，供 AssetManager 执行启动工作。

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

## GameFeatureAction_AddAbilities.h

[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddAbilities.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddAbilities.h)

面向配置 Actor 授予能力、属性与 AbilitySet，维护撤销句柄。

## GameFeatureAction_AddGameplayCuePath.h

[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddGameplayCuePath.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddGameplayCuePath.h)

声明和校验 Cue 路径配置；Policy 内实际增删路径仍注释。

## GameFeatureAction_AddInputBinding.h

[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputBinding.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputBinding.h)

额外 InputConfig 扩展；依赖 Hero 的主体和添加分支仍注释。

## GameFeatureAction_AddInputContextMapping.h

[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputContextMapping.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputContextMapping.h)

IMC 与本地玩家/设置集成，Controller 扩展添加分支仍注释。

## GameFeatureAction_AddWidget.h

[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddWidget.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddWidget.h)

Widget 注入迁移草稿，当前实现停用。

状态：文件无有效非注释内容。

## GameFeatureAction_SplitscreenConfig.h

[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_SplitscreenConfig.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_SplitscreenConfig.h)

GameFeature 激活期间的分屏策略调整。

## GameFeatureAction_WorldActionBase.h

[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)

按游戏世界和激活上下文组织 Action 生命周期。

## HodgeGameFeaturePolicy.h

[Source/Hodgepodge/Public/GameFeatures/HodgeGameFeaturePolicy.h](../../../Source/Hodgepodge/Public/GameFeatures/HodgeGameFeaturePolicy.h)

GameFeature 项目策略和资源加载观察扩展。

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

## Hodgepodge.Target.cs

[Source/Hodgepodge.Target.cs](../../../Source/Hodgepodge.Target.cs)

模块定义或基础代码；请查看对应文件。

## HodgepodgeEditor.Target.cs

[Source/HodgepodgeEditor.Target.cs](../../../Source/HodgepodgeEditor.Target.cs)

模块定义或基础代码；请查看对应文件。
