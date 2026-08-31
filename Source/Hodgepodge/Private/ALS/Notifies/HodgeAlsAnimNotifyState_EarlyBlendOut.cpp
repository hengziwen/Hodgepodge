#include "ALS/Notifies/HodgeAlsAnimNotifyState_EarlyBlendOut.h"

#include "ALS/HodgeAlsCharacter.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "ALS/Utility/HodgeAlsMacros.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsAnimNotifyState_EarlyBlendOut)

UHodgeAlsAnimNotifyState_EarlyBlendOut::UHodgeAlsAnimNotifyState_EarlyBlendOut()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif

	bIsNativeBranchingPoint = true;
}

FString UHodgeAlsAnimNotifyState_EarlyBlendOut::GetNotifyName_Implementation() const
{
	return FString{TEXTVIEW("HodgeAls Early Blend Out")};
}

#if WITH_EDITOR
bool UHodgeAlsAnimNotifyState_EarlyBlendOut::CanBePlaced(UAnimSequenceBase* Sequence) const
{
	return IsValid(Sequence) && Sequence->IsA<UAnimMontage>();
}
#endif

void UHodgeAlsAnimNotifyState_EarlyBlendOut::BranchingPointNotifyTick(FBranchingPointNotifyPayload& NotifyPayload, const float DeltaTime)
{
	Super::BranchingPointNotifyTick(NotifyPayload, DeltaTime);

	const auto* Mesh{NotifyPayload.SkelMeshComponent};
	auto* AnimationInstance{IsValid(Mesh) ? Mesh->GetAnimInstance() : nullptr};
	const auto* Character{IsValid(AnimationInstance) ? Cast<AHodgeAlsCharacter>(Mesh->GetOwner()) : nullptr};

	if (IsValid(Character) &&
	    ((bCheckInput && Character->GetLocomotionState().bHasInput) ||
	     (bCheckLocomotionMode && Character->GetLocomotionMode() == LocomotionModeEquals) ||
	     (bCheckRotationMode && Character->GetRotationMode() == RotationModeEquals) ||
	     (bCheckStance && Character->GetStance() == StanceEquals)))
	{
		auto* MontageInstance{AnimationInstance->GetMontageInstanceForID(NotifyPayload.MontageInstanceID)};
		if (!HODGEALS_ENSURE(MontageInstance != nullptr))
		{
			return;
		}

		const auto* Montage{MontageInstance->Montage.Get()};

		FMontageBlendSettings BlendOutSettings{Montage->BlendOut};
		BlendOutSettings.Blend.BlendTime = BlendOutDuration;
		BlendOutSettings.BlendMode = Montage->BlendModeOut;
		BlendOutSettings.BlendProfile = Montage->BlendProfileOut;

		MontageInstance->Stop(BlendOutSettings);
	}
}
