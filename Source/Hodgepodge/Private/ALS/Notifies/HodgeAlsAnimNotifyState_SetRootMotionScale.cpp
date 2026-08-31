#include "ALS/Notifies/HodgeAlsAnimNotifyState_SetRootMotionScale.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "ALS/Utility/HodgeAlsLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsAnimNotifyState_SetRootMotionScale)

UHodgeAlsAnimNotifyState_SetRootMotionScale::UHodgeAlsAnimNotifyState_SetRootMotionScale()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

FString UHodgeAlsAnimNotifyState_SetRootMotionScale::GetNotifyName_Implementation() const
{
	TStringBuilder<64> NotifyNameBuilder{InPlace, TEXTVIEW("HodgeAls Set Root Motion Scale: ")};

	NotifyNameBuilder.Appendf(TEXT("%.2f"), TranslationScale);

	return FString{NotifyNameBuilder};
}

void UHodgeAlsAnimNotifyState_SetRootMotionScale::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
                                                         const float Duration, const FAnimNotifyEventReference& NotifyEventReference)
{
	Super::NotifyBegin(Mesh, Sequence, Duration, NotifyEventReference);

	auto* Character{Cast<ACharacter>(Mesh->GetOwner())};

	if (IsValid(Character) && Character->GetLocalRole() >= ROLE_AutonomousProxy)
	{
		Character->SetAnimRootMotionTranslationScale(TranslationScale);
	}
}

void UHodgeAlsAnimNotifyState_SetRootMotionScale::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
                                                       const FAnimNotifyEventReference& NotifyEventReference)
{
	Super::NotifyEnd(Mesh, Sequence, NotifyEventReference);

	auto* Character{Cast<ACharacter>(Mesh->GetOwner())};

	if (IsValid(Character) && Character->GetLocalRole() >= ROLE_AutonomousProxy)
	{
		if (FMath::IsNearlyEqual(Character->GetAnimRootMotionTranslationScale(), TranslationScale))
		{
			Character->SetAnimRootMotionTranslationScale(1.0f);
		}
		else
		{
			UE_LOG(LogHodgeAls, Warning, TEXT("%hs: The current translation scale does not match the translation scale from the ")
			       TEXT("animation notify! Probably something changed it before the animation notify ended."), __FUNCTION__);
		}
	}
}
