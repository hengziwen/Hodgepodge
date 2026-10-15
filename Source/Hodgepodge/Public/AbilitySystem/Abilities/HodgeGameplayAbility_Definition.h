/** @file HodgeGameplayAbility_Definition.h
 * @brief 声明玩法能力定义相关的类型、接口与现有处理流程。
 */
#pragma once
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Equipment/HodgeWeaponPresentationTypes.h"
#include "Combat/HodgeAttackFacingTypes.h"
#include "Combat/HodgeCharacterFacingTypes.h"
#include "TimerManager.h"
#include "HodgeGameplayAbility_Definition.generated.h"

class UHodgeAbilityDefinition;
class UHodgeWeaponInstance;
class UHodgeCombatComponentBase;
class UAnimMontage;

/** Executes one granted definition; the combo coordinator owns transitions and input. */
/** 封装玩法能力定义的状态与操作接口。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_Definition : public UHodgeGameplayAbility
{
	GENERATED_BODY()

public:
	/** 采样锁定、移动意图或镜头水平朝向，供预测与服务器发起的激活使用。 */
	static bool BuildAttackFacingData(const FGameplayAbilityActorInfo* Info, FHodgeAttackFacingTargetData& OutData);
	/** 依据实际 Montage 位置补偿被跳过的恢复通知。 */
	void UpdateAttackFacingLifecycle();
	bool IsFacingExecutionSuspended() const { return bPredictedMoveCancel; }
	bool IsFacingAuthorityRetired() const { return bAttackFacingRetired; }
	bool IsAwaitingFacingData() const { return bFacingDataWaiting && !bEnding; }
	UFUNCTION(BlueprintPure, Category="Hodge|Facing") float GetAttackFacingYaw() const { return FacingTargetYaw; }
	/** 本刀起手的确认锁定目标快照，不强制吸附或限制真实命中。 */
	UFUNCTION(BlueprintPure, Category="Hodge|Targeting") AActor* GetExecutionLockTarget() const { return ExecutionLockTarget.Get(); }
	/** 构造玩法能力定义并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeGameplayAbility_Definition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/** 获取定义。
	 * @return 定义的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Ability")
	const UHodgeAbilityDefinition* GetDefinition() const;
	/** 获取执行Windows。
	 * @return 执行Windows的当前值或引用。
	 */
	FGameplayTagContainer GetExecutionWindows() const;
	/** 在仍活动时按指定结束方式结束当前执行。
	 * @param bCancelled 是否已取消的控制标记。
	 * @param bReplicate 是否Replicate的控制标记。
	 */
	void FinishExecution(bool bCancelled, bool bReplicate);
	/** 开始Predicted移动取消。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool BeginPredictedMoveCancel();
	/** 解析Predicted移动取消。
	 * @param bEnd 是否结束的控制标记。
	 * @param ServerPosition 服务器位置，由调用方传入。
	 * @param ServerTime 服务器时间，由调用方传入。
	 * @param ServerPlayRate 服务器Play速率，由调用方传入。
	 */
	void ResolvePredictedMoveCancel(bool bEnd, float ServerPosition, float ServerTime, float ServerPlayRate);
	/** 校验通知是否属于当前Avatar、蒙太奇实例与执行身份。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool AcceptsNotify(const FBranchingPointNotifyPayload& Payload) const;
	/** 为本次执行分配通知次数身份。
	 * @return 本次计算、查询或创建得到的整数32。
	 */
	int32 AllocateNotifyOccurrence();
	/** 为被当前执行接受的通知建立资源记录。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 * @return 本次计算、查询或创建得到的整数32。
	 */
	int32 BeginNotifyResource(const FBranchingPointNotifyPayload& Payload);
	/** 按通知记录释放其标签、手持或检测资源。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 */
	void EndNotifyResource(const FBranchingPointNotifyPayload& Payload);
	/** 为本次通知申请执行期间的状态Tag。
	 * @param OccurrenceId 通知次数标识，由调用方传入。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 */
	void AcquireNotifyTag(int32 OccurrenceId, FGameplayTag Tag);
	/** 为本次通知申请武器手持资源。
	 * @param OccurrenceId 通知次数标识，由调用方传入。
	 */
	void AcquireNotifyWeapon(int32 OccurrenceId, EHodgeWeaponHandEndBehavior EndBehavior = EHodgeWeaponHandEndBehavior::ReturnToBack);
	/** 按当前执行身份发送动作语义事件。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 */
	void SendExecutionEvent(FGameplayTag Tag);
	/** 获取执行标识。
	 * @return 执行标识的当前值或引用。
	 */
	FGuid GetExecutionId() const { return ExecutionId; }
	/** 精确读取当前主实例；预测取消、结束或旧Avatar时返回false。 */
	bool GetCinematicSource(UAnimMontage*& OutMontage, int32& OutInstanceId, float& OutPosition) const;
	/** 判断连段Coordinated。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsComboCoordinated() const;
	/** 判断执行Ending。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsExecutionEnding() const { return bEnding; }
	/** 校验执行Configuration。
	 * @param Definition 当前操作使用的定义配置。
	 * @param Errors 收集配置或执行失败的诊断。
	 */
	virtual void ValidateExecutionConfiguration(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const;
	/** 激活Confirmed定义。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param Key 键，由调用方传入。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 */
	void ActivateConfirmedDefinition(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	                                 const FPredictionKey& Key, const FGameplayEventData& Payload);
	/** 检查技能激活所需的上下文、标签与现有GAS限制。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param SourceTags 来源标签，用于对应标签查询或匹配。
	 * @param TargetTags 目标标签，用于对应标签查询或匹配。
	 * @param OptionalRelevantTags OptionalRelevant标签，用于对应标签查询或匹配。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags = nullptr,
	                                const FGameplayTagContainer* TargetTags = nullptr,
	                                FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
	// 播放蒙太奇前准备执行任务，原生通知可以直接消费本次请求。
	/** 处理执行就绪。 */
	virtual void OnExecutionReady() {}
	/** 合法动画通知的语义扩展，不另建不带执行身份的全局事件总线。 */
	virtual void OnExecutionEvent(FGameplayTag Tag) {}
	/** 处理执行Ending。
	 * @param EndingExecutionId Ending执行标识，由调用方传入。
	 */
	virtual void OnExecutionEnding(const FGuid& EndingExecutionId) {}
	/** 处理通知资源结束。
	 * @param OccurrenceId 通知次数标识，由调用方传入。
	 */
	virtual void OnNotifyResourceEnded(int32 OccurrenceId) {}
	/** 启动本次技能，按现有配置提交消耗并建立执行资源。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 * @param TriggerEventData Trigger事件数据，由调用方传入。
	 */
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	/** 结束本次技能并释放其拥有的资源，再交还GAS生命周期。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 * @param bReplicateEndAbility 是否Replicate结束能力的控制标记。
	 * @param bWasCancelled 是否Was已取消的控制标记。
	 */
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

private:
	friend struct FHodgeAttackWarpTestAccess;
	void StartExecutionMontage();
	void InitializeAttackFacing(const FGameplayEventData* Payload);
	void OnAttackFacingData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ApplicationTag, FGuid ForExecution);
	bool ApplyAttackFacing(const FHodgeAttackFacingTargetData& Data);
	void OnAttackFacingTimeout(FGuid ForExecution);
	void ReleaseAttackFacing();
	FDelegateHandle FacingDataDelegate;
	FTimerHandle FacingDataTimer;
	FHodgeFacingRequestHandle AttackFacingHandle;
	float FacingTargetYaw = 0.f;
	bool bFacingDataWaiting = false;
	bool bFacingDataProcessed = false;
	bool bActivationReturned = false;
	bool bAttackFacingRetired = false;
	friend struct FHodgeCombatActionTestAccess;
	// 同一个 GA 实例会重复激活，每次执行使用新的身份。
	// 每次执行独立采样，切换全局锁定不修改本刀快照。
	UPROPERTY(Transient) TWeakObjectPtr<AActor> ExecutionLockTarget;
	FGuid ExecutionId;

	/** 处理蒙太奇Blending输出。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @param bInterrupted 是否Interrupted的控制标记。
	 * @param Execution 执行，由调用方传入。
	 * @param InstanceId 用于区分运行实例的标识。
	 */
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted, FGuid Execution, int32 InstanceId);
	/** 处理蒙太奇结束。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @param bInterrupted 是否Interrupted的控制标记。
	 * @param Execution 执行，由调用方传入。
	 * @param InstanceId 用于区分运行实例的标识。
	 */
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, FGuid Execution, int32 InstanceId);
	/** 通知Windows变化。 */
	void NotifyWindowsChanged();
	/** 保存通知资源的数据和接口契约。 */
	struct FNotifyResource
	{
		// 通知次数标识的数值或阈值。
		int32 OccurrenceId = INDEX_NONE;
		// 标签的数据或对象引用。
		FGameplayTag Tag;
		// 武器句柄的数据或对象引用。
		FGuid WeaponHandle;
	};
	// 通知Resources的记录集合。
	TMap<const UObject*, FNotifyResource> NotifyResources;
	// 状态Counts的记录集合。
	TMap<FGameplayTag, int32> StateCounts;
	// 下次通知次数的数值或阈值。
	int32 NextOccurrence = 0;
	// 蒙太奇Instance标识的数值或阈值。
	int32 MontageInstanceId = INDEX_NONE;
	// 本次动作持有的姿势占用句柄。
	FGuid PoseLease;
	// 执行体状态标签的数据或对象引用。
	FGameplayTag ExecutionBodyTag;
	// 执行战斗的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeCombatComponentBase> ExecutionCombat;
	// 表现武器的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeWeaponInstance> PresentationWeapon;
	// 控制是否Ending的状态标记。
	bool bEnding = false;
	// 控制是否Lifecycle事件Sent的状态标记。
	bool bLifecycleEventSent = false;
	// 控制是否Predicted移动取消的状态标记。
	bool bPredictedMoveCancel = false;
};
