/** @file HodgeCharacterMovementComponent.h
 * @brief 声明角色移动组件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "NativeGameplayTags.h"
#include "GameFramework/CharacterMovementReplication.h"
#include "Combat/HodgeMovementActionTypes.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "GameplayAbilitySpecHandle.h"

#include "HodgeCharacterMovementComponent.generated.h"

class UObject;
class UHodgeAbilityDefinition;
class UAnimMontage;
struct FFrame;
struct FHodgeCharacterRotationState;
namespace HodgeRotationPrediction { class FSavedMove; }

// 角色移动停止时使用的 GameplayTag。
HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_MovementStopped);

/**
 * @brief 角色当前脚下地面的信息。
 * @note 地面信息不会每帧强制更新，而是在需要访问时检查并更新。
 */
USTRUCT(BlueprintType)
struct FHodgeCharacterGroundInfo
{
	GENERATED_BODY()

	// 初始化地面信息的缓存帧号和地面距离。
	/** 构造角色地面信息并初始化其默认状态。 */
	FHodgeCharacterGroundInfo()
		: LastUpdateFrame(0)
		  , GroundDistance(0.0f)
	{
	}

	// 上一次更新地面信息时对应的帧号，用于判断缓存是否过期。
	uint64 LastUpdateFrame;

	// 角色脚下检测到的地面碰撞信息。
	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	// 角色当前位置到地面的距离。
	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};


/**
 * @brief Hodgepodge 项目使用的角色移动组件基类。
 *
 * 继承自 UCharacterMovementComponent，在 UE 默认角色移动系统的基础上扩展项目自己的移动逻辑。
 */
