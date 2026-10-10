#include "Data/HodgeSprintAbilityProfile.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Misc/DataValidation.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeSprintAbilityProfile)

bool UHodgeSprintAbilityProfile::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	const float Positive[] = {Duration, Distance, MontagePlayRate, FacingPreparationTimeout, HandoffNetworkGrace, MoveIntentThreshold,
		SprintMaxSpeed, SprintAcceleration, SprintBraking, SprintTurnRate, BlockedExitDelay, SprintTurnPlayRate, SprintCyclePlayRate,
		SprintCycleReferenceSpeed, SprintCycleMinPlayRate, SprintCycleMaxPlayRate};
	for (float Value : Positive)
	{ if (!FMath::IsFinite(Value) || Value <= 0.f) { Errors.Add(FText::FromString(TEXT("Movement profile requires finite positive rates, times and movement parameters."))); break; } }
	const float NonNegative[] = {HoldThreshold, NoMoveIntentGrace, InvulnerabilityStart, InvulnerabilityEnd, PerfectStart, PerfectEnd, HandoffOpenTime, HandoffCloseTime, MoveCancelOpenTime, BackwardConeAngle, SprintTurnBlendOutTime};
	for (float Value : NonNegative)
	{ if (!FMath::IsFinite(Value) || Value < 0.f) { Errors.Add(FText::FromString(TEXT("Movement windows and cone angles must be finite and nonnegative."))); break; } }
	if (!(PerfectStart >= InvulnerabilityStart && PerfectStart < PerfectEnd && PerfectEnd <= InvulnerabilityEnd &&
		InvulnerabilityStart < InvulnerabilityEnd && InvulnerabilityEnd <= Duration && Duration <= HandoffOpenTime &&
		HandoffOpenTime < HandoffCloseTime && MoveCancelOpenTime >= Duration && MoveIntentThreshold <= 1.f && BackwardConeAngle < 90.f))
	{ Errors.Add(FText::FromString(TEXT("Invalid defense/handoff windows, hold threshold or backward half cone."))); }
	if (!ForwardMontage || (bEnableBackwardVariant && !BackwardMontage))
	{ Errors.Add(FText::FromString(TEXT("Forward montage and enabled backward montage are required."))); }
	for (const UAnimMontage* Montage : {ForwardMontage.Get(), bEnableBackwardVariant ? BackwardMontage.Get() : nullptr})
	{
		if (!Montage) { continue; }
		const float AnimationDuration = Montage->GetPlayLength() / (MontagePlayRate * Montage->RateScale);
		if (!FMath::IsFinite(AnimationDuration) || Montage->RateScale <= 0.f || HandoffCloseTime >= AnimationDuration ||
			Duration >= AnimationDuration || (bAllowMoveCancel && MoveCancelOpenTime >= AnimationDuration))
		{ Errors.Add(FText::FromString(TEXT("Movement/recovery/handoff windows must fit each montage at its configured playback rate."))); }
		if (Montage->HasRootMotion() || Montage->SlotAnimTracks.Num() != 1 || Montage->SlotAnimTracks[0].SlotName != FName(TEXT("FullBody")))
		{ Errors.Add(FText::FromString(TEXT("RMS Dash requires an in-place montage with one FullBody slot."))); }
	}
	if (ForwardMontage && BackwardMontage && ForwardMontage->GetSkeleton() != BackwardMontage->GetSkeleton())
	{ Errors.Add(FText::FromString(TEXT("Dash variants must share a skeleton."))); }
	if (bAllowSprintPivot && (!SprintTurn || SprintTurnBlendOutTime >= SprintTurn->GetPlayLength() / SprintTurnPlayRate))
	{ Errors.Add(FText::FromString(TEXT("Sprint Pivot needs a Turn sequence with a blend-out shorter than its playback duration."))); }
	if (!FMath::IsFinite(PivotTriggerAngle) || PivotTriggerAngle < 90.f || PivotTriggerAngle > 180.f ||
		!FMath::IsFinite(PivotMinimumSpeed) || PivotMinimumSpeed < 0.f || !FMath::IsFinite(PivotReentryDelay) || PivotReentryDelay < 0.f)
	{ Errors.Add(FText::FromString(TEXT("Invalid Pivot angle, speed or reentry delay."))); }
	if (SprintCycleMinPlayRate > SprintCycleMaxPlayRate)
	{ Errors.Add(FText::FromString(TEXT("Sprint cycle minimum playback rate exceeds maximum."))); }
	if (SprintPivotMontage && (!SprintPivotMontage->HasRootMotion() || SprintPivotMontage->SlotAnimTracks.Num() != 1 ||
		SprintPivotMontage->SlotAnimTracks[0].SlotName != FName(TEXT("FullBody")) ||
		(ForwardMontage && SprintPivotMontage->GetSkeleton() != ForwardMontage->GetSkeleton())))
	{ Errors.Add(FText::FromString(TEXT("Pivot requires a matching skeleton and one root-motion FullBody track."))); }
	if (SprintPivotMontage && (!FMath::IsFinite(PivotRecoveryEndTime) || PivotRecoveryEndTime <= 0.f ||
		PivotRecoveryEndTime >= SprintPivotMontage->GetPlayLength()))
	{ Errors.Add(FText::FromString(TEXT("Pivot recovery end must be a finite time inside its montage."))); }
	return Before == Errors.Num();
}
bool UHodgeSprintAbilityProfile::IsHandoffWindow(double DashAge) const
{ return FMath::IsFinite(DashAge) && DashAge >= HandoffOpenTime && DashAge <= HandoffCloseTime; }
bool UHodgeSprintAbilityProfile::IsAuthorityHandoffWindow(double DashAge) const
{ return FMath::IsFinite(DashAge) && DashAge >= HandoffOpenTime && DashAge <= HandoffCloseTime + HandoffNetworkGrace; }
bool UHodgeSprintAbilityProfile::CanMoveCancel(double DashAge) const
{ return bAllowMoveCancel && FMath::IsFinite(DashAge) && DashAge >= MoveCancelOpenTime; }
#if WITH_EDITOR
EDataValidationResult UHodgeSprintAbilityProfile::IsDataValid(FDataValidationContext& Context) const
{
	TArray<FText> Errors; Validate(Errors);
	for (const FText& Error : Errors) { Context.AddError(Error); }
	return Errors.IsEmpty() ? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}
#endif
