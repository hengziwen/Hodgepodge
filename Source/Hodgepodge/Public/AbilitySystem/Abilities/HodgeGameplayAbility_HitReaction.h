#pragma once

#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "HodgeGameplayAbility_HitReaction.generated.h"

class UHodgeHitReactionComponent;
class UHodgeCombatComponentBase;
class USkeletalMeshComponent;
class ACharacter;

/** 目标的共享受击 GA；不消费攻击 Definition 或连段授权。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_HitReaction : public UHodgeGameplayAbility
{
	GENERATED_BODY()
public:
	UHodgeGameplayAbility_HitReaction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	bool Synchronize(const FHodgeHitReactionState& State);
	void FinishReaction(bool bCancelled = false);
protected:
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
		FGameplayTagContainer* RelevantTags) const override;
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Payload) override;
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
private:
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeHitReactionComponent> Reaction;
	UPROPERTY(Transient) TWeakObjectPtr<UAnimMontage> PlayedMontage;
	UPROPERTY(Transient) TWeakObjectPtr<ACharacter> ReactionAvatar;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeCombatComponentBase> PoseCombat;
	UPROPERTY(Transient) TWeakObjectPtr<USkeletalMeshComponent> PoseMesh;
	FGuid PoseLease;
	uint8 SavedPosePolicy = 0;
	int32 AppliedSequence = 0;
	EHodgeHitReactionPhase AppliedPhase = EHodgeHitReactionPhase::None;
	float AppliedStartedAt = -1.f;
	uint16 MotionSourceId = 0;
	FVector QueuedLaunchVelocity = FVector::ZeroVector;
	bool bEnding = false;
	void ClearMotion();
};
