#pragma once

#include "HodgeAlsStandingSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsStandingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float AnimatedWalkSpeed{150.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float AnimatedRunSpeed{350.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float AnimatedSprintSpeed{600.0f};

	// Movement speed to stride blend amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UCurveFloat> StrideBlendAmountWalkCurve;

	// Movement speed to stride blend amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UCurveFloat> StrideBlendAmountRunCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float PivotActivationSpeedThreshold{200.0f};
};
