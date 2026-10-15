/** @file HodgeAbilityCost_ResonanceEnergy.h @brief 接入Lyra AdditionalCosts的满充全消耗成本。 */
#pragma once
#include "AbilitySystem/Abilities/HodgeAbilityCost.h"
#include "HodgeAbilityCost_ResonanceEnergy.generated.h"
class UGameplayEffect;

/** 冷却继续由GA管理，本对象只检查和支付能量。 */
UCLASS(EditInlineNew, DefaultToInstanced)
class HODGEPODGE_API UHodgeAbilityCost_ResonanceEnergy : public UHodgeAbilityCost
{
	GENERATED_BODY()
public:
	/** 默认使用共享Instant增量GE，蓝图成本可覆盖为兼容的资产。 */
	UHodgeAbilityCost_ResonanceEnergy();
	UPROPERTY(EditAnywhere, Category="Costs") TSubclassOf<UGameplayEffect> CostGameplayEffectClass;
	/** 满充与成本修饰器契约同时满足才允许Commit。 */
	virtual bool CheckCost(const UHodgeGameplayAbility* Ability, FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* FailureTags) const override;
	/** 使用本次激活PredictionKey提交负的最大能量增量。 */
	virtual void ApplyCost(const UHodgeGameplayAbility* Ability, FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo) override;
	/** 严格要求单一Instant能量增量，不接受额外属性副作用。 */
	bool ValidateEffect() const;
};
