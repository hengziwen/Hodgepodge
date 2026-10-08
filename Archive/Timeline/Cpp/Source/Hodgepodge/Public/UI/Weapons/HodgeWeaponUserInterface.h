// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #pragma once

// #include "CommonUserWidget.h"

// #include "HodgeWeaponUserInterface.generated.h"

// class UHodgeWeaponInstance;
// class UObject;
// struct FGeometry;

// UCLASS()
// class UHodgeWeaponUserInterface : public UCommonUserWidget
// {
// 	GENERATED_BODY()

// public:
// 	UHodgeWeaponUserInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

// 	virtual void NativeConstruct() override;
// 	virtual void NativeDestruct() override;
// 	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

// 	UFUNCTION(BlueprintImplementableEvent)
// 	void OnWeaponChanged(UHodgeWeaponInstance* OldWeapon, UHodgeWeaponInstance* NewWeapon);

// private:
// 	void RebuildWidgetFromWeapon();

// private:
// 	UPROPERTY(Transient)
// 	TObjectPtr<UHodgeWeaponInstance> CurrentInstance;
// };
