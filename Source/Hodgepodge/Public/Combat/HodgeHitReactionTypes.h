/** @file HodgeHitReactionTypes.h
 * @brief 声明受击判定与Impact数据相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "Engine/EngineTypes.h"
#include "HodgeHitReactionTypes.generated.h"

class UAbilitySystemComponent;
class UAnimMontage;
class USkeletalMesh;

/** 补转等级独立于削韧等级；独立脚本名避免与配置预设结构同名。 */
UENUM(BlueprintType, meta=(ScriptName="HodgeHitReactionFacingPresetKind"))
enum class EHodgeHitReactionFacingPreset : uint8 { None, Light, Heavy, Launch };

/** 命中时冻结的水平位置；不持有来源对象，也不替代选片和运动快照。 */
struct HODGEPODGE_API FHodgeHitReactionFacingSource
{
	FVector SourceLocation = FVector::ZeroVector;
	FVector TargetLocation = FVector::ZeroVector;
	bool bValid = false;
	// 只供目标组件拒绝死亡或换 Avatar 前尚未结算的旧补转候选。
	uint32 TargetGeneration = 0;
};

/** 服务器提交的单个补转段；同级保持身份，结束仍保留实际朝向供同段校正。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitReactionFacingState
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FGuid ChainId;
	UPROPERTY() uint32 Segment = 0;
	UPROPERTY() uint32 Revision = 0;
	UPROPERTY(BlueprintReadOnly) EHodgeHitReactionFacingPreset Preset = EHodgeHitReactionFacingPreset::None;
	UPROPERTY(BlueprintReadOnly) float StartedAt = 0.f;
	UPROPERTY(BlueprintReadOnly) float EndsAt = 0.f;
	UPROPERTY(BlueprintReadOnly) float StartYaw = 0.f;
	UPROPERTY(BlueprintReadOnly) float TargetYaw = 0.f;
	UPROPERTY(BlueprintReadOnly) float MaxYawSpeed = 0.f;
	UPROPERTY(BlueprintReadOnly) float FinalYaw = 0.f;
	// 无效来源仍占据本级尝试，不能被后续同级接触重新取源。
	UPROPERTY(BlueprintReadOnly) bool bHasTarget = false;
	UPROPERTY(BlueprintReadOnly) bool bFinal = false;
	UPROPERTY(BlueprintReadOnly) bool bHardLocked = false;
	// 根运动接管后只保留结束记录与级别，同级不能重新授权旧段。
	UPROPERTY(BlueprintReadOnly) bool bRetired = false;
	bool HasIdentity() const { return ChainId.IsValid() && Segment > 0 && Revision > 0; }
	bool IsTurning() const { return HasIdentity() && bHasTarget && !bFinal && !bRetired; }
};

namespace HodgeHitReactionTags
{
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Body_Normal);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Body_Skill);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Body_SuperArmor);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Body_Vajra);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judgement_Normal);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judgement_Skill);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judgement_SuperArmor);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judgement_Vajra);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Controlled);
}

/** 定义冲击类型的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeImpactType : uint8 { /* 使用强受击硬直。 */ HitStun, /* 使用击退控制。 */ Knockback, /* 使用挑飞控制。 */ Launch, /* 使用击倒控制。 */ Knockdown, /* Air命中选项。 */ AirHit, /* Slam选项。 */ Slam };

/** 接触来源在目标局部坐标中的方向，不增加玩法 Impact 类型。 */
UENUM(BlueprintType)
enum class EHodgeHitPresentationDirection : uint8 { Front, Back, Left, Right };

/** 恢复窗口只放行指定行动，不能作为通用免控标记。 */
UENUM(BlueprintType)
enum class EHodgeHitRecoveryAction : uint8 { None, Dodge, BasicAttack, Movement };

/** 每个阶段只有一种胶囊运动来源；动画根运动仍由 CMC 执行。 */
UENUM(BlueprintType)
enum class EHodgeReactionMotionSource : uint8 { Impact, AnimationRootMotion };

