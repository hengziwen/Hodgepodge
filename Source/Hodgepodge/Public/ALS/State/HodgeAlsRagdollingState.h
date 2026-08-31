#pragma once

#include "HodgeAlsRagdollingState.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsRagdollingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FVector Velocity{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ForceUnits = "N"))
	float PullForce{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0))
	int32 SpeedLimitFrameTimeRemaining{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float SpeedLimit{0.0f};
};
