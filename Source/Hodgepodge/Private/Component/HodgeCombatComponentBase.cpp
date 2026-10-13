/** @file HodgeCombatComponentBase.cpp
 * @brief 实现统一战斗组件相关的类型、接口与现有处理流程。
 */
#include "Component/HodgeCombatComponentBase.h"
#include "Component/HodgeEnemyInitComponent.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "Character/HodgeHeroCharacter.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Combat/HodgeHitReactionTypes.h"
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
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCombatComponentBase)

namespace
{
	/** 查找来源组件。
	 * @param Actor 本次操作关联的Actor。
	 * @param Name 名称，由调用方传入。
	 * @return 来源组件的当前值或引用。
	 */
	USceneComponent* FindSourceComponent(AActor* Actor, FName Name)
	{
		// 先检查对象与上下文，避免在依赖失效后继续处理。
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

/** 构造统一战斗组件并初始化其默认状态。 */
UHodgeCombatComponentBase::UHodgeCombatComponentBase()
{
	// 组件 Tick 只推进连招缓存，几何采样仍由技能 Task 独立调度。
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

/** 申请一份动作姿势占用，供结束时按来源释放。 */
FGuid UHodgeCombatComponentBase::AcquirePoseLease()
{
	// 按服务器权限区分本次处理分支。
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

/** 释放该动作拥有的姿势占用。 */
void UHodgeCombatComponentBase::ReleasePoseLease(FGuid Handle)
{
	if (!PoseLeases.Remove(Handle) || !PoseLeases.IsEmpty()) { return; }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (PoseMesh.IsValid() && PoseMesh->VisibilityBasedAnimTickOption ==
		EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones)
	{
		PoseMesh->VisibilityBasedAnimTickOption = static_cast<EVisibilityBasedAnimTickOption>(SavedPosePolicy);
	}
	PoseMesh.Reset();
}

/** 离开世界运行阶段并解除本对象持有的监听或资源。 */
void UHodgeCombatComponentBase::EndPlay(const EEndPlayReason::Type Reason)
{
	Shutdown();
	TArray<FGuid> Leases = PoseLeases.Array();
	for (FGuid Lease : Leases) { ReleasePoseLease(Lease); }
	Super::EndPlay(Reason);
}

/** 解析来源。 */
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

/** 判断来源有效。 */
bool UHodgeCombatComponentBase::IsSourceValid(const FHodgeHitDetectionSession& Session) const
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
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

/** 采集检测几何。 */
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

/** 创建检测会话。 */
uint64 UHodgeCombatComponentBase::CreateDetectionSession(const FGuid& ExecutionId, int32 OccurrenceId,
                                                         const FHodgeHitDetectionRequest& Request)
{
	// 按服务器权限区分本次处理分支。
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
	// 先检查对象与上下文，避免在依赖失效后继续处理。
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

/** 判断检测会话有效。 */
bool UHodgeCombatComponentBase::IsDetectionSessionValid(uint64 Handle, const FGuid& ExecutionId) const
{
	const FHodgeHitDetectionSession* Session = Sessions.Find(Handle);
	return Session && Session->ExecutionId == ExecutionId && IsSourceValid(*Session);
}

/** 重置检测历史。 */
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

/** 获取检测来源组件。 */
USceneComponent* UHodgeCombatComponentBase::GetDetectionSourceComponent(uint64 Handle, const FGuid& ExecutionId) const
{
	const FHodgeHitDetectionSession* Session = Sessions.Find(Handle);
	return Session && Session->ExecutionId == ExecutionId && IsSourceValid(*Session)
		       ? Session->SourceComponent.Get()
		       : nullptr;
}

/** 结束检测会话。 */
void UHodgeCombatComponentBase::EndDetectionSession(uint64 Handle, const FGuid& ExecutionId)
{
	const FHodgeHitDetectionSession* Session = Sessions.Find(Handle);
	if (Session && Session->ExecutionId == ExecutionId) { Sessions.Remove(Handle); }
}

/** 结束检测Sessions关联执行。 */
void UHodgeCombatComponentBase::EndDetectionSessionsForExecution(const FGuid& ExecutionId)
{
	for (auto It = Sessions.CreateIterator(); It; ++It)
	{
		if (It.Value().ExecutionId == ExecutionId) { It.RemoveCurrent(); }
	}
}

/** 结束All检测Sessions。 */
void UHodgeCombatComponentBase::EndAllDetectionSessions()
{
	Sessions.Reset();
}

/** 更新检测锚点。 */
void UHodgeCombatComponentBase::UpdateDetectionAnchor(const FGuid& ExecutionId, FName Key, const FTransform& Transform)
{
	// 按服务器权限区分本次处理分支。
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

/** 按当前检测配置采样几何与目标。 */
bool UHodgeCombatComponentBase::SampleDetection(uint64 Handle, const FGuid& ExecutionId,
                                                FHodgeHitDetectionBatch& OutBatch)
{
	OutBatch = FHodgeHitDetectionBatch();
	FHodgeHitDetectionSession* Live = Sessions.Find(Handle);
	// 按服务器权限区分本次处理分支。
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

/** 绑定当前执行所用的ASC与配置数据。 */
void UHodgeCombatComponentBase::Configure(UHodgeAbilitySystemComponent* InASC,
                                          const UHodgeComboDefinition* InDefinition)
{
	// 根据当前生命周期状态决定是否继续执行。
	if (bShuttingDown) { return; }
	if (InASC && InASC->GetAvatarActor() != GetOwner()) { return; }
	if (ASC == InASC && Definition == InDefinition) { return; }
	Shutdown();
	ASC = InASC;
	// 按当前对象生命周期建立状态监听。
	if (ASC) { ActionEndedHandle = ASC->OnAbilityEnded.AddUObject(this, &ThisClass::HandleActionAbilityEnded); }
	TArray<FText> Errors;
	if (InDefinition && InDefinition->ValidateDefinition(Errors)) { Definition = InDefinition; }
	for (const FText& Error : Errors) { UE_LOG(LogTemp, Error, TEXT("[Hodge] Combo: %s"), *Error.ToString()); }
	CurrentComboTag = Definition ? Definition->EntryComboTag : FGameplayTag();
	SetComponentTickEnabled(ASC && Definition && ASC->AbilityActorInfo.IsValid() &&
		(GetOwner()->HasAuthority() || ASC->AbilityActorInfo->IsLocallyControlled()));
	// 观察者标签可能先于 Pawn 的 ASC 绑定到达，绑定完成后补应用缓存状态。
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy) { OnRep_ObserverTags({}); }
}

/** 停止当前对象的运行绑定并清理其资源。 */
void UHodgeCombatComponentBase::Shutdown()
{
	// 根据当前生命周期状态决定是否继续执行。
	if (bShuttingDown) { return; }
	TGuardValue<bool> Guard(bShuttingDown, true);
	CloseActionRequests();
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
	PredictedMoveCancelAbility.Reset();
	PendingMoveHandle = {};
	PendingMoveKey = 0;
	RejectedMoveHandle = {};
	RejectedMoveKey = 0;
	bSwitching = false;
	bMemoryCorrectionPending = false;
	PreviousComboMemory = {};
	ComboMemory = {};
	bTransitionStarted = false;
	bEvaluating = false;
}


/** 消费已缓存输入。 */
void UHodgeCombatComponentBase::ConsumeBufferedInput()
{
	BufferedInput = FGameplayTag(); InputExpiresAt = 0; BufferedActionRequest.Invalidate();
}

/** 清空输入。 */
void UHodgeCombatComponentBase::ClearInput()
{
	const FGuid Previous = BufferedActionRequest;
	ConsumeBufferedInput();
	SetActionRequestStatus(Previous, EHodgeCombatActionRequestStatus::Cancelled, TEXT("Input cleared or replaced"));
}

/** 将按下的输入Tag交给现有连段意图映射。 */
bool UHodgeCombatComponentBase::InputPressed(FGameplayTag InputTag)
{
	if (!IsComboReady()) { return false; }
	for (const auto& Binding : Definition->InputBindings)
	{
		if (Binding.InputTag != InputTag) { continue; }
		SubmitCombatIntent(Binding.IntentTag);
		return true;
	}
	return false;
}

/** 按当前意图与窗口条件选择连段转换边。 */
const FHodgeComboTransition* UHodgeCombatComponentBase::SelectTransition(FGameplayTag Trigger, bool bEvent) const
{
	// 本次操作的来源。
	const FGameplayTag Source = TransitionSourceNode();
	const auto* Node = Definition ? Definition->FindNode(Source) : nullptr;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
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

/** 判断Authorized。 */
bool UHodgeCombatComponentBase::IsAuthorized(FGameplayAbilitySpecHandle Handle) const
{
	return IsComboReady() && Handle.IsValid() && AuthorizedHandle == Handle;
}

/** 判断移动取消Predicted。 */
bool UHodgeCombatComponentBase::IsMoveCancelPredicted() const
{
	return bMoveRequestPending && PredictedMoveCancelAbility.IsValid() && CurrentAbility == PredictedMoveCancelAbility.Get()
		&& CurrentAbility->IsActive() && CurrentAbility->GetCurrentAbilitySpecHandle() == PendingMoveHandle
		&& ExecutionKey() == PendingMoveKey;
}

/** 读取当前动作与网络确认使用的激活键。 */
int16 UHodgeCombatComponentBase::ExecutionKey() const
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!CurrentAbility) { return GetRememberedComboTag().IsValid() ? ComboMemory.ExecutionKey : 0; }
	const auto Key = CurrentAbility->GetCurrentActivationInfo().GetActivationPredictionKey();
	return Key.IsServerInitiatedKey() ? -Key.Current : Key.Current;
}

/** 准备转换。 */
bool UHodgeCombatComponentBase::PrepareTransition(const FHodgeComboTransition& Edge, FGameplayAbilitySpecHandle Handle)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(Handle);
	const auto* AttackDefinition = ASC->FindAbilityDefinition(Handle);
	if (!AttackDefinition || AttackDefinition->ExecutionRoute != EHodgeAbilityExecutionRoute::ComboCoordinated)
	{
		return false;
	}
	// 根据当前生命周期状态决定是否继续执行。
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
	ConsumeBufferedInput();
	if (CurrentAbility) { CurrentAbility->FinishExecution(true, false); }
	// 结束旧技能可能同步卸载组件或更换 Pawn，不能继续使用已清理的连招配置。
	if (!IsComboReady() || !bSwitching) { return false; }
	CurrentAbility = nullptr;
	SetNode(Definition->EntryComboTag);
	return true;
}

/** 尝试转换。 */
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
	// 本次操作的目标。
	const auto* Target = Definition->FindNode(Edge->TargetComboTag);
	if (!Target) { return false; }
	// 本次请求的唯一身份。
	const FGuid RequestId = bEvent ? FGuid() : BufferedActionRequest;
	if (Target->ComboTag == Definition->EntryComboTag)
	{
		ConsumeBufferedInput();
		SetActionRequestStatus(RequestId, EHodgeCombatActionRequestStatus::Completed);
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
	BindActionRequest(RequestId, Handle);
	const FPredictionKey Key = bEvent && GetOwner()->HasAuthority()
		                           ? FPredictionKey::CreateNewServerInitiatedKey(ASC)
		                           : FPredictionKey();
	// 服务器发起的激活把固定方向放入原事件载荷，拥有者不另行重采样。
	if (Key.IsServerInitiatedKey())
	{
		const auto* AttackData=ASC->FindAbilityDefinition(Handle); FHodgeAttackFacingTargetData Facing;
		if (AttackData && AttackData->FacingWarp.bEnabled && UHodgeGameplayAbility_Definition::BuildAttackFacingData(ASC->AbilityActorInfo.Get(),Facing))
		{ Payload.TargetData.Add(new FHodgeAttackFacingTargetData(Facing)); }
	}
	// 通过ASC发起激活，保留GAS自身的条件与消耗校验。
	const bool bActivated = ASC->InternalTryActivateAbility(Handle, Key, nullptr, nullptr, &Payload);
	CompleteServerActivation();
	ConfirmActionRequest(RequestId, bActivated);
	return bActivated && (CurrentAbility != nullptr || bSwitching);
}

/** 获取服务器输入缓存秒。 */
float UHodgeCombatComponentBase::GetServerInputBufferSeconds() const
{
	return Definition ? FMath::Max(0.01f, Definition->InputBufferSeconds) : 0.f;
}

/** 校验服务器请求身份。 */
bool UHodgeCombatComponentBase::ValidateServerRequestIdentity(const FGameplayEventData* Payload) const
{
	// 来源、预测身份与输入类型先核对，不能缓存客户端自行声明的权威玩法消息。
	return IsComboReady() && !ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked)
		&& Payload && Payload->OptionalObject == Definition && Payload->Instigator == ASC->GetAvatarActor()
		&& Payload->TargetTags.Num() == 1 && Payload->InstigatorTags.Num() == 1
		&& Payload->TargetTags.HasTagExact(TransitionSourceNode()) && Payload->EventMagnitude == ExecutionKey()
		&& Payload->EventTag == TAG_ComboInputRequest;
}

/** 检查是否允许缓存服务器Activation。 */
bool UHodgeCombatComponentBase::CanBufferServerActivation(FGameplayAbilitySpecHandle Handle,
                                                          const FGameplayEventData* Payload) const
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
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

/** 准备服务器Activation。 */
bool UHodgeCombatComponentBase::PrepareServerActivation(FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayEventData* Payload)
{
	ExpireComboMemory();
	if (!ValidateServerRequestIdentity(Payload)) { return false; }
	TGuardValue<bool> Guard(bEvaluating, true);
	const auto* Edge = SelectTransition(Payload->InstigatorTags.First(), false);
	// 本次操作的目标。
	const auto* Target = Edge ? Definition->FindNode(Edge->TargetComboTag) : nullptr;
	if (!Target || ASC->FindDefinitionAbility(Target->AbilityTag) != Handle) { return false; }
	return PrepareTransition(*Edge, Handle);
}

/** 准备ConfirmedActivation。 */
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

/** 拒绝当前服务器激活请求并清理对应记录。 */
void UHodgeCombatComponentBase::RejectServerActivation(const FGameplayEventData* Payload)
{
	// 拒绝请求不能清除服务器已经确认的动作或连段记忆。
	ExpireComboMemory();
}

/** 完成服务器Activation。 */
void UHodgeCombatComponentBase::CompleteServerActivation()
{
	if (!bSwitching) { return; }
	const auto* PendingSpec=ASC ? ASC->FindAbilitySpecFromHandle(AuthorizedHandle) : nullptr;
	const auto* PendingAbility=PendingSpec ? Cast<UHodgeGameplayAbility_Definition>(PendingSpec->GetPrimaryInstance()) : nullptr;
	// 目标数据等待是本次激活的一部分，不能提前丢弃连段授权。
	if (PendingAbility && PendingAbility->IsActive() && PendingAbility->IsAwaitingFacingData()) { return; }
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

/** 记录当前动作开始并更新连段执行状态。 */
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

/** 接收当前动作结束结果并收敛连段状态。 */
void UHodgeCombatComponentBase::ExecutionEnded(UHodgeGameplayAbility_Definition* Ability)
{
	if (CurrentAbility != Ability) { return; }
	CurrentAbility = nullptr;
	// 按服务器权限区分本次处理分支。
	if (!bSwitching)
	{
		RetainComboMemory();
		QueuedEvents.Reset();
		ClearInput();
		SetNode(Definition ? Definition->EntryComboTag : FGameplayTag());
		if (!GetOwner()->HasAuthority()) { OnRep_ComboMemory(); }
	}
}

/** 在动画窗口变化时尝试推进连段或取消状态。 */
void UHodgeCombatComponentBase::WindowsChanged(UHodgeGameplayAbility_Definition* Ability)
{
	// 按服务器权限区分本次处理分支。
	if (!IsComboReady() || CurrentAbility != Ability || bMoveRequestPending || (!GetOwner()->HasAuthority() && !ASC->AbilityActorInfo->IsLocallyControlled())) { return; }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (BufferedInput.IsValid() && GetWorld()->GetTimeSeconds() < InputExpiresAt)
	{
		TryTransition(BufferedInput, false);
	}
}

/** 接收当前动作发出的语义事件。 */
void UHodgeCombatComponentBase::ExecutionEvent(UHodgeGameplayAbility_Definition* Ability, FGameplayTag Event)
{
	// 按服务器权限区分本次处理分支。
	if (!IsComboReady() || CurrentAbility != Ability || !GetOwner()->HasAuthority()) { return; }
	if (bSwitching || bEvaluating) { QueuedEvents.Add({Ability, ExecutionKey(), Event}); }
	else { TryTransition(Event, true); }
}

/** 按队列顺序消费当前待处理事件。 */
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

/** 设置节点。 */
void UHodgeCombatComponentBase::SetNode(FGameplayTag Node)
{
	FGameplayTagContainer Previous = MoveTemp(OwnedNodeTags);
	OwnedNodeTags.Reset();
	CurrentComboTag = Node;
	if (ASC) { ASC->RemoveLooseGameplayTags(Previous); }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (const auto* Row = Definition ? Definition->FindNode(Node) : nullptr)
	{
		OwnedNodeTags = Row->GrantedTags;
		if (ASC) { ASC->AddLooseGameplayTags(OwnedNodeTags); }
	}
	// 按服务器权限区分本次处理分支。
	if (GetOwner()->HasAuthority()) { ObserverTags = OwnedNodeTags; }
}

/** 重置会话。 */
void UHodgeCombatComponentBase::ResetSession(bool bEndAbility)
{
	TGuardValue<bool> Guard(bSwitching, true);
	auto* Previous = CurrentAbility.Get();
	CurrentAbility = nullptr;
	QueuedEvents.Reset();
	ClearInput();
	// 按服务器权限区分本次处理分支。
	if (bEndAbility && Previous) { Previous->FinishExecution(true, GetOwner()->HasAuthority()); }
	SetNode(Definition ? Definition->EntryComboTag : FGameplayTag());
	ComboMemory.Node = FGameplayTag();
	ComboMemory.ExpiresAt = 0;
	PublishComboMemory();
}

/** 读取当前连段计时基准。 */
double UHodgeCombatComponentBase::ComboTime() const
{
	// 本次操作所在的世界。
	const auto* World = GetWorld();
	const auto* State = World ? World->GetGameState() : nullptr;
	return State ? State->GetServerWorldTimeSeconds() : (World ? World->GetTimeSeconds() : 0.0);
}

/** 获取Remembered连段标签。 */
FGameplayTag UHodgeCombatComponentBase::GetRememberedComboTag() const
{
	return ComboMemory.Node.IsValid() && (CurrentAbility || ComboMemory.ExpiresAt > ComboTime())
		       ? ComboMemory.Node
		       : FGameplayTag();
}

/** 获取连段记忆Remaining时间。 */
float UHodgeCombatComponentBase::GetComboMemoryRemainingTime() const
{
	return !CurrentAbility && GetRememberedComboTag().IsValid()
		       ? static_cast<float>(FMath::Max(0.0, ComboMemory.ExpiresAt - ComboTime()))
		       : 0.f;
}

/** 取得本次连段转换使用的源节点。 */
FGameplayTag UHodgeCombatComponentBase::TransitionSourceNode() const
{
	if (CurrentAbility) { return CurrentComboTag; }
	const FGameplayTag Memory = GetRememberedComboTag();
	return Memory.IsValid() ? Memory : CurrentComboTag;
}

/** 判断是否存在恢复转换。 */
bool UHodgeCombatComponentBase::HasResumeTransition(FGameplayTag Node) const
{
	const auto* Row = Definition ? Definition->FindNode(Node) : nullptr;
	return Row && Row->Transitions.ContainsByPredicate([](const FHodgeComboTransition& Edge)
	{
		return Edge.bAllowAfterExecutionEnded && Edge.TriggerInputIntentTag.IsValid();
	});
}

/** 记录当前允许保留的连段节点与有效时间。 */
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

/** 按现有规则清除已经过期的连段记忆。 */
void UHodgeCombatComponentBase::ExpireComboMemory()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!CurrentAbility && ComboMemory.Node.IsValid() && ComboMemory.ExpiresAt <= ComboTime())
	{
		ComboMemory.Node = FGameplayTag();
		ComboMemory.ExpiresAt = 0;
		PublishComboMemory();
	}
}

/** 发布当前连段记忆及其修订信息。 */
void UHodgeCombatComponentBase::PublishComboMemory()
{
	// 按服务器权限区分本次处理分支。
	if (GetOwner()->HasAuthority())
	{
		ReplicatedComboMemory = ComboMemory;
		GetOwner()->ForceNetUpdate();
	}
}

/** 结束当前执行。 */
void UHodgeCombatComponentBase::EndCurrentExecution()
{
	// 按服务器权限区分本次处理分支。
	if (CurrentAbility) { CurrentAbility->FinishExecution(true, GetOwner()->HasAuthority()); }
}

/** 收到连段记忆复制后更新本地状态。 */
void UHodgeCombatComponentBase::OnRep_ComboMemory()
{
	if (IsComboReady() && !CurrentAbility && !bSwitching
		&& (ComboMemory.ExecutionKey == 0 || ComboMemory.ExecutionKey == ReplicatedComboMemory.ExecutionKey))
	{
		ComboMemory = ReplicatedComboMemory;
	}
}

/** 处理预测Rejected。 */
void UHodgeCombatComponentBase::HandlePredictionRejected()
{
	// 按服务器权限区分本次处理分支。
	if (!IsComboReady() || GetOwner()->HasAuthority()) { return; }
	ClearInput();
	bMemoryCorrectionPending = true;
	ServerSynchronizeComboMemory();
}

/** 处理服务器侧的同步连段记忆消息，沿用当前身份与权限校验。 */
void UHodgeCombatComponentBase::ServerSynchronizeComboMemory_Implementation()
{
	ExpireComboMemory();
	ClientCorrectComboMemory(ComboMemory);
}

/** 处理客户端侧的Correct连段记忆消息，沿用当前身份与权限校验。 */
void UHodgeCombatComponentBase::ClientCorrectComboMemory_Implementation(FHodgeComboMemoryState State)
{
	bMemoryCorrectionPending = false;
	if (IsComboReady() && !CurrentAbility && !bSwitching) { ComboMemory = State; }
}

/** 更新组件。 */
void UHodgeCombatComponentBase::TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Tick)
{
	Super::TickComponent(Delta, Type, Tick);
	DrainEvents();
	ExpireComboMemory();
	// 按服务器权限区分本次处理分支。
	if (!IsComboReady() || (!GetOwner()->HasAuthority() && !ASC->AbilityActorInfo->IsLocallyControlled()))
	{
		return;
	}
	if (ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
	{
		ClearInput();
		return;
	}
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (BufferedInput.IsValid() && GetWorld()->GetTimeSeconds() >= InputExpiresAt)
	{
		const FGuid Expired = BufferedActionRequest;
		ConsumeBufferedInput();
		SetActionRequestStatus(Expired, EHodgeCombatActionRequestStatus::Expired, TEXT("Combo input buffer expired"));
	}
	if (bMoveRequestPending) { return; }
	if (BufferedInput.IsValid() && TryTransition(BufferedInput, false)) { return; }
	if (!CurrentAbility || bSwitching || !ASC->AbilityActorInfo->IsLocallyControlled()) { return; }
	const auto* Avatar = ASC->GetAvatarActor();
	const auto* Hero = Avatar ? Avatar->FindComponentByClass<UHodgeHeroComponent>() : nullptr;
	if (!Hero || !Hero->HasMoveIntent(Definition->MoveIntentThreshold))
	{ RejectedMoveHandle = {}; RejectedMoveKey = 0; return; }
	if (RejectedMoveHandle == CurrentAbility->GetCurrentAbilitySpecHandle() && RejectedMoveKey == ExecutionKey()) { return; }
	if (Hero && Hero->HasMoveIntent(Definition->MoveIntentThreshold) && CurrentAbility->CanBeCanceled()
		&& CurrentAbility->GetExecutionWindows().HasTag(Definition->MoveCancelWindowTag))
	{
		bMoveRequestPending = true;
		PendingMoveHandle = CurrentAbility->GetCurrentAbilitySpecHandle();
		PendingMoveKey = ExecutionKey();
		if (CurrentAbility->BeginPredictedMoveCancel()) { PredictedMoveCancelAbility = CurrentAbility; }
		ServerMoveCancel(ASC->GetAvatarActor(), PendingMoveHandle, PendingMoveKey, Hero->GetMoveIntent());
	}
}

/** 处理服务器侧的移动取消消息，沿用当前身份与权限校验。 */
void UHodgeCombatComponentBase::ServerMoveCancel_Implementation(AActor* Avatar, FGameplayAbilitySpecHandle Handle,
                                                                int32 Key, FVector2D Intent)
{
	if (!GetWorld())
	{
		ClientMoveCancelResult(Avatar, Handle, Key, true, 0.f, 0.f, 0.f);
		return;
	}
	TWeakObjectPtr<AActor> WeakAvatar = Avatar;
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this,
		[this, WeakAvatar, Handle, Key, Intent]() { ProcessServerMoveCancel(WeakAvatar, Handle, Key, Intent); }));
}

