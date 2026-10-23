/** @file HodgeHitReactionProfile.cpp
 * @brief 实现目标受击动画与恢复配置相关的类型、接口与现有处理流程。
 */
#include "Data/HodgeHitReactionProfile.h"
#include "Animation/AnimMontage.h"
#include "Curves/CurveFloat.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHitReactionProfile)

/** 角度有明确上界，时窗与速率只要求有限正值，不擅自增加玩法上限。 */
bool FHodgeHitReactionFacingPreset::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	if (!FMath::IsFinite(MaxCorrectionAngle) || MaxCorrectionAngle <= 0.f || MaxCorrectionAngle > 180.f)
	{ Errors.Add(FText::FromString(TEXT("MaxCorrectionAngle must be finite and in (0, 180] degrees."))); }
	if (!FMath::IsFinite(WindowDuration) || WindowDuration <= 0.f)
	{ Errors.Add(FText::FromString(TEXT("WindowDuration must be finite and positive seconds."))); }
	if (!FMath::IsFinite(MaxYawSpeed) || MaxYawSpeed <= 0.f)
	{ Errors.Add(FText::FromString(TEXT("MaxYawSpeed must be finite and positive degrees per second."))); }
	return Before == Errors.Num();
}

/** 未显式覆盖的目标采用玩家参数，启用开关仍保持关闭。 */
FHodgeHitReactionFacingSettings::FHodgeHitReactionFacingSettings()
{
	Light.MaxCorrectionAngle = 30.f;
	Light.WindowDuration = 0.08f;
	Light.MaxYawSpeed = 720.f;
	Heavy.MaxCorrectionAngle = 60.f;
	Heavy.WindowDuration = 0.12f;
	Heavy.MaxYawSpeed = 720.f;
	Launch.MaxCorrectionAngle = 60.f;
	Launch.WindowDuration = 0.12f;
	Launch.MaxYawSpeed = 720.f;
}

/** 完整配置检查保留预设路径；禁用功能不因未消费的旧值拒绝资产。 */
bool FHodgeHitReactionFacingSettings::Validate(TArray<FText>& Errors) const
{
	if (!bEnabled) { return true; }
	const int32 Before = Errors.Num();
	auto CheckPreset = [&Errors](const TCHAR* Name, const FHodgeHitReactionFacingPreset& Preset)
	{
		TArray<FText> PresetErrors;
		Preset.Validate(PresetErrors);
		for (const FText& Error : PresetErrors)
		{ Errors.Add(FText::FromString(FString::Printf(TEXT("Facing.%s: %s"), Name, *Error.ToString()))); }
	};
	CheckPreset(TEXT("Light"), Light);
	CheckPreset(TEXT("Heavy"), Heavy);
	CheckPreset(TEXT("Launch"), Launch);
	if (!FMath::IsFinite(DeadZoneAngle) || DeadZoneAngle < 0.f || DeadZoneAngle > Light.MaxCorrectionAngle ||
		DeadZoneAngle > Heavy.MaxCorrectionAngle || DeadZoneAngle > Launch.MaxCorrectionAngle)
	{ Errors.Add(FText::FromString(TEXT("Facing.DeadZoneAngle must be finite, nonnegative and no greater than every preset MaxCorrectionAngle."))); }
	return Before == Errors.Num();
}

/** 查找动画。 */
const FHodgeHitReactionAnimation* UHodgeHitReactionProfile::FindAnimation(EHodgeImpactType Type) const
{
	return Animations.FindByPredicate([Type](const auto& Entry) { return Entry.Type == Type; });
}

