# Core 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeGameInstanceBase.cpp

注册 Init State 顺序、主控制器访问和全局生命周期扩展。

源码：[Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp](../../../Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/GameInstance/HodgeGameInstanceBase.h](../../../Source/Hodgepodge/Public/Core/GameInstance/HodgeGameInstanceBase.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)

定义候选（多行签名仅展示首行）：

- L17: `UHodgeGameInstanceBase::UHodgeGameInstanceBase(const FObjectInitializer& ObjectInitializer)`
- L22: `AHodgePlayerController* UHodgeGameInstanceBase::GetPrimaryPlayerController() const`
- L28: `void UHodgeGameInstanceBase::Shutdown()`
- L34: `void UHodgeGameInstanceBase::Init()`

## HodgeGameModeBase.cpp

服务器选择玩法、等待加载、生成 Pawn，并在 FinishSpawning 前注入 PawnData；选用 HodgePlayerController。

源码：[Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp](../../../Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/GameMode/HodgeGameModeBase.h](../../../Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h)、[Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)、[Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Core/GameState/HodgeGameState.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h)、[Core/GameState/HodgeGameStateBase.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h)、[Core/HUD/HodgeHUDBase.h](../../../Source/Hodgepodge/Public/Core/HUD/HodgeHUDBase.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)、[Data/HodgeExperienceDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)

定义候选（多行签名仅展示首行）：

- L21: `AHodgeGameModeBase::AHodgeGameModeBase(const FObjectInitializer& ObjectInitializer)`
- L46: `const UHodgePawnData* AHodgeGameModeBase::GetPawnDataForController(const AController* InController) const`
- L87: `void AHodgeGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)`
- L96: `void AHodgeGameModeBase::HandleMatchAssignmentIfNotExpectingOne()`
- L193: `bool AHodgeGameModeBase::TryDedicatedServerLogin()`
- L337: `void AHodgeGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)`
- L361: `void AHodgeGameModeBase::OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience)`
- L382: `bool AHodgeGameModeBase::IsExperienceLoaded() const`
- L396: `UClass* AHodgeGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)`
- L412: `APawn* AHodgeGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,`
- L469: `bool AHodgeGameModeBase::ShouldSpawnAtStartSpot(AController* Player)`
- L475: `void AHodgeGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)`
- L485: `AActor* AHodgeGameModeBase::ChoosePlayerStart_Implementation(AController* Player)`
- L498: `void AHodgeGameModeBase::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)`
- L511: `bool AHodgeGameModeBase::PlayerCanRestart_Implementation(APlayerController* Player)`
- L517: `bool AHodgeGameModeBase::ControllerCanRestart(AController* Controller)`
- L546: `void AHodgeGameModeBase::InitGameState()`
- L561: `void AHodgeGameModeBase::GenericPlayerInitialization(AController* NewPlayer)`
- L570: `void AHodgeGameModeBase::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)`
- L591: `bool AHodgeGameModeBase::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)`
- L597: `void AHodgeGameModeBase::FailedToRestartPlayer(AController* NewPlayer)`

## HodgeGameState.cpp

创建 ExperienceManager 和世界状态 ASC，处理游戏状态复制/扩展。

源码：[Source/Hodgepodge/Private/Core/GameState/HodgeGameState.cpp](../../../Source/Hodgepodge/Private/Core/GameState/HodgeGameState.cpp)

项目内直接 include（不是运行调用关系）：[Core/GameState/HodgeGameState.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)、[Core/PlayState/HodgePlayerStateBase.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h)

定义候选（多行签名仅展示首行）：

- L25: `AHodgeGameState::AHodgeGameState(const FObjectInitializer& ObjectInitializer)`
- L50: `void AHodgeGameState::PreInitializeComponents()`
- L56: `void AHodgeGameState::PostInitializeComponents()`
- L67: `UAbilitySystemComponent* AHodgeGameState::GetAbilitySystemComponent() const`
- L73: `void AHodgeGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L79: `void AHodgeGameState::AddPlayerState(APlayerState* PlayerState)`
- L85: `void AHodgeGameState::RemovePlayerState(APlayerState* PlayerState)`
- L92: `void AHodgeGameState::SeamlessTravelTransitionCheckpoint(bool bToTransitionMap)`
- L108: `void AHodgeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L120: `void AHodgeGameState::Tick(float DeltaSeconds)`
- L147: `float AHodgeGameState::GetServerFPS() const`
- L153: `void AHodgeGameState::SetRecorderPlayerState(APlayerState* NewPlayerState)`
- L172: `APlayerState* AHodgeGameState::GetRecorderPlayerState() const`
- L178: `void AHodgeGameState::OnRep_RecorderPlayerState()`

