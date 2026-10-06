#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/Stats/HodgeCharacterBaseStatEffect.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCharacterBaseStatEffect)

UHodgeCharacterBaseStatEffect::UHodgeCharacterBaseStatEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo Health;
	Health.Attribute = UHodgeHealthSet::GetMaxHealthAttribute();
	Health.ModifierOp = EGameplayModOp::Override;
	FSetByCallerFloat HealthValue;
	HealthValue.DataTag = HodgeGameplayTags::SetByCaller_Stat_MaxHealth;
	Health.ModifierMagnitude = FGameplayEffectModifierMagnitude(HealthValue);
	Modifiers.Add(Health);
	FGameplayModifierInfo Damage;
	Damage.Attribute = UHodgeCombatSet::GetBaseDamageAttribute();
	Damage.ModifierOp = EGameplayModOp::Override;
	FSetByCallerFloat DamageValue;
	DamageValue.DataTag = HodgeGameplayTags::SetByCaller_Stat_BaseDamage;
	Damage.ModifierMagnitude = FGameplayEffectModifierMagnitude(DamageValue);
	Modifiers.Add(Damage);
}
