#pragma once
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h"
#include "HodgeHitReactionTestAbility.generated.h"

/** 原生测试用可实例化动作，不改变正式 Jump 的抽象类契约。 */
UCLASS(Transient, NotBlueprintable)
class UHodgeHitReactionTestAbility : public UHodgeGameplayAbility_Jump
{
	GENERATED_BODY()
};
