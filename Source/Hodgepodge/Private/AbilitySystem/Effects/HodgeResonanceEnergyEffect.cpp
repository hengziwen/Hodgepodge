/** @file HodgeResonanceEnergyEffect.cpp @brief 初始化共享增量修饰器，不直接写客户端属性。 */
#include "AbilitySystem/Effects/HodgeResonanceEnergyEffect.h"
#include "AbilitySystem/AttributeSet/HodgeResonanceSet.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeResonanceEnergyEffect)

UHodgeResonanceEnergyEffect::UHodgeResonanceEnergyEffect()
{
	DurationPolicy=EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo Modifier;
	Modifier.Attribute=UHodgeResonanceSet::GetResonanceEnergyAttribute(); Modifier.ModifierOp=EGameplayModOp::Additive;
	FSetByCallerFloat Delta; Delta.DataTag=HodgeResonanceTags::EnergyDelta;
	Modifier.ModifierMagnitude=FGameplayEffectModifierMagnitude(Delta); Modifiers.Add(Modifier);
}

UHodgeResonanceEnergyInitialization::UHodgeResonanceEnergyInitialization()
{
	DurationPolicy=EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo Modifier;
	Modifier.Attribute=UHodgeResonanceSet::GetMaxResonanceEnergyAttribute(); Modifier.ModifierOp=EGameplayModOp::Override;
	Modifier.ModifierMagnitude=FGameplayEffectModifierMagnitude(FScalableFloat(100.f)); Modifiers.Add(Modifier);
}
