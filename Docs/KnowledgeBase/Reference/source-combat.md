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

- L23: `UHodgeHitDetectionProfile::UHodgeHitDetectionProfile()`
- L29: `bool UHodgeHitDetectionProfile::Validate(TArray<FText>& Errors) const`
- L53: `EDataValidationResult UHodgeHitDetectionProfile::IsDataValid(FDataValidationContext& Context) const`
- L64: `bool UHodgeSocketSweepStrategy::Capture(const USceneComponent* Component, const FHodgeHitSource& Source,`
- L97: `void UHodgeSocketSweepStrategy::Detect(UWorld* World, const FHodgeHitSource& Source,`
- L114: `bool UHodgeBoxSweepStrategy::Capture(const USceneComponent* Component, const FHodgeHitSource& Source,`
- L126: `void UHodgeBoxSweepStrategy::Detect(UWorld* World, const FHodgeHitSource& Source,`

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
  16: USTRUCT(BlueprintType)
  17: struct HODGEPODGE_API FHodgeHitSource
  18: {
  19: 	GENERATED_BODY()
  21: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Combat.Source"))
  22: 	FGameplayTag SourceTag;
  25: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  26: 	FName ComponentName;
  29: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
  30: 	int32 WeaponActorIndex = 0;
  33: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  34: 	TArray<FName> Sockets;
  36: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="2", ClampMax="32"))
  37: 	int32 SegmentSamples = 5;
  40: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  41: 	FVector LocalOffset = FVector::ZeroVector;
  43: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1", Units="cm"))
  44: 	float Radius = 10.f;
  45: };
  48: struct FHodgeHitGeometry
  49: {
  50: 	TArray<FVector> Points;
  51: 	FTransform Transform = FTransform::Identity;
  52: 	FVector BoxExtent = FVector::ZeroVector;
  53: };
  56: UCLASS(BlueprintType, Const)
  57: class HODGEPODGE_API UHodgeHitDetectionProfile : public UDataAsset
  58: {
  59: 	GENERATED_BODY()
  60: public:
  61: 	UHodgeHitDetectionProfile();
  63: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  64: 	TSubclassOf<UHodgeHitDetectionStrategy> Strategy;
  67: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  68: 	TArray<TEnumAsByte<ECollisionChannel>> ObjectTypes;
  70: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  71: 	bool bRequireLineOfSight = true;
  73: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  74: 	TEnumAsByte<ECollisionChannel> ObstructionChannel = ECC_Visibility;
  77: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="180"))
  78: 	float HalfAngleDegrees = 180.f;
  81: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", Units="cm"))
  82: 	float MaxSweepDistance = 300.f;
  85: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="32"))
  86: 	int32 RotationSubsteps = 8;
  88: 	bool Validate(TArray<FText>& Errors) const;
  89: #if WITH_EDITOR
  90: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
  91: #endif
  92: };
  95: USTRUCT(BlueprintType)
  96: struct HODGEPODGE_API FHodgeHitDetectionRequest
  97: {
  98: 	GENERATED_BODY()
 100: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Combat.Source"))
 101: 	FGameplayTag SourceTag;
 103: 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
 104: 	TObjectPtr<UHodgeHitDetectionProfile> Profile;
 106: 	UPROPERTY()
 107: 	TArray<TWeakObjectPtr<AActor>> IgnoredActors;
 108: };
 111: USTRUCT(BlueprintType)
 112: struct HODGEPODGE_API FHodgeHitDetectionBatch
 113: {
 114: 	GENERATED_BODY()
 116: 	UPROPERTY(BlueprintReadOnly) FGuid ExecutionId;
 117: 	UPROPERTY(BlueprintReadOnly) int32 EventIndex = INDEX_NONE;
 118: 	UPROPERTY() uint64 SessionHandle = 0;
 119: 	UPROPERTY(BlueprintReadOnly) int32 SampleSequence = 0;
 120: 	UPROPERTY(BlueprintReadOnly) double SampleTime = 0.0;
 121: 	UPROPERTY(BlueprintReadOnly) TArray<FHitResult> Hits;
 122: 	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<USceneComponent> SourceComponent;
 123: 	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<UHodgeWeaponInstance> Weapon;
 124: 	UPROPERTY(BlueprintReadOnly) FVector SourceOrigin = FVector::ZeroVector;
 125: };
 128: USTRUCT(BlueprintType)
 129: struct HODGEPODGE_API FHodgeHitWindowBinding
 130: {
 131: 	GENERATED_BODY()
 133: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Status.Attack.HitCheck"))
 134: 	FGameplayTag WindowTag;
 136: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Combat.Source"))
 137: 	FGameplayTag SourceTag;
 139: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 140: 	TObjectPtr<UHodgeHitDetectionProfile> Profile;
 143: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 144: 	TSubclassOf<UGameplayEffect> DamageEffect;
 146: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
 147: 	float DamageMultiplier = 1.f;
 149: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="GameplayEffect.DamageType"))
 150: 	FGameplayTag DamageType;
 153: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", Units="s"))
 154: 	float RepeatHitInterval = 0.f;
 157: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 158: 	FName HitGroup;
 160: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 161: 	bool bAllowFriendlyFire = false;
 162: };
 165: UCLASS(Abstract, BlueprintType)
 166: class HODGEPODGE_API UHodgeHitDetectionStrategy : public UObject
 167: {
 168: 	GENERATED_BODY()
 169: public:
 170: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 171: 		FHodgeHitGeometry& OutGeometry) const PURE_VIRTUAL(UHodgeHitDetectionStrategy::Capture, return false;);
 172: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 173: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 174: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const
 175: 		PURE_VIRTUAL(UHodgeHitDetectionStrategy::Detect, );
 176: };
 178: UCLASS()
 179: class HODGEPODGE_API UHodgeSocketSweepStrategy : public UHodgeHitDetectionStrategy
 180: {
 181: 	GENERATED_BODY()
 182: public:
 183: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 184: 		FHodgeHitGeometry& OutGeometry) const override;
 185: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 186: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 187: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
 188: };
 190: UCLASS()
 191: class HODGEPODGE_API UHodgeBoxSweepStrategy : public UHodgeHitDetectionStrategy
 192: {
 193: 	GENERATED_BODY()
 194: public:
 195: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 196: 		FHodgeHitGeometry& OutGeometry) const override;
 197: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 198: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 199: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
 200: };
```
