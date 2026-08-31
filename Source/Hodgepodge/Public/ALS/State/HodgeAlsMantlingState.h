#pragma once

#include "HodgeAlsMantlingState.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsMantlingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	int32 RootMotionSourceId = 0;
};
