#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ScalableFloat.h"
#include "HodgeEquipmentStatProfile.generated.h"

class UGameplayEffect;

/** 每件装备按自身等级计算的可撤销加成。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeEquipmentStatProfile : public UDataAsset
{
	GENERATED_BODY()
public:
	UHodgeEquipmentStatProfile();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="1000")) int32 MinLevel = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="1000")) int32 MaxLevel = 90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FScalableFloat MaxHealthBonus = FScalableFloat(0.f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FScalableFloat BaseDamageBonus = FScalableFloat(0.f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UGameplayEffect> AttributeEffect;
	bool Evaluate(int32 Level, float& Health, float& Damage) const;
	bool Validate(TArray<FText>& Errors) const;
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
