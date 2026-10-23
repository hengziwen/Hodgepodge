/** @file HodgeLockOnComponent.h @brief 当前 Pawn 的锁定事实、命令协调和表现资源所有者。 */
#pragma once
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Targeting/HodgeLockOnTypes.h"
#include "Combat/HodgeCharacterFacingTypes.h"
#include "GameplayEffectTypes.h"
#include "HodgeLockOnComponent.generated.h"
class UHodgeAbilitySystemComponent;
class UHodgePawnExtensionComponent;
class UHodgeLockOnProfile;
class UHodgeCharacterRotationComponent;
class UIndicatorDescriptor;
class UHodgeHealthComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHodgeLockOnChanged, FHodgeLockOnState, Confirmed, AActor*, PresentationTarget, int32, PresentationRevision);
/** 锁定跨越多个技能；短 GA 只输送命令，不持有长期选择。 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeLockOnComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UHodgeLockOnComponent(const FObjectInitializer& Initializer = FObjectInitializer::Get());
	static const FName NAME_ActorFeatureName;
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag Current, FGameplayTag Desired) const override;
	virtual void CheckDefaultInitialization() override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	/** 无目标时获取，有目标时提交显式解除；返回是否成功提交或排队。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|LockOn") bool ToggleLock();
	/** 提交指定候选，返回值不是服务器已接受的证明。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|LockOn") bool TryLockTarget(AActor* Target, FName LockPointId = NAME_None);
	/** 从屏幕上选择左／右邻居；无候选时保留目标。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|LockOn") bool SwitchTarget(int32 Direction);
	/** 用户解除走 GA；权威死亡／销毁清理不依赖能力激活。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|LockOn") void ClearLock(EHodgeLockOnReason Reason = EHodgeLockOnReason::UserClear);
	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") AActor* GetPresentationTarget() const;
	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") AActor* GetConfirmedTarget() const { return ConfirmedState.Target; }
	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") FHodgeLockOnState GetConfirmedState() const { return ConfirmedState; }
	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") bool IsReady() const;
	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") bool IsCommandPending() const { return bHasPending || bHasQueued; }
	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") EHodgeLockOnReason GetLastLocalReason() const { return LastLocalReason; }
	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") const UHodgeLockOnProfile* GetProfile() const { return Profile; }
	/** 表现修订包括预测目标变化；消费者不能只按服务器版本过滤预测事件。 */
	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") int32 GetPresentationRevision() const { return PresentationRevision; }
	UPROPERTY(BlueprintAssignable) FHodgeLockOnChanged OnLockStateChanged;
	/** 记录等效角位移；原始输入仍送 Controller，混合中的模式共享采样而不吞输入。 */
	bool SubmitLookIntent(FVector2D Degrees, bool bAllowTargetSwitch = true);
	/** CMC 内解析角色目标朝向；保留历史名称，不再改写相机 ControlRotation。 */
	bool ResolveControlRotation(FRotator& InOutRotation, float DeltaSeconds) const;
	/** 验证当前目标锚点，失败不输出世界原点。 */
	bool GetPresentationLockPoint(FVector& OutLocation) const;
	/** 每刀起手验证确认目标，不让持久选择冒充有效攻击上下文。 */
	bool IsConfirmedTargetValid() const;
	/** 按动作状态同步本组件独占的基础朝向句柄。 */
	void RefreshFacing();
	/** GA 取走本次命令；只有一个活动激活，其他意图合并到一条队列。 */
	bool TakeCommand(FHodgeLockOnTargetData& OutData);
	/** 仅权威处理复制的载荷，逐项校验身份、版本与资格。 */
	void ProcessAuthorityCommand(const FHodgeLockOnTargetData& Data);
	/** 对应激活结束；下一 Tick 才允许发送排队命令，避免重入活动 GA。 */
	void NotifyCommandEnded(int32 Sequence, bool bCancelled);
	/** GAS 激活拒绝只回收对应初始化代与请求的预测资源。 */
	void RejectPrediction(int32 Sequence, int32 Generation);
	int32 GetGeneration() const { return Generation; }
	/** 旋转组件仅接受来自该组件当前有效状态的锁定占用。 */
	bool CanOwnControllerFacing() const;
	/** 新模式晋升栈顶时继承实际视角，不从角色目标朝向重新开始。 */
	void SeedCameraView(FRotator CurrentView);
	void BeginCameraAcquire() { SoftCamera.BeginAcquire(); }
	/** CameraMode 求值时消费一次本帧输入；临时技能镜头期间不自动构图。 */
	FRotator UpdateCameraView(const FHodgeLockCameraFrame& Frame, float DeltaSeconds, bool bAllowAssist);
	/** 明确的 ASC 解绑入口，重复清理幂等。 */
	void Uninitialize();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
	virtual void OnUnregister() override;
	virtual void TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Function) override;
