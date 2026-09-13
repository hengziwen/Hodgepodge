// 111屎山代码来袭
// Copyright Epic Games, Inc. All Rights Reserved.

// AHodgePlayerController / AHodgeReplayPlayerController 类定义。
#include "Core/PlayerController/HodgePlayerController.h"

// CommonInput 输入设备类型定义，当前暂未启用。
//#include "CommonInputTypeEnum.h"

// PrimitiveComponent，用于处理相机穿模时需要隐藏的渲染组件。
#include "Components/PrimitiveComponent.h"

// 项目自定义 PlayerState。
#include "Core/PlayState/HodgePlayerState.h"

// 项目自定义 PlayerCameraManager。
#include "Camera/HodgePlayerCameraManager.h"

// 项目 HUD 基类。
#include "Core/HUD/HodgeHUDBase.h"

// 项目自定义 AbilitySystemComponent。
#include "AbilitySystem/HodgeAbilitySystemComponent.h"

// Actor 遍历工具，例如 TActorIterator。
#include "EngineUtils.h"

// 项目统一 GameplayTag 定义。
#include "AbilitySystem/HodgeGameplayTags.h"

// APawn 定义。
#include "GameFramework/Pawn.h"

// 网络属性复制相关宏。
#include "Net/UnrealNetwork.h"

// UGameInstance 定义。
#include "Engine/GameInstance.h"

// GAS 全局辅助接口，用于从 Actor 获取 ASC。
#include "AbilitySystemGlobals.h"

// CommonInputSubsystem，当前暂未启用。
//#include "CommonInputSubsystem.h"

// 项目自定义 LocalPlayer。
#include "Core/LocalPlayer/HodgeLocalPlayerBase.h"

// 项目自定义 GameState。
#include "Core/GameState/HodgeGameState.h"

// 本地设置类，当前暂未启用。
//#include "Settings/HodgeSettingsLocal.h"

// 共享设置类，当前暂未启用。
//#include "Settings/HodgeSettingsShared.h"

// 项目自定义 ReplaySubsystem，当前暂未启用。
//#include "Replays/HodgeReplaySubsystem.h"

// UE ReplaySubsystem。
#include "ReplaySubsystem.h"

// 开发者设置，当前暂未启用。
//#include "Development/HodgeDeveloperSettings.h"

// 游戏地图设置，用于获取默认地图。
#include "GameMapsSettings.h"

// RPC Registry 调试功能相关代码，当前未启用。
// #if WITH_RPC_REGISTRY
// #include "Tests/HodgeGameplayRpcRegistrationComponent.h"
// #include "HttpServerModule.h"
// #endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgePlayerController)

// Hodge 项目命名空间。
namespace Hodge
{
	// 输入相关配置命名空间。
	namespace Input
	{
		// 是否无视当前输入设备类型，始终播放力反馈。
		static int32 ShouldAlwaysPlayForceFeedback = 0;

		// 注册控制台变量 HodgePC.ShouldAlwaysPlayForceFeedback。
		static FAutoConsoleVariableRef CVarShouldAlwaysPlayForceFeedback(TEXT("HodgePC.ShouldAlwaysPlayForceFeedback"),
		                                                                 ShouldAlwaysPlayForceFeedback,
		                                                                 TEXT(
			                                                                 "Should force feedback effects be played, even if the last input device was not a gamepad?"));
	}
}

// PlayerController 构造函数。
AHodgePlayerController::AHodgePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 使用项目自定义 PlayerCameraManager。
	PlayerCameraManagerClass = AHodgePlayerCameraManager::StaticClass();

	// 如果启用了 CheatManager，则可以在这里指定项目自定义 CheatManager。
	// #if USING_CHEAT_MANAGER
	//     CheatClass = UHodgeCheatManager::StaticClass();
	// #endif // #if USING_CHEAT_MANAGER
}

// Actor 组件初始化前调用。
void AHodgePlayerController::PreInitializeComponents()
{
	// 执行父类初始化逻辑。
	Super::PreInitializeComponents();
}

