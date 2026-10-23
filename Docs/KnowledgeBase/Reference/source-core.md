# Core 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeGameInstanceBase.cpp

注册 Init State 顺序、主控制器访问和全局生命周期扩展。

源码：[Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp](../../../Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/GameInstance/HodgeGameInstanceBase.h](../../../Source/Hodgepodge/Public/Core/GameInstance/HodgeGameInstanceBase.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)

定义候选（多行签名仅展示首行）：

- L22: `UHodgeGameInstanceBase::UHodgeGameInstanceBase(const FObjectInitializer& ObjectInitializer)`
- L27: `AHodgePlayerController* UHodgeGameInstanceBase::GetPrimaryPlayerController() const`
- L33: `void UHodgeGameInstanceBase::Shutdown()`
- L39: `void UHodgeGameInstanceBase::Init()`
- L89: `int32 UHodgeGameInstanceBase::AddLocalPlayer(ULocalPlayer* Player, FPlatformUserId UserId)`
- L100: `bool UHodgeGameInstanceBase::RemoveLocalPlayer(ULocalPlayer* Player)`

## HodgeGameModeBase.cpp

服务器选择玩法、等待加载、生成 Pawn，并在 FinishSpawning 前注入 PawnData；选用 HodgePlayerController。

源码：[Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp](../../../Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/GameMode/HodgeGameModeBase.h](../../../Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h)、[AbilitySystem/Stats/HodgeAttributeCoordinator.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeCoordinator.h)、[Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)、[Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Core/GameState/HodgeGameState.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h)、[Core/GameState/HodgeGameStateBase.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h)、[Core/HUD/HodgeHUD.h](../../../Source/Hodgepodge/Public/Core/HUD/HodgeHUD.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)、[Data/HodgeExperienceDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)

定义候选（多行签名仅展示首行）：

- L25: `AHodgeGameModeBase::AHodgeGameModeBase(const FObjectInitializer& ObjectInitializer)`
- L51: `const UHodgePawnData* AHodgeGameModeBase::GetPawnDataForController(const AController* InController) const`
- L93: `void AHodgeGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)`
- L103: `void AHodgeGameModeBase::HandleMatchAssignmentIfNotExpectingOne()`
- L201: `bool AHodgeGameModeBase::TryDedicatedServerLogin()`
- L346: `void AHodgeGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)`
- L371: `void AHodgeGameModeBase::OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience)`
- L393: `bool AHodgeGameModeBase::IsExperienceLoaded() const`
- L408: `UClass* AHodgeGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)`
- L425: `APawn* AHodgeGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,`
- L483: `bool AHodgeGameModeBase::ShouldSpawnAtStartSpot(AController* Player)`
- L490: `void AHodgeGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)`
- L501: `AActor* AHodgeGameModeBase::ChoosePlayerStart_Implementation(AController* Player)`
- L515: `void AHodgeGameModeBase::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)`
- L529: `bool AHodgeGameModeBase::PlayerCanRestart_Implementation(APlayerController* Player)`
- L536: `bool AHodgeGameModeBase::ControllerCanRestart(AController* Controller)`
- L566: `void AHodgeGameModeBase::InitGameState()`
- L582: `void AHodgeGameModeBase::GenericPlayerInitialization(AController* NewPlayer)`
- L592: `void AHodgeGameModeBase::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)`
- L614: `bool AHodgeGameModeBase::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)`
- L621: `void AHodgeGameModeBase::FailedToRestartPlayer(AController* NewPlayer)`
- L661: `void AHodgeGameModeBase::RestartPlayer(AController* NewPlayer)`

## HodgeGameState.cpp

创建 ExperienceManager 和世界状态 ASC，处理游戏状态复制/扩展。

源码：[Source/Hodgepodge/Private/Core/GameState/HodgeGameState.cpp](../../../Source/Hodgepodge/Private/Core/GameState/HodgeGameState.cpp)

项目内直接 include（不是运行调用关系）：[Core/GameState/HodgeGameState.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)、[Core/PlayState/HodgePlayerStateBase.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h)

定义候选（多行签名仅展示首行）：

- L29: `AHodgeGameState::AHodgeGameState(const FObjectInitializer& ObjectInitializer)`
- L55: `void AHodgeGameState::PreInitializeComponents()`
- L62: `void AHodgeGameState::PostInitializeComponents()`
- L74: `UAbilitySystemComponent* AHodgeGameState::GetAbilitySystemComponent() const`
- L81: `void AHodgeGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L88: `void AHodgeGameState::AddPlayerState(APlayerState* PlayerState)`
- L95: `void AHodgeGameState::RemovePlayerState(APlayerState* PlayerState)`
- L103: `void AHodgeGameState::SeamlessTravelTransitionCheckpoint(bool bToTransitionMap)`
- L120: `void AHodgeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L133: `void AHodgeGameState::Tick(float DeltaSeconds)`
- L160: `float AHodgeGameState::GetServerFPS() const`
- L167: `void AHodgeGameState::SetRecorderPlayerState(APlayerState* NewPlayerState)`
- L187: `APlayerState* AHodgeGameState::GetRecorderPlayerState() const`
- L194: `void AHodgeGameState::OnRep_RecorderPlayerState()`

