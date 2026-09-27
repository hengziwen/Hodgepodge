// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/HodgeTouchRegion.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeTouchRegion)

struct FGeometry;
struct FPointerEvent;

FReply UHodgeTouchRegion::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	bShouldSimulateInput = true;
	return Super::NativeOnTouchStarted(InGeometry, InGestureEvent);
}

FReply UHodgeTouchRegion::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	// Input our associatied key as long as the player is touching within our bounds
	//InputKeyValue(FVector::OneVector);
	bShouldSimulateInput = true;
	// Continuously trigger the input if we should
	return Super::NativeOnTouchMoved(InGeometry, InGestureEvent);
}

FReply UHodgeTouchRegion::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	bShouldSimulateInput = false;
	return Super::NativeOnTouchEnded(InGeometry, InGestureEvent);
}

void UHodgeTouchRegion::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bShouldSimulateInput)
	{
		InputKeyValue(FVector::OneVector);
	}
}
