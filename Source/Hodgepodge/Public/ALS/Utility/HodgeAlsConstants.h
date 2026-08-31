#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HodgeAlsConstants.generated.h"

UCLASS(Meta = (BlueprintThreadSafe))
class HODGEPODGE_API UHodgeAlsConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Bones

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& RootBoneName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& PelvisBoneName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& HeadBoneName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& Spine03BoneName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootLeftBoneName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootRightBoneName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& HandLeftGunVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& HandRightGunVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootLeftIkBoneName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootRightIkBoneName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootLeftVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootRightVirtualBoneName();

	// Animation Slots

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static const FName& TransitionSlotName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static const FName& TurnInPlaceStandingSlotName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static const FName& TurnInPlaceCrouchingSlotName();

	// Layering Animation Curves

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHeadCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHeadAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHeadSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftLocalSpaceCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightLocalSpaceCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHandLeftCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHandRightCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerSpineCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerSpineAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerSpineSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerPelvisCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerPelvisSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerLegsCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerLegsSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& HandLeftIkCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& HandRightIkCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& ViewBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& AllowAimingCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& HipsDirectionLockCurveName();

	// Pose Animation Curves

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseGaitCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseMovingCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseStandingCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseCrouchingCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseGroundedCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseInAirCurveName();

	// Feet Animation Curves

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootLeftIkCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootLeftLockCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootRightIkCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootRightLockCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootPlantedCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FeetCrossingCurveName();

	// Other Animation Curves

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& RotationYawSpeedCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& RotationYawOffsetCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& AllowTransitionsCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& SprintBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& GroundPredictionBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootstepSoundBlockCurveName();

	// Debug

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& CurvesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& StateDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& ShapesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& TracesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& MantlingDebugDisplayName();
};

inline const FName& UHodgeAlsConstants::RootBoneName()
{
	static const FName Name{TEXTVIEW("root")};
	return Name;
}

inline const FName& UHodgeAlsConstants::PelvisBoneName()
{
	static const FName Name{TEXTVIEW("pelvis")};
	return Name;
}

inline const FName& UHodgeAlsConstants::HeadBoneName()
{
	static const FName Name{TEXTVIEW("head")};
	return Name;
}

inline const FName& UHodgeAlsConstants::Spine03BoneName()
{
	static const FName Name{TEXTVIEW("spine_03")};
	return Name;
}

inline const FName& UHodgeAlsConstants::FootLeftBoneName()
{
	static const FName Name{TEXTVIEW("foot_l")};
	return Name;
}

inline const FName& UHodgeAlsConstants::FootRightBoneName()
{
	static const FName Name{TEXTVIEW("foot_r")};
	return Name;
}

inline const FName& UHodgeAlsConstants::HandLeftGunVirtualBoneName()
{
	static const FName Name{TEXTVIEW("VB hand_l_to_ik_hand_gun")};
	return Name;
}

inline const FName& UHodgeAlsConstants::HandRightGunVirtualBoneName()
{
	static const FName Name{TEXTVIEW("VB hand_r_to_ik_hand_gun")};
	return Name;
}

inline const FName& UHodgeAlsConstants::FootLeftIkBoneName()
{
	static const FName Name{TEXTVIEW("ik_foot_l")};
	return Name;
}

inline const FName& UHodgeAlsConstants::FootRightIkBoneName()
{
	static const FName Name{TEXTVIEW("ik_foot_r")};
	return Name;
}

inline const FName& UHodgeAlsConstants::FootLeftVirtualBoneName()
{
	static const FName Name{TEXTVIEW("VB foot_l")};
	return Name;
}

inline const FName& UHodgeAlsConstants::FootRightVirtualBoneName()
{
	static const FName Name{TEXTVIEW("VB foot_r")};
	return Name;
}

inline const FName& UHodgeAlsConstants::TransitionSlotName()
{
	static const FName Name{TEXTVIEW("Transition")};
	return Name;
}

inline const FName& UHodgeAlsConstants::TurnInPlaceStandingSlotName()
{
	static const FName Name{TEXTVIEW("TurnInPlaceStanding")};
	return Name;
}