/** 处理服务器移动取消。 */
void UHodgeCombatComponentBase::ProcessServerMoveCancel(TWeakObjectPtr<AActor> Avatar,
                                                        FGameplayAbilitySpecHandle Handle, int32 Key, FVector2D Intent)
{
	// RPC 可能早于本帧 NotifyBegin 到达；等动画更新完成后再检查，不能提前授予取消权限。
	const bool bSameExecution = Avatar.IsValid() && IsComboReady() && CurrentAbility && Avatar.Get() == GetOwner()
		&& Handle == CurrentAbility->GetCurrentAbilitySpecHandle() && Key == ExecutionKey();
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (bSameExecution && !Intent.ContainsNaN() && Intent.SizeSquared() <= 2.01f
		&& Intent.SizeSquared() >= FMath::Square(Definition->MoveIntentThreshold) && CurrentAbility->CanBeCanceled()
		&& !ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked)
		&& CurrentAbility->GetExecutionWindows().HasTag(Definition->MoveCancelWindowTag))
	{
		EndCurrentExecution();
	}
	const bool bStillExecuting = bSameExecution && CurrentAbility &&
		CurrentAbility->GetCurrentAbilitySpecHandle() == Handle && ExecutionKey() == Key;
	// 当前网格使用的动画实例。
	UAnimInstance* Anim = bStillExecuting && ASC->AbilityActorInfo.IsValid() ? ASC->AbilityActorInfo->GetAnimInstance() : nullptr;
	UAnimMontage* Montage = bStillExecuting ? CurrentAbility->GetDefinition()->ExecutionConfig.Montage.Get() : nullptr;
	ClientMoveCancelResult(Avatar.Get(), Handle, Key, !bStillExecuting,
		Anim && Montage ? Anim->Montage_GetPosition(Montage) : 0.f, ComboTime(),
		Anim && Montage ? Anim->Montage_GetPlayRate(Montage) : 0.f);
}

