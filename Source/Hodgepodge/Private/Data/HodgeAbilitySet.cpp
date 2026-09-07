// Copyright Epic Games, Inc. All Rights Reserved.

#include "Data/HodgeAbilitySet.h"

#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAbilitySet)

// 添加一个有效的 GameplayAbility SpecHandle，用于之后移除对应 Ability
void FHodgeAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

// 添加一个有效的 GameplayEffect Handle，用于之后移除对应 GameplayEffect
void FHodgeAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

// 保存授予的 AttributeSet 指针，用于之后从 ASC 中移除
void FHodgeAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

// 移除当前 GrantedHandles 所记录的所有 Ability、GameplayEffect 和 AttributeSet
void FHodgeAbilitySet_GrantedHandles::TakeFromAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC)
{
	check(HodgeASC);

	// AbilitySet 的授予和移除必须由服务器执行
	if (!HodgeASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// 遍历并移除之前授予的 GameplayAbility
	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			HodgeASC->ClearAbility(Handle);
		}
	}

	// 遍历并移除之前应用的 GameplayEffect
	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			HodgeASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	// 遍历并移除之前添加到 ASC 的 AttributeSet
	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		HodgeASC->RemoveSpawnedAttribute(Set);
	}

	// 清空所有记录，避免重复移除或持有无效 Handle
	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UHodgeAbilitySet::UHodgeAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// 将当前 AbilitySet 中配置的 Ability、Effect 和 AttributeSet 全部授予指定 ASC
void UHodgeAbilitySet::GiveToAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC,
                                           FHodgeAbilitySet_GrantedHandles* OutGrantedHandles,
                                           UObject* SourceObject) const
{
	check(HodgeASC);

	// AbilitySet 的授予必须由服务器执行
	if (!HodgeASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// 授予 AttributeSet。
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		// 获取当前要授予的 AttributeSet 配置
		const FHodgeAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		// 检查配置的 AttributeSet 类型是否有效
		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"),
			       SetIndex, *GetNameSafe(this));
			continue;
		}

		// 根据配置创建 AttributeSet 实例，Owner 设置为 ASC 的 OwnerActor
		UAttributeSet* NewSet = NewObject<UAttributeSet>(HodgeASC->GetOwner(), SetToGrant.AttributeSet);

		// 将新创建的 AttributeSet 添加到 ASC
		HodgeASC->AddAttributeSetSubobject(NewSet);

		// 如果调用者需要回收信息，则记录这个 AttributeSet
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}

	// 授予 GameplayAbility。
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		// 获取当前要授予的 GameplayAbility 配置
		const FHodgeAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		// 检查 GameplayAbility 类型是否有效
		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."),
			       AbilityIndex, *GetNameSafe(this));
			continue;
		}

		// 获取 GameplayAbility 的 CDO，用于构造 GameplayAbilitySpec
		UHodgeGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UHodgeGameplayAbility>();

		// 根据 Ability 类和等级创建 AbilitySpec
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);

		// 设置 Ability 的 SourceObject，例如可以指向授予该 Ability 的武器、装备或 AbilitySet 来源
		AbilitySpec.SourceObject = SourceObject;

		// 将配置的 InputTag 写入 AbilitySpec 的动态 Source Tags，用于输入系统识别该 Ability
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		// 将 AbilitySpec 授予 ASC，并获得唯一的 SpecHandle
		const FGameplayAbilitySpecHandle AbilitySpecHandle = HodgeASC->GiveAbility(AbilitySpec);

		// 如果调用者需要回收信息，则记录这个 AbilitySpecHandle
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// 授予 GameplayEffect。
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		// 获取当前要授予的 GameplayEffect 配置
		const FHodgeAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		// 检查 GameplayEffect 类型是否有效
		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"),
			       EffectIndex, *GetNameSafe(this));
			continue;
		}

		// 获取 GameplayEffect 的 CDO，因为应用 GameplayEffect 时使用的是 GE 定义对象
		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();

		// 将 GameplayEffect 直接应用到 ASC 自己身上，并获得对应的 ActiveGameplayEffectHandle
		const FActiveGameplayEffectHandle GameplayEffectHandle = HodgeASC->ApplyGameplayEffectToSelf(
			GameplayEffect, EffectToGrant.EffectLevel, HodgeASC->MakeEffectContext());

		// 如果调用者需要回收信息，则记录这个 GameplayEffectHandle
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}
}
