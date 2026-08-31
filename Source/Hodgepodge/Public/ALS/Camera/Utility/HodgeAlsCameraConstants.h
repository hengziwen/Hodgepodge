#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HodgeAlsCameraConstants.generated.h"

UCLASS(Meta = (BlueprintThreadSafe))
class HODGEPODGE_API UHodgeAlsCameraConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Animation Curves

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& CameraOffsetXCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& CameraOffsetYCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& CameraOffsetZCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FovOffsetCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PivotOffsetXCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PivotOffsetYCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PivotOffsetZCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LocationLagXCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LocationLagYCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LocationLagZCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& RotationLagCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FirstPersonOverrideCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& TraceOverrideCurveName();

	// Debug

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& CameraCurvesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& CameraShapesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Camera Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& CameraTracesDebugDisplayName();
};

inline const FName& UHodgeAlsCameraConstants::CameraOffsetXCurveName()
{
	static const FName Name{TEXTVIEW("CameraOffsetX")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::CameraOffsetYCurveName()
{
	static const FName Name{TEXTVIEW("CameraOffsetY")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::CameraOffsetZCurveName()
{
	static const FName Name{TEXTVIEW("CameraOffsetZ")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::FovOffsetCurveName()
{
	static const FName Name{TEXTVIEW("FovOffset")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::PivotOffsetXCurveName()
{
	static const FName Name{TEXTVIEW("PivotOffsetX")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::PivotOffsetYCurveName()
{
	static const FName Name{TEXTVIEW("PivotOffsetY")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::PivotOffsetZCurveName()
{
	static const FName Name{TEXTVIEW("PivotOffsetZ")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::LocationLagXCurveName()
{
	static const FName Name{TEXTVIEW("LocationLagX")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::LocationLagYCurveName()
{
	static const FName Name{TEXTVIEW("LocationLagY")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::LocationLagZCurveName()
{
	static const FName Name{TEXTVIEW("LocationLagZ")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::RotationLagCurveName()
{
	static const FName Name{TEXTVIEW("RotationLag")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::FirstPersonOverrideCurveName()
{
	static const FName Name{TEXTVIEW("FirstPersonOverride")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::TraceOverrideCurveName()
{
	static const FName Name{TEXTVIEW("TraceOverride")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::CameraCurvesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("HODGEALS.CameraCurves")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::CameraShapesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("HODGEALS.CameraShapes")};
	return Name;
}

inline const FName& UHodgeAlsCameraConstants::CameraTracesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("HODGEALS.CameraTraces")};
	return Name;
}
