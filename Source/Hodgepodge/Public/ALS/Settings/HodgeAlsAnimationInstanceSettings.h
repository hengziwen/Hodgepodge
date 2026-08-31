#pragma once

#include "HodgeAlsCrouchingSettings.h"
#include "HodgeAlsDynamicTransitionsSettings.h"
#include "HodgeAlsFeetSettings.h"
#include "HodgeAlsGeneralAnimationSettings.h"
#include "HodgeAlsGroundedSettings.h"
#include "HodgeAlsInAirSettings.h"
#include "HodgeAlsRotateInPlaceSettings.h"
#include "HodgeAlsStandingSettings.h"
#include "HodgeAlsTransitionsSettings.h"
#include "HodgeAlsTurnInPlaceSettings.h"
#include "HodgeAlsViewAnimationSettings.h"
#include "Engine/DataAsset.h"
#include "HodgeAlsAnimationInstanceSettings.generated.h"

UCLASS(Blueprintable, BlueprintType)
class HODGEPODGE_API UHodgeAlsAnimationInstanceSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsGeneralAnimationSettings General;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsViewAnimationSettings View;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsGroundedSettings Grounded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsStandingSettings Standing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsCrouchingSettings Crouching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsInAirSettings InAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsFeetSettings Feet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsTransitionsSettings Transitions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsDynamicTransitionsSettings DynamicTransitions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsRotateInPlaceSettings RotateInPlace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsGeneralTurnInPlaceSettings TurnInPlace;

public:
	UHodgeAlsAnimationInstanceSettings();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent) override;
#endif
};
