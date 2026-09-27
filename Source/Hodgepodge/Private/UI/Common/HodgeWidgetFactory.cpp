// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Common/HodgeWidgetFactory.h"
#include "Templates/SubclassOf.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeWidgetFactory)

class UUserWidget;

TSubclassOf<UUserWidget> UHodgeWidgetFactory::FindWidgetClassForData_Implementation(const UObject* Data) const
{
	return TSubclassOf<UUserWidget>();
}