/** 精简布局只在使用时展开；Fly 与 Push 不因合并公共起身而混淆落地路径。 */
bool UHodgeHitReactionProfile::ResolveAnimation(EHodgeImpactType Type, FHodgeHitReactionAnimation& Out) const
{
	if (!bUseCompactAnimations)
	{
		const auto* Entry = FindAnimation(Type);
		if (Entry) { Out = *Entry; }
		return Entry != nullptr;
	}
	Out = FHodgeHitReactionAnimation();
	Out.Type = Type;
	Out.GetUpMontage = Compact.GetUp;
	if (Type == EHodgeImpactType::HitStun || Type == EHodgeImpactType::Knockback)
	{
		const auto& Pair = Type == EHodgeImpactType::HitStun ? Compact.Light : Compact.Heavy;
		Out.Montage = Pair.Right ? Pair.Right : Pair.Left;
		Out.DefaultDuration = Pair.ControlDuration;
		Out.bUseLateralVariants = true;
		for (auto Side : {EHodgeHitPresentationDirection::Left, EHodgeHitPresentationDirection::Right})
		{
			UAnimMontage* Montage = Side == EHodgeHitPresentationDirection::Left ? Pair.Left.Get() : Pair.Right.Get();
			if (Montage) { FHodgeHitReactionVariant Variant; Variant.Direction = Side; Variant.Montage = Montage; Out.Variants.Add(Variant); }
		}
	}
	else if (Type == EHodgeImpactType::Knockdown) { Out.Montage = Compact.Downed; }
	else
	{
		const auto& Family = Type == EHodgeImpactType::Launch ? Compact.Fly : Compact.Push;
		Out.Montage = Family.Start;
		Out.AirLoopMontage = Family.Loop;
		Out.LandingMontage = Family.Landing;
		Out.AirLoopTransitionTime = Compact.AirLoopTransitionTime;
	}
	return Out.Montage != nullptr;
}

/** 精简布局不支持的动作必须显式缺失，不能读取隐藏旧字段冒充成功。 */
bool UHodgeHitReactionProfile::SupportsImpact(EHodgeImpactType Type) const
{
	FHodgeHitReactionAnimation Entry;
	return ResolveAnimation(Type, Entry);
}

/** 选用局部来源最主要的水平轴；退化方向按正面处理。 */
UAnimMontage* UHodgeHitReactionProfile::FindLightFeedback(FVector LocalSourceDirection) const
{
	const auto Direction = FMath::Abs(LocalSourceDirection.X) >= FMath::Abs(LocalSourceDirection.Y) ?
		(LocalSourceDirection.X >= 0.f ? EHodgeHitPresentationDirection::Front : EHodgeHitPresentationDirection::Back) :
		(LocalSourceDirection.Y >= 0.f ? EHodgeHitPresentationDirection::Right : EHodgeHitPresentationDirection::Left);
	const auto* Entry = DirectionalLightFeedback.FindByPredicate([Direction](const auto& Value) { return Value.Direction == Direction; });
	return Entry ? Entry->Montage.Get() : LightFeedbackMontage.Get();
}

/** 同一方向优先满足的最高力度阈值，不随机改变网络解析结果。 */
UAnimMontage* UHodgeHitReactionProfile::FindReactionMontage(EHodgeImpactType Type, FVector Direction, float Strength,
	EHodgeHitSide RequestedSide, int32 Variation, EHodgeReactionLevel Level) const
{
	FHodgeHitReactionAnimation Selected;
	return ResolvePresentation(Type, Direction, Strength, RequestedSide, Variation, Selected, Level) ? Selected.Montage.Get() : nullptr;
}