/** 处理客户端侧的移动取消结果消息，沿用当前身份与权限校验。 */
void UHodgeCombatComponentBase::ClientMoveCancelResult_Implementation(AActor* Avatar, FGameplayAbilitySpecHandle Handle,
	int32 Key, bool bEnd, float ServerPosition, float ServerTime, float ServerPlayRate)
{
	if (!bMoveRequestPending || Avatar != GetOwner() || Handle != PendingMoveHandle || Key != PendingMoveKey) { return; }
	const TWeakObjectPtr<UHodgeGameplayAbility_Definition> Predicted = PredictedMoveCancelAbility;
	const FGameplayTag Input = BufferedInput;
	const double Expires = InputExpiresAt;
	const FGuid InputRequest = BufferedActionRequest;
	ConsumeBufferedInput();
	if (!bEnd) { RejectedMoveHandle = Handle; RejectedMoveKey = Key; }
	bMoveRequestPending = false;
	PredictedMoveCancelAbility.Reset();
	PendingMoveHandle = {};
	PendingMoveKey = 0;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Predicted.IsValid() && Predicted->IsActive() && CurrentAbility == Predicted.Get() && ExecutionKey() == Key)
	{ Predicted->ResolvePredictedMoveCancel(bEnd, ServerPosition, ServerTime, ServerPlayRate); }
	FHodgeCombatActionRequestState Saved;
	const bool bRequestQueued = !InputRequest.IsValid() || (GetCombatActionRequest(InputRequest, Saved) && Saved.Status == EHodgeCombatActionRequestStatus::Queued);
	if (IsComboReady() && Input.IsValid() && bRequestQueued && !BufferedInput.IsValid() && GetWorld()->GetTimeSeconds() < Expires)
	{ BufferedInput = Input; InputExpiresAt = Expires; BufferedActionRequest = InputRequest; }
	else
	{ SetActionRequestStatus(InputRequest, EHodgeCombatActionRequestStatus::Cancelled, TEXT("Buffered action superseded during movement resolution")); }
}

