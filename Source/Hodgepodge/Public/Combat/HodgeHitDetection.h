/** @file HodgeHitDetection.h
 * @brief 声明命中检测配置相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagContainer.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "HodgeHitDetection.generated.h"

class UGameplayEffect;
class USceneComponent;
class UHodgeHitDetectionStrategy;
class UHodgeWeaponInstance;
struct FCollisionQueryParams;

/** 定义命中几何Mode的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeHitGeometryMode : uint8 { /* Existing来源选项。 */ ExistingSource, /* Configured形状选项。 */ ConfiguredShape };
/** 定义命中形状的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeHitShape : uint8 { /* 球体选项。 */ Sphere, /* 盒体选项。 */ Box, /* 胶囊选项。 */ Capsule };
/** 定义命中锚点Kind的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeHitAnchorKind : uint8 { /* 表现角色根节点选项。 */ AvatarRoot, /* Registered来源选项。 */ RegisteredSource, /* 执行变换选项。 */ ExecutionTransform, /* 执行目标选项。 */ ExecutionTarget };
/** 定义命中变换策略的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeHitTransformPolicy : uint8 { /* Follow选项。 */ Follow, /* 快照事件事件Enter选项。 */ SnapshotOnEventEnter };
/** 定义命中查询Mode的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeHitQueryMode : uint8 { /* 扫掠选项。 */ Sweep, /* 重叠选项。 */ Overlap };
/** 定义命中采样Mode的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeHitSampleMode : uint8 { /* Every帧选项。 */ EveryFrame, /* Once事件Enter选项。 */ OnceOnEnter };
/** 定义命中筛选帧的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeHitFilterFrame : uint8 { /* Caster选项。 */ Caster, /* 检测锚点选项。 */ DetectionAnchor };
/** 定义命中目标策略的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeHitTargetPolicy : uint8 { /* Any输入Volume选项。 */ AnyInVolume, /* 锁定目标输入Volume选项。 */ LockedTargetInVolume, /* Confirmed目标选项。 */ ConfirmedTarget };
/** 定义命中结果Kind的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeHitResultKind : uint8 { /* 扫掠选项。 */ Sweep, /* 重叠选项。 */ Overlap, /* Confirmed目标选项。 */ ConfirmedTarget };

