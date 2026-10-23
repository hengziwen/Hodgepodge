/** @file HodgeHitReactionProfile.h
 * @brief 声明目标受击动画与恢复配置相关的类型、接口与现有处理流程。
 */
#pragma once

#include "Engine/DataAsset.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "HodgeHitReactionProfile.generated.h"

class UNiagaraSystem;
class USoundBase;
class UCurveFloat;
class USkeletalMesh;

/** 同一 Impact 内的方向与力度变体；不改变服务器打断判定。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitReactionVariant
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitPresentationDirection Direction = EHodgeHitPresentationDirection::Front;
	// 选取不超过本次力度的最高阈值；同方向同阈值禁止重复。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1", ClampMax="2")) float MinimumStrength = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Montage;
};

/** 目标骨架对应的方向性局部动作，继续使用既有 Additive Slot。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeDirectionalHitFeedback
{
	GENERATED_BODY()
	// 方向由目标局部坐标确定，不依赖观察相机。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitPresentationDirection Direction = EHodgeHitPresentationDirection::Front;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Montage;
};

/** 每种目标保存自己的受击动作，不由攻击者引用目标骨骼资产。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitReactionAnimation
{
	GENERATED_BODY()
	// 类型的枚举配置或当前取值。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeImpactType Type = EHodgeImpactType::HitStun;
	// 本次动作使用的蒙太奇。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Montage;
	// AirLoop蒙太奇的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> AirLoopMontage;
	// 落地蒙太奇的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> LandingMontage;
	// 获取Up蒙太奇的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> GetUpMontage;
	// 默认持续时间的数值或阈值。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s", DisplayName="Default Control Duration", EditCondition="Type == EHodgeImpactType::HitStun || Type == EHodgeImpactType::Knockback", EditConditionHides)) float DefaultDuration = 0.4f;
	// 控制是否AllowWithout蒙太奇的状态标记。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bAllowWithoutMontage = false;
	// 缺少匹配条目仍使用 Montage，旧资产不需要迁移。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation") TArray<FHodgeHitReactionVariant> Variants;
	// 可选持续段与恢复段必须成对配置，默认空值保留旧资产行为。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation") FName SustainSection;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation") FName RecoverySection;
	// 左右素材不伪装成四向；正面自动选择可按服务器反应身份交替。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation") bool bUseLateralVariants = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Motion") EHodgeReactionMotionSource MotionSource = EHodgeReactionMotionSource::Impact;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Motion") EHodgeReactionMotionSource LandingMotionSource = EHodgeReactionMotionSource::Impact;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Motion") EHodgeReactionMotionSource GetUpMotionSource = EHodgeReactionMotionSource::Impact;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation", meta=(ClampMin="0", Units="s")) float AirLoopTransitionTime = 0.f;
};

/** 左右现有动作只填写一次；不把方向伪装成额外玩法等级。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeGroundReactionPair
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Left;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Right;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s")) float ControlDuration = 0.4f;
};

/** 起始、空中循环、真实落地三个片段保持各自语义。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAirReactionFamily
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Start;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Loop;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Landing;
};

/** 精简作者结构；运行时仍解析为原有 State 的各阶段 Montage。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeCompactReactionAnimations
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeGroundReactionPair Light;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeGroundReactionPair Heavy;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAirReactionFamily Fly;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAirReactionFamily Push;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Downed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> GetUp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, meta=(ClampMin="0", Units="s")) float AirLoopTransitionTime = 0.18f;
};

/** 单个受击转向段的有限补转参数；仅保存值，不执行朝向变更。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitReactionFacingPreset
{
	GENERATED_BODY()
	// 本段相对实际起始朝向的最大修正角。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Units="deg")) float MaxCorrectionAngle = 30.f;
	// 本段允许补转的有效时长，不延长原受击控制。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Units="s")) float WindowDuration = 0.08f;
	// 实际移动步内的水平角速率上限。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Units="deg/s")) float MaxYawSpeed = 720.f;
	/** 仅追加本组非法数值的诊断；返回值只反映本次是否新增错误。 */
	bool Validate(TArray<FText>& Errors) const;
};

