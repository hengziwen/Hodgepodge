#include "AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgeHeroComponent.h"
#include "Component/HodgeLocomotionPolicyComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeDefenseComponent.h"
#include "Data/HodgeSprintAbilityProfile.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "Components/SkeletalMeshComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_Dash)

UHodgeGameplayAbility_Dash::UHodgeGameplayAbility_Dash()
{ FGameplayTagContainer Tags = GetAssetTags(); Tags.AddTag(HodgeGameplayTags::Ability_Type_Action_Dash); SetAssetTags(Tags); }
bool UHodgeGameplayAbility_Dash::CheckCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayTagContainer* Tags) const
{
	const auto* ASC = Info ? Info->AbilitySystemComponent.Get() : nullptr;
	return ASC && !ASC->HasMatchingGameplayTag(HodgeMovementTags::Dashing);
}
void UHodgeGameplayAbility_Dash::BeginMovement(const FHodgeMoveIntentSnapshot& Intent)
{
	bHandoffOpened = bHandedOff = bHandoffRequested = bEnding = false; MotionTask = nullptr; MontageTask = nullptr; DefenseHandle.Invalidate();
	const auto* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character || !FHodgeDashDirections::Select(Intent.DesiredDirectionWorld,
		FRotator(0.f, ActorYawAtStart, 0.f).Vector(), ExecutionProfile->bEnableBackwardVariant, ExecutionProfile->BackwardConeAngle, Directions)) { Finish(); return; }
	const UAnimMontage* Montage = Directions.Variant == EHodgeDashVariant::Backward ? ExecutionProfile->BackwardMontage.Get() : ExecutionProfile->ForwardMontage.Get();
	if (!Character->GetMesh()->GetAnimInstance() || !Montage || Montage->GetSkeleton() != Character->GetMesh()->GetSkeletalMeshAsset()->GetSkeleton()) { Finish(); return; }
	FacingHandle = Rotation->RequestActionFacing(Directions.FacingDirection, this, ExecutionId, ExecutionProfile->bInstantFacingAtStart);
	if (!FacingHandle.IsValid()) { Finish(); return; }
	if (CurrentActorInfo->IsNetAuthority() && !CurrentActorInfo->IsLocallyControlled())
	{
		auto* ASC = GetAbilitySystemComponentFromActorInfo();
		CommitRequestedHandle = ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericConfirm,
			CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnCommitRequested);
	}
	UpdateMovement(0.f);
}
void UHodgeGameplayAbility_Dash::OnCommitRequested()
{
	if (!IsActive() || bCommitted || !IsMovementValid() || !FacingHandle.IsValid() ||
		!Rotation->IsActionFacingApplied(FacingHandle)) { return; }
	GetAbilitySystemComponentFromActorInfo()->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GenericConfirm,
		CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
	CommitDash();
}
void UHodgeGameplayAbility_Dash::CommitDash()
{
	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo)) { Finish(); return; }
	bCommitted = true; CommittedAt = GetWorld()->GetTimeSeconds(); SetMovementTag(HodgeMovementTags::Dashing, true);
	if (auto* Hero = GetHeroComponentFromActorInfo()) { Hero->NotifyDashCommitted(SessionId); }
	if (auto* Defense = GetAvatarActorFromActorInfo()->FindComponentByClass<UHodgeDefenseComponent>())
	{ DefenseHandle = Defense->RegisterDodgeWindow(this, ExecutionId, ExecutionProfile, CommittedAt); }
	UAnimMontage* Montage = Directions.Variant == EHodgeDashVariant::Backward ? ExecutionProfile->BackwardMontage.Get() : ExecutionProfile->ForwardMontage.Get();
	const float Rate = ExecutionProfile->MontagePlayRate;
	AnimationEndsAt = CommittedAt + Montage->GetPlayLength() / (Rate * Montage->RateScale);
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, Rate);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageBlendOut);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted); MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
	MontageTask->ReadyForActivation(); if (!IsActive()) { return; }
	MotionInstanceName = FName(*FString::Printf(TEXT("HodgeDash_%s_%d"), *CurrentSpecHandle.ToString(), SessionId));
	MotionTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this, MotionInstanceName, Directions.MoveDirection,
		ExecutionProfile->Distance / ExecutionProfile->Duration, ExecutionProfile->Duration, false, nullptr,
		ERootMotionFinishVelocityMode::SetVelocity, FVector::ZeroVector, 0.f, false);
	MotionTask->ReadyForActivation(); StartCue(HodgeGameplayTags::GameplayCue_Character_Dash);
}
void UHodgeGameplayAbility_Dash::UpdateMovement(float DeltaTime)
{
	if (!bCommitted)
	{
		if (GetWorld()->GetTimeSeconds() - PreparingAt > ExecutionProfile->FacingPreparationTimeout + .5) { Finish(); return; }
		const auto Status = Rotation->GetFacingRequestStatus(FacingHandle);
		if (Rotation->IsActionFacingApplied(FacingHandle))
		{
			auto* ASC = GetAbilitySystemComponentFromActorInfo();
			if (CurrentActorInfo->IsNetAuthority() && !CurrentActorInfo->IsLocallyControlled())
			{ ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GenericConfirm, CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()); }
			else
			{
				if (!CurrentActorInfo->IsNetAuthority())
				{ ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericConfirm, CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), ASC->ScopedPredictionKey); }
				CommitDash();
			}
			return;
		}
		if (Status != EHodgeFacingRequestStatus::Pending || GetWorld()->GetTimeSeconds() - PreparingAt > ExecutionProfile->FacingPreparationTimeout) { Finish(); }
		return;
	}
	if (FacingHandle.IsValid() && Rotation->GetFacingRequestStatus(FacingHandle) == EHodgeFacingRequestStatus::Rejected) { Finish(); return; }
	const double Age = GetWorld()->GetTimeSeconds() - CommittedAt;
	const bool bOwnerWindow = ExecutionProfile->IsHandoffWindow(Age);
	const bool bRemoteAuthorityWindow = CurrentActorInfo->IsNetAuthority() && !CurrentActorInfo->IsLocallyControlled() && ExecutionProfile->IsAuthorityHandoffWindow(Age);
	if (bOwnerWindow || bRemoteAuthorityWindow)
	{
		bHandoffOpened = true;
		Policy->AuthorizeHandoff(CurrentActivationInfo.GetActivationPredictionKey().Current, SessionId,
			FMath::Max(.01, ExecutionProfile->HandoffCloseTime - Age + ExecutionProfile->HandoffNetworkGrace));
		if (bOwnerWindow && !bHandoffRequested && CurrentActorInfo->IsLocallyControlled() && Policy->IsSessionHeld(SessionId) &&
			Policy->SessionHeldTime(SessionId) >= ExecutionProfile->HoldThreshold)
		{
			if (auto* Hero = GetHeroComponentFromActorInfo(); Hero && Hero->HasMoveIntent(ExecutionProfile->MoveIntentThreshold))
			{ bHandoffRequested = true; Hero->RequestSprintHandoff(SessionId, ExecutionProfile->HoldThreshold); }
		}
	}
	if (!IsActive()) { return; }
	// 长按优先等待衔接窗口；短按后移动只在后摇窗口取消。
	if (CurrentActorInfo->IsLocallyControlled() && ExecutionProfile->CanMoveCancel(Age) &&
		(!Policy->IsSessionHeld(SessionId) || Age > ExecutionProfile->HandoffCloseTime))
	{
		if (auto* Hero = GetHeroComponentFromActorInfo(); Hero && Hero->HasMoveIntent(ExecutionProfile->MoveIntentThreshold))
		{
			if (const auto* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo())) { PrepareLocomotionExit(Character->GetCharacterMovement()->GetMaxSpeed()); }
			Finish(false); return;
		}
	}
	// 完成由蒙太奇回调驱动；丢失实例时保留有限的收尾保护。
	if (GetWorld()->GetTimeSeconds() > AnimationEndsAt + ExecutionProfile->HandoffNetworkGrace) { Finish(); }

}
void UHodgeGameplayAbility_Dash::CompleteHandoff()
{
	if (!IsActive() || !bCommitted || !bHandoffOpened) { return; }
	bHandedOff = true;
	PrepareLocomotionExit(ExecutionProfile->SprintMaxSpeed * Policy->GetResolvedPolicy().SpeedScale);
	// 客户端只预测交接；服务器批准 Sprint 后才结束自己的 Dash。
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, CurrentActorInfo->IsNetAuthority(), false);
}
void UHodgeGameplayAbility_Dash::OnMontageInterrupted() { if (IsActive() && !bEnding) { Finish(); } }
void UHodgeGameplayAbility_Dash::PrepareLocomotionExit(float MaximumSpeed)
{
	const auto* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character || !FMath::IsFinite(MaximumSpeed) || MaximumSpeed <= 0.f) { return; }
	if (Policy.IsValid()) { Policy->RecordDashExitVelocity(MotionInstanceName, MaximumSpeed); }
	if (const auto Source = Character->GetCharacterMovement()->GetRootMotionSource(MotionInstanceName))
	{
		// 只处理本次 Dash 的 RMS；衔接保留已有速度，并限制到下一步态的上限。
		Source->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::ClampVelocity;
		Source->FinishVelocityParams.ClampVelocity = MaximumSpeed;
	}
}
void UHodgeGameplayAbility_Dash::OnMontageBlendOut()
{
	if (!IsActive() || bEnding || !bCommitted) { return; }
	// 正常混出先开放移动，能力仍等动画完成；不以位移结束截断无输入的短按。
	SetMovementTag({}, false);
	if (Rotation.IsValid()) { Rotation->ReleaseFacingRequest(FacingHandle); FacingHandle = {}; }
}
void UHodgeGameplayAbility_Dash::OnMontageCompleted() { if (IsActive() && !bEnding) { Finish(false); } }
void UHodgeGameplayAbility_Dash::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled)
{
	if (bEnding) { return; }
	bEnding = true;
	if (!bCancelled && !bHandedOff)
	{
		if (const auto* Character = Info ? Cast<ACharacter>(Info->AvatarActor.Get()) : nullptr)
		{ PrepareLocomotionExit(Character->GetCharacterMovement()->GetMaxSpeed()); }
	}
	if (MotionTask) { MotionTask->EndTask(); MotionTask = nullptr; }
	if (Info && Info->AvatarActor.IsValid())
	{
		if (auto* Defense = Info->AvatarActor->FindComponentByClass<UHodgeDefenseComponent>()) { Defense->UnregisterDodgeWindow(DefenseHandle); }
		if (auto* Hero = GetHeroComponentFromActorInfo()) { Hero->NotifyMovementActionEnded(SessionId, bHandedOff); }
	}
	if (Info && Info->AbilitySystemComponent.IsValid())
	{
		Info->AbilitySystemComponent->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericConfirm,
			Handle, ActivationInfo.GetActivationPredictionKey()).Remove(CommitRequestedHandle);
		Info->AbilitySystemComponent->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GenericConfirm, Handle, ActivationInfo.GetActivationPredictionKey());
	}
	if (!bHandedOff && Policy.IsValid()) { Policy->ConsumeHandoff(); }
	Super::EndAbility(Handle, Info, ActivationInfo, bReplicate, bCancelled);
	bEnding = false;
}
