// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #pragma once

// #include "CommonUserWidget.h"

// #include "HodgeReticleWidgetBase.generated.h"

// class UHodgeInventoryItemInstance;
// class UHodgeWeaponInstance;
// class UObject;
// struct FFrame;

// UCLASS(Abstract)
// class UHodgeReticleWidgetBase : public UCommonUserWidget
// {
// 	GENERATED_BODY()

// public:
// 	UHodgeReticleWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

// 	UFUNCTION(BlueprintImplementableEvent)
// 	void OnWeaponInitialized();

// 	UFUNCTION(BlueprintCallable)
// 	void InitializeFromWeapon(UHodgeWeaponInstance* InWeapon);

// 	/** Returns the current weapon's diametrical spread angle, in degrees */
// 	UFUNCTION(BlueprintCallable, BlueprintPure)
// 	float ComputeSpreadAngle() const;

// 	/** Returns the current weapon's maximum spread radius in screenspace units (pixels) */
// 	UFUNCTION(BlueprintCallable, BlueprintPure)
// 	float ComputeMaxScreenspaceSpreadRadius() const;

// 	/**
// 	 * Returns true if the current weapon is at 'first shot accuracy'
// 	 * (the weapon allows it and it is at min spread)
// 	 */
// 	UFUNCTION(BlueprintCallable, BlueprintPure)
// 	bool HasFirstShotAccuracy() const;

// protected:
// 	UPROPERTY(BlueprintReadOnly)
// 	TObjectPtr<UHodgeWeaponInstance> WeaponInstance;

// 	UPROPERTY(BlueprintReadOnly)
// 	TObjectPtr<UHodgeInventoryItemInstance> InventoryInstance;
// };
