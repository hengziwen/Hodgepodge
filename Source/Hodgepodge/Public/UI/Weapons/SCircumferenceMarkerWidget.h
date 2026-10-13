/** @file SCircumferenceMarkerWidget.h
 * @brief 声明圆周标记控件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Styling/CoreStyle.h"
#include "Widgets/Accessibility/SlateWidgetAccessibleTypes.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"

#include "SCircumferenceMarkerWidget.generated.h"

class FPaintArgs;
class FSlateRect;
class FSlateWindowElementList;
class FWidgetStyle;
struct FGeometry;
struct FSlateBrush;

/** 保存圆周标记条目的数据和接口契约。 */
USTRUCT(BlueprintType)
struct FCircumferenceMarkerEntry
{
	GENERATED_BODY()

	// The angle to place this marker around the circle (in degrees)
	// 位置角度的数值或阈值。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ForceUnits=deg))
	float PositionAngle = 0.0f;

	// The angle to rotate the marker image (in degrees)
	// 图像旋转角度的数值或阈值。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ForceUnits=deg))
	float ImageRotationAngle = 0.0f;
};

/** 封装圆周标记控件的状态与操作接口。 */
class SCircumferenceMarkerWidget : public SLeafWidget
{
	SLATE_BEGIN_ARGS(SCircumferenceMarkerWidget)
		: _MarkerBrush(FCoreStyle::Get().GetBrush("Throbber.CircleChunk"))
		, _Radius(48.0f)
	{
	}
		/** What each marker on the circumference looks like */
		SLATE_ARGUMENT(const FSlateBrush*, MarkerBrush)
		/** At which angles should a marker be drawn */
		SLATE_ARGUMENT(TArray<FCircumferenceMarkerEntry>, MarkerList)
		/** The radius of the circle */
		SLATE_ATTRIBUTE(float, Radius)
		/** The color and opacity of the marker */
		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)
	SLATE_END_ARGS()

public:
	/** 根据传入参数建立当前Slate控件。
	 * @param InArgs Slate声明式控件构造参数。
	 */
	void Construct(const FArguments& InArgs);

	/** 构造圆周标记控件并初始化其默认状态。 */
	SCircumferenceMarkerWidget();

	//~SWidget interface
	/** 把当前控件与其子元素提交给Slate绘制。
	 * @param Args Args，由调用方传入。
	 * @param AllottedGeometry Allotted几何，由调用方传入。
	 * @param MyCullingRect MyCulling矩形，由调用方传入。
	 * @param OutDrawElements 输出绘制Elements。
	 * @param LayerId 层标识，由调用方传入。
	 * @param InWidgetStyle 控件风格，由调用方传入。
	 * @param bParentEnabled 是否父项启用的控制标记。
	 * @return 本次计算、查询或创建得到的整数32。
	 */
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	/** 计算当前Slate控件期望占用的尺寸。
	 * float参数；用于保持该接口的调用签名。
	 * @return 本次计算、查询或创建得到的向量2D。
	 */
	virtual FVector2D ComputeDesiredSize(float) const override;
	/** 判断该控件是否需要持续更新绘制状态。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool ComputeVolatility() const override { return true; }
	//~End of SWidget interface

	/** 设置半径。
	 * @param NewRadius 新半径，由调用方传入。
	 */
	void SetRadius(float NewRadius);
	/** 设置标记列表。
	 * @param NewMarkerList 新标记列表，由调用方传入。
	 */
	void SetMarkerList(TArray<FCircumferenceMarkerEntry>& NewMarkerList);

private:
	/** 获取标记渲染变换。
	 * @param Marker 标记，由调用方传入。
	 * @param BaseRadius 基础半径，由调用方传入。
	 * @param HUDScale HUD界面缩放，由调用方传入。
	 * @return 标记渲染变换的当前值或引用。
	 */
	FSlateRenderTransform GetMarkerRenderTransform(const FCircumferenceMarkerEntry& Marker, const float BaseRadius, const float HUDScale) const;

private:
	/** What each marker on the circumference looks like */
	// 标记画刷的数据或对象引用。
	const FSlateBrush* MarkerBrush;

	/** Angles around the reticle center to place ReticleCornerImage icons */
	// 标记列表的记录集合。
	TArray<FCircumferenceMarkerEntry> MarkerList;

	/** The radius of the circle */
	// 半径的数据或对象引用。
	TAttribute<float> Radius;

	/** Color and opacity of the markers */
	// 颜色And不透明度的数据或对象引用。
	TAttribute<FSlateColor> ColorAndOpacity;
	// 控制是否颜色And不透明度设置的状态标记。
	bool bColorAndOpacitySet;

	/** Whether reticle corner images are placed outside the spread radius */
	//@TODO: Make this a 0-1 float alignment instead (e.g., inside/on/outside the radius)?
	// 控制是否准星角点OutsideSpread半径的状态标记。
	uint8 bReticleCornerOutsideSpreadRadius : 1;
};
