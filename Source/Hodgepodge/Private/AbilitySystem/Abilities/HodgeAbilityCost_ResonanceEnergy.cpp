/** @file HodgeAbilityCost_ResonanceEnergy.cpp @brief 沿用GAS预测效果的确认／拒绝，不自行同步扣能。 */
#include "AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.h"
#include "AbilitySystem/Effects/HodgeResonanceEnergyEffect.h"
#include "AbilitySystem/AttributeSet/HodgeResonanceSet.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAbilityCost_ResonanceEnergy)

UHodgeAbilityCost_ResonanceEnergy::UHodgeAbilityCost_ResonanceEnergy()
{
	CostGameplayEffectClass = UHodgeResonanceEnergyEffect::StaticClass();
	bOnlyApplyCostOnHit = false;
}

bool UHodgeAbilityCost_ResonanceEnergy::ValidateEffect() const
{
	const auto* Effect = CostGameplayEffectClass
		                     ? CostGameplayEffectClass->GetDefaultObject<UGameplayEffect>()
		                     : nullptr;
	if (!Effect || Effect->DurationPolicy != EGameplayEffectDurationType::Instant || Effect->Modifiers.Num() != 1 || !
		Effect->Executions.IsEmpty()) { return false; }
	const auto& Modifier = Effect->Modifiers[0];
	return Modifier.Attribute == UHodgeResonanceSet::GetResonanceEnergyAttribute() && Modifier.ModifierOp ==
		EGameplayModOp::Additive &&
		Modifier.ModifierMagnitude.GetMagnitudeCalculationType() == EGameplayEffectMagnitudeCalculation::SetByCaller &&
		Modifier.ModifierMagnitude.GetSetByCallerFloat().DataTag == HodgeResonanceTags::EnergyDelta;
}

bool UHodgeAbilityCost_ResonanceEnergy::CheckCost(const UHodgeGameplayAbility* Ability,
                                                  FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo,
                                                  FGameplayTagContainer* FailureTags) const
{
	const auto* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	const auto* Set = ASC ? ASC->GetSet<UHodgeResonanceSet>() : nullptr;
	const bool bReady = Set && Set->IsResourceReady() && Set->GetResonanceEnergy() >= Set->GetMaxResonanceEnergy() &&
		ValidateEffect() && !bOnlyApplyCostOnHit;
	if (!bReady && FailureTags) { FailureTags->AddTag(HodgeGameplayTags::Ability_ActivateFail_Cost); }
	return bReady;
}

void UHodgeAbilityCost_ResonanceEnergy::ApplyCost(const UHodgeGameplayAbility* Ability,
                                                  FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo,
                                                  FGameplayAbilityActivationInfo ActivationInfo)
{
	auto* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	const auto* Set = ASC ? ASC->GetSet<UHodgeResonanceSet>() : nullptr;
	if (!Set || !Set->IsResourceReady() || !ValidateEffect())
	{
		UE_LOG(LogTemp, Error, TEXT("Resonance cost configuration invalid on [%s]"), *GetNameSafe(ASC));
		return;
	}
	auto Spec = ASC->MakeOutgoingSpec(CostGameplayEffectClass, 1.f, ASC->MakeEffectContext());
	if (!Spec.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Resonance cost spec creation failed"));
		return;
	}
	Spec.Data->SetSetByCallerMagnitude(HodgeResonanceTags::EnergyDelta, -Set->GetMaxResonanceEnergy());
	ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get(), ActivationInfo.GetActivationPredictionKey());
}
