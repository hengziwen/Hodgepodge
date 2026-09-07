// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "Engine/DataAsset.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"

#include "GameplayAbilitySpecHandle.h"
#include "HodgeAbilitySet.generated.h"

class UAttributeSet;
class UGameplayEffect;
class UHodgeAbilitySystemComponent;
class UHodgeGameplayAbility;
class UObject;


/**
 * FHodgeAbilitySet_GameplayAbility
 *
 * AbilitySet 中用于配置要授予的 GameplayAbility 的数据。
 */
USTRUCT(BlueprintType)
struct FHodgeAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	// 要授予的 GameplayAbility 类型。
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHodgeGameplayAbility> Ability = nullptr;

	// 授予 GameplayAbility 时使用的等级。
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// 用于将输入映射到该 Ability 的 GameplayTag。
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


/**
 * FHodgeAbilitySet_GameplayEffect
 *
 * AbilitySet 中用于配置要授予的 GameplayEffect 的数据。
 */
USTRUCT(BlueprintType)
struct FHodgeAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:
	// 要授予的 GameplayEffect 类型。
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// 授予 GameplayEffect 时使用的等级。
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

/**
 * FHodgeAbilitySet_AttributeSet
 *
 * AbilitySet 中用于配置要授予的 AttributeSet 的数据。
 */
USTRUCT(BlueprintType)
struct FHodgeAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	// 要添加到 AbilitySystemComponent 中的 AttributeSet 类型。
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
};

/**
 * FHodgeAbilitySet_GrantedHandles
 *
 * 用于保存 AbilitySet 授予的 Ability、GameplayEffect 和 AttributeSet，
 * 方便之后通过这些引用将已经授予的内容从 AbilitySystem 中移除。
 */
USTRUCT(BlueprintType)
struct FHodgeAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	// 保存授予的 GameplayAbility 对应的 SpecHandle。
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);

	// 保存授予的 GameplayEffect 对应的 ActiveGameplayEffectHandle。
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);

	// 保存授予的 AttributeSet 对象指针。
	void AddAttributeSet(UAttributeSet* Set);

	// 根据保存的 Handles 和 AttributeSet，将 AbilitySet 授予的内容从 ASC 中全部移除。
	void TakeFromAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC);

protected:
	// 已授予 GameplayAbility 的 SpecHandle 列表。
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// 已应用 GameplayEffect 的 ActiveGameplayEffectHandle 列表。
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// 已添加到 AbilitySystemComponent 的 AttributeSet 对象列表。
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};


/**
 * UHodgeAbilitySet
 *
 * 不可修改的 PrimaryDataAsset，用于统一配置一组 GameplayAbility、GameplayEffect 和 AttributeSet。
 *
 * 可以把它理解成一个“能力包”：
 * 一个 AbilitySet 可以一次性给角色安装一整套战斗能力、属性和效果。
 */
UCLASS(BlueprintType, Const)
class UHodgeAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UHodgeAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 将当前 AbilitySet 中配置的所有内容授予指定的 AbilitySystemComponent，并可通过返回的 Handles 在之后移除这些内容。
	void GiveToAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC, FHodgeAbilitySet_GrantedHandles* OutGrantedHandles,
	                         UObject* SourceObject = nullptr) const;

protected:
	// 授予 AbilitySet 时需要添加的 GameplayAbility 列表。
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FHodgeAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// 授予 AbilitySet 时需要应用的 GameplayEffect 列表。
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FHodgeAbilitySet_GameplayEffect> GrantedGameplayEffects;

	// 授予 AbilitySet 时需要添加的 AttributeSet 列表。
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FHodgeAbilitySet_AttributeSet> GrantedAttributes;
};
