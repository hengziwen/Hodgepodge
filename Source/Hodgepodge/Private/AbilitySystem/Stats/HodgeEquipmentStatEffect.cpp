#include "AbilitySystem/Stats/HodgeEquipmentStatEffect.h"
#include "AbilitySystem/HodgeGameplayTags.h"

#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEquipmentStatEffect)

UHodgeEquipmentStatEffect::UHodgeEquipmentStatEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	FGameplayModifierInfo Health;
	Health.Attribute = UHodgeHealthSet::GetMaxHealthAttribute();
	Health.ModifierOp = EGameplayModOp::Additive;
	FSetByCallerFloat HealthValue;
	HealthValue.DataTag = HodgeGameplayTags::SetByCaller_Stat_MaxHealth;
	Health.ModifierMagnitude = FGameplayEffectModifierMagnitude(HealthValue);
	Modifiers.Add(Health);
	FGameplayModifierInfo Damage;
	Damage.Attribute = UHodgeCombatSet::GetBaseDamageAttribute();
	Damage.ModifierOp = EGameplayModOp::Additive;
	FSetByCallerFloat DamageValue;
	DamageValue.DataTag = HodgeGameplayTags::SetByCaller_Stat_BaseDamage;
	Damage.ModifierMagnitude = FGameplayEffectModifierMagnitude(DamageValue);
	Modifiers.Add(Damage);
}
