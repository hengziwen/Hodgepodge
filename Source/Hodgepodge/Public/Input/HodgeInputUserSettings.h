// Copyright Epic Games, Inc. All Rights Reserved.

// 防止头文件被重复包含。
#pragma once

// Enhanced Input 玩家输入设置基类。
#include "UserSettings/EnhancedInputUserSettings.h"

// 玩家可映射按键的基础设置。
#include "PlayerMappableKeySettings.h"

// 当前头文件对应的 UE 反射代码生成文件。
#include "HodgeInputUserSettings.generated.h"

/** 
 * Custom settings class for any input related settings for the Hodge game.
 * This will be serialized out at the same time as the Hodge Shared Settings and is
 * compatible with cloud saves through by calling the "Serialize" function.
 */
// Hodge 项目自己的输入用户设置，用于保存玩家层面的输入相关配置。
// 这些设置可以与其他 Shared Settings 一起序列化，也可以通过 Serialize 支持云端存档。
UCLASS()
class HODGEPODGE_API UHodgeInputUserSettings : public UEnhancedInputUserSettings
{
	GENERATED_BODY()

public:
	//~ Begin UEnhancedInputUserSettings interface
	// 应用当前输入用户设置，使修改后的配置正式生效。
	virtual void ApplySettings() override;
	//~ End UEnhancedInputUserSettings interface

	// Add any additional Input Settings here!
	// 这里可以继续添加项目自定义的玩家输入设置。
	// Some ideas could be:
	// 一些典型的输入设置例如：
	// - "toggle vs. hold" to trigger in game actions
	// - 切换模式或按住模式，用于控制游戏中的动作。
	// - aim sensitivity should go here
	// - 瞄准灵敏度等玩家输入参数。
	// - etc
	// - 其他需要保存的输入相关配置。

	// Make sure to mark your properties with the "SaveGame" metadata to have them serialize when saved
	// 如果希望属性随用户设置一起保存，需要添加 SaveGame 元数据。
	//UPROPERTY(SaveGame, BlueprintReadWrite, Category="Enhanced Input|User Settings")
	// 示例：定义一个可以保存并从蓝图访问的输入用户设置。
	// bool bSomeExampleProperty;
};

/**
 * Player Mappable Key settings are settings that are accessible per-action key mapping.
 * This is where you could place additional metadata that may be used by your settings UI,
 * input triggers, or other places where you want to know about a key setting.
 */
// 玩家可映射按键的额外设置，用于保存单个可映射按键相关的元数据。
// 这里的配置可以被设置界面、Input Trigger 或其他输入系统逻辑读取。
UCLASS()
class HODGEPODGE_API UHodgePlayerMappableKeySettings : public UPlayerMappableKeySettings
{
	GENERATED_BODY()

public:
	/** Returns the tooltip that should be displayed on the settings screen for this key */
	// 获取该按键在输入设置界面中显示的提示文本。
	const FText& GetTooltipText() const;

protected:
	/** The tooltip that should be associated with this action when displayed on the settings screen */
	// 当前可映射按键在设置界面中显示的 Tooltip 文本。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta=(AllowPrivateAccess=true))
	FText Tooltip = FText::GetEmpty();
};
