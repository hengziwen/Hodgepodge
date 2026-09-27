// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

#include "HodgeWidgetFactory.generated.h"

template <class TClass>
class TSubclassOf;

class UUserWidget;
struct FFrame;

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class HODGEPODGE_API UHodgeWidgetFactory : public UObject
{
	GENERATED_BODY()

public:
	UHodgeWidgetFactory()
	{
	}

	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<UUserWidget> FindWidgetClassForData(const UObject* Data) const;
};
