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

## HodgeAttributeSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp)

项目 AttributeSet 基础和 ASC 访问。

- `UHodgeAttributeSet::UHodgeAttributeSet` — L23
- `UHodgeAttributeSet::GetWorld` — L28
- `UHodgeAttributeSet::GetHodgeAbilitySystemComponent` — L41

## HodgeHealthSet.cpp

[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp)

Health/MaxHealth、BaseDamage/BaseHeal 和 Damage/Healing 元属性；有效结算、夹取、免疫和耗尽广播。

- `UHodgeHealthSet::UHodgeHealthSet` — L37
- `UHodgeHealthSet::GetLifetimeReplicatedProps` — L53
- `UHodgeHealthSet::OnRep_Health` — L72
- `UHodgeHealthSet::OnRep_MaxHealth` — L108
- `UHodgeHealthSet::OnRep_BaseDamage` — L125
- `UHodgeHealthSet::OnRep_BaseHeal` — L132
- `UHodgeHealthSet::PreGameplayEffectExecute` — L139
- `UHodgeHealthSet::PostGameplayEffectExecute` — L197
- `UHodgeHealthSet::PreAttributeBaseChange` — L334
- `UHodgeHealthSet::PreAttributeChange` — L344
- `UHodgeHealthSet::PostAttributeChange` — L354
- `UHodgeHealthSet::ClampAttribute` — L386

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

- `UHodgeAbilitySystemComponent::UHodgeAbilitySystemComponent` — L27
- `UHodgeAbilitySystemComponent::EndPlay` — L43
- `UHodgeAbilitySystemComponent::InitAbilityActorInfo` — L55
- `UHodgeAbilitySystemComponent::TryActivateAbilitiesOnSpawn` — L126
- `UHodgeAbilitySystemComponent::CancelAbilitiesByFunc` — L143
- `UHodgeAbilitySystemComponent::CancelInputActivatedAbilities` — L211
- `UHodgeAbilitySystemComponent::AbilitySpecInputPressed` — L228
- `UHodgeAbilitySystemComponent::AbilitySpecInputReleased` — L255
- `UHodgeAbilitySystemComponent::AbilityInputTagPressed` — L282
- `UHodgeAbilitySystemComponent::AbilityInputTagReleased` — L303
- `UHodgeAbilitySystemComponent::ProcessAbilityInput` — L324
- `UHodgeAbilitySystemComponent::ClearAbilityInput` — L454
- `UHodgeAbilitySystemComponent::NotifyAbilityActivated` — L466
- `UHodgeAbilitySystemComponent::NotifyAbilityFailed` — L480
- `UHodgeAbilitySystemComponent::NotifyAbilityEnded` — L502
- `UHodgeAbilitySystemComponent::ApplyAbilityBlockAndCancelTags` — L516
- `UHodgeAbilitySystemComponent::HandleChangeAbilityCanBeCanceled` — L545
- `UHodgeAbilitySystemComponent::GetAdditionalActivationTagRequirements` — L556
- `UHodgeAbilitySystemComponent::SetTagRelationshipMapping` — L569
- `UHodgeAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation` — L575
- `UHodgeAbilitySystemComponent::HandleAbilityFailed` — L582
- `UHodgeAbilitySystemComponent::IsActivationGroupBlocked` — L595
- `UHodgeAbilitySystemComponent::AddAbilityToActivationGroup` — L626
- `UHodgeAbilitySystemComponent::RemoveAbilityFromActivationGroup` — L674
- `UHodgeAbilitySystemComponent::CancelActivationGroupAbilities` — L687
- `UHodgeAbilitySystemComponent::AddDynamicTagGameplayEffect` — L703
- `UHodgeAbilitySystemComponent::RemoveDynamicTagGameplayEffect` — L739
- `UHodgeAbilitySystemComponent::GetAbilityTargetData` — L764

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

仅设置 AI 自动控制；旋转/移动参数继承 Combat 基类，尚未完成敌人 ASC 初始化。

- `AHodgeEnemyCharacter::AHodgeEnemyCharacter` — L10
- `AHodgeEnemyCharacter::BeginPlay` — L41
- `AHodgeEnemyCharacter::PossessedBy` — L46
- `AHodgeEnemyCharacter::PostEditChangeProperty` — L53

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

模块定义或基础代码；请查看对应文件。

- `UHodgeGroundedAuthoring::AddGroundedLayer` — L39

## HodgeGroundedLocomotion.cpp

[Source/Hodgepodge/Private/CodexText/HodgeGroundedLocomotion.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeGroundedLocomotion.cpp)

CodexText 实验：在 ALS 基础动画上增加平地起停/转身/脚锁。

- `UHodgeGroundedLocomotion::NativeInitializeAnimation` — L10
- `UHodgeGroundedLocomotion::GetGroundedState` — L18
- `UHodgeGroundedLocomotion::NativePostEvaluateAnimation` — L24
- `UHodgeGroundedLocomotion::NativeUpdateAnimation` — L36

## HodgeLocomotionLab.cpp

[Source/Hodgepodge/Private/CodexText/HodgeLocomotionLab.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeLocomotionLab.cpp)

模块定义或基础代码；请查看对应文件。

- `UHodgeLocomotionLabComponent::UHodgeLocomotionLabComponent` — L27
- `UHodgeLocomotionLabComponent::BeginPlay` — L33
- `UHodgeLocomotionLabComponent::SetCombatFacing` — L42
- `UHodgeLocomotionLabComponent::SetWalking` — L55
- `UHodgeLocomotionLabComponent::TickComponent` — L64
- `AHodgeLocomotionLabMode::InitGame` — L103
- `UHodgeLocomotionLabAuthoring::RemapCopy` — L110
- `UHodgeLocomotionLabAuthoring::ConfigureGroundBlend` — L147

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