/** 处理服务器侧的返回目标条目消息，沿用当前身份与权限校验。 */
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

/** 登记本类型需要网络复制的属性及复制条件。 */
void UHodgeCombatComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UHodgeCombatComponentBase, ObserverTags, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UHodgeCombatComponentBase, ReplicatedComboMemory, COND_OwnerOnly);
}

/** 收到观察者标签复制后更新本地状态。 */
void UHodgeCombatComponentBase::OnRep_ObserverTags(const FGameplayTagContainer& Previous)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (IsValid(ASC) && ASC->GetAvatarActor() == GetOwner() && GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		// 只撤销本组件实际应用的标签，重复绑定不会叠加其他来源的计数。
		ASC->RemoveLooseGameplayTags(AppliedObserverTags);
		AppliedObserverTags = ObserverTags;
		ASC->AddLooseGameplayTags(AppliedObserverTags);
	}
}

/** 查找战斗组件。 */
UHodgeCombatComponentBase* UHodgeCombatComponentBase::FindCombatComponent(const AActor* Avatar)
{
	return IsValid(Avatar) ? Avatar->FindComponentByClass<UHodgeCombatComponentBase>() : nullptr;
}

/** 判断连段就绪。 */
bool UHodgeCombatComponentBase::IsComboReady() const
{
	// 旧 Pawn 的组件不能继续操作已经切换 Avatar 的 PlayerState ASC。
	return !bShuttingDown && IsRegistered() && IsValid(ASC) && Definition &&
		ASC->AbilityActorInfo.IsValid() && ASC->GetAvatarActor() == GetOwner() &&
		!ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death);
}

