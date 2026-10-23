/** @file HodgeHitReactionComponent.cpp
 * @brief 实现受击状态组件相关的类型、接口与现有处理流程。
 */
#include "Component/HodgeHitReactionComponent.h"
#include "Combat/HodgeHitReactionPlanBuilder.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Death.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeCombatComponentBase.h"
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
#include "TimerManager.h"
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
	if (ASC.Get() != InASC)
	{
		// 首次接入保留本Pawn已复制的受击段；失效的旧弱绑定仍须完整清理。
		if (!ASC.IsExplicitlyNull()) { UninitializeFromAbilitySystem(); }
		ASC = InASC;
	}
	if (!PoiseDeathHandle.IsValid())
	{
		// 死亡标签只负责清理；不在属性回调内提交新的受击计划。
		PoiseDeathHandle = ASC->RegisterGameplayTagEvent(HodgeGameplayTags::Status_Death, EGameplayTagEventType::NewOrRemoved).AddWeakLambda(this,
			[this](FGameplayTag, int32 Count) { if (Count > 0) { ResetPoise(); ClearFacing(true); } });
	}
	if (!PoiseHealthHandle.IsValid())
	{
		// 直接生命修改也可能致死，不能等下一次命中才清理旧积累。
		PoiseHealthHandle = ASC->GetGameplayAttributeValueChangeDelegate(UHodgeHealthSet::GetHealthAttribute()).AddWeakLambda(this,
			[this](const FOnAttributeChangeData& Change) { if (Change.NewValue <= 0.f) { ResetPoise(); ClearFacing(true); } });
	}
	RefreshConfiguration();
	OnRep_State();
	SetComponentTickEnabled(true);
}

