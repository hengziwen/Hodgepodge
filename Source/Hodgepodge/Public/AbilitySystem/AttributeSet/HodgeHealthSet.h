
// 111屎山代码来袭

#pragma once

// UE 核心类型与基础宏。
#include "CoreMinimal.h"

// GAS AbilitySystemComponent 相关类型定义。
#include "AbilitySystemComponent.h"

// 项目 AttributeSet 基类，以及 ATTRIBUTE_ACCESSORS 和 FHodgeAttributeEvent。
#include "NativeGameplayTags.h"
#include "AbilitySystem/AttributeSet/HodgeAttributeSet.h"

#include "HodgeHealthSet.generated.h"

// UObject 前置声明。
class UObject;

// 蓝图 VM 调用栈结构前置声明。
struct FFrame;

// 普通伤害相关 GameplayTag。
HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);

// 伤害免疫 GameplayTag，拥有该 Tag 的目标可以阻止普通伤害。
HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);

// 自毁伤害 GameplayTag，用于标记主动自毁等特殊伤害来源。
HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);

// 掉出世界造成的伤害 GameplayTag。
HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);

// Hodge 项目的伤害消息 GameplayTag，用于标识或广播伤害相关 GameplayMessage。
HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Hodge_Damage_Message);

// GameplayEffect 修改 Attribute 前后回调时携带的数据结构。
struct FGameplayEffectModCallbackData;


/**
 * UHodgeHealthSet
 *
 *	Class that defines attributes that are necessary for taking damage.
 *	Attribute examples include: health, shields, and resistances.
 *
 * Hodge 项目的生命属性集。
 * 负责维护生命值、最大生命值，以及 Damage / Healing 等临时 Meta Attribute。
 *
 * Damage 和 Healing 本身不作为最终状态长期保存，
 * 而是在 GameplayEffect 执行后转换为对 Health 的实际修改。
 */
UCLASS(BlueprintType)
class HODGEPODGE_API UHodgeHealthSet : public UHodgeAttributeSet
{
	GENERATED_BODY()

public:
	// 构造生命属性集，并初始化各项生命相关 Attribute 的默认值。
	UHodgeHealthSet();

	// 生成 Health 对应的 GAS Attribute 访问函数。
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Health);

	// 生成 MaxHealth 对应的 GAS Attribute 访问函数。
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, MaxHealth);

	// 生成 Healing 对应的 GAS Attribute 访问函数。
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Healing);

	// 生成 Damage 对应的 GAS Attribute 访问函数。
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Damage);

	// Delegate when health changes due to damage/healing, some information may be missing on the client
	// Health 因伤害、治疗或其他效果发生变化时广播；客户端通过属性复制触发时可能缺少完整 GE 上下文。
	mutable FHodgeAttributeEvent OnHealthChanged;

	// Delegate when max health changes
	// MaxHealth 发生变化时广播。
	mutable FHodgeAttributeEvent OnMaxHealthChanged;

	// Delegate to broadcast when the health attribute reaches zero
	// Health 首次下降到 0 或以下时广播，用于驱动死亡等后续逻辑。
	mutable FHodgeAttributeEvent OnOutOfHealth;

protected:
	// Health 从服务器复制到客户端后触发，用于处理 GAS RepNotify 并广播生命值变化事件。
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	// MaxHealth 从服务器复制到客户端后触发，用于处理 GAS RepNotify 并广播最大生命值变化事件。
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	// GameplayEffect 真正修改 Attribute 之前调用，可在这里提前检查伤害免疫等条件。
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;

	// GameplayEffect 完成 Attribute 修改后调用，用于将 Damage / Healing 等 Meta Attribute 转换为实际 Health 变化。
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// Attribute BaseValue 即将发生变化前调用，用于对基础值进行合法范围限制。
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	// Attribute CurrentValue 即将发生变化前调用，用于对最终属性值进行合法范围限制。
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// Attribute 完成变化后调用，用于处理 MaxHealth 变化后修正 Health 等关联逻辑。
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	// 根据 Attribute 类型限制 NewValue 的合法范围，例如 Health 不超过 MaxHealth。
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
	// The current health attribute.  The health will be capped by the max health attribute.  Health is hidden from modifiers so only executions can modify it.
	// 当前生命值，最终值不能超过 MaxHealth，也不能低于生命系统允许的最小值。
	// HideFromModifiers 用于将 Health 从普通 Modifier 配置入口中隐藏，项目倾向通过执行计算或 Meta Attribute 间接修改 Health。
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Hodge|Health",
		Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current max health attribute.  Max health is an attribute since gameplay effects can modify it.
	// 当前最大生命值，本身也是 GAS Attribute，因此可以受到 GameplayEffect 等系统影响。
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Hodge|Health",
		Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// Used to track when the health reaches 0.
	// 记录当前是否已经进入生命值耗尽状态，避免 Health 持续为 0 时重复广播 OnOutOfHealth。
	bool bOutOfHealth;

	// Store the health before any changes 
	// 缓存 GameplayEffect 执行前的 MaxHealth，用于执行完成后计算实际变化并广播事件。
	float MaxHealthBeforeAttributeChange;

	// 缓存 GameplayEffect 执行前的 Health，用于执行完成后计算实际生命值变化并判断是否首次归零。
	float HealthBeforeAttributeChange;

	// -------------------------------------------------------------------
	//	Meta Attribute (please keep attributes that aren't 'stateful' below 
	// -------------------------------------------------------------------
	// 以下为 Meta Attribute。
	// Meta Attribute 通常只是 GameplayEffect / ExecutionCalculation 执行过程中的临时数据载体，
	// 不代表需要长期保存的角色状态，处理完成后通常会被消费并清零。

	// Incoming healing. This is mapped directly to +Health
	// 本次收到的治疗量，GameplayEffect 可以先写入 Healing，随后再将其转换为 Health 增量。
	UPROPERTY(BlueprintReadOnly, Category="Hodge|Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Healing;

	// Incoming damage. This is mapped directly to -Health
	// 本次收到的伤害量，GameplayEffect / ExecutionCalculation 可以先写入 Damage，
	// 随后在 PostGameplayEffectExecute 中将其转换为 Health 减少量。
	UPROPERTY(BlueprintReadOnly, Category="Hodge|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
	FGameplayAttributeData Damage;
};