/** 绑定Pawn角色扩展。 */
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

/** 处理能力SystemInitialized。 */
void UHodgeCombatComponentBase::HandleAbilitySystemInitialized()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!IsRegistered() || !PawnExtension.IsValid()) { return; }
	const auto* PawnData = PawnExtension->GetPawnData<UHodgePawnData>();
	Configure(PawnExtension->GetHodgeAbilitySystemComponent(), PawnData ? PawnData->ComboDefinition.Get() : nullptr);
	if (auto* Init = GetOwner()->FindComponentByClass<UHodgeEnemyInitComponent>()) { Init->TryInitialize(); }
}

/** 处理能力SystemUninitialized。 */
void UHodgeCombatComponentBase::HandleAbilitySystemUninitialized()
{
	Shutdown();
}

/** 组件注册后接入当前Owner的依赖。 */
void UHodgeCombatComponentBase::OnRegister()
{
	Super::OnRegister();
	BindPawnExtension();
	if (auto* Init = GetOwner()->FindComponentByClass<UHodgeEnemyInitComponent>()) { Init->TryInitialize(); }
}

/** 进入世界运行阶段并接入本对象的生命周期依赖。 */
void UHodgeCombatComponentBase::BeginPlay()
{
	Super::BeginPlay();
	// 覆盖动态添加时其他默认组件尚未完成注册的顺序。
	BindPawnExtension();
}

