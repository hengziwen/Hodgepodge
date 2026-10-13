/** @file HodgeCombatCharacter.h
 * @brief 声明共用战斗角色相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "Character/HodgeCharacterBase.h"
#include "HodgeCombatCharacter.generated.h"

class UHodgeHealthComponent;
class UHodgeHitReactionComponent;
class UHodgeCharacterRotationComponent;
class UHodgeAttackMotionWarpingComponent;
class UHodgeLocomotionPolicyComponent;
class UHodgeDefenseComponent;
class UHodgeEquipmentInstance;
class UHodgeEquipmentManagerComponent;
class UHodgePawnExtensionComponent;
class UHodgeCameraComponent;
class AHodgePlayerController;
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
	/** 构造共享复制移动并初始化其默认状态。 */
	FSharedRepMovement();

	// 从指定角色当前状态填充共享移动数据
	/** 从当前角色采集本次移动复制需要的数据。
	 * @param Character 角色，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool FillForCharacter(ACharacter* Character);

	// 判断共享移动数据是否与指定角色当前状态一致
	/** 按当前类型定义比较两个值是否相等。
	 * @param Other Other，由调用方传入。
	 * @param Character 角色，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool Equals(const FSharedRepMovement& Other, ACharacter* Character) const;

	// 自定义网络序列化，用于控制移动数据的网络传输格式
	/** 按既有协议读写网络载荷及对象引用。
	 * @param Ar 读取或写入数据的归档对象。
	 * @param Map 序列化使用的对象映射或当前映射表。
	 * @param bOutSuccess 输出序列化或处理是否成功。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
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
/** 保存TStructOps类型TraitsF共享复制移动的数据和接口契约。 */
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
	// 默认组件只提供根运动执行，未配置的攻击不会启用辅助。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hodge|Facing", meta=(AllowPrivateAccess="true")) TObjectPtr<UHodgeAttackMotionWarpingComponent> AttackMotionWarpingComponent;

