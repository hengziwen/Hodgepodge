#include "Data/HodgeAbilityDefinition.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Animation/AnimMontage.h"
#include "Data/HodgeAbilityTimeline.h"
#include "AbilitySystem/HodgeTimelineEvaluator.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAbilityDefinition)

FAlphaBlend FHodgeAbilityBlendSettings::MakeBlend() const
{
	FAlphaBlend Result;
	Result.SetBlendTime(Time);
	Result.SetBlendOption(Curve);
	Result.SetCustomCurve(CustomCurve);
	return Result;
}

bool FHodgeAbilityBlendSettings::IsValid() const
{
	return FMath::IsFinite(Time) && Time >= 0.f
		&& Mode == EMontageBlendMode::Standard
		&& (Curve != EAlphaBlendOption::Custom || CustomCurve != nullptr);
}

float UHodgeAbilityDefinition::GetDuration() const
{
	return ExecutionConfig.Montage ? ExecutionConfig.Montage->GetPlayLength() : 0.f;
}

bool UHodgeAbilityDefinition::ValidateDefinition(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	auto Error = [&Errors](const FString& Message)
	{
		Errors.Add(FText::FromString(Message));
	};
	if (!AbilityTag.IsValid() || !AbilityClass)
	{
		Error(TEXT("AbilityTag / AbilityClass is required"));
	}
	if (AbilityClass && !AbilityClass->IsChildOf(UHodgeGameplayAbility_Definition::StaticClass()))
	{
		Error(TEXT("AbilityClass must derive from HodgeGameplayAbility_Definition"));
	}
	if (AbilityClass)
	{
		const auto* Ability = AbilityClass->GetDefaultObject<UHodgeGameplayAbility>();
		if (Ability->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::InstancedPerActor
			|| Ability->GetNetExecutionPolicy() != EGameplayAbilityNetExecutionPolicy::LocalPredicted)
		{
			Error(TEXT("Definition abilities require InstancedPerActor and LocalPredicted"));
		}
	}
	const auto& C = ExecutionConfig;
	if (!C.Montage || !C.TimelineTaskConfig.Timeline)
	{
		Error(TEXT("Montage / Timeline is required"));
	}
	if (C.TimelineTaskConfig.Timeline && !C.TimelineTaskConfig.Timeline->bUseMontageDuration)
	{
		Error(TEXT("Definition Timeline must enable Use Montage Duration"));
	}
	if (!FMath::IsFinite(C.PlayRate) || C.PlayRate <= 0.f)
	{
		Error(TEXT("PlayRate must be finite and positive"));
	}
	if (!C.BlendIn.IsValid() || !C.NaturalBlendOut.IsValid() || !C.StopBlendOut.IsValid())
	{
		Error(
			TEXT("Invalid blend settings: V1 requires Standard mode, finite nonnegative time, and a curve for Custom"));
	}
	if (C.Montage)
	{
		if (!FMath::IsFinite(GetDuration()) || GetDuration() <= 0.f)
		{
			Error(TEXT("Montage length must be finite and positive"));
		}
		if (C.Montage->BlendModeIn != EMontageBlendMode::Standard)
		{
			Error(TEXT("V1 requires a Standard Montage BlendModeIn"));
		}
		// 第一版只允许单个线性 Section，不接受循环或跳段图。
		if (C.Montage->CompositeSections.Num() != 1 || C.Montage->CompositeSections[0].NextSectionName != NAME_None)
		{
			Error(TEXT("V1 requires one non-looping Montage section"));
		}
		if (!FMath::IsFinite(C.Montage->RateScale) || C.Montage->RateScale <= 0.f)
		{
			Error(TEXT("Montage RateScale must be finite and positive"));
		}
		if (C.TimelineTaskConfig.Timeline)
		{
			C.TimelineTaskConfig.Timeline->ValidateForPlayback(Errors, GetDuration());
		}
	}
	TSet<FGameplayTag> WindowTags;
	TSet<FGameplayTag> PointTags;
	TMap<FName, float> GroupIntervals;
	const UHodgeAbilityTimeline* Timeline = C.TimelineTaskConfig.Timeline;
	if (ExecutionRoute == EHodgeAbilityExecutionRoute::ComboCoordinated && InputTag.IsValid())
	{
		Error(TEXT("Combo definitions receive input through their graph; InputTag is for Standalone only."));
	}
	if (WeaponUseWindowTag.IsValid() && Timeline && !Timeline->Events.ContainsByPredicate([this](const auto& Event)
		{ return Event.Kind == EHodgeTimelineEventKind::Window && Event.WindowTag == WeaponUseWindowTag; }))
	{
		Error(TEXT("WeaponUseWindowTag requires a matching Timeline window"));
	}
	auto ValidateHit = [&](const FHodgeHitEffectConfig& Binding)
	{
		const bool bNeedsSource = Binding.TargetPolicy != EHodgeHitTargetPolicy::ConfirmedTarget &&
			(Binding.Volume.GeometryMode == EHodgeHitGeometryMode::ExistingSource || Binding.Volume.AnchorKind == EHodgeHitAnchorKind::RegisteredSource);
		if (!Binding.Profile || (bNeedsSource && !Binding.SourceTag.IsValid())) { Error(TEXT("Hit binding requires Profile and a source tag when using a registered component.")); }
		Binding.Volume.Validate(Errors);
		if (Binding.Profile)
		{
			Binding.Profile->Validate(Errors);
			const bool bShapeStrategy = Binding.Profile->Strategy && Binding.Profile->Strategy->IsChildOf(UHodgeShapeQueryStrategy::StaticClass());
			if (Binding.TargetPolicy != EHodgeHitTargetPolicy::ConfirmedTarget &&
				bShapeStrategy != (Binding.Volume.GeometryMode == EHodgeHitGeometryMode::ConfiguredShape))
			{
				Error(TEXT("ConfiguredShape requires ShapeQueryStrategy; ExistingSource requires its component strategy."));
			}
			if (Binding.Volume.GeometryMode == EHodgeHitGeometryMode::ExistingSource &&
				Binding.Profile->QueryMode != EHodgeHitQueryMode::Sweep && Binding.TargetPolicy != EHodgeHitTargetPolicy::ConfirmedTarget)
			{
				Error(TEXT("Legacy component strategies use Sweep; configure a shape for Overlap."));
			}
		}
		if (!FMath::IsFinite(Binding.DamageMultiplier) || Binding.DamageMultiplier < 0.f ||
			!FMath::IsFinite(Binding.RepeatHitInterval) || Binding.RepeatHitInterval < 0.f ||
			!FMath::IsFinite(Binding.MaxTargetDistance) || Binding.MaxTargetDistance <= 0.f)
		{
			Error(TEXT("Hit multipliers, intervals and target distance must be finite and valid."));
		}
		if ((Binding.TargetPolicy != EHodgeHitTargetPolicy::AnyInVolume ||
			(Binding.Volume.GeometryMode == EHodgeHitGeometryMode::ConfiguredShape && Binding.Volume.AnchorKind == EHodgeHitAnchorKind::ExecutionTarget)) && Binding.TargetKey.IsNone())
		{
			Error(TEXT("A locked or confirmed target requires a server-populated TargetKey."));
		}
		if (!Binding.HitGroup.IsNone())
		{
			const float* Interval = GroupIntervals.Find(Binding.HitGroup);
			if (Interval && *Interval != Binding.RepeatHitInterval) { Error(TEXT("Shared hit groups require the same repeat interval")); }
			GroupIntervals.Add(Binding.HitGroup, Binding.RepeatHitInterval);
		}
	};
	auto CheckHand = [&](const FHodgeHitEffectConfig& Binding, const FHodgeTimelineEvent& Hit)
	{
		if (!Binding.RequiresWeaponInHand || !WeaponUseWindowTag.IsValid() || !Timeline) { return; }
		const bool bCovered = Timeline->Events.ContainsByPredicate([&](const FHodgeTimelineEvent& Use)
		{
			const float UseEnd = FHodgeTimelineEvaluator::WindowEnd(Use, GetDuration());
			const bool bEndCovered = Hit.Kind == EHodgeTimelineEventKind::Point ? Hit.StartTime < UseEnd : FHodgeTimelineEvaluator::WindowEnd(Hit, GetDuration()) <= UseEnd;
			return Use.Kind == EHodgeTimelineEventKind::Window && Use.WindowTag == WeaponUseWindowTag &&
				Use.StartTime <= Hit.StartTime && bEndCovered &&
				(Hit.Kind == EHodgeTimelineEventKind::Point || Use.StartTime < Hit.StartTime || Use.Priority < Hit.Priority);
		});
		if (!bCovered) { Error(TEXT("Required hand-use window must cover the hit occurrence and enter before sampling.")); }
	};
	for (const auto& Binding : HitWindows)
	{
		ValidateHit(Binding);
		if (!Binding.WindowTag.IsValid() || WindowTags.Contains(Binding.WindowTag)) { Error(TEXT("Hit window requires a unique WindowTag.")); }
		WindowTags.Add(Binding.WindowTag);
		bool bMatched = false;
		if (Timeline) { for (const auto& Event : Timeline->Events)
		{
			if (Event.Kind == EHodgeTimelineEventKind::Window && Event.WindowTag == Binding.WindowTag) { bMatched = true; CheckHand(Binding, Event); }
		} }
		if (!bMatched) { Error(TEXT("Hit window binding has no matching Timeline window")); }
	}
	for (const auto& Binding : HitPoints)
	{
		ValidateHit(Binding);
		if (!Binding.PointEventTag.IsValid() || PointTags.Contains(Binding.PointEventTag)) { Error(TEXT("Hit point requires a unique PointEventTag.")); }
		PointTags.Add(Binding.PointEventTag);
		bool bMatched = false;
		if (Timeline) { for (const auto& Event : Timeline->Events)
		{
			if (Event.Kind != EHodgeTimelineEventKind::Point || Event.PointEventTag != Binding.PointEventTag) { continue; }
			bMatched = true;
			if (Event.NetPolicy == EHodgeTimelineEventNetPolicy::LocallyControlledOnly || Event.PointEffectClass)
			{
				Error(TEXT("Hit points must reach authority and cannot combine a source PointEffectClass; use binding DamageEffect."));
			}
			CheckHand(Binding, Event);
		} }
		if (!bMatched) { Error(TEXT("Hit point binding has no matching Timeline point.")); }
	}
	// 能力自行校验效果契约，通用检测配置不限定 GE 的执行方式。
	if (AbilityClass && AbilityClass->IsChildOf(UHodgeGameplayAbility_Definition::StaticClass()))
	{
		AbilityClass->GetDefaultObject<UHodgeGameplayAbility_Definition>()->ValidateExecutionConfiguration(*this, Errors);
	}
	return Before == Errors.Num();
}
#if WITH_EDITOR
EDataValidationResult UHodgeAbilityDefinition::IsDataValid(FDataValidationContext& Context) const
{
	const auto Parent = Super::IsDataValid(Context);
	TArray<FText> Errors;
	ValidateDefinition(Errors);
	for (const FText& Error : Errors)
	{
		Context.AddError(Error);
	}
	return Errors.IsEmpty() && Parent != EDataValidationResult::Invalid
		       ? EDataValidationResult::Valid
		       : EDataValidationResult::Invalid;
}
#endif
