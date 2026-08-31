#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "HodgeAlsAnimNotifyState_SetRootMotionScale.generated.h"

UCLASS(DisplayName = "HodgeAls Set Root Motion Scale Animation Notify State")
class HODGEPODGE_API UHodgeAlsAnimNotifyState_SetRootMotionScale : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ForceUnits = "x"))
	float TranslationScale{1.0f};

public:
	UHodgeAlsAnimNotifyState_SetRootMotionScale();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
	                         float Duration, const FAnimNotifyEventReference& NotifyEventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
	                       const FAnimNotifyEventReference& NotifyEventReference) override;
};
