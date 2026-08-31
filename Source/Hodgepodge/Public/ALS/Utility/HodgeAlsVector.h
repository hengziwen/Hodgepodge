#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HodgeAlsVector.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsSpringVectorState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FVector Velocity{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FVector PreviousTarget{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bStateValid : 1 {false};

public:
	void Reset();
};

UCLASS(Meta = (BlueprintThreadSafe))
class HODGEPODGE_API UHodgeAlsVector : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "HODGEALS|Vector Utility", Meta = (AutoCreateRefTerm = "Vector", ReturnDisplayName = "Vector"))
	static FVector ClampMagnitude01(const FVector& Vector);

	static FVector3f ClampMagnitude01(const FVector3f& Vector);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Vector Utility", DisplayName = "Clamp Magnitude 01 2D",
		Meta = (AutoCreateRefTerm = "Vector", ReturnDisplayName = "Vector"))
	static FVector2D ClampMagnitude012D(const FVector2D& Vector);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Vector Utility", Meta = (ReturnDisplayName = "Direction"))
	static FVector2D RadianToDirection(float Radian);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Vector Utility", Meta = (ReturnDisplayName = "Direction"))
	static FVector RadianToDirectionXY(float Radian);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Vector Utility", Meta = (ReturnDisplayName = "Direction"))
	static FVector2D AngleToDirection(float Angle);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Vector Utility", Meta = (ReturnDisplayName = "Direction"))
	static FVector AngleToDirectionXY(float Angle);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Vector Utility", Meta = (AutoCreateRefTerm = "Direction", ReturnDisplayName = "Angle"))
	static double DirectionToAngle(const FVector2D& Direction);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Vector Utility", Meta = (AutoCreateRefTerm = "Direction", ReturnDisplayName = "Angle"))
	static double DirectionToAngleXY(const FVector& Direction);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Vector Utility", Meta = (AutoCreateRefTerm = "Vector", ReturnDisplayName = "Vector"))
	static FVector PerpendicularClockwiseXY(const FVector& Vector);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Vector Utility", Meta = (AutoCreateRefTerm = "Vector", ReturnDisplayName = "Vector"))
	static FVector PerpendicularCounterClockwiseXY(const FVector& Vector);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Vector Utility", DisplayName = "Angle Between (Skip Normalization)",
		Meta = (AutoCreateRefTerm = "From, To", ReturnDisplayName = "Angle"))
	static double AngleBetweenSkipNormalization(const FVector& From, const FVector& To);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Vector Utility", Meta = (AutoCreateRefTerm = "From, To", ReturnDisplayName = "Angle"))
	static float AngleBetweenSignedXY(const FVector3f& From, const FVector3f& To);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Vector Utility", DisplayName = "Slerp (Skip Normalization)",
		Meta = (AutoCreateRefTerm = "From, To", ReturnDisplayName = "Direction"))
	static FVector SlerpSkipNormalization(const FVector& From, const FVector& To, float Ratio);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Vector Utility", Meta = (AutoCreateRefTerm = "Current, Target", ReturnDisplayName = "Vector"))
	static FVector SpringDampVector(UPARAM(ref) FHodgeAlsSpringVectorState& SpringState, const FVector& Current, const FVector& Target,
	                                float DeltaTime, float Frequency, float DampingRatio, float TargetVelocityAmount = 1.0f);
};

inline void FHodgeAlsSpringVectorState::Reset()
{
	Velocity = FVector::ZeroVector;
	PreviousTarget = FVector::ZeroVector;
	bStateValid = false;
}

inline FVector UHodgeAlsVector::ClampMagnitude01(const FVector& Vector)
{
	const auto MagnitudeSquared{Vector.SizeSquared()};

	if (MagnitudeSquared <= 1.0f)
	{
		return Vector;
	}

	const auto Scale{FMath::InvSqrt(MagnitudeSquared)};

	return {Vector.X * Scale, Vector.Y * Scale, Vector.Z * Scale};
}

inline FVector3f UHodgeAlsVector::ClampMagnitude01(const FVector3f& Vector)
{
	const auto MagnitudeSquared{Vector.SizeSquared()};

	if (MagnitudeSquared <= 1.0f)
	{
		return Vector;
	}

	const auto Scale{FMath::InvSqrt(MagnitudeSquared)};

	return {Vector.X * Scale, Vector.Y * Scale, Vector.Z * Scale};
}

inline FVector2D UHodgeAlsVector::ClampMagnitude012D(const FVector2D& Vector)
{
	const auto MagnitudeSquared{Vector.SizeSquared()};

	if (MagnitudeSquared <= 1.0f)
	{
		return Vector;
	}

	const auto Scale{FMath::InvSqrt(MagnitudeSquared)};

	return {Vector.X * Scale, Vector.Y * Scale};
}

inline FVector2D UHodgeAlsVector::RadianToDirection(const float Radian)
{
	float Sin, Cos;
	FMath::SinCos(&Sin, &Cos, Radian);

	return {Cos, Sin};
}

inline FVector UHodgeAlsVector::RadianToDirectionXY(const float Radian)
{
	float Sin, Cos;
	FMath::SinCos(&Sin, &Cos, Radian);

	return {Cos, Sin, 0.0f};
}

inline FVector2D UHodgeAlsVector::AngleToDirection(const float Angle)
{
	return RadianToDirection(FMath::DegreesToRadians(Angle));
}

inline FVector UHodgeAlsVector::AngleToDirectionXY(const float Angle)
{
	return RadianToDirectionXY(FMath::DegreesToRadians(Angle));
}

inline double UHodgeAlsVector::DirectionToAngle(const FVector2D& Direction)
{
	return FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));
}

inline double UHodgeAlsVector::DirectionToAngleXY(const FVector& Direction)
{
	return FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));
}

inline FVector UHodgeAlsVector::PerpendicularClockwiseXY(const FVector& Vector)
{
	return {Vector.Y, -Vector.X, Vector.Z};
}

inline FVector UHodgeAlsVector::PerpendicularCounterClockwiseXY(const FVector& Vector)
{
	return {-Vector.Y, Vector.X, Vector.Z};
}

inline double UHodgeAlsVector::AngleBetweenSkipNormalization(const FVector& From, const FVector& To)
{
	return FMath::RadiansToDegrees(FMath::Acos(From | To));
}

inline float UHodgeAlsVector::AngleBetweenSignedXY(const FVector3f& From, const FVector3f& To)
{
	const auto FromXY{FVector2f{From}.GetSafeNormal()};
	const auto ToXY{FVector2f{To}.GetSafeNormal()};

	// return FMath::RadiansToDegrees(FMath::Atan2(FromXY ^ ToXY, FromXY | ToXY));

	return FMath::RadiansToDegrees(FMath::Acos(FromXY | ToXY)) * FMath::Sign(FromXY ^ ToXY);
}
