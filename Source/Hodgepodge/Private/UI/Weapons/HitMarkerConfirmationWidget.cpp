// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #include "UI/Weapons/HitMarkerConfirmationWidget.h"

// #include "Blueprint/UserWidget.h"
// #include "UI/Weapons/SHitMarkerConfirmationWidget.h"

// #include UE_INLINE_GENERATED_CPP_BY_NAME(HitMarkerConfirmationWidget)

// class SWidget;

// UHitMarkerConfirmationWidget::UHitMarkerConfirmationWidget(const FObjectInitializer& ObjectInitializer)
// 	: Super(ObjectInitializer)
// {
// 	SetVisibility(ESlateVisibility::HitTestInvisible);
// 	bIsVolatile = true;
// 	AnyHitsMarkerImage.DrawAs = ESlateBrushDrawType::NoDrawType;
// }

// void UHitMarkerConfirmationWidget::ReleaseSlateResources(bool bReleaseChildren)
// {
// 	Super::ReleaseSlateResources(bReleaseChildren);

// 	MyMarkerWidget.Reset();
// }

// TSharedRef<SWidget> UHitMarkerConfirmationWidget::RebuildWidget()
// {
// 	UUserWidget* OuterUserWidget = GetTypedOuter<UUserWidget>();
// 	FLocalPlayerContext DummyContext;
// 	const FLocalPlayerContext& PlayerContextRef = (OuterUserWidget != nullptr)
// 		                                              ? OuterUserWidget->GetPlayerContext()
// 		                                              : DummyContext;

// 	MyMarkerWidget = SNew(SHitMarkerConfirmationWidget, PlayerContextRef, PerHitMarkerZoneOverrideImages)
// 		.PerHitMarkerImage(&(this->PerHitMarkerImage))
// 		.AnyHitsMarkerImage(&(this->AnyHitsMarkerImage))
// 		.HitNotifyDuration(this->HitNotifyDuration);

// 	return MyMarkerWidget.ToSharedRef();
// }