// PlayerController 正式进入游戏时调用。
void AHodgePlayerController::BeginPlay()
{
	// 执行父类 BeginPlay。
	Super::BeginPlay();

	// RPC Registry / HTTP 调试接口相关逻辑，当前关闭。
	// #if WITH_RPC_REGISTRY
	//     FHttpServerModule::Get().StartAllListeners();
	//     int32 RpcPort = 0;
	//     if (FParse::Value(FCommandLine::Get(), TEXT("rpcport="), RpcPort))
	//     {
	//        UHodgeGameplayRpcRegistrationComponent* ObjectInstance = UHodgeGameplayRpcRegistrationComponent::GetInstance();
	//        if (ObjectInstance && ObjectInstance->IsValidLowLevel())
	//        {
	//           ObjectInstance->RegisterAlwaysOnHttpCallbacks();
	//           ObjectInstance->RegisterInMatchHttpCallbacks();
	//        }
	//     }
	// #endif

	// 确保 PlayerController Actor 本身不处于 Hidden 状态。
	SetActorHiddenInGame(false);
}

// PlayerController 生命周期结束时调用。
void AHodgePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 执行父类 EndPlay。
	Super::EndPlay(EndPlayReason);
}

// 注册网络复制属性。
void AHodgePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 先注册父类复制属性。
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Disable replicating the PC target view as it doesn't work well for replays or client-side spectating.
	// 禁止复制 APlayerController 原生 TargetViewRotation，因为它对 Replay 和客户端观战支持并不好。

	// The engine TargetViewRotation is only set in APlayerController::TickActor if the server knows ahead of time that 
	// a specific pawn is being spectated and it only replicates down for COND_OwnerOnly.
	// 引擎只有在服务器提前知道正在观战哪个 Pawn 时才更新 TargetViewRotation，并且它只按照 COND_OwnerOnly 复制。

	// In client-saved replays, COND_OwnerOnly is never true and the target pawn is not always known at the time of recording.
	// 客户端保存 Replay 时 COND_OwnerOnly 不会成立，而且录制时也不一定提前知道目标 Pawn。

	// To support client-saved replays, the replication of this was moved to ReplicatedViewRotation and updated in PlayerTick.
	// 因此项目改为通过 PlayerState 中的 ReplicatedViewRotation 自己同步视角旋转，并在 PlayerTick 中持续更新。

	// 禁用 APlayerController 原生 TargetViewRotation 属性复制。
	DISABLE_REPLICATED_PROPERTY(APlayerController, TargetViewRotation);
}

// 客户端成功接收到 PlayerController 后调用。
void AHodgePlayerController::ReceivedPlayer()
{
	// 执行父类逻辑。
	Super::ReceivedPlayer();
}

// PlayerController 每帧 Tick。
void AHodgePlayerController::PlayerTick(float DeltaTime)
{
	// 执行父类每帧逻辑。
	Super::PlayerTick(DeltaTime);

	// If we are auto running then add some player input
	// 如果当前处于自动奔跑状态，则每帧主动注入向前移动输入。
	if (GetIsAutoRunning())
	{
		// 获取当前 Controller 正在控制的 Pawn。
		if (APawn* CurrentPawn = GetPawn())
		{
			// 只使用 Controller 的 Yaw 构造水平移动旋转。
			const FRotator MovementRotation(0.0f, GetControlRotation().Yaw, 0.0f);

			// 将世界 ForwardVector 旋转到当前控制朝向，得到自动奔跑方向。
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);

			// 持续向当前 Pawn 添加满强度前进输入。
			CurrentPawn->AddMovementInput(MovementDirection, 1.0f);
		}
	}

	// 获取当前 Controller 自己的 HodgePlayerState。
	AHodgePlayerState* HodgePlayerState = GetHodgePlayerState();

	// 相机管理器和 PlayerState 都有效时更新观战视角同步。
	if (PlayerCameraManager && HodgePlayerState)
	{
		// 获取当前真正作为 Camera ViewTarget 的 Pawn。
		APawn* TargetPawn = PlayerCameraManager->GetViewTargetPawn();

		// 存在有效目标 Pawn 时处理视角旋转。
		if (TargetPawn)
		{
			// Update view rotation on the server so it replicates
			// 权威端或本地控制目标时，将 Pawn 当前视角写入 PlayerState 进行复制。
			if (HasAuthority() || TargetPawn->IsLocallyControlled())
			{
				// 保存当前目标 Pawn 的视角旋转。
				HodgePlayerState->SetReplicatedViewRotation(TargetPawn->GetViewRotation());
			}

			// Update the target view rotation if the pawn isn't locally controlled
			// 如果正在观察的 Pawn 不是本地控制，则从该 Pawn 自己的 PlayerState 读取复制视角。
			if (!TargetPawn->IsLocallyControlled())
			{
				// 注意这里获取的是被观战 Pawn 的 PlayerState，而不是当前 Controller 自己的 PlayerState。
				HodgePlayerState = TargetPawn->GetPlayerState<AHodgePlayerState>();

				// 被观战 Pawn 存在有效 HodgePlayerState。
				if (HodgePlayerState)
				{
					// Get it from the spectated pawn's player state, which may not be the same as the PC's playerstate
					// 从被观战玩家自己的 PlayerState 获取复制后的视角旋转。

					// 更新 Controller 的 TargetViewRotation，供观战相机使用。
					TargetViewRotation = HodgePlayerState->GetReplicatedViewRotation();
				}
			}
		}
	}
}

