/** @file HodgeGameplayAbility_Death.h
 * @brief 声明玩法能力死亡相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "HodgeGameplayAbility.h"

#include "HodgeGameplayAbility_Death.generated.h"

class UObject;
struct FFrame;
struct FGameplayAbilityActorInfo;
struct FGameplayEventData;


/**
 * UHodgeGameplayAbility_Death
 *
 *	Gameplay ability used for handling death.
 *	Ability is activated automatically via the "GameplayEvent.Death" ability trigger tag.
 */
/** 封装玩法能力死亡的状态与操作接口。 */
UCLASS(Abstract)
class UHodgeGameplayAbility_Death : public UHodgeGameplayAbility
{
	GENERATED_BODY()

public:
	/** 构造玩法能力死亡并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeGameplayAbility_Death(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	/** 启动本次技能，按现有配置提交消耗并建立执行资源。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 * @param TriggerEventData Trigger事件数据，由调用方传入。
	 */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
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

	// Starts the death sequence.
	/** 进入死亡开始阶段并应用既有死亡限制。 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	void StartDeath();

	// Finishes the death sequence.
	/** 结束死亡。 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	void FinishDeath();

protected:
	// If enabled, the ability will automatically call StartDeath.  FinishDeath is always called when the ability ends if the death was started.
	// 控制是否自动起始死亡的状态标记。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Death")
	bool bAutoStartDeath;
};
