// Copyright Epic Games, Inc. All Rights Reserved.

// 防止头文件被重复包含。
#pragma once

// 引入 GameFeatureAction 基类。
#include "GameFeatureAction.h"

// 引入软对象路径以及目录路径相关类型。
#include "UObject/SoftObjectPath.h"

// 引入当前类的反射生成代码。
#include "GameFeatureAction_AddGameplayCuePath.generated.h"

/**
 * GameFeatureAction responsible for adding gameplay cue paths to the gameplay cue manager.
 *
 * @see UAbilitySystemGlobals::GameplayCueNotifyPaths
 */
// 负责将 Gameplay Cue 资源目录添加到 GameplayCueManager 中的 GameFeature Action。
// GameFeature 激活后，这些目录中的 Gameplay Cue 就可以被 Gameplay Cue 系统发现和加载。
UCLASS(MinimalAPI, meta = (DisplayName = "Add Gameplay Cue Path"))
class UGameFeatureAction_AddGameplayCuePath final : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	// 构造函数。
	UGameFeatureAction_AddGameplayCuePath();

	//~UObject interface
#if WITH_EDITOR
	// 编辑器下验证 Gameplay Cue 路径配置是否有效。
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	// 获取当前 GameFeature 配置的 Gameplay Cue 目录列表。
	const TArray<FDirectoryPath>& GetDirectoryPathsToAdd() const { return DirectoryPathsToAdd; }

private:
	// List of paths to register to the gameplay cue manager. These are relative tot he game content directory
	// 需要注册到 Gameplay Cue Manager 的目录列表。
	// 这些目录路径相对于项目的 Content 目录。
	UPROPERTY(EditAnywhere, Category = "Game Feature | Gameplay Cues",
		meta = (RelativeToGameContentDir, LongPackageName))
	TArray<FDirectoryPath> DirectoryPathsToAdd;
};
