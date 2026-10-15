/** @file HodgeAnimNotifyState_WingAction.h
 * @brief 定义角色攻击期间的翅膀显示窗口，不保存角色运行状态。
 */
#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "HodgeAnimNotifyState_WingAction.generated.h"

class UAnimMontage;
class USkeletalMesh;

/** 通知条定义显示窗口，通知属性直接定义本次翅膀蒙太奇。 */
UCLASS(meta=(DisplayName="Hodge Wing Action Window"))
class HODGEPODGE_API UHodgeAnimNotifyState_WingAction : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	/** 初始化分支点通知属性。 */
	UHodgeAnimNotifyState_WingAction();
	/** 在独立翅膀 AnimInstance 上播放，不占用角色 ASC 的主蒙太奇。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing") TObjectPtr<UAnimMontage> WingMontage;
	/** 相对角色攻击源时间的播放倍率，攻击暂停或变速时同步。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing", meta=(ClampMin="0.01")) float PlayRate = 1.f;
	/** 通知开始时的翅膀蒙太奇位置，单位秒。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing", meta=(ClampMin="0", Units="s")) float StartPosition = 0.f;
	/** -1 使用 Profile 默认淡出，零立即隐藏，正值覆盖本次淡出秒数。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing", meta=(ClampMin="-1", Units="s")) float FadeOutSeconds = -1.f;
	/** 仅保留旧资产序列化，不再作为正式配置入口。 */
	UPROPERTY() FName WindowId = TEXT("AttackWing");
	/** 校验资源、骨架和有限参数，失败信息由组件统一报告。 */
	bool Validate(const USkeletalMesh* Mesh, TArray<FText>& Errors) const;
	/** 将通知内经过的角色源秒转换为有界翼蒙太奇秒。 */
	float EvaluateWingTime(float SourceTime, float WindowStart) const;
	/** 在时间轴上显示实际选用的翼蒙太奇名称。 */
	virtual FString GetNotifyName_Implementation() const override;
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