## HodgeGameStateBase.cpp

GameState 基础扩展生命周期。

源码：[Source/Hodgepodge/Private/Core/GameState/HodgeGameStateBase.cpp](../../../Source/Hodgepodge/Private/Core/GameState/HodgeGameStateBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/GameState/HodgeGameStateBase.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h)

定义候选（多行签名仅展示首行）：

- L17: `AHodgeGameStateBase::AHodgeGameStateBase(const FObjectInitializer& ObjectInitializer)`
- L23: `void AHodgeGameStateBase::PreInitializeComponents()`
- L32: `void AHodgeGameStateBase::BeginPlay()`
- L42: `void AHodgeGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)`

## HodgeHUD.cpp

HUD Receiver 提供 Experience AddWidgets 的真实注入／撤销入口，同时保留 GAS 调试 Actor 列表。

源码：[Source/Hodgepodge/Private/Core/HUD/HodgeHUD.cpp](../../../Source/Hodgepodge/Private/Core/HUD/HodgeHUD.cpp)

项目内直接 include（不是运行调用关系）：[Core/HUD/HodgeHUD.h](../../../Source/Hodgepodge/Public/Core/HUD/HodgeHUD.h)

定义候选（多行签名仅展示首行）：

- L40: `AHodgeHUD::AHodgeHUD(const FObjectInitializer& ObjectInitializer)`
- L49: `void AHodgeHUD::PreInitializeComponents()`
- L62: `void AHodgeHUD::BeginPlay()`
- L76: `void AHodgeHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L88: `void AHodgeHUD::GetDebugActorList(TArray<AActor*>& InOutList)`

## HodgeLocalPlayerBase.cpp

本地玩家对象及控制器、PlayerState、Pawn 就绪事件桥。

源码：[Source/Hodgepodge/Private/Core/LocalPlayer/HodgeLocalPlayerBase.cpp](../../../Source/Hodgepodge/Private/Core/LocalPlayer/HodgeLocalPlayerBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)

定义候选（多行签名仅展示首行）：

- L23: `UHodgeLocalPlayerBase::UHodgeLocalPlayerBase()`
- L40: `FDelegateHandle UHodgeLocalPlayerBase::CallAndRegister_OnPlayerControllerSet(`
- L67: `FDelegateHandle UHodgeLocalPlayerBase::CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate)`
- L94: `FDelegateHandle UHodgeLocalPlayerBase::CallAndRegister_OnPlayerPawnSet(FPlayerPawnSetDelegate::FDelegate Delegate)`
- L128: `bool UHodgeLocalPlayerBase::GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData,`

## HodgePlayerController.cpp

具体控制器：每帧消费 ASC 输入、相机管理、AutoRun、UnPossess Avatar 清理及 Replay 扩展。

源码：[Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerController.cpp](../../../Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerController.cpp)

项目内直接 include（不是运行调用关系）：[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[Component/HodgeLockOnComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h)、[UI/IndicatorSystem/HodgeIndicatorManagerComponent.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Camera/HodgePlayerCameraManager.h](../../../Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h)、[Core/HUD/HodgeHUD.h](../../../Source/Hodgepodge/Public/Core/HUD/HodgeHUD.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)、[AbilitySystem/Effects/HodgeResonanceEnergyEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h)、[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)、[AbilitySystem/Effects/HodgeHavocEffects.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeHavocEffects.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)、[Core/GameState/HodgeGameState.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)

定义候选（多行签名仅展示首行）：

- L107: `AHodgePlayerController::AHodgePlayerController(const FObjectInitializer& ObjectInitializer)`
- L121: `void AHodgePlayerController::PreInitializeComponents()`
- L128: `void AHodgePlayerController::BeginPlay()`
- L153: `void AHodgePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L160: `void AHodgePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L183: `void AHodgePlayerController::ReceivedPlayer()`
- L190: `void AHodgePlayerController::PlayerTick(float DeltaTime)`
- L255: `AHodgePlayerState* AHodgePlayerController::GetHodgePlayerState() const`
- L262: `UHodgeAbilitySystemComponent* AHodgePlayerController::GetHodgeAbilitySystemComponent() const`
- L272: `AHodgeHUD* AHodgePlayerController::GetHodgeHUD() const`
- L279: `bool AHodgePlayerController::TryToRecordClientReplay()`
- L308: `bool AHodgePlayerController::ShouldRecordClientReplay()`
- L375: `void AHodgePlayerController::OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)`
- L382: `void AHodgePlayerController::OnPlayerStateChanged()`
- L389: `void AHodgePlayerController::BroadcastOnPlayerStateChanged()`
- L434: `void AHodgePlayerController::InitPlayerState()`
- L444: `void AHodgePlayerController::CleanupPlayerState()`
- L454: `void AHodgePlayerController::OnRep_PlayerState()`
- L464: `void AHodgePlayerController::SetPlayer(UPlayer* InPlayer)`
- L487: `void AHodgePlayerController::AddCheats(bool bForce)`
- L498: `void AHodgePlayerController::ServerCheat_Implementation(const FString& Msg)`
- L507: `void AHodgePlayerController::HodgeFillResonance()`
- L536: `void AHodgePlayerController::HodgeFillDarkFlow()`
- L567: `bool AHodgePlayerController::ServerCheat_Validate(const FString& Msg)`
- L574: `void AHodgePlayerController::ServerCheatAll_Implementation(const FString& Msg)`
- L594: `bool AHodgePlayerController::ServerCheatAll_Validate(const FString& Msg)`
- L601: `void AHodgePlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)`
- L608: `void AHodgePlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)`
- L623: `void AHodgePlayerController::OnCameraPenetratingTarget()`
- L630: `void AHodgePlayerController::OnPossess(APawn* InPawn)`
- L655: `void AHodgePlayerController::SetIsAutoRunning(const bool bEnabled)`
- L678: `bool AHodgePlayerController::GetIsAutoRunning() const`
- L695: `void AHodgePlayerController::OnStartAutoRun()`
- L709: `void AHodgePlayerController::OnEndAutoRun()`
- L723: `void AHodgePlayerController::UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId)`
- L746: `void AHodgePlayerController::UpdateHiddenComponents(const FVector& ViewLocation,`
- L856: `void AHodgePlayerController::OnUnPossess()`
- L885: `void AHodgeReplayPlayerController::Tick(float DeltaSeconds)`
- L923: `void AHodgeReplayPlayerController::SmoothTargetViewRotation(APawn* TargetPawn, float DeltaSeconds)`
- L933: `bool AHodgeReplayPlayerController::ShouldRecordClientReplay()`
- L940: `void AHodgeReplayPlayerController::RecorderPlayerStateUpdated(APlayerState* NewRecorderPlayerState)`
- L960: `void AHodgeReplayPlayerController::OnPlayerStatePawnSet(APlayerState* ChangedPlayerState, APawn* NewPlayerPawn,`

## HodgePlayerControllerBase.cpp

控制器和 Pawn 生命周期桥接到 LocalPlayer 委托；具体输入消费在派生 HodgePlayerController。

源码：[Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerControllerBase.cpp](../../../Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerControllerBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/PlayerController/HodgePlayerControllerBase.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h)、[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)

定义候选（多行签名仅展示首行）：

- L25: `AHodgePlayerControllerBase::AHodgePlayerControllerBase(const FObjectInitializer& ObjectInitializer)`
- L53: `void AHodgePlayerControllerBase::ReceivedPlayer()`
- L85: `void AHodgePlayerControllerBase::SetPawn(APawn* InPawn)`
- L121: `void AHodgePlayerControllerBase::OnPossess(class APawn* APawn)`
- L153: `void AHodgePlayerControllerBase::OnUnPossess()`
- L192: `void AHodgePlayerControllerBase::OnRep_PlayerState()`

## HodgePlayerState.cpp

玩家 ASC、HealthSet、PawnData、阵营/标签栈等持有者；SetPawnData 在权威端授予 AbilitySets（未记录句柄）。

源码：[Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp](../../../Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp)

项目内直接 include（不是运行调用关系）：[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[AbilitySystem/Stats/HodgeAttributeCoordinator.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeCoordinator.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)、[Core/GameMode/HodgeGameModeBase.h](../../../Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)、[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)

定义候选（多行签名仅展示首行）：

- L43: `const FName AHodgePlayerState::NAME_HodgeAbilityReady("HodgeAbilitiesReady");`
- L46: `AHodgePlayerState::AHodgePlayerState(const FObjectInitializer& ObjectInitializer)`
- L80: `AHodgePlayerController* AHodgePlayerState::GetHodgePlayerController() const`
- L87: `UAbilitySystemComponent* AHodgePlayerState::GetAbilitySystemComponent() const`
- L94: `void AHodgePlayerState::SetPawnData(const UHodgePawnData* InPawnData)`
- L131: `void AHodgePlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L139: `void AHodgePlayerState::PreInitializeComponents()`
- L146: `void AHodgePlayerState::PostInitializeComponents()`
- L182: `void AHodgePlayerState::Reset()`
- L189: `void AHodgePlayerState::ClientInitialize(AController* C)`
- L202: `void AHodgePlayerState::CopyProperties(APlayerState* PlayerState)`
- L212: `void AHodgePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L249: `void AHodgePlayerState::OnDeactivated()`
- L285: `void AHodgePlayerState::OnReactivated()`
- L299: `void AHodgePlayerState::SetPlayerConnectionType(EHodgePlayerConnectionType NewType)`
- L309: `void AHodgePlayerState::SetSquadID(int32 NewSquadID)`
- L323: `void AHodgePlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)`
- L330: `void AHodgePlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)`
- L337: `int32 AHodgePlayerState::GetStatTagStackCount(FGameplayTag Tag) const`
- L344: `bool AHodgePlayerState::HasStatTag(FGameplayTag Tag) const`
- L351: `FRotator AHodgePlayerState::GetReplicatedViewRotation() const`
- L358: `void AHodgePlayerState::SetReplicatedViewRotation(const FRotator& NewRotation)`
- L372: `void AHodgePlayerState::OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience)`
- L395: `void AHodgePlayerState::OnRep_PawnData()`
- L402: `void AHodgePlayerState::OnRep_MyTeamID(FGenericTeamId OldTeamID)`
- L409: `void AHodgePlayerState::OnRep_MySquadID()`
- L416: `bool AHodgePlayerState::SetCharacterLevel(int32 Level)`
- L422: `bool AHodgePlayerState::RestoreCharacterHealth(float Health)`
- L428: `bool AHodgePlayerState::InitializeCharacterProgression(FGuid CharacterId, int32 Level, float SavedHealth)`
- L441: `bool AHodgePlayerState::AreAttributesReadyFor(const APawn* Avatar) const`
- L448: `void AHodgePlayerState::NotifyAttributeReadiness()`
- L459: `void AHodgePlayerState::OnRep_AttributeReadyState()`

## HodgePlayerStateBase.cpp

PlayerState ModularGameplay Receiver 注册、注销及组件 Reset/CopyProperties。

源码：[Source/Hodgepodge/Private/Core/PlayState/HodgePlayerStateBase.cpp](../../../Source/Hodgepodge/Private/Core/PlayState/HodgePlayerStateBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/PlayState/HodgePlayerStateBase.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h)

定义候选（多行签名仅展示首行）：

- L13: `void AHodgePlayerStateBase::PreInitializeComponents()`
- L22: `void AHodgePlayerStateBase::BeginPlay()`
- L32: `void AHodgePlayerStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L41: `void AHodgePlayerStateBase::Reset()`
- L58: `void AHodgePlayerStateBase::CopyProperties(APlayerState* PlayerState)`

