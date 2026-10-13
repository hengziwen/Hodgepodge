/** @file HodgeLockOnIndicatorTests.cpp @brief 验证 Designer 尺寸真正进入 Slate，不被原生回退样式覆盖。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UI/Targeting/HodgeLockOnIndicatorWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "Widgets/SWidget.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeLockOnIndicatorDesignerTest,"Hodge.UI.LockOnIndicatorDesigner",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeLockOnIndicatorDesignerTest::RunTest(const FString&)
{
	auto* Widget = NewObject<UHodgeLockOnIndicatorWidget>();
	Widget->WidgetTree = NewObject<UWidgetTree>(Widget);
	auto* Box = Widget->WidgetTree->ConstructWidget<USizeBox>();
	Box->SetWidthOverride(64.f);Box->SetHeightOverride(48.f);Widget->WidgetTree->RootWidget=Box;
	const auto Slate = Widget->TakeWidget();Slate->SlatePrepass(1.f);
	TestTrue(TEXT("Designer 尺寸取代原生36方框"),Slate->GetDesiredSize().Equals(FVector2D(64.,48.),.1));
	Box->SetWidthOverride(80.f);Slate->SlatePrepass(1.f);
	TestTrue(TEXT("再次调整 Designer 尺寸真实生效"),Slate->GetDesiredSize().Equals(FVector2D(80.,48.),.1));
	Widget->ReleaseSlateResources(true);return true;
}
#endif
