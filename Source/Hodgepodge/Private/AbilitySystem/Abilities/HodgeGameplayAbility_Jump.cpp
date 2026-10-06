// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h"

#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "Character/HodgeCombatCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_Jump)

struct FGameplayTagContainer;


UHodgeGameplayAbility_Jump::UHodgeGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UHodgeGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayTagContainer* SourceTags,
                                                    const FGameplayTagContainer* TargetTags,
                                                    FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const AHodgeCombatCharacter* HodgeCharacter = Cast<AHodgeCombatCharacter>(ActorInfo->AvatarActor.Get());
	if (!HodgeCharacter || !HodgeCharacter->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UHodgeGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop jumping in case the ability blueprint doesn't call it.
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHodgeGameplayAbility_Jump::CharacterJumpStart()
{
	if (AHodgeCombatCharacter* HodgeCharacter = GetHodgeCharacterFromActorInfo())
	{
		if (HodgeCharacter->IsLocallyControlled() && !HodgeCharacter->bPressedJump)
		{
			HodgeCharacter->UnCrouch();
			HodgeCharacter->Jump();
		}
	}
}

void UHodgeGameplayAbility_Jump::CharacterJumpStop()
{
	if (AHodgeCombatCharacter* HodgeCharacter = GetHodgeCharacterFromActorInfo())
	{
		if (HodgeCharacter->IsLocallyControlled() && HodgeCharacter->bPressedJump)
		{
			HodgeCharacter->StopJumping();
		}
	}
}
