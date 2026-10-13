/** @file HodgeAttackWarpTestTypes.h @brief 使用真实 GAS 激活测试朝向占用，播放链由 PIE 验证。 */
#pragma once
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
#include "HodgeAttackWarpTestTypes.generated.h"
UCLASS(Transient, NotBlueprintable)
class UHodgeAttackWarpTestAbility : public UHodgeGameplayAbility_Melee
{
	GENERATED_BODY()
public:
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		const FGameplayTagContainer* Source, const FGameplayTagContainer* Target, FGameplayTagContainer* Relevant) const override;
protected:
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo Activation, const FGameplayEventData* Payload) override;
};
