#pragma once
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h"
#include "HodgeGameplayAbility_Definition.generated.h"

class UHodgeAbilityDefinition;

/** Executes one granted definition; the combo coordinator owns transitions and input. */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_Definition : public UHodgeGameplayAbility
{
	GENERATED_BODY()
public:
	UHodgeGameplayAbility_Definition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	UFUNCTION(BlueprintPure, Category="Hodge|Ability") const UHodgeAbilityDefinition* GetDefinition() const;
	FGameplayTagContainer GetExecutionWindows() const;
	void FinishExecution(bool bCancelled, bool bReplicate);
	void RefreshExecutionClock();
	void ActivateConfirmedDefinition(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		const FPredictionKey& Key, const FGameplayEventData& Payload);
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
protected:
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
private:
	void OnTimelineFinished(EHodgeTimelineStopReason Reason);
	void OnWindowsChanged();
	void OnPoint(FGameplayTag Tag);
	UPROPERTY(Transient) TObjectPtr<UHodgeAbilityTask_PlayTimeline> TimelineTask;
	bool bEnding = false;
};
