#pragma once

#include "HodgeAlsLeanState.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsLeanState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = -1, ClampMax = 1))
	float RightAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = -1, ClampMax = 1))
	float ForwardAmount{0.0f};
};
