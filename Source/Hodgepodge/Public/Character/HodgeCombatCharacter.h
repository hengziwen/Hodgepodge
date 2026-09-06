// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystem/HodgeAbilitySystemComponentBase.h"
#include "Character/HodgeCharacterBase.h"
#include "HodgeCombatCharacter.generated.h"

class UHodgeCameraComponent;
class AHodgePlayerControllerBase;
class AHodgePlayerState;

/**
 * @brief 角色加速度的压缩网络表示
 *
 * 用较少的网络数据表示角色当前的加速度方向、大小以及 Z 轴加速度，
 * 用于角色移动状态的网络同步。
 */
USTRUCT()
struct FHodgeReplicatedAcceleration
{
	GENERATED_BODY()

	// XY 平面加速度方向，量化到 [0, 2π] 范围后使用 uint8 存储
	UPROPERTY()
	uint8 AccelXYRadians = 0;

	// XY 平面加速度大小，量化到 [0, MaxAcceleration] 范围后使用 uint8 存储
	UPROPERTY()
	uint8 AccelXYMagnitude = 0;

	// Z 轴加速度，量化到 [-MaxAcceleration, MaxAcceleration] 范围后使用 int8 存储
	UPROPERTY()
	int8 AccelZ = 0;
};

/**
 * @brief FastShared 移动同步使用的数据结构
 *
 * 用于在默认属性复制被跳过的帧中，通过 RPC 快速同步一次角色移动状态。
 */
USTRUCT()
struct FSharedRepMovement
{
	GENERATED_BODY()

	// 初始化共享移动数据
	FSharedRepMovement();

	// 从指定角色当前状态填充共享移动数据
	bool FillForCharacter(ACharacter* Character);

	// 判断共享移动数据是否与指定角色当前状态一致
	bool Equals(const FSharedRepMovement& Other, ACharacter* Character) const;

	// 自定义网络序列化，用于控制移动数据的网络传输格式
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	// 角色标准移动复制数据
	UPROPERTY(Transient)
	FRepMovement RepMovement;

	// 移动数据对应的时间戳
	UPROPERTY(Transient)
	float RepTimeStamp = 0.0f;

	// 角色当前移动模式
	UPROPERTY(Transient)
	uint8 RepMovementMode = 0;

	// 代理角色是否应用了跳跃力
	UPROPERTY(Transient)
	bool bProxyIsJumpForceApplied = false;

	// 角色是否处于蹲伏状态
	UPROPERTY(Transient)
	bool bIsCrouched = false;
};

// 声明 FSharedRepMovement 使用自定义网络序列化和共享序列化
template <>
struct TStructOpsTypeTraits<FSharedRepMovement> : public TStructOpsTypeTraitsBase2<FSharedRepMovement>
{
	enum
	{
		// 使用自定义 NetSerialize 进行网络序列化
		WithNetSerializer = true,

		// 支持网络共享序列化，减少重复序列化带来的开销
		WithNetSharedSerialization = true,
	};
};

/**
 * @brief Hodgepodge 框架的战斗角色基类
 *
 * 继承自 AHodgeCharacterBase，是 CharacterBase 和 HeroCharacter 之间的中间层。
 * 预留给战斗相关的通用逻辑使用，如战斗组件、攻击判定等。
 *
 * 继承层次:
 * AHodgeCharacterBase → AHodgeCombatCharacter → AHodgeHeroCharacter
 *
 * 使用场景:
 * - 如果需要多种战斗角色（如近战英雄、远程英雄），可以在此类中放置
 *   共用的战斗逻辑，避免在 HeroCharacter 中重复代码。
 * - 敌人 AI 角色也可以继承此类获得战斗能力，但不会包含 GAS 初始化逻辑。
 */
