// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/HodgeGlobalAbilitySystem.h"

#include "AbilitySystem/HodgeAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGlobalAbilitySystem)

// 将指定 GameplayAbility 添加到一个 ASC，并记录对应的 AbilitySpecHandle
void FGlobalAppliedAbilityList::AddToASC(TSubclassOf<UGameplayAbility> Ability, UHodgeAbilitySystemComponent* ASC)
{
	// 如果这个 ASC 已经拥有该全局 Ability，则先移除旧的 Ability
	if (FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASC))
	{
		RemoveFromASC(ASC);
	}

	// 获取 GameplayAbility 的 CDO，用于创建 AbilitySpec
	UGameplayAbility* AbilityCDO = Ability->GetDefaultObject<UGameplayAbility>();

	// 使用 Ability CDO 创建默认等级的 AbilitySpec
	FGameplayAbilitySpec AbilitySpec(AbilityCDO);

	// 将 AbilitySpec 授予 ASC，并获取对应的唯一 Handle
	const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);

	// 保存 ASC 与 AbilitySpecHandle 的对应关系，方便之后精确移除
	Handles.Add(ASC, AbilitySpecHandle);
}

// 从指定 ASC 上移除该全局 GameplayAbility
void FGlobalAppliedAbilityList::RemoveFromASC(UHodgeAbilitySystemComponent* ASC)
{
	// 查找该 ASC 上之前记录的 AbilitySpecHandle
	if (FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASC))
	{
		// 根据 Handle 精确移除由全局系统授予的 Ability
		ASC->ClearAbility(*SpecHandle);

		// 移除该 ASC 的 Handle 记录
		Handles.Remove(ASC);
	}
}

// 从所有已经应用该 Ability 的 ASC 上移除 Ability
void FGlobalAppliedAbilityList::RemoveFromAll()
{
	// 遍历所有记录的 ASC 和对应 AbilitySpecHandle
	for (auto& KVP : Handles)
	{
		// 确保 ASC 仍然有效
		if (KVP.Key != nullptr)
		{
			// 根据保存的 Handle 移除全局授予的 Ability
			KVP.Key->ClearAbility(KVP.Value);
		}
	}

	// 清空所有 ASC 与 Handle 的对应关系
	Handles.Empty();
}


// 将指定 GameplayEffect 应用到一个 ASC，并记录对应的 ActiveGameplayEffectHandle
void FGlobalAppliedEffectList::AddToASC(TSubclassOf<UGameplayEffect> Effect, UHodgeAbilitySystemComponent* ASC)
{
	// 如果这个 ASC 已经拥有该全局 Effect，则先移除旧的 Effect
	if (FActiveGameplayEffectHandle* EffectHandle = Handles.Find(ASC))
	{
		RemoveFromASC(ASC);
	}

	// 获取 GameplayEffect 的 CDO，用于应用 GameplayEffect
	const UGameplayEffect* GameplayEffectCDO = Effect->GetDefaultObject<UGameplayEffect>();

	// 将 GameplayEffect 应用到 ASC 自己身上，并获取对应的 ActiveGameplayEffectHandle
	const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(
		GameplayEffectCDO, /*Level=*/ 1, ASC->MakeEffectContext());

	// 保存 ASC 与 GameplayEffectHandle 的对应关系，方便之后精确移除
	Handles.Add(ASC, GameplayEffectHandle);
}

// 从指定 ASC 上移除该全局 GameplayEffect
void FGlobalAppliedEffectList::RemoveFromASC(UHodgeAbilitySystemComponent* ASC)
{
	// 查找该 ASC 上之前记录的 GameplayEffectHandle
	if (FActiveGameplayEffectHandle* EffectHandle = Handles.Find(ASC))
	{
		// 根据 Handle 移除由全局系统应用的 GameplayEffect
		ASC->RemoveActiveGameplayEffect(*EffectHandle);

		// 移除该 ASC 的 Handle 记录
		Handles.Remove(ASC);
	}
}

// 从所有已经应用该 Effect 的 ASC 上移除 Effect
void FGlobalAppliedEffectList::RemoveFromAll()
{
	// 遍历所有记录的 ASC 和对应 GameplayEffectHandle
	for (auto& KVP : Handles)
	{
		// 确保 ASC 仍然有效
		if (KVP.Key != nullptr)
		{
			// 根据保存的 Handle 移除全局系统应用的 GameplayEffect
			KVP.Key->RemoveActiveGameplayEffect(KVP.Value);
		}
	}

	// 清空所有 ASC 与 Handle 的对应关系
	Handles.Empty();
}

