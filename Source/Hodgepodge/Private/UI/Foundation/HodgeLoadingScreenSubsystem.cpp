/** @file HodgeLoadingScreenSubsystem.cpp
 * @brief 实现加载页面配置服务相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Foundation/HodgeLoadingScreenSubsystem.h"

#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeLoadingScreenSubsystem)

class UUserWidget;

//////////////////////////////////////////////////////////////////////
// UHodgeLoadingScreenSubsystem

/** 构造加载页面配置服务并初始化其默认状态。 */
UHodgeLoadingScreenSubsystem::UHodgeLoadingScreenSubsystem()
{
}

/** 设置加载屏幕内容控件。 */
void UHodgeLoadingScreenSubsystem::SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	if (LoadingScreenWidgetClass != NewWidgetClass)
	{
		LoadingScreenWidgetClass = NewWidgetClass;

		OnLoadingScreenWidgetChanged.Broadcast(LoadingScreenWidgetClass);
	}
}

/** 获取加载屏幕内容控件。 */
TSubclassOf<UUserWidget> UHodgeLoadingScreenSubsystem::GetLoadingScreenContentWidget() const
{
	return LoadingScreenWidgetClass;
}

