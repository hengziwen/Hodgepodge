/** @file HodgeHitReactionComponent.cpp
 * @brief 实现受击状态组件相关的类型、接口与现有处理流程。
 */
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
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHitReactionComponent)

// 独立分类使逐接触网络诊断可按需启用，不污染每帧日志。
DEFINE_LOG_CATEGORY_STATIC(LogHodgeHitPresentation, Log, All);

/** 构造受击状态组件并初始化其默认状态。 */
UHodgeHitReactionComponent::UHodgeHitReactionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

/** 判断已绑定。 */
bool UHodgeHitReactionComponent::IsBound() const
{
	return ASC.IsValid() && ASC->GetAvatarActor() == GetOwner() && IsRegistered();
}

/** 初始化关联能力System。 */
void UHodgeHitReactionComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)
{
	if (!InASC || InASC->GetAvatarActor() != GetOwner()) { return; }
	if (ASC.Get() != InASC) { UninitializeFromAbilitySystem(); ASC = InASC; }
	RefreshConfiguration();
	OnRep_State();
	SetComponentTickEnabled(true);
}

/** 解除初始化来源能力System。 */
void UHodgeHitReactionComponent::UninitializeFromAbilitySystem()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (ActiveAbility.IsValid()) { ActiveAbility->FinishReaction(true); }
	// 解绑只混出本组件播放的局部动作，避免旧 Avatar 留下反馈或停止无关 Montage。
	if (auto* Character = Cast<ACharacter>(GetOwner()); Character && LastLightMontage.IsValid())
	{
		if (UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance(); Anim && Anim->Montage_IsPlaying(LastLightMontage.Get()))
		{ Anim->Montage_Stop(LastLightMontage->BlendOut.GetBlendTime(), LastLightMontage.Get()); }
	}
	SetControl(false);
	ActiveAbility.Reset();
	ASC.Reset();
	PendingHits.Reset();
	PresentedHits.Reset();
	LastLightMontage.Reset();
	LastLightStrength = 0.f;
	PresentationCount = 0;
	LastPresentation = FHodgeHitPresentation();
	// 按服务器权限区分本次处理分支。
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		State.Phase = EHodgeHitReactionPhase::None;
		Profile = nullptr;
		GetOwner()->ForceNetUpdate();
	}
	SetComponentTickEnabled(false);
}

