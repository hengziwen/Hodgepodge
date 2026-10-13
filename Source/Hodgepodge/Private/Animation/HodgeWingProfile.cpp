/** @file HodgeWingProfile.cpp
 * @brief 实现翅膀姿势配置相关的类型、接口与现有处理流程。
 */
#include "Animation/HodgeWingProfile.h"
#include "Animation/HodgeWingAnimInstance.h"
#include "Animation/HodgeAnimNotifyState_WingAction.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Engine/SkeletalMesh.h"
#include "Misc/DataValidation.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeWingProfile)

void FHodgeWingFadeState::Show()
{
	Reset();
	Amount = 1.f;
}

void FHodgeWingFadeState::BeginFade(float Seconds)
{
	if (bFading || Amount <= 0.f) { return; }
	if (!FMath::IsFinite(Seconds) || Seconds <= 0.f) { Reset(); return; }
	StartAmount = Amount;
	Elapsed = 0.f;
	Duration = Seconds;
	bFading = true;
}

void FHodgeWingFadeState::Advance(float DeltaSeconds)
{
	if (!bFading || !FMath::IsFinite(DeltaSeconds) || DeltaSeconds <= 0.f) { return; }
	Elapsed = FMath::Min(Elapsed + DeltaSeconds, Duration);
	const float Alpha = FMath::Clamp(Elapsed / Duration, 0.f, 1.f);
	// 平滑端点斜率，避免淡出开始和结束时出现突变。
	Amount = StartAmount * (1.f - Alpha * Alpha * (3.f - 2.f * Alpha));
	if (Elapsed >= Duration) { Reset(); }
}

void FHodgeWingFadeState::Reset()
{
	*this = FHodgeWingFadeState();
}

FHodgeWingActionHandle FHodgeWingPresentationState::Begin(int32 Binding, int32 Instance, FGuid Execution)
{
	if (Binding < 0 || Instance < 0 || !Execution.IsValid()) { return {}; }
	if (Handle.IsValid() && BindingIndex == Binding && SourceInstanceId == Instance && ExecutionId == Execution)
	{
		return Handle;
	}
	Handle.Id = FGuid::NewGuid();
	Handle.Generation = Generation;
	BindingIndex = Binding;
	SourceInstanceId = Instance;
	ExecutionId = Execution;
	bVisible = false;
	return Handle;
}

bool FHodgeWingPresentationState::End(FHodgeWingActionHandle EndingHandle)
{
	if (!EndingHandle.IsValid() || EndingHandle.Generation != Generation || EndingHandle.Id != Handle.Id) { return false; }
	Handle = {};
	BindingIndex = SourceInstanceId = INDEX_NONE;
	ExecutionId.Invalidate();
	bVisible = false;
	SourceTime = WingTime = 0.f;
	return true;
}

void FHodgeWingPresentationState::Reset()
{
	const int32 NextGeneration = Generation < MAX_int32 ? Generation + 1 : 1;
	const int32 Evaluations = PoseEvaluations;
	*this = FHodgeWingPresentationState();
	Generation = NextGeneration;
	PoseEvaluations = Evaluations;
}

float FHodgeWingAnimationBinding::EvaluateWingTime(float SourceTime) const
{
	if (!WingAnimation || !FMath::IsFinite(SourceTime) || !FMath::IsFinite(PlayRateRatio) || PlayRateRatio <= 0.f) { return 0.f; }
	const double Position = double(WingStartTime) + (double(SourceTime) - SourceStartTime) * PlayRateRatio;
	return FMath::IsFinite(Position) ? float(FMath::Clamp(Position, 0., double(WingAnimation->GetPlayLength()))) : 0.f;
}

bool FHodgeWingAnimationBinding::IsWindowOpen(float SourceTime) const
{
	if (!SourceAnimation || !FMath::IsFinite(SourceTime)) { return false; }
	for (const FAnimNotifyEvent& Event : SourceAnimation->Notifies)
	{
		const auto* Window = Cast<UHodgeAnimNotifyState_WingAction>(Event.NotifyStateClass);
		if (Window && Window->WindowId == WindowId && SourceTime >= Event.GetTime() && SourceTime < Event.GetTime() + Event.GetDuration())
		{
			return true;
		}
	}
	return false;
}