## HodgeGameStateBase.cpp

GameState 基础扩展生命周期。

源码：[Source/Hodgepodge/Private/Core/GameState/HodgeGameStateBase.cpp](../../../Source/Hodgepodge/Private/Core/GameState/HodgeGameStateBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/GameState/HodgeGameStateBase.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h)

定义候选（多行签名仅展示首行）：

- L14: `AHodgeGameStateBase::AHodgeGameStateBase(const FObjectInitializer& ObjectInitializer)`
- L19: `void AHodgeGameStateBase::PreInitializeComponents()`
- L27: `void AHodgeGameStateBase::BeginPlay()`
- L36: `void AHodgeGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)`

## HodgeHUDBase.cpp

项目 HUD 基类扩展入口，不代表 CommonUI 已完成。

源码：[Source/Hodgepodge/Private/Core/HUD/HodgeHUDBase.cpp](../../../Source/Hodgepodge/Private/Core/HUD/HodgeHUDBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/HUD/HodgeHUDBase.h](../../../Source/Hodgepodge/Public/Core/HUD/HodgeHUDBase.h)

定义候选（多行签名仅展示首行）：


## HodgeLocalPlayerBase.cpp

本地玩家对象及控制器、PlayerState、Pawn 就绪事件桥。

源码：[Source/Hodgepodge/Private/Core/LocalPlayer/HodgeLocalPlayerBase.cpp](../../../Source/Hodgepodge/Private/Core/LocalPlayer/HodgeLocalPlayerBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)

定义候选（多行签名仅展示首行）：

- L20: `UHodgeLocalPlayerBase::UHodgeLocalPlayerBase()`
- L37: `FDelegateHandle UHodgeLocalPlayerBase::CallAndRegister_OnPlayerControllerSet(`
- L63: `FDelegateHandle UHodgeLocalPlayerBase::CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate)`
- L88: `FDelegateHandle UHodgeLocalPlayerBase::CallAndRegister_OnPlayerPawnSet(FPlayerPawnSetDelegate::FDelegate Delegate)`
- L120: `bool UHodgeLocalPlayerBase::GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData,`

## HodgePlayerController.cpp

具体控制器：每帧消费 ASC 输入、相机管理、AutoRun、UnPossess Avatar 清理及 Replay 扩展。

源码：[Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerController.cpp](../../../Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerController.cpp)

项目内直接 include（不是运行调用关系）：[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Camera/HodgePlayerCameraManager.h](../../../Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h)、[Core/HUD/HodgeHUDBase.h](../../../Source/Hodgepodge/Public/Core/HUD/HodgeHUDBase.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)、[Core/GameState/HodgeGameState.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h)

定义候选（多行签名仅展示首行）：

