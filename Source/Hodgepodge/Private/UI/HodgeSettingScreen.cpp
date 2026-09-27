// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// // Copyright Epic Games, Inc. All Rights Reserved.
//
// #include "UI/HodgeSettingScreen.h"
//
// #include "Input/CommonUIInputTypes.h"
// #include "Player/HodgeLocalPlayer.h"
// #include "Settings/HodgeGameSettingRegistry.h"
//
// #include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeSettingScreen)
//
// class UGameSettingRegistry;
//
// void UHodgeSettingScreen::NativeOnInitialized()
// {
// 	Super::NativeOnInitialized();
//
// 	BackHandle = RegisterUIActionBinding(FBindUIActionArgs(BackInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction)));
// 	ApplyHandle = RegisterUIActionBinding(FBindUIActionArgs(ApplyInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleApplyAction)));
// 	CancelChangesHandle = RegisterUIActionBinding(FBindUIActionArgs(CancelChangesInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCancelChangesAction)));
// }
//
// UGameSettingRegistry* UHodgeSettingScreen::CreateRegistry()
// {
// 	UHodgeGameSettingRegistry* NewRegistry = NewObject<UHodgeGameSettingRegistry>();
//
// 	if (UHodgeLocalPlayer* LocalPlayer = CastChecked<UHodgeLocalPlayer>(GetOwningLocalPlayer()))
// 	{
// 		NewRegistry->Initialize(LocalPlayer);
// 	}
//
// 	return NewRegistry;
// }
//
// void UHodgeSettingScreen::HandleBackAction()
// {
// 	if (AttemptToPopNavigation())
// 	{
// 		return;
// 	}
//
// 	ApplyChanges();
//
// 	DeactivateWidget();
// }
//
// void UHodgeSettingScreen::HandleApplyAction()
// {
// 	ApplyChanges();
// }
//
// void UHodgeSettingScreen::HandleCancelChangesAction()
// {
// 	CancelChanges();
// }
//
// void UHodgeSettingScreen::OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty)
// {
// 	if (bSettingsDirty)
// 	{
// 		if (!GetActionBindings().Contains(ApplyHandle))
// 		{
// 			AddActionBinding(ApplyHandle);
// 		}
// 		if (!GetActionBindings().Contains(CancelChangesHandle))
// 		{
// 			AddActionBinding(CancelChangesHandle);
// 		}
// 	}
// 	else
// 	{
// 		RemoveActionBinding(ApplyHandle);
// 		RemoveActionBinding(CancelChangesHandle);
// 	}
// }
