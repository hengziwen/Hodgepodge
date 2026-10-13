/** @file HodgeGameplayAbility_Dash.cpp
 * @brief 实现玩法能力Dash冲刺相关的类型、接口与现有处理流程。
 */
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgeHeroComponent.h"
#include "Component/HodgeLocomotionPolicyComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeLockOnComponent.h"
#include "Component/HodgeDefenseComponent.h"
#include "Data/HodgeSprintAbilityProfile.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "Components/SkeletalMeshComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_Dash)

/** 构造玩法能力Dash冲刺并初始化其默认状态。 */
UHodgeGameplayAbility_Dash::UHodgeGameplayAbility_Dash()
{ FGameplayTagContainer Tags = GetAssetTags(); Tags.AddTag(HodgeGameplayTags::Ability_Type_Action_Dash); SetAssetTags(Tags); }
/** 检查消耗。 */
bool UHodgeGameplayAbility_Dash::CheckCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayTagContainer* Tags) const
{
	// 后续GAS操作使用的能力系统组件。
	const auto* ASC = Info ? Info->AbilitySystemComponent.Get() : nullptr;
	return ASC && !ASC->HasMatchingGameplayTag(HodgeMovementTags::Dashing);
}
/** 开始移动。 */
void UHodgeGameplayAbility_Dash::BeginMovement(const FHodgeMoveIntentSnapshot& Intent)
{
	bHandoffOpened = bHandedOff = bHandoffRequested = bEnding = false; MotionTask = nullptr; MontageTask = nullptr; DefenseHandle.Invalidate();
	const auto* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character || !FHodgeDashDirections::Select(Intent.DesiredDirectionWorld,
		FRotator(0.f, ActorYawAtStart, 0.f).Vector(), ExecutionProfile->bEnableBackwardVariant, ExecutionProfile->BackwardConeAngle, Directions)) { Finish(); return; }
	const UAnimMontage* Montage = Directions.Variant == EHodgeDashVariant::Backward ? ExecutionProfile->BackwardMontage.Get() : ExecutionProfile->ForwardMontage.Get();
	if (!Character->GetMesh()->GetAnimInstance() || !Montage || Montage->GetSkeleton() != Character->GetMesh()->GetSkeletalMeshAsset()->GetSkeleton()) { Finish(); return; }
	if (auto* Lock = GetAvatarActorFromActorInfo()->FindComponentByClass<UHodgeLockOnComponent>()) { Lock->RefreshFacing(); }
	DashBaseFacing = Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Movement,this);
	if (!DashBaseFacing.IsValid()) { Finish(); return; }
	FacingHandle = Rotation->RequestActionFacing(Directions.FacingDirection, this, ExecutionId, ExecutionProfile->bInstantFacingAtStart);
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!FacingHandle.IsValid()) { Finish(); return; }
	// 按服务器权限区分本次处理分支。
	if (CurrentActorInfo->IsNetAuthority() && !CurrentActorInfo->IsLocallyControlled())
	{
		auto* ASC = GetAbilitySystemComponentFromActorInfo();
		CommitRequestedHandle = ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericConfirm,
			CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnCommitRequested);
	}
	UpdateMovement(0.f);
}
/** 处理提交Requested。 */
void UHodgeGameplayAbility_Dash::OnCommitRequested()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!IsActive() || bCommitted || !IsMovementValid() || !FacingHandle.IsValid() ||
		!Rotation->IsActionFacingApplied(FacingHandle)) { return; }
	GetAbilitySystemComponentFromActorInfo()->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GenericConfirm,
		CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
	CommitDash();
}
/** 提交Dash冲刺。 */
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
/** 更新移动。 */
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
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (FacingHandle.IsValid() && Rotation->GetFacingRequestStatus(FacingHandle) == EHodgeFacingRequestStatus::Rejected) { Finish(); return; }
	const double Age = GetWorld()->GetTimeSeconds() - CommittedAt;
	const bool bOwnerWindow = ExecutionProfile->IsHandoffWindow(Age);
	const bool bRemoteAuthorityWindow = CurrentActorInfo->IsNetAuthority() && !CurrentActorInfo->IsLocallyControlled() && ExecutionProfile->IsAuthorityHandoffWindow(Age);
	// 按预测状态或网络角色选择对应处理。
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
	// 根据当前生命周期状态决定是否继续执行。
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
/** 完成Dash到Sprint的会话交接并释放交接记录。 */
void UHodgeGameplayAbility_Dash::CompleteHandoff()
{
	// 根据当前生命周期状态决定是否继续执行。
	if (!IsActive() || !bCommitted || !bHandoffOpened) { return; }
	bHandedOff = true;
	PrepareLocomotionExit(ExecutionProfile->SprintMaxSpeed * Policy->GetResolvedPolicy().SpeedScale);
	// 客户端只预测交接；服务器批准 Sprint 后才结束自己的 Dash。
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, CurrentActorInfo->IsNetAuthority(), false);
}
/** 处理蒙太奇Interrupted。 */
void UHodgeGameplayAbility_Dash::OnMontageInterrupted() { if (IsActive() && !bEnding) { Finish(); } }
/** 准备移动表现退出。 */
void UHodgeGameplayAbility_Dash::PrepareLocomotionExit(float MaximumSpeed)
{
	const auto* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character || !FMath::IsFinite(MaximumSpeed) || MaximumSpeed <= 0.f) { return; }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Policy.IsValid()) { Policy->RecordDashExitVelocity(MotionInstanceName, MaximumSpeed); }
	if (const auto Source = Character->GetCharacterMovement()->GetRootMotionSource(MotionInstanceName))
	{
		// 只处理本次 Dash 的 RMS；衔接保留已有速度，并限制到下一步态的上限。
		Source->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::ClampVelocity;
		Source->FinishVelocityParams.ClampVelocity = MaximumSpeed;
	}
}
/** 处理蒙太奇混合输出。 */
void UHodgeGameplayAbility_Dash::OnMontageBlendOut()
{
	// 根据当前生命周期状态决定是否继续执行。
	if (!IsActive() || bEnding || !bCommitted) { return; }
	// 正常混出先开放移动，能力仍等动画完成；不以位移结束截断无输入的短按。
	SetMovementTag({}, false);
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Rotation.IsValid()) { Rotation->ReleaseFacingRequest(FacingHandle); FacingHandle = {}; }
}
/** 处理蒙太奇已完成。 */
void UHodgeGameplayAbility_Dash::OnMontageCompleted() { if (IsActive() && !bEnding) { Finish(false); } }
/** 结束本次技能并释放其拥有的资源，再交还GAS生命周期。 */
void UHodgeGameplayAbility_Dash::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled)
{
	// 根据当前生命周期状态决定是否继续执行。
	if (bEnding) { return; }
	bEnding = true;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
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
	if (Rotation.IsValid()) { Rotation->ReleaseFacingRequest(DashBaseFacing); } DashBaseFacing = {};
	Super::EndAbility(Handle, Info, ActivationInfo, bReplicate, bCancelled);
	bEnding = false;
}
