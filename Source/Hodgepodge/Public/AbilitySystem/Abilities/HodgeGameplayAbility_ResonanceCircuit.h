/** @file HodgeGameplayAbility_ResonanceCircuit.h @brief 灭音复用近战执行，只增加满燃料资格与一次入态提交。 */
#pragma once
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
#include "HodgeGameplayAbility_ResonanceCircuit.generated.h"

/** 共鸣回路攻击不扣空资源，不承担跨多个动作的持续状态。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_ResonanceCircuit : public UHodgeGameplayAbility_Melee
{
	GENERATED_BODY()
public:
	/** 校验当前资源、形态与现有动作取消窗口，成本和CD仍走父类。 */
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* Info,
		const FGameplayTagContainer* SourceTags=nullptr,const FGameplayTagContainer* TargetTags=nullptr,FGameplayTagContainer* RelevantTags=nullptr) const override;
	/** 分支点通知的服务器提交；重复通知不重放范围伤害或重新建立Buff。 */
	bool CommitDarkSurge(const FBranchingPointNotifyPayload& Payload,const FHodgeAnimHitConfig& Burst);
	/** 入口限制放在角色具体蓝图配置，不能从资源满条推导空中许可。 */
	UPROPERTY(EditDefaultsOnly, Category="Hodge|Havoc") bool bRequireGrounded=true;
	/** 当前攻击必须开放此窗口才允许灭音切入；无动作时不要求窗口。 */
	UPROPERTY(EditDefaultsOnly, Category="Hodge|Havoc") FGameplayTag EntryCancelWindow;
protected:
	/** 每次执行独立锁存，不把上一次提交状态带到缓存GA实例。 */
	virtual void OnExecutionReady() override;
	/** PostPhysics查询结算后再入态，灭音本刀不提前获得本次20%增伤。 */
	virtual void ProcessMeleeHitResults_Implementation(const FHodgeHitDetectionBatch& Batch) override;
private:
	bool bCommitted=false;
	int32 BurstOccurrence=INDEX_NONE;
};