/** 解除初始化来源能力System。 */
void UHodgeHitReactionComponent::UninitializeFromAbilitySystem()
{
	// 先退休本段，随后释放 Controlled 时旧回调不能恢复补转占用。
	ClearFacing(true);
	ResetPoise();
	if (ASC.IsValid() && PoiseDeathHandle.IsValid())
	{ ASC->RegisterGameplayTagEvent(HodgeGameplayTags::Status_Death, EGameplayTagEventType::NewOrRemoved).Remove(PoiseDeathHandle); }
	PoiseDeathHandle.Reset();
	if (ASC.IsValid() && PoiseHealthHandle.IsValid())
	{ ASC->GetGameplayAttributeValueChangeDelegate(UHodgeHealthSet::GetHealthAttribute()).Remove(PoiseHealthHandle); }
	PoiseHealthHandle.Reset();
	PoisePawnData.Reset();
	StopRetiredPresentation();
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
	if (PoisePawnData.Get() != PawnData) { ResetPoise(); PoisePawnData = PawnData; }
	UHodgeHitReactionProfile* Desired = OverrideProfile ? OverrideProfile.Get() : PawnData ? PawnData->HitReactionProfile.Get() : nullptr;
	if (Profile != Desired)
	{
		ResetPoise();
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
	Hit.FacingSource.TargetGeneration = FacingGeneration;
	if (IsValid(Cast<ACharacter>(Source)) && Source != GetOwner())
	{
		Hit.FacingSource.SourceLocation = Source->GetActorLocation();
		Hit.FacingSource.TargetLocation = GetOwner()->GetActorLocation();
		Hit.FacingSource.bValid = !Hit.FacingSource.SourceLocation.ContainsNaN() && !Hit.FacingSource.TargetLocation.ContainsNaN();
		Hit.FacingSource.SourceLocation.Z = Hit.FacingSource.TargetLocation.Z = 0.f;
	}
	// 检测体原点可能越过目标或位于刀刃侧面；普通削韧位移以攻击者快照为源，接触表现仍保留原采样。
	Hit.Origin = Request.InputMode == EHodgeReactionInputMode::PoiseDriven && IsValid(Source) && Source != GetOwner()
		? Source->GetActorLocation() : SourceOrigin;
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
	else if (Hit.Request.InputMode == EHodgeReactionInputMode::PoiseDriven && Hit.Damage <= 0.f)
	{ Result.Outcome = EHodgeHitReactionOutcome::NoDamageOutput; }
	else if (State.Phase == EHodgeHitReactionPhase::Airborne && !Hit.Request.bCanLaunch)
	{
		// 不续飞也不提前播放地面动作；保持当前弹道，在真实接地后倒地并起身。
		const bool bEligible = HodgeHitReaction::CanImpact(Result.AttackRank, Result.TargetBodyRank);
		if (bEligible) { State.bKnockdownOnLanding = true; PublishState(); }
		Result.Outcome = bEligible ? EHodgeHitReactionOutcome::NoImpact : EHodgeHitReactionOutcome::LowJudgement;
	}
	else if (Hit.Request.bCanLaunch)
	{ RefreshConfiguration(); ResolveSegmentLaunch(Hit, Result); }
	else if (Hit.Request.InputMode == EHodgeReactionInputMode::PoiseDriven)
	{
		// 接受回调可能对应零生命损失；普通削韧只消费实际正伤害。
		if (Hit.Damage <= 0.f) { Result.Outcome = EHodgeHitReactionOutcome::NoDamageOutput; }
		else { RefreshConfiguration(); ResolvePoise(Hit, Result); }
	}
	else if (!HodgeHitReaction::CanImpact(Result.AttackRank, Result.TargetBodyRank))
	{ Result.Outcome = EHodgeHitReactionOutcome::LowJudgement; }
	else if (Hit.Request.Impacts.IsEmpty() || (!Hit.Request.bCanLaunch && !Hit.Request.Impacts.ContainsByPredicate([](const auto& Impact)
		{ return Impact.Type != EHodgeImpactType::Launch && Impact.Type != EHodgeImpactType::AirHit; })))
	{ Result.Outcome = EHodgeHitReactionOutcome::NoImpact; }
	// 较轻地面请求不撤销正在进行的击退，也不把倒地或起身拉回站立。
	else if (HodgeHitReaction::IsContactOnlyTransition(State, Hit.Request))
	{ Result.Outcome = EHodgeHitReactionOutcome::NoImpact; }
	else
	{
		RefreshConfiguration();
		FHodgeHitReactionState Plan;
		if (!Profile) { Result.Outcome = EHodgeHitReactionOutcome::NotReady; }
		else if (!BuildPlan(Hit, Plan, Result.AppliedImpacts)) { Result.Outcome = EHodgeHitReactionOutcome::Unsupported; }
		else if (SubmitPlan(Plan, Result.Outcome, Hit.FacingSource)) { Result.Outcome = EHodgeHitReactionOutcome::Applied; }
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
		Result.Outcome == EHodgeHitReactionOutcome::CannotInterrupt ||
		// 普通命中即使暂时不能提交控制，也保留已经确认的接触反馈。
		(Hit.Request.InputMode == EHodgeReactionInputMode::PoiseDriven && Hit.bAccepted && !Hit.bRejected && Hit.Damage > 0.f &&
			(Result.Outcome == EHodgeHitReactionOutcome::NotReady || Result.Outcome == EHodgeHitReactionOutcome::Busy)) ||
		(Result.Outcome == EHodgeHitReactionOutcome::Dead && Hit.bAccepted && !Hit.bRejected && Hit.Damage > 0.f))
	{
		// 致死接触仍有音画，但不能播放站立轻反馈覆盖死亡姿态。
		Hit.Presentation.bLightFeedback = Result.Outcome != EHodgeHitReactionOutcome::Dead && (Result.Outcome != EHodgeHitReactionOutcome::Applied ||
			(State.Phase == EHodgeHitReactionPhase::Controlled && State.Type == EHodgeImpactType::HitStun &&
				GetServerTime() - State.StartedAt > 0.01f));
		MulticastContactFeedback(Hit.Presentation);
	}
	LastResult = Result;
	if (Result.Outcome == EHodgeHitReactionOutcome::Dead) { ResetPoise(); }
	// 按需留存同次命中结算快照，避免编辑器Python结构委托转接改变或中断战斗调用链。
	if (UE_LOG_ACTIVE(LogHodgeHitPresentation, Verbose))
	{
		const auto* Character = Cast<ACharacter>(GetOwner());
		auto* Anim = Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
		const auto* Instance = Anim && State.Montage ? Anim->GetActiveInstanceForMontage(State.Montage) : nullptr;
		UE_LOG(LogHodgeHitPresentation, Verbose, TEXT("HitFacingResolved Avatar=%s Hit=%s Time=%.6f Outcome=%s Level=%s Phase=%s Falling=%d Controlled=%d Sequence=%d ChainStartedAt=%.6f Chain=%s Segment=%u Revision=%u Montage=%s MontageInstance=%d CanLaunch=%d"),
			*GetPathNameSafe(GetOwner()), *HitId.ToString(EGuidFormats::Digits), GetWorld()->GetTimeSeconds(),
			*StaticEnum<EHodgeHitReactionOutcome>()->GetNameStringByValue(int64(Result.Outcome)),
			*StaticEnum<EHodgeReactionLevel>()->GetNameStringByValue(int64(Result.ReactionLevel)),
			*StaticEnum<EHodgeHitReactionPhase>()->GetNameStringByValue(int64(State.Phase)),
			Character && Character->GetCharacterMovement()->IsFalling(), IsControlled(), State.Sequence, State.ChainStartedAt,
			*FacingState.ChainId.ToString(EGuidFormats::Digits), FacingState.Segment, FacingState.Revision,
			*GetPathNameSafe(State.Montage.Get()), Instance ? Instance->GetInstanceID() : INDEX_NONE, Hit.Request.bCanLaunch);
	}
	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	OnReactionResolved.Broadcast(Result);
	return Result;
}

/** 诊断只统计仍在滑动窗口内的样本，不因 UI 读取改变玩法状态。 */
float UHodgeHitReactionComponent::GetAccumulatedPoise() const
{
	const auto* Data = PoisePawnData.Get();
	if (!Data || !Data->Poise.bEnabled) { return 0.f; }
	const float Now = GetServerTime();
	float Total = 0.f;
	for (const auto& Sample : PoiseSamples) { if (Now - Sample.Time < Data->Poise.Window) { Total += Sample.Amount; } }
	return Total;
}

/** 重置使所有迟到计时器失效；解除 ASC 后也不留旧 Pawn 的保护或阈值。 */
void UHodgeHitReactionComponent::ResetPoise()
{
	++PoiseGeneration;
	if (GetWorld()) { GetWorld()->GetTimerManager().ClearTimer(PoiseTimer); }
	PoiseSamples.Reset();
	ConsumedPoiseLevel = EHodgeReactionLevel::None;
}

/** 窗口清空只开启下一轮削韧，不提前取消还在执行的受控状态。 */
void UHodgeHitReactionComponent::ExpirePoise(float Now)
{
	const auto* Data = PoisePawnData.Get();
	if (!Data) { ResetPoise(); return; }
	PoiseSamples.RemoveAll([Now, Data](const auto& Sample) { return Now - Sample.Time >= Data->Poise.Window; });
	if (PoiseSamples.IsEmpty()) { ConsumedPoiseLevel = EHodgeReactionLevel::None; }
}

/** 空闲目标只在最早样本过期时唤醒，不为削韧开启逐帧 Tick。 */
void UHodgeHitReactionComponent::SchedulePoiseExpiry()
{
	if (!GetWorld()) { return; }
	GetWorld()->GetTimerManager().ClearTimer(PoiseTimer);
	const auto* Data = PoisePawnData.Get();
	if (!Data || PoiseSamples.IsEmpty()) { return; }
	const uint32 Generation = PoiseGeneration;
	const TWeakObjectPtr<AActor> Avatar = GetOwner();
	GetWorld()->GetTimerManager().SetTimer(PoiseTimer, FTimerDelegate::CreateWeakLambda(this, [this, Generation, Avatar]()
	{
		if (Generation != PoiseGeneration || !IsBound() || ASC->GetAvatarActor() != Avatar.Get()) { return; }
		ExpirePoise(GetServerTime());
		SchedulePoiseExpiry();
	}), FMath::Max(0.001f, PoiseSamples[0].Time + Data->Poise.Window - GetServerTime()), false);
}

/** 只在 FinishHit 消费完整结算；多个 GE 回调和重复 Finish 不会重复积累。 */
void UHodgeHitReactionComponent::ResolvePoise(FPendingHit Hit, FHodgeHitReactionResult& Result)
{
	Result.Outcome = EHodgeHitReactionOutcome::NoImpact;
	const auto* Data = PoisePawnData.Get();
	TArray<FText> Errors;
	if (!Profile || !Data || !Data->Poise.bEnabled || !Data->Poise.Validate(Errors) ||
		!Profile->SupportsImpact(EHodgeImpactType::HitStun) || !Profile->SupportsImpact(EHodgeImpactType::Knockback))
	{
		UE_LOG(LogHodgeHitPresentation, Warning, TEXT("Poise target not configured: %s PawnData=%s"), *GetNameSafe(GetOwner()), *GetNameSafe(Data));
		return;
	}
	const auto& Settings = Data->Poise;
	const float Now = GetServerTime();
	ExpirePoise(Now);
	const bool bEligible = HodgeHitReaction::CanImpact(Result.AttackRank, Result.TargetBodyRank);
	if (!bEligible) { Result.Outcome = EHodgeHitReactionOutcome::LowJudgement; }
	const auto* Character = Cast<ACharacter>(GetOwner());
	if (!Character || Character->GetCharacterMovement()->IsFalling() ||
		State.Phase == EHodgeHitReactionPhase::Airborne || State.Phase == EHodgeHitReactionPhase::Downed || State.Phase == EHodgeHitReactionPhase::GettingUp ||
		(!bEligible && !Settings.bAccumulateUnderArmor)) { Result.AccumulatedPoise = GetAccumulatedPoise(); return; }
	const float Amount = Hit.Request.ReservedPoiseDamage;
	if (Amount <= 0.f) { Result.AccumulatedPoise = GetAccumulatedPoise(); return; }
	// 超限显式拒绝本次削韧，生命伤害与接触照常，不能静默丢失旧样本。
	if (PoiseSamples.Num() >= Settings.MaxSamples || Amount > MAX_flt - GetAccumulatedPoise())
	{
		UE_LOG(LogHodgeHitPresentation, Warning, TEXT("Poise history capacity exceeded: %s Count=%d Limit=%d"), *GetNameSafe(GetOwner()), PoiseSamples.Num(), Settings.MaxSamples);
		Result.AccumulatedPoise = GetAccumulatedPoise();
		return;
	}
	PoiseSamples.Add({Now, Amount});
	SchedulePoiseExpiry();
	Result.AccumulatedPoise = GetAccumulatedPoise();
	if (!bEligible) { return; }
	const auto Level = Result.AccumulatedPoise >= Settings.HeavyThreshold ? EHodgeReactionLevel::Heavy :
		Result.AccumulatedPoise >= Settings.LightThreshold ? EHodgeReactionLevel::Light : EHodgeReactionLevel::None;
	if (Level <= ConsumedPoiseLevel) { return; }
	FHodgeImpactSpec Impact;
	Impact.Type = Level == EHodgeReactionLevel::Heavy ? EHodgeImpactType::Knockback : EHodgeImpactType::HitStun;
	const float Scale = Hit.Request.ImpulseScale * Settings.DisplacementScale;
	Impact.KnockbackDistance = FMath::Min(Settings.MaxDistance, Settings.HeavyDistance * Scale);
	Impact.MoveDuration = Settings.MoveDuration;
	Hit.Request.Impacts = {Impact};
	Hit.ReactionLevel = Level;
	const uint32 Generation = PoiseGeneration;
	FHodgeHitReactionState Plan;
	if (!BuildPlan(Hit, Plan, Result.AppliedImpacts)) { Result.Outcome = EHodgeHitReactionOutcome::Unsupported; }
	else if (SubmitPlan(Plan, Result.Outcome, Hit.FacingSource) && IsBound() && Generation == PoiseGeneration)
	{
		ConsumedPoiseLevel = Level;
		Result.ReactionLevel = Level;
		Result.Outcome = EHodgeHitReactionOutcome::Applied;
	}
	if (Result.Outcome != EHodgeHitReactionOutcome::Applied) { Result.AppliedImpacts.Reset(); }
}

/** 击飞与削韧解耦，沿用目标 Profile、Body、CMC 以及已有网络计划。 */
void UHodgeHitReactionComponent::ResolveSegmentLaunch(FPendingHit Hit, FHodgeHitReactionResult& Result)
{
	Result.Outcome = EHodgeHitReactionOutcome::NoImpact;
	if (!HodgeHitReaction::CanImpact(Result.AttackRank, Result.TargetBodyRank)) { Result.Outcome = EHodgeHitReactionOutcome::LowJudgement; return; }
	if (!Profile) { Result.Outcome = EHodgeHitReactionOutcome::NotReady; return; }
	if (!Profile->SupportsImpact(EHodgeImpactType::Launch) || !Profile->SupportsImpact(EHodgeImpactType::Knockdown))
	{ Result.Outcome = EHodgeHitReactionOutcome::Unsupported; return; }
	// 续飞不能重置首次离地时间或绕过最大空中次数。
	if (State.Phase == EHodgeHitReactionPhase::Airborne && State.AirHitCount >= Profile->MaxAirHits) { return; }
	FHodgeImpactSpec Launch;
	const auto* Existing = Hit.Request.Impacts.FindByPredicate([](const auto& Impact) { return Impact.Type == EHodgeImpactType::Launch || Impact.Type == EHodgeImpactType::AirHit; });
	if (Existing) { Launch = *Existing; }
	else if (const auto* Data = PoisePawnData.Get())
	{
		TArray<FText> Errors;
		if (!Data->Poise.ValidateLaunchMotion(Errors)) { Result.Outcome = EHodgeHitReactionOutcome::Unsupported; return; }
		const float Scale = Hit.Request.ImpulseScale * Data->Poise.DisplacementScale;
		Launch.HorizontalSpeed = Data->Poise.LaunchHorizontalSpeed * Scale;
		Launch.VerticalSpeed = Data->Poise.LaunchVerticalSpeed * Scale;
	}
	Launch.Type = EHodgeImpactType::Launch;
	Launch.bKnockdownOnLanding = true;
	Hit.Request.Impacts = {Launch};
	FHodgeHitReactionState Plan;
	if (!BuildPlan(Hit, Plan, Result.AppliedImpacts)) { Result.Outcome = EHodgeHitReactionOutcome::Unsupported; }
	else if (SubmitPlan(Plan, Result.Outcome, Hit.FacingSource)) { Result.Outcome = EHodgeHitReactionOutcome::Applied; }
	if (Result.Outcome != EHodgeHitReactionOutcome::Applied) { Result.AppliedImpacts.Reset(); }
}

/** 读取同步快照并委托纯计算；组件保留对象依赖诊断和最终提交职责。 */
bool UHodgeHitReactionComponent::BuildPlan(const FPendingHit& Hit, FHodgeHitReactionState& Plan, TArray<EHodgeImpactType>& Applied) const
{
	const auto RejectDependency = [this, &Hit](const TCHAR* Stage, const TCHAR* Reason)
	{
		UE_LOG(LogHodgeHitPresentation, Warning, TEXT("Reaction plan rejected Owner=%s Profile=%s Execution=%s Stage=%s: %s"),
			*GetNameSafe(GetOwner()), *GetPathNameSafe(Profile), *Hit.Presentation.ExecutionId.ToString(), Stage, Reason);
		return false;
	};
	// 保留原先的前置检查顺序，任何失败都不能提前写入候选输出。
	if (!Profile) { return RejectDependency(TEXT("Dependencies"), TEXT("Target reaction Profile is unavailable.")); }
	if (!FMath::IsFinite(Profile->MaxControlDuration) || Profile->MaxControlDuration <= 0.f)
	{ return RejectDependency(TEXT("SafetyLimits"), TEXT("MaxControlDuration must be finite and positive.")); }
	const auto* Character = Cast<ACharacter>(GetOwner());
	if (!Character) { return RejectDependency(TEXT("Dependencies"), TEXT("Reaction owner is not a Character.")); }

	FHodgeHitReactionPlanContext Context;
	Context.Request = Hit.Request;
	Context.Presentation = Hit.Presentation;
	Context.ReactionLevel = Hit.ReactionLevel;
	Context.State = State;
	Context.Profile = Profile;
	Context.Mesh = Character->GetMesh()->GetSkeletalMeshAsset();
	Context.Origin = Hit.Origin;
	Context.FacingSource = Hit.FacingSource;
	Context.TargetLocation = Character->GetActorLocation();
	Context.TargetForward = Character->GetActorForwardVector();
	Context.bSourceValid = Hit.Source.IsValid();
	if (Context.bSourceValid) { Context.SourceForward = Hit.Source->GetActorForwardVector(); }
	Context.ServerTime = GetServerTime();
	Context.Variation = NextSequence + 1;
	const auto* Movement = Character->GetCharacterMovement();
	// 排队的 Launch 尚未物理起飞时仍属于空中，不能只采样 IsFalling。
	Context.bAir = Movement->IsFalling() || (State.Phase == EHodgeHitReactionPhase::Airborne && !bSawAirborne);
	Context.bCanMove = Movement->IsActive() && Movement->MovementMode != MOVE_None && Movement->HasValidData();
	Context.bHasAnimInstance = ASC.IsValid() && ASC->AbilityActorInfo.IsValid() && ASC->AbilityActorInfo->GetAnimInstance() != nullptr;

	TArray<FText> Errors;
	const bool bBuilt = FHodgeHitReactionPlanBuilder::Build(Context, Plan, Applied, Errors);
	// 计算器只返回诊断，日志继续携带当前组件与执行身份。
	for (const FText& Error : Errors)
	{
		const FString Message = Error.ToString();
		if (Message.StartsWith(TEXT("Stage=")))
		{
			UE_LOG(LogHodgeHitPresentation, Warning, TEXT("Reaction plan rejected Owner=%s Profile=%s Execution=%s %s"),
				*GetNameSafe(GetOwner()), *GetPathNameSafe(Profile), *Hit.Presentation.ExecutionId.ToString(), *Message);
		}
		else
		{
			UE_LOG(LogHodgeHitPresentation, Warning, TEXT("Invalid selected reaction Owner=%s Profile=%s Type=%d: %s"),
				*GetNameSafe(GetOwner()), *GetPathNameSafe(Profile), int32(Plan.Type), *Message);
		}
	}
	return bBuilt;
}

/** 提交Plan。 */
bool UHodgeHitReactionComponent::SubmitPlan(FHodgeHitReactionState Plan, EHodgeHitReactionOutcome& Failure,
	const FHodgeHitReactionFacingSource& FacingSource)
{
	Failure = EHodgeHitReactionOutcome::NotReady;
	// 按服务器权限区分本次处理分支。
	if (!IsBound() || !GetOwner()->HasAuthority()) { return false; }
	if (bSubmitting || bCompleting) { Failure = EHodgeHitReactionOutcome::Busy; return false; }
	TGuardValue<bool> Guard(bSubmitting, true);
	const FHodgeHitReactionState Previous = State;
	const auto* PreviousCharacter = Cast<ACharacter>(GetOwner());
	const bool bWasAir = PreviousCharacter->GetCharacterMovement()->IsFalling() || Previous.Phase == EHodgeHitReactionPhase::Airborne;
	const uint32 Generation = PoiseGeneration;
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
	StopRetiredPresentation();
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
	if (!IsBound() || ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) ||
		(Plan.ReactionLevel != EHodgeReactionLevel::None && Generation != PoiseGeneration)) { SetControl(State.IsActive()); return false; }
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
	CommitFacing(FacingSource, Previous, bWasAir);
	return true;
}

