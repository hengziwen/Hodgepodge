/** @file HodgePlayerController.h
 * @brief 声明玩家控制器相关的类型、接口与现有处理流程。
 */
// 111屎山代码来袭

#pragma once

// UE 核心类型、基础宏与常用容器。
#include "CoreMinimal.h"

// 项目 PlayerController 基类。
#include "HodgePlayerControllerBase.h"

// 项目相机辅助接口，用于接收相机穿透角色等反馈。
#include "Camera/HodgeCameraAssistInterface.h"
#include "Feedback/NumberPops/HodgeNumberPopComponent.h"

#include "HodgePlayerController.generated.h"

// 项目自定义 AbilitySystemComponent 前置声明。
class UHodgeAbilitySystemComponent;

// 项目自定义 PlayerState 前置声明。
class AHodgePlayerState;

// 项目自定义 HUD 前置声明。
class AHodgeHUD;

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
	/** 构造玩家控制器并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	AHodgePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 获取当前 PlayerController 对应的 Hodge PlayerState。
	/** 获取玩家持久状态与ASC所有者。
	 * @return 玩家持久状态与ASC所有者的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
	AHodgePlayerState* GetHodgePlayerState() const;

	// 获取当前玩家对应的 Hodge AbilitySystemComponent。
	/** 获取能力系统组件。
	 * @return 能力系统组件的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const;

	// 获取当前 PlayerController 对应的 Hodge HUD。
	/** 获取HUD界面。
	 * @return HUD界面的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
	AHodgeHUD* GetHodgeHUD() const;

	// Call from game state logic to start recording an automatic client replay if ShouldRecordClientReplay returns true
	// 由 GameState 等游戏逻辑调用；当 ShouldRecordClientReplay 返回 true 时尝试启动客户端自动 Replay 录制。
	/** 尝试目标记录客户端回放。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerController")
	bool TryToRecordClientReplay();

	/** 开发控制台回满当前玩家的大招能量；客户端交给服务器执行，不清除冷却。 */
	UFUNCTION(Exec)
	void HodgeFillResonance();
	/** 开发控制台回满当前玩家暗流；不自动灭音、不改变Buff消耗或技能冷却。 */
	UFUNCTION(Exec)
	void HodgeFillDarkFlow();

	/** 仅服务器接受已结算伤害，向本 Controller 的拥有者发送纯表现数据。 */
	void SendDamageNumber(FHodgeNumberPopRequest Request);
	/** Controller 持有共享后端，Pawn 更换不影响仍在播放的数字。 */
	UFUNCTION(BlueprintPure, Category="Hodge|NumberPop")
	class UHodgeNumberPopComponent_NiagaraText* GetNumberPopComponent() const { return NumberPopComponent; }
	/** 诊断实际发送次数与最后一笔原始数值，不参与伤害结算。 */
	UFUNCTION(BlueprintPure, Category="Hodge|NumberPop")
	int32 GetDamageNumberSentCount() const { return DamageNumberSentCount; }
	UFUNCTION(BlueprintPure, Category="Hodge|NumberPop")
	FHodgeNumberPopRequest GetLastDamageNumberSent() const { return LastDamageNumberSent; }

	// Call to see if we should record a replay, subclasses could change this
	// 判断当前 PlayerController 是否应该录制客户端 Replay，子类可以重写策略。
	/** 判断是否应记录客户端回放。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool ShouldRecordClientReplay();

	// Run a cheat command on the server.
	// 将作弊命令发送到服务器执行。
	/** 处理服务器侧的Cheat消息，沿用当前身份与权限校验。
	 * @param Msg Msg，由调用方传入。
	 */
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheat(const FString& Msg);

	// Run a cheat command on the server for all players.
	// 将作弊命令发送到服务器，并面向所有玩家执行对应作弊逻辑。
	/** 处理服务器侧的CheatAll消息，沿用当前身份与权限校验。
	 * @param Msg Msg，由调用方传入。
	 */
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheatAll(const FString& Msg);

	//~AActor interface

	// Actor 组件初始化前调用，可用于完成 PlayerController 早期初始化。
	/** 在组件初始化前接入本对象所需的框架入口。 */
	virtual void PreInitializeComponents() override;

	// Actor 正式进入游戏世界后调用。
	/** 进入世界运行阶段并接入本对象的生命周期依赖。 */
	virtual void BeginPlay() override;

	// Actor 结束生命周期时调用。
	/** 离开世界运行阶段并解除本对象持有的监听或资源。
	 * @param EndPlayReason 结束PlayReason，由调用方传入。
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 注册需要通过网络复制的成员属性。
	/** 登记本类型需要网络复制的属性及复制条件。
	 * @param OutLifetimeProps 输出LifetimeProps。
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~End of AActor interface

	//~AController interface

	// 成功 Possess 一个 Pawn 后调用。
	/** 开始新的控制器Possess周期并接入被控制Pawn。
	 * @param InPawn 当前操作关联的Pawn角色。
	 */
	virtual void OnPossess(APawn* InPawn) override;

	// Controller 解除对当前 Pawn 的控制时调用。
	/** 结束当前Possess周期并解除旧Pawn依赖。 */
	virtual void OnUnPossess() override;

	// 初始化当前 Controller 对应的 PlayerState。
	/** 初始化玩家持久状态与ASC所有者。 */
	virtual void InitPlayerState() override;

	// 清理当前 Controller 对应的 PlayerState。
	/** 解除旧玩家状态关联并执行现有清理。 */
	virtual void CleanupPlayerState() override;

	// PlayerState 从服务器复制到客户端后触发。
	/** 收到PlayerState复制后刷新角色绑定。 */
	virtual void OnRep_PlayerState() override;

	//~End of AController interface

	//~APlayerController interface

	// 客户端完成 PlayerController 初始化并成功接收玩家后调用。
	/** 在控制器取得玩家连接后更新本地接入。 */
	virtual void ReceivedPlayer() override;

	// PlayerController 每帧更新。
	/** 在玩家控制器更新阶段处理其当前状态。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	virtual void PlayerTick(float DeltaTime) override;

	// 设置当前 PlayerController 所属的 UPlayer。
	/** 设置玩家。
	 * @param InPlayer 玩家，由调用方传入。
	 */
	virtual void SetPlayer(UPlayer* InPlayer) override;

	// 为当前 PlayerController 创建并启用 CheatManager。
	/** 添加Cheats。
	 * @param bForce 是否Force的控制标记。
	 */
	virtual void AddCheats(bool bForce) override;

	// 更新当前控制器的力反馈输出。
	/** 更新ForceFeedback。
	 * @param InputInterface 输入接口，由调用方传入。
	 * @param ControllerId 控制器标识，由调用方传入。
	 */
	virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;

	// 根据当前视角位置决定需要为该 PlayerController 隐藏的场景组件。
	/** 更新HiddenComponents。
	 * @param ViewLocation 视图位置，由调用方传入。
	 * @param OutHiddenComponents 输出HiddenComponents。
	 */
	virtual void
	UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;

	// PlayerInput 正式处理当前帧输入之前调用。
	/** 准备处理输入。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 * @param bGamePaused 是否游戏Paused的控制标记。
	 */
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;

	// PlayerInput 完成当前帧输入处理之后调用。
	/** 在引擎输入处理之后推进当前能力输入。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 * @param bGamePaused 是否游戏Paused的控制标记。
	 */
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	//~End of APlayerController interface

	//~IHodgeCameraAssistInterface interface

	// 当相机发生穿透目标问题时由相机系统回调。
	/** 处理相机Penetrating目标。 */
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
	/** 设置状态判断自动Running。
	 * @param bEnabled 是否启用的控制标记。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
	void SetIsAutoRunning(const bool bEnabled);

	// 获取当前玩家是否处于自动奔跑状态。
	/** 获取状态判断自动Running。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
	bool GetIsAutoRunning() const;

private:
	/** Unreliable 允许丢弃纯表现事件；不在其他玩家广播，也不回写玩法状态。 */
	UFUNCTION(Client, Unreliable)
	void ClientShowDamageNumbers(const TArray<FHodgeNumberPopRequest>& Requests);
	/** 合并同帧传输而非合并伤害，降低多目标瞬时 RPC 数量。 */
	void FlushDamageNumbers();
	// 专服只有无 Tick 的门面，Niagara 和样式仅在本地客户端创建。
	UPROPERTY(VisibleAnywhere, Category="Hodge|NumberPop") TObjectPtr<class UHodgeNumberPopComponent_NiagaraText> NumberPopComponent;
	UPROPERTY(Transient) FHodgeNumberPopRequest LastDamageNumberSent;
	int32 DamageNumberSentCount = 0;
	int32 NextDamageNumberEventId = 0;
	TArray<FHodgeNumberPopRequest> PendingDamageNumbers;
	FTimerHandle DamageNumberFlushTimer;

	// Controller 持有本地投影描述；专服只保留不复制、无 Tick 的空管理组件。
	UPROPERTY(VisibleAnywhere, Category="Hodge|UI") TObjectPtr<class UHodgeIndicatorManagerComponent> IndicatorManager;
	// 保存 Controller 队伍变化事件的多播委托。
	//UPROPERTY()
	//FOnHodgeTeamIndexChangedDelegate OnTeamChangedDelegate;

	// 缓存最近一次观察到的 PlayerState，用于判断 PlayerState 是否发生替换。
	UPROPERTY()
	TObjectPtr<APlayerState> LastSeenPlayerState;