## HodgeHeroComponent.cpp

[Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp)

玩家 Init State 协调、ASC 接入、输入与相机；额外输入句柄持久化并在移除/EndPlay 解绑。

- `UHodgeHeroComponent::NAME_BindInputsNow` — L81
- `UHodgeHeroComponent::NAME_ActorFeatureName` — L84
- `UHodgeHeroComponent::UHodgeHeroComponent` — L87
- `UHodgeHeroComponent::OnRegister` — L98
- `UHodgeHeroComponent::CanChangeInitState` — L142
- `UHodgeHeroComponent::HandleChangeInitState` — L247
- `UHodgeHeroComponent::OnActorInitStateChanged` — L312
- `UHodgeHeroComponent::CheckDefaultInitialization` — L328
- `UHodgeHeroComponent::BeginPlay` — L346
- `UHodgeHeroComponent::EndPlay` — L368
- `UHodgeHeroComponent::InitializePlayerInput` — L392
- `UHodgeHeroComponent::AddAdditionalInputConfig` — L610
- `UHodgeHeroComponent::RemoveAdditionalInputConfig` — L701
- `UHodgeHeroComponent::IsReadyToBindInputs` — L730
- `UHodgeHeroComponent::Input_AbilityInputTagPressed` — L737
- `UHodgeHeroComponent::Input_AbilityInputTagReleased` — L760
- `UHodgeHeroComponent::Input_Move` — L788
- `UHodgeHeroComponent::Input_LookMouse` — L846
- `UHodgeHeroComponent::Input_LookStick` — L885
- `UHodgeHeroComponent::Input_Crouch` — L932
- `UHodgeHeroComponent::Input_AutoRun` — L945
- `UHodgeHeroComponent::DetermineCameraMode` — L965
- `UHodgeHeroComponent::SetAbilityCameraMode` — L998
- `UHodgeHeroComponent::ClearAbilityCameraMode` — L1013

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

## HodgeHUDBase.cpp

[Source/Hodgepodge/Private/Core/HUD/HodgeHUDBase.cpp](../../../Source/Hodgepodge/Private/Core/HUD/HodgeHUDBase.cpp)

项目 HUD 基类扩展入口，不代表 CommonUI 已完成。

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

- `AHodgePlayerState::NAME_HodgeAbilityReady` — L34
- `AHodgePlayerState::AHodgePlayerState` — L37
- `AHodgePlayerState::GetHodgePlayerController` — L65
- `AHodgePlayerState::GetAbilitySystemComponent` — L72
- `AHodgePlayerState::SetPawnData` — L79
- `AHodgePlayerState::PreInitializeComponents` — L124
- `AHodgePlayerState::PostInitializeComponents` — L162
- `AHodgePlayerState::Reset` — L169
- `AHodgePlayerState::ClientInitialize` — L176
- `AHodgePlayerState::CopyProperties` — L189
- `AHodgePlayerState::GetLifetimeReplicatedProps` — L199
- `AHodgePlayerState::OnDeactivated` — L233
- `AHodgePlayerState::OnReactivated` — L269
- `AHodgePlayerState::SetPlayerConnectionType` — L283
- `AHodgePlayerState::SetSquadID` — L293
- `AHodgePlayerState::AddStatTagStack` — L307
- `AHodgePlayerState::RemoveStatTagStack` — L314
- `AHodgePlayerState::GetStatTagStackCount` — L321
- `AHodgePlayerState::HasStatTag` — L328
- `AHodgePlayerState::GetReplicatedViewRotation` — L335
- `AHodgePlayerState::SetReplicatedViewRotation` — L342
- `AHodgePlayerState::OnExperienceLoaded` — L356
- `AHodgePlayerState::OnRep_PawnData` — L379
- `AHodgePlayerState::OnRep_MyTeamID` — L385
- `AHodgePlayerState::OnRep_MySquadID` — L392

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

## HodgeAbilityCost.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h)

自定义额外能力消耗的扩展契约。

## HodgeGameplayAbility.h

[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。（PreloadPrimaryAssetsOnGrant 已随未提交改动回退，当前不存在。）

## HodgeAttributeSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)

项目 AttributeSet 基础和 ASC 访问。

## HodgeHealthSet.h

[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)

Health/MaxHealth、BaseDamage/BaseHeal 和 Damage/Healing 元属性；有效结算、夹取、免疫和耗尽广播。

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

CodexText 实验：在 ALS 基础动画上增加平地起停/转身/脚锁。

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

## HodgeExperienceManagerComponent.h

[Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)

GameState 上的 Experience 复制、资源加载、插件激活、Action 执行与 Loaded 委托。

## HodgeHeroComponent.h

[Source/Hodgepodge/Public/Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)

玩家 Init State 协调、ASC 接入、输入与相机；额外输入句柄持久化并在移除/EndPlay 解绑。

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

## HodgeHUDBase.h

[Source/Hodgepodge/Public/Core/HUD/HodgeHUDBase.h](../../../Source/Hodgepodge/Public/Core/HUD/HodgeHUDBase.h)

项目 HUD 基类扩展入口，不代表 CommonUI 已完成。

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

状态：文件无有效非注释内容。

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

## Hodgepodge.Target.cs

[Source/Hodgepodge.Target.cs](../../../Source/Hodgepodge.Target.cs)

模块定义或基础代码；请查看对应文件。

## HodgepodgeEditor.Target.cs

[Source/HodgepodgeEditor.Target.cs](../../../Source/HodgepodgeEditor.Target.cs)

模块定义或基础代码；请查看对应文件。
