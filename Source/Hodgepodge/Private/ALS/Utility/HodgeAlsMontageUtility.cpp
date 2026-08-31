#include "ALS/Utility/HodgeAlsMontageUtility.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "ALS/Utility/HodgeAlsMacros.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsMontageUtility)

FTransform UHodgeAlsMontageUtility::ExtractRootTransformFromMontage(const UAnimMontage* Montage, const float Time)
{
	// Based on UMotionWarpingUtilities::ExtractRootTransformFromAnimation().

	if (!HODGEALS_ENSURE(IsValid(Montage)) || !HODGEALS_ENSURE(Montage->SlotAnimTracks.Num() > 0))
	{
		return FTransform::Identity;
	}

	const auto* Segment{Montage->SlotAnimTracks[0].AnimTrack.GetSegmentAtTime(Time)};
	if (!HODGEALS_ENSURE(Segment != nullptr))
	{
		return FTransform::Identity;
	}

	const auto* Sequence{Cast<UAnimSequence>(Segment->GetAnimReference())};
	if (!HODGEALS_ENSURE(IsValid(Sequence)))
	{
		return FTransform::Identity;
	}

	return Sequence->ExtractRootTrackTransform(Segment->ConvertTrackPosToAnimPos(Time), nullptr);
}

FTransform UHodgeAlsMontageUtility::ExtractLastRootTransformFromMontage(const UAnimMontage* Montage)
{
	// Based on UMotionWarpingUtilities::ExtractRootTransformFromAnimation().

	if (!HODGEALS_ENSURE(IsValid(Montage)) || !HODGEALS_ENSURE(Montage->SlotAnimTracks.Num() > 0) ||
	    !HODGEALS_ENSURE(Montage->SlotAnimTracks[0].AnimTrack.AnimSegments.Num() > 0))
	{
		return FTransform::Identity;
	}

	const auto& Segment{Montage->SlotAnimTracks[0].AnimTrack.AnimSegments.Last()};
	const auto* Sequence{Cast<UAnimSequence>(Segment.GetAnimReference())};

	if (!HODGEALS_ENSURE(IsValid(Sequence)))
	{
		return FTransform::Identity;
	}

	return Sequence->ExtractRootTrackTransform(Segment.GetEndPos(), nullptr);
}

void UHodgeAlsMontageUtility::StopMontagesWithAnySharedSlots(UAnimInstance* AnimationInstance, const UAnimMontage* ReferenceMontage,
                                                        const float BlendOutDuration)
{
	if (!HODGEALS_ENSURE(IsValid(AnimationInstance)) || !HODGEALS_ENSURE(IsValid(ReferenceMontage)))
	{
		return;
	}

	TSet<FName, DefaultKeyFuncs<FName>, TInlineSetAllocator<4>> SlotNames;
	SlotNames.Reserve(ReferenceMontage->SlotAnimTracks.Num());

	for (const auto& SlotTrack : ReferenceMontage->SlotAnimTracks)
	{
		SlotNames.Emplace(SlotTrack.SlotName);
	}

	for (auto* MontageInstance : AnimationInstance->MontageInstances)
	{
		if (MontageInstance == nullptr || !MontageInstance->IsActive())
		{
			continue;
		}

		const auto* Montage{MontageInstance->Montage.Get()};

		for (const auto& SlotTrack : Montage->SlotAnimTracks)
		{
			if (!SlotNames.Contains(SlotTrack.SlotName))
			{
				continue;
			}

			FMontageBlendSettings BlendOutSettings{Montage->BlendOut};

			if (BlendOutDuration >= 0.0f)
			{
				BlendOutSettings.Blend.BlendTime = BlendOutDuration;
			}

			BlendOutSettings.BlendMode = Montage->BlendModeOut;
			BlendOutSettings.BlendProfile = Montage->BlendProfileOut;

			MontageInstance->Stop(BlendOutSettings);
			break;
		}
	}
}
