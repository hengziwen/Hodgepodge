/** @file HodgeLockOnIndicatorWidget.cpp @brief 仅生成标记样式，不扫描敌人或更新目标状态。 */
#include "UI/Targeting/HodgeLockOnIndicatorWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeLockOnIndicatorWidget)
TSharedRef<SWidget> UHodgeLockOnIndicatorWidget::RebuildWidget()
{
	// 蓝图已有 Designer 根控件时使用真实 WidgetTree，原生菱形仅为缺省回退。
	if (WidgetTree && WidgetTree->RootWidget) { return Super::RebuildWidget(); }
	return SNew(SBox).WidthOverride(36.f).HeightOverride(36.f)
	[SNew(STextBlock).Text(FText::FromString(TEXT("◇"))).Justification(ETextJustify::Center)
	.ColorAndOpacity(FLinearColor::White).Font(FCoreStyle::GetDefaultFontStyle("Bold",26))];
}
