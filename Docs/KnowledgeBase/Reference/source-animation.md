# Animation 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAnimInstance.cpp

ASC 属性映射、GroundDistance 与旋转策略快照；FullBody 权重协调蓝图程序性根 Yaw。

源码：[Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)、[Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)

定义候选（多行签名仅展示首行）：

- L15: `UHodgeAnimInstance::UHodgeAnimInstance(const FObjectInitializer& ObjectInitializer)`
- L20: `void UHodgeAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)`
- L31: `EDataValidationResult UHodgeAnimInstance::IsDataValid(class FDataValidationContext& Context) const`
- L45: `void UHodgeAnimInstance::NativeInitializeAnimation()`
- L63: `void UHodgeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)`

## HodgeCombatAnimNotifies.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Animation/HodgeCombatAnimNotifies.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeCombatAnimNotifies.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L29: `FHodgeAnimHitConfig::FHodgeAnimHitConfig()`
- L34: `bool FHodgeAnimHitConfig::Validate(TArray<FText>& Errors) const`
- L72: `UHodgeAnimNotifyState_HitCheck::UHodgeAnimNotifyState_HitCheck() { bIsNativeBranchingPoint = true; }`
- L74: `UHodgeAnimNotify_Hit::UHodgeAnimNotify_Hit()`
- L80: `UHodgeAnimNotifyState_GameplayTag::UHodgeAnimNotifyState_GameplayTag() { bIsNativeBranchingPoint = true; }`
- L81: `UHodgeAnimNotifyState_WeaponHand::UHodgeAnimNotifyState_WeaponHand() { bIsNativeBranchingPoint = true; }`
- L82: `UHodgeAnimNotify_GameplayEvent::UHodgeAnimNotify_GameplayEvent() { bIsNativeBranchingPoint = true; }`
- L84: `void UHodgeAnimNotifyState_HitCheck::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)`
- L93: `void UHodgeAnimNotifyState_HitCheck::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)`
- L98: `void UHodgeAnimNotify_Hit::BranchingPointNotify(FBranchingPointNotifyPayload& Payload)`
- L115: `void UHodgeAnimNotifyState_GameplayTag::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)`
- L124: `void UHodgeAnimNotifyState_GameplayTag::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)`
- L129: `void UHodgeAnimNotifyState_WeaponHand::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)`
- L138: `void UHodgeAnimNotifyState_WeaponHand::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)`
- L143: `void UHodgeAnimNotify_GameplayEvent::BranchingPointNotify(FBranchingPointNotifyPayload& Payload)`
- L160: `void UHodgeAnimNotifyState_HitCheck::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L167: `void UHodgeAnimNotifyState_HitCheck::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L174: `void UHodgeAnimNotifyState_GameplayTag::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L181: `void UHodgeAnimNotifyState_GameplayTag::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L188: `void UHodgeAnimNotifyState_WeaponHand::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L195: `void UHodgeAnimNotifyState_WeaponHand::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L202: `void UHodgeAnimNotify_Hit::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L209: `void UHodgeAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,`
- L232: `void UHodgeAnimNotifyState_HitCheck::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)`
- L237: `void UHodgeAnimNotify_Hit::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) { ConfigureGameplayNotify(Event); }`
- L239: `void UHodgeAnimNotifyState_GameplayTag::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)`
- L244: `void UHodgeAnimNotifyState_WeaponHand::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)`
- L249: `void UHodgeAnimNotify_GameplayEvent::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)`

## HodgeAnimInstance.h

ASC 属性映射、GroundDistance 与旋转策略快照；FullBody 权重协调蓝图程序性根 Yaw。

源码：[Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "GameplayEffectTypes.h"
   7: #include "Animation/AnimInstance.h"
   8: #include "HodgeAnimInstance.generated.h"
  13: UCLASS(Config = Game)
  14: class HODGEPODGE_API UHodgeAnimInstance : public UAnimInstance
  15: {
  16: 	GENERATED_BODY()
  18: public:
  20: 	UHodgeAnimInstance(const FObjectInitializer& ObjectInitializer);
  23: 	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);
  25: protected:
  26: #if WITH_EDITOR
  28: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
  29: #endif
  32: 	virtual void NativeInitializeAnimation() override;
  35: 	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
  37: protected:
  39: 	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
  40: 	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
  43: 	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
  44: 	float GroundDistance = -1.0f;
  47: 	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
  48: 	bool bSuppressLocomotionYaw = false;
  50: 	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
  51: 	bool bResetLocomotionYaw = false;
  53: 	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
  54: 	float LocomotionRootYawScale = 1.f;
  55: };
```

