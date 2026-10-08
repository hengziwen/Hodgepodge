#include "Component/HodgeCombatComponentBase.h"
#include "Equipment/HodgeEquipmentManagerComponent.h"
#include "Equipment/HodgeWeaponInstance.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Component/HodgeHeroComponent.h"
#include "Data/HodgeComboDefinition.h"
#include "Data/HodgeAbilityDefinition.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Data/HodgePawnData.h"

#include "TimerManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCombatComponentBase)

namespace
{
	USceneComponent* FindSourceComponent(AActor* Actor, FName Name)
	{
		if (!Actor) { return nullptr; }
		if (Name.IsNone())
		{
			if (ACharacter* Character = Cast<ACharacter>(Actor)) { return Character->GetMesh(); }
			return Actor->GetRootComponent();
		}
		TInlineComponentArray<USceneComponent*> Components(Actor);
		for (USceneComponent* Component : Components)
		{
			if (Component->GetFName() == Name) { return Component; }
		}
		return nullptr;
	}
}

UHodgeCombatComponentBase::UHodgeCombatComponentBase()
{
	// 组件 Tick 只推进连招缓存，几何采样仍由技能 Task 独立调度。
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

FGuid UHodgeCombatComponentBase::AcquirePoseLease()
{
	if (!CanExecuteAbilities() || !GetOwner() || !GetOwner()->HasAuthority()) { return {}; }
	const auto* Character = Cast<ACharacter>(GetOwner());
	if (!Character || !Character->GetMesh()) { return {}; }
	if (PoseLeases.IsEmpty())
	{
		PoseMesh = Character->GetMesh();
		SavedPosePolicy = static_cast<uint8>(PoseMesh->VisibilityBasedAnimTickOption);
		PoseMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	}
	const FGuid Handle = FGuid::NewGuid();
	PoseLeases.Add(Handle);
	return Handle;
}

void UHodgeCombatComponentBase::ReleasePoseLease(FGuid Handle)
{
	if (!PoseLeases.Remove(Handle) || !PoseLeases.IsEmpty()) { return; }
	if (PoseMesh.IsValid() && PoseMesh->VisibilityBasedAnimTickOption ==
		EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones)
	{
		PoseMesh->VisibilityBasedAnimTickOption = static_cast<EVisibilityBasedAnimTickOption>(SavedPosePolicy);
	}
	PoseMesh.Reset();
}

void UHodgeCombatComponentBase::EndPlay(const EEndPlayReason::Type Reason)
{
	Shutdown();
	TArray<FGuid> Leases = PoseLeases.Array();
	for (FGuid Lease : Leases) { ReleasePoseLease(Lease); }
	Super::EndPlay(Reason);
}

bool UHodgeCombatComponentBase::ResolveSource(FGameplayTag Tag, FHodgeHitDetectionSession& Session) const
{
	int32 Matches = 0;
	for (const FHodgeHitSource& Source : HitSources)
	{
		if (Source.SourceTag != Tag) { continue; }
		++Matches;
		Session.Source = Source;
		Session.SourceComponent = FindSourceComponent(GetOwner(), Source.ComponentName);
	}
	if (auto* Equipment = GetOwner()->FindComponentByClass<UHodgeEquipmentManagerComponent>())
	{
		for (UHodgeEquipmentInstance* Instance : Equipment->GetEquipmentInstancesOfType(
			     UHodgeWeaponInstance::StaticClass()))
		{
			UHodgeWeaponInstance* Weapon = CastChecked<UHodgeWeaponInstance>(Instance);
			for (const FHodgeHitSource& Source : Weapon->HitSources)
			{
				if (Source.SourceTag != Tag) { continue; }
				++Matches;
				Session.Source = Source;
				Session.Weapon = Weapon;
				Session.bWeaponSource = true;
				const TArray<AActor*> Actors = Weapon->GetSpawnedActors();
				Session.SourceComponent = Actors.IsValidIndex(Source.WeaponActorIndex)
					                          ? FindSourceComponent(Actors[Source.WeaponActorIndex],
					                                                Source.ComponentName)
					                          : nullptr;
			}
		}
	}
	// 同一来源标签必须唯一，配置冲突时不猜测使用哪一把武器。
	return Matches == 1 && Session.SourceComponent.IsValid();
}

bool UHodgeCombatComponentBase::IsSourceValid(const FHodgeHitDetectionSession& Session) const
{
	if (!IsValid(GetOwner())) { return false; }
	if (Session.Request.TargetPolicy != EHodgeHitTargetPolicy::AnyInVolume ||
		Session.Request.Volume.AnchorKind == EHodgeHitAnchorKind::ExecutionTarget)
	{
		AActor* Target = Session.Request.RuntimeTarget.Get();
		if (!IsValid(Target) || Target->GetWorld() != GetWorld() ||
			FVector::DistSquared(GetOwner()->GetActorLocation(), Target->GetActorLocation()) > FMath::Square(
				Session.Request.MaxTargetDistance)) { return false; }
	}
	if (Session.Request.TargetPolicy == EHodgeHitTargetPolicy::ConfirmedTarget) { return true; }
	if (Session.Request.Volume.GeometryMode == EHodgeHitGeometryMode::ConfiguredShape &&
		Session.Request.Volume.AnchorKind != EHodgeHitAnchorKind::RegisteredSource)
	{
		return Session.Request.Volume.AnchorKind != EHodgeHitAnchorKind::ExecutionTransform || Session.Request.
			bHasRuntimeAnchor;
	}
	if (!Session.SourceComponent.IsValid()) { return false; }
	if (Session.bWeaponSource)
	{
		const auto* Equipment = GetOwner()->FindComponentByClass<UHodgeEquipmentManagerComponent>();
		return Session.Weapon.IsValid() && Equipment &&
			Equipment->GetEquipmentInstancesOfType(UHodgeWeaponInstance::StaticClass()).Contains(Session.Weapon.Get());
	}
	return Session.SourceComponent->GetOwner() == GetOwner();
}

bool UHodgeCombatComponentBase::CaptureDetectionGeometry(const FHodgeHitDetectionSession& Session,
                                                         FHodgeHitGeometry& Out) const
{
	if (!IsSourceValid(Session)) { return false; }
	const auto& Request = Session.Request;
	if (Request.TargetPolicy == EHodgeHitTargetPolicy::ConfirmedTarget)
	{
		Out = FHodgeHitGeometry();
		Out.Transform = GetOwner()->GetActorTransform();
		return true;
	}
	const auto& Volume = Request.Volume;
	if (Volume.GeometryMode == EHodgeHitGeometryMode::ExistingSource)
	{
		return Session.Strategy.GetDefaultObject()->Capture(Session.SourceComponent.Get(), Session.Source, Out);
	}
	FTransform Anchor;
	if (Session.bHasFixedAnchor) { Anchor = Session.FixedAnchor; }
	else if (Volume.AnchorKind == EHodgeHitAnchorKind::AvatarRoot) { Anchor = GetOwner()->GetActorTransform(); }
	else if (Volume.AnchorKind == EHodgeHitAnchorKind::ExecutionTransform) { Anchor = Request.RuntimeAnchor; }
	else if (Volume.AnchorKind == EHodgeHitAnchorKind::ExecutionTarget)
	{
		Anchor = Request.RuntimeTarget->GetActorTransform();
	}
	else
	{
		USceneComponent* Component = Session.SourceComponent.Get();
		if (!Volume.AnchorSocket.IsNone() && !Component->DoesSocketExist(Volume.AnchorSocket)) { return false; }
		Anchor = Volume.AnchorSocket.IsNone()
			         ? Component->GetComponentTransform()
			         : Component->GetSocketTransform(Volume.AnchorSocket);
	}
	if (Anchor.ContainsNaN() || !Anchor.GetRotation().IsNormalized() ||
		FVector::DistSquared(GetOwner()->GetActorLocation(), Anchor.GetLocation()) > FMath::Square(
			Volume.MaxAnchorDistance)) { return false; }
	Anchor.SetScale3D(FVector::OneVector);
	Out = FHodgeHitGeometry();
	Out.Transform = Volume.LocalTransform * Anchor;
	Out.Shape = Volume.Shape;
	Out.Radius = Volume.Shape == EHodgeHitShape::Capsule ? Volume.CapsuleRadius : Volume.SphereRadius;
	Out.HalfHeight = Volume.CapsuleHalfHeight;
	Out.BoxExtent = Volume.BoxHalfExtent;
	return !Out.Transform.ContainsNaN();
}

uint64 UHodgeCombatComponentBase::CreateDetectionSession(const FGuid& ExecutionId, int32 OccurrenceId,
                                                         const FHodgeHitDetectionRequest& Request)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority() || !ExecutionId.IsValid() || OccurrenceId < 0 ||
		!Request.Profile) { return 0; }
	for (const auto& Pair : Sessions)
	{
		if (Pair.Value.ExecutionId == ExecutionId && Pair.Value.OccurrenceId == OccurrenceId) { return Pair.Key; }
	}
	TArray<FText> Errors;
	if (!Request.Profile->Validate(Errors) || !Request.Volume.Validate(Errors) ||
		!FMath::IsFinite(Request.MaxTargetDistance) || Request.MaxTargetDistance <= 0.f)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid detection profile [%s] on [%s]."),
		       *GetNameSafe(Request.Profile), *GetNameSafe(GetOwner()));
		return 0;
	}
	FHodgeHitDetectionSession Session;
	Session.ExecutionId = ExecutionId;
	Session.OccurrenceId = OccurrenceId;
	Session.Request = Request;
	const bool bComponentSource = Request.TargetPolicy != EHodgeHitTargetPolicy::ConfirmedTarget &&
	(Request.Volume.GeometryMode == EHodgeHitGeometryMode::ExistingSource || Request.Volume.AnchorKind ==
		EHodgeHitAnchorKind::RegisteredSource);
	if (bComponentSource)
	{
		if (Request.DirectComponent.IsValid())
		{
			Session.SourceComponent = Request.DirectComponent;
			Session.Source = Request.DirectSource;
		}
		else if (!Request.SourceTag.IsValid() || !ResolveSource(Request.SourceTag, Session)) { return 0; }
	}
	Session.Strategy = Request.Volume.GeometryMode == EHodgeHitGeometryMode::ConfiguredShape
		                   ? UHodgeShapeQueryStrategy::StaticClass()
		                   : (Session.SourceComponent.IsValid() && Session.SourceComponent->IsA<UBoxComponent>()
			                      ? UHodgeBoxSweepStrategy::StaticClass()
			                      : UHodgeSocketSweepStrategy::StaticClass());
	if (!CaptureDetectionGeometry(Session, Session.Previous))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot resolve notification hit geometry on [%s]."), *GetNameSafe(GetOwner()));
		return 0;
	}

	if (Request.Volume.GeometryMode == EHodgeHitGeometryMode::ConfiguredShape && Request.Volume.TransformPolicy ==
		EHodgeHitTransformPolicy::SnapshotOnEventEnter)
	{
		// 存储锚点本身，后续 Capture 仍只施加一次局部变换。
		Session.FixedAnchor = Request.Volume.LocalTransform.Inverse() * Session.Previous.Transform;
		Session.bHasFixedAnchor = true;
	}
	// 创建会话不广播结果，调用方登记句柄和回调后再首次采样。
	if (NextHandle == MAX_uint64) { return 0; }
	const uint64 Handle = ++NextHandle;
	Sessions.Add(Handle, MoveTemp(Session));
	return Handle;
}

