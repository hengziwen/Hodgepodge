#include "ALS/Notifies/HodgeAlsAnimNotify_SetGroundedEntryMode.h"

#include "ALS/HodgeAlsAnimationInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "ALS/Utility/HodgeAlsUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsAnimNotify_SetGroundedEntryMode)

UHodgeAlsAnimNotify_SetGroundedEntryMode::UHodgeAlsAnimNotify_SetGroundedEntryMode()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

FString UHodgeAlsAnimNotify_SetGroundedEntryMode::GetNotifyName_Implementation() const
{
	TStringBuilder<256> NotifyNameBuilder{
		InPlace, TEXTVIEW("HodgeAls Set Grounded Entry Mode: "),
		FName::NameToDisplayString(UHodgeAlsUtility::GetSimpleTagName(GroundedEntryMode).ToString(), false)
	};

	return FString{NotifyNameBuilder};
}

void UHodgeAlsAnimNotify_SetGroundedEntryMode::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
                                                 const FAnimNotifyEventReference& NotifyEventReference)
{
	Super::Notify(Mesh, Sequence, NotifyEventReference);

	auto* AnimationInstance{Cast<UHodgeAlsAnimationInstance>(Mesh->GetAnimInstance())};
	if (IsValid(AnimationInstance))
	{
		AnimationInstance->SetGroundedEntryMode(GroundedEntryMode);
	}
}
