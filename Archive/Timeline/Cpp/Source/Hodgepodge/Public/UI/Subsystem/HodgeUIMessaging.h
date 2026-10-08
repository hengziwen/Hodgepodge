// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #pragma once

// #include "Messaging/CommonMessagingSubsystem.h"
// #include "Templates/SubclassOf.h"
// #include "UObject/SoftObjectPtr.h"

// #include "HodgeUIMessaging.generated.h"

// class FSubsystemCollectionBase;
// class UCommonGameDialogDescriptor;
// class UObject;

// /**
//  * 
//  */
// UCLASS()
// class UHodgeUIMessaging : public UCommonMessagingSubsystem
// {
// 	GENERATED_BODY()

// public:
// 	UHodgeUIMessaging() { }

// 	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

// 	virtual void ShowConfirmation(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback = FCommonMessagingResultDelegate()) override;
// 	virtual void ShowError(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback = FCommonMessagingResultDelegate()) override;

// private:
// 	UPROPERTY()
// 	TSubclassOf<UCommonGameDialog> ConfirmationDialogClassPtr;

// 	UPROPERTY()
// 	TSubclassOf<UCommonGameDialog> ErrorDialogClassPtr;

// 	UPROPERTY(config)
// 	TSoftClassPtr<UCommonGameDialog> ConfirmationDialogClass;

// 	UPROPERTY(config)
// 	TSoftClassPtr<UCommonGameDialog> ErrorDialogClass;
// };
