#include "Component/HodgeHitReactionComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Death.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Data/HodgeHitReactionProfile.h"
#include "Data/HodgePawnData.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "AIController.h"
#include "Net/UnrealNetwork.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHitReactionComponent)

UHodgeHitReactionComponent::UHodgeHitReactionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

bool UHodgeHitReactionComponent::IsBound() const
{
	return ASC.IsValid() && ASC->GetAvatarActor() == GetOwner() && IsRegistered();
}

void UHodgeHitReactionComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)
{
	if (!InASC || InASC->GetAvatarActor() != GetOwner()) { return; }
	if (ASC.Get() != InASC) { UninitializeFromAbilitySystem(); ASC = InASC; }
	RefreshConfiguration();
	OnRep_State();
	SetComponentTickEnabled(true);
}

void UHodgeHitReactionComponent::UninitializeFromAbilitySystem()
{
	if (ActiveAbility.IsValid()) { ActiveAbility->FinishReaction(true); }
	SetControl(false);
	ActiveAbility.Reset();
	ASC.Reset();
	PendingHits.Reset();
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		State.Phase = EHodgeHitReactionPhase::None;
		Profile = nullptr;
		GetOwner()->ForceNetUpdate();
	}
	SetComponentTickEnabled(false);
}

void UHodgeHitReactionComponent::RefreshConfiguration()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) { return; }
	const auto* Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(GetOwner());
	const auto* PawnData = Extension ? Extension->GetPawnData<UHodgePawnData>() : nullptr;
	UHodgeHitReactionProfile* Desired = OverrideProfile ? OverrideProfile.Get() : PawnData ? PawnData->HitReactionProfile.Get() : nullptr;
	if (Profile != Desired)
	{
		CancelReaction();
		Profile = Desired;
		GetOwner()->ForceNetUpdate();
	}
}

FGuid UHodgeHitReactionComponent::BeginHit(const FHodgeHitReactionConfig& Request, AActor* Source, FVector SourceOrigin)
{
	if (!GetOwner()->HasAuthority() || !IsBound() || !Request.IsConfigured() || SourceOrigin.ContainsNaN()) { return {}; }
	FPendingHit Hit;
	Hit.Request = Request;
	Hit.Source = Source;
	Hit.Avatar = ASC->GetAvatarActor();
	Hit.Origin = SourceOrigin;
	Hit.BodyRank = HodgeHitReaction::ResolveBody(ASC.Get(), Hit.BodyTag);
	const FGuid Id = FGuid::NewGuid();
	PendingHits.Add(Id, MoveTemp(Hit));
	return Id;
}

void UHodgeHitReactionComponent::RecordDamage(const FGuid& HitId, bool bAccepted, float ActualDamage)
{
	if (!GetOwner()->HasAuthority() || !IsBound()) { return; }
	if (FPendingHit* Hit = PendingHits.Find(HitId); Hit && Hit->Avatar.Get() == ASC->GetAvatarActor())
	{
		Hit->bAccepted |= bAccepted;
		Hit->bRejected |= !bAccepted;
		if (bAccepted && FMath::IsFinite(ActualDamage)) { Hit->Damage += FMath::Max(ActualDamage, 0.f); }
	}
}

