/** @file HodgeHitDetectionComponent.cpp @brief 复用既有几何策略，集中保存检测运行状态。 */
#include "Component/HodgeHitDetectionComponent.h"
#include "Equipment/HodgeEquipmentManagerComponent.h"
#include "Equipment/HodgeWeaponInstance.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Component/HodgeWingComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHitDetectionComponent)
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

/** 检测服务由 Task 驱动，不增加第二条采样 Tick。 */
UHodgeHitDetectionComponent::UHodgeHitDetectionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

/** 采样等待角色与实际来源骨骼完成刷新，不注册额外 Tick。 */
void UHodgeHitDetectionComponent::AddSamplingPrerequisites(FTickFunction& Tick, uint64 Handle, const FGuid& ExecutionId) const
{
	if (const auto* Character = Cast<ACharacter>(GetOwner()))
	{ if (auto* Mesh = Character->GetMesh()) { Tick.AddPrerequisite(Mesh, Mesh->PrimaryComponentTick); } }
	if (auto* Source = GetDetectionSourceComponent(Handle, ExecutionId))
	{
		Tick.AddPrerequisite(Source, Source->PrimaryComponentTick);
		if (auto* Wing = GetOwner()->FindComponentByClass<UHodgeWingComponent>(); Wing && Wing->GetWingMesh() == Source)
		{ Tick.AddPrerequisite(Wing, Wing->PrimaryComponentTick); }
	}
}

/** 独立动画翼在首次几何历史重置之前准备权威姿势。 */
bool UHodgeHitDetectionComponent::PrepareSourcePose(uint64 Handle, const FGuid& ExecutionId) const
{
	if (!CanExecuteAbilities() || !IsValid(GetOwner())) { return false; }
	if (auto* Wing = GetOwner()->FindComponentByClass<UHodgeWingComponent>(); Wing && Wing->Profile &&
		Wing->Profile->PlaybackMode == EHodgeWingPlaybackMode::AnimationAsset && Wing->GetWingMesh() == GetDetectionSourceComponent(Handle, ExecutionId))
	{ return Wing->PrepareAuthorityHitPose(ExecutionId); }
	return true;
}
/** 先关闭入口，再配对释放自身拥有的会话和姿势。 */
void UHodgeHitDetectionComponent::Shutdown()
{
	bEnabled = false;
	EndAllDetectionSessions();
	const auto Leases = PoseLeases.Array();
	for (const FGuid Lease : Leases) { ReleasePoseLease(Lease); }
}
/** 注销组件不能残留专服骨骼刷新占用。 */
void UHodgeHitDetectionComponent::OnUnregister()
{
	Shutdown(); Super::OnUnregister();
}
/** 结束世界时重复清理安全。 */
void UHodgeHitDetectionComponent::EndPlay(const EEndPlayReason::Type Reason)
{
	Shutdown(); Super::EndPlay(Reason);
}
/** 检测组件集中处理AcquirePoseLease，沿用原有身份与采样语义。 */
FGuid UHodgeHitDetectionComponent::AcquirePoseLease()
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

/** 检测组件集中处理ReleasePoseLease，沿用原有身份与采样语义。 */
void UHodgeHitDetectionComponent::ReleasePoseLease(FGuid Handle)
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

/** 检测组件集中处理ResolveSource，沿用原有身份与采样语义。 */
bool UHodgeHitDetectionComponent::ResolveSource(FGameplayTag Tag, FHodgeHitDetectionSession& Session) const
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

/** 检测组件集中处理IsSourceValid，沿用原有身份与采样语义。 */
bool UHodgeHitDetectionComponent::IsSourceValid(const FHodgeHitDetectionSession& Session) const
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

/** 检测组件集中处理CaptureDetectionGeometry，沿用原有身份与采样语义。 */
bool UHodgeHitDetectionComponent::CaptureDetectionGeometry(const FHodgeHitDetectionSession& Session,
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

/** 检测组件集中处理CreateDetectionSession，沿用原有身份与采样语义。 */
uint64 UHodgeHitDetectionComponent::CreateDetectionSession(const FGuid& ExecutionId, int32 OccurrenceId,
                                                         const FHodgeHitDetectionRequest& Request)
{
	// 按服务器权限区分本次处理分支。
	if (!CanExecuteAbilities() || !IsValid(GetOwner()) || !GetOwner()->HasAuthority() || !ExecutionId.IsValid() || OccurrenceId < 0 ||
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

/** 检测组件集中处理IsDetectionSessionValid，沿用原有身份与采样语义。 */
bool UHodgeHitDetectionComponent::IsDetectionSessionValid(uint64 Handle, const FGuid& ExecutionId) const
{
	const FHodgeHitDetectionSession* Session = Sessions.Find(Handle);
	return Session && Session->ExecutionId == ExecutionId && IsSourceValid(*Session);
}

/** 检测组件集中处理ResetDetectionHistory，沿用原有身份与采样语义。 */
bool UHodgeHitDetectionComponent::ResetDetectionHistory(uint64 Handle, const FGuid& ExecutionId)
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

/** 检测组件集中处理GetDetectionSourceComponent，沿用原有身份与采样语义。 */
USceneComponent* UHodgeHitDetectionComponent::GetDetectionSourceComponent(uint64 Handle, const FGuid& ExecutionId) const
{
	const FHodgeHitDetectionSession* Session = Sessions.Find(Handle);
	return Session && Session->ExecutionId == ExecutionId && IsSourceValid(*Session)
		       ? Session->SourceComponent.Get()
		       : nullptr;
}

/** 检测组件集中处理EndDetectionSession，沿用原有身份与采样语义。 */
void UHodgeHitDetectionComponent::EndDetectionSession(uint64 Handle, const FGuid& ExecutionId)
{
	const FHodgeHitDetectionSession* Session = Sessions.Find(Handle);
	if (Session && Session->ExecutionId == ExecutionId) { Sessions.Remove(Handle); }
}

/** 检测组件集中处理EndDetectionSessionsForExecution，沿用原有身份与采样语义。 */
void UHodgeHitDetectionComponent::EndDetectionSessionsForExecution(const FGuid& ExecutionId)
{
	for (auto It = Sessions.CreateIterator(); It; ++It)
	{
		if (It.Value().ExecutionId == ExecutionId) { It.RemoveCurrent(); }
	}
}

/** 检测组件集中处理EndAllDetectionSessions，沿用原有身份与采样语义。 */
void UHodgeHitDetectionComponent::EndAllDetectionSessions()
{
	Sessions.Reset();
}

/** 检测组件集中处理UpdateDetectionAnchor，沿用原有身份与采样语义。 */
void UHodgeHitDetectionComponent::UpdateDetectionAnchor(const FGuid& ExecutionId, FName Key, const FTransform& Transform)
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

/** 检测组件集中处理SampleDetection，沿用原有身份与采样语义。 */
bool UHodgeHitDetectionComponent::SampleDetection(uint64 Handle, const FGuid& ExecutionId,
                                                FHodgeHitDetectionBatch& OutBatch)
{
	OutBatch = FHodgeHitDetectionBatch();
	FHodgeHitDetectionSession* Live = Sessions.Find(Handle);
	// 按服务器权限区分本次处理分支。
	if (!CanExecuteAbilities() || !Live || Live->ExecutionId != ExecutionId || !IsValid(GetOwner()) || !GetOwner()->HasAuthority() || !GetWorld())
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
