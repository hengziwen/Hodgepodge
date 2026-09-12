// 111屎山代码来袭

#pragma once

// UE 核心类型与基础宏。
#include "CoreMinimal.h"

// GAS AbilitySystemComponent 相关类型定义。
#include "AbilitySystemComponent.h"

// 项目 AttributeSet 基类，以及 ATTRIBUTE_ACCESSORS 和 FHodgeAttributeEvent。
#include "AbilitySystem/AttributeSet/HodgeAttributeSet.h"

#include "HodgeHealthSet.generated.h"

/**
 * UHodgeHealthSet
 *
 * 负责管理角色生命、最大生命、治疗、伤害以及基础伤害/治疗等战斗属性。
 * Health / MaxHealth 属于持续存在的状态属性。
 * Healing / Damage 属于临时 Meta 属性，用于接收一次 GameplayEffect 计算结果。
 */
UCLASS()
class HODGEPODGE_API UHodgeHealthSet : public UHodgeAttributeSet
{
	GENERATED_BODY()

public:
	// 构造函数。
	UHodgeHealthSet();

	// 生成 Health 属性对应的 Getter、Setter、Init 和 FGameplayAttribute 获取函数。
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Health);

	// 生成 MaxHealth 属性对应的辅助访问函数。
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, MaxHealth);

	// 生成 Healing Meta 属性对应的辅助访问函数。
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Healing);

	// 生成 Damage Meta 属性对应的辅助访问函数。
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Damage);

	// 生成 BaseDamage 属性对应的辅助访问函数。
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, BaseDamage);

	// 生成 BaseHeal 属性对应的辅助访问函数。
	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, BaseHeal);

	// Delegate when 生命 changes due to damage/healing, some information may be missing on the client
	// 当 Health 因伤害或治疗发生变化时广播；客户端可能拿不到完整的 GameplayEffect 上下文。
	mutable FHodgeAttributeEvent OnHealthChanged;

	// Delegate when max 生命 changes
	// 当 MaxHealth 发生变化时广播。
	mutable FHodgeAttributeEvent OnMaxHealthChanged;

	// Delegate to broadcast when the 生命 attribute reaches zero
	// 当 Health 降到 0 时广播，用于触发死亡等后续逻辑。
	mutable FHodgeAttributeEvent OnOutOfHealth;

protected:
	// Health 网络复制到客户端后触发，用于处理属性变化通知。
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	// MaxHealth 网络复制到客户端后触发。
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	// BaseDamage 网络复制到客户端后触发。
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	// BaseHeal 网络复制到客户端后触发。
	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

	// GameplayEffect 真正修改属性前调用，可用于检查或缓存执行前状态。
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;

	// GameplayEffect 修改属性完成后调用，通常在这里将 Damage / Healing 转换到 Health。
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// 属性 BaseValue 修改前调用，用于限制基础值范围。
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	// 属性 CurrentValue 修改前调用，用于限制最终属性值范围。
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// 属性值真正发生变化后调用，可用于处理 MaxHealth 变化、死亡状态恢复等逻辑。
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	// 根据属性类型对 NewValue 执行统一 Clamp。
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
	// The current 生命 attribute.  The 生命 will be capped by the max 生命 attribute.  生命 is hidden from modifiers so only executions can modify it.
	// 当前生命值，始终限制在 MaxHealth 范围内。
	// HideFromModifiers 表示不希望普通 Modifier 直接修改 Health，通常通过 Damage / Healing 等 Meta 属性间接修改。
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Hodge|Health",
		Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current max 生命 attribute.  Max 生命 is an attribute since gameplay effects can modify it.
	// 当前最大生命值，因为最大生命可能受到 Buff / Debuff 修改，所以本身也是 GameplayAttribute。
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Hodge|Health",
		Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// Used to track when the 生命 reaches 0.
	// 记录当前是否已经进入过 Health <= 0 状态，避免重复广播死亡事件。
	bool bOutOfHealth;

	// Store the 生命 before any changes 
	// 保存属性变化前的 MaxHealth，用于后续事件广播或变化比较。
	float MaxHealthBeforeAttributeChange;

	// 保存属性变化前的 Health，用于计算实际生命变化。
	float HealthBeforeAttributeChange;

	// -------------------------------------------------------------------
	// Meta 属性 (please keep attributes that aren't 'stateful' below 
	// 以下属于 Meta 属性或中间计算属性，不用于长期保存角色状态。
	// -------------------------------------------------------------------

	// Incoming healing. This is mapped directly to +生命
	// 本次收到的治疗量，通常在 PostGameplayEffectExecute 中转换成 Health 增量后再清零。
	UPROPERTY(BlueprintReadOnly, Category="Hodge|Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Healing;

	// Incoming damage. This is mapped directly to -生命
	// 本次收到的伤害量，通常在 PostGameplayEffectExecute 中转换成 Health 扣减后再清零。
	UPROPERTY(BlueprintReadOnly, Category="Hodge|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
	FGameplayAttributeData Damage;

	// Base combat attributes
	// 基础战斗伤害属性，可作为伤害计算公式中的基础输入值。
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Hodge|Combat",
		Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	// 基础治疗属性，可作为治疗计算公式中的基础输入值。
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Hodge|Combat",
		Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;
};
