// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// // Copyright Epic Games, Inc. All Rights Reserved.
//
// #include "UI/Weapons/HodgeWeaponUserInterface.h"
//
// #include "Equipment/HodgeEquipmentManagerComponent.h"
// #include "GameFramework/Pawn.h"
// #include "Weapons/HodgeWeaponInstance.h"
//
// #include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeWeaponUserInterface)
//
// struct FGeometry;
//
// UHodgeWeaponUserInterface::UHodgeWeaponUserInterface(const FObjectInitializer& ObjectInitializer)
// 	: Super(ObjectInitializer)
// {
// }
//
// void UHodgeWeaponUserInterface::NativeConstruct()
// {
// 	Super::NativeConstruct();
// }
//
// void UHodgeWeaponUserInterface::NativeDestruct()
// {
// 	Super::NativeDestruct();
// }
//
// void UHodgeWeaponUserInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
// {
// 	Super::NativeTick(MyGeometry, InDeltaTime);
//
// 	if (APawn* Pawn = GetOwningPlayerPawn())
// 	{
// 		if (UHodgeEquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<UHodgeEquipmentManagerComponent>())
// 		{
// 			if (UHodgeWeaponInstance* NewInstance = EquipmentManager->GetFirstInstanceOfType<UHodgeWeaponInstance>())
// 			{
// 				if (NewInstance != CurrentInstance && NewInstance->GetInstigator() != nullptr)
// 				{
// 					UHodgeWeaponInstance* OldWeapon = CurrentInstance;
// 					CurrentInstance = NewInstance;
// 					RebuildWidgetFromWeapon();
// 					OnWeaponChanged(OldWeapon, CurrentInstance);
// 				}
// 			}
// 		}
// 	}
// }
//
// void UHodgeWeaponUserInterface::RebuildWidgetFromWeapon()
// {
// 	
// }
//
