// =============================================================================
// [UI-MIGRATION-PENDING] 本文件整体注释，暂不参与编译。
// 原因：依赖尚未引入的 Lyra 插件或上游类型，详见
// Docs/Design/lyra-ui-migration-plan.md 的「待复活文件清单」一节。
// 复活时必须 .h 与 .cpp 成对恢复，不要只恢复其中一半。
// =============================================================================

// Copyright Epic Games, Inc. All Rights Reserved.

// #pragma once

// #include "Components/GameStateComponent.h"
// #include "ControlFlowNode.h"
// #include "LoadingProcessInterface.h"

// #include "HodgeFrontendStateComponent.generated.h"

// class FControlFlow;
// class FString;
// class FText;
// class UObject;
// struct FFrame;

// enum class ECommonUserOnlineContext : uint8;
// enum class ECommonUserPrivilege : uint8;
// class UCommonActivatableWidget;
// class UCommonUserInfo;
// class UHodgeExperienceDefinition;

// UCLASS(Abstract)
// class UHodgeFrontendStateComponent : public UGameStateComponent, public ILoadingProcessInterface
// {
// 	GENERATED_BODY()

// public:

// 	UHodgeFrontendStateComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
// 	virtual void BeginPlay() override;
// 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	//~ILoadingProcessInterface interface
// 	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~End of ILoadingProcessInterface

// private:
// 	void OnExperienceLoaded(const UHodgeExperienceDefinition* Experience);

// 	UFUNCTION()
// 	void OnUserInitialized(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext);

// 	void FlowStep_WaitForUserInitialization(FControlFlowNodeRef SubFlow);
// 	void FlowStep_TryShowPressStartScreen(FControlFlowNodeRef SubFlow);
// 	void FlowStep_TryJoinRequestedSession(FControlFlowNodeRef SubFlow);
// 	void FlowStep_TryShowMainScreen(FControlFlowNodeRef SubFlow);

// 	bool bShouldShowLoadingScreen = true;

// 	UPROPERTY(EditAnywhere, Category = UI)
// 	TSoftClassPtr<UCommonActivatableWidget> PressStartScreenClass;

// 	UPROPERTY(EditAnywhere, Category = UI)
// 	TSoftClassPtr<UCommonActivatableWidget> MainScreenClass;

// 	TSharedPtr<FControlFlow> FrontEndFlow;
	
	// If set, this is the in-progress press start screen task
// 	FControlFlowNodePtr InProgressPressStartScreen;

// 	FDelegateHandle OnJoinSessionCompleteEventHandle;
// };
