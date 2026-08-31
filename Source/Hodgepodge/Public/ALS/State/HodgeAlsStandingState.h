#pragma once

#include "HodgeAlsStandingState.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsStandingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ClampMax = 1))
	float StrideBlendAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ClampMax = 1))
	float WalkRunBlendAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ClampMax = 3, ForceUnits = "x"))
	float PlayRate{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ClampMax = 1))
	float SprintBlockAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float SprintTime{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = -1, ClampMax = 1))
	float SprintAccelerationAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bPivotActive : 1 {false};
};
