#pragma once

#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "HodgeAlsAnimNotify_SetGroundedEntryMode.generated.h"

UCLASS(DisplayName = "HodgeAls Set Grounded Entry Mode Animation Notify")
class HODGEPODGE_API UHodgeAlsAnimNotify_SetGroundedEntryMode : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FGameplayTag GroundedEntryMode;

public:
	UHodgeAlsAnimNotify_SetGroundedEntryMode();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
	                    const FAnimNotifyEventReference& NotifyEventReference) override;
};
