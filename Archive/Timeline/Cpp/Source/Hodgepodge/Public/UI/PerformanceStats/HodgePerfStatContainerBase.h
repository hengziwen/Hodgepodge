// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #pragma once

// #include "CommonUserWidget.h"
// #include "Performance/HodgePerformanceStatTypes.h"

// #include "HodgePerfStatContainerBase.generated.h"

// class UObject;
// struct FFrame;

// /**
//  * UHodgePerfStatsContainerBase
//  *
//  * Panel that contains a set of UHodgePerfStatWidgetBase widgets and manages
//  * their visibility based on user settings.
//  */
// UCLASS(Abstract)
// class UHodgePerfStatContainerBase : public UCommonUserWidget
// {
// public:
// 	UHodgePerfStatContainerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

// 	GENERATED_BODY()

	//~UUserWidget interface
// 	virtual void NativeConstruct() override;
// 	virtual void NativeDestruct() override;
	//~End of UUserWidget interface

// 	UFUNCTION(BlueprintCallable)
// 	void UpdateVisibilityOfChildren();

// protected:
	// Are we showing text or graph stats?
// 	UPROPERTY(EditAnywhere, Category=Display)
// 	EHodgeStatDisplayMode StatDisplayModeFilter = EHodgeStatDisplayMode::TextAndGraph;
// };
