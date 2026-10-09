#pragma once

#include "Engine/DataAsset.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "HodgeHitReactionProfile.generated.h"

/** 每种目标保存自己的受击动作，不由攻击者引用目标骨骼资产。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitReactionAnimation
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeImpactType Type = EHodgeImpactType::HitStun;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Montage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> AirLoopMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> LandingMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> GetUpMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s")) float DefaultDuration = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bAllowWithoutMontage = false;
};

UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeHitReactionProfile : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FHodgeHitReactionAnimation> Animations;
	// 不支持某种效果时可显式降级为 HitStun，仍要求等级已通过。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<EHodgeImpactType> FallbackToHitStun;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UAnimMontage> LightFeedbackMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag LightFeedbackCue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTagContainer InterruptibleAbilityTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s")) float DownedDuration = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s")) float MaxControlDuration = 6.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s")) float MaxAirborneDuration = 4.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", Units="cm")) float MaxLaunchHeight = 600.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", Units="cm/s")) float MaxLaunchSpeed = 2500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) int32 MaxAirHits = 6;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bAutoGetUp = true;
	const FHodgeHitReactionAnimation* FindAnimation(EHodgeImpactType Type) const;
	bool Validate(TArray<FText>& Errors) const;
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
