#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "Templates/SubclassOf.h"
#include "HodgeAlsAnimNotify_CameraShake.generated.h"

class UCameraShakeBase;

UCLASS(DisplayName = "HodgeAls Camera Shake Animation Notify")
class HODGEPODGE_API UHodgeAlsAnimNotify_CameraShake : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ForceUnits = "x"))
	float CameraShakeScale{1.0f};

public:
	UHodgeAlsAnimNotify_CameraShake();

	virtual FString GetNotifyName_Implementation() const override;

#if WITH_EDITOR
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& NotifyEvent) override;
#endif

	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
	                    const FAnimNotifyEventReference& NotifyEventReference) override;
};
