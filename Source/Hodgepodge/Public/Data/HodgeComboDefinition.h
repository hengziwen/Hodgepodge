#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "HodgeComboDefinition.generated.h"

USTRUCT(BlueprintType)
struct FHodgeComboTransition
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag TriggerInputIntentTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag TriggerEventTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag TargetComboTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer RequiredWindowTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer RequiredSourceTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer BlockedSourceTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 TransitionPriority = 0;
};

USTRUCT(BlueprintType)
struct FHodgeComboRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag ComboTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag AbilityTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer GrantedTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FHodgeComboTransition> Transitions;
};

USTRUCT(BlueprintType)
struct FHodgeComboInputBinding
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag InputTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag IntentTag;
};

UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeComboDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UDataTable> ComboTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag EntryComboTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FHodgeComboInputBinding> InputBindings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01")) float InputBufferSeconds = 0.3f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag MoveCancelWindowTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float MoveIntentThreshold = 0.1f;
	const FHodgeComboRow* FindNode(FGameplayTag Tag) const;
	bool ValidateDefinition(TArray<FText>& Errors) const;
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
