#include "Component/HodgeCharacterRotationComponent.h"
#include "Combat/HodgeHitReactionTypes.h"

#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCharacterRotationComponent)

UHodgeCharacterRotationComponent::UHodgeCharacterRotationComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	SetIsReplicatedByDefault(true);
}

void UHodgeCharacterRotationComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)
{
	if (!InASC || !InASC->IsRegistered() || InASC->GetAvatarActor() != GetOwner()) { return; }
	if (AbilitySystemComponent.Get() == InASC)
	{
		RefreshLocalState();
		return;
	}
	UninitializeFromAbilitySystem();
	AbilitySystemComponent = InASC;
	++AvatarGeneration;
	if (auto* Character = GetPawn<ACharacter>())
	{
		bOriginalControllerYaw = Character->bUseControllerRotationYaw;
		bOriginalOrientToMovement = Character->GetCharacterMovement()->bOrientRotationToMovement;
		bOriginalControllerDesired = Character->GetCharacterMovement()->bUseControllerDesiredRotation;
		Character->GetCharacterMovement()->AddTickPrerequisiteComponent(this);
	}
	RotationTagHandle = InASC->RegisterGameplayTagEvent(HodgeGameplayTags::Status_Rotation_Locked,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HandleConstraintTagChanged);
	MovementStoppedTagHandle = InASC->RegisterGameplayTagEvent(TAG_Gameplay_MovementStopped,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HandleConstraintTagChanged);
	HitReactionTagHandle = InASC->RegisterGameplayTagEvent(HodgeHitReactionTags::Controlled,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HandleConstraintTagChanged);
	DeathTagHandle = InASC->RegisterGameplayTagEvent(HodgeGameplayTags::Status_Death,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HandleConstraintTagChanged);
	RefreshLocalState();
	SetComponentTickEnabled(true);
}

void UHodgeCharacterRotationComponent::UninitializeFromAbilitySystem()
{
	if (UHodgeAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		if (RotationTagHandle.IsValid())
		{
			ASC->UnregisterGameplayTagEvent(RotationTagHandle, HodgeGameplayTags::Status_Rotation_Locked,
				EGameplayTagEventType::NewOrRemoved);
		}
		if (MovementStoppedTagHandle.IsValid())
		{
			ASC->UnregisterGameplayTagEvent(MovementStoppedTagHandle, TAG_Gameplay_MovementStopped,
				EGameplayTagEventType::NewOrRemoved);
		}
		if (HitReactionTagHandle.IsValid())
		{
			ASC->UnregisterGameplayTagEvent(HitReactionTagHandle, HodgeHitReactionTags::Controlled,
				EGameplayTagEventType::NewOrRemoved);
		}
		if (DeathTagHandle.IsValid())
		{ ASC->UnregisterGameplayTagEvent(DeathTagHandle, HodgeGameplayTags::Status_Death); }
	}
	RotationTagHandle.Reset();
	MovementStoppedTagHandle.Reset();
	HitReactionTagHandle.Reset();
	DeathTagHandle.Reset();
	if (AbilitySystemComponent.IsValid())
	{
		if (auto* Character = GetPawn<ACharacter>())
		{
			Character->bUseControllerRotationYaw = bOriginalControllerYaw;
			Character->GetCharacterMovement()->bOrientRotationToMovement = bOriginalOrientToMovement;
			Character->GetCharacterMovement()->bUseControllerDesiredRotation = bOriginalControllerDesired;
			Character->GetCharacterMovement()->RemoveTickPrerequisiteComponent(this);
		}
	}
	AbilitySystemComponent.Reset();
	Requests.Reset(); CompletedRequests.Reset(); ServerRequestHistory.Reset();
	LocalRequestSequence = AcknowledgedSequence = LastServerRequestSequence = 0;
	bServerMoveOverride = false;
	LocalState = {};
	ClearMoveReplayState();
	PublishAuthorityState();
	SetComponentTickEnabled(false);
}

void UHodgeCharacterRotationComponent::HandleConstraintTagChanged(FGameplayTag Tag, int32 NewCount)
{
	RefreshLocalState();
}

