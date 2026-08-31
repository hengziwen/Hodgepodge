#pragma once

#include "HodgeAlsInAirState.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsInAirState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ForceUnits = "cm/s"))
	float VerticalVelocity{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bJumpRequested : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bJumped : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "x"))
	float JumpPlayRate{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ClampMax = 1))
	float GroundPredictionAmount{1.0f};
};
