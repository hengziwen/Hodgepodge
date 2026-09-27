// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #pragma once

// #include "Engine/LocalPlayer.h"
// #include "GameplayTagContainer.h"
// #include "Styling/CoreStyle.h"
// #include "Styling/SlateBrush.h"
// #include "Widgets/DeclarativeSyntaxSupport.h"
// #include "Widgets/Accessibility/SlateWidgetAccessibleTypes.h"
// #include "Widgets/SLeafWidget.h"

// class FPaintArgs;
// class FSlateRect;
// class FSlateWindowElementList;
// class FWidgetStyle;
// struct FGameplayTag;
// struct FGeometry;
// struct FSlateBrush;

// class SHitMarkerConfirmationWidget : public SLeafWidget
// {
// 	SLATE_BEGIN_ARGS(SHitMarkerConfirmationWidget)
// 		: _PerHitMarkerImage(FCoreStyle::Get().GetBrush("Throbber.CircleChunk"))
// 		, _AnyHitsMarkerImage(nullptr)
// 		, _HitNotifyDuration(0.4f)
// 	{
// 	}
// 		/** The marker image to draw for individual hit markers. */
// 		SLATE_ARGUMENT(const FSlateBrush*, PerHitMarkerImage)
// 		/** The marker image to draw if there are any hits at all. */
// 		SLATE_ARGUMENT(const FSlateBrush*, AnyHitsMarkerImage)
// 		/** The duration (in seconds) to display hit notifies (they fade to transparent over this time)  */
// 		SLATE_ATTRIBUTE(float, HitNotifyDuration)
// 		/** The color and opacity of the marker */
// 		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)
// 	SLATE_END_ARGS()

// public:
// 	void Construct(const FArguments& InArgs, const FLocalPlayerContext& InContext, const TMap<FGameplayTag, FSlateBrush>& ZoneOverrideImages);

// 	SHitMarkerConfirmationWidget();

	//~SWidget interface
// 	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
// 	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
// 	virtual FVector2D ComputeDesiredSize(float) const override;
// 	virtual bool ComputeVolatility() const override { return true; }
	//~End of SWidget interface

// private:
// 	/** The marker image to draw for individual hit markers. */
// 	const FSlateBrush* PerHitMarkerImage = nullptr;

// 	/** Map from zone tag (e.g., weak spot) to override marker images. */
// 	TMap<FGameplayTag, FSlateBrush> PerHitMarkerZoneOverrideImages;

// 	/** The marker image to draw if there are any hits at all. */
// 	const FSlateBrush* AnyHitsMarkerImage = nullptr;

// 	/** The opacity for the hit markers */
// 	float HitNotifyOpacity = 0.0f;

// 	/** The duration (in seconds) to display hit notifies (they fade to transparent over this time)  */
// 	float HitNotifyDuration = 0.4f;

// 	/** Color and opacity of the markers */
// 	TAttribute<FSlateColor> ColorAndOpacity;
// 	bool bColorAndOpacitySet;

// 	/** Player context for the owning HUD */
// 	FLocalPlayerContext MyContext;
// };
