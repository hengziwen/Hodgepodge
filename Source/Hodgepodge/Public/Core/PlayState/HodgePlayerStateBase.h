// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "HodgePlayerStateBase.generated.h"

class UHodgeAbilitySystemComponentBase;
class UAbilitySystemComponent;
class UAttributeSet;

/**
* 等级

名字

金币

队伍

排名
 */
UCLASS()
class HODGEPODGE_API AHodgePlayerStateBase : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AHodgePlayerStateBase();

	/** Ability System Component owned by PlayerState */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UHodgeAbilitySystemComponentBase* AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Initialize ASC for given avatar (Character)
	void InitializeAbilitySystemForCharacter(class ACharacter* Avatar);

	/** AttributeSet classes to create and add to ASC (set in defaults) */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<UAttributeSet>> AttributeSetClasses;

	/** Runtime instances of added AttributeSets */
	UPROPERTY()
	TArray<UAttributeSet*> AddedAttributeSets;
};