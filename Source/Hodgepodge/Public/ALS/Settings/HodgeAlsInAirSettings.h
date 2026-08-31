#pragma once

#include "Engine/EngineTypes.h"
#include "HodgeAlsInAirSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsInAirSettings
{
	GENERATED_BODY()

public:
	// Vertical velocity to lean amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UCurveFloat> LeanAmountCurve;

	// Ground prediction sweep hit time to ground prediction amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UCurveFloat> GroundPredictionAmountCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TEnumAsByte<ECollisionChannel> GroundPredictionSweepChannel{ECC_Visibility};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TArray<TEnumAsByte<ECollisionChannel>> GroundPredictionResponseChannels;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HODGEALS", AdvancedDisplay)
	FCollisionResponseContainer GroundPredictionSweepResponses{ECR_Ignore};

public:
#if WITH_EDITOR
	void PostEditChangeProperty(const FPropertyChangedEvent& ChangedEvent);
#endif
};
