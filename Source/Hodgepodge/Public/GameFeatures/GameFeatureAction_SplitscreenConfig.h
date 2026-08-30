// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFeatureAction_WorldActionBase.h"
#include "UObject/ObjectKey.h"

#include "GameFeatureAction_SplitscreenConfig.generated.h"

class UObject;
struct FGameFeatureDeactivatingContext;
struct FGameFeatureStateChangeContext;
struct FWorldContext;

//////////////////////////////////////////////////////////////////////
// UGameFeatureAction_SplitscreenConfig

/**
 * GameFeatureAction：负责根据配置修改游戏的分屏设置。
 * 它继承自 WorldActionBase，因此会在 Game Feature 激活时对相关 World 执行 AddToWorld。
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Splitscreen Config"))
class UGameFeatureAction_SplitscreenConfig final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface

	// Game Feature 停用时调用，用于撤销当前 Feature 对分屏设置产生的影响。
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	//~ End UGameFeatureAction interface

	//~ Begin UGameFeatureAction_WorldActionBase interface

	// Game Feature 激活后，将分屏配置应用到指定的 World。
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

	//~ End UGameFeatureAction_WorldActionBase interface

public:
	// 是否禁用分屏；为 true 时表示当前 Game Feature 希望禁用分屏。
	UPROPERTY(EditAnywhere, Category=Action)
	bool bDisableSplitscreen = true;

private:
	// 记录当前对象在哪些 World/GameInstance 上提交过禁用分屏的请求。
	TArray<FObjectKey> LocalDisableVotes;

	// 全局记录每个对象当前有多少个 Game Feature 请求禁用分屏。
	static TMap<FObjectKey, int32> GlobalDisableVotes;
};