#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AlphaBlend.h"
#include "Animation/AnimMontage.h"
#include "GameplayTagContainer.h"
#include "Combat/HodgeHitDetection.h"
#include "HodgeAbilityDefinition.generated.h"

class UHodgeGameplayAbility;
class UAnimMontage;

UENUM(BlueprintType)
enum class EHodgeAbilityExecutionRoute : uint8 { ComboCoordinated, Standalone };

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
	FAlphaBlend MakeBlend() const;
	bool IsValid() const;
};

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
	// 此 Definition 的技能身份；连段节点通过这个标签查找已授予的技能。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag AbilityTag;
	// 执行此配置的 GA 类型；命中通知使用 Melee 子类，当前要求逐角色实例和本地预测。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UHodgeGameplayAbility> AbilityClass;
	// 本次动作的蒙太奇、播放倍率、混合设置。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FHodgeAbilityExecutionConfig ExecutionConfig;
	// 连段技能走协调器授权，Standalone 技能走正常 GAS 激活流程。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EHodgeAbilityExecutionRoute ExecutionRoute = EHodgeAbilityExecutionRoute::ComboCoordinated;
	// 仅独立 Definition 使用；授予时写入 Spec 的输入标签。
	// Standalone 授予时写入 Spec 的输入标签，不会自动创建输入动作或按键映射。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="InputTag")) FGameplayTag InputTag;
	// 当前动作的默认伤害参数，命中通知可继承或显式覆盖。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat") FHodgeHitEffectConfig DefaultHitConfig;
	UFUNCTION(BlueprintPure) float GetDuration() const;
	bool ValidateDefinition(TArray<FText>& Errors) const;
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
