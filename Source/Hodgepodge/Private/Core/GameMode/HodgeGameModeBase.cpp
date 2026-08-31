// HodgeGameModeBase 的实现文件。
#include "Core/GameMode/HodgeGameModeBase.h"

#include "GameMapsSettings.h"
#include "Character/HodgeCharacterBase.h"
#include "Component/HodgeExperienceManagerComponent.h"
#include "Core/GameState/HodgeGameState.h"
#include "Core/GameState/HodgeGameStateBase.h"
#include "Core/HUD/HodgeHUDBase.h"
#include "Core/PlayerController/HodgePlayerControllerBase.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Data/HodgeAssetManager.h"
#include "Data/HodgeExperienceDefinition.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameModeBase)


// 构造函数，设置当前游戏使用的默认 Gameplay Framework 类。
AHodgeGameModeBase::AHodgeGameModeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 指定当前游戏使用的 GameState 类。
	GameStateClass = AHodgeGameState::StaticClass();

	// 指定当前游戏使用的 GameSession 类，目前暂未启用。
	//GameSessionClass = AHodgeGameSession::StaticClass();

	// 指定玩家连接后创建的 PlayerController 类。
	PlayerControllerClass = AHodgePlayerControllerBase::StaticClass();

	// 指定 Replay 模式下使用的 PlayerController 类，目前暂未启用。
	//ReplaySpectatorPlayerControllerClass = AHodgeReplayPlayerControllerBase::StaticClass();

	// 指定玩家使用的 PlayerState 类。
	PlayerStateClass = AHodgePlayerState::StaticClass();

	// 指定没有特殊 PawnData 时使用的默认 Pawn 类。
	DefaultPawnClass = AHodgeCharacterBase::StaticClass();

	// 指定当前游戏使用的 HUD 类。
	HUDClass = AHodgeHUDBase::StaticClass();
}

const UHodgePawnData* AHodgeGameModeBase::GetPawnDataForController(const AController* InController) const
{
	// 优先从 Controller 对应的 PlayerState 中获取已经设置好的 PawnData。
	if (InController != nullptr)
	{
		if (const AHodgePlayerState* HodgePS = InController->GetPlayerState<AHodgePlayerState>())
		{
			if (const UHodgePawnData* PawnData = HodgePS->GetPawnData<UHodgePawnData>())
			{
				return PawnData;
			}
		}
	}

	// PlayerState 没有 PawnData 时，回退到当前 Experience 配置的默认 PawnData。
	check(GameState);

	// 从 GameState 上查找 Experience 管理组件。
	UHodgeExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<
		UHodgeExperienceManagerComponent>();
	check(ExperienceComponent);

	// 只有 Experience 加载完成后才能安全获取当前 Experience。
	if (ExperienceComponent->IsExperienceLoaded())
	{
		const UHodgeExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();

		// 当前 Experience 配置了默认 PawnData 时优先使用它。
		if (Experience->DefaultPawnData != nullptr)
		{
			return Experience->DefaultPawnData;
		}

		// Experience 已加载但没有配置 PawnData 时，使用 AssetManager 中的默认 PawnData。
		return UHodgeAssetManager::Get().GetDefaultPawnData();
	}

	// Experience 尚未加载完成，此时暂时没有可用的 PawnData。
	return nullptr;
}

void AHodgeGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	// 执行父类的游戏初始化流程。
	Super::InitGame(MapName, Options, ErrorMessage);

	// 延迟到下一帧处理 Experience 分配，确保启动阶段的相关配置已经完成初始化。
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void AHodgeGameModeBase::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;

	// Experience 选择优先级从高到低：Matchmaking → URL Options → Developer Settings → Command Line → World Settings → Dedicated Server → Default。
	// Matchmaking 分配逻辑目前未接入，因此这里主要处理本地启动参数和默认 Experience。

	UWorld* World = GetWorld();

	// 从地图 URL 的 Experience 参数中获取指定的 Experience。
	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));

		// 将字符串转换为 ExperienceDefinition 类型的 PrimaryAssetId。
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UHodgeExperienceDefinition::StaticClass()->GetFName()),
		                               FName(*ExperienceFromOptions));
		ExperienceIdSource = TEXT("OptionsString");
	}

	// PIE 环境下可以从开发者设置中覆盖当前 Experience。
	if (!ExperienceId.IsValid() && World->IsPlayInEditor())
	{
		//ExperienceId = GetDefault<UHodgeDeveloperSettings>()->ExperienceOverride;
		ExperienceIdSource = TEXT("DeveloperSettings");
	}

	// 从命令行参数中查找 Experience=XXX。
	if (!ExperienceId.IsValid())
	{
		FString ExperienceFromCommandLine;

		if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
		{
			// 优先按照 PrimaryAssetId 格式解析命令行参数。
			ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);

			// 如果没有指定 AssetType，则默认认为它是 HodgeExperienceDefinition 类型。
			if (!ExperienceId.PrimaryAssetType.IsValid())
			{
				ExperienceId = FPrimaryAssetId(
					FPrimaryAssetType(UHodgeExperienceDefinition::StaticClass()->GetFName()),
					FName(*ExperienceFromCommandLine));
			}

			ExperienceIdSource = TEXT("CommandLine");
		}
	}

	// 从 WorldSettings 中获取当前地图配置的默认 Experience。
	if (!ExperienceId.IsValid())
	{
		// if (AHodgeWorldSettings* TypedWorldSettings = Cast<AHodgeWorldSettings>(GetWorldSettings()))
		// {
		//     ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
		//     ExperienceIdSource = TEXT("WorldSettings");
		// }
	}

	// 获取 Hodge AssetManager，用于验证 Experience 是否存在。
	UHodgeAssetManager& AssetManager = UHodgeAssetManager::Get();

	FAssetData Dummy;

	// 如果指定的 Experience 不存在，则输出错误并回退到默认 Experience。
	if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	{
		UE_LOG(LogTemp, Error,
		       TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"),
		       *ExperienceId.ToString());

		ExperienceId = FPrimaryAssetId();
	}

	// 所有指定方式都没有得到有效 Experience 时，最终使用默认 Experience。
	if (!ExperienceId.IsValid())
	{
		// Dedicated Server 启动时可以在这里执行服务器注册或登录流程。
		if (TryDedicatedServerLogin())
		{
			// Dedicated Server 登录流程完成后再继续启动 Match。
			return;
		}

		// TODO：后续可以将默认 Experience 从配置文件或 Developer Settings 中读取。
		ExperienceId = FPrimaryAssetId(
			FPrimaryAssetType("HodgeExperienceDefinition"),
			FName("Exp_HodgeDefaultExperience"));

		ExperienceIdSource = TEXT("Default");
	}

	// 将最终确定的 Experience 分配给当前游戏。
	OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

bool AHodgeGameModeBase::TryDedicatedServerLogin()
{
	// 获取项目默认地图，用于判断 Dedicated Server 是否正在默认地图上启动。
	FString DefaultMap = UGameMapsSettings::GetGameDefaultMap();

	UWorld* World = GetWorld();
	UGameInstance* GameInstance = GetGameInstance();

	// 只有 Dedicated Server 且当前地图是默认地图时才执行 Dedicated Server 登录流程。
	if (GameInstance && World && World->GetNetMode() == NM_DedicatedServer && World->URL.Map == DefaultMap)
	{
		// Dedicated Server 的在线登录和 Session 注册逻辑目前暂未启用。
		// // Only register if this is the default map on a dedicated server
		// UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>();
		//
		// // Dedicated servers may need to do an online login
		// UserSubsystem->OnUserInitializeComplete.AddDynamic(
		//     this, &AHodgeGameModeBase::OnUserInitializedForDedicatedServer);
		//
		// // There are no local users on dedicated server, but index 0 means the default platform user which is handled by the online login code
		// if (!UserSubsystem->TryToLoginForOnlinePlay(0))
		// {
		//     OnUserInitializedForDedicatedServer(nullptr, false, FText(), ECommonUserPrivilege::CanPlayOnline,
		//                                         ECommonUserOnlineContext::Default);
		// }

		// 告诉调用者当前启动流程属于 Dedicated Server。
		return true;
	}

	return false;
}


