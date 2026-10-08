// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #include "UI/Foundation/HodgeConfirmationScreen.h"

// #if WITH_EDITOR
// #include "CommonInputSettings.h"
// #include "Editor/WidgetCompilerLog.h"
// #endif

// #include "CommonBorder.h"
// #include "CommonRichTextBlock.h"
// #include "CommonTextBlock.h"
// #include "Components/DynamicEntryBox.h"
// #include "ICommonInputModule.h"
// #include "UI/Foundation/HodgeButtonBase.h"

// #include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeConfirmationScreen)

// void UHodgeConfirmationScreen::SetupDialog(UCommonGameDialogDescriptor* Descriptor,
//                                            FCommonMessagingResultDelegate ResultCallback)
// {
// 	Super::SetupDialog(Descriptor, ResultCallback);

// 	Text_Title->SetText(Descriptor->Header);
// 	RichText_Description->SetText(Descriptor->Body);

// 	EntryBox_Buttons->Reset<UHodgeButtonBase>([](UHodgeButtonBase& Button)
// 	{
// 		Button.OnClicked().Clear();
// 	});

// 	for (const FConfirmationDialogAction& Action : Descriptor->ButtonActions)
// 	{
// 		FDataTableRowHandle ActionRow;

// 		switch (Action.Result)
// 		{
// 		case ECommonMessagingResult::Confirmed:
// 			ActionRow = ICommonInputModule::GetSettings().GetDefaultClickAction();
// 			break;
// 		case ECommonMessagingResult::Declined:
// 			ActionRow = ICommonInputModule::GetSettings().GetDefaultBackAction();
// 			break;
// 		case ECommonMessagingResult::Cancelled:
// 			ActionRow = CancelAction;
// 			break;
// 		default:
// 			ensure(false);
// 			continue;
// 		}

// 		UHodgeButtonBase* Button = EntryBox_Buttons->CreateEntry<UHodgeButtonBase>();
// 		Button->SetTriggeringInputAction(ActionRow);
// 		Button->OnClicked().AddUObject(this, &ThisClass::CloseConfirmationWindow, Action.Result);
// 		Button->SetButtonText(Action.OptionalDisplayText);
// 	}

// 	OnResultCallback = ResultCallback;
// }

// void UHodgeConfirmationScreen::KillDialog()
// {
// 	Super::KillDialog();
// }

// void UHodgeConfirmationScreen::NativeOnInitialized()
// {
// 	Super::NativeOnInitialized();

// 	Border_TapToCloseZone->OnMouseButtonDownEvent.BindDynamic(
// 		this, &UHodgeConfirmationScreen::HandleTapToCloseZoneMouseButtonDown);
// }

// void UHodgeConfirmationScreen::CloseConfirmationWindow(ECommonMessagingResult Result)
// {
// 	DeactivateWidget();
// 	OnResultCallback.ExecuteIfBound(Result);
// }

// FEventReply UHodgeConfirmationScreen::HandleTapToCloseZoneMouseButtonDown(
// 	FGeometry MyGeometry, const FPointerEvent& MouseEvent)
// {
// 	FEventReply Reply;
// 	Reply.NativeReply = FReply::Unhandled();

// 	if (MouseEvent.IsTouchEvent() || MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
// 	{
// 		CloseConfirmationWindow(ECommonMessagingResult::Declined);
// 		Reply.NativeReply = FReply::Handled();
// 	}

// 	return Reply;
// }

// #if WITH_EDITOR
// void UHodgeConfirmationScreen::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
// {
// 	if (CancelAction.IsNull())
// 	{
// 		CompileLog.Error(FText::Format(FText::FromString(TEXT("{0} has unset property: CancelAction.")),
// 		                               FText::FromString(GetName())));
// 	}
// }
// #endif
