/** @file HodgeGameplayAbility_EnemyDeath.h
 * @brief 声明玩法能力敌人死亡相关的类型、接口与现有处理流程。
 */
#pragma once
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Death.h"
#include "HodgeGameplayAbility_EnemyDeath.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UHodgeCombatComponentBase;
class UAnimMontage;

/** 普通怪物的可配置死亡表现；死亡状态及销毁仍由既有 Death/Health 链路管理。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_EnemyDeath : public UHodgeGameplayAbility_Death
{
	GENERATED_BODY()
public:
	// 死亡蒙太奇的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Death") TObjectPtr<UAnimMontage> DeathMontage;
protected:
	/** 启动本次技能，按现有配置提交消耗并建立执行资源。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 * @param Event 事件，由调用方传入。
	 */
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
	/** 结束本次技能并释放其拥有的资源，再交还GAS生命周期。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 * @param bReplicate 是否Replicate的控制标记。
	 * @param bCancelled 是否已取消的控制标记。
	 */
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
private:
	/** 结束蒙太奇。 */
	UFUNCTION() void FinishMontage();
	/** 检查按住死亡姿势。 */
	void CheckHeldDeathPose();
	// 按住姿势计时器对应操作的配对句柄。
	FTimerHandle HeldPoseTimer;
	// 任务的数据或对象引用。
	UPROPERTY() TObjectPtr<UAbilityTask_PlayMontageAndWait> Task;
	// 统一战斗组件。
	TWeakObjectPtr<UHodgeCombatComponentBase> Combat;
	// 本次动作持有的姿势占用句柄。
	FGuid PoseLease;
};
