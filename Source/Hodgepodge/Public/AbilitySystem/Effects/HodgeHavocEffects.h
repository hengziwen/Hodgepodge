/** @file HodgeHavocEffects.h @brief 暗流增量、容量、持续状态与独立燃料消耗效果。 */
#pragma once
#include "GameplayEffect.h"
#include "GameplayEffectExecutionCalculation.h"
#include "HodgeHavocEffects.generated.h"

/** Instant增量效果；金额必须由服务器受检收益或燃料结算提供。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeDarkFlowEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	/** 使用SetByCaller.DarkFlowDelta，不修改容量。 */
	UHodgeDarkFlowEffect();
};

/** 初始化只设置100容量，不因重复授予而恢复当前燃料。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeDarkFlowInitialization : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UHodgeDarkFlowInitialization();
};

/** 非周期持续状态；移除时由GAS撤销20%元素增伤。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeDarkSurgeEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UHodgeDarkSurgeEffect();
};

/** 周期效果只承担燃料扣除，不包含任何持续增伤Modifier。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeDarkFlowDrainEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UHodgeDarkFlowDrainEffect();
};

/** 依当前会话的实际服务器时间结算，卡顿补执行不能重复扣同一时间段。 */
UCLASS()
class HODGEPODGE_API UHodgeDarkFlowDrainExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	/** 只有有效规则来源及当前Avatar才输出负燃料。 */
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& Parameters,
		FGameplayEffectCustomExecutionOutput& Output) const override;
};
