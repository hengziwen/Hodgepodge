#include "Component/HodgeComboComponent.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Component/HodgeHeroComponent.h"
#include "Data/HodgeComboDefinition.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeComboComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_ComboInputRequest, "GameplayEvent.Combo.InputRequest");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_ComboEventRequest, "GameplayEvent.Combo.EventRequest");

UHodgeComboComponent::UHodgeComboComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UHodgeComboComponent::Configure(UHodgeAbilitySystemComponent* InASC, const UHodgeComboDefinition* InDefinition)
{
	if (ASC == InASC && Definition == InDefinition) { return; }
	Shutdown();
	ASC = InASC;
	TArray<FText> Errors;
	if (InDefinition && InDefinition->ValidateDefinition(Errors)) { Definition = InDefinition; }
	for (const FText& Error : Errors) { UE_LOG(LogTemp, Error, TEXT("[Hodge] Combo: %s"), *Error.ToString()); }
	CurrentComboTag = Definition ? Definition->EntryComboTag : FGameplayTag();
}

void UHodgeComboComponent::Shutdown()
{
	ResetSession(true);
	Definition = nullptr;
	ASC = nullptr;
	CurrentComboTag = FGameplayTag();
	bMoveRequestPending = false;
}

void UHodgeComboComponent::ClearInput()
{
	BufferedInput = FGameplayTag();
	InputExpiresAt = 0;
}

bool UHodgeComboComponent::InputPressed(FGameplayTag InputTag)
{
	if (!Definition || !ASC) { return false; }
	for (const auto& Binding : Definition->InputBindings)
	{
		if (Binding.InputTag != InputTag) { continue; }
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
		{
			ClearInput();
			return true;
		}
		BufferedInput = Binding.IntentTag;
		InputExpiresAt = GetWorld()->GetTimeSeconds() + Definition->InputBufferSeconds;
		TryTransition(BufferedInput, false);
		return true;
	}
	return false;
}

const FHodgeComboTransition* UHodgeComboComponent::SelectTransition(FGameplayTag Trigger, bool bEvent) const
{
	const auto* Node = Definition ? Definition->FindNode(CurrentComboTag) : nullptr;
	if (!ASC || !Node || !Trigger.IsValid()) { return nullptr; }
	const auto Windows = CurrentAbility ? CurrentAbility->GetExecutionWindows() : FGameplayTagContainer();
	const auto& Tags = ASC->GetOwnedGameplayTags();
	const FHodgeComboTransition* Best = nullptr;
	for (const auto& Edge : Node->Transitions)
	{
		if ((bEvent ? Edge.TriggerEventTag : Edge.TriggerInputIntentTag) != Trigger
			|| !Windows.HasAll(Edge.RequiredWindowTags) || !Tags.HasAll(Edge.RequiredSourceTags)
			|| Tags.HasAny(Edge.BlockedSourceTags)) { continue; }
		if (!Best || Edge.TransitionPriority > Best->TransitionPriority) { Best = &Edge; }
	}
	return Best;
}

bool UHodgeComboComponent::IsAuthorized(FGameplayAbilitySpecHandle Handle) const
{
	return Handle.IsValid() && AuthorizedHandle == Handle;
}

int16 UHodgeComboComponent::ExecutionKey() const
{
	if (!CurrentAbility) { return 0; }
	const auto Key = CurrentAbility->GetCurrentActivationInfo().GetActivationPredictionKey();
	return Key.IsServerInitiatedKey() ? -Key.Current : Key.Current;
}

bool UHodgeComboComponent::PrepareTransition(const FHodgeComboTransition& Edge, FGameplayAbilitySpecHandle Handle)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(Handle);
	if (!Spec || (Spec->IsActive() && (!CurrentAbility || CurrentAbility->GetCurrentAbilitySpecHandle() != Handle)))
	{
		return false;
	}
	AuthorizedHandle = Handle;
	if (!Spec->Ability->CanActivateAbility(Handle, ASC->AbilityActorInfo.Get()))
	{
		AuthorizedHandle = {};
		return false;
	}
	bSwitching = true;
	PendingNode = Edge.TargetComboTag;
	ClearInput();
	if (CurrentAbility) { CurrentAbility->FinishExecution(true, false); }
	CurrentAbility = nullptr;
	SetNode(Definition->EntryComboTag);
	return true;
}