FHodgeHitReactionResult UHodgeHitReactionComponent::FinishHit(const FGuid& HitId)
{
	FHodgeHitReactionResult Result;
	Result.HitId = HitId;
	FPendingHit Hit;
	if (!GetOwner()->HasAuthority() || !PendingHits.RemoveAndCopyValue(HitId, Hit) || !IsBound() || Hit.Avatar.Get() != GetOwner()) { return Result; }
	Result.AttackJudgementTag = Hit.Request.AttackJudgementTag;
	Result.TargetBodyTag = Hit.BodyTag;
	Result.TargetBodyRank = Hit.BodyRank;
	Result.AttackRank = HodgeHitReaction::JudgementRank(Hit.Request.AttackJudgementTag);
	Result.ActualDamage = Hit.Damage;
	Result.ReservedPoiseDamage = Hit.Request.ReservedPoiseDamage;
	TArray<FText> Errors;
	const auto* Health = ASC->GetSet<UHodgeHealthSet>();
	const bool bExplicitControl = Hit.Request.HitAcceptancePolicy == EHodgeHitAcceptancePolicy::ExplicitControl;
	if (!Hit.Request.Validate(Errors)) { Result.Outcome = EHodgeHitReactionOutcome::InvalidRequest; }
	else if (ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) || (Health && Health->GetHealth() <= 0.f))
	{ Result.Outcome = EHodgeHitReactionOutcome::Dead; }
	else if (Hit.bRejected || (bExplicitControl && (ASC->HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) || ASC->HasMatchingGameplayTag(HodgeGameplayTags::Cheat_GodMode))))
	{ Result.Outcome = EHodgeHitReactionOutcome::Rejected; }
	else if (!Hit.bAccepted && !bExplicitControl) { Result.Outcome = EHodgeHitReactionOutcome::NoDamageOutput; }
	else if (!HodgeHitReaction::CanImpact(Result.AttackRank, Result.TargetBodyRank))
	{ Result.Outcome = EHodgeHitReactionOutcome::LowJudgement; }
	else if (Hit.Request.Impacts.IsEmpty()) { Result.Outcome = EHodgeHitReactionOutcome::NoImpact; }
	else
	{
		RefreshConfiguration();
		FHodgeHitReactionState Plan;
		if (!Profile) { Result.Outcome = EHodgeHitReactionOutcome::NotReady; }
		else if (!BuildPlan(Hit, Plan, Result.AppliedImpacts)) { Result.Outcome = EHodgeHitReactionOutcome::Unsupported; }
		else if (SubmitPlan(Plan, Result.Outcome)) { Result.Outcome = EHodgeHitReactionOutcome::Applied; }
		if (Result.Outcome != EHodgeHitReactionOutcome::Applied) { Result.AppliedImpacts.Reset(); }
	}
	if (Result.Outcome == EHodgeHitReactionOutcome::LowJudgement || Result.Outcome == EHodgeHitReactionOutcome::Unsupported || Result.Outcome == EHodgeHitReactionOutcome::NoImpact)
	{
		const FVector Direction = (GetOwner()->GetActorLocation() - Hit.Origin).GetSafeNormal();
		if (Profile && Profile->LightFeedbackCue.IsValid())
		{
			FGameplayCueParameters Parameters;
			Parameters.Location = GetOwner()->GetActorLocation();
			Parameters.Normal = Direction;
			Parameters.Instigator = Hit.Source.Get();
			ASC->ExecuteGameplayCue(Profile->LightFeedbackCue, Parameters);
		}
		MulticastLightFeedback(Direction);
	}
	LastResult = Result;
	OnReactionResolved.Broadcast(Result);
	return Result;
}