/** 技能自己的检测体；尺寸不写回角色组件或武器来源。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitVolumeConfig
{
	GENERATED_BODY()
	// ExistingSource 沿用来源组件几何，ConfiguredShape 使用本技能配置的虚拟检测体。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitGeometryMode GeometryMode = EHodgeHitGeometryMode::ExistingSource;
	// 配置检测体的形状；球、盒和胶囊只读取各自对应的尺寸字段。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
	EHodgeHitShape Shape = EHodgeHitShape::Sphere;
	// 检测体的位置来源；角色根、注册组件、服务器世界变换或已选择目标。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
	EHodgeHitAnchorKind AnchorKind = EHodgeHitAnchorKind::AvatarRoot;
	// Follow 每次采样读取锚点，SnapshotOnEventEnter 在本段开始时固定位置和朝向。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
	EHodgeHitTransformPolicy TransformPolicy = EHodgeHitTransformPolicy::Follow;
	// 相对锚点的位置和旋转偏移，Scale 必须为 1，检测尺寸不继承锚点缩放。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
	FTransform LocalTransform = FTransform::Identity;
	// 配置球体的半径，单位厘米，必须大于 0。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Sphere", EditConditionHides, ClampMin="0.1", Units="cm"))
	float SphereRadius = 100.f;
	// 配置盒体的三个半尺寸，单位厘米，各轴必须大于 0。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Box", EditConditionHides, Units="cm"))
	FVector BoxHalfExtent = FVector(100.f);
	// 配置胶囊体的半径，单位厘米，必须大于 0。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Capsule", EditConditionHides, ClampMin="0.1", Units="cm"))
	float CapsuleRadius = 50.f;
	// 配置胶囊体的半高，包含端帽，必须不小于胶囊半径。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Capsule", EditConditionHides, ClampMin="0.1", Units="cm"))
	float CapsuleHalfHeight = 100.f;
	// RegisteredSource 使用的单个骨骼或插槽，留空取来源组件变换。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName AnchorSocket;
	// ExecutionTransform 对应的运行期变换键，由服务器 GA 提供而不是资产中的世界坐标。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName AnchorKey;
	// 限制运行期锚点与施法者的距离，配置本身来自服务器资产。
	// 配置锚点离施法者允许的最大距离，单位厘米，创建和采样时检查。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", Units="cm")) float MaxAnchorDistance = 5000.f;
	/** 检查当前配置并收集诊断。
	 * @param Errors 收集配置或执行失败的诊断。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool Validate(TArray<FText>& Errors) const;
};

/** 来源只描述几何位置，检测算法和伤害参数由攻击配置决定。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitSource
{
	GENERATED_BODY()

	// 注册检测来源的标识；组件路径要求与角色或武器来源精确匹配，纯配置锚点可以不填。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Combat.Source"))
	FGameplayTag SourceTag;

	// 组件对象名；留空使用角色 Mesh 或武器 Actor 的根组件。
	// 来源组件的实际对象名；留空时角色取 Mesh，普通 Actor 取根组件。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ComponentName;

	// 武器有多个表现 Actor 时必须指定下标。
	// 武器生成的 Actor 数组下标，仅武器来源使用，下标从 0 开始。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 WeaponActorIndex = 0;

	// 空列表使用组件原点；两个 Socket 可沿连线增加刀身采样点。
	// 刀身采样插槽；空列表取组件原点，一个取单点，两个之间插值，多于两个逐个采样。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> Sockets;

	// 仅两个 Socket 时使用的刀身采样点数，包含两端，当前有效范围为 2 到 32。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="2", ClampMax="32"))
	int32 SegmentSamples = 5;

	// 相对组件或 Socket 的偏移，适用于本体前方的检测范围。
	// 来源原点或 Socket 局部坐标中的采样偏移，组件盒体策略不读取它。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector LocalOffset = FVector::ZeroVector;

	// SocketSweep 每个采样点的球扫半径，单位厘米，配置检测体读取 Volume 自己的尺寸。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1", Units="cm"))
	float Radius = 10.f;
};

// 每个会话独立保存上一帧几何，策略类默认对象不保存运行状态。
struct FHodgeHitGeometry
{
	// Points的记录集合。
	TArray<FVector> Points;
	// 变换的几何或变换数据。
	FTransform Transform = FTransform::Identity;
	// 盒体Extent的几何或变换数据。
	FVector BoxExtent = FVector::ZeroVector;
	// 形状的枚举配置或当前取值。
	EHodgeHitShape Shape = EHodgeHitShape::Sphere;
	// 半径的数值或阈值。
	float Radius = 0.f;
	// Half高度的数值或阈值。
	float HalfHeight = 0.f;
};

/** 可复用的检测配置，不保存命中列表或历史坐标。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeHitDetectionProfile : public UDataAsset
{
	GENERATED_BODY()
public:
	/** 构造命中检测配置并初始化其默认状态。 */
	UHodgeHitDetectionProfile();

	// 默认只查询 Pawn；项目使用自定义受击通道时在此调整。
	// 查询的目标碰撞对象类型，不是伤害类型；默认只查询 Pawn。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TEnumAsByte<ECollisionChannel>> ObjectTypes;

	// 开启后会检查过滤原点到目标之间的遮挡，不通过视线检查的几何命中被剔除。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRequireLineOfSight = true;

	// 视线检查使用的 Trace Channel，只有需要视线时才参与过滤。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> ObstructionChannel = ECC_Visibility;

	// 180 表示全方向，其他值按角色朝向过滤水平夹角。
	// 水平扇区的半角，180 表示全方向，原点和朝向由 FilterFrame 决定。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="180"))
	float HalfAngleDegrees = 180.f;

	// 大位移视为传送，只检测新位置，避免横扫整条传送路径。
	// 未开启连续位移时的单次位移保护阈值，超过后只检测新位置，单位厘米。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", Units="cm"))
	float MaxSweepDistance = 300.f;

	// 盒体旋转采用有限次插值查询，并非精确的旋转连续碰撞。
	// 盒体和配置胶囊旋转的有限插值查询次数，当前范围为 1 到 32。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="32"))
	int32 RotationSubsteps = 8;
	// Sweep 查询相邻采样点间的运动，Overlap 查询当前位置；旧组件策略只支持 Sweep。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitQueryMode QueryMode = EHodgeHitQueryMode::Sweep;
	// Window 可每帧检测或仅进入时检测一次；Point 无论此值如何都只检测一次。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitSampleMode SampleMode = EHodgeHitSampleMode::EveryFrame;
	// Caster 用角色位置和朝向过滤，DetectionAnchor 用实际检测区域位置和朝向过滤。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitFilterFrame FilterFrame = EHodgeHitFilterFrame::Caster;
	// 显式连续位移才覆盖大位移路径；瞬移保持默认保护并重置历史。
	// 明确用于真实连续冲刺的大位移扫掠；瞬移后应重置几何历史以免扫伤沿途。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bContinuousMotion = false;

	/** 检查当前配置并收集诊断。
	 * @param Errors 收集配置或执行失败的诊断。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool Validate(TArray<FText>& Errors) const;
#if WITH_EDITOR
	/** 在编辑器校验当前配置并收集诊断。
	 * @param Context 本次操作的上下文。
	 * @return 当前配置的校验结果。
	 */
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};

