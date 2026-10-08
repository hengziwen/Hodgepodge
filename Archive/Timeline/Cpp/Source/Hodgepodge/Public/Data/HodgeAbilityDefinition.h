#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AlphaBlend.h"
#include "Animation/AnimMontage.h"
#include "GameplayTagContainer.h"
#include "Combat/HodgeHitDetection.h"
#include "HodgeAbilityDefinition.generated.h"

class UHodgeGameplayAbility;
class UHodgeAbilityTimeline;
class UAnimMontage;

UENUM(BlueprintType)
enum class EHodgeAbilityExecutionRoute : uint8 { ComboCoordinated, Standalone };

USTRUCT(BlueprintType)
struct FHodgeAbilityBlendSettings
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
struct FHodgeTimelineTaskConfig
{
	GENERATED_BODY()
	// Definition 技能第一版固定使用本次 Montage 实例时钟，独立 Task API 保持兼容。
	// 本次动作使用的逻辑时间轴，Definition 要求它使用蒙太奇时长。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UHodgeAbilityTimeline> Timeline;
};

USTRUCT(BlueprintType)
struct FHodgeAbilityExecutionConfig
{
	GENERATED_BODY()
	// 本次动作播放的蒙太奇，当前只支持一个不循环的线性 Section。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Montage;
	// 动作播放倍率，必须大于 0；Timeline 的时间仍按动画源时间填写。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01")) float PlayRate = 1.f;
	// 开始播放动作时使用的混合设置。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings BlendIn;
	// 动作自然完成时使用的退出混合设置。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings NaturalBlendOut;
	// 动作被取消或切换打断时使用的退出混合设置。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings StopBlendOut;
	// 逻辑时间轴任务的配置，目前包含本次动作引用的 Timeline。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeTimelineTaskConfig TimelineTaskConfig;
};

/** 不可变的单段执行定义；通过 ASC 授予记录传入 GA，SourceObject 保留原装备来源。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeAbilityDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// 此 Definition 的技能身份；连段节点通过这个标签查找已授予的技能。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag AbilityTag;
	// 执行此配置的 GA 类型；命中绑定使用 Melee 子类，当前要求逐角色实例和本地预测。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UHodgeGameplayAbility> AbilityClass;
	// 本次动作的蒙太奇、播放倍率、混合和时间轴设置。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FHodgeAbilityExecutionConfig ExecutionConfig;
	// 连段技能走协调器授权，Standalone 技能走正常 GAS 激活流程。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EHodgeAbilityExecutionRoute ExecutionRoute = EHodgeAbilityExecutionRoute::ComboCoordinated;
	// 仅独立 Definition 使用；授予时写入 Spec 的输入标签。
	// Standalone 授予时写入 Spec 的输入标签，不会自动创建输入动作或按键映射。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="InputTag")) FGameplayTag InputTag;
	// 同一 WindowTag 只能配置一次；多个窗口可复用该绑定。
	// 持续命中绑定；按 WindowTag 精确匹配，同标签多个顺序窗口可以复用一项。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat", meta=(TitleProperty="WindowTag"))
	TArray<FHodgeHitWindowBinding> HitWindows;
	// 单次命中绑定；按 PointEventTag 精确匹配，同标签多个时刻各自独立命中。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat", meta=(TitleProperty="PointEventTag"))
	TArray<FHodgeHitPointBinding> HitPoints;
	// 留空保持原行为；配置后使用该窗口占用当前武器的手持表现。
	// 与 Timeline 的手持窗口精确匹配，控制武器显现和手持请求，不直接开启伤害。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Presentation")
	FGameplayTag WeaponUseWindowTag;
	UFUNCTION(BlueprintPure) float GetDuration() const;
	bool ValidateDefinition(TArray<FText>& Errors) const;
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
