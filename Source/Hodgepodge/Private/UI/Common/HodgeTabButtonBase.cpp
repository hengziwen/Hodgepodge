/** @file HodgeTabButtonBase.cpp
 * @brief 实现标签页按钮相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Common/HodgeTabButtonBase.h"

#include "CommonLazyImage.h"
#include "UI/Common/HodgeTabListWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeTabButtonBase)

class UObject;
struct FSlateBrush;

/** 设置Icon来源Lazy对象。 */
void UHodgeTabButtonBase::SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrushFromLazyDisplayAsset(LazyObject);
	}
}

/** 设置Icon画刷。 */
void UHodgeTabButtonBase::SetIconBrush(const FSlateBrush& Brush)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrush(Brush);
	}
}

/** 设置Tab标签信息。 */
void UHodgeTabButtonBase::SetTabLabelInfo_Implementation(const FHodgeTabDescriptor& TabLabelInfo)
{
	SetButtonText(TabLabelInfo.TabText);
	SetIconBrush(TabLabelInfo.IconBrush);
}