/** 查询与计划共用一次选择结果；返回副本不改变资产，也不持有数组元素指针。 */
bool UHodgeHitReactionProfile::ResolvePresentation(EHodgeImpactType Type, FVector Direction, float Strength,
	EHodgeHitSide RequestedSide, int32 Variation, FHodgeHitReactionAnimation& Out, EHodgeReactionLevel Level) const
{
	// 轻重由目标等级决定；接触特效强度不再选取 S/B 动作。
	const auto AnimationType = bUseCompactAnimations && Type == EHodgeImpactType::HitStun && Level >= EHodgeReactionLevel::Heavy ? EHodgeImpactType::Knockback : Type;
	if (!ResolveAnimation(AnimationType, Out)) { return false; }
	const auto* Entry = &Out;
	auto Side = FMath::Abs(Direction.X) >= FMath::Abs(Direction.Y) ?
		(Direction.X >= 0.f ? EHodgeHitPresentationDirection::Front : EHodgeHitPresentationDirection::Back) :
		(Direction.Y >= 0.f ? EHodgeHitPresentationDirection::Right : EHodgeHitPresentationDirection::Left);
	if (RequestedSide != EHodgeHitSide::Automatic)
	{ Side = RequestedSide == EHodgeHitSide::Left ? EHodgeHitPresentationDirection::Left : EHodgeHitPresentationDirection::Right; }
	else if (Entry->bUseLateralVariants && FMath::Abs(Direction.Y) <= FMath::Abs(Direction.X))
	{ Side = Variation % 2 == 0 ? EHodgeHitPresentationDirection::Left : EHodgeHitPresentationDirection::Right; }
	const FHodgeHitReactionVariant* Selected = nullptr;
	for (const auto& Variant : Entry->Variants)
	{
		if (Variant.Direction == Side && Variant.MinimumStrength <= Strength &&
			(!Selected || Variant.MinimumStrength > Selected->MinimumStrength)) { Selected = &Variant; }
	}
	if (Selected)
	{
		const FHodgeHitReactionVariant Variant = *Selected;
		Out.Montage = Variant.Montage;
		// 同方向同阈值的重复配置仍属于本次路径，交由共同校验报告歧义。
		Out.Variants.RemoveAll([&Variant](const auto& Value)
		{ return Value.Direction != Variant.Direction || Value.MinimumStrength != Variant.MinimumStrength; });
	}
	else { Out.Variants.Reset(); }
	return true;
}

/** 使用固定分段梯形积分，保证各端对同一配置得到一致的位移归一化。 */
float UHodgeHitReactionProfile::GetKnockbackCurveIntegral(float& OutPeak) const
{
	OutPeak = 1.f;
	if (!KnockbackStrengthCurve) { return 1.f; }
	if (KnockbackStrengthCurve->FloatCurve.GetNumKeys() == 0) { return 0.f; }
	for (const auto& Key : KnockbackStrengthCurve->FloatCurve.GetConstRefOfKeys())
	{
		if (!FMath::IsFinite(Key.Time) || !FMath::IsFinite(Key.Value) || !FMath::IsFinite(Key.ArriveTangent) ||
			!FMath::IsFinite(Key.LeaveTangent) || Key.Time < 0.f || Key.Time > 1.f || Key.Value < 0.f) { return 0.f; }
	}
	constexpr int32 Steps = 128;
	float Integral = 0.f;
	OutPeak = 0.f;
	for (int32 Index = 0; Index <= Steps; ++Index)
	{
		const float Value = KnockbackStrengthCurve->GetFloatValue(float(Index) / Steps);
		if (!FMath::IsFinite(Value) || Value < 0.f) { return 0.f; }
		OutPeak = FMath::Max(OutPeak, Value);
		Integral += Value * ((Index == 0 || Index == Steps) ? 0.5f : 1.f) / Steps;
	}
	float Minimum = 0.f;
	float Maximum = 0.f;
	KnockbackStrengthCurve->FloatCurve.GetValueRange(Minimum, Maximum);
	if (!FMath::IsFinite(Integral) || !FMath::IsFinite(Minimum) || !FMath::IsFinite(Maximum) || Minimum < 0.f) { return 0.f; }
	OutPeak = FMath::Max(OutPeak, Maximum);
	return Integral;
}

