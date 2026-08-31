#pragma once

#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "HodgeAlsAnimNotifyState_SetLocomotionAction.generated.h"

UCLASS(DisplayName = "HodgeAls Set Locomotion Action Animation Notify State")
class HODGEPODGE_API UHodgeAlsAnimNotifyState_SetLocomotionAction : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FGameplayTag LocomotionAction;

public:
	UHodgeAlsAnimNotifyState_SetLocomotionAction();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
	                         float Duration, const FAnimNotifyEventReference& NotifyEventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
	                       const FAnimNotifyEventReference& NotifyEventReference) override;
};
