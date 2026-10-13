/** @file HodgeBoundActionButton.cpp
 * @brief 实现输入动作绑定按钮相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Common/HodgeBoundActionButton.h"

#include "CommonInputSubsystem.h"
#include "CommonInputTypeEnum.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeBoundActionButton)

class UCommonButtonStyle;

/** 在控件构建时接入界面数据和生命周期。 */
void UHodgeBoundActionButton::NativeConstruct()
{
	Super::NativeConstruct();

	// 按当前对象生命周期建立状态监听。
	if (UCommonInputSubsystem* InputSubsystem = GetInputSubsystem())
	{
		InputSubsystem->OnInputMethodChangedNative.AddUObject(this, &ThisClass::HandleInputMethodChanged);
		HandleInputMethodChanged(InputSubsystem->GetCurrentInputType());
	}
}

/** 处理输入Method变化。 */
void UHodgeBoundActionButton::HandleInputMethodChanged(ECommonInputType NewInputMethod)
{
	TSubclassOf<UCommonButtonStyle> NewStyle = nullptr;

	if (NewInputMethod == ECommonInputType::Gamepad)
	{
		NewStyle = GamepadStyle;
	}
	else if (NewInputMethod == ECommonInputType::Touch)
	{
		NewStyle = TouchStyle;
	}
	else
	{
		NewStyle = KeyboardStyle;
	}

	if (NewStyle)
	{
		SetStyle(NewStyle);
	}
}
