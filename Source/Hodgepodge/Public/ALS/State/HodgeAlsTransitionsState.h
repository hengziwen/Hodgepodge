#pragma once

#include "HodgeAlsTransitionsState.generated.h"

class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsTransitionsState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bTransitionsAllowed : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UAnimSequenceBase> QueuedTransitionSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float QueuedTransitionBlendInDuration{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float QueuedTransitionBlendOutDuration{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "x"))
	float QueuedTransitionPlayRate{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float QueuedTransitionStartTime{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bStopTransitionsQueued : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float QueuedStopTransitionsBlendOutDuration{0.0f};
};
