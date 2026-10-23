/** @file HodgeCharacterRotationComponent.h
 * @brief 声明角色朝向组件相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "GameplayTagContainer.h"
#include "Combat/HodgeCharacterFacingTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "Combat/HodgeAttackFacingTypes.h"
#include "HodgeCharacterRotationComponent.generated.h"

class UHodgeAbilityDefinition;

class UHodgeAbilitySystemComponent;
class UHodgeLockOnComponent;
class UHodgeGameplayAbility_Definition;

/** 可复制的旋转约束；Actor Transform 仍由 CharacterMovement 同步。 */
USTRUCT(BlueprintType)
struct FHodgeCharacterRotationState
{
	GENERATED_BODY()

	// 当前是否存在有效的Yaw冻结约束。
	UPROPERTY(BlueprintReadOnly)
	bool bYawLocked = false;

	// 是否正在按恢复规则回到基础朝向。
	UPROPERTY(BlueprintReadOnly)
	bool bRecoveringFacing = false;

	// 冻结约束使用的Yaw。
	UPROPERTY(BlueprintReadOnly)
	float LockedYaw = 0.f;

	// 动作覆盖结束后继续使用的基础朝向驱动。
	// 基础模式是否由锁定组件派生，避免旧确认恢复已解除的本地占用。
	UPROPERTY() bool bLockOnFacing = false;
	UPROPERTY(BlueprintReadOnly) EHodgeCharacterFacingDriver BaseDriver = EHodgeCharacterFacingDriver::Movement;
	// 当前朝向的驱动来源。
	UPROPERTY(BlueprintReadOnly) EHodgeCharacterFacingDriver Driver = EHodgeCharacterFacingDriver::Movement;
	// 当前移动动画风格。
	UPROPERTY(BlueprintReadOnly) EHodgeLocomotionStyle Style = EHodgeLocomotionStyle::FreeDirectional;
	// 根运动策略按动作授权，不成为第四种基础驱动。
	UPROPERTY(BlueprintReadOnly) EHodgeFacingExecutionMode ExecutionMode = EHodgeFacingExecutionMode::PhysicalTurn;
	UPROPERTY() TObjectPtr<const UHodgeAbilityDefinition> WarpDefinition = nullptr;
	// 动作当前请求的世界Yaw。
	UPROPERTY(BlueprintReadOnly) float ActionYaw = 0.f;
	// 当前生效动作朝向请求的身份。
	UPROPERTY(BlueprintReadOnly) FGuid ActionRequestId;
	// 该朝向关联的能力Spec句柄。
	UPROPERTY() FGameplayAbilitySpecHandle ActionAbilityHandle;
	// 该朝向关联的预测激活键。
	UPROPERTY() int32 ActionPredictionKey = 0;
	// 本状态快照的修订序号。
	UPROPERTY(BlueprintReadOnly) int32 StateVersion = 0;
	// 本端朝向请求的顺序号。
	UPROPERTY() uint32 RequestSequence = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHodgeFacingStateChanged, FHodgeCharacterRotationState, Previous, FHodgeCharacterRotationState, Current);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHodgeActionFacingApplied, FHodgeFacingRequestHandle, Handle, EHodgeFacingRequestStatus, Status);

/**
 * 解析 Pawn 的旋转约束，控制器与移动组件负责执行。
 * ASC 由 PawnExtension 生命周期注入；组件不修改共享 ASC 的标签。
 */