void UHodgeCharacterRotationComponent::RefreshLocalState()
{
	const APawn* Pawn = GetPawn<APawn>();
	const UHodgeAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Pawn || (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) || !ASC || ASC->GetAvatarActor() != Pawn) { return; }
	const FHodgeCharacterRotationState Previous = LocalState;
	const bool bNewLock = ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Rotation_Locked)
		|| ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped) || ASC->HasMatchingGameplayTag(HodgeHitReactionTags::Controlled)
		|| ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death);
	if (bNewLock != LocalState.bYawLocked)
	{
		LocalState.bYawLocked = bNewLock;
		LocalState.bRecoveringFacing = !bNewLock;
		if (bNewLock) { LocalState.LockedYaw = FRotator::NormalizeAxis(Pawn->GetActorRotation().Yaw); }
		++LocalState.StateVersion;
	}
	ResolveRequests();
	if (Previous.bYawLocked != LocalState.bYawLocked) { OnFacingStateChanged.Broadcast(Previous, LocalState); }
}

void UHodgeCharacterRotationComponent::PublishAuthorityState()
{
	AActor* Owner = GetOwner();
	if (Owner && Owner->HasAuthority())
	{
		ReplicatedState = LocalState;
		ServerRequestHistory.Add(LastServerRequestSequence, LocalState);
		while (ServerRequestHistory.Num() > 64)
		{
			uint32 Oldest = MAX_uint32;
			for (const auto& Pair : ServerRequestHistory) { Oldest = FMath::Min(Oldest, Pair.Key); }
			ServerRequestHistory.Remove(Oldest);
		}
		Owner->ForceNetUpdate();
	}
}

bool UHodgeCharacterRotationComponent::IsReplayingMove() const
{
	const ACharacter* Character = GetPawn<ACharacter>();
	return bHasReplayState && Character && Character->bClientUpdating;
}

FHodgeCharacterRotationState UHodgeCharacterRotationComponent::GetResolvedState() const
{
	if (IsReplayingMove()) { return ReplayState; }
	if (bServerMoveOverride) { return ServerMoveState; }
	const APawn* Pawn = GetPawn<APawn>();
	if (Pawn && !Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) { return ReplicatedState; }
	return LocalState;
}

bool UHodgeCharacterRotationComponent::IsYawLocked() const { return GetResolvedState().bYawLocked; }
bool UHodgeCharacterRotationComponent::IsRecoveringFacing() const { return GetResolvedState().bRecoveringFacing; }
float UHodgeCharacterRotationComponent::GetLockedYaw() const { return GetResolvedState().LockedYaw; }

FRotator UHodgeCharacterRotationComponent::FilterControlRotation(const FRotator& DesiredRotation, float DeltaSeconds) const
{
	FRotator Result = DesiredRotation;
	const FHodgeCharacterRotationState State = GetResolvedState();
	if (State.bYawLocked) { Result.Yaw = State.LockedYaw; }
	else if (State.Driver != EHodgeCharacterFacingDriver::Controller && IsFacingSystemReady())
	{ Result.Yaw = GetOwner()->GetActorRotation().Yaw; }
	else if (IsFacingSystemReady() && GetOwner())
	{
		Result.Yaw = FMath::FixedTurn(GetOwner()->GetActorRotation().Yaw, DesiredRotation.Yaw,
			(State.bRecoveringFacing ? GetRecoveryTurnRate() : FMath::IsFinite(ActionTurnRate) && ActionTurnRate > 0.f ? ActionTurnRate : 720.f)
			* FMath::Max(0.f, DeltaSeconds));
	}
	return Result;
}

void UHodgeCharacterRotationComponent::NotifyFacingApplied(float DesiredYaw)
{
	if (IsReplayingMove() || bServerMoveOverride || !FMath::IsFinite(DesiredYaw)) { return; }
	FHodgeCharacterRotationState& State = LocalState;
	const FHodgeCharacterRotationState Previous = State;
	if (!State.bYawLocked && State.bRecoveringFacing && GetOwner()
		&& FMath::Abs(FMath::FindDeltaAngleDegrees(GetOwner()->GetActorRotation().Yaw, DesiredYaw)) < 1.f)
	{
		const FGuid AppliedAction = State.ActionRequestId;
		State.bRecoveringFacing = false;
		++State.StateVersion;
		if (!IsReplayingMove()) { PublishAuthorityState(); }
		OnFacingStateChanged.Broadcast(Previous, LocalState);
		if (AppliedAction.IsValid()) { SetRequestStatus(AppliedAction, EHodgeFacingRequestStatus::Applied); }
	}
}

