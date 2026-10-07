# Combat 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeDamageRules.cpp

检测和 Execution 共用目标、ASC、死亡/Health、自伤/友伤及队伍规则。

源码：[Source/Hodgepodge/Private/Combat/HodgeDamageRules.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeDamageRules.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeDamageRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeDamageRules.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)

定义候选（多行签名仅展示首行）：

- L23: `bool FHodgeDamageRules::CanDamage(const AActor* Source, const AActor* Target, bool bAllowFriendlyFire)`

## HodgeHitDetection.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Combat/HodgeHitDetection.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeHitDetection.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)

定义候选（多行签名仅展示首行）：

- L122: `bool FHodgeHitVolumeConfig::Validate(TArray<FText>& Errors) const`
- L149: `UHodgeHitDetectionProfile::UHodgeHitDetectionProfile()`
- L155: `bool UHodgeHitDetectionProfile::Validate(TArray<FText>& Errors) const`
- L179: `EDataValidationResult UHodgeHitDetectionProfile::IsDataValid(FDataValidationContext& Context) const`
- L190: `bool UHodgeSocketSweepStrategy::Capture(const USceneComponent* Component, const FHodgeHitSource& Source,`
- L223: `void UHodgeSocketSweepStrategy::Detect(UWorld* World, const FHodgeHitSource& Source,`
- L246: `bool UHodgeBoxSweepStrategy::Capture(const USceneComponent* Component, const FHodgeHitSource& Source,`
- L258: `void UHodgeBoxSweepStrategy::Detect(UWorld* World, const FHodgeHitSource& Source,`
- L287: `bool UHodgeShapeQueryStrategy::Capture(const USceneComponent*, const FHodgeHitSource&, FHodgeHitGeometry&) const`
- L293: `void UHodgeShapeQueryStrategy::Detect(UWorld* World, const FHodgeHitSource& Source,`

## HodgeDamageRules.h

检测和 Execution 共用目标、ASC、死亡/Health、自伤/友伤及队伍规则。