bool FHodgeWingAnimationBinding::Validate(const USkeletalMesh* Mesh, TArray<FText>& Errors) const
{
	if (!SourceAnimation || (!SourceAnimation->IsA<UAnimSequence>() && !SourceAnimation->IsA<UAnimMontage>()) || !WingAnimation)
	{
		Errors.Add(FText::FromString(TEXT("Wing binding requires a source Sequence/Montage and a wing Sequence")));
	}
	if (Mesh && WingAnimation && Mesh->GetSkeleton() != WingAnimation->GetSkeleton())
	{
		Errors.Add(FText::FromString(TEXT("Wing animation skeleton does not match the wing mesh")));
	}
	if (!FMath::IsFinite(SourceStartTime) || !FMath::IsFinite(WingStartTime) || !FMath::IsFinite(PlayRateRatio) ||
		SourceStartTime < 0.f || WingStartTime < 0.f || PlayRateRatio <= 0.f ||
		(SourceAnimation && SourceStartTime >= SourceAnimation->GetPlayLength()) ||
		(WingAnimation && WingStartTime >= WingAnimation->GetPlayLength()))
	{
		Errors.Add(FText::FromString(TEXT("Wing binding times and positive rate must be finite and inside the source assets")));
	}
	bool bHasWindow = false;
	if (SourceAnimation)
	{
		for (const FAnimNotifyEvent& Event : SourceAnimation->Notifies)
		{
			const auto* Window = Cast<UHodgeAnimNotifyState_WingAction>(Event.NotifyStateClass);
			if (!Window || Window->WindowId != WindowId) { continue; }
			bHasWindow = true;
			const float Begin = Event.GetTime();
			const float End = Begin + Event.GetDuration();
			if (!FMath::IsFinite(Begin) || !FMath::IsFinite(End) || Begin < 0.f || End <= Begin || End > SourceAnimation->GetPlayLength() + KINDA_SMALL_NUMBER)
			{
				Errors.Add(FText::FromString(TEXT("Wing notification window must be ordered and inside its source animation")));
			}
		}
	}
	if (WindowId.IsNone() || !bHasWindow) { Errors.Add(FText::FromString(TEXT("Wing binding has no matching WingAction notification window"))); }
	return Errors.IsEmpty();
}

/** 根据给定权重混合两份状态或表现数据。 */
FHodgeWingPose FHodgeWingPose::Blend(const FHodgeWingPose& A, const FHodgeWingPose& B, float Alpha)
{
	const float Weight = FMath::IsFinite(Alpha) ? FMath::Clamp(Alpha, 0.f, 1.f) : 0.f;
	FHodgeWingPose Result;
	Result.Root = FQuat::Slerp(A.Root.Quaternion(), B.Root.Quaternion(), Weight).Rotator();
	Result.Mid = FQuat::Slerp(A.Mid.Quaternion(), B.Mid.Quaternion(), Weight).Rotator();
	Result.Tip = FQuat::Slerp(A.Tip.Quaternion(), B.Tip.Quaternion(), Weight).Rotator();
	return Result;
}

/** 判断是否存在有效Timing。 */
bool UHodgeWingProfile::HasValidTiming() const
{
	return FMath::IsFinite(WindupStart) && FMath::IsFinite(StrikeStart) && FMath::IsFinite(StrikePeak) &&
		FMath::IsFinite(RecoveryEnd) && FMath::IsFinite(CancelReturnSeconds) && WindupStart >= 0.f &&
		WindupStart < StrikeStart && StrikeStart < StrikePeak && StrikePeak < RecoveryEnd && CancelReturnSeconds > 0.f;
}

/** 计算当前配置结果。 */
FHodgeWingPose UHodgeWingProfile::Evaluate(float Position) const
{
	if (!HasValidTiming() || !FMath::IsFinite(Position) || Position <= WindupStart || Position >= RecoveryEnd) { return {}; }
	const auto Ease = [](float T) { return T * T * (3.f - 2.f * T); };
	if (Position < StrikeStart)
	{
		return FHodgeWingPose::Blend({}, WindupPose, Ease((Position - WindupStart) / (StrikeStart - WindupStart)));
	}
	if (Position < StrikePeak)
	{
		return FHodgeWingPose::Blend(WindupPose, StrikePose, Ease((Position - StrikeStart) / (StrikePeak - StrikeStart)));
	}
	return FHodgeWingPose::Blend(StrikePose, {}, Ease((Position - StrikePeak) / (RecoveryEnd - StrikePeak)));
}

