#pragma once

#include "HodgeAlsGroundedSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsGroundedSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UCurveFloat> RotationYawOffsetForwardCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UCurveFloat> RotationYawOffsetBackwardCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UCurveFloat> RotationYawOffsetLeftCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UCurveFloat> RotationYawOffsetRightCurve;

	// The higher the value, the faster the interpolation. A zero value results in instant interpolation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0))
	float VelocityBlendInterpolationSpeed{12.0f};
};
