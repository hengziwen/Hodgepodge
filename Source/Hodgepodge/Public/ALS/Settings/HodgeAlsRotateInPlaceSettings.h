#pragma once

#include "HodgeAlsRotateInPlaceSettings.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsRotateInPlaceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float ViewYawAngleThreshold{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0))
	FVector2f ReferenceViewYawSpeed{180.0f, 460.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0))
	FVector2f PlayRate{1.15f, 3.0f};
};
