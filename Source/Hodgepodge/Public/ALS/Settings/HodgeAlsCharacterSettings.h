#pragma once

#include "HodgeAlsInAirRotationMode.h"
#include "HodgeAlsMantlingSettings.h"
#include "HodgeAlsRagdollingSettings.h"
#include "HodgeAlsRollingSettings.h"
#include "HodgeAlsViewSettings.h"
#include "HodgeAlsCharacterSettings.generated.h"

UCLASS(Blueprintable, BlueprintType)
class HODGEPODGE_API UHodgeAlsCharacterSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	// Actor will be considered as teleported if he has moved farther than this distance in 1 frame.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float TeleportDistanceThreshold{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float MovingSpeedThreshold{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	EHodgeAlsInAirRotationMode InAirRotationMode{EHodgeAlsInAirRotationMode::RotateToVelocityOnJump};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bAllowAimingWhenInAir : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bSprintHasPriorityOverAiming : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bRotateToVelocityWhenSprinting : 1 {false};

	// If checked, the character will rotate relative to the object it is standing on in the velocity
	// direction rotation mode, otherwise the character will ignore that object and keep its world rotation.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bInheritMovementBaseRotationInVelocityDirectionRotationMode : 1 {false};

	// If checked, the character will rotate towards the direction they want to move, but is not always able to due to obstacles.
	// This setting is only used if the bIgnoreBaseRotation setting from the character movement component is set to true.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bRotateTowardsDesiredVelocityInVelocityDirectionRotationMode : 1 {true};

	// If checked, the character will automatically rotate towards the view direction on any movement input while the actor is not moving.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bAutoRotateOnAnyInputWhileNotMovingInViewDirectionRotationMode : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsViewSettings View;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsGeneralMantlingSettings Mantling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsRagdollingSettings Ragdolling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsRollingSettings Rolling;

public:
	UHodgeAlsCharacterSettings();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent) override;
#endif
};
