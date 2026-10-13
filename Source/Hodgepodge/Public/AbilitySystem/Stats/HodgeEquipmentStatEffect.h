/** @file HodgeEquipmentStatEffect.h
 * @brief 声明装备属性效果相关的类型、接口与现有处理流程。
 */
#pragma once

#include "GameplayEffect.h"
#include "HodgeEquipmentStatEffect.generated.h"

/** 每件装备独立持有的非周期加成。 */
UCLASS()
class HODGEPODGE_API UHodgeEquipmentStatEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	/** 构造装备属性效果并初始化其默认状态。 */
	UHodgeEquipmentStatEffect();
};