bool UHodgeCombatComponentBase::IsDetectionSessionValid(uint64 Handle, const FGuid& ExecutionId) const
{
	const FHodgeHitDetectionSession* Session = Sessions.Find(Handle);
	return Session && Session->ExecutionId == ExecutionId && IsSourceValid(*Session);
}

bool UHodgeCombatComponentBase::ResetDetectionHistory(uint64 Handle, const FGuid& ExecutionId)
{
	FHodgeHitDetectionSession* Session = Sessions.Find(Handle);
	if (!Session || Session->ExecutionId != ExecutionId) { return false; }
	if (!CaptureDetectionGeometry(*Session, Session->Previous))
	{
		Sessions.Remove(Handle);
		return false;
	}
	return true;
}

USceneComponent* UHodgeCombatComponentBase::GetDetectionSourceComponent(uint64 Handle, const FGuid& ExecutionId) const
{
	const FHodgeHitDetectionSession* Session = Sessions.Find(Handle);
	return Session && Session->ExecutionId == ExecutionId && IsSourceValid(*Session)
		       ? Session->SourceComponent.Get()
		       : nullptr;
}

void UHodgeCombatComponentBase::EndDetectionSession(uint64 Handle, const FGuid& ExecutionId)
{
	const FHodgeHitDetectionSession* Session = Sessions.Find(Handle);
	if (Session && Session->ExecutionId == ExecutionId) { Sessions.Remove(Handle); }
}

