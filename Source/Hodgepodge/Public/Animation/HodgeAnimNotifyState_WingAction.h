/** @file HodgeAnimNotifyState_WingAction.h
 * @brief 定义角色攻击期间的翅膀显示窗口，不保存角色运行状态。
 */
#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "HodgeAnimNotifyState_WingAction.generated.h"

/** 通知只请求组件刷新，真实显隐由当前源时间和配置窗口决定。 */
UCLASS(meta=(DisplayName="Hodge Wing Action Window"))
class HODGEPODGE_API UHodgeAnimNotifyState_WingAction : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	/** 初始化分支点通知属性。 */
	UHodgeAnimNotifyState_WingAction();
	/** 对应 Profile 动画映射中的窗口标识。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing") FName WindowId = TEXT("AttackWing");
	/** 处理普通通知开始，Mesh 为拥有本次源动画的网格。 */
	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float Duration, const FAnimNotifyEventReference& Reference) override;
	/** 处理普通通知结束，迟到结束只刷新当前源。 */
	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
	/** 处理带蒙太奇实例身份的同步开始通知。 */
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
	/** 处理带蒙太奇实例身份的同步结束通知。 */
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;
private:
	/** 校验通知网格所有者，只刷新该角色的组件。 */
	void RefreshOwner(USkeletalMeshComponent* Mesh) const;
};
