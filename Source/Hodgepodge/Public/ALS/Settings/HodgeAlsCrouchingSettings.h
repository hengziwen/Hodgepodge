#pragma once

#include "HodgeAlsCrouchingSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsCrouchingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float AnimatedCrouchSpeed{150.0f};

	// Movement speed to stride blend amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UCurveFloat> StrideBlendAmountCurve;
};