void UHodgeCombatComponentBase::EndDetectionSessionsForExecution(const FGuid& ExecutionId)
{
	for (auto It = Sessions.CreateIterator(); It; ++It)
	{
		if (It.Value().ExecutionId == ExecutionId) { It.RemoveCurrent(); }
	}
}

void UHodgeCombatComponentBase::EndAllDetectionSessions()
{
	Sessions.Reset();
}

void UHodgeCombatComponentBase::UpdateDetectionAnchor(const FGuid& ExecutionId, FName Key, const FTransform& Transform)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || Transform.ContainsNaN() || !Transform.GetRotation().
		IsNormalized()) { return; }
	for (auto& Pair : Sessions)
	{
		auto& Session = Pair.Value;
		if (Session.ExecutionId == ExecutionId && Session.Request.Volume.AnchorKind ==
			EHodgeHitAnchorKind::ExecutionTransform &&
			Session.Request.Volume.AnchorKey == Key && !Session.bHasFixedAnchor)
		{
			Session.Request.RuntimeAnchor = Transform;
		}
	}
}

bool UHodgeCombatComponentBase::SampleDetection(uint64 Handle, const FGuid& ExecutionId,
                                                FHodgeHitDetectionBatch& OutBatch)
{
	OutBatch = FHodgeHitDetectionBatch();
	FHodgeHitDetectionSession* Live = Sessions.Find(Handle);
	if (!Live || Live->ExecutionId != ExecutionId || !IsValid(GetOwner()) || !GetOwner()->HasAuthority() || !GetWorld())
	{
		return false;
	}
	if (!IsSourceValid(*Live))
	{
		Sessions.Remove(Handle);
		return false;
	}
	const FHodgeHitDetectionSession Snapshot = *Live;
	FHodgeHitGeometry Current;
	const UHodgeHitDetectionStrategy* Strategy = Snapshot.Strategy.GetDefaultObject();
	if (!CaptureDetectionGeometry(Snapshot, Current))
	{
		Sessions.Remove(Handle);
		return false;
	}
	// 策略扩展可能使会话失效，捕获返回后重新查找再更新历史。
	Live = Sessions.Find(Handle);
	if (!Live || Live->ExecutionId != ExecutionId || !IsSourceValid(*Live)) { return false; }
	Live->Previous = Current;
	OutBatch.ExecutionId = ExecutionId;
	OutBatch.OccurrenceId = Snapshot.OccurrenceId;
	OutBatch.SessionHandle = Handle;
	OutBatch.SampleSequence = ++Live->SampleSequence;
	OutBatch.SampleTime = GetWorld()->GetTimeSeconds();
	OutBatch.SourceComponent = Snapshot.SourceComponent;
	OutBatch.Weapon = Snapshot.Weapon;
	OutBatch.SourceOrigin = Current.Transform.GetLocation();
	OutBatch.ResultKind = Snapshot.Request.TargetPolicy == EHodgeHitTargetPolicy::ConfirmedTarget
		                      ? EHodgeHitResultKind::ConfirmedTarget
		                      : Snapshot.Request.Profile->QueryMode == EHodgeHitQueryMode::Overlap
		                      ? EHodgeHitResultKind::Overlap
		                      : EHodgeHitResultKind::Sweep;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(HodgeHitDetection), false, GetOwner());
	Params.bReturnPhysicalMaterial = true;
	for (const auto& Actor : Snapshot.Request.IgnoredActors)
	{
		if (Actor.IsValid()) { Params.AddIgnoredActor(Actor.Get()); }
	}
	if (auto* Equipment = GetOwner()->FindComponentByClass<UHodgeEquipmentManagerComponent>())
	{
		for (UHodgeEquipmentInstance* Instance : Equipment->GetEquipmentInstancesOfType(
			     UHodgeEquipmentInstance::StaticClass()))
		{
			Params.AddIgnoredActors(Instance->GetSpawnedActors());
		}
	}
	TArray<FHitResult> Hits;
	if (Snapshot.Request.TargetPolicy == EHodgeHitTargetPolicy::ConfirmedTarget)
	{
		AActor* Target = Snapshot.Request.RuntimeTarget.Get();
		auto* Component = Cast<UPrimitiveComponent>(Target->GetRootComponent());
		if (Component) { Hits.Add(FHitResult(Target, Component, Target->GetActorLocation(), FVector::ZeroVector)); }
	}
	else if (Snapshot.Request.Profile->QueryMode == EHodgeHitQueryMode::Overlap && Snapshot.Request.Volume.GeometryMode
		== EHodgeHitGeometryMode::ExistingSource)
	{
		const auto* ShapeQuery = GetDefault<UHodgeShapeQueryStrategy>();
		FHodgeHitGeometry Geometry = Current;
		Geometry.Shape = Snapshot.SourceComponent.IsValid() && Snapshot.SourceComponent->IsA<UBoxComponent>()
			                 ? EHodgeHitShape::Box
			                 : EHodgeHitShape::Sphere;
		if (Geometry.Shape == EHodgeHitShape::Box)
		{
			ShapeQuery->Detect(GetWorld(), Snapshot.Source, Snapshot.Request.Profile, Geometry, Geometry, Params, Hits);
		}
		else
		{
			for (const FVector& Point : Current.Points)
			{
				Geometry.Transform.SetLocation(Point);
				ShapeQuery->Detect(GetWorld(), Snapshot.Source, Snapshot.Request.Profile, Geometry, Geometry, Params,
				                   Hits);
			}
		}
	}
	else
	{
		Strategy->Detect(GetWorld(), Snapshot.Source, Snapshot.Request.Profile, Snapshot.Previous, Current, Params,
		                 Hits);
	}
	const FTransform Filter = Snapshot.Request.Profile->FilterFrame == EHodgeHitFilterFrame::DetectionAnchor
		                          ? Current.Transform
		                          : GetOwner()->GetActorTransform();
	for (const FHitResult& Hit : Hits)
	{
		AActor* Target = Hit.GetActor();
		if (!IsValid(Target) || Target == GetOwner()) { continue; }
		if (Snapshot.Request.TargetPolicy == EHodgeHitTargetPolicy::LockedTargetInVolume && Target != Snapshot.Request.
			RuntimeTarget) { continue; }
		const FVector Direction = (Target->GetActorLocation() - Filter.GetLocation()).GetSafeNormal2D();
		if (Snapshot.Request.Profile->HalfAngleDegrees < 180.f && !Direction.IsNearlyZero() &&
			FVector::DotProduct(Filter.GetRotation().GetForwardVector().GetSafeNormal2D(), Direction) <
			FMath::Cos(FMath::DegreesToRadians(Snapshot.Request.Profile->HalfAngleDegrees))) { continue; }
		if (Snapshot.Request.Profile->bRequireLineOfSight)
		{
			FCollisionQueryParams VisibilityParams = Params;
			VisibilityParams.AddIgnoredActor(Target);
			FHitResult Obstruction;
			if (GetWorld()->LineTraceSingleByChannel(Obstruction, Filter.GetLocation(), Target->GetActorLocation(),
			                                         Snapshot.Request.Profile->ObstructionChannel, VisibilityParams))
			{
				continue;
			}
		}
		// 保留没有 ASC 的几何命中，交由接收 GA 决定其用途。
		OutBatch.Hits.Add(Hit);
	}
	return true;
}