// 获取当前 PlayerController 的 HodgePlayerState。
AHodgePlayerState* AHodgePlayerController::GetHodgePlayerState() const
{
	// 允许 PlayerState 为空，但非空时要求必须是 AHodgePlayerState。
	return CastChecked<AHodgePlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

// 获取当前玩家的 Hodge AbilitySystemComponent。
UHodgeAbilitySystemComponent* AHodgePlayerController::GetHodgeAbilitySystemComponent() const
{
	// 先从 Controller 获取 PlayerState。
	const AHodgePlayerState* HodgePS = GetHodgePlayerState();

	// ASC 挂在 PlayerState 上，因此通过 PlayerState 获取。
	return (HodgePS ? HodgePS->GetHodgeAbilitySystemComponent() : nullptr);
}

// 获取当前 Controller 使用的 Hodge HUD。
AHodgeHUDBase* AHodgePlayerController::GetHodgeHUD() const
{
	// 允许 HUD 为空，但存在时要求类型必须为 AHodgeHUDBase。
	return CastChecked<AHodgeHUDBase>(GetHUD(), ECastCheckedType::NullAllowed);
}

// 尝试开始录制客户端 Replay。
bool AHodgePlayerController::TryToRecordClientReplay()
{
	// See if we should record a replay
	// 先判断当前运行环境是否允许录制客户端 Replay。
	if (ShouldRecordClientReplay())
	{
		// 项目自定义 ReplaySubsystem 录制逻辑，当前暂未启用。
		// if (UHodgeReplaySubsystem* ReplaySubsystem = GetGameInstance()->GetSubsystem<UHodgeReplaySubsystem>())
		// {
		//     APlayerController* FirstLocalPlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		//     if (FirstLocalPlayerController == this)
		//     {
		//        // If this is the first player, update the spectator player for local replays and then record
		//        if (AHodgeGameState* GameState = Cast<AHodgeGameState>(GetWorld()->GetGameState()))
		//        {
		//           GameState->SetRecorderPlayerState(PlayerState);
		//
		//           ReplaySubsystem->RecordClientReplay(this);
		//           return true;
		//        }
		//     }
		// }
	}

	// 当前实际录制代码未启用，因此返回 false。
	return false;
}

// 判断当前 Controller 是否满足自动录制客户端 Replay 的条件。
bool AHodgePlayerController::ShouldRecordClientReplay()
{
	// 获取当前 World。
	UWorld* World = GetWorld();

	// 获取当前 GameInstance。
	UGameInstance* GameInstance = GetGameInstance();

	// 只有有效 GameInstance / World、当前不在播放 Replay、不在录制 Replay、
	// 非 DedicatedServer 且当前为本地 PlayerController 时才继续判断。
	if (GameInstance != nullptr &&
		World != nullptr &&
		!World->IsPlayingReplay() &&
		!World->IsRecordingClientReplay() &&
		NM_DedicatedServer != GetNetMode() &&
		IsLocalPlayerController())
	{
		// 获取项目配置的默认游戏地图。
		FString DefaultMap = UGameMapsSettings::GetGameDefaultMap();

		// 获取当前 World 正在使用的地图名称。
		FString CurrentMap = World->URL.Map;

#if WITH_EDITOR
		// PIE 环境下移除地图包名前面的 PIE 前缀，便于和真实默认地图名比较。
		CurrentMap = UWorld::StripPIEPrefixFromPackageName(CurrentMap, World->StreamingLevelsPrefix);
#endif

		// 当前正处于默认地图。
		if (CurrentMap == DefaultMap)
		{
			// Never record demos on the default frontend map, this could be replaced with a better check for being in the main menu
			// 默认前端 / 主菜单地图不录制 Replay。
			return false;
		}

		// 获取 UE ReplaySubsystem。
		if (UReplaySubsystem* ReplaySubsystem = GameInstance->GetSubsystem<UReplaySubsystem>())
		{
			// 如果已经在录制或播放 Replay。
			if (ReplaySubsystem->IsRecording() || ReplaySubsystem->IsPlaying())
			{
				// Only one at a time
				// 同一时间只允许存在一个 Replay 会话。
				return false;
			}
		}

		// If this is possible, now check the settings
		// 基础条件满足后，再检查玩家本地设置是否允许自动录制。

		// 获取项目自定义 LocalPlayer。
		if (const UHodgeLocalPlayerBase* HodgeLocalPlayer = Cast<UHodgeLocalPlayerBase>(GetLocalPlayer()))
		{
			// 自动录制设置检查，当前暂未启用。
			// if (HodgeLocalPlayer->GetLocalSettings()->ShouldAutoRecordReplays())
			// {
			//     return true;
			// }
		}
	}

	// 不满足自动录制 Replay 条件。
	return false;
}

// PlayerState 队伍发生变化时的回调。
void AHodgePlayerController::OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	// 原本用于将 PlayerState 的队伍变化继续广播到 Controller。
	//ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamId(OldTeam), IntegerToGenericTeamId(NewTeam));
}

