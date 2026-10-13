/** @file HodgeLockOnComponent.cpp @brief 生命周期、预测命令和确认目标的唯一协调实现。 */
#include "Component/HodgeLockOnComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/HodgeLockOnStateEffect.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeHealthComponent.h"
#include "Combat/HodgeDamageRules.h"
#include "Combat/HodgeMovementActionTypes.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "Data/HodgePawnData.h"
#include "Data/HodgeLockOnProfile.h"
#include "Interface/HodgeLockOnTargetInterface.h"
#include "UI/IndicatorSystem/HodgeIndicatorManagerComponent.h"
#include "UI/IndicatorSystem/IndicatorDescriptor.h"
#include "UI/Subsystem/HodgeUIManagerSubsystem.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "Net/UnrealNetwork.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeLockOnComponent)

const FName UHodgeLockOnComponent::NAME_ActorFeatureName(TEXT("LockOn"));
UHodgeLockOnComponent::UHodgeLockOnComponent(const FObjectInitializer& Initializer) : Super(Initializer)
{
	PrimaryComponentTick.bCanEverTick = true; PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = TG_PrePhysics; SetIsReplicatedByDefault(true);
}
void UHodgeLockOnComponent::OnRegister() { Super::OnRegister(); bClosed = false; RegisterInitStateFeature(); }
void UHodgeLockOnComponent::BeginPlay()
{
	Super::BeginPlay(); BindOnActorInitStateChanged(NAME_None, {}, false);
	Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(GetOwner());
	if (Extension.IsValid())
	{
		Extension->OnPawnDataChanged.AddUObject(this,&ThisClass::CheckDefaultInitialization);
		Extension->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this,&ThisClass::Uninitialize));
		Extension->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this,&ThisClass::CheckDefaultInitialization));
	}
	CheckDefaultInitialization();
}
bool UHodgeLockOnComponent::CanChangeInitState(UGameFrameworkComponentManager*, FGameplayTag Current, FGameplayTag Desired) const
{
	if (bClosed || !GetPawn<APawn>()) { return false; }
	if (!Current.IsValid() && Desired == HodgeGameplayTags::InitState_Spawned) { return true; }
	const auto* Data = Extension.IsValid() ? Extension->GetPawnData<UHodgePawnData>() : nullptr;
	// DataAvailable 不等待 ASC，否则会与 PawnExtension 的全 Feature 屏障互相等待。
	if (Current == HodgeGameplayTags::InitState_Spawned && Desired == HodgeGameplayTags::InitState_DataAvailable) { return Data != nullptr; }
	if (Data && !Data->LockOnProfile)
	{ return Extension->HasReachedInitState(HodgeGameplayTags::InitState_DataInitialized) && (Desired == HodgeGameplayTags::InitState_DataInitialized || Desired == HodgeGameplayTags::InitState_GameplayReady); }
	if (Current == HodgeGameplayTags::InitState_DataAvailable && Desired == HodgeGameplayTags::InitState_DataInitialized)
	{ return bBound && ASC->GetAvatarActor() == GetOwner() && Rotation.IsValid() && Rotation->IsFacingSystemReady(); }
	return Current == HodgeGameplayTags::InitState_DataInitialized && Desired == HodgeGameplayTags::InitState_GameplayReady && bBound;
}
void UHodgeLockOnComponent::OnActorInitStateChanged(const FActorInitStateChangedParams&) { CheckDefaultInitialization(); }
void UHodgeLockOnComponent::CheckDefaultInitialization()
{
	if (bClosed) { return; }
	if (bCheckingInitialization) { bRecheckInitialization = true; return; }
	TGuardValue<bool> Guard(bCheckingInitialization,true);
	static const TArray<FGameplayTag> Chain = {HodgeGameplayTags::InitState_Spawned,HodgeGameplayTags::InitState_DataAvailable,HodgeGameplayTags::InitState_DataInitialized,HodgeGameplayTags::InitState_GameplayReady};
	// ContinueInitStateChain 不会从空状态进入 Spawned，必须显式建立初始事实。
	for (int32 Pass = 0; Pass < 4; ++Pass)
	{
		bRecheckInitialization = false; TryToChangeInitState(HodgeGameplayTags::InitState_Spawned);
		InitializeFromExtension(); ContinueInitStateChain(Chain);
		if (!bRecheckInitialization || bClosed) { break; }
	}
	SetComponentTickEnabled(IsReady());
}
void UHodgeLockOnComponent::InitializeFromExtension()
{
	if (bClosed || !Extension.IsValid()) { return; }
	auto* CurrentASC = Extension->GetHodgeAbilitySystemComponent();
	const auto* Data = Extension->GetPawnData<UHodgePawnData>();
	if (!Data || !Data->LockOnProfile) { if (bBound) { Uninitialize(); } return; }
	if (bBound && ASC == CurrentASC && Profile == Data->LockOnProfile) { return; }
	FString Error;
	if (!Data->LockOnProfile->Validate(Error))
	{
		if (Diagnostic != Error) { Diagnostic = Error; UE_LOG(LogTemp,Error,TEXT("LockOn %s: %s"),*GetPathName(),*Error); }
		return;
	}
	if (!CurrentASC || CurrentASC->GetAvatarActor() != GetOwner()) { Profile = Data->LockOnProfile; return; }
	Uninitialize(); bClosed = false; ++Generation; ASC = CurrentASC; Profile = Data->LockOnProfile;
	Rotation = GetOwner()->FindComponentByClass<UHodgeCharacterRotationComponent>(); bBound = true;
	const FGameplayTag Tags[] = {HodgeGameplayTags::Status_Death,HodgeHitReactionTags::Controlled,HodgeMovementTags::DashPreparing,HodgeMovementTags::Dashing,HodgeMovementTags::Sprinting};
	for (const FGameplayTag& Tag : Tags) { ConstraintHandles.Emplace(Tag,ASC->RegisterGameplayTagEvent(Tag).AddUObject(this,&ThisClass::HandleConstraint)); }
	SynchronizePresentation();
}
bool UHodgeLockOnComponent::IsReady() const
{ return !bClosed && bBound && Profile && ASC.IsValid() && ASC->GetAvatarActor() == GetOwner() && HasReachedInitState(HodgeGameplayTags::InitState_GameplayReady); }
void UHodgeLockOnComponent::Uninitialize()
{
	++Generation; bBound = false; SetComponentTickEnabled(false);
	if (Rotation.IsValid()) { Rotation->ReleaseFacingRequest(FacingHandle); } FacingHandle = {};
	if (ASC.IsValid())
	{
		for (auto& Pair : ConstraintHandles) { ASC->RegisterGameplayTagEvent(Pair.Key).Remove(Pair.Value); }
		if (GetOwner()->HasAuthority() && StateEffect.IsValid()) { ASC->RemoveActiveGameplayEffect(StateEffect); }
	}
	ConstraintHandles.Reset(); StateEffect.Invalidate();
	if (Indicator) { Indicator->UnregisterIndicator(); Indicator = nullptr; }
	if (ObservedTarget.IsValid()) { ObservedTarget->OnDestroyed.RemoveDynamic(this,&ThisClass::HandleTargetDestroyed); }
	if (ObservedHealth.IsValid()) { ObservedHealth->OnDeathStarted.RemoveDynamic(this,&ThisClass::HandleTargetDeath); }
	ObservedTarget.Reset(); ObservedHealth.Reset(); bHasPending = bHasQueued = bCommandActive = false;
	Gesture.Reset(); SoftCamera.Reset(); OutOfRangeAt = OccludedAt = ReferenceWaitAt = -1.;
	if (GetOwner() && GetOwner()->HasAuthority() && ConfirmedState.bHasTarget)
	{ PublishTarget(nullptr,NAME_None,ConfirmedState.RequestSequence,EHodgeLockOnReason::Uninitialized,true); }
	ASC.Reset(); Rotation.Reset(); Profile = nullptr;
}
void UHodgeLockOnComponent::EndPlay(EEndPlayReason::Type Reason)
{ bClosed = true; Uninitialize(); Super::EndPlay(Reason); }
void UHodgeLockOnComponent::OnUnregister()
{
	bClosed = true; Uninitialize();
	if (Extension.IsValid()) { Extension->UnregisterAbilitySystemDelegates(this); Extension->OnPawnDataChanged.RemoveAll(this); }
	UnregisterInitStateFeature(); Super::OnUnregister();
}
void UHodgeLockOnComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{ Super::GetLifetimeReplicatedProps(OutLifetimeProps); DOREPLIFETIME(ThisClass,ConfirmedState); }
AActor* UHodgeLockOnComponent::GetPresentationTarget() const
{
	const auto* Pawn = GetPawn<APawn>();
	if (Pawn && Pawn->IsLocallyControlled())
	{
		if (bHasQueued) { return Queued.CandidateActor.Get(); }
		if (bHasPending) { return Pending.CandidateActor.Get(); }
	}
	return IsValid(ConfirmedState.Target) ? ConfirmedState.Target.Get() : nullptr;
}
bool UHodgeLockOnComponent::GetPresentationLockPoint(FVector& OutLocation) const
{
	AActor* Target = GetPresentationTarget(); const auto* Pawn = GetPawn<APawn>();
	const FName Point = Pawn && Pawn->IsLocallyControlled() && bHasQueued ? Queued.LockPointId : Pawn && Pawn->IsLocallyControlled() && bHasPending ? Pending.LockPointId : ConfirmedState.LockPointId;
	return Target && Target->Implements<UHodgeLockOnTargetInterface>() && IHodgeLockOnTargetInterface::Execute_GetLockPoint(Target,Point,OutLocation) && !OutLocation.ContainsNaN();
}
bool UHodgeLockOnComponent::IsConfirmedTargetValid() const
{
	EHodgeLockOnReason Reason;
	return IsTargetValid(ConfirmedState.Target,ConfirmedState.LockPointId,false,Reason);
}
bool UHodgeLockOnComponent::HasSight(AActor* Target, FVector Point) const
{
	const auto* Pawn = GetPawn<APawn>(); if (!Pawn || !Profile || Point.ContainsNaN()) { return false; }
	FCollisionQueryParams Params(SCENE_QUERY_STAT(HodgeLockOnSight),false,Pawn); Params.AddIgnoredActor(Target);
	TArray<AActor*> Attached; Pawn->GetAttachedActors(Attached,true,true); Params.AddIgnoredActors(Attached);
	Target->GetAttachedActors(Attached,true,true); Params.AddIgnoredActors(Attached);
	FHitResult Hit;
	return !GetWorld()->LineTraceSingleByChannel(Hit,Pawn->GetPawnViewLocation(),Point,Profile->SightChannel,Params);
}
bool UHodgeLockOnComponent::IsTargetValid(AActor* Target, FName Point, bool bAcquiring, EHodgeLockOnReason& Reason) const
{
	Reason = EHodgeLockOnReason::InvalidTarget; const auto* Pawn = GetPawn<APawn>();
	if (!IsReady() || !Pawn || !IsValid(Target) || Target->IsActorBeingDestroyed() || !Target->Implements<UHodgeLockOnTargetInterface>() ||
		!IHodgeLockOnTargetInterface::Execute_CanBeLockedBy(Target,const_cast<APawn*>(Pawn)) || !FHodgeDamageRules::CanDamage(Pawn,Target,false)) { return false; }
	FVector Location;
	if (!IHodgeLockOnTargetInterface::Execute_GetLockPoint(Target,Point,Location) || Location.ContainsNaN()) { return false; }
	const float Distance = bAcquiring ? Profile->AcquireDistance : Profile->KeepDistance;
	if (FVector::DistSquared(Pawn->GetActorLocation(),Target->GetActorLocation()) > FMath::Square(Distance)) { Reason = EHodgeLockOnReason::OutOfRange; return false; }
	if (bAcquiring && !HasSight(Target,Location)) { Reason = EHodgeLockOnReason::Occluded; return false; }
	Reason = EHodgeLockOnReason::Accepted; return true;
}
AActor* UHodgeLockOnComponent::SelectCandidate(int32 Direction) const
{
	const auto* Pawn = GetPawn<APawn>(); auto* PC = Pawn ? Pawn->GetController<APlayerController>() : nullptr;
	if (!IsReady() || !PC || !PC->IsLocalController() || !PC->PlayerCameraManager) { return nullptr; }
	int32 Width = 0, Height = 0; PC->GetViewportSize(Width,Height); if (Width <= 0 || Height <= 0) { return nullptr; }
	FVector ViewLocation; FRotator ViewRotation; PC->GetPlayerViewPoint(ViewLocation,ViewRotation);
	const FVector2D Center(Width*.5f,Height*.5f); FVector2D CurrentScreen = Center;
	FVector CurrentPoint; if (Direction && GetPresentationLockPoint(CurrentPoint)) { PC->ProjectWorldLocationToScreen(CurrentPoint,CurrentScreen); }
	TArray<FOverlapResult> Hits; FCollisionObjectQueryParams Objects;
	Objects.AddObjectTypesToQuery(ECC_Pawn); Objects.AddObjectTypesToQuery(ECC_WorldDynamic);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(HodgeLockOnCandidates),false,Pawn);
	GetWorld()->OverlapMultiByObjectType(Hits,Pawn->GetActorLocation(),FQuat::Identity,Objects,FCollisionShape::MakeSphere(Profile->AcquireDistance),Params);
	TSet<AActor*> Visited; AActor* Best = nullptr; double BestScore = -DBL_MAX; FString BestPath;
	const FVector Weights = Profile->ScoreWeights / (Profile->ScoreWeights.X+Profile->ScoreWeights.Y+Profile->ScoreWeights.Z);
	for (const auto& Hit : Hits)
	{
		AActor* Target = Hit.GetActor(); if (!Target || Visited.Contains(Target) || (Direction && Target == GetPresentationTarget())) { continue; } Visited.Add(Target);
		EHodgeLockOnReason Reason;
		if (!IsTargetValid(Target,NAME_None,true,Reason))
		{ UE_LOG(LogTemp,Verbose,TEXT("LockOn candidate rejected: source=%s target=%s reason=%d"),*GetNameSafe(Pawn),*GetNameSafe(Target),int32(Reason)); continue; }
		FVector Point; IHodgeLockOnTargetInterface::Execute_GetLockPoint(Target,NAME_None,Point);
		if (FVector::DotProduct(ViewRotation.Vector(),(Point-ViewLocation).GetSafeNormal()) < FMath::Cos(FMath::DegreesToRadians(Profile->AcquireHalfAngle))) { continue; }
		FVector2D Screen;
		// 获取必须在屏幕内；自由环绕后的方向切换允许前方视锥内的屏幕外邻居。
		if (!PC->ProjectWorldLocationToScreen(Point,Screen) || (!Direction && (Screen.X < 0 || Screen.X > Width || Screen.Y < 0 || Screen.Y > Height))) { continue; }
		if (Direction && (Screen.X-CurrentScreen.X)*Direction <= 1.f) { continue; }
		const double ScreenScore = 1.-FMath::Clamp((Screen-Center).Size()/Center.Size(),0.,1.);
		const double DistanceScore = 1.-FMath::Clamp(FVector::Dist(Pawn->GetActorLocation(),Target->GetActorLocation())/Profile->AcquireDistance,0.,1.);
		const float Priority = IHodgeLockOnTargetInterface::Execute_GetLockPriority(Target);
		const double Score = ScreenScore*Weights.X+DistanceScore*Weights.Y+(FMath::IsFinite(Priority)?FMath::Clamp(Priority,0.f,1.f):0.f)*Weights.Z;
		UE_LOG(LogTemp,Verbose,TEXT("LockOn candidate score: source=%s target=%s screen=%.3f distance=%.3f priority=%.3f total=%.3f direction=%d"),
			*GetNameSafe(Pawn),*GetNameSafe(Target),ScreenScore,DistanceScore,Priority,Score,Direction);
		const FString Path = Target->GetPathName();
		if (Score > BestScore + UE_DOUBLE_SMALL_NUMBER || (FMath::IsNearlyEqual(Score,BestScore) && Path < BestPath)) { Best = Target; BestScore = Score; BestPath = Path; }
	}
	return Best;
}
bool UHodgeLockOnComponent::ToggleLock()
{
	if (GetPresentationTarget() || (bHasPending && Pending.Operation != EHodgeLockOnOperation::Clear)) { return SubmitCommand(EHodgeLockOnOperation::Clear,nullptr,NAME_None); }
	return TryLockTarget(SelectCandidate(0));
}
bool UHodgeLockOnComponent::TryLockTarget(AActor* Target, FName Point)
{
	EHodgeLockOnReason Reason; if (!IsTargetValid(Target,Point,true,Reason)) { LastLocalReason = Reason; return false; }
	return SubmitCommand(EHodgeLockOnOperation::Acquire,Target,Point);
}
bool UHodgeLockOnComponent::SwitchTarget(int32 Direction)
{
	if (!GetPresentationTarget() || Direction == 0) { return false; }
	AActor* Next = SelectCandidate(Direction > 0 ? 1 : -1); return Next && SubmitCommand(EHodgeLockOnOperation::Switch,Next,NAME_None);
}
void UHodgeLockOnComponent::ClearLock(EHodgeLockOnReason Reason)
{
	if (GetOwner()->HasAuthority() && Reason != EHodgeLockOnReason::UserClear)
	{ bHasPending = bHasQueued = false; PublishTarget(nullptr,NAME_None,ConfirmedState.RequestSequence,Reason,true); return; }
	if (Reason == EHodgeLockOnReason::UserClear) { SubmitCommand(EHodgeLockOnOperation::Clear,nullptr,NAME_None); }
	else { bHasPending = bHasQueued = false; LastLocalReason = Reason; SynchronizePresentation(); }
}
bool UHodgeLockOnComponent::SubmitCommand(EHodgeLockOnOperation Operation, AActor* Target, FName Point)
{
	const auto* Pawn = GetPawn<APawn>(); auto* PC = Pawn ? Pawn->GetController<APlayerController>() : nullptr;
	if (!IsReady() || !PC || !PC->IsLocalController() || !UHodgeUIManagerSubsystem::AllowsGameplayInput(PC) || ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death))
	{ LastLocalReason = EHodgeLockOnReason::NotReady; return false; }
	FHodgeLockOnTargetData Intent; Intent.Operation = Operation; Intent.CandidateActor = Target; Intent.LockPointId = Point; Intent.SourceAvatar = GetOwner();
	Queued = Intent; bHasQueued = true; SynchronizePresentation(); DispatchQueuedCommand();
	return bHasQueued || bHasPending || (ConfirmedState.bAccepted && LastLocalReason == ConfirmedState.Reason);
}
void UHodgeLockOnComponent::DispatchQueuedCommand()
{
	if (!bHasQueued || bCommandActive || bHasPending || !IsReady()) { return; }
	const FGameplayAbilitySpec* Spec = nullptr;
	for (const auto& Candidate : ASC->GetActivatableAbilities())
	{
		if (!Candidate.Ability || !Candidate.Ability->IsA<UHodgeGameplayAbility_LockOn>()) { continue; }
		if (Spec) { bHasQueued = false; LastLocalReason = EHodgeLockOnReason::ActivationFailed; SynchronizePresentation(); return; }
		Spec = &Candidate;
	}
	if (!Spec || Spec->IsActive())
	{ if (!Spec) { bHasQueued = false; LastLocalReason = EHodgeLockOnReason::ActivationFailed; SynchronizePresentation(); } return; }
	bCommandActive = true;
	if (!ASC->TryActivateAbility(Spec->Handle))
	{ bCommandActive = false; bHasQueued = false; LastLocalReason = EHodgeLockOnReason::ActivationFailed; SynchronizePresentation(); }
}
bool UHodgeLockOnComponent::TakeCommand(FHodgeLockOnTargetData& Out)
{
	if (!bHasQueued || !IsReady() || NextSequence == MAX_int32) { return false; }
	Out = Queued; Out.RequestSequence = ++NextSequence; Out.BaseStateVersion = ConfirmedState.StateVersion;
	Pending = Out; bHasPending = true; bHasQueued = false; PendingAt = GetWorld()->GetTimeSeconds(); SynchronizePresentation(); return true;
}
void UHodgeLockOnComponent::ProcessAuthorityCommand(const FHodgeLockOnTargetData& Data)
{
	if (!GetOwner()->HasAuthority()) { return; }
	const auto* Pawn = GetPawn<APawn>();
	if (!IsReady() || !Pawn || !Pawn->GetController() || Pawn->GetController()->GetPawn() != Pawn || Data.SourceAvatar.Get() != Pawn)
	{ LastLocalReason = EHodgeLockOnReason::InvalidSource; return; }
	if (Data.RequestSequence <= ConfirmedState.RequestSequence || Data.RequestSequence <= 0)
	{ ClientCommandResult(ConfirmedState); return; }
	EHodgeLockOnReason Reason = EHodgeLockOnReason::Accepted; bool bAccepted = true;
	if (static_cast<uint8>(Data.Operation) > static_cast<uint8>(EHodgeLockOnOperation::Clear) || Data.LockPointId.ToString().Len() > NAME_SIZE / 2)
	{ bAccepted = false; Reason = EHodgeLockOnReason::InvalidTarget; }
	else if (Data.Operation != EHodgeLockOnOperation::Clear && ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death)) { bAccepted = false; Reason = EHodgeLockOnReason::Death; }
	else if (Data.Operation != EHodgeLockOnOperation::Clear && Data.BaseStateVersion != ConfirmedState.StateVersion) { bAccepted = false; Reason = EHodgeLockOnReason::StaleVersion; }
	else if (Data.Operation == EHodgeLockOnOperation::Switch && !ConfirmedState.Target) { bAccepted = false; Reason = EHodgeLockOnReason::InvalidTarget; }
	else if (Data.Operation != EHodgeLockOnOperation::Clear) { bAccepted = IsTargetValid(Data.CandidateActor.Get(),Data.LockPointId,true,Reason); }
	if (bAccepted && Data.Operation == EHodgeLockOnOperation::Clear) { Reason = EHodgeLockOnReason::UserClear; }
	PublishTarget(bAccepted && Data.Operation == EHodgeLockOnOperation::Clear ? nullptr : bAccepted ? Data.CandidateActor.Get() : ConfirmedState.Target.Get(),
		bAccepted && Data.Operation == EHodgeLockOnOperation::Clear ? NAME_None : bAccepted ? Data.LockPointId : ConfirmedState.LockPointId,Data.RequestSequence,Reason,bAccepted);
	ClientCommandResult(ConfirmedState);
}
void UHodgeLockOnComponent::PublishTarget(AActor* Target, FName Point, int32 Sequence, EHodgeLockOnReason Reason, bool bAccepted)
{
	ConfirmedState.Target = Target; ConfirmedState.bHasTarget = Target != nullptr; ConfirmedState.LockPointId = Point;
	ConfirmedState.RequestSequence = Sequence; ++ConfirmedState.StateVersion; ConfirmedState.Reason = Reason; ConfirmedState.bAccepted = bAccepted;
	if (ASC.IsValid() && GetOwner()->HasAuthority())
	{
		if (Target && !StateEffect.IsValid())
		{ FGameplayEffectContextHandle Context = ASC->MakeEffectContext(); Context.AddSourceObject(this); StateEffect = ASC->ApplyGameplayEffectToSelf(GetDefault<UHodgeLockOnStateEffect>(),1.f,Context); }
		else if (!Target && StateEffect.IsValid()) { ASC->RemoveActiveGameplayEffect(StateEffect); StateEffect.Invalidate(); }
	}
	if (GetPawn<APawn>()->IsLocallyControlled()) { ApplyResult(ConfirmedState); } else { SynchronizePresentation(); }
	GetOwner()->ForceNetUpdate();
	UE_LOG(LogTemp,Log,TEXT("LockOn [%s] seq=%d version=%d accepted=%d target=%s reason=%d"),*GetNameSafe(GetOwner()),Sequence,ConfirmedState.StateVersion,bAccepted,*GetNameSafe(Target),int32(Reason));
}
void UHodgeLockOnComponent::ClientCommandResult_Implementation(FHodgeLockOnState State) { ApplyResult(State); }
void UHodgeLockOnComponent::OnRep_ConfirmedState() { ApplyResult(ConfirmedState); }
void UHodgeLockOnComponent::ApplyResult(const FHodgeLockOnState& State)
{
	if (State.StateVersion < ConfirmedState.StateVersion) { return; }
	ConfirmedState = State;
	// 尚未映射的目标引用不立即按解除处理，复制到达后再消费回执。
	if (State.bHasTarget && !State.Target)
	{ if (ReferenceWaitAt < 0.) { ReferenceWaitAt = GetWorld()->GetTimeSeconds(); } return; }
	ReferenceWaitAt = -1.;
	if (bHasPending && State.RequestSequence >= Pending.RequestSequence) { bHasPending = false; LastLocalReason = State.Reason; }
	SynchronizePresentation();
}
void UHodgeLockOnComponent::NotifyCommandEnded(int32 Sequence, bool bCancelled)
{
	if (Sequence && bHasPending && Pending.RequestSequence != Sequence) { return; }
	bCommandActive = false;
	if (bCancelled && bHasPending)
	{
		// Timer 与组件 Tick 在同一截止帧执行时，也保持一致的超时原因。
		LastLocalReason = Profile && GetWorld()->GetTimeSeconds()-PendingAt >= Profile->CommandTimeout ? EHodgeLockOnReason::Timeout : EHodgeLockOnReason::ActivationFailed;
		bHasPending = false;
	}
	SynchronizePresentation();
}
void UHodgeLockOnComponent::RejectPrediction(int32 Sequence, int32 InGeneration)
{
	if (Generation != InGeneration || !bHasPending || Pending.RequestSequence != Sequence) { return; }
	bHasPending = false; bCommandActive = false; LastLocalReason = EHodgeLockOnReason::ActivationFailed; SynchronizePresentation();
}
bool UHodgeLockOnComponent::CanOwnControllerFacing() const
{
	const auto* Pawn = GetPawn<APawn>();
	if (!Pawn || (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) || !IsReady() || !Rotation.IsValid() || !GetPresentationTarget() || ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) ||
		ASC->HasMatchingGameplayTag(HodgeHitReactionTags::Controlled) || ASC->HasActiveAbilityTag(HodgeGameplayTags::Ability_Type_Action_Dash) ||
		ASC->HasActiveAbilityTag(HodgeMovementTags::SprintAbility) || ASC->HasMatchingGameplayTag(HodgeMovementTags::DashPreparing) ||
		ASC->HasMatchingGameplayTag(HodgeMovementTags::Dashing) || ASC->HasMatchingGameplayTag(HodgeMovementTags::Sprinting)) { return false; }
	return !GetOwner()->HasAuthority() || ConfirmedState.Target == GetPresentationTarget();
}
void UHodgeLockOnComponent::RefreshFacing()
{
	if (!Rotation.IsValid()) { return; }
	if (FacingHandle.IsValid() && (!CanOwnControllerFacing() || Rotation->GetFacingRequestStatus(FacingHandle) == EHodgeFacingRequestStatus::Invalid))
	{ Rotation->ReleaseFacingRequest(FacingHandle); FacingHandle = {}; }
	if (!FacingHandle.IsValid() && CanOwnControllerFacing()) { FacingHandle = Rotation->AcquireLockOnFacing(this); }
}
bool UHodgeLockOnComponent::ResolveControlRotation(FRotator& InOut, float Delta) const
{
	if (!CanOwnControllerFacing() || Rotation->GetResolvedState().BaseDriver != EHodgeCharacterFacingDriver::Controller) { return false; }
	FVector Point; if (!GetPresentationLockPoint(Point)) { return false; }
	const FVector Direction = (Point-GetOwner()->GetActorLocation()).GetSafeNormal2D(); if (Direction.IsNearlyZero()) { return false; }
	InOut.Yaw = Direction.Rotation().Yaw; InOut.Roll = 0.f;
	InOut = Rotation->FilterControlRotation(InOut,Delta); return true;
}
void UHodgeLockOnComponent::SeedCameraView(FRotator CurrentView)
{
	SoftCamera.Seed(CurrentView, GetWorld()->GetTimeSeconds());
}
FRotator UHodgeLockOnComponent::UpdateCameraView(const FHodgeLockCameraFrame& Frame, float DeltaSeconds, bool bAllowAssist)
{
	const auto* Pawn = GetPawn<APawn>(); const auto* PC = Pawn ? Pawn->GetController<APlayerController>() : nullptr;
	const bool bGameplayInput = PC && !PC->IsLookInputIgnored() && UHodgeUIManagerSubsystem::AllowsGameplayInput(PC);
	return Profile ? SoftCamera.Update(Frame, DeltaSeconds, GetWorld()->GetTimeSeconds(), *Profile, bAllowAssist && bGameplayInput && GetPresentationTarget()) : SoftCamera.GetRotation();
}
bool UHodgeLockOnComponent::SubmitLookIntent(FVector2D Degrees, bool bAllowTargetSwitch)
{
	const auto* Pawn = GetPawn<APawn>(); auto* PC = Pawn ? Pawn->GetController<APlayerController>() : nullptr;
	if (!IsReady() || !GetPresentationTarget() || !PC || PC->IsLookInputIgnored() || !UHodgeUIManagerSubsystem::AllowsGameplayInput(PC) || Degrees.ContainsNaN()) { return false; }
	const double Now = GetWorld()->GetTimeSeconds();
	// 目标切换与视角输入共享采样，但切换不能吞掉玩家的环绕角位移。
	SoftCamera.AddLook(Degrees, Now);
	const int32 Direction = bAllowTargetSwitch && Profile->bEnableMouseFlickSwitch ? Gesture.Sample(Degrees,Now,*Profile) : 0;
	if (Direction) { SwitchTarget(Direction); }
	return true;
}
void UHodgeLockOnComponent::SynchronizePresentation()
{
	AActor* Target = GetPresentationTarget();
	if (ObservedTarget.Get() != Target)
	{
		if (ObservedTarget.IsValid()) { ObservedTarget->OnDestroyed.RemoveDynamic(this,&ThisClass::HandleTargetDestroyed); }
		if (ObservedHealth.IsValid()) { ObservedHealth->OnDeathStarted.RemoveDynamic(this,&ThisClass::HandleTargetDeath); }
		if (Indicator) { Indicator->UnregisterIndicator(); Indicator = nullptr; }
		// 切换目标保留手势防连发，只有进入或退出锁定会话才重置。
		if (!Target || !ObservedTarget.IsValid()) { Gesture.Reset(); }
		ObservedTarget = Target; ObservedHealth.Reset(); OutOfRangeAt = OccludedAt = -1.;
		if (Target)
		{
			Target->OnDestroyed.AddUniqueDynamic(this,&ThisClass::HandleTargetDestroyed);
			ObservedHealth = UHodgeHealthComponent::FindHealthComponent(Target);
			if (ObservedHealth.IsValid()) { ObservedHealth->OnDeathStarted.AddUniqueDynamic(this,&ThisClass::HandleTargetDeath); }
		}
	}
	const auto* Pawn = GetPawn<APawn>(); auto* PC = Pawn ? Pawn->GetController<APlayerController>() : nullptr;
	if (Target && Profile && PC && PC->IsLocalController() && !Indicator)
	{
		if (auto* Manager = PC->FindComponentByClass<UHodgeIndicatorManagerComponent>())
		{
			Indicator = NewObject<UIndicatorDescriptor>(this); Indicator->SetSceneComponent(Target->GetRootComponent());
			Indicator->SetIndicatorClass(Profile->IndicatorClass); Indicator->SetDataObject(this); Indicator->SetClampToScreen(true); Manager->AddIndicator(Indicator);
		}
	}
	RefreshFacing();
	const bool bPendingNow = IsCommandPending();
	if (LastBroadcastTarget.Get() != Target || LastBroadcastVersion != ConfirmedState.StateVersion || bLastBroadcastPending != bPendingNow || LastBroadcastReason != LastLocalReason)
	{
		LastBroadcastTarget = Target; LastBroadcastVersion = ConfirmedState.StateVersion;
		bLastBroadcastPending = bPendingNow; LastBroadcastReason = LastLocalReason;
		OnLockStateChanged.Broadcast(ConfirmedState,Target,++PresentationRevision);
	}
}
void UHodgeLockOnComponent::HandleConstraint(FGameplayTag Tag, int32 Count)
{ if (Tag == HodgeGameplayTags::Status_Death && Count > 0) { ClearLock(EHodgeLockOnReason::Death); } else { RefreshFacing(); } }
void UHodgeLockOnComponent::HandleTargetDeath(AActor* Actor)
{ if (Actor == GetPresentationTarget() || Actor == ConfirmedState.Target) { ClearLock(EHodgeLockOnReason::Death); } }
void UHodgeLockOnComponent::HandleTargetDestroyed(AActor* Actor)
{ if (Actor == ObservedTarget.Get() || Actor == ConfirmedState.Target) { ClearLock(EHodgeLockOnReason::Destroyed); } }
void UHodgeLockOnComponent::CheckTargetValidity(double Now)
{
	if (!GetOwner()->HasAuthority() || !ConfirmedState.bHasTarget) { return; }
	AActor* Target = ConfirmedState.Target; EHodgeLockOnReason Reason;
	const bool bValid = IsTargetValid(Target,ConfirmedState.LockPointId,false,Reason);
	if (!bValid && Reason != EHodgeLockOnReason::OutOfRange) { ClearLock(EHodgeLockOnReason::InvalidTarget); return; }
	if (Reason == EHodgeLockOnReason::OutOfRange)
	{
		if (OutOfRangeAt < 0.) { OutOfRangeAt = Now; }
		if (Now-OutOfRangeAt >= Profile->DistanceGrace) { ClearLock(Reason); return; }
	}
	else { OutOfRangeAt = -1.; }
	FVector Point;
	if (!IHodgeLockOnTargetInterface::Execute_GetLockPoint(Target,ConfirmedState.LockPointId,Point)) { ClearLock(EHodgeLockOnReason::InvalidTarget); return; }
	if (!HasSight(Target,Point))
	{
		if (OccludedAt < 0.) { OccludedAt = Now; }
		if (Now-OccludedAt >= Profile->OcclusionGrace) { ClearLock(EHodgeLockOnReason::Occluded); }
	}
	else { OccludedAt = -1.; }
}
void UHodgeLockOnComponent::TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Function)
{
	Super::TickComponent(Delta,Type,Function);
	if (!IsReady()) { if (ASC.IsValid() && ASC->GetAvatarActor() != GetOwner()) { Uninitialize(); } else { CheckDefaultInitialization(); } return; }
	// 模拟代理通常没有 PlayerController，不能因此撤销其表现初始化。
	if ((GetOwner()->HasAuthority() || GetOwner()->GetLocalRole() == ROLE_AutonomousProxy) && !GetPawn<APawn>()->GetController()) { Uninitialize(); return; }
	const double Now = GetWorld()->GetTimeSeconds();
	if (bHasPending && Now-PendingAt > Profile->CommandTimeout)
	{ bHasPending = false; bCommandActive = false; LastLocalReason = EHodgeLockOnReason::Timeout; SynchronizePresentation(); }
	if (ReferenceWaitAt >= 0.)
	{
		if (ConfirmedState.Target) { ApplyResult(ConfirmedState); }
		else if (Now-ReferenceWaitAt >= Profile->ReferenceTimeout)
		{ ReferenceWaitAt = -1.; if (bHasPending && ConfirmedState.RequestSequence >= Pending.RequestSequence) { bHasPending = false; } LastLocalReason = EHodgeLockOnReason::Timeout; SynchronizePresentation(); }
	}
	const auto* PC = GetPawn<APawn>()->GetController<APlayerController>();
	if (PC && PC->IsLocalController())
	{
		if (!UHodgeUIManagerSubsystem::AllowsGameplayInput(PC)) { Gesture.Reset(); }
		else if (GetPresentationTarget())
		{
			Gesture.Sample(FVector2D::ZeroVector,Now,*Profile);
		}
	}
	if (Now >= NextCheckAt) { NextCheckAt = Now+Profile->KeepCheckInterval; CheckTargetValidity(Now); }
	if (Indicator)
	{
		FVector Point; const bool bValid = GetPresentationLockPoint(Point); Indicator->SetDesiredVisibility(bValid);
		if (bValid) { Indicator->SetWorldPositionOffset(Point-GetPresentationTarget()->GetActorLocation()); }
	}
	RefreshFacing(); DispatchQueuedCommand();
}