bool UHodgeComboComponent::TryTransition(FGameplayTag Trigger, bool bEvent)
{
	if (!ASC || !Definition || bSwitching || bEvaluating) { return false; }
	const FGameplayTag SourceNode = CurrentComboTag;
	const int16 SourceKey = ExecutionKey();
	if (CurrentAbility) { CurrentAbility->RefreshExecutionClock(); }
	if (SourceNode != CurrentComboTag || SourceKey != ExecutionKey()) { return false; }
	TGuardValue<bool> Guard(bEvaluating, true);
	const auto* Edge = SelectTransition(Trigger, bEvent);
	if (!Edge) { return false; }
	const auto* Target = Definition->FindNode(Edge->TargetComboTag);
	if (!Target) { return false; }
	if (Target->ComboTag == Definition->EntryComboTag)
	{
		ClearInput();
		if (bEvent) { ResetSession(true); }
		else { ServerReturnToEntry(ASC->GetAvatarActor(), CurrentComboTag, ExecutionKey(), Trigger); }
		return true;
	}
	const auto Handle = ASC->FindDefinitionAbility(Target->AbilityTag);
	FGameplayEventData Payload;
	Payload.EventTag = bEvent ? TAG_ComboEventRequest : TAG_ComboInputRequest;
	Payload.Instigator = ASC->GetAvatarActor();
	Payload.OptionalObject = Definition;
	Payload.TargetTags.AddTag(CurrentComboTag);
	Payload.InstigatorTags.AddTag(Trigger);
	Payload.EventMagnitude = ExecutionKey();
	if (bEvent) { Payload.TargetTags.AddTag(Target->ComboTag); }
	if (!PrepareTransition(*Edge, Handle)) { return false; }
	const FPredictionKey Key = bEvent && GetOwner()->HasAuthority()
		                           ? FPredictionKey::CreateNewServerInitiatedKey(ASC)
		                           : FPredictionKey();
	const bool bActivated = ASC->InternalTryActivateAbility(Handle, Key, nullptr, nullptr, &Payload);
	CompleteServerActivation();
	return bActivated && CurrentAbility != nullptr;
}

bool UHodgeComboComponent::PrepareServerActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData* Payload)
{
	if (!ASC || !Definition || ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked)
		|| !Payload || Payload->OptionalObject != Definition
		|| Payload->Instigator != ASC->GetAvatarActor() || Payload->TargetTags.Num() != 1
		|| Payload->InstigatorTags.Num() != 1 || !Payload->TargetTags.HasTagExact(CurrentComboTag)
		|| Payload->EventMagnitude != ExecutionKey()) { return false; }
	// Timeline events originate on authority; a client cannot assert that an event happened.
	if (Payload->EventTag != TAG_ComboInputRequest) { return false; }
	if (CurrentAbility) { CurrentAbility->RefreshExecutionClock(); }
	if (!Payload->TargetTags.HasTagExact(CurrentComboTag) || Payload->EventMagnitude != ExecutionKey())
	{
		return false;
	}
	TGuardValue<bool> Guard(bEvaluating, true);
	const auto* Edge = SelectTransition(Payload->InstigatorTags.First(), false);
	const auto* Target = Edge ? Definition->FindNode(Edge->TargetComboTag) : nullptr;
	if (!Target || ASC->FindDefinitionAbility(Target->AbilityTag) != Handle) { return false; }
	return PrepareTransition(*Edge, Handle);
}

