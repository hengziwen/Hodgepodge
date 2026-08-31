#pragma once

#include "HodgeAlsControlRigInput.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsControlRigInput
{
	GENERATED_BODY()

	// TODO Replace bool with uint8 in future engine versions.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	bool bUseHandIkBones{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	bool bUseFootIkBones{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	bool bFootOffsetAllowed{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ClampMax = 1))
	float VelocityBlendForwardAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ClampMax = 1))
	float VelocityBlendBackwardAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FVector FootLeftLocation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FQuat FootLeftRotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FVector FootRightLocation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FQuat FootRightRotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float SpineYawAngle{0.0f};
};
