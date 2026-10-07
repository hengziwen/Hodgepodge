#include "Data/HodgeCharacterStatProfile.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/Stats/HodgeCharacterBaseStatEffect.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCharacterStatProfile)

UHodgeCharacterStatProfile::UHodgeCharacterStatProfile()
{
	InitializationEffect = UHodgeCharacterBaseStatEffect::StaticClass();
}

bool UHodgeCharacterStatProfile::Evaluate(int32 Level, float& Health, float& Damage) const
{
	if (Level < MinLevel || Level > MaxLevel || !MaxHealth.IsValid() || !BaseDamage.IsValid()) { return false; }
	Health = MaxHealth.GetValueAtLevel(Level);
	Damage = BaseDamage.GetValueAtLevel(Level);
	return FMath::IsFinite(Health) && Health > 0.f && FMath::IsFinite(Damage) && Damage >= 0.f;
}

bool UHodgeCharacterStatProfile::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	if (MinLevel < 1 || MaxLevel < MinLevel || MaxLevel > 1000 || ConfigurationVersion < 1)
	{ Errors.Add(FText::FromString(TEXT("Invalid character stat level range/version"))); }
	const auto* Effect = InitializationEffect ? InitializationEffect->GetDefaultObject<UGameplayEffect>() : nullptr;
	if (!Effect || Effect->DurationPolicy != EGameplayEffectDurationType::Instant || Effect->Modifiers.Num() != 2 || !Effect->Executions.IsEmpty())
	{ Errors.Add(FText::FromString(TEXT("Character stat effect requires two Instant SetByCaller Override modifiers"))); }
	else
	{
		for (int32 Index = 0; Index < 2; ++Index)
		{
			const auto& Mod = Effect->Modifiers[Index];
			const auto Attribute = Index == 0 ? UHodgeHealthSet::GetMaxHealthAttribute() : UHodgeCombatSet::GetBaseDamageAttribute();
			const FGameplayTag Tag = Index == 0 ? FGameplayTag(HodgeGameplayTags::SetByCaller_Stat_MaxHealth) : FGameplayTag(HodgeGameplayTags::SetByCaller_Stat_BaseDamage);
			if (Mod.Attribute != Attribute || Mod.ModifierOp != EGameplayModOp::Override || Mod.ModifierMagnitude.GetMagnitudeCalculationType() != EGameplayEffectMagnitudeCalculation::SetByCaller
				|| Mod.ModifierMagnitude.GetSetByCallerFloat().DataTag != Tag || !Mod.ModifierMagnitude.GetSetByCallerFloat().DataName.IsNone())
			{ Errors.Add(FText::FromString(TEXT("Invalid character stat modifier contract"))); }
		}
	}
	for (int32 Level = FMath::Max(1, MinLevel); Level <= FMath::Min(1000, MaxLevel); ++Level)
	{
		float Health, Damage;
		if (!Evaluate(Level, Health, Damage)) { Errors.Add(FText::FromString(TEXT("Character stat curve has an invalid value"))); break; }
	}
	return Errors.Num() == Before;
}
#if WITH_EDITOR
EDataValidationResult UHodgeCharacterStatProfile::IsDataValid(FDataValidationContext& Context) const
{
	TArray<FText> Errors;
	Validate(Errors);
	for (const auto& Error : Errors) { Context.AddError(Error); }
	return Errors.IsEmpty() ? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}
#endif
