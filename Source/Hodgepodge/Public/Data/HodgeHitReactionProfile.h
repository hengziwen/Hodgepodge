/** @file HodgeHitReactionProfile.h
 * @brief 声明目标受击动画与恢复配置相关的类型、接口与现有处理流程。
 */
#pragma once

#include "Engine/DataAsset.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "HodgeHitReactionProfile.generated.h"

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s")) float DefaultDuration = 0.4f;
	// 控制是否AllowWithout蒙太奇的状态标记。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bAllowWithoutMontage = false;
};

/** 封装目标受击动画与恢复配置的状态与操作接口。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeHitReactionProfile : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// Animations的记录集合。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FHodgeHitReactionAnimation> Animations;
	// 不支持某种效果时可显式降级为 HitStun，仍要求等级已通过。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<EHodgeImpactType> FallbackToHitStun;
	// LightFeedback蒙太奇的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UAnimMontage> LightFeedbackMontage;
	// LightFeedbackGameplayCue的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag LightFeedbackCue;
	// Interruptible能力标签的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTagContainer InterruptibleAbilityTags;
	// Downed持续时间的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s")) float DownedDuration = 1.f;
	// 最大控制持续时间的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s")) float MaxControlDuration = 6.f;
	// 最大Airborne持续时间的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s")) float MaxAirborneDuration = 4.f;
	// 最大抛飞高度的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", Units="cm")) float MaxLaunchHeight = 600.f;
	// 最大抛飞速度的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", Units="cm/s")) float MaxLaunchSpeed = 2500.f;
	// 最大AirHits的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) int32 MaxAirHits = 6;
	// 控制是否自动获取Up的状态标记。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bAutoGetUp = true;
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
};
