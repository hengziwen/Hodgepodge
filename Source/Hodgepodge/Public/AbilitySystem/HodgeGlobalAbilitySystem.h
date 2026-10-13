/** @file HodgeGlobalAbilitySystem.h
 * @brief 声明全局能力授予服务相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayAbilitySpecHandle.h"
#include "Templates/SubclassOf.h"

#include "HodgeGlobalAbilitySystem.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UHodgeAbilitySystemComponent;
class UObject;
struct FActiveGameplayEffectHandle;
struct FFrame;
struct FGameplayAbilitySpecHandle;

/**
 * 记录某一个全局 GameplayAbility 当前被授予到哪些 ASC，以及对应的 AbilitySpecHandle。
 * Key 是 ASC，Value 是该 Ability 在这个 ASC 上对应的 Handle。
 */
USTRUCT()
struct FGlobalAppliedAbilityList
{
	GENERATED_BODY()

	// 保存每个 ASC 对应的 GameplayAbility SpecHandle，方便之后精确移除。
	UPROPERTY()
	TMap<TObjectPtr<UHodgeAbilitySystemComponent>, FGameplayAbilitySpecHandle> Handles;

	// 将指定 GameplayAbility 授予指定 ASC，并记录产生的 Handle。
	/** 添加目标ASC。
	 * @param Ability 能力，由调用方传入。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 */
	void AddToASC(TSubclassOf<UGameplayAbility> Ability, UHodgeAbilitySystemComponent* ASC);

	// 从指定 ASC 上移除该全局 GameplayAbility。
	/** 移除来源ASC。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 */
	void RemoveFromASC(UHodgeAbilitySystemComponent* ASC);

	// 从所有已经应用过该 Ability 的 ASC 上移除 Ability。
	/** 移除来源All。 */
	void RemoveFromAll();
};

/**
 * 记录某一个全局 GameplayEffect 当前被应用到哪些 ASC，以及对应的 ActiveGameplayEffectHandle。
 * Key 是 ASC，Value 是该 Effect 在这个 ASC 上对应的 Handle。
 */
USTRUCT()
struct FGlobalAppliedEffectList
{
	GENERATED_BODY()

	// 保存每个 ASC 对应的 GameplayEffect Handle，方便之后精确移除。
	UPROPERTY()
	TMap<TObjectPtr<UHodgeAbilitySystemComponent>, FActiveGameplayEffectHandle> Handles;

	// 将指定 GameplayEffect 应用到指定 ASC，并记录产生的 Handle。
	/** 添加目标ASC。
	 * @param Effect 效果，由调用方传入。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 */
	void AddToASC(TSubclassOf<UGameplayEffect> Effect, UHodgeAbilitySystemComponent* ASC);

	// 从指定 ASC 上移除该全局 GameplayEffect。
	/** 移除来源ASC。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 */
	void RemoveFromASC(UHodgeAbilitySystemComponent* ASC);

	// 从所有已经应用过该 Effect 的 ASC 上移除 Effect。
	/** 移除来源All。 */
	void RemoveFromAll();
};

/**
 * World 级别的全局 AbilitySystem 管理器。
 *
 * 用于管理需要对当前 World 中所有 ASC 生效的 GameplayAbility 和 GameplayEffect。
 * 例如全局规则、特殊地图效果、PVP/PVE 模式效果、全局事件 Ability 等。
 */
UCLASS()
class UHodgeGlobalAbilitySystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** 构造全局能力授予服务并初始化其默认状态。 */
	UHodgeGlobalAbilitySystem();

	// 将指定 GameplayAbility 授予当前 GlobalAbilitySystem 中注册的所有 ASC。
	/** 应用能力目标All。
	 * @param Ability 能力，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
	void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);

	// 将指定 GameplayEffect 应用到当前 GlobalAbilitySystem 中注册的所有 ASC。
	/** 应用效果目标All。
	 * @param Effect 效果，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);

	// 从所有已注册 ASC 上移除指定 GameplayAbility。
	/** 移除能力来源All。
	 * @param Ability 能力，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);

	// 从所有已注册 ASC 上移除指定 GameplayEffect。
	/** 移除效果来源All。
	 * @param Effect 效果，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);

	// 将 ASC 注册到全局系统，并自动补发当前已经激活的全局 Ability 和 Effect。
	/** 注册ASC。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 */
	void RegisterASC(UHodgeAbilitySystemComponent* ASC);

	// 将 ASC 从全局系统注销，并移除该 ASC 上由全局系统授予的 Ability 和 Effect。
	/** 注销ASC。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 */
	void UnregisterASC(UHodgeAbilitySystemComponent* ASC);

private:
	// 保存当前所有全局 GameplayAbility，以及每个 Ability 对应的 ASC 和 Handle。
	UPROPERTY()
	TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;

	// 保存当前所有全局 GameplayEffect，以及每个 Effect 对应的 ASC 和 Handle。
	UPROPERTY()
	TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;

	// 当前已经注册到全局 AbilitySystem 的所有 ASC。
	UPROPERTY()
	TArray<TObjectPtr<UHodgeAbilitySystemComponent>> RegisteredASCs;
};
