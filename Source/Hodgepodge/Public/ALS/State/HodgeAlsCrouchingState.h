#pragma once

#include "HodgeAlsCrouchingState.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsCrouchingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ClampMax = 1))
	float StrideBlendAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ClampMax = 2, ForceUnits = "x"))
	float PlayRate{1.0f};
};
