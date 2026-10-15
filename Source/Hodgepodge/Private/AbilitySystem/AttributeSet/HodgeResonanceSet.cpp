/** @file HodgeResonanceSet.cpp @brief 充能范围、容量变化和拥有者复制实现。 */
#include "AbilitySystem/AttributeSet/HodgeResonanceSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeResonanceSet)

namespace HodgeResonanceTags
{
	UE_DEFINE_GAMEPLAY_TAG(EnergyDelta,"SetByCaller.ResonanceEnergyDelta");
	UE_DEFINE_GAMEPLAY_TAG(Ability,"Ability.Type.Action.Melee.ResonanceLiberation");
	UE_DEFINE_GAMEPLAY_TAG(Input,"InputTag.Ability.ResonanceLiberation");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown,"Cooldown.Hero.ResonanceLiberation");
}

UHodgeResonanceSet::UHodgeResonanceSet() : ResonanceEnergy(0.f), MaxResonanceEnergy(0.f) {}

bool UHodgeResonanceSet::IsResourceReady() const
{
	return FMath::IsFinite(GetMaxResonanceEnergy()) && GetMaxResonanceEnergy()>0.f && FMath::IsFinite(GetResonanceEnergy());
}

void UHodgeResonanceSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UHodgeResonanceSet,ResonanceEnergy,COND_OwnerOnly,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHodgeResonanceSet,MaxResonanceEnergy,COND_OwnerOnly,REPNOTIFY_Always);
}

void UHodgeResonanceSet::Clamp(const FGameplayAttribute& Attribute, float& Value) const
{
	if (!FMath::IsFinite(Value)) { Value=0.f; }
	if (Attribute==GetMaxResonanceEnergyAttribute()) { Value=FMath::Max(0.f,Value); }
	else if (Attribute==GetResonanceEnergyAttribute()) { Value=FMath::Clamp(Value,0.f,FMath::Max(0.f,GetMaxResonanceEnergy())); }
}

void UHodgeResonanceSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute,NewValue); Clamp(Attribute,NewValue);
}

void UHodgeResonanceSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute,NewValue); Clamp(Attribute,NewValue);
}

void UHodgeResonanceSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute,OldValue,NewValue);
	if (Attribute==GetMaxResonanceEnergyAttribute() && GetResonanceEnergy()>NewValue) { SetResonanceEnergy(NewValue); }
}

void UHodgeResonanceSet::OnRep_ResonanceEnergy(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHodgeResonanceSet,ResonanceEnergy,Old);
}

void UHodgeResonanceSet::OnRep_MaxResonanceEnergy(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHodgeResonanceSet,MaxResonanceEnergy,Old);
}