// PlayerState 被设置、替换或清理时调用。
void AHodgePlayerController::OnPlayerStateChanged()
{
	// Empty, place for derived classes to implement without having to hook all the other events
	// 默认不做任何操作，作为子类扩展 PlayerState 变化行为的统一入口。
}

// 统一处理 PlayerState 替换后的解绑、重新绑定以及变化广播。
void AHodgePlayerController::BroadcastOnPlayerStateChanged()
{
	// 通知当前类或子类 PlayerState 已发生变化。
	OnPlayerStateChanged();

	// Unbind from the old player state, if any
	// 先准备记录旧 PlayerState 的队伍 ID。
	FGenericTeamId OldTeamID = FGenericTeamId::NoTeam;

	// 如果此前记录过旧 PlayerState。
	if (LastSeenPlayerState != nullptr)
	{
		// 原逻辑：从旧 PlayerState 获取 TeamInterface，并解绑队伍变化委托。
		// if (IHodgeTeamAgentInterface* PlayerStateTeamInterface = Cast<IHodgeTeamAgentInterface>(LastSeenPlayerState))
		// {
		//     OldTeamID = PlayerStateTeamInterface->GetGenericTeamId();
		//     PlayerStateTeamInterface->GetTeamChangedDelegateChecked().RemoveAll(this);
		// }
	}

	// Bind to the new player state, if any
	// 准备记录新 PlayerState 的队伍 ID。
	FGenericTeamId NewTeamID = FGenericTeamId::NoTeam;

	// 当前存在新的 PlayerState。
	if (PlayerState != nullptr)
	{
		// 原逻辑：从新 PlayerState 获取 TeamInterface，并绑定队伍变化委托。
		// if (IHodgeTeamAgentInterface* PlayerStateTeamInterface = Cast<IHodgeTeamAgentInterface>(PlayerState))
		// {
		//     NewTeamID = PlayerStateTeamInterface->GetGenericTeamId();
		//     PlayerStateTeamInterface->GetTeamChangedDelegateChecked().AddDynamic(
		//        this, &ThisClass::OnPlayerStateChangedTeam);
		// }
	}

	// Broadcast the team change (if it really has)
	// 如果队伍确实发生变化，则继续广播 TeamChanged。
	//ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);

	// 缓存当前 PlayerState，作为下一次变化时的旧值。
	LastSeenPlayerState = PlayerState;
}

// 服务端初始化 PlayerState 后调用。
void AHodgePlayerController::InitPlayerState()
{
	// 执行 AController 默认 PlayerState 初始化。
	Super::InitPlayerState();

	// 统一处理新 PlayerState。
	BroadcastOnPlayerStateChanged();
}

// PlayerState 被清理时调用。
void AHodgePlayerController::CleanupPlayerState()
{
	// 执行父类清理逻辑。
	Super::CleanupPlayerState();

	// 统一处理 PlayerState 被移除后的变化。
	BroadcastOnPlayerStateChanged();
}

// PlayerState 在客户端完成复制后调用。
void AHodgePlayerController::OnRep_PlayerState()
{
	// 执行父类 RepNotify。
	Super::OnRep_PlayerState();

	// 统一处理客户端新 PlayerState。
	BroadcastOnPlayerStateChanged();
}

