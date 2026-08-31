#pragma once

#include "HodgeAlsMovementBaseState.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsMovementBaseState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UPrimitiveComponent> Primitive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FName BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bBaseChanged : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bHasRelativeLocation : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bHasRelativeRotation : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FVector Location{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FQuat Rotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FRotator DeltaRotation{ForceInit};
};