// Dedicated Server 创建在线 Match 的完整流程，目前暂未启用。
// void AHodgeGameModeBase::HostDedicatedServerMatch(ECommonSessionOnlineMode OnlineMode)
// {
//  FPrimaryAssetType UserExperienceType = UHodgeUserFacingExperienceDefinition::StaticClass()->GetFName();
//
//  // 根据命令行参数查找需要加载的 UserFacingExperience。
//  FPrimaryAssetId UserExperienceId;
//  FString UserExperienceFromCommandLine;
//  if (FParse::Value(FCommandLine::Get(), TEXT("UserExperience="), UserExperienceFromCommandLine) ||
//     FParse::Value(FCommandLine::Get(), TEXT("Playlist="), UserExperienceFromCommandLine))
//  {
//     UserExperienceId = FPrimaryAssetId::ParseTypeAndName(UserExperienceFromCommandLine);
//     if (!UserExperienceId.PrimaryAssetType.IsValid())
//     {
//        UserExperienceId = FPrimaryAssetId(FPrimaryAssetType(UserExperienceType),
//                                           FName(*UserExperienceFromCommandLine));
//     }
//  }
//
//  // Dedicated Server 启动阶段可以直接同步加载 UserFacingExperience。
//  UHodgeAssetManager& AssetManager = UHodgeAssetManager::Get();
//  TSharedPtr<FStreamableHandle> Handle = AssetManager.LoadPrimaryAssetsWithType(UserExperienceType);
//  if (ensure(Handle.IsValid()))
//  {
//     Handle->WaitUntilComplete();
//  }
//
//  // 获取所有已经加载的 UserFacingExperience。
//  TArray<UObject*> UserExperiences;
//  AssetManager.GetPrimaryAssetObjectList(UserExperienceType, UserExperiences);
//  UHodgeUserFacingExperienceDefinition* FoundExperience = nullptr;
//  UHodgeUserFacingExperienceDefinition* DefaultExperience = nullptr;
//
//  // 查找命令行指定的 Experience，同时记录默认 Experience。
//  for (UObject* Object : UserExperiences)
//  {
//     UHodgeUserFacingExperienceDefinition* UserExperience = Cast<UHodgeUserFacingExperienceDefinition>(Object);
//     if (ensure(UserExperience))
//     {
//        if (UserExperience->GetPrimaryAssetId() == UserExperienceId)
//        {
//           FoundExperience = UserExperience;
//           break;
//        }
//
//        if (UserExperience->bIsDefaultExperience && DefaultExperience == nullptr)
//        {
//           DefaultExperience = UserExperience;
//        }
//     }
//  }
//
//  // 指定 Experience 不存在时使用默认 Experience。
//  if (FoundExperience == nullptr)
//  {
//     FoundExperience = DefaultExperience;
//  }
//
//  UGameInstance* GameInstance = GetGameInstance();
//  if (ensure(FoundExperience && GameInstance))
//  {
//     // 创建服务器 Session 请求。
//     UCommonSession_HostSessionRequest* HostRequest = FoundExperience->CreateHostingRequest(this);
//     if (ensure(HostRequest))
//     {
//        HostRequest->OnlineMode = OnlineMode;
//
//        // TODO：这里可以根据项目需求覆盖其他 Session 参数。
//
//        UCommonSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UCommonSessionSubsystem>();
//        SessionSubsystem->HostSession(nullptr, HostRequest);
//
//        // Session 创建完成后会负责地图切换。
//     }
//  }
// }


// Dedicated Server 用户初始化完成后的回调，目前暂未启用。
// void AHodgeGameModeBase::OnUserInitializedForDedicatedServer(const UCommonUserInfo* UserInfo, bool bSuccess,
//                                                              FText Error, ECommonUserPrivilege RequestedPrivilege,
//                                                              ECommonUserOnlineContext OnlineContext)
// {
//  UGameInstance* GameInstance = GetGameInstance();
//  if (GameInstance)
//  {
//     // 移除用户初始化回调绑定。
//     UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>();
//     UserSubsystem->OnUserInitializeComplete.RemoveDynamic(
//        this, &AHodgeGameModeBase::OnUserInitializedForDedicatedServer);
//
//     // Dedicated Server 通常不需要真实用户登录，但部分在线子系统可能要求执行登录流程。
//     if (bSuccess && ensure(UserInfo))
//     {
//        UE_LOG(LogTemp, Log,
//               TEXT("Dedicated server user login succeeded for id %s, starting online server"),
//               *UserInfo->GetNetId().ToString());
//     }
//     else
//     {
//        UE_LOG(LogTemp, Log,
//               TEXT("Dedicated server user login unsuccessful, starting online server as login is not required"));
//     }
//
//     // 用户初始化完成后正式创建在线 Match。
//     HostDedicatedServerMatch(ECommonSessionOnlineMode::Online);
//  }
// }


void AHodgeGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
	// Experience 有效时开始加载对应的游戏 Experience。
	if (ExperienceId.IsValid())
	{
		// 输出最终选择的 Experience 以及它的来源，方便调试游戏启动流程。
		UE_LOG(LogTemp, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(),
		       *ExperienceIdSource);

		// 从 GameState 获取 Experience 管理组件。
		UHodgeExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<
			UHodgeExperienceManagerComponent>();
		check(ExperienceComponent);

		// 将当前 Experience 设置给管理组件，由组件负责后续加载。
		ExperienceComponent->SetCurrentExperience(ExperienceId);
	}
	else
	{
		// Experience 无效时没有可启动的游戏内容，加载流程可能会一直等待。
		UE_LOG(LogTemp, Error, TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
}

void AHodgeGameModeBase::OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience)
{
	// Experience 加载完成后，为已经连接但尚未生成 Pawn 的玩家补充生成 Pawn。
	// 注意：这里当前只处理 PlayerController，Bot 的处理逻辑在其他位置。
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);

		// 只处理有效的玩家 Controller，并且只处理当前还没有 Pawn 的玩家。
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			// Experience 已经加载完成后，检查玩家是否允许重生。
			if (PlayerCanRestart(PC))
			{
				// 为玩家生成默认 Pawn。
				RestartPlayer(PC);
			}
		}
	}
}

bool AHodgeGameModeBase::IsExperienceLoaded() const
{
	// GameState 必须存在，因为 ExperienceManagerComponent 挂载在 GameState 上。
	check(GameState);

	// 从 GameState 获取 Experience 管理组件。
	UHodgeExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<
		UHodgeExperienceManagerComponent>();
	check(ExperienceComponent);

	// 返回当前 Experience 是否已经完成加载。
	return ExperienceComponent->IsExperienceLoaded();
}