// 连段、输入缓存和网络协调

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_ComboInputRequest, "GameplayEvent.Combo.InputRequest");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_ComboEventRequest, "GameplayEvent.Combo.EventRequest");

void UHodgeCombatComponentBase::Configure(UHodgeAbilitySystemComponent* InASC,
                                          const UHodgeComboDefinition* InDefinition)
{
	if (bShuttingDown) { return; }
	if (InASC && InASC->GetAvatarActor() != GetOwner()) { return; }
	if (ASC == InASC && Definition == InDefinition) { return; }
	Shutdown();
	ASC = InASC;
	TArray<FText> Errors;
	if (InDefinition && InDefinition->ValidateDefinition(Errors)) { Definition = InDefinition; }
	for (const FText& Error : Errors) { UE_LOG(LogTemp, Error, TEXT("[Hodge] Combo: %s"), *Error.ToString()); }
	CurrentComboTag = Definition ? Definition->EntryComboTag : FGameplayTag();
	SetComponentTickEnabled(ASC && Definition && ASC->AbilityActorInfo.IsValid() &&
		(GetOwner()->HasAuthority() || ASC->AbilityActorInfo->IsLocallyControlled()));
	// 观察者标签可能先于 Pawn 的 ASC 绑定到达，绑定完成后补应用缓存状态。
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy) { OnRep_ObserverTags({}); }
}

void UHodgeCombatComponentBase::Shutdown()
{
	if (bShuttingDown) { return; }
	TGuardValue<bool> Guard(bShuttingDown, true);
	SetComponentTickEnabled(false);
	if (ASC)
	{
		if (auto* Ability = Cast<UHodgeGameplayAbility_Definition>(ASC->GetAnimatingAbility()))
		{
			if (Ability->GetAvatarActorFromActorInfo() == GetOwner()) { Ability->FinishExecution(true, true); }
		}
	}
	ResetSession(true);
	SetNode(FGameplayTag());
	if (ASC) { ASC->RemoveLooseGameplayTags(AppliedObserverTags); }
	AppliedObserverTags.Reset();
	EndAllDetectionSessions();
	const TArray<FGuid> Leases = PoseLeases.Array();
	for (FGuid Lease : Leases) { ReleasePoseLease(Lease); }
	AuthorizedHandle = {};
	PendingNode = FGameplayTag();
	Definition = nullptr;
	ASC = nullptr;
	CurrentComboTag = FGameplayTag();
	bMoveRequestPending = false;
	bSwitching = false;
	bMemoryCorrectionPending = false;
	PreviousComboMemory = {};
	ComboMemory = {};
	bTransitionStarted = false;
	bEvaluating = false;
}

void UHodgeCombatComponentBase::ClearInput()
{
	BufferedInput = FGameplayTag();
	InputExpiresAt = 0;
}

bool UHodgeCombatComponentBase::InputPressed(FGameplayTag InputTag)
{
	if (!IsComboReady()) { return false; }
	for (const auto& Binding : Definition->InputBindings)
	{
		if (Binding.InputTag != InputTag) { continue; }
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
		{
			ClearInput();
			return true;
		}
		BufferedInput = Binding.IntentTag;
		InputExpiresAt = GetWorld()->GetTimeSeconds() + Definition->InputBufferSeconds;
		TryTransition(BufferedInput, false);
		return true;
	}
	return false;
}