## HodgeCombatAnimNotifies.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "Animation/AnimNotifies/AnimNotify.h"
   4: #include "Animation/AnimNotifies/AnimNotifyState.h"
   5: #include "Combat/HodgeHitDetection.h"
   6: #include "HodgeCombatAnimNotifies.generated.h"
   8: UENUM(BlueprintType)
   9: enum class EHodgeAnimHitSource : uint8 { CharacterMeshSocket, EquippedWeapon, AvatarRoot, NamedComponent, ExecutionAnchor, ExecutionTarget };
  12: USTRUCT(BlueprintType)
  13: struct HODGEPODGE_API FHodgeAnimHitConfig
  14: {
  15: 	GENERATED_BODY()
  16: 	FHodgeAnimHitConfig();
  18: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeAnimHitSource Source = EHodgeAnimHitSource::CharacterMeshSocket;
  20: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source==EHodgeAnimHitSource::CharacterMeshSocket || Source==EHodgeAnimHitSource::NamedComponent", EditConditionHides)) FName BoneOrSocket;
  22: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Combat.Source.Weapon", EditCondition="Source==EHodgeAnimHitSource::EquippedWeapon", EditConditionHides)) FGameplayTag WeaponSourceTag;
  24: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source==EHodgeAnimHitSource::NamedComponent", EditConditionHides)) FName ComponentName;
  26: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon", EditConditionHides)) EHodgeHitShape Shape = EHodgeHitShape::Sphere;
  28: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1", Units="cm", EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon && Shape!=EHodgeHitShape::Box", EditConditionHides)) float Radius = 15.f;
  30: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon && Shape==EHodgeHitShape::Box", EditConditionHides)) FVector BoxHalfExtent = FVector(100.f);
  32: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1", Units="cm", EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon && Shape==EHodgeHitShape::Capsule", EditConditionHides)) float CapsuleHalfHeight = 100.f;
  34: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon", EditConditionHides)) FTransform LocalTransform = FTransform::Identity;
  36: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon", EditConditionHides)) EHodgeHitTransformPolicy TransformPolicy = EHodgeHitTransformPolicy::Follow;
  38: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay) TObjectPtr<UHodgeHitDetectionProfile> Profile;
  40: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source==EHodgeAnimHitSource::ExecutionAnchor", EditConditionHides)) FName AnchorKey;
  42: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitTargetPolicy TargetPolicy = EHodgeHitTargetPolicy::AnyInVolume;
  44: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="TargetPolicy!=EHodgeHitTargetPolicy::AnyInVolume || Source==EHodgeAnimHitSource::ExecutionTarget", EditConditionHides)) FName TargetKey;
  46: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, meta=(ClampMin="1", EditCondition="TargetPolicy!=EHodgeHitTargetPolicy::AnyInVolume || Source==EHodgeAnimHitSource::ExecutionTarget", EditConditionHides)) float MaxTargetDistance = 2000.f;
  48: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, meta=(ClampMin="1")) float MaxAnchorDistance = 5000.f;
  50: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bUseDefaultDamage = true;
  52: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bUseDefaultDamage", EditConditionHides)) FHodgeHitEffectConfig Damage;
  54: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float DamageScale = 1.f;
  56: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float RepeatHitInterval = 0.f;
  58: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay) FName HitGroup;
  60: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay) FName AttackPhase;
  62: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay) bool bAllowFriendlyFire = false;
  63: 	bool Validate(TArray<FText>& Errors) const;
  64: };
  67: UCLASS(meta=(DisplayName="Hodge 持续命中"))
  68: class HODGEPODGE_API UHodgeAnimNotifyState_HitCheck : public UAnimNotifyState
  69: {
  70: 	GENERATED_BODY()
  71: public:
  72: 	UHodgeAnimNotifyState_HitCheck();
  73: #if WITH_EDITOR
  74: 	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
  75: #endif
  77: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hit") FHodgeAnimHitConfig Hit;
  78: 	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& Reference) override;
  79: 	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
  80: 	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
  81: 	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;
  82: };
  84: UCLASS(meta=(DisplayName="Hodge 单次命中"))
  85: class HODGEPODGE_API UHodgeAnimNotify_Hit : public UAnimNotify
  86: {
  87: 	GENERATED_BODY()
  88: public:
  89: 	UHodgeAnimNotify_Hit();
  90: #if WITH_EDITOR
  91: 	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
  92: #endif
  94: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hit") FHodgeAnimHitConfig Hit;
  96: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category="Hit") TArray<FHodgeAnimHitConfig> AdditionalHits;
  97: 	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
  98: 	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& Payload) override;
  99: };
 101: UCLASS(meta=(DisplayName="Hodge 状态区间"))
 102: class HODGEPODGE_API UHodgeAnimNotifyState_GameplayTag : public UAnimNotifyState
 103: {
 104: 	GENERATED_BODY()
 105: public:
 106: 	UHodgeAnimNotifyState_GameplayTag();
 107: #if WITH_EDITOR
 108: 	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
 109: #endif
 111: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="State") FGameplayTag StateTag;
 112: 	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& Reference) override;
 113: 	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
 114: 	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
 115: 	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;
 116: };
 118: UCLASS(meta=(DisplayName="Hodge 武器手持区间"))
 119: class HODGEPODGE_API UHodgeAnimNotifyState_WeaponHand : public UAnimNotifyState
 120: {
 121: 	GENERATED_BODY()
 122: public:
 123: 	UHodgeAnimNotifyState_WeaponHand();
 124: #if WITH_EDITOR
 125: 	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
 126: #endif
 127: 	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& Reference) override;
 128: 	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
 129: 	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
 130: 	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;
 131: };
 133: UCLASS(meta=(DisplayName="Hodge 玩法消息"))
 134: class HODGEPODGE_API UHodgeAnimNotify_GameplayEvent : public UAnimNotify
 135: {
 136: 	GENERATED_BODY()
 137: public:
 138: 	UHodgeAnimNotify_GameplayEvent();
 139: #if WITH_EDITOR
 140: 	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
 141: #endif
 143: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Event") FGameplayTag EventTag;
 144: 	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
 145: 	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& Payload) override;
 146: };
```