UClass* AHodgeGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// 优先根据 Controller 对应的 PawnData 决定 Pawn 类型。
	if (const UHodgePawnData* PawnData = GetPawnDataForController(InController))
	{
		// PawnData 指定了 PawnClass 时使用指定的 Pawn 类型。
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	// 没有有效 PawnData 或 PawnClass 时使用 GameMode 默认 PawnClass。
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* AHodgeGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
                                                                      const FTransform& SpawnTransform)
{
	// 设置 Pawn 的生成参数。
	FActorSpawnParameters SpawnInfo;

	// 设置生成 Pawn 的 Instigator。
	SpawnInfo.Instigator = GetInstigator();

	// 默认玩家 Pawn 不需要保存到地图资源中，因此标记为临时对象。
	SpawnInfo.ObjectFlags |= RF_Transient;

	// 延迟 Actor 构造，允许在 FinishSpawning 前完成额外配置。
	SpawnInfo.bDeferConstruction = true;

	// 获取当前 Controller 对应的 PawnClass。
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		// 根据指定的 PawnClass 和出生 Transform 生成 Pawn。
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			// 可以在这里通过 PawnExtensionComponent 设置 PawnData，目前暂未启用。
			// if (UHodgePawnExtensionComponent* PawnExtComp = UHodgePawnExtensionComponent::FindPawnExtensionComponent(
			//     SpawnedPawn))
			// {
			//     if (const UHodgePawnData* PawnData = GetPawnDataForController(NewPlayer))
			//     {
			//        PawnExtComp->SetPawnData(PawnData);
			//     }
			//     else
			//     {
			//        UE_LOG(LogTemp, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."),
			//               *GetNameSafe(SpawnedPawn));
			//     }
			// }

			// 完成延迟构造，使 Pawn 正式完成初始化。
			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		else
		{
			// Pawn 生成失败时输出错误日志。
			UE_LOG(LogTemp, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."),
			       *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		// 没有找到有效 PawnClass 时输出错误日志。
		UE_LOG(LogTemp, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}

bool AHodgeGameModeBase::ShouldSpawnAtStartSpot(AController* Player)
{
	// 不使用 Controller 自带的 StartSpot，出生点由其他生成管理逻辑决定。
	return false;
}

void AHodgeGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Experience 加载完成后才允许正式处理新玩家，否则暂时等待 OnExperienceLoaded。
	if (IsExperienceLoaded())
	{
		// Experience 已加载完成，执行父类的新玩家初始化流程。
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

AActor* AHodgeGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	// 可以在这里接入 PlayerSpawningManagerComponent 统一管理玩家出生点，目前暂未启用。
	// if (UHodgePlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<
	//     UHodgePlayerSpawningManagerComponent>())
	// {
	//     return PlayerSpawningComponent->ChoosePlayerStart(Player);
	// }

	// 暂时使用父类的 PlayerStart 选择逻辑。
	return Super::ChoosePlayerStart_Implementation(Player);
}

void AHodgeGameModeBase::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	// 可以在这里交给 PlayerSpawningManagerComponent 完成最终重生逻辑，目前暂未启用。
	// if (UHodgePlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<
	//     UHodgePlayerSpawningManagerComponent>())
	// {
	//     PlayerSpawningComponent->FinishRestartPlayer(NewPlayer, StartRotation);
	// }

	// 暂时使用父类完成玩家重生。
	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

bool AHodgeGameModeBase::PlayerCanRestart_Implementation(APlayerController* Player)
{
	// 玩家 Controller 的重生检查统一交给 ControllerCanRestart，同时兼容 Bot。
	return ControllerCanRestart(Player);
}

bool AHodgeGameModeBase::ControllerCanRestart(AController* Controller)
{
	// 玩家 Controller 使用 AGameModeBase 原有的重生条件进行检查。
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (!Super::PlayerCanRestart_Implementation(PC))
		{
			return false;
		}
	}
	else
	{
		// Bot 没有 APlayerController，因此这里使用与父类类似的基础有效性检查。
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}

	// 可以在这里接入 PlayerSpawningManagerComponent 进一步判断是否允许重生，目前暂未启用。
	// if (UHodgePlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<
	//     UHodgePlayerSpawningManagerComponent>())
	// {
	//     return PlayerSpawningComponent->ControllerCanRestart(Controller);
	// }

	return true;
}

void AHodgeGameModeBase::InitGameState()
{
	// 先执行父类的 GameState 初始化流程。
	Super::InitGameState();

	// 获取 GameState 上的 Experience 管理组件。
	UHodgeExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<
		UHodgeExperienceManagerComponent>();
	check(ExperienceComponent);

	// 注册 Experience 加载完成回调，加载完成后执行 OnExperienceLoaded。
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(
		FOnHodgeExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void AHodgeGameModeBase::GenericPlayerInitialization(AController* NewPlayer)
{
	// 执行父类的通用玩家初始化逻辑。
	Super::GenericPlayerInitialization(NewPlayer);

	// 通知所有监听者当前玩家已经完成 GameMode 层面的初始化。
	OnGameModePlayerInitialized.Broadcast(this, NewPlayer);
}

void AHodgeGameModeBase::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	// 如果要求强制重置，则先放弃当前 Pawn 并重置 Controller。
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	// 玩家 Controller 在下一帧执行服务器重生请求。
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}

	// Bot Controller 的下一帧重生逻辑目前暂未启用。
	// else if (AHodgePlayerBotController* BotController = Cast<AHodgePlayerBotController>(Controller))
	// {
	//     GetWorldTimerManager().SetTimerForNextTick(BotController, &AHodgePlayerBotController::ServerRestartController);
	// }
}

bool AHodgeGameModeBase::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
	// 此阶段暂不确定最终出生点，等待 PostLogin 等后续流程完成玩家相关数据初始化后再真正生成玩家。
	return true;
}

void AHodgeGameModeBase::FailedToRestartPlayer(AController* NewPlayer)
{
	// 先执行父类的重生失败处理。
	Super::FailedToRestartPlayer(NewPlayer);

	// 重生失败后，如果当前仍然存在有效 PawnClass，则尝试在下一帧重新生成。
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		// 玩家 Controller 和 Bot Controller 分别处理重生失败逻辑。
		if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
		{
			// 玩家不会无条件无限重试，只有仍然允许重生时才继续尝试。
			if (PlayerCanRestart(NewPC))
			{
				RequestPlayerRestartNextFrame(NewPlayer, false);
			}
			else
			{
				// 玩家已经不允许重生，因此停止继续尝试。
				UE_LOG(LogTemp, Verbose,
				       TEXT(
					       "FailedToRestartPlayer(%s) and PlayerCanRestart returned false, so we're not going to try again."
				       ), *GetPathNameSafe(NewPlayer));
			}
		}
		else
		{
			// Bot 当前直接在下一帧重新尝试重生。
			RequestPlayerRestartNextFrame(NewPlayer, false);
		}
	}
	else
	{
		// 没有有效 PawnClass 时无法生成 Pawn，因此停止重试。
		UE_LOG(LogTemp, Verbose, TEXT("FailedToRestartPlayer(%s) but there's no pawn class so giving up."),
		       *GetPathNameSafe(NewPlayer));
	}
}
