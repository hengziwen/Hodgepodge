// 111屎山代码来袭

#pragma once

// UE 核心类型、基础宏与常用容器。
#include "CoreMinimal.h"

// 项目 PlayerController 基类。
#include "HodgePlayerControllerBase.h"

// 项目相机辅助接口，用于接收相机穿透角色等反馈。
#include "Camera/HodgeCameraAssistInterface.h"

#include "HodgePlayerController.generated.h"

// 项目自定义 AbilitySystemComponent 前置声明。
class UHodgeAbilitySystemComponent;

// 项目自定义 PlayerState 前置声明。
class AHodgePlayerState;

// 项目自定义 HUD 基类前置声明。
class AHodgeHUDBase;

/**
 * Hodge 主 PlayerController。
 *
 * 负责连接本地玩家输入、PlayerState、Pawn、ASC、HUD、相机以及客户端 Replay 等系统。
 * 同时实现 IHodgeCameraAssistInterface，用于接收相机辅助系统相关回调。
 */
UCLASS()
class HODGEPODGE_API AHodgePlayerController : public AHodgePlayerControllerBase, public IHodgeCameraAssistInterface
{
	GENERATED_BODY()

public:
	// 构造函数，允许通过 FObjectInitializer 初始化默认子对象。
	AHodgePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 获取当前 PlayerController 对应的 Hodge PlayerState。
	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
	AHodgePlayerState* GetHodgePlayerState() const;

	// 获取当前玩家对应的 Hodge AbilitySystemComponent。
	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const;

	// 获取当前 PlayerController 对应的 Hodge HUD。
	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
	AHodgeHUDBase* GetHodgeHUD() const;

	// Call from game state logic to start recording an automatic client replay if ShouldRecordClientReplay returns true
	// 由 GameState 等游戏逻辑调用；当 ShouldRecordClientReplay 返回 true 时尝试启动客户端自动 Replay 录制。
	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
	bool TryToRecordClientReplay();

	// Call to see if we should record a replay, subclasses could change this
	// 判断当前 PlayerController 是否应该录制客户端 Replay，子类可以重写策略。
	virtual bool ShouldRecordClientReplay();

	// Run a cheat command on the server.
	// 将作弊命令发送到服务器执行。
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheat(const FString& Msg);

	// Run a cheat command on the server for all players.
	// 将作弊命令发送到服务器，并面向所有玩家执行对应作弊逻辑。
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheatAll(const FString& Msg);

	//~AActor interface

	// Actor 组件初始化前调用，可用于完成 PlayerController 早期初始化。
	virtual void PreInitializeComponents() override;

	// Actor 正式进入游戏世界后调用。
	virtual void BeginPlay() override;

	// Actor 结束生命周期时调用。
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 注册需要通过网络复制的成员属性。
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~End of AActor interface

	//~AController interface

	// 成功 Possess 一个 Pawn 后调用。
	virtual void OnPossess(APawn* InPawn) override;

	// Controller 解除对当前 Pawn 的控制时调用。
	virtual void OnUnPossess() override;

	// 初始化当前 Controller 对应的 PlayerState。
	virtual void InitPlayerState() override;

	// 清理当前 Controller 对应的 PlayerState。
	virtual void CleanupPlayerState() override;

	// PlayerState 从服务器复制到客户端后触发。
	virtual void OnRep_PlayerState() override;

	//~End of AController interface

	//~APlayerController interface

	// 客户端完成 PlayerController 初始化并成功接收玩家后调用。
	virtual void ReceivedPlayer() override;

	// PlayerController 每帧更新。
	virtual void PlayerTick(float DeltaTime) override;

	// 设置当前 PlayerController 所属的 UPlayer。
	virtual void SetPlayer(UPlayer* InPlayer) override;

	// 为当前 PlayerController 创建并启用 CheatManager。
	virtual void AddCheats(bool bForce) override;

	// 更新当前控制器的力反馈输出。
	virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;

	// 根据当前视角位置决定需要为该 PlayerController 隐藏的场景组件。
	virtual void
	UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;

