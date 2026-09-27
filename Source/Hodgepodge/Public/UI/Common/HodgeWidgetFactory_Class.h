// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "HodgeWidgetFactory.h"
#include "Templates/SubclassOf.h"
#include "UObject/SoftObjectPtr.h"

#include "HodgeWidgetFactory_Class.generated.h"

class UObject;
class UUserWidget;

UCLASS()
class HODGEPODGE_API UHodgeWidgetFactory_Class : public UHodgeWidgetFactory
{
	GENERATED_BODY()

public:
	UHodgeWidgetFactory_Class()
	{
	}

	virtual TSubclassOf<UUserWidget> FindWidgetClassForData_Implementation(const UObject* Data) const override;

protected:
	UPROPERTY(EditAnywhere, Category = ListEntries, meta = (AllowAbstract))
	TMap<TSoftClassPtr<UObject>, TSubclassOf<UUserWidget>> EntryWidgetForClass;
};
