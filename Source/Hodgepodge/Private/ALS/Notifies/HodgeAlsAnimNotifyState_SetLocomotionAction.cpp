#include "ALS/Notifies/HodgeAlsAnimNotifyState_SetLocomotionAction.h"

#include "ALS/HodgeAlsCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "ALS/Utility/HodgeAlsUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsAnimNotifyState_SetLocomotionAction)

UHodgeAlsAnimNotifyState_SetLocomotionAction::UHodgeAlsAnimNotifyState_SetLocomotionAction()
{
	bIsNativeBranchingPoint = true;
}

FString UHodgeAlsAnimNotifyState_SetLocomotionAction::GetNotifyName_Implementation() const
{
	TStringBuilder<256> NotifyNameBuilder{
		InPlace, TEXTVIEW("HodgeAls Set Locomotion Action: "),
		FName::NameToDisplayString(UHodgeAlsUtility::GetSimpleTagName(LocomotionAction).ToString(), false)
	};

	return FString{NotifyNameBuilder};
}

void UHodgeAlsAnimNotifyState_SetLocomotionAction::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
                                                          const float Duration, const FAnimNotifyEventReference& NotifyEventReference)
{
	Super::NotifyBegin(Mesh, Sequence, Duration, NotifyEventReference);

	auto* Character{Cast<AHodgeAlsCharacter>(Mesh->GetOwner())};
	if (IsValid(Character))
	{
		Character->SetLocomotionAction(LocomotionAction);
	}
}

void UHodgeAlsAnimNotifyState_SetLocomotionAction::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
                                                        const FAnimNotifyEventReference& NotifyEventReference)
{
	Super::NotifyEnd(Mesh, Sequence, NotifyEventReference);

	auto* Character{Cast<AHodgeAlsCharacter>(Mesh->GetOwner())};

	if (IsValid(Character) && Character->GetLocomotionAction() == LocomotionAction)
	{
		Character->SetLocomotionAction(FGameplayTag::EmptyTag);
	}
}
