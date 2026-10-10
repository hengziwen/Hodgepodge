// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "HodgeAbilityTagRelationshipMapping.generated.h"

class UObject;

/**
 * 描述不同 Ability Tag 之间的关系。
 *
 * 一个 AbilityTag 可以定义它会阻止、取消哪些 Ability，
 * 以及拥有该 Tag 的 Ability 在激活时额外需要或禁止哪些 Tag。
 */
USTRUCT(BlueprintType)
struct FHodgeAbilityTagRelationship
{
	GENERATED_BODY()

	// 当前关系所针对的 Ability Tag；一个 Ability 可以同时拥有多个这样的 Tag。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability, meta = (Categories = "Ability"))
	FGameplayTag AbilityTag;

	// 拥有当前 AbilityTag 的 Ability 激活时，会阻止拥有这些 Tag 的其他 Ability。
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer AbilityTagsToBlock;

	// 拥有当前 AbilityTag 的 Ability 激活时，会取消拥有这些 Tag 的其他 Ability。
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer AbilityTagsToCancel;
	/** 取消豁免优先于所有命中的规则；Death 另有不可移除的保护。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	FGameplayTagContainer AbilityTagsToCancelExceptions;
	/** 映射授权后，中断非死亡技能的不可取消阶段。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	bool bForceCancel = false;

	// 拥有当前 AbilityTag 的 Ability 会隐式获得这些 Activation Required Tags。
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationRequiredTags;

	// 拥有当前 AbilityTag 的 Ability 会隐式获得这些 Activation Blocked Tags。
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationBlockedTags;
};


/**
 * Ability Tag 关系映射表。
 *
 * 用于配置不同 Ability 之间的阻止、取消以及激活条件关系。
 * 通过 DataAsset 可以在编辑器中集中配置整套 Ability Tag 规则。
 */
UCLASS(BlueprintType)
class HODGEPODGE_API UHodgeAbilityTagRelationshipMapping : public UDataAsset
{
	GENERATED_BODY()

private:
	// 保存所有 Ability Tag 之间的关系配置。
	UPROPERTY(EditAnywhere, Category = Ability, meta=(TitleProperty="AbilityTag"))
	TArray<FHodgeAbilityTagRelationship> AbilityTagRelationships;

public:
	friend struct FHodgeRelationshipTestAccess;
	UFUNCTION(BlueprintPure, Category="Hodge|Ability Relationships")
	static bool IsCancellationProtected(const FGameplayTagContainer& TargetTags);
	void GetAbilityCancelExceptions(const FGameplayTagContainer& SourceTags, FGameplayTagContainer& OutExceptions) const;
	UFUNCTION(BlueprintPure, Category="Hodge|Ability Relationships")
	bool IsAbilityCancelledByTags(const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags) const;
	bool IsForcedCancellation(const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags) const;
	// 根据一组 Ability Tags，计算这些 Ability 会阻止和取消哪些其他 Ability。
	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock,
	                                    FGameplayTagContainer* OutTagsToCancel) const;

	// 根据一组 Ability Tags，补充这些 Ability 激活时需要满足和禁止的 Tag。
	void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags,
	                                         FGameplayTagContainer* OutActivationRequired,
	                                         FGameplayTagContainer* OutActivationBlocked) const;

	// 判断指定的 ActionTag 是否会取消拥有给定 Ability Tags 的 Ability。
	bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;
};
