#include "AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h"
#include "Component/HodgeHitReactionComponent.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_HitReaction)

UHodgeGameplayAbility_HitReaction::UHodgeGameplayAbility_HitReaction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;
	ActivationGroup = EHodgeAbilityActivationGroup::Independent;
	ActivationBlockedTags.AddTag(HodgeGameplayTags::Status_Death);
	bServerRespectsRemoteAbilityCancellation = false;
	bAllowWhileHitReacting = true;
}

bool UHodgeGameplayAbility_HitReaction::CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* RelevantTags) const
{
	const auto* Component = Info && Info->AvatarActor.IsValid() ? Info->AvatarActor->FindComponentByClass<UHodgeHitReactionComponent>() : nullptr;
	return Component && (!Info->IsNetAuthority() || Component->IsControlled()) &&
		Super::CanActivateAbility(Handle, Info, SourceTags, TargetTags, RelevantTags);
}

void UHodgeGameplayAbility_HitReaction::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Payload)
{
	bEnding = false;
	AppliedSequence = 0;
	AppliedPhase = EHodgeHitReactionPhase::None;
	AppliedStartedAt = -1.f;
	Reaction = Info->AvatarActor->FindComponentByClass<UHodgeHitReactionComponent>();
	if (!Reaction.IsValid()) { FinishReaction(true); return; }
	ReactionAvatar = Cast<ACharacter>(Info->AvatarActor.Get());
	if (Info->IsNetAuthority() && ReactionAvatar.IsValid())
	{
		PoseCombat = UHodgeCombatComponentBase::FindCombatComponent(ReactionAvatar.Get());
		if (PoseCombat.IsValid()) { PoseLease = PoseCombat->AcquirePoseLease(); }
		if (!PoseLease.IsValid())
		{
			PoseMesh = ReactionAvatar->GetMesh();
			SavedPosePolicy = uint8(PoseMesh->VisibilityBasedAnimTickOption);
			PoseMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		}
	}
	Reaction->AttachAbility(this);
	if (Reaction->IsControlled() && !Synchronize(Reaction->GetReactionState()) && Info->IsNetAuthority()) { FinishReaction(true); return; }
	Super::ActivateAbility(Handle, Info, ActivationInfo, Payload);
}

