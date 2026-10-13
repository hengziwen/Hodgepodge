/** @file HodgeLockOnProfile.cpp @brief 校验锁定配置，默认值仅作为可编辑原型基线。 */
#include "Data/HodgeLockOnProfile.h"
#include "Camera/HodgeCameraMode_LockOn.h"
#include "UI/Targeting/HodgeLockOnIndicatorWidget.h"
#include "Misc/DataValidation.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeLockOnProfile)
UHodgeLockOnProfile::UHodgeLockOnProfile()
{ CameraMode = UHodgeCameraMode_LockOn::StaticClass(); IndicatorClass = UHodgeLockOnIndicatorWidget::StaticClass(); }
bool UHodgeLockOnProfile::Validate(FString& Error) const
{
	const float Positive[] = {AcquireDistance,KeepDistance,AcquireHalfAngle,KeepCheckInterval,GestureWindow,SwitchAngle,SwitchSpeed,RearmWindow,CommandTimeout,ReferenceTimeout,CameraYawSpeed,CameraPitchSpeed,CameraAcquireDuration,CameraAcquireSpeed,CameraAssistRecoveryTime,CameraMinTargetDistance};
	for (float Value : Positive) { if (!FMath::IsFinite(Value) || Value <= 0.f) { Error = TEXT("LockOnProfile has non-finite or non-positive thresholds"); return false; } }
	const float Nonnegative[] = {DistanceGrace,OcclusionGrace,SwitchDebounce,RearmAngle,CameraBlendTime,DirectionConsistency,CameraManualHold,CameraYawStrength,CameraPitchStrength,CameraMouseSuppression};
	for (float Value : Nonnegative) { if (!FMath::IsFinite(Value) || Value < 0.f) { Error = TEXT("LockOnProfile has invalid grace or input limits"); return false; } }
	if (KeepDistance < AcquireDistance || AcquireHalfAngle >= 90.f || DirectionConsistency > 1.f || ScoreWeights.ContainsNaN() ||
		ScoreWeights.GetMin() < 0.f || ScoreWeights.X + ScoreWeights.Y + ScoreWeights.Z <= 0.f || PitchLimits.ContainsNaN() ||
		PitchLimits.X < -89.f || PitchLimits.Y > 89.f || PitchLimits.X >= PitchLimits.Y ||
		CameraMouseSuppression > 1.f || CameraScreenDeadZone.ContainsNaN() || CameraScreenSoftZone.ContainsNaN() ||
		CameraScreenDeadZone.GetMin() < 0.f || CameraScreenSoftZone.GetMax() > 1.f ||
		CameraScreenDeadZone.X >= CameraScreenSoftZone.X || CameraScreenDeadZone.Y >= CameraScreenSoftZone.Y ||
		!CameraMode || CameraMode->HasAnyClassFlags(CLASS_Abstract) || !CameraMode->IsChildOf(UHodgeCameraMode_LockOn::StaticClass()) || IndicatorClass.IsNull())
	{ Error = TEXT("LockOnProfile has invalid ranges, weights or presentation classes"); return false; }
	Error.Reset(); return true;
}
#if WITH_EDITOR
EDataValidationResult UHodgeLockOnProfile::IsDataValid(FDataValidationContext& Context) const
{
	FString Error; if (!Validate(Error)) { Context.AddError(FText::FromString(Error)); return EDataValidationResult::Invalid; }
	return EDataValidationResult::Valid;
}
#endif
