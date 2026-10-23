/** @file HodgeAttackRuleTestAbility.h @brief 使用真实 GAS 激活但跳过动画的执行规则夹具。 */
#pragma once
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
#include "HodgeAttackRuleTestAbility.generated.h"

/** 仅测试能力不查 Montage／Definition 授权，生产入口不使用此类型。 */
UCLASS(Transient, NotBlueprintable)
class UHodgeAttackRuleTestAbility : public UHodgeGameplayAbility_Melee
{
	GENERATED_BODY()
public:
	UHodgeAttackRuleTestAbility();
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* RelevantTags) const override;
protected:
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
};