/** 最终计划及 GA 已成功后才发许可，配置失败不撤销原本合法的受击。 */
void UHodgeHitReactionComponent::CommitFacing(const FHodgeHitReactionFacingSource& Source,
	const FHodgeHitReactionState& Previous, bool bWasAir)
{
	if (!IsBound() || !State.IsActive()) { return; }
	if (State.MotionSource == EHodgeReactionMotionSource::AnimationRootMotion)
	{
		// 退休只撤销可选Yaw许可，保留实际结束记录及同级已尝试的事实。
		if (FacingState.HasIdentity() && !FacingState.bRetired)
		{ FinishFacingSegment(FacingState.ChainId, FacingState.Segment, FacingState.Revision, GetOwner()->GetActorRotation().Yaw, false); }
		return;
	}
	if (!Profile || !Profile->Facing.bEnabled || Source.TargetGeneration != FacingGeneration) { return; }
	FHodgeHitReactionPlanContext Context;
	Context.Profile = Profile;
	Context.State = Previous;
	Context.bAir = bWasAir;
	Context.FacingSource = Source;
	const float ActualYaw = GetOwner()->GetActorRotation().Yaw;
	FHodgeHitReactionFacingCandidate Candidate;
	TArray<FText> Errors;
	const bool bValid = FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, State, ActualYaw, Candidate, Errors);
	for (const FText& Error : Errors)
	{ UE_LOG(LogHodgeHitPresentation, Warning, TEXT("Optional facing skipped Owner=%s Profile=%s Sequence=%d: %s"), *GetNameSafe(GetOwner()), *GetPathNameSafe(Profile), State.Sequence, *Error.ToString()); }
	if (Candidate.Preset == EHodgeHitReactionFacingPreset::None)
	{
		// 被排除的根运动或倒地阶段不能继续执行之前尚未结束的补转。
		if (FacingState.IsTurning() && (State.MotionSource != EHodgeReactionMotionSource::Impact ||
			State.Phase == EHodgeHitReactionPhase::Downed || State.Phase == EHodgeHitReactionPhase::GettingUp))
		{ FinishFacingSegment(FacingState.ChainId, FacingState.Segment, FacingState.Revision, ActualYaw, false); }
		return;
	}
	const bool bContinueChain = Previous.IsActive() && FacingState.HasIdentity();
	if (bContinueChain && uint8(Candidate.Preset) <= uint8(FacingState.Preset)) { return; }
	FHodgeHitReactionFacingState Next;
	Next.ChainId = bContinueChain ? FacingState.ChainId : FGuid::NewGuid();
	Next.Segment = bContinueChain ? FacingState.Segment + 1 : 1;
	Next.Revision = 1;
	Next.Preset = Candidate.Preset;
	Next.StartedAt = GetServerTime();
	Next.EndsAt = FMath::Min(Next.StartedAt + Candidate.WindowDuration, State.EndsAt);
	Next.StartYaw = FMath::IsFinite(ActualYaw) ? FRotator::NormalizeAxis(ActualYaw) : 0.f;
	Next.TargetYaw = Candidate.TargetYaw;
	Next.MaxYawSpeed = Candidate.MaxYawSpeed;
	Next.FinalYaw = Next.StartYaw;
	Next.bHasTarget = bValid && Candidate.bHasTarget && Next.EndsAt > Next.StartedAt;
	Next.bFinal = !Next.bHasTarget;
	FacingState = Next;
	OnRep_FacingState();
	GetOwner()->ForceNetUpdate();
}

