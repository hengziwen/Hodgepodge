/** @file HodgeTouchRegion.h
 * @brief 声明触摸输入区域相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UI/HodgeSimulatedInputWidget.h"

#include "HodgeTouchRegion.generated.h"

class UObject;
struct FFrame;
struct FGeometry;
struct FPointerEvent;

/**
 * A "Touch Region" is used to define an area on the screen that should trigger some
 * input when the user presses a finger on it
 */
/** 封装触摸输入区域的状态与操作接口。 */
UCLASS(meta=( DisplayName="Hodge Touch Region" ))
class HODGEPODGE_API UHodgeTouchRegion : public UHodgeSimulatedInputWidget
{
	GENERATED_BODY()

public:
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
	/** 执行原生帧更新。
	 * @param MyGeometry My几何，由调用方传入。
	 * @param InDeltaTime 本次更新的时间步长，单位秒。
	 */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget interface

	/** 判断是否应Simulate输入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintCallable)
	bool ShouldSimulateInput() const { return bShouldSimulateInput; }

protected:
	/** True while this widget is being touched */
	// 控制是否策略判断Simulate输入的状态标记。
	bool bShouldSimulateInput = false;
};