/** 刷新Configuration。 */
void UHodgeHitReactionComponent::RefreshConfiguration()
{
	// 按服务器权限区分本次处理分支。
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

/** 开始命中。 */
FGuid UHodgeHitReactionComponent::BeginHit(const FHodgeHitReactionConfig& Request, AActor* Source, FVector SourceOrigin,
	const FHitResult* Contact, FGuid ExecutionId, FName AttackPhase)
{
	// 按服务器权限区分本次处理分支。
	if (!GetOwner()->HasAuthority() || !IsBound() || !Request.IsConfigured() || SourceOrigin.ContainsNaN()) { return {}; }
	FPendingHit Hit;
	Hit.Request = Request;
	Hit.Source = Source;
	Hit.Avatar = ASC->GetAvatarActor();
	Hit.Origin = SourceOrigin;
	Hit.BodyRank = HodgeHitReaction::ResolveBody(ASC.Get(), Hit.BodyTag);
	const FGuid Id = FGuid::NewGuid();
	Hit.Presentation.HitId = Id;
	Hit.Presentation.ExecutionId = ExecutionId;
	Hit.Presentation.AttackPhase = AttackPhase;
	Hit.Presentation.Strength = Request.PresentationStrength;
	Hit.Presentation.Direction = (GetOwner()->GetActorLocation() - SourceOrigin).GetSafeNormal();
	Hit.Presentation.LocalSourceDirection = GetOwner()->GetActorTransform().InverseTransformVectorNoScale(-Hit.Presentation.Direction);
	// 只有真实非穿透 Sweep 接触使用表面坐标；其他情况明确回退目标附近。
	Hit.Presentation.bPreciseContact = Contact && Contact->bBlockingHit && !Contact->bStartPenetrating &&
		!Contact->ImpactPoint.ContainsNaN() && !Contact->ImpactNormal.ContainsNaN() && !Contact->ImpactNormal.IsNearlyZero();
	Hit.Presentation.Location = Hit.Presentation.bPreciseContact ? FVector(Contact->ImpactPoint) : GetOwner()->GetActorLocation() + FVector(0.f, 0.f, 35.f);
	Hit.Presentation.Normal = Hit.Presentation.bPreciseContact ? Contact->ImpactNormal.GetSafeNormal() : -Hit.Presentation.Direction;
	Hit.Presentation.bHasSurface = Hit.Presentation.bPreciseContact && Contact->PhysMaterial.IsValid();
	Hit.Presentation.SurfaceType = Hit.Presentation.bHasSurface ? UPhysicalMaterial::DetermineSurfaceType(Contact->PhysMaterial.Get()) : SurfaceType_Default;
	PendingHits.Add(Id, MoveTemp(Hit));
	return Id;
}

/** 记录本次采样观察到的伤害结果。 */
void UHodgeHitReactionComponent::RecordDamage(const FGuid& HitId, bool bAccepted, float ActualDamage)
{
	// 按服务器权限区分本次处理分支。
	if (!GetOwner()->HasAuthority() || !IsBound()) { return; }
	if (FPendingHit* Hit = PendingHits.Find(HitId); Hit && Hit->Avatar.Get() == ASC->GetAvatarActor())
	{
		Hit->bAccepted |= bAccepted;
		Hit->bRejected |= !bAccepted;
		if (bAccepted && FMath::IsFinite(ActualDamage)) { Hit->Damage += FMath::Max(ActualDamage, 0.f); }
	}
}

/** 结束命中。 */
FHodgeHitReactionResult UHodgeHitReactionComponent::FinishHit(const FGuid& HitId)
{
	FHodgeHitReactionResult Result;
	Result.HitId = HitId;
	FPendingHit Hit;
	// 按服务器权限区分本次处理分支。
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
	// 倒地或起身中的普通硬直只作接触反馈，不突然跳回站立动作。
	else if ((State.Phase == EHodgeHitReactionPhase::Downed || State.Phase == EHodgeHitReactionPhase::GettingUp) &&
		!Hit.Request.Impacts.ContainsByPredicate([](const auto& Impact) { return Impact.Type != EHodgeImpactType::HitStun; }))
	{ Result.Outcome = EHodgeHitReactionOutcome::NoImpact; }
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
	}
	// 每个被接受且配置合法的接触只走一次公共音画；控制裁决不负责播放主动画。
	if (Result.Outcome == EHodgeHitReactionOutcome::Applied || Result.Outcome == EHodgeHitReactionOutcome::LowJudgement ||
		Result.Outcome == EHodgeHitReactionOutcome::Unsupported || Result.Outcome == EHodgeHitReactionOutcome::NoImpact ||
		Result.Outcome == EHodgeHitReactionOutcome::CannotInterrupt)
	{
		Hit.Presentation.bLightFeedback = Result.Outcome != EHodgeHitReactionOutcome::Applied ||
			(State.Phase == EHodgeHitReactionPhase::Controlled && State.Type == EHodgeImpactType::HitStun &&
				GetServerTime() - State.StartedAt > 0.01f);
		MulticastContactFeedback(Hit.Presentation);
	}
	LastResult = Result;
	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	OnReactionResolved.Broadcast(Result);
	return Result;
}

/** 构建Plan。 */
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
		const bool bGroundControl = Type == EHodgeImpactType::HitStun || Type == EHodgeImpactType::Knockback;
		if (bGroundControl)
		{ Duration = FMath::Max(Duration, Requested.ControlDuration > 0.f ? Requested.ControlDuration : Entry->DefaultDuration); }
		if (!Presentation || Type != EHodgeImpactType::HitStun) { Presentation = Entry; Plan.Type = Type; }
		if (!bFallback && Requested.IsMovement())
		{
			Plan.bHasMovement = true;
			Plan.MoveDuration = Type == EHodgeImpactType::Knockback ? FMath::Clamp(Requested.MoveDuration, 0.01f, Profile->MaxControlDuration) : 0.f;
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
	// 先检查对象与上下文，避免在依赖失效后继续处理。
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
	// 延续尚未结束的同类地面硬直动作，但保持新命中控制结束时间与身份。
	if (Plan.Phase == EHodgeHitReactionPhase::Controlled && State.Phase == Plan.Phase && Plan.Type == EHodgeImpactType::HitStun &&
		State.Type == Plan.Type && !Plan.bHasMovement && !State.bHasMovement && Plan.Montage && Plan.Montage == State.Montage &&
		Now - State.StartedAt < Plan.Montage->GetPlayLength() - 0.03f)
	{ Plan.StartedAt = State.StartedAt; }
	return Plan.EndsAt > Now;
}