// 为当前 PlayerController 设置 UPlayer。
void AHodgePlayerController::SetPlayer(UPlayer* InPlayer)
{
	// 执行父类 SetPlayer。
	Super::SetPlayer(InPlayer);

	// 本地玩家为项目自定义 HodgeLocalPlayer 时，可以绑定项目设置。
	if (const UHodgeLocalPlayerBase* HodgeLocalPlayer = Cast<UHodgeLocalPlayerBase>(InPlayer))
	{
		// 获取共享设置并监听设置变化，当前暂未启用。
		// UHodgeSettingsShared* UserSettings = HodgeLocalPlayer->GetSharedSettings();
		// UserSettings->OnSettingChanged.AddUObject(this, &ThisClass::OnSettingsChanged);
		//
		// OnSettingsChanged(UserSettings);
	}
}

// 玩家共享设置变化后的处理逻辑，当前暂未启用。
// void AHodgePlayerController::OnSettingsChanged(UHodgeSettingsShared* InSettings)
// {
//  bForceFeedbackEnabled = InSettings->GetForceFeedbackEnabled();
// }

// 创建 / 启用 CheatManager。
void AHodgePlayerController::AddCheats(bool bForce)
{
	// 根据 USING_CHEAT_MANAGER 决定是否强制创建 CheatManager，当前代码被禁用。
	// #if USING_CHEAT_MANAGER
	//     Super::AddCheats(true);
	// #else //#if USING_CHEAT_MANAGER
	//     Super::AddCheats(bForce);
	// #endif // #else //#if USING_CHEAT_MANAGER
}

// ServerCheat 服务端 RPC 实现。
void AHodgePlayerController::ServerCheat_Implementation(const FString& Msg)
{
	// 在启用 CheatManager 时由服务器执行控制台作弊命令，当前暂未启用。
	// #if USING_CHEAT_MANAGER
	//     if (CheatManager)
	//     {
	//        UE_LOG(LogHodge, Warning, TEXT("ServerCheat: %s"), *Msg);
	//        ClientMessage(ConsoleCommand(Msg));
	//     }
	// #endif // #if USING_CHEAT_MANAGER
}

// ServerCheat RPC 参数验证。
bool AHodgePlayerController::ServerCheat_Validate(const FString& Msg)
{
	// 当前不限制 Cheat 消息内容，始终通过验证。
	return true;
}

// ServerCheatAll 服务端 RPC 实现。
void AHodgePlayerController::ServerCheatAll_Implementation(const FString& Msg)
{
	// 启用 CheatManager 时可以遍历所有 HodgePlayerController 并执行同一作弊指令。
	// #if USING_CHEAT_MANAGER
	//  if (CheatManager)
	//  {
	//     UE_LOG(LogHodge, Warning, TEXT("ServerCheatAll: %s"), *Msg);
	//     for (TActorIterator<AHodgePlayerController> It(GetWorld()); It; ++It)
	//     {
	//        AHodgePlayerController* HodgePC = (*It);
	//        if (HodgePC)
	//        {
	//           HodgePC->ClientMessage(HodgePC->ConsoleCommand(Msg));
	//        }
	//     }
	//  }
	// #endif // #if USING_CHEAT_MANAGER
}

// ServerCheatAll RPC 参数验证。
bool AHodgePlayerController::ServerCheatAll_Validate(const FString& Msg)
{
	// 当前始终允许执行。
	return true;
}

// 当前帧正式处理玩家输入之前调用。
void AHodgePlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	// 执行父类输入预处理。
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