const FHodgeComboTransition* UHodgeCombatComponentBase::SelectTransition(FGameplayTag Trigger, bool bEvent) const
{
	const FGameplayTag Source = TransitionSourceNode();
	const auto* Node = Definition ? Definition->FindNode(Source) : nullptr;
	if (!ASC || !Node || !Trigger.IsValid()) { return nullptr; }
	const auto Windows = CurrentAbility ? CurrentAbility->GetExecutionWindows() : FGameplayTagContainer();
	const auto& Tags = ASC->GetOwnedGameplayTags();
	const FHodgeComboTransition* Best = nullptr;
	for (const auto& Edge : Node->Transitions)
	{
		const bool bResume = !CurrentAbility && Source != Definition->EntryComboTag;
		if ((bEvent ? Edge.TriggerEventTag : Edge.TriggerInputIntentTag) != Trigger
			|| (bResume ? (bEvent || !Edge.bAllowAfterExecutionEnded) : !Windows.HasAll(Edge.RequiredWindowTags))
			|| !Tags.HasAll(Edge.RequiredSourceTags)
			|| Tags.HasAny(Edge.BlockedSourceTags)) { continue; }
		if (!Best || Edge.TransitionPriority > Best->TransitionPriority) { Best = &Edge; }
	}
	return Best;
}

bool UHodgeCombatComponentBase::IsAuthorized(FGameplayAbilitySpecHandle Handle) const
{
	return IsComboReady() && Handle.IsValid() && AuthorizedHandle == Handle;
}

int16 UHodgeCombatComponentBase::ExecutionKey() const
{
	if (!CurrentAbility) { return GetRememberedComboTag().IsValid() ? ComboMemory.ExecutionKey : 0; }
	const auto Key = CurrentAbility->GetCurrentActivationInfo().GetActivationPredictionKey();
	return Key.IsServerInitiatedKey() ? -Key.Current : Key.Current;
}

bool UHodgeCombatComponentBase::PrepareTransition(const FHodgeComboTransition& Edge, FGameplayAbilitySpecHandle Handle)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(Handle);
	const auto* AttackDefinition = ASC->FindAbilityDefinition(Handle);
	if (!AttackDefinition || AttackDefinition->ExecutionRoute != EHodgeAbilityExecutionRoute::ComboCoordinated)
	{
		return false;
	}
	if (!Spec || (Spec->IsActive() && (!CurrentAbility || CurrentAbility->GetCurrentAbilitySpecHandle() != Handle)))
	{
		return false;
	}
	AuthorizedHandle = Handle;
	if (!Spec->Ability->CanActivateAbility(Handle, ASC->AbilityActorInfo.Get()))
	{
		AuthorizedHandle = {};
		return false;
	}
	if (!IsComboReady() || AuthorizedHandle != Handle) { return false; }
	bSwitching = true;
	bTransitionStarted = false;
	PreviousComboMemory = ComboMemory;
	if (CurrentAbility)
	{
		PreviousComboMemory.ExpiresAt = ComboTime() + Definition->ComboRetentionSeconds;
		if (!HasResumeTransition(CurrentComboTag)) { PreviousComboMemory.Node = FGameplayTag(); }
	}
	PendingNode = Edge.TargetComboTag;
	ClearInput();
	if (CurrentAbility) { CurrentAbility->FinishExecution(true, false); }
	// 结束旧技能可能同步卸载组件或更换 Pawn，不能继续使用已清理的连招配置。
	if (!IsComboReady() || !bSwitching) { return false; }
	CurrentAbility = nullptr;
	SetNode(Definition->EntryComboTag);
	return true;
}

bool UHodgeCombatComponentBase::TryTransition(FGameplayTag Trigger, bool bEvent)
{
	if (!IsComboReady() || bSwitching || bEvaluating || bMemoryCorrectionPending) { return false; }
	ExpireComboMemory();
	const FGameplayTag SourceNode = TransitionSourceNode();
	const int16 SourceKey = ExecutionKey();
	if (!IsComboReady() || SourceNode != TransitionSourceNode() || SourceKey != ExecutionKey()) { return false; }
	TGuardValue<bool> Guard(bEvaluating, true);
	const auto* Edge = SelectTransition(Trigger, bEvent);
	if (!Edge) { return false; }
	const auto* Target = Definition->FindNode(Edge->TargetComboTag);
	if (!Target) { return false; }
	if (Target->ComboTag == Definition->EntryComboTag)
	{
		ClearInput();
		if (bEvent) { ResetSession(true); }
		else { ServerReturnToEntry(ASC->GetAvatarActor(), SourceNode, ExecutionKey(), Trigger); }
		return true;
	}
	const auto Handle = ASC->FindDefinitionAbility(Target->AbilityTag);
	FGameplayEventData Payload;
	Payload.EventTag = bEvent ? TAG_ComboEventRequest : TAG_ComboInputRequest;
	Payload.Instigator = ASC->GetAvatarActor();
	Payload.OptionalObject = Definition;
	Payload.TargetTags.AddTag(SourceNode);
	Payload.InstigatorTags.AddTag(Trigger);
	Payload.EventMagnitude = ExecutionKey();
	if (bEvent) { Payload.TargetTags.AddTag(Target->ComboTag); }
	if (!PrepareTransition(*Edge, Handle)) { return false; }
	const FPredictionKey Key = bEvent && GetOwner()->HasAuthority()
		                           ? FPredictionKey::CreateNewServerInitiatedKey(ASC)
		                           : FPredictionKey();
	const bool bActivated = ASC->InternalTryActivateAbility(Handle, Key, nullptr, nullptr, &Payload);
	CompleteServerActivation();
	return bActivated && CurrentAbility != nullptr;
}

float UHodgeCombatComponentBase::GetServerInputBufferSeconds() const
{
	return Definition ? FMath::Max(0.01f, Definition->InputBufferSeconds) : 0.f;
}