/** 提交Plan。 */
bool UHodgeHitReactionComponent::SubmitPlan(FHodgeHitReactionState Plan, EHodgeHitReactionOutcome& Failure)
{
	Failure = EHodgeHitReactionOutcome::NotReady;
	// 按服务器权限区分本次处理分支。
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
	// 先检查对象与上下文，避免在依赖失效后继续处理。
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

/** 设置控制。 */
void UHodgeHitReactionComponent::SetControl(bool bControlled)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!ASC.IsValid()) { bOwnsControl = false; return; }
	if (bControlled == bOwnsControl) { return; }
	if (bControlled) { ASC->AddLooseGameplayTag(HodgeHitReactionTags::Controlled); }
	else { ASC->RemoveLooseGameplayTag(HodgeHitReactionTags::Controlled); }
	bOwnsControl = bControlled;
}

/** 发布本轮已经提交的状态快照。 */
void UHodgeHitReactionComponent::PublishState()
{
	GetOwner()->ForceNetUpdate();
	OnRep_State();
}

/** 收到状态复制后更新本地状态。 */
void UHodgeHitReactionComponent::OnRep_State()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
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
	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	OnReactionStateChanged.Broadcast(State);
}

/** 收到配置复制后更新本地状态。 */
void UHodgeHitReactionComponent::OnRep_Profile() { OnRep_State(); }

/** 接入当前来源授予的技能记录。 */
void UHodgeHitReactionComponent::AttachAbility(UHodgeGameplayAbility_HitReaction* Ability)
{
	ActiveAbility = Ability;
	SetComponentTickEnabled(true);
}

/** 解除当前来源对应的能力关联。 */
void UHodgeHitReactionComponent::DetachAbility(UHodgeGameplayAbility_HitReaction* Ability)
{
	if (ActiveAbility.Get() != Ability) { return; }
	ActiveAbility.Reset();
	// 按服务器权限区分本次处理分支。
	if (GetOwner()->HasAuthority()) { CompletePlan(State.Sequence); }
}

/** 完成Plan。 */
void UHodgeHitReactionComponent::CompletePlan(int32 Sequence)
{
	// 按服务器权限区分本次处理分支。
	if (!GetOwner()->HasAuthority() || State.Sequence != Sequence || !State.IsActive()) { return; }
	TGuardValue<bool> Guard(bCompleting, true);
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (ActiveAbility.IsValid() && ActiveAbility->IsActive()) { ActiveAbility->FinishReaction(); return; }
	State.Phase = EHodgeHitReactionPhase::None;
	State.Montage = nullptr;
	SetControl(false);
	PublishState();
}

/** 检查是否允许cel受击。 */
void UHodgeHitReactionComponent::CancelReaction()
{
	// 按服务器权限区分本次处理分支。
	if (GetOwner() && GetOwner()->HasAuthority()) { CompletePlan(State.Sequence); }
}

/** 切换当前状态流程的执行阶段。 */
void UHodgeHitReactionComponent::ChangePhase(EHodgeHitReactionPhase Phase, UAnimMontage* Montage, float Duration)
{
	State.Phase = Phase;
	State.StartedAt = GetServerTime();
	State.EndsAt = FMath::Min(State.StartedAt + Duration, State.ChainStartedAt + Profile->MaxControlDuration);
	State.Montage = Montage;
	PublishState();
}

/** 请求获取Up。 */
bool UHodgeHitReactionComponent::RequestGetUp()
{
	// 按服务器权限区分本次处理分支。
	if (!GetOwner()->HasAuthority() || State.Phase != EHodgeHitReactionPhase::Downed || !Profile) { return false; }
	if (State.GetUpMontage) { ChangePhase(EHodgeHitReactionPhase::GettingUp, State.GetUpMontage, State.GetUpMontage->GetPlayLength()); }
	else { CompletePlan(State.Sequence); }
	return true;
}

/** 更新Plan。 */
void UHodgeHitReactionComponent::TickPlan()
{
	// 根据当前生命周期状态决定是否继续执行。
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

/** 获取服务器时间。 */
float UHodgeHitReactionComponent::GetServerTime() const
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (const auto* GameState = GetWorld() ? GetWorld()->GetGameState() : nullptr) { return GameState->GetServerWorldTimeSeconds(); }
	return GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
}

/** 更新组件。 */
void UHodgeHitReactionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!IsBound()) { if (ASC.IsValid()) { UninitializeFromAbilitySystem(); } return; }
	if (!Profile) { RefreshConfiguration(); }
	// 按服务器权限区分本次处理分支。
	if (GetOwner()->HasAuthority()) { TickPlan(); }
	else if (ActiveAbility.IsValid() && State.IsActive()) { ActiveAbility->Synchronize(State); }
	// 根据当前生命周期状态决定是否继续执行。
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

