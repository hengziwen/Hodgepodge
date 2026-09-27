// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #include "UI/PerformanceStats/HodgePerfStatWidgetBase.h"

// #include "Engine/GameInstance.h"
//#include "Performance/HodgePerformanceStatSubsystem.h"

// #include UE_INLINE_GENERATED_CPP_BY_NAME(HodgePerfStatWidgetBase)

//////////////////////////////////////////////////////////////////////
// UHodgePerfStatWidgetBase

// UHodgePerfStatWidgetBase::UHodgePerfStatWidgetBase(const FObjectInitializer& ObjectInitializer)
// 	: Super(ObjectInitializer)
// {
// }

// double UHodgePerfStatWidgetBase::FetchStatValue()
// {
// 	if (CachedStatSubsystem == nullptr)
// 	{
// 		if (UWorld* World = GetWorld())
// 		{
// 			if (UGameInstance* GameInstance = World->GetGameInstance())
// 			{
// 				CachedStatSubsystem = GameInstance->GetSubsystem<UHodgePerformanceStatSubsystem>();
// 			}
// 		}
// 	}

	// if (CachedStatSubsystem)
	// {
	// 	return CachedStatSubsystem->GetCachedStat(StatToDisplay);
	// }
// 	else
// 	{
// 		return 0.0;
// 	}
// }