bool UHodgeCombatComponentBase::ValidateServerRequestIdentity(const FGameplayEventData* Payload) const
{
	// 来源、预测身份与输入类型先核对，不能缓存客户端自行声明的权威玩法消息。
	return IsComboReady() && !ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked)
		&& Payload && Payload->OptionalObject == Definition && Payload->Instigator == ASC->GetAvatarActor()
		&& Payload->TargetTags.Num() == 1 && Payload->InstigatorTags.Num() == 1
		&& Payload->TargetTags.HasTagExact(TransitionSourceNode()) && Payload->EventMagnitude == ExecutionKey()
		&& Payload->EventTag == TAG_ComboInputRequest;
}

bool UHodgeCombatComponentBase::CanBufferServerActivation(FGameplayAbilitySpecHandle Handle,
                                                          const FGameplayEventData* Payload) const
{
	if (!Handle.IsValid() || !CurrentAbility || !ValidateServerRequestIdentity(Payload)
		|| !ASC->FindAbilitySpecFromHandle(Handle)) { return false; }
	const auto* Node = Definition->FindNode(TransitionSourceNode());
	if (!Node) { return false; }
	const auto Windows = CurrentAbility->GetExecutionWindows();
	const auto& Tags = ASC->GetOwnedGameplayTags();
	for (const auto& Edge : Node->Transitions)
	{
		const auto* Target = Definition->FindNode(Edge.TargetComboTag);
		if (Edge.TriggerInputIntentTag == Payload->InstigatorTags.First() && Target
			&& ASC->FindDefinitionAbility(Target->AbilityTag) == Handle && Tags.HasAll(Edge.RequiredSourceTags)
			&& !Tags.HasAny(Edge.BlockedSourceTags) && !Windows.HasAll(Edge.RequiredWindowTags)) { return true; }
	}
	return false;
}

bool UHodgeCombatComponentBase::PrepareServerActivation(FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayEventData* Payload)
{
	ExpireComboMemory();
	if (!ValidateServerRequestIdentity(Payload)) { return false; }
	TGuardValue<bool> Guard(bEvaluating, true);
	const auto* Edge = SelectTransition(Payload->InstigatorTags.First(), false);
	const auto* Target = Edge ? Definition->FindNode(Edge->TargetComboTag) : nullptr;
	if (!Target || ASC->FindDefinitionAbility(Target->AbilityTag) != Handle) { return false; }
	return PrepareTransition(*Edge, Handle);
}

bool UHodgeCombatComponentBase::PrepareConfirmedActivation(FGameplayAbilitySpecHandle Handle,
                                                           const FGameplayEventData& Payload)
{
	if (!IsComboReady() || Payload.OptionalObject != Definition || Payload.Instigator != ASC->GetAvatarActor()
		|| Payload.EventTag != TAG_ComboEventRequest) { return false; }
	for (FGameplayTag Tag : Payload.TargetTags)
	{
		const auto* Row = Definition->FindNode(Tag);
		if (Row && ASC->FindDefinitionAbility(Row->AbilityTag) == Handle)
		{
			const FHodgeComboMemoryState MemoryBeforeConfirmation = ComboMemory;
			ResetSession(true);
			if (!IsComboReady()) { return false; }
			bSwitching = true;
			bTransitionStarted = false;
			PreviousComboMemory = MemoryBeforeConfirmation;
			PendingNode = Tag;
			AuthorizedHandle = Handle;
			return true;
		}
	}
	return false;
}

void UHodgeCombatComponentBase::RejectServerActivation(const FGameplayEventData* Payload)
{
	// 拒绝请求不能清除服务器已经确认的动作或连段记忆。
	ExpireComboMemory();
}

void UHodgeCombatComponentBase::CompleteServerActivation()
{
	if (!CurrentAbility && bTransitionStarted && IsComboReady())
	{
		RetainComboMemory();
		ClearInput();
		SetNode(Definition->EntryComboTag);
	}
	if (!bTransitionStarted && IsComboReady())
	{
		ComboMemory = PreviousComboMemory;
		ExpireComboMemory();
		PublishComboMemory();
	}
	bSwitching = false;
	AuthorizedHandle = {};
	PendingNode = FGameplayTag();
	PreviousComboMemory = {};
	DrainEvents();
}

void UHodgeCombatComponentBase::ExecutionStarted(UHodgeGameplayAbility_Definition* Ability)
{
	if (!Ability || !IsAuthorized(Ability->GetCurrentAbilitySpecHandle())) { return; }
	CurrentAbility = Ability;
	SetNode(PendingNode);
	bTransitionStarted = true;
	ComboMemory.Node = CurrentComboTag;
	ComboMemory.ExpiresAt = 0;
	ComboMemory.ExecutionKey = ExecutionKey();
}

void UHodgeCombatComponentBase::ExecutionEnded(UHodgeGameplayAbility_Definition* Ability)
{
	if (CurrentAbility != Ability) { return; }
	CurrentAbility = nullptr;
	if (!bSwitching)
	{
		RetainComboMemory();
		QueuedEvents.Reset();
		ClearInput();
		SetNode(Definition ? Definition->EntryComboTag : FGameplayTag());
		if (!GetOwner()->HasAuthority()) { OnRep_ComboMemory(); }
	}
}

void UHodgeCombatComponentBase::WindowsChanged(UHodgeGameplayAbility_Definition* Ability)
{
	if (!IsComboReady() || CurrentAbility != Ability || !ASC->AbilityActorInfo->IsLocallyControlled()) { return; }
	if (BufferedInput.IsValid() && GetWorld()->GetTimeSeconds() < InputExpiresAt)
	{
		TryTransition(BufferedInput, false);
	}
}

void UHodgeCombatComponentBase::ExecutionEvent(UHodgeGameplayAbility_Definition* Ability, FGameplayTag Event)
{
	if (!IsComboReady() || CurrentAbility != Ability || !GetOwner()->HasAuthority()) { return; }
	if (bSwitching || bEvaluating) { QueuedEvents.Add({Ability, ExecutionKey(), Event}); }
	else { TryTransition(Event, true); }
}

