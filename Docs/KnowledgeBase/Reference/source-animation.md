# Animation 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAnimInstance.cpp

ASC 属性映射、GroundDistance 与旋转策略快照；FullBody 权重协调蓝图程序性根 Yaw。

源码：[Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)、[Component/HodgeLocomotionPolicyComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h)、[Data/HodgeSprintAbilityProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h)、[Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)

定义候选（多行签名仅展示首行）：

- L24: `UHodgeAnimInstance::UHodgeAnimInstance(const FObjectInitializer& ObjectInitializer)`
- L29: `void UHodgeAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)`
- L40: `EDataValidationResult UHodgeAnimInstance::IsDataValid(class FDataValidationContext& Context) const`
- L54: `void UHodgeAnimInstance::NativeInitializeAnimation()`
- L72: `void UHodgeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)`
- L142: `float UHodgeAnimInstance::ResolveFootIKAlpha(float FullBodyWeight, float DashWeight)`
- L149: `FName UHodgeAnimInstance::ResolveCycleSyncGroup(bool bSprint)`
- L153: `float UHodgeAnimInstance::AdvanceTurnTime(float Time, float DeltaTime, float Length, float Rate)`
- L160: `float UHodgeAnimInstance::MatchCycleRate(float Speed, float ReferenceSpeed, float BaseRate, float Minimum, float Maximum)`
- L167: `bool UHodgeAnimInstance::UpdateSprintPivot(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)`
- L184: `bool UHodgeAnimInstance::IsTurnComplete(float Elapsed, float Length, float Rate, float BlendOut)`
- L190: `bool UHodgeAnimInstance::UpdateSprintCycle(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)`
- L202: `bool UHodgeAnimInstance::ShouldExitSprintPivot()`

## HodgeAnimNotify_DarkSurgeCommit.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Animation/HodgeAnimNotify_DarkSurgeCommit.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeAnimNotify_DarkSurgeCommit.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeAnimNotify_DarkSurgeCommit.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotify_DarkSurgeCommit.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_ResonanceCircuit.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_ResonanceCircuit.h)

定义候选（多行签名仅展示首行）：

- L9: `UHodgeAnimNotify_DarkSurgeCommit::UHodgeAnimNotify_DarkSurgeCommit()`
- L17: `void UHodgeAnimNotify_DarkSurgeCommit::BranchingPointNotify(FBranchingPointNotifyPayload& Payload)`

## HodgeAnimNotifyState_CinematicCamera.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Animation/HodgeAnimNotifyState_CinematicCamera.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeAnimNotifyState_CinematicCamera.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeAnimNotifyState_CinematicCamera.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_CinematicCamera.h)、[Data/HodgeCinematicCameraProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCinematicCameraProfile.h)

定义候选（多行签名仅展示首行）：

- L6: `FString UHodgeAnimNotifyState_CinematicCamera::GetNotifyName_Implementation() const`

## HodgeAnimNotifyState_WingAction.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Animation/HodgeAnimNotifyState_WingAction.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeAnimNotifyState_WingAction.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeAnimNotifyState_WingAction.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_WingAction.h)、[Component/HodgeWingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeWingComponent.h)

定义候选（多行签名仅展示首行）：

- L12: `UHodgeAnimNotifyState_WingAction::UHodgeAnimNotifyState_WingAction()`
- L17: `bool UHodgeAnimNotifyState_WingAction::Validate(const USkeletalMesh* Mesh, TArray<FText>& Errors) const`
- L30: `float UHodgeAnimNotifyState_WingAction::EvaluateWingTime(float SourceTime, float WindowStart) const`
- L38: `FString UHodgeAnimNotifyState_WingAction::GetNotifyName_Implementation() const`
- L43: `void UHodgeAnimNotifyState_WingAction::RefreshOwner(USkeletalMeshComponent* Mesh) const`
- L53: `void UHodgeAnimNotifyState_WingAction::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float Duration, const FAnimNotifyEventReference& Reference)`
- L58: `void UHodgeAnimNotifyState_WingAction::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference)`
- L63: `void UHodgeAnimNotifyState_WingAction::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)`
- L68: `void UHodgeAnimNotifyState_WingAction::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)`

