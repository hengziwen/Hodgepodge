#include "AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgeLocomotionPolicyComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeHeroComponent.h"
#include "Data/HodgeSprintAbilityProfile.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "HAL/IConsoleManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_Sprint)

static TAutoConsoleVariable<int32> CVarHodgeSprintDebug(TEXT("Hodge.Movement.DebugSprint"), 0,
	TEXT("Log Sprint lifecycle and input exit conditions."), ECVF_Cheat);

UHodgeGameplayAbility_Sprint::UHodgeGameplayAbility_Sprint()
{ FGameplayTagContainer Tags = GetAssetTags(); Tags.AddTag(HodgeMovementTags::SprintAbility); SetAssetTags(Tags); }

bool FHodgeSprintPivotTargetData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bSuccess)
{ Ar << IncomingDirection << DesiredDirection << StartingYaw << SessionId << SequenceId; bSuccess = !Ar.IsError(); return bSuccess; }
bool UHodgeGameplayAbility_Sprint::QualifiesPivot(FVector Velocity, FVector Desired, float MinimumSpeed, float Angle)
{
	FVector Direction;
	return !Velocity.ContainsNaN() && FMath::IsFinite(MinimumSpeed) && MinimumSpeed >= 0.f &&
		FMath::IsFinite(Angle) && Angle >= 90.f && Angle <= 180.f && Velocity.Size2D() > FMath::Max(MinimumSpeed, KINDA_SMALL_NUMBER) &&
		HodgeFacing::NormalizeDirection(Desired, Direction) &&
		FVector::DotProduct(Velocity.GetSafeNormal2D(), Direction) <= FMath::Cos(FMath::DegreesToRadians(Angle)) + KINDA_SMALL_NUMBER;
}
bool UHodgeGameplayAbility_Sprint::IsPivotRecoveryComplete(float Position, float EndTime)
{ return FMath::IsFinite(Position) && FMath::IsFinite(EndTime) && EndTime > 0.f && Position >= EndTime; }
void UHodgeGameplayAbility_Sprint::ProcessMovementIntent(FVector Desired)
{
	if (!IsActive() || !bCommitted || !CurrentActorInfo || !CurrentActorInfo->IsLocallyControlled() ||
		!IsMovementValid() || !Policy->IsSessionHeld(SessionId)) { return; }
	UpdatePivotRecovery();
	RequestPivot(GetAvatarActorFromActorInfo()->GetVelocity(), Desired);
}