private:
#if WITH_DEV_AUTOMATION_TESTS
	friend struct FHodgeLockOnTestAccess;
	friend struct FHodgeAttackWarpTestAccess;
#endif
	void InitializeFromExtension();
	void SynchronizePresentation();
	void CheckTargetValidity(double Now);
	bool SubmitCommand(EHodgeLockOnOperation Operation, AActor* Target, FName Point);
	void DispatchQueuedCommand();
	bool IsTargetValid(AActor* Target, FName Point, bool bAcquiring, EHodgeLockOnReason& Reason) const;
	bool HasSight(AActor* Target, FVector Point) const;
	AActor* SelectCandidate(int32 Direction) const;
	void PublishTarget(AActor* Target, FName Point, int32 Sequence, EHodgeLockOnReason Reason, bool bAccepted);
	void HandleConstraint(FGameplayTag Tag, int32 Count);
	UFUNCTION() void HandleTargetDeath(AActor* Actor);
	UFUNCTION() void HandleTargetDestroyed(AActor* Actor);
	UFUNCTION() void OnRep_ConfirmedState();
	// 下行结果是确认回执；上行锁定命令仅通过 GAS TargetData。
	UFUNCTION(Client, Reliable) void ClientCommandResult(FHodgeLockOnState State);
	void ApplyResult(const FHodgeLockOnState& State);
	UPROPERTY(ReplicatedUsing=OnRep_ConfirmedState) FHodgeLockOnState ConfirmedState;
	UPROPERTY(Transient) TObjectPtr<const UHodgeLockOnProfile> Profile;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgePawnExtensionComponent> Extension;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeCharacterRotationComponent> Rotation;
	UPROPERTY(Transient) TObjectPtr<UIndicatorDescriptor> Indicator;
	UPROPERTY(Transient) TWeakObjectPtr<AActor> ObservedTarget;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeHealthComponent> ObservedHealth;
	FHodgeLockOnTargetData Pending, Queued;
	FHodgeFacingRequestHandle FacingHandle;
	FActiveGameplayEffectHandle StateEffect;
	TArray<TPair<FGameplayTag,FDelegateHandle>> ConstraintHandles;
	FHodgeLockOnGesture Gesture;
	FHodgeSoftLockCamera SoftCamera;
	// 每个 Pawn 单独维护请求序号与本地初始化代。
	int32 NextSequence = 0, Generation = 0;
	bool bBound = false, bClosed = false, bHasPending = false, bHasQueued = false, bCommandActive = false;
	bool bCheckingInitialization = false;
	bool bRecheckInitialization = false;
	double PendingAt = 0., NextCheckAt = 0., OutOfRangeAt = -1., OccludedAt = -1., ReferenceWaitAt = -1.;
	EHodgeLockOnReason LastLocalReason = EHodgeLockOnReason::None;
	// 对 RPC 与属性复制的重复回执去重，不重复触发消费者的切换反馈。
	TWeakObjectPtr<AActor> LastBroadcastTarget;
	int32 PresentationRevision = 0, LastBroadcastVersion = -1;
	bool bLastBroadcastPending = false;
	EHodgeLockOnReason LastBroadcastReason = EHodgeLockOnReason::None;
	FString Diagnostic;
};
