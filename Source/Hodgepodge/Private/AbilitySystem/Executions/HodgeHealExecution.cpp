// 111屎山代码来袭


// UHodgeHealExecution 类定义。
#include "AbilitySystem/Executions/HodgeHealExecution.h"

// CombatSet 中定义了 BaseHeal 等战斗基础属性。
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"

// HealthSet 中定义了 Healing Meta Attribute，用于接收最终治疗计算结果。
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHealExecution)


// 集中保存当前治疗 ExecutionCalculation 需要使用的 Attribute 捕获定义。
struct FHealStatics
{
	// BaseHeal 的 Attribute 捕获定义。
	FGameplayEffectAttributeCaptureDefinition BaseHealDef;

	FHealStatics()
	{
		// 捕获 Source ASC 上 CombatSet 的 BaseHeal 属性。
		// Source 表示该属性来自 GameplayEffect 的施加者。
		// 最后的 true 表示使用 Snapshot，在 GameplayEffectSpec 创建时捕获并固定该属性值。
		BaseHealDef = FGameplayEffectAttributeCaptureDefinition(UHodgeCombatSet::GetBaseHealAttribute(),
		                                                        EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

// 获取全局唯一的治疗属性捕获定义集合。
static FHealStatics& HealStatics()
{
	// 使用函数内静态对象，避免每次执行治疗计算时重复创建 CaptureDefinition。
	static FHealStatics Statics;

	// 返回治疗计算使用的静态捕获定义。
	return Statics;
}


// 构造治疗 ExecutionCalculation。
UHodgeHealExecution::UHodgeHealExecution()
{
	// 告诉 GAS 当前 ExecutionCalculation 需要捕获 BaseHeal 属性。
	RelevantAttributesToCapture.Add(HealStatics().BaseHealDef);
}

// 执行实际的治疗数值计算。
void UHodgeHealExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                 FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	// 获取当前正在执行的 GameplayEffectSpec。
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// 获取 GameplayEffectSpec 捕获到的 Source GameplayTag。
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();

	// 获取 GameplayEffectSpec 捕获到的 Target GameplayTag。
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 构造 Attribute 聚合器计算参数。
	// 捕获属性在计算最终 Magnitude 时，可以根据 Source / Target Tag 应用对应 Modifier。
	FAggregatorEvaluateParameters EvaluateParameters;

	// 将 Source Tag 提供给 Attribute 聚合器。
	EvaluateParameters.SourceTags = SourceTags;

	// 将 Target Tag 提供给 Attribute 聚合器。
	EvaluateParameters.TargetTags = TargetTags;

	// 保存从 Source 捕获并计算得到的基础治疗量。
	float BaseHeal = 0.0f;

	// 根据 BaseHealDef 和当前 Source / Target Tag，
	// 计算最终捕获到的 BaseHeal Magnitude，并写入 BaseHeal。
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealDef, EvaluateParameters, BaseHeal);

	// 最终治疗量最低限制为 0，避免负数 BaseHeal 被当成治疗结果继续输出。
	const float HealingDone = FMath::Max(0.0f, BaseHeal);

	// 只有存在有效治疗量时才向 ExecutionOutput 添加属性修改。
	if (HealingDone > 0.0f)
	{
		// Apply a healing modifier, this gets turned into + health on the target
		// 将最终治疗量以 Additive 方式写入目标 HealthSet 的 Healing Meta Attribute。
		// Healing 本身不是最终 Health，而会在 HealthSet::PostGameplayEffectExecute 中被转换为 +Health。
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(UHodgeHealthSet::GetHealingAttribute(), EGameplayModOp::Additive,
			                               HealingDone));
	}
#endif // #if WITH_SERVER_CODE
}