void UHodgeCombatComponentBase::DrainEvents()
{
	if (bDrainingEvents || bSwitching || bEvaluating) { return; }
	TGuardValue<bool> Guard(bDrainingEvents, true);
	int32 Remaining = 32;
	while (!QueuedEvents.IsEmpty() && Remaining-- > 0)
	{
		const FQueuedEvent Event = QueuedEvents[0];
		QueuedEvents.RemoveAt(0);
		if (Event.Ability == CurrentAbility && Event.Key == ExecutionKey()) { TryTransition(Event.Tag, true); }
	}
	if (!QueuedEvents.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[Hodge] Combo event chain exceeded 32 transitions; returning to Entry"));
		ResetSession(true);
	}
}

void UHodgeCombatComponentBase::SetNode(FGameplayTag Node)
{
	FGameplayTagContainer Previous = MoveTemp(OwnedNodeTags);
	OwnedNodeTags.Reset();
	CurrentComboTag = Node;
	if (ASC) { ASC->RemoveLooseGameplayTags(Previous); }
	if (const auto* Row = Definition ? Definition->FindNode(Node) : nullptr)
	{
		OwnedNodeTags = Row->GrantedTags;
		if (ASC) { ASC->AddLooseGameplayTags(OwnedNodeTags); }
	}
	if (GetOwner()->HasAuthority()) { ObserverTags = OwnedNodeTags; }
}

void UHodgeCombatComponentBase::ResetSession(bool bEndAbility)
{
	TGuardValue<bool> Guard(bSwitching, true);
	auto* Previous = CurrentAbility.Get();
	CurrentAbility = nullptr;
	QueuedEvents.Reset();
	ClearInput();
	if (bEndAbility && Previous) { Previous->FinishExecution(true, GetOwner()->HasAuthority()); }
	SetNode(Definition ? Definition->EntryComboTag : FGameplayTag());
	ComboMemory.Node = FGameplayTag();
	ComboMemory.ExpiresAt = 0;
	PublishComboMemory();
}

double UHodgeCombatComponentBase::ComboTime() const
{
	const auto* World = GetWorld();
	const auto* State = World ? World->GetGameState() : nullptr;
	return State ? State->GetServerWorldTimeSeconds() : (World ? World->GetTimeSeconds() : 0.0);
}

FGameplayTag UHodgeCombatComponentBase::GetRememberedComboTag() const
{
	return ComboMemory.Node.IsValid() && (CurrentAbility || ComboMemory.ExpiresAt > ComboTime())
		       ? ComboMemory.Node
		       : FGameplayTag();
}

float UHodgeCombatComponentBase::GetComboMemoryRemainingTime() const
{
	return !CurrentAbility && GetRememberedComboTag().IsValid()
		       ? static_cast<float>(FMath::Max(0.0, ComboMemory.ExpiresAt - ComboTime()))
		       : 0.f;
}

FGameplayTag UHodgeCombatComponentBase::TransitionSourceNode() const
{
	if (CurrentAbility) { return CurrentComboTag; }
	const FGameplayTag Memory = GetRememberedComboTag();
	return Memory.IsValid() ? Memory : CurrentComboTag;
}

bool UHodgeCombatComponentBase::HasResumeTransition(FGameplayTag Node) const
{
	const auto* Row = Definition ? Definition->FindNode(Node) : nullptr;
	return Row && Row->Transitions.ContainsByPredicate([](const FHodgeComboTransition& Edge)
	{
		return Edge.bAllowAfterExecutionEnded && Edge.TriggerInputIntentTag.IsValid();
	});
}

void UHodgeCombatComponentBase::RetainComboMemory()
{
	if (!Definition || !HasResumeTransition(ComboMemory.Node) || Definition->ComboRetentionSeconds <= 0.f)
	{
		ComboMemory.Node = FGameplayTag();
		ComboMemory.ExpiresAt = 0;
	}
	else { ComboMemory.ExpiresAt = ComboTime() + Definition->ComboRetentionSeconds; }
	PublishComboMemory();
}

void UHodgeCombatComponentBase::ExpireComboMemory()
{
	if (!CurrentAbility && ComboMemory.Node.IsValid() && ComboMemory.ExpiresAt <= ComboTime())
	{
		ComboMemory.Node = FGameplayTag();
		ComboMemory.ExpiresAt = 0;
		PublishComboMemory();
	}
}

void UHodgeCombatComponentBase::PublishComboMemory()
{
	if (GetOwner()->HasAuthority())
	{
		ReplicatedComboMemory = ComboMemory;
		GetOwner()->ForceNetUpdate();
	}
}

void UHodgeCombatComponentBase::EndCurrentExecution()
{
	if (CurrentAbility) { CurrentAbility->FinishExecution(true, GetOwner()->HasAuthority()); }
}

void UHodgeCombatComponentBase::OnRep_ComboMemory()
{
	if (IsComboReady() && !CurrentAbility && !bSwitching
		&& (ComboMemory.ExecutionKey == 0 || ComboMemory.ExecutionKey == ReplicatedComboMemory.ExecutionKey))
	{
		ComboMemory = ReplicatedComboMemory;
	}
}

void UHodgeCombatComponentBase::HandlePredictionRejected()
{
	if (!IsComboReady() || GetOwner()->HasAuthority()) { return; }
	ClearInput();
	bMemoryCorrectionPending = true;
	ServerSynchronizeComboMemory();
}

void UHodgeCombatComponentBase::ServerSynchronizeComboMemory_Implementation()
{
	ExpireComboMemory();
	ClientCorrectComboMemory(ComboMemory);
}

void UHodgeCombatComponentBase::ClientCorrectComboMemory_Implementation(FHodgeComboMemoryState State)
{
	bMemoryCorrectionPending = false;
	if (IsComboReady() && !CurrentAbility && !bSwitching) { ComboMemory = State; }
}