源码：[Source/Hodgepodge/Public/Combat/HodgeDamageRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeDamageRules.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   5: class AActor;
   8: struct HODGEPODGE_API FHodgeDamageRules
   9: {
  10: 	static bool CanDamage(const AActor* Source, const AActor* Target, bool bAllowFriendlyFire);
  11: };
```

## HodgeHitDetection.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "Engine/DataAsset.h"
   5: #include "Engine/EngineTypes.h"
   6: #include "GameplayTagContainer.h"
   7: #include "HodgeHitDetection.generated.h"
   9: class UGameplayEffect;
  10: class USceneComponent;
  11: class UHodgeHitDetectionStrategy;
  12: class UHodgeWeaponInstance;
  13: struct FCollisionQueryParams;
  15: UENUM(BlueprintType)
  16: enum class EHodgeHitGeometryMode : uint8 { ExistingSource, ConfiguredShape };
  17: UENUM(BlueprintType)
  18: enum class EHodgeHitShape : uint8 { Sphere, Box, Capsule };
  19: UENUM(BlueprintType)
  20: enum class EHodgeHitAnchorKind : uint8 { AvatarRoot, RegisteredSource, ExecutionTransform, ExecutionTarget };
  21: UENUM(BlueprintType)
  22: enum class EHodgeHitTransformPolicy : uint8 { Follow, SnapshotOnEventEnter };
  23: UENUM(BlueprintType)
  24: enum class EHodgeHitQueryMode : uint8 { Sweep, Overlap };
  25: UENUM(BlueprintType)
  26: enum class EHodgeHitSampleMode : uint8 { EveryFrame, OnceOnEnter };
  27: UENUM(BlueprintType)
  28: enum class EHodgeHitFilterFrame : uint8 { Caster, DetectionAnchor };
  29: UENUM(BlueprintType)
  30: enum class EHodgeHitTargetPolicy : uint8 { AnyInVolume, LockedTargetInVolume, ConfirmedTarget };
  31: UENUM(BlueprintType)
  32: enum class EHodgeHitResultKind : uint8 { Sweep, Overlap, ConfirmedTarget };
  33: UENUM(BlueprintType)
  34: enum class EHodgeHitGroupScope : uint8 { Execution, TriggerTime };
  37: USTRUCT(BlueprintType)
  38: struct HODGEPODGE_API FHodgeHitVolumeConfig
  39: {
  40: 	GENERATED_BODY()
  42: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitGeometryMode GeometryMode = EHodgeHitGeometryMode::ExistingSource;
  44: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
  45: 	EHodgeHitShape Shape = EHodgeHitShape::Sphere;
  47: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
  48: 	EHodgeHitAnchorKind AnchorKind = EHodgeHitAnchorKind::AvatarRoot;
  50: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
  51: 	EHodgeHitTransformPolicy TransformPolicy = EHodgeHitTransformPolicy::Follow;
  53: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
  54: 	FTransform LocalTransform = FTransform::Identity;
  56: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Sphere", EditConditionHides, ClampMin="0.1", Units="cm"))
  57: 	float SphereRadius = 100.f;
  59: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Box", EditConditionHides, Units="cm"))
  60: 	FVector BoxHalfExtent = FVector(100.f);
  62: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Capsule", EditConditionHides, ClampMin="0.1", Units="cm"))
  63: 	float CapsuleRadius = 50.f;
  65: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Capsule", EditConditionHides, ClampMin="0.1", Units="cm"))
  66: 	float CapsuleHalfHeight = 100.f;
  68: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName AnchorSocket;
  70: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName AnchorKey;
  73: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", Units="cm")) float MaxAnchorDistance = 5000.f;
  74: 	bool Validate(TArray<FText>& Errors) const;
  75: };
  78: USTRUCT(BlueprintType)
  79: struct HODGEPODGE_API FHodgeHitSource
  80: {
  81: 	GENERATED_BODY()
  84: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Combat.Source"))
  85: 	FGameplayTag SourceTag;
  89: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  90: 	FName ComponentName;
  94: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
  95: 	int32 WeaponActorIndex = 0;
  99: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 100: 	TArray<FName> Sockets;
 103: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="2", ClampMax="32"))
 104: 	int32 SegmentSamples = 5;
 108: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 109: 	FVector LocalOffset = FVector::ZeroVector;
 112: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1", Units="cm"))
 113: 	float Radius = 10.f;
 114: };
 117: struct FHodgeHitGeometry
 118: {
 119: 	TArray<FVector> Points;
 120: 	FTransform Transform = FTransform::Identity;
 121: 	FVector BoxExtent = FVector::ZeroVector;
 122: 	EHodgeHitShape Shape = EHodgeHitShape::Sphere;
 123: 	float Radius = 0.f;
 124: 	float HalfHeight = 0.f;
 125: };
 128: UCLASS(BlueprintType, Const)
 129: class HODGEPODGE_API UHodgeHitDetectionProfile : public UDataAsset
 130: {
 131: 	GENERATED_BODY()
 132: public:
 133: 	UHodgeHitDetectionProfile();
 136: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 137: 	TSubclassOf<UHodgeHitDetectionStrategy> Strategy;
 141: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 142: 	TArray<TEnumAsByte<ECollisionChannel>> ObjectTypes;
 145: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 146: 	bool bRequireLineOfSight = true;
 149: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 150: 	TEnumAsByte<ECollisionChannel> ObstructionChannel = ECC_Visibility;
 154: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="180"))
 155: 	float HalfAngleDegrees = 180.f;
 159: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", Units="cm"))
 160: 	float MaxSweepDistance = 300.f;
 164: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="32"))
 165: 	int32 RotationSubsteps = 8;
 167: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitQueryMode QueryMode = EHodgeHitQueryMode::Sweep;
 169: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitSampleMode SampleMode = EHodgeHitSampleMode::EveryFrame;
 171: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitFilterFrame FilterFrame = EHodgeHitFilterFrame::Caster;
 174: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bContinuousMotion = false;
 176: 	bool Validate(TArray<FText>& Errors) const;
 177: #if WITH_EDITOR
 178: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
 179: #endif
 180: };
 183: USTRUCT(BlueprintType)
 184: struct HODGEPODGE_API FHodgeHitDetectionRequest
 185: {
 186: 	GENERATED_BODY()
 189: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Combat.Source"))
 190: 	FGameplayTag SourceTag;
 193: 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
 194: 	TObjectPtr<UHodgeHitDetectionProfile> Profile;
 196: 	UPROPERTY(EditAnywhere, BlueprintReadWrite) FHodgeHitVolumeConfig Volume;
 198: 	UPROPERTY() FTransform RuntimeAnchor = FTransform::Identity;
 200: 	UPROPERTY() bool bHasRuntimeAnchor = false;
 202: 	UPROPERTY() TWeakObjectPtr<AActor> RuntimeTarget;
 204: 	UPROPERTY() EHodgeHitTargetPolicy TargetPolicy = EHodgeHitTargetPolicy::AnyInVolume;
 206: 	UPROPERTY() float MaxTargetDistance = 2000.f;
 209: 	UPROPERTY()
 210: 	TArray<TWeakObjectPtr<AActor>> IgnoredActors;
 211: };
 214: USTRUCT(BlueprintType)
 215: struct HODGEPODGE_API FHodgeHitDetectionBatch
 216: {
 217: 	GENERATED_BODY()
 220: 	UPROPERTY(BlueprintReadOnly) FGuid ExecutionId;
 222: 	UPROPERTY(BlueprintReadOnly) int32 EventIndex = INDEX_NONE;
 224: 	UPROPERTY() uint64 SessionHandle = 0;
 226: 	UPROPERTY(BlueprintReadOnly) int32 SampleSequence = 0;
 228: 	UPROPERTY(BlueprintReadOnly) double SampleTime = 0.0;
 230: 	UPROPERTY(BlueprintReadOnly) TArray<FHitResult> Hits;
 232: 	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<USceneComponent> SourceComponent;
 234: 	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<UHodgeWeaponInstance> Weapon;
 236: 	UPROPERTY(BlueprintReadOnly) FVector SourceOrigin = FVector::ZeroVector;
 238: 	UPROPERTY(BlueprintReadOnly) EHodgeHitResultKind ResultKind = EHodgeHitResultKind::Sweep;
 239: };
 242: USTRUCT(BlueprintType)
 243: struct HODGEPODGE_API FHodgeHitEffectConfig
 244: {
 245: 	GENERATED_BODY()
 248: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Combat.Source"))
 249: 	FGameplayTag SourceTag;
 252: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 253: 	TObjectPtr<UHodgeHitDetectionProfile> Profile;
 255: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeHitVolumeConfig Volume;
 257: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitTargetPolicy TargetPolicy = EHodgeHitTargetPolicy::AnyInVolume;
 259: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName TargetKey;
 261: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", Units="cm")) float MaxTargetDistance = 2000.f;
 263: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool RequiresWeaponInHand = true;
 267: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 268: 	TSubclassOf<UGameplayEffect> DamageEffect;
 271: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
 272: 	float DamageMultiplier = 1.f;
 275: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="GameplayEffect.DamageType"))
 276: 	FGameplayTag DamageType;
 280: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", Units="s"))
 281: 	float RepeatHitInterval = 0.f;
 285: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 286: 	FName HitGroup;
 289: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitGroupScope HitGroupScope = EHodgeHitGroupScope::Execution;
 292: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 293: 	bool bAllowFriendlyFire = false;
 294: };
 296: USTRUCT(BlueprintType)
 297: struct HODGEPODGE_API FHodgeHitWindowBinding : public FHodgeHitEffectConfig
 298: {
 299: 	GENERATED_BODY()
 301: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Status.Attack.HitCheck")) FGameplayTag WindowTag;
 302: };
 304: USTRUCT(BlueprintType)
 305: struct HODGEPODGE_API FHodgeHitPointBinding : public FHodgeHitEffectConfig
 306: {
 307: 	GENERATED_BODY()
 309: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="GameplayEvent")) FGameplayTag PointEventTag;
 310: };
 313: UCLASS(Abstract, BlueprintType)
 314: class HODGEPODGE_API UHodgeHitDetectionStrategy : public UObject
 315: {
 316: 	GENERATED_BODY()
 317: public:
 318: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 319: 		FHodgeHitGeometry& OutGeometry) const PURE_VIRTUAL(UHodgeHitDetectionStrategy::Capture, return false;);
 320: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 321: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 322: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const
 323: 		PURE_VIRTUAL(UHodgeHitDetectionStrategy::Detect, );
 324: };
 326: UCLASS()
 327: class HODGEPODGE_API UHodgeSocketSweepStrategy : public UHodgeHitDetectionStrategy
 328: {
 329: 	GENERATED_BODY()
 330: public:
 331: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 332: 		FHodgeHitGeometry& OutGeometry) const override;
 333: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 334: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 335: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
 336: };
 338: UCLASS()
 339: class HODGEPODGE_API UHodgeBoxSweepStrategy : public UHodgeHitDetectionStrategy
 340: {
 341: 	GENERATED_BODY()
 342: public:
 343: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 344: 		FHodgeHitGeometry& OutGeometry) const override;
 345: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 346: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 347: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
 348: };
 351: UCLASS()
 352: class HODGEPODGE_API UHodgeShapeQueryStrategy : public UHodgeHitDetectionStrategy
 353: {
 354: 	GENERATED_BODY()
 355: public:
 356: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 357: 		FHodgeHitGeometry& OutGeometry) const override;
 358: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 359: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 360: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
 361: };
```