// 当前帧输入处理完成后调用。
void AHodgePlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	// 获取玩家 ASC。
	if (UHodgeAbilitySystemComponent* HodgeASC = GetHodgeAbilitySystemComponent())
	{
		// 在所有 InputAction 更新之后统一让 ASC 处理 Pressed / Held / Released Ability 输入。
		HodgeASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	// 最后执行父类输入后处理。
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

// 相机检测到正在穿透当前观察目标时调用。
void AHodgePlayerController::OnCameraPenetratingTarget()
{
	// 标记下一帧隐藏当前 ViewTarget Pawn，避免摄像机进入角色模型内部。
	bHideViewTargetPawnNextFrame = true;
}

// Controller 成功 Possess 新 Pawn 后调用。
void AHodgePlayerController::OnPossess(APawn* InPawn)
{
	// 先完成父类 Possess 流程。
	Super::OnPossess(InPawn);

#if WITH_SERVER_CODE && WITH_EDITOR
	// 仅服务器代码并且 Editor 环境下执行开发调试逻辑。
	if (GIsEditor && (InPawn != nullptr) && (GetPawn() == InPawn))
	{
		// 原本可以在玩家 Possess 时执行开发者配置中的 Cheat 命令。
		// for (const FHodgeCheatToRun& CheatRow : GetDefault<UHodgeDeveloperSettings>()->CheatsToRun)
		// {
		//     if (CheatRow.Phase == ECheatExecutionTime::OnPlayerPawnPossession)
		//     {
		//        ConsoleCommand(CheatRow.Cheat, /*bWriteToLog=*/ true);
		//     }
		// }
	}
#endif

	// 切换 Pawn 后强制结束自动奔跑。
	SetIsAutoRunning(false);
}

// 设置自动奔跑状态。
void AHodgePlayerController::SetIsAutoRunning(const bool bEnabled)
{
	// 获取当前实际自动奔跑状态。
	const bool bIsAutoRunning = GetIsAutoRunning();

	// 只有目标状态与当前状态不同时才执行切换。
	if (bEnabled != bIsAutoRunning)
	{
		// 目标状态为关闭自动奔跑。
		if (!bEnabled)
		{
			// 执行自动奔跑结束逻辑。
			OnEndAutoRun();
		}
		else
		{
			// 执行自动奔跑开始逻辑。
			OnStartAutoRun();
		}
	}
}

// 查询当前是否处于自动奔跑状态。
bool AHodgePlayerController::GetIsAutoRunning() const
{
	// 默认未开启自动奔跑。
	bool bIsAutoRunning = false;

	// 获取当前玩家 ASC。
	if (const UHodgeAbilitySystemComponent* HodgeASC = GetHodgeAbilitySystemComponent())
	{
		// 通过 ASC 上的 Status.AutoRunning GameplayTag 判断自动奔跑状态。
		bIsAutoRunning = HodgeASC->GetTagCount(HodgeGameplayTags::Status_AutoRunning) > 0;
	}

	// 返回当前自动奔跑状态。
	return bIsAutoRunning;
}

// 开始自动奔跑。
void AHodgePlayerController::OnStartAutoRun()
{
	// 获取玩家 ASC。
	if (UHodgeAbilitySystemComponent* HodgeASC = GetHodgeAbilitySystemComponent())
	{
		// 给 ASC 设置本地 Loose GameplayTag，表示进入 AutoRunning 状态。
		HodgeASC->SetLooseGameplayTagCount(HodgeGameplayTags::Status_AutoRunning, 1);

		// 通知蓝图执行自动奔跑开始表现。
		K2_OnStartAutoRun();
	}
}

// 结束自动奔跑。
void AHodgePlayerController::OnEndAutoRun()
{
	// 获取玩家 ASC。
	if (UHodgeAbilitySystemComponent* HodgeASC = GetHodgeAbilitySystemComponent())
	{
		// 清除 AutoRunning Loose GameplayTag。
		HodgeASC->SetLooseGameplayTagCount(HodgeGameplayTags::Status_AutoRunning, 0);

		// 通知蓝图执行自动奔跑结束表现。
		K2_OnEndAutoRun();
	}
}

// 更新手柄力反馈输出。
void AHodgePlayerController::UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId)
{
	// 当前允许播放力反馈。
	if (bForceFeedbackEnabled)
	{
		// 原设计：根据当前最后使用的输入设备决定是否真正播放力反馈。
		// if (const UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetLocalPlayer()))
		// {
		//     const ECommonInputType CurrentInputType = CommonInputSubsystem->GetCurrentInputType();
		//     if (Hodge::Input::ShouldAlwaysPlayForceFeedback || CurrentInputType == ECommonInputType::Gamepad ||
		//        CurrentInputType == ECommonInputType::Touch)
		//     {
		//        InputInterface->SetForceFeedbackChannelValues(ControllerId, ForceFeedbackValues);
		//        return;
		//     }
		// }
	}

	// 当前条件不满足时，将所有力反馈通道输出清零。
	InputInterface->SetForceFeedbackChannelValues(ControllerId, FForceFeedbackValues());
}

