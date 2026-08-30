// 111屎山代码来袭

#include "Core/GameState/HodgeGameState.h"

// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/HodgeAbilitySystemComponentBase.h"
#include "Async/TaskGraphInterfaces.h"
#include "Component/HodgeExperienceManagerComponent.h"
#include "Core/PlayState/HodgePlayerStateBase.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameState)

// 前置声明 PlayerState，避免在当前文件的声明阶段引入完整类型。
class APlayerState;

// 前置声明网络复制属性结构体。
class FLifetimeProperty;

// 引用 UE 引擎提供的全局平均 FPS 数据。
extern ENGINE_API float GAverageFPS;


AHodgeGameState::AHodgeGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// GameState 开启 Tick，用于服务器端持续更新 FPS 等需要逐帧执行的逻辑。
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// 创建 GameState 级别的 AbilitySystemComponent，用于处理游戏全局的 GAS 功能。
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UHodgeAbilitySystemComponentBase>(
		this, TEXT("AbilitySystemComponent"));

	// 开启 ASC 的网络复制，使其能够同步到客户端。
	AbilitySystemComponent->SetIsReplicated(true);

	// 使用 Mixed 复制模式，平衡 GameplayEffect 的网络同步范围和网络开销。
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// 创建 Experience 管理组件，用于管理当前游戏 Experience 相关的内容。
	ExperienceManagerComponent = CreateDefaultSubobject<UHodgeExperienceManagerComponent>(
		TEXT("ExperienceManagerComponent"));

	// 初始化服务器 FPS，避免在第一次服务器 Tick 前读取到未初始化数据。
	ServerFPS = 0.0f;
}

void AHodgeGameState::PreInitializeComponents()
{
	// 先执行父类初始化，保证 Modular GameState 的组件接收者注册流程正常执行。
	Super::PreInitializeComponents();
}

void AHodgeGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 确保 GameState 的 ASC 已经成功创建。
	check(AbilitySystemComponent);

	// 初始化 ASC 的 ActorInfo，将 GameState 同时作为 Owner 和 Avatar。
	AbilitySystemComponent->InitAbilityActorInfo(/*Owner=*/ this, /*Avatar=*/ this);
}

UAbilitySystemComponent* AHodgeGameState::GetAbilitySystemComponent() const
{
	// 返回 GameState 持有的全局 AbilitySystemComponent。
	return AbilitySystemComponent;
}

void AHodgeGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 执行父类的结束运行逻辑。
	Super::EndPlay(EndPlayReason);
}

void AHodgeGameState::AddPlayerState(APlayerState* PlayerState)
{
	// 将新加入的 PlayerState 交给父类维护 PlayerArray。
	Super::AddPlayerState(PlayerState);
}

void AHodgeGameState::RemovePlayerState(APlayerState* PlayerState)
{
	// TODO：当前该函数似乎不会正常触发，因为 AGameModeBase 不会像传统 AGameMode 一样调用它。
	// TODO：后续可以进一步检查引擎相关代码，并考虑调整 PlayerState 的移除逻辑。
	Super::RemovePlayerState(PlayerState);
}

void AHodgeGameState::SeamlessTravelTransitionCheckpoint(bool bToTransitionMap)
{
	// 无缝地图切换过程中清理不再有效的玩家状态，包括 Bot 和不活跃玩家。
	for (int32 i = PlayerArray.Num() - 1; i >= 0; i--)
	{
		APlayerState* PlayerState = PlayerArray[i];

		// 从 PlayerArray 末尾向前遍历，避免删除数组元素后影响尚未遍历的元素。
		if (PlayerState && (PlayerState->IsABot() || PlayerState->IsInactive()))
		{
			// 移除 Bot 或已经处于不活跃状态的 PlayerState。
			RemovePlayerState(PlayerState);
		}
	}
}

void AHodgeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 先注册父类已有的网络复制属性。
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 将服务器 FPS 设置为普通网络属性，使服务器计算结果同步给客户端。
	DOREPLIFETIME(ThisClass, ServerFPS);

	// 仅在 Replay 数据流中复制 RecorderPlayerState，正常游戏网络复制不会同步该属性。
	DOREPLIFETIME_CONDITION(ThisClass, RecorderPlayerState, COND_ReplayOnly);
}

void AHodgeGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 只有服务器负责计算和更新 ServerFPS，客户端只接收复制后的结果。
	if (GetLocalRole() == ROLE_Authority)
	{
		// 使用 UE 引擎提供的平均 FPS 更新服务器当前帧率。
		ServerFPS = GAverageFPS;
	}
}

// 向所有客户端发送不保证可靠到达的消息，适合可以容忍丢失的客户端通知。
// void AHodgeGameState::MulticastMessageToClients_Implementation(const FHodgeVerbMessage Message)
// {
//  if (GetNetMode() == NM_Client)
//  {
//     UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
//  }
// }

// 向所有客户端可靠发送消息，适合不能丢失的重要客户端通知。
// void AHodgeGameState::MulticastReliableMessageToClients_Implementation(const FHodgeVerbMessage Message)
// {
//  MulticastMessageToClients_Implementation(Message);
// }

float AHodgeGameState::GetServerFPS() const
{
	// 返回当前 GameState 保存的服务器 FPS。
	return ServerFPS;
}

void AHodgeGameState::SetRecorderPlayerState(APlayerState* NewPlayerState)
{
	// RecorderPlayerState 只允许在当前游戏中设置一次。
	if (RecorderPlayerState == nullptr)
	{
		// 保存触发回放录制的 PlayerState，并主动调用 OnRep 函数执行相关初始化逻辑。
		RecorderPlayerState = NewPlayerState;
		OnRep_RecorderPlayerState();
	}
	else
	{
		// 如果已经设置过录制者，则输出警告日志，避免重复修改录制者。
		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "SetRecorderPlayerState was called on %s but should only be called once per game on the primary user"
		       ), *GetName());
	}
}

APlayerState* AHodgeGameState::GetRecorderPlayerState() const
{
	// TODO：如果 RecorderPlayerState 为空，后续可以考虑自动选择一个合适的 PlayerState。
	return RecorderPlayerState;
}

void AHodgeGameState::OnRep_RecorderPlayerState()
{
	// RecorderPlayerState 发生变化时通知所有监听者，例如 UI 或回放相关系统。
	OnRecorderPlayerStateChangedEvent.Broadcast(RecorderPlayerState);
}
