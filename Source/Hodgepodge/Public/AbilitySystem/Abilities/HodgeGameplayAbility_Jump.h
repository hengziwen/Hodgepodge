/** @file HodgeGameplayAbility_Jump.h
 * @brief 声明玩法能力跳跃相关的类型、接口与现有处理流程。
 */
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
/** 封装玩法能力跳跃的状态与操作接口。 */
UCLASS(Abstract)
class UHodgeGameplayAbility_Jump : public UHodgeGameplayAbility
{
	GENERATED_BODY()

public:
	/** 构造玩法能力跳跃并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	/** 检查技能激活所需的上下文、标签与现有GAS限制。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param SourceTags 来源标签，用于对应标签查询或匹配。
	 * @param TargetTags 目标标签，用于对应标签查询或匹配。
	 * @param OptionalRelevantTags OptionalRelevant标签，用于对应标签查询或匹配。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	                                FGameplayTagContainer* OptionalRelevantTags) const override;
	/** 结束本次技能并释放其拥有的资源，再交还GAS生命周期。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 * @param bReplicateEndAbility 是否Replicate结束能力的控制标记。
	 * @param bWasCancelled 是否Was已取消的控制标记。
	 */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	/** 按当前角色状态开始跳跃。 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	void CharacterJumpStart();

	/** 停止当前跳跃输入或保持状态。 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	void CharacterJumpStop();
};
