#pragma once

#include "Animation/PoseSnapshot.h"
#include "HodgeAlsRagdollingAnimationState.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsRagdollingAnimationState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FPoseSnapshot FinalRagdollPose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ClampMax = 1, ForceUnits = "x"))
	float FlailPlayRate{1.0f};
};
