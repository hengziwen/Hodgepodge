#include "Combat/HodgeCharacterFacingTypes.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCharacterFacingTypes)

bool HodgeFacing::NormalizeDirection(const FVector& Input, FVector& Output)
{
	Output = FVector::ZeroVector;
	if (Input.ContainsNaN()) { return false; }
	Output = Input.GetSafeNormal2D();
	return !Output.IsNearlyZero();
}

FHodgeMoveIntentSnapshot HodgeFacing::MakeMoveIntent(FVector2D Input, float ControlYaw, double Time, int32 Generation)
{
	FHodgeMoveIntentSnapshot Result;
	Result.AvatarGeneration = Generation;
	Result.SampleTime = FMath::IsFinite(Time) ? Time : 0.;
	if (Input.ContainsNaN() || !FMath::IsFinite(ControlYaw)) { return Result; }
	Result.RawInput2D = Input;
	Result.ControllerYawAtSample = FRotator::NormalizeAxis(ControlYaw);
	Result.InputMagnitude = Input.Size();
	const FVector World = FRotator(0.f, Result.ControllerYawAtSample, 0.f).RotateVector(FVector(Input.Y, Input.X, 0.f));
	NormalizeDirection(World, Result.DesiredDirectionWorld);
	return Result;
}

bool HodgeFacing::IsBaseDriver(EHodgeCharacterFacingDriver Driver)
{
	return Driver == EHodgeCharacterFacingDriver::Movement || Driver == EHodgeCharacterFacingDriver::Controller;
}
