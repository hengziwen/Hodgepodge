#pragma once

#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Combat/HodgeHitDetection.h"
#include "HodgeGameplayAbility_Melee.generated.h"

class UHodgeAbilityTask_WaitHitResults;
class UAbilitySystemComponent;

// 按目标 ASC 归并碰撞组件，记录只属于本次 GA 执行。
struct FHodgeMeleeHitHistory
{
	TMap<TWeakObjectPtr<UAbilitySystemComponent>, double> LastHitTimes;
	bool CanHit(UAbilitySystemComponent* Target, double Time, float RepeatInterval) const;
	void RecordHit(UAbilitySystemComponent* Target, double Time);
};

struct FHodgeHitGroupKey
{
	FName Group;
	uint32 TimeKey = 0;
	bool bByTime = false;
	bool operator==(const FHodgeHitGroupKey& Other) const { return Group == Other.Group && TimeKey == Other.TimeKey && bByTime == Other.bByTime; }
	friend uint32 GetTypeHash(const FHodgeHitGroupKey& Key) { return HashCombine(GetTypeHash(Key.Group), HashCombine(Key.TimeKey, uint32(Key.bByTime))); }
};

USTRUCT()
struct FHodgeMeleeWindowState
{
	GENERATED_BODY()
	UPROPERTY() FHodgeHitWindowBinding Binding;
	uint64 SessionHandle = 0;
	int32 LastSampleSequence = 0;
	TSharedPtr<FHodgeMeleeHitHistory> History;
};

/** 近战 GA 接收几何结果，统一决定目标、命中次数和效果应用。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_Melee : public UHodgeGameplayAbility_Definition
{
	GENERATED_BODY()
public:
	virtual void ValidateExecutionConfiguration(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const override;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Combat")
	bool SetHitAnchor(FName Key, const FTransform& WorldTransform);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Combat")
	bool SetHitTarget(FName Key, AActor* Target);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Combat")
	void ResetHitGeometryHistory();

protected:
	virtual void OnExecutionReady() override;
	virtual void OnExecutionEnding(const FGuid& EndingExecutionId) override;
	virtual void OnExecutionWindowEntered(int32 EventIndex, FGameplayTag WindowTag) override;
	virtual void OnExecutionWindowExited(int32 EventIndex, bool bSampleFinal) override;
	virtual void OnExecutionPoint(int32 EventIndex, FGameplayTag PointTag) override;
	UFUNCTION(BlueprintNativeEvent, Category="Hodge|Combat") bool PrepareHitExecutionContext();
	virtual bool PrepareHitExecutionContext_Implementation();

	// 覆盖此事件会替换整批处理，可用于没有 ASC 的交互目标。
	UFUNCTION(BlueprintNativeEvent, Category="Hodge|Melee")
	void ProcessMeleeHitResults(const FHodgeHitDetectionBatch& Batch);
	virtual void ProcessMeleeHitResults_Implementation(const FHodgeHitDetectionBatch& Batch);

	UFUNCTION(BlueprintNativeEvent, Category="Hodge|Melee")
	bool CanApplyMeleeHit(AActor* Target, const FHitResult& Hit, const FHodgeHitWindowBinding& Binding) const;
	virtual bool CanApplyMeleeHit_Implementation(AActor* Target, const FHitResult& Hit,
		const FHodgeHitWindowBinding& Binding) const;

	// 覆盖此事件会替换默认 GE 提交；调用父实现时不要再次提交同一效果。
	UFUNCTION(BlueprintNativeEvent, Category="Hodge|Melee")
	void ApplyMeleeHitEffects(const FHodgeHitDetectionBatch& Batch, const FHitResult& Hit,
		const FHodgeHitWindowBinding& Binding, UAbilitySystemComponent* TargetASC, const FGameplayEffectSpecHandle& Spec);
	virtual void ApplyMeleeHitEffects_Implementation(const FHodgeHitDetectionBatch& Batch, const FHitResult& Hit,
		const FHodgeHitWindowBinding& Binding, UAbilitySystemComponent* TargetASC, const FGameplayEffectSpecHandle& Spec);

	virtual FGameplayEffectContextHandle MakeMeleeHitContext(const FHodgeHitDetectionBatch& Batch, const FHitResult& Hit) const;
	virtual FGameplayEffectSpecHandle BuildMeleeHitSpec(const FHodgeHitDetectionBatch& Batch,
		const FHitResult& Hit, const FHodgeHitWindowBinding& Binding) const;
	bool IsMeleeBatchCurrent(const FHodgeHitDetectionBatch& Batch) const;
	bool OpenHit(int32 EventIndex, const FHodgeHitWindowBinding& Binding);

private:
	friend struct FHodgeMeleeTestAccess;
	void OnHitResults(const FHodgeHitDetectionBatch& Batch);
	UPROPERTY(Transient) TObjectPtr<UHodgeAbilityTask_WaitHitResults> DetectionTask;
	UPROPERTY(Transient) TMap<int32, FHodgeMeleeWindowState> WindowStates;
	// 共享组记录保留到 GA 执行结束，允许相邻或不相邻窗口共用次数。
	TMap<FHodgeHitGroupKey, TSharedPtr<FHodgeMeleeHitHistory>> HitGroups;
	UPROPERTY(Transient) TMap<FName, FTransform> HitAnchors;
	UPROPERTY(Transient) TMap<FName, TWeakObjectPtr<AActor>> HitTargets;
	TSet<int32> ConsumedPoints;
};