void UHodgeCombatComponentBase::TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Tick)
{
	Super::TickComponent(Delta, Type, Tick);
	DrainEvents();
	ExpireComboMemory();
	if (!IsComboReady() || !ASC->AbilityActorInfo->IsLocallyControlled())
	{
		return;
	}
	if (ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
	{
		ClearInput();
		return;
	}
	if (GetWorld()->GetTimeSeconds() >= InputExpiresAt) { ClearInput(); }
	if (BufferedInput.IsValid() && TryTransition(BufferedInput, false)) { return; }
	if (!CurrentAbility || bSwitching || bMoveRequestPending) { return; }
	const auto* Avatar = ASC->GetAvatarActor();
	const auto* Hero = Avatar ? Avatar->FindComponentByClass<UHodgeHeroComponent>() : nullptr;
	if (Hero && Hero->HasMoveIntent(Definition->MoveIntentThreshold)
		&& CurrentAbility->GetExecutionWindows().HasTag(Definition->MoveCancelWindowTag))
	{
		bMoveRequestPending = true;
		ServerMoveCancel(ASC->GetAvatarActor(), CurrentAbility->GetCurrentAbilitySpecHandle(), ExecutionKey());
	}
}

void UHodgeCombatComponentBase::ServerMoveCancel_Implementation(AActor* Avatar, FGameplayAbilitySpecHandle Handle,
                                                                int32 Key)
{
	if (!GetWorld())
	{
		ClientMoveCancelResult();
		return;
	}
	TWeakObjectPtr<AActor> WeakAvatar = Avatar;
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this,
		[this, WeakAvatar, Handle, Key]() { ProcessServerMoveCancel(WeakAvatar, Handle, Key); }));
}

void UHodgeCombatComponentBase::ProcessServerMoveCancel(TWeakObjectPtr<AActor> Avatar,
                                                        FGameplayAbilitySpecHandle Handle, int32 Key)
{
	// RPC 可能早于本帧 NotifyBegin 到达；等动画更新完成后再检查，不能提前授予取消权限。
	if (Avatar.IsValid() && IsComboReady() && CurrentAbility && Avatar.Get() == GetOwner()
		&& Handle == CurrentAbility->GetCurrentAbilitySpecHandle() && Key == ExecutionKey()
		&& CurrentAbility->GetExecutionWindows().HasTag(Definition->MoveCancelWindowTag))
	{
		EndCurrentExecution();
	}
	ClientMoveCancelResult();
}

void UHodgeCombatComponentBase::ClientMoveCancelResult_Implementation() { bMoveRequestPending = false; }

void UHodgeCombatComponentBase::ServerReturnToEntry_Implementation(AActor* Avatar, FGameplayTag SourceNode, int32 Key,
                                                                   FGameplayTag Intent)
{
	ExpireComboMemory();
	if (!IsComboReady() || Avatar != GetOwner() || SourceNode != TransitionSourceNode()
		|| Key != ExecutionKey() || ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked)) { return; }
	if (!IsComboReady() || SourceNode != TransitionSourceNode() || Key != ExecutionKey()) { return; }
	TGuardValue<bool> Guard(bEvaluating, true);
	const auto* Edge = SelectTransition(Intent, false);
	if (Edge && Edge->TargetComboTag == Definition->EntryComboTag) { ResetSession(true); }
}

void UHodgeCombatComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UHodgeCombatComponentBase, ObserverTags, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UHodgeCombatComponentBase, ReplicatedComboMemory, COND_OwnerOnly);
}

void UHodgeCombatComponentBase::OnRep_ObserverTags(const FGameplayTagContainer& Previous)
{
	if (IsValid(ASC) && ASC->GetAvatarActor() == GetOwner() && GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		// 只撤销本组件实际应用的标签，重复绑定不会叠加其他来源的计数。
		ASC->RemoveLooseGameplayTags(AppliedObserverTags);
		AppliedObserverTags = ObserverTags;
		ASC->AddLooseGameplayTags(AppliedObserverTags);
	}
}

UHodgeCombatComponentBase* UHodgeCombatComponentBase::FindCombatComponent(const AActor* Avatar)
{
	return IsValid(Avatar) ? Avatar->FindComponentByClass<UHodgeCombatComponentBase>() : nullptr;
}

bool UHodgeCombatComponentBase::IsComboReady() const
{
	// 旧 Pawn 的组件不能继续操作已经切换 Avatar 的 PlayerState ASC。
	return !bShuttingDown && IsRegistered() && IsValid(ASC) && Definition &&
		ASC->AbilityActorInfo.IsValid() && ASC->GetAvatarActor() == GetOwner() &&
		!ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death);
}

void UHodgeCombatComponentBase::BindPawnExtension()
{
	auto* Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(GetOwner());
	if (!Extension) { return; }
	PawnExtension = Extension;
	Extension->OnAbilitySystemUninitialized_Register(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::HandleAbilitySystemUninitialized));
	Extension->OnAbilitySystemInitialized_RegisterAndCall(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::HandleAbilitySystemInitialized));
}

void UHodgeCombatComponentBase::HandleAbilitySystemInitialized()
{
	if (!IsRegistered() || !PawnExtension.IsValid()) { return; }
	const auto* PawnData = PawnExtension->GetPawnData<UHodgePawnData>();
	Configure(PawnExtension->GetHodgeAbilitySystemComponent(), PawnData ? PawnData->ComboDefinition.Get() : nullptr);
}

void UHodgeCombatComponentBase::HandleAbilitySystemUninitialized()
{
	Shutdown();
}

void UHodgeCombatComponentBase::OnRegister()
{
	Super::OnRegister();
	BindPawnExtension();
}

void UHodgeCombatComponentBase::BeginPlay()
{
	Super::BeginPlay();
	// 覆盖动态添加时其他默认组件尚未完成注册的顺序。
	BindPawnExtension();
}

void UHodgeCombatComponentBase::OnUnregister()
{
	if (PawnExtension.IsValid()) { PawnExtension->UnregisterAbilitySystemDelegates(this); }
	PawnExtension.Reset();
	Shutdown();
	Super::OnUnregister();
}
