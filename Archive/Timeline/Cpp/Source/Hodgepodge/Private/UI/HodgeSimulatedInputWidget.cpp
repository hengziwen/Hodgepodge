// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/HodgeSimulatedInputWidget.h"
#include "EnhancedInputSubsystems.h"
//#include "HodgeLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeSimulatedInputWidget)

#define LOCTEXT_NAMESPACE "HodgeSimulatedInputWidget"

UHodgeSimulatedInputWidget::UHodgeSimulatedInputWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetConsumePointerInput(true);
}

#if WITH_EDITOR
const FText UHodgeSimulatedInputWidget::GetPaletteCategory()
{
	return LOCTEXT("PalleteCategory", "Input");
}
#endif // WITH_EDITOR

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

void UHodgeSimulatedInputWidget::NativeDestruct()
{
	if (UEnhancedInputLocalPlayerSubsystem* System = GetEnhancedInputSubsystem())
	{
		System->ControlMappingsRebuiltDelegate.RemoveAll(this);
	}

	Super::NativeDestruct();
}

FReply UHodgeSimulatedInputWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	FlushSimulatedInput();

	return Super::NativeOnTouchEnded(InGeometry, InGestureEvent);
}

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

UEnhancedPlayerInput* UHodgeSimulatedInputWidget::GetPlayerInput() const
{
	if (UEnhancedInputLocalPlayerSubsystem* System = GetEnhancedInputSubsystem())
	{
		return System->GetPlayerInput();
	}
	return nullptr;
}

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

void UHodgeSimulatedInputWidget::InputKeyValue2D(const FVector2D& Value)
{
	InputKeyValue(FVector(Value.X, Value.Y, 0.0));
}

void UHodgeSimulatedInputWidget::FlushSimulatedInput()
{
	if (UEnhancedPlayerInput* Input = GetPlayerInput())
	{
		Input->FlushPressedKeys();
	}
}

void UHodgeSimulatedInputWidget::QueryKeyToSimulate()
{
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

void UHodgeSimulatedInputWidget::OnControlMappingsRebuilt()
{
	QueryKeyToSimulate();
}

#undef LOCTEXT_NAMESPACE
