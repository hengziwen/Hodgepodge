/** @file HodgeHavocSet.cpp @brief 暗流范围约束及拥有者复制。 */
#include "AbilitySystem/AttributeSet/HodgeHavocSet.h"
#include "Net/UnrealNetwork.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHavocSet)

namespace HodgeHavocTags
{
	UE_DEFINE_GAMEPLAY_TAG(DarkFlowDelta,"SetByCaller.DarkFlowDelta");
	UE_DEFINE_GAMEPLAY_TAG(DarkSurge,"State.Combat.Mode.DarkSurge");
	UE_DEFINE_GAMEPLAY_TAG(HavocElement,"GameplayEffect.DamageElement.Havoc");
	UE_DEFINE_GAMEPLAY_TAG(CircuitInput,"InputTag.Ability.ResonanceCircuit");
	UE_DEFINE_GAMEPLAY_TAG(CircuitAbility,"Ability.Type.Action.Melee.ResonanceCircuit");
	UE_DEFINE_GAMEPLAY_TAG(CastCommitted,"GameplayEvent.Attack.CastCommitted");
}

UHodgeHavocSet::UHodgeHavocSet() : DarkFlow(0.f), MaxDarkFlow(0.f) {}

bool UHodgeHavocSet::IsResourceReady() const
{
	return FMath::IsFinite(GetMaxDarkFlow()) && GetMaxDarkFlow()>0.f && FMath::IsFinite(GetDarkFlow());
}

void UHodgeHavocSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UHodgeHavocSet,DarkFlow,COND_OwnerOnly,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHodgeHavocSet,MaxDarkFlow,COND_OwnerOnly,REPNOTIFY_Always);
}

void UHodgeHavocSet::Clamp(const FGameplayAttribute& Attribute, float& Value) const
{
	if (!FMath::IsFinite(Value)) { Value=0.f; }
	if (Attribute==GetMaxDarkFlowAttribute()) { Value=FMath::Max(0.f,Value); }
	else if (Attribute==GetDarkFlowAttribute()) { Value=FMath::Clamp(Value,0.f,FMath::Max(0.f,GetMaxDarkFlow())); }
}

void UHodgeHavocSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute,NewValue); Clamp(Attribute,NewValue);
}

void UHodgeHavocSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute,NewValue); Clamp(Attribute,NewValue);
}

void UHodgeHavocSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute,OldValue,NewValue);
	if (Attribute==GetMaxDarkFlowAttribute() && GetDarkFlow()>NewValue) { SetDarkFlow(NewValue); }
}

void UHodgeHavocSet::OnRep_DarkFlow(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHodgeHavocSet,DarkFlow,Old);
}

void UHodgeHavocSet::OnRep_MaxDarkFlow(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHodgeHavocSet,MaxDarkFlow,Old);
}