/** 检查当前配置并收集诊断。 */
bool UHodgeWingProfile::Validate(TArray<FText>& Errors) const
{
	if (PlaybackMode == EHodgeWingPlaybackMode::AnimationAsset)
	{
		if (!FMath::IsFinite(FadeOutSeconds) || FadeOutSeconds < 0.f || (FadeOutSeconds > 0.f && VisibilityParameter.IsNone()))
		{
			Errors.Add(FText::FromString(TEXT("Wing fade requires a finite nonnegative duration and a visibility parameter")));
		}
		if (!Mesh || AnimationBindings.IsEmpty() || AnimationBindings.Num() > 32)
		{
			Errors.Add(FText::FromString(TEXT("Animation wing requires a mesh and between 1 and 32 bindings")));
		}
		const FVector Scale = MeshOffset.GetScale3D();
		if (AttachSocket.IsNone() || !MeshOffset.IsValid() || Scale.X <= 0.f || Scale.X > 10.f || !Scale.Equals(FVector(Scale.X)))
		{
			Errors.Add(FText::FromString(TEXT("Animation wing offset requires a socket and a finite positive uniform scale no greater than 10")));
		}
		TSet<const UAnimSequenceBase*> Sources;
		for (const FHodgeWingAnimationBinding& Binding : AnimationBindings)
		{
			Binding.Validate(Mesh, Errors);
			if (Sources.Contains(Binding.SourceAnimation)) { Errors.Add(FText::FromString(TEXT("Duplicate source animation in wing bindings"))); }
			Sources.Add(Binding.SourceAnimation);
		}
		return Errors.IsEmpty();
	}
	if (!Mesh || !AnimClass || !AnimClass->IsChildOf(UHodgeWingAnimInstance::StaticClass()) || !AttackMontage)
	{ Errors.Add(FText::FromString(TEXT("Wing Mesh, HodgeWingAnimInstance blueprint and AttackMontage are required"))); }
	if (Mesh && AnimClass)
	{
		const auto* BlueprintClass = Cast<UAnimBlueprintGeneratedClass>(AnimClass.Get());
		if (!BlueprintClass || BlueprintClass->GetTargetSkeleton() != Mesh->GetSkeleton())
		{ Errors.Add(FText::FromString(TEXT("Wing animation blueprint must use the wing's own skeleton"))); }
		for (FName Bone : {FName(TEXT("Wing000")), FName(TEXT("Wing003")), FName(TEXT("Wing006"))})
		{
			if (Mesh->GetRefSkeleton().FindBoneIndex(Bone) == INDEX_NONE)
			{ Errors.Add(FText::FromString(FString::Printf(TEXT("Wing mesh is missing controlled bone %s"), *Bone.ToString()))); }
		}
	}
	if (!HasValidTiming() || (AttackMontage && RecoveryEnd > AttackMontage->GetPlayLength()))
	{ Errors.Add(FText::FromString(TEXT("Wing times must be finite, ordered and inside the source montage"))); }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (AttachSocket.IsNone() || !MeshOffset.IsValid() || !MeshOffset.GetScale3D().Equals(FVector::OneVector))
	{ Errors.Add(FText::FromString(TEXT("Wing attachment requires a socket and a finite, unit-scale offset"))); }
	if (WindupPose.Root.ContainsNaN() || WindupPose.Mid.ContainsNaN() || WindupPose.Tip.ContainsNaN() ||
		StrikePose.Root.ContainsNaN() || StrikePose.Mid.ContainsNaN() || StrikePose.Tip.ContainsNaN())
	{ Errors.Add(FText::FromString(TEXT("Wing pose rotations must be finite"))); }
	return Errors.IsEmpty();
}

#if WITH_EDITOR
/** 在编辑器校验当前配置并收集诊断。 */
EDataValidationResult UHodgeWingProfile::IsDataValid(FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);
	TArray<FText> Errors;
	Validate(Errors);
	for (const FText& Error : Errors) { Context.AddError(Error); }
	return Errors.IsEmpty() ? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}
#endif
