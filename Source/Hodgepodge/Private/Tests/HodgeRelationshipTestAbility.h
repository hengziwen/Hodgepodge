/** @file HodgeRelationshipTestAbility.h
 * @brief 声明标签关系测试专用能力相关的类型、接口与现有处理流程。
 */
#pragma once
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "HodgeRelationshipTestAbility.generated.h"

/** 关系测试使用的持久能力，无动画或场景副作用。 */
UCLASS(Transient, NotBlueprintable)
class UHodgeRelationshipTestAbility : public UHodgeGameplayAbility
{
	GENERATED_BODY()
public:
	/** 构造标签关系测试专用能力并初始化其默认状态。 */
	UHodgeRelationshipTestAbility();
protected:
	/** 启动本次技能，按现有配置提交消耗并建立执行资源。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 * @param Event 事件，由调用方传入。
	 */
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
};

/** 验证动作朝向的预测激活生命周期，无动画与资源副作用。 */
UCLASS(Transient, NotBlueprintable)
class UHodgeFacingPredictionTestAbility : public UHodgeRelationshipTestAbility
{
	GENERATED_BODY()
public:
	/** 构造朝向预测测试能力并初始化其默认状态。 */
	UHodgeFacingPredictionTestAbility();
};
