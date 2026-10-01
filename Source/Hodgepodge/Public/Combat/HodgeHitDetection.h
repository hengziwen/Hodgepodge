#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagContainer.h"
#include "HodgeHitDetection.generated.h"

class UGameplayEffect;
class USceneComponent;
class UHodgeHitDetectionStrategy;
class UHodgeWeaponInstance;
struct FCollisionQueryParams;

/** 来源只描述几何位置，检测算法和伤害参数由攻击配置决定。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitSource
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Combat.Source"))
	FGameplayTag SourceTag;

	// 组件对象名；留空使用角色 Mesh 或武器 Actor 的根组件。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ComponentName;

	// 武器有多个表现 Actor 时必须指定下标。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 WeaponActorIndex = 0;

	// 空列表使用组件原点；两个 Socket 可沿连线增加刀身采样点。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> Sockets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="2", ClampMax="32"))
	int32 SegmentSamples = 5;

	// 相对组件或 Socket 的偏移，适用于本体前方的检测范围。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector LocalOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1", Units="cm"))
	float Radius = 10.f;
};

// 每个会话独立保存上一帧几何，策略类默认对象不保存运行状态。
struct FHodgeHitGeometry
{
	TArray<FVector> Points;
	FTransform Transform = FTransform::Identity;
	FVector BoxExtent = FVector::ZeroVector;
};

/** 可复用的检测配置，不保存命中列表或历史坐标。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeHitDetectionProfile : public UDataAsset
{
	GENERATED_BODY()
public:
	UHodgeHitDetectionProfile();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UHodgeHitDetectionStrategy> Strategy;

	// 默认只查询 Pawn；项目使用自定义受击通道时在此调整。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TEnumAsByte<ECollisionChannel>> ObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRequireLineOfSight = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> ObstructionChannel = ECC_Visibility;

	// 180 表示全方向，其他值按角色朝向过滤水平夹角。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="180"))
	float HalfAngleDegrees = 180.f;

	// 大位移视为传送，只检测新位置，避免横扫整条传送路径。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", Units="cm"))
	float MaxSweepDistance = 300.f;

	// 盒体旋转采用有限次插值查询，并非精确的旋转连续碰撞。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="32"))
	int32 RotationSubsteps = 8;

	bool Validate(TArray<FText>& Errors) const;
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};

/** 检测请求只描述来源和几何，不携带效果或伤害参数。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitDetectionRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Combat.Source"))
	FGameplayTag SourceTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UHodgeHitDetectionProfile> Profile;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> IgnoredActors;
};

/** 单次采样结果保留空间信息，由启动检测的 GA 决定用途。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitDetectionBatch
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FGuid ExecutionId;
	UPROPERTY(BlueprintReadOnly) int32 EventIndex = INDEX_NONE;
	UPROPERTY() uint64 SessionHandle = 0;
	UPROPERTY(BlueprintReadOnly) int32 SampleSequence = 0;
	UPROPERTY(BlueprintReadOnly) double SampleTime = 0.0;
	UPROPERTY(BlueprintReadOnly) TArray<FHitResult> Hits;
	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<USceneComponent> SourceComponent;
	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<UHodgeWeaponInstance> Weapon;
	UPROPERTY(BlueprintReadOnly) FVector SourceOrigin = FVector::ZeroVector;
};

/** GA 的命中窗口配置；只有来源和 Profile 会传入检测组件。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitWindowBinding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Status.Attack.HitCheck"))
	FGameplayTag WindowTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Combat.Source"))
	FGameplayTag SourceTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UHodgeHitDetectionProfile> Profile;

	// 显式选择项目现有的伤害 GE，常规伤害使用 GameplayEffectParent_Damage_Basic 或其子类。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	float DamageMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="GameplayEffect.DamageType"))
	FGameplayTag DamageType;

	// 0 表示每目标只命中一次；正数允许按世界时间间隔再次命中。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", Units="s"))
	float RepeatHitInterval = 0.f;

	// 同次执行中相同组名共享命中记录；留空时每个窗口独立去重。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName HitGroup;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAllowFriendlyFire = false;
};

/** 策略只执行几何查询，目标资格和效果应用由 GA 处理。 */
UCLASS(Abstract, BlueprintType)
class HODGEPODGE_API UHodgeHitDetectionStrategy : public UObject
{
	GENERATED_BODY()
public:
	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
		FHodgeHitGeometry& OutGeometry) const PURE_VIRTUAL(UHodgeHitDetectionStrategy::Capture, return false;);
	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const
		PURE_VIRTUAL(UHodgeHitDetectionStrategy::Detect, );
};

UCLASS()
class HODGEPODGE_API UHodgeSocketSweepStrategy : public UHodgeHitDetectionStrategy
{
	GENERATED_BODY()
public:
	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
		FHodgeHitGeometry& OutGeometry) const override;
	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
};

UCLASS()
class HODGEPODGE_API UHodgeBoxSweepStrategy : public UHodgeHitDetectionStrategy
{
	GENERATED_BODY()
public:
	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
		FHodgeHitGeometry& OutGeometry) const override;
	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
};
