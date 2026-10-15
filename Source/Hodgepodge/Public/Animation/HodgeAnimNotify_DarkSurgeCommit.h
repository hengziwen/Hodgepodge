/** @file HodgeAnimNotify_DarkSurgeCommit.h @brief 将灭音点伤害和随后入态放在同一个正式动画分支点。 */
#pragma once
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "HodgeAnimNotify_DarkSurgeCommit.generated.h"

/** 显式专用提交不允许普通攻击通知意外建立暗涌。 */
UCLASS(meta=(DisplayName="Hodge 灭音提交"))
class HODGEPODGE_API UHodgeAnimNotify_DarkSurgeCommit : public UAnimNotify
{
	GENERATED_BODY()
public:
	/** 默认角色中心的固定Box；尺寸与实际通知帧由Montage配置。 */
	UHodgeAnimNotify_DarkSurgeCommit();
	UPROPERTY(EditAnywhere, Category="Hodge|Havoc") FHodgeAnimHitConfig Burst;
	/** 通知必须匹配当前Avatar与Montage实例才进入技能提交。 */
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& Payload) override;
};
