#pragma once

#include "HodgeAlsRollingState.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsRollingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float TargetYawAngle{0.0f};
};
