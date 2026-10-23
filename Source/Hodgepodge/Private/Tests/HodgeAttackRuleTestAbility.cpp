/** @file HodgeAttackRuleTestAbility.cpp @brief 原生回归夹具没有动画、资源与场景副作用。 */
#include "HodgeAttackRuleTestAbility.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAttackRuleTestAbility)

/** 服务器执行确保所有收益使用真实权威 ASC。 */
UHodgeAttackRuleTestAbility::UHodgeAttackRuleTestAbility()
{ NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly; bRequiresInitializedAttributes = false; }

/** 夹具绕过 Definition 动画配置，其余 GAS 激活条件仍由引擎检查。 */
bool UHodgeAttackRuleTestAbility::CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* RelevantTags) const
{ return UGameplayAbility::CanActivateAbility(Handle, Info, SourceTags, TargetTags, RelevantTags); }

/** 激活状态由 GAS 建立，执行身份和规则由测试访问器配置。 */
void UHodgeAttackRuleTestAbility::ActivateAbility(FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo*, FGameplayAbilityActivationInfo, const FGameplayEventData*) {}
