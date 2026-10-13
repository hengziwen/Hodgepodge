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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeReactionResolved, const FHodgeHitReactionResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeReactionStateChanged, const FHodgeHitReactionState&, State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeLightFeedback, FVector, Direction);

/** 目标侧协调命中结算和受击计划，强动作由目标自己的 GA 执行。 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeHitReactionComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
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
	FGuid BeginHit(const FHodgeHitReactionConfig& Request, AActor* Source, FVector SourceOrigin);
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
	/** 保存待处理命中的数据和接口契约。 */
	struct FPendingHit
	{
		// 请求的数据或对象引用。
		FHodgeHitReactionConfig Request;
		// 本次操作的来源。
		TWeakObjectPtr<AActor> Source;
		// 表现角色的弱引用，不延长目标对象生命周期。
		TWeakObjectPtr<AActor> Avatar;
		// Origin的几何或变换数据。
		FVector Origin = FVector::ZeroVector;
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
	};
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
	bool SubmitPlan(FHodgeHitReactionState Plan, EHodgeHitReactionOutcome& Failure);
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
};
