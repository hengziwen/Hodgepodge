#include "ALS/Nodes/HodgeAlsAnimNode_GameplayTagsBlend.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsAnimNode_GameplayTagsBlend)

int32 FHodgeAlsAnimNode_GameplayTagsBlend::GetActiveChildIndex()
{
	const auto& CurrentActiveTag{GetActiveTag()};

	return CurrentActiveTag.IsValid()
		       ? GetTags().Find(CurrentActiveTag) + 1
		       : 0;
}

const FGameplayTag& FHodgeAlsAnimNode_GameplayTagsBlend::GetActiveTag() const
{
	return GET_ANIM_NODE_DATA(FGameplayTag, ActiveTag);
}

const TArray<FGameplayTag>& FHodgeAlsAnimNode_GameplayTagsBlend::GetTags() const
{
	return GET_ANIM_NODE_DATA(TArray<FGameplayTag>, Tags);
}

#if WITH_EDITOR
void FHodgeAlsAnimNode_GameplayTagsBlend::RefreshPosePins()
{
	const auto Difference{BlendPose.Num() - GetTags().Num() - 1};
	if (Difference == 0)
	{
		return;
	}

	if (Difference > 0)
	{
		for (auto i{Difference}; i > 0; i--)
		{
			RemovePose(BlendPose.Num() - 1);
		}
	}
	else
	{
		for (auto i{Difference}; i < 0; i++)
		{
			AddPose();
		}
	}
}
#endif
