#pragma once
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h"
#include "HodgeGameplayAbility_Definition.generated.h"

class UHodgeAbilityDefinition;
class UHodgeWeaponInstance;

/** Executes one granted definition; the combo coordinator owns transitions and input. */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_Definition : public UHodgeGameplayAbility
{
	GENERATED_BODY()

public:
	UHodgeGameplayAbility_Definition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	UFUNCTION(BlueprintPure, Category="Hodge|Ability")
	const UHodgeAbilityDefinition* GetDefinition() const;
	FGameplayTagContainer GetExecutionWindows() const;
	void FinishExecution(bool bCancelled, bool bReplicate);
	void RefreshExecutionClock();
	FGuid GetExecutionId() const { return ExecutionId; }
	bool IsComboCoordinated() const;
	bool IsExecutionEnding() const { return bEnding; }
	virtual void ValidateExecutionConfiguration(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const;
	void ActivateConfirmedDefinition(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	                                 const FPredictionKey& Key, const FGameplayEventData& Payload);
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags = nullptr,
	                                const FGameplayTagContainer* TargetTags = nullptr,
	                                FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
	// 子类在 Timeline 激活前准备任务，窗口进入时即可安全消费结果。
	virtual void OnExecutionReady() {}
	virtual void OnExecutionEnding(const FGuid& EndingExecutionId) {}
	virtual void OnExecutionWindowEntered(int32 EventIndex, FGameplayTag WindowTag) {}
	virtual void OnExecutionWindowExited(int32 EventIndex, bool bSampleFinal) {}
	virtual void OnExecutionPoint(int32 EventIndex, FGameplayTag PointTag) {}
	UHodgeAbilityTask_PlayTimeline* GetExecutionTimeline() const { return TimelineTask; }
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

private:
	// 同一个 GA 实例会重复激活，每次执行使用新的身份。
	FGuid ExecutionId;

	void OnTimelineFinished(EHodgeTimelineStopReason Reason);
	void OnWindowsChanged();
	void OnPoint(FGameplayTag Tag);
	void HandleExecutionPoint(int32 EventIndex, FGameplayTag Tag);
	void HandleExecutionWindowEntered(int32 EventIndex, FGameplayTag WindowTag);
	void HandleExecutionWindowExited(int32 EventIndex, bool bSampleFinal);
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeWeaponInstance> PresentationWeapon;
	TMap<int32, FGuid> WeaponUseHandles;
	UPROPERTY(Transient)
	TObjectPtr<UHodgeAbilityTask_PlayTimeline> TimelineTask;
	bool bEnding = false;
};
