/** @file HodgeLegacyAttackRuleAdapter.cpp @brief 兼容旧配置而不维护第二套收益执行逻辑。 */
#include "Combat/HodgeLegacyAttackRuleAdapter.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Data/HodgeHavocAttackRule.h"
#include "Data/HodgeResonanceAttackRule.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/Effects/HodgeResonanceEnergyEffect.h"
#include "UObject/StrongObjectPtr.h"

namespace
{
	/** 只为缺少新配置的旧字段构造临时配置，所有处理仍走同一规则类。 */
	TArray<TStrongObjectPtr<UHodgeAttackRuleConfig>> MakeLegacyConfigs(const UHodgeAbilityDefinition& Data)
	{
		TArray<TStrongObjectPtr<UHodgeAttackRuleConfig>> Result;
		bool bHasHavoc = false, bHasResonance = false;
		for (const UHodgeAttackRuleConfig* Rule : Data.AttackRules)
		{ bHasHavoc |= Rule && Rule->IsA<UHodgeHavocAttackRuleConfig>(); bHasResonance |= Rule && Rule->IsA<UHodgeResonanceAttackRuleConfig>(); }
		if (!bHasResonance && (Data.ResonanceEnergyGainPerPhase.GetValueAtLevel(1.f) != 0.f || Data.ResonanceEnergyGainPerPhase.Curve.CurveTable))
		{
			auto* Rule = NewObject<UHodgeResonanceAttackRuleConfig>(GetTransientPackage());
			Rule->GainPerPhase = Data.ResonanceEnergyGainPerPhase; Rule->GainEffect = Data.ResonanceEnergyGainEffect;
			Result.Emplace(Rule);
		}
		if (!bHasHavoc && (Data.DarkFlowGainPerPhase != 0.f || Data.DarkFlowGainOnCast != 0.f || Data.bEnhancedNormalAttack))
		{
			auto* Rule = NewObject<UHodgeHavocAttackRuleConfig>(GetTransientPackage());
			Rule->DarkFlowGainPerPhase = Data.DarkFlowGainPerPhase; Rule->DarkFlowGainOnCast = Data.DarkFlowGainOnCast;
			Rule->bEnhancedNormalAttack = Data.bEnhancedNormalAttack; Result.Emplace(Rule);
		}
		return Result;
	}
}

/** 旧类默认值沿用原先 GE，避免未迁移的资产／夹具改变行为。 */
void HodgeLegacyAttackRules::InitializeDefaults(UHodgeAbilityDefinition& Definition)
{ Definition.ResonanceEnergyGainEffect = UHodgeResonanceEnergyEffect::StaticClass(); }

/** 优先共鸣再暗流，保持旧收益广播顺序；规则配置由执行对象持有。 */
void HodgeLegacyAttackRules::CreateRules(const UHodgeAbilityDefinition& Definition, UHodgeGameplayAbility_Definition* Source, TArray<TObjectPtr<UHodgeAttackRule>>& OutRules)
{
	for (const auto& Config : MakeLegacyConfigs(Definition))
	{
		if (auto* Rule = Config->CreateRule(Source)) { OutRules.Add(Rule); Rule->Initialize(Source, Config.Get()); }
	}
}

/** 迁移前后的验证复用同一配置校验函数。 */
void HodgeLegacyAttackRules::Validate(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors)
{ for (const auto& Config : MakeLegacyConfigs(Definition)) { Config->ValidateRule(Definition, Errors); } }
