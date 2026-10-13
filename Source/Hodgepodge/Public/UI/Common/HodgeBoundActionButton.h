/** @file HodgeBoundActionButton.h
 * @brief 声明输入动作绑定按钮相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Input/CommonBoundActionButton.h"

#include "HodgeBoundActionButton.generated.h"

class UCommonButtonStyle;
class UObject;

/**
 * 
 */
/** 封装输入动作绑定按钮的状态与操作接口。 */
UCLASS(Abstract, meta = (DisableNativeTick))
class HODGEPODGE_API UHodgeBoundActionButton : public UCommonBoundActionButton
{
	GENERATED_BODY()

protected:
	/** 在控件构建时接入界面数据和生命周期。 */
	virtual void NativeConstruct() override;

private:
	/** 处理输入Method变化。
	 * @param NewInputMethod 新输入Method，由调用方传入。
	 */
	void HandleInputMethodChanged(ECommonInputType NewInputMethod);

	// 键盘风格的数据或对象引用。
	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> KeyboardStyle;

	// 手柄风格的数据或对象引用。
	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> GamepadStyle;

	// Touch风格的数据或对象引用。
	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> TouchStyle;
};
