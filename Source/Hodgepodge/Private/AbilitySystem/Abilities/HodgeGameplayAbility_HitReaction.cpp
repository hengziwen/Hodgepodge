/** @file HodgeGameplayAbility_HitReaction.cpp
 * @brief 实现玩法能力命中受击相关的类型、接口与现有处理流程。
 */
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
#include "Data/HodgeHitReactionProfile.h"
#include "Combat/HodgeHitReactionMotion.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_HitReaction)

/** 构造玩法能力命中受击并初始化其默认状态。 */
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

/** 检查技能激活所需的上下文、标签与现有GAS限制。 */
bool UHodgeGameplayAbility_HitReaction::CanActivateAbility(FGameplayAbilitySpecHandle Handle,
                                                           const FGameplayAbilityActorInfo* Info,
                                                           const FGameplayTagContainer* SourceTags,
                                                           const FGameplayTagContainer* TargetTags,
                                                           FGameplayTagContainer* RelevantTags) const
{
	const auto* Component = Info && Info->AvatarActor.IsValid()
		                        ? Info->AvatarActor->FindComponentByClass<UHodgeHitReactionComponent>()
		                        : nullptr;
	return Component && (!Info->IsNetAuthority() || Component->IsControlled()) &&
		Super::CanActivateAbility(Handle, Info, SourceTags, TargetTags, RelevantTags);
}

/** 启动本次技能，按现有配置提交消耗并建立执行资源。 */
void UHodgeGameplayAbility_HitReaction::ActivateAbility(FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayAbilityActorInfo* Info,
                                                        FGameplayAbilityActivationInfo ActivationInfo,
                                                        const FGameplayEventData* Payload)
{
	bEnding = false;
	AppliedSequence = 0;
	AppliedPhase = EHodgeHitReactionPhase::None;
	AppliedStartedAt = -1.f;
	Reaction = Info->AvatarActor->FindComponentByClass<UHodgeHitReactionComponent>();
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Reaction.IsValid())
	{
		FinishReaction(true);
		return;
	}
	ReactionAvatar = Cast<ACharacter>(Info->AvatarActor.Get());
	// 按服务器权限区分本次处理分支。
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
	if (Reaction->IsControlled() && !Synchronize(Reaction->GetReactionState()) && Info->IsNetAuthority())
	{
		FinishReaction(true);
		return;
	}
	Super::ActivateAbility(Handle, Info, ActivationInfo, Payload);
}

/** 把当前反应状态同步到技能控制与表现。 */
bool UHodgeGameplayAbility_HitReaction::Synchronize(const FHodgeHitReactionState& State)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!IsActive() || bEnding || !Reaction.IsValid() || !CurrentActorInfo ||
		CurrentActorInfo->AvatarActor.Get() != Reaction->GetOwner()) { return false; }
	// 根据当前生命周期状态决定是否继续执行。
	if (!State.IsActive()) { return true; }
	auto* ASC = GetAbilitySystemComponentFromActorInfo();
	if (AppliedSequence == State.Sequence && AppliedPhase == State.Phase && PlayedMontage.Get() == State.Montage &&
		AppliedStartedAt == State.StartedAt)
	{
		const auto* Animating = ASC->GetAnimatingAbility();
		const auto* Instance = CurrentActorInfo->GetAnimInstance() && State.Montage ? CurrentActorInfo->GetAnimInstance()->GetInstanceForMontage(State.Montage) : nullptr;
		// 预测反击被拒绝后重建尚未结束的受击；新动作仍活跃时不能抢回主动画。
		if (!State.Montage || (Animating && Animating != this && Animating->IsActive()) ||
			(Animating == this && Instance && (Instance->IsActive() || Instance->GetWeight() > UE_KINDA_SMALL_NUMBER))) { return true; }
	}
	if (State.Montage && !CurrentActorInfo->GetAnimInstance()) { return false; }
	auto* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) { return false; }
	auto* Movement = Character->GetCharacterMovement();
	// 控制身份更新不等于动画重播；服务端保留的起播时间使迟到客户端得到同一姿态。
	const auto* CurrentInstance = State.Montage ? CurrentActorInfo->GetAnimInstance()->GetInstanceForMontage(State.Montage) : nullptr;
	const bool bContinuePresentation = AppliedPhase == State.Phase && PlayedMontage.Get() == State.Montage &&
		AppliedStartedAt == State.StartedAt && ASC->GetAnimatingAbility() == this && CurrentInstance &&
		(CurrentInstance->IsActive() || CurrentInstance->GetWeight() > UE_KINDA_SMALL_NUMBER);
	if (State.Sequence != AppliedSequence && State.Phase != EHodgeHitReactionPhase::Airborne)
	{
		Movement->StopMovementImmediately();
	}
	const bool bNewMotion = State.Sequence != AppliedSequence;
	// 没有新运动的空中接触继续原 Launch，不能清除尚未由 CMC 消费的排队速度。
	if (State.Phase != EHodgeHitReactionPhase::Airborne || (bNewMotion && State.bHasMovement)) { ClearMotion(); }
	const float Elapsed = FMath::Max(0.f, Reaction->GetServerTime() - State.StartedAt);
	// 按服务器权限区分本次处理分支。
	if (bNewMotion && State.bHasMovement && State.MotionSource == EHodgeReactionMotionSource::Impact && (CurrentActorInfo->IsNetAuthority() || CurrentActorInfo->
		IsLocallyControlled()))
	{
		if (State.Type == EHodgeImpactType::Knockback && Elapsed < State.MoveDuration)
		{
			auto Force = MakeShared<FHodgeRootMotionSource_Knockback>();
			Force->InstanceName = FName(*FString::Printf(TEXT("HodgeReaction_%d"), State.Sequence));
			Force->Priority = 500;
			Force->AccumulateMode = ERootMotionAccumulateMode::Override;
			Force->Force = State.MotionVelocity;
			Force->StrengthOverTime = Reaction->GetProfile() ? Reaction->GetProfile()->KnockbackStrengthCurve : nullptr;
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
	if (!bContinuePresentation && ASC->GetAnimatingAbility() == this && PlayedMontage.IsValid())
	{
		if (PlayedMontage->HasRootMotion() && CurrentActorInfo->GetAnimInstance())
		{
			if (auto* Old = CurrentActorInfo->GetAnimInstance()->GetInstanceForMontage(PlayedMontage.Get())) { Old->PushDisableRootMotion(); }
		}
		ASC->CurrentMontageStop(Reaction->GetProfile() ? Reaction->GetProfile()->ReplacementBlendOut : 0.05f);
		ASC->ClearAnimatingAbility(this);
	}
	if (!bContinuePresentation) { PlayedMontage.Reset(); }
	if (!bContinuePresentation && State.Montage && (CurrentActorInfo->IsNetAuthority() || CurrentActorInfo->
		IsLocallyControlled()))
	{
		const float PresentationTime = Reaction->GetProfile() ? Reaction->GetProfile()->GetPresentationPosition(State, Elapsed) : Elapsed;
		const float Position = FMath::Min(PresentationTime, FMath::Max(0.f, State.Montage->GetPlayLength() - 0.01f));
		if (ASC->PlayMontage(this, CurrentActivationInfo, State.Montage, 1.f, NAME_None, Position) <= 0.f)
		{
			return false;
		}
		PlayedMontage = State.Montage;
		FHodgeHitReactionAnimation Entry;
		if (Reaction->GetProfile() && Reaction->GetProfile()->ResolveAnimation(State.Type, Entry) &&
			!State.bLanding && State.Phase == EHodgeHitReactionPhase::Controlled && !Entry.SustainSection.IsNone())
		{ ASC->CurrentMontageSetNextSectionName(Entry.SustainSection, Entry.SustainSection); }
		if (UAnimInstance* Anim = CurrentActorInfo->GetAnimInstance())
		{
			if (FAnimMontageInstance* Instance = Anim->GetActiveInstanceForMontage(State.Montage))
			{
				Instance->bEnableAutoBlendOut = Reaction->GetProfile() ? !Reaction->GetProfile()->ShouldSustainPose(State) : State.Phase != EHodgeHitReactionPhase::Downed;
			}
		}
	}
	AppliedSequence = State.Sequence;
	AppliedPhase = State.Phase;
	AppliedStartedAt = State.StartedAt;
	return true;
}

/** 清空Motion。 */
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
		{
			Character->GetCharacterMovement()->RemoveRootMotionSourceByID(MotionSourceId);
		}
		MotionSourceId = 0;
	}
}