bool UHodgeHitReactionComponent::BuildPlan(const FPendingHit& Hit, FHodgeHitReactionState& Plan, TArray<EHodgeImpactType>& Applied) const
{
	TArray<FText> Errors;
	if (!Profile || !Profile->Validate(Errors)) { return false; }
	const auto* Character = Cast<ACharacter>(GetOwner());
	if (!Character) { return false; }
	const bool bAir = Character->GetCharacterMovement()->IsFalling();
	const auto* Movement = Character->GetCharacterMovement();
	const bool bCanMove = Movement->IsActive() && Movement->MovementMode != MOVE_None && Movement->HasValidData();
	Plan.bKnockdownOnLanding = bAir && State.bKnockdownOnLanding;
	const auto* MeshAsset = Character->GetMesh()->GetSkeletalMeshAsset();
	float Duration = 0.f;
	const FHodgeHitReactionAnimation* Presentation = nullptr;
	for (const auto& Requested : Hit.Request.Impacts)
	{
		const auto* Entry = Profile->FindAnimation(Requested.Type);
		const bool bApplicable = (!Requested.IsMovement() || bCanMove) && ((Requested.Type != EHodgeImpactType::AirHit && Requested.Type != EHodgeImpactType::Slam) || bAir) &&
			(Requested.Type != EHodgeImpactType::Knockback || !bAir) &&
			(Requested.Type != EHodgeImpactType::AirHit || State.AirHitCount < Profile->MaxAirHits);
		bool bFallback = false;
		if (!Entry || !bApplicable)
		{
			if (!Profile->FallbackToHitStun.Contains(Requested.Type)) { continue; }
			Entry = Profile->FindAnimation(EHodgeImpactType::HitStun);
			bFallback = true;
		}
		if (bAir && (bFallback || Requested.Type == EHodgeImpactType::HitStun))
		{ Entry = Profile->FindAnimation(EHodgeImpactType::AirHit); }
		if (!Entry || (!Entry->Montage && !Entry->bAllowWithoutMontage)) { continue; }
		for (UAnimMontage* Montage : {Entry->Montage.Get(), Entry->AirLoopMontage.Get(), Entry->LandingMontage.Get(), Entry->GetUpMontage.Get()})
		{ if (Montage && !HodgeHitReaction::CanUseMontage(Montage, MeshAsset)) { return false; } }
		const EHodgeImpactType Type = bFallback ? EHodgeImpactType::HitStun : Requested.Type;
		Applied.AddUnique(Type);
		Duration = FMath::Max(Duration, Requested.ControlDuration > 0.f ? Requested.ControlDuration : Entry->DefaultDuration);
		if (!Presentation || Type != EHodgeImpactType::HitStun) { Presentation = Entry; Plan.Type = Type; }
		if (!bFallback && Requested.IsMovement())
		{
			Plan.bHasMovement = true;
			Plan.MoveDuration = FMath::Clamp(Requested.MoveDuration, 0.01f, Profile->MaxControlDuration);
			FVector Direction = Requested.Direction == EHodgeImpactDirection::WorldDirection ? Requested.WorldDirection.GetSafeNormal2D() :
				Requested.Direction == EHodgeImpactDirection::SourceForward && Hit.Source.IsValid() ? Hit.Source->GetActorForwardVector().GetSafeNormal2D() :
				(GetOwner()->GetActorLocation() - Hit.Origin).GetSafeNormal2D();
			if (Direction.IsNearlyZero()) { Direction = GetOwner()->GetActorForwardVector().GetSafeNormal2D(); }
			Plan.MotionVelocity = Direction * (Type == EHodgeImpactType::Knockback ?
				FMath::Min(Requested.KnockbackDistance / Plan.MoveDuration, Profile->MaxLaunchSpeed) : FMath::Min(Requested.HorizontalSpeed, Profile->MaxLaunchSpeed));
			if (Type != EHodgeImpactType::Knockback)
			{
				Plan.MotionVelocity.Z = FMath::Min(Requested.VerticalSpeed, Profile->MaxLaunchSpeed) * (Type == EHodgeImpactType::Slam ? -1.f : 1.f);
				Plan.bKnockdownOnLanding |= Requested.bKnockdownOnLanding || Type == EHodgeImpactType::Slam;
			}
			Duration = FMath::Max(Duration, Plan.MoveDuration);
		}
	}
	if (!Presentation || Applied.IsEmpty()) { return false; }
	Plan.Montage = Presentation->Montage;
	Plan.AirLoopMontage = Presentation->AirLoopMontage;
	Plan.LandingMontage = Presentation->LandingMontage;
	Plan.GetUpMontage = Presentation->GetUpMontage;
	if (Plan.Montage && (!ASC.IsValid() || !ASC->AbilityActorInfo->GetAnimInstance())) { return false; }
	if (bAir && Plan.Type == EHodgeImpactType::Knockdown)
	{
		const auto* Air = Profile->FindAnimation(EHodgeImpactType::AirHit);
		if (!Air) { return false; }
		Plan.bKnockdownOnLanding = true;
		Plan.LandingMontage = Plan.Montage;
		Plan.Montage = Air->Montage;
	}
	if (Plan.bKnockdownOnLanding && !Profile->FindAnimation(EHodgeImpactType::Knockdown)) { return false; }
	const float Now = GetServerTime();
	Plan.StartedAt = Now;
	Plan.ChainStartedAt = State.IsActive() ? State.ChainStartedAt : Now;
	Plan.EndsAt = FMath::Min(Now + Duration, Plan.ChainStartedAt + Profile->MaxControlDuration);
	Plan.Phase = bAir || Plan.Type == EHodgeImpactType::Launch || Plan.Type == EHodgeImpactType::Slam ? EHodgeHitReactionPhase::Airborne :
		Plan.Type == EHodgeImpactType::Knockdown ? EHodgeHitReactionPhase::Downed : EHodgeHitReactionPhase::Controlled;
	if (Plan.Phase == EHodgeHitReactionPhase::Airborne)
	{
		Plan.AirStartedAt = State.Phase == EHodgeHitReactionPhase::Airborne ? State.AirStartedAt : Now;
		Plan.AirOriginZ = State.Phase == EHodgeHitReactionPhase::Airborne ? State.AirOriginZ : GetOwner()->GetActorLocation().Z;
		Plan.AirHitCount = State.Phase == EHodgeHitReactionPhase::Airborne ? State.AirHitCount : 0;
		Plan.AirHitCount += Plan.Type == EHodgeImpactType::AirHit ? 1 : 0;
		Plan.EndsAt = FMath::Min(Plan.AirStartedAt + Profile->MaxAirborneDuration, Plan.ChainStartedAt + Profile->MaxControlDuration);
	}
	if (Plan.Phase == EHodgeHitReactionPhase::Downed)
	{ Plan.EndsAt = FMath::Min(Now + Profile->DownedDuration, Plan.ChainStartedAt + Profile->MaxControlDuration); }
	return Plan.EndsAt > Now;
}

