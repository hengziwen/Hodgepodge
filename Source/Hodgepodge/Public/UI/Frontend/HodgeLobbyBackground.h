/** @file HodgeLobbyBackground.h
 * @brief 声明前端背景控件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "HodgeLobbyBackground.generated.h"

class UObject;
class UWorld;

/**
 * Developer settings / editor cheats
 */
/** 封装前端背景控件的状态与操作接口。 */
UCLASS(config=EditorPerProjectUserSettings, MinimalAPI)
class UHodgeLobbyBackground : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	// 背景等级的资源软引用，按现有加载入口解析。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> BackgroundLevel;
};
