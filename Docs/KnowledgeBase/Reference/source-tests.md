# Tests 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAbilityDefinitionTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAbilityDefinitionTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAbilityDefinitionTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)、[Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)

定义候选（多行签名仅展示首行）：

- L19: `bool FHodgeDefinitionGrantTest::RunTest(const FString& Parameters)`
- L55: `bool FHodgeComboValidationTest::RunTest(const FString& Parameters)`
- L128: `bool FHodgeComboRetentionTest::RunTest(const FString& Parameters)`
- L214: `bool FHodgeComboSessionTest::RunTest(const FString& Parameters)`
- L303: `bool FHodgeMontageDurationTest::RunTest(const FString& Parameters)`

## HodgeAbilityTimelineTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAbilityTimelineTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAbilityTimelineTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)

定义候选（多行签名仅展示首行）：

- L77: `bool FHodgeTimelineValidationTest::RunTest(const FString& Parameters)`
- L115: `bool FHodgeTimelineRejectTest::RunTest(const FString& Parameters)`
- L157: `bool FHodgeTimelineCleanupTest::RunTest(const FString& Parameters)`
- L222: `bool FHodgeTimelineWindowIdentityTest::RunTest(const FString& Parameters)`
- L269: `bool FHodgeTimelineWindowReentryTest::RunTest(const FString& Parameters)`

## HodgeAttributeGrowthTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAttributeGrowthTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAttributeGrowthTests.cpp)

项目内直接 include（不是运行调用关系）：[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)、[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)、[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/Stats/HodgeAttributeCoordinator.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeCoordinator.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Data/HodgeCharacterStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h)、[Data/HodgeEquipmentStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeEquipmentStatProfile.h)

定义候选（多行签名仅展示首行）：

- L97: `bool FHodgeStatProfileTest::RunTest(const FString& Parameters)`
- L117: `bool FHodgeStatGrowthTest::RunTest(const FString& Parameters)`
- L153: `bool FHodgeStatReentryTest::RunTest(const FString& Parameters)`
- L182: `bool FHodgeStatAvatarTest::RunTest(const FString& Parameters)`

## HodgeCharacterRotationTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeCharacterRotationTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeCharacterRotationTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)

定义候选（多行签名仅展示首行）：

- L14: `bool FHodgeCharacterRotationConstraintsTest::RunTest(const FString& Parameters)`

## HodgeHitDetectionTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeHitDetectionTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeHitDetectionTests.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)

定义候选（多行签名仅展示首行）：

- L11: `bool FHodgeHitGeometryTest::RunTest(const FString& Parameters)`
- L36: `bool FHodgeHitProfileValidationTest::RunTest(const FString& Parameters)`

## HodgeMeleeRoutingTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeMeleeRoutingTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeMeleeRoutingTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)

定义候选（多行签名仅展示首行）：

- L82: `bool FHodgeDetectionRoutingTest::RunTest(const FString& Parameters)`
- L112: `bool FHodgeMeleeHitHistoryTest::RunTest(const FString& Parameters)`
- L135: `bool FHodgeMeleeSpecContextTest::RunTest(const FString& Parameters)`

## HodgeSkillHitVolumeTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeSkillHitVolumeTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeSkillHitVolumeTests.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h)、[AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)

定义候选（多行签名仅展示首行）：

- L79: `bool FHodgeConfiguredVolumesTest::RunTest(const FString&)`
- L117: `bool FHodgeVolumeFilteringTest::RunTest(const FString&)`
- L167: `bool FHodgeStandaloneRouteTest::RunTest(const FString&)`
- L196: `bool FHodgeIndexedPointsTest::RunTest(const FString&)`
- L237: `bool FHodgePointBindingValidationTest::RunTest(const FString&)`

## HodgeTimelineEvaluatorTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeTimelineEvaluatorTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeTimelineEvaluatorTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeTimelineEvaluator.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeTimelineEvaluator.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L8: `bool FHodgeEvaluatorTest::RunTest(const FString& Parameters)`

## HodgeWeaponPresentationTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeWeaponPresentationTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeWeaponPresentationTests.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)、[Equipment/HodgeWeaponPresentationActor.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationActor.h)、[Equipment/HodgeWeaponPresentationProfile.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h)

定义候选（多行签名仅展示首行）：

- L21: `bool FHodgeWeaponRequestsTest::RunTest(const FString& Parameters)`
- L83: `bool FHodgeWeaponGeometryTest::RunTest(const FString& Parameters)`
