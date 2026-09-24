#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "HodgeGameplayAbility_BasicAttack.generated.h"

class UHodgeAbilityTimeline;
class UHodgeAbilityTask_PlayTimeline;
class UHodgeAbilityTask_WaitMoveCancel;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitInputPress;

USTRUCT(BlueprintType)
struct FHodgeBasicAttackStep
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UHodgeAbilityTimeline> Timeline;
};

/** Blueprint 配置每段动画和逻辑时间轴；每段最多缓存一次点击，窗口驱动接段和移动取消。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_BasicAttack : public UHodgeGameplayAbility
{
	GENERATED_BODY()

public:
	UHodgeGameplayAbility_BasicAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	TArray<FHodgeBasicAttackStep> AttackSteps;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack", meta=(ClampMin="0.01"))
	float PlayRate = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack", meta=(ClampMin="0.0"))
	float CancelBlendOutTime = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack", meta=(ClampMin="0.0"))
	float MoveIntentThreshold = 0.1f;

	// 运行时从 1 开始，未激活为 0，便于蓝图和 PIE 诊断。
	UPROPERTY(BlueprintReadOnly, Transient, Category="Attack")
	int32 CurrentAttackStep = 0;

protected:
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	void StartStep();
	void ClearStep();
	void TryAdvance();
	void OnComboWindowChanged(FGameplayTag Tag, int32 NewCount);
	void OnTimelineEnded(const FGameplayEventData* Payload);
	UFUNCTION() void OnAttackPressed(float TimeWaited);
	UFUNCTION() void OnCompleted();
	UFUNCTION() void OnInterrupted();

	UPROPERTY(Transient) TObjectPtr<UHodgeAbilityTask_PlayTimeline> TimelineTask;
	UPROPERTY(Transient) TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
	UPROPERTY(Transient) TObjectPtr<UAbilityTask_WaitInputPress> InputTask;
	UPROPERTY(Transient) TObjectPtr<UHodgeAbilityTask_WaitMoveCancel> MoveTask;
	FDelegateHandle ComboHandle;
	FDelegateHandle TimelineEndHandle;
	bool bBufferedAttack = false;
	bool bChangingStep = false;
	bool bEndingAttack = false;
};
