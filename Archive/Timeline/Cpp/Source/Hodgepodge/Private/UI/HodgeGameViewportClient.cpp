// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #include "UI/HodgeGameViewportClient.h"

// #include "CommonUISettings.h"
// #include "ICommonUIModule.h"

// #include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameViewportClient)

// class UGameInstance;

// namespace GameViewportTags
// {
// 	UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_Input_HardwareCursor, "Platform.Trait.Input.HardwareCursor");
// }

// UHodgeGameViewportClient::UHodgeGameViewportClient()
// 	: Super(FObjectInitializer::Get())
// {
// }

// void UHodgeGameViewportClient::Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice)
// {
// 	Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);
	
	// We have software cursors set up in our project settings for console/mobile use, but on desktop we're fine with
	// the standard hardware cursors
// 	const bool UseHardwareCursor = ICommonUIModule::GetSettings().GetPlatformTraits().HasTag(GameViewportTags::TAG_Platform_Trait_Input_HardwareCursor);
// 	SetUseSoftwareCursorWidgets(!UseHardwareCursor);
// }
