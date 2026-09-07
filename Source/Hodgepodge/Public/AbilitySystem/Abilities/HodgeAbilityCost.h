// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "HodgeAbilityCost.generated.h"

class UHodgeGameplayAbility;

/**
 * Ability 的单项消耗基类。
 *
 * 一个 Ability 可以配置多个 UHodgeAbilityCost 子类，
 * 例如消耗弹药、体力、能量、技能次数或其他资源。
 *
 * 这个类本身不直接定义具体消耗什么资源，
 * 而是把“检查是否付得起”和“实际扣除资源”交给子类实现。
 *
 * UHodgeAbilityCost
 *
 * Base class for costs that a HodgeGameplayAbility has (e.g., ammo or charges)
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class HODGEPODGE_API UHodgeAbilityCost : public UObject
{
	GENERATED_BODY()

public:
	// AbilityCost 作为对象配置存在时使用的默认构造函数。
	UHodgeAbilityCost()
	{
	}

	/**
	 * 检查当前 Ability 是否能够支付这项消耗。
	 *
	 * 这里只负责“能不能付”，不负责真正扣除资源。
	 *
	 * 如果检查失败，可以通过 OptionalRelevantTags 返回失败原因，
	 * 例如弹药不足、能量不足等，供 UI 或其他系统根据 Tag 给玩家反馈。
	 *
	 * Ability 和 ActorInfo 进入函数时保证不为空，
	 * OptionalRelevantTags 则可能为空，因此使用前需要判断。
	 *
	 * @return true 表示当前资源足够，可以支付该消耗。
	 * @return false 表示资源不足，Ability 不应该因为这项 Cost 被激活。
	 */
	virtual bool CheckCost(const UHodgeGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       FGameplayTagContainer* OptionalRelevantTags) const
	{
		// 基类默认没有任何实际消耗，因此始终认为可以支付。
		return true;
	}

	/**
	 * 实际应用 Ability 的资源消耗。
	 *
	 * CheckCost() 负责检查是否付得起，
	 * ApplyCost() 则负责真正修改资源，例如扣除弹药或能量。
	 *
	 * 是否只在命中后扣除，由 ShouldOnlyApplyCostOnHit() 控制。
	 *
	 * 调用方会负责检查 ShouldOnlyApplyCostOnHit()，
	 * 因此具体 Cost 的实现不需要再次判断这个条件。
	 *
	 * Ability 和 ActorInfo 进入函数时保证不为空。
	 */
	virtual void ApplyCost(const UHodgeGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       const FGameplayAbilityActivationInfo ActivationInfo)
	{
		// 基类默认没有任何实际消耗，因此不执行任何操作。
	}

	// 返回该 Cost 是否只应该在 Ability 成功命中后才真正扣除。
	bool ShouldOnlyApplyCostOnHit() const { return bOnlyApplyCostOnHit; }

protected:
	// 为 true 时，该 Cost 不会在 Ability 激活时立即扣除，而是等待 Ability 成功命中后再扣除。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
	bool bOnlyApplyCostOnHit = false;
};
