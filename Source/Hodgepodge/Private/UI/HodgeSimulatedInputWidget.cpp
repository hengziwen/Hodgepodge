/** @file HodgeSimulatedInputWidget.cpp
 * @brief 实现模拟输入控件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/HodgeSimulatedInputWidget.h"
#include "EnhancedInputSubsystems.h"
//#include "HodgeLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeSimulatedInputWidget)

#define LOCTEXT_NAMESPACE "HodgeSimulatedInputWidget"

/** 构造模拟输入控件并初始化其默认状态。 */
UHodgeSimulatedInputWidget::UHodgeSimulatedInputWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetConsumePointerInput(true);
}

#if WITH_EDITOR
/** 获取PaletteCategory。 */
const FText UHodgeSimulatedInputWidget::GetPaletteCategory()
{
	return LOCTEXT("PalleteCategory", "Input");
}
#endif // WITH_EDITOR

/** 在控件构建时接入界面数据和生命周期。 */
void UHodgeSimulatedInputWidget::NativeConstruct()
{
	// Find initial key, then listen for any changes to control mappings
	QueryKeyToSimulate();

	if (UEnhancedInputLocalPlayerSubsystem* System = GetEnhancedInputSubsystem())
	{
		System->ControlMappingsRebuiltDelegate.AddUniqueDynamic(
			this, &UHodgeSimulatedInputWidget::OnControlMappingsRebuilt);
	}

	Super::NativeConstruct();
}

/** 在控件析构时解除界面监听与临时资源。 */
void UHodgeSimulatedInputWidget::NativeDestruct()
{
	// 解除本对象的监听，避免旧生命周期回调继续进入。
	if (UEnhancedInputLocalPlayerSubsystem* System = GetEnhancedInputSubsystem())
	{
		System->ControlMappingsRebuiltDelegate.RemoveAll(this);
	}

	Super::NativeDestruct();
}

/** 执行原生事件Touch结束。 */
FReply UHodgeSimulatedInputWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	FlushSimulatedInput();

	return Super::NativeOnTouchEnded(InGeometry, InGestureEvent);
}

/** 获取Enhanced输入Subsystem。 */
UEnhancedInputLocalPlayerSubsystem* UHodgeSimulatedInputWidget::GetEnhancedInputSubsystem() const
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ULocalPlayer* LP = GetOwningLocalPlayer())
		{
			return LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		}
	}
	return nullptr;
}

/** 获取玩家输入。 */
UEnhancedPlayerInput* UHodgeSimulatedInputWidget::GetPlayerInput() const
{
	if (UEnhancedInputLocalPlayerSubsystem* System = GetEnhancedInputSubsystem())
	{
		return System->GetPlayerInput();
	}
	return nullptr;
}

/** 向对应按键提交当前模拟输入值。 */
void UHodgeSimulatedInputWidget::InputKeyValue(const FVector& Value)
{
	// If we have an associated input action then we can use it
	if (AssociatedAction)
	{
		if (UEnhancedInputLocalPlayerSubsystem* System = GetEnhancedInputSubsystem())
		{
			// We don't want to apply any modifiers or triggers to this action, but they are required for the function signature
			TArray<UInputModifier*> Modifiers;
			TArray<UInputTrigger*> Triggers;
			System->InjectInputVectorForAction(AssociatedAction, Value, Modifiers, Triggers);
		}
	}
	// In case there is no associated input action, we can attempt to simulate input on the fallback key
	else if (UEnhancedPlayerInput* Input = GetPlayerInput())
	{
		if (KeyToSimulate.IsValid())
		{
			FInputKeyParams Params;
			Params.Delta = Value;
			Params.Key = KeyToSimulate;
			Params.NumSamples = 1;
			Params.DeltaTime = GetWorld()->GetDeltaSeconds();
			Params.bIsGamepadOverride = KeyToSimulate.IsGamepadKey();

			Input->InputKey(Params);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' is attempting to simulate input but has no player input!"),
		       *GetNameSafe(this));
	}
}

/** 向对应按键提交二维模拟输入值。 */
void UHodgeSimulatedInputWidget::InputKeyValue2D(const FVector2D& Value)
{
	InputKeyValue(FVector(Value.X, Value.Y, 0.0));
}

/** 释放尚未清理的模拟输入状态。 */
void UHodgeSimulatedInputWidget::FlushSimulatedInput()
{
	if (UEnhancedPlayerInput* Input = GetPlayerInput())
	{
		Input->FlushPressedKeys();
	}
}

/** 根据当前输入映射查询需要模拟的按键。 */
void UHodgeSimulatedInputWidget::QueryKeyToSimulate()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (UEnhancedInputLocalPlayerSubsystem* System = GetEnhancedInputSubsystem())
	{
		TArray<FKey> Keys = System->QueryKeysMappedToAction(AssociatedAction);
		if (!Keys.IsEmpty() && Keys[0].IsValid())
		{
			KeyToSimulate = Keys[0];
		}
		else
		{
			KeyToSimulate = FallbackBindingKey;
		}
	}
}

/** 处理控制Mappings重建。 */
void UHodgeSimulatedInputWidget::OnControlMappingsRebuilt()
{
	QueryKeyToSimulate();
}

#undef LOCTEXT_NAMESPACE