## HodgeCombatAnimNotifies.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Animation/HodgeCombatAnimNotifies.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeCombatAnimNotifies.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L39: `FHodgeAnimHitConfig::FHodgeAnimHitConfig()`
- L45: `bool FHodgeAnimHitConfig::Validate(TArray<FText>& Errors) const`
- L86: `UHodgeAnimNotifyState_HitCheck::UHodgeAnimNotifyState_HitCheck() { bIsNativeBranchingPoint = true; }`
- L89: `UHodgeAnimNotify_Hit::UHodgeAnimNotify_Hit()`
- L96: `UHodgeAnimNotifyState_GameplayTag::UHodgeAnimNotifyState_GameplayTag() { bIsNativeBranchingPoint = true; }`
- L98: `UHodgeAnimNotifyState_WeaponHand::UHodgeAnimNotifyState_WeaponHand() { bIsNativeBranchingPoint = true; }`
- L100: `UHodgeAnimNotify_GameplayEvent::UHodgeAnimNotify_GameplayEvent() { bIsNativeBranchingPoint = true; }`
- L103: `void UHodgeAnimNotifyState_HitCheck::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)`
- L113: `void UHodgeAnimNotifyState_HitCheck::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)`
- L119: `void UHodgeAnimNotify_Hit::BranchingPointNotify(FBranchingPointNotifyPayload& Payload)`
- L137: `void UHodgeAnimNotifyState_GameplayTag::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)`
- L147: `void UHodgeAnimNotifyState_GameplayTag::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)`
- L153: `void UHodgeAnimNotifyState_WeaponHand::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)`
- L163: `void UHodgeAnimNotifyState_WeaponHand::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)`
- L169: `void UHodgeAnimNotify_GameplayEvent::BranchingPointNotify(FBranchingPointNotifyPayload& Payload)`
- L192: `void UHodgeAnimNotifyState_HitCheck::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L200: `void UHodgeAnimNotifyState_HitCheck::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L208: `void UHodgeAnimNotifyState_GameplayTag::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L216: `void UHodgeAnimNotifyState_GameplayTag::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L224: `void UHodgeAnimNotifyState_WeaponHand::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L232: `void UHodgeAnimNotifyState_WeaponHand::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L240: `void UHodgeAnimNotify_Hit::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L248: `void UHodgeAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L275: `void UHodgeAnimNotifyState_HitCheck::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)`
- L281: `void UHodgeAnimNotify_Hit::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) { ConfigureGameplayNotify(Event); }`
- L284: `void UHodgeAnimNotifyState_GameplayTag::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)`
- L290: `void UHodgeAnimNotifyState_WeaponHand::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)`
- L296: `void UHodgeAnimNotify_GameplayEvent::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)`

## HodgeEnemyAnimInstance.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Animation/HodgeEnemyAnimInstance.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeEnemyAnimInstance.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeEnemyAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeEnemyAnimInstance.h)

定义候选（多行签名仅展示首行）：

- L9: `void UHodgeEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)`

## HodgeRootMotionModifier_AttackFacing.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Animation/HodgeRootMotionModifier_AttackFacing.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeRootMotionModifier_AttackFacing.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeRootMotionModifier_AttackFacing.h](../../../Source/Hodgepodge/Public/Animation/HodgeRootMotionModifier_AttackFacing.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)

定义候选（多行签名仅展示首行）：

- L8: `void UHodgeRootMotionModifier_AttackFacing::Update(const FMotionWarpingUpdateContext& Context)`

## HodgeWingAnimInstance.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Animation/HodgeWingAnimInstance.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeWingAnimInstance.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeWingAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeWingAnimInstance.h)、[Component/HodgeWingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeWingComponent.h)

定义候选（多行签名仅展示首行）：

- L10: `void UHodgeWingAnimInstance::NativeUpdateAnimation(float DeltaSeconds)`