/** 生命周期及独立身份约束仍保留的记录，不将运动政策混入实际终点写入。 */
bool UHodgeHitReactionComponent::IsFacingIdentityCurrent(const FGuid& ChainId, uint32 Segment) const
{
	const auto* Health = ASC.IsValid() ? ASC->GetSet<UHodgeHealthSet>() : nullptr;
	return IsBound() && State.IsActive() && (!Health || Health->GetHealth() > 0.f) && !ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) &&
		FacingState.HasIdentity() && FacingState.ChainId == ChainId && FacingState.Segment == Segment;
}

/** 历史结束记录不能为根运动新计划或已退休同级段重新授权Yaw。 */
bool UHodgeHitReactionComponent::IsFacingStateCurrent(const FGuid& ChainId, uint32 Segment) const
{
	return IsFacingIdentityCurrent(ChainId, Segment) && FacingState.bHasTarget && !FacingState.bRetired &&
		State.MotionSource == EHodgeReactionMotionSource::Impact;
}

/** 动画显式暂停不推进可选朝向；截止仍使用现有服务器世界时钟。 */
bool UHodgeHitReactionComponent::IsFacingPaused() const
{
	const auto* Character = Cast<ACharacter>(GetOwner());
	const auto* Mesh = Character ? Character->GetMesh() : nullptr;
	if (!Mesh || Mesh->bPauseAnims || Mesh->GlobalAnimRateScale <= 0.f || Character->CustomTimeDilation <= 0.f) { return true; }
	const auto* Anim = Mesh->GetAnimInstance();
	const auto* Instance = Anim && State.Montage ? Anim->GetInstanceForMontage(State.Montage) : nullptr;
	return Instance && (!Instance->IsPlaying() || Instance->GetPlayRate() == 0.f);
}

