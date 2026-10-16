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
	// 攻击Judgement标签的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Combat.Attack.Judgement,State.Combat.Body")) FGameplayTag AttackJudgementTag;
	// Impacts的记录集合。
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FHodgeImpactSpec> Impacts;
	// 仅预留，不消费韧性属性。
	UPROPERTY(BlueprintReadWrite) float ReservedPoiseDamage = 0.f;
	// 只影响接触表现的强度，不改变伤害、判定或控制时长。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.1", ClampMax="2")) float PresentationStrength = 1.f;
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
	// 序号的数值或阈值。
	UPROPERTY(BlueprintReadOnly) int32 Sequence = 0;
	// Phase的枚举配置或当前取值。
	UPROPERTY(BlueprintReadOnly) EHodgeHitReactionPhase Phase = EHodgeHitReactionPhase::None;
	// 类型的枚举配置或当前取值。
	UPROPERTY(BlueprintReadOnly) EHodgeImpactType Type = EHodgeImpactType::HitStun;
	// 开始时刻的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float StartedAt = 0.f;
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
	// 控制是否存在判断移动的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bHasMovement = false;
	// 控制是否Knockdown事件落地的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bKnockdownOnLanding = false;
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
	/** 检查是否允许Use蒙太奇。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @param Mesh 网格，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	HODGEPODGE_API bool CanUseMontage(const UAnimMontage* Montage, const USkeletalMesh* Mesh);
}
