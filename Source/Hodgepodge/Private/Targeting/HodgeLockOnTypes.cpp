/** @file HodgeLockOnTypes.cpp @brief TargetData 序列化与设备无关的方向手势。 */
#include "Targeting/HodgeLockOnTypes.h"
#include "Data/HodgeLockOnProfile.h"
#include "Engine/PackageMapClient.h"
#include "GameFramework/Actor.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeLockOnTypes)
namespace HodgeLockOnTags
{
	UE_DEFINE_GAMEPLAY_TAG(Input,"InputTag.Ability.LockOn");
	UE_DEFINE_GAMEPLAY_TAG(Ability,"Ability.Type.Targeting.LockOn");
	UE_DEFINE_GAMEPLAY_TAG(Locked,"State.Targeting.LockedOn");
}
bool FHodgeLockOnTargetData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!Map) { bOutSuccess = false; return false; }
	UObject* Source = SourceAvatar.Get(); UObject* Target = CandidateActor.Get();
	const bool bSourceMapped = Map->SerializeObject(Ar, AActor::StaticClass(), Source);
	const bool bTargetMapped = Map->SerializeObject(Ar, AActor::StaticClass(), Target);
	if (Ar.IsLoading()) { SourceAvatar = Cast<AActor>(Source); CandidateActor = Cast<AActor>(Target); }
	uint8 Op = static_cast<uint8>(Operation);
	Ar << Op << RequestSequence << BaseStateVersion << LockPointId;
	if (Ar.IsLoading()) { Operation = static_cast<EHodgeLockOnOperation>(Op); }
	bOutSuccess = !Ar.IsError();
	return bSourceMapped && bTargetMapped;
}
void FHodgeLockOnGesture::Reset() { Samples.Reset(); LastTrigger = -DBL_MAX; bArmed = true; }
int32 FHodgeLockOnGesture::Sample(FVector2D Degrees, double Now, const UHodgeLockOnProfile& Profile)
{
	if (Degrees.ContainsNaN() || !FMath::IsFinite(Now)) { Reset(); return 0; }
	Samples.RemoveAll([&](const FSample& S) { return S.Time < Now - FMath::Max(Profile.GestureWindow, Profile.RearmWindow); });
	Samples.Add({Now,Degrees.X});
	double Sum = 0., Absolute = 0., RearmAbsolute = 0.;
	for (const auto& S : Samples)
	{
		if (S.Time >= Now - Profile.GestureWindow) { Sum += S.X; Absolute += FMath::Abs(S.X); }
		if (S.Time >= Now - Profile.RearmWindow) { RearmAbsolute += FMath::Abs(S.X); }
	}
	if (!bArmed && Now - LastTrigger >= Profile.SwitchDebounce && RearmAbsolute <= Profile.RearmAngle)
	{ bArmed = true; Samples.Reset(); return 0; }
	if (bArmed && Now - LastTrigger >= Profile.SwitchDebounce && Absolute > SMALL_NUMBER &&
		FMath::Abs(Sum) >= Profile.SwitchAngle && FMath::Abs(Sum) / Profile.GestureWindow >= Profile.SwitchSpeed &&
		FMath::Abs(Sum) / Absolute >= Profile.DirectionConsistency)
	{ bArmed = false; LastTrigger = Now; Samples.Reset(); return Sum > 0. ? 1 : -1; }
	return 0;
}
void FHodgeSoftLockCamera::Seed(FRotator CurrentView, double Now)
{
	if (CurrentView.ContainsNaN() || !FMath::IsFinite(Now)) { return; }
	// 被技能镜头完全覆盖期间的旧输入已体现在实际视角，返回时不能再应用一次。
	if (LastManualAt != Now) { PendingLook = FVector2D::ZeroVector; }
	Rotation = CurrentView.GetNormalized(); Rotation.Roll = 0.f; LastManualAt = Now;
	// 重新晋升从实际画面进入跟随；新目标由 BeginAcquire 单独开启获取。
	AcquireElapsed = 0.f; bAcquiring = false;
}
void FHodgeSoftLockCamera::AddLook(FVector2D Degrees, double Now)
{
	if (Degrees.ContainsNaN() || !FMath::IsFinite(Now) || Degrees.IsNearlyZero()) { return; }
	if (LastManualAt != Now) { PendingLook = FVector2D::ZeroVector; }
	PendingLook += Degrees; LastManualAt = Now;
}
void FHodgeSoftLockCamera::BeginAcquire()
{
	AcquireElapsed = 0.f; bAcquiring = PendingLook.IsNearlyZero();
	// 模式 Seed 不属于手动输入，不能让首次转向额外等待停手宽限。
	if (bAcquiring) { LastManualAt = -DBL_MAX; }
}
FRotator FHodgeSoftLockCamera::Update(const FHodgeLockCameraFrame& Frame, float DeltaSeconds, double Now, const UHodgeLockOnProfile& Profile, bool bAllowAssist)
{
	const bool bManual = !PendingLook.IsNearlyZero();
	// 玩家接管获取时，以实际混合画面为基准，丢弃尚未显示的辅助转向。
	if (bManual && bAcquiring && !Frame.ViewRotation.ContainsNaN()) { Rotation = Frame.ViewRotation.GetNormalized(); }
	Rotation.Yaw = FRotator::NormalizeAxis(Rotation.Yaw + PendingLook.X);
	Rotation.Pitch = FMath::Clamp(Rotation.Pitch + PendingLook.Y, Profile.PitchLimits.X, Profile.PitchLimits.Y);
	PendingLook = FVector2D::ZeroVector;
	if (bManual) { bAcquiring = false; }
	if (!bAllowAssist || !Profile.bEnableSoftCameraAssist || !FMath::IsFinite(Now) || !FMath::IsFinite(DeltaSeconds) || DeltaSeconds <= 0.f ||
		Frame.TargetLocation.ContainsNaN() || Frame.CameraLocation.ContainsNaN() || Frame.PivotLocation.ContainsNaN() || Frame.ViewRotation.ContainsNaN() ||
		!FMath::IsFinite(Frame.HorizontalFOV) || Frame.HorizontalFOV <= 1.f || Frame.HorizontalFOV >= 179.f || !FMath::IsFinite(Frame.AspectRatio) || Frame.AspectRatio <= 0.f) { return Rotation; }
	const float Distance = FVector::Dist2D(Frame.TargetLocation, Frame.PivotLocation);
	if (Distance <= Profile.CameraMinTargetDistance) { bAcquiring = false; return Rotation; }
	// 用实际避障后的局部偏移预测转向后的镜头位置，避免从角色脚底或中点瞄准。
	const FVector LocalOffset = Frame.ViewRotation.UnrotateVector(Frame.CameraLocation - Frame.PivotLocation);
	const FVector Origin = Frame.PivotLocation + Rotation.RotateVector(LocalOffset);
	const FVector Direction = Frame.TargetLocation - Origin;
	if (Direction.SizeSquared() < 1.f) { return Rotation; }
	const FVector LocalDirection = Rotation.UnrotateVector(Direction);
	const FRotator AimDirection = Direction.Rotation();
	const FRotator Error(FMath::FindDeltaAngleDegrees(Rotation.Pitch,AimDirection.Pitch), FMath::FindDeltaAngleDegrees(Rotation.Yaw,AimDirection.Yaw), 0.f);
	const float Dt = FMath::Min(DeltaSeconds, .05f);
	const float NearWeight = FMath::Clamp((Distance-Profile.CameraMinTargetDistance)/Profile.CameraMinTargetDistance, 0.f, 1.f);
	if (bAcquiring)
	{
		const float Before = FMath::Clamp(AcquireElapsed/Profile.CameraAcquireDuration, 0.f, 1.f);
		AcquireElapsed += Dt;
		const float After = FMath::Clamp(AcquireElapsed/Profile.CameraAcquireDuration, 0.f, 1.f);
		const auto Ease = [](float T) { return T*T*T*(T*(T*6.f-15.f)+10.f); };
		const float Fraction = FMath::Clamp((Ease(After)-Ease(Before))/FMath::Max(1.f-Ease(Before), SMALL_NUMBER), 0.f, 1.f);
		// 目标在获取期间仍可移动；重新求解轨道方向，并限制单帧旋转速度。
		FRotator Aim = Rotation;
		for (int32 I=0; I<4; ++I)
		{
			const FVector Ray = Frame.TargetLocation-(Frame.PivotLocation+Aim.RotateVector(LocalOffset));
			if (Ray.SizeSquared() < 1.f) { break; }
			Aim = Ray.Rotation(); Aim.Pitch = FMath::Clamp(Aim.Pitch, Profile.PitchLimits.X, Profile.PitchLimits.Y);
		}
		const float Limit = Profile.CameraAcquireSpeed*Dt*NearWeight;
		Rotation.Yaw = FRotator::NormalizeAxis(Rotation.Yaw+FMath::Clamp(FMath::FindDeltaAngleDegrees(Rotation.Yaw,Aim.Yaw)*Fraction,-Limit,Limit));
		Rotation.Pitch += FMath::Clamp((Aim.Pitch-Rotation.Pitch)*Fraction,-Limit,Limit);
		if (After >= 1.f) { bAcquiring = false; }
	}
	else
	{
		const float TanYaw = FMath::Tan(FMath::DegreesToRadians(Frame.HorizontalFOV*.5f));
		const float TanPitch = TanYaw/Frame.AspectRatio;
		// 使用相机局部坐标投影；后方目标使用饱和偏差，防止透视除数接近零。
		const auto ScreenError = [](float Side, float Depth, float TanHalfFOV)
		{ return Depth<=1.f ? FMath::Sign(Side)*2.f : Side/(Depth*TanHalfFOV); };
		const FVector2D Screen(LocalDirection.X<=1.f ? FMath::Sign(Error.Yaw)*2.f : ScreenError(LocalDirection.Y,LocalDirection.X,TanYaw),
			ScreenError(LocalDirection.Z,LocalDirection.X,TanPitch));
		const float Recovery = FMath::Clamp(static_cast<float>((Now-LastManualAt-Profile.CameraManualHold)/Profile.CameraAssistRecoveryTime), 0.f, 1.f);
		const float SmoothRecovery = Recovery*Recovery*(3.f-2.f*Recovery);
		const float InputWeight = 1.f-Profile.CameraMouseSuppression*(1.f-SmoothRecovery);
		const auto Step = [Dt, InputWeight, NearWeight](float Angle, float ScreenDeviation, float TanHalfFOV, float Dead, float Soft, float Strength, float Speed)
		{
			const float Screen = FMath::Abs(ScreenDeviation);
			if (Screen <= Dead) { return 0.f; }
			const float Zone = FMath::Clamp((Screen-Dead)/(Soft-Dead), 0.f, 1.f);
			const float Weight = Zone*Zone*(3.f-2.f*Zone);
			const float DeadAngle = FMath::RadiansToDegrees(FMath::Atan(Dead*TanHalfFOV));
			const float Excess = FMath::Sign(Angle)*FMath::Max(FMath::Abs(Angle)-DeadAngle, 0.f);
			const float Alpha = 1.f-FMath::Exp(-Strength*(.25f+.75f*Weight)*InputWeight*NearWeight*Dt);
			return FMath::Clamp(Excess*Alpha,-Speed*Dt,Speed*Dt);
		};
		Rotation.Yaw = FRotator::NormalizeAxis(Rotation.Yaw+Step(Error.Yaw,Screen.X,TanYaw,Profile.CameraScreenDeadZone.X,Profile.CameraScreenSoftZone.X,Profile.CameraYawStrength,Profile.CameraYawSpeed));
		Rotation.Pitch += Step(Error.Pitch,Screen.Y,TanPitch,Profile.CameraScreenDeadZone.Y,Profile.CameraScreenSoftZone.Y,Profile.CameraPitchStrength,Profile.CameraPitchSpeed);
	}
	Rotation.Pitch = FMath::Clamp(Rotation.Pitch,Profile.PitchLimits.X,Profile.PitchLimits.Y); Rotation.Roll = 0.f;
	return Rotation;
}
void FHodgeSoftLockCamera::Reset()
{ Rotation = FRotator::ZeroRotator; PendingLook = FVector2D::ZeroVector; LastManualAt = -DBL_MAX; AcquireElapsed = 0.f; bAcquiring = false; }