## HodgeWingProfile.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Animation/HodgeWingProfile.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeWingProfile.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeWingProfile.h](../../../Source/Hodgepodge/Public/Animation/HodgeWingProfile.h)、[Animation/HodgeWingAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeWingAnimInstance.h)、[Animation/HodgeAnimNotifyState_WingAction.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_WingAction.h)

定义候选（多行签名仅展示首行）：

- L17: `void FHodgeWingFadeState::Show()`
- L23: `void FHodgeWingFadeState::BeginFade(float Seconds)`
- L33: `void FHodgeWingFadeState::Advance(float DeltaSeconds)`
- L43: `void FHodgeWingFadeState::Reset()`
- L48: `FHodgeWingActionHandle FHodgeWingPresentationState::Begin(int32 Binding, int32 Instance, FGuid Execution)`
- L64: `bool FHodgeWingPresentationState::End(FHodgeWingActionHandle EndingHandle)`
- L75: `void FHodgeWingPresentationState::Reset()`
- L85: `FHodgeWingPose FHodgeWingPose::Blend(const FHodgeWingPose& A, const FHodgeWingPose& B, float Alpha)`
- L96: `bool UHodgeWingProfile::HasValidTiming() const`
- L104: `FHodgeWingPose UHodgeWingProfile::Evaluate(float Position) const`
- L120: `bool UHodgeWingProfile::Validate(TArray<FText>& Errors) const`
- L179: `EDataValidationResult UHodgeWingProfile::IsDataValid(FDataValidationContext& Context) const`

## HodgeAnimInstance.h

ASC 属性映射、GroundDistance 与旋转策略快照；FullBody 权重协调蓝图程序性根 Yaw。