/** 倒地维持已有规则；地面硬直按目标开关保持，不暂停攻击者或世界时间。 */
bool UHodgeHitReactionProfile::ShouldSustainPose(const FHodgeHitReactionState& State) const
{
	// 根运动完整播放后只保持末姿态；不循环位移段，也不提前冻结完整动作。
	if (State.MotionSource == EHodgeReactionMotionSource::AnimationRootMotion)
	{ return State.Phase == EHodgeHitReactionPhase::Controlled || State.Phase == EHodgeHitReactionPhase::Downed; }
	if (State.bLanding) { return false; }
	if (State.Phase == EHodgeHitReactionPhase::Airborne && State.AirLoopTransitionTime > 0.f) { return true; }
	FHodgeHitReactionAnimation Resolved;
	const auto* Entry = ResolveAnimation(State.Type, Resolved) ? &Resolved : nullptr;
	return State.Phase == EHodgeHitReactionPhase::Downed ||
		(State.Phase == EHodgeHitReactionPhase::Controlled && ((bSustainGroundHitStun && State.Type == EHodgeImpactType::HitStun) ||
			(Entry && !Entry->SustainSection.IsNone())));
}

/** 持续段按同一服务器起播时间取模，各端不通过重播主动作续硬直。 */
float UHodgeHitReactionProfile::GetPresentationPosition(const FHodgeHitReactionState& State, float Elapsed) const
{
	FHodgeHitReactionAnimation Resolved;
	const auto* Entry = ResolveAnimation(State.Type, Resolved) ? &Resolved : nullptr;
	if (Entry && State.Montage && !State.bLanding && State.Phase == EHodgeHitReactionPhase::Controlled && !Entry->SustainSection.IsNone())
	{
		const int32 Index = State.Montage->GetSectionIndex(Entry->SustainSection);
		if (Index != INDEX_NONE)
		{
			float Start = 0.f, End = 0.f;
			State.Montage->GetSectionStartAndEndTime(Index, Start, End);
			if (End > Start && Elapsed >= Start) { return Start + FMath::Fmod(Elapsed - Start, End - Start); }
		}
	}
	return FMath::Max(0.f, Elapsed);
}

/** 最新受击时间与动画起点独立，续播动画不会让后续攻击立即逃脱。 */
bool UHodgeHitReactionProfile::IsRecoveryWindowOpen(const FHodgeHitReactionState& State, EHodgeHitRecoveryAction Action, float Now) const
{
	if (!bEnableGroundRecovery || State.Phase != EHodgeHitReactionPhase::Controlled || State.Type != EHodgeImpactType::HitStun ||
		State.bHasMovement || State.bLanding || !FMath::IsFinite(Now) || Now < State.RecoveryStartedAt) { return false; }
	float Delay = -1.f;
	switch (Action)
	{
	case EHodgeHitRecoveryAction::Dodge: Delay = DodgeRecoveryDelay; break;
	case EHodgeHitRecoveryAction::BasicAttack: Delay = AttackRecoveryDelay; break;
	case EHodgeHitRecoveryAction::Movement: Delay = MovementRecoveryDelay; break;
	default: return false;
	}
	return FMath::IsFinite(Delay) && Delay >= 0.f && Now - State.RecoveryStartedAt >= Delay;
}

