#include "Component/HodgeCombatComponentBase.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Component/HodgeHeroComponent.h"
#include "Data/HodgeComboDefinition.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Data/HodgePawnData.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_ComboInputRequest, "GameplayEvent.Combo.InputRequest");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_ComboEventRequest, "GameplayEvent.Combo.EventRequest");

void UHodgeCombatComponentBase::Configure(UHodgeAbilitySystemComponent* InASC, const UHodgeComboDefinition* InDefinition)
{
	if (bShuttingDown) { return; }
	if (InASC && InASC->GetAvatarActor() != GetOwner()) { return; }
	if (ASC == InASC && Definition == InDefinition) { return; }
	Shutdown();
	ASC = InASC;
	TArray<FText> Errors;
	if (InDefinition && InDefinition->ValidateDefinition(Errors)) { Definition = InDefinition; }
	for (const FText& Error : Errors) { UE_LOG(LogTemp, Error, TEXT("[Hodge] Combo: %s"), *Error.ToString()); }
	CurrentComboTag = Definition ? Definition->EntryComboTag : FGameplayTag();
	SetComponentTickEnabled(ASC && Definition && ASC->AbilityActorInfo.IsValid() &&
		(GetOwner()->HasAuthority() || ASC->AbilityActorInfo->IsLocallyControlled()));
	// 观察者标签可能先于 Pawn 的 ASC 绑定到达，绑定完成后补应用缓存状态。
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy) { OnRep_ObserverTags({}); }
}

void UHodgeCombatComponentBase::Shutdown()
{
	if (bShuttingDown) { return; }
	TGuardValue<bool> Guard(bShuttingDown, true);
	SetComponentTickEnabled(false);
	ResetSession(true);
	SetNode(FGameplayTag());
	if (ASC) { ASC->RemoveLooseGameplayTags(AppliedObserverTags); }
	AppliedObserverTags.Reset();
	EndAllDetectionSessions();
	AuthorizedHandle = {};
	PendingNode = FGameplayTag();
	Definition = nullptr;
	ASC = nullptr;
	CurrentComboTag = FGameplayTag();
	bMoveRequestPending = false;
	bSwitching = false;
	bEvaluating = false;
}

void UHodgeCombatComponentBase::ClearInput()
{
	BufferedInput = FGameplayTag();
	InputExpiresAt = 0;
}

bool UHodgeCombatComponentBase::InputPressed(FGameplayTag InputTag)
{
	if (!IsComboReady()) { return false; }
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

const FHodgeComboTransition* UHodgeCombatComponentBase::SelectTransition(FGameplayTag Trigger, bool bEvent) const
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

bool UHodgeCombatComponentBase::IsAuthorized(FGameplayAbilitySpecHandle Handle) const
{
	return IsComboReady() && Handle.IsValid() && AuthorizedHandle == Handle;
}

int16 UHodgeCombatComponentBase::ExecutionKey() const
{
	if (!CurrentAbility) { return 0; }
	const auto Key = CurrentAbility->GetCurrentActivationInfo().GetActivationPredictionKey();
	return Key.IsServerInitiatedKey() ? -Key.Current : Key.Current;
}

bool UHodgeCombatComponentBase::PrepareTransition(const FHodgeComboTransition& Edge, FGameplayAbilitySpecHandle Handle)
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
	if (!IsComboReady() || AuthorizedHandle != Handle) { return false; }
	bSwitching = true;
	PendingNode = Edge.TargetComboTag;
	ClearInput();
	if (CurrentAbility) { CurrentAbility->FinishExecution(true, false); }
	// 结束旧技能可能同步卸载组件或更换 Pawn，不能继续使用已清理的连招配置。
	if (!IsComboReady() || !bSwitching) { return false; }
	CurrentAbility = nullptr;
	SetNode(Definition->EntryComboTag);
	return true;
}

