#include "Component/HodgeCharacterRotationComponent.h"

#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCharacterRotationComponent)

UHodgeCharacterRotationComponent::UHodgeCharacterRotationComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
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
	RotationTagHandle = InASC->RegisterGameplayTagEvent(HodgeGameplayTags::Status_Rotation_Locked,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HandleConstraintTagChanged);
	MovementStoppedTagHandle = InASC->RegisterGameplayTagEvent(TAG_Gameplay_MovementStopped,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HandleConstraintTagChanged);
	RefreshLocalState();
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
	}
	RotationTagHandle.Reset();
	MovementStoppedTagHandle.Reset();
	AbilitySystemComponent.Reset();
	LocalState = {};
	ClearMoveReplayState();
	PublishAuthorityState();
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
	const bool bNewLock = ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Rotation_Locked)
		|| ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped);
	if (bNewLock == LocalState.bYawLocked) { return; }
	LocalState.bYawLocked = bNewLock;
	LocalState.bRecoveringFacing = !bNewLock;
	if (bNewLock) { LocalState.LockedYaw = FRotator::NormalizeAxis(Pawn->GetActorRotation().Yaw); }
	PublishAuthorityState();
}

void UHodgeCharacterRotationComponent::PublishAuthorityState()
{
	AActor* Owner = GetOwner();
	if (Owner && Owner->HasAuthority())
	{
		ReplicatedState = LocalState;
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
	else if (State.bRecoveringFacing && GetOwner())
	{
		Result.Yaw = FMath::FixedTurn(GetOwner()->GetActorRotation().Yaw, DesiredRotation.Yaw,
			RecoveryTurnRate * FMath::Max(0.f, DeltaSeconds));
	}
	return Result;
}

void UHodgeCharacterRotationComponent::NotifyFacingApplied(float DesiredYaw)
{
	FHodgeCharacterRotationState& State = IsReplayingMove() ? ReplayState : LocalState;
	if (!State.bYawLocked && State.bRecoveringFacing && GetOwner()
		&& FMath::Abs(FMath::FindDeltaAngleDegrees(GetOwner()->GetActorRotation().Yaw, DesiredYaw)) < 1.f)
	{
		State.bRecoveringFacing = false;
		if (!IsReplayingMove()) { PublishAuthorityState(); }
	}
}

void UHodgeCharacterRotationComponent::SetMoveReplayState(const FHodgeCharacterRotationState& State)
{
	ReplayState = State;
	bHasReplayState = true;
}

void UHodgeCharacterRotationComponent::ClearMoveReplayState()
{
	bHasReplayState = false;
	ReplayState = {};
}

void UHodgeCharacterRotationComponent::OnRep_RotationState()
{
	const APawn* Pawn = GetPawn<APawn>();
	// 本地窗口决定预测锁的期限，权威结果只校正仍有效的锁定朝向。
	if (Pawn && Pawn->IsLocallyControlled() && LocalState.bYawLocked && ReplicatedState.bYawLocked)
	{
		LocalState.LockedYaw = ReplicatedState.LockedYaw;
	}
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
