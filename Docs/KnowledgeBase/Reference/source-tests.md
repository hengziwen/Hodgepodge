# Tests 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAbilityDefinitionTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAbilityDefinitionTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAbilityDefinitionTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)

定义候选（多行签名仅展示首行）：

- L23: `bool FHodgeDefinitionGrantTest::RunTest(const FString& Parameters)`
- L62: `bool FHodgeComboValidationTest::RunTest(const FString& Parameters)`
- L176: `bool FHodgeComboRetentionTest::RunTest(const FString& Parameters)`
- L298: `bool FHodgeComboSessionTest::RunTest(const FString& Parameters)`

## HodgeAbilityIconTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAbilityIconTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAbilityIconTests.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeAbilityIconWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeAbilityIconWidget.h)、[UI/Data/HodgeGameplayUIDataSource.h](../../../Source/Hodgepodge/Public/UI/Data/HodgeGameplayUIDataSource.h)

定义候选（多行签名仅展示首行）：

- L9: `bool FHodgeAbilityIconCooldownTest::RunTest(const FString& Parameters)`
- L31: `bool FHodgeAbilityIconEnergyTest::RunTest(const FString& Parameters)`

## HodgeAbilityRelationshipTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAbilityRelationshipTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAbilityRelationshipTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilityTagRelationshipMapping.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L47: `bool FHodgeRelationshipRuleTest::RunTest(const FString& Parameters)`
- L72: `bool FHodgeRelationshipRuntimeTest::RunTest(const FString& Parameters)`

## HodgeAnimNotifyTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAnimNotifyTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAnimNotifyTests.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h)

定义候选（多行签名仅展示首行）：

- L14: `bool FHodgeNotifyConfigurationTest::RunTest(const FString& Parameters)`
- L35: `bool FHodgeNotifyContextTest::RunTest(const FString& Parameters)`

## HodgeAttackRuleTestAbility.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAttackRuleTestAbility.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAttackRuleTestAbility.cpp)

定义候选（多行签名仅展示首行）：

- L6: `UHodgeAttackRuleTestAbility::UHodgeAttackRuleTestAbility()`
- L10: `bool UHodgeAttackRuleTestAbility::CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`
- L15: `void UHodgeAttackRuleTestAbility::ActivateAbility(FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo*, FGameplayAbilityActivationInfo, const FGameplayEventData*) {}`

## HodgeAttackRuleTestAbility.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAttackRuleTestAbility.h](../../../Source/Hodgepodge/Private/Tests/HodgeAttackRuleTestAbility.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
   4: #include "HodgeAttackRuleTestAbility.generated.h"
   7: UCLASS(Transient, NotBlueprintable)
   8: class UHodgeAttackRuleTestAbility : public UHodgeGameplayAbility_Melee
   9: {
  10: 	GENERATED_BODY()
  11: public:
  12: 	UHodgeAttackRuleTestAbility();
  13: 	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
  14: 		const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* RelevantTags) const override;
  15: protected:
  16: 	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
  17: 		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
  18: };
```

## HodgeAttackWarpTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAttackWarpTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAttackWarpTests.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeAttackFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackFacingTypes.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Component/HodgeAttackMotionWarpingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeAttackMotionWarpingComponent.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Animation/HodgeRootMotionModifier_AttackFacing.h](../../../Source/Hodgepodge/Public/Animation/HodgeRootMotionModifier_AttackFacing.h)、[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)

定义候选（多行签名仅展示首行）：

- L70: `bool FHodgeAttackWarpConfigTest::RunTest(const FString&)`
- L80: `bool FHodgeAttackWarpYawTest::RunTest(const FString&)`
- L88: `bool FHodgeAttackWarpWindowTest::RunTest(const FString&)`
- L97: `bool FHodgeAttackWarpLeaseTest::RunTest(const FString&)`
- L109: `bool FHodgeAttackWarpRootTest::RunTest(const FString&)`
- L122: `bool FHodgeAttackWarpProtocolTest::RunTest(const FString&)`
- L133: `bool FHodgeAttackWarpReplayTest::RunTest(const FString&)`
- L146: `bool FHodgeAttackWarpValidationTest::RunTest(const FString&)`
- L157: `bool FHodgeAttackWarpRetiredStateTest::RunTest(const FString&)`

## HodgeAttackWarpTestTypes.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAttackWarpTestTypes.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAttackWarpTestTypes.cpp)

定义候选（多行签名仅展示首行）：

- L4: `bool UHodgeAttackWarpTestAbility::CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`
- L7: `void UHodgeAttackWarpTestAbility::ActivateAbility(FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo*, FGameplayAbilityActivationInfo, const FGameplayEventData*) {}`

## HodgeAttackWarpTestTypes.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAttackWarpTestTypes.h](../../../Source/Hodgepodge/Private/Tests/HodgeAttackWarpTestTypes.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
   4: #include "HodgeAttackWarpTestTypes.generated.h"
   5: UCLASS(Transient, NotBlueprintable)
   6: class UHodgeAttackWarpTestAbility : public UHodgeGameplayAbility_Melee
   7: {
   8: 	GENERATED_BODY()
   9: public:
  10: 	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
  11: 		const FGameplayTagContainer* Source, const FGameplayTagContainer* Target, FGameplayTagContainer* Relevant) const override;
  12: protected:
  13: 	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
  14: 		FGameplayAbilityActivationInfo Activation, const FGameplayEventData* Payload) override;
  15: };
```

## HodgeAttributeGrowthTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAttributeGrowthTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAttributeGrowthTests.cpp)

项目内直接 include（不是运行调用关系）：[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)、[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)、[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/Stats/HodgeAttributeCoordinator.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeCoordinator.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Data/HodgeCharacterStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h)、[Data/HodgeEquipmentStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeEquipmentStatProfile.h)

定义候选（多行签名仅展示首行）：

- L121: `bool FHodgeStatProfileTest::RunTest(const FString& Parameters)`
- L143: `bool FHodgeStatGrowthTest::RunTest(const FString& Parameters)`
- L181: `bool FHodgeStatReentryTest::RunTest(const FString& Parameters)`
- L211: `bool FHodgeStatAvatarTest::RunTest(const FString& Parameters)`

## HodgeCharacterFacingTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeCharacterFacingTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeCharacterFacingTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h)、[Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Combat/HodgeCharacterFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeCharacterFacingTypes.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)、[Combat/HodgeMovementActionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeMovementActionTypes.h)、[Data/HodgeSprintAbilityProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)

定义候选（多行签名仅展示首行）：

- L92: `bool FHodgeFacingAuthorityActionTest::RunTest(const FString& Parameters)`
- L117: `bool FHodgeFacingInputTest::RunTest(const FString& Parameters)`
- L137: `bool FHodgeFacingModesTest::RunTest(const FString& Parameters)`
- L164: `bool FHodgeFacingActionTest::RunTest(const FString& Parameters)`
- L198: `bool FHodgeFacingLifecycleTest::RunTest(const FString& Parameters)`
- L231: `bool FHodgeFacingActivationPairTest::RunTest(const FString& Parameters)`
- L254: `bool FHodgeFacingReplayTest::RunTest(const FString& Parameters)`
- L286: `bool FHodgeFacingAnimationTest::RunTest(const FString& Parameters)`
- L304: `bool FHodgeDashInstantFacingTest::RunTest(const FString&)`

## HodgeCharacterRotationTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeCharacterRotationTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeCharacterRotationTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)

定义候选（多行签名仅展示首行）：

- L19: `bool FHodgeCharacterRotationConstraintsTest::RunTest(const FString& Parameters)`

## HodgeCinematicCameraTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeCinematicCameraTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeCinematicCameraTests.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeCinematicCameraProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCinematicCameraProfile.h)、[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Camera/HodgeCameraMode_Cinematic.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_Cinematic.h)、[Camera/HodgeCameraMode_ThirdPerson.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_ThirdPerson.h)

定义候选（多行签名仅展示首行）：

- L32: `bool FHodgeCinematicSchemaTest::RunTest(const FString& Parameters)`
- L61: `bool FHodgeCinematicHeroLeaseTest::RunTest(const FString& Parameters)`
- L79: `bool FHodgeCinematicSamplingTest::RunTest(const FString& Parameters)`
- L102: `bool FHodgeCinematicHandleTest::RunTest(const FString& Parameters)`

## HodgeCombatActionRequestTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeCombatActionRequestTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeCombatActionRequestTests.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)、[AI/HodgeEnemyAIController.h](../../../Source/Hodgepodge/Public/AI/HodgeEnemyAIController.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Data/HodgeCharacterStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)

定义候选（多行签名仅展示首行）：