/** 服务器以实际执行后的朝向结束同一修订，不从剩余预算推算未执行角度。 */
void UHodgeHitReactionComponent::FinishFacingSegment(const FGuid& ChainId, uint32 Segment, uint32 Revision,
	float FinalYaw, bool bHardLocked)
{
	if (!GetOwner()->HasAuthority() || !IsFacingIdentityCurrent(ChainId, Segment) || FacingState.Revision != Revision ||
		!FMath::IsFinite(FinalYaw)) { return; }
	const bool bRetire = State.MotionSource == EHodgeReactionMotionSource::AnimationRootMotion;
	if (FacingState.bFinal && (!bRetire || FacingState.bRetired)) { return; }
	// 根运动阶段只能撤销旧许可；已有实际终点及硬锁记录保持原值。
	if (!FacingState.bFinal)
	{
		FacingState.FinalYaw = FRotator::NormalizeAxis(FinalYaw);
		FacingState.bFinal = true;
		FacingState.bHardLocked = bHardLocked;
	}
	FacingState.bRetired |= bRetire;
	++FacingState.Revision;
	OnRep_FacingState();
	GetOwner()->ForceNetUpdate();
}

/** 只刷新现有仲裁状态；真实旋转与校正在 CMC 内消费。 */
void UHodgeHitReactionComponent::OnRep_FacingState()
{
	if (auto* Rotation = GetOwner() ? GetOwner()->FindComponentByClass<UHodgeCharacterRotationComponent>() : nullptr)
	{ Rotation->ApplyResolvedMode(); }
}