void UHodgeCharacterRotationComponent::SetMoveReplayState(const FHodgeCharacterRotationState& State)
{
	ReplayState = State;
	bHasReplayState = true;
	ApplyResolvedMode();
}

void UHodgeCharacterRotationComponent::ClearMoveReplayState()
{
	bHasReplayState = false;
	ReplayState = {};
	ApplyResolvedMode();
}

void UHodgeCharacterRotationComponent::OnRep_RotationState()
{
	const APawn* Pawn = GetPawn<APawn>();
	// 本地窗口决定预测锁的期限，权威结果只校正仍有效的锁定朝向。
	if (Pawn && Pawn->IsLocallyControlled() && LocalState.bYawLocked && ReplicatedState.bYawLocked)
	{
		LocalState.LockedYaw = ReplicatedState.LockedYaw;
	}
	ResolveRequests();
}

void UHodgeCharacterRotationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, ReplicatedState);
}

void UHodgeCharacterRotationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeFromAbilitySystem();
	Super::EndPlay(EndPlayReason);
}

bool UHodgeCharacterRotationComponent::IsFacingSystemReady() const
{
	return AbilitySystemComponent.IsValid() && AbilitySystemComponent->IsRegistered() &&
		AbilitySystemComponent->GetAvatarActor() == GetOwner() && IsRegistered();
}

bool UHodgeCharacterRotationComponent::IsValidSource(const UObject* Source, bool bAction) const
{
	if (!IsFacingSystemReady() || !IsValid(Source)) { return false; }
	if (const auto* Ability = Cast<UGameplayAbility>(Source))
	{ return Ability->IsActive() && Ability->GetAvatarActorFromActorInfo() == GetOwner(); }
	// 任意方向动作的客户端入口必须来自正在执行的能力。
	if (bAction && !GetOwner()->HasAuthority()) { return false; }
	return Source == GetOwner() || Source->IsIn(GetOwner());
}

bool UHodgeCharacterRotationComponent::HasPendingPrediction() const
{
	return GetOwner() && !GetOwner()->HasAuthority() && LocalRequestSequence > AcknowledgedSequence;
}

FHodgeFacingRequestHandle UHodgeCharacterRotationComponent::AcquireBaseFacingMode(EHodgeCharacterFacingDriver Driver, UObject* Source)
{
	if (!HodgeFacing::IsBaseDriver(Driver) || !IsValidSource(Source, false)) { return {}; }
	const auto* Pawn = GetPawn<APawn>();
	if (!Pawn || (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) ||
		(Driver == EHodgeCharacterFacingDriver::Controller && (!bAllowControllerFacingRequests || !Pawn->Controller))) { return {}; }
	return AddRequest(Driver, Source, {}, 0.f, false);
}

FHodgeFacingRequestHandle UHodgeCharacterRotationComponent::RequestActionFacing(FVector WorldDirection, UObject* Source,
	FGuid ExecutionId, bool bInstantAtStart)
{
	FVector Direction;
	const auto* Pawn = GetPawn<APawn>();
	if (!Pawn || (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) || !IsValidSource(Source, true) ||
		!ExecutionId.IsValid() || !HodgeFacing::NormalizeDirection(WorldDirection, Direction) || IsYawLocked()) { return {}; }
	if (const auto* Definition = Cast<UHodgeGameplayAbility_Definition>(Source))
	{ if (Definition->GetExecutionId() != ExecutionId || Definition->IsExecutionEnding()) { return {}; } }
	return AddRequest(EHodgeCharacterFacingDriver::ActionDirection, Source, ExecutionId, Direction.Rotation().Yaw, bInstantAtStart);
}

