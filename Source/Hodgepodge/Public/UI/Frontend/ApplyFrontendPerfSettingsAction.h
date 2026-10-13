/** @file ApplyFrontendPerfSettingsAction.h
 * @brief 声明前端性能策略动作相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFeatureAction.h"

#include "ApplyFrontendPerfSettingsAction.generated.h"

class UObject;
struct FGameFeatureActivatingContext;
struct FGameFeatureDeactivatingContext;

//////////////////////////////////////////////////////////////////////
// UApplyFrontendPerfSettingsAction

/**
 * GameFeatureAction responsible for telling the user settings to apply frontend/menu specific performance settings
 */
/** 封装应用FrontendPerf设置动作的状态与操作接口。 */
UCLASS(MinimalAPI, meta = (DisplayName = "Use Frontend Perf Settings"))
class UApplyFrontendPerfSettingsAction final : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	//~UGameFeatureAction interface
	/** 处理游戏功能扩展Activating。
	 * @param Context 本次操作的上下文。
	 */
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	/** 处理游戏功能扩展Deactivating。
	 * @param Context 本次操作的上下文。
	 */
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~End of UGameFeatureAction interface

private:
	// ApplicationCounter的数值或阈值。
	static int32 ApplicationCounter;
};