源码：[Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeCharacterFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeCharacterFacingTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "GameplayEffectTypes.h"
  10: #include "Animation/AnimInstance.h"
  11: #include "Combat/HodgeCharacterFacingTypes.h"
  12: #include "Animation/AnimExecutionContext.h"
  13: #include "Animation/AnimNodeReference.h"
  14: #include "HodgeAnimInstance.generated.h"
  19: UCLASS(Config = Game)
  20: class HODGEPODGE_API UHodgeAnimInstance : public UAnimInstance
  21: {
  22: 	GENERATED_BODY()
  24: public:
  29: 	UHodgeAnimInstance(const FObjectInitializer& ObjectInitializer);
  35: 	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);
  42: 	UFUNCTION(BlueprintCallable, Category="Hodge|Animation", meta=(BlueprintThreadSafe))
  43: 	bool UpdateSprintPivot(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
  49: 	UFUNCTION(BlueprintCallable, Category="Hodge|Animation", meta=(BlueprintThreadSafe))
  50: 	bool UpdateSprintCycle(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
  54: 	UFUNCTION(BlueprintPure, Category="Hodge|Animation", meta=(BlueprintThreadSafe))
  55: 	bool ShouldExitSprintPivot();
  63: 	static float AdvanceTurnTime(float Time, float DeltaTime, float Length, float Rate);
  71: 	static bool IsTurnComplete(float Elapsed, float Length, float Rate, float BlendOut);
  80: 	static float MatchCycleRate(float Speed, float ReferenceSpeed, float BaseRate, float Minimum, float Maximum);
  86: 	static float ResolveFootIKAlpha(float FullBodyWeight, float DashWeight);
  91: 	static FName ResolveCycleSyncGroup(bool bSprint);
  93: protected:
  94: #if WITH_EDITOR
 100: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
 101: #endif
 105: 	virtual void NativeInitializeAnimation() override;
 111: 	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
 113: protected:
 115: 	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
 116: 	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
 119: 	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
 120: 	float GroundDistance = -1.0f;
 123: 	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
 124: 	bool bSuppressLocomotionYaw = false;
 127: 	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
 128: 	bool bResetLocomotionYaw = false;
 131: 	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
 132: 	float LocomotionRootYawScale = 1.f;
 134: 	UPROPERTY(BlueprintReadOnly, Transient, Category="Character State Data")
 135: 	float LocomotionFootIKAlpha = 1.f;
 138: 	UPROPERTY(BlueprintReadOnly, Category="Character State Data")
 139: 	FHodgeFacingPresentationSnapshot FacingPresentation;
 141: 	UPROPERTY(BlueprintReadOnly, Category="Character State Data")
 142: 	bool bUseStrafeLocomotion = false;
 144: 	UPROPERTY(BlueprintReadOnly, Category="Character State Data")
 145: 	float StrafeLocomotionWeight = 0.f;
 147: 	UPROPERTY(BlueprintReadOnly, Category="Character State Data")
 148: 	bool bAllowLocomotionPivot = false;
 150: 	UPROPERTY(EditDefaultsOnly, Category="Character State Data", meta=(ClampMin="0", Units="s"))
 151: 	float FacingModeBlendTime = .12f;
 153: 	UPROPERTY(BlueprintReadOnly, Category="Character State Data") bool bSprinting = false;
 155: 	UPROPERTY(BlueprintReadOnly, Category="Character State Data") bool bSprintPivotAllowed = false;
 157: 	UPROPERTY(BlueprintReadOnly, Category="Character State Data") TObjectPtr<class UAnimSequence> SprintCycle;
 159: 	UPROPERTY(BlueprintReadOnly, Category="Character State Data") TObjectPtr<class UAnimSequence> SprintTurn;
 161: 	UPROPERTY(BlueprintReadOnly, Category="Character State Data") float SprintTurnPlayRate = 1.f;
 163: 	UPROPERTY(BlueprintReadOnly, Category="Character State Data") float SprintTurnBlendOutTime = .12f;
 165: 	UPROPERTY(BlueprintReadOnly, Category="Character State Data") float SprintCyclePlayRate = 1.f;
 166: };
```

## HodgeAnimNotify_DarkSurgeCommit.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Animation/HodgeAnimNotify_DarkSurgeCommit.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotify_DarkSurgeCommit.h)

项目内直接 include（不是运行调用关系）：[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "Animation/AnimNotifies/AnimNotify.h"
   4: #include "Animation/HodgeCombatAnimNotifies.h"
   5: #include "HodgeAnimNotify_DarkSurgeCommit.generated.h"
   8: UCLASS(meta=(DisplayName="Hodge 灭音提交"))
   9: class HODGEPODGE_API UHodgeAnimNotify_DarkSurgeCommit : public UAnimNotify
  10: {
  11: 	GENERATED_BODY()
  12: public:
  14: 	UHodgeAnimNotify_DarkSurgeCommit();
  15: 	UPROPERTY(EditAnywhere, Category="Hodge|Havoc") FHodgeAnimHitConfig Burst;
  17: 	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& Payload) override;
  18: };
```

## HodgeAnimNotifyState_CinematicCamera.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_CinematicCamera.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_CinematicCamera.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "Animation/AnimNotifies/AnimNotifyState.h"
   4: #include "HodgeAnimNotifyState_CinematicCamera.generated.h"
   5: class UHodgeCinematicCameraProfile;
   8: UCLASS(meta=(DisplayName="Hodge Cinematic Camera Window"))
   9: class HODGEPODGE_API UHodgeAnimNotifyState_CinematicCamera : public UAnimNotifyState
  10: {
  11: 	GENERATED_BODY()
  12: public:
  14: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Camera") TObjectPtr<UHodgeCinematicCameraProfile> Profile;
  16: 	virtual FString GetNotifyName_Implementation() const override;
  17: };
```

## HodgeAnimNotifyState_WingAction.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_WingAction.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_WingAction.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "Animation/AnimNotifies/AnimNotifyState.h"
   7: #include "HodgeAnimNotifyState_WingAction.generated.h"
   9: class UAnimMontage;
  10: class USkeletalMesh;
  13: UCLASS(meta=(DisplayName="Hodge Wing Action Window"))
  14: class HODGEPODGE_API UHodgeAnimNotifyState_WingAction : public UAnimNotifyState
  15: {
  16: 	GENERATED_BODY()
  17: public:
  19: 	UHodgeAnimNotifyState_WingAction();
  21: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing") TObjectPtr<UAnimMontage> WingMontage;
  23: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing", meta=(ClampMin="0.01")) float PlayRate = 1.f;
  25: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing", meta=(ClampMin="0", Units="s")) float StartPosition = 0.f;
  27: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing", meta=(ClampMin="-1", Units="s")) float FadeOutSeconds = -1.f;
  29: 	UPROPERTY() FName WindowId = TEXT("AttackWing");
  31: 	bool Validate(const USkeletalMesh* Mesh, TArray<FText>& Errors) const;
  33: 	float EvaluateWingTime(float SourceTime, float WindowStart) const;
  35: 	virtual FString GetNotifyName_Implementation() const override;
  37: 	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float Duration, const FAnimNotifyEventReference& Reference) override;
  39: 	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
  41: 	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
  43: 	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;
  44: private:
  46: 	void RefreshOwner(USkeletalMeshComponent* Mesh) const;
  47: };
```

## HodgeCombatAnimNotifies.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)、[Equipment/HodgeWeaponPresentationTypes.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "Animation/AnimNotifies/AnimNotify.h"
   7: #include "Animation/AnimNotifies/AnimNotifyState.h"
   8: #include "Combat/HodgeHitDetection.h"
   9: #include "Equipment/HodgeWeaponPresentationTypes.h"
  10: #include "HodgeCombatAnimNotifies.generated.h"
  13: UENUM(BlueprintType)
  14: enum class EHodgeAnimHitSource : uint8 {                 CharacterMeshSocket,                     EquippedWeapon,                  AvatarRoot,                  NamedComponent,               ExecutionAnchor,               ExecutionTarget };
  17: USTRUCT(BlueprintType)
  18: struct HODGEPODGE_API FHodgeAnimHitConfig
  19: {
  20: 	GENERATED_BODY()
  22: 	FHodgeAnimHitConfig();
  24: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeAnimHitSource Source = EHodgeAnimHitSource::CharacterMeshSocket;
  26: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source==EHodgeAnimHitSource::CharacterMeshSocket || Source==EHodgeAnimHitSource::NamedComponent", EditConditionHides)) FName BoneOrSocket;
  28: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Combat.Source.Weapon", EditCondition="Source==EHodgeAnimHitSource::EquippedWeapon", EditConditionHides)) FGameplayTag WeaponSourceTag;
  30: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source==EHodgeAnimHitSource::NamedComponent", EditConditionHides)) FName ComponentName;
  32: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon", EditConditionHides)) EHodgeHitShape Shape = EHodgeHitShape::Sphere;
  34: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1", Units="cm", EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon && Shape!=EHodgeHitShape::Box", EditConditionHides)) float Radius = 15.f;
  36: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon && Shape==EHodgeHitShape::Box", EditConditionHides)) FVector BoxHalfExtent = FVector(100.f);
  38: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1", Units="cm", EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon && Shape==EHodgeHitShape::Capsule", EditConditionHides)) float CapsuleHalfHeight = 100.f;
  40: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon", EditConditionHides)) FTransform LocalTransform = FTransform::Identity;
  42: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon", EditConditionHides)) EHodgeHitTransformPolicy TransformPolicy = EHodgeHitTransformPolicy::Follow;
  44: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay) TObjectPtr<UHodgeHitDetectionProfile> Profile;
  46: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source==EHodgeAnimHitSource::ExecutionAnchor", EditConditionHides)) FName AnchorKey;
  48: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitTargetPolicy TargetPolicy = EHodgeHitTargetPolicy::AnyInVolume;
  50: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="TargetPolicy!=EHodgeHitTargetPolicy::AnyInVolume || Source==EHodgeAnimHitSource::ExecutionTarget", EditConditionHides)) FName TargetKey;
  52: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, meta=(ClampMin="1", EditCondition="TargetPolicy!=EHodgeHitTargetPolicy::AnyInVolume || Source==EHodgeAnimHitSource::ExecutionTarget", EditConditionHides)) float MaxTargetDistance = 2000.f;
  54: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, meta=(ClampMin="1")) float MaxAnchorDistance = 5000.f;
  56: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bUseDefaultDamage = true;
  58: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction") bool bUseDefaultReaction = true;
  60: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction", meta=(EditCondition="!bUseDefaultReaction", EditConditionHides)) FHodgeHitReactionConfig ReactionOverride;
  62: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bUseDefaultDamage", EditConditionHides)) FHodgeHitEffectConfig Damage;
  64: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float DamageScale = 1.f;
  66: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float RepeatHitInterval = 0.f;
  68: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay) FName HitGroup;
  70: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay) FName AttackPhase;
  72: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay) bool bAllowFriendlyFire = false;
  77: 	bool Validate(TArray<FText>& Errors) const;
  78: };
  81: UCLASS(meta=(DisplayName="Hodge 持续命中"))
  82: class HODGEPODGE_API UHodgeAnimNotifyState_HitCheck : public UAnimNotifyState
  83: {
  84: 	GENERATED_BODY()
  85: public:
  87: 	UHodgeAnimNotifyState_HitCheck();
  88: #if WITH_EDITOR
  92: 	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
  93: #endif
  95: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hit") FHodgeAnimHitConfig Hit;
 102: 	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& Reference) override;
 108: 	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
 112: 	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
 116: 	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;
 117: };
 120: UCLASS(meta=(DisplayName="Hodge 单次命中"))
 121: class HODGEPODGE_API UHodgeAnimNotify_Hit : public UAnimNotify
 122: {
 123: 	GENERATED_BODY()
 124: public:
 126: 	UHodgeAnimNotify_Hit();
 127: #if WITH_EDITOR
 131: 	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
 132: #endif
 134: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hit") FHodgeAnimHitConfig Hit;
 136: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category="Hit") TArray<FHodgeAnimHitConfig> AdditionalHits;
 142: 	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
 146: 	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& Payload) override;
 147: };
 150: UCLASS(meta=(DisplayName="Hodge 状态区间"))
 151: class HODGEPODGE_API UHodgeAnimNotifyState_GameplayTag : public UAnimNotifyState
 152: {
 153: 	GENERATED_BODY()
 154: public:
 156: 	UHodgeAnimNotifyState_GameplayTag();
 157: #if WITH_EDITOR
 161: 	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
 162: #endif
 164: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="State") FGameplayTag StateTag;
 171: 	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& Reference) override;
 177: 	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
 181: 	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
 185: 	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;
 186: };
 189: UCLASS(meta=(DisplayName="Hodge 武器手持区间"))
 190: class HODGEPODGE_API UHodgeAnimNotifyState_WeaponHand : public UAnimNotifyState
 191: {
 192: 	GENERATED_BODY()
 193: public:
 195: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon", meta=(DisplayName="结束方式"))
 196: 	EHodgeWeaponHandEndBehavior EndBehavior = EHodgeWeaponHandEndBehavior::ReturnToBack;
 198: 	UHodgeAnimNotifyState_WeaponHand();
 199: #if WITH_EDITOR
 203: 	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
 204: #endif
 211: 	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& Reference) override;
 217: 	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
 221: 	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
 225: 	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;
 226: };
 229: UCLASS(meta=(DisplayName="Hodge 玩法消息"))
 230: class HODGEPODGE_API UHodgeAnimNotify_GameplayEvent : public UAnimNotify
 231: {
 232: 	GENERATED_BODY()
 233: public:
 235: 	UHodgeAnimNotify_GameplayEvent();
 236: #if WITH_EDITOR
 240: 	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
 241: #endif
 243: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Event") FGameplayTag EventTag;
 249: 	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
 253: 	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& Payload) override;
 254: };
