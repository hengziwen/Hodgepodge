#pragma once

#include "HodgeAlsGeneralAnimationSettings.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsGeneralAnimationSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bUseHandIkBones : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bUseFootIkBones : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float MovingSmoothSpeedThreshold{150.0f};

	// The higher the value, the faster the interpolation. A zero value results in instant interpolation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0))
	float LeanInterpolationSpeed{4.0f};
};
