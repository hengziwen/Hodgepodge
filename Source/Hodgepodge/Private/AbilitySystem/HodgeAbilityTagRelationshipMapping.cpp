// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/HodgeAbilityTagRelationshipMapping.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAbilityTagRelationshipMapping)

// 根据当前 Ability 的标签，获取需要 Block 和 Cancel 的其他 Ability 标签。
void UHodgeAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(
	const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock,
	FGameplayTagContainer* OutTagsToCancel) const
{
	// 当前实现直接遍历所有标签关系，后续可以根据需要优化查找方式。
	// Simple iteration for now
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		// 获取当前遍历到的 Ability 标签关系配置。
		const FHodgeAbilityTagRelationship& Tags = AbilityTagRelationships[i];

		// 当前 Ability 拥有该关系配置对应的标签时，应用这条关系。
		if (AbilityTags.HasTag(Tags.AbilityTag))
		{
			// 如果调用方需要 Block 标签，则追加该 Ability 需要 Block 的标签。
			if (OutTagsToBlock)
			{
				OutTagsToBlock->AppendTags(Tags.AbilityTagsToBlock);
			}

			// 如果调用方需要 Cancel 标签，则追加该 Ability 需要 Cancel 的标签。
			if (OutTagsToCancel)
			{
				OutTagsToCancel->AppendTags(Tags.AbilityTagsToCancel);
			}
		}
	}
}

// 根据当前 Ability 的标签，获取激活时必须满足和禁止满足的标签。
void UHodgeAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(
	const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired,
	FGameplayTagContainer* OutActivationBlocked) const
{
	// 当前实现直接遍历所有标签关系，后续可以根据需要优化查找方式。
	// Simple iteration for now
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		// 获取当前遍历到的 Ability 标签关系配置。
		const FHodgeAbilityTagRelationship& Tags = AbilityTagRelationships[i];

		// 当前 Ability 拥有该关系配置对应的标签时，应用这条关系。
		if (AbilityTags.HasTag(Tags.AbilityTag))
		{
			// 如果调用方需要 Required 标签，则追加激活该 Ability 必须拥有的标签。
			if (OutActivationRequired)
			{
				OutActivationRequired->AppendTags(Tags.ActivationRequiredTags);
			}

			// 如果调用方需要 Blocked 标签，则追加激活该 Ability 不能拥有的标签。
			if (OutActivationBlocked)
			{
				OutActivationBlocked->AppendTags(Tags.ActivationBlockedTags);
			}
		}
	}
}

// 判断指定 ActionTag 对应的 Ability，是否会因为当前标签而被取消。
bool UHodgeAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags,
                                                                  const FGameplayTag& ActionTag) const
{
	// 当前实现直接遍历所有标签关系，后续可以根据需要优化查找方式。
	// Simple iteration for now
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		// 获取当前遍历到的 Ability 标签关系配置。
		const FHodgeAbilityTagRelationship& Tags = AbilityTagRelationships[i];

		// 找到目标 Ability 的关系配置，并且当前标签命中了它的 Cancel 条件。
		if (Tags.AbilityTag == ActionTag && Tags.AbilityTagsToCancel.HasAny(AbilityTags))
		{
			// 只要存在任意一条匹配关系，就说明该 Ability 应该被取消。
			return true;
		}
	}

	// 没有任何标签关系满足取消条件。
	return false;
}
