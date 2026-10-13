/** @file HodgeWidgetFactory_Class.cpp
 * @brief 实现按类选择控件的工厂相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Common/HodgeWidgetFactory_Class.h"

#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeWidgetFactory_Class)

TSubclassOf<UUserWidget> UHodgeWidgetFactory_Class::FindWidgetClassForData_Implementation(const UObject* Data) const
{
	// Starting with the current class, work backwards to see if there are any construction rules for this class.
	for (UClass* Class = Data->GetClass(); Class; Class = Class->GetSuperClass())
	{
		/** 读取指定反射配置字段保存的类引用。
		 * Class参数；用于保持该接口的调用签名。
		 * @return 本次计算、查询或创建得到的TSoft类型PtrU对象。
		 */
		TSoftClassPtr<UObject> ClassPtr(Class);
		if (const TSubclassOf<UUserWidget> EntryWidgetClassPtr = EntryWidgetForClass.FindRef(ClassPtr))
		{
			return EntryWidgetClassPtr;
		}
	}

	return TSubclassOf<UUserWidget>();
}
