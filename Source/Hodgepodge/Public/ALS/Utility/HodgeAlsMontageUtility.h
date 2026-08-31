#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HodgeAlsMontageUtility.generated.h"

UCLASS()
class HODGEPODGE_API UHodgeAlsMontageUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "HODGEALS|Montage Utility", Meta = (ReturnDisplayName = "Transform"))
	static FTransform ExtractRootTransformFromMontage(const UAnimMontage* Montage, float Time);

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Montage Utility", Meta = (ReturnDisplayName = "Transform"))
	static FTransform ExtractLastRootTransformFromMontage(const UAnimMontage* Montage);

	// Stops all active animation montages that share at least one slot with the reference montage.
	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Montage Utility")
	static void StopMontagesWithAnySharedSlots(UAnimInstance* AnimationInstance, const UAnimMontage* ReferenceMontage,
	                                           float BlendOutDuration = -1.0f);
};