- L173: `bool FHodgeActionLifecycleTest::RunTest(const FString&)`
- L195: `bool FHodgeStandaloneMovePolicyTest::RunTest(const FString&)`
- L224: `bool FHodgeStandaloneMoveValidationTest::RunTest(const FString&)`
- L254: `bool FHodgeStandaloneMoveIdentityTest::RunTest(const FString&)`
- L267: `bool FHodgeStandaloneMoveConfigTest::RunTest(const FString&)`
- L283: `bool FHodgeActionIntentTest::RunTest(const FString&)`
- L308: `bool FHodgeActionAIRouteTest::RunTest(const FString&)`
- L320: `bool FHodgeActionSynchronousTest::RunTest(const FString&)`
- L344: `bool FHodgeActionFailuresTest::RunTest(const FString&)`
- L361: `bool FHodgeActionHistoryTest::RunTest(const FString&)`
- L375: `bool FHodgeActionMoveResolutionTest::RunTest(const FString&)`
- L402: `bool FHodgeVerifyHeldDeath::Update()`
- L415: `bool FHodgeActionHeldDeathTest::RunTest(const FString&)`

## HodgeCombatSeparationTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeCombatSeparationTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeCombatSeparationTests.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeAttackRule.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackRule.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Data/HodgeHavocAttackRule.h](../../../Source/Hodgepodge/Public/Data/HodgeHavocAttackRule.h)、[Data/HodgeResonanceAttackRule.h](../../../Source/Hodgepodge/Public/Data/HodgeResonanceAttackRule.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Component/HodgeHitDetectionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHitDetectionComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)、[AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)、[AbilitySystem/Effects/HodgeResonanceEnergyEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h)、[AbilitySystem/Effects/HodgeHavocEffects.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeHavocEffects.h)

定义候选（多行签名仅展示首行）：

- L79: `bool FHodgeDetectionOwnerCleanupTest::RunTest(const FString&)`
- L103: `bool FHodgeAttackRulePhaseTest::RunTest(const FString&)`
- L128: `bool FHodgeAttackRuleReentryTest::RunTest(const FString&)`
- L140: `bool FHodgeAttackRuleCompatibilityTest::RunTest(const FString&)`
- L152: `bool FHodgeAttackRuleCastTest::RunTest(const FString&)`
- L163: `bool FHodgeAttackRuleCancellationTest::RunTest(const FString&)`

## HodgeDashAirTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeDashAirTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeDashAirTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Component/HodgeLocomotionPolicyComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)

定义候选（多行签名仅展示首行）：

- L67: `bool FHodgeDashAirEligibilityTest::RunTest(const FString&)`
- L90: `bool FHodgeDashAirConstraintsTest::RunTest(const FString&)`
- L110: `bool FHodgeDashAirFinishPolicyTest::RunTest(const FString&)`
- L136: `bool FHodgeDashAirPhysicsTest::RunTest(const FString&)`

## HodgeEnemyAITests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeEnemyAITests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeEnemyAITests.cpp)

项目内直接 include（不是运行调用关系）：[AI/HodgeEnemyAIController.h](../../../Source/Hodgepodge/Public/AI/HodgeEnemyAIController.h)、[AI/HodgeBTTask_ExecuteAbility.h](../../../Source/Hodgepodge/Public/AI/HodgeBTTask_ExecuteAbility.h)、[Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Data/HodgeCharacterStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)、[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)

定义候选（多行签名仅展示首行）：

- L155: `bool FHodgeEnemyIdempotencyTest::RunTest(const FString&)`
- L173: `bool FHodgeEnemyASCConflictTest::RunTest(const FString&)`
- L190: `bool FHodgeEnemyMissingStatsTest::RunTest(const FString&)`
- L202: `bool FHodgeEnemyInvalidLevelTest::RunTest(const FString&)`
- L211: `bool FHodgeEnemyLateDependenciesTest::RunTest(const FString&)`
- L228: `bool FHodgeEnemyBindingInvalidationTest::RunTest(const FString&)`
- L242: `bool FHodgeEnemyGrantPreflightTest::RunTest(const FString&)`
- L256: `bool FHodgeEnemyGrantRebindTest::RunTest(const FString&)`
- L268: `bool FHodgeEnemyEquipmentFailureTest::RunTest(const FString&)`
- L289: `bool FHodgeEnemyBrainLifecycleTest::RunTest(const FString&)`
- L303: `bool FHodgeEnemyControlTest::RunTest(const FString&)`
- L318: `bool FHodgeEnemyLateCallbackTest::RunTest(const FString&)`
- L328: `bool FHodgeEnemyComponentRemovalTest::RunTest(const FString&)`
- L342: `bool FHodgeEnemyInstantFailureTest::RunTest(const FString&)`
- L359: `bool FHodgeEnemyTargetKeyTest::RunTest(const FString&)`
- L391: `bool FHodgeEnemyClearedIdentityTest::RunTest(const FString&)`

