// Copyright Epic Games, Inc. All Rights Reserved.

// 项目自定义 AbilitySystemGlobals 类定义。
#include "AbilitySystem/HodgeAbilitySystemGlobals.h"

// 项目自定义 GameplayEffectContext，用于替代 GAS 默认的 FGameplayEffectContext。
#include "AbilitySystem/HodgeGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAbilitySystemGlobals)

// GameplayEffectContext 前置声明。
struct FGameplayEffectContext;

// UHodgeAbilitySystemGlobals 构造函数。
UHodgeAbilitySystemGlobals::UHodgeAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
// 将 ObjectInitializer 继续传递给父类 UAbilitySystemGlobals。
	: Super(ObjectInitializer)
{
}

// 为 GAS 创建新的 GameplayEffectContext。
FGameplayEffectContext* UHodgeAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	// 创建项目自定义 FHodgeGameplayEffectContext，使后续 GameplayEffect 默认使用扩展后的 EffectContext。
	return new FHodgeGameplayEffectContext();
}
