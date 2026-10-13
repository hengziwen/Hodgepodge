/** @file GameFeatureAction_WorldActionBase.h
 * @brief 声明GameFeature世界动作基类相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"

#include "GameFeatureAction_WorldActionBase.generated.h"

class FDelegateHandle;
class UGameInstance;
class UObject;
struct FGameFeatureActivatingContext;
struct FGameFeatureDeactivatingContext;
struct FGameFeatureStateChangeContext;
struct FWorldContext;

/**
 * GameFeatureAction 的 World 级基类，用于处理与 World/GameInstance 相关的功能逻辑。
 */
UCLASS(Abstract)
class UGameFeatureAction_WorldActionBase : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface

	// GameFeature 激活时调用，在这里开始监听 World/GameInstance 并执行对应逻辑。
	/** 处理游戏功能扩展Activating。
	 * @param Context 本次操作的上下文。
	 */
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;

	// GameFeature 停用时调用，在这里解除激活阶段建立的监听和相关逻辑。
	/** 处理游戏功能扩展Deactivating。
	 * @param Context 本次操作的上下文。
	 */
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	//~ End UGameFeatureAction interface

private:
	// 当 GameInstance 启动时调用，用于处理当前 GameFeature 在该 GameInstance 中的 World 初始化。
	/** 处理游戏Instance起始。
	 * @param GameInstance 游戏Instance，由调用方传入。
	 * @param ChangeContext 变更上下文，由调用方传入。
	 */
	void HandleGameInstanceStart(UGameInstance* GameInstance, FGameFeatureStateChangeContext ChangeContext);

	// 子类重写这个函数，实现具体的 World 级 GameFeature 功能。
	/** 添加目标世界。
	 * @param WorldContext 世界上下文，由调用方传入。
	 * @param ChangeContext 变更上下文，由调用方传入。
	 */
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
		PURE_VIRTUAL(UGameFeatureAction_WorldActionBase::AddToWorld,);

private:
	// 保存每次 GameFeature 激活时注册的 GameInstance 启动委托，便于停用时解除绑定。
	TMap<FGameFeatureStateChangeContext, FDelegateHandle> GameInstanceStartHandles;
};