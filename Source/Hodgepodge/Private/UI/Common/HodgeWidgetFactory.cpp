/** @file HodgeWidgetFactory.cpp
 * @brief 实现控件工厂接口相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Common/HodgeWidgetFactory.h"
#include "Templates/SubclassOf.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeWidgetFactory)

class UUserWidget;

/** 查找控件类型关联数据。 */
TSubclassOf<UUserWidget> UHodgeWidgetFactory::FindWidgetClassForData_Implementation(const UObject* Data) const
{
	return TSubclassOf<UUserWidget>();
}