/** 运行时只检查本次会执行的资源，完整资产检查仍保留所有方向和家族错误。 */
bool UHodgeHitReactionProfile::ValidatePlanPresentation(const FHodgeHitReactionAnimation& Selection,
	const FHodgeHitReactionState& Plan, const USkeletalMesh* Mesh, TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	const bool bAir = Plan.Phase == EHodgeHitReactionPhase::Airborne;
	const bool bDown = Plan.Phase == EHodgeHitReactionPhase::Downed || Plan.bKnockdownOnLanding;
	const bool bGroundHit = Plan.Phase == EHodgeHitReactionPhase::Controlled && Plan.Type == EHodgeImpactType::HitStun;
	if (!bUseCompactAnimations)
	{
		// 空中击倒会跨用AirHit和Knockdown，所需后续条目也必须唯一。
		auto CheckUnique = [this, &Errors](EHodgeImpactType Type)
		{
			int32 Matches = 0;
			for (const auto& Entry : Animations)
			{
				if (Entry.Type == Type && ++Matches > 1)
				{ Errors.Add(FText::FromString(TEXT("Selected reaction type has duplicate animation entries."))); break; }
			}
		};
		CheckUnique(Selection.Type);
		if (bAir && Plan.Type == EHodgeImpactType::Knockdown) { CheckUnique(EHodgeImpactType::AirHit); }
		if (bAir && bDown && Selection.Type != EHodgeImpactType::Knockdown) { CheckUnique(EHodgeImpactType::Knockdown); }
	}
	if (!FMath::IsFinite(MaxControlDuration) || MaxControlDuration <= 0.f ||
		!FMath::IsFinite(ReplacementBlendOut) || ReplacementBlendOut < 0.f ||
		(bAir && (!FMath::IsFinite(MaxAirborneDuration) || MaxAirborneDuration <= 0.f ||
			!FMath::IsFinite(MaxLaunchHeight) || MaxLaunchHeight <= 0.f || MaxAirHits < 0)) ||
		(bDown && (!FMath::IsFinite(DownedDuration) || DownedDuration <= 0.f)) ||
		(Plan.bHasMovement && (!FMath::IsFinite(MaxLaunchSpeed) || MaxLaunchSpeed <= 0.f)) ||
		(bGroundHit && !bUseCompactAnimations && (!FMath::IsFinite(GroundReplayInterval) || GroundReplayInterval < 0.f)))
	{ Errors.Add(FText::FromString(TEXT("Selected reaction has invalid control, blend, airborne, downed or movement limits."))); }
	if (bGroundHit && !Plan.bHasMovement && bEnableGroundRecovery &&
		(!FMath::IsFinite(DodgeRecoveryDelay) || DodgeRecoveryDelay < 0.f || !FMath::IsFinite(AttackRecoveryDelay) || AttackRecoveryDelay < 0.f ||
		 !FMath::IsFinite(MovementRecoveryDelay) || MovementRecoveryDelay < 0.f))
	{ Errors.Add(FText::FromString(TEXT("Selected ground reaction requires finite nonnegative recovery delays."))); }
	if (bUseCompactAnimations && bAir && (!Plan.Montage || !Plan.AirLoopMontage || !Plan.LandingMontage))
	{ Errors.Add(FText::FromString(TEXT("Selected compact airborne path requires its actual Start, Loop and Landing montages."))); }
	FHodgeHitReactionAnimation Actual = Selection;
	if (Plan.Montage != Selection.Montage)
	{ Errors.Add(FText::FromString(TEXT("Selected presentation must describe the actual main montage; continuation must be resolved by its caller."))); }
	Actual.Montage = Plan.Montage;
	// 调用者传入真正播放的阶段选择；续播时只由明确的续播分支移除未使用变体。
	Actual.MotionSource = Plan.MotionSource;
	Actual.AirLoopMontage = bAir ? Plan.AirLoopMontage : nullptr;
	Actual.LandingMontage = bAir ? Plan.LandingMontage : nullptr;
	Actual.GetUpMontage = bDown ? Plan.GetUpMontage : nullptr;
	Actual.AirLoopTransitionTime = bAir ? Plan.AirLoopTransitionTime : 0.f;
	Actual.LandingMotionSource = bAir ? Plan.LandingMotionSource : EHodgeReactionMotionSource::Impact;
	Actual.GetUpMotionSource = bDown ? Plan.GetUpMotionSource : EHodgeReactionMotionSource::Impact;
	if (bAir && bDown)
	{
		// 真实接地会读取Knockdown的起身配置，提前检查该路径实际采用的资源。
		FHodgeHitReactionAnimation Down;
		if (!ResolveAnimation(EHodgeImpactType::Knockdown, Down))
		{ Errors.Add(FText::FromString(TEXT("Selected landing-to-downed path has no Knockdown configuration."))); }
		else
		{
			Actual.GetUpMontage = Down.GetUpMontage;
			Actual.GetUpMotionSource = Down.GetUpMotionSource;
			if (!Actual.LandingMontage)
			{
				Actual.LandingMontage = Down.Montage;
				Actual.LandingMotionSource = Down.MotionSource;
				if (!Down.Montage && !Down.bAllowWithoutMontage)
				{ Errors.Add(FText::FromString(TEXT("Selected landing-to-downed fallback requires a Knockdown montage or explicit animation-free permission."))); }
			}
		}
	}
	// 请求已覆盖控制时长时，不再因资产中未消费的默认时长拒绝本次执行。
	if (Actual.Type == EHodgeImpactType::HitStun || Actual.Type == EHodgeImpactType::Knockback)
	{ Actual.DefaultDuration = Plan.EndsAt - Plan.StartedAt; }
	ValidateAnimationEntry(Actual, Errors);
	for (const auto* Montage : {Actual.Montage.Get(), Actual.AirLoopMontage.Get(), Actual.LandingMontage.Get(), Actual.GetUpMontage.Get()})
	{
		if (Montage && !HodgeHitReaction::CanUseMontage(Montage, Mesh))
		{ Errors.Add(FText::FromString(FString::Printf(TEXT("Selected montage is incompatible with target mesh: %s"), *GetPathNameSafe(Montage)))); }
	}
	return Before == Errors.Num();
}

