/** @file HodgePlayerState.h
 * @brief 声明玩家持久状态与ASC所有者相关的类型、接口与现有处理流程。
 */
// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "HodgePlayerStateBase.h"
#include "AbilitySystem/GameplayTagStack.h"
#include "AbilitySystem/Stats/HodgeAttributeTypes.h"
#include "Data/HodgeAbilitySet.h"
#include "HodgePlayerState.generated.h"

class UHodgeExperienceDefinition;
class UHodgePawnData;
class UHodgeAbilitySystemComponent;
class AHodgePlayerController;
class UHodgeAttributeCoordinator;

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
	/** 构造玩家持久状态与ASC所有者并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	AHodgePlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 获取该 PlayerState 对应的 HodgePlayerController。
	/** 获取玩家控制器。
	 * @return 玩家控制器的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerState")
	AHodgePlayerController* GetHodgePlayerController() const;

	// 获取玩家专属的 AbilitySystemComponent。
	/** 获取能力系统组件。
	 * @return 能力系统组件的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerState")
	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const { return AbilitySystemComponent; }
	/** 获取角色等级。
	 * @return 角色等级的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Attributes") int32 GetCharacterLevel() const { return CharacterProgression.Level; }
	/** 设置角色等级。
	 * @param Level 配置或运行实例的等级。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Attributes") bool SetCharacterLevel(int32 Level);
	/** 恢复角色生命值。
	 * @param Health 当前生命值。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Attributes") bool RestoreCharacterHealth(float Health);
	/** 初始化角色成长。
	 * @param CharacterId 角色标识，由调用方传入。
	 * @param Level 配置或运行实例的等级。
	 * @param SavedHealth Saved生命值，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Attributes") bool InitializeCharacterProgression(FGuid CharacterId, int32 Level, float SavedHealth = -1.f);
	/** 获取属性就绪状态。
	 * @return 属性就绪状态的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Attributes") FHodgeAttributeReadyState GetAttributeReadyState() const { return AttributeReadyState; }
	/** 获取PlayerState属性协调器。
	 * @return PlayerState属性协调器的当前值或引用。
	 */
	UHodgeAttributeCoordinator* GetAttributeCoordinator() const { return AttributeCoordinator; }
	/** 检查属性是否已为指定Avatar完成准备。
	 * @param Avatar 表现角色，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool AreAttributesReadyFor(const APawn* Avatar) const;
	/** 通知属性就绪。 */
	void NotifyAttributeReadiness();
	// 同一就绪入口通知本地订阅者，玩法代码不引用具体 UI。
	FSimpleMulticastDelegate OnAttributeReadinessChanged;

	// 实现 IAbilitySystemInterface，返回玩家的 AbilitySystemComponent。
	/** 获取能力系统组件。
	 * @return 能力系统组件的当前值或引用。
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 获取玩家当前配置的 PawnData，并转换成指定类型。
	template <class T>
	/** 获取Pawn能力/输入/相机配置。
	 * @return Pawn能力/输入/相机配置的当前值或引用。
	 */
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	// 设置玩家使用的 PawnData。
	/** 设置Pawn能力/输入/相机配置。
	 * @param InPawnData 当前角色使用的PawnData配置。
	 */
	void SetPawnData(const UHodgePawnData* InPawnData);

	//~AActor interface

	// 注册并初始化 PlayerState 上的模块化组件。
	/** 在组件初始化前接入本对象所需的框架入口。 */
	virtual void PreInitializeComponents() override;

	// 初始化 PlayerState 上的 Gameplay 组件和 GAS。
	/** 在组件初始化后建立本对象的运行依赖。 */
	virtual void PostInitializeComponents() override;
	/** 结束持久 Owner 生命周期时，只撤销基础来源自己的句柄。 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//~End of AActor interface

	//~APlayerState interface

	// 重置玩家状态，同时重置相关 PlayerStateComponent。
	/** 恢复本对象的初始状态。 */
	virtual void Reset() override;

	// 客户端初始化 PlayerState 时调用，用于完成客户端侧初始化。
	/** 处理客户端侧的初始化消息，沿用当前身份与权限校验。
	 * @param C C，由调用方传入。
	 */
	virtual void ClientInitialize(AController* C) override;

	// 在玩家状态迁移或复制时，将当前 PlayerState 的属性复制到目标 PlayerState。
	/** 按当前类型的生命周期迁移需要保留的属性。
	 * @param PlayerState 玩家持有的持久状态与ASC入口。
	 */
	virtual void CopyProperties(APlayerState* PlayerState) override;

	// 玩家断开连接时调用，将玩家状态标记为非活跃。
	/** 处理Deactivated。 */
	virtual void OnDeactivated() override;

	// 玩家重新连接时调用，将玩家状态恢复为活跃状态。
	/** 处理Reactivated。 */
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
	/** 设置玩家Connection类型。
	 * @param NewType 新类型，由调用方传入。
	 */
	void SetPlayerConnectionType(EHodgePlayerConnectionType NewType);

	// 获取玩家当前的连接状态。
	/** 获取玩家Connection类型。
	 * @return 玩家Connection类型的当前值或引用。
	 */
	EHodgePlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }

	// 获取玩家当前所属的小队 ID。
	/** 获取Squad标识。
	 * @return Squad标识的当前值或引用。
	 */
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
	/** 设置SquadID。
	 * @param NewSquadID 新SquadID，由调用方传入。
	 */
	void SetSquadID(int32 NewSquadID);

	// 为玩家添加指定数量的统计 Tag 堆叠。
	/** 添加数值标签堆叠。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @param StackCount 标签或条目的堆叠数量。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// 移除玩家指定数量的统计 Tag 堆叠。
	/** 移除数值标签堆叠。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @param StackCount 标签或条目的堆叠数量。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// 获取玩家指定统计 Tag 当前的堆叠数量。
	/** 获取数值标签堆叠数量。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @return 数值标签堆叠数量的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category=Teams)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// 判断玩家是否拥有指定统计 Tag。
	/** 判断是否存在数值标签。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintCallable, Category=Teams)
	bool HasStatTag(FGameplayTag Tag) const;

	// // Send a message to just this player
	// // (use only for client notifications like accolades, quest toasts, etc... that can handle being occasionally lost)
	// UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "Hodge|PlayerState")
	// void ClientBroadcastMessage(const FHodgeVerbMessage Message);

	// 获取复制到客户端的玩家视角旋转，用于观战系统。
	/** 获取复制视图旋转。
	 * @return 复制视图旋转的当前值或引用。
	 */
	FRotator GetReplicatedViewRotation() const;

	// 设置玩家复制到客户端的视角旋转，只允许服务器调用。
	/** 设置复制视图旋转。
	 * @param NewRotation 新旋转，由调用方传入。
	 */
	void SetReplicatedViewRotation(const FRotator& NewRotation);

