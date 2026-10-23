/** @file HodgeGameplayAbility_Melee.h
 * @brief 声明玩法能力Melee相关的类型、接口与现有处理流程。
 */
#pragma once

#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Combat/HodgeHitDetection.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "Combat/HodgeAttackRule.h"
#include "HodgeGameplayAbility_Melee.generated.h"

class UHodgeAbilityTask_WaitHitResults;
class UAbilitySystemComponent;

// 按目标 ASC 归并碰撞组件，记录只属于本次 GA 执行。
struct FHodgeMeleeHitHistory
{
	// 上次命中Times的弱引用，不延长目标对象生命周期。
	TMap<TWeakObjectPtr<UAbilitySystemComponent>, double> LastHitTimes;
	/** 根据目标资格与当前检测会话判断是否允许命中。
	 * @param Target 本次操作的目标。
	 * @param Time 当前处理使用的时间值。
	 * @param RepeatInterval RepeatInterval，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool CanHit(UAbilitySystemComponent* Target, double Time, float RepeatInterval) const;
	/** 记录本次检测会话已命中的目标以供去重。
	 * @param Target 本次操作的目标。
	 * @param Time 当前处理使用的时间值。
	 */
	void RecordHit(UAbilitySystemComponent* Target, double Time);
};

/** 保存Melee命中状态的数据和接口契约。 */
USTRUCT()
struct FHodgeMeleeHitState
{
	GENERATED_BODY()
	// 绑定的数据或对象引用。
	UPROPERTY() FHodgeHitEffectConfig Binding;
	// 会话句柄的数值或阈值。
	uint64 SessionHandle = 0;
	// 上次采样序号的数值或阈值。
	int32 LastSampleSequence = 0;
	// 历史的数据或对象引用。
	TSharedPtr<FHodgeMeleeHitHistory> History;
};