## HodgeGameInstanceBase.h

注册 Init State 顺序、主控制器访问和全局生命周期扩展。

源码：[Source/Hodgepodge/Public/Core/GameInstance/HodgeGameInstanceBase.h](../../../Source/Hodgepodge/Public/Core/GameInstance/HodgeGameInstanceBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  11: #pragma once
  13: #include "CoreMinimal.h"
  14: #include "Engine/GameInstance.h"
  15: #include "HodgeGameInstanceBase.generated.h"
  17: class AHodgePlayerController;
  25: UCLASS()
  26: class HODGEPODGE_API UHodgeGameInstanceBase : public UGameInstance
  27: {
  28: 	GENERATED_BODY()
  30: public:
  35: 	UHodgeGameInstanceBase(const FObjectInitializer& ObjectInitializer);
  41: 	AHodgePlayerController* GetPrimaryPlayerController() const;
  48:     virtual int32 AddLocalPlayer(ULocalPlayer* Player, FPlatformUserId UserId) override;
  53:     virtual bool RemoveLocalPlayer(ULocalPlayer* Player) override;
  55: protected:
  58: 	virtual void Init() override;
  62: 	virtual void Shutdown() override;
  65: 	TArray<uint8> DebugTestEncryptionKey;
  66: };
```

## HodgeGameModeBase.h

服务器选择玩法、等待加载、生成 Pawn，并在 FinishSpawning 前注入 PawnData；选用 HodgePlayerController。

源码：[Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h](../../../Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  11: #pragma once
  13: #include "CoreMinimal.h"
  14: #include "GameFramework/GameModeBase.h"
  15: #include "HodgeGameModeBase.generated.h"
  17: class UHodgeExperienceDefinition;
  18: class UHodgePawnData;
  24: DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHodgeGameModePlayerInitialized, AGameModeBase*             ,
  25:                                      AController*              );
  28: UCLASS()
  29: class HODGEPODGE_API AHodgeGameModeBase : public AGameModeBase
  30: {
  31: 	GENERATED_BODY()
  33: public:
  38: 	AHodgeGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  45: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Pawn")
  46: 	const UHodgePawnData* GetPawnDataForController(const AController* InController) const;
  56: 	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
  60: 	virtual void RestartPlayer(AController* NewPlayer) override;
  67: 	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
  75: 	virtual APawn*
  76: 	SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
  83: 	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
  89: 	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
  96: 	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
 103: 	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
 110: 	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
 114: 	virtual void InitGameState() override;
 123: 	virtual bool UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage) override;
 129: 	virtual void GenericPlayerInitialization(AController* NewPlayer) override;
 135: 	virtual void FailedToRestartPlayer(AController* NewPlayer) override;
 145: 	UFUNCTION(BlueprintCallable)
 146: 	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset = false);
 153: 	virtual bool ControllerCanRestart(AController* Controller);
 156: 	FOnHodgeGameModePlayerInitialized OnGameModePlayerInitialized;
 158: protected:
 163: 	void OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience);
 169: 	bool IsExperienceLoaded() const;
 176: 	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource);
 180: 	void HandleMatchAssignmentIfNotExpectingOne();
 186: 	bool TryDedicatedServerLogin();
 194: };
```

## HodgeGameState.h

创建 ExperienceManager 和世界状态 ASC，处理游戏状态复制/扩展。

源码：[Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "AbilitySystemInterface.h"
  10: #include "HodgeGameStateBase.h"
  11: #include "HodgeGameState.generated.h"
  13: class UHodgeExperienceManagerComponent;
  14: class UHodgeAbilitySystemComponent;
  21: UCLASS()
  22: class HODGEPODGE_API AHodgeGameState : public AHodgeGameStateBase, public IAbilitySystemInterface
  23: {
  24: 	GENERATED_BODY()
  26: public:
  31: 	AHodgeGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  36: 	virtual void PreInitializeComponents() override;
  40: 	virtual void PostInitializeComponents() override;
  46: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  52: 	virtual void Tick(float DeltaSeconds) override;
  60: 	virtual void AddPlayerState(APlayerState* PlayerState) override;
  66: 	virtual void RemovePlayerState(APlayerState* PlayerState) override;
  72: 	virtual void SeamlessTravelTransitionCheckpoint(bool bToTransitionMap) override;
  80: 	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
  87: 	UFUNCTION(BlueprintCallable, Category = "Hodge|GameState")
  88: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const { return AbilitySystemComponent; }
 102: 	float GetServerFPS() const;
 108: 	void SetRecorderPlayerState(APlayerState* NewPlayerState);
 114: 	APlayerState* GetRecorderPlayerState() const;
 117: 	DECLARE_MULTICAST_DELEGATE_OneParam(FOnRecorderPlayerStateChanged, APlayerState*);
 119: 	FOnRecorderPlayerStateChanged OnRecorderPlayerStateChangedEvent;
 121: private:
 123: 	UPROPERTY()
 124: 	TObjectPtr<UHodgeExperienceManagerComponent> ExperienceManagerComponent;
 127: 	UPROPERTY(VisibleAnywhere, Category = "Hodge|GameState")
 128: 	TObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;
 130: protected:
 132: 	UPROPERTY(Replicated)
 133: 	float ServerFPS;
 137: 	UPROPERTY(Transient, ReplicatedUsing = OnRep_RecorderPlayerState)
 138: 	TObjectPtr<APlayerState> RecorderPlayerState;
 142: 	UFUNCTION()
 143: 	void OnRep_RecorderPlayerState();
 144: };
```

## HodgeGameStateBase.h

GameState 基础扩展生命周期。

源码：[Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  11: #pragma once
  13: #include "CoreMinimal.h"
  14: #include "GameFramework/GameStateBase.h"
  15: #include "HodgeGameStateBase.generated.h"
  25: UCLASS()
  26: class HODGEPODGE_API AHodgeGameStateBase : public AGameStateBase
  27: {
  28: 	GENERATED_BODY()
  30: public:
  35: 	AHodgeGameStateBase(const FObjectInitializer& ObjectInitializer);
  40: 	virtual void PreInitializeComponents() override;
  44: 	virtual void BeginPlay() override;
  50: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  52: };
```

## HodgeHUD.h

HUD Receiver 提供 Experience AddWidgets 的真实注入／撤销入口，同时保留 GAS 调试 Actor 列表。

源码：[Source/Hodgepodge/Public/Core/HUD/HodgeHUD.h](../../../Source/Hodgepodge/Public/Core/HUD/HodgeHUD.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
  10: #include "GameFramework/HUD.h"
  12: #include "HodgeHUD.generated.h"
  16: namespace EEndPlayReason
  17: {
  18: 	enum Type : int;
  19: }
  23: class AActor;
  26: class UObject;
  48: UCLASS(Config = Game)
  49: class HODGEPODGE_API AHodgeHUD : public AHUD
  50: {
  51: 	GENERATED_BODY()
  53: public:
  59: 	AHodgeHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  61: protected:
  67: 	virtual void PreInitializeComponents() override;
  75: 	virtual void BeginPlay() override;
  82: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  95: 	virtual void GetDebugActorList(TArray<AActor*>& InOutList) override;
  98: };
```

## HodgeLocalPlayerBase.h

本地玩家对象及控制器、PlayerState、Pawn 就绪事件桥。

源码：[Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  12: #pragma once
  14: #include "CoreMinimal.h"
  15: #include "Engine/LocalPlayer.h"
  16: #include "HodgeLocalPlayerBase.generated.h"
  33: UCLASS()
  34: class HODGEPODGE_API UHodgeLocalPlayerBase : public ULocalPlayer
  35: {
  36: 	GENERATED_BODY()
  38: public:
  46: 	UHodgeLocalPlayerBase();
  56: 	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerControllerSetDelegate, UHodgeLocalPlayerBase* LocalPlayer,
  57: 	                                     APlayerController* PlayerController);
  59: 	FPlayerControllerSetDelegate OnPlayerControllerSet;
  68: 	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerStateSetDelegate, UHodgeLocalPlayerBase* LocalPlayer,
  69: 	                                     APlayerState* PlayerState);
  71: 	FPlayerStateSetDelegate OnPlayerStateSet;
  80: 	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerPawnSetDelegate, UHodgeLocalPlayerBase* LocalPlayer, APawn* Pawn);
  82: 	FPlayerPawnSetDelegate OnPlayerPawnSet;
  98: 	FDelegateHandle CallAndRegister_OnPlayerControllerSet(FPlayerControllerSetDelegate::FDelegate Delegate);
 113: 	FDelegateHandle CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate);
 128: 	FDelegateHandle CallAndRegister_OnPlayerPawnSet(FPlayerPawnSetDelegate::FDelegate Delegate);
 130: public:
 149: 	virtual bool GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData,
 150: 	                               int32 StereoViewIndex) const override;
 159: 	bool IsPlayerViewEnabled() const { return bIsPlayerViewEnabled; }
 172: 	void SetIsPlayerViewEnabled(bool bInIsPlayerViewEnabled) { bIsPlayerViewEnabled = bInIsPlayerViewEnabled; }
 177: private:
 184: 	bool bIsPlayerViewEnabled = true;
 185: };
```

## HodgePlayerController.h

具体控制器：每帧消费 ASC 输入、相机管理、AutoRun、UnPossess Avatar 清理及 Replay 扩展。

源码：[Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCameraAssistInterface.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraAssistInterface.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   9: #include "CoreMinimal.h"
  12: #include "HodgePlayerControllerBase.h"
  15: #include "Camera/HodgeCameraAssistInterface.h"
  17: #include "HodgePlayerController.generated.h"
  20: class UHodgeAbilitySystemComponent;
  23: class AHodgePlayerState;
  26: class AHodgeHUD;
  34: UCLASS()
  35: class HODGEPODGE_API AHodgePlayerController : public AHodgePlayerControllerBase, public IHodgeCameraAssistInterface
  36: {
  37: 	GENERATED_BODY()
  39: public:
  44: 	AHodgePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  50: 	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
  51: 	AHodgePlayerState* GetHodgePlayerState() const;
  57: 	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
  58: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const;
  64: 	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
  65: 	AHodgeHUD* GetHodgeHUD() const;
  72: 	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
  73: 	bool TryToRecordClientReplay();
  76: 	UFUNCTION(Exec)
  77: 	void HodgeFillResonance();
  79: 	UFUNCTION(Exec)
  80: 	void HodgeFillDarkFlow();
  87: 	virtual bool ShouldRecordClientReplay();
  94: 	UFUNCTION(Reliable, Server, WithValidation)
  95: 	void ServerCheat(const FString& Msg);
 102: 	UFUNCTION(Reliable, Server, WithValidation)
 103: 	void ServerCheatAll(const FString& Msg);
 109: 	virtual void PreInitializeComponents() override;
 113: 	virtual void BeginPlay() override;
 119: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 125: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 135: 	virtual void OnPossess(APawn* InPawn) override;
 139: 	virtual void OnUnPossess() override;
 143: 	virtual void InitPlayerState() override;
 147: 	virtual void CleanupPlayerState() override;
 151: 	virtual void OnRep_PlayerState() override;
 159: 	virtual void ReceivedPlayer() override;
 165: 	virtual void PlayerTick(float DeltaTime) override;
 171: 	virtual void SetPlayer(UPlayer* InPlayer) override;
 177: 	virtual void AddCheats(bool bForce) override;
 184: 	virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
 191: 	virtual void
 192: 	UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
 199: 	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
 206: 	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
 214: 	virtual void OnCameraPenetratingTarget() override;
 235: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 236: 	void SetIsAutoRunning(const bool bEnabled);
 242: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 243: 	bool GetIsAutoRunning() const;
 245: private:
 247: 	UPROPERTY(VisibleAnywhere, Category="Hodge|UI") TObjectPtr<class UHodgeIndicatorManagerComponent> IndicatorManager;
 253: 	UPROPERTY()
 254: 	TObjectPtr<APlayerState> LastSeenPlayerState;
 256: private:
 263: 	UFUNCTION()
 264: 	void OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);
 266: protected:
 270: 	virtual void OnPlayerStateChanged();
 272: private:
 275: 	void BroadcastOnPlayerStateChanged();
 277: protected:
 287: 	void OnStartAutoRun();
 291: 	void OnEndAutoRun();
 295: 	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartAutoRun"))
 296: 	void K2_OnStartAutoRun();
 300: 	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndAutoRun"))
 301: 	void K2_OnEndAutoRun();
 304: 	bool bHideViewTargetPawnNextFrame = false;
 305: };
 309: UCLASS()
 310: class AHodgeReplayPlayerController : public AHodgePlayerController
 311: {
 312: 	GENERATED_BODY()
 318: 	virtual void Tick(float DeltaSeconds) override;
 325: 	virtual void SmoothTargetViewRotation(APawn* TargetPawn, float DeltaSeconds) override;
 331: 	virtual bool ShouldRecordClientReplay() override;
 338: 	void RecorderPlayerStateUpdated(APlayerState* NewRecorderPlayerState);
 347: 	UFUNCTION()
 348: 	void OnPlayerStatePawnSet(APlayerState* ChangedPlayerState, APawn* NewPlayerPawn, APawn* OldPlayerPawn);
 352: 	UPROPERTY(Transient)
 353: 	TObjectPtr<APlayerState> FollowedPlayerState;
 354: };
```

## HodgePlayerControllerBase.h

控制器和 Pawn 生命周期桥接到 LocalPlayer 委托；具体输入消费在派生 HodgePlayerController。

源码：[Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  12: #pragma once
  14: #include "CoreMinimal.h"
  15: #include "GameFramework/PlayerController.h"
  16: #include "HodgePlayerControllerBase.generated.h"
  41: UCLASS()
  42: class HODGEPODGE_API AHodgePlayerControllerBase : public APlayerController
  43: {
  44: 	GENERATED_BODY()
  46: public:
  56: 	AHodgePlayerControllerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  72: 	virtual void ReceivedPlayer() override;
  88: 	virtual void SetPawn(APawn* InPawn) override;
 108: 	virtual void OnPossess(class APawn* APawn) override;
 123: 	virtual void OnUnPossess() override;
 125: protected:
 138: 	virtual void OnRep_PlayerState() override;
 139: };
```

## HodgePlayerState.h

玩家 ASC、HealthSet、PawnData、阵营/标签栈等持有者；SetPawnData 在权威端授予 AbilitySets（未记录句柄）。

源码：[Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/GameplayTagStack.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h)、[AbilitySystem/Stats/HodgeAttributeTypes.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeTypes.h)、[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "AbilitySystemInterface.h"
  10: #include "GenericTeamAgentInterface.h"
  11: #include "HodgePlayerStateBase.h"
  12: #include "AbilitySystem/GameplayTagStack.h"
  13: #include "AbilitySystem/Stats/HodgeAttributeTypes.h"
  14: #include "Data/HodgeAbilitySet.h"
  15: #include "HodgePlayerState.generated.h"
  17: class UHodgeExperienceDefinition;
  18: class UHodgePawnData;
  19: class UHodgeAbilitySystemComponent;
  20: class AHodgePlayerController;
  21: class UHodgeAttributeCoordinator;
  26: UENUM()
  27: enum class EHodgePlayerConnectionType : uint8
  28: {
  30: 	Player = 0,
  33: 	LiveSpectator,
  36: 	ReplaySpectator,
  39: 	InactivePlayer
  40: };
  48: UCLASS(Config = Game)
  49: class AHodgePlayerState : public AHodgePlayerStateBase, public IAbilitySystemInterface
  50: {
  51: 	GENERATED_BODY()
  53: public:
  58: 	AHodgePlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  64: 	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerState")
  65: 	AHodgePlayerController* GetHodgePlayerController() const;
  71: 	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerState")
  72: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const { return AbilitySystemComponent; }
  76: 	UFUNCTION(BlueprintPure, Category="Hodge|Attributes") int32 GetCharacterLevel() const { return CharacterProgression.Level; }
  81: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Attributes") bool SetCharacterLevel(int32 Level);
  86: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Attributes") bool RestoreCharacterHealth(float Health);
  93: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Attributes") bool InitializeCharacterProgression(FGuid CharacterId, int32 Level, float SavedHealth = -1.f);
  97: 	UFUNCTION(BlueprintPure, Category="Hodge|Attributes") FHodgeAttributeReadyState GetAttributeReadyState() const { return AttributeReadyState; }
 101: 	UHodgeAttributeCoordinator* GetAttributeCoordinator() const { return AttributeCoordinator; }
 106: 	bool AreAttributesReadyFor(const APawn* Avatar) const;
 108: 	void NotifyAttributeReadiness();
 110: 	FSimpleMulticastDelegate OnAttributeReadinessChanged;
 116: 	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
 119: 	template <class T>
 123: 	const T* GetPawnData() const { return Cast<T>(PawnData); }
 129: 	void SetPawnData(const UHodgePawnData* InPawnData);
 135: 	virtual void PreInitializeComponents() override;
 139: 	virtual void PostInitializeComponents() override;
 141: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 149: 	virtual void Reset() override;
 155: 	virtual void ClientInitialize(AController* C) override;
 161: 	virtual void CopyProperties(APlayerState* PlayerState) override;
 165: 	virtual void OnDeactivated() override;
 169: 	virtual void OnReactivated() override;
 187: 	static const FName NAME_HodgeAbilityReady;
 193: 	void SetPlayerConnectionType(EHodgePlayerConnectionType NewType);
 199: 	EHodgePlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }
 205: 	UFUNCTION(BlueprintCallable)
 206: 	int32 GetSquadId() const
 207: 	{
 208: 		return MySquadID;
 209: 	}
 224: 	void SetSquadID(int32 NewSquadID);
 231: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
 232: 	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);
 239: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
 240: 	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);
 247: 	UFUNCTION(BlueprintCallable, Category=Teams)
 248: 	int32 GetStatTagStackCount(FGameplayTag Tag) const;
 255: 	UFUNCTION(BlueprintCallable, Category=Teams)
 256: 	bool HasStatTag(FGameplayTag Tag) const;
 267: 	FRotator GetReplicatedViewRotation() const;
 273: 	void SetReplicatedViewRotation(const FRotator& NewRotation);
 275: private:
 280: 	void OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience);
 281: 	friend class UHodgeAttributeCoordinator;
 283: 	UPROPERTY(VisibleAnywhere, Category="Hodge|Attributes") TObjectPtr<UHodgeAttributeCoordinator> AttributeCoordinator;
 285: 	UPROPERTY(Replicated) FHodgeCharacterProgression CharacterProgression;
 287: 	UPROPERTY(ReplicatedUsing=OnRep_AttributeReadyState) FHodgeAttributeReadyState AttributeReadyState;
 289: 	UPROPERTY(Replicated) TArray<FHodgeOwnedEquipmentState> OwnedEquipmentStates;
 291: 	UFUNCTION() void OnRep_AttributeReadyState();
 293: protected:
 296: 	UFUNCTION()
 297: 	void OnRep_PawnData();
 300: 	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
 301: 	TObjectPtr<const UHodgePawnData> PawnData;
 303: private:
 305: 	UPROPERTY(VisibleAnywhere, Category = "Hodge|PlayerState")
 306: 	TObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;
 309: 	UPROPERTY()
 310: 	TObjectPtr<const class UHodgeHealthSet> HealthSet;
 314: 	UPROPERTY()
 315: 	TObjectPtr<const class UHodgeCombatSet> CombatSet;
 317: 	UPROPERTY() TObjectPtr<const class UHodgeResonanceSet> ResonanceSet;
 319: 	UPROPERTY() TObjectPtr<const class UHodgeHavocSet> HavocSet;
 322: 	UPROPERTY(Replicated)
 323: 	EHodgePlayerConnectionType MyPlayerConnectionType;
 329: 	UPROPERTY(ReplicatedUsing=OnRep_MyTeamID)
 330: 	FGenericTeamId MyTeamID;
 333: 	UPROPERTY(ReplicatedUsing=OnRep_MySquadID)
 334: 	int32 MySquadID;
 337: 	UPROPERTY(Replicated)
 338: 	FGameplayTagStackContainer StatTags;
 341: 	UPROPERTY(Replicated)
 342: 	FRotator ReplicatedViewRotation;
 344: private:
 346: 	UPROPERTY(Transient)
 347: 	FHodgeAbilitySet_GrantedHandles BaseAbilityHandles;
 349: 	bool bBaseGrantFailed = false;
 350: 	bool bBaseGrantInProgress = false;
 351: 	bool bBaseGrantsClosed = false;
 357: 	UFUNCTION()
 358: 	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
 362: 	UFUNCTION()
 363: 	void OnRep_MySquadID();
 364: };
```

## HodgePlayerStateBase.h

PlayerState ModularGameplay Receiver 注册、注销及组件 Reset/CopyProperties。

源码：[Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "GameFramework/PlayerState.h"
   8: #include "HodgePlayerStateBase.generated.h"
  14: UCLASS()
  15: class HODGEPODGE_API AHodgePlayerStateBase : public APlayerState
  16: {
  17: 	GENERATED_BODY()
  19: public:
  24: 	virtual void PreInitializeComponents() override;
  28: 	virtual void BeginPlay() override;
  34: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  38: 	virtual void Reset() override;
  42: protected:
  49: 	virtual void CopyProperties(APlayerState* PlayerState);
  52: };
```
