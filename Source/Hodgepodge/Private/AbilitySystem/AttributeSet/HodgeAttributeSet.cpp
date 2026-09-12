/**
 * @file (文件): (文件): Hodge属性集.cpp
 * @brief (简介): (简介): UHodge属性集 类的实现
 *
 * 属性集基类,当前为空实现,仅包含头文件引用。
 * 子类应在此文件中实现 GetLifetime已复制Props 注册属性复制,
 * 以及 Pre属性Change/Post游戏效果Execute 等回调。
 */

// UHodgeAttributeSet 类定义。
#include "AbilitySystem/AttributeSet/HodgeAttributeSet.h"

// 项目自定义 AbilitySystemComponent。
#include "AbilitySystem/HodgeAbilitySystemComponent.h"

// 使用当前 CPP 对应的 UE 内联生成代码。
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAttributeSet)

// 前置声明 UWorld。
class UWorld;

// UHodgeAttributeSet 构造函数。
UHodgeAttributeSet::UHodgeAttributeSet()
{
}

// 获取当前 AttributeSet 所属对象对应的 World。
UWorld* UHodgeAttributeSet::GetWorld() const
{
	// 获取当前 AttributeSet 的 Outer。
	const UObject* Outer = GetOuter();

	// AttributeSet 正常情况下必须拥有有效 Outer。
	check(Outer);

	// 使用 UAttributeSet / UObject 默认逻辑返回当前 World。
	return Super::GetWorld();
}

// 获取当前 AttributeSet 所属的 Hodge AbilitySystemComponent。
UHodgeAbilitySystemComponent* UHodgeAttributeSet::GetHodgeAbilitySystemComponent() const
{
	// 获取 GAS 记录的 OwningAbilitySystemComponent，并转换成项目自定义 ASC 类型。
	return Cast<UHodgeAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
