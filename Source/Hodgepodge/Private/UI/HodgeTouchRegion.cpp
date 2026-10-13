/** @file HodgeTouchRegion.cpp
 * @brief 实现触摸输入区域相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/HodgeTouchRegion.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeTouchRegion)

struct FGeometry;
struct FPointerEvent;

/** 执行原生事件Touch开始。 */
FReply UHodgeTouchRegion::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	bShouldSimulateInput = true;
	return Super::NativeOnTouchStarted(InGeometry, InGestureEvent);
}

/** 执行原生事件TouchMoved。 */
FReply UHodgeTouchRegion::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	// Input our associatied key as long as the player is touching within our bounds
	//InputKeyValue(FVector::OneVector);
	bShouldSimulateInput = true;
	// Continuously trigger the input if we should
	return Super::NativeOnTouchMoved(InGeometry, InGestureEvent);
}

/** 执行原生事件Touch结束。 */
FReply UHodgeTouchRegion::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	bShouldSimulateInput = false;
	return Super::NativeOnTouchEnded(InGeometry, InGestureEvent);
}

/** 执行原生帧更新。 */
void UHodgeTouchRegion::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bShouldSimulateInput)
	{
		InputKeyValue(FVector::OneVector);
	}
}
