#pragma once

#include "HodgeAlsTurnInPlaceSettings.generated.h"

class UAnimSequenceBase;

UCLASS(BlueprintType, EditInlineNew)
class HODGEPODGE_API UHodgeAlsTurnInPlaceSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UAnimSequenceBase> Sequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ForceUnits = "x"))
	float PlayRate{1.2f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bScalePlayRateByAnimatedTurnAngle : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0.0001, ClampMax = 180, ForceUnits = "deg"))
	float AnimatedTurnAngle{0.0f};
};

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsGeneralTurnInPlaceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float ViewYawAngleThreshold{45.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "deg/s"))
	float ViewYawSpeedThreshold{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0))
	FVector2f ViewYawAngleToActivationDelay{0.0f, 0.75f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS",
		DisplayName = "Turn 180 Angle Threshold", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float Turn180AngleThreshold{130.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float BlendDuration{0.2f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Instanced, DisplayName = "Standing Turn 90 Left")
	TObjectPtr<UHodgeAlsTurnInPlaceSettings> StandingTurn90Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Instanced, DisplayName = "Standing Turn 90 Right")
	TObjectPtr<UHodgeAlsTurnInPlaceSettings> StandingTurn90Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Instanced, DisplayName = "Standing Turn 180 Left")
	TObjectPtr<UHodgeAlsTurnInPlaceSettings> StandingTurn180Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Instanced, DisplayName = "Standing Turn 180 Right")
	TObjectPtr<UHodgeAlsTurnInPlaceSettings> StandingTurn180Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Instanced, DisplayName = "Crouching Turn 90 Left")
	TObjectPtr<UHodgeAlsTurnInPlaceSettings> CrouchingTurn90Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Instanced, DisplayName = "Crouching Turn 90 Right")
	TObjectPtr<UHodgeAlsTurnInPlaceSettings> CrouchingTurn90Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Instanced, DisplayName = "Crouching Turn 180 Left")
	TObjectPtr<UHodgeAlsTurnInPlaceSettings> CrouchingTurn180Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Instanced, DisplayName = "Crouching Turn 180 Right")
	TObjectPtr<UHodgeAlsTurnInPlaceSettings> CrouchingTurn180Right;
};
