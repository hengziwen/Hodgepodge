// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// // Copyright Epic Games, Inc. All Rights Reserved.
//
// #include "UI/Subsystem/HodgeUIManagerSubsystem.h"
//
// #include "CommonLocalPlayer.h"
// #include "Engine/GameInstance.h"
// #include "GameFramework/HUD.h"
// #include "GameUIPolicy.h"
// #include "PrimaryGameLayout.h"
//
// #include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeUIManagerSubsystem)
//
// class FSubsystemCollectionBase;
//
// UHodgeUIManagerSubsystem::UHodgeUIManagerSubsystem()
// {
// }
//
// void UHodgeUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
// {
// 	Super::Initialize(Collection);
//
// 	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UHodgeUIManagerSubsystem::Tick), 0.0f);
// }
//
// void UHodgeUIManagerSubsystem::Deinitialize()
// {
// 	Super::Deinitialize();
//
// 	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
// }
//
// bool UHodgeUIManagerSubsystem::Tick(float DeltaTime)
// {
// 	SyncRootLayoutVisibilityToShowHUD();
// 	
// 	return true;
// }
//
// void UHodgeUIManagerSubsystem::SyncRootLayoutVisibilityToShowHUD()
// {
// 	if (const UGameUIPolicy* Policy = GetCurrentUIPolicy())
// 	{
// 		for (const ULocalPlayer* LocalPlayer : GetGameInstance()->GetLocalPlayers())
// 		{
// 			bool bShouldShowUI = true;
// 			
// 			if (const APlayerController* PC = LocalPlayer->GetPlayerController(GetWorld()))
// 			{
// 				const AHUD* HUD = PC->GetHUD();
//
// 				if (HUD && !HUD->bShowHUD)
// 				{
// 					bShouldShowUI = false;
// 				}
// 			}
//
// 			if (UPrimaryGameLayout* RootLayout = Policy->GetRootLayout(CastChecked<UCommonLocalPlayer>(LocalPlayer)))
// 			{
// 				const ESlateVisibility DesiredVisibility = bShouldShowUI ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;
// 				if (DesiredVisibility != RootLayout->GetVisibility())
// 				{
// 					RootLayout->SetVisibility(DesiredVisibility);	
// 				}
// 			}
// 		}
// 	}
// }
