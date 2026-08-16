/**
 * @file (文件): (文件): Hodge属性集.cpp
 * @brief (简介): (简介): UHodge属性集 类的实现
 *
 * 属性集基类,当前为空实现,仅包含头文件引用。
 * 子类应在此文件中实现 GetLifetime已复制Props 注册属性复制,
 * 以及 Pre属性Change/Post游戏效果Execute 等回调。
 */

#include "AbilitySystem/AttributeSet/HodgeAttributeSet.h"

#include "AbilitySystem/HodgeAbilitySystemComponentBase.h"

class UWorld;

UHodgeAttributeSet::UHodgeAttributeSet()
{
}

UWorld* UHodgeAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);
	return Super::GetWorld();
}

UHodgeAbilitySystemComponentBase* UHodgeAttributeSet::GetHodgeAbilitySystemComponent() const
{
	return Cast<UHodgeAbilitySystemComponentBase>(GetOwningAbilitySystemComponent());
}
