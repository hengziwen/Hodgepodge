// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #pragma once

// #include "Engine/DataTable.h"
// #include "Widgets/GameSettingScreen.h"

// #include "HodgeSettingScreen.generated.h"

// class UGameSettingRegistry;
// class UHodgeTabListWidgetBase;
// class UObject;

// UCLASS(Abstract, meta = (Category = "Settings", DisableNativeTick))
// class UHodgeSettingScreen : public UGameSettingScreen
// {
// 	GENERATED_BODY()

// public:

// protected:
// 	virtual void NativeOnInitialized() override;
// 	virtual UGameSettingRegistry* CreateRegistry() override;

// 	void HandleBackAction();
// 	void HandleApplyAction();
// 	void HandleCancelChangesAction();

// 	virtual void OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty) override;
	
// protected:
// 	UPROPERTY(BlueprintReadOnly, Category = Input, meta = (BindWidget, OptionalWidget = true, AllowPrivateAccess = true))
// 	TObjectPtr<UHodgeTabListWidgetBase> TopSettingsTabs;
	
// 	UPROPERTY(EditDefaultsOnly)
// 	FDataTableRowHandle BackInputActionData;

// 	UPROPERTY(EditDefaultsOnly)
// 	FDataTableRowHandle ApplyInputActionData;

// 	UPROPERTY(EditDefaultsOnly)
// 	FDataTableRowHandle CancelChangesInputActionData;

// 	FUIActionBindingHandle BackHandle;
// 	FUIActionBindingHandle ApplyHandle;
// 	FUIActionBindingHandle CancelChangesHandle;
// };
