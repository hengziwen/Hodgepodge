/** @file HodgeGameplayAbility_HitReaction.h
 * @brief 声明玩法能力命中受击相关的类型、接口与现有处理流程。
 */
#pragma once

#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "HodgeGameplayAbility_HitReaction.generated.h"

class UHodgeHitReactionComponent;
class UHodgeCombatComponentBase;
class USkeletalMeshComponent;
class ACharacter;

/** 目标的共享受击 GA；不消费攻击 Definition 或连段授权。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_HitReaction : public UHodgeGameplayAbility
{
	GENERATED_BODY()
public:
	/** 构造玩法能力命中受击并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeGameplayAbility_HitReaction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/** 把当前反应状态同步到技能控制与表现。
	 * @param State 当前状态数据。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool Synchronize(const FHodgeHitReactionState& State);
	/** 结束受击。
	 * @param bCancelled 是否已取消的控制标记。
	 */
	void FinishReaction(bool bCancelled = false);
protected:
	/** 检查技能激活所需的上下文、标签与现有GAS限制。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param SourceTags 来源标签，用于对应标签查询或匹配。
	 * @param TargetTags 目标标签，用于对应标签查询或匹配。
	 * @param RelevantTags Relevant标签，用于对应标签查询或匹配。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
		FGameplayTagContainer* RelevantTags) const override;
	/** 启动本次技能，按现有配置提交消耗并建立执行资源。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 */
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Payload) override;
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
	// 受击的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeHitReactionComponent> Reaction;
	// Played蒙太奇的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<UAnimMontage> PlayedMontage;
	// 受击表现角色的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<ACharacter> ReactionAvatar;
	// 姿势战斗的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeCombatComponentBase> PoseCombat;
	// 姿势网格的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<USkeletalMeshComponent> PoseMesh;
	// 本次动作持有的姿势占用句柄。
	FGuid PoseLease;
	// Saved姿势策略的数值或阈值。
	uint8 SavedPosePolicy = 0;
	// 已应用序号的数值或阈值。
	int32 AppliedSequence = 0;
	// 已应用Phase的枚举配置或当前取值。
	EHodgeHitReactionPhase AppliedPhase = EHodgeHitReactionPhase::None;
	// 已应用开始时刻的数值或阈值。
	float AppliedStartedAt = -1.f;
	// Motion来源标识的数据或对象引用。
	uint16 MotionSourceId = 0;
	// Queued抛飞速度的几何或变换数据。
	FVector QueuedLaunchVelocity = FVector::ZeroVector;
	// 控制是否Ending的状态标记。
	bool bEnding = false;
	/** 清空Motion。 */
	void ClearMotion();
};
