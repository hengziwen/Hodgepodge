// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "HodgeTabListWidgetBase.h"
#include "UI/Foundation/HodgeButtonBase.h"

#include "HodgeTabButtonBase.generated.h"

class UCommonLazyImage;
class UObject;
struct FFrame;
struct FSlateBrush;

UCLASS(Abstract, Blueprintable, meta = (DisableNativeTick))
class HODGEPODGE_API UHodgeTabButtonBase : public UHodgeButtonBase, public IHodgeTabButtonInterface
{
	GENERATED_BODY()

public:
	void SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject);
	void SetIconBrush(const FSlateBrush& Brush);

protected:
	UFUNCTION()
	virtual void SetTabLabelInfo_Implementation(const FHodgeTabDescriptor& TabLabelInfo) override;

private:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonLazyImage> LazyImage_Icon;
};
