/** @file HodgeJoystickWidget.cpp
 * @brief 实现虚拟摇杆控件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/HodgeJoystickWidget.h"

#include "CommonHardwareVisibilityBorder.h"
#include "Components/Image.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeJoystickWidget)

#define LOCTEXT_NAMESPACE "HodgeJoystick"

/** 构造虚拟摇杆控件并初始化其默认状态。 */
UHodgeJoystickWidget::UHodgeJoystickWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetConsumePointerInput(true);
}

/** 执行原生事件Touch开始。 */
FReply UHodgeJoystickWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	Super::NativeOnTouchStarted(InGeometry, InGestureEvent);
	
	TouchOrigin = InGestureEvent.GetScreenSpacePosition();

	FReply Reply = FReply::Handled();
	if (!HasMouseCaptureByUser(InGestureEvent.GetUserIndex(), InGestureEvent.GetPointerIndex()))
	{
		Reply.CaptureMouse(GetCachedWidget().ToSharedRef());
	}
	return Reply;
}

/** 执行原生事件TouchMoved。 */
FReply UHodgeJoystickWidget::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	Super::NativeOnTouchMoved(InGeometry, InGestureEvent);
	HandleTouchDelta(InGeometry, InGestureEvent);

	FReply Reply = FReply::Handled();
	if (!HasMouseCaptureByUser(InGestureEvent.GetUserIndex(), InGestureEvent.GetPointerIndex()))
	{
		Reply.CaptureMouse(GetCachedWidget().ToSharedRef());
	}
	return Reply;
}

/** 执行原生事件Touch结束。 */
FReply UHodgeJoystickWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	StopInputSimulation();
	return FReply::Handled().ReleaseMouseCapture();
}

/** 执行原生事件鼠标Leave。 */
void UHodgeJoystickWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	StopInputSimulation();
}

/** 执行原生帧更新。 */
void UHodgeJoystickWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!CommonVisibilityBorder || CommonVisibilityBorder->IsVisible())
	{
		// Move the inner stick icon around with the vector
		if (JoystickForeground && JoystickBackground)
		{
			JoystickForeground->SetRenderTranslation(
				(bNegateYAxis ? FVector2D(1.0f, -1.0f) : FVector2D(1.0f)) *
				StickVector *
				(JoystickBackground->GetDesiredSize() * 0.5f)
			);
		}
		InputKeyValue2D(StickVector);
	}
}

/** 处理Touch增量。 */
void UHodgeJoystickWidget::HandleTouchDelta(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	const FVector2D& ScreenSpacePos = InGestureEvent.GetScreenSpacePosition();
	
	// The center of the geo locally is just its size
	FVector2D LocalStickCenter = InGeometry.GetAbsoluteSize();

	FVector2D ScreenSpaceStickCenter = InGeometry.LocalToAbsolute(LocalStickCenter);
	// Get the offset from the origin
	FVector2D MoveStickOffset = (ScreenSpacePos - ScreenSpaceStickCenter);
	if (bNegateYAxis)
	{
		MoveStickOffset *= FVector2D(1.0f, -1.0f);
	}
	
	FVector2D MoveStickDir = FVector2D::ZeroVector;
	float MoveStickLength = 0.0f;
	MoveStickOffset.ToDirectionAndLength(MoveStickDir, MoveStickLength);

	MoveStickLength = FMath::Min(MoveStickLength, StickRange);
	MoveStickOffset = MoveStickDir * MoveStickLength;

	StickVector = MoveStickOffset / StickRange;
}

/** 结束本控件拥有的模拟输入保持状态。 */
void UHodgeJoystickWidget::StopInputSimulation()
{
	TouchOrigin = FVector2D::ZeroVector;
	StickVector = FVector2D::ZeroVector;
}

#undef LOCTEXT_NAMESPACE