FHodgeFacingRequestHandle UHodgeCharacterRotationComponent::AddRequest(EHodgeCharacterFacingDriver Driver, UObject* Source,
	FGuid ExecutionId, float Yaw, bool bInstant)
{
	if (Requests.Num() >= 32 || AbilitySystemComponent->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death)) { return {}; }
	FHodgeFacingRequestHandle Handle; Handle.Id = FGuid::NewGuid(); Handle.AvatarGeneration = AvatarGeneration;
	FRequest& Request = Requests.Add(Handle.Id);
	Request.Source = Source; Request.Driver = Driver; Request.ExecutionId = ExecutionId;
	Request.Yaw = FRotator::NormalizeAxis(Yaw); Request.Order = ++NextRequestOrder; Request.bInstant = bInstant;
	Request.Status = Driver == EHodgeCharacterFacingDriver::ActionDirection ? EHodgeFacingRequestStatus::Pending : EHodgeFacingRequestStatus::Applied;
	if (!GetOwner()->HasAuthority()) { ++LocalRequestSequence; }
	ResolveRequests();
	if (bInstant && !IsYawLocked())
	{
		if (auto* Character = GetPawn<ACharacter>())
		{
			Character->GetCharacterMovement()->MoveUpdatedComponent(FVector::ZeroVector, FRotator(0.f, Yaw, 0.f), false);
			NotifyFacingApplied(Yaw);
		}
	}
	if (!GetOwner()->HasAuthority()) { SendRequest(Handle.Id, false); }
	return Handle;
}

bool UHodgeCharacterRotationComponent::ReleaseFacingRequest(FHodgeFacingRequestHandle Handle)
{
	if (!Handle.IsValid() || Handle.AvatarGeneration != AvatarGeneration || !Requests.Contains(Handle.Id)) { return false; }
	if (!GetOwner()->HasAuthority())
	{
		++LocalRequestSequence;
		SendRequest(Handle.Id, true);
	}
	Requests.Remove(Handle.Id);
	CompletedRequests.Add(Handle.Id, EHodgeFacingRequestStatus::Released);
	ResolveRequests();
	return true;
}

void UHodgeCharacterRotationComponent::ReleaseRequestsForSource(const UObject* Source)
{
	TArray<FGuid> Matches;
	for (const auto& Pair : Requests) { if (Pair.Value.Source.Get() == Source) { Matches.Add(Pair.Key); } }
	for (const FGuid& Id : Matches)
	{ FHodgeFacingRequestHandle Handle; Handle.Id = Id; Handle.AvatarGeneration = AvatarGeneration; ReleaseFacingRequest(Handle); }
}

EHodgeFacingRequestStatus UHodgeCharacterRotationComponent::GetFacingRequestStatus(FHodgeFacingRequestHandle Handle) const
{
	if (!Handle.IsValid() || Handle.AvatarGeneration != AvatarGeneration) { return EHodgeFacingRequestStatus::Invalid; }
	if (const FRequest* Request = Requests.Find(Handle.Id)) { return Request->Status; }
	if (const auto* Status = CompletedRequests.Find(Handle.Id)) { return *Status; }
	return EHodgeFacingRequestStatus::Invalid;
}

void UHodgeCharacterRotationComponent::SetRequestStatus(const FGuid& Id, EHodgeFacingRequestStatus Status)
{
	FRequest* Request = Requests.Find(Id);
	if (!Request || Request->Status == Status) { return; }
	Request->Status = Status;
	FHodgeFacingRequestHandle Handle; Handle.Id = Id; Handle.AvatarGeneration = AvatarGeneration;
	OnActionFacingApplied.Broadcast(Handle, Status);
}

