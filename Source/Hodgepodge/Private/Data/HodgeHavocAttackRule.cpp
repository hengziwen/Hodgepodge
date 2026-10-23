/** @file HodgeHavocAttackRule.cpp @brief 专属收益离开公共 Melee，模式与角色限流继续复用 HavocRules。 */
#include "Data/HodgeHavocAttackRule.h"
#include "Data/HodgeAbilityDefinition.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h"
#include "AbilitySystem/AttributeSet/HodgeHavocSet.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "Animation/AnimMontage.h"
#include "AbilitySystemComponent.h"
#include "UObject/StrongObjectPtr.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHavocAttackRule)

/** 按执行创建独立收益账本。 */
UHodgeAttackRule* UHodgeHavocAttackRuleConfig::CreateRule(UObject* Outer) const { return NewObject<UHodgeHavocAttackRule>(Outer); }
/** 施放收益必须有且仅有一个明确提交通知。 */
void UHodgeHavocAttackRuleConfig::ValidateRule(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const
{
	if (!FMath::IsFinite(DarkFlowGainPerPhase) || DarkFlowGainPerPhase < 0.f || !FMath::IsFinite(DarkFlowGainOnCast) || DarkFlowGainOnCast < 0.f)
	{ Errors.Add(FText::FromString(TEXT("DarkFlow gains must be finite and nonnegative"))); }
	if (DarkFlowGainOnCast > 0.f)
	{
		int32 Count = 0;
		if (Definition.ExecutionConfig.Montage) for (const auto& Event : Definition.ExecutionConfig.Montage->Notifies)
		{ if (const auto* Notify = Cast<UHodgeAnimNotify_GameplayEvent>(Event.Notify); Notify && Notify->EventTag == HodgeHavocTags::CastCommitted) { ++Count; } }
		if (Count != 1) { Errors.Add(FText::FromString(TEXT("Cast DarkFlow gain requires one GameplayEvent.Attack.CastCommitted notify"))); }
	}
}
/** 阶段收益和角色级限流分开；只把实际接受的伤害交给既有宿主。 */
void UHodgeHavocAttackRule::OnSettlement(const FHodgeAttackSettlement& Settlement)
{
	if (!Accepts(Settlement) || Settlement.AcceptedDamage <= 0.f) { return; }
	const auto* Data = Cast<UHodgeHavocAttackRuleConfig>(Config);
	auto* ASC = Source->GetAbilitySystemComponentFromActorInfo();
	TStrongObjectPtr<UHodgeGameplayAbility_HavocRules> Rules(UHodgeGameplayAbility_HavocRules::Find(ASC));
	if (!Data || !Rules.Get()) { return; }
	if (!ASC->HasMatchingGameplayTag(HodgeHavocTags::DarkSurge) && Data->DarkFlowGainPerPhase > 0.f &&
		Phases.TryReserve(Settlement.HitGroup, Settlement.AttackPhase, Settlement.AcceptedDamage))
	{ Rules->ApplyDarkFlowGain(Source.Get(), Data->DarkFlowGainPerPhase); }
	if (IsCurrent() && Data->bEnhancedNormalAttack) { Rules->HandleEnhancedNormalHit(Source.Get(), Settlement.AcceptedDamage); }
}
/** 空挥提交可回暗流，重复通知和提交前取消都不会额外回能。 */
void UHodgeHavocAttackRule::OnExecutionEvent(FGameplayTag Tag)
{
	if (!IsCurrent() || Tag != HodgeHavocTags::CastCommitted || bCastCommitted) { return; }
	const auto* Data = Cast<UHodgeHavocAttackRuleConfig>(Config);
	if (!Data || Data->DarkFlowGainOnCast <= 0.f) { return; }
	if (auto* Rules = UHodgeGameplayAbility_HavocRules::Find(Source->GetAbilitySystemComponentFromActorInfo()))
	{ bCastCommitted = true; Rules->ApplyDarkFlowGain(Source.Get(), Data->DarkFlowGainOnCast); }
}
