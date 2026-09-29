#include "Data/HodgeAbilityDefinition.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Animation/AnimMontage.h"
#include "Data/HodgeAbilityTimeline.h"
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
	auto Error = [&Errors](const FString& Message) { Errors.Add(FText::FromString(Message)); };
	if (!AbilityTag.IsValid() || !AbilityClass) { Error(TEXT("AbilityTag / AbilityClass is required")); }
	if (AbilityClass && !AbilityClass->IsChildOf(UHodgeGameplayAbility_Definition::StaticClass()))
	{ Error(TEXT("AbilityClass must derive from HodgeGameplayAbility_Definition")); }
	if (AbilityClass)
	{
		const auto* Ability = AbilityClass->GetDefaultObject<UHodgeGameplayAbility>();
		if (Ability->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::InstancedPerActor
			|| Ability->GetNetExecutionPolicy() != EGameplayAbilityNetExecutionPolicy::LocalPredicted)
		{ Error(TEXT("Definition abilities require InstancedPerActor and LocalPredicted")); }
	}
	const auto& C = ExecutionConfig;
	if (!C.Montage || !C.TimelineTaskConfig.Timeline) { Error(TEXT("Montage / Timeline is required")); }
	if (C.TimelineTaskConfig.Timeline && !C.TimelineTaskConfig.Timeline->bUseMontageDuration)
	{ Error(TEXT("Definition Timeline must enable Use Montage Duration")); }
	if (!FMath::IsFinite(C.PlayRate) || C.PlayRate <= 0.f) { Error(TEXT("PlayRate must be finite and positive")); }
	if (!C.BlendIn.IsValid() || !C.NaturalBlendOut.IsValid() || !C.StopBlendOut.IsValid())
	{ Error(TEXT("Invalid blend settings: V1 requires Standard mode, finite nonnegative time, and a curve for Custom")); }
	if (C.Montage)
	{
		if (!FMath::IsFinite(GetDuration()) || GetDuration() <= 0.f) { Error(TEXT("Montage length must be finite and positive")); }
		if (C.Montage->BlendModeIn != EMontageBlendMode::Standard) { Error(TEXT("V1 requires a Standard Montage BlendModeIn")); }
		// 第一版只允许单个线性 Section，不接受循环或跳段图。
		if (C.Montage->CompositeSections.Num() != 1 || C.Montage->CompositeSections[0].NextSectionName != NAME_None)
		{ Error(TEXT("V1 requires one non-looping Montage section")); }
		if (!FMath::IsFinite(C.Montage->RateScale) || C.Montage->RateScale <= 0.f)
		{ Error(TEXT("Montage RateScale must be finite and positive")); }
		if (C.TimelineTaskConfig.Timeline)
		{ C.TimelineTaskConfig.Timeline->ValidateForPlayback(Errors, GetDuration()); }
	}
	return Before == Errors.Num();
}
#if WITH_EDITOR
EDataValidationResult UHodgeAbilityDefinition::IsDataValid(FDataValidationContext& Context) const
{
	const auto Parent = Super::IsDataValid(Context);
	TArray<FText> Errors;
	ValidateDefinition(Errors);
	for (const FText& Error : Errors) { Context.AddError(Error); }
	return Errors.IsEmpty() && Parent != EDataValidationResult::Invalid ? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}
#endif