## HodgeEnemyAITestSupport.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeEnemyAITestSupport.h](../../../Source/Hodgepodge/Private/Tests/HodgeEnemyAITestSupport.h)

项目内直接 include（不是运行调用关系）：[Component/HodgeEnemyInitComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeEnemyInitComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   5: #include "Component/HodgeEnemyInitComponent.h"
   6: #include "Components/GameFrameworkComponentManager.h"
   7: #include "AbilitySystem/HodgeGameplayTags.h"
   8: #include "Engine/GameInstance.h"
   9: #include "Engine/Engine.h"
  10: #include "Engine/World.h"
  13: struct FHodgeEnemyInitTestAccess
  14: {
  18: 	static void WithoutExperience(UHodgeEnemyInitComponent* Init) { Init->bRequireExperience = false; }
  22: 	static void AwaitExperience(UHodgeEnemyInitComponent* Init) { Init->bRequireExperience = true; Init->bExperienceReady = false; }
  27: 	static UWorld* CreateWorld(UGameInstance*& Instance)
  28: 	{
  29: 		Instance = NewObject<UGameInstance>(GEngine); Instance->InitializeStandalone();
  30: 		auto* Manager = Instance->GetSubsystem<UGameFrameworkComponentManager>();
  31: 		Manager->RegisterInitState(HodgeGameplayTags::InitState_Spawned, false, FGameplayTag());
  32: 		Manager->RegisterInitState(HodgeGameplayTags::InitState_DataAvailable, false, HodgeGameplayTags::InitState_Spawned);
  33: 		Manager->RegisterInitState(HodgeGameplayTags::InitState_DataInitialized, false, HodgeGameplayTags::InitState_DataAvailable);
  34: 		Manager->RegisterInitState(HodgeGameplayTags::InitState_GameplayReady, false, HodgeGameplayTags::InitState_DataInitialized);
  35: 		return Instance->GetWorld();
  36: 	}
  41: 	static void DestroyWorld(UGameInstance* Instance, UWorld* World)
  42: 	{
  43: 		Instance->Shutdown(); World->DestroyWorld(false); GEngine->DestroyWorldContext(World);
  44: 	}
  45: };
```

## HodgeEnemyAITestTypes.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeEnemyAITestTypes.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeEnemyAITestTypes.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)

定义候选（多行签名仅展示首行）：

- L12: `void UHodgeEnemyTestAbilitySet::AddAbility(TSubclassOf<UHodgeGameplayAbility> Class)`
- L17: `void UHodgeEnemyTestAbilitySet::AddEffect(TSubclassOf<UGameplayEffect> Class)`
- L22: `void UHodgeEnemyTestAbilitySet::AddAttribute(TSubclassOf<UAttributeSet> Class)`
- L27: `UHodgeOwnerOnlyOnSpawnTestAbility::UHodgeOwnerOnlyOnSpawnTestAbility()`
- L33: `UHodgeEnemyTestEquipment::UHodgeEnemyTestEquipment() { InstanceType = UHodgeWeaponInstance::StaticClass(); }`
- L35: `void UHodgeEnemyRebindOnGrantAbility::OnGiveAbility(const FGameplayAbilityActorInfo* Info, const FGameplayAbilitySpec& Spec)`
- L43: `UHodgeEnemyOnSpawnTestAbility::UHodgeEnemyOnSpawnTestAbility() { ActivationPolicy = EHodgeAbilityActivationPolicy::OnSpawn; }`
- L45: `AHodgeEnemyEquipmentTestActor::AHodgeEnemyEquipmentTestActor()`
- L50: `void AHodgeEnemyEquipmentFailActor::OnConstruction(const FTransform& Transform)`
- L56: `UHodgeEnemyInstantTestEffect::UHodgeEnemyInstantTestEffect()`

## HodgeEnemyAITestTypes.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeEnemyAITestTypes.h](../../../Source/Hodgepodge/Private/Tests/HodgeEnemyAITestTypes.h)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)、[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)、[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   5: #include "Data/HodgeAbilitySet.h"
   6: #include "Equipment/HodgeEquipmentDefinition.h"
   7: #include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
   8: #include "GameplayEffect.h"
   9: #include "GameFramework/Actor.h"
  10: #include "HodgeRelationshipTestAbility.h"
  11: #include "HodgeEnemyAITestTypes.generated.h"
  14: UCLASS(Transient, NotBlueprintable)
  15: class UHodgeEnemyTestAbilitySet : public UHodgeAbilitySet
  16: {
  17: 	GENERATED_BODY()
  18: public:
  22: 	void AddAbility(TSubclassOf<UHodgeGameplayAbility> Class);
  26: 	void AddEffect(TSubclassOf<UGameplayEffect> Class);
  28: 	void AddAttribute(TSubclassOf<UAttributeSet> Class);
  29: };
  32: UCLASS(Transient, NotBlueprintable)
  33: class UHodgeOwnerOnlyOnSpawnTestAbility : public UHodgeRelationshipTestAbility
  34: {
  35: 	GENERATED_BODY()
  36: public:
  37: 	UHodgeOwnerOnlyOnSpawnTestAbility();
  38: };
  41: UCLASS(Transient, NotBlueprintable)
  42: class UHodgeEnemyTestEquipment : public UHodgeEquipmentDefinition
  43: {
  44: 	GENERATED_BODY()
  45: public:
  47: 	UHodgeEnemyTestEquipment();
  48: };
  51: UCLASS(Transient, NotBlueprintable)
  52: class UHodgeEnemyRebindOnGrantAbility : public UHodgeGameplayAbility
  53: {
  54: 	GENERATED_BODY()
  55: protected:
  60: 	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* Info, const FGameplayAbilitySpec& Spec) override;
  61: };
  64: UCLASS(Transient, NotBlueprintable)
  65: class UHodgeEnemyOnSpawnTestAbility : public UHodgeGameplayAbility
  66: {
  67: 	GENERATED_BODY()
  68: public:
  70: 	UHodgeEnemyOnSpawnTestAbility();
  71: };
  74: UCLASS(Transient, NotBlueprintable)
  75: class AHodgeEnemyEquipmentTestActor : public AActor
  76: {
  77: 	GENERATED_BODY()
  78: public:
  80: 	AHodgeEnemyEquipmentTestActor();
  81: };
  84: UCLASS(Transient, NotBlueprintable)
  85: class UHodgeEnemyInstantTestEffect : public UGameplayEffect
  86: {
  87: 	GENERATED_BODY()
  88: public:
  90: 	UHodgeEnemyInstantTestEffect();
  91: };
  94: UCLASS(Transient, NotBlueprintable)
  95: class AHodgeEnemyEquipmentFailActor : public AHodgeEnemyEquipmentTestActor
  96: {
  97: 	GENERATED_BODY()
  98: protected:
 102: 	virtual void OnConstruction(const FTransform& Transform) override;
 103: };
```

## HodgeGrantContractTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeGrantContractTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeGrantContractTests.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_AddAbilities.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddAbilities.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)、[Data/HodgeGameData.h](../../../Source/Hodgepodge/Public/Data/HodgeGameData.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)

定义候选（多行签名仅展示首行）：

- L85: `bool FHodgeGameDataLifetimeTest::RunTest(const FString&)`
- L100: `bool FHodgeDynamicTagContractTest::RunTest(const FString&)`
- L112: `bool FHodgeSelfDestructContractTest::RunTest(const FString&)`
- L136: `bool FHodgeOwnerOnlyGrantTest::RunTest(const FString&)`
- L158: `bool FHodgeGrantBatchAtomicTest::RunTest(const FString&)`
- L181: `bool FHodgePlayerBaseGrantTest::RunTest(const FString&)`
- L212: `bool FHodgeGFOverlappingEntriesTest::RunTest(const FString&)`
- L234: `bool FHodgeGFPreflightTest::RunTest(const FString&)`
- L260: `bool FHodgeGFWithdrawalTest::RunTest(const FString&)`
- L279: `bool FHodgeGFLateBindingTest::RunTest(const FString&)`
- L300: `bool FHodgeGFBindingLifecycleTest::RunTest(const FString&)`

## HodgeHavocTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeHavocTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeHavocTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[AbilitySystem/Effects/HodgeHavocEffects.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeHavocEffects.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[Combat/HodgeHavocRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeHavocRules.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Data/HodgeHavocAttackRule.h](../../../Source/Hodgepodge/Public/Data/HodgeHavocAttackRule.h)、[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)、[Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)、[Animation/HodgeAnimNotify_DarkSurgeCommit.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotify_DarkSurgeCommit.h)、[UI/Data/HodgeGameplayUIDataSource.h](../../../Source/Hodgepodge/Public/UI/Data/HodgeGameplayUIDataSource.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)

定义候选（多行签名仅展示首行）：

- L71: `bool FHodgeHavocResourceTest::RunTest(const FString& Parameters)`
- L88: `bool FHodgeHavocFuelTest::RunTest(const FString& Parameters)`
- L104: `bool FHodgeHavocProcTest::RunTest(const FString& Parameters)`
- L118: `bool FHodgeHavocEffectTest::RunTest(const FString& Parameters)`
- L137: `bool FHodgeHavocHoldTest::RunTest(const FString& Parameters)`
- L153: `bool FHodgeHavocVariantTest::RunTest(const FString& Parameters)`
- L186: `bool FHodgeHavocSessionTest::RunTest(const FString& Parameters)`
- L258: `bool FHodgeHavocAssetsTest::RunTest(const FString& Parameters)`
- L301: `bool FHodgeHavocDebugFillTest::RunTest(const FString& Parameters)`

## HodgeHitDetectionTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeHitDetectionTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeHitDetectionTests.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)

定义候选（多行签名仅展示首行）：

- L15: `bool FHodgeHitGeometryTest::RunTest(const FString& Parameters)`
- L42: `bool FHodgeHitProfileValidationTest::RunTest(const FString& Parameters)`

## HodgeHitReactionTestAbility.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeHitReactionTestAbility.h](../../../Source/Hodgepodge/Private/Tests/HodgeHitReactionTestAbility.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   5: #include "AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h"
   6: #include "HodgeHitReactionTestAbility.generated.h"
   9: UCLASS(Transient, NotBlueprintable)
  10: class UHodgeHitReactionTestAbility : public UHodgeGameplayAbility_Jump
  11: {
  12: 	GENERATED_BODY()
  13: };
```

## HodgeHitReactionTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeHitReactionTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeHitReactionTests.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)、[Component/HodgeHitReactionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHitReactionComponent.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Data/HodgeHitReactionProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeHitReactionProfile.h)、[Data/HodgeCharacterStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h)

定义候选（多行签名仅展示首行）：

- L137: `bool FHodgeReactionRanksTest::RunTest(const FString& Parameters)`
- L166: `bool FHodgeReactionSettlementTest::RunTest(const FString& Parameters)`
- L190: `bool FHodgeReactionImmuneTest::RunTest(const FString& Parameters)`
- L206: `bool FHodgeReactionLifecycleTest::RunTest(const FString& Parameters)`
- L245: `bool FHodgeReactionFallbackTest::RunTest(const FString& Parameters)`
- L266: `bool FHodgeReactionMotionTest::RunTest(const FString& Parameters)`
- L288: `bool FHodgeReactionCancellationTest::RunTest(const FString& Parameters)`
- L316: `bool FHodgeReactionEnemyTest::RunTest(const FString& Parameters)`

## HodgeLockOnIndicatorTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeLockOnIndicatorTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeLockOnIndicatorTests.cpp)

项目内直接 include（不是运行调用关系）：[UI/Targeting/HodgeLockOnIndicatorWidget.h](../../../Source/Hodgepodge/Public/UI/Targeting/HodgeLockOnIndicatorWidget.h)

定义候选（多行签名仅展示首行）：

- L10: `bool FHodgeLockOnIndicatorDesignerTest::RunTest(const FString&)`

## HodgeLockOnTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeLockOnTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeLockOnTests.cpp)

项目内直接 include（不是运行调用关系）：[Targeting/HodgeLockOnTypes.h](../../../Source/Hodgepodge/Public/Targeting/HodgeLockOnTypes.h)、[Component/HodgeLockOnComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[Data/HodgeLockOnProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeLockOnProfile.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Character/HodgeHeroCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h)、[Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)、[Combat/HodgeMovementActionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeMovementActionTypes.h)、[Combat/HodgeDamageRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeDamageRules.h)、[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[Camera/HodgeCameraMode_LockOn.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_LockOn.h)、[Camera/HodgeCameraMode_ThirdPerson.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_ThirdPerson.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)

定义候选（多行签名仅展示首行）：

- L93: `bool FHodgeLockOnGestureTest::RunTest(const FString&)`
- L109: `bool FHodgeLockOnProfileTest::RunTest(const FString&)`
- L128: `bool FHodgeLockOnAuthorityTest::RunTest(const FString&)`
- L155: `bool FHodgeLockOnRecoveryTest::RunTest(const FString&)`
- L175: `bool FHodgeLockOnMotionTest::RunTest(const FString&)`
- L194: `bool FHodgeLockOnRangeTest::RunTest(const FString&)`
- L209: `bool FHodgeLockOnGASTest::RunTest(const FString&)`
- L219: `bool FHodgeLockOnDeathTest::RunTest(const FString&)`
- L231: `bool FHodgeLockOnCacheTest::RunTest(const FString&)`
- L252: `bool FHodgeSoftLockCameraTest::RunTest(const FString&)`
- L282: `bool FHodgeLockCameraAcquireTest::RunTest(const FString&)`
- L305: `bool FHodgeLockCameraTrackingTest::RunTest(const FString&)`
- L327: `bool FHodgeLockCameraRecoveryTest::RunTest(const FString&)`
- L344: `bool FHodgeLockOnCameraStackTest::RunTest(const FString&)`

## HodgeLockOnTestTypes.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeLockOnTestTypes.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeLockOnTestTypes.cpp)

定义候选（多行签名仅展示首行）：

- L4: `UHodgeCameraMode_LockOnTestFree::UHodgeCameraMode_LockOnTestFree()`

## HodgeLockOnTestTypes.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeLockOnTestTypes.h](../../../Source/Hodgepodge/Private/Tests/HodgeLockOnTestTypes.h)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCameraMode_ThirdPerson.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_ThirdPerson.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "Camera/HodgeCameraMode_ThirdPerson.h"
   4: #include "HodgeLockOnTestTypes.generated.h"
   7: UCLASS(NotBlueprintable)
   8: class UHodgeCameraMode_LockOnTestFree : public UHodgeCameraMode_ThirdPerson
   9: {
  10: 	GENERATED_BODY()
  11: public:
  12: 	UHodgeCameraMode_LockOnTestFree();
  13: };
```

## HodgeMeleeRoutingTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeMeleeRoutingTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeMeleeRoutingTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Component/HodgeHitDetectionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHitDetectionComponent.h)

定义候选（多行签名仅展示首行）：

- L111: `bool FHodgeDetectionRoutingTest::RunTest(const FString& Parameters)`
- L142: `bool FHodgeMeleeHitHistoryTest::RunTest(const FString& Parameters)`
- L167: `bool FHodgeMeleeSpecContextTest::RunTest(const FString& Parameters)`

## HodgeMovementActionTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeMovementActionTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeMovementActionTests.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeMovementActionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeMovementActionTypes.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Component/HodgeLocomotionPolicyComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h)、[Component/HodgeDefenseComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeDefenseComponent.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Data/HodgeSprintAbilityProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h)、[Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)

定义候选（多行签名仅展示首行）：

- L41: `bool FHodgeDashDirectionsTest::RunTest(const FString& Parameters)`
- L66: `bool FHodgeSprintSessionTest::RunTest(const FString& Parameters)`
- L84: `bool FHodgeMovementTargetDataTest::RunTest(const FString& Parameters)`
- L99: `bool FHodgeMovementPolicyTest::RunTest(const FString& Parameters)`
- L178: `bool FHodgeMovementTimingTest::RunTest(const FString& Parameters)`
- L210: `bool FHodgeDashFootIKTest::RunTest(const FString& Parameters)`
- L225: `bool FHodgeSprintPivotTest::RunTest(const FString& Parameters)`
- L254: `bool FHodgeMovementDefenseTest::RunTest(const FString& Parameters)`

## HodgeRelationshipTestAbility.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeRelationshipTestAbility.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeRelationshipTestAbility.cpp)

定义候选（多行签名仅展示首行）：

- L8: `UHodgeRelationshipTestAbility::UHodgeRelationshipTestAbility()`
- L11: `void UHodgeRelationshipTestAbility::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,`
- L16: `UHodgeFacingPredictionTestAbility::UHodgeFacingPredictionTestAbility()`

## HodgeRelationshipTestAbility.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeRelationshipTestAbility.h](../../../Source/Hodgepodge/Private/Tests/HodgeRelationshipTestAbility.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   5: #include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
   6: #include "HodgeRelationshipTestAbility.generated.h"
   9: UCLASS(Transient, NotBlueprintable)
  10: class UHodgeRelationshipTestAbility : public UHodgeGameplayAbility
  11: {
  12: 	GENERATED_BODY()
  13: public:
  15: 	UHodgeRelationshipTestAbility();
  16: protected:
  23: 	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
  24: 		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
  25: };
  28: UCLASS(Transient, NotBlueprintable)
  29: class UHodgeFacingPredictionTestAbility : public UHodgeRelationshipTestAbility
  30: {
  31: 	GENERATED_BODY()
  32: public:
  34: 	UHodgeFacingPredictionTestAbility();
  35: };
```

## HodgeResonanceEnergyTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeResonanceEnergyTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeResonanceEnergyTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)、[AbilitySystem/Effects/HodgeResonanceEnergyEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h)、[AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h)、[UI/Data/HodgeGameplayUIDataSource.h](../../../Source/Hodgepodge/Public/UI/Data/HodgeGameplayUIDataSource.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)

定义候选（多行签名仅展示首行）：

- L50: `bool FHodgeResonanceCostTest::RunTest(const FString& Parameters)`
- L77: `bool FHodgeResonancePhaseTest::RunTest(const FString& Parameters)`
- L94: `bool FHodgeResonancePlayerStateBootstrapTest::RunTest(const FString& Parameters)`
- L121: `bool FHodgeResonanceDebugFillTest::RunTest(const FString& Parameters)`

## HodgeUIDataSourceTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeUIDataSourceTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeUIDataSourceTests.cpp)

项目内直接 include（不是运行调用关系）：[UI/Data/HodgeGameplayUIDataSource.h](../../../Source/Hodgepodge/Public/UI/Data/HodgeGameplayUIDataSource.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L12: `bool FHodgeUIInactiveDataTest::RunTest(const FString& Parameters)`

## HodgeUIFoundationTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeUIFoundationTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeUIFoundationTests.cpp)

项目内直接 include（不是运行调用关系）：[UI/Foundation/HodgePrimaryGameLayout.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgePrimaryGameLayout.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L15: `bool FHodgeUIClosedRootTest::RunTest(const FString& Parameters)`
- L39: `bool FHodgeUINonHodgeInstanceTest::RunTest(const FString& Parameters)`

## HodgeWeaponPresentationTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeWeaponPresentationTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeWeaponPresentationTests.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)、[Equipment/HodgeWeaponPresentationActor.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationActor.h)、[Equipment/HodgeWeaponPresentationProfile.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)

定义候选（多行签名仅展示首行）：

- L42: `bool FHodgeWeaponRequestsTest::RunTest(const FString& Parameters)`
- L114: `bool FHodgeWeaponGeometryTest::RunTest(const FString& Parameters)`
- L171: `bool FHodgeWeaponEndBehaviorTest::RunTest(const FString& Parameters)`

## HodgeWingAttackPresentationTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeWingAttackPresentationTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeWingAttackPresentationTests.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeWingProfile.h](../../../Source/Hodgepodge/Public/Animation/HodgeWingProfile.h)、[Animation/HodgeAnimNotifyState_WingAction.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_WingAction.h)、[Component/HodgeWingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeWingComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L55: `bool FHodgeWingFadeStateTest::RunTest(const FString& Parameters)`
- L85: `bool FHodgeWingAnimationMappingTest::RunTest(const FString& Parameters)`
- L127: `bool FHodgeWingPresentationSessionTest::RunTest(const FString& Parameters)`
- L150: `bool FHodgeWingAttackIntegrationTest::RunTest(const FString& Parameters)`

## HodgeWingBoxHitTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeWingBoxHitTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeWingBoxHitTests.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)、[Animation/HodgeWingProfile.h](../../../Source/Hodgepodge/Public/Animation/HodgeWingProfile.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)

定义候选（多行签名仅展示首行）：

- L18: `bool FHodgeWingBoxHitTest::RunTest(const FString& Parameters)`

## HodgeWingTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeWingTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeWingTests.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeWingProfile.h](../../../Source/Hodgepodge/Public/Animation/HodgeWingProfile.h)、[Component/HodgeWingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeWingComponent.h)

定义候选（多行签名仅展示首行）：

- L13: `bool FHodgeWingEnvelopeTest::RunTest(const FString& Parameters)`
- L42: `bool FHodgeWingCancellationTest::RunTest(const FString& Parameters)`
- L68: `bool FHodgeWingAssetContractTest::RunTest(const FString& Parameters)`
