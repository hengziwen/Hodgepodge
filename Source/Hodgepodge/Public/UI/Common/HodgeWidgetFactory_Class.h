/** @file HodgeWidgetFactory_Class.h
 * @brief 声明按类选择控件的工厂相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "HodgeWidgetFactory.h"
#include "Templates/SubclassOf.h"
#include "UObject/SoftObjectPtr.h"

#include "HodgeWidgetFactory_Class.generated.h"

class UObject;
class UUserWidget;

/** 封装按类选择控件的工厂的状态与操作接口。 */
UCLASS()
class HODGEPODGE_API UHodgeWidgetFactory_Class : public UHodgeWidgetFactory
{
	GENERATED_BODY()

public:
	/** 构造按类选择控件的工厂并初始化其默认状态。 */
	UHodgeWidgetFactory_Class()
	{
	}

	/** 查找控件类型关联数据。
	 * @param Data 当前操作使用的数据。
	 * @return 控件类型关联数据的当前值或引用。
	 */
	virtual TSubclassOf<UUserWidget> FindWidgetClassForData_Implementation(const UObject* Data) const override;

protected:
	// 条目控件关联类型的资源软引用，按现有加载入口解析。
	UPROPERTY(EditAnywhere, Category = ListEntries, meta = (AllowAbstract))
	TMap<TSoftClassPtr<UObject>, TSubclassOf<UUserWidget>> EntryWidgetForClass;
};
