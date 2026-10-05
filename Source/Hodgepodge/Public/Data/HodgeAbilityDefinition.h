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

USTRUCT(BlueprintType)
struct FHodgeAbilityBlendSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float Time = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EMontageBlendMode Mode = EMontageBlendMode::Standard;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EAlphaBlendOption Curve = EAlphaBlendOption::Linear;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UCurveFloat> CustomCurve;
	FAlphaBlend MakeBlend() const;
	bool IsValid() const;
};

USTRUCT(BlueprintType)
struct FHodgeTimelineTaskConfig
{
	GENERATED_BODY()
	// Definition 技能第一版固定使用本次 Montage 实例时钟，独立 Task API 保持兼容。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UHodgeAbilityTimeline> Timeline;
};

USTRUCT(BlueprintType)
struct FHodgeAbilityExecutionConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Montage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01")) float PlayRate = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings BlendIn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings NaturalBlendOut;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings StopBlendOut;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeTimelineTaskConfig TimelineTaskConfig;
};

/** 不可变的单段执行定义；通过 ASC 授予记录传入 GA，SourceObject 保留原装备来源。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeAbilityDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag AbilityTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UHodgeGameplayAbility> AbilityClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FHodgeAbilityExecutionConfig ExecutionConfig;
	// 同一 WindowTag 只能配置一次；多个窗口可复用该绑定。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat", meta=(TitleProperty="WindowTag"))
	TArray<FHodgeHitWindowBinding> HitWindows;
	// 留空保持原行为；配置后使用该窗口占用当前武器的手持表现。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Presentation")
	FGameplayTag WeaponUseWindowTag;
	UFUNCTION(BlueprintPure) float GetDuration() const;
	bool ValidateDefinition(TArray<FText>& Errors) const;
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
