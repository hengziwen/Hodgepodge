// Copyright Epic Games, Inc. All Rights Reserved.

// Indicator 系统的蓝图工具函数库定义。
#include "UI/IndicatorSystem/IndicatorLibrary.h"

// Hodge Indicator 管理组件。
// 实际查找 Controller 对应 IndicatorManagerComponent 的逻辑由该组件自身提供。
#include "UI/IndicatorSystem/HodgeIndicatorManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IndicatorLibrary)

// 前向声明 Controller。
class AController;

// 构造 Indicator 蓝图函数库。
UIndicatorLibrary::UIndicatorLibrary()
{
}

// 根据指定 Controller 获取其对应的 IndicatorManagerComponent。
UHodgeIndicatorManagerComponent* UIndicatorLibrary::GetIndicatorManagerComponent(AController* Controller)
{
	// 实际查找逻辑交给 UHodgeIndicatorManagerComponent::GetComponent。
	// 当前函数主要作为 BlueprintFunctionLibrary 的蓝图访问入口，
	// 让蓝图不需要直接了解 ManagerComponent 的具体查找方式。
	return UHodgeIndicatorManagerComponent::GetComponent(Controller);
}
