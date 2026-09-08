// Copyright Epic Games, Inc. All Rights Reserved.

// 引入项目自定义的输入用户设置。
#include "Input/HodgeInputUserSettings.h"

// 应用当前玩家的输入设置。
void UHodgeInputUserSettings::ApplySettings()
{
	// 先执行父类的输入设置应用逻辑。
	Super::ApplySettings();

	// Add any functionality you want to happen when the input settings are applied to the user
	// 可以在这里添加输入设置应用完成后需要执行的项目自定义逻辑。
	// This is a good place to put a breakpoint in your debugger to see the flow of
	// 可以在这里打断点，观察输入设置从修改到正式应用的完整调用流程。
	// how input settings are used :)
	// 便于调试输入设置最终是如何被输入系统使用的。
}
