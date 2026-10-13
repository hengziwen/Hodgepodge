/** @file HodgeEquipmentStatProfile.cpp
 * @brief 实现装备等级属性配置相关的类型、接口与现有处理流程。
 */
#include "Data/HodgeEquipmentStatProfile.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/Stats/HodgeEquipmentStatEffect.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEquipmentStatProfile)

/** 构造装备等级属性配置并初始化其默认状态。 */
UHodgeEquipmentStatProfile::UHodgeEquipmentStatProfile()
{
	AttributeEffect = UHodgeEquipmentStatEffect::StaticClass();
}

/** 计算当前配置结果。 */
bool UHodgeEquipmentStatProfile::Evaluate(int32 Level, float& Health, float& Damage) const
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Level < MinLevel || Level > MaxLevel || !MaxHealthBonus.IsValid() || !BaseDamageBonus.IsValid()) { return false; }
	Health = MaxHealthBonus.GetValueAtLevel(Level);
	Damage = BaseDamageBonus.GetValueAtLevel(Level);
	return FMath::IsFinite(Health) && Health >= 0.f && FMath::IsFinite(Damage) && Damage >= 0.f;
}

/** 检查当前配置并收集诊断。 */
bool UHodgeEquipmentStatProfile::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	if (MinLevel < 1 || MaxLevel < MinLevel || MaxLevel > 1000) { Errors.Add(FText::FromString(TEXT("Invalid equipment stat level range"))); }
	const auto* Effect = AttributeEffect ? AttributeEffect->GetDefaultObject<UGameplayEffect>() : nullptr;
	if (!Effect || Effect->DurationPolicy != EGameplayEffectDurationType::Infinite || Effect->Period.GetValueAtLevel(1) != 0.f
		|| Effect->Modifiers.Num() != 2 || !Effect->Executions.IsEmpty() || Effect->StackingType != EGameplayEffectStackingType::None)
	{ Errors.Add(FText::FromString(TEXT("Equipment stat effect requires independent nonperiodic Infinite modifiers"))); }
	else
	{
		for (int32 Index = 0; Index < 2; ++Index)
		{
			const auto& Mod = Effect->Modifiers[Index];
			const auto Attribute = Index == 0 ? UHodgeHealthSet::GetMaxHealthAttribute() : UHodgeCombatSet::GetBaseDamageAttribute();
			const FGameplayTag Tag = Index == 0 ? FGameplayTag(HodgeGameplayTags::SetByCaller_Stat_MaxHealth) : FGameplayTag(HodgeGameplayTags::SetByCaller_Stat_BaseDamage);
			if (Mod.Attribute != Attribute || Mod.ModifierOp != EGameplayModOp::Additive || Mod.ModifierMagnitude.GetMagnitudeCalculationType() != EGameplayEffectMagnitudeCalculation::SetByCaller
				|| Mod.ModifierMagnitude.GetSetByCallerFloat().DataTag != Tag || !Mod.ModifierMagnitude.GetSetByCallerFloat().DataName.IsNone())
			{ Errors.Add(FText::FromString(TEXT("Invalid equipment stat modifier contract"))); }
		}
	}
	for (int32 Level = FMath::Max(1, MinLevel); Level <= FMath::Min(1000, MaxLevel); ++Level)
	{
		float Health, Damage;
		if (!Evaluate(Level, Health, Damage)) { Errors.Add(FText::FromString(TEXT("Equipment stat curve has an invalid value"))); break; }
	}
	return Errors.Num() == Before;
}
#if WITH_EDITOR
/** 在编辑器校验当前配置并收集诊断。 */
EDataValidationResult UHodgeEquipmentStatProfile::IsDataValid(FDataValidationContext& Context) const
{
	TArray<FText> Errors;
	Validate(Errors);
	for (const auto& Error : Errors) { Context.AddError(Error); }
	return Errors.IsEmpty() ? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}
#endif