bool UHodgeHitReactionComponent::SubmitPlan(FHodgeHitReactionState Plan, EHodgeHitReactionOutcome& Failure)
{
	Failure = EHodgeHitReactionOutcome::NotReady;
	if (!IsBound() || !GetOwner()->HasAuthority()) { return false; }
	if (bSubmitting || bCompleting) { Failure = EHodgeHitReactionOutcome::Busy; return false; }
	TGuardValue<bool> Guard(bSubmitting, true);
	FGameplayAbilitySpecHandle ReactionHandle;
	TArray<TWeakObjectPtr<UGameplayAbility>> ToCancel;
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.Ability->IsA<UHodgeGameplayAbility_HitReaction>())
		{
			if (ReactionHandle.IsValid()) { return false; }
			ReactionHandle = Spec.Handle;
			continue;
		}
		for (UGameplayAbility* Ability : Spec.GetAbilityInstances())
		{
			if (!Ability || !Ability->IsActive() || Ability->IsA<UHodgeGameplayAbility_Death>()) { continue; }
			if (Ability->IsA<UHodgeGameplayAbility_Definition>() || Ability->IsA<UHodgeGameplayAbility_Jump>() ||
				Ability->GetAssetTags().HasAny(Profile->InterruptibleAbilityTags))
			{
				if (!Ability->CanBeCanceled()) { Failure = EHodgeHitReactionOutcome::CannotInterrupt; return false; }
				ToCancel.Add(Ability);
			}
		}
	}
	if (!ReactionHandle.IsValid()) { return false; }
	SetControl(true);
	for (const auto& WeakAbility : ToCancel)
	{
		if (!IsBound()) { SetControl(false); return false; }
		if (UGameplayAbility* Ability = WeakAbility.Get(); Ability && Ability->IsActive())
		{
			Ability->CancelAbility(Ability->GetCurrentAbilitySpecHandle(), ASC->AbilityActorInfo.Get(), Ability->GetCurrentActivationInfo(), true);
			if (Ability->IsActive()) { Failure = EHodgeHitReactionOutcome::CannotInterrupt; SetControl(State.IsActive()); return false; }
		}
	}
	if (!IsBound() || ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death)) { SetControl(false); return false; }
	NextSequence = NextSequence == MAX_int32 ? 1 : NextSequence + 1;
	Plan.Sequence = NextSequence;
	State = Plan;
	if (auto* AI = Cast<AAIController>(CastChecked<ACharacter>(GetOwner())->GetController())) { AI->StopMovement(); }
	if (State.Phase != EHodgeHitReactionPhase::Airborne) { CastChecked<ACharacter>(GetOwner())->GetCharacterMovement()->StopMovementImmediately(); }
	bSawAirborne = CastChecked<ACharacter>(GetOwner())->GetCharacterMovement()->IsFalling();
	SetComponentTickEnabled(true);
	PublishState();
	if (!ActiveAbility.IsValid() && !ASC->TryActivateAbility(ReactionHandle, false)) { CompletePlan(State.Sequence); return false; }
	if (!State.IsActive() || !ActiveAbility.IsValid() || !ActiveAbility->IsActive()) { CompletePlan(State.Sequence); return false; }
	return true;
}

