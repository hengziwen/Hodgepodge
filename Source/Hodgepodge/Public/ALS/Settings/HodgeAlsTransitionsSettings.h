#pragma once

#include "HodgeAlsTransitionsSettings.generated.h"

class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsTransitionsSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float QuickStopBlendInDuration{0.1f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float QuickStopBlendOutDuration{0.2f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0))
	FVector2f QuickStopPlayRate{1.75f, 3.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float QuickStopStartTime{0.3f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UAnimSequenceBase> StandingLeftSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UAnimSequenceBase> StandingRightSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UAnimSequenceBase> CrouchingLeftSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UAnimSequenceBase> CrouchingRightSequence;
};
