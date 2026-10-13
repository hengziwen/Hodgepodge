/** @file HodgeEquipmentStatEffect.cpp
 * @brief 实现装备属性效果相关的类型、接口与现有处理流程。
 */
#include "AbilitySystem/Stats/HodgeEquipmentStatEffect.h"
#include "AbilitySystem/HodgeGameplayTags.h"

#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEquipmentStatEffect)

/** 构造装备属性效果并初始化其默认状态。 */
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