void UHodgeHitReactionComponent::SetControl(bool bControlled)
{
	if (!ASC.IsValid()) { bOwnsControl = false; return; }
	if (bControlled == bOwnsControl) { return; }
	if (bControlled) { ASC->AddLooseGameplayTag(HodgeHitReactionTags::Controlled); }
	else { ASC->RemoveLooseGameplayTag(HodgeHitReactionTags::Controlled); }
	bOwnsControl = bControlled;
}

void UHodgeHitReactionComponent::PublishState()
{
	GetOwner()->ForceNetUpdate();
	OnRep_State();
}

void UHodgeHitReactionComponent::OnRep_State()
{
	if (IsBound())
	{
		SetControl(State.IsActive());
		if (ActiveAbility.IsValid())
		{
			if (!State.IsActive()) { ActiveAbility->FinishReaction(); }
			else if (!ActiveAbility->Synchronize(State) && GetOwner()->HasAuthority()) { CompletePlan(State.Sequence); }
		}
	}
	SetComponentTickEnabled(State.IsActive() || !Profile);
	OnReactionStateChanged.Broadcast(State);
}

void UHodgeHitReactionComponent::OnRep_Profile() { OnRep_State(); }

void UHodgeHitReactionComponent::AttachAbility(UHodgeGameplayAbility_HitReaction* Ability)
{
	ActiveAbility = Ability;
	SetComponentTickEnabled(true);
}

void UHodgeHitReactionComponent::DetachAbility(UHodgeGameplayAbility_HitReaction* Ability)
{
	if (ActiveAbility.Get() != Ability) { return; }
	ActiveAbility.Reset();
	if (GetOwner()->HasAuthority()) { CompletePlan(State.Sequence); }
}

void UHodgeHitReactionComponent::CompletePlan(int32 Sequence)
{
	if (!GetOwner()->HasAuthority() || State.Sequence != Sequence || !State.IsActive()) { return; }
	TGuardValue<bool> Guard(bCompleting, true);
	if (ActiveAbility.IsValid() && ActiveAbility->IsActive()) { ActiveAbility->FinishReaction(); return; }
	State.Phase = EHodgeHitReactionPhase::None;
	State.Montage = nullptr;
	SetControl(false);
	PublishState();
}

void UHodgeHitReactionComponent::CancelReaction()
{
	if (GetOwner() && GetOwner()->HasAuthority()) { CompletePlan(State.Sequence); }
}

void UHodgeHitReactionComponent::ChangePhase(EHodgeHitReactionPhase Phase, UAnimMontage* Montage, float Duration)
{
	State.Phase = Phase;
	State.StartedAt = GetServerTime();
	State.EndsAt = FMath::Min(State.StartedAt + Duration, State.ChainStartedAt + Profile->MaxControlDuration);
	State.Montage = Montage;
	PublishState();
}

bool UHodgeHitReactionComponent::RequestGetUp()
{
	if (!GetOwner()->HasAuthority() || State.Phase != EHodgeHitReactionPhase::Downed || !Profile) { return false; }
	if (State.GetUpMontage) { ChangePhase(EHodgeHitReactionPhase::GettingUp, State.GetUpMontage, State.GetUpMontage->GetPlayLength()); }
	else { CompletePlan(State.Sequence); }
	return true;
}

