#include "HodgeRelationshipTestAbility.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeRelationshipTestAbility)

UHodgeRelationshipTestAbility::UHodgeRelationshipTestAbility()
{ NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly; }
void UHodgeRelationshipTestAbility::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event)
{}

UHodgeFacingPredictionTestAbility::UHodgeFacingPredictionTestAbility()
{ NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted; bRequiresInitializedAttributes = false; }