- L96: `AHodgePlayerController::AHodgePlayerController(const FObjectInitializer& ObjectInitializer)`
- L109: `void AHodgePlayerController::PreInitializeComponents()`
- L116: `void AHodgePlayerController::BeginPlay()`
- L141: `void AHodgePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L148: `void AHodgePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L171: `void AHodgePlayerController::ReceivedPlayer()`
- L178: `void AHodgePlayerController::PlayerTick(float DeltaTime)`
- L243: `AHodgePlayerState* AHodgePlayerController::GetHodgePlayerState() const`
- L250: `UHodgeAbilitySystemComponent* AHodgePlayerController::GetHodgeAbilitySystemComponent() const`
- L260: `AHodgeHUDBase* AHodgePlayerController::GetHodgeHUD() const`
- L267: `bool AHodgePlayerController::TryToRecordClientReplay()`
- L296: `bool AHodgePlayerController::ShouldRecordClientReplay()`
- L363: `void AHodgePlayerController::OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)`
- L370: `void AHodgePlayerController::OnPlayerStateChanged()`
- L377: `void AHodgePlayerController::BroadcastOnPlayerStateChanged()`
- L422: `void AHodgePlayerController::InitPlayerState()`
- L432: `void AHodgePlayerController::CleanupPlayerState()`
- L442: `void AHodgePlayerController::OnRep_PlayerState()`
- L452: `void AHodgePlayerController::SetPlayer(UPlayer* InPlayer)`
- L475: `void AHodgePlayerController::AddCheats(bool bForce)`
- L486: `void AHodgePlayerController::ServerCheat_Implementation(const FString& Msg)`
- L499: `bool AHodgePlayerController::ServerCheat_Validate(const FString& Msg)`
- L506: `void AHodgePlayerController::ServerCheatAll_Implementation(const FString& Msg)`
- L526: `bool AHodgePlayerController::ServerCheatAll_Validate(const FString& Msg)`
- L533: `void AHodgePlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)`
- L540: `void AHodgePlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)`
- L554: `void AHodgePlayerController::OnCameraPenetratingTarget()`
- L561: `void AHodgePlayerController::OnPossess(APawn* InPawn)`
- L586: `void AHodgePlayerController::SetIsAutoRunning(const bool bEnabled)`
- L609: `bool AHodgePlayerController::GetIsAutoRunning() const`
- L626: `void AHodgePlayerController::OnStartAutoRun()`
- L640: `void AHodgePlayerController::OnEndAutoRun()`
- L654: `void AHodgePlayerController::UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId)`
- L677: `void AHodgePlayerController::UpdateHiddenComponents(const FVector& ViewLocation,`
- L787: `void AHodgePlayerController::OnUnPossess()`
- L816: `void AHodgeReplayPlayerController::Tick(float DeltaSeconds)`
- L854: `void AHodgeReplayPlayerController::SmoothTargetViewRotation(APawn* TargetPawn, float DeltaSeconds)`
- L864: `bool AHodgeReplayPlayerController::ShouldRecordClientReplay()`
- L871: `void AHodgeReplayPlayerController::RecorderPlayerStateUpdated(APlayerState* NewRecorderPlayerState)`
- L891: `void AHodgeReplayPlayerController::OnPlayerStatePawnSet(APlayerState* ChangedPlayerState, APawn* NewPlayerPawn,`

## HodgePlayerControllerBase.cpp

控制器和 Pawn 生命周期桥接到 LocalPlayer 委托；具体输入消费在派生 HodgePlayerController。

源码：[Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerControllerBase.cpp](../../../Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerControllerBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/PlayerController/HodgePlayerControllerBase.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h)、[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)

定义候选（多行签名仅展示首行）：

- L22: `AHodgePlayerControllerBase::AHodgePlayerControllerBase(const FObjectInitializer& ObjectInitializer)`
- L50: `void AHodgePlayerControllerBase::ReceivedPlayer()`
- L81: `void AHodgePlayerControllerBase::SetPawn(APawn* InPawn)`
- L116: `void AHodgePlayerControllerBase::OnPossess(class APawn* APawn)`
- L147: `void AHodgePlayerControllerBase::OnUnPossess()`
- L185: `void AHodgePlayerControllerBase::OnRep_PlayerState()`

## HodgePlayerState.cpp

玩家 ASC、HealthSet、PawnData、阵营/标签栈等持有者；SetPawnData 在权威端授予 AbilitySets（未记录句柄）。

源码：[Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp](../../../Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp)

项目内直接 include（不是运行调用关系）：[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)、[Core/GameMode/HodgeGameModeBase.h](../../../Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)

定义候选（多行签名仅展示首行）：

