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
	UHodgeHitReactionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
	void UninitializeFromAbilitySystem();
	void RefreshConfiguration();
	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") FHodgeHitReactionState GetReactionState() const { return State; }
	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") FHodgeHitReactionResult GetLastResult() const { return LastResult; }
	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") bool IsControlled() const { return State.IsActive(); }
	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") const UHodgeHitReactionProfile* GetProfile() const { return Profile; }
	// 用于不通过玩家 PawnData 生成的目标，仍必须先绑定合法 ASC。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|HitReaction") TObjectPtr<UHodgeHitReactionProfile> OverrideProfile;
	UPROPERTY(BlueprintAssignable) FHodgeReactionResolved OnReactionResolved;
	UPROPERTY(BlueprintAssignable) FHodgeReactionStateChanged OnReactionStateChanged;
	UPROPERTY(BlueprintAssignable) FHodgeLightFeedback OnLightFeedback;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|HitReaction") bool RequestGetUp();
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|HitReaction") void CancelReaction();
	FGuid BeginHit(const FHodgeHitReactionConfig& Request, AActor* Source, FVector SourceOrigin);
	void RecordDamage(const FGuid& HitId, bool bAccepted, float ActualDamage);
	FHodgeHitReactionResult FinishHit(const FGuid& HitId);
	void AttachAbility(UHodgeGameplayAbility_HitReaction* Ability);
	void DetachAbility(UHodgeGameplayAbility_HitReaction* Ability);
	void CompletePlan(int32 Sequence);
	void TickPlan();
	float GetServerTime() const;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	friend struct FHodgeHitReactionTestAccess;
	struct FPendingHit
	{
		FHodgeHitReactionConfig Request;
		TWeakObjectPtr<AActor> Source;
		TWeakObjectPtr<AActor> Avatar;
		FVector Origin = FVector::ZeroVector;
		FGameplayTag BodyTag;
		int32 BodyRank = 0;
		float Damage = 0.f;
		bool bAccepted = false;
		bool bRejected = false;
	};
	TMap<FGuid, FPendingHit> PendingHits;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeGameplayAbility_HitReaction> ActiveAbility;
	UPROPERTY(ReplicatedUsing=OnRep_Profile) TObjectPtr<UHodgeHitReactionProfile> Profile;
	UPROPERTY(ReplicatedUsing=OnRep_State) FHodgeHitReactionState State;
	UPROPERTY(Transient) FHodgeHitReactionResult LastResult;
	int32 NextSequence = 0;
	bool bOwnsControl = false;
	bool bSubmitting = false;
	bool bCompleting = false;
	bool bSawAirborne = false;
	bool IsBound() const;
	bool BuildPlan(const FPendingHit& Hit, FHodgeHitReactionState& Plan, TArray<EHodgeImpactType>& Applied) const;
	bool SubmitPlan(FHodgeHitReactionState Plan, EHodgeHitReactionOutcome& Failure);
	void SetControl(bool bControlled);
	void PublishState();
	void ChangePhase(EHodgeHitReactionPhase Phase, UAnimMontage* Montage, float Duration);
	UFUNCTION() void OnRep_State();
	UFUNCTION() void OnRep_Profile();
	UFUNCTION(NetMulticast, Unreliable) void MulticastLightFeedback(FVector Direction);
};
