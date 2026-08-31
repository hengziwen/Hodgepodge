#pragma once

#include "Engine/DataAsset.h"
#include "Engine/Scene.h"
#include "ALS/Utility/HodgeAlsConstants.h"
#include "HodgeAlsCameraSettings.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsFirstPersonCameraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 5, ClampMax = 175, ForceUnits = "deg"))
	float FieldOfView{90.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FName CameraSocketName{TEXTVIEW("FirstPersonCamera")};
};

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsTraceDistanceSmoothingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0))
	float InterpolationSpeed{3.0f};
};

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsThirdPersonCameraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 5, ClampMax = 175, ForceUnits = "deg"))
	float FieldOfView{90.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FName FirstPivotSocketName{UHodgeAlsConstants::RootBoneName()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FName SecondPivotSocketName{UHodgeAlsConstants::HeadBoneName()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float TraceRadius{15.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TEnumAsByte<ECollisionChannel> TraceChannel{ECC_Visibility};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FName TraceShoulderLeftSocketName{TEXTVIEW("ThirdPersonTraceShoulderLeft")};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FName TraceShoulderRightSocketName{TEXTVIEW("ThirdPersonTraceShoulderRight")};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FVector3f TraceOverrideOffset{0.0f, 0.0f, 40.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (InlineEditConditionToggle))
	uint8 bEnableTraceDistanceSmoothing : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS",
		DisplayName = "Enable Trace Distance Smoothing", Meta = (EditCondition = "bEnableTraceDistanceSmoothing"))
	FHodgeAlsTraceDistanceSmoothingSettings TraceDistanceSmoothing;
};

UCLASS(Blueprintable, BlueprintType)
class HODGEPODGE_API UHodgeAlsCameraSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bIgnoreTimeDilation : 1 {true};

	// Camera will be teleported if the actor has moved farther than this
	// distance in 1 frame. If zero is specified, teleportation will be disabled.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float TeleportDistanceThreshold{200.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsFirstPersonCameraSettings FirstPerson;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHodgeAlsThirdPersonCameraSettings ThirdPerson;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FPostProcessSettings PostProcess;

public:
#if WITH_EDITORONLY_DATA
	virtual void Serialize(FArchive& Archive) override;
#endif
};
