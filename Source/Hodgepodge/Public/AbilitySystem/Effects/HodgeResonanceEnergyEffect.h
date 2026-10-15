/** @file HodgeResonanceEnergyEffect.h @brief 提供可蓝图派生的充能增量和容量初始化GE。 */
#pragma once
#include "GameplayEffect.h"
#include "HodgeResonanceEnergyEffect.generated.h"

/** Instant增量效果供服务器回能和预测成本共用，不持有计时器。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeResonanceEnergyEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	/** 只修改ResonanceEnergy，金额通过SetByCaller.ResonanceEnergyDelta传入。 */
	UHodgeResonanceEnergyEffect();
};

/** 初始化只设置容量，重复授予不能恢复已经花掉的充能。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeResonanceEnergyInitialization : public UGameplayEffect
{
	GENERATED_BODY()
public:
	/** 初始容量100保留在GE修饰器中，蓝图资产可配置。 */
	UHodgeResonanceEnergyInitialization();
};
