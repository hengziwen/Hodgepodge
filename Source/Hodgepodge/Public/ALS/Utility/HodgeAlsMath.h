#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HodgeAlsMath.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsSpringFloatState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	float Velocity{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	float PreviousTarget{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bStateValid : 1 {false};

public:
	void Reset();
};

UCLASS(Meta = (BlueprintThreadSafe))
class HODGEPODGE_API UHodgeAlsMath : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "HODGEALS|Math Utility", Meta = (ReturnDisplayName = "Value"))
	static float Clamp01(float Value);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Math Utility", Meta = (ReturnDisplayName = "Value"))
	static float LerpClamped(float From, float To, float Ratio);

	template <typename ValueType>
	static ValueType Damp(const ValueType& Current, const ValueType& Target, float DeltaTime, float Smoothing);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Math Utility", Meta = (ReturnDisplayName = "Interpolation Amount"))
	static float Damp(float DeltaTime, float Smoothing);

	template <typename ValueType>
	static ValueType ExponentialDecay(const ValueType& Current, const ValueType& Target, float DeltaTime, float Lambda);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Math Utility", Meta = (ReturnDisplayName = "Interpolation Amount"))
	static float ExponentialDecay(float DeltaTime, float Lambda);

	template <typename ValueType, typename StateType>
	static ValueType SpringDamp(StateType& SpringState, const ValueType& Current, const ValueType& Target,
	                            float DeltaTime, float Frequency, float DampingRatio, float TargetVelocityAmount = 1.0f);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Math Utility", Meta = (ReturnDisplayName = "Value"))
	static float SpringDampFloat(UPARAM(ref) FHodgeAlsSpringFloatState& SpringState, float Current, float Target,
	                             float DeltaTime, float Frequency, float DampingRatio, float TargetVelocityAmount = 1.0f);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Math Utility", Meta = (ReturnDisplayName = "Direction"))
	static EHodgeAlsMovementDirection CalculateMovementDirection(float Angle, float ForwardHalfAngle, float AngleThreshold);

	// Calculates the projection location and direction of the perpendicular to AC through B.
	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Math Utility",
		Meta = (AutoCreateRefTerm = "ALocation, BLocation, CLocation", ExpandBoolAsExecs = "ReturnValue"))
	static bool TryCalculatePoleVector(const FVector& ALocation, const FVector& BLocation, const FVector& CLocation,
	                                   FVector& ProjectionLocation, FVector& PoleDirection);
};

inline void FHodgeAlsSpringFloatState::Reset()
{
	Velocity = 0.f;
	PreviousTarget = 0.f;
	bStateValid = false;
}

inline float UHodgeAlsMath::Clamp01(const float Value)
{
	return Value <= 0.0f
		       ? 0.0f
		       : Value >= 1.0f
		       ? 1.0f
		       : Value;
}

inline float UHodgeAlsMath::LerpClamped(const float From, const float To, const float Ratio)
{
	return From + (To - From) * Clamp01(Ratio);
}

template <typename ValueType>
ValueType UHodgeAlsMath::Damp(const ValueType& Current, const ValueType& Target, const float DeltaTime, const float Smoothing)
{
	return Smoothing > 0.0f
		       ? FMath::Lerp(Current, Target, Damp(DeltaTime, Smoothing))
		       : Target;
}

inline float UHodgeAlsMath::Damp(const float DeltaTime, const float Smoothing)
{
	// https://www.rorydriscoll.com/2016/03/07/frame-rate-independent-damping-using-lerp/

	return 1.0f - FMath::Pow(Smoothing, DeltaTime);
}

inline float UHodgeAlsMath::ExponentialDecay(const float DeltaTime, const float Lambda)
{
	// https://www.rorydriscoll.com/2016/03/07/frame-rate-independent-damping-using-lerp/

	return 1.0f - FMath::InvExpApprox(Lambda * DeltaTime);
}

template <typename ValueType>
ValueType UHodgeAlsMath::ExponentialDecay(const ValueType& Current, const ValueType& Target, const float DeltaTime, const float Lambda)
{
	return Lambda > 0.0f
		       ? FMath::Lerp(Current, Target, ExponentialDecay(DeltaTime, Lambda))
		       : Target;
}

template <typename ValueType, typename StateType>
ValueType UHodgeAlsMath::SpringDamp(StateType& SpringState, const ValueType& Current, const ValueType& Target, const float DeltaTime,
                               const float Frequency, const float DampingRatio, const float TargetVelocityAmount)
{
	if (DeltaTime <= UE_SMALL_NUMBER)
	{
		return Current;
	}

	if (!SpringState.bStateValid)
	{
		SpringState.Velocity = ValueType{0.0f};
		SpringState.PreviousTarget = Target;
		SpringState.bStateValid = true;

		return Target;
	}

	ValueType Result{Current};
	FMath::SpringDamper(Result, SpringState.Velocity, Target,
	                    (Target - SpringState.PreviousTarget) * Clamp01(TargetVelocityAmount) / DeltaTime,
	                    DeltaTime, Frequency, DampingRatio);

	SpringState.PreviousTarget = Target;

	return Result;
}
