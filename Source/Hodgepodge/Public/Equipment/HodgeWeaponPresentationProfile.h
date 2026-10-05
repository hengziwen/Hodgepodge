#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HodgeWeaponPresentationProfile.generated.h"

class USkeletalMesh;
class UMaterialInterface;
class UCurveFloat;

/** 单件武器的表现配置，时间使用游戏秒，手部和背部姿势相对角色 Mesh 的插槽。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeWeaponPresentationProfile : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<USkeletalMesh> WeaponMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<TObjectPtr<UMaterialInterface>> Materials;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName HandSocket = TEXT("WeaponOnHand");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FTransform HandOffset = FTransform::Identity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName BackSocket = TEXT("WeaponOnBack");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ToolTip="Offset relative to the character Mesh BackSocket, not the Pawn root."))
	FTransform BackTransform = FTransform::Identity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FVector ReturnArcOffset = FVector(0.f, 15.f, 10.f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UCurveFloat> ReturnCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float DrawSeconds = .08f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float ReturnGraceSeconds = .06f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01")) float ReturnSeconds = .3f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float HoverSeconds = 2.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01")) float FadeSeconds = .35f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float HoverAmplitude = 2.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float HoverFrequency = .8f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName VisibilityParameter = TEXT("WeaponVisibility");
	bool Validate(TArray<FText>& Errors) const;
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