void UHodgeCharacterRotationComponent::ResolveRequests()
{
	if (bResolvingRequests || !IsFacingSystemReady()) { return; }
	TGuardValue<bool> Guard(bResolvingRequests, true);
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn || (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled())) { ApplyResolvedMode(); return; }
	const FHodgeCharacterRotationState Previous = LocalState;
	const FRequest* Base = nullptr; const FRequest* Action = nullptr; FGuid ActionId;
	for (const auto& Pair : Requests)
	{
		if (Pair.Value.Driver == EHodgeCharacterFacingDriver::ActionDirection)
		{ if (!Action || Pair.Value.Order > Action->Order) { Action = &Pair.Value; ActionId = Pair.Key; } }
		else if (!Base || Pair.Value.Order > Base->Order) { Base = &Pair.Value; }
	}
	const bool bAuthorityFallback = !Pawn->HasAuthority() && !HasPendingPrediction() && ReplicatedState.StateVersion > 0;
	LocalState.BaseDriver = Base ? Base->Driver : bAuthorityFallback ? ReplicatedState.BaseDriver : EHodgeCharacterFacingDriver::Movement;
	LocalState.Driver = Action ? Action->Driver : LocalState.BaseDriver;
	LocalState.ActionRequestId = Action ? ActionId : FGuid();
	LocalState.ActionYaw = Action ? Action->Yaw : 0.f;
	if (!Action && bAuthorityFallback && ReplicatedState.Driver == EHodgeCharacterFacingDriver::ActionDirection)
	{
		LocalState.Driver = ReplicatedState.Driver;
		LocalState.ActionRequestId = ReplicatedState.ActionRequestId; LocalState.ActionYaw = ReplicatedState.ActionYaw;
	}
	LocalState.Style = LocalState.BaseDriver == EHodgeCharacterFacingDriver::Controller ?
		EHodgeLocomotionStyle::ReservedStrafe : EHodgeLocomotionStyle::FreeDirectional;
	LocalState.RequestSequence = Pawn->HasAuthority() ? LastServerRequestSequence : LocalRequestSequence;
	const bool bChanged = Previous.BaseDriver != LocalState.BaseDriver || Previous.Driver != LocalState.Driver ||
		Previous.Style != LocalState.Style || Previous.ActionRequestId != LocalState.ActionRequestId ||
		Previous.RequestSequence != LocalState.RequestSequence;
	if (bChanged)
	{
		LocalState.bRecoveringFacing = !LocalState.bYawLocked;
		++LocalState.StateVersion;
	}
	ApplyResolvedMode();
	if (bChanged || ReplicatedState.StateVersion != LocalState.StateVersion) { PublishAuthorityState(); }
	if (bChanged) { OnFacingStateChanged.Broadcast(Previous, LocalState); }
	if (ActionId.IsValid())
	{
		const auto Status = Requests.Find(ActionId) ? Requests.Find(ActionId)->Status : EHodgeFacingRequestStatus::Invalid;
		if (LocalState.bYawLocked && Status == EHodgeFacingRequestStatus::Pending) { SetRequestStatus(ActionId, EHodgeFacingRequestStatus::Blocked); }
		else if (!LocalState.bYawLocked && Status == EHodgeFacingRequestStatus::Blocked) { SetRequestStatus(ActionId, EHodgeFacingRequestStatus::Pending); }
	}
	while (CompletedRequests.Num() > 64) { CompletedRequests.Remove(CompletedRequests.CreateConstIterator().Key()); }
}

