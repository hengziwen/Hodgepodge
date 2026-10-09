#pragma once
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "HodgeGameplayAbility_Definition.generated.h"

class UHodgeAbilityDefinition;
class UHodgeWeaponInstance;
class UHodgeCombatComponentBase;
class UAnimMontage;

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
	bool BeginPredictedMoveCancel();
	void ResolvePredictedMoveCancel(bool bEnd, float ServerPosition, float ServerTime, float ServerPlayRate);
	bool AcceptsNotify(const FBranchingPointNotifyPayload& Payload) const;
	int32 AllocateNotifyOccurrence();
	int32 BeginNotifyResource(const FBranchingPointNotifyPayload& Payload);
	void EndNotifyResource(const FBranchingPointNotifyPayload& Payload);
	void AcquireNotifyTag(int32 OccurrenceId, FGameplayTag Tag);
	void AcquireNotifyWeapon(int32 OccurrenceId);
	void SendExecutionEvent(FGameplayTag Tag);
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
	// 播放蒙太奇前准备执行任务，原生通知可以直接消费本次请求。
	virtual void OnExecutionReady() {}
	virtual void OnExecutionEnding(const FGuid& EndingExecutionId) {}
	virtual void OnNotifyResourceEnded(int32 OccurrenceId) {}
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

private:
	// 同一个 GA 实例会重复激活，每次执行使用新的身份。
	FGuid ExecutionId;

	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted, FGuid Execution, int32 InstanceId);
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, FGuid Execution, int32 InstanceId);
	void NotifyWindowsChanged();
	struct FNotifyResource
	{
		int32 OccurrenceId = INDEX_NONE;
		FGameplayTag Tag;
		FGuid WeaponHandle;
	};
	TMap<const UObject*, FNotifyResource> NotifyResources;
	TMap<FGameplayTag, int32> StateCounts;
	int32 NextOccurrence = 0;
	int32 MontageInstanceId = INDEX_NONE;
	FGuid PoseLease;
	FGameplayTag ExecutionBodyTag;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeCombatComponentBase> ExecutionCombat;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeWeaponInstance> PresentationWeapon;
	bool bEnding = false;
	bool bLifecycleEventSent = false;
	bool bPredictedMoveCancel = false;
};
