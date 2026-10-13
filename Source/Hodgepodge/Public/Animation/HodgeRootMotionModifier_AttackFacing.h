/** @file HodgeRootMotionModifier_AttackFacing.h @brief 单招固定 Yaw 的旋转 Skew Warp。 */
#pragma once
#include "RootMotionModifier_SkewWarp.h"
#include "HodgeRootMotionModifier_AttackFacing.generated.h"
/** 配置留在 Definition，窗口留在 Montage；不追加平移、不绕过旋转约束。 */
UCLASS(meta=(DisplayName="Hodge Attack Facing Warp"))
class HODGEPODGE_API UHodgeRootMotionModifier_AttackFacing : public URootMotionModifier_SkewWarp
{
	GENERATED_BODY()
public:
	virtual void Update(const FMotionWarpingUpdateContext& Context) override;
};