UHodgeGlobalAbilitySystem::UHodgeGlobalAbilitySystem()
{
}

// 将指定 GameplayAbility 应用到当前所有已经注册的 ASC
void UHodgeGlobalAbilitySystem::ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability)
{
	// 只有 Ability 有效且当前还没有应用过该 Ability 时才执行
	if ((Ability.Get() != nullptr) && (!AppliedAbilities.Contains(Ability)))
	{
		// 为该 Ability 创建全局应用记录
		FGlobalAppliedAbilityList& Entry = AppliedAbilities.Add(Ability);

		// 将该 Ability 添加到当前所有已注册 ASC
		for (UHodgeAbilitySystemComponent* ASC : RegisteredASCs)
		{
			Entry.AddToASC(Ability, ASC);
		}
	}
}

// 将指定 GameplayEffect 应用到当前所有已经注册的 ASC
void UHodgeGlobalAbilitySystem::ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect)
{
	// 只有 Effect 有效且当前还没有应用过该 Effect 时才执行
	if ((Effect.Get() != nullptr) && (!AppliedEffects.Contains(Effect)))
	{
		// 为该 Effect 创建全局应用记录
		FGlobalAppliedEffectList& Entry = AppliedEffects.Add(Effect);

		// 将该 Effect 应用到当前所有已注册 ASC
		for (UHodgeAbilitySystemComponent* ASC : RegisteredASCs)
		{
			Entry.AddToASC(Effect, ASC);
		}
	}
}

// 从所有 ASC 上移除指定 GameplayAbility
void UHodgeGlobalAbilitySystem::RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability)
{
	// 只有 Ability 有效且当前确实存在对应的全局记录时才执行
	if ((Ability.Get() != nullptr) && AppliedAbilities.Contains(Ability))
	{
		// 获取该 Ability 对应的全局应用记录
		FGlobalAppliedAbilityList& Entry = AppliedAbilities[Ability];

		// 从所有曾经应用过该 Ability 的 ASC 上移除
		Entry.RemoveFromAll();

		// 删除该 Ability 的全局记录
		AppliedAbilities.Remove(Ability);
	}
}

// 从所有 ASC 上移除指定 GameplayEffect
void UHodgeGlobalAbilitySystem::RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect)
{
	// 只有 Effect 有效且当前确实存在对应的全局记录时才执行
	if ((Effect.Get() != nullptr) && AppliedEffects.Contains(Effect))
	{
		// 获取该 Effect 对应的全局应用记录
		FGlobalAppliedEffectList& Entry = AppliedEffects[Effect];

		// 从所有曾经应用过该 Effect 的 ASC 上移除
		Entry.RemoveFromAll();

		// 删除该 Effect 的全局记录
		AppliedEffects.Remove(Effect);
	}
}

// 将一个 ASC 注册到全局 AbilitySystem，并补发当前已经存在的全局 Ability 和 Effect
void UHodgeGlobalAbilitySystem::RegisterASC(UHodgeAbilitySystemComponent* ASC)
{
	check(ASC);

	// 将当前已经激活的所有全局 Ability 添加到新注册的 ASC
	for (auto& Entry : AppliedAbilities)
	{
		Entry.Value.AddToASC(Entry.Key, ASC);
	}

	// 将当前已经激活的所有全局 Effect 应用到新注册的 ASC
	for (auto& Entry : AppliedEffects)
	{
		Entry.Value.AddToASC(Entry.Key, ASC);
	}

	// 将 ASC 加入注册列表，AddUnique 可以避免重复注册
	RegisteredASCs.AddUnique(ASC);
}

// 从全局 AbilitySystem 注销一个 ASC，并移除该 ASC 上由全局系统授予的所有内容
void UHodgeGlobalAbilitySystem::UnregisterASC(UHodgeAbilitySystemComponent* ASC)
{
	check(ASC);

	// 从该 ASC 上移除所有全局 GameplayAbility
	for (auto& Entry : AppliedAbilities)
	{
		Entry.Value.RemoveFromASC(ASC);
	}

	// 从该 ASC 上移除所有全局 GameplayEffect
	for (auto& Entry : AppliedEffects)
	{
		Entry.Value.RemoveFromASC(ASC);
	}

	// 从注册列表中移除该 ASC
	RegisteredASCs.Remove(ASC);
}
