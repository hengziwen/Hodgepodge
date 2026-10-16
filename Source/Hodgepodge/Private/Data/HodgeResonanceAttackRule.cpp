/** @file HodgeResonanceAttackRule.cpp @brief 实际生命伤害确认后的共鸣收益，不参与公共伤害提交。 */
#include "Data/HodgeResonanceAttackRule.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/AttributeSet/HodgeResonanceSet.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/Effects/HodgeResonanceEnergyEffect.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeResonanceAttackRule)

/** 默认使用现有共鸣增量 GE，不创建新资源机制。 */
UHodgeResonanceAttackRuleConfig::UHodgeResonanceAttackRuleConfig() { GainEffect = UHodgeResonanceEnergyEffect::StaticClass(); }
/** 每次激活创建独立账本，不把状态写回共享配置。 */
UHodgeAttackRule* UHodgeResonanceAttackRuleConfig::CreateRule(UObject* Outer) const { return NewObject<UHodgeResonanceAttackRule>(Outer); }
/** 保留原先的有限非负数和 GE 类型契约。 */
void UHodgeResonanceAttackRuleConfig::ValidateRule(const UHodgeAbilityDefinition&, TArray<FText>& Errors) const
{
	const float Gain = GainPerPhase.GetValueAtLevel(1.f);
	if (!FMath::IsFinite(Gain) || Gain < 0.f || (Gain > 0.f && (!GainEffect || !GainEffect->IsChildOf(UHodgeResonanceEnergyEffect::StaticClass()))))
	{ Errors.Add(FText::FromString(TEXT("Resonance gain requires a finite nonnegative amount and compatible energy GE"))); }
}
/** 同阶段多目标只回一次；死亡、免疫和嵌套伤害不会冒充有效收益。 */
void UHodgeResonanceAttackRule::OnSettlement(const FHodgeAttackSettlement& Settlement)
{
	if (!Accepts(Settlement) || Settlement.AcceptedDamage <= 0.f) { return; }
	const auto* Data = Cast<UHodgeResonanceAttackRuleConfig>(Config);
	auto* ASC = Source->GetAbilitySystemComponentFromActorInfo();
	const auto* Resource = ASC ? ASC->GetSet<UHodgeResonanceSet>() : nullptr;
	const auto* Health = ASC ? ASC->GetSet<UHodgeHealthSet>() : nullptr;
	if (!Data || !Resource || !Resource->IsResourceReady() || ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) || (Health && Health->GetHealth() <= 0.f)) { return; }
	const float Gain = Data->GainPerPhase.GetValueAtLevel(Source->GetAbilityLevel());
	if (!FMath::IsFinite(Gain) || Gain <= 0.f || !Data->GainEffect) { return; }
	auto Spec = ASC->MakeOutgoingSpec(Data->GainEffect, Source->GetAbilityLevel(), ASC->MakeEffectContext());
	if (!Spec.IsValid() || !Phases.TryReserve(Settlement.HitGroup, Settlement.AttackPhase, Settlement.AcceptedDamage)) { return; }
	// 占用阶段先于属性广播，重入不能重复提交本阶段。
	Spec.Data->SetSetByCallerMagnitude(HodgeResonanceTags::EnergyDelta, Gain);
	ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
}
