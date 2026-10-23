/** @file HodgeHitReactionPlanBuilder.cpp @brief 从同步快照生成受击候选计划，保持原选择顺序与部分失败输出。 */
#include "Combat/HodgeHitReactionPlanBuilder.h"

#include "Animation/AnimMontage.h"
#include "Data/HodgeHitReactionProfile.h"

/** 只读取已确认上下文；只有返回成功的计划才能交给组件提交。 */
bool FHodgeHitReactionPlanBuilder::Build(const FHodgeHitReactionPlanContext& Context, FHodgeHitReactionState& Plan,
	TArray<EHodgeImpactType>& Applied, TArray<FText>& OutErrors)
{
	OutErrors.Reset();
	const auto* Profile = Context.Profile;
	const auto& State = Context.State;
	// 只收集原失败阶段，组件门面负责日志和结果映射。
	const auto RejectPlan = [&OutErrors](const TCHAR* Stage, const TCHAR* Reason)
	{
		OutErrors.Add(FText::FromString(FString::Printf(TEXT("Stage=%s: %s"), Stage, Reason)));
		return false;
	};
	if (!Profile) { return RejectPlan(TEXT("Dependencies"), TEXT("Target reaction Profile is unavailable.")); }
	if (!FMath::IsFinite(Profile->MaxControlDuration) || Profile->MaxControlDuration <= 0.f)
	{ return RejectPlan(TEXT("SafetyLimits"), TEXT("MaxControlDuration must be finite and positive.")); }
	// 快照保留已提交但尚未物理起飞的空中语义，计算器不再读取 CMC。
	const bool bAir = Context.bAir;
	const bool bCanMove = Context.bCanMove;
	Plan.bKnockdownOnLanding = bAir && State.bKnockdownOnLanding;
	const auto* MeshAsset = Context.Mesh;
	float Duration = 0.f;
	FHodgeHitReactionAnimation PresentationStorage;
	FHodgeHitReactionAnimation LandingSelection;
	bool bHasLandingSelection = false;
	const FHodgeHitReactionAnimation* Presentation = nullptr;
	for (const auto& Requested : Context.Request.Impacts)
	{
		// 兼容数组只提供运动参数，未开启分段开关时不能借旧 Launch／AirHit 绕过资格。
		if (!Context.Request.bCanLaunch && (Requested.Type == EHodgeImpactType::Launch || Requested.Type == EHodgeImpactType::AirHit)) { continue; }
		FHodgeHitReactionAnimation Resolved;
		// 破韧选择原地强失衡时，动画和控制时长共同使用目标 Heavy 预设。
		const auto AnimationType = Profile->bUseCompactAnimations && Requested.Type == EHodgeImpactType::HitStun &&
			Context.ReactionLevel >= EHodgeReactionLevel::Heavy ? EHodgeImpactType::Knockback : Requested.Type;
		const auto* Entry = Profile->ResolvePresentation(AnimationType, Context.Presentation.LocalSourceDirection,
			Context.Presentation.Strength, Context.Request.PresentationSide, Context.Variation, Resolved) ? &Resolved : nullptr;
		const bool bApplicable = (!Requested.IsMovement() || bCanMove) && ((Requested.Type != EHodgeImpactType::AirHit && Requested.Type != EHodgeImpactType::Slam) || bAir) &&
			(Requested.Type != EHodgeImpactType::Knockback || !bAir) &&
			((Requested.Type != EHodgeImpactType::AirHit && !(Requested.Type == EHodgeImpactType::Launch && bAir)) || State.AirHitCount < Profile->MaxAirHits);
		bool bFallback = false;
		if (!Entry || !bApplicable)
		{
			if (!Profile->FallbackToHitStun.Contains(Requested.Type)) { continue; }
			Entry = Profile->ResolvePresentation(EHodgeImpactType::HitStun, Context.Presentation.LocalSourceDirection,
				Context.Presentation.Strength, Context.Request.PresentationSide, Context.Variation, Resolved) ? &Resolved : nullptr;
			bFallback = true;
		}
		if (bAir && (bFallback || Requested.Type == EHodgeImpactType::HitStun))
		{ Entry = Profile->ResolvePresentation(EHodgeImpactType::AirHit, Context.Presentation.LocalSourceDirection,
			Context.Presentation.Strength, Context.Request.PresentationSide, Context.Variation, Resolved) ? &Resolved : nullptr; }
		if (!Entry || (!Entry->Montage && !Entry->bAllowWithoutMontage)) { continue; }
		// 空中普通硬直也进入 AirHit 语义，否则后续循环/落地会误读地面条目。
		const EHodgeImpactType Type = bAir && (bFallback || Requested.Type == EHodgeImpactType::HitStun) ? EHodgeImpactType::AirHit :
			bFallback ? EHodgeImpactType::HitStun : Requested.Type;
		Applied.AddUnique(Type);
		const bool bGroundControl = Type == EHodgeImpactType::HitStun || Type == EHodgeImpactType::Knockback;
		if (bGroundControl)
		{
			const float Control = Requested.ControlDuration > 0.f ? Requested.ControlDuration : Entry->DefaultDuration;
			if (!FMath::IsFinite(Control) || Control <= 0.f)
			{ return RejectPlan(TEXT("ControlDuration"), TEXT("Selected control duration must be finite and positive.")); }
			Duration = FMath::Max(Duration, Control);
		}
		if (!Presentation || Type != EHodgeImpactType::HitStun) { PresentationStorage = *Entry; Presentation = &PresentationStorage; Plan.Type = Type; }
		// 兼容根运动击退仍沿用原控制时长上限，但不求值未执行的外部运动或曲线。
		if (!bFallback && Type == EHodgeImpactType::Knockback && Entry->MotionSource == EHodgeReactionMotionSource::AnimationRootMotion)
		{ Duration = FMath::Max(Duration, FMath::Clamp(Requested.MoveDuration, 0.01f, Profile->MaxControlDuration)); }
		if (!bFallback && Requested.IsMovement() &&
			!(Type == EHodgeImpactType::Knockback && Entry->MotionSource == EHodgeReactionMotionSource::AnimationRootMotion))
		{
			if (!FMath::IsFinite(Profile->MaxLaunchSpeed) || Profile->MaxLaunchSpeed <= 0.f)
			{ return RejectPlan(TEXT("MovementLimits"), TEXT("Selected motion requires finite positive MaxLaunchSpeed.")); }
			Plan.bHasMovement = true;
			Plan.MoveDuration = Type == EHodgeImpactType::Knockback ? FMath::Clamp(Requested.MoveDuration, 0.01f, Profile->MaxControlDuration) : 0.f;
			FVector Direction = Requested.Direction == EHodgeImpactDirection::WorldDirection ? Requested.WorldDirection.GetSafeNormal2D() :
				Requested.Direction == EHodgeImpactDirection::SourceForward && Context.bSourceValid ? Context.SourceForward.GetSafeNormal2D() :
				(Context.TargetLocation - Context.Origin).GetSafeNormal2D();
			if (Direction.IsNearlyZero()) { Direction = Context.TargetForward.GetSafeNormal2D(); }
			Plan.MotionVelocity = Direction * (Type == EHodgeImpactType::Knockback ?
				FMath::Min(Requested.KnockbackDistance / Plan.MoveDuration, Profile->MaxLaunchSpeed) : FMath::Min(Requested.HorizontalSpeed, Profile->MaxLaunchSpeed));
			if (Type == EHodgeImpactType::Knockback)
			{
				float Peak = 0.f;
				const float Integral = Profile->GetKnockbackCurveIntegral(Peak);
				if (!FMath::IsFinite(Integral) || Integral <= UE_KINDA_SMALL_NUMBER || !FMath::IsFinite(Peak) || Peak <= UE_KINDA_SMALL_NUMBER)
				{ return RejectPlan(TEXT("KnockbackCurve"), TEXT("Selected external knockback curve has invalid integral or peak.")); }
				// 曲线先按积分归一化，再限制峰值；碰撞或限速允许实际距离小于期望值。
				Plan.MotionVelocity = Direction * FMath::Min(Requested.KnockbackDistance / (Plan.MoveDuration * Integral), Profile->MaxLaunchSpeed / Peak);
			}
			if (Type != EHodgeImpactType::Knockback)
			{
				Plan.MotionVelocity.Z = FMath::Min(Requested.VerticalSpeed, Profile->MaxLaunchSpeed) * (Type == EHodgeImpactType::Slam ? -1.f : 1.f);
				Plan.bKnockdownOnLanding |= Requested.bKnockdownOnLanding || Type == EHodgeImpactType::Slam;
			}
			Duration = FMath::Max(Duration, Plan.MoveDuration);
		}
	}
	if (!Presentation || Applied.IsEmpty())
	{ return RejectPlan(TEXT("Selection"), TEXT("No applicable impact has a usable target animation entry.")); }
	Plan.Montage = Presentation->Montage;
	Plan.ReactionLevel = Context.ReactionLevel;
	Plan.MotionSource = Presentation->MotionSource;
	Plan.LandingMotionSource = Presentation->LandingMotionSource;
	Plan.GetUpMotionSource = Presentation->GetUpMotionSource;
	Plan.AirLoopTransitionTime = Presentation->AirLoopTransitionTime;
	Plan.PresentationStrength = Context.Presentation.Strength;
	Plan.AirLoopMontage = Presentation->AirLoopMontage;
	Plan.LandingMontage = Presentation->LandingMontage;
	Plan.GetUpMontage = Presentation->GetUpMontage;
	if (bAir && Plan.Type == EHodgeImpactType::Knockdown)
	{
		FHodgeHitReactionAnimation AirEntry;
		const auto* Air = Profile->ResolvePresentation(EHodgeImpactType::AirHit, Context.Presentation.LocalSourceDirection,
			Context.Presentation.Strength, Context.Request.PresentationSide, Context.Variation, AirEntry) ? &AirEntry : nullptr;
		if (!Air) { return RejectPlan(TEXT("AirSelection"), TEXT("Airborne Knockdown requires an AirHit presentation.")); }
		LandingSelection = *Presentation;
		bHasLandingSelection = true;
		Plan.bKnockdownOnLanding = true;
		Plan.LandingMontage = Plan.Montage;
		Plan.LandingMotionSource = Presentation->MotionSource;
		Plan.Montage = Air->Montage;
		Plan.AirLoopMontage = Air->AirLoopMontage;
		Plan.AirLoopTransitionTime = Air->AirLoopTransitionTime;
		Plan.MotionSource = EHodgeReactionMotionSource::Impact;
		// 主片已切换到 AirHit，不能继续用 Knockdown 的变体上下文校验它。
		PresentationStorage = AirEntry;
	}
	if (Plan.Montage && !HodgeHitReaction::CanUseMontage(Plan.Montage, MeshAsset))
	{ return RejectPlan(TEXT("MainMontage"), TEXT("Selected main montage is incompatible with the target mesh.")); }
	if (Plan.bKnockdownOnLanding && !Profile->SupportsImpact(EHodgeImpactType::Knockdown))
	{ return RejectPlan(TEXT("Landing"), TEXT("Landing-to-downed path has no Knockdown entry.")); }
	const float Now = Context.ServerTime;
	Plan.StartedAt = Now;
	Plan.RecoveryStartedAt = Now;
	Plan.ChainStartedAt = State.IsActive() ? State.ChainStartedAt : Now;
	Plan.EndsAt = FMath::Min(Now + Duration, Plan.ChainStartedAt + Profile->MaxControlDuration);
	Plan.Phase = bAir || Plan.Type == EHodgeImpactType::Launch || Plan.Type == EHodgeImpactType::Slam ? EHodgeHitReactionPhase::Airborne :
		Plan.Type == EHodgeImpactType::Knockdown ? EHodgeHitReactionPhase::Downed : EHodgeHitReactionPhase::Controlled;
	if (Plan.Phase != EHodgeHitReactionPhase::Airborne && Plan.MotionSource == EHodgeReactionMotionSource::AnimationRootMotion)
	{
		// 根运动拥有本阶段位移，不同时提交按距离/时长计算的外部 RMS。
		Plan.bHasMovement = false;
		Plan.MotionVelocity = FVector::ZeroVector;
		Plan.MoveDuration = 0.f;
	}
	if (Plan.Phase == EHodgeHitReactionPhase::Airborne)
	{
		Plan.AirStartedAt = State.Phase == EHodgeHitReactionPhase::Airborne ? State.AirStartedAt : Now;
		Plan.AirOriginZ = State.Phase == EHodgeHitReactionPhase::Airborne ? State.AirOriginZ : Context.TargetLocation.Z;
		Plan.AirHitCount = State.Phase == EHodgeHitReactionPhase::Airborne ? State.AirHitCount : 0;
		// 普通空中接触只换姿态；只有真实提交续空运动才消耗浮空次数。
		Plan.AirHitCount += Plan.bHasMovement && (Plan.Type == EHodgeImpactType::AirHit || (Plan.Type == EHodgeImpactType::Launch && State.Phase == EHodgeHitReactionPhase::Airborne)) ? 1 : 0;
		Plan.EndsAt = FMath::Min(Plan.AirStartedAt + Profile->MaxAirborneDuration, Plan.ChainStartedAt + Profile->MaxControlDuration);
		Plan.MotionSource = EHodgeReactionMotionSource::Impact;
	}
	if (Plan.Phase == EHodgeHitReactionPhase::Downed)
	{ Plan.EndsAt = FMath::Min(Now + Profile->DownedDuration, Plan.ChainStartedAt + Profile->MaxControlDuration); }
	if (Context.ReactionLevel != EHodgeReactionLevel::None || Context.Request.InputMode == EHodgeReactionInputMode::PoiseDriven)
	{
		// 只检查真正采用的后续阶段；完整资产检查仍报告未使用配置的全局违规。
		const bool bAirPlan = Plan.Phase == EHodgeHitReactionPhase::Airborne;
		const bool bDownPlan = Plan.Phase == EHodgeHitReactionPhase::Downed || Plan.bKnockdownOnLanding;
		UAnimMontage* Landing = bAirPlan ? Plan.LandingMontage.Get() : nullptr;
		UAnimMontage* GetUp = bDownPlan ? Plan.GetUpMontage.Get() : nullptr;
		auto LandingMotion = Plan.LandingMotionSource;
		auto GetUpMotion = Plan.GetUpMotionSource;
		FHodgeHitReactionAnimation Down;
		if (bAirPlan && bDownPlan && Profile->ResolveAnimation(EHodgeImpactType::Knockdown, Down))
		{
			GetUp = Down.GetUpMontage;
			GetUpMotion = Down.GetUpMotionSource;
			if (!Landing) { Landing = Down.Montage; LandingMotion = Down.MotionSource; }
		}
		if (Plan.MotionSource != EHodgeReactionMotionSource::Impact ||
			(Landing && LandingMotion != EHodgeReactionMotionSource::Impact) ||
			(GetUp && GetUpMotion != EHodgeReactionMotionSource::Impact))
		{ return RejectPlan(TEXT("MotionPolicy"), TEXT("Actual ordinary-poise phases must use Impact motion.")); }
	}
	// 同级或更轻的接触延续原动作，新的控制身份与结束时间仍会更新。
	if (Plan.Phase == EHodgeHitReactionPhase::Controlled && State.Phase == Plan.Phase && Plan.Type == EHodgeImpactType::HitStun &&
		State.Type == Plan.Type && Plan.ReactionLevel == State.ReactionLevel && !Plan.bHasMovement && !State.bHasMovement && Plan.Montage && State.Montage &&
		(Profile->bUseCompactAnimations || Profile->GroundReplayInterval <= 0.f || Now - State.RecoveryStartedAt < Profile->GroundReplayInterval ||
			Plan.PresentationStrength + UE_KINDA_SMALL_NUMBER < State.PresentationStrength) &&
		(Plan.Montage == State.Montage || Plan.PresentationStrength <= State.PresentationStrength))
	{
		Plan.Montage = State.Montage;
		Plan.PresentationStrength = FMath::Max(Plan.PresentationStrength, State.PresentationStrength);
		Plan.StartedAt = State.StartedAt;
		// 明确续播后才移除未执行的新变体，不能由校验器按片段指针差异猜测。
		PresentationStorage.Montage = State.Montage;
		PresentationStorage.Variants.Reset();
	}
	// 最终跨阶段选片与续播决定后再检查播放依赖，不能先取消旧动作才发现实际主片无法执行。
	if (Plan.Montage && !Context.bHasAnimInstance)
	{ return RejectPlan(TEXT("Dependencies"), TEXT("Selected main montage has no current ability animation instance.")); }
	bool bValidPresentation = Profile->ValidatePlanPresentation(*Presentation, Plan, MeshAsset, OutErrors);
	if (bHasLandingSelection)
	{
		// 原 Knockdown 已成为真实落地主片，其选中变体也必须独立保持合法。
		auto LandingPlan = Plan;
		LandingPlan.Type = EHodgeImpactType::Knockdown;
		LandingPlan.Phase = EHodgeHitReactionPhase::Downed;
		LandingPlan.Montage = Plan.LandingMontage;
		LandingPlan.MotionSource = Plan.LandingMotionSource;
		LandingPlan.bKnockdownOnLanding = false;
		LandingPlan.bHasMovement = false;
		bValidPresentation = Profile->ValidatePlanPresentation(LandingSelection, LandingPlan, MeshAsset, OutErrors) && bValidPresentation;
	}
	if (!bValidPresentation) { return false; }
	if (!(Plan.EndsAt > Now)) { return RejectPlan(TEXT("ControlBudget"), TEXT("Current control chain has no remaining duration.")); }
	return true;
}

