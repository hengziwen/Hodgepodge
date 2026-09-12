
 // * @file HodgeAttributeSet.h
 // * @brief UHodgeAttributeSet 类的头文件
 // *
 // * 属性集基类,GAS 属性系统的基类,
 // * 用于定义和管理玩家的数值属性(血量、蓝量、攻击力等)。
 // */

#pragma once

// UE 核心类型与基础宏。
#include "CoreMinimal.h"

// GAS 属性集基类 UAttributeSet 以及 FGameplayAttributeData 等类型。
#include "AttributeSet.h"

#include "HodgeAttributeSet.generated.h"

/**
 * @brief Hodgepodge 框架的属性集基类
 *
 * 继承自 UAttributeSet,是 GAS 属性系统的基类。
 * AttributeSet 用于定义和管理玩家的数值属性,如:
 * - Health(生命值)
 * - MaxHealth(最大生命值)
 * - Mana(法力值)
 * - Stamina(耐力值)
 * - AttackPower(攻击力)
 * - Defense(防御力)
 *
 * 使用方式:
 * 1. 继承此类并添加 FGameplayAttribute 宏声明的属性
 * 2. 重写 GetLifetimeReplicatedProps 注册属性复制
 * 3. 在 AHodgePlayerStateBase::AttributeSetClasses 中配置此类
 *    服务器初始化时会自动创建并添加到 ASC 中
 *
 * 示例:
 * UCLASS()
 * class UHodgeHealthSet : public UHodgeAttributeSet
 * {
 *     GENERATED_BODY()
 * public:
 *     ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Health);
 *     ATTRIBUTE_ACCESSORS(UHodgeHealthSet, MaxHealth);
 * protected:
 *     UPROPERTY() FGameplayAttributeData Health;
 *     UPROPERTY() FGameplayAttributeData MaxHealth;
 * };
 */

/**
 * This macro defines a set of helper functions for accessing and initializing attributes.
 *
 * 这个宏用于一次性生成某个 GameplayAttribute 常用的属性访问辅助函数。
 *
 * The following example of the macro:
 *		ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Health)
 * will create the following functions:
 *		static FGameplayAttribute GetHealthAttribute();
 *		float GetHealth() const;
 *		void SetHealth(float NewVal);
 *		void InitHealth(float NewVal);
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
/* 生成静态 GetXXXAttribute()，用于获取该成员对应的 FGameplayAttribute 描述对象。 */ \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
/* 生成 GetXXX()，用于读取当前属性值。 */ \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
/* 生成 SetXXX()，用于直接设置当前属性值。 */ \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
/* 生成 InitXXX()，用于初始化属性值。 */ \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// GameplayEffectSpec：描述一次具体 GameplayEffect 应用的完整运行时规格。
struct FGameplayEffectSpec;

// 项目自定义 AbilitySystemComponent。
class UHodgeAbilitySystemComponent;

/** 
 * Delegate used to broadcast attribute events, some of these parameters may be null on clients: 
 *
 * 属性发生变化时使用的多播委托。
 * 某些与 GameplayEffect 相关的参数在客户端上可能为空。
 *
 * @param EffectInstigator	The original instigating actor for this event
 * @param EffectCauser		The physical actor that caused the change
 * @param EffectSpec		The full effect spec for this change
 * @param EffectMagnitude	The raw magnitude, this is before clamping
 * @param OldValue			The value of the attribute before it was changed
 * @param NewValue			The value after it was changed
 *
 * EffectInstigator：最初发起这次属性变化的 Actor。
 * EffectCauser：实际造成这次属性变化的物理 Actor。
 * EffectSpec：导致属性变化的 GameplayEffectSpec。
 * EffectMagnitude：本次效果产生的原始数值变化量，通常是 Clamp 之前的值。
 * OldValue：属性变化之前的数值。
 * NewValue：属性变化之后的数值。
*/

// 定义一个拥有六个参数的多播委托，用于向外广播属性变化事件。
DECLARE_MULTICAST_DELEGATE_SixParams(FHodgeAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/,
                                     const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/,
                                     float /*OldValue*/, float /*NewValue*/);

/**
 * UHodge属性集
 *
 *	Base attribute set class for the project.  // 已翻译: 此行已转换为中文注释
 *
 * Hodgepodge 项目所有 AttributeSet 的公共基类。
 * 主要提供项目 ASC 获取等所有属性集都可能使用的基础能力。
 */
UCLASS()
class HODGEPODGE_API UHodgeAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// 构造函数。
	UHodgeAttributeSet();

	// 返回当前 AttributeSet 所属 ASC 对应的 World。
	virtual UWorld* GetWorld() const override;

	// 获取当前 AttributeSet 所属的项目自定义 HodgeAbilitySystemComponent。
	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const;
};
