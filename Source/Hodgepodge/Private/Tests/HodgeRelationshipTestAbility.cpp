/** @file HodgeRelationshipTestAbility.cpp
 * @brief 实现标签关系测试专用能力相关的类型、接口与现有处理流程。
 */
#include "HodgeRelationshipTestAbility.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeRelationshipTestAbility)

/** 构造标签关系测试专用能力并初始化其默认状态。 */
UHodgeRelationshipTestAbility::UHodgeRelationshipTestAbility()
{ NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly; }
/** 启动本次技能，按现有配置提交消耗并建立执行资源。 */
void UHodgeRelationshipTestAbility::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event)
{}

/** 构造朝向预测测试能力并初始化其默认状态。 */
UHodgeFacingPredictionTestAbility::UHodgeFacingPredictionTestAbility()
{ NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted; bRequiresInitializedAttributes = false; }
