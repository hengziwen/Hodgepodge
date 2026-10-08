// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #include "UI/PerformanceStats/HodgePerfStatContainerBase.h"

// #include "Blueprint/WidgetTree.h"
// #include "UI/PerformanceStats/HodgePerfStatWidgetBase.h"
//#include "Settings/HodgeSettingsLocal.h"

// #include UE_INLINE_GENERATED_CPP_BY_NAME(HodgePerfStatContainerBase)

//////////////////////////////////////////////////////////////////////
// UHodgePerfStatsContainerBase

// UHodgePerfStatContainerBase::UHodgePerfStatContainerBase(const FObjectInitializer& ObjectInitializer)
// 	: Super(ObjectInitializer)
// {
// }

// void UHodgePerfStatContainerBase::NativeConstruct()
// {
// 	Super::NativeConstruct();
// 	UpdateVisibilityOfChildren();

	//UHodgeSettingsLocal::Get()->OnPerfStatDisplayStateChanged().AddUObject(this, &ThisClass::UpdateVisibilityOfChildren);
// }

// void UHodgePerfStatContainerBase::NativeDestruct()
// {
	//UHodgeSettingsLocal::Get()->OnPerfStatDisplayStateChanged().RemoveAll(this);

// 	Super::NativeDestruct();
// }

// void UHodgePerfStatContainerBase::UpdateVisibilityOfChildren()
// {
	// UHodgeSettingsLocal* UserSettings = UHodgeSettingsLocal::Get();
	//
	// const bool bShowTextWidgets = (StatDisplayModeFilter == EHodgeStatDisplayMode::TextOnly) || (StatDisplayModeFilter == EHodgeStatDisplayMode::TextAndGraph);
	// const bool bShowGraphWidgets = (StatDisplayModeFilter == EHodgeStatDisplayMode::GraphOnly) || (StatDisplayModeFilter == EHodgeStatDisplayMode::TextAndGraph);
	//
	// check(WidgetTree);
	// WidgetTree->ForEachWidget([&](UWidget* Widget)
	// {
	// 	if (UHodgePerfStatWidgetBase* TypedWidget = Cast<UHodgePerfStatWidgetBase>(Widget))
	// 	{
	// 		const EHodgeStatDisplayMode SettingMode = UserSettings->GetPerfStatDisplayState(TypedWidget->GetStatToDisplay());
	//
	// 		bool bShowWidget = false;
	// 		switch (SettingMode)
	// 		{
	// 		case EHodgeStatDisplayMode::Hidden:
	// 			bShowWidget = false;
	// 			break;
	// 		case EHodgeStatDisplayMode::TextOnly:
	// 			bShowWidget = bShowTextWidgets;
	// 			break;
	// 		case EHodgeStatDisplayMode::GraphOnly:
	// 			bShowWidget = bShowGraphWidgets;
	// 			break;
	// 		case EHodgeStatDisplayMode::TextAndGraph:
	// 			bShowWidget = bShowTextWidgets || bShowGraphWidgets;
	// 			break;
	// 		}
	//
	// 		TypedWidget->SetVisibility(bShowWidget ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	// 	}
	// });
// }