/** 目标受击有限补转配置；默认关闭，数值采用玩家保守组。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitReactionFacingSettings
{
	GENERATED_BODY()
	FHodgeHitReactionFacingSettings();
	// 仅显式启用的目标参与受击补转，旧资产保持原行为。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bEnabled = false;
	// 水平角差处于死区内时保持当前朝向。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Units="deg")) float DeadZoneAngle = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeHitReactionFacingPreset Light;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeHitReactionFacingPreset Heavy;
	// 只供首次进入击飞使用，空中后续命中不重新取用。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeHitReactionFacingPreset Launch;
	/** 禁用时保留休眠数值；启用时追加三组预设及死区诊断，不清空既有错误。 */
	bool Validate(TArray<FText>& Errors) const;
};

/** 封装目标受击动画与恢复配置的状态与操作接口。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeHitReactionProfile : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// 显式选择唯一配置来源；保留旧属性序列化以支持备份和回退。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bUseCompactAnimations = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bUseCompactAnimations", EditConditionHides)) FHodgeCompactReactionAnimations Compact;
	// Animations的记录集合。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="!bUseCompactAnimations", EditConditionHides)) TArray<FHodgeHitReactionAnimation> Animations;
	// 有限补转与动画布局独立；配置本身不申请或执行朝向控制。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Facing") FHodgeHitReactionFacingSettings Facing;
	/** 按当前布局解析一份条目；不缓存可失效的数组指针，不改变复制结构。 */
	bool ResolveAnimation(EHodgeImpactType Type, FHodgeHitReactionAnimation& Out) const;
	/** 仅查询所选布局，不回落到隐藏的旧配置。 */
	bool SupportsImpact(EHodgeImpactType Type) const;
	// 不支持某种效果时可显式降级为 HitStun，仍要求等级已通过。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay) TArray<EHodgeImpactType> FallbackToHitStun;
	// LightFeedback蒙太奇的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UAnimMontage> LightFeedbackMontage;
	// 新资产按方向选取；未配置的旧目标继续使用 LightFeedbackMontage。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") TArray<FHodgeDirectionalHitFeedback> DirectionalLightFeedback;
	// 接触音画与强受击是否成功分开，资源为空时不伪造可用效果。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") TObjectPtr<UNiagaraSystem> ContactEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") TObjectPtr<USoundBase> ContactSound;
	/** 根据目标局部攻击来源选择局部动作，缺少方向条目时保留旧入口。 */
	UAnimMontage* FindLightFeedback(FVector LocalSourceDirection) const;
	// LightFeedbackGameplayCue的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag LightFeedbackCue;
	// Interruptible能力标签的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay) FGameplayTagContainer InterruptibleAbilityTags;
	// Downed持续时间的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s")) float DownedDuration = 1.f;
	// 最大控制持续时间的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay, meta=(ClampMin="0.01", Units="s")) float MaxControlDuration = 6.f;
	// 最大Airborne持续时间的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay, meta=(ClampMin="0.01", Units="s")) float MaxAirborneDuration = 4.f;
	// 最大抛飞高度的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay, meta=(ClampMin="1", Units="cm")) float MaxLaunchHeight = 600.f;
	// 最大抛飞速度的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay, meta=(ClampMin="1", Units="cm/s")) float MaxLaunchSpeed = 2500.f;
	// 最大AirHits的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay, meta=(ClampMin="0")) int32 MaxAirHits = 6;
	// 控制是否自动获取Up的状态标记。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bAutoGetUp = true;
	// 只控制目标受击替换的混出，不影响攻击时机和自然退出混出。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation", meta=(ClampMin="0", Units="s")) float ReplacementBlendOut = 0.05f;
	// 地面硬直保留最后姿态直到控制结束，避免延长控制后提前退回移动层。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") bool bSustainGroundHitStun = true;
	// 仅自然释放且仍有有效内容时保留原地恢复尾部；取消、死亡和解绑始终清理。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") bool bAllowNaturalRecoveryTail = false;
	// 根运动恢复仍移动胶囊，显式开放并在输入、新动作或阶段失效时精确取消。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Motion", meta=(EditCondition="!bUseCompactAnimations", EditConditionHides)) bool bAllowRootMotionRecoveryTail = false;
	// 0 保留旧续播策略；正数允许独立新命中在间隔达到后进入下一受力动作。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation", AdvancedDisplay, meta=(ClampMin="0", Units="s", EditCondition="!bUseCompactAnimations", EditConditionHides)) float GroundReplayInterval = 0.f;
	// 归一化时间的非负速度权重；空值保持现有恒力运动。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Motion") TObjectPtr<UCurveFloat> KnockbackStrengthCurve;
	// 只对地面 HitStun 开放分时恢复，旧目标默认保持完整控制。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recovery") bool bEnableGroundRecovery = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recovery", meta=(ClampMin="0", Units="s", EditCondition="bEnableGroundRecovery")) float DodgeRecoveryDelay = 0.2f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recovery", meta=(ClampMin="0", Units="s", EditCondition="bEnableGroundRecovery")) float AttackRecoveryDelay = 0.3f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recovery", meta=(ClampMin="0", Units="s", EditCondition="bEnableGroundRecovery")) float MovementRecoveryDelay = 0.4f;
	/** 纯配置判定不解除控制，实际行动仍需通过 GAS 提交和服务器检查。 */
	bool IsRecoveryWindowOpen(const FHodgeHitReactionState& State, EHodgeHitRecoveryAction Action, float Now) const;
	/** 根据接触快照选择强动画，方向和力度不参与玩法判定。 */
	UAnimMontage* FindReactionMontage(EHodgeImpactType Type, FVector LocalSourceDirection, float Strength,
		EHodgeHitSide Side = EHodgeHitSide::Automatic, int32 Variation = 0, EHodgeReactionLevel Level = EHodgeReactionLevel::None) const;
	/** 解析实际使用的主片与单个变体，未选中方向只由完整资产校验检查。 */
	bool ResolvePresentation(EHodgeImpactType Type, FVector LocalSourceDirection, float Strength,
		EHodgeHitSide Side, int32 Variation, FHodgeHitReactionAnimation& Out,
		EHodgeReactionLevel Level = EHodgeReactionLevel::None) const;
	/** 校验已解析计划的实际资源、骨架和相关安全参数，不扫描其他动画家族。 */
	bool ValidatePlanPresentation(const FHodgeHitReactionAnimation& Selection, const FHodgeHitReactionState& Plan,
		const USkeletalMesh* Mesh, TArray<FText>& Errors) const;
	/** 返回速度曲线积分和峰值，非法曲线返回 0，供校验与位移归一化共用。 */
	float GetKnockbackCurveIntegral(float& OutPeak) const;
	/** 持续控制的姿态由目标配置保持，结束仍由服务器计划驱动。 */
	bool ShouldSustainPose(const FHodgeHitReactionState& State) const;
	/** 晚到的受控状态映射到持续段，不能把世界时间直接夹取到恢复段末尾。 */
	float GetPresentationPosition(const FHodgeHitReactionState& State, float Elapsed) const;
	/** 查找动画。
	 * @param Type 类型，由调用方传入。
	 * @return 动画的当前值或引用。
	 */
	const FHodgeHitReactionAnimation* FindAnimation(EHodgeImpactType Type) const;
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
private:
	/** 共用条目校验，完整资产传入全变体，运行计划只传入已选变体。 */
	static void ValidateAnimationEntry(const FHodgeHitReactionAnimation& Entry, TArray<FText>& Errors);
};
