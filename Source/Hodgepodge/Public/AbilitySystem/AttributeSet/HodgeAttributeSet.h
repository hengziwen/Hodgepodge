
 // * @file HodgeAttributeSet.h
 // * @brief UHodgeAttributeSet 类的头文件
 // *
 // * 属性集基类,GAS 属性系统的基类,
 // * 用于定义和管理玩家的数值属性(血量、蓝量、攻击力等)。
 // */

#pragma once

#include "CoreMinimal.h"
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
 * The following example of the macro:
 *		ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Health)
 * will create the following functions:
 *		static FGameplayAttribute GetHealthAttribute();
 *		float GetHealth() const;
 *		void SetHealth(float NewVal);
 *		void InitHealth(float NewVal);
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

struct FGameplayEffectSpec;
class UHodgeAbilitySystemComponentBase;
/** 
 * Delegate used to broadcast attribute events, some of these parameters may be null on clients: 
 * @param EffectInstigator	The original instigating actor for this event
 * @param EffectCauser		The physical actor that caused the change
 * @param EffectSpec		The full effect spec for this change
 * @param EffectMagnitude	The raw magnitude, this is before clamping
 * @param OldValue			The value of the attribute before it was changed
 * @param NewValue			The value after it was changed
*/
DECLARE_MULTICAST_DELEGATE_SixParams(FHodgeAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/,
                                     const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/,
                                     float /*OldValue*/, float /*NewValue*/);

/**
 * UHodge属性集
 *
 *	Base attribute set class for the project.  // 已翻译: 此行已转换为中文注释
 */
UCLASS()
class HODGEPODGE_API UHodgeAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UHodgeAttributeSet();

	virtual UWorld* GetWorld() const override;

	UHodgeAbilitySystemComponentBase* GetHodgeAbilitySystemComponent() const;
};
