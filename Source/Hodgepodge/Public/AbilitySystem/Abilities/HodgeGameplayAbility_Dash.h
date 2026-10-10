#pragma once

#include "AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h"
#include "HodgeGameplayAbility_Dash.generated.h"
class UAbilityTask_ApplyRootMotionConstantForce;
class UAbilityTask_PlayMontageAndWait;

UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_Dash : public UHodgeGameplayAbility_MovementAction
{
	GENERATED_BODY()
public:
	UHodgeGameplayAbility_Dash();
	UFUNCTION(BlueprintPure) FHodgeDashDirections GetDashDirections() const { return Directions; }
	void CompleteHandoff();
	virtual bool CheckCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayTagContainer* Tags = nullptr) const override;
protected:
	virtual void BeginMovement(const FHodgeMoveIntentSnapshot& Intent) override;
	virtual void UpdateMovement(float DeltaTime) override;
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
private:
	void CommitDash();
	void OnCommitRequested();
	void PrepareLocomotionExit(float MaximumSpeed);
	UFUNCTION() void OnMontageBlendOut();
	FDelegateHandle CommitRequestedHandle;
	UFUNCTION() void OnMontageInterrupted();
	UFUNCTION() void OnMontageCompleted();
	FHodgeDashDirections Directions;
	FGuid DefenseHandle;
	FName MotionInstanceName;
	bool bHandoffOpened = false;
	bool bHandedOff = false;
	bool bHandoffRequested = false;
	bool bEnding = false;
	double AnimationEndsAt = 0.;
	UPROPERTY(Transient) TObjectPtr<UAbilityTask_ApplyRootMotionConstantForce> MotionTask;
	UPROPERTY(Transient) TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
};