bool UHodgeComboComponent::PrepareConfirmedActivation(FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayEventData& Payload)
{
	if (!ASC || !Definition || Payload.OptionalObject != Definition || Payload.Instigator != ASC->GetAvatarActor()
		|| Payload.EventTag != TAG_ComboEventRequest) { return false; }
	for (FGameplayTag Tag : Payload.TargetTags)
	{
		const auto* Row = Definition->FindNode(Tag);
		if (Row && ASC->FindDefinitionAbility(Row->AbilityTag) == Handle)
		{
			ResetSession(true);
			bSwitching = true;
			PendingNode = Tag;
			AuthorizedHandle = Handle;
			return true;
		}
	}
	return false;
}

void UHodgeComboComponent::RejectServerActivation(const FGameplayEventData* Payload)
{
	if (ASC && Definition && Payload && Payload->OptionalObject == Definition && Payload->Instigator == ASC->
		GetAvatarActor()
		&& Payload->TargetTags.Num() == 1 && Payload->TargetTags.HasTagExact(CurrentComboTag)
		&& Payload->EventMagnitude == ExecutionKey()) { ResetSession(true); }
}

void UHodgeComboComponent::CompleteServerActivation()
{
	bSwitching = false;
	AuthorizedHandle = {};
	PendingNode = FGameplayTag();
	if (!CurrentAbility) { ResetSession(false); }
	DrainEvents();
}

void UHodgeComboComponent::ExecutionStarted(UHodgeGameplayAbility_Definition* Ability)
{
	if (!Ability || !IsAuthorized(Ability->GetCurrentAbilitySpecHandle())) { return; }
	CurrentAbility = Ability;
	SetNode(PendingNode);
}

void UHodgeComboComponent::ExecutionEnded(UHodgeGameplayAbility_Definition* Ability)
{
	if (CurrentAbility != Ability) { return; }
	CurrentAbility = nullptr;
	if (!bSwitching) { ResetSession(false); }
}

void UHodgeComboComponent::WindowsChanged(UHodgeGameplayAbility_Definition* Ability)
{
	if (CurrentAbility != Ability || !ASC->AbilityActorInfo->IsLocallyControlled()) { return; }
	if (BufferedInput.IsValid() && GetWorld()->GetTimeSeconds() < InputExpiresAt)
	{
		TryTransition(BufferedInput, false);
	}
}

void UHodgeComboComponent::TimelineEvent(UHodgeGameplayAbility_Definition* Ability, FGameplayTag Event)
{
	if (CurrentAbility != Ability || !GetOwner()->HasAuthority()) { return; }
	if (bSwitching || bEvaluating) { QueuedEvents.Add({Ability, ExecutionKey(), Event}); }
	else { TryTransition(Event, true); }
}

void UHodgeComboComponent::DrainEvents()
{
	if (bDrainingEvents || bSwitching || bEvaluating) { return; }
	TGuardValue<bool> Guard(bDrainingEvents, true);
	int32 Remaining = 32;
	while (!QueuedEvents.IsEmpty() && Remaining-- > 0)
	{
		const FQueuedEvent Event = QueuedEvents[0];
		QueuedEvents.RemoveAt(0);
		if (Event.Ability == CurrentAbility && Event.Key == ExecutionKey()) { TryTransition(Event.Tag, true); }
	}
	if (!QueuedEvents.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[Hodge] Combo event chain exceeded 32 transitions; returning to Entry"));
		ResetSession(true);
	}
}

void UHodgeComboComponent::SetNode(FGameplayTag Node)
{
	FGameplayTagContainer Previous = MoveTemp(OwnedNodeTags);
	OwnedNodeTags.Reset();
	CurrentComboTag = Node;
	if (ASC) { ASC->RemoveLooseGameplayTags(Previous); }
	if (const auto* Row = Definition ? Definition->FindNode(Node) : nullptr)
	{
		OwnedNodeTags = Row->GrantedTags;
		if (ASC) { ASC->AddLooseGameplayTags(OwnedNodeTags); }
	}
	if (GetOwner()->HasAuthority()) { ObserverTags = OwnedNodeTags; }
}

