// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/HodgeAbilityTagRelationshipMapping.h"
#include "AbilitySystem/HodgeGameplayTags.h"


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
	if (OutActivationBlocked && AbilityTags.HasTag(HodgeGameplayTags::Ability_Type_Action_Dash))
	{ OutActivationBlocked->AddTag(HodgeGameplayTags::Status_Death); }
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
bool UHodgeAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& TargetTags, const FGameplayTag& ActionTag) const
{ return IsAbilityCancelledByTags(FGameplayTagContainer(ActionTag), TargetTags); }

bool UHodgeAbilityTagRelationshipMapping::IsCancellationProtected(const FGameplayTagContainer& TargetTags)
{
	return TargetTags.HasTag(HodgeGameplayTags::Ability_Type_StatusChange_Death) || TargetTags.HasTag(HodgeGameplayTags::Status_Death);
}
void UHodgeAbilityTagRelationshipMapping::GetAbilityCancelExceptions(const FGameplayTagContainer& SourceTags, FGameplayTagContainer& OutExceptions) const
{
	OutExceptions.AddTag(HodgeGameplayTags::Ability_Type_StatusChange_Death); OutExceptions.AddTag(HodgeGameplayTags::Status_Death);
	for (const auto& Row : AbilityTagRelationships)
	{ if (SourceTags.HasTag(Row.AbilityTag)) { OutExceptions.AppendTags(Row.AbilityTagsToCancelExceptions); } }
}
bool UHodgeAbilityTagRelationshipMapping::IsAbilityCancelledByTags(const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags) const
{
	FGameplayTagContainer Exceptions; GetAbilityCancelExceptions(SourceTags, Exceptions);
	if (TargetTags.HasAny(Exceptions)) { return false; }
	FGameplayTagContainer Cancel; GetAbilityTagsToBlockAndCancel(SourceTags, nullptr, &Cancel);
	return TargetTags.HasAny(Cancel);
}
bool UHodgeAbilityTagRelationshipMapping::IsForcedCancellation(const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags) const
{
	if (!IsAbilityCancelledByTags(SourceTags, TargetTags)) { return false; }
	for (const auto& Row : AbilityTagRelationships)
	{ if (Row.bForceCancel && SourceTags.HasTag(Row.AbilityTag) && TargetTags.HasAny(Row.AbilityTagsToCancel)) { return true; } }
	return false;
}
