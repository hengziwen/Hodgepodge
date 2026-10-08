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
- L153: `UHodgeHitDetectionProfile::UHodgeHitDetectionProfile()`
- L158: `bool UHodgeHitDetectionProfile::Validate(TArray<FText>& Errors) const`
- L183: `EDataValidationResult UHodgeHitDetectionProfile::IsDataValid(FDataValidationContext& Context) const`
- L195: `bool UHodgeSocketSweepStrategy::Capture(const USceneComponent* Component, const FHodgeHitSource& Source,`
- L228: `void UHodgeSocketSweepStrategy::Detect(UWorld* World, const FHodgeHitSource& Source,`
- L255: `bool UHodgeBoxSweepStrategy::Capture(const USceneComponent* Component, const FHodgeHitSource& Source,`
- L267: `void UHodgeBoxSweepStrategy::Detect(UWorld* World, const FHodgeHitSource& Source,`
- L298: `bool UHodgeShapeQueryStrategy::Capture(const USceneComponent*, const FHodgeHitSource&, FHodgeHitGeometry&) const`
- L304: `void UHodgeShapeQueryStrategy::Detect(UWorld* World, const FHodgeHitSource& Source,`

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
  35: USTRUCT(BlueprintType)
  36: struct HODGEPODGE_API FHodgeHitVolumeConfig
  37: {
  38: 	GENERATED_BODY()
  40: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitGeometryMode GeometryMode = EHodgeHitGeometryMode::ExistingSource;
  42: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
  43: 	EHodgeHitShape Shape = EHodgeHitShape::Sphere;
  45: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
  46: 	EHodgeHitAnchorKind AnchorKind = EHodgeHitAnchorKind::AvatarRoot;
  48: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
  49: 	EHodgeHitTransformPolicy TransformPolicy = EHodgeHitTransformPolicy::Follow;
  51: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
  52: 	FTransform LocalTransform = FTransform::Identity;
  54: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Sphere", EditConditionHides, ClampMin="0.1", Units="cm"))
  55: 	float SphereRadius = 100.f;
  57: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Box", EditConditionHides, Units="cm"))
  58: 	FVector BoxHalfExtent = FVector(100.f);
  60: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Capsule", EditConditionHides, ClampMin="0.1", Units="cm"))
  61: 	float CapsuleRadius = 50.f;
  63: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Capsule", EditConditionHides, ClampMin="0.1", Units="cm"))
  64: 	float CapsuleHalfHeight = 100.f;
  66: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName AnchorSocket;
  68: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName AnchorKey;
  71: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", Units="cm")) float MaxAnchorDistance = 5000.f;
  72: 	bool Validate(TArray<FText>& Errors) const;
  73: };
  76: USTRUCT(BlueprintType)
  77: struct HODGEPODGE_API FHodgeHitSource
  78: {
  79: 	GENERATED_BODY()
  82: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Combat.Source"))
  83: 	FGameplayTag SourceTag;
  87: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  88: 	FName ComponentName;
  92: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
  93: 	int32 WeaponActorIndex = 0;
  97: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  98: 	TArray<FName> Sockets;
 101: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="2", ClampMax="32"))
 102: 	int32 SegmentSamples = 5;
 106: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 107: 	FVector LocalOffset = FVector::ZeroVector;
 110: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1", Units="cm"))
 111: 	float Radius = 10.f;
 112: };
 115: struct FHodgeHitGeometry
 116: {
 117: 	TArray<FVector> Points;
 118: 	FTransform Transform = FTransform::Identity;
 119: 	FVector BoxExtent = FVector::ZeroVector;
 120: 	EHodgeHitShape Shape = EHodgeHitShape::Sphere;
 121: 	float Radius = 0.f;
 122: 	float HalfHeight = 0.f;
 123: };
 126: UCLASS(BlueprintType, Const)
 127: class HODGEPODGE_API UHodgeHitDetectionProfile : public UDataAsset
 128: {
 129: 	GENERATED_BODY()
 130: public:
 131: 	UHodgeHitDetectionProfile();
 135: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 136: 	TArray<TEnumAsByte<ECollisionChannel>> ObjectTypes;
 139: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 140: 	bool bRequireLineOfSight = true;
 143: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 144: 	TEnumAsByte<ECollisionChannel> ObstructionChannel = ECC_Visibility;
 148: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="180"))
 149: 	float HalfAngleDegrees = 180.f;
 153: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", Units="cm"))
 154: 	float MaxSweepDistance = 300.f;
 158: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="32"))
 159: 	int32 RotationSubsteps = 8;
 161: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitQueryMode QueryMode = EHodgeHitQueryMode::Sweep;
 163: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitSampleMode SampleMode = EHodgeHitSampleMode::EveryFrame;
 165: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitFilterFrame FilterFrame = EHodgeHitFilterFrame::Caster;
 168: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bContinuousMotion = false;
 170: 	bool Validate(TArray<FText>& Errors) const;
 171: #if WITH_EDITOR
 172: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
 173: #endif
 174: };
 177: USTRUCT(BlueprintType)
 178: struct HODGEPODGE_API FHodgeHitDetectionRequest
 179: {
 180: 	GENERATED_BODY()
 183: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Combat.Source"))
 184: 	FGameplayTag SourceTag;
 187: 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
 188: 	TObjectPtr<UHodgeHitDetectionProfile> Profile;
 190: 	UPROPERTY(EditAnywhere, BlueprintReadWrite) FHodgeHitVolumeConfig Volume;
 192: 	UPROPERTY() FTransform RuntimeAnchor = FTransform::Identity;
 194: 	UPROPERTY() bool bHasRuntimeAnchor = false;
 196: 	UPROPERTY() TWeakObjectPtr<AActor> RuntimeTarget;
 198: 	UPROPERTY() EHodgeHitTargetPolicy TargetPolicy = EHodgeHitTargetPolicy::AnyInVolume;
 200: 	UPROPERTY() float MaxTargetDistance = 2000.f;
 203: 	UPROPERTY() TWeakObjectPtr<USceneComponent> DirectComponent;
 204: 	UPROPERTY() FHodgeHitSource DirectSource;
 207: 	UPROPERTY()
 208: 	TArray<TWeakObjectPtr<AActor>> IgnoredActors;
 209: };
 212: USTRUCT(BlueprintType)
 213: struct HODGEPODGE_API FHodgeHitDetectionBatch
 214: {
 215: 	GENERATED_BODY()
 218: 	UPROPERTY(BlueprintReadOnly) FGuid ExecutionId;
 220: 	UPROPERTY(BlueprintReadOnly) int32 OccurrenceId = INDEX_NONE;
 222: 	UPROPERTY() uint64 SessionHandle = 0;
 224: 	UPROPERTY(BlueprintReadOnly) int32 SampleSequence = 0;
 226: 	UPROPERTY(BlueprintReadOnly) double SampleTime = 0.0;
 228: 	UPROPERTY(BlueprintReadOnly) TArray<FHitResult> Hits;
 230: 	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<USceneComponent> SourceComponent;
 232: 	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<UHodgeWeaponInstance> Weapon;
 234: 	UPROPERTY(BlueprintReadOnly) FVector SourceOrigin = FVector::ZeroVector;
 236: 	UPROPERTY(BlueprintReadOnly) EHodgeHitResultKind ResultKind = EHodgeHitResultKind::Sweep;
 237: };
 240: USTRUCT(BlueprintType)
 241: struct HODGEPODGE_API FHodgeHitEffectConfig
 242: {
 243: 	GENERATED_BODY()
 245: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<UGameplayEffect> DamageEffect;
 247: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float DamageMultiplier = 1.f;
 249: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="GameplayEffect.DamageType")) FGameplayTag DamageType;
 251: 	UPROPERTY(BlueprintReadOnly) EHodgeHitTargetPolicy TargetPolicy = EHodgeHitTargetPolicy::AnyInVolume;
 253: 	UPROPERTY(BlueprintReadOnly) float RepeatHitInterval = 0.f;
 255: 	UPROPERTY(BlueprintReadOnly) FName HitGroup;
 257: 	UPROPERTY(BlueprintReadOnly) FName AttackPhase;
 259: 	UPROPERTY(BlueprintReadOnly) bool bAllowFriendlyFire = false;
 260: };
 263: UCLASS(Abstract, BlueprintType)
 264: class HODGEPODGE_API UHodgeHitDetectionStrategy : public UObject
 265: {
 266: 	GENERATED_BODY()
 267: public:
 268: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 269: 		FHodgeHitGeometry& OutGeometry) const PURE_VIRTUAL(UHodgeHitDetectionStrategy::Capture, return false;);
 270: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 271: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 272: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const
 273: 		PURE_VIRTUAL(UHodgeHitDetectionStrategy::Detect, );
 274: };
 276: UCLASS()
 277: class HODGEPODGE_API UHodgeSocketSweepStrategy : public UHodgeHitDetectionStrategy
 278: {
 279: 	GENERATED_BODY()
 280: public:
 281: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 282: 		FHodgeHitGeometry& OutGeometry) const override;
 283: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 284: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 285: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
 286: };
 288: UCLASS()
 289: class HODGEPODGE_API UHodgeBoxSweepStrategy : public UHodgeHitDetectionStrategy
 290: {
 291: 	GENERATED_BODY()
 292: public:
 293: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 294: 		FHodgeHitGeometry& OutGeometry) const override;
 295: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 296: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 297: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
 298: };
 301: UCLASS()
 302: class HODGEPODGE_API UHodgeShapeQueryStrategy : public UHodgeHitDetectionStrategy
 303: {
 304: 	GENERATED_BODY()
 305: public:
 306: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 307: 		FHodgeHitGeometry& OutGeometry) const override;
 308: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 309: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 310: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
 311: };
```