void UHodgeGameplayAbility_Sprint::RequestPivot(FVector Velocity, FVector Desired)
{
	if (!ExecutionProfile->bAllowSprintPivot || !ExecutionProfile->SprintPivotMontage || bPivoting ||
		GetWorld()->GetTimeSeconds() < NextPivotAt || !QualifiesPivot(Velocity, Desired, ExecutionProfile->PivotMinimumSpeed, ExecutionProfile->PivotTriggerAngle)) { return; }
	auto* Payload = new FHodgeSprintPivotTargetData(); Payload->IncomingDirection = Velocity.GetSafeNormal2D();
	Payload->DesiredDirection = Desired.GetSafeNormal2D(); Payload->SessionId = SessionId; Payload->SequenceId = PivotSequence + 1;
	Payload->StartingYaw = GetAvatarActorFromActorInfo()->GetActorRotation().Yaw;
	FGameplayAbilityTargetDataHandle Data(Payload);
	if (!CurrentActorInfo->IsNetAuthority())
	{ GetAbilitySystemComponentFromActorInfo()->ServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), Data, {}, GetAbilitySystemComponentFromActorInfo()->ScopedPredictionKey); }
	HandleMovementFollowUp(Data);
}
void UHodgeGameplayAbility_Sprint::HandleMovementFollowUp(const FGameplayAbilityTargetDataHandle& Data)
{
	if (!bCommitted || !IsMovementValid() || !Policy->IsSessionHeld(SessionId) || Data.Num() != 1 || !Data.Get(0) ||
		Data.Get(0)->GetScriptStruct() != FHodgeSprintPivotTargetData::StaticStruct() ||
		!ExecutionProfile->bAllowSprintPivot || !ExecutionProfile->SprintPivotMontage || GetWorld()->GetTimeSeconds() < NextPivotAt) { return; }
	const auto* Payload = static_cast<const FHodgeSprintPivotTargetData*>(Data.Get(0));
	if (Payload->SessionId != SessionId || Payload->SequenceId != PivotSequence + 1 || Payload->IncomingDirection.ContainsNaN() ||
		Payload->DesiredDirection.ContainsNaN() || !FMath::IsNearlyEqual(Payload->IncomingDirection.SizeSquared(), 1.f, .01f) ||
		!FMath::IsNearlyEqual(Payload->DesiredDirection.SizeSquared(), 1.f, .01f)) { return; }
	if (bPivoting)
	{
		// 远端请求可在旧动作恢复点前到达；保留同一会话的下一序号，之后重新验证。
		if (CurrentActorInfo->IsNetAuthority() && !CurrentActorInfo->IsLocallyControlled())
		{ DeferredPivotData = Data; DeferredPivotExpiresAt = GetWorld()->GetTimeSeconds() + ExecutionProfile->HandoffNetworkGrace; }
		return;
	}
	const FVector Velocity = GetAvatarActorFromActorInfo()->GetVelocity();
	if (Payload->SessionId != SessionId || Payload->SequenceId <= PivotSequence || Payload->SequenceId > PivotSequence + 1 ||
		!FMath::IsFinite(Payload->StartingYaw) || FMath::Abs(FMath::FindDeltaAngleDegrees(GetAvatarActorFromActorInfo()->GetActorRotation().Yaw, Payload->StartingYaw)) > 90.f ||
		!QualifiesPivot(Velocity, Payload->DesiredDirection, ExecutionProfile->PivotMinimumSpeed, ExecutionProfile->PivotTriggerAngle) ||
		Payload->IncomingDirection.ContainsNaN() || !FMath::IsNearlyEqual(Payload->IncomingDirection.SizeSquared(), 1.f, .01f) ||
		FVector::DotProduct(Payload->IncomingDirection, Velocity.GetSafeNormal2D()) < .8f) { return; }
	PivotSequence = Payload->SequenceId; StartPivot(Payload->StartingYaw);
}
void UHodgeGameplayAbility_Sprint::StartPivot(float StartingYaw)
{
	const auto* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	UAnimMontage* Montage = ExecutionProfile->SprintPivotMontage;
	if (!Character || !Character->GetMesh()->GetAnimInstance() || Montage->GetSkeleton() != Character->GetMesh()->GetSkeletalMeshAsset()->GetSkeleton()) { return; }
	// 拥有者保留当前朝向；服务器对齐验证过的起手快照，避免客户端起手瞬跳。
	if (CurrentActorInfo->IsNetAuthority() && FMath::Abs(FMath::FindDeltaAngleDegrees(Character->GetActorRotation().Yaw, StartingYaw)) > .1f)
	{
		const auto Align = Rotation->RequestActionFacing(FRotator(0.f, StartingYaw, 0.f).Vector(), this, ExecutionId, true);
		if (!Align.IsValid()) { return; } Rotation->ReleaseFacingRequest(Align);
	}
	ReleasePivotTask();
	bPivoting = true; BlockedAt = 0.; NextPivotAt = GetWorld()->GetTimeSeconds() + ExecutionProfile->PivotReentryDelay;
	PivotTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SprintPivot"), Montage, ExecutionProfile->SprintTurnPlayRate);
	PivotTask->OnCompleted.AddDynamic(this, &ThisClass::OnPivotFinished);
	PivotTask->OnInterrupted.AddDynamic(this, &ThisClass::OnPivotFinished);
	PivotTask->OnCancelled.AddDynamic(this, &ThisClass::OnPivotFinished);
	PivotTask->ReadyForActivation();
	if (const auto* Instance = Character->GetMesh()->GetAnimInstance()->GetActiveInstanceForMontage(Montage))
	{ PivotMontageInstanceId = Instance->GetInstanceID(); }
}
void UHodgeGameplayAbility_Sprint::ReleasePivotTask()
{
	if (!PivotTask) { return; }
	PivotTask->OnCompleted.RemoveAll(this); PivotTask->OnInterrupted.RemoveAll(this); PivotTask->OnCancelled.RemoveAll(this);
	PivotTask->EndTask(); PivotTask = nullptr;
}
void UHodgeGameplayAbility_Sprint::UpdatePivotRecovery()
{
	if (!bPivoting) { return; }
	const auto* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	auto* Anim = Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	const auto* Instance = Anim ? Anim->GetMontageInstanceForID(PivotMontageInstanceId) : nullptr;
	if (Instance && !IsPivotRecoveryComplete(Instance->GetPosition(), ExecutionProfile->PivotRecoveryEndTime)) { return; }
	bPivoting = false; PivotMontageInstanceId = INDEX_NONE; ReleasePivotTask();
	auto* ASC = GetAbilitySystemComponentFromActorInfo();
	if (Instance && ASC->GetAnimatingAbility() == this && ASC->GetCurrentMontage() == ExecutionProfile->SprintPivotMontage)
	{ ASC->CurrentMontageStop(ExecutionProfile->SprintTurnBlendOutTime); }
}
void UHodgeGameplayAbility_Sprint::OnPivotFinished()
{
	bPivoting = false; PivotMontageInstanceId = INDEX_NONE;
}