/** 检测请求只描述来源和几何，不携带效果或伤害参数。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitDetectionRequest
{
	GENERATED_BODY()

	// 注册检测来源的标识；组件路径要求与角色或武器来源精确匹配，纯配置锚点可以不填。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Combat.Source"))
	FGameplayTag SourceTag;

	// 可复用的查询和过滤策略资产，不保存本次技能的坐标或命中历史。
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UHodgeHitDetectionProfile> Profile;
	// 本技能或本次请求的检测体配置，不会修改角色组件和武器来源的尺寸。
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FHodgeHitVolumeConfig Volume;
	// 运行期由服务器准备的世界变换，不是内容作者填写的固定世界位置。
	UPROPERTY() FTransform RuntimeAnchor = FTransform::Identity;
	// 运行期世界锚点是否已经提供，缺失时拒绝查询而不是退回世界零点。
	UPROPERTY() bool bHasRuntimeAnchor = false;
	// 本次查询持有的目标弱引用，目标失效后不能继续沿用旧对象。
	UPROPERTY() TWeakObjectPtr<AActor> RuntimeTarget;
	// 范围内所有目标、范围内指定目标或确认目标直接结算三种目标选择策略。
	UPROPERTY() EHodgeHitTargetPolicy TargetPolicy = EHodgeHitTargetPolicy::AnyInVolume;
	// 指定目标离施法者允许的最大距离，单位厘米，确认目标也必须通过此检查。
	UPROPERTY() float MaxTargetDistance = 2000.f;

	// 本次直接解析的组件，仅供执行入口使用，不需要全局来源注册。
	UPROPERTY() TWeakObjectPtr<USceneComponent> DirectComponent;
	// Direct来源的数据或对象引用。
	UPROPERTY() FHodgeHitSource DirectSource;

	// 本次请求额外忽略的 Actor 弱引用列表，不改变伤害资格规则。
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> IgnoredActors;
};

/** 单次采样结果保留空间信息，由启动检测的 GA 决定用途。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitDetectionBatch
{
	GENERATED_BODY()

	// 运行期技能执行身份，每次激活重新生成，用于拒绝旧执行的结果。
	UPROPERTY(BlueprintReadOnly) FGuid ExecutionId;
	// 运行期 通知进入时分配的运行身份，每次进入独立区分会话。
	UPROPERTY(BlueprintReadOnly) int32 OccurrenceId = INDEX_NONE;
	// 运行期检测会话句柄，不是资产配置或全局命中组名称。
	UPROPERTY() uint64 SessionHandle = 0;
	// 运行期会话内递增的采样序号，用于拒绝重复或过期批次。
	UPROPERTY(BlueprintReadOnly) int32 SampleSequence = 0;
	// 采样发生的世界时间，单位秒，用于每目标重复命中间隔判断。
	UPROPERTY(BlueprintReadOnly) double SampleTime = 0.0;
	// 几何查询返回的命中列表，后续还要通过目标资格和命中次数检查。
	UPROPERTY(BlueprintReadOnly) TArray<FHitResult> Hits;
	// 本批次的来源组件弱引用，虚拟检测体可能没有对应组件。
	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<USceneComponent> SourceComponent;
	// 本批次解析到的武器实例弱引用，本体和虚拟范围攻击可以为空。
	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<UHodgeWeaponInstance> Weapon;
	// 本批次的实际检测原点，用于效果上下文和距离计算。
	UPROPERTY(BlueprintReadOnly) FVector SourceOrigin = FVector::ZeroVector;
	// 标识结果来自扫掠、范围重叠或确认目标，后两者不保证真实刀刃接触信息。
	UPROPERTY(BlueprintReadOnly) EHodgeHitResultKind ResultKind = EHodgeHitResultKind::Sweep;
};

/** 动作默认效果参数与本次命中快照，不包含动画时机或来源绑定。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitEffectConfig
{
	GENERATED_BODY()
	// 控制是否允许BeDodged的状态标记。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Defense") bool bCanBeDodged = true;
	// 控制是否允许Trigger完美闪避闪避的状态标记。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Defense") bool bCanTriggerPerfectDodge = true;
	// 受击的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction") FHodgeHitReactionConfig Reaction;
	// 显式伤害 GE，默认生命伤害要求 Instant 且包含 HodgeDamageExecution。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<UGameplayEffect> DamageEffect;
	// 伤害倍率，命中通知的 DamageScale 再乘在此值上。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float DamageMultiplier = 1.f;
	// 写入伤害 Spec 的分类标签，不会增加一笔独立伤害。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="GameplayEffect.DamageType")) FGameplayTag DamageType;
	// 以下字段是通知进入时生成的运行快照，不是 Definition 上的重复配置。
	UPROPERTY(BlueprintReadOnly) EHodgeHitTargetPolicy TargetPolicy = EHodgeHitTargetPolicy::AnyInVolume;
	// 0 表示本记录内每目标一次，正数允许按世界秒重复命中。
	UPROPERTY(BlueprintReadOnly) float RepeatHitInterval = 0.f;
	// 共享命中记录的显式组名，留空每次通知进入独立去重。
	UPROPERTY(BlueprintReadOnly) FName HitGroup;
	// 同组的显式攻击阶段，留空共享整次执行。
	UPROPERTY(BlueprintReadOnly) FName AttackPhase;
	// 同队目标的伤害许可，仍不允许自身、失效或已经死亡目标。
	UPROPERTY(BlueprintReadOnly) bool bAllowFriendlyFire = false;
};

/** 策略只执行几何查询，目标资格和效果应用由 GA 处理。 */
UCLASS(Abstract, BlueprintType)
class HODGEPODGE_API UHodgeHitDetectionStrategy : public UObject
{
	GENERATED_BODY()
public:
	/** 采集当前动画或测试世界的状态。
	 * @param Component 组件，由调用方传入。
	 * @param Source 本次操作的来源。
	 * @param OutGeometry 输出几何。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
		FHodgeHitGeometry& OutGeometry) const PURE_VIRTUAL(UHodgeHitDetectionStrategy::Capture, return false;);
	/** 按当前命中配置采集候选结果。
	 * @param World 本次操作所在的世界。
	 * @param Source 本次操作的来源。
	 * @param Profile 当前操作使用的配置对象。
	 * @param Previous 更新前的状态。
	 * @param Current 当前，由调用方传入。
	 * @param Params 参数，由调用方传入。
	 * @param OutHits 输出Hits。
	 */
	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const
		PURE_VIRTUAL(UHodgeHitDetectionStrategy::Detect, );
};

