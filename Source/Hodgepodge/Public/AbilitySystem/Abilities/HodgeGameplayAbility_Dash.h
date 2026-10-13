/** @file HodgeGameplayAbility_Dash.h
 * @brief 声明玩法能力Dash冲刺相关的类型、接口与现有处理流程。
 */
#pragma once

#include "AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h"
#include "HodgeGameplayAbility_Dash.generated.h"
class UAbilityTask_ApplyRootMotionConstantForce;
class UAbilityTask_PlayMontageAndWait;

/** 封装玩法能力Dash冲刺的状态与操作接口。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_Dash : public UHodgeGameplayAbility_MovementAction
{
	GENERATED_BODY()
public:
	/** 构造玩法能力Dash冲刺并初始化其默认状态。 */
	UHodgeGameplayAbility_Dash();
	/** 获取Dash冲刺Directions。
	 * @return Dash冲刺Directions的当前值或引用。
	 */
	UFUNCTION(BlueprintPure) FHodgeDashDirections GetDashDirections() const { return Directions; }
	/** 完成Dash到Sprint的会话交接并释放交接记录。 */
	void CompleteHandoff();
	/** 检查消耗。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param Tags 标签，用于对应标签查询或匹配。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool CheckCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayTagContainer* Tags = nullptr) const override;
protected:
	/** 开始移动。
	 * @param Intent 意图，由调用方传入。
	 */
	virtual void BeginMovement(const FHodgeMoveIntentSnapshot& Intent) override;
	/** 更新移动。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	virtual void UpdateMovement(float DeltaTime) override;
	/** 结束本次技能并释放其拥有的资源，再交还GAS生命周期。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 * @param bReplicate 是否Replicate的控制标记。
	 * @param bCancelled 是否已取消的控制标记。
	 */
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
private:
	// Dash 自己持有自由移动基础占用，结束或交接只释放本句柄。
	FHodgeFacingRequestHandle DashBaseFacing;
	/** 提交Dash冲刺。 */
	void CommitDash();
	/** 处理提交Requested。 */
	void OnCommitRequested();
	/** 准备移动表现退出。
	 * @param MaximumSpeed 最大速度，由调用方传入。
	 */
	void PrepareLocomotionExit(float MaximumSpeed);
	/** 处理蒙太奇混合输出。 */
	UFUNCTION() void OnMontageBlendOut();
	// 提交Requested句柄对应的事件通知。
	FDelegateHandle CommitRequestedHandle;
	/** 处理蒙太奇Interrupted。 */
	UFUNCTION() void OnMontageInterrupted();
	/** 处理蒙太奇已完成。 */
	UFUNCTION() void OnMontageCompleted();
	// Directions的数据或对象引用。
	FHodgeDashDirections Directions;
	// 防御句柄的数据或对象引用。
	FGuid DefenseHandle;
	// MotionInstance名称的数据或对象引用。
	FName MotionInstanceName;
	// 控制是否HandoffOpened的状态标记。
	bool bHandoffOpened = false;
	// 控制是否HandedOff的状态标记。
	bool bHandedOff = false;
	// 控制是否HandoffRequested的状态标记。
	bool bHandoffRequested = false;
	// 控制是否Ending的状态标记。
	bool bEnding = false;
	// 动画Ends时刻的数值或阈值。
	double AnimationEndsAt = 0.;
	// Motion任务的数据或对象引用。
	UPROPERTY(Transient) TObjectPtr<UAbilityTask_ApplyRootMotionConstantForce> MotionTask;
	// 蒙太奇任务的数据或对象引用。
	UPROPERTY(Transient) TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
};
