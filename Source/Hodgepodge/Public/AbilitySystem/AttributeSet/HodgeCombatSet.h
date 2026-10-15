/** @file HodgeCombatSet.h
 * @brief 声明战斗属性集相关的类型、接口与现有处理流程。
 */
// 111屎山代码来袭

#pragma once

// UE 核心类型与基础宏。
#include "CoreMinimal.h"

// GAS AbilitySystemComponent 以及 FGameplayAttributeData 等相关类型。
#include "AbilitySystemComponent.h"

// 项目自定义 AttributeSet 基类，并提供 ATTRIBUTE_ACCESSORS 等公共能力。
#include "HodgeAttributeSet.h"

#include "HodgeCombatSet.generated.h"

/**
 * UHodgeCombatSet
 *
 *  Class that defines attributes that are necessary for applying damage or healing.
 *	Attribute examples include: damage, healing, attack power, and shield penetrations.
 *
 * Hodge 项目的战斗基础属性集。
 * 负责保存伤害、治疗等战斗计算过程中需要使用的基础输入属性。
 *
 * 与 HealthSet 中 Damage / Healing 这类一次性的 Meta Attribute 不同，
 * CombatSet 中的 BaseDamage / BaseHeal 属于可以长期保存并参与 GameplayEffect 计算的战斗属性。
 */
UCLASS(BlueprintType)
class UHodgeCombatSet : public UHodgeAttributeSet
{
	GENERATED_BODY()

public:
	// 构造战斗属性集，并初始化战斗相关 Attribute 的默认值。
	/** 构造战斗属性集并初始化其默认状态。 */
	UHodgeCombatSet();

	// 生成 BaseDamage 对应的 GAS Attribute 访问函数。
	ATTRIBUTE_ACCESSORS(UHodgeCombatSet, BaseDamage);

	// 生成 BaseHeal 对应的 GAS Attribute 访问函数。
	ATTRIBUTE_ACCESSORS(UHodgeCombatSet, BaseHeal);
	/** 元素加成独立于基础攻击力，0.20表示湮灭伤害增加20%。 */
	ATTRIBUTE_ACCESSORS(UHodgeCombatSet, HavocDamageBonus);

protected:
	// BaseDamage 从服务器复制到客户端后触发，用于通知 GAS 属性系统完成网络同步。
	/** 收到基础伤害复制后更新本地状态。
	 * @param OldValue 旧值，由调用方传入。
	 */
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	// BaseHeal 从服务器复制到客户端后触发，用于通知 GAS 属性系统完成网络同步。
	/** 收到基础治疗复制后更新本地状态。
	 * @param OldValue 旧值，由调用方传入。
	 */
	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

private:
	/** 持续GE聚合增伤，效果移除后自动还原。 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_HavocDamageBonus, Category="Hodge|Combat", meta=(AllowPrivateAccess=true))
	FGameplayAttributeData HavocDamageBonus;
	/** 沿用GAS属性复制通知，不复制另一份增伤状态。 */
	UFUNCTION() void OnRep_HavocDamageBonus(const FGameplayAttributeData& OldValue);
	// The base amount of damage to apply in the damage execution.
	// 基础伤害值，通常作为伤害 ExecutionCalculation 计算最终伤害时的基础输入。
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Hodge|Combat",
		Meta = (AllowPrivateAccess = true))
	// 基础伤害的数据或对象引用。
	FGameplayAttributeData BaseDamage;

	// The base amount of healing to apply in the heal execution.
	// 基础治疗值，通常作为治疗 ExecutionCalculation 计算最终治疗量时的基础输入。
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Hodge|Combat",
		Meta = (AllowPrivateAccess = true))
	// 基础治疗的数据或对象引用。
	FGameplayAttributeData BaseHeal;
};
