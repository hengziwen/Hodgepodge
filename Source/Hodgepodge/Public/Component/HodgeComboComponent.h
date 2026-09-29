#pragma once
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "HodgeComboComponent.generated.h"

class UHodgeAbilitySystemComponent;
class UHodgeComboDefinition;
class UHodgeGameplayAbility_Definition;
struct FGameplayEventData;
struct FHodgeComboTransition;

/** PlayerState-owned combo session. Windows always come from the current ability execution. */
UCLASS()
class HODGEPODGE_API UHodgeComboComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHodgeComboComponent();
	void Configure(UHodgeAbilitySystemComponent* InASC, const UHodgeComboDefinition* InDefinition);
	void Shutdown();
	bool InputPressed(FGameplayTag InputTag);
	void ClearInput();
	void ExecutionStarted(UHodgeGameplayAbility_Definition* Ability);
	void ExecutionEnded(UHodgeGameplayAbility_Definition* Ability);
	void WindowsChanged(UHodgeGameplayAbility_Definition* Ability);
	void TimelineEvent(UHodgeGameplayAbility_Definition* Ability, FGameplayTag Event);
	bool IsAuthorized(FGameplayAbilitySpecHandle Handle) const;
	bool PrepareServerActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData* Payload);
	void RejectServerActivation(const FGameplayEventData* Payload);
	bool PrepareConfirmedActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData& Payload);
	void CompleteServerActivation();
	UFUNCTION(BlueprintPure)
	FGameplayTag GetCurrentComboTag() const { return CurrentComboTag; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void
	TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* TickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

private:
	friend struct FHodgeComboTestAccess;

	struct FQueuedEvent
	{
		TWeakObjectPtr<UHodgeGameplayAbility_Definition> Ability;
		int16 Key = 0;
		FGameplayTag Tag;
	};

	void DrainEvents();
	TArray<FQueuedEvent> QueuedEvents;
	bool bDrainingEvents = false;
	const FHodgeComboTransition* SelectTransition(FGameplayTag Trigger, bool bEvent) const;
	bool TryTransition(FGameplayTag Trigger, bool bEvent);
	bool PrepareTransition(const FHodgeComboTransition& Edge, FGameplayAbilitySpecHandle Handle);
	void ResetSession(bool bEndAbility);
	void SetNode(FGameplayTag Node);
	int16 ExecutionKey() const;
	UFUNCTION(Server, Reliable)
	void ServerMoveCancel(AActor* Avatar, FGameplayAbilitySpecHandle Handle, int32 Key);
	UFUNCTION(Server, Reliable)
	void ServerReturnToEntry(AActor* Avatar, FGameplayTag SourceNode, int32 Key, FGameplayTag Intent);
	UFUNCTION(Client, Reliable)
	void ClientMoveCancelResult();
	UFUNCTION()
	void OnRep_ObserverTags(const FGameplayTagContainer& Previous);
	UPROPERTY(Transient)
	TObjectPtr<UHodgeAbilitySystemComponent> ASC;
	UPROPERTY(Transient)
	TObjectPtr<const UHodgeComboDefinition> Definition;
	UPROPERTY(Transient)
	TObjectPtr<UHodgeGameplayAbility_Definition> CurrentAbility;
	UPROPERTY(Transient)
	FGameplayTag CurrentComboTag;
	UPROPERTY(ReplicatedUsing=OnRep_ObserverTags)
	FGameplayTagContainer ObserverTags;
	FGameplayTagContainer OwnedNodeTags;
	FGameplayTag BufferedInput;
	double InputExpiresAt = 0;
	FGameplayAbilitySpecHandle AuthorizedHandle;
	FGameplayTag PendingNode;
	bool bSwitching = false;
	bool bMoveRequestPending = false;
	bool bEvaluating = false;
};
