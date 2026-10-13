/** @file HodgeSimulatedInputWidget.h
 * @brief 声明模拟输入控件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"
#include "HodgeSimulatedInputWidget.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UCommonHardwareVisibilityBorder;
class UEnhancedPlayerInput;

/**
 *  A UMG widget with base functionality to inject input (keys or input actions)
 *  to the enhanced input subsystem.
 */
/** 封装模拟输入控件的状态与操作接口。 */
UCLASS(meta=( DisplayName="Hodge Simulated Input Widget" ))
class HODGEPODGE_API UHodgeSimulatedInputWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	/** 构造模拟输入控件并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeSimulatedInputWidget(const FObjectInitializer& ObjectInitializer);

	//~ Begin UWidget
#if WITH_EDITOR
	/** 获取PaletteCategory。
	 * @return PaletteCategory的当前值或引用。
	 */
	virtual const FText GetPaletteCategory() override;
#endif
	//~ End UWidget interface

	//~ Begin UUserWidget
	/** 在控件构建时接入界面数据和生命周期。 */
	virtual void NativeConstruct() override;
	/** 在控件析构时解除界面监听与临时资源。 */
	virtual void NativeDestruct() override;
	/** 执行原生事件Touch结束。
	 * @param InGeometry 几何，由调用方传入。
	 * @param InGestureEvent Gesture事件，由调用方传入。
	 * @return 当前输入事件的处理结果，决定事件是否继续传播。
	 */
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	//~ End UUserWidget interface

	/** Get the enhanced input subsystem based on the owning local player of this widget. Will return null if there is no owning player */
	/** 获取Enhanced输入Subsystem。
	 * @return Enhanced输入Subsystem的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable)
	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;

	/** Get the current player input from the current input subsystem */
	/** 获取玩家输入。
	 * @return 玩家输入的当前值或引用。
	 */
	UEnhancedPlayerInput* GetPlayerInput() const;

	/**  */
	/** 获取Associated动作。
	 * @return Associated动作的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable)
	const UInputAction* GetAssociatedAction() const { return AssociatedAction; }

	/** Returns the current key that will be used to input any values. */
	/** 获取Simulated键。
	 * @return Simulated键的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable)
	FKey GetSimulatedKey() const { return KeyToSimulate; }

	/**
	 * Injects the given vector as an input to the current simulated key.
	 * This calls "InputKey" on the current player.
	 */
	/** 向对应按键提交当前模拟输入值。
	 * @param Value 值，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable)
	void InputKeyValue(const FVector& Value);

	/**
	 * Injects the given vector as an input to the current simulated key.
	 * This calls "InputKey" on the current player.
	 */
	/** 向对应按键提交二维模拟输入值。
	 * @param Value 值，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable)
	void InputKeyValue2D(const FVector2D& Value);

	/** 释放尚未清理的模拟输入状态。 */
	UFUNCTION(BlueprintCallable)
	void FlushSimulatedInput();

protected:
	/** Set the KeyToSimulate based on a query from enhanced input about what keys are mapped to the associated action */
	/** 根据当前输入映射查询需要模拟的按键。 */
	void QueryKeyToSimulate();

	/** Called whenever control mappings change, so we have a chance to adapt our own keys */
	/** 处理控制Mappings重建。 */
	UFUNCTION()
	void OnControlMappingsRebuilt();

	/** The common visibility border will allow you to specify UI for only specific platforms if desired */
	// Common可见性Border的数据或对象引用。
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonHardwareVisibilityBorder> CommonVisibilityBorder = nullptr;

	/** The associated input action that we should simulate input for */
	// Associated动作的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<const UInputAction> AssociatedAction = nullptr;

	/** The Key to simulate input for in the case where none are currently bound to the associated action */
	// Fallback绑定键的数据或对象引用。
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FKey FallbackBindingKey = EKeys::Gamepad_Right2D;

	/** The key that should be input via InputKey on the player input */
	// 键目标Simulate的数据或对象引用。
	FKey KeyToSimulate;
};
