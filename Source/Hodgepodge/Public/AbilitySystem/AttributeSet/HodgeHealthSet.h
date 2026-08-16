// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/HodgeAttributeSet.h"
#include "HodgeHealthSet.generated.h"

/**
 * 
 */
UCLASS()
class HODGEPODGE_API UHodgeHealthSet : public UHodgeAttributeSet
{
	GENERATED_BODY()

public:
	UHodgeHealthSet();

	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Healing);
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Damage);
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, BaseHeal);

	// Delegate when 生命 changes due to damage/healing, some information may be missing on the client
	mutable FHodgeAttributeEvent OnHealthChanged;

	// Delegate when max 生命 changes
	mutable FHodgeAttributeEvent OnMaxHealthChanged;

	// Delegate to broadcast when the 生命 attribute reaches zero
	mutable FHodgeAttributeEvent OnOutOfHealth;

protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
	// The current 生命 attribute.  The 生命 will be capped by the max 生命 attribute.  生命 is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Hodge|Health",
		Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current max 生命 attribute.  Max 生命 is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Hodge|Health",
		Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// Used to track when the 生命 reaches 0.
	bool bOutOfHealth;

	// Store the 生命 before any changes 
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;

	// -------------------------------------------------------------------
	//	Meta 属性 (please keep attributes that aren't 'stateful' below 
	// -------------------------------------------------------------------

	// Incoming healing. This is mapped directly to +生命
	UPROPERTY(BlueprintReadOnly, Category="Hodge|Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Healing;

	// Incoming damage. This is mapped directly to -生命
	UPROPERTY(BlueprintReadOnly, Category="Hodge|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
	FGameplayAttributeData Damage;

	// Base combat attributes
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Hodge|Combat",
		Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Hodge|Combat",
		Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;
};