/** 完整资产与运行时已选条目共用动画契约，校验范围由调用者明确提供。 */
void UHodgeHitReactionProfile::ValidateAnimationEntry(const FHodgeHitReactionAnimation& Entry, TArray<FText>& Errors)
{
	const bool bGroundControl = Entry.Type == EHodgeImpactType::HitStun || Entry.Type == EHodgeImpactType::Knockback;
	if (uint8(Entry.Type) > uint8(EHodgeImpactType::Slam) ||
		(bGroundControl && (!FMath::IsFinite(Entry.DefaultDuration) || Entry.DefaultDuration <= 0.f)) || (!Entry.Montage && !Entry.bAllowWithoutMontage))
	{
		Errors.Add(FText::FromString(TEXT("Reaction entries require unique valid types, a positive duration and a montage or explicit animation-free permission.")));
	}
	const bool bAirType = Entry.Type == EHodgeImpactType::Launch || Entry.Type == EHodgeImpactType::AirHit || Entry.Type == EHodgeImpactType::Slam;
	if (uint8(Entry.MotionSource) > uint8(EHodgeReactionMotionSource::AnimationRootMotion) ||
		uint8(Entry.LandingMotionSource) > uint8(EHodgeReactionMotionSource::AnimationRootMotion) ||
		uint8(Entry.GetUpMotionSource) > uint8(EHodgeReactionMotionSource::AnimationRootMotion) ||
		(bAirType && Entry.MotionSource != EHodgeReactionMotionSource::Impact) ||
		!FMath::IsFinite(Entry.AirLoopTransitionTime) || Entry.AirLoopTransitionTime < 0.f ||
		(Entry.MotionSource == EHodgeReactionMotionSource::AnimationRootMotion && !Entry.SustainSection.IsNone()))
	{ Errors.Add(FText::FromString(TEXT("Reaction motion source is invalid: airborne motion uses Impact and root-motion ground clips cannot loop Sustain."))); }
	if (Entry.SustainSection.IsNone() != Entry.RecoverySection.IsNone() || (!bGroundControl && !Entry.SustainSection.IsNone()))
	{ Errors.Add(FText::FromString(TEXT("Sustain/Recovery sections must be paired and are supported only by ground HitStun/Knockback."))); }
	TArray<UAnimMontage*> Presentations = {Entry.Montage.Get()};
	for (const auto& Variant : Entry.Variants) { Presentations.Add(Variant.Montage.Get()); }
	if (!Entry.SustainSection.IsNone())
	{
		for (const auto* Montage : Presentations)
		{
			const int32 Loop = Montage ? Montage->GetSectionIndex(Entry.SustainSection) : INDEX_NONE;
			const int32 Recovery = Montage ? Montage->GetSectionIndex(Entry.RecoverySection) : INDEX_NONE;
			if (Loop == INDEX_NONE || Recovery == INDEX_NONE || Recovery <= Loop)
			{ Errors.Add(FText::FromString(TEXT("Every reaction variant requires valid ordered Sustain and Recovery sections."))); }
		}
	}
	for (int32 Index = 0; Index < Entry.Variants.Num(); ++Index)
	{
		const auto& Variant = Entry.Variants[Index];
		const bool bDuplicate = Entry.Variants.ContainsByPredicate([&](const auto& Other)
		{ return &Other != &Variant && Other.Direction == Variant.Direction && Other.MinimumStrength == Variant.MinimumStrength; });
		if (uint8(Variant.Direction) > uint8(EHodgeHitPresentationDirection::Right) ||
			!FMath::IsFinite(Variant.MinimumStrength) || Variant.MinimumStrength < 0.1f || Variant.MinimumStrength > 2.f ||
			!Variant.Montage || Variant.Montage->GetPlayLength() <= 0.f || bDuplicate)
		{ Errors.Add(FText::FromString(TEXT("Reaction variants require a valid montage, direction and unique finite strength threshold (0.1..2)."))); }
	}
	// 默认片、每个变体和后续阶段分别校验运动契约，禁止只给默认片开绿灯。
	auto CheckMotion = [&Errors](const UAnimMontage* Montage, EHodgeReactionMotionSource Motion)
	{
		if (Montage && (Montage->GetPlayLength() <= 0.f || Montage->HasRootMotion() != (Motion == EHodgeReactionMotionSource::AnimationRootMotion)))
		{ Errors.Add(FText::FromString(TEXT("Reaction montage root motion must match its explicit phase motion source."))); }
	};
	for (const auto* Montage : Presentations) { CheckMotion(Montage, Entry.MotionSource); }
	CheckMotion(Entry.AirLoopMontage, EHodgeReactionMotionSource::Impact);
	CheckMotion(Entry.LandingMontage, Entry.LandingMotionSource);
	CheckMotion(Entry.GetUpMontage, Entry.GetUpMotionSource);
}