bool UHodgeCombatComponentBase::TryTransition(FGameplayTag Trigger, bool bEvent)
{
	if (!IsComboReady() || bSwitching || bEvaluating) { return false; }
	const FGameplayTag SourceNode = CurrentComboTag;
	const int16 SourceKey = ExecutionKey();
	if (CurrentAbility) { CurrentAbility->RefreshExecutionClock(); }
	if (!IsComboReady() || SourceNode != CurrentComboTag || SourceKey != ExecutionKey()) { return false; }
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

bool UHodgeCombatComponentBase::PrepareServerActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData* Payload)
{
	if (!IsComboReady() || ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked)
		|| !Payload || Payload->OptionalObject != Definition
		|| Payload->Instigator != ASC->GetAvatarActor() || Payload->TargetTags.Num() != 1
		|| Payload->InstigatorTags.Num() != 1 || !Payload->TargetTags.HasTagExact(CurrentComboTag)
		|| Payload->EventMagnitude != ExecutionKey()) { return false; }
	// 时间轴事件由服务器产生，不接受客户端自行声明事件已发生。
	if (Payload->EventTag != TAG_ComboInputRequest) { return false; }
	if (CurrentAbility) { CurrentAbility->RefreshExecutionClock(); }
	if (!IsComboReady() || !Payload->TargetTags.HasTagExact(CurrentComboTag) || Payload->EventMagnitude != ExecutionKey())
	{
		return false;
	}
	TGuardValue<bool> Guard(bEvaluating, true);
	const auto* Edge = SelectTransition(Payload->InstigatorTags.First(), false);
	const auto* Target = Edge ? Definition->FindNode(Edge->TargetComboTag) : nullptr;
	if (!Target || ASC->FindDefinitionAbility(Target->AbilityTag) != Handle) { return false; }
	return PrepareTransition(*Edge, Handle);
}

bool UHodgeCombatComponentBase::PrepareConfirmedActivation(FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayEventData& Payload)
{
	if (!IsComboReady() || Payload.OptionalObject != Definition || Payload.Instigator != ASC->GetAvatarActor()
		|| Payload.EventTag != TAG_ComboEventRequest) { return false; }
	for (FGameplayTag Tag : Payload.TargetTags)
	{
		const auto* Row = Definition->FindNode(Tag);
		if (Row && ASC->FindDefinitionAbility(Row->AbilityTag) == Handle)
		{
			ResetSession(true);
			if (!IsComboReady()) { return false; }
			bSwitching = true;
			PendingNode = Tag;
			AuthorizedHandle = Handle;
			return true;
		}
	}
	return false;
}

void UHodgeCombatComponentBase::RejectServerActivation(const FGameplayEventData* Payload)
{
	if (IsComboReady() && Payload && Payload->OptionalObject == Definition && Payload->Instigator == ASC->
		GetAvatarActor()
		&& Payload->TargetTags.Num() == 1 && Payload->TargetTags.HasTagExact(CurrentComboTag)
		&& Payload->EventMagnitude == ExecutionKey()) { ResetSession(true); }
}

void UHodgeCombatComponentBase::CompleteServerActivation()
{
	bSwitching = false;
	AuthorizedHandle = {};
	PendingNode = FGameplayTag();
	if (!CurrentAbility) { ResetSession(false); }
	DrainEvents();
}

void UHodgeCombatComponentBase::ExecutionStarted(UHodgeGameplayAbility_Definition* Ability)
{
	if (!Ability || !IsAuthorized(Ability->GetCurrentAbilitySpecHandle())) { return; }
	CurrentAbility = Ability;
	SetNode(PendingNode);
}

void UHodgeCombatComponentBase::ExecutionEnded(UHodgeGameplayAbility_Definition* Ability)
{
	if (CurrentAbility != Ability) { return; }
	CurrentAbility = nullptr;
	if (!bSwitching) { ResetSession(false); }
}

void UHodgeCombatComponentBase::WindowsChanged(UHodgeGameplayAbility_Definition* Ability)
{
	if (!IsComboReady() || CurrentAbility != Ability || !ASC->AbilityActorInfo->IsLocallyControlled()) { return; }
	if (BufferedInput.IsValid() && GetWorld()->GetTimeSeconds() < InputExpiresAt)
	{
		TryTransition(BufferedInput, false);
	}
}

void UHodgeCombatComponentBase::TimelineEvent(UHodgeGameplayAbility_Definition* Ability, FGameplayTag Event)
{
	if (!IsComboReady() || CurrentAbility != Ability || !GetOwner()->HasAuthority()) { return; }
	if (bSwitching || bEvaluating) { QueuedEvents.Add({Ability, ExecutionKey(), Event}); }
	else { TryTransition(Event, true); }
}

void UHodgeCombatComponentBase::DrainEvents()
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

void UHodgeCombatComponentBase::SetNode(FGameplayTag Node)
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

void UHodgeCombatComponentBase::ResetSession(bool bEndAbility)
{
	TGuardValue<bool> Guard(bSwitching, true);
	auto* Previous = CurrentAbility.Get();
	CurrentAbility = nullptr;
	QueuedEvents.Reset();
	ClearInput();
	if (bEndAbility && Previous) { Previous->FinishExecution(true, GetOwner()->HasAuthority()); }
	SetNode(Definition ? Definition->EntryComboTag : FGameplayTag());
}

