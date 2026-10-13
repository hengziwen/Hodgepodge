/** @file HodgeActionWidget.h
 * @brief 声明输入动作提示控件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonActionWidget.h"
#include "HodgeActionWidget.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;

/** An action widget that will get the icon of key that is currently assigned to the common input action on this widget */
/** 封装输入动作提示控件的状态与操作接口。 */
UCLASS(BlueprintType, Blueprintable)
class UHodgeActionWidget : public UCommonActionWidget
{
	GENERATED_BODY()

public:

	//~ Begin UCommonActionWidget interface
	/** 获取Icon。
	 * @return Icon的当前值或引用。
	 */
	virtual FSlateBrush GetIcon() const override;
	//~ End of UCommonActionWidget interface

	/** The Enhanced Input Action that is associated with this Common Input action. */
	// Associated输入动作的数据或对象引用。
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	const TObjectPtr<UInputAction> AssociatedInputAction;

private:

	/** 获取Enhanced输入Subsystem。
	 * @return Enhanced输入Subsystem的当前值或引用。
	 */
	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;
	
};