/** 组件注销时解除依赖并清理本组件资源。 */
void UHodgeCombatComponentBase::OnUnregister()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (PawnExtension.IsValid()) { PawnExtension->UnregisterAbilitySystemDelegates(this); }
	PawnExtension.Reset();
	Shutdown();
	Super::OnUnregister();
}


/** 检查当前Avatar绑定、生命和战斗初始化依赖。 */
bool UHodgeCombatComponentBase::IsCombatReadyForActions() const
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!CanExecuteAbilities() || !IsValid(ASC) || !GetOwner() || GetOwner()->IsActorBeingDestroyed() ||
		!ASC->AbilityActorInfo.IsValid() || ASC->GetAvatarActor() != GetOwner() ||
		ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death)) { return false; }
	if (const auto* Health = ASC->GetSet<UHodgeHealthSet>(); Health && Health->GetHealth() <= 0.f) { return false; }
	if (const auto* Init = GetOwner()->FindComponentByClass<UHodgeEnemyInitComponent>())
	{ return Init->IsGameplayReady() && Init->HasValidCombatDependencies(); }
	if (GetOwner()->IsA<AHodgeHeroCharacter>())
	{
		const auto* State = Cast<AHodgePlayerState>(ASC->GetOwnerActor());
		return State && State->AreAttributesReadyFor(Cast<APawn>(GetOwner()));
	}
	return true;
}

/** 添加动作请求。 */
FHodgeCombatActionRequestState UHodgeCombatComponentBase::AddActionRequest(EHodgeCombatActionRequestType Type, FGameplayTag Tag)
{
	FHodgeCombatActionRequestState State;
	State.RequestId = FGuid::NewGuid(); State.Type = Type; State.ActionTag = Tag;
	if (bSubmittingAction) { State.Reason = TEXT("Reentrant action submission rejected"); return State; }
	int32 Pending = 0;
	for (const auto& Pair : ActionRequests) { Pending += !Pair.Value.State.IsTerminal(); }
	if (Pending >= 16) { State.Reason = TEXT("Too many pending combat requests"); return State; }
	// 终结记录最多保留 64 项；活动请求不会因为历史裁剪而丢失。
	while (ActionRequestOrder.Num() >= 64)
	{
		const int32 Index = ActionRequestOrder.IndexOfByPredicate([this](FGuid Id) { return ActionRequests.FindChecked(Id).State.IsTerminal(); });
		if (Index == INDEX_NONE) { State.Reason = TEXT("Combat request history full"); return State; }
		ActionRequests.Remove(ActionRequestOrder[Index]); ActionRequestOrder.RemoveAt(Index);
	}
	State.Status = EHodgeCombatActionRequestStatus::Queued;
	ActionRequests.Add(State.RequestId).State = State; ActionRequestOrder.Add(State.RequestId);
	return State;
}

