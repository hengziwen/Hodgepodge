// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "HodgeGameStateBase.h"
#include "HodgeGameState.generated.h"

class UHodgeExperienceManagerComponent;
class UHodgeAbilitySystemComponentBase;

/**
 * @brief Hodgepodge 框架的游戏状态类
 *
 * 在 AHodgeGameStateBase 的基础上扩展游戏级 Gameplay Ability System、Experience 管理、服务器 FPS 监控、玩家状态管理和回放录制者追踪等功能。
 */
UCLASS()
class HODGEPODGE_API AHodgeGameState : public AHodgeGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// 构造函数，用于创建 GameState 及其默认子对象。
	AHodgeGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	// GameState 的组件初始化前调用，用于注册和准备框架组件。
	virtual void PreInitializeComponents() override;

	// GameState 的组件初始化完成后调用，用于执行依赖组件完成初始化的逻辑。
	virtual void PostInitializeComponents() override;

	// GameState 结束运行时调用，用于清理组件和解除相关资源。
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// GameState Tick，每帧执行，用于服务器 FPS 等需要持续更新的逻辑。
	virtual void Tick(float DeltaSeconds) override;
	//~End of AActor interface

	//~AGameStateBase interface
	// 当新的 PlayerState 加入游戏时调用，用于维护 GameState 的玩家列表及相关逻辑。
	virtual void AddPlayerState(APlayerState* PlayerState) override;

	// 当 PlayerState 离开游戏时调用，用于从 GameState 中移除玩家并处理相关逻辑。
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	// 无缝地图切换进入检查点时调用，用于清理 Bot 和不活跃的 PlayerState 等无效玩家状态。
	virtual void SeamlessTravelTransitionCheckpoint(bool bToTransitionMap) override;
	//~End of AGameStateBase interface

	//~IAbilitySystemInterface
	// 返回当前 GameState 使用的 AbilitySystemComponent。
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

	// 获取用于处理全局 Gameplay 能力和 Gameplay Cue 等功能的 AbilitySystemComponent。
	UFUNCTION(BlueprintCallable, Category = "Hodge|GameState")
	UHodgeAbilitySystemComponentBase* GetHodgeAbilitySystemComponent() const { return AbilitySystemComponent; }

	// 向所有客户端发送可能丢失的消息，适合击杀提示、玩家加入等不要求可靠到达的通知。
	// UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "Hodge|GameState")
	// void MulticastMessageToClients(const FHodgeVerbMessage Message);

	// 向所有客户端可靠发送消息，适合不能丢失的重要客户端通知。
	// UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Hodge|GameState")
	// void MulticastReliableMessageToClients(const FHodgeVerbMessage Message);

	// 获取服务器当前 FPS，客户端通过复制后的值获取服务器帧率。
	float GetServerFPS() const;

	// 设置触发当前回放录制的 PlayerState。
	void SetRecorderPlayerState(APlayerState* NewPlayerState);

	// 获取触发当前回放录制的 PlayerState。
	APlayerState* GetRecorderPlayerState() const;

	// 回放录制者 PlayerState 发生变化时触发的委托。
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnRecorderPlayerStateChanged, APlayerState*);
	FOnRecorderPlayerStateChanged OnRecorderPlayerStateChangedEvent;

private:
	// Experience 管理组件，负责加载、管理和切换当前游戏 Experience。
	UPROPERTY()
	TObjectPtr<UHodgeExperienceManagerComponent> ExperienceManagerComponent;

	// GameState 使用的游戏级 AbilitySystemComponent，主要用于处理全局 Gameplay Cue 等能力系统功能。
	UPROPERTY(VisibleAnywhere, Category = "Hodge|GameState")
	TObjectPtr<UHodgeAbilitySystemComponentBase> AbilitySystemComponent;

protected:
	// 服务器当前 FPS，服务器计算后复制给客户端。
	UPROPERTY(Replicated)
	float ServerFPS;

	// 触发回放录制的 PlayerState，用于回放播放时确定需要跟随的玩家 Pawn。
	// 该属性仅用于回放数据流，在正常游戏中不会进行普通网络复制。
	UPROPERTY(Transient, ReplicatedUsing = OnRep_RecorderPlayerState)
	TObjectPtr<APlayerState> RecorderPlayerState;

	// RecorderPlayerState 在客户端完成复制后调用，用于通知相关系统录制者发生变化。
	UFUNCTION()
	void OnRep_RecorderPlayerState();
};
