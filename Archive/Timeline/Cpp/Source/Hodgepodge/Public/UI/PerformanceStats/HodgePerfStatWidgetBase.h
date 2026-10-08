// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #pragma once

// #include "CommonUserWidget.h"

// #include "HodgePerfStatWidgetBase.generated.h"

// enum class EHodgeDisplayablePerformanceStat : uint8;

// class UHodgePerformanceStatSubsystem;
// class UObject;
// struct FFrame;

// /**
//  * UHodgePerfStatWidgetBase
//  *
//  * Base class for a widget that displays a single stat, e.g., FPS, ping, etc...
//  */
//  UCLASS(Abstract)
// class UHodgePerfStatWidgetBase : public UCommonUserWidget
// {
// public:
// 	UHodgePerfStatWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

// 	GENERATED_BODY()

// public:
	// Returns the stat this widget is supposed to display
// 	UFUNCTION(BlueprintPure)
// 	EHodgeDisplayablePerformanceStat GetStatToDisplay() const
// 	{
// 		return StatToDisplay;
// 	}

	// Polls for the value of this stat (unscaled)
// 	UFUNCTION(BlueprintPure)
// 	double FetchStatValue();

// protected:
	// Cached subsystem pointer
// 	UPROPERTY(Transient)
// 	TObjectPtr<UHodgePerformanceStatSubsystem> CachedStatSubsystem;

	// The stat to display
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Display)
// 	EHodgeDisplayablePerformanceStat StatToDisplay;
//  };
