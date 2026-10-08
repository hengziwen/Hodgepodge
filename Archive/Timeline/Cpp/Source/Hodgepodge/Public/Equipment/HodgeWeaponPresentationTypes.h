#pragma once

#include "CoreMinimal.h"
#include "HodgeWeaponPresentationTypes.generated.h"

UENUM(BlueprintType)
enum class EHodgeWeaponPresentationPhase : uint8
{
	Hidden,
	Hand,
	Returning,
	Hovering,
	Fading
};

/** 持续表现的复制快照，身份由所属 WeaponInstance 和本次激活 Key 共同确定。 */
USTRUCT(BlueprintType)
struct FHodgeWeaponPresentationState
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) EHodgeWeaponPresentationPhase Phase = EHodgeWeaponPresentationPhase::Hidden;
	UPROPERTY(BlueprintReadOnly) double StartTime = 0;
	UPROPERTY(BlueprintReadOnly) FTransform StartRelativeTransform = FTransform::Identity;
	UPROPERTY(BlueprintReadOnly) float StartVisibility = 0.f;
	UPROPERTY(BlueprintReadOnly) int32 Revision = 0;
	UPROPERTY(BlueprintReadOnly) int32 ActivationKey = 0;
};