UCLASS()
class HODGEPODGE_API AHodgeCombatCharacter : public AHodgeCharacterBase,
                                             public IAbilitySystemInterface,
                                             public IGameplayCueInterface,
                                             public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// 构造函数
	AHodgeCombatCharacter(const FObjectInitializer& ObjectInitializer);

	// 获取当前角色对应的 Hodge PlayerController
	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
	AHodgePlayerControllerBase* GetHodgePlayerController() const;

	// 获取当前角色对应的 Hodge PlayerState
	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
	AHodgePlayerState* GetHodgePlayerState() const;

	// 获取当前角色使用的 Hodge AbilitySystemComponent
	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
	UHodgeAbilitySystemComponentBase* GetHodgeAbilitySystemComponent() const;

	// 实现 GAS 的 AbilitySystemInterface，返回角色对应的 AbilitySystemComponent
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 获取角色当前拥有的所有 GameplayTag
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	// 判断角色是否拥有指定 GameplayTag
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;

	// 判断角色是否同时拥有指定容器中的所有 GameplayTag
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	// 判断角色是否拥有指定容器中的任意 GameplayTag
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	// 切换角色蹲伏状态
	void ToggleCrouch();

	//~AActor interface

	// Actor 组件初始化之前执行，用于准备角色依赖的组件和状态
	virtual void PreInitializeComponents() override;

	// Actor 开始游戏时调用
	virtual void BeginPlay() override;

	// Actor 结束生命周期时调用
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 重置角色状态
	virtual void Reset() override;

	// 注册需要进行网络复制的属性
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 每次 Actor 进行网络复制前调用，可动态控制属性是否需要复制
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

	//~End of AActor interface

	//~APawn interface

	// 当 Pawn 的 Controller 发生变化时调用
	virtual void NotifyControllerChanged() override;

	//~End of APawn interface

	// 在默认属性复制被跳过的帧中，通过 Multicast RPC 向其他客户端同步一次移动状态
	UFUNCTION(NetMulticast, unreliable)
	void FastSharedReplication(const FSharedRepMovement& SharedRepMovement);

	// 上一次发送出去的 FastShared 移动数据，用于避免重复发送相同数据
	FSharedRepMovement LastSharedReplication;

	// 检查并更新 FastShared 移动复制数据
	virtual bool UpdateSharedReplication();

protected:
	// AbilitySystem 初始化完成后调用
	virtual void OnAbilitySystemInitialized();

	// AbilitySystem 被反初始化后调用
	virtual void OnAbilitySystemUninitialized();

	// Pawn 被 Controller 占有时调用
	virtual void PossessedBy(AController* NewController) override;

	// Pawn 失去 Controller 占有时调用
	virtual void UnPossessed() override;

	// Controller 在网络复制到客户端后发生变化时调用
	virtual void OnRep_Controller() override;

	// PlayerState 在网络复制到客户端后发生变化时调用
	virtual void OnRep_PlayerState() override;

	// 设置角色的玩家输入
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// 初始化角色相关的 GameplayTag
	void InitializeGameplayTags();

	// 角色掉出世界时调用
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	// 开始角色死亡流程，例如关闭碰撞、停止移动等
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// 结束角色死亡流程，例如解除 Controller、销毁 Pawn 等
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	// 禁用角色移动和碰撞
	void DisableMovementAndCollision();

	// 因死亡原因销毁角色
	void DestroyDueToDeath();

	// 解除角色相关系统并销毁角色
	void UninitAndDestroy();

	// 死亡流程完成后触发的蓝图事件
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeathFinished"))
	void K2_OnDeathFinished();

	// 角色移动模式发生变化时调用
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	// 根据当前移动模式启用或禁用对应的 GameplayTag
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

	// 角色开始蹲伏时调用
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	// 角色结束蹲伏时调用
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	// 判断角色当前是否允许跳跃
	virtual bool CanJumpInternal_Implementation() const;

private:
	// Pawn 扩展组件，当前暂未启用
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UHodgePawnExtensionComponent> PawnExtComponent;

	// 角色生命值组件，当前暂未启用
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UHodgeHealthComponent> HealthComponent;

	// 角色使用的 Camera Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHodgeCameraComponent> CameraComponent;

	// 角色当前加速度的压缩网络复制数据
	UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedAcceleration)
	FHodgeReplicatedAcceleration ReplicatedAcceleration;

	// 角色所属的 Gameplay Team ID
	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

protected:
	// Controller 失去对角色的控制后，决定角色新的 Team ID
	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
	{
		// 可以返回 OldTeamID 保留原阵营，也可以返回其他 ID 让角色进入中立阵营
		return FGenericTeamId::NoTeam;
	}

private:
	// 当 Controller 所属 Team 发生变化时调用
	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

	// 当角色的加速度网络数据发生变化时调用
	UFUNCTION()
	void OnRep_ReplicatedAcceleration();

	// 当角色的 Team ID 网络复制发生变化时调用
	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
};
