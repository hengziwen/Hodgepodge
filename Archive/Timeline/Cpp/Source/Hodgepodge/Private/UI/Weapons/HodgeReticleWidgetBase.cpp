// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// // Copyright Epic Games, Inc. All Rights Reserved.
//
// #include "UI/Weapons/HodgeReticleWidgetBase.h"
//
// #include "Inventory/HodgeInventoryItemInstance.h"
// #include "Weapons/HodgeRangedWeaponInstance.h"
// #include "Weapons/HodgeWeaponInstance.h"
//
// #include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeReticleWidgetBase)
//
// UHodgeReticleWidgetBase::UHodgeReticleWidgetBase(const FObjectInitializer& ObjectInitializer)
// 	: Super(ObjectInitializer)
// {
// }
//
// void UHodgeReticleWidgetBase::InitializeFromWeapon(UHodgeWeaponInstance* InWeapon)
// {
// 	WeaponInstance = InWeapon;
// 	InventoryInstance = nullptr;
// 	if (WeaponInstance)
// 	{
// 		InventoryInstance = Cast<UHodgeInventoryItemInstance>(WeaponInstance->GetInstigator());
// 	}
// 	OnWeaponInitialized();
// }
//
//
// float UHodgeReticleWidgetBase::ComputeSpreadAngle() const
// {
// 	if (const UHodgeRangedWeaponInstance* RangedWeapon = Cast<const UHodgeRangedWeaponInstance>(WeaponInstance))
// 	{
// 		const float BaseSpreadAngle = RangedWeapon->GetCalculatedSpreadAngle();
// 		const float SpreadAngleMultiplier = RangedWeapon->GetCalculatedSpreadAngleMultiplier();
// 		const float ActualSpreadAngle = BaseSpreadAngle * SpreadAngleMultiplier;
//
// 		return ActualSpreadAngle;
// 	}
// 	else
// 	{
// 		return 0.0f;
// 	}
// }
//
// bool UHodgeReticleWidgetBase::HasFirstShotAccuracy() const
// {
// 	if (const UHodgeRangedWeaponInstance* RangedWeapon = Cast<const UHodgeRangedWeaponInstance>(WeaponInstance))
// 	{
// 		return RangedWeapon->HasFirstShotAccuracy();
// 	}
// 	else
// 	{
// 		return false;
// 	}
// }
//
// float UHodgeReticleWidgetBase::ComputeMaxScreenspaceSpreadRadius() const
// {
// 	const float LongShotDistance = 10000.f;
//
// 	APlayerController* PC = GetOwningPlayer();
// 	if (PC && PC->PlayerCameraManager)
// 	{
// 		// A weapon's spread can be thought of as a cone shape. To find the screenspace spread for reticle visualization,
// 		// we create a line on the edge of the cone at a long distance. The end of that point is on the edge of the cone's circle.
// 		// We then project it back onto the screen. Its distance from screen center is the spread radius.
//
// 		// This isn't perfect, due to there being some distance between the camera location and the gun muzzle.
//
// 		const float SpreadRadiusRads = FMath::DegreesToRadians(ComputeSpreadAngle() * 0.5f);
// 		const float SpreadRadiusAtDistance = FMath::Tan(SpreadRadiusRads) * LongShotDistance;
//
// 		FVector CamPos;
// 		FRotator CamOrient;
// 		PC->PlayerCameraManager->GetCameraViewPoint(CamPos, CamOrient);
//
// 		FVector CamForwDir = CamOrient.RotateVector(FVector::ForwardVector);
// 		FVector CamUpDir   = CamOrient.RotateVector(FVector::UpVector);
//
// 		FVector OffsetTargetAtDistance = CamPos + (CamForwDir * LongShotDistance) + (CamUpDir * SpreadRadiusAtDistance);
//
// 		FVector2D OffsetTargetInScreenspace;
//
// 		if (PC->ProjectWorldLocationToScreen(OffsetTargetAtDistance, OffsetTargetInScreenspace, true))
// 		{
// 			int32 ViewportSizeX(0), ViewportSizeY(0);
// 			PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
//
// 			const FVector2D ScreenSpaceCenter(FVector::FReal(ViewportSizeX) * 0.5f, FVector::FReal(ViewportSizeY) * 0.5f);
//
// 			return (OffsetTargetInScreenspace - ScreenSpaceCenter).Length();
// 		}
// 	}
// 	
// 	return 0.0f;
// }
