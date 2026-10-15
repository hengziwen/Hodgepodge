/** @file HodgeCinematicCameraProfile.cpp @brief 实现连续分镜校验、平滑采样和源时间混合权重。 */
#include "Data/HodgeCinematicCameraProfile.h"
#include "Curves/CurveVector.h"
#include "Curves/CurveFloat.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Sound/SoundBase.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCinematicCameraProfile)

namespace
{
	/** 三次缓入缓出函数的端点速度为零。 */
	float Smooth(float Value) { const float T = FMath::Clamp(Value, 0.f, 1.f); return T*T*(3.f-2.f*T); }
	/** 求一段的位置和FOV，曲线覆盖默认端点插值。 */
	bool SampleShot(const FHodgeCinematicCameraShot& Shot, float T, FHodgeCinematicCameraSample& Out)
	{
		Out.CameraOffset = Shot.PositionCurve ? Shot.PositionCurve->GetVectorValue(T) : FMath::Lerp(Shot.StartOffset, Shot.EndOffset, Smooth(T));
		Out.LookAtOffset = Shot.LookAtOffset;
		Out.FieldOfView = Shot.FOVCurve ? Shot.FOVCurve->GetFloatValue(T) : FMath::Lerp(float(Shot.FOV.X), float(Shot.FOV.Y), Smooth(T));
		return !Out.CameraOffset.ContainsNaN() && !Out.LookAtOffset.ContainsNaN() && FMath::IsFinite(Out.FieldOfView) && Out.FieldOfView >= 5.f && Out.FieldOfView <= 170.f;
	}
}

bool UHodgeCinematicCameraProfile::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	if (!FMath::IsFinite(PostProcessWeight) || PostProcessWeight<0.f || PostProcessWeight>1.f || LocalEvents.Num()>16)
	{ Errors.Add(FText::FromString(TEXT("Invalid cinematic PP weight or too many local events"))); }
	TSet<FName> EventIds;
	for (const auto& Event : LocalEvents)
	{
		if (Event.EventId.IsNone() || EventIds.Contains(Event.EventId) || !FMath::IsFinite(Event.Time) || Event.Time<0.f || Event.Time>1.f ||
			!FMath::IsFinite(Event.ShakeScale) || Event.ShakeScale<0.f || (Event.Sound && Event.Sound->IsLooping()) ||
			(Event.CameraShake && Event.CameraShake->GetDefaultObject<UCameraShakeBase>()->bSingleInstance))
		{ Errors.Add(FText::FromString(TEXT("Invalid cinematic event, looping audio or shared shake instance"))); }
		EventIds.Add(Event.EventId);
	}
	if (Shots.IsEmpty()) { Errors.Add(FText::FromString(TEXT("Cinematic profile requires shots"))); }
	if (BlendFractions.ContainsNaN() || BlendFractions.X <= 0. || BlendFractions.Y <= 0. || BlendFractions.X + BlendFractions.Y > 1. ||
		!FMath::IsFinite(CancelBlendSeconds) || CancelBlendSeconds < 0.f || !FMath::IsFinite(ProbeRadius) || ProbeRadius <= 0.f ||
		!FMath::IsFinite(MinLookAtDistance) || MinLookAtDistance <= ProbeRadius || PitchLimits.ContainsNaN() || PitchLimits.X < -89. || PitchLimits.Y > 89. || PitchLimits.X >= PitchLimits.Y)
	{ Errors.Add(FText::FromString(TEXT("Invalid cinematic blend or safety parameters"))); }
	float Previous = 0.f;
	TSet<FName> Names;
	for (const auto& Shot : Shots)
	{
		if (Shot.ShotId.IsNone() || Names.Contains(Shot.ShotId) || !FMath::IsFinite(Shot.Start) || !FMath::IsFinite(Shot.End) ||
			!FMath::IsNearlyEqual(Shot.Start, Previous) || Shot.End <= Shot.Start || Shot.End > 1.f || !FMath::IsFinite(Shot.BlendFraction) || Shot.BlendFraction < 0.f || Shot.BlendFraction > 1.f)
		{ Errors.Add(FText::FromString(TEXT("Cinematic shots need unique IDs and contiguous ordered ranges covering 0..1"))); }
		// 配置验证同时检查未被曲线使用的端点，防止后续移除曲线暴露非法值。
		if (Shot.StartOffset.ContainsNaN() || Shot.EndOffset.ContainsNaN() || Shot.LookAtOffset.ContainsNaN() || Shot.FOV.ContainsNaN() || Shot.FOV.X < 5. || Shot.FOV.X > 170. || Shot.FOV.Y < 5. || Shot.FOV.Y > 170.)
		{ Errors.Add(FText::FromString(TEXT("Cinematic shot has invalid offsets or FOV"))); }
		for (int32 Index=0; Index<=16; ++Index)
		{
			FHodgeCinematicCameraSample Sampled;
			if (!SampleShot(Shot, Index/16.f, Sampled)) { Errors.Add(FText::FromString(TEXT("Cinematic curve sample is invalid"))); break; }
		}
		Names.Add(Shot.ShotId); Previous = Shot.End;
	}
	if (!Shots.IsEmpty() && !FMath::IsNearlyEqual(Previous, 1.f)) { Errors.Add(FText::FromString(TEXT("Cinematic last shot must end at 1"))); }
	return Errors.Num() == Before;
}

