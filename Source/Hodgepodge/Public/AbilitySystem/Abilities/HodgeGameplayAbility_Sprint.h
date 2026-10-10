#pragma once

#include "AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h"
#include "HodgeGameplayAbility_Sprint.generated.h"

class UAbilityTask_PlayMontageAndWait;
USTRUCT()
struct FHodgeSprintPivotTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	FVector IncomingDirection = FVector::ZeroVector;
	FVector DesiredDirection = FVector::ZeroVector;
	float StartingYaw = 0.f;
	int32 SessionId = 0;
	int32 SequenceId = 0;
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bSuccess);
};
template<> struct TStructOpsTypeTraits<FHodgeSprintPivotTargetData> : TStructOpsTypeTraitsBase2<FHodgeSprintPivotTargetData>
{ enum { WithNetSerializer = true, WithCopy = true }; };

UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_Sprint : public UHodgeGameplayAbility_MovementAction
{
	GENERATED_BODY()
public:
	UHodgeGameplayAbility_Sprint();
	UFUNCTION(BlueprintPure) bool IsPivoting() const { return bPivoting; }
	static bool QualifiesPivot(FVector Velocity, FVector Desired, float MinimumSpeed, float Angle);
	static bool IsPivotRecoveryComplete(float Position, float EndTime);
	/** 在输入交给 CMC 前捕获掉头起手，避免制动后丢失来向速度。 */
	void ProcessMovementIntent(FVector Desired);
protected:
	virtual void BeginMovement(const FHodgeMoveIntentSnapshot& Intent) override;
	virtual void UpdateMovement(float DeltaTime) override;
	virtual void HandleMovementFollowUp(const FGameplayAbilityTargetDataHandle& Data) override;
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
private:
	void RequestPivot(FVector Velocity, FVector Desired);
	void StartPivot(float StartingYaw);
	void UpdatePivotRecovery();
	void ReleasePivotTask();
	UFUNCTION() void OnPivotFinished();
	UPROPERTY(Transient) TObjectPtr<UAbilityTask_PlayMontageAndWait> PivotTask;
	bool bPivoting = false;
	bool bEnding = false;
	int32 PivotSequence = 0;
	int32 PivotMontageInstanceId = INDEX_NONE;
	FGameplayAbilityTargetDataHandle DeferredPivotData;
	double DeferredPivotExpiresAt = 0.;
	double NextPivotAt = 0.;
	void OnAuthorityEnded();
	void OnAttackStarted(FGameplayTag Tag, int32 Count);
	FDelegateHandle AttackHandle;
	void OnDamage(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* Spec, float Magnitude, float Old, float New);
	FGuid PolicyHandle;
	FDelegateHandle AuthorityHandle;
	FDelegateHandle DamageHandle;
	double NoInputAt = 0.;
	double BlockedAt = 0.;
	FHodgeMoveIntentSnapshot PendingIntent;
};
