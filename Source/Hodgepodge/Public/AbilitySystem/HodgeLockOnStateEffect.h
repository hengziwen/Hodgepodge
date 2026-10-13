/** @file HodgeLockOnStateEffect.h @brief 由锁定组件独占句柄的持续状态效果。 */
#pragma once
#include "GameplayEffect.h"
#include "HodgeLockOnStateEffect.generated.h"
/** 技能结束不移除此效果；解除、死亡和 Avatar 解绑移除。 */
UCLASS()
class HODGEPODGE_API UHodgeLockOnStateEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UHodgeLockOnStateEffect();
};
