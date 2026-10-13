/** @file HodgeLoadingScreenSubsystem.h
 * @brief 声明加载页面配置服务相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/SubclassOf.h"

#include "UObject/WeakObjectPtr.h"
#include "HodgeLoadingScreenSubsystem.generated.h"

class UObject;
class UUserWidget;
struct FFrame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadingScreenWidgetChangedDelegate, TSubclassOf<UUserWidget>, NewWidgetClass);

/**
 * Tracks/stores the current loading screen configuration in a place
 * that persists across map transitions
 */
/** 封装加载页面配置服务的状态与操作接口。 */
UCLASS()
class HODGEPODGE_API UHodgeLoadingScreenSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** 构造加载页面配置服务并初始化其默认状态。 */
	UHodgeLoadingScreenSubsystem();

	// Sets the loading screen widget class to display inside of the loading screen widget host
	/** 设置加载屏幕内容控件。
	 * @param NewWidgetClass 新控件类型，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable)
	void SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	// Returns the last set loading screen widget class to display inside of the loading screen widget host
	/** 获取加载屏幕内容控件。
	 * @return 加载屏幕内容控件的当前值或引用。
	 */
	UFUNCTION(BlueprintPure)
	TSubclassOf<UUserWidget> GetLoadingScreenContentWidget() const;

private:
	// 事件加载屏幕控件变化对应的事件通知。
	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess))
	FLoadingScreenWidgetChangedDelegate OnLoadingScreenWidgetChanged;

	// 加载屏幕控件类型的数据或对象引用。
	UPROPERTY()
	TSubclassOf<UUserWidget> LoadingScreenWidgetClass;
};
