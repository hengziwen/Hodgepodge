/** @file HodgeButtonBase.h
 * @brief 声明可复用按钮基类相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonButtonBase.h"

#include "HodgeButtonBase.generated.h"

class UObject;
struct FFrame;

/** 封装可复用按钮基类的状态与操作接口。 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class UHodgeButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	/** 设置按钮文字。
	 * @param InText 文字，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable)
	void SetButtonText(const FText& InText);
	
protected:
	// UUserWidget interface
	/** 执行原生PreConstruct。 */
	virtual void NativePreConstruct() override;
	// End of UUserWidget interface

	// UCommonButtonBase interface
	/** 更新输入动作控件。 */
	virtual void UpdateInputActionWidget() override;
	/** 处理输入Method变化。
	 * @param CurrentInputType 当前输入类型，由调用方传入。
	 */
	virtual void OnInputMethodChanged(ECommonInputType CurrentInputType) override;
	// End of UCommonButtonBase interface

	/** 刷新按钮文字。 */
	void RefreshButtonText();
	
	/** 更新按钮文字。
	 * @param InText 文字，由调用方传入。
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonText(const FText& InText);

	/** 更新按钮风格。 */
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonStyle();
	
private:
	// 控制是否Override按钮文字的状态标记。
	UPROPERTY(EditAnywhere, Category="Button", meta=(InlineEditConditionToggle))
	uint8 bOverride_ButtonText : 1;
	
	// 按钮文字的数据或对象引用。
	UPROPERTY(EditAnywhere, Category="Button", meta=( editcondition="bOverride_ButtonText" ))
	FText ButtonText;
};