public:
	// 构造函数
	/** 构造共用战斗角色并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	AHodgeCombatCharacter(const FObjectInitializer& ObjectInitializer);

	// 获取当前角色对应的 Hodge PlayerController
	/** 获取玩家控制器。
	 * @return 玩家控制器的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
	AHodgePlayerController* GetHodgePlayerController() const;

	// 获取当前角色对应的 Hodge PlayerState
	/** 获取玩家持久状态与ASC所有者。
	 * @return 玩家持久状态与ASC所有者的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
	AHodgePlayerState* GetHodgePlayerState() const;

	// 获取当前角色使用的 Hodge AbilitySystemComponent
	/** 获取能力系统组件。
	 * @return 能力系统组件的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const;

	/** 获取角色朝向组件。
	 * @return 角色朝向组件的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category = "Hodge|Character")
	UHodgeCharacterRotationComponent* GetCharacterRotationComponent() const { return RotationComponent; }
	/** 获取受击状态组件。
	 * @return 受击状态组件的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Character")
	UHodgeHitReactionComponent* GetHitReactionComponent() const { return HitReactionComponent; }
	/** 尝试初始化AttributesAnd装备。 */
	void TryInitializeAttributesAndEquipment();
	/** 准备默认装备。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool PrepareDefaultEquipment();
	/** 判断默认装备就绪。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsDefaultEquipmentReady() const;

	/** 按当前角色朝向策略处理控制器提出的旋转。
	 * @param NewControlRotation 新控制旋转，由调用方传入。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	virtual void FaceRotation(FRotator NewControlRotation, float DeltaTime = 0.f) override;

	// 实现 GAS 的 AbilitySystemInterface，返回角色对应的 AbilitySystemComponent
	/** 获取能力系统组件。
	 * @return 能力系统组件的当前值或引用。
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 获取角色当前拥有的所有 GameplayTag
	/** 获取Owned玩法标签。
	 * @param TagContainer 标签Container，用于对应标签查询或匹配。
	 */
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	// 判断角色是否拥有指定 GameplayTag
	/** 判断是否存在Matching玩法标签。
	 * @param TagToCheck 标签目标检查，用于对应标签查询或匹配。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;

	// 判断角色是否同时拥有指定容器中的所有 GameplayTag
	/** 判断是否存在AllMatching玩法标签。
	 * @param TagContainer 标签Container，用于对应标签查询或匹配。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	// 判断角色是否拥有指定容器中的任意 GameplayTag
	/** 判断是否存在AnyMatching玩法标签。
	 * @param TagContainer 标签Container，用于对应标签查询或匹配。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	// 切换角色蹲伏状态
	/** 按角色当前状态切换下蹲意图。 */
	void ToggleCrouch();

	//~AActor interface

	// Actor 组件初始化之前执行，用于准备角色依赖的组件和状态
	/** 在组件初始化前接入本对象所需的框架入口。 */
	virtual void PreInitializeComponents() override;

	// Actor 开始游戏时调用
	/** 进入世界运行阶段并接入本对象的生命周期依赖。 */
	virtual void BeginPlay() override;

	// Actor 结束生命周期时调用
	/** 离开世界运行阶段并解除本对象持有的监听或资源。
	 * @param EndPlayReason 结束PlayReason，由调用方传入。
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 重置角色状态
	/** 恢复本对象的初始状态。 */
	virtual void Reset() override;

	// 注册需要进行网络复制的属性
	/** 登记本类型需要网络复制的属性及复制条件。
	 * @param OutLifetimeProps 输出LifetimeProps。
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 每次 Actor 进行网络复制前调用，可动态控制属性是否需要复制
	/** 准备复制。
	 * @param ChangedPropertyTracker 变化属性Tracker，由调用方传入。
	 */
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

	//~End of AActor interface

	//~APawn interface

	// 当 Pawn 的 Controller 发生变化时调用
	/** 通知控制器变化。 */
	virtual void NotifyControllerChanged() override;

	//~End of APawn interface

	// 在默认属性复制被跳过的帧中，通过 Multicast RPC 向其他客户端同步一次移动状态
	/** 按当前共享移动复制路径发送角色状态。
	 * @param SharedRepMovement 共享复制移动，由调用方传入。
	 */
	UFUNCTION(NetMulticast, unreliable)
	void FastSharedReplication(const FSharedRepMovement& SharedRepMovement);

	// 上一次发送出去的 FastShared 移动数据，用于避免重复发送相同数据
	FSharedRepMovement LastSharedReplication;

	// 检查并更新 FastShared 移动复制数据
	/** 更新共享复制。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool UpdateSharedReplication();

protected:
	friend class UHodgeEnemyInitComponent;
	// AbilitySystem 初始化完成后调用
	/** 在ASC可用后接入角色或组件依赖。 */
	virtual void OnAbilitySystemInitialized();

	// AbilitySystem 被反初始化后调用
	/** 在ASC解绑时清理当前角色相关依赖。 */
	virtual void OnAbilitySystemUninitialized();

	// 为当前 Pawn 建立默认武器，重复初始化时保留已有实例。
	/** 按当前角色配置准备默认武器，复用已有有效实例。 */
	void InitializeDefaultEquipment();

	// 撤销本 Pawn 初始化时创建的武器及其授予内容。
	/** 撤销本角色默认装备来源并清空缓存引用。 */
	void UninitializeDefaultEquipment();

	// Pawn 被 Controller 占有时调用
	/** 服务器角色被控制器接管时更新绑定和初始化。
	 * @param NewController 新控制器，由调用方传入。
	 */
	virtual void PossessedBy(AController* NewController) override;

	// Pawn 失去 Controller 占有时调用
	/** 在角色失去控制器时解除当前控制关联。 */
	virtual void UnPossessed() override;

	// Controller 在网络复制到客户端后发生变化时调用
	/** 收到控制器复制后刷新本地依赖。 */
	virtual void OnRep_Controller() override;

	// PlayerState 在网络复制到客户端后发生变化时调用
	/** 收到PlayerState复制后刷新角色绑定。 */
	virtual void OnRep_PlayerState() override;

	// 设置角色的玩家输入
	/** 将角色输入组件绑定到既有动作与标签配置。
	 * @param PlayerInputComponent 玩家输入组件，由调用方传入。
	 */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// 初始化角色相关的 GameplayTag
	/** 初始化项目原生GameplayTag。 */
	void InitializeGameplayTags();

	// 角色掉出世界时调用
	/** 处理角色离开世界有效范围后的退出行为。
	 * @param dmgType dmg类型，由调用方传入。
	 */
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	// 开始角色死亡流程，例如关闭碰撞、停止移动等
	/** 处理死亡开始。
	 * @param OwningActor OwningActor对象，由调用方传入。
	 */
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// 结束角色死亡流程，例如解除 Controller、销毁 Pawn 等
	/** 处理死亡已结束。
	 * @param OwningActor OwningActor对象，由调用方传入。
	 */
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	// 禁用角色移动和碰撞
	/** 按现有退出或死亡流程关闭移动与碰撞。 */
	void DisableMovementAndCollision();

	// 因死亡原因销毁角色
	/** 销毁Due目标死亡。 */
	void DestroyDueToDeath();

	// 解除角色相关系统并销毁角色
	/** 按现有角色退出流程先解绑再销毁。 */
	void UninitAndDestroy();

	// 死亡流程完成后触发的蓝图事件
	/** 触发蓝图死亡已结束。 */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeathFinished"))
	void K2_OnDeathFinished();

	// 角色移动模式发生变化时调用
	/** 处理移动Mode变化。
	 * @param PrevMovementMode Prev移动Mode，由调用方传入。
	 * @param PreviousCustomMode 前次自定义Mode，由调用方传入。
	 */
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	// 根据当前移动模式启用或禁用对应的 GameplayTag
	/** 设置移动Mode标签。
	 * @param MovementMode 移动Mode，由调用方传入。
	 * @param CustomMovementMode 自定义移动Mode，由调用方传入。
	 * @param bTagEnabled 是否标签启用的控制标记。
	 */
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

	// 角色开始蹲伏时调用
	/** 处理起始下蹲。
	 * @param HalfHeightAdjust Half高度Adjust，由调用方传入。
	 * @param ScaledHalfHeightAdjust ScaledHalf高度Adjust，由调用方传入。
	 */
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	// 角色结束蹲伏时调用
	/** 处理结束下蹲。
	 * @param HalfHeightAdjust Half高度Adjust，由调用方传入。
	 * @param ScaledHalfHeightAdjust ScaledHalf高度Adjust，由调用方传入。
	 */
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	// 判断角色当前是否允许跳跃
	/** 检查是否允许跳跃内部。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool CanJumpInternal_Implementation() const;

private:
	// 受击状态组件的数据或对象引用。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hodge|Character", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UHodgeHitReactionComponent> HitReactionComponent;
	// 仅服务器记录默认武器，客户端通过装备组件接收复制。
	UPROPERTY(Transient)
	TObjectPtr<UHodgeEquipmentInstance> DefaultWeaponInstance;

	// 记录实际创建默认武器的组件，避免清理时找到其他容器。
	UPROPERTY(Transient)
	TWeakObjectPtr<UHodgeEquipmentManagerComponent> DefaultEquipmentManager;

	// 防止装备回调同步触发重复初始化。
	bool bInitializingDefaultEquipment = false;

	// Pawn 扩展组件，当前暂未启用
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHodgePawnExtensionComponent> PawnExtComponent;

	// 角色生命值组件，当前暂未启用
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHodgeHealthComponent> HealthComponent;

	// 旋转组件的数据或对象引用。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHodgeCharacterRotationComponent> RotationComponent;
	// 移动表现策略组件的数据或对象引用。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true")) TObjectPtr<UHodgeLocomotionPolicyComponent> LocomotionPolicyComponent;
	// 防御窗口组件的数据或对象引用。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true")) TObjectPtr<UHodgeDefenseComponent> DefenseComponent;

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
	/** 按当前队伍策略确定解绑后的团队身份。
	 * @param OldTeamID 旧队伍ID，由调用方传入。
	 * @return 本次计算、查询或创建得到的Generic队伍标识。
	 */
	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
	{
		// 可以返回 OldTeamID 保留原阵营，也可以返回其他 ID 让角色进入中立阵营
		return FGenericTeamId::NoTeam;
	}

private:
	// 当 Controller 所属 Team 发生变化时调用
	/** 处理控制器变化队伍。
	 * @param TeamAgent 队伍Agent，由调用方传入。
	 * @param OldTeam 旧队伍，由调用方传入。
	 * @param NewTeam 新队伍，由调用方传入。
	 */
	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

	// 当角色的加速度网络数据发生变化时调用
	/** 收到复制加速度复制后更新本地状态。 */
	UFUNCTION()
	void OnRep_ReplicatedAcceleration();

	// 当角色的 Team ID 网络复制发生变化时调用
	/** 收到My队伍ID复制后更新本地状态。
	 * @param OldTeamID 旧队伍ID，由调用方传入。
	 */
	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
};
