#pragma once

#include "HodgeAlsDynamicTransitionsSettings.generated.h"

class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsDynamicTransitionsSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float FootLockDistanceThreshold{8.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float BlendDuration{0.2f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "x"))
	float PlayRate{1.5f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UAnimSequenceBase> StandingLeftSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UAnimSequenceBase> StandingRightSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UAnimSequenceBase> CrouchingLeftSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UAnimSequenceBase> CrouchingRightSequence;
};