void UHodgeComboComponent::ResetSession(bool bEndAbility)
{
	TGuardValue<bool> Guard(bSwitching, true);
	auto* Previous = CurrentAbility.Get();
	CurrentAbility = nullptr;
	QueuedEvents.Reset();
	ClearInput();
	if (bEndAbility && Previous) { Previous->FinishExecution(true, GetOwner()->HasAuthority()); }
	SetNode(Definition ? Definition->EntryComboTag : FGameplayTag());
}

void UHodgeComboComponent::TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Tick)
{
	Super::TickComponent(Delta, Type, Tick);
	DrainEvents();
	if (!ASC || !Definition || !ASC->AbilityActorInfo.IsValid() || !ASC->AbilityActorInfo->IsLocallyControlled())
	{
		return;
	}
	if (ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
	{
		ClearInput();
		return;
	}
	if (GetWorld()->GetTimeSeconds() >= InputExpiresAt) { ClearInput(); }
	if (BufferedInput.IsValid() && TryTransition(BufferedInput, false)) { return; }
	if (!CurrentAbility || bSwitching || bMoveRequestPending) { return; }
	const auto* Avatar = ASC->GetAvatarActor();
	const auto* Hero = Avatar ? Avatar->FindComponentByClass<UHodgeHeroComponent>() : nullptr;
	if (Hero && Hero->HasMoveIntent(Definition->MoveIntentThreshold)
		&& CurrentAbility->GetExecutionWindows().HasTag(Definition->MoveCancelWindowTag))
	{
		bMoveRequestPending = true;
		ServerMoveCancel(ASC->GetAvatarActor(), CurrentAbility->GetCurrentAbilitySpecHandle(), ExecutionKey());
	}
}

void UHodgeComboComponent::ServerMoveCancel_Implementation(AActor* Avatar, FGameplayAbilitySpecHandle Handle, int32 Key)
{
	if (ASC && Definition && CurrentAbility && Avatar == ASC->GetAvatarActor()
		&& Handle == CurrentAbility->GetCurrentAbilitySpecHandle() && Key == ExecutionKey())
	{
		CurrentAbility->RefreshExecutionClock();
		if (CurrentAbility && Handle == CurrentAbility->GetCurrentAbilitySpecHandle() && Key == ExecutionKey()
			&& CurrentAbility->GetExecutionWindows().HasTag(Definition->MoveCancelWindowTag)) { ResetSession(true); }
	}
	ClientMoveCancelResult();
}

void UHodgeComboComponent::ClientMoveCancelResult_Implementation() { bMoveRequestPending = false; }

void UHodgeComboComponent::ServerReturnToEntry_Implementation(AActor* Avatar, FGameplayTag SourceNode, int32 Key,
                                                              FGameplayTag Intent)
{
	if (!ASC || !Definition || Avatar != ASC->GetAvatarActor() || SourceNode != CurrentComboTag
		|| Key != ExecutionKey() || ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked)) { return; }
	if (CurrentAbility) { CurrentAbility->RefreshExecutionClock(); }
	if (SourceNode != CurrentComboTag || Key != ExecutionKey()) { return; }
	TGuardValue<bool> Guard(bEvaluating, true);
	const auto* Edge = SelectTransition(Intent, false);
	if (Edge && Edge->TargetComboTag == Definition->EntryComboTag) { ResetSession(true); }
}

void UHodgeComboComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UHodgeComboComponent, ObserverTags, COND_SkipOwner);
}

void UHodgeComboComponent::OnRep_ObserverTags(const FGameplayTagContainer& Previous)
{
	if (auto* OwnerASC = GetOwner()->FindComponentByClass<UHodgeAbilitySystemComponent>())
	{
		OwnerASC->RemoveLooseGameplayTags(Previous);
		OwnerASC->AddLooseGameplayTags(ObserverTags);
	}
}

void UHodgeComboComponent::EndPlay(const EEndPlayReason::Type Reason)
{
	Shutdown();
	Super::EndPlay(Reason);
}