/** 清理共享先退休后释放的顺序，死亡不等待下一次 PostPhysics 才阻止补转。 */
void UHodgeHitReactionComponent::ClearFacing(bool bInvalidatePending)
{
	if (bInvalidatePending) { FacingGeneration = FacingGeneration == MAX_uint32 ? 1 : FacingGeneration + 1; }
	FacingState = {};
	OnRep_FacingState();
	const auto* Character = Cast<ACharacter>(GetOwner());
	if (auto* Movement = Character ? Cast<UHodgeCharacterMovementComponent>(Character->GetCharacterMovement()) : nullptr)
	{ Movement->ResetHitReactionFacing(); }
	if (GetOwner() && GetOwner()->HasAuthority()) { GetOwner()->ForceNetUpdate(); }
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
	SetComponentTickEnabled(State.IsActive() || !Profile || RetiredMontage.IsValid());
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

/** 按具体实例接管恢复尾部；根运动尾部另外持有姿势更新占用，不延长控制。 */
bool UHodgeHitReactionComponent::RetirePresentation(UAnimMontage* Montage, int32 InstanceId)
{
	if (!Profile || !Montage || !IsBound() ||
		(Montage->HasRootMotion() ? !Profile->bAllowRootMotionRecoveryTail : !Profile->bAllowNaturalRecoveryTail) ||
		ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death)) { return false; }
	const auto* Health = ASC->GetSet<UHodgeHealthSet>();
	if (Health && Health->GetHealth() <= 0.f) { return false; }
	const auto* Character = Cast<ACharacter>(GetOwner());
	UAnimInstance* Anim = Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	auto* Instance = Anim ? Anim->GetMontageInstanceForID(InstanceId) : nullptr;
	if (!Instance || Instance->Montage != Montage || Instance->IsStopped() ||
		Instance->GetPosition() >= Montage->GetPlayLength() - 0.01f) { return false; }
	if (RetiredMontage.Get() == Montage && RetiredMontageInstanceId == InstanceId) { return true; }
	StopRetiredPresentation();
	Instance->bEnableAutoBlendOut = true;
	RetiredMontage = Montage;
	RetiredMontageInstanceId = InstanceId;
	RetiredExpiresAt = GetServerTime() + Montage->GetPlayLength() + Montage->BlendOut.GetBlendTime();
	if (GetOwner()->HasAuthority() && Montage->HasRootMotion())
	{
		RetiredPoseCombat = UHodgeCombatComponentBase::FindCombatComponent(GetOwner());
		if (RetiredPoseCombat.IsValid()) { RetiredPoseLease = RetiredPoseCombat->AcquirePoseLease(); }
	}
	SetComponentTickEnabled(true);
	return true;
}