void UHodgeCharacterRotationComponent::ApplyResolvedMode()
{
	if (!IsFacingSystemReady()) { return; }
	if (auto* Character = GetPawn<ACharacter>())
	{
		const auto State = GetResolvedState();
		Character->bUseControllerRotationYaw = State.Driver == EHodgeCharacterFacingDriver::Controller;
		Character->GetCharacterMovement()->bOrientRotationToMovement = State.Driver == EHodgeCharacterFacingDriver::Movement;
		Character->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
}

FHodgeFacingPresentationSnapshot UHodgeCharacterRotationComponent::GetFacingPresentationSnapshot() const
{
	const auto State = GetResolvedState(); FHodgeFacingPresentationSnapshot Result;
	Result.Style = State.Style; Result.Driver = State.Driver; Result.StateVersion = State.StateVersion;
	Result.bYawLocked = State.bYawLocked; Result.bRecoveringFacing = State.bRecoveringFacing;
	return Result;
}

void UHodgeCharacterRotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* Tick)
{
	Super::TickComponent(DeltaTime, TickType, Tick);
	if (!IsFacingSystemReady()) { UninitializeFromAbilitySystem(); return; }
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) { ApplyResolvedMode(); return; }
	TArray<FGuid> Expired;
	for (const auto& Pair : Requests)
	{
		const bool bDead = AbilitySystemComponent->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death);
		const auto* Definition = Cast<UHodgeGameplayAbility_Definition>(Pair.Value.Source.Get());
		if (bDead || !IsValidSource(Pair.Value.Source.Get(), Pair.Value.Driver == EHodgeCharacterFacingDriver::ActionDirection) ||
			(Pair.Value.Driver == EHodgeCharacterFacingDriver::Controller && !Pawn->Controller) ||
			(Definition && Pair.Value.Driver == EHodgeCharacterFacingDriver::ActionDirection && Definition->GetExecutionId() != Pair.Value.ExecutionId))
		{ Expired.Add(Pair.Key); }
	}
	for (const FGuid& Id : Expired)
	{ FHodgeFacingRequestHandle Handle; Handle.Id = Id; Handle.AvatarGeneration = AvatarGeneration; ReleaseFacingRequest(Handle); }
	RefreshLocalState();
	if (!LocalState.bYawLocked && LocalState.bRecoveringFacing)
	{
		float DesiredYaw = Pawn->GetActorRotation().Yaw;
		if (LocalState.Driver == EHodgeCharacterFacingDriver::ActionDirection) { DesiredYaw = LocalState.ActionYaw; }
		else if (LocalState.Driver == EHodgeCharacterFacingDriver::Controller && Pawn->Controller) { DesiredYaw = Pawn->Controller->GetControlRotation().Yaw; }
		else if (const auto* Character = Cast<ACharacter>(Pawn); Character && !Character->GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero())
		{ DesiredYaw = Character->GetCharacterMovement()->GetCurrentAcceleration().Rotation().Yaw; }
		NotifyFacingApplied(DesiredYaw);
	}
}

void UHodgeCharacterRotationComponent::SendRequest(const FGuid& Id, bool bRelease)
{
	const FRequest* Request = Requests.Find(Id);
	if (!Request || !IsFacingSystemReady()) { return; }
	FGameplayAbilitySpecHandle AbilityHandle; int32 Key = 0;
	if (const auto* Ability = Cast<UGameplayAbility>(Request->Source.Get()))
	{ AbilityHandle = Ability->GetCurrentAbilitySpecHandle(); Key = Ability->GetCurrentActivationInfo().GetActivationPredictionKey().Current; }
	ServerFacingRequest(GetOwner(), LocalRequestSequence, Id, bRelease, Request->Driver,
		Request->Yaw, Request->bInstant, AbilityHandle, Key, Request->ExecutionId);
}

