#pragma once

#include "HodgeAlsSpineState.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsSpineState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bSpineRotationAllowed : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ClampMax = 1))
	float SpineAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "x"))
	float SpineAmountScale{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	float SpineAmountBias{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float LastYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float CurrentYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float YawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float LastActorYawAngle{0.0f};
};