/** 直接按实例清理，不能通过 ASC 当前 Montage 误停新动作。 */
void UHodgeHitReactionComponent::StopRetiredPresentation()
{
	const auto* Character = Cast<ACharacter>(GetOwner());
	UAnimInstance* Anim = Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (auto* Instance = Anim ? Anim->GetMontageInstanceForID(RetiredMontageInstanceId) : nullptr;
		Instance && RetiredMontage.IsValid() && Instance->Montage == RetiredMontage.Get())
	{
		// 旧动作可继续混出姿势，但取消后不能继续拖动新动作的胶囊。
		if (RetiredMontage->HasRootMotion()) { Instance->PushDisableRootMotion(); }
		Instance->Stop(FAlphaBlend(RetiredMontage->BlendOut), true);
	}
	RetiredMontage.Reset();
	RetiredMontageInstanceId = INDEX_NONE;
	RetiredExpiresAt = 0.f;
	if (RetiredPoseCombat.IsValid()) { RetiredPoseCombat->ReleasePoseLease(RetiredPoseLease); }
	RetiredPoseCombat.Reset();
	RetiredPoseLease.Invalidate();
}

/** 恢复资格必须绑定当前存活 Avatar，配置不向其他阶段泄漏。 */
bool UHodgeHitReactionComponent::CanRecover(EHodgeHitRecoveryAction Action) const
{
	if (!IsBound() || !Profile || ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death)) { return false; }
	// 恢复动作只解除本次受击，不得借窗口绕过独立的旋转或移动禁令。
	if (Action != EHodgeHitRecoveryAction::Movement &&
		(ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Rotation_Locked) ||
		 ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped))) { return false; }
	const auto* Health = ASC->GetSet<UHodgeHealthSet>();
	const auto* Character = Cast<ACharacter>(GetOwner());
	return (!Health || Health->GetHealth() > 0.f) && Character && Character->GetCharacterMovement()->IsMovingOnGround() &&
		Profile->IsRecoveryWindowOpen(State, Action, GetServerTime());
}

/** 新动作提交成功后由服务器撤销旧 GA，客户端不预测删除服务器的受击身份。 */
bool UHodgeHitReactionComponent::TryRecover(EHodgeHitRecoveryAction Action, int32 ExpectedSequence)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || State.Sequence != ExpectedSequence || !CanRecover(Action)) { return false; }
	if (ActiveAbility.IsValid()) { ActiveAbility->FinishReaction(true); }
	else { CompletePlan(ExpectedSequence); }
	return !State.IsActive();
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
	ClearFacing();
	SetControl(false);
	PublishState();
}

/** 检查是否允许cel受击。 */
void UHodgeHitReactionComponent::CancelReaction()
{
	// 按服务器权限区分本次处理分支。
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		StopRetiredPresentation();
		if (ActiveAbility.IsValid()) { ActiveAbility->FinishReaction(true); }
		else { CompletePlan(State.Sequence); }
	}
}

/** 切换当前状态流程的执行阶段。 */
void UHodgeHitReactionComponent::ChangePhase(EHodgeHitReactionPhase Phase, UAnimMontage* Montage, float Duration)
{
	if (FacingState.IsTurning() && (Phase == EHodgeHitReactionPhase::Downed || Phase == EHodgeHitReactionPhase::GettingUp))
	{ FinishFacingSegment(FacingState.ChainId, FacingState.Segment, FacingState.Revision, GetOwner()->GetActorRotation().Yaw, false); }
	State.Phase = Phase;
	State.StartedAt = GetServerTime();
	State.EndsAt = FMath::Min(State.StartedAt + Duration, State.ChainStartedAt + Profile->MaxControlDuration);
	State.Montage = Montage;
	// 落地或起身切入根运动时同样退休旧段，后续原地阶段不能重新授权。
	if (State.MotionSource == EHodgeReactionMotionSource::AnimationRootMotion && FacingState.HasIdentity() && !FacingState.bRetired)
	{ FinishFacingSegment(FacingState.ChainId, FacingState.Segment, FacingState.Revision, GetOwner()->GetActorRotation().Yaw, false); }
	PublishState();
}

