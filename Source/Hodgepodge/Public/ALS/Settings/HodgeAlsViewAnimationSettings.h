#pragma once

#include "HodgeAlsViewAnimationSettings.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsViewAnimationSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0))
	float LookTowardsCameraRotationInterpolationSpeed{8.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0))
	float LookTowardsInputYawAngleInterpolationSpeed{8.0f};
};
