/** @file HodgeGameplayAbility_Interact.h
 * @brief 由 Hero 能力输入发起一次交互，权威业务交给 Controller 组件。
 */
#pragma once

#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "HodgeGameplayAbility_Interact.generated.h"

/** 本地输入入口不预测刷怪或奖励，不授予目标能力或创建提示 UI。 */
UCLASS()
class HODGEPODGE_API UHodgeGameplayAbility_Interact : public UHodgeGameplayAbility
{
	GENERATED_BODY()

public:
	UHodgeGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
