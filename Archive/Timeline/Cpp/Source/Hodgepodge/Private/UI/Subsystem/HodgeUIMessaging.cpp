// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// // Copyright Epic Games, Inc. All Rights Reserved.
//
// #include "UI/Subsystem/HodgeUIMessaging.h"
//
// #include "Messaging/CommonGameDialog.h"
// #include "NativeGameplayTags.h"
// #include "CommonLocalPlayer.h"
// #include "PrimaryGameLayout.h"
// #include "Widgets/CommonActivatableWidgetContainer.h"
//
// #include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeUIMessaging)
//
// class FSubsystemCollectionBase;
//
// UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MODAL, "UI.Layer.Modal");
//
// void UHodgeUIMessaging::Initialize(FSubsystemCollectionBase& Collection)
// {
// 	Super::Initialize(Collection);
//
// 	ConfirmationDialogClassPtr = ConfirmationDialogClass.LoadSynchronous();
// 	ErrorDialogClassPtr = ErrorDialogClass.LoadSynchronous();
// }
//
// void UHodgeUIMessaging::ShowConfirmation(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
// {
// 	if (UCommonLocalPlayer* LocalPlayer = GetLocalPlayer<UCommonLocalPlayer>())
// 	{
// 		if (UPrimaryGameLayout* RootLayout = LocalPlayer->GetRootUILayout())
// 		{
// 			RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(TAG_UI_LAYER_MODAL, ConfirmationDialogClassPtr, [DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog) {
// 				Dialog.SetupDialog(DialogDescriptor, ResultCallback);
// 			});
// 		}
// 	}
// }
//
// void UHodgeUIMessaging::ShowError(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
// {
// 	if (UCommonLocalPlayer* LocalPlayer = GetLocalPlayer<UCommonLocalPlayer>())
// 	{
// 		if (UPrimaryGameLayout* RootLayout = LocalPlayer->GetRootUILayout())
// 		{
// 			RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(TAG_UI_LAYER_MODAL, ErrorDialogClassPtr, [DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog) {
// 				Dialog.SetupDialog(DialogDescriptor, ResultCallback);
// 			});
// 		}
// 	}
// }