bool UHodgeCinematicCameraProfile::ValidateMesh(const USkeletalMeshComponent* Mesh, TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	const bool bHasFallback = Mesh && !FallbackAnchor.IsNone() && Mesh->DoesSocketExist(FallbackAnchor);
	for (const auto& Shot : Shots)
	{
		for (FName Bone : {Shot.Anchor, Shot.LookAt})
		{
			if (!Bone.IsNone() && (!Mesh || !Mesh->DoesSocketExist(Bone)) && !bHasFallback)
			{ Errors.Add(FText::FromString(FString::Printf(TEXT("Cinematic anchor [%s] is missing with no valid fallback"), *Bone.ToString()))); }
		}
	}
	return Errors.Num() == Before;
}

bool UHodgeCinematicCameraProfile::Sample(float NormalizedTime, FHodgeCinematicCameraSample& Out) const
{
	Out = FHodgeCinematicCameraSample();
	if (!FMath::IsFinite(NormalizedTime) || NormalizedTime < 0.f || NormalizedTime > 1.f) { return false; }
	for (int32 Index=0; Index<Shots.Num(); ++Index)
	{
		const auto& Shot = Shots[Index];
		if (NormalizedTime < Shot.Start || (NormalizedTime >= Shot.End && !(Index==Shots.Num()-1 && NormalizedTime==1.f))) { continue; }
		if (Shot.End <= Shot.Start || !SampleShot(Shot, (NormalizedTime-Shot.Start)/(Shot.End-Shot.Start), Out)) { return false; }
		Out.ShotIndex = Index;
		return true;
	}
	return false;
}

float UHodgeCinematicCameraProfile::EvaluateWeight(float Position, float WindowStart, float WindowEnd) const
{
	if (!FMath::IsFinite(Position) || !FMath::IsFinite(WindowStart) || !FMath::IsFinite(WindowEnd) || WindowEnd <= WindowStart ||
		Position <= WindowStart || Position >= WindowEnd || BlendFractions.ContainsNaN() || BlendFractions.X <= 0. || BlendFractions.Y <= 0.) { return 0.f; }
	const float T = (Position-WindowStart)/(WindowEnd-WindowStart);
	return FMath::Min(Smooth(T/BlendFractions.X), Smooth((1.f-T)/BlendFractions.Y));
}

#if WITH_EDITOR
EDataValidationResult UHodgeCinematicCameraProfile::IsDataValid(FDataValidationContext& Context) const
{
	TArray<FText> Errors; Validate(Errors);
	for (const FText& Error : Errors) { Context.AddError(Error); }
	return Errors.IsEmpty() ? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}
#endif