void UHodgeGameplayAbility_Sprint::BeginMovement(const FHodgeMoveIntentSnapshot& Intent)
{
	PendingIntent = Intent;
	if (!Policy->CanHandoff(SessionId))
	{
		bool bWaitingForDash = false;
		for (const FGameplayAbilitySpec& Spec : GetAbilitySystemComponentFromActorInfo()->GetActivatableAbilities())
		{
			const auto* Dash = Cast<UHodgeGameplayAbility_Dash>(Spec.GetPrimaryInstance());
			bWaitingForDash |= Dash && Dash->IsActive() && Dash->HasCommittedMovement() && Dash->GetInputSessionId() == SessionId;
		}
		if (!bWaitingForDash || GetWorld()->GetTimeSeconds() - PreparingAt > ExecutionProfile->HandoffNetworkGrace) { Finish(); }
		return;
	}
	if (!Policy->IsSessionHeld(SessionId) || Intent.InputMagnitude < ExecutionProfile->MoveIntentThreshold) { Finish(); return; }
	if (Policy->SessionHeldTime(SessionId) < ExecutionProfile->HoldThreshold)
	{
		if (GetWorld()->GetTimeSeconds() - PreparingAt > ExecutionProfile->HandoffNetworkGrace) { Finish(); }
		return;
	}
	FacingHandle = Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Movement, this);
	if (!FacingHandle.IsValid()) { Finish(); return; }
	UHodgeGameplayAbility_Dash* Dash = nullptr;
	for (const FGameplayAbilitySpec& Spec : GetAbilitySystemComponentFromActorInfo()->GetActivatableAbilities())
	{
		if (auto* Candidate = Cast<UHodgeGameplayAbility_Dash>(Spec.GetPrimaryInstance()); Candidate && Candidate->IsActive()) { Dash = Candidate; break; }
	}
	if (Dash) { Dash->CompleteHandoff(); }
	if (Rotation->GetResolvedState().Driver != EHodgeCharacterFacingDriver::Movement || Rotation->IsYawLocked()) { Finish(); return; }
	PolicyHandle = Policy->AcquireSprint(this, ExecutionProfile, CurrentActivationInfo.GetActivationPredictionKey().Current);
	if (!PolicyHandle.IsValid()) { Finish(); return; }
	Policy->ConsumeHandoff(); bCommitted = true; CommittedAt = GetWorld()->GetTimeSeconds(); NoInputAt = BlockedAt = 0.;
	bPivoting = bEnding = false; PivotSequence = 0; NextPivotAt = 0.; PivotTask = nullptr;
	PivotMontageInstanceId = INDEX_NONE; DeferredPivotData.Clear(); DeferredPivotExpiresAt = 0.;
	SetMovementTag(HodgeMovementTags::Sprinting, true);
	AuthorityHandle = Policy->OnSprintAuthorityEnded.AddUObject(this, &ThisClass::OnAuthorityEnded);
	AttackHandle = GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(HodgeGameplayTags::Status_Attack).AddUObject(this, &ThisClass::OnAttackStarted);
	if (const auto* Health = GetAbilitySystemComponentFromActorInfo()->GetSet<UHodgeHealthSet>())
	{ DamageHandle = Health->OnDamageAccepted.AddUObject(this, &ThisClass::OnDamage); }
	if (auto* Hero = GetHeroComponentFromActorInfo()) { Hero->NotifySprintCommitted(SessionId); }
	StartCue(HodgeMovementTags::SprintCue);
}
void UHodgeGameplayAbility_Sprint::UpdateMovement(float DeltaTime)
{
	if (!bCommitted) { BeginMovement(PendingIntent); return; }
	UpdatePivotRecovery();
	if (DeferredPivotData.Num() && !bPivoting)
	{
		const auto Data = DeferredPivotData; DeferredPivotData.Clear();
		if (GetWorld()->GetTimeSeconds() <= DeferredPivotExpiresAt) { HandleMovementFollowUp(Data); }
	}
	if (!Policy->IsSessionHeld(SessionId) || Rotation->GetResolvedState().Driver != EHodgeCharacterFacingDriver::Movement)
	{ Finish(); return; }
	const auto* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	const auto* Hero = GetHeroComponentFromActorInfo();
	if (CurrentActorInfo->IsLocallyControlled() && Hero)
	{ RequestPivot(Character->GetVelocity(), Hero->GetWorldMoveIntent()); }
	const bool bHasInput = CurrentActorInfo->IsLocallyControlled() ? Hero && Hero->HasMoveIntent(ExecutionProfile->MoveIntentThreshold) :
		!Character->GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero();
	const double Time = GetWorld()->GetTimeSeconds();
	if (bHasInput) { NoInputAt = 0.; }
	else if (NoInputAt == 0.) { NoInputAt = Time; }
	else if (Time - NoInputAt >= ExecutionProfile->NoMoveIntentGrace) { Finish(); return; }
	if (bPivoting || !bHasInput || Character->GetVelocity().SizeSquared2D() > 100.f || Rotation->IsRecoveringFacing()) { BlockedAt = 0.; }
	else if (BlockedAt == 0.) { BlockedAt = Time; }
	else if (Time - BlockedAt >= ExecutionProfile->BlockedExitDelay) { Finish(); }
}
void UHodgeGameplayAbility_Sprint::OnAuthorityEnded()
{
	if (CVarHodgeSprintDebug.GetValueOnGameThread()) { UE_LOG(LogTemp, Log, TEXT("[HodgeSprint] Authority policy ended SID=%d"), SessionId); }
	Finish();
}
void UHodgeGameplayAbility_Sprint::OnAttackStarted(FGameplayTag Tag, int32 Count) { if (Count > 0) { Finish(); } }
void UHodgeGameplayAbility_Sprint::OnDamage(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* Spec, float Magnitude, float Old, float New)
{ if (Magnitude > 0.f) { Finish(); } }
void UHodgeGameplayAbility_Sprint::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled)
{
	if (bEnding) { return; } bEnding = true;
	if (CVarHodgeSprintDebug.GetValueOnGameThread() && Info)
	{
		const auto* Hero = GetHeroComponentFromActorInfo(); const auto* Character = Cast<ACharacter>(Info->AvatarActor.Get());
		const double Time = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.;
		UE_LOG(LogTemp, Log, TEXT("[HodgeSprint] End T=%.3f SID=%d Key=%d Cancel=%d Auth=%d Local=%d NoInputAge=%.3f BlockedAge=%.3f Held=%d Driver=%d Ground=%d Input=%d"),
			Time, SessionId, ActivationInfo.GetActivationPredictionKey().Current, bCancelled, Info->IsNetAuthority(), Info->IsLocallyControlled(),
			NoInputAt ? Time - NoInputAt : 0., BlockedAt ? Time - BlockedAt : 0.,
			Policy.IsValid() && Policy->IsSessionHeld(SessionId), Rotation.IsValid() ? int32(Rotation->GetResolvedState().Driver) : -1,
			Character && Character->GetCharacterMovement()->IsMovingOnGround(), Hero && ExecutionProfile && Hero->HasMoveIntent(ExecutionProfile->MoveIntentThreshold));
	}
	bPivoting = false;
	DeferredPivotData.Clear(); ReleasePivotTask(); PivotMontageInstanceId = INDEX_NONE;
	if (Info && Info->AbilitySystemComponent.IsValid() && ExecutionProfile &&
		Info->AbilitySystemComponent->GetAnimatingAbility() == this && Info->AbilitySystemComponent->GetCurrentMontage() == ExecutionProfile->SprintPivotMontage)
	{ Info->AbilitySystemComponent->CurrentMontageStop(ExecutionProfile->SprintTurnBlendOutTime); }
	if (Policy.IsValid()) { Policy->OnSprintAuthorityEnded.Remove(AuthorityHandle); Policy->ReleasePolicy(PolicyHandle); Policy->ConsumeHandoff(); }
	if (Info && Info->AbilitySystemComponent.IsValid())
	{
		Info->AbilitySystemComponent->RegisterGameplayTagEvent(HodgeGameplayTags::Status_Attack).Remove(AttackHandle);
		if (const auto* Health = Info->AbilitySystemComponent->GetSet<UHodgeHealthSet>()) { Health->OnDamageAccepted.Remove(DamageHandle); }
	}
	if (auto* Hero = GetHeroComponentFromActorInfo()) { Hero->NotifyMovementActionEnded(SessionId, false); }
	PolicyHandle.Invalidate(); Super::EndAbility(Handle, Info, ActivationInfo, bReplicate, bCancelled);
	PivotTask = nullptr; bEnding = false;
}
