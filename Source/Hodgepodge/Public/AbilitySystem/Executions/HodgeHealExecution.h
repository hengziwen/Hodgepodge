// 111屎山代码来袭

#pragma once

// UE 核心类型与基础宏。
#include "CoreMinimal.h"

// GAS 自定义 GameplayEffect ExecutionCalculation 基类。
// 用于实现需要通过代码动态计算最终数值的 GameplayEffect 执行逻辑。
#include "GameplayEffectExecutionCalculation.h"

#include "HodgeHealExecution.generated.h"

/**
 * UHodgeHealExecution
 *
 * Hodge 项目的治疗数值执行计算类。
 *
 * 继承自 UGameplayEffectExecutionCalculation，
 * 用于在 GameplayEffect 执行阶段根据捕获到的 Attribute、
 * SetByCaller、GameplayTag 等信息计算最终治疗量。
 *
 * 计算结果通常会输出到 HealthSet 的 Healing Meta Attribute，
 * 再由 HealthSet::PostGameplayEffectExecute 将 Healing 转换为实际 Health 增加。
 */
UCLASS()
class HODGEPODGE_API UHodgeHealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	// 构造治疗 ExecutionCalculation，
	// 通常在这里注册本次计算需要捕获的 Attribute。
	UHodgeHealExecution();

protected:
	// GAS 执行当前治疗 GameplayEffect 时调用的核心计算函数。
	// ExecutionParams 提供 Source / Target ASC、EffectSpec、捕获属性等计算上下文。
	// OutExecutionOutput 用于输出最终需要修改的 Attribute 数值。
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
