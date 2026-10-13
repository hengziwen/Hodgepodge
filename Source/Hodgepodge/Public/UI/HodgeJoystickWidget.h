/** @file HodgeJoystickWidget.h
 * @brief 声明虚拟摇杆控件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UI/HodgeSimulatedInputWidget.h"

#include "HodgeJoystickWidget.generated.h"

class UImage;
class UObject;
struct FGeometry;
struct FPointerEvent;

/**
 *  A UMG wrapper for the Hodge virtual joystick.
 *
 *  This will calculate a 2D vector clamped between -1 and 1
 *  to input as a key value to the player, simulating a gamepad analog stick.
 *
 *  This is intended for use with and Enhanced Input player.
 */
/** 封装虚拟摇杆控件的状态与操作接口。 */
UCLASS(meta=( DisplayName="Hodge Joystick" ))
class HODGEPODGE_API UHodgeJoystickWidget : public UHodgeSimulatedInputWidget
{
	GENERATED_BODY()
	
public:
	
	/** 构造虚拟摇杆控件并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeJoystickWidget(const FObjectInitializer& ObjectInitializer);

	//~ Begin UUserWidget
	/** 执行原生事件Touch开始。
	 * @param InGeometry 几何，由调用方传入。
	 * @param InGestureEvent Gesture事件，由调用方传入。
	 * @return 当前输入事件的处理结果，决定事件是否继续传播。
	 */
	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	/** 执行原生事件TouchMoved。
	 * @param InGeometry 几何，由调用方传入。
	 * @param InGestureEvent Gesture事件，由调用方传入。
	 * @return 当前输入事件的处理结果，决定事件是否继续传播。
	 */
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	/** 执行原生事件Touch结束。
	 * @param InGeometry 几何，由调用方传入。
	 * @param InGestureEvent Gesture事件，由调用方传入。
	 * @return 当前输入事件的处理结果，决定事件是否继续传播。
	 */
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	/** 执行原生事件鼠标Leave。
	 * @param InMouseEvent 鼠标事件，由调用方传入。
	 */
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	/** 执行原生帧更新。
	 * @param MyGeometry My几何，由调用方传入。
	 * @param InDeltaTime 本次更新的时间步长，单位秒。
	 */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget interface
	
protected:

	/**
	 * Calculate the delta position of the current touch from the origin.
	 *
	 * Input the associated gamepad key on the player
	 *
	 * Move the foreground joystick image in association with the given input to give the appearance that it
	 * is moving along with the player's finger
	 */
	/** 处理Touch增量。
	 * @param InGeometry 几何，由调用方传入。
	 * @param InGestureEvent Gesture事件，由调用方传入。
	 */
	void HandleTouchDelta(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent);
	
	/** Flush any player input that has been injected and disable the use of this analog stick. */
	/** 结束本控件拥有的模拟输入保持状态。 */
	void StopInputSimulation();

	/** How far can the inner image of the joystick be moved? */
	// Stick范围的数值或阈值。
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float StickRange = 50.0f;

	/** Image to be used as the background of the joystick */
	// Joystick背景的数据或对象引用。
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> JoystickBackground;

	/** Image to be used as the foreground of the joystick */
	// JoystickForeground的数据或对象引用。
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> JoystickForeground;

	/** Should we negate the Y-axis value of the joystick? This is common for "movement" sticks */
	// 控制是否NegateYAxis的状态标记。
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bNegateYAxis = false;

	/** The origin of the touch. Set on NativeOnTouchStarted */
	// TouchOrigin的几何或变换数据。
	UPROPERTY(Transient)
	FVector2D TouchOrigin = FVector2D::ZeroVector;

	// Stick向量的几何或变换数据。
	UPROPERTY(Transient)
	FVector2D StickVector = FVector2D::ZeroVector;
};
