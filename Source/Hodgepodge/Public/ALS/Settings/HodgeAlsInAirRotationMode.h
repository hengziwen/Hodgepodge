#pragma once

#include "HodgeAlsInAirRotationMode.generated.h"

UENUM(BlueprintType)
enum class EHodgeAlsInAirRotationMode : uint8
{
	RotateToVelocityOnJump,
	KeepRelativeRotation,
	KeepWorldRotation
};
