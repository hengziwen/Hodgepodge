// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Foundation/HodgeLoadingScreenSubsystem.h"

#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeLoadingScreenSubsystem)

class UUserWidget;

//////////////////////////////////////////////////////////////////////
// UHodgeLoadingScreenSubsystem

UHodgeLoadingScreenSubsystem::UHodgeLoadingScreenSubsystem()
{
}

void UHodgeLoadingScreenSubsystem::SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (LoadingScreenWidgetClass != NewWidgetClass)
	{
		LoadingScreenWidgetClass = NewWidgetClass;

		OnLoadingScreenWidgetChanged.Broadcast(LoadingScreenWidgetClass);
	}
}

TSubclassOf<UUserWidget> UHodgeLoadingScreenSubsystem::GetLoadingScreenContentWidget() const
{
	return LoadingScreenWidgetClass;
}