	// PlayerInput 正式处理当前帧输入之前调用。
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;

	// PlayerInput 完成当前帧输入处理之后调用。
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	//~End of APlayerController interface

	//~IHodgeCameraAssistInterface interface

	// 当相机发生穿透目标问题时由相机系统回调。
	virtual void OnCameraPenetratingTarget() override;

	//~End of IHodgeCameraAssistInterface interface

	// //~IHodgeTeamAgentInterface interface

	// 设置当前 Controller 所属队伍 ID。
	// virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	// 获取当前 Controller 所属队伍 ID。
	// virtual FGenericTeamId GetGenericTeamId() const override;

	// 获取队伍索引发生变化时使用的委托。
	// virtual FOnHodgeTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;

	// //~End of IHodgeTeamAgentInterface interface

	// 设置当前玩家是否处于自动奔跑状态。
	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
	void SetIsAutoRunning(const bool bEnabled);

	// 获取当前玩家是否处于自动奔跑状态。
	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
	bool GetIsAutoRunning() const;

private:
	// 保存 Controller 队伍变化事件的多播委托。
	//UPROPERTY()
	//FOnHodgeTeamIndexChangedDelegate OnTeamChangedDelegate;

	// 缓存最近一次观察到的 PlayerState，用于判断 PlayerState 是否发生替换。
	UPROPERTY()
	TObjectPtr<APlayerState> LastSeenPlayerState;

private:
	// PlayerState 所属队伍发生变化时的回调。
	UFUNCTION()
	void OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

protected:
	// Called when the player state is set or cleared
	// 当 PlayerState 被设置、替换或清空时调用，供子类扩展 PlayerState 切换逻辑。
	virtual void OnPlayerStateChanged();

private:
	// 检查并广播 PlayerState 发生变化的内部辅助函数。
	void BroadcastOnPlayerStateChanged();

protected:
	//~APlayerController interface

	//~End of APlayerController interface

	// 玩家设置发生变化时的回调预留。
	//void OnSettingsChanged(UHodgeSettingsShared* Settings);

	// 开始自动奔跑时调用。
	void OnStartAutoRun();

	// 结束自动奔跑时调用。
	void OnEndAutoRun();

	// 自动奔跑开始时通知蓝图。
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartAutoRun"))
	void K2_OnStartAutoRun();

	// 自动奔跑结束时通知蓝图。
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndAutoRun"))
	void K2_OnEndAutoRun();

	// 标记下一帧是否需要隐藏当前 ViewTarget 对应的 Pawn。
	bool bHideViewTargetPawnNextFrame = false;
};

// A player controller used for replay capture and playback
// 专门用于 Replay 录制和回放流程的 PlayerController。
UCLASS()
class AHodgeReplayPlayerController : public AHodgePlayerController
{
	GENERATED_BODY()

	// Replay PlayerController 每帧更新逻辑。
	virtual void Tick(float DeltaSeconds) override;

	// 平滑 Replay 中目标 Pawn 的观察旋转。
	virtual void SmoothTargetViewRotation(APawn* TargetPawn, float DeltaSeconds) override;

	// Replay PlayerController 自定义是否允许客户端录制 Replay。
	virtual bool ShouldRecordClientReplay() override;

	// Callback for when the game state's RecorderPlayerState gets replicated during replay playback
	// Replay 回放过程中，当 GameState 中记录者 PlayerState 完成复制时调用。
	void RecorderPlayerStateUpdated(APlayerState* NewRecorderPlayerState);

	// Callback for when the followed player state changes pawn
	// 当前正在跟随的 PlayerState 切换 Pawn 时触发。
	UFUNCTION()
	void OnPlayerStatePawnSet(APlayerState* ChangedPlayerState, APawn* NewPlayerPawn, APawn* OldPlayerPawn);

	// The player state we are currently following */
	// Replay 当前正在观察和跟随的 PlayerState。
	UPROPERTY(Transient)
	TObjectPtr<APlayerState> FollowedPlayerState;
};
