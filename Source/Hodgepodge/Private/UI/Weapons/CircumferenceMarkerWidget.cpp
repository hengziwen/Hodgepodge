/** @file CircumferenceMarkerWidget.cpp
 * @brief 实现圆周标记控件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Weapons/CircumferenceMarkerWidget.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(CircumferenceMarkerWidget)

class SWidget;

/** 构造圆周标记控件并初始化其默认状态。 */
UCircumferenceMarkerWidget::UCircumferenceMarkerWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	bIsVolatile = true;
}

/** 释放Slate界面Resources。 */
void UCircumferenceMarkerWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyMarkerWidget.Reset();
}

/** 根据当前属性创建对应的Slate控件。 */
TSharedRef<SWidget> UCircumferenceMarkerWidget::RebuildWidget()
{
	MyMarkerWidget = SNew(SCircumferenceMarkerWidget)
		.MarkerBrush(&MarkerImage)
		.Radius(this->Radius)
		.MarkerList(this->MarkerList);

	return MyMarkerWidget.ToSharedRef();
}

/** 将配置属性同步到现有控件或表现资源。 */
void UCircumferenceMarkerWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyMarkerWidget->SetRadius(Radius);
	MyMarkerWidget->SetMarkerList(MarkerList);
}

/** 设置半径。 */
void UCircumferenceMarkerWidget::SetRadius(float InRadius)
{
	Radius = InRadius;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (MyMarkerWidget.IsValid())
	{
		MyMarkerWidget->SetRadius(InRadius);
	}
}
