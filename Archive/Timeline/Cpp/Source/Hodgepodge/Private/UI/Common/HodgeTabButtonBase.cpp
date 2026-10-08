// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Common/HodgeTabButtonBase.h"

#include "CommonLazyImage.h"
#include "UI/Common/HodgeTabListWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeTabButtonBase)

class UObject;
struct FSlateBrush;

void UHodgeTabButtonBase::SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrushFromLazyDisplayAsset(LazyObject);
	}
}

void UHodgeTabButtonBase::SetIconBrush(const FSlateBrush& Brush)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrush(Brush);
	}
}

void UHodgeTabButtonBase::SetTabLabelInfo_Implementation(const FHodgeTabDescriptor& TabLabelInfo)
{
	SetButtonText(TabLabelInfo.TabText);
	SetIconBrush(TabLabelInfo.IconBrush);
}