/** 攻击可指定受力侧；自动选择由服务器基于方向与反应身份确定。 */
UENUM(BlueprintType)
enum class EHodgeHitSide : uint8 { Automatic, Left, Right };

/** 普通攻击只提交削韧，特殊技能显式提交 Impact；旧资产保持原语义。 */
UENUM(BlueprintType)
enum class EHodgeReactionInputMode : uint8 { Legacy, PoiseDriven, ExplicitImpact };

/** 目标侧消费的阈值等级，与空中、倒地等执行阶段正交。 */
UENUM(BlueprintType)
enum class EHodgeReactionLevel : uint8 { None, Light, Heavy, Break };

/** 定义冲击方向的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeImpactDirection : uint8 { /* Away来源来源选项。 */ AwayFromSource, /* 来源前向选项。 */ SourceForward, /* 世界方向选项。 */ WorldDirection };

/** 定义命中Acceptance策略的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeHitAcceptancePolicy : uint8 { /* 已接受伤害选项。 */ AcceptedDamage, /* Explicit控制选项。 */ ExplicitControl };

/** 定义命中受击Phase的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeHitReactionPhase : uint8 { /* 没有对应的活动状态或选项。 */ None, /* Controlled选项。 */ Controlled, /* Airborne选项。 */ Airborne, /* Downed选项。 */ Downed, /* GettingUp选项。 */ GettingUp };

/** 定义命中受击Outcome的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeHitReactionOutcome : uint8
{
	// No请求选项。
	NoRequest, /* No伤害输出选项。 */ NoDamageOutput, /* 请求被拒绝。 */ Rejected, /* 死亡选项。 */ Dead, /* 无效请求选项。 */ InvalidRequest, /* LowJudgement选项。 */ LowJudgement,
	// No冲击选项。
	NoImpact, /* Unsupported选项。 */ Unsupported, /* Not就绪选项。 */ NotReady, /* CannotInterrupt选项。 */ CannotInterrupt, /* Busy选项。 */ Busy, /* 请求已经应用。 */ Applied
};