bool UHodgeGameplayAbility_HitReaction::Synchronize(const FHodgeHitReactionState& State)
{
	if (!IsActive() || bEnding || !Reaction.IsValid() || !CurrentActorInfo ||
		CurrentActorInfo->AvatarActor.Get() != Reaction->GetOwner()) { return false; }
	if (!State.IsActive()) { return true; }
	if (AppliedSequence == State.Sequence && AppliedPhase == State.Phase && PlayedMontage.Get() == State.Montage && AppliedStartedAt == State.StartedAt) { return true; }
	if (State.Montage && !CurrentActorInfo->GetAnimInstance()) { return false; }
	auto* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) { return false; }
	auto* Movement = Character->GetCharacterMovement();
	auto* ASC = GetAbilitySystemComponentFromActorInfo();
	if (State.Sequence != AppliedSequence && State.Phase != EHodgeHitReactionPhase::Airborne) { Movement->StopMovementImmediately(); }
	const bool bNewMotion = State.Sequence != AppliedSequence;
	if (bNewMotion || State.Phase != EHodgeHitReactionPhase::Airborne) { ClearMotion(); }
	const float Elapsed = FMath::Max(0.f, Reaction->GetServerTime() - State.StartedAt);
	if (bNewMotion && State.bHasMovement && (CurrentActorInfo->IsNetAuthority() || CurrentActorInfo->IsLocallyControlled()))
	{
		if (State.Type == EHodgeImpactType::Knockback && Elapsed < State.MoveDuration)
		{
			auto Force = MakeShared<FRootMotionSource_ConstantForce>();
			Force->InstanceName = FName(*FString::Printf(TEXT("HodgeReaction_%d"), State.Sequence));
			Force->Priority = 500;
			Force->AccumulateMode = ERootMotionAccumulateMode::Override;
			Force->Force = State.MotionVelocity;
			Force->Duration = State.MoveDuration;
			Force->SetTime(Elapsed);
			Force->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
			Force->FinishVelocityParams.SetVelocity = FVector::ZeroVector;
			MotionSourceId = Movement->ApplyRootMotionSource(Force);
		}
		else if (State.Type != EHodgeImpactType::Knockback && State.Phase == EHodgeHitReactionPhase::Airborne)
		{
			FVector Velocity = State.MotionVelocity;
			Velocity.Z += Movement->GetGravityZ() * Elapsed;
			QueuedLaunchVelocity = Velocity;
			Character->LaunchCharacter(Velocity, true, true);
		}
	}
	if (ASC->GetAnimatingAbility() == this && PlayedMontage.IsValid())
	{
		ASC->CurrentMontageStop(0.05f);
		ASC->ClearAnimatingAbility(this);
	}
	PlayedMontage.Reset();
	if (State.Montage && (CurrentActorInfo->IsNetAuthority() || CurrentActorInfo->IsLocallyControlled()))
	{
		const float Position = FMath::Min(Elapsed, FMath::Max(0.f, State.Montage->GetPlayLength() - 0.01f));
		if (ASC->PlayMontage(this, CurrentActivationInfo, State.Montage, 1.f, NAME_None, Position) <= 0.f) { return false; }
		PlayedMontage = State.Montage;
		if (UAnimInstance* Anim = CurrentActorInfo->GetAnimInstance())
		{
			if (FAnimMontageInstance* Instance = Anim->GetActiveInstanceForMontage(State.Montage))
			{ Instance->bEnableAutoBlendOut = State.Phase != EHodgeHitReactionPhase::Downed; }
		}
	}
	AppliedSequence = State.Sequence;
	AppliedPhase = State.Phase;
	AppliedStartedAt = State.StartedAt;
	return true;
}

void UHodgeGameplayAbility_HitReaction::ClearMotion()
{
	if (!QueuedLaunchVelocity.IsNearlyZero())
	{
		if (auto* Character = ReactionAvatar.Get())
		{
			auto* Movement = Character->GetCharacterMovement();
			if (Movement->PendingLaunchVelocity.Equals(QueuedLaunchVelocity)) { Movement->Launch(FVector::ZeroVector); }
		}
		QueuedLaunchVelocity = FVector::ZeroVector;
	}
	if (MotionSourceId)
	{
		if (auto* Character = ReactionAvatar.Get())
		{ Character->GetCharacterMovement()->RemoveRootMotionSourceByID(MotionSourceId); }
		MotionSourceId = 0;
	}
}

void UHodgeGameplayAbility_HitReaction::FinishReaction(bool bCancelled)
{
	if (IsActive() && !bEnding)
	{ EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, CurrentActorInfo && CurrentActorInfo->IsNetAuthority(), bCancelled); }
}

void UHodgeGameplayAbility_HitReaction::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled)
{
	if (bEnding || !IsActive()) { return; }
	TGuardValue<bool> Guard(bEnding, true);
	ClearMotion();
	if (PoseCombat.IsValid()) { PoseCombat->ReleasePoseLease(PoseLease); }
	if (PoseMesh.IsValid() && PoseMesh->VisibilityBasedAnimTickOption == EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones)
	{ PoseMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption(SavedPosePolicy); }
	PoseLease.Invalidate();
	PoseCombat.Reset();
	PoseMesh.Reset();
	if (auto* ASC = GetAbilitySystemComponentFromActorInfo(); ASC && ASC->GetAnimatingAbility() == this &&
		Info && Info->AvatarActor.Get() == ReactionAvatar.Get())
	{
		ASC->CurrentMontageStop();
		ASC->ClearAnimatingAbility(this);
	}
	PlayedMontage.Reset();
	if (Reaction.IsValid()) { Reaction->DetachAbility(this); }
	Reaction.Reset();
	ReactionAvatar.Reset();
	Super::EndAbility(Handle, Info, ActivationInfo, bReplicate, bCancelled);
}