void UHodgeHitReactionComponent::TickPlan()
{
	if (!IsBound() || !Profile || !State.IsActive()) { return; }
	const auto* Health = ASC->GetSet<UHodgeHealthSet>();
	const float Now = GetServerTime();
	if (ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) || (Health && Health->GetHealth() <= 0.f) ||
		Now >= State.ChainStartedAt + Profile->MaxControlDuration) { CompletePlan(State.Sequence); return; }
	auto* Character = CastChecked<ACharacter>(GetOwner());
	auto* Movement = Character->GetCharacterMovement();
	if (auto* AI = Cast<AAIController>(Character->GetController())) { AI->StopMovement(); }
	if (State.Phase == EHodgeHitReactionPhase::Airborne)
	{
		bSawAirborne |= Movement->IsFalling();
		if (Character->GetActorLocation().Z - State.AirOriginZ >= Profile->MaxLaunchHeight && Movement->Velocity.Z > 0.f) { Movement->Velocity.Z = 0.f; }
		if (bSawAirborne && Movement->IsMovingOnGround())
		{
			if (State.bKnockdownOnLanding)
			{
				const auto* Down = Profile->FindAnimation(EHodgeImpactType::Knockdown);
				if (Down) { State.GetUpMontage = Down->GetUpMontage; ChangePhase(EHodgeHitReactionPhase::Downed, State.LandingMontage ? State.LandingMontage.Get() : Down->Montage.Get(), Profile->DownedDuration); }
				else { CompletePlan(State.Sequence); }
			}
			else if (State.LandingMontage) { ChangePhase(EHodgeHitReactionPhase::Controlled, State.LandingMontage, State.LandingMontage->GetPlayLength()); }
			else { CompletePlan(State.Sequence); }
		}
		else if (Now >= State.EndsAt) { CompletePlan(State.Sequence); }
		else if (State.AirLoopMontage &&
			(!State.Montage || Now - State.StartedAt >= State.Montage->GetPlayLength()))
		{
			const float EndsAt = State.EndsAt;
			ChangePhase(EHodgeHitReactionPhase::Airborne, State.AirLoopMontage, EndsAt - Now);
		}
	}
	else if (Now >= State.EndsAt)
	{
		if (State.Phase == EHodgeHitReactionPhase::Downed)
		{ if (Profile->bAutoGetUp) { RequestGetUp(); } }
		else { CompletePlan(State.Sequence); }
	}
}

float UHodgeHitReactionComponent::GetServerTime() const
{
	if (const auto* GameState = GetWorld() ? GetWorld()->GetGameState() : nullptr) { return GameState->GetServerWorldTimeSeconds(); }
	return GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
}

void UHodgeHitReactionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!IsBound()) { if (ASC.IsValid()) { UninitializeFromAbilitySystem(); } return; }
	if (!Profile) { RefreshConfiguration(); }
	if (GetOwner()->HasAuthority()) { TickPlan(); }
	else if (ActiveAbility.IsValid() && State.IsActive()) { ActiveAbility->Synchronize(State); }
	if (State.IsActive() && State.Montage)
	{
		if (const auto* Character = Cast<ACharacter>(GetOwner()))
		{
			if (UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance())
			{
				if (FAnimMontageInstance* Instance = Anim->GetActiveInstanceForMontage(State.Montage))
				{ Instance->bEnableAutoBlendOut = State.Phase != EHodgeHitReactionPhase::Downed; }
			}
		}
	}
}

void UHodgeHitReactionComponent::MulticastLightFeedback_Implementation(FVector Direction)
{
	if (Profile && Profile->LightFeedbackMontage && !Profile->LightFeedbackMontage->HasRootMotion())
	{
		if (auto* Character = Cast<ACharacter>(GetOwner()))
		{
			UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance();
			UAnimMontage* Main = ASC.IsValid() ? ASC->GetCurrentMontage() : nullptr;
			if (Anim && (!Main || Main->GetGroupName() != Profile->LightFeedbackMontage->GetGroupName()))
			{ Anim->Montage_Play(Profile->LightFeedbackMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, false); }
		}
	}
	OnLightFeedback.Broadcast(Direction);
}

void UHodgeHitReactionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHodgeHitReactionComponent, Profile);
	DOREPLIFETIME(UHodgeHitReactionComponent, State);
}

void UHodgeHitReactionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeFromAbilitySystem();
	Super::EndPlay(EndPlayReason);
}