```

## HodgeEnemyAnimInstance.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Animation/HodgeEnemyAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeEnemyAnimInstance.h)

项目内直接 include（不是运行调用关系）：[Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   5: #include "Animation/HodgeAnimInstance.h"
   6: #include "HodgeEnemyAnimInstance.generated.h"
   9: UCLASS(Blueprintable)
  10: class HODGEPODGE_API UHodgeEnemyAnimInstance : public UHodgeAnimInstance
  11: {
  12: 	GENERATED_BODY()
  13: public:
  15: 	UPROPERTY(BlueprintReadOnly, Transient, Category="Hodge|Enemy") bool bEnemyMoving = false;
  17: 	UPROPERTY(BlueprintReadOnly, Transient, Category="Hodge|Enemy") float EnemyGroundSpeed = 0.f;
  18: protected:
  22: 	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
  23: };
```

## HodgeRootMotionModifier_AttackFacing.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Animation/HodgeRootMotionModifier_AttackFacing.h](../../../Source/Hodgepodge/Public/Animation/HodgeRootMotionModifier_AttackFacing.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "RootMotionModifier_SkewWarp.h"
   4: #include "HodgeRootMotionModifier_AttackFacing.generated.h"
   6: UCLASS(meta=(DisplayName="Hodge Attack Facing Warp"))
   7: class HODGEPODGE_API UHodgeRootMotionModifier_AttackFacing : public URootMotionModifier_SkewWarp
   8: {
   9: 	GENERATED_BODY()
  10: public:
  11: 	virtual void Update(const FMotionWarpingUpdateContext& Context) override;
  12: };