UCLASS(Config = Game)
class HODGEPODGE_API UHodgeCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	// 构造角色移动组件并初始化移动相关配置。
	/** 构造角色移动组件并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	// 模拟角色移动，用于处理角色移动状态和网络移动模拟。
	/** 按当前复制状态更新模拟代理的角色移动。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	virtual void SimulateMovement(float DeltaTime) override;
	/** 在现有动作与网络预测状态下执行角色移动。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	virtual void PerformMovement(float DeltaTime) override;
	/** 仅追踪开关启用时记录本步实际Warp输入；回放和移动作用域外调用不录入。 */
	void RecordAttackWarpEvaluation(const FHodgeCharacterRotationState& State, const UAnimMontage* Montage);
	/** 硬锁出现时记住当前段的实际Yaw；服务器发布冻结，客户端等待同段权威结果。 */
	void FreezeHitReactionFacing();
	/** Avatar解绑或换代时清理补转与一次校正，不改Actor朝向。 */
	void ResetHitReactionFacing();

	// 判断角色当前是否允许尝试跳跃。
	/** 检查是否允许尝试序号跳跃。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool CanAttemptJump() const override;

	// 获取角色当前的地面信息，如果缓存已经过期则先更新地面信息。
	/** 获取地面信息。
	 * @return 地面信息的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|CharacterMovement")
	const FHodgeCharacterGroundInfo& GetGroundInfo();

	// 设置从服务器复制过来的角色加速度。
	/** 设置复制加速度。
	 * @param InAcceleration 加速度，由调用方传入。
	 */
	void SetReplicatedAcceleration(const FVector& InAcceleration);

	//~UMovementComponent interface

	// 获取指定时间间隔内角色应该产生的旋转变化量。
	/** 获取增量旋转。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 * @return 增量旋转的当前值或引用。
	 */
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;

	// 获取角色当前移动状态下允许使用的最大速度。
	/** 获取最大速度。
	 * @return 最大速度的当前值或引用。
	 */
	virtual float GetMaxSpeed() const override;
	/** 获取最大加速度。
	 * @return 最大加速度的当前值或引用。
	 */
	virtual float GetMaxAcceleration() const override;
	/** 获取最大BrakingDeceleration。
	 * @return 最大BrakingDeceleration的当前值或引用。
	 */
	virtual float GetMaxBrakingDeceleration() const override;
	/** 按当前朝向与动作策略约束输入加速度。
	 * @param InputAcceleration 输入加速度，由调用方传入。
	 * @return 本次计算、查询或创建得到的向量。
	 */
	virtual FVector ConstrainInputAcceleration(const FVector& InputAcceleration) const override;
	/** 应用Requested移动。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 * @param MaxAccel 最大Accel，由调用方传入。
	 * @param MaxSpeed 最大速度，由调用方传入。
	 * @param Friction Friction，由调用方传入。
	 * @param BrakingDeceleration BrakingDeceleration，由调用方传入。
	 * @param OutAcceleration 输出加速度。
	 * @param OutRequestedSpeed 输出Requested速度。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool ApplyRequestedMove(float DeltaTime, float MaxAccel, float MaxSpeed, float Friction,
		float BrakingDeceleration, FVector& OutAcceleration, float& OutRequestedSpeed) override;
	/** 设置命中受击移动回放。
	 * @param bControlled 是否Controlled的控制标记。
	 */
	void SetHitReactionMoveReplay(bool bControlled) { bHasReactionReplay = true; bReactionReplayControlled = bControlled; }

	/** 通过当前旋转模式和约束更新角色实际朝向。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	virtual void PhysicsRotation(float DeltaTime) override;
	/** 处理服务器侧的移动Perform移动消息，沿用当前身份与权限校验。
	 * @param MoveData 移动数据，由调用方传入。
	 */
	virtual void ServerMove_PerformMovement(const FCharacterNetworkMoveData& MoveData) override;
	/** 获取预测数据客户端。
	 * @return 预测数据客户端的当前值或引用。
	 */
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	/** 处理客户端侧的更新位置After服务器更新消息，沿用当前身份与权限校验。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool ClientUpdatePositionAfterServerUpdate() override;
	/** 处理客户端侧的句柄移动响应消息，沿用当前身份与权限校验。
	 * @param MoveResponse 移动响应，由调用方传入。
	 */
	virtual void ClientHandleMoveResponse(const FCharacterMoveResponseDataContainer& MoveResponse) override;
	/** 处理客户端侧的Adjust位置消息，沿用当前身份与权限校验。
	 * @param TimeStamp 时间Stamp，由调用方传入。
	 * @param NewLoc 新Loc，由调用方传入。
	 * @param NewVel 新Vel，由调用方传入。
	 * @param NewBase 新基础，由调用方传入。
	 * @param NewBaseBoneName 新基础骨骼名称，由调用方传入。
	 * @param bHasBase 是否存在判断基础的控制标记。
	 * @param bBaseRelativePosition 是否基础相对位置的控制标记。
	 * @param ServerMovementMode 服务器移动Mode，由调用方传入。
	 * @param OptionalRotation Optional旋转，由调用方传入。
	 */
	virtual void ClientAdjustPosition_Implementation(float TimeStamp, FVector NewLoc, FVector NewVel,
		UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition,
		uint8 ServerMovementMode, TOptional<FRotator> OptionalRotation = TOptional<FRotator>()) override;
	/** 原生Montage纠正携带真实朝向，legacy入口也禁止可选补转锚点覆盖它。 */
	virtual void ClientAdjustRootMotionPosition_Implementation(float TimeStamp, float ServerMontageTrackPosition,
		FVector ServerLoc, FVector_NetQuantizeNormal ServerRotation, float ServerVelZ, UPrimitiveComponent* ServerBase,
		FName ServerBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode) override;
	/** 原生RMS纠正沿原有Super执行，作用域只隔离嵌套位置纠正的锚点记录。 */
	virtual void ClientAdjustRootMotionSourcePosition_Implementation(float TimeStamp, FRootMotionSourceGroup ServerRootMotion,
		bool bHasAnimRootMotion, float ServerMontageTrackPosition, FVector ServerLoc, FVector_NetQuantizeNormal ServerRotation,
		float ServerVelZ, UPrimitiveComponent* ServerBase, FName ServerBoneName, bool bHasBase, bool bBaseRelativePosition,
		uint8 ServerMovementMode) override;
#if WITH_DEV_AUTOMATION_TESTS
	/** 获取朝向移动数据关联Tests。
	 * @return 朝向移动数据关联Tests的当前值或引用。
	 */
	FCharacterNetworkMoveDataContainer& GetFacingMoveDataForTests() { return *FacingMoveData; }
#endif
	/** 按现有网络策略平滑本次位置修正。
	 * @param OldLocation 旧位置，由调用方传入。
	 * @param OldRotation 旧旋转，由调用方传入。
	 * @param NewLocation 新位置，由调用方传入。
	 * @param NewRotation 新旋转，由调用方传入。
	 */
	virtual void SmoothCorrection(const FVector& OldLocation, const FQuat& OldRotation,
		const FVector& NewLocation, const FQuat& NewRotation) override;

	//~End of UMovementComponent interface