// 更新当前视角需要隐藏的 PrimitiveComponent。
void AHodgePlayerController::UpdateHiddenComponents(const FVector& ViewLocation,
                                                    TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
	// 先执行 PlayerController 默认隐藏组件逻辑。
	Super::UpdateHiddenComponents(ViewLocation, OutHiddenComponents);

	// 相机上一阶段检测到穿透观察目标。
	if (bHideViewTargetPawnNextFrame)
	{
		// 获取当前相机真正的 ViewTarget Actor。
		AActor* const ViewTargetPawn = PlayerCameraManager
			                               ? Cast<AActor>(PlayerCameraManager->GetViewTarget())
			                               : nullptr;

		// 当前存在有效观察目标。
		if (ViewTargetPawn)
		{
			// internal helper func to hide all the components
			// 内部辅助 Lambda：将传入的 PrimitiveComponent 及其附着子组件加入隐藏集合。
			auto AddToHiddenComponents = [&OutHiddenComponents](
				const TInlineComponentArray<UPrimitiveComponent*>& InComponents)
			{
				// add every component and all attached children
				// 遍历 Actor 自身的所有 PrimitiveComponent。
				for (UPrimitiveComponent* Comp : InComponents)
				{
					// 只有已经注册到场景中的组件才具有有效 PrimitiveSceneId。
					if (Comp->IsRegistered())
					{
						// 将当前组件加入本帧隐藏集合。
						OutHiddenComponents.Add(Comp->GetPrimitiveSceneId());

						// 同时检查附着在当前组件下面的子组件。
						for (USceneComponent* AttachedChild : Comp->GetAttachChildren())
						{
							// 用于阻止子组件跟随父组件自动隐藏的特殊 ComponentTag。
							static FName NAME_NoParentAutoHide(TEXT("NoParentAutoHide"));

							// 只有 PrimitiveComponent 才能加入渲染隐藏集合。
							UPrimitiveComponent* AttachChildPC = Cast<UPrimitiveComponent>(AttachedChild);

							// 子组件有效、已注册，并且没有 NoParentAutoHide 标签时一起隐藏。
							if (AttachChildPC && AttachChildPC->IsRegistered() && !AttachChildPC->ComponentTags.
								Contains(NAME_NoParentAutoHide))
							{
								// 隐藏附着子组件。
								OutHiddenComponents.Add(AttachChildPC->GetPrimitiveSceneId());
							}
						}
					}
				}
			};

			//TODO Solve with an interface.  Gather hidden components or something.
			//TODO：后续可以通过接口让 ViewTarget 自己提供需要被隐藏的组件。

			//TODO Hiding isn't awesome, sometimes you want the effect of a fade out over a proximity, needs to bubble up to designers.
			//TODO：直接隐藏并不是最佳表现，后续可以考虑按摄像机距离渐隐等更平滑方案。

			// hide pawn's components
			// 收集当前 ViewTarget 上的全部 PrimitiveComponent。
			TInlineComponentArray<UPrimitiveComponent*> PawnComponents;

			// 获取目标 Actor 所有 PrimitiveComponent。
			ViewTargetPawn->GetComponents(PawnComponents);

			// 将 Pawn 组件及其附着子组件加入隐藏集合。
			AddToHiddenComponents(PawnComponents);

			//// hide weapon too
			// 也可以进一步隐藏角色当前武器，当前暂未启用。
			//if (ViewTargetPawn->CurrentWeapon)
			//{
			// TInlineComponentArray<UPrimitiveComponent*> WeaponComponents;
			// ViewTargetPawn->CurrentWeapon->GetComponents(WeaponComponents);
			// AddToHiddenComponents(WeaponComponents);
			//}
		}

		// we consumed it, reset for next frame
		// 本帧已经消费该标记，重置状态等待下一次相机穿透事件。
		bHideViewTargetPawnNextFrame = false;
	}
}

// PlayerController 队伍 ID 不应该直接设置，而应由关联 PlayerState 驱动。
// void AHodgePlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
// {
//  UE_LOG(LogHodgeTeams, Error,
//         TEXT("You can't set the team ID on a player controller (%s); it's driven by the associated player state"),
//         *GetPathNameSafe(this));
// }
//
// 从 PlayerState 的 TeamInterface 获取当前 Controller 所属队伍。
// FGenericTeamId AHodgePlayerController::GetGenericTeamId() const
// {
//  if (const IHodgeTeamAgentInterface* PSWithTeamInterface = Cast<IHodgeTeamAgentInterface>(PlayerState))
//  {
//     return PSWithTeamInterface->GetGenericTeamId();
//  }
//  return FGenericTeamId::NoTeam;
// }
//
// 返回 Controller 自己的队伍变化委托。
// FOnHodgeTeamIndexChangedDelegate* AHodgePlayerController::GetOnTeamIndexChangedDelegate()
// {
//  return &OnTeamChangedDelegate;
// }