```

## HodgeWingAnimInstance.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Animation/HodgeWingAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeWingAnimInstance.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "Animation/AnimInstance.h"
   7: #include "HodgeWingAnimInstance.generated.h"
  10: UCLASS(Blueprintable)
  11: class HODGEPODGE_API UHodgeWingAnimInstance : public UAnimInstance
  12: {
  13: 	GENERATED_BODY()
  14: public:
  16: 	UPROPERTY(BlueprintReadOnly, Transient) FRotator WingRootRotation = FRotator::ZeroRotator;
  18: 	UPROPERTY(BlueprintReadOnly, Transient) FRotator WingMidRotation = FRotator::ZeroRotator;
  20: 	UPROPERTY(BlueprintReadOnly, Transient) FRotator WingTipRotation = FRotator::ZeroRotator;
  21: protected:
  25: 	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
  26: };
```

## HodgeWingProfile.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Animation/HodgeWingProfile.h](../../../Source/Hodgepodge/Public/Animation/HodgeWingProfile.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Engine/DataAsset.h"
   8: #include "HodgeWingProfile.generated.h"
  10: class USkeletalMesh;
  11: class UAnimInstance;
  12: class UAnimMontage;
  13: class UAnimSequence;
  14: class UAnimSequenceBase;
  15: class UMaterialInterface;
  18: struct HODGEPODGE_API FHodgeWingFadeState
  19: {
  21: 	float Amount = 0.f;
  23: 	float StartAmount = 0.f;
  25: 	float Elapsed = 0.f;
  27: 	float Duration = 0.f;
  29: 	bool bFading = false;
  31: 	void Show();
  33: 	void BeginFade(float Seconds);
  35: 	void Advance(float DeltaSeconds);
  37: 	void Reset();
  38: };
  41: UENUM(BlueprintType)
  42: enum class EHodgeWingPlaybackMode : uint8
  43: {
  44: 	Procedural,
  45: 	AnimationAsset
  46: };
  49: USTRUCT(BlueprintType)
  50: struct HODGEPODGE_API FHodgeWingAnimationBinding
  51: {
  52: 	GENERATED_BODY()
  54: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimSequenceBase> SourceAnimation;
  56: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimSequence> WingAnimation;
  58: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName WindowId = TEXT("AttackWing");
  60: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float SourceStartTime = 0.f;
  62: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float WingStartTime = 0.f;
  64: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01")) float PlayRateRatio = 1.f;
  66: };
  69: USTRUCT(BlueprintType)
  70: struct HODGEPODGE_API FHodgeWingActionHandle
  71: {
  72: 	GENERATED_BODY()
  74: 	UPROPERTY(BlueprintReadOnly) FGuid Id;
  76: 	UPROPERTY(BlueprintReadOnly) int32 Generation = 0;
  78: 	bool IsValid() const { return Id.IsValid() && Generation > 0; }
  79: };
  82: USTRUCT(BlueprintType)
  83: struct HODGEPODGE_API FHodgeWingPresentationState
  84: {
  85: 	GENERATED_BODY()
  87: 	UPROPERTY(BlueprintReadOnly) FHodgeWingActionHandle Handle;
  89: 	UPROPERTY(BlueprintReadOnly) FGuid ExecutionId;
  91: 	UPROPERTY(BlueprintReadOnly) int32 BindingIndex = INDEX_NONE;
  93: 	UPROPERTY(BlueprintReadOnly) int32 SourceInstanceId = INDEX_NONE;
  95: 	UPROPERTY(BlueprintReadOnly) float SourceTime = 0.f;
  97: 	UPROPERTY(BlueprintReadOnly) float WingTime = 0.f;
  99: 	UPROPERTY(BlueprintReadOnly) bool bVisible = false;
 101: 	UPROPERTY(BlueprintReadOnly) int32 Generation = 1;
 103: 	UPROPERTY(BlueprintReadOnly) int32 PoseEvaluations = 0;
 110: 	FHodgeWingActionHandle Begin(int32 Binding, int32 Instance, FGuid Execution);
 115: 	bool End(FHodgeWingActionHandle EndingHandle);
 117: 	void Reset();
 118: };
 121: USTRUCT(BlueprintType)
 122: struct HODGEPODGE_API FHodgeWingPose
 123: {
 124: 	GENERATED_BODY()
 126: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FRotator Root = FRotator::ZeroRotator;
 128: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FRotator Mid = FRotator::ZeroRotator;
 130: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FRotator Tip = FRotator::ZeroRotator;
 137: 	static FHodgeWingPose Blend(const FHodgeWingPose& A, const FHodgeWingPose& B, float Alpha);
 138: };
 141: UCLASS(BlueprintType)
 142: class HODGEPODGE_API UHodgeWingProfile : public UDataAsset
 143: {
 144: 	GENERATED_BODY()
 145: public:
 147: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing") EHodgeWingPlaybackMode PlaybackMode = EHodgeWingPlaybackMode::Procedural;
 149: 	UPROPERTY() TArray<FHodgeWingAnimationBinding> AnimationBindings;
 151: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing|Hit") bool bEnableAuthorityHitSampling = false;
 153: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing|Fade", meta=(ClampMin="0", Units="s")) float FadeOutSeconds = .35f;
 155: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing|Fade") FName VisibilityParameter = TEXT("WingVisibility");
 157: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing|Fade") TArray<TObjectPtr<UMaterialInterface>> Materials;
 159: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMesh> Mesh;
 161: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<UAnimInstance> AnimClass;
 163: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName AttachSocket = TEXT("WingAttach");
 165: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FTransform MeshOffset = FTransform::Identity;
 167: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="PlaybackMode == EHodgeWingPlaybackMode::Procedural", EditConditionHides)) TObjectPtr<UAnimMontage> AttackMontage;
 169: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="PlaybackMode == EHodgeWingPlaybackMode::Procedural", EditConditionHides, Units="s", ClampMin="0")) float WindupStart = .05f;
 171: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="PlaybackMode == EHodgeWingPlaybackMode::Procedural", EditConditionHides, Units="s", ClampMin="0")) float StrikeStart = .25f;
 173: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="PlaybackMode == EHodgeWingPlaybackMode::Procedural", EditConditionHides, Units="s", ClampMin="0")) float StrikePeak = .4f;
 175: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="PlaybackMode == EHodgeWingPlaybackMode::Procedural", EditConditionHides, Units="s", ClampMin="0")) float RecoveryEnd = .75f;
 177: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="PlaybackMode == EHodgeWingPlaybackMode::Procedural", EditConditionHides, Units="s", ClampMin="0.01")) float CancelReturnSeconds = .18f;
 179: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="PlaybackMode == EHodgeWingPlaybackMode::Procedural", EditConditionHides)) FHodgeWingPose WindupPose;
 181: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="PlaybackMode == EHodgeWingPlaybackMode::Procedural", EditConditionHides)) FHodgeWingPose StrikePose;
 185: 	bool HasValidTiming() const;
 190: 	FHodgeWingPose Evaluate(float MontagePosition) const;
 195: 	bool Validate(TArray<FText>& Errors) const;
 196: #if WITH_EDITOR
 201: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
 202: #endif
 203: };
```
