/** @file HodgeAttackWarpTestTypes.cpp @brief 仅隔离动画启动，保留真实 GAS 的活动身份和来源。 */
#include "HodgeAttackWarpTestTypes.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAttackWarpTestTypes)
bool UHodgeAttackWarpTestAbility::CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	const FGameplayTagContainer* Source, const FGameplayTagContainer* Target, FGameplayTagContainer* Relevant) const
{ return UGameplayAbility::CanActivateAbility(Handle,Info,Source,Target,Relevant); }
void UHodgeAttackWarpTestAbility::ActivateAbility(FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo*, FGameplayAbilityActivationInfo, const FGameplayEventData*) {}