/** 攻击方只提供效果参数，目标动画和恢复规则由目标 Profile 提供。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeImpactSpec
{
	GENERATED_BODY()
	// 类型的枚举配置或当前取值。
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EHodgeImpactType Type = EHodgeImpactType::HitStun;
	// 0 使用目标默认时长。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", Units="s", EditCondition="Type == EHodgeImpactType::HitStun || Type == EHodgeImpactType::Knockback", EditConditionHides)) float ControlDuration = 0.f;
	// 方向的枚举配置或当前取值。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="Type != EHodgeImpactType::HitStun && Type != EHodgeImpactType::Knockdown", EditConditionHides)) EHodgeImpactDirection Direction = EHodgeImpactDirection::AwayFromSource;
	// 世界方向的几何或变换数据。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="Direction == EHodgeImpactDirection::WorldDirection && Type != EHodgeImpactType::HitStun && Type != EHodgeImpactType::Knockdown", EditConditionHides)) FVector WorldDirection = FVector::ForwardVector;
	// 击退距离的数值或阈值。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", Units="cm", EditCondition="Type == EHodgeImpactType::Knockback", EditConditionHides)) float KnockbackDistance = 120.f;
	// 移动持续时间的数值或阈值。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.01", Units="s", EditCondition="Type == EHodgeImpactType::Knockback", EditConditionHides)) float MoveDuration = 0.2f;
	// 水平速度的数值或阈值。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", Units="cm/s", EditCondition="Type == EHodgeImpactType::Launch || Type == EHodgeImpactType::AirHit || Type == EHodgeImpactType::Slam", EditConditionHides)) float HorizontalSpeed = 180.f;
	// Launch/AirHit 向上，Slam 向下；配置保存非负幅值。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", Units="cm/s", EditCondition="Type == EHodgeImpactType::Launch || Type == EHodgeImpactType::AirHit || Type == EHodgeImpactType::Slam", EditConditionHides)) float VerticalSpeed = 650.f;
	// 控制是否Knockdown事件落地的状态标记。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="Type == EHodgeImpactType::Launch || Type == EHodgeImpactType::AirHit", EditConditionHides)) bool bKnockdownOnLanding = false;
	/** 判断移动。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsMovement() const;
	/** 检查当前配置并收集诊断。
	 * @param Errors 收集配置或执行失败的诊断。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool Validate(TArray<FText>& Errors) const;
};

/** 保存命中受击配置的数据和接口契约。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitReactionConfig
{
	GENERATED_BODY()
	// 由命中分段解析后传入；不序列化为默认反应，防止出现第二个作者开关。
	UPROPERTY(Transient, BlueprintReadOnly) bool bCanLaunch = false;
	// 同一命中只能选择一个控制裁决入口。
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EHodgeReactionInputMode InputMode = EHodgeReactionInputMode::Legacy;
	// 攻击Judgement标签的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Combat.Attack.Judgement,State.Combat.Body")) FGameplayTag AttackJudgementTag;
	// Impacts的记录集合。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="InputMode != EHodgeReactionInputMode::PoiseDriven", EditConditionHides)) TArray<FHodgeImpactSpec> Impacts;
	// 沿用已序列化的存储字段，只有削韧模式消费，不创建第二个数值来源。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Poise Damage", ClampMin="0", EditCondition="InputMode == EHodgeReactionInputMode::PoiseDriven", EditConditionHides)) float ReservedPoiseDamage = 0.f;
	// 只缩放目标给出的位移计划，不改变削韧与生命伤害。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", ClampMax="4", EditCondition="InputMode == EHodgeReactionInputMode::PoiseDriven", EditConditionHides)) float ImpulseScale = 1.f;
	// 只影响接触表现的强度，不改变伤害、判定或控制时长。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.1", ClampMax="2")) float PresentationStrength = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EHodgeHitSide PresentationSide = EHodgeHitSide::Automatic;
	// 命中Acceptance策略的枚举配置或当前取值。
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EHodgeHitAcceptancePolicy HitAcceptancePolicy = EHodgeHitAcceptancePolicy::AcceptedDamage;
	/** 判断Configured。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsConfigured() const;
	/** 检查当前配置并收集诊断。
	 * @param Errors 收集配置或执行失败的诊断。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool Validate(TArray<FText>& Errors) const;
};

/** 轻量目标规则嵌入 PawnData；不是属性槽，也不拥有第二套受击状态。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgePoiseSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s", EditCondition="bEnabled")) float Window = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01", EditCondition="bEnabled")) float LightThreshold = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01", EditCondition="bEnabled")) float HeavyThreshold = 55.f;
	// 仅保留旧资产序列化；削韧不再消费破韧阈值、保护或击飞结果。
	UPROPERTY() float BreakThreshold = 100.f;
	UPROPERTY() float BreakProtection = 1.f;
	UPROPERTY() EHodgeImpactType BreakImpact = EHodgeImpactType::Launch;
	// 可选霸体积累不会绕过 Body；仅下一次合格的新命中可以重新裁决。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, meta=(EditCondition="bEnabled")) bool bAccumulateUnderArmor = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, meta=(ClampMin="1", ClampMax="1024", EditCondition="bEnabled")) int32 MaxSamples = 128;
	// 地面运动使用目标预设与抵抗系数，动画不再决定胶囊距离。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", Units="cm", EditCondition="bEnabled")) float HeavyDistance = 65.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s", EditCondition="bEnabled")) float MoveDuration = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="4", EditCondition="bEnabled")) float DisplacementScale = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, meta=(ClampMin="0", Units="cm", EditCondition="bEnabled")) float MaxDistance = 200.f;
	// 分段允许击飞时使用的目标速度预设，与是否启用削韧及旧破韧字段无关。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Launch", meta=(ClampMin="0", Units="cm/s")) float LaunchHorizontalSpeed = 180.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Launch", meta=(ClampMin="0", Units="cm/s")) float LaunchVerticalSpeed = 650.f;
	/** 仅校验已启用的规则，隐藏的兼容配置不阻止旧目标加载。 */
	bool Validate(TArray<FText>& Errors) const;
	/** 独立分段击飞只检查实际消费的目标速度与抵抗，不读取轻重窗口。 */
	bool ValidateLaunchMotion(TArray<FText>& Errors) const;
};

