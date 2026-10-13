/** @file HodgeWidgetFactory.h
 * @brief 声明控件工厂接口相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

#include "HodgeWidgetFactory.generated.h"

template <class TClass>
class TSubclassOf;

class UUserWidget;
struct FFrame;

/** 封装控件工厂接口的状态与操作接口。 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class HODGEPODGE_API UHodgeWidgetFactory : public UObject
{
	GENERATED_BODY()

public:
	/** 构造控件工厂接口并初始化其默认状态。 */
	UHodgeWidgetFactory()
	{
	}

	/** 查找控件类型关联数据。
	 * @param Data 当前操作使用的数据。
	 * @return 控件类型关联数据的当前值或引用。
	 */
	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<UUserWidget> FindWidgetClassForData(const UObject* Data) const;
};
