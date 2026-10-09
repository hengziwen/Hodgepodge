#include "Data/HodgeAbilityDefinition.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Animation/AnimMontage.h"
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
	DefaultHitConfig.Reaction.Validate(Errors);
	if (ExecutionBodyTag.IsValid() && HodgeHitReaction::BodyRank(ExecutionBodyTag) == 0)
	{ Error(TEXT("ExecutionBodyTag must be an exact State.Combat.Body leaf tag.")); }
	const auto& C = ExecutionConfig;
	if (!C.Montage) { Error(TEXT("Montage is required")); }
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
		if (!FMath::IsFinite(C.Montage->RateScale) || C.Montage->RateScale <= 0.f)
		{
			Error(TEXT("Montage RateScale must be finite and positive"));
		}
	}
	if (ExecutionRoute == EHodgeAbilityExecutionRoute::ComboCoordinated && InputTag.IsValid()) { Error(TEXT("InputTag is for Standalone definitions only.")); }
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