/** 结束受击。 */
void UHodgeGameplayAbility_HitReaction::FinishReaction(bool bCancelled)
{
	// 按服务器权限区分本次处理分支。
	if (IsActive() && !bEnding)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
		           CurrentActorInfo && CurrentActorInfo->IsNetAuthority(), bCancelled);
	}
}

/** 结束本次技能并释放其拥有的资源，再交还GAS生命周期。 */
void UHodgeGameplayAbility_HitReaction::EndAbility(FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* Info,
                                                   FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate,
                                                   bool bCancelled)
{
	// 根据当前生命周期状态决定是否继续执行。
	if (bEnding || !IsActive()) { return; }
	TGuardValue<bool> Guard(bEnding, true);
	ClearMotion();
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (PoseCombat.IsValid()) { PoseCombat->ReleasePoseLease(PoseLease); }
	if (PoseMesh.IsValid() && PoseMesh->VisibilityBasedAnimTickOption ==
		EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones)
	{
		PoseMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption(SavedPosePolicy);
	}
	PoseLease.Invalidate();
	PoseCombat.Reset();
	PoseMesh.Reset();
	if (auto* ASC = GetAbilitySystemComponentFromActorInfo(); ASC && ASC->GetAnimatingAbility() == this &&
		Info && Info->AvatarActor.Get() == ReactionAvatar.Get())
	{
		auto* Instance = Info->GetAnimInstance() && PlayedMontage.IsValid() ?
			Info->GetAnimInstance()->GetInstanceForMontage(PlayedMontage.Get()) : nullptr;
		const bool bRetired = !bCancelled && Instance && Reaction.IsValid() &&
			Reaction->RetirePresentation(PlayedMontage.Get(), Instance->GetInstanceID());
		if (bRetired && AppliedPhase == EHodgeHitReactionPhase::Controlled)
		{
			FHodgeHitReactionAnimation Entry;
			if (Reaction->GetProfile()->ResolveAnimation(Reaction->GetReactionState().Type, Entry) && !Entry.RecoverySection.IsNone())
			{ ASC->CurrentMontageJumpToSection(Entry.RecoverySection); }
		}
		if (!bRetired)
		{
			if (Instance && PlayedMontage->HasRootMotion())
			{ Instance->PushDisableRootMotion(); }
			ASC->CurrentMontageStop();
		}
		ASC->ClearAnimatingAbility(this);
	}
	PlayedMontage.Reset();
	if (Reaction.IsValid()) { Reaction->DetachAbility(this); }
	Reaction.Reset();
	ReactionAvatar.Reset();
	Super::EndAbility(Handle, Info, ActivationInfo, bReplicate, bCancelled);
}
