// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #pragma once

// #include "Components/SlateWrapperTypes.h"
// #include "Engine/DataTable.h"
// #include "Messaging/CommonGameDialog.h"

// #include "Messaging/CommonMessagingSubsystem.h"
// #include "UI/Subsystem/HodgeUIMessaging.h"
// #include "HodgeConfirmationScreen.generated.h"

// class IWidgetCompilerLog;

// class UCommonTextBlock;
// class UCommonRichTextBlock;
// class UDynamicEntryBox;
// class UCommonBorder;

// /**
//  *	
//  */
// UCLASS(Abstract, BlueprintType, Blueprintable)
// class UHodgeConfirmationScreen : public UCommonGameDialog
// {
// 	GENERATED_BODY()
// public:
// 	virtual void SetupDialog(UCommonGameDialogDescriptor* Descriptor, FCommonMessagingResultDelegate ResultCallback) override;
// 	virtual void KillDialog() override;

// protected:
// 	virtual void NativeOnInitialized() override;
// 	virtual void CloseConfirmationWindow(ECommonMessagingResult Result);

// #if WITH_EDITOR
// 	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
// #endif

// private:

// 	UFUNCTION()
// 	FEventReply HandleTapToCloseZoneMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

// 	FCommonMessagingResultDelegate OnResultCallback;

// private:
// 	UPROPERTY(Meta = (BindWidget))
// 	TObjectPtr<UCommonTextBlock> Text_Title;

// 	UPROPERTY(Meta = (BindWidget))
// 	TObjectPtr<UCommonRichTextBlock> RichText_Description;

// 	UPROPERTY(Meta = (BindWidget))
// 	TObjectPtr<UDynamicEntryBox> EntryBox_Buttons;

// 	UPROPERTY(Meta = (BindWidget))
// 	TObjectPtr<UCommonBorder> Border_TapToCloseZone;

// 	UPROPERTY(EditDefaultsOnly, meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
// 	FDataTableRowHandle CancelAction;
// };