/** 把轻受击表现通知到网络观察者。 */
void UHodgeHitReactionComponent::MulticastLightFeedback_Implementation(FVector Direction)
{
	PlayLightFeedback(Direction, GetOwner()->GetActorTransform().InverseTransformVectorNoScale(-Direction), 1.f);
}

/** 方向只影响目标动画选择，不写回角色朝向或控制状态。 */
void UHodgeHitReactionComponent::PlayLightFeedback(FVector Direction, FVector LocalSourceDirection, float Strength)
{
	if (Profile && State.Phase != EHodgeHitReactionPhase::Downed && State.Phase != EHodgeHitReactionPhase::GettingUp)
	{
		if (auto* Character = Cast<ACharacter>(GetOwner()))
		{
			UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance();
			UAnimMontage* Main = ASC.IsValid() ? ASC->GetCurrentMontage() : nullptr;
			UAnimMontage* Montage = Profile->FindLightFeedback(LocalSourceDirection);
			// 混出中的短动作仍占有姿态，同级命中不能在恢复阶段从零重启。
			const auto* LastLightInstance = Anim && LastLightMontage.IsValid() ? Anim->GetInstanceForMontage(LastLightMontage.Get()) : nullptr;
			const bool bPlayingLight = LastLightInstance && (LastLightInstance->IsActive() || LastLightInstance->GetWeight() > UE_KINDA_SMALL_NUMBER);
			if (Anim && Montage && !Montage->HasRootMotion() && HodgeHitReaction::CanUseMontage(Montage, Character->GetMesh()->GetSkeletalMeshAsset()) &&
				(!Main || Main->GetGroupName() != Montage->GetGroupName()) && (!bPlayingLight || Strength > LastLightStrength + UE_KINDA_SMALL_NUMBER))
			{
				Anim->Montage_Play(Montage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, false);
				LastLightMontage = Montage;
				LastLightStrength = Strength;
			}
		}
	}
	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	OnLightFeedback.Broadcast(Direction);
}

/** 身份只用于去重；漏掉不可靠表现事件不能改变玩法结算。 */
void UHodgeHitReactionComponent::MulticastContactFeedback_Implementation(const FHodgeHitPresentation& Presentation)
{
	if (!IsBound() || !Profile || !Presentation.HitId.IsValid() || PresentedHits.Contains(Presentation.HitId)) { return; }
	PresentedHits.Add(Presentation.HitId);
	++PresentationCount;
	LastPresentation = Presentation;
	UE_LOG(LogHodgeHitPresentation, Verbose, TEXT("Contact Actor=%s Role=%d Hit=%s Execution=%s Phase=%s Strength=%.2f Precise=%d Surface=%d Light=%d Position=%s"),
		*GetNameSafe(GetOwner()), int32(GetOwner()->GetLocalRole()), *Presentation.HitId.ToString(), *Presentation.ExecutionId.ToString(),
		*Presentation.AttackPhase.ToString(), Presentation.Strength, Presentation.bPreciseContact, Presentation.bHasSurface,
		Presentation.bLightFeedback, *Presentation.Location.ToString());
	if (PresentedHits.Num() > 64) { PresentedHits.RemoveAt(0, PresentedHits.Num() - 64, EAllowShrinking::No); }
	if (GetNetMode() == NM_DedicatedServer) { return; }
	if (Profile->ContactEffect)
	{ UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Profile->ContactEffect, Presentation.Location, Presentation.Normal.Rotation(), FVector(FMath::Clamp(Presentation.Strength, 0.1f, 2.f))); }
	if (Profile->ContactSound)
	{ UGameplayStatics::PlaySoundAtLocation(this, Profile->ContactSound, Presentation.Location, FMath::Clamp(Presentation.Strength, 0.1f, 2.f)); }
	if (Presentation.bLightFeedback)
	{
		// 更强命中允许替换局部反馈；同级连续命中保留当前短动作。
		PlayLightFeedback(Presentation.Direction, Presentation.LocalSourceDirection, Presentation.Strength);
	}
}

/** 登记本类型需要网络复制的属性及复制条件。 */
void UHodgeHitReactionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHodgeHitReactionComponent, Profile);
	DOREPLIFETIME(UHodgeHitReactionComponent, State);
}

/** 离开世界运行阶段并解除本对象持有的监听或资源。 */
void UHodgeHitReactionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeFromAbilitySystem();
	Super::EndPlay(EndPlayReason);
}