inline const FName& UHodgeAlsConstants::TurnInPlaceCrouchingSlotName()
{
	static const FName Name{TEXTVIEW("TurnInPlaceCrouching")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerHeadCurveName()
{
	static const FName Name{TEXTVIEW("LayerHead")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerHeadAdditiveCurveName()
{
	static const FName Name{TEXTVIEW("LayerHeadAdditive")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerHeadSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerHeadSlot")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerArmLeftCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmLeft")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerArmLeftAdditiveCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmLeftAdditive")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerArmLeftLocalSpaceCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmLeftLocalSpace")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerArmLeftSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmLeftSlot")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerArmRightCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmRight")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerArmRightAdditiveCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmRightAdditive")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerArmRightLocalSpaceCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmRightLocalSpace")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerArmRightSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmRightSlot")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerHandLeftCurveName()
{
	static const FName Name{TEXTVIEW("LayerHandLeft")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerHandRightCurveName()
{
	static const FName Name{TEXTVIEW("LayerHandRight")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerSpineCurveName()
{
	static const FName Name{TEXTVIEW("LayerSpine")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerSpineAdditiveCurveName()
{
	static const FName Name{TEXTVIEW("LayerSpineAdditive")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerSpineSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerSpineSlot")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerPelvisCurveName()
{
	static const FName Name{TEXTVIEW("LayerPelvis")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerPelvisSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerPelvisSlot")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerLegsCurveName()
{
	static const FName Name{TEXTVIEW("LayerLegs")};
	return Name;
}

inline const FName& UHodgeAlsConstants::LayerLegsSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerLegsSlot")};
	return Name;
}

inline const FName& UHodgeAlsConstants::HandLeftIkCurveName()
{
	static const FName Name{TEXTVIEW("HandLeftIk")};
	return Name;
}

inline const FName& UHodgeAlsConstants::HandRightIkCurveName()
{
	static const FName Name{TEXTVIEW("HandRightIk")};
	return Name;
}

inline const FName& UHodgeAlsConstants::ViewBlockCurveName()
{
	static const FName Name{TEXTVIEW("ViewBlock")};
	return Name;
}

inline const FName& UHodgeAlsConstants::AllowAimingCurveName()
{
	static const FName Name{TEXTVIEW("AllowAiming")};
	return Name;
}

inline const FName& UHodgeAlsConstants::HipsDirectionLockCurveName()
{
	static const FName Name{TEXTVIEW("HipsDirectionLock")};
	return Name;
}

inline const FName& UHodgeAlsConstants::PoseGaitCurveName()
{
	static const FName Name{TEXTVIEW("PoseGait")};
	return Name;
}

inline const FName& UHodgeAlsConstants::PoseMovingCurveName()
{
	static const FName Name{TEXTVIEW("PoseMoving")};
	return Name;
}

inline const FName& UHodgeAlsConstants::PoseStandingCurveName()
{
	static const FName Name{TEXTVIEW("PoseStanding")};
	return Name;
}

inline const FName& UHodgeAlsConstants::PoseCrouchingCurveName()
{
	static const FName Name{TEXTVIEW("PoseCrouching")};
	return Name;
}

inline const FName& UHodgeAlsConstants::PoseGroundedCurveName()
{
	static const FName Name{TEXTVIEW("PoseGrounded")};
	return Name;
}

inline const FName& UHodgeAlsConstants::PoseInAirCurveName()
{
	static const FName Name{TEXTVIEW("PoseInAir")};
	return Name;
}

inline const FName& UHodgeAlsConstants::FootLeftIkCurveName()
{
	static const FName Name{TEXTVIEW("FootLeftIk")};
	return Name;
}

inline const FName& UHodgeAlsConstants::FootLeftLockCurveName()
{
	static const FName Name{TEXTVIEW("FootLeftLock")};
	return Name;
}

inline const FName& UHodgeAlsConstants::FootRightIkCurveName()
{
	static const FName Name{TEXTVIEW("FootRightIk")};
	return Name;
}

inline const FName& UHodgeAlsConstants::FootRightLockCurveName()
{
	static const FName Name{TEXTVIEW("FootRightLock")};
	return Name;
}

inline const FName& UHodgeAlsConstants::FootPlantedCurveName()
{
	static const FName Name{TEXTVIEW("FootPlanted")};
	return Name;
}

inline const FName& UHodgeAlsConstants::FeetCrossingCurveName()
{
	static const FName Name{TEXTVIEW("FeetCrossing")};
	return Name;
}

inline const FName& UHodgeAlsConstants::RotationYawSpeedCurveName()
{
	static const FName Name{TEXTVIEW("RotationYawSpeed")};
	return Name;
}

inline const FName& UHodgeAlsConstants::RotationYawOffsetCurveName()
{
	static const FName Name{TEXTVIEW("RotationYawOffset")};
	return Name;
}

inline const FName& UHodgeAlsConstants::AllowTransitionsCurveName()
{
	static const FName Name{TEXTVIEW("AllowTransitions")};
	return Name;
}

inline const FName& UHodgeAlsConstants::SprintBlockCurveName()
{
	static const FName Name{TEXTVIEW("SprintBlock")};
	return Name;
}

inline const FName& UHodgeAlsConstants::GroundPredictionBlockCurveName()
{
	static const FName Name{TEXTVIEW("GroundPredictionBlock")};
	return Name;
}

inline const FName& UHodgeAlsConstants::FootstepSoundBlockCurveName()
{
	static const FName Name{TEXTVIEW("FootstepSoundBlock")};
	return Name;
}

inline const FName& UHodgeAlsConstants::CurvesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("HODGEALS.Curves")};
	return Name;
}

inline const FName& UHodgeAlsConstants::StateDebugDisplayName()
{
	static const FName Name{TEXTVIEW("HODGEALS.State")};
	return Name;
}

inline const FName& UHodgeAlsConstants::ShapesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("HODGEALS.Shapes")};
	return Name;
}

inline const FName& UHodgeAlsConstants::TracesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("HODGEALS.Traces")};
	return Name;
}

inline const FName& UHodgeAlsConstants::MantlingDebugDisplayName()
{
	static const FName Name{TEXTVIEW("HODGEALS.Mantling")};
	return Name;
}