/** 近战 GA 接收几何结果，统一决定目标、命中次数和效果应用。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_Melee : public UHodgeGameplayAbility_Definition
{
	GENERATED_BODY()
public:
	/** 构造玩法能力Melee并初始化其默认状态。 */
	UHodgeGameplayAbility_Melee();
	/** 校验执行Configuration。
	 * @param Definition 当前操作使用的定义配置。
	 * @param Errors 收集配置或执行失败的诊断。
	 */
	virtual void ValidateExecutionConfiguration(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const override;
	/** 设置命中锚点。
	 * @param Key 键，由调用方传入。
	 * @param WorldTransform 世界变换，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Combat")
	bool SetHitAnchor(FName Key, const FTransform& WorldTransform);
	/** 设置命中目标。
	 * @param Key 键，由调用方传入。
	 * @param Target 本次操作的目标。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Combat")
	bool SetHitTarget(FName Key, AActor* Target);
	/** 重置命中几何历史。 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Combat")
	void ResetHitGeometryHistory();

	/** 开始通知命中。
	 * @param OccurrenceId 通知次数标识，由调用方传入。
	 * @param Config 配置，由调用方传入。
	 * @param Mesh 网格，由调用方传入。
	 * @param bSingle 是否Single的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool BeginNotifyHit(int32 OccurrenceId, const FHodgeAnimHitConfig& Config, USkeletalMeshComponent* Mesh, bool bSingle);

protected:
	/** 处理执行就绪。 */
	virtual void OnExecutionReady() override;
	/** 处理执行Ending。
	 * @param EndingExecutionId Ending执行标识，由调用方传入。
	 */
	virtual void OnExecutionEnding(const FGuid& EndingExecutionId) override;
	/** 处理通知资源结束。
	 * @param OccurrenceId 通知次数标识，由调用方传入。
	 */
	virtual void OnNotifyResourceEnded(int32 OccurrenceId) override;
	/** 建立当前服务器命中执行所需的配置与身份。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Hodge|Combat") bool PrepareHitExecutionContext();
	/** 建立当前服务器命中执行所需的配置与身份。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool PrepareHitExecutionContext_Implementation();

	// 覆盖此事件会替换整批处理，可用于没有 ASC 的交互目标。
	/** 消费检测得到的命中结果并提交允许的命中效果。
	 * @param Batch Batch，由调用方传入。
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Hodge|Melee")
	void ProcessMeleeHitResults(const FHodgeHitDetectionBatch& Batch);
	/** 消费检测得到的命中结果并提交允许的命中效果。
	 * @param Batch Batch，由调用方传入。
	 */
	virtual void ProcessMeleeHitResults_Implementation(const FHodgeHitDetectionBatch& Batch);

	/** 检查该候选命中是否满足目标与执行约束。
	 * @param Target 本次操作的目标。
	 * @param Hit 命中，由调用方传入。
	 * @param Binding 绑定，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Hodge|Melee")
	bool CanApplyMeleeHit(AActor* Target, const FHitResult& Hit, const FHodgeHitEffectConfig& Binding) const;
	/** 检查该候选命中是否满足目标与执行约束。
	 * @param Target 本次操作的目标。
	 * @param Hit 命中，由调用方传入。
	 * @param Binding 绑定，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool CanApplyMeleeHit_Implementation(AActor* Target, const FHitResult& Hit,
		const FHodgeHitEffectConfig& Binding) const;

	// 覆盖此事件会替换默认 GE 提交；调用父实现时不要再次提交同一效果。
	/** 按命中配置提交伤害效果与附带反应。
	 * @param Batch Batch，由调用方传入。
	 * @param Hit 命中，由调用方传入。
	 * @param Binding 绑定，由调用方传入。
	 * @param TargetASC 目标ASC，由调用方传入。
	 * @param Spec 规格，由调用方传入。
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Hodge|Melee")
	void ApplyMeleeHitEffects(const FHodgeHitDetectionBatch& Batch, const FHitResult& Hit,
		const FHodgeHitEffectConfig& Binding, UAbilitySystemComponent* TargetASC, const FGameplayEffectSpecHandle& Spec);
	/** 提交Melee命中Effects。
	 * @param Batch Batch，由调用方传入。
	 * @param Hit 命中，由调用方传入。
	 * @param Binding 绑定，由调用方传入。
	 * @param TargetASC 目标ASC，由调用方传入。
	 * @param Spec 规格，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Melee")
	void SubmitMeleeHitEffects(const FHodgeHitDetectionBatch& Batch, const FHitResult& Hit,
		const FHodgeHitEffectConfig& Binding, UAbilitySystemComponent* TargetASC, const FGameplayEffectSpecHandle& Spec);
	/** 处理Melee受击Resolved。
	 * @param Target 本次操作的目标。
	 * @param Result 本次操作的结果。
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Hodge|Melee")
	void OnMeleeReactionResolved(AActor* Target, const FHodgeHitReactionResult& Result);
	/** 按命中配置提交伤害效果与附带反应。
	 * @param Batch Batch，由调用方传入。
	 * @param Hit 命中，由调用方传入。
	 * @param Binding 绑定，由调用方传入。
	 * @param TargetASC 目标ASC，由调用方传入。
	 * @param Spec 规格，由调用方传入。
	 */
	virtual void ApplyMeleeHitEffects_Implementation(const FHodgeHitDetectionBatch& Batch, const FHitResult& Hit,
		const FHodgeHitEffectConfig& Binding, UAbilitySystemComponent* TargetASC, const FGameplayEffectSpecHandle& Spec);

	/** 构造Melee命中上下文。
	 * @param Batch Batch，由调用方传入。
	 * @param Hit 命中，由调用方传入。
	 * @return 本次操作的句柄，调用方应按现有接口检查有效性。
	 */
	virtual FGameplayEffectContextHandle MakeMeleeHitContext(const FHodgeHitDetectionBatch& Batch, const FHitResult& Hit) const;
	/** 根据本次命中上下文构造实际效果规格。
	 * @param Batch Batch，由调用方传入。
	 * @param Hit 命中，由调用方传入。
	 * @param Binding 绑定，由调用方传入。
	 * @return 本次操作的句柄，调用方应按现有接口检查有效性。
	 */
	virtual FGameplayEffectSpecHandle BuildMeleeHitSpec(const FHodgeHitDetectionBatch& Batch,
		const FHitResult& Hit, const FHodgeHitEffectConfig& Binding) const;
	/** 判断MeleeBatch当前。
	 * @param Batch Batch，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsMeleeBatchCurrent(const FHodgeHitDetectionBatch& Batch) const;
	/** 打开命中。
	 * @param OccurrenceId 通知次数标识，由调用方传入。
	 * @param Binding 绑定，由调用方传入。
	 * @param Request 请求，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool OpenHit(int32 OccurrenceId, const FHodgeHitEffectConfig& Binding, const FHodgeHitDetectionRequest& Request);

private:
	friend struct FHodgeMeleeTestAccess;
	/** 处理命中Results。
	 * @param Batch Batch，由调用方传入。
	 */
	void OnHitResults(const FHodgeHitDetectionBatch& Batch);
	// 检测任务的数据或对象引用。
	UPROPERTY(Transient) TObjectPtr<UHodgeAbilityTask_WaitHitResults> DetectionTask;
	// 命中States的记录集合。
	UPROPERTY(Transient) TMap<int32, FHodgeMeleeHitState> HitStates;
	// 共享组记录保留到 GA 执行结束，允许相邻或不相邻窗口共用次数。
	TMap<FHodgeHitGroupKey, TSharedPtr<FHodgeMeleeHitHistory>> HitGroups;
	// 命中Anchors的记录集合。
	UPROPERTY(Transient) TMap<FName, FTransform> HitAnchors;
	// 命中目标集合的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TMap<FName, TWeakObjectPtr<AActor>> HitTargets;
};
