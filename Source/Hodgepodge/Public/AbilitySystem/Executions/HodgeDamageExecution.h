// 111屎山代码来袭

#pragma once

// UE 核心类型与基础宏。
#include "CoreMinimal.h"

// GAS 自定义 GameplayEffect ExecutionCalculation 基类。
// 用于实现需要通过代码计算最终伤害数值的 GameplayEffect 执行逻辑。
#include "GameplayEffectExecutionCalculation.h"

#include "HodgeDamageExecution.generated.h"

/**
 * UHodgeDamageExecution
 *
 * Hodge 项目的伤害数值执行计算类。
 *
 * 继承自 UGameplayEffectExecutionCalculation，
 * 用于在 GameplayEffect 执行阶段，根据捕获到的攻击属性、
 * 防御属性、GameplayTag、SetByCaller 等上下文计算最终伤害。
 *
 * 最终计算出的伤害通常会输出到 HealthSet 的 Damage Meta Attribute，
 * 再由 HealthSet::PostGameplayEffectExecute 将 Damage 转换为实际的 Health 扣除。
 */
UCLASS()
class HODGEPODGE_API UHodgeDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	// 构造伤害 ExecutionCalculation。
	// 通常在这里注册伤害计算过程中需要捕获的 Source / Target Attribute。
	UHodgeDamageExecution();

protected:
	// GAS 执行当前伤害 GameplayEffect 时调用的核心伤害计算函数。
	// ExecutionParams 提供 EffectSpec、Source / Target ASC、捕获属性等计算上下文。
	// OutExecutionOutput 用于输出最终需要应用到目标 Attribute 上的修改结果。
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
