// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "HodgeGameplayAbility.h"

#include "HodgeGameplayAbility_Jump.generated.h"

class UObject;
struct FFrame;
struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;


/**
 * UHodgeGameplayAbility_Jump
 *
 *	Gameplay ability used for character jumping.
 */
UCLASS(Abstract)
class UHodgeGameplayAbility_Jump : public UHodgeGameplayAbility
{
	GENERATED_BODY()

public:
	UHodgeGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	                                FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	void CharacterJumpStart();

	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	void CharacterJumpStop();
};