/** 最终计划决定预设，来源只读命中快照；旧Build和世界运动计算不参与补转。 */
bool FHodgeHitReactionPlanBuilder::BuildFacingCandidate(const FHodgeHitReactionPlanContext& Context,
	const FHodgeHitReactionState& SuccessfulPlan, float ActualStartYaw, FHodgeHitReactionFacingCandidate& Out,
	TArray<FText>& Errors)
{
	Out = {};
	if (FMath::IsFinite(ActualStartYaw)) { Out.TargetYaw = FRotator::NormalizeAxis(ActualStartYaw); }
	if (Context.bAir || Context.State.Phase == EHodgeHitReactionPhase::Airborne ||
		Context.State.Phase == EHodgeHitReactionPhase::Downed || Context.State.Phase == EHodgeHitReactionPhase::GettingUp ||
		SuccessfulPlan.MotionSource != EHodgeReactionMotionSource::Impact ||
		(SuccessfulPlan.ReactionLevel != EHodgeReactionLevel::None && SuccessfulPlan.ReactionLevel != EHodgeReactionLevel::Light &&
		 SuccessfulPlan.ReactionLevel != EHodgeReactionLevel::Heavy)) { return true; }
	if (SuccessfulPlan.Phase == EHodgeHitReactionPhase::Controlled)
	{
		if (SuccessfulPlan.Type == EHodgeImpactType::HitStun)
		{ Out.Preset = SuccessfulPlan.ReactionLevel == EHodgeReactionLevel::Heavy ? EHodgeHitReactionFacingPreset::Heavy : EHodgeHitReactionFacingPreset::Light; }
		else if (SuccessfulPlan.Type == EHodgeImpactType::Knockback) { Out.Preset = EHodgeHitReactionFacingPreset::Heavy; }
	}
	else if (SuccessfulPlan.Phase == EHodgeHitReactionPhase::Airborne && SuccessfulPlan.Type == EHodgeImpactType::Launch)
	{ Out.Preset = EHodgeHitReactionFacingPreset::Launch; }
	if (Out.Preset == EHodgeHitReactionFacingPreset::None) { return true; }
	if (!Context.Profile)
	{ Errors.Add(FText::FromString(TEXT("Facing candidate requires the submitted target Profile."))); return false; }
	const auto& Settings = Context.Profile->Facing;
	// 无目标也保留预设，提交者据此防止后续同级命中重新取得来源。
	if (!Settings.bEnabled || !Context.FacingSource.bValid) { return true; }
	const FVector Direction(Context.FacingSource.SourceLocation.X - Context.FacingSource.TargetLocation.X,
		Context.FacingSource.SourceLocation.Y - Context.FacingSource.TargetLocation.Y, 0.);
	if (!FMath::IsFinite(Context.FacingSource.SourceLocation.X) || !FMath::IsFinite(Context.FacingSource.SourceLocation.Y) ||
		!FMath::IsFinite(Context.FacingSource.TargetLocation.X) || !FMath::IsFinite(Context.FacingSource.TargetLocation.Y) || Direction.ContainsNaN())
	{ Errors.Add(FText::FromString(TEXT("Facing candidate source and target XY must be finite."))); return false; }
	if (!FMath::IsFinite(ActualStartYaw))
	{ Errors.Add(FText::FromString(TEXT("Facing candidate ActualStartYaw must be finite."))); return false; }
	const auto& Preset = Out.Preset == EHodgeHitReactionFacingPreset::Light ? Settings.Light :
		Out.Preset == EHodgeHitReactionFacingPreset::Heavy ? Settings.Heavy : Settings.Launch;
	const TCHAR* PresetName = Out.Preset == EHodgeHitReactionFacingPreset::Light ? TEXT("Light") :
		Out.Preset == EHodgeHitReactionFacingPreset::Heavy ? TEXT("Heavy") : TEXT("Launch");
	const int32 Before = Errors.Num();
	TArray<FText> PresetErrors;
	Preset.Validate(PresetErrors);
	for (const FText& Error : PresetErrors)
	{ Errors.Add(FText::FromString(FString::Printf(TEXT("Facing.%s: %s"), PresetName, *Error.ToString()))); }
	// 运行时仅检查本次消费的预设，其他预设非法仍由完整资产检查报告。
	if (!FMath::IsFinite(Settings.DeadZoneAngle) || Settings.DeadZoneAngle < 0.f || Settings.DeadZoneAngle > Preset.MaxCorrectionAngle)
	{ Errors.Add(FText::FromString(TEXT("Facing.DeadZoneAngle must be finite, nonnegative and no greater than the selected MaxCorrectionAngle."))); }
	if (Before != Errors.Num()) { return false; }
	Out.MaxCorrectionAngle = Preset.MaxCorrectionAngle;
	Out.WindowDuration = Preset.WindowDuration;
	Out.MaxYawSpeed = Preset.MaxYawSpeed;
	Out.DeadZoneAngle = Settings.DeadZoneAngle;
	if (Direction.IsNearlyZero()) { return true; }
	Out.SourceYaw = FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));
	const float StartYaw = FRotator::NormalizeAxis(ActualStartYaw);
	// NormalizeAxis使用(-180,180]，恰好背向时稳定选择正Yaw，不在末帧补齐。
	const float DeltaYaw = FRotator::NormalizeAxis(Out.SourceYaw - StartYaw);
	if (FMath::Abs(DeltaYaw) <= Settings.DeadZoneAngle) { return true; }
	Out.TargetYaw = FRotator::NormalizeAxis(StartYaw + FMath::Clamp(DeltaYaw, -Preset.MaxCorrectionAngle, Preset.MaxCorrectionAngle));
	Out.bHasTarget = true;
	return true;
}

