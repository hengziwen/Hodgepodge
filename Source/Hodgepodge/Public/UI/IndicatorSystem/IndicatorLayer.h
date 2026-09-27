// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// UWidget 是 UMG 中所有基础 Widget 的基类。
// UIndicatorLayer 作为一个 UMG Widget，可以被放入 Widget Blueprint 的 UI 层级中。
#include "Components/Widget.h"

#include "IndicatorLayer.generated.h"

// Indicator 系统底层使用的 Slate Canvas。
// 真正负责 Indicator 的投影、布局、排序、屏幕边缘 Clamp 等底层显示逻辑。
class SActorCanvas;

// Slate Widget 前向声明。
class SWidget;

class UObject;

// Indicator 系统的 UMG 容器层。
// 负责在 UMG Widget Tree 中提供一个 Indicator 显示区域，
// 并在内部创建和持有真正执行 Indicator 布局工作的 SActorCanvas。
UCLASS()
class UIndicatorLayer : public UWidget
{
	GENERATED_UCLASS_BODY()

public:
	/** Default arrow brush to use if UI is clamped to the screen and needs to show an arrow. */
	// Indicator 被限制到屏幕边缘，并且需要显示方向箭头时使用的默认 Slate Brush。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FSlateBrush ArrowBrush;

protected:
	// UWidget interface

	// 释放当前 UWidget 持有的 Slate 资源。
	// IndicatorLayer 销毁或重建时，需要在这里释放 MyActorCanvas 等 Slate 对象引用。
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	// 根据当前 UMG Widget 重建其底层 Slate Widget。
	// UIndicatorLayer 会在这里创建并返回实际负责 Indicator 布局的 SActorCanvas。
	virtual TSharedRef<SWidget> RebuildWidget() override;

	// End UWidget

protected:
	// 当前 IndicatorLayer 对应的底层 Slate ActorCanvas。
	// UIndicatorLayer 负责提供 UMG 层入口，
	// SActorCanvas 则负责真正的 Indicator 投影、布局和显示处理。
	TSharedPtr<SActorCanvas> MyActorCanvas;
};