private:
	// Experience 加载完成后的回调，用于根据当前 Experience 初始化玩家状态。
	/** 处理Experience配置已加载。
	 * @param CurrentExperience 当前Experience配置，由调用方传入。
	 */
	void OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience);
	friend class UHodgeAttributeCoordinator;
	// PlayerState属性协调器的数据或对象引用。
	UPROPERTY(VisibleAnywhere, Category="Hodge|Attributes") TObjectPtr<UHodgeAttributeCoordinator> AttributeCoordinator;
	// 角色成长的数据或对象引用。
	UPROPERTY(Replicated) FHodgeCharacterProgression CharacterProgression;
	// 属性就绪状态的数据或对象引用。
	UPROPERTY(ReplicatedUsing=OnRep_AttributeReadyState) FHodgeAttributeReadyState AttributeReadyState;
	// Owned装备States的记录集合。
	UPROPERTY(Replicated) TArray<FHodgeOwnedEquipmentState> OwnedEquipmentStates;
	/** 收到属性就绪状态复制后更新本地状态。 */
	UFUNCTION() void OnRep_AttributeReadyState();

protected:
	// PawnData 复制到客户端后触发的回调。
	/** 收到PawnData复制后推进本地初始化。 */
	UFUNCTION()
	void OnRep_PawnData();

	// 玩家当前使用的 PawnData，决定 Pawn 的类型和相关配置。
	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UHodgePawnData> PawnData;

private:
	// 玩家专属的 AbilitySystemComponent，负责玩家技能、GameplayEffect、GameplayCue 等 GAS 功能。
	UPROPERTY(VisibleAnywhere, Category = "Hodge|PlayerState")
	TObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;

	// 玩家生命值相关的 AttributeSet。
	UPROPERTY()
	TObjectPtr<const class UHodgeHealthSet> HealthSet;

	// Combat attribute set used by this actor.
	// 战斗属性集的数据或对象引用。
	UPROPERTY()
	TObjectPtr<const class UHodgeCombatSet> CombatSet;
	/** 共鸣解放资源只创建一次，随玩家ASC复制。 */
	UPROPERTY() TObjectPtr<const class UHodgeResonanceSet> ResonanceSet;
	/** 共鸣回路的独立个人资源，不复用大招能量。 */
	UPROPERTY() TObjectPtr<const class UHodgeHavocSet> HavocSet;

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
	// 基础授予属于 PlayerState，换 Pawn 不重新授予或交给装备撤销。
	UPROPERTY(Transient)
	FHodgeAbilitySet_GrantedHandles BaseAbilityHandles;
	// 配置错误及不可逆副作用失败均停止本 PlayerState 的自动重试。
	bool bBaseGrantFailed = false;
	bool bBaseGrantInProgress = false;
	bool bBaseGrantsClosed = false;

	// MyTeamID 复制到客户端后触发的回调。
	/** 收到My队伍ID复制后更新本地状态。
	 * @param OldTeamID 旧队伍ID，由调用方传入。
	 */
	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);

	// MySquadID 复制到客户端后触发的回调。
	/** 收到MySquadID复制后更新本地状态。 */
	UFUNCTION()
	void OnRep_MySquadID();
};
