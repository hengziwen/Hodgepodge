/** @file SCircumferenceMarkerWidget.cpp
 * @brief 实现圆周标记控件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Weapons/SCircumferenceMarkerWidget.h"

#include "Engine/UserInterfaceSettings.h"
#include "Styling/SlateBrush.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SCircumferenceMarkerWidget)

class FPaintArgs;
class FSlateRect;

/** 构造圆周标记控件并初始化其默认状态。 */
SCircumferenceMarkerWidget::SCircumferenceMarkerWidget()
{
}

/** 根据传入参数建立当前Slate控件。 */
void SCircumferenceMarkerWidget::Construct(const FArguments& InArgs)
{
	MarkerBrush = InArgs._MarkerBrush;
	MarkerList = InArgs._MarkerList;
	Radius = InArgs._Radius;
	bColorAndOpacitySet = InArgs._ColorAndOpacity.IsSet();
	ColorAndOpacity = InArgs._ColorAndOpacity;
}

/** 获取标记渲染变换。 */
FSlateRenderTransform SCircumferenceMarkerWidget::GetMarkerRenderTransform(
	const FCircumferenceMarkerEntry& Marker, const float BaseRadius, const float HUDScale) const
{
	// Determine the radius to use for the corners
	float XRadius = BaseRadius;
	float YRadius = BaseRadius;
	if (bReticleCornerOutsideSpreadRadius)
	{
		XRadius += MarkerBrush->ImageSize.X * 0.5f;
		YRadius += MarkerBrush->ImageSize.X * 0.5f;
	}

	// Get the angle and orientation for this reticle corner
	const float LocalRotationRadians = FMath::DegreesToRadians(Marker.ImageRotationAngle);
	const float PositionAngleRadians = FMath::DegreesToRadians(Marker.PositionAngle);

	// First rotate the corner image about the origin
	FSlateRenderTransform RotateAboutOrigin(Concatenate(
		FVector2D(-MarkerBrush->ImageSize.X * 0.5f, -MarkerBrush->ImageSize.Y * 0.5f), FQuat2D(LocalRotationRadians),
		FVector2D(MarkerBrush->ImageSize.X * 0.5f, MarkerBrush->ImageSize.Y * 0.5f)));

	// Move the rotated image to the right place on the spread radius
	return TransformCast<FSlateRenderTransform>(Concatenate(RotateAboutOrigin,
	                                                        FVector2D(
		                                                        XRadius * FMath::Sin(PositionAngleRadians) * HUDScale,
		                                                        -YRadius * FMath::Cos(
			                                                        PositionAngleRadians) * HUDScale)));
}

/** 把当前控件与其子元素提交给Slate绘制。 */
int32 SCircumferenceMarkerWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                                          const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
                                          int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
	const ESlateDrawEffect DrawEffects = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
	const FVector2D LocalCenter = AllottedGeometry.GetLocalPositionAtCoordinates(FVector2D(0.5f, 0.5f));

	const bool bDrawMarkers = (MarkerList.Num() > 0) && (MarkerBrush != nullptr);

	if (bDrawMarkers == true)
	{
		const FLinearColor MarkerColor = bColorAndOpacitySet
			                                 ? ColorAndOpacity.Get().GetColor(InWidgetStyle)
			                                 : (InWidgetStyle.GetColorAndOpacityTint() * MarkerBrush->GetTint(
				                                 InWidgetStyle));

		if (MarkerColor.A > KINDA_SMALL_NUMBER)
		{
			const float BaseRadius = Radius.Get();
			const float ApplicationScale = GetDefault<UUserInterfaceSettings>()->ApplicationScale;
			for (const FCircumferenceMarkerEntry& Marker : MarkerList)
			{
				const FSlateRenderTransform MarkerTransform = GetMarkerRenderTransform(
					Marker, BaseRadius, ApplicationScale);

				const FPaintGeometry Geometry(AllottedGeometry.ToPaintGeometry(
					MarkerBrush->ImageSize, FSlateLayoutTransform(LocalCenter - (MarkerBrush->ImageSize * 0.5f)),
					MarkerTransform, FVector2D(0.0f, 0.0f)));
				FSlateDrawElement::MakeBox(OutDrawElements, LayerId, Geometry, MarkerBrush, DrawEffects, MarkerColor);
			}
		}
	}

	return LayerId;
}

/** 计算当前Slate控件期望占用的尺寸。 */
FVector2D SCircumferenceMarkerWidget::ComputeDesiredSize(float) const
{
	check(MarkerBrush);
	const float SampledRadius = Radius.Get();
	return FVector2D((MarkerBrush->ImageSize.X + SampledRadius) * 2.0f,
	                 (MarkerBrush->ImageSize.Y + SampledRadius) * 2.0f);
}

/** 设置半径。 */
void SCircumferenceMarkerWidget::SetRadius(float NewRadius)
{
	if (Radius.IsBound() || (Radius.Get() != NewRadius))
	{
		Radius = NewRadius;
		Invalidate(EInvalidateWidgetReason::Layout);
	}
}

/** 设置标记列表。 */
void SCircumferenceMarkerWidget::SetMarkerList(TArray<FCircumferenceMarkerEntry>& NewMarkerList)
{
	MarkerList = NewMarkerList;
}