/** 检查当前配置并收集诊断。 */
bool UHodgeHitReactionProfile::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	// 此入口检查完整资产，运行时已选动画校验不因未消费的补转配置拒绝合法受击。
	Facing.Validate(Errors);
	if (bEnableGroundRecovery && (!FMath::IsFinite(DodgeRecoveryDelay) || !FMath::IsFinite(AttackRecoveryDelay) ||
		!FMath::IsFinite(MovementRecoveryDelay) || DodgeRecoveryDelay < 0.f || AttackRecoveryDelay < 0.f || MovementRecoveryDelay < 0.f))
	{ Errors.Add(FText::FromString(TEXT("Enabled ground recovery delays must be finite and non-negative seconds."))); }
	TSet<EHodgeImpactType> Types;
	TArray<FHodgeHitReactionAnimation> ResolvedAnimations;
	if (bUseCompactAnimations)
	{
		for (auto Type : {EHodgeImpactType::HitStun, EHodgeImpactType::Knockback, EHodgeImpactType::Launch, EHodgeImpactType::AirHit, EHodgeImpactType::Slam, EHodgeImpactType::Knockdown})
		{ FHodgeHitReactionAnimation Entry; if (ResolveAnimation(Type, Entry)) { ResolvedAnimations.Add(Entry); } }
		if (!FMath::IsFinite(Compact.AirLoopTransitionTime) || Compact.AirLoopTransitionTime < 0.f || ResolvedAnimations.IsEmpty())
		{ Errors.Add(FText::FromString(TEXT("Compact animations require at least one supported path and a nonnegative finite airborne transition."))); }
		for (const auto& Family : {Compact.Fly, Compact.Push})
		{
			if ((Family.Start || Family.Loop || Family.Landing) && (!Family.Start || !Family.Loop || !Family.Landing))
			{ Errors.Add(FText::FromString(TEXT("Configured Fly/Push family requires Start, Loop and Landing together."))); }
		}
	}
	else { ResolvedAnimations = Animations; }
	for (const auto& Entry : ResolvedAnimations)
	{
		if (Types.Contains(Entry.Type)) { Errors.Add(FText::FromString(TEXT("Duplicate reaction animation type."))); }
		Types.Add(Entry.Type);
		ValidateAnimationEntry(Entry, Errors);
	}
	for (auto Type : FallbackToHitStun)
	{
		if (uint8(Type) > uint8(EHodgeImpactType::Slam) || Type == EHodgeImpactType::HitStun || !SupportsImpact(EHodgeImpactType::HitStun))
		{ Errors.Add(FText::FromString(TEXT("Fallback requires a valid effect type and a configured HitStun entry."))); }
	}
	if (!FMath::IsFinite(DownedDuration) || DownedDuration <= 0.f || !FMath::IsFinite(MaxControlDuration) || MaxControlDuration <= 0.f ||
		!FMath::IsFinite(MaxAirborneDuration) || MaxAirborneDuration <= 0.f || !FMath::IsFinite(MaxLaunchHeight) || MaxLaunchHeight <= 0.f ||
		!FMath::IsFinite(MaxLaunchSpeed) || MaxLaunchSpeed <= 0.f || MaxAirHits < 0)
	{ Errors.Add(FText::FromString(TEXT("Reaction bounds must be finite and positive; MaxAirHits must be nonnegative."))); }
	float Peak = 0.f;
	if (!FMath::IsFinite(ReplacementBlendOut) || ReplacementBlendOut < 0.f ||
		(!bUseCompactAnimations && (!FMath::IsFinite(GroundReplayInterval) || GroundReplayInterval < 0.f)) ||
		GetKnockbackCurveIntegral(Peak) <= UE_KINDA_SMALL_NUMBER)
	{ Errors.Add(FText::FromString(TEXT("Reaction replacement blend must be finite and nonnegative; knockback curve must be finite, nonnegative and have positive area on [0,1]."))); }
	if (LightFeedbackMontage && LightFeedbackMontage->HasRootMotion())
	{ Errors.Add(FText::FromString(TEXT("Light feedback cannot contain root motion."))); }
	TSet<EHodgeHitPresentationDirection> Directions;
	for (const auto& Entry : DirectionalLightFeedback)
	{
		if (uint8(Entry.Direction) > uint8(EHodgeHitPresentationDirection::Right) || Directions.Contains(Entry.Direction) ||
			!Entry.Montage || Entry.Montage->HasRootMotion() || !Entry.Montage->IsValidAdditive() || Entry.Montage->GetPlayLength() <= 0.f)
		{ Errors.Add(FText::FromString(TEXT("Directional feedback requires unique directions and valid in-place additive montages."))); }
		Directions.Add(Entry.Direction);
	}
	return Before == Errors.Num();
}

#if WITH_EDITOR
/** 在编辑器校验当前配置并收集诊断。 */
EDataValidationResult UHodgeHitReactionProfile::IsDataValid(FDataValidationContext& Context) const
{
	const auto Parent = Super::IsDataValid(Context);
	TArray<FText> Errors;
	Validate(Errors);
	for (const auto& Error : Errors) { Context.AddError(Error); }
	return Errors.IsEmpty() && Parent != EDataValidationResult::Invalid ? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}
#endif