void UHodgeCombatComponentBase::TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Tick)
{
	Super::TickComponent(Delta, Type, Tick);
	DrainEvents();
	if (!IsComboReady() || !ASC->AbilityActorInfo->IsLocallyControlled())
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

void UHodgeCombatComponentBase::ServerMoveCancel_Implementation(AActor* Avatar, FGameplayAbilitySpecHandle Handle, int32 Key)
{
	if (IsComboReady() && CurrentAbility && Avatar == GetOwner()
		&& Handle == CurrentAbility->GetCurrentAbilitySpecHandle() && Key == ExecutionKey())
	{
		CurrentAbility->RefreshExecutionClock();
		if (IsComboReady() && CurrentAbility && Handle == CurrentAbility->GetCurrentAbilitySpecHandle() && Key == ExecutionKey()
			&& CurrentAbility->GetExecutionWindows().HasTag(Definition->MoveCancelWindowTag)) { ResetSession(true); }
	}
	ClientMoveCancelResult();
}

void UHodgeCombatComponentBase::ClientMoveCancelResult_Implementation() { bMoveRequestPending = false; }

void UHodgeCombatComponentBase::ServerReturnToEntry_Implementation(AActor* Avatar, FGameplayTag SourceNode, int32 Key,
                                                              FGameplayTag Intent)
{
	if (!IsComboReady() || Avatar != GetOwner() || SourceNode != CurrentComboTag
		|| Key != ExecutionKey() || ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked)) { return; }
	if (CurrentAbility) { CurrentAbility->RefreshExecutionClock(); }
	if (!IsComboReady() || SourceNode != CurrentComboTag || Key != ExecutionKey()) { return; }
	TGuardValue<bool> Guard(bEvaluating, true);
	const auto* Edge = SelectTransition(Intent, false);
	if (Edge && Edge->TargetComboTag == Definition->EntryComboTag) { ResetSession(true); }
}

void UHodgeCombatComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UHodgeCombatComponentBase, ObserverTags, COND_SkipOwner);
}

void UHodgeCombatComponentBase::OnRep_ObserverTags(const FGameplayTagContainer& Previous)
{
	if (IsValid(ASC) && ASC->GetAvatarActor() == GetOwner() && GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		// 只撤销本组件实际应用的标签，重复绑定不会叠加其他来源的计数。
		ASC->RemoveLooseGameplayTags(AppliedObserverTags);
		AppliedObserverTags = ObserverTags;
		ASC->AddLooseGameplayTags(AppliedObserverTags);
	}
}

UHodgeCombatComponentBase* UHodgeCombatComponentBase::FindCombatComponent(const AActor* Avatar)
{
	return IsValid(Avatar) ? Avatar->FindComponentByClass<UHodgeCombatComponentBase>() : nullptr;
}

bool UHodgeCombatComponentBase::IsComboReady() const
{
	// 旧 Pawn 的组件不能继续操作已经切换 Avatar 的 PlayerState ASC。
	return !bShuttingDown && IsRegistered() && IsValid(ASC) && Definition &&
		ASC->AbilityActorInfo.IsValid() && ASC->GetAvatarActor() == GetOwner() &&
		!ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death);
}

void UHodgeCombatComponentBase::BindPawnExtension()
{
	auto* Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(GetOwner());
	if (!Extension) { return; }
	PawnExtension = Extension;
	Extension->OnAbilitySystemUninitialized_Register(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::HandleAbilitySystemUninitialized));
	Extension->OnAbilitySystemInitialized_RegisterAndCall(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::HandleAbilitySystemInitialized));
}

void UHodgeCombatComponentBase::HandleAbilitySystemInitialized()
{
	if (!IsRegistered() || !PawnExtension.IsValid()) { return; }
	const auto* PawnData = PawnExtension->GetPawnData<UHodgePawnData>();
	Configure(PawnExtension->GetHodgeAbilitySystemComponent(), PawnData ? PawnData->ComboDefinition.Get() : nullptr);
}

void UHodgeCombatComponentBase::HandleAbilitySystemUninitialized()
{
	Shutdown();
}

void UHodgeCombatComponentBase::OnRegister()
{
	Super::OnRegister();
	BindPawnExtension();
}

void UHodgeCombatComponentBase::BeginPlay()
{
	Super::BeginPlay();
	// 覆盖动态添加时其他默认组件尚未完成注册的顺序。
	BindPawnExtension();
}

void UHodgeCombatComponentBase::OnUnregister()
{
	if (PawnExtension.IsValid()) { PawnExtension->UnregisterAbilitySystemDelegates(this); }
	PawnExtension.Reset();
	Shutdown();
	Super::OnUnregister();
}
