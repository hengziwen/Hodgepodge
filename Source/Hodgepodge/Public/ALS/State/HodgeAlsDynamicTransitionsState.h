#pragma once

#include "HodgeAlsDynamicTransitionsState.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsDynamicTransitionsState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bUpdatedThisFrame : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	int32 FrameDelay{0};
};