void UHodgeCharacterRotationComponent::ServerFacingRequest_Implementation(AActor* Avatar, uint32 Sequence, FGuid Id,
	bool bRelease, EHodgeCharacterFacingDriver Driver, float Yaw, bool bInstant, FGameplayAbilitySpecHandle AbilityHandle,
	int32 PredictionKey, FGuid ExecutionId)
{
	bool bAccepted = false;
	if (IsFacingSystemReady() && Avatar == GetOwner() && Id.IsValid() && Sequence > LastServerRequestSequence &&
		Sequence - LastServerRequestSequence <= 64)
	{
		LastServerRequestSequence = Sequence;
		if (bRelease)
		{
			if (const FRequest* Existing = Requests.Find(Id); Existing && Existing->bRemoteClient)
			{ Requests.Remove(Id); bAccepted = true; }
		}
		else if (!Requests.Contains(Id) && Requests.Num() < 32 && FMath::IsFinite(Yaw) &&
			!AbilitySystemComponent->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death))
		{
			UObject* Source = GetOwner();
			if (AbilityHandle.IsValid())
			{
				const auto* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(AbilityHandle);
				UGameplayAbility* Ability = Spec ? Spec->GetPrimaryInstance() : nullptr;
				Source = Ability && Ability->IsActive() && Ability->GetCurrentActivationInfo().GetActivationPredictionKey().Current == PredictionKey ? Ability : nullptr;
			}
			const auto* Definition = Cast<UHodgeGameplayAbility_Definition>(Source);
			const bool bAction = Driver == EHodgeCharacterFacingDriver::ActionDirection;
			const bool bLegalDriver = HodgeFacing::IsBaseDriver(Driver) || (bAction && AbilityHandle.IsValid() && ExecutionId.IsValid() && !IsYawLocked());
			if (Source && bLegalDriver && IsValidSource(Source, bAction) &&
				(Driver != EHodgeCharacterFacingDriver::Controller || (bAllowControllerFacingRequests && GetPawn<APawn>()->Controller)) &&
				(!Definition || !bAction || (Definition->GetExecutionId().IsValid() && !Definition->IsExecutionEnding())))
			{
				FRequest& Request = Requests.Add(Id); Request.Source = Source; Request.Driver = Driver;
				Request.Yaw = FRotator::NormalizeAxis(Yaw); Request.Order = ++NextRequestOrder;
				// Definition 的本地 GUID 不跨端共享，服务器用已验证 Spec / PredictionKey 映射自己的执行。
				Request.ExecutionId = Definition && bAction ? Definition->GetExecutionId() : ExecutionId;
				Request.bRemoteClient = true; Request.bInstant = bInstant;
				Request.Status = bAction ? EHodgeFacingRequestStatus::Pending : EHodgeFacingRequestStatus::Applied;
				bAccepted = true;
			}
		}
		ResolveRequests();
		if (bAccepted && !bRelease && bInstant && Driver == EHodgeCharacterFacingDriver::ActionDirection && !IsYawLocked())
		{
			if (auto* Character = GetPawn<ACharacter>())
			{ Character->GetCharacterMovement()->MoveUpdatedComponent(FVector::ZeroVector, FRotator(0.f, Yaw, 0.f), false); NotifyFacingApplied(Yaw); }
		}
	}
	ClientFacingResult(Sequence, Id, bAccepted, LocalState);
}

void UHodgeCharacterRotationComponent::ClientFacingResult_Implementation(uint32 Sequence, FGuid Id, bool bAccepted,
	FHodgeCharacterRotationState State)
{
	if (!IsFacingSystemReady() || Sequence <= AcknowledgedSequence || Sequence > LocalRequestSequence) { return; }
	AcknowledgedSequence = Sequence;
	if (State.StateVersion >= ReplicatedState.StateVersion) { ReplicatedState = State; }
	if (!bAccepted && Requests.Contains(Id))
	{ Requests.Remove(Id); CompletedRequests.Add(Id, EHodgeFacingRequestStatus::Rejected); }
	ResolveRequests();
}

bool UHodgeCharacterRotationComponent::BeginServerMove(uint32 Sequence)
{
	if (!GetOwner()->HasAuthority() || !IsFacingSystemReady()) { return false; }
	// 未获授权的移动包使用当前状态，不接受客户端直接携带的朝向模式。
	const auto* Historical = ServerRequestHistory.Find(Sequence);
	ServerMoveState = Historical ? *Historical : LocalState;
	if (LocalState.bYawLocked) { ServerMoveState.bYawLocked = true; ServerMoveState.LockedYaw = LocalState.LockedYaw; }
	bServerMoveOverride = true;
	ApplyResolvedMode();
	return Historical != nullptr;
}

void UHodgeCharacterRotationComponent::EndServerMove()
{
	bServerMoveOverride = false;
	ApplyResolvedMode();
}

#if WITH_EDITOR
EDataValidationResult UHodgeCharacterRotationComponent::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult Parent = Super::IsDataValid(Context);
	if (!FMath::IsFinite(RecoveryTurnRate) || RecoveryTurnRate <= 0.f || !FMath::IsFinite(ActionTurnRate) || ActionTurnRate <= 0.f)
	{
		Context.AddError(NSLOCTEXT("HodgeFacing", "InvalidTurnRates", "Facing turn rates must be finite and greater than zero."));
		return EDataValidationResult::Invalid;
	}
	return Parent == EDataValidationResult::Invalid ? Parent : EDataValidationResult::Valid;
}
#endif