/** 请求获取Up。 */
bool UHodgeHitReactionComponent::RequestGetUp()
{
	// 按服务器权限区分本次处理分支。
	if (!GetOwner()->HasAuthority() || State.Phase != EHodgeHitReactionPhase::Downed || !Profile) { return false; }
	if (State.GetUpMontage)
	{
		State.MotionSource = State.GetUpMotionSource;
		State.bLanding = false;
		ChangePhase(EHodgeHitReactionPhase::GettingUp, State.GetUpMontage, State.GetUpMontage->GetPlayLength());
	}
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
	// 无有效移动或局部动画暂停时也按世界截止记录实际结果，不补齐剩余角度。
	if (FacingState.IsTurning() && Now >= FacingState.EndsAt)
	{ FinishFacingSegment(FacingState.ChainId, FacingState.Segment, FacingState.Revision, GetOwner()->GetActorRotation().Yaw, false); }
	// 普通移动只按配置自然开放，不提前开放被明确禁止的闪避或普攻。
	if (CanRecover(EHodgeHitRecoveryAction::Movement)) { CompletePlan(State.Sequence); return; }
	auto* Character = CastChecked<ACharacter>(GetOwner());
	auto* Movement = Character->GetCharacterMovement();
	if (auto* AI = Cast<AAIController>(Character->GetController())) { AI->StopMovement(); }
	if (State.Phase == EHodgeHitReactionPhase::Airborne)
	{
		bSawAirborne |= Movement->IsFalling();
		if (Character->GetActorLocation().Z - State.AirOriginZ >= Profile->MaxLaunchHeight && Movement->Velocity.Z > 0.f) { Movement->Velocity.Z = 0.f; }
		if (bSawAirborne && Movement->IsMovingOnGround())
		{
			State.bLanding = true;
			State.bHasMovement = false;
			State.MotionSource = State.LandingMotionSource;
			if (State.bKnockdownOnLanding)
			{
				FHodgeHitReactionAnimation DownEntry;
				const auto* Down = Profile->ResolveAnimation(EHodgeImpactType::Knockdown, DownEntry) ? &DownEntry : nullptr;
				if (Down)
				{
					State.GetUpMontage = Down->GetUpMontage;
					State.GetUpMotionSource = Down->GetUpMotionSource;
					UAnimMontage* Landing = State.LandingMontage ? State.LandingMontage.Get() : Down->Montage.Get();
					if (!State.LandingMontage) { State.MotionSource = Down->MotionSource; }
					ChangePhase(EHodgeHitReactionPhase::Downed, Landing, FMath::Max(Profile->DownedDuration, Landing ? Landing->GetPlayLength() : 0.f));
				}
				else { CompletePlan(State.Sequence); }
			}
			else if (State.LandingMontage) { ChangePhase(EHodgeHitReactionPhase::Controlled, State.LandingMontage, State.LandingMontage->GetPlayLength()); }
			else { CompletePlan(State.Sequence); }
		}
		else if (Now >= State.EndsAt) { CompletePlan(State.Sequence); }
		else if (State.AirLoopMontage && bSawAirborne &&
			(!State.Montage || (State.Montage == State.AirLoopMontage || State.AirLoopTransitionTime <= 0.f ?
				Now - State.StartedAt >= State.Montage->GetPlayLength() :
				Now - State.StartedAt >= State.AirLoopTransitionTime && Movement->Velocity.Z <= 0.f)))
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
	// 恢复尾部允许动画自身位移；主动输入或新动作接管时不等待整段结束。
	if (RetiredMontage.IsValid())
	{
		const auto* Character = Cast<ACharacter>(GetOwner());
		auto* Anim = Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
		const auto* Instance = Anim ? Anim->GetMontageInstanceForID(RetiredMontageInstanceId) : nullptr;
		const auto* Health = ASC->GetSet<UHodgeHealthSet>();
		if (!Instance || Instance->Montage != RetiredMontage.Get() || Instance->GetWeight() <= UE_KINDA_SMALL_NUMBER ||
			GetServerTime() >= RetiredExpiresAt || ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) ||
			(Health && Health->GetHealth() <= 0.f) || !Character->GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero() ||
			(!RetiredMontage->HasRootMotion() && Character->GetVelocity().SizeSquared2D() > 1.f) ||
			(RetiredMontage->HasRootMotion() && Character->GetCharacterMovement()->IsFalling()) ||
			ASC->GetCurrentMontage() != RetiredMontage.Get())
		{ StopRetiredPresentation(); }
	}
	if (!State.IsActive() && Profile && !RetiredMontage.IsValid()) { SetComponentTickEnabled(false); }
	// 根据当前生命周期状态决定是否继续执行。
	if (State.IsActive() && State.Montage)
	{
		if (const auto* Character = Cast<ACharacter>(GetOwner()))
		{
			if (UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance())
			{
				if (FAnimMontageInstance* Instance = Anim->GetActiveInstanceForMontage(State.Montage))
				{
					Instance->bEnableAutoBlendOut = Profile ? !Profile->ShouldSustainPose(State) : State.Phase != EHodgeHitReactionPhase::Downed;
				}
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
	DOREPLIFETIME(UHodgeHitReactionComponent, FacingState);
}

/** 离开世界运行阶段并解除本对象持有的监听或资源。 */
void UHodgeHitReactionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeFromAbilitySystem();
	Super::EndPlay(EndPlayReason);
}
