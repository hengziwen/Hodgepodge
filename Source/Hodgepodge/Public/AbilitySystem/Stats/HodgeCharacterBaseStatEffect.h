/** @file HodgeCharacterBaseStatEffect.h
 * @brief 声明角色基础属性效果相关的类型、接口与现有处理流程。
 */
#pragma once

#include "GameplayEffect.h"
#include "HodgeCharacterBaseStatEffect.generated.h"

/** 一次性写角色基础值，不保留可撤销的活动效果。 */
UCLASS()
class HODGEPODGE_API UHodgeCharacterBaseStatEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	/** 构造角色基础属性效果并初始化其默认状态。 */
	UHodgeCharacterBaseStatEffect();
};