protected:
	// 初始化移动组件，并执行组件级初始化逻辑。
	/** 初始化组件。 */
	virtual void InitializeComponent() override;

	/** 按当前组件约束执行实际移动与碰撞处理。
	 * @param Delta 增量，由调用方传入。
	 * @param NewRotation 新旋转，由调用方传入。
	 * @param bSweep 是否扫掠的控制标记。
	 * @param OutHit 输出命中。
	 * @param Teleport Teleport，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool MoveUpdatedComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep,
		FHitResult* OutHit = nullptr, ETeleportType Teleport = ETeleportType::None) override;

protected:
	// 缓存角色当前脚下的地面信息，不要直接访问，应通过 GetGroundInfo() 获取。
	FHodgeCharacterGroundInfo CachedGroundInfo;

	// 标记当前是否已经获得过网络复制的加速度。
	UPROPERTY(Transient)
	bool bHasReplicatedAcceleration = false;

	// 控制是否Applying旋转Correction的状态标记。
	bool bApplyingRotationCorrection = false;
	// 控制是否存在判断受击回放的状态标记。
	bool bHasReactionReplay = false;
	// 控制是否受击回放Controlled的状态标记。
	bool bReactionReplayControlled = false;
	// Dash冲刺Correction速度的数据或对象引用。
	TOptional<FVector> DashCorrectionVelocity;
	/** 判断命中受击Controlled。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsHitReactionControlled() const;
	// 朝向移动数据的数据或对象引用。
	TUniquePtr<FCharacterNetworkMoveDataContainer> FacingMoveData;

private:
	friend class HodgeRotationPrediction::FSavedMove;
	friend struct FHodgeAttackWarpTestAccess;
	friend struct FHodgeHitReactionFacingMovementTestAccess;
	/** 单个历史移动只消费自身的权威快照、时段与暂停/硬锁事实。 */
	struct FHitReactionFacingMoveSnapshot
	{
		FHodgeHitReactionFacingState Facing;
		TWeakObjectPtr<ACharacter> Avatar;
		int32 AvatarGeneration = INDEX_NONE;
		float EvaluatedAt = 0.f;
		float EvaluationStartedAt = 0.f;
		bool bCurrent = false;
		bool bPaused = false;
		bool bExternalLock = false;
		bool bCorrectionPending = false;
		bool bLocallyFrozen = false;
		bool bHasRecordedYaw = false;
		float RecordedYaw = 0.f;
	};
	/** 录制实时事实；SetMoveFor在真正移动前调用，回放不重新读取世界时间。 */
	FHitReactionFacingMoveSnapshot CaptureHitReactionFacing(float DeltaTime) const;
	/** 在PerformMovement唯一入口消费本步补转，不改世界运动速度。 */
	void ApplyHitReactionFacing(const FHitReactionFacingMoveSnapshot& Snapshot, float DeltaTime);
	/** 当前段Final只消费一次；回放或原生新ACK后的首允许入口可恢复已消费约束。 */
	void ApplyHitReactionFacingCorrection(bool bRestoreAfterReplay = false);
	/** 所有合法Yaw写入均经过CMC，并同步当前冻结参考。 */
	void MoveHitReactionFacingYaw(float Yaw);
	/** 核对当前Avatar代与段，历史移动不能恢复已退役的当前占用。 */
	bool IsLiveHitReactionFacing(const FHodgeHitReactionFacingState& Facing) const;
	TOptional<FHitReactionFacingMoveSnapshot> HitReactionReplay;
	FGuid LiveFacingChainId;
	uint32 LiveFacingSegment = 0;
	int32 LiveFacingAvatarGeneration = INDEX_NONE;
	float LastFacingEvaluatedAt = -MAX_flt;
	// 客户端本地硬锁永久终止本段普通补转，权威冻结Yaw仍可校正一次。
	bool bFacingLocallyFrozen = false;
	float LocallyFrozenFacingYaw = 0.f;
	FGuid CorrectedFacingChainId;
	uint32 CorrectedFacingSegment = 0;
	uint32 CorrectedFacingRevision = 0;
	float CorrectedFacingYaw = 0.f;
	// 原生新ACK可改变身体Yaw；已消费Final须在首个允许入口恢复，暂停不丢失待办。
	bool bNativeFacingFinalRestorePending = false;
	bool bApplyingHitReactionFacing = false;
	/** 保留同身份当前参考；显式原生Yaw只有新ACK实际应用后才可成为锚点。 */
	struct FHitReactionReplayAnchor
	{
		TWeakObjectPtr<ACharacter> Avatar;
		int32 AvatarGeneration = INDEX_NONE;
		FGuid ChainId;
		uint32 Segment = 0;
		FGuid ActionRequestId;
		uint8 Driver = 0;
		float VisibleYaw = 0.f;
		bool bMayRestore = false;
		bool bNativeCorrection = false;
		bool bFacingCurrent = false;
	};
	/** 默认仅为受击历史留参考；原生纠正核验可额外捕获当前有效段身份。 */
	FHitReactionReplayAnchor CaptureHitReactionReplayAnchor(bool bMayRestore, bool bRequireFacingHistory = true) const;
	/** 保存原生处理前的真实未确认移动对象，重复时间戳不能冒充新ACK。 */
	FSavedMovePtr FindHitReactionCorrectionMove(float TimeStamp) const;
	/** 新ACK必须替换原引用并移出未确认列表，不能只比较LastAcked时间戳。 */
	bool IsHitReactionCorrectionAcknowledged(const FSavedMovePtr& ExpectedAck, const FSavedMovePtr& PreviousAck,
		float TimeStamp) const;
	/** 只同步新ACK实际应用的同身份原生Yaw，不改变RPC校验和历史移动载荷。 */
	void CommitHitReactionNativeCorrection(const FHitReactionReplayAnchor& Before, const FSavedMovePtr& ExpectedAck,
		const FSavedMovePtr& PreviousAck, float TimeStamp, const TCHAR* NativeType, float BeforeYaw, float ServerYaw);
	/** Super仍完整执行；未获新ACK的RMS只能恢复同身份回包前姿态，不能取得新授权。 */
	void RestoreRejectedHitReactionNativeCorrection(const FHitReactionReplayAnchor& Before, const FSavedMovePtr& ExpectedAck,
		const FSavedMovePtr& PreviousAck, float TimeStamp);
	/** 回放只恢复仍匹配的当前约束；原生新ACK恢复纠正后Yaw，动画根运动保持原机制。 */
	void RestoreHitReactionFacingAfterReplay(const FHitReactionReplayAnchor& Anchor);
	TOptional<FHitReactionReplayAnchor> HitReactionReplayAnchor;
	// Packed与根运动入口已记录本次响应，嵌套ClientAdjustPosition不能覆盖其真实Rotation标记。
	bool bHitReactionFacingResponseScope = false;
	/** 求值仅保留弱身份；完成后交给同一SavedMove回填Yaw，不延长动作或资产生命周期。 */
	struct FAttackWarpEvaluation
	{
		uint64 Sequence = 0;
		FGuid RequestId;
		FGameplayAbilitySpecHandle AbilityHandle;
		int32 PredictionKey = 0;
		int32 AvatarGeneration = INDEX_NONE;
		TWeakObjectPtr<ACharacter> Avatar;
		TWeakObjectPtr<const UHodgeAbilityDefinition> Definition;
		TWeakObjectPtr<const UAnimMontage> Montage;
		float ActionYaw = 0.f;
	};
	/** 每次移动开始清旧结果；序号把SetMoveFor和随后唯一移动步配对。 */
	void BeginAttackWarpEvaluation();
	/** 离开作用域后停止录入，仅保留本步无歧义的完成结果供PostUpdate消费。 */
	void EndAttackWarpEvaluation();
	/** SetMoveFor在PerformMovement之前执行，预记下一次求值序号。 */
	uint64 GetNextAttackWarpEvaluationSequence() const;
	/** 只消费同一步同身份结果；失败不改OutYaw或SavedMove其他状态。 */
	bool ConsumeAttackWarpEvaluation(uint64 Sequence, int32 AvatarGeneration,
		const FHodgeCharacterRotationState& InitialState, const UAnimMontage* Montage, float& OutYaw);
	uint64 AttackWarpEvaluationSequence = 0;
	bool bAttackWarpEvaluationActive = false;
	bool bAttackWarpEvaluationAmbiguous = false;
	TOptional<FAttackWarpEvaluation> ActiveAttackWarpEvaluation;
	TOptional<FAttackWarpEvaluation> CompletedAttackWarpEvaluation;
};
