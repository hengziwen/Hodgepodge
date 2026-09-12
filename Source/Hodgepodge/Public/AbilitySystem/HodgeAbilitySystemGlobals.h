// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// GAS 全局配置与管理类 UAbilitySystemGlobals。
#include "AbilitySystemGlobals.h"

#include "HodgeAbilitySystemGlobals.generated.h"

// UObject 前置声明。
class UObject;

// GameplayEffect 上下文结构，用于携带 Instigator、Causer、HitResult 等效果执行上下文信息。
struct FGameplayEffectContext;

// 项目自定义 GAS 全局管理类，用于扩展 UAbilitySystemGlobals 的全局行为。
// Config=Game 表示该类可以读取 Game 配置文件中的相关配置。
UCLASS(Config=Game)
class UHodgeAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_UCLASS_BODY()
	//~UAbilitySystemGlobals interface

	// 创建 GameplayEffectContext 实例。
	// 通过重写该函数，可以让 GAS 默认创建项目自定义的 FGameplayEffectContext 派生结构。
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;

	//~End of UAbilitySystemGlobals interface
};