UCLASS(BlueprintType, Meta = (BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeCharacterRotationComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	/** 构造角色朝向组件并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeCharacterRotationComponent(const FObjectInitializer& ObjectInitializer);

	/** 初始化关联能力System。
	 * @param InASC 后续GAS操作使用的能力系统组件。
	 */
	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
	/** 解除初始化来源能力System。 */
	void UninitializeFromAbilitySystem();

	/** 预留侧移只消费控制朝向，不搜索目标或调整相机。 */
	/** 按来源申请基础朝向模式，返回可配对释放的句柄。
	 * @param Driver 当前朝向的驱动来源。
	 * @param Source 本次操作的来源。
	 * @return 本次操作的句柄，调用方应按现有接口检查有效性。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Rotation")
	FHodgeFacingRequestHandle AcquireBaseFacingMode(EHodgeCharacterFacingDriver Driver, UObject* Source);
	/** 只派生本端有效锁定占用，不经过通用旋转命令 RPC。 */
	FHodgeFacingRequestHandle AcquireLockOnFacing(UHodgeLockOnComponent* Source);
	/** 申请本次动作的目标朝向，并交给现有仲裁与移动链。
	 * @param WorldDirection 世界方向，由调用方传入。
	 * @param Source 本次操作的来源。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 * @param bInstantAtStart 是否Instant时刻起始的控制标记。
	 * @return 本次操作的句柄，调用方应按现有接口检查有效性。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Rotation")
	FHodgeFacingRequestHandle RequestActionFacing(FVector WorldDirection, UObject* Source, FGuid ExecutionId, bool bInstantAtStart = false);
	/** GAS 已验证方向后申请根运动占用，不另发方向 RPC。 */
	FHodgeFacingRequestHandle RequestMontageWarpFacing(float TargetYaw, UObject* Source, FGuid ExecutionId);
	/** 只更新当前 GAS 请求的有界方向，同身份的服务器移动覆盖同步消费新 Yaw。 */
	bool UpdateMontageWarpFacing(FHodgeFacingRequestHandle Handle, UHodgeGameplayAbility_Definition* Source,
		FGuid ExecutionId, float TargetYaw);
	/** 预测取消只改变本地执行许可，不提前撤销权威请求。 */
	bool SetFacingRequestSuspended(FHodgeFacingRequestHandle Handle, bool bSuspended);
	/** 查询当前请求是否获得执行权，不等同于已转到目标。 */
	bool IsFacingRequestSelected(FHodgeFacingRequestHandle Handle) const;
	/** 释放匹配当前初始化代的朝向请求。
	 * @param Handle 对应操作或能力的句柄。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Rotation")
	bool ReleaseFacingRequest(FHodgeFacingRequestHandle Handle);
	/** 释放Requests关联来源。
	 * @param Source 本次操作的来源。
	 */
	void ReleaseRequestsForSource(const UObject* Source);
	/** 获取朝向请求状态。
	 * @param Handle 对应操作或能力的句柄。
	 * @return 朝向请求状态的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Rotation")
	EHodgeFacingRequestStatus GetFacingRequestStatus(FHodgeFacingRequestHandle Handle) const;
	/** 同一 GAS 激活的权威与预测记录可以共同确认相同动作朝向。 */
	/** 确认本来源与执行身份的朝向请求已实际应用。
	 * @param Handle 对应操作或能力的句柄。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Rotation")
	bool IsActionFacingApplied(FHodgeFacingRequestHandle Handle) const;
	/** 获取朝向表现快照。
	 * @return 朝向表现快照的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Rotation")
	FHodgeFacingPresentationSnapshot GetFacingPresentationSnapshot() const;
	/** 获取表现角色初始化代。
	 * @return 表现角色初始化代的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Rotation")
	int32 GetAvatarGeneration() const { return AvatarGeneration; }
	/** 判断朝向System就绪。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Rotation")
	bool IsFacingSystemReady() const;
	// 事件朝向状态变化对应的事件通知。
	UPROPERTY(BlueprintAssignable) FHodgeFacingStateChanged OnFacingStateChanged;
	// 事件动作朝向已应用对应的事件通知。
	UPROPERTY(BlueprintAssignable) FHodgeActionFacingApplied OnActionFacingApplied;

	/** 应用ResolvedMode。 */
	void ApplyResolvedMode();
	/** 开始服务器移动。
	 * @param Sequence 序号，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool BeginServerMove(uint32 Sequence, int32 MontageWarpKey = 0);
	/** 结束服务器移动。 */
	void EndServerMove();

	/** 判断Yaw角锁定。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
	bool IsYawLocked() const;
	/** 区分外部硬锁与受击自身的控制；回放读取该移动保存的锁原因。 */
	bool IsExternallyYawLocked() const;
	/** 仅由CMC把合法受击结果交回冻结约束，不申请普通动作或另发RPC。 */
	void UpdateHitReactionLockedYaw(float Yaw);

	/** 判断Recovering朝向。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
	bool IsRecoveringFacing() const;

	/** 获取锁定Yaw角。
	 * @return 锁定Yaw角的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
	float GetLockedYaw() const;

	/** 获取Resolved状态。
	 * @return Resolved状态的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
	FHodgeCharacterRotationState GetResolvedState() const;

	/** 筛选控制旋转。
	 * @param DesiredRotation 期望旋转，由调用方传入。
	 * @param DeltaSeconds 本次更新的时间步长，单位秒。
	 * @return 本次计算、查询或创建得到的Rotator。
	 */
	FRotator FilterControlRotation(const FRotator& DesiredRotation, float DeltaSeconds) const;
	/** 将实际朝向结果反馈给当前动作请求。
	 * @param DesiredYaw 期望Yaw角，由调用方传入。
	 */
	void NotifyFacingApplied(float DesiredYaw);
	/** 获取恢复转向速率。
	 * @return 恢复转向速率的当前值或引用。
	 */
	float GetRecoveryTurnRate() const { return FMath::IsFinite(RecoveryTurnRate) && RecoveryTurnRate > 0.f ? RecoveryTurnRate : 360.f; }
#if WITH_EDITOR
	/** 在编辑器校验当前配置并收集诊断。
	 * @param Context 本次操作的上下文。
	 * @return 当前配置的校验结果。
	 */
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

	/** 设置移动回放状态。
	 * @param State 当前状态数据。
	 */
	void SetMoveReplayState(const FHodgeCharacterRotationState& State, TOptional<bool> bExternalLock = {});
	/** 清空移动回放状态。 */
	void ClearMoveReplayState();

	/** 登记本类型需要网络复制的属性及复制条件。
	 * @param OutLifetimeProps 输出LifetimeProps。
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	/** 离开世界运行阶段并解除本对象持有的监听或资源。
	 * @param EndPlayReason 结束PlayReason，由调用方传入。
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	/** 更新组件。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 * @param TickType 帧更新类型，由调用方传入。
	 * @param ThisTickFunction This帧更新Function，由调用方传入。
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
#if WITH_DEV_AUTOMATION_TESTS
	friend struct FHodgeFacingLifecycleTestAccess;
	friend struct FHodgeAttackWarpTestAccess;
#endif
	/** 处理约束标签变化。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @param NewCount 新数量，由调用方传入。
	 */
	void HandleConstraintTagChanged(FGameplayTag Tag, int32 NewCount);
	/** 刷新本地状态。 */
	void RefreshLocalState();
	/** 发布本对象当前的服务器权威状态。 */
	void PublishAuthorityState();
	/** 判断Replaying移动。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsReplayingMove() const;
	/** 解析Requests。 */
	void ResolveRequests();
	/** 判断有效来源。
	 * @param Source 本次操作的来源。
	 * @param bAction 是否动作的控制标记。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsValidSource(const UObject* Source, bool bAction) const;
	/** 添加请求。
	 * @param Driver 当前朝向的驱动来源。
	 * @param Source 本次操作的来源。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 * @param Yaw Yaw角，由调用方传入。
	 * @param bInstant 是否Instant的控制标记。
	 * @return 本次操作的句柄，调用方应按现有接口检查有效性。
	 */
	FHodgeFacingRequestHandle AddRequest(EHodgeCharacterFacingDriver Driver, UObject* Source, FGuid ExecutionId, float Yaw, bool bInstant, bool bLocalOnly = false);
	/** 沿现有复制入口提交当前请求及其身份。
	 * @param Id 当前记录的唯一标识。
	 * @param bRelease 是否释放的控制标记。
	 */
	void SendRequest(const FGuid& Id, bool bRelease);
	/** 设置请求状态。
	 * @param Id 当前记录的唯一标识。
	 * @param Status 状态，由调用方传入。
	 */
	void SetRequestStatus(const FGuid& Id, EHodgeFacingRequestStatus Status);
	/** 判断是否存在待处理预测。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool HasPendingPrediction() const;
	/** 判断权威动作当前。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsAuthorityActionCurrent() const;

	/** 处理服务器侧的朝向请求消息，沿用当前身份与权限校验。
	 * @param Avatar 表现角色，由调用方传入。
	 * @param Sequence 序号，由调用方传入。
	 * @param Id 当前记录的唯一标识。
	 * @param bRelease 是否释放的控制标记。
	 * @param Driver 当前朝向的驱动来源。
	 * @param Yaw Yaw角，由调用方传入。
	 * @param bInstant 是否Instant的控制标记。
	 * @param AbilityHandle 已授予能力的Spec句柄。
	 * @param PredictionKey 客户端预测与服务器确认的关联键。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 */
	UFUNCTION(Server, Reliable)
	void ServerFacingRequest(AActor* Avatar, uint32 Sequence, FGuid Id, bool bRelease, EHodgeCharacterFacingDriver Driver,
		float Yaw, bool bInstant, FGameplayAbilitySpecHandle AbilityHandle, int32 PredictionKey, FGuid ExecutionId);
	/** 处理客户端侧的朝向结果消息，沿用当前身份与权限校验。
	 * @param Sequence 序号，由调用方传入。
	 * @param Id 当前记录的唯一标识。
	 * @param bAccepted 是否已接受的控制标记。
	 * @param State 当前状态数据。
	 */
	UFUNCTION(Client, Reliable)
	void ClientFacingResult(uint32 Sequence, FGuid Id, bool bAccepted, FHodgeCharacterRotationState State);

	/** 收到旋转状态复制后更新本地状态。 */
	UFUNCTION()
	void OnRep_RotationState();

	// 恢复转向速率的数值或阈值。
	UPROPERTY(EditDefaultsOnly, Category = "Hodge|Rotation", Meta = (ClampMin = "1", Units = "deg/s"))
	float RecoveryTurnRate = 360.f;
	// 动作转向速率的数值或阈值。
	UPROPERTY(EditDefaultsOnly, Category="Hodge|Rotation", meta=(ClampMin="1", Units="deg/s"))
	float ActionTurnRate = 720.f;
	// 控制是否Allow控制器朝向Requests的状态标记。
	UPROPERTY(EditDefaultsOnly, Category="Hodge|Rotation")
	bool bAllowControllerFacingRequests = true;

	// 能力系统组件的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient)
	TWeakObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;

	// 复制状态的数据或对象引用。
	UPROPERTY(ReplicatedUsing = OnRep_RotationState)
	FHodgeCharacterRotationState ReplicatedState;

	// 本地状态的数据或对象引用。
	FHodgeCharacterRotationState LocalState;
	// 回放状态的数据或对象引用。
	FHodgeCharacterRotationState ReplayState;
	// 旋转标签句柄对应的事件通知。
	FDelegateHandle RotationTagHandle;
	// 移动Stopped标签句柄对应的事件通知。
	FDelegateHandle MovementStoppedTagHandle;
	// 命中受击标签句柄对应的事件通知。
	FDelegateHandle HitReactionTagHandle;
	// 死亡标签句柄对应的事件通知。
	FDelegateHandle DeathTagHandle;
	// 控制是否存在判断回放状态的状态标记。
	bool bHasReplayState = false;
	// 旧移动的硬锁不能从当前受击标签反推。
	bool bReplayExternalYawLocked = false;
	// 控制是否服务器移动Override的状态标记。
	bool bServerMoveOverride = false;
	// 控制是否ResolvingRequests的状态标记。
	bool bResolvingRequests = false;
	// 控制是否Original控制器Yaw角的状态标记。
	bool bOriginalControllerYaw = false;
	// 控制是否OriginalOrient目标移动的状态标记。
	bool bOriginalOrientToMovement = true;
	// 控制是否Original控制器期望的状态标记。
	bool bOriginalControllerDesired = false;
	// 用于拒绝旧Pawn回调的初始化代号。
	int32 AvatarGeneration = 0;
	// 下次请求顺序的数值或阈值。
	uint64 NextRequestOrder = 0;
	// 本地请求序号的数值或阈值。
	uint32 LocalRequestSequence = 0;
	// Acknowledged序号的数值或阈值。
	uint32 AcknowledgedSequence = 0;
	// 上次服务器请求序号的数值或阈值。
	uint32 LastServerRequestSequence = 0;
	// 服务器移动状态的数据或对象引用。
	FHodgeCharacterRotationState ServerMoveState;
	/** 保存请求的数据和接口契约。 */
	struct FRequest
	{
		// 本次操作的来源。
		TWeakObjectPtr<UObject> Source;
		// 本次动作执行的唯一身份。
		FGuid ExecutionId;
		// 当前朝向的驱动来源。
		EHodgeCharacterFacingDriver Driver = EHodgeCharacterFacingDriver::Movement;
		// 状态的枚举配置或当前取值。
		EHodgeFacingRequestStatus Status = EHodgeFacingRequestStatus::Pending;
		// Yaw角的数值或阈值。
		float Yaw = 0.f;
		// 顺序的数值或阈值。
		uint64 Order = 0;
		// 控制是否Instant的状态标记。
		bool bInstant = false;
		// 控制是否远端客户端的状态标记。
		bool bRemoteClient = false;
		// 本端由已确认／预测目标派生，不创建第二条命令授权。
		bool bLocalOnly = false;
		bool bAbilityManaged = false;
		bool bSuspended = false;
		EHodgeFacingExecutionMode ExecutionMode = EHodgeFacingExecutionMode::PhysicalTurn;
	};
	// Requests的记录集合。
	TMap<FGuid, FRequest> Requests;
	// 已完成Requests的记录集合。
	TMap<FGuid, EHodgeFacingRequestStatus> CompletedRequests;
	// 服务器请求历史的记录集合。
	TMap<uint32, FHodgeCharacterRotationState> ServerRequestHistory;
};