- L34: `const FName AHodgePlayerState::NAME_HodgeAbilityReady("HodgeAbilitiesReady");`
- L37: `AHodgePlayerState::AHodgePlayerState(const FObjectInitializer& ObjectInitializer)`
- L65: `AHodgePlayerController* AHodgePlayerState::GetHodgePlayerController() const`
- L72: `UAbilitySystemComponent* AHodgePlayerState::GetAbilitySystemComponent() const`
- L79: `void AHodgePlayerState::SetPawnData(const UHodgePawnData* InPawnData)`
- L124: `void AHodgePlayerState::PreInitializeComponents()`
- L162: `void AHodgePlayerState::PostInitializeComponents()`
- L169: `void AHodgePlayerState::Reset()`
- L176: `void AHodgePlayerState::ClientInitialize(AController* C)`
- L189: `void AHodgePlayerState::CopyProperties(APlayerState* PlayerState)`
- L199: `void AHodgePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L233: `void AHodgePlayerState::OnDeactivated()`
- L269: `void AHodgePlayerState::OnReactivated()`
- L283: `void AHodgePlayerState::SetPlayerConnectionType(EHodgePlayerConnectionType NewType)`
- L293: `void AHodgePlayerState::SetSquadID(int32 NewSquadID)`
- L307: `void AHodgePlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)`
- L314: `void AHodgePlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)`
- L321: `int32 AHodgePlayerState::GetStatTagStackCount(FGameplayTag Tag) const`
- L328: `bool AHodgePlayerState::HasStatTag(FGameplayTag Tag) const`
- L335: `FRotator AHodgePlayerState::GetReplicatedViewRotation() const`
- L342: `void AHodgePlayerState::SetReplicatedViewRotation(const FRotator& NewRotation)`
- L356: `void AHodgePlayerState::OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience)`
- L379: `void AHodgePlayerState::OnRep_PawnData()`
- L385: `void AHodgePlayerState::OnRep_MyTeamID(FGenericTeamId OldTeamID)`
- L392: `void AHodgePlayerState::OnRep_MySquadID()`

## HodgePlayerStateBase.cpp

PlayerState ModularGameplay Receiver 注册、注销及组件 Reset/CopyProperties。

源码：[Source/Hodgepodge/Private/Core/PlayState/HodgePlayerStateBase.cpp](../../../Source/Hodgepodge/Private/Core/PlayState/HodgePlayerStateBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/PlayState/HodgePlayerStateBase.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h)

定义候选（多行签名仅展示首行）：

- L10: `void AHodgePlayerStateBase::PreInitializeComponents()`
- L19: `void AHodgePlayerStateBase::BeginPlay()`
- L29: `void AHodgePlayerStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L38: `void AHodgePlayerStateBase::Reset()`
- L55: `void AHodgePlayerStateBase::CopyProperties(APlayerState* PlayerState)`

## HodgeGameInstanceBase.h

注册 Init State 顺序、主控制器访问和全局生命周期扩展。

