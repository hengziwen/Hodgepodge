#pragma once

#include "HodgeAlsTurnInPlaceState.generated.h"

class UHodgeAlsTurnInPlaceSettings;

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsTurnInPlaceState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bUpdatedThisFrame : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ForceUnits = "s"))
	float ActivationDelay{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UHodgeAlsTurnInPlaceSettings> QueuedSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FName QueuedSlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float QueuedTurnYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "x"))
	float PlayRate{1.0f};
};