// Controller 准备解除当前 Pawn 控制时调用。
void AHodgePlayerController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	// 确保即将被解除控制的 Pawn 不会继续残留为 PlayerState ASC 的 AvatarActor。

	// 获取当前即将被 UnPossess 的 Pawn。
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		// ASC 挂在 PlayerState 上，因此通过 AbilitySystemGlobals 从 PlayerState 获取 ASC。
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			// 只有当前 ASC Avatar 确实是这个即将解除控制的 Pawn 时才清理。
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				// 清空 AvatarActor，避免 ASC 继续引用已经不受该 Controller 控制的 Pawn。
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	// 最后执行父类 UnPossess。
	Super::OnUnPossess();
}

//////////////////////////////////////////////////////////////////////
// AHodgeReplayPlayerController
// 以下为专门用于 Replay 录制 / 回放的 PlayerController 实现。

// Replay PlayerController 每帧 Tick。
void AHodgeReplayPlayerController::Tick(float DeltaSeconds)
{
	// 执行父类 Tick。
	Super::Tick(DeltaSeconds);

	// The state may go invalid at any time due to scrubbing during a replay
	// Replay 拖动时间轴时 PlayerState 可能随时失效，因此每帧都需要检查当前跟随状态。

	// 当前 FollowedPlayerState 已失效。
	if (!IsValid(FollowedPlayerState))
	{
		// 获取当前 World。
		UWorld* World = GetWorld();

		// Listen for changes for both recording and playback
		// 无论录制还是回放，都监听 GameState 的 RecorderPlayerState 变化。

		// 获取项目 GameState。
		if (AHodgeGameState* GameState = Cast<AHodgeGameState>(World->GetGameState()))
		{
			// 避免重复绑定 RecorderPlayerStateChanged 事件。
			if (!GameState->OnRecorderPlayerStateChangedEvent.IsBoundToObject(this))
			{
				// 监听 Replay 记录者 PlayerState 发生变化。
				GameState->OnRecorderPlayerStateChangedEvent.AddUObject(this, &ThisClass::RecorderPlayerStateUpdated);
			}

			// 如果当前 GameState 已经拥有 RecorderPlayerState。
			if (APlayerState* RecorderState = GameState->GetRecorderPlayerState())
			{
				// 立即同步一次当前跟随对象。
				RecorderPlayerStateUpdated(RecorderState);
			}
		}
	}
}

// Replay 下平滑目标 Pawn 的观察旋转。
void AHodgeReplayPlayerController::SmoothTargetViewRotation(APawn* TargetPawn, float DeltaSeconds)
{
	// Default behavior is to interpolate to TargetViewRotation which is set from APlayerController::TickActor but it's not very smooth
	// 默认逻辑会向 TargetViewRotation 插值，但原生 TickActor 更新出来的 TargetViewRotation 在 Replay 下并不够平滑。

	// 当前仍使用父类默认插值行为。
	Super::SmoothTargetViewRotation(TargetPawn, DeltaSeconds);
}

// Replay 专用 PlayerController 不应该再次触发客户端 Replay 录制。
bool AHodgeReplayPlayerController::ShouldRecordClientReplay()
{
	// 禁止 Replay PlayerController 进行嵌套录制。
	return false;
}

// GameState 中 RecorderPlayerState 更新后的回调。
void AHodgeReplayPlayerController::RecorderPlayerStateUpdated(APlayerState* NewRecorderPlayerState)
{
	// 只有传入有效 PlayerState 时才切换跟随对象。
	if (NewRecorderPlayerState)
	{
		// 保存当前 Replay 正在跟随的 PlayerState。
		FollowedPlayerState = NewRecorderPlayerState;

		// Bind to when pawn changes and call now
		// 监听该 PlayerState 后续 Pawn 变化，并立即按当前 Pawn 执行一次更新。

		// 当 PlayerState 对应 Pawn 切换时触发回调。
		NewRecorderPlayerState->OnPawnSet.AddUniqueDynamic(this, &AHodgeReplayPlayerController::OnPlayerStatePawnSet);

		// 立即使用当前 Pawn 更新 Replay ViewTarget。
		OnPlayerStatePawnSet(NewRecorderPlayerState, NewRecorderPlayerState->GetPawn(), nullptr);
	}
}

// 当前正在跟随的 PlayerState 切换 Pawn 时调用。
void AHodgeReplayPlayerController::OnPlayerStatePawnSet(APlayerState* ChangedPlayerState, APawn* NewPlayerPawn,
                                                        APawn* OldPlayerPawn)
{
	// 只有发生变化的是当前 Replay 正在跟随的 PlayerState 时才响应。
	if (ChangedPlayerState == FollowedPlayerState)
	{
		// 将 Replay 相机观察目标切换到该 PlayerState 当前的新 Pawn。
		SetViewTarget(NewPlayerPawn);
	}
}