源码：[Source/Hodgepodge/Public/Core/GameInstance/HodgeGameInstanceBase.h](../../../Source/Hodgepodge/Public/Core/GameInstance/HodgeGameInstanceBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   8: #pragma once
  10: #include "CoreMinimal.h"
  11: #include "Engine/GameInstance.h"
  12: #include "HodgeGameInstanceBase.generated.h"
  14: class AHodgePlayerController;
  22: UCLASS()
  23: class HODGEPODGE_API UHodgeGameInstanceBase : public UGameInstance
  24: {
  25: 	GENERATED_BODY()
  27: public:
  29: 	UHodgeGameInstanceBase(const FObjectInitializer& ObjectInitializer);
  32: 	AHodgePlayerController* GetPrimaryPlayerController() const;
  34: protected:
  36: 	virtual void Init() override;
  39: 	virtual void Shutdown() override;
  42: 	TArray<uint8> DebugTestEncryptionKey;
  43: };
```

## HodgeGameModeBase.h

服务器选择玩法、等待加载、生成 Pawn，并在 FinishSpawning 前注入 PawnData；选用 HodgePlayerController。

源码：[Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h](../../../Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   8: #pragma once
  10: #include "CoreMinimal.h"
  11: #include "GameFramework/GameModeBase.h"
  12: #include "HodgeGameModeBase.generated.h"
  14: class UHodgeExperienceDefinition;
  15: class UHodgePawnData;
  21: DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHodgeGameModePlayerInitialized, AGameModeBase*             ,
  22:                                      AController*              );
  24: UCLASS()
  25: class HODGEPODGE_API AHodgeGameModeBase : public AGameModeBase
  26: {
  27: 	GENERATED_BODY()
  29: public:
  31: 	AHodgeGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  34: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Pawn")
  35: 	const UHodgePawnData* GetPawnDataForController(const AController* InController) const;
  40: 	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
  43: 	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
  46: 	virtual APawn*
  47: 	SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
  50: 	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
  53: 	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
  56: 	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
  59: 	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
  62: 	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
  65: 	virtual void InitGameState() override;
  68: 	virtual bool UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage) override;
  71: 	virtual void GenericPlayerInitialization(AController* NewPlayer) override;
  74: 	virtual void FailedToRestartPlayer(AController* NewPlayer) override;
  80: 	UFUNCTION(BlueprintCallable)
  81: 	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset = false);
  84: 	virtual bool ControllerCanRestart(AController* Controller);
  87: 	FOnHodgeGameModePlayerInitialized OnGameModePlayerInitialized;
  89: protected:
  91: 	void OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience);
  94: 	bool IsExperienceLoaded() const;
  97: 	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource);
 100: 	void HandleMatchAssignmentIfNotExpectingOne();
 103: 	bool TryDedicatedServerLogin();
 111: };
```

## HodgeGameState.h

创建 ExperienceManager 和世界状态 ASC，处理游戏状态复制/扩展。

源码：[Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "AbilitySystemInterface.h"
   7: #include "HodgeGameStateBase.h"
   8: #include "HodgeGameState.generated.h"
  10: class UHodgeExperienceManagerComponent;
  11: class UHodgeAbilitySystemComponent;
  18: UCLASS()
  19: class HODGEPODGE_API AHodgeGameState : public AHodgeGameStateBase, public IAbilitySystemInterface
  20: {
  21: 	GENERATED_BODY()
  23: public:
  25: 	AHodgeGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  29: 	virtual void PreInitializeComponents() override;
  32: 	virtual void PostInitializeComponents() override;
  35: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  38: 	virtual void Tick(float DeltaSeconds) override;
  43: 	virtual void AddPlayerState(APlayerState* PlayerState) override;
  46: 	virtual void RemovePlayerState(APlayerState* PlayerState) override;
  49: 	virtual void SeamlessTravelTransitionCheckpoint(bool bToTransitionMap) override;
  54: 	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
  58: 	UFUNCTION(BlueprintCallable, Category = "Hodge|GameState")
  59: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const { return AbilitySystemComponent; }
  70: 	float GetServerFPS() const;
  73: 	void SetRecorderPlayerState(APlayerState* NewPlayerState);
  76: 	APlayerState* GetRecorderPlayerState() const;
  79: 	DECLARE_MULTICAST_DELEGATE_OneParam(FOnRecorderPlayerStateChanged, APlayerState*);
  80: 	FOnRecorderPlayerStateChanged OnRecorderPlayerStateChangedEvent;
  82: private:
  84: 	UPROPERTY()
  85: 	TObjectPtr<UHodgeExperienceManagerComponent> ExperienceManagerComponent;
  88: 	UPROPERTY(VisibleAnywhere, Category = "Hodge|GameState")
  89: 	TObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;
  91: protected:
  93: 	UPROPERTY(Replicated)
  94: 	float ServerFPS;
  98: 	UPROPERTY(Transient, ReplicatedUsing = OnRep_RecorderPlayerState)
  99: 	TObjectPtr<APlayerState> RecorderPlayerState;
 102: 	UFUNCTION()
 103: 	void OnRep_RecorderPlayerState();
 104: };
```

## HodgeGameStateBase.h

GameState 基础扩展生命周期。

源码：[Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   8: #pragma once
  10: #include "CoreMinimal.h"
  11: #include "GameFramework/GameStateBase.h"
  12: #include "HodgeGameStateBase.generated.h"
  22: UCLASS()
  23: class HODGEPODGE_API AHodgeGameStateBase : public AGameStateBase
  24: {
  25: 	GENERATED_BODY()
  27: public:
  29: 	AHodgeGameStateBase(const FObjectInitializer& ObjectInitializer);
  33: 	virtual void PreInitializeComponents() override;
  36: 	virtual void BeginPlay() override;
  39: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  41: };
```

## HodgeHUDBase.h

项目 HUD 基类扩展入口，不代表 CommonUI 已完成。

源码：[Source/Hodgepodge/Public/Core/HUD/HodgeHUDBase.h](../../../Source/Hodgepodge/Public/Core/HUD/HodgeHUDBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   8: #pragma once
  10: #include "CoreMinimal.h"
  11: #include "GameFramework/HUD.h"
  12: #include "HodgeHUDBase.generated.h"
  30: UCLASS()
  31: class HODGEPODGE_API AHodgeHUDBase : public AHUD
  32: {
  33: 	GENERATED_BODY()
  34: };
```

## HodgeLocalPlayerBase.h

本地玩家对象及控制器、PlayerState、Pawn 就绪事件桥。

源码：[Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   9: #pragma once
  11: #include "CoreMinimal.h"
  12: #include "Engine/LocalPlayer.h"
  13: #include "HodgeLocalPlayerBase.generated.h"
  30: UCLASS()
  31: class HODGEPODGE_API UHodgeLocalPlayerBase : public ULocalPlayer
  32: {
  33: 	GENERATED_BODY()
  35: public:
  42: 	UHodgeLocalPlayerBase();
  52: 	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerControllerSetDelegate, UHodgeLocalPlayerBase* LocalPlayer,
  53: 	                                     APlayerController* PlayerController);
  54: 	FPlayerControllerSetDelegate OnPlayerControllerSet;
  63: 	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerStateSetDelegate, UHodgeLocalPlayerBase* LocalPlayer,
  64: 	                                     APlayerState* PlayerState);
  65: 	FPlayerStateSetDelegate OnPlayerStateSet;
  74: 	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerPawnSetDelegate, UHodgeLocalPlayerBase* LocalPlayer, APawn* Pawn);
  75: 	FPlayerPawnSetDelegate OnPlayerPawnSet;
  87: 	FDelegateHandle CallAndRegister_OnPlayerControllerSet(FPlayerControllerSetDelegate::FDelegate Delegate);
  98: 	FDelegateHandle CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate);
 109: 	FDelegateHandle CallAndRegister_OnPlayerPawnSet(FPlayerPawnSetDelegate::FDelegate Delegate);
 111: public:
 124: 	virtual bool GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData,
 125: 	                               int32 StereoViewIndex) const override;
 131: 	bool IsPlayerViewEnabled() const { return bIsPlayerViewEnabled; }
 141: 	void SetIsPlayerViewEnabled(bool bInIsPlayerViewEnabled) { bIsPlayerViewEnabled = bInIsPlayerViewEnabled; }
 146: private:
 153: 	bool bIsPlayerViewEnabled = true;
 154: };
```

## HodgePlayerController.h

具体控制器：每帧消费 ASC 输入、相机管理、AutoRun、UnPossess Avatar 清理及 Replay 扩展。

源码：[Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCameraAssistInterface.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraAssistInterface.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   6: #include "CoreMinimal.h"
   9: #include "HodgePlayerControllerBase.h"
  12: #include "Camera/HodgeCameraAssistInterface.h"
  14: #include "HodgePlayerController.generated.h"
  17: class UHodgeAbilitySystemComponent;
  20: class AHodgePlayerState;
  23: class AHodgeHUDBase;
  31: UCLASS()
  32: class HODGEPODGE_API AHodgePlayerController : public AHodgePlayerControllerBase, public IHodgeCameraAssistInterface
  33: {
  34: 	GENERATED_BODY()
  36: public:
  38: 	AHodgePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  41: 	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
  42: 	AHodgePlayerState* GetHodgePlayerState() const;
  45: 	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
  46: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const;
  49: 	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
  50: 	AHodgeHUDBase* GetHodgeHUD() const;
  54: 	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
  55: 	bool TryToRecordClientReplay();
  59: 	virtual bool ShouldRecordClientReplay();
  63: 	UFUNCTION(Reliable, Server, WithValidation)
  64: 	void ServerCheat(const FString& Msg);
  68: 	UFUNCTION(Reliable, Server, WithValidation)
  69: 	void ServerCheatAll(const FString& Msg);
  74: 	virtual void PreInitializeComponents() override;
  77: 	virtual void BeginPlay() override;
  80: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  83: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
  90: 	virtual void OnPossess(APawn* InPawn) override;
  93: 	virtual void OnUnPossess() override;
  96: 	virtual void InitPlayerState() override;
  99: 	virtual void CleanupPlayerState() override;
 102: 	virtual void OnRep_PlayerState() override;
 109: 	virtual void ReceivedPlayer() override;
 112: 	virtual void PlayerTick(float DeltaTime) override;
 115: 	virtual void SetPlayer(UPlayer* InPlayer) override;
 118: 	virtual void AddCheats(bool bForce) override;
 121: 	virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
 124: 	virtual void
 125: 	UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
 128: 	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
 131: 	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
 138: 	virtual void OnCameraPenetratingTarget() override;
 156: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 157: 	void SetIsAutoRunning(const bool bEnabled);
 160: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 161: 	bool GetIsAutoRunning() const;
 163: private:
 169: 	UPROPERTY()
 170: 	TObjectPtr<APlayerState> LastSeenPlayerState;
 172: private:
 174: 	UFUNCTION()
 175: 	void OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);
 177: protected:
 180: 	virtual void OnPlayerStateChanged();
 182: private:
 184: 	void BroadcastOnPlayerStateChanged();
 186: protected:
 195: 	void OnStartAutoRun();
 198: 	void OnEndAutoRun();
 201: 	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartAutoRun"))
 202: 	void K2_OnStartAutoRun();
 205: 	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndAutoRun"))
 206: 	void K2_OnEndAutoRun();
 209: 	bool bHideViewTargetPawnNextFrame = false;
 210: };
 214: UCLASS()
 215: class AHodgeReplayPlayerController : public AHodgePlayerController
 216: {
 217: 	GENERATED_BODY()
 220: 	virtual void Tick(float DeltaSeconds) override;
 223: 	virtual void SmoothTargetViewRotation(APawn* TargetPawn, float DeltaSeconds) override;
 226: 	virtual bool ShouldRecordClientReplay() override;
 230: 	void RecorderPlayerStateUpdated(APlayerState* NewRecorderPlayerState);
 234: 	UFUNCTION()
 235: 	void OnPlayerStatePawnSet(APlayerState* ChangedPlayerState, APawn* NewPlayerPawn, APawn* OldPlayerPawn);
 239: 	UPROPERTY(Transient)
 240: 	TObjectPtr<APlayerState> FollowedPlayerState;
 241: };
```

## HodgePlayerControllerBase.h

控制器和 Pawn 生命周期桥接到 LocalPlayer 委托；具体输入消费在派生 HodgePlayerController。

源码：[Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   9: #pragma once
  11: #include "CoreMinimal.h"
  12: #include "GameFramework/PlayerController.h"
  13: #include "HodgePlayerControllerBase.generated.h"
  38: UCLASS()
  39: class HODGEPODGE_API AHodgePlayerControllerBase : public APlayerController
  40: {
  41: 	GENERATED_BODY()
  43: public:
  50: 	AHodgePlayerControllerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  65: 	virtual void ReceivedPlayer() override;
  78: 	virtual void SetPawn(APawn* InPawn) override;
  95: 	virtual void OnPossess(class APawn* APawn) override;
 109: 	virtual void OnUnPossess() override;
 111: protected:
 123: 	virtual void OnRep_PlayerState() override;
 124: };
```

## HodgePlayerState.h

玩家 ASC、HealthSet、PawnData、阵营/标签栈等持有者；SetPawnData 在权威端授予 AbilitySets（未记录句柄）。

源码：[Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/GameplayTagStack.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "AbilitySystemInterface.h"
   7: #include "GenericTeamAgentInterface.h"
   8: #include "HodgePlayerStateBase.h"
   9: #include "AbilitySystem/GameplayTagStack.h"
  10: #include "HodgePlayerState.generated.h"
  12: class UHodgeExperienceDefinition;
  13: class UHodgePawnData;
  14: class UHodgeAbilitySystemComponent;
  15: class AHodgePlayerController;
  20: UENUM()
  21: enum class EHodgePlayerConnectionType : uint8
  22: {
  24: 	Player = 0,
  27: 	LiveSpectator,
  30: 	ReplaySpectator,
  33: 	InactivePlayer
  34: };
  42: UCLASS(Config = Game)
  43: class AHodgePlayerState : public AHodgePlayerStateBase, public IAbilitySystemInterface
  44: {
  45: 	GENERATED_BODY()
  47: public:
  49: 	AHodgePlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  52: 	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerState")
  53: 	AHodgePlayerController* GetHodgePlayerController() const;
  56: 	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerState")
  57: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const { return AbilitySystemComponent; }
  60: 	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
  63: 	template <class T>
  64: 	const T* GetPawnData() const { return Cast<T>(PawnData); }
  67: 	void SetPawnData(const UHodgePawnData* InPawnData);
  72: 	virtual void PreInitializeComponents() override;
  75: 	virtual void PostInitializeComponents() override;
  82: 	virtual void Reset() override;
  85: 	virtual void ClientInitialize(AController* C) override;
  88: 	virtual void CopyProperties(APlayerState* PlayerState) override;
  91: 	virtual void OnDeactivated() override;
  94: 	virtual void OnReactivated() override;
 112: 	static const FName NAME_HodgeAbilityReady;
 115: 	void SetPlayerConnectionType(EHodgePlayerConnectionType NewType);
 118: 	EHodgePlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }
 121: 	UFUNCTION(BlueprintCallable)
 122: 	int32 GetSquadId() const
 123: 	{
 124: 		return MySquadID;
 125: 	}
 137: 	void SetSquadID(int32 NewSquadID);
 140: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
 141: 	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);
 144: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
 145: 	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);
 148: 	UFUNCTION(BlueprintCallable, Category=Teams)
 149: 	int32 GetStatTagStackCount(FGameplayTag Tag) const;
 152: 	UFUNCTION(BlueprintCallable, Category=Teams)
 153: 	bool HasStatTag(FGameplayTag Tag) const;
 161: 	FRotator GetReplicatedViewRotation() const;
 164: 	void SetReplicatedViewRotation(const FRotator& NewRotation);
 166: private:
 168: 	void OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience);
 170: protected:
 172: 	UFUNCTION()
 173: 	void OnRep_PawnData();
 176: 	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
 177: 	TObjectPtr<const UHodgePawnData> PawnData;
 179: private:
 181: 	UPROPERTY(VisibleAnywhere, Category = "Hodge|PlayerState")
 182: 	TObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;
 185: 	UPROPERTY()
 186: 	TObjectPtr<const class UHodgeHealthSet> HealthSet;
 189: 	UPROPERTY(Replicated)
 190: 	EHodgePlayerConnectionType MyPlayerConnectionType;
 196: 	UPROPERTY(ReplicatedUsing=OnRep_MyTeamID)
 197: 	FGenericTeamId MyTeamID;
 200: 	UPROPERTY(ReplicatedUsing=OnRep_MySquadID)
 201: 	int32 MySquadID;
 204: 	UPROPERTY(Replicated)
 205: 	FGameplayTagStackContainer StatTags;
 208: 	UPROPERTY(Replicated)
 209: 	FRotator ReplicatedViewRotation;
 211: private:
 213: 	UFUNCTION()
 214: 	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
 217: 	UFUNCTION()
 218: 	void OnRep_MySquadID();
 219: };
```

## HodgePlayerStateBase.h

PlayerState ModularGameplay Receiver 注册、注销及组件 Reset/CopyProperties。

源码：[Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "GameFramework/PlayerState.h"
   5: #include "HodgePlayerStateBase.generated.h"
  11: UCLASS()
  12: class HODGEPODGE_API AHodgePlayerStateBase : public APlayerState
  13: {
  14: 	GENERATED_BODY()
  16: public:
  20: 	virtual void PreInitializeComponents() override;
  23: 	virtual void BeginPlay() override;
  26: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  29: 	virtual void Reset() override;
  33: protected:
  37: 	virtual void CopyProperties(APlayerState* PlayerState);
  40: };
```
