/** @file HodgeHavocEffects.cpp @brief 分离持续增伤与周期消耗，避免每周期重复增益。 */
#include "AbilitySystem/Effects/HodgeHavocEffects.h"
#include "AbilitySystem/AttributeSet/HodgeHavocSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHavocEffects)

UHodgeDarkFlowEffect::UHodgeDarkFlowEffect()
{
	DurationPolicy=EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo Modifier;
	Modifier.Attribute=UHodgeHavocSet::GetDarkFlowAttribute(); Modifier.ModifierOp=EGameplayModOp::Additive;
	FSetByCallerFloat Delta; Delta.DataTag=HodgeHavocTags::DarkFlowDelta;
	Modifier.ModifierMagnitude=FGameplayEffectModifierMagnitude(Delta); Modifiers.Add(Modifier);
}

UHodgeDarkFlowInitialization::UHodgeDarkFlowInitialization()
{
	DurationPolicy=EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo Modifier;
	Modifier.Attribute=UHodgeHavocSet::GetMaxDarkFlowAttribute(); Modifier.ModifierOp=EGameplayModOp::Override;
	Modifier.ModifierMagnitude=FGameplayEffectModifierMagnitude(FScalableFloat(100.f)); Modifiers.Add(Modifier);
}

UHodgeDarkSurgeEffect::UHodgeDarkSurgeEffect()
{
	DurationPolicy=EGameplayEffectDurationType::Infinite;
	auto* Tags=CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("DarkSurgeTags"));
	FInheritedTagContainer Changes; Changes.Added.AddTag(HodgeHavocTags::DarkSurge);
	Tags->SetAndApplyTargetTagChanges(Changes); GEComponents.Add(Tags);
	FGameplayModifierInfo Modifier;
	Modifier.Attribute=UHodgeCombatSet::GetHavocDamageBonusAttribute(); Modifier.ModifierOp=EGameplayModOp::Additive;
	Modifier.ModifierMagnitude=FGameplayEffectModifierMagnitude(FScalableFloat(.20f)); Modifiers.Add(Modifier);
}

UHodgeDarkFlowDrainEffect::UHodgeDarkFlowDrainEffect()
{
	DurationPolicy=EGameplayEffectDurationType::Infinite; Period=FScalableFloat(.1f);
	bExecutePeriodicEffectOnApplication=false;
	FGameplayEffectExecutionDefinition Execution; Execution.CalculationClass=UHodgeDarkFlowDrainExecution::StaticClass();
	Executions.Add(Execution);
}

void UHodgeDarkFlowDrainExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& Parameters,
	FGameplayEffectCustomExecutionOutput& Output) const
{
	auto* Rules=Cast<UHodgeGameplayAbility_HavocRules>(Parameters.GetOwningSpec().GetContext().GetSourceObject());
	if (!Rules || Parameters.GetTargetAbilitySystemComponent()!=Rules->GetAbilitySystemComponentFromActorInfo()) { return; }
	const float Amount=Rules->SettleFuel(Parameters.GetOwningSpec());
	if (Amount>0.f) { Output.AddOutputModifier(FGameplayModifierEvaluatedData(UHodgeHavocSet::GetDarkFlowAttribute(),EGameplayModOp::Additive,-Amount)); }
}
