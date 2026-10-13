/** @file HodgeActionWidget.cpp
 * @brief 实现输入动作提示控件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Foundation/HodgeActionWidget.h"

#include "CommonInputBaseTypes.h"
#include "CommonInputSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"

/** 获取Icon。 */
FSlateBrush UHodgeActionWidget::GetIcon() const
{
	// If there is an Enhanced Input action associated with this widget, then search for any
	// keys bound to that action and display those instead of the default data table settings.
	// This covers the case of when a player has rebound a key to something else
	if (AssociatedInputAction)
	{
		if (const UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = GetEnhancedInputSubsystem())
		{
			TArray<FKey> BoundKeys = EnhancedInputSubsystem->QueryKeysMappedToAction(AssociatedInputAction);
			FSlateBrush SlateBrush;

			const UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();
			if (!BoundKeys.IsEmpty() && CommonInputSubsystem && UCommonInputPlatformSettings::Get()->TryGetInputBrush(
				SlateBrush, BoundKeys[0], CommonInputSubsystem->GetCurrentInputType(),
				CommonInputSubsystem->GetCurrentGamepadName()))
			{
				return SlateBrush;
			}
		}
	}

	return Super::GetIcon();
}

/** 获取Enhanced输入Subsystem。 */
UEnhancedInputLocalPlayerSubsystem* UHodgeActionWidget::GetEnhancedInputSubsystem() const
{
	const UWidget* BoundWidget = DisplayedBindingHandle.GetBoundWidget();
	if (const ULocalPlayer* BindingOwner = BoundWidget ? BoundWidget->GetOwningLocalPlayer() : GetOwningLocalPlayer())
	{
		return BindingOwner->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	}
	return nullptr;
}
