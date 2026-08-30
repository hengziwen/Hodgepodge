// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "HodgePlayerStateBase.h"
#include "AbilitySystem/GameplayTagStack.h"
#include "HodgePlayerState.generated.h"

class UHodgeExperienceDefinition;
class UHodgePawnData;
class UHodgeAbilitySystemComponentBase;
class AHodgePlayerControllerBase;

/**
 * @brief 玩家当前的连接状态。
 */
UENUM()
enum class EHodgePlayerConnectionType : uint8
{
	// 正常连接并参与游戏的玩家。
	Player = 0,

	// 已连接到正在进行游戏的实时观众。
	LiveSpectator,

	// 离线观看回放的观众。
	ReplaySpectator,

	// 已断开连接但 PlayerState 暂时保留的玩家。
	InactivePlayer
};

/**
 * @brief Hodgepodge 项目的玩家状态类。
 *
 * 继承 AHodgePlayerStateBase，负责保存玩家在游戏中的持久状态，
 * 同时作为玩家 GAS、PawnData、队伍、小队和统计数据等信息的载体。
 */
UCLASS(Config = Game)
class AHodgePlayerState : public AHodgePlayerStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// 构造玩家状态并创建玩家专属的 Gameplay 组件。
	AHodgePlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 获取该 PlayerState 对应的 HodgePlayerController。
	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerState")
	AHodgePlayerControllerBase* GetHodgePlayerController() const;

	// 获取玩家专属的 AbilitySystemComponent。
	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerState")
	UHodgeAbilitySystemComponentBase* GetHodgeAbilitySystemComponent() const { return AbilitySystemComponent; }

	// 实现 IAbilitySystemInterface，返回玩家的 AbilitySystemComponent。
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 获取玩家当前配置的 PawnData，并转换成指定类型。
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	// 设置玩家使用的 PawnData。
	void SetPawnData(const UHodgePawnData* InPawnData);

	//~AActor interface

	// 注册并初始化 PlayerState 上的模块化组件。
	virtual void PreInitializeComponents() override;

	// 初始化 PlayerState 上的 Gameplay 组件和 GAS。
	virtual void PostInitializeComponents() override;

	//~End of AActor interface

	//~APlayerState interface

	// 重置玩家状态，同时重置相关 PlayerStateComponent。
	virtual void Reset() override;

	// 客户端初始化 PlayerState 时调用，用于完成客户端侧初始化。
	virtual void ClientInitialize(AController* C) override;

	// 在玩家状态迁移或复制时，将当前 PlayerState 的属性复制到目标 PlayerState。
	virtual void CopyProperties(APlayerState* PlayerState) override;

	// 玩家断开连接时调用，将玩家状态标记为非活跃。
	virtual void OnDeactivated() override;

	// 玩家重新连接时调用，将玩家状态恢复为活跃状态。
	virtual void OnReactivated() override;

	//~End of APlayerState interface

	// //~IHodgeTeamAgentInterface interface

	// // 设置玩家所属的队伍 ID。
	// virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	// // 获取玩家所属的队伍 ID。
	// virtual FGenericTeamId GetGenericTeamId() const override;

	// // 获取队伍 ID 发生变化时触发的委托。
	// virtual FOnHodgeTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;

	// //~End of IHodgeTeamAgentInterface interface

	// 玩家 AbilitySystemComponent 初始化完成后使用的 GameplayTag。
	static const FName NAME_HodgeAbilityReady;

	// 设置玩家当前的连接状态。
	void SetPlayerConnectionType(EHodgePlayerConnectionType NewType);

	// 获取玩家当前的连接状态。
	EHodgePlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }

	// 获取玩家当前所属的小队 ID。
	UFUNCTION(BlueprintCallable)
	int32 GetSquadId() const
	{
		return MySquadID;
	}

	// /** Returns the Team ID of the team the player belongs to. */

	// // 获取玩家当前所属的队伍 ID。
	// UFUNCTION(BlueprintCallable)
	// int32 GetTeamId() const
	// {
	//     return GenericTeamIdToInteger(MyTeamID);
	// }

	// 设置玩家所属的小队 ID。
	void SetSquadID(int32 NewSquadID);

	// 为玩家添加指定数量的统计 Tag 堆叠。
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// 移除玩家指定数量的统计 Tag 堆叠。
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// 获取玩家指定统计 Tag 当前的堆叠数量。
	UFUNCTION(BlueprintCallable, Category=Teams)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// 判断玩家是否拥有指定统计 Tag。
	UFUNCTION(BlueprintCallable, Category=Teams)
	bool HasStatTag(FGameplayTag Tag) const;

	// // Send a message to just this player
	// // (use only for client notifications like accolades, quest toasts, etc... that can handle being occasionally lost)
	// UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "Hodge|PlayerState")
	// void ClientBroadcastMessage(const FHodgeVerbMessage Message);

	// 获取复制到客户端的玩家视角旋转，用于观战系统。
	FRotator GetReplicatedViewRotation() const;

	// 设置玩家复制到客户端的视角旋转，只允许服务器调用。
	void SetReplicatedViewRotation(const FRotator& NewRotation);

private:
	// Experience 加载完成后的回调，用于根据当前 Experience 初始化玩家状态。
	void OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience);

protected:
	// PawnData 复制到客户端后触发的回调。
	UFUNCTION()
	void OnRep_PawnData();

	// 玩家当前使用的 PawnData，决定 Pawn 的类型和相关配置。
	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UHodgePawnData> PawnData;

private:
	// 玩家专属的 AbilitySystemComponent，负责玩家技能、GameplayEffect、GameplayCue 等 GAS 功能。
	UPROPERTY(VisibleAnywhere, Category = "Hodge|PlayerState")
	TObjectPtr<UHodgeAbilitySystemComponentBase> AbilitySystemComponent;

	// 玩家生命值相关的 AttributeSet。
	UPROPERTY()
	TObjectPtr<const class UHodgeHealthSet> HealthSet;

	// 玩家当前的连接状态，并同步到客户端。
	UPROPERTY(Replicated)
	EHodgePlayerConnectionType MyPlayerConnectionType;

	//UPROPERTY()
	//FOnHodgeTeamIndexChangedDelegate OnTeamChangedDelegate;

	// 玩家所属的队伍 ID，发生复制变化时触发 OnRep_MyTeamID。
	UPROPERTY(ReplicatedUsing=OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

	// 玩家所属的小队 ID，发生复制变化时触发 OnRep_MySquadID。
	UPROPERTY(ReplicatedUsing=OnRep_MySquadID)
	int32 MySquadID;

	// 玩家统计数据使用的 GameplayTag 堆栈容器，并通过 FastArray 进行网络同步。
	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;

	// 玩家当前视角旋转，并复制给客户端用于观战。
	UPROPERTY(Replicated)
	FRotator ReplicatedViewRotation;

private:
	// MyTeamID 复制到客户端后触发的回调。
	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);

	// MySquadID 复制到客户端后触发的回调。
	UFUNCTION()
	void OnRep_MySquadID();
};
