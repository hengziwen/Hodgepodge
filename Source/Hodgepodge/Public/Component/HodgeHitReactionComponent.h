/** @file HodgeHitReactionComponent.h
 * @brief 声明受击状态组件相关的类型、接口与现有处理流程。
 */
#pragma once

#include "Components/PawnComponent.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "HodgeHitReactionComponent.generated.h"

class UHodgeAbilitySystemComponent;
class UHodgeHitReactionProfile;
class UHodgeGameplayAbility_HitReaction;
class UAbilitySystemComponent;
class UHodgeCombatComponentBase;
class UHodgePawnData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeReactionResolved, const FHodgeHitReactionResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeReactionStateChanged, const FHodgeHitReactionState&, State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeLightFeedback, FVector, Direction);

/** 目标侧协调命中结算和受击计划，强动作由目标自己的 GA 执行。 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeHitReactionComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	/** 返回权威补转段；客户端只消费复制值，不能提交目标角或有效时间。 */
	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction")
	FHodgeHitReactionFacingState GetFacingState() const { return FacingState; }
	/** 旧段及旧 Avatar 的回放不能取得当前控制许可。 */
	bool IsFacingStateCurrent(const FGuid& ChainId, uint32 Segment) const;
	/** 局部动画停顿也暂停可选补转及校正，但不延长服务器截止时间。 */
	bool IsFacingPaused() const;
	/** 仅 CMC 已执行的同段结果可冻结服务器状态，不接受客户端请求。 */
	void FinishFacingSegment(const FGuid& ChainId, uint32 Segment, uint32 Revision, float FinalYaw, bool bHardLocked);
	/** 构造受击状态组件并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeHitReactionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/** 初始化关联能力System。
	 * @param InASC 后续GAS操作使用的能力系统组件。
	 */
	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
	/** 解除初始化来源能力System。 */
	void UninitializeFromAbilitySystem();
	/** 刷新Configuration。 */
	void RefreshConfiguration();
	/** 获取受击状态。
	 * @return 受击状态的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") FHodgeHitReactionState GetReactionState() const { return State; }
	/** 获取上次结果。
	 * @return 上次结果的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") FHodgeHitReactionResult GetLastResult() const { return LastResult; }
	/** 服务器诊断值；客户端不反推权威削韧，不复制时间窗队列。 */
	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") float GetAccumulatedPoise() const;
	/** 死亡、解绑或显式重置目标时清空积累与保护，不取消其他系统动作。 */
	void ResetPoise();
	/** 当前 Avatar 已接受的独立表现次数，供跨端核对，不参与结算。 */
	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") int32 GetPresentationCount() const { return PresentationCount; }
	/** 最近一次接触快照不代替逐事件日志，同帧多接触以日志为准。 */
	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") FHodgeHitPresentation GetLastPresentation() const { return LastPresentation; }
	/** 判断Controlled。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") bool IsControlled() const { return State.IsActive(); }
	/** 获取配置。
	 * @return 配置的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") const UHodgeHitReactionProfile* GetProfile() const { return Profile; }
	// 用于不通过玩家 PawnData 生成的目标，仍必须先绑定合法 ASC。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|HitReaction") TObjectPtr<UHodgeHitReactionProfile> OverrideProfile;
	// 事件受击Resolved对应的事件通知。
	UPROPERTY(BlueprintAssignable) FHodgeReactionResolved OnReactionResolved;
	// 事件受击状态变化对应的事件通知。
	UPROPERTY(BlueprintAssignable) FHodgeReactionStateChanged OnReactionStateChanged;
	// 事件LightFeedback对应的事件通知。
	UPROPERTY(BlueprintAssignable) FHodgeLightFeedback OnLightFeedback;
	/** 请求获取Up。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|HitReaction") bool RequestGetUp();
	/** 检查是否允许cel受击。 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|HitReaction") void CancelReaction();
	/** 开始命中。
	 * @param Request 请求，由调用方传入。
	 * @param Source 本次操作的来源。
	 * @param SourceOrigin 来源Origin，由调用方传入。
	 * @return 本次计算、查询或创建得到的唯一标识。
	 */
	FGuid BeginHit(const FHodgeHitReactionConfig& Request, AActor* Source, FVector SourceOrigin,
		const FHitResult* Contact = nullptr, FGuid ExecutionId = FGuid(), FName AttackPhase = NAME_None);
	/** 记录本次采样观察到的伤害结果。
	 * @param HitId 命中标识，由调用方传入。
	 * @param bAccepted 是否已接受的控制标记。
	 * @param ActualDamage Actual伤害，由调用方传入。
	 */
	void RecordDamage(const FGuid& HitId, bool bAccepted, float ActualDamage);
	/** 结束命中。
	 * @param HitId 命中标识，由调用方传入。
	 * @return 本次计算、查询或创建得到的命中受击结果。
	 */
	FHodgeHitReactionResult FinishHit(const FGuid& HitId);
	/** 接入当前来源授予的技能记录。
	 * @param Ability 能力，由调用方传入。
	 */
	void AttachAbility(UHodgeGameplayAbility_HitReaction* Ability);
	/** 解除当前来源对应的能力关联。
	 * @param Ability 能力，由调用方传入。
	 */
	void DetachAbility(UHodgeGameplayAbility_HitReaction* Ability);
	/** 接管已释放控制的恢复尾部；根运动仍由 CMC 消费，保存具体实例身份。 */
	bool RetirePresentation(UAnimMontage* Montage, int32 InstanceId);
	/** 新强反应或解绑只停止本组件持有的旧视觉实例。 */
	void StopRetiredPresentation();
	/** 查询本 Avatar 当前地面硬直的恢复资格，不放行死亡、击退或空中状态。 */
	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") bool CanRecover(EHodgeHitRecoveryAction Action) const;
	/** 仅供成功提交的行动在服务器交接；迟到身份不得解除新受击。 */
	bool TryRecover(EHodgeHitRecoveryAction Action, int32 ExpectedSequence);
	/** 完成Plan。
	 * @param Sequence 序号，由调用方传入。
	 */
	void CompletePlan(int32 Sequence);
	/** 更新Plan。 */
	void TickPlan();
	/** 获取服务器时间。
	 * @return 服务器时间的当前值或引用。
	 */
	float GetServerTime() const;
	/** 登记本类型需要网络复制的属性及复制条件。
	 * @param OutLifetimeProps 输出LifetimeProps。
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/** 更新组件。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 * @param TickType 帧更新类型，由调用方传入。
	 * @param ThisTickFunction This帧更新Function，由调用方传入。
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/** 离开世界运行阶段并解除本对象持有的监听或资源。
	 * @param EndPlayReason 结束PlayReason，由调用方传入。
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	friend struct FHodgeHitReactionTestAccess;
	friend struct FHodgeHitReactionFacingMovementTestAccess;
	/** 保存待处理命中的数据和接口契约。 */
	struct FPendingHit
	{
		// 请求的数据或对象引用。
		FHodgeHitReactionConfig Request;
		// 同一结算的接触快照，只有确认命中后才发送表现。
		FHodgeHitPresentation Presentation;
		// 本次操作的来源。
		TWeakObjectPtr<AActor> Source;
		// 表现角色的弱引用，不延长目标对象生命周期。
		TWeakObjectPtr<AActor> Avatar;
		// Origin的几何或变换数据。
		FVector Origin = FVector::ZeroVector;
		// 仅供补转使用，来源销毁后仍保留命中时的有效值。
		FHodgeHitReactionFacingSource FacingSource;
		// 体状态标签的数据或对象引用。
		FGameplayTag BodyTag;
		// 体状态Rank的数值或阈值。
		int32 BodyRank = 0;
		// 伤害的数值或阈值。
		float Damage = 0.f;
		// 控制是否已接受的状态标记。
		bool bAccepted = false;
		// 控制是否Rejected的状态标记。
		bool bRejected = false;
		// 仅内部目标裁决设置，攻击方不能通过此字段直接选择等级。
		EHodgeReactionLevel ReactionLevel = EHodgeReactionLevel::None;
	};
	/** 只记录服务器已接受的正值样本，数量由目标配置限制。 */
	struct FPoiseSample { float Time = 0.f; float Amount = 0.f; };
	TArray<FPoiseSample> PoiseSamples;
	EHodgeReactionLevel ConsumedPoiseLevel = EHodgeReactionLevel::None;
	uint32 PoiseGeneration = 0;
	FTimerHandle PoiseTimer;
	FDelegateHandle PoiseDeathHandle;
	FDelegateHandle PoiseHealthHandle;
	TWeakObjectPtr<const UHodgePawnData> PoisePawnData;
	/** 计时器只清理过期样本，永远不自动触发控制。 */
	void ExpirePoise(float Now);
	void SchedulePoiseExpiry();
	/** 将合格的一次结算转为一个目标计划，失败不消费阈值。 */
	void ResolvePoise(FPendingHit Hit, FHodgeHitReactionResult& Result);
	/** 分段开关只提交一次 Fly；空中重播沿用原控制链与续飞次数上限。 */
	void ResolveSegmentLaunch(FPendingHit Hit, FHodgeHitReactionResult& Result);
	// 待处理Hits的记录集合。
	TMap<FGuid, FPendingHit> PendingHits;
	// 后续GAS操作使用的能力系统组件。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
	// 活动能力的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeGameplayAbility_HitReaction> ActiveAbility;
	// 当前操作使用的配置对象。
	UPROPERTY(ReplicatedUsing=OnRep_Profile) TObjectPtr<UHodgeHitReactionProfile> Profile;
	// 当前状态数据。
	UPROPERTY(ReplicatedUsing=OnRep_State) FHodgeHitReactionState State;
	// 与动画计划分别发布，GA 激活或同步失败时不能泄漏候选许可。
	UPROPERTY(ReplicatedUsing=OnRep_FacingState) FHodgeHitReactionFacingState FacingState;
	// 生命周期代只约束可选补转，不能改变既有伤害及受击裁决。
	uint32 FacingGeneration = 1;
	// 上次结果的数据或对象引用。
	UPROPERTY(Transient) FHodgeHitReactionResult LastResult;
	// 下次序号的数值或阈值。
	int32 NextSequence = 0;
	// 控制是否Owns控制的状态标记。
	bool bOwnsControl = false;
	// 正在提交，暂不处理缺少请求身份的同步通知。
	bool bSubmitting = false;
	// 控制是否Completing的状态标记。
	bool bCompleting = false;
	// 控制是否SawAirborne的状态标记。
	bool bSawAirborne = false;
	// 客户端只保留有限命中身份，解绑时清空，避免重复副作用与无限增长。
	TArray<FGuid> PresentedHits;
	UPROPERTY(Transient) TWeakObjectPtr<UAnimMontage> LastLightMontage;
	// 恢复尾部不持有控制或收益；可继续动画根运动，同资产的新实例不受旧记录影响。
	UPROPERTY(Transient) TWeakObjectPtr<UAnimMontage> RetiredMontage;
	int32 RetiredMontageInstanceId = INDEX_NONE;
	// 动画更新被可见性策略暂停时，尾部也必须有有限的收尾期限。
	float RetiredExpiresAt = 0.f;
	// 根运动尾部在权威端继续持有骨骼更新租约，不能随受击 GA 提前释放。
	FGuid RetiredPoseLease;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeCombatComponentBase> RetiredPoseCombat;
	float LastLightStrength = 0.f;
	int32 PresentationCount = 0;
	UPROPERTY(Transient) FHodgeHitPresentation LastPresentation;
	/** 判断已绑定。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsBound() const;
	/** 构建Plan。
	 * @param Hit 命中，由调用方传入。
	 * @param Plan Plan，由调用方传入。
	 * @param Applied 已应用，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool BuildPlan(const FPendingHit& Hit, FHodgeHitReactionState& Plan, TArray<EHodgeImpactType>& Applied) const;
	/** 提交Plan。
	 * @param Plan Plan，由调用方传入。
	 * @param Failure 失败，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool SubmitPlan(FHodgeHitReactionState Plan, EHodgeHitReactionOutcome& Failure,
		const FHodgeHitReactionFacingSource& FacingSource = FHodgeHitReactionFacingSource());
	/** 在原 GA 提交成功后建立或升级补转段，同级不重取来源。 */
	void CommitFacing(const FHodgeHitReactionFacingSource& Source, const FHodgeHitReactionState& Previous, bool bWasAir);
	/** 纯身份核验供冻结记录使用；记录仍在不代表拥有执行许可。 */
	bool IsFacingIdentityCurrent(const FGuid& ChainId, uint32 Segment) const;
	/** 先使段身份失效，再通知仲裁和移动缓存释放，避免旧状态复活。 */
	void ClearFacing(bool bInvalidatePending = false);
	/** 复制只通知现有仲裁链，不在 OnRep 中写 Actor 朝向。 */
	UFUNCTION() void OnRep_FacingState();
	/** 设置控制。
	 * @param bControlled 是否Controlled的控制标记。
	 */
	void SetControl(bool bControlled);
	/** 发布本轮已经提交的状态快照。 */
	void PublishState();
	/** 切换当前状态流程的执行阶段。
	 * @param Phase Phase，由调用方传入。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @param Duration 持续时间，由调用方传入。
	 */
	void ChangePhase(EHodgeHitReactionPhase Phase, UAnimMontage* Montage, float Duration);
	/** 收到状态复制后更新本地状态。 */
	UFUNCTION() void OnRep_State();
	/** 收到配置复制后更新本地状态。 */
	UFUNCTION() void OnRep_Profile();
	/** 把轻受击表现通知到网络观察者。
	 * @param Direction 方向，由调用方传入。
	 */
	UFUNCTION(NetMulticast, Unreliable) void MulticastLightFeedback(FVector Direction);
	/** 只分发表现，不改变伤害、受控状态或权威位移。 */
	UFUNCTION(NetMulticast, Unreliable) void MulticastContactFeedback(const FHodgeHitPresentation& Presentation);
	/** 局部动作和通知分开；抑制重播时仍保留这次接触事件。 */
	void PlayLightFeedback(FVector Direction, FVector LocalSourceDirection, float Strength);
};
