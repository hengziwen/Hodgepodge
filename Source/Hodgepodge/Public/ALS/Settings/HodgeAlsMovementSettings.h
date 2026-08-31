#pragma once

#include "Engine/DataAsset.h"
#include "ALS/Utility/HodgeAlsGameplayTags.h"
#include "HodgeAlsMovementSettings.generated.h"

class UCurveFloat;
class UCurveVector;

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsMovementGaitSettings
{
	GENERATED_BODY()

public:
	// Currently, the direction-dependent movement speed can cause some jitter in multiplayer, so enable it at your own risk.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HODGEALS")
	uint8 bAllowDirectionDependentMovementSpeed : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float WalkForwardSpeed{175.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS",
		Meta = (ClampMin = 0, EditCondition = "bAllowDirectionDependentMovementSpeed", ForceUnits = "cm/s"))
	float WalkBackwardSpeed{175.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float RunForwardSpeed{375.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS",
		Meta = (ClampMin = 0, EditCondition = "bAllowDirectionDependentMovementSpeed", ForceUnits = "cm/s"))
	float RunBackwardSpeed{375.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float SprintSpeed{650.0f};

	// Gait amount to acceleration, deceleration, and ground friction curve.
	// Gait amount ranges from 0 to 3, where 0 is stopped, 1 is walking, 2 is running, and 3 is sprinting.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UCurveVector> AccelerationAndDecelerationAndGroundFrictionCurve;

	// Gait amount to rotation interpolation speed curve.
	// Gait amount ranges from 0 to 3, where 0 is stopped, 1 is walking, 2 is running, and 3 is sprinting.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TObjectPtr<UCurveFloat> RotationInterpolationSpeedCurve;

public:
	float GetMaxWalkSpeed() const;

	float GetMaxRunSpeed() const;
};

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsMovementStanceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ForceInlineRow))
	TMap<FGameplayTag, FHodgeAlsMovementGaitSettings> Stances
	{
		{HodgeAlsStanceTags::Standing, {}},
		{HodgeAlsStanceTags::Crouching, {}}
	};
};

UCLASS(Blueprintable, BlueprintType)
class HODGEPODGE_API UHodgeAlsMovementSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	// Range of velocity angle relative to the view direction at which
	// interpolation from forward speed to backward speed will take place.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	FVector2f VelocityAngleToSpeedInterpolationRange{100.0f, 125.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ForceInlineRow))
	TMap<FGameplayTag, FHodgeAlsMovementStanceSettings> RotationModes
	{
		{HodgeAlsRotationModeTags::VelocityDirection, {}},
		{HodgeAlsRotationModeTags::ViewDirection, {}},
		{HodgeAlsRotationModeTags::Aiming, {}}
	};

public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent) override;
#endif
};

inline float FHodgeAlsMovementGaitSettings::GetMaxWalkSpeed() const
{
	return bAllowDirectionDependentMovementSpeed
		       ? FMath::Max(WalkForwardSpeed, WalkBackwardSpeed)
		       : WalkForwardSpeed;
}

inline float FHodgeAlsMovementGaitSettings::GetMaxRunSpeed() const
{
	return bAllowDirectionDependentMovementSpeed
		       ? FMath::Max(RunForwardSpeed, RunBackwardSpeed)
		       : RunForwardSpeed;
}
