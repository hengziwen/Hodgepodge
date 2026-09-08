// Copyright Epic Games, Inc. All Rights Reserved.

// 引入 Gameplay Cue 路径 GameFeature Action。
#include "GameFeatures/GameFeatureAction_AddGameplayCuePath.h"

#if WITH_EDITOR
// 编辑器下引入数据验证功能。
#include "Misc/DataValidation.h"
#endif

// 引入当前 CPP 对应的内联生成代码。
#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddGameplayCuePath)

// 本文件使用 GameFeatures 作为本地化文本命名空间。
#define LOCTEXT_NAMESPACE "GameFeatures"

// 构造 Gameplay Cue 路径 GameFeature Action。
UGameFeatureAction_AddGameplayCuePath::UGameFeatureAction_AddGameplayCuePath()
{
	// Add a default path that is commonly used
	// 默认添加一个项目中常用的 Gameplay Cue 目录。
	DirectoryPathsToAdd.Add(FDirectoryPath{TEXT("/GameplayCues")});
}

#if WITH_EDITOR
// 编辑器下验证 Gameplay Cue 路径配置是否有效。
EDataValidationResult UGameFeatureAction_AddGameplayCuePath::IsDataValid(FDataValidationContext& Context) const
{
	// 先执行父类的数据验证。
	EDataValidationResult Result = Super::IsDataValid(Context);

	// 遍历当前配置的所有 Gameplay Cue 目录。
	for (const FDirectoryPath& Directory : DirectoryPathsToAdd)
	{
		// 检查目录路径是否为空。
		if (Directory.Path.IsEmpty())
		{
			// 构造具体的无效路径错误信息。
			const FText InvalidCuePathError = FText::Format(
				LOCTEXT("InvalidCuePathError", "'{0}' is not a valid path!"), FText::FromString(Directory.Path));

			// 将路径错误报告给编辑器的数据验证系统。
			Context.AddError(InvalidCuePathError);

			// 将当前验证结果标记为无效。
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
		}
	}

	// 合并当前验证结果与 Valid 状态并返回最终结果。
	return CombineDataValidationResults(Result, EDataValidationResult::Valid);
}
#endif	// WITH_EDITOR

// 恢复本地化文本命名空间。
#undef LOCTEXT_NAMESPACE
