/** @file HodgeAbilityDefinition.h
 * @brief 声明单段技能执行定义相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AlphaBlend.h"
#include "Animation/AnimMontage.h"
#include "GameplayTagContainer.h"
#include "Combat/HodgeHitDetection.h"
#include "Combat/HodgeAttackFacingTypes.h"
#include "ScalableFloat.h"
#include "HodgeAbilityDefinition.generated.h"

class UHodgeGameplayAbility;
class UHodgeAttackRuleConfig;
class UHodgeAttackRule;
class UHodgeGameplayAbility_Definition;
class UAnimMontage;

/** 区分连段协调器授权与独立GAS激活两条执行路由。 */
UENUM(BlueprintType)
enum class EHodgeAbilityExecutionRoute : uint8 { /* 由连段协调器授权执行。 */ ComboCoordinated, /* 通过GAS常规入口独立执行。 */ Standalone };

/** 保存能力混合设置的数据和接口契约。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAbilityBlendSettings
{
	GENERATED_BODY()
	// 混合持续时间，单位秒；0 表示立即切换，不能为负数。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float Time = 0.1f;
	// 混合模式；当前校验只支持 Standard，不能配置惯性化模式。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EMontageBlendMode Mode = EMontageBlendMode::Standard;
	// 混合权重的变化曲线类型；选择 Custom 时还必须指定 CustomCurve。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EAlphaBlendOption Curve = EAlphaBlendOption::Linear;
	// 自定义混合权重曲线，仅在 Curve 为 Custom 时必填。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UCurveFloat> CustomCurve;
	/** 构造混合。
	 * @return 本次计算、查询或创建得到的混合权重混合。
	 */
	FAlphaBlend MakeBlend() const;
	/** 检查当前对象、配置或句柄是否满足其有效性条件。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsValid() const;
};

/** 保存能力执行配置的数据和接口契约。 */
USTRUCT(BlueprintType)
struct FHodgeAbilityExecutionConfig
{
	GENERATED_BODY()
	// 本次动作播放的蒙太奇，通知直接在该蒙太奇编辑。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Montage;
	// 动作播放倍率，必须大于 0；通知时间按动画源时间填写。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01")) float PlayRate = 1.f;
	// 开始播放动作时使用的混合设置。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings BlendIn;
	// 动作自然完成时使用的退出混合设置。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings NaturalBlendOut;
	// 动作被取消或切换打断时使用的退出混合设置。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings StopBlendOut;
};

/** 不可变的单段执行定义；通过 ASC 授予记录传入 GA，SourceObject 保留原装备来源。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeAbilityDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	/** 为独立技能设置标准移动取消窗口，既有资产无需重新配置。 */
	UHodgeAbilityDefinition();
	// 此 Definition 的技能身份；连段节点通过这个标签查找已授予的技能。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag AbilityTag;
	// 只声明本动作的体状态，不作为输出攻击判定。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat", meta=(Categories="State.Combat.Body")) FGameplayTag ExecutionBodyTag;
	// 执行此配置的 GA 类型；命中通知使用 Melee 子类，当前要求逐角色实例和本地预测。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UHodgeGameplayAbility> AbilityClass;
	// 本次动作的蒙太奇、播放倍率、混合设置。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FHodgeAbilityExecutionConfig ExecutionConfig;
	// 连段技能走协调器授权，Standalone 技能走正常 GAS 激活流程。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EHodgeAbilityExecutionRoute ExecutionRoute = EHodgeAbilityExecutionRoute::ComboCoordinated;
	// 仅独立 Definition 使用；授予时写入 Spec 的输入标签。
	// Standalone 授予时写入 Spec 的输入标签，不会自动创建输入动作或按键映射。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="InputTag")) FGameplayTag InputTag;
	/** 形态资格先于冷却判断，不因分支冷却而回退其他形态。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Variant") FGameplayTagContainer RequiredSourceTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Variant") FGameplayTagContainer BlockedSourceTags;
	/** 可选执行规则拥有专属配置与收益账本；普通 NPC 可留空。 */
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category="Hodge|Rules") TArray<TObjectPtr<UHodgeAttackRuleConfig>> AttackRules;
	/** 仅服务器为本次执行创建规则，失败不启动动作。 */
	bool CreateAttackRules(UHodgeGameplayAbility_Definition* Source, TArray<TObjectPtr<UHodgeAttackRule>>& OutRules) const;
	/** 以下旧字段仅供未迁移资产读取；正式资产统一使用 AttackRules。 */
	/** 常态普攻实际生命伤害按阶段回暗流，默认零保持原技能。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Havoc", meta=(ClampMin="0")) float DarkFlowGainPerPhase=0.f;
	/** 成功施放通知提交后回暗流，空挥有效，提交前取消无收益。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Havoc", meta=(ClampMin="0")) float DarkFlowGainOnCast=0.f;
	/** 只有强化普攻参与角色级拢音限流。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Havoc") bool bEnhancedNormalAttack=false;
	// 独立技能的移动取消通知标签；留空关闭，连段技能仍由 ComboDefinition 配置。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|MoveCancel", meta=(EditCondition="ExecutionRoute == EHodgeAbilityExecutionRoute::Standalone", Categories="Status.Attack.Cancel"))
	FGameplayTag StandaloneMoveCancelWindowTag;
	// 独立技能取消所需的原始移动输入长度，范围 (0, 1]，不是角色实际速度。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|MoveCancel", meta=(EditCondition="ExecutionRoute == EHodgeAbilityExecutionRoute::Standalone", ClampMin="0.01", ClampMax="1"))
	float StandaloneMoveIntentThreshold = .1f;
	// 攻击旋转 Warp 默认关闭，不改变现有 Dash 或伤害时间轴。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Facing") FHodgeAttackFacingConfig FacingWarp;
	/** 获取唯一 Warp 窗口的源时间，失败时不输出隐式窗口。 */
	bool GetFacingWarpWindow(float& OutStart, float& OutEnd) const;
	// 当前动作的默认伤害参数，命中通知可继承或显式覆盖。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat") FHodgeHitEffectConfig DefaultHitConfig;
	/** 每个真正造成生命伤害的阶段回能一次，默认零不改变现有攻击。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Resonance") FScalableFloat ResonanceEnergyGainPerPhase = 0.f;
	/** 使用Instant增量GE，NPC没有资源Set时不额外创建玩家资源。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Resonance") TSubclassOf<UGameplayEffect> ResonanceEnergyGainEffect;
	/** 根据当前配置计算动作持续时间。
	 * @return 持续时间的当前值或引用。
	 */
	UFUNCTION(BlueprintPure) float GetDuration() const;
	/** 检查定义配置及其引用资源是否满足执行契约。
	 * @param Errors 收集配置或执行失败的诊断。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool ValidateDefinition(TArray<FText>& Errors) const;
#if WITH_EDITOR
	/** 在编辑器校验当前配置并收集诊断。
	 * @param Context 本次操作的上下文。
	 * @return 当前配置的校验结果。
	 */
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