/** 服务器确认的一次接触表现；不复制完整 HitResult 或玩法控制状态。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitPresentation
{
	GENERATED_BODY()
	// 同一命中的结算身份用于客户端有界去重。
	UPROPERTY(BlueprintReadOnly) FGuid HitId;
	// 攻击执行与阶段用于调试关联，不作为多接触合并键。
	UPROPERTY(BlueprintReadOnly) FGuid ExecutionId;
	UPROPERTY(BlueprintReadOnly) FName AttackPhase;
	// 没有可靠 Sweep 接触时使用目标附近回退位置，并显式标记。
	UPROPERTY(BlueprintReadOnly) FVector Location = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly) FVector Normal = FVector::UpVector;
	UPROPERTY(BlueprintReadOnly) bool bPreciseContact = false;
	// 仅可靠接触且携带物理材质时声明表面有效，缺失时使用默认材质回退。
	UPROPERTY(BlueprintReadOnly) bool bHasSurface = false;
	UPROPERTY(BlueprintReadOnly) TEnumAsByte<EPhysicalSurface> SurfaceType = SurfaceType_Default;
	// Direction 保留冲击运动方向，动画选择使用其反向的攻击来源。
	UPROPERTY(BlueprintReadOnly) FVector Direction = FVector::ForwardVector;
	// 命中瞬间的局部来源快照，避免 RPC 到达前目标转向改变客户端选片。
	UPROPERTY(BlueprintReadOnly) FVector LocalSourceDirection = FVector::ForwardVector;
	UPROPERTY(BlueprintReadOnly) float Strength = 1.f;
	UPROPERTY(BlueprintReadOnly) bool bLightFeedback = false;
};

/** 保存命中受击结果的数据和接口契约。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitReactionResult
{
	GENERATED_BODY()
	// 仅表示本次成功消费的目标阈值；失败提交不伪报等级。
	UPROPERTY(BlueprintReadOnly) EHodgeReactionLevel ReactionLevel = EHodgeReactionLevel::None;
	UPROPERTY(BlueprintReadOnly) float AccumulatedPoise = 0.f;
	// 命中标识的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) FGuid HitId;
	// 攻击Judgement标签的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) FGameplayTag AttackJudgementTag;
	// 目标体状态标签的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) FGameplayTag TargetBodyTag;
	// 攻击Rank的数值或阈值。
	UPROPERTY(BlueprintReadOnly) int32 AttackRank = 0;
	// 目标体状态Rank的数值或阈值。
	UPROPERTY(BlueprintReadOnly) int32 TargetBodyRank = 0;
	// Actual伤害的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float ActualDamage = 0.f;
	// ReservedPoise伤害的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float ReservedPoiseDamage = 0.f;
	// Outcome的枚举配置或当前取值。
	UPROPERTY(BlueprintReadOnly) EHodgeHitReactionOutcome Outcome = EHodgeHitReactionOutcome::NoRequest;
	// 已应用Impacts的记录集合。
	UPROPERTY(BlueprintReadOnly) TArray<EHodgeImpactType> AppliedImpacts;
};

/** 当前目标的一份解析后计划；不依赖命中时的临时 Spec。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitReactionState
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) EHodgeReactionLevel ReactionLevel = EHodgeReactionLevel::None;
	// 序号的数值或阈值。
	UPROPERTY(BlueprintReadOnly) int32 Sequence = 0;
	// Phase的枚举配置或当前取值。
	UPROPERTY(BlueprintReadOnly) EHodgeHitReactionPhase Phase = EHodgeHitReactionPhase::None;
	// 类型的枚举配置或当前取值。
	UPROPERTY(BlueprintReadOnly) EHodgeImpactType Type = EHodgeImpactType::HitStun;
	// 开始时刻的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float StartedAt = 0.f;
	// 最新一次强受击重开恢复等待；动画续播时不能沿用旧恢复资格。
	UPROPERTY(BlueprintReadOnly) float RecoveryStartedAt = 0.f;
	// Ends时刻的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float EndsAt = 0.f;
	// Chain开始时刻的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float ChainStartedAt = 0.f;
	// Air开始时刻的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float AirStartedAt = 0.f;
	// AirOriginZ的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float AirOriginZ = 0.f;
	// Air命中数量的数值或阈值。
	UPROPERTY(BlueprintReadOnly) int32 AirHitCount = 0;
	// Motion速度的几何或变换数据。
	UPROPERTY(BlueprintReadOnly) FVector MotionVelocity = FVector::ZeroVector;
	// 移动持续时间的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float MoveDuration = 0.f;
	// 仅用于反应动画变体与弱命中续播，不参与 Body／Judgement。
	UPROPERTY(BlueprintReadOnly) float PresentationStrength = 1.f;
	// 控制是否存在判断移动的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bHasMovement = false;
	// 控制是否Knockdown事件落地的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bKnockdownOnLanding = false;
	// 运动来源随阶段复制，模拟端不能根据本地资产重新猜测。
	UPROPERTY(BlueprintReadOnly) EHodgeReactionMotionSource MotionSource = EHodgeReactionMotionSource::Impact;
	UPROPERTY(BlueprintReadOnly) EHodgeReactionMotionSource LandingMotionSource = EHodgeReactionMotionSource::Impact;
	UPROPERTY(BlueprintReadOnly) EHodgeReactionMotionSource GetUpMotionSource = EHodgeReactionMotionSource::Impact;
	UPROPERTY(BlueprintReadOnly) bool bLanding = false;
	// 正数表示起始动作最短播放时间，实际下降后才交接 Loop；0 保留旧时序。
	UPROPERTY(BlueprintReadOnly) float AirLoopTransitionTime = 0.f;
	// 本次动作使用的蒙太奇。
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UAnimMontage> Montage;
	// AirLoop蒙太奇的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UAnimMontage> AirLoopMontage;
	// 落地蒙太奇的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UAnimMontage> LandingMontage;
	// 获取Up蒙太奇的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UAnimMontage> GetUpMontage;
	/** 判断活动。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsActive() const { return Phase != EHodgeHitReactionPhase::None; }
};

namespace HodgeHitReaction
{
	/** 将体状态Tag映射为判定强度。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @return 本次计算、查询或创建得到的整数32。
	 */
	HODGEPODGE_API int32 BodyRank(FGameplayTag Tag);
	/** 将攻击判定Tag映射为比较用强度。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @return 本次计算、查询或创建得到的整数32。
	 */
	HODGEPODGE_API int32 JudgementRank(FGameplayTag Tag);
	/** 解析体状态。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 * @param OutTag 标签，用于对应标签查询或匹配。
	 * @return 本次计算、查询或创建得到的整数32。
	 */
	HODGEPODGE_API int32 ResolveBody(const UAbilitySystemComponent* ASC, FGameplayTag& OutTag);
	/** 检查是否允许冲击。
	 * @param AttackRank 攻击Rank，由调用方传入。
	 * @param BodyRank 体状态Rank，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	HODGEPODGE_API bool CanImpact(int32 AttackRank, int32 BodyRank);
	/** 已倒地或强运动时，较轻的地面请求仅保留接触，不降级当前反应。 */
	HODGEPODGE_API bool IsContactOnlyTransition(const FHodgeHitReactionState& Current, const FHodgeHitReactionConfig& Incoming);
	/** 检查是否允许Use蒙太奇。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @param Mesh 网格，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	HODGEPODGE_API bool CanUseMontage(const UAnimMontage* Montage, const USkeletalMesh* Mesh);
}
