#pragma once

#include "HodgeAlsRollingSettings.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsRollingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bCrouchOnStart : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bRotateToInputOnStart : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0))
	float RotationInterpolationSpeed{10.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bStartRollingOnLand : 1 {true};

	// Rolling will start if the character lands with a speed greater than the specified value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS",
		Meta = (ClampMin = 0, EditCondition = "bStartRollingOnLand", ForceUnits = "cm/s"))
	float RollingOnLandSpeedThreshold{700.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bInterruptRollingWhenInAir : 1 {true};
};
