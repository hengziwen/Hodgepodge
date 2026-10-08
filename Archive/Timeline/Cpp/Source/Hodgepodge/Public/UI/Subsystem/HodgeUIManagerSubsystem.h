// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #pragma once

// #include "Containers/Ticker.h"
// #include "GameUIManagerSubsystem.h"

// #include "HodgeUIManagerSubsystem.generated.h"

// class FSubsystemCollectionBase;
// class UObject;

// UCLASS()
// class UHodgeUIManagerSubsystem : public UGameUIManagerSubsystem
// {
// 	GENERATED_BODY()

// public:

// 	UHodgeUIManagerSubsystem();

// 	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
// 	virtual void Deinitialize() override;

// private:
// 	bool Tick(float DeltaTime);
// 	void SyncRootLayoutVisibilityToShowHUD();
	
// 	FTSTicker::FDelegateHandle TickHandle;
// };