/** 获取共享战斗请求数据。 */
bool UHodgeCombatComponentBase::GetCombatActionRequest(FGuid Id, FHodgeCombatActionRequestState& OutState) const
{
	if (const auto* Record = ActionRequests.Find(Id)) { OutState = Record->State; return true; }
	OutState = {}; OutState.RequestId = Id; OutState.Reason = TEXT("Unknown or evicted request"); return false;
}

/** 更新请求状态，并按队列顺序分发原生和蓝图通知。 */
void UHodgeCombatComponentBase::SetActionRequestStatus(FGuid Id, EHodgeCombatActionRequestStatus Status, const FString& Reason)
{
	auto* Record = ActionRequests.Find(Id);
	if (!Record || Record->State.IsTerminal()) { return; }
	Record->State.Status = Status; Record->State.Reason = Reason;
	PendingActionNotifications.Add(Record->State);
	if (bNotifyingActions) { return; }
	TGuardValue<bool> Guard(bNotifyingActions, true);
	// 重入产生的终结事件排在当前事件之后，原生与蓝图监听者看到相同顺序。
	int32 Budget = 64;
	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	while (!PendingActionNotifications.IsEmpty() && Budget-- > 0)
	{
		const FHodgeCombatActionRequestState Snapshot = PendingActionNotifications[0];
		PendingActionNotifications.RemoveAt(0);
		OnActionRequestChanged.Broadcast(Snapshot);
		OnCombatActionRequestChanged.Broadcast(Snapshot);
	}
	if (!PendingActionNotifications.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Combat request notification chain exceeded 64 events"));
		PendingActionNotifications.Reset();
	}
}

/** 校验独立技能请求并通过现有ASC执行激活。 */
FHodgeCombatActionRequestState UHodgeCombatComponentBase::RequestStandaloneAbility(FGameplayTag Tag)
{
	auto State = AddActionRequest(EHodgeCombatActionRequestType::StandaloneAbility, Tag);
	if (State.IsTerminal()) { return State; }
	TGuardValue<bool> Guard(bSubmittingAction, true);
	FString Error;
	// 按服务器权限区分本次处理分支。
	if (!Tag.IsValid()) { Error = TEXT("Invalid AbilityTag"); }
	else if (!IsCombatReadyForActions() || (!GetOwner()->HasAuthority() && !ASC->AbilityActorInfo->IsLocallyControlled())) { Error = TEXT("Combat or local activation context not ready"); }
	else if (ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked)) { Error = TEXT("Combat input blocked"); }
	const FGameplayAbilitySpecHandle Handle = Error.IsEmpty() ? ASC->FindDefinitionAbility(Tag) : FGameplayAbilitySpecHandle();
	const auto* Data = Handle.IsValid() ? ASC->FindAbilityDefinition(Handle) : nullptr;
	const auto* Spec = Handle.IsValid() ? ASC->FindAbilitySpecFromHandle(Handle) : nullptr;
	if (Error.IsEmpty() && (!Data || !Spec)) { Error = TEXT("Missing or ambiguous granted Definition"); }
	if (Error.IsEmpty() && Data->ExecutionRoute != EHodgeAbilityExecutionRoute::Standalone) { Error = TEXT("Combo ability requires an intent and coordinator authorization"); }
	// 根据当前生命周期状态决定是否继续执行。
	if (Error.IsEmpty() && Spec->IsActive()) { Error = TEXT("Ability already active"); }
	// 通过ASC发起激活，保留GAS自身的条件与消耗校验。
	if (!Error.IsEmpty()) { SetActionRequestStatus(State.RequestId, EHodgeCombatActionRequestStatus::Rejected, Error); }
	else
	{
		BindActionRequest(State.RequestId, Handle);
		const bool bActivated = ASC->TryActivateAbility(Handle);
		ConfirmActionRequest(State.RequestId, bActivated);
	}
	GetCombatActionRequest(State.RequestId, State); return State;
}

/** 提交语义意图，由连段图选择并授权实际动作。 */
FHodgeCombatActionRequestState UHodgeCombatComponentBase::SubmitCombatIntent(FGameplayTag Tag)
{
	auto State = AddActionRequest(EHodgeCombatActionRequestType::ComboIntent, Tag);
	if (State.IsTerminal()) { return State; }
	TGuardValue<bool> Guard(bSubmittingAction, true);
	FString Error;
	// 按服务器权限区分本次处理分支。
	if (!IsComboReady() || !IsCombatReadyForActions() || (!GetOwner()->HasAuthority() && !ASC->AbilityActorInfo->IsLocallyControlled())) { Error = TEXT("Combo or local activation context not ready"); }
	else if (ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked)) { Error = TEXT("Combat input blocked"); }
	bool bKnownIntent = Error.IsEmpty() && Tag.IsValid() && Definition->InputBindings.ContainsByPredicate(
		[Tag](const FHodgeComboInputBinding& Binding) { return Binding.IntentTag == Tag; });
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Error.IsEmpty() && Tag.IsValid())
	{
		for (const auto& Pair : Definition->ComboTable->GetRowMap())
		{
			const auto* Row = reinterpret_cast<const FHodgeComboRow*>(Pair.Value);
			bKnownIntent |= Row->Transitions.ContainsByPredicate([Tag](const FHodgeComboTransition& Edge) { return Edge.TriggerInputIntentTag == Tag; });
		}
	}
	if (Error.IsEmpty() && !bKnownIntent) { Error = TEXT("Unknown combat intent"); }
	if (!Error.IsEmpty()) { SetActionRequestStatus(State.RequestId, EHodgeCombatActionRequestStatus::Rejected, Error); }
	else
	{
		ClearInput();
		BufferedInput = Tag; BufferedActionRequest = State.RequestId;
		InputExpiresAt = GetWorld()->GetTimeSeconds() + Definition->InputBufferSeconds;
		if (!bMoveRequestPending) { TryTransition(Tag, false); }
		FHodgeCombatActionRequestState Current;
		if (GetCombatActionRequest(State.RequestId, Current) && Current.Status == EHodgeCombatActionRequestStatus::Queued)
		{ SetActionRequestStatus(State.RequestId, EHodgeCombatActionRequestStatus::Queued); }
	}
	GetCombatActionRequest(State.RequestId, State); return State;
}