/** 封装插槽扫掠Strategy的状态与操作接口。 */
UCLASS()
class HODGEPODGE_API UHodgeSocketSweepStrategy : public UHodgeHitDetectionStrategy
{
	GENERATED_BODY()
public:
	/** 采集当前动画或测试世界的状态。
	 * @param Component 组件，由调用方传入。
	 * @param Source 本次操作的来源。
	 * @param OutGeometry 输出几何。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
		FHodgeHitGeometry& OutGeometry) const override;
	/** 按当前命中配置采集候选结果。
	 * @param World 本次操作所在的世界。
	 * @param Source 本次操作的来源。
	 * @param Profile 当前操作使用的配置对象。
	 * @param Previous 更新前的状态。
	 * @param Current 当前，由调用方传入。
	 * @param Params 参数，由调用方传入。
	 * @param OutHits 输出Hits。
	 */
	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
};

/** 封装盒体扫掠Strategy的状态与操作接口。 */
UCLASS()
class HODGEPODGE_API UHodgeBoxSweepStrategy : public UHodgeHitDetectionStrategy
{
	GENERATED_BODY()
public:
	/** 采集当前动画或测试世界的状态。
	 * @param Component 组件，由调用方传入。
	 * @param Source 本次操作的来源。
	 * @param OutGeometry 输出几何。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
		FHodgeHitGeometry& OutGeometry) const override;
	/** 按当前命中配置采集候选结果。
	 * @param World 本次操作所在的世界。
	 * @param Source 本次操作的来源。
	 * @param Profile 当前操作使用的配置对象。
	 * @param Previous 更新前的状态。
	 * @param Current 当前，由调用方传入。
	 * @param Params 参数，由调用方传入。
	 * @param OutHits 输出Hits。
	 */
	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
};

/** 输入为已解析的世界几何，不依赖场景中存在对应碰撞组件。 */
UCLASS()
class HODGEPODGE_API UHodgeShapeQueryStrategy : public UHodgeHitDetectionStrategy
{
	GENERATED_BODY()
public:
	/** 采集当前动画或测试世界的状态。
	 * @param Component 组件，由调用方传入。
	 * @param Source 本次操作的来源。
	 * @param OutGeometry 输出几何。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
		FHodgeHitGeometry& OutGeometry) const override;
	/** 按当前命中配置采集候选结果。
	 * @param World 本次操作所在的世界。
	 * @param Source 本次操作的来源。
	 * @param Profile 当前操作使用的配置对象。
	 * @param Previous 更新前的状态。
	 * @param Current 当前，由调用方传入。
	 * @param Params 参数，由调用方传入。
	 * @param OutHits 输出Hits。
	 */
	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
};