private:
	// PlayerState 所属队伍发生变化时的回调。
	/** 处理玩家状态变化队伍。
	 * @param TeamAgent 队伍Agent，由调用方传入。
	 * @param OldTeam 旧队伍，由调用方传入。
	 * @param NewTeam 新队伍，由调用方传入。
	 */
	UFUNCTION()
	void OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

protected:
	// Called when the player state is set or cleared
	// 当 PlayerState 被设置、替换或清空时调用，供子类扩展 PlayerState 切换逻辑。
	/** 处理玩家状态变化。 */
	virtual void OnPlayerStateChanged();

private:
	// 检查并广播 PlayerState 发生变化的内部辅助函数。
	/** 广播事件玩家状态变化。 */
	void BroadcastOnPlayerStateChanged();

protected:
	//~APlayerController interface

	//~End of APlayerController interface

	// 玩家设置发生变化时的回调预留。
	//void OnSettingsChanged(UHodgeSettingsShared* Settings);

	// 开始自动奔跑时调用。
	/** 处理起始自动奔跑。 */
	void OnStartAutoRun();

	// 结束自动奔跑时调用。
	/** 处理结束自动奔跑。 */
	void OnEndAutoRun();

	// 自动奔跑开始时通知蓝图。
	/** 触发蓝图起始自动奔跑。 */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartAutoRun"))
	void K2_OnStartAutoRun();

	// 自动奔跑结束时通知蓝图。
	/** 触发蓝图结束自动奔跑。 */
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
	/** 推进当前对象的随帧更新。
	 * @param DeltaSeconds 本次更新的时间步长，单位秒。
	 */
	virtual void Tick(float DeltaSeconds) override;

	// 平滑 Replay 中目标 Pawn 的观察旋转。
	/** 平滑远端玩家复制到达的视角旋转。
	 * @param TargetPawn 目标Pawn角色，由调用方传入。
	 * @param DeltaSeconds 本次更新的时间步长，单位秒。
	 */
	virtual void SmoothTargetViewRotation(APawn* TargetPawn, float DeltaSeconds) override;

	// Replay PlayerController 自定义是否允许客户端录制 Replay。
	/** 判断是否应记录客户端回放。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool ShouldRecordClientReplay() override;

	// Callback for when the game state's RecorderPlayerState gets replicated during replay playback
	// Replay 回放过程中，当 GameState 中记录者 PlayerState 完成复制时调用。
	/** 在记录器所用玩家状态变化后刷新其关联。
	 * @param NewRecorderPlayerState 新Recorder玩家状态，由调用方传入。
	 */
	void RecorderPlayerStateUpdated(APlayerState* NewRecorderPlayerState);

	// Callback for when the followed player state changes pawn
	// 当前正在跟随的 PlayerState 切换 Pawn 时触发。
	/** 处理玩家状态Pawn角色设置。
	 * @param ChangedPlayerState 变化玩家状态，由调用方传入。
	 * @param NewPlayerPawn 新玩家Pawn角色，由调用方传入。
	 * @param OldPlayerPawn 旧玩家Pawn角色，由调用方传入。
	 */
	UFUNCTION()
	void OnPlayerStatePawnSet(APlayerState* ChangedPlayerState, APawn* NewPlayerPawn, APawn* OldPlayerPawn);

	// The player state we are currently following */
	// Replay 当前正在观察和跟随的 PlayerState。
	UPROPERTY(Transient)
	TObjectPtr<APlayerState> FollowedPlayerState;
};
