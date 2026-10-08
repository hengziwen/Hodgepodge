#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ScalableFloat.h"
#include "HodgeCharacterStatProfile.generated.h"

class UGameplayEffect;

/** 角色等级到固有基础属性的不可变映射。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeCharacterStatProfile : public UDataAsset
{
	GENERATED_BODY()
public:
	UHodgeCharacterStatProfile();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="1000")) int32 MinLevel = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="1000")) int32 MaxLevel = 90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1")) int32 ConfigurationVersion = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FScalableFloat MaxHealth = FScalableFloat(100.f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FScalableFloat BaseDamage = FScalableFloat(20.f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UGameplayEffect> InitializationEffect;
	bool Evaluate(int32 Level, float& Health, float& Damage) const;
	bool Validate(TArray<FText>& Errors) const;
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
