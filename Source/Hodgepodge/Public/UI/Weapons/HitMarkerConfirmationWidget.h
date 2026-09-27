// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #pragma once

// #include "Components/Widget.h"
// #include "GameplayTagContainer.h"

// #include "HitMarkerConfirmationWidget.generated.h"

// class SHitMarkerConfirmationWidget;
// class SWidget;
// class UObject;
// struct FGameplayTag;

// UCLASS()
// class UHitMarkerConfirmationWidget : public UWidget
// {
// 	GENERATED_BODY()

// public:
// 	UHitMarkerConfirmationWidget(const FObjectInitializer& ObjectInitializer);

	//~UWidget interface
// protected:
// 	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~End of UWidget interface

	//~UVisual interface
// public:
// 	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~End of UVisual interface
	
// public:
// 	/** The duration (in seconds) to display hit notifies (they fade to transparent over this time)  */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, meta=(ClampMin=0.0, ForceUnits=s))
// 	float HitNotifyDuration = 0.4f;

// 	/** The marker image to draw for individual hit markers. */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
// 	FSlateBrush PerHitMarkerImage;

// 	/** Map from zone tag (e.g., weak spot) to override marker images for individual location hits. */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
// 	TMap<FGameplayTag, FSlateBrush> PerHitMarkerZoneOverrideImages;

// 	/** The marker image to draw if there are any hits at all. */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
// 	FSlateBrush AnyHitsMarkerImage;

// private:
// 	/** Internal slate widget representing the actual marker visuals */
// 	TSharedPtr<SHitMarkerConfirmationWidget> MyMarkerWidget;
// };
