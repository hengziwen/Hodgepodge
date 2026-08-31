#pragma once

#include "HodgeAlsRagdollingSettings.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsRagdollingSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bStartRagdollingOnLand : 1 {true};

	// Ragdolling will start if the character lands with a speed greater than the specified value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS",
		Meta = (ClampMin = 0, EditCondition = "bStartRagdollingOnLand", ForceUnits = "cm/s"))
	float RagdollingOnLandSpeedThreshold{1000.0f};

	// If checked, the ragdoll's speed will be limited by the character's last speed for a few frames
	// after activation. This hack is used to prevent the ragdoll from getting a very high initial speed
	// at unstable FPS, which can be reproduced by jumping and activating the ragdoll at the same time.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bLimitInitialRagdollSpeed : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UAnimMontage> GetUpFrontMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UAnimMontage> GetUpBackMontage;
};