/** 绑定动作请求。 */
void UHodgeCombatComponentBase::BindActionRequest(FGuid Id, FGameplayAbilitySpecHandle Handle)
{
	if (auto* Record = ActionRequests.Find(Id); Record && !Record->State.IsTerminal())
	{
		Record->State.AbilityHandle = Handle; Record->bActivating = true;
		const auto* Spec = ASC ? ASC->FindAbilitySpecFromHandle(Handle) : nullptr;
		Record->Ability = Spec ? Cast<UHodgeGameplayAbility_Definition>(Spec->GetPrimaryInstance()) : nullptr;
	}
}

/** 根据实际GAS激活结果提交请求的执行状态。 */
void UHodgeCombatComponentBase::ConfirmActionRequest(FGuid Id, bool bActivated)
{
	auto* Record = ActionRequests.Find(Id);
	if (!Record || Record->State.IsTerminal()) { return; }
	Record->bActivating = false;
	const auto* Spec = ASC ? ASC->FindAbilitySpecFromHandle(Record->State.AbilityHandle) : nullptr;
	auto* Ability = Spec ? Cast<UHodgeGameplayAbility_Definition>(Spec->GetPrimaryInstance()) : nullptr;
	// 根据当前生命周期状态决定是否继续执行。
	if (!bActivated || !Ability || !Ability->IsActive() || !IsCombatReadyForActions())
	{ SetActionRequestStatus(Id, EHodgeCombatActionRequestStatus::Rejected, TEXT("GAS activation failed or execution context invalidated")); return; }
	Record->Ability = Ability; Record->State.ExecutionId = Ability->GetExecutionId();
	SetActionRequestStatus(Id, EHodgeCombatActionRequestStatus::Started);
}

/** 将匹配的技能结束结果写入对应战斗请求。 */
void UHodgeCombatComponentBase::HandleActionAbilityEnded(const FAbilityEndedData& Data)
{
	const TArray<FGuid> Snapshot = ActionRequestOrder;
	for (FGuid Id : Snapshot)
	{
		const auto* Record = ActionRequests.Find(Id);
		if (!Record || Record->State.IsTerminal() || Record->State.AbilityHandle != Data.AbilitySpecHandle ||
			(!Record->bActivating && Record->State.Status != EHodgeCombatActionRequestStatus::Started) ||
			(Record->Ability.IsValid() && Record->Ability.Get() != Data.AbilityThatEnded)) { continue; }
		// Definition 正常结束会清空身份；有效的新身份才代表同实例的另一轮执行。
		if (Record->State.ExecutionId.IsValid() && Record->Ability.IsValid() && Record->Ability->GetExecutionId().IsValid() &&
			Record->State.ExecutionId != Record->Ability->GetExecutionId()) { continue; }
		SetActionRequestStatus(Id, Data.bWasCancelled ? EHodgeCombatActionRequestStatus::Cancelled : EHodgeCombatActionRequestStatus::Completed);
	}
}

/** 按请求身份取消其对应的缓存或技能执行。 */
bool UHodgeCombatComponentBase::CancelCombatActionRequest(FGuid Id)
{
	const auto* Record = ActionRequests.Find(Id);
	if (!Record || Record->State.IsTerminal()) { return false; }
	if (Record->State.Status == EHodgeCombatActionRequestStatus::Queued && !Record->bActivating)
	{
		if (BufferedActionRequest == Id) { ConsumeBufferedInput(); }
		SetActionRequestStatus(Id, EHodgeCombatActionRequestStatus::Cancelled, TEXT("Queued action cancelled")); return true;
	}
	// 按服务器权限区分本次处理分支。
	if (Record->State.Type == EHodgeCombatActionRequestType::ComboIntent && !GetOwner()->HasAuthority()) { return false; }
	const auto Ability = Record->Ability;
	const FGuid Execution = Record->State.ExecutionId;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!ASC || ASC->GetAvatarActor() != GetOwner() || !Ability.IsValid() || !Ability->IsActive() || !Ability->CanBeCanceled() ||
		Ability->GetCurrentAbilitySpecHandle() != Record->State.AbilityHandle || Ability->GetExecutionId() != Execution) { return false; }
	Ability->FinishExecution(true, true); return true;
}

/** 关闭动作Requests。 */
void UHodgeCombatComponentBase::CloseActionRequests()
{
	if (ASC) { ASC->OnAbilityEnded.Remove(ActionEndedHandle); }
	ActionEndedHandle.Reset();
	const TArray<FGuid> Snapshot = ActionRequestOrder;
	for (FGuid Id : Snapshot)
	{
		CancelCombatActionRequest(Id);
		SetActionRequestStatus(Id, EHodgeCombatActionRequestStatus::Cancelled, TEXT("Combat binding ended"));
	}
}
