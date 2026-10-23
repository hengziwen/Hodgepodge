/** @file HodgeCharacterRotationComponent.cpp
 * @brief 实现角色朝向组件相关的类型、接口与现有处理流程。
 */
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeLockOnComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h"
#include "Combat/HodgeHitReactionTypes.h"

#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Component/HodgeHitReactionComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Misc/DataValidation.h"
#include "Data/HodgeAbilityDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCharacterRotationComponent)

/** 构造角色朝向组件并初始化其默认状态。 */
UHodgeCharacterRotationComponent::UHodgeCharacterRotationComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	SetIsReplicatedByDefault(true);
}

/** 初始化关联能力System。 */
void UHodgeCharacterRotationComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)
{
	if (!InASC || !InASC->IsRegistered() || InASC->GetAvatarActor() != GetOwner()) { return; }
	if (AbilitySystemComponent.Get() == InASC)
	{
		RefreshLocalState();
		return;
	}
	UninitializeFromAbilitySystem();
	AbilitySystemComponent = InASC;
	++AvatarGeneration;
	if (auto* Character = GetPawn<ACharacter>())
	{
		bOriginalControllerYaw = Character->bUseControllerRotationYaw;
		bOriginalOrientToMovement = Character->GetCharacterMovement()->bOrientRotationToMovement;
		bOriginalControllerDesired = Character->GetCharacterMovement()->bUseControllerDesiredRotation;
		Character->GetCharacterMovement()->AddTickPrerequisiteComponent(this);
	}
	// 按当前对象生命周期建立状态监听。
	RotationTagHandle = InASC->RegisterGameplayTagEvent(HodgeGameplayTags::Status_Rotation_Locked,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HandleConstraintTagChanged);
	MovementStoppedTagHandle = InASC->RegisterGameplayTagEvent(TAG_Gameplay_MovementStopped,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HandleConstraintTagChanged);
	HitReactionTagHandle = InASC->RegisterGameplayTagEvent(HodgeHitReactionTags::Controlled,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HandleConstraintTagChanged);
	DeathTagHandle = InASC->RegisterGameplayTagEvent(HodgeGameplayTags::Status_Death,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HandleConstraintTagChanged);
	RefreshLocalState();
	SetComponentTickEnabled(true);
}

/** 解除初始化来源能力System。 */
void UHodgeCharacterRotationComponent::UninitializeFromAbilitySystem()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (UHodgeAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		if (RotationTagHandle.IsValid())
		{
			ASC->UnregisterGameplayTagEvent(RotationTagHandle, HodgeGameplayTags::Status_Rotation_Locked,
				EGameplayTagEventType::NewOrRemoved);
		}
		if (MovementStoppedTagHandle.IsValid())
		{
			ASC->UnregisterGameplayTagEvent(MovementStoppedTagHandle, TAG_Gameplay_MovementStopped,
				EGameplayTagEventType::NewOrRemoved);
		}
		if (HitReactionTagHandle.IsValid())
		{
			ASC->UnregisterGameplayTagEvent(HitReactionTagHandle, HodgeHitReactionTags::Controlled,
				EGameplayTagEventType::NewOrRemoved);
		}
		if (DeathTagHandle.IsValid())
		{ ASC->UnregisterGameplayTagEvent(DeathTagHandle, HodgeGameplayTags::Status_Death); }
	}
	RotationTagHandle.Reset();
	MovementStoppedTagHandle.Reset();
	HitReactionTagHandle.Reset();
	DeathTagHandle.Reset();
	if (AbilitySystemComponent.IsValid())
	{
		if (auto* Character = GetPawn<ACharacter>())
		{
			Character->bUseControllerRotationYaw = bOriginalControllerYaw;
			Character->GetCharacterMovement()->bOrientRotationToMovement = bOriginalOrientToMovement;
			Character->GetCharacterMovement()->bUseControllerDesiredRotation = bOriginalControllerDesired;
			Character->GetCharacterMovement()->RemoveTickPrerequisiteComponent(this);
		}
	}
	AbilitySystemComponent.Reset();
	Requests.Reset(); CompletedRequests.Reset(); ServerRequestHistory.Reset();
	LocalRequestSequence = AcknowledgedSequence = LastServerRequestSequence = 0;
	bServerMoveOverride = false;
	LocalState = {};
	ClearMoveReplayState();
	PublishAuthorityState();
	SetComponentTickEnabled(false);
}

/** 处理约束标签变化。 */
void UHodgeCharacterRotationComponent::HandleConstraintTagChanged(FGameplayTag Tag, int32 NewCount)
{
	// 外部硬锁只终止本段补转；解锁不能重新取得该段预算。
	if (NewCount > 0 && (Tag == HodgeGameplayTags::Status_Rotation_Locked || Tag == TAG_Gameplay_MovementStopped))
	{
		if (auto* Character = GetPawn<ACharacter>())
		{
			if (auto* Movement = Cast<UHodgeCharacterMovementComponent>(Character->GetCharacterMovement()))
			{ Movement->FreezeHitReactionFacing(); }
		}
	}
	RefreshLocalState();
}

/** 刷新本地状态。 */
void UHodgeCharacterRotationComponent::RefreshLocalState()
{
	// 当前操作关联的Pawn角色。
	const APawn* Pawn = GetPawn<APawn>();
	// 后续GAS操作使用的能力系统组件。
	const UHodgeAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	// 按服务器权限区分本次处理分支。
	if (!Pawn || (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) || !ASC || ASC->GetAvatarActor() != Pawn) { return; }
	const FHodgeCharacterRotationState Previous = LocalState;
	const bool bNewLock = ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Rotation_Locked)
		|| ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped) || ASC->HasMatchingGameplayTag(HodgeHitReactionTags::Controlled)
		|| ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death);
	if (bNewLock != LocalState.bYawLocked)
	{
		LocalState.bYawLocked = bNewLock;
		LocalState.bRecoveringFacing = !bNewLock;
		if (bNewLock) { LocalState.LockedYaw = FRotator::NormalizeAxis(Pawn->GetActorRotation().Yaw); }
		++LocalState.StateVersion;
	}
	ResolveRequests();
	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	if (Previous.bYawLocked != LocalState.bYawLocked) { OnFacingStateChanged.Broadcast(Previous, LocalState); }
}

/** 发布本对象当前的服务器权威状态。 */
void UHodgeCharacterRotationComponent::PublishAuthorityState()
{
	// 当前操作关联的所属对象。
	AActor* Owner = GetOwner();
	// 按服务器权限区分本次处理分支。
	if (Owner && Owner->HasAuthority())
	{
		ReplicatedState = LocalState;
		ServerRequestHistory.Add(LastServerRequestSequence, LocalState);
		while (ServerRequestHistory.Num() > 64)
		{
			uint32 Oldest = MAX_uint32;
			for (const auto& Pair : ServerRequestHistory) { Oldest = FMath::Min(Oldest, Pair.Key); }
			ServerRequestHistory.Remove(Oldest);
		}
		Owner->ForceNetUpdate();
	}
}

/** 判断Replaying移动。 */
bool UHodgeCharacterRotationComponent::IsReplayingMove() const
{
	const ACharacter* Character = GetPawn<ACharacter>();
	return bHasReplayState && Character && Character->bClientUpdating;
}

/** 获取Resolved状态。 */
FHodgeCharacterRotationState UHodgeCharacterRotationComponent::GetResolvedState() const
{
	if (IsReplayingMove()) { return ReplayState; }
	if (bServerMoveOverride) { return ServerMoveState; }
	// 当前操作关联的Pawn角色。
	const APawn* Pawn = GetPawn<APawn>();
	// 按服务器权限区分本次处理分支。
	if (Pawn && !Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) { return ReplicatedState; }
	return LocalState;
}

/** 判断Yaw角锁定。 */
bool UHodgeCharacterRotationComponent::IsYawLocked() const { return GetResolvedState().bYawLocked; }
/** 受击自身仍禁止输入转向，但不属于使本段永久冻结的外部硬锁。 */
bool UHodgeCharacterRotationComponent::IsExternallyYawLocked() const
{
	if (IsReplayingMove()) { return bReplayExternalYawLocked; }
	const auto* ASC = AbilitySystemComponent.Get();
	return ASC && (ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Rotation_Locked) ||
		ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped) || ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death));
}

/** 合法受击转向改变冻结参考，避免后续根运动或控制器把本步结果改回起点。 */
void UHodgeCharacterRotationComponent::UpdateHitReactionLockedYaw(float Yaw)
{
	if (!FMath::IsFinite(Yaw)) { return; }
	const float Normalized = FRotator::NormalizeAxis(Yaw);
	if (IsReplayingMove()) { ReplayState.LockedYaw = Normalized; }
	else
	{
		LocalState.LockedYaw = Normalized;
		if (bServerMoveOverride) { ServerMoveState.LockedYaw = Normalized; }
	}
}
/** 判断Recovering朝向。 */
bool UHodgeCharacterRotationComponent::IsRecoveringFacing() const { return GetResolvedState().bRecoveringFacing; }
/** 获取锁定Yaw角。 */
float UHodgeCharacterRotationComponent::GetLockedYaw() const { return GetResolvedState().LockedYaw; }

/** 筛选控制旋转。 */
FRotator UHodgeCharacterRotationComponent::FilterControlRotation(const FRotator& DesiredRotation, float DeltaSeconds) const
{
	FRotator Result = DesiredRotation;
	const FHodgeCharacterRotationState State = GetResolvedState();
	if (State.bYawLocked) { Result.Yaw = State.LockedYaw; }
	else if (State.Driver != EHodgeCharacterFacingDriver::Controller && IsFacingSystemReady())
	{ Result.Yaw = GetOwner()->GetActorRotation().Yaw; }
	else if (IsFacingSystemReady() && GetOwner())
	{
		Result.Yaw = FMath::FixedTurn(GetOwner()->GetActorRotation().Yaw, DesiredRotation.Yaw,
			(State.bRecoveringFacing ? GetRecoveryTurnRate() : FMath::IsFinite(ActionTurnRate) && ActionTurnRate > 0.f ? ActionTurnRate : 720.f)
			* FMath::Max(0.f, DeltaSeconds));
	}
	return Result;
}

/** 将实际朝向结果反馈给当前动作请求。 */
void UHodgeCharacterRotationComponent::NotifyFacingApplied(float DesiredYaw)
{
	if (IsReplayingMove() || bServerMoveOverride || !FMath::IsFinite(DesiredYaw)) { return; }
	FHodgeCharacterRotationState& State = LocalState;
	const FHodgeCharacterRotationState Previous = State;
	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	if (!State.bYawLocked && State.bRecoveringFacing && GetOwner()
		&& FMath::Abs(FMath::FindDeltaAngleDegrees(GetOwner()->GetActorRotation().Yaw, DesiredYaw)) < 1.f)
	{
		const FGuid AppliedAction = State.ActionRequestId;
		State.bRecoveringFacing = false;
		++State.StateVersion;
		if (!IsReplayingMove()) { PublishAuthorityState(); }
		OnFacingStateChanged.Broadcast(Previous, LocalState);
		if (AppliedAction.IsValid()) { SetRequestStatus(AppliedAction, EHodgeFacingRequestStatus::Applied); }
	}
}

/** 设置移动回放状态。 */
void UHodgeCharacterRotationComponent::SetMoveReplayState(const FHodgeCharacterRotationState& State, TOptional<bool> bExternalLock)
{
	ReplayState = State;
	bReplayExternalYawLocked = bExternalLock.Get(State.bYawLocked);
	bHasReplayState = true;
	ApplyResolvedMode();
}

/** 清空移动回放状态。 */
void UHodgeCharacterRotationComponent::ClearMoveReplayState()
{
	bHasReplayState = false;
	bReplayExternalYawLocked = false;
	ReplayState = {};
	ApplyResolvedMode();
}

/** 收到旋转状态复制后更新本地状态。 */
void UHodgeCharacterRotationComponent::OnRep_RotationState()
{
	// 当前操作关联的Pawn角色。
	const APawn* Pawn = GetPawn<APawn>();
	// 本地窗口决定预测锁的期限，权威结果只校正仍有效的锁定朝向。
	const auto* Reaction = GetOwner() ? GetOwner()->FindComponentByClass<UHodgeHitReactionComponent>() : nullptr;
	const bool bReactionFacingCurrent = Reaction && Reaction->IsFacingStateCurrent(
		Reaction->GetFacingState().ChainId, Reaction->GetFacingState().Segment);
	if (Pawn && Pawn->IsLocallyControlled() && LocalState.bYawLocked && ReplicatedState.bYawLocked && !bReactionFacingCurrent)
	{
		LocalState.LockedYaw = ReplicatedState.LockedYaw;
	}
	ResolveRequests();
}

/** 登记本类型需要网络复制的属性及复制条件。 */
void UHodgeCharacterRotationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, ReplicatedState);
}

/** 离开世界运行阶段并解除本对象持有的监听或资源。 */
void UHodgeCharacterRotationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeFromAbilitySystem();
	Super::EndPlay(EndPlayReason);
}

/** 判断朝向System就绪。 */
bool UHodgeCharacterRotationComponent::IsFacingSystemReady() const
{
	return AbilitySystemComponent.IsValid() && AbilitySystemComponent->IsRegistered() &&
		AbilitySystemComponent->GetAvatarActor() == GetOwner() && IsRegistered();
}

/** 判断有效来源。 */
bool UHodgeCharacterRotationComponent::IsValidSource(const UObject* Source, bool bAction) const
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!IsFacingSystemReady() || !IsValid(Source)) { return false; }
	// 根据当前生命周期状态决定是否继续执行。
	if (const auto* Ability = Cast<UGameplayAbility>(Source))
	{ return Ability->IsActive() && Ability->GetAvatarActorFromActorInfo() == GetOwner(); }
	// 任意方向动作的客户端入口必须来自正在执行的能力。
	if (bAction && !GetOwner()->HasAuthority()) { return false; }
	return Source == GetOwner() || Source->IsIn(GetOwner());
}

/** 判断是否存在待处理预测。 */
bool UHodgeCharacterRotationComponent::HasPendingPrediction() const
{
	if (!GetOwner() || GetOwner()->HasAuthority()) { return false; }
	for (const auto& Pair : Requests) { if (Pair.Value.bAbilityManaged) { return true; } }
	return LocalRequestSequence>AcknowledgedSequence;
}

/** 判断权威动作当前。 */
bool UHodgeCharacterRotationComponent::IsAuthorityActionCurrent() const
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!ReplicatedState.ActionAbilityHandle.IsValid() || !AbilitySystemComponent.IsValid()) { return true; }
	const auto* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(ReplicatedState.ActionAbilityHandle);
	if (!Spec || !Spec->Ability || Spec->Ability->GetNetExecutionPolicy() != EGameplayAbilityNetExecutionPolicy::LocalPredicted) { return true; }
	// 预测动作本地结束后，迟到的服务器租约不能恢复它的动作朝向。
	for (const UGameplayAbility* Instance : Spec->GetAbilityInstances())
	{
		const auto* DefinitionAbility=Cast<UHodgeGameplayAbility_Definition>(Instance);
		if (DefinitionAbility && (DefinitionAbility->IsFacingExecutionSuspended() || DefinitionAbility->IsFacingAuthorityRetired())) { continue; }
		if (Instance && Instance->IsActive() && Instance->GetAvatarActorFromActorInfo() == GetOwner() &&
			Instance->GetCurrentActivationInfo().GetActivationPredictionKey().Current == ReplicatedState.ActionPredictionKey) { return true; }
	}
	return false;
}

/** 按来源申请基础朝向模式，返回可配对释放的句柄。 */
FHodgeFacingRequestHandle UHodgeCharacterRotationComponent::AcquireBaseFacingMode(EHodgeCharacterFacingDriver Driver, UObject* Source)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!HodgeFacing::IsBaseDriver(Driver) || !IsValidSource(Source, false)) { return {}; }
	// 当前操作关联的Pawn角色。
	const auto* Pawn = GetPawn<APawn>();
	// 按服务器权限区分本次处理分支。
	if (!Pawn || (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) ||
		(Driver == EHodgeCharacterFacingDriver::Controller && (!bAllowControllerFacingRequests || !Pawn->Controller))) { return {}; }
	if (auto* Lock = Cast<UHodgeLockOnComponent>(Source)) { return Driver == EHodgeCharacterFacingDriver::Controller ? AcquireLockOnFacing(Lock) : FHodgeFacingRequestHandle(); }
	return AddRequest(Driver, Source, {}, 0.f, false);
}

/** 锁定组件必须属于本 Pawn 且当前有合法朝向占用条件。 */
FHodgeFacingRequestHandle UHodgeCharacterRotationComponent::AcquireLockOnFacing(UHodgeLockOnComponent* Source)
{
 const auto* Pawn = GetPawn<APawn>();
 if (!Pawn || (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) || !bAllowControllerFacingRequests || !Pawn->Controller ||
  !Source || Source->GetOwner() != Pawn || !IsValidSource(Source,false) || !Source->CanOwnControllerFacing()) { return {}; }
 return AddRequest(EHodgeCharacterFacingDriver::Controller,Source,{},0.f,false,true);
}

/** 申请本次动作的目标朝向，并交给现有仲裁与移动链。 */
FHodgeFacingRequestHandle UHodgeCharacterRotationComponent::RequestActionFacing(FVector WorldDirection, UObject* Source,
	FGuid ExecutionId, bool bInstantAtStart)
{
	FVector Direction;
	// 当前操作关联的Pawn角色。
	const auto* Pawn = GetPawn<APawn>();
	// 按服务器权限区分本次处理分支。
	if (!Pawn || (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) || !IsValidSource(Source, true) ||
		!ExecutionId.IsValid() || !HodgeFacing::NormalizeDirection(WorldDirection, Direction) || IsYawLocked()) { return {}; }
	if (const auto* Definition = Cast<UHodgeGameplayAbility_Definition>(Source))
	{ if (Definition->GetExecutionId() != ExecutionId || Definition->IsExecutionEnding()) { return {}; } }
	return AddRequest(EHodgeCharacterFacingDriver::ActionDirection, Source, ExecutionId, Direction.Rotation().Yaw, bInstantAtStart);
}

/** 根运动请求的方向已经由当前 GA 校验，组件只协调执行所有权。 */
FHodgeFacingRequestHandle UHodgeCharacterRotationComponent::RequestMontageWarpFacing(float TargetYaw, UObject* Source, FGuid ExecutionId)
{
	const auto* Ability=Cast<UHodgeGameplayAbility_Definition>(Source);
	const auto* Data=Ability ? Ability->GetDefinition() : nullptr;
	const auto* Pawn=GetPawn<APawn>();
	if (!Pawn || (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) || !IsValidSource(Source,true) || !Data || !Data->FacingWarp.bEnabled ||
		Ability->GetExecutionId()!=ExecutionId || !ExecutionId.IsValid() || IsYawLocked() || !FMath::IsFinite(TargetYaw)) { return {}; }
	const auto Handle=AddRequest(EHodgeCharacterFacingDriver::ActionDirection,Source,ExecutionId,TargetYaw,false,true);
	if (auto* Request=Requests.Find(Handle.Id))
	{ Request->bAbilityManaged=true; Request->ExecutionMode=EHodgeFacingExecutionMode::MontageWarp; ResolveRequests(); }
	return Handle;
}
/** 同一受控请求持续更新方向，保持唯一仲裁者与原运动执行链。 */
bool UHodgeCharacterRotationComponent::UpdateMontageWarpFacing(FHodgeFacingRequestHandle Handle,
	UHodgeGameplayAbility_Definition* Source, FGuid ExecutionId, float TargetYaw)
{
	const auto* Pawn = GetPawn<APawn>();
	const auto* Character = Cast<ACharacter>(Pawn);
	if (!Pawn || (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) || !IsFacingSystemReady() ||
		IsReplayingMove() || (Character && Character->bClientUpdating) || IsYawLocked() ||
		!Handle.IsValid() || Handle.AvatarGeneration != AvatarGeneration ||
		!IsValidSource(Source, true) || !Source->IsActive() || Source->IsExecutionEnding() ||
		Source->IsFacingExecutionSuspended() || Source->IsFacingAuthorityRetired() ||
		Source->GetExecutionId() != ExecutionId || !ExecutionId.IsValid() || !FMath::IsFinite(TargetYaw)) { return false; }
	auto* Request = Requests.Find(Handle.Id);
	if (!Request || Request->Source.Get() != Source || Request->ExecutionId != ExecutionId || !Request->bAbilityManaged ||
		Request->bSuspended || Request->ExecutionMode != EHodgeFacingExecutionMode::MontageWarp ||
		LocalState.ActionRequestId != Handle.Id) { return false; }
	const float Yaw = FRotator::NormalizeAxis(TargetYaw);
	Request->Yaw = Yaw;
	// 当前服务器移动视图已在模拟前复制，不能等下一步才看到本次 Warp 采用的方向。
	if (bServerMoveOverride && ServerMoveState.ActionRequestId == Handle.Id &&
		ServerMoveState.Driver == EHodgeCharacterFacingDriver::ActionDirection &&
		ServerMoveState.ExecutionMode == EHodgeFacingExecutionMode::MontageWarp &&
		ServerMoveState.WarpDefinition == Source->GetDefinition() &&
		ServerMoveState.ActionAbilityHandle == Source->GetCurrentAbilitySpecHandle() &&
		ServerMoveState.ActionPredictionKey == Source->GetCurrentActivationInfo().GetActivationPredictionKey().Current)
	{ ServerMoveState.ActionYaw = Yaw; }
	ResolveRequests();
	return true;
}
/** 暂停只影响本地解析，保留当前执行身份及拒绝取消后的恢复数据。 */
bool UHodgeCharacterRotationComponent::SetFacingRequestSuspended(FHodgeFacingRequestHandle Handle, bool bSuspended)
{
	if (!Handle.IsValid() || Handle.AvatarGeneration!=AvatarGeneration) { return false; }
	auto* Request=Requests.Find(Handle.Id); if (!Request || !Request->bAbilityManaged) { return false; }
	Request->bSuspended=bSuspended; ResolveRequests(); return true;
}
bool UHodgeCharacterRotationComponent::IsFacingRequestSelected(FHodgeFacingRequestHandle Handle) const
{
	return Handle.IsValid() && Handle.AvatarGeneration==AvatarGeneration && Requests.Contains(Handle.Id) &&
		!Requests[Handle.Id].bSuspended && !IsYawLocked() && GetResolvedState().ActionRequestId==Handle.Id;
}

/** 添加请求。 */
FHodgeFacingRequestHandle UHodgeCharacterRotationComponent::AddRequest(EHodgeCharacterFacingDriver Driver, UObject* Source,
	FGuid ExecutionId, float Yaw, bool bInstant, bool bLocalOnly)
{
	if (Requests.Num() >= 32 || AbilitySystemComponent->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death)) { return {}; }
	FHodgeFacingRequestHandle Handle; Handle.Id = FGuid::NewGuid(); Handle.AvatarGeneration = AvatarGeneration;
	FRequest& Request = Requests.Add(Handle.Id);
	Request.Source = Source; Request.Driver = Driver; Request.ExecutionId = ExecutionId;
	Request.bLocalOnly = bLocalOnly;
	Request.Yaw = FRotator::NormalizeAxis(Yaw); Request.Order = ++NextRequestOrder; Request.bInstant = bInstant;
	Request.Status = Driver == EHodgeCharacterFacingDriver::ActionDirection ? EHodgeFacingRequestStatus::Pending : EHodgeFacingRequestStatus::Applied;
	// 按服务器权限区分本次处理分支。
	if (!GetOwner()->HasAuthority() && !bLocalOnly) { ++LocalRequestSequence; }
	ResolveRequests();
	if (bInstant && !IsYawLocked())
	{
		if (auto* Character = GetPawn<ACharacter>())
		{
			Character->GetCharacterMovement()->MoveUpdatedComponent(FVector::ZeroVector, FRotator(0.f, Yaw, 0.f), false);
			NotifyFacingApplied(Yaw);
		}
	}
	if (!GetOwner()->HasAuthority() && !bLocalOnly) { SendRequest(Handle.Id, false); }
	return Handle;
}

/** 释放匹配当前初始化代的朝向请求。 */
bool UHodgeCharacterRotationComponent::ReleaseFacingRequest(FHodgeFacingRequestHandle Handle)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Handle.IsValid() || Handle.AvatarGeneration != AvatarGeneration || !Requests.Contains(Handle.Id)) { return false; }
	// 按服务器权限区分本次处理分支。
	if (!GetOwner()->HasAuthority() && !Requests[Handle.Id].bLocalOnly)
	{
		++LocalRequestSequence;
		SendRequest(Handle.Id, true);
	}
	Requests.Remove(Handle.Id);
	CompletedRequests.Add(Handle.Id, EHodgeFacingRequestStatus::Released);
	ResolveRequests();
	return true;
}

/** 释放Requests关联来源。 */
void UHodgeCharacterRotationComponent::ReleaseRequestsForSource(const UObject* Source)
{
	TArray<FGuid> Matches;
	for (const auto& Pair : Requests) { if (Pair.Value.Source.Get() == Source) { Matches.Add(Pair.Key); } }
	for (const FGuid& Id : Matches)
	{ FHodgeFacingRequestHandle Handle; Handle.Id = Id; Handle.AvatarGeneration = AvatarGeneration; ReleaseFacingRequest(Handle); }
}

/** 获取朝向请求状态。 */
EHodgeFacingRequestStatus UHodgeCharacterRotationComponent::GetFacingRequestStatus(FHodgeFacingRequestHandle Handle) const
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Handle.IsValid() || Handle.AvatarGeneration != AvatarGeneration) { return EHodgeFacingRequestStatus::Invalid; }
	if (const FRequest* Request = Requests.Find(Handle.Id)) { return Request->Status; }
	if (const auto* Status = CompletedRequests.Find(Handle.Id)) { return *Status; }
	return EHodgeFacingRequestStatus::Invalid;
}

/** 确认本来源与执行身份的朝向请求已实际应用。 */
bool UHodgeCharacterRotationComponent::IsActionFacingApplied(FHodgeFacingRequestHandle Handle) const
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Handle.IsValid() || Handle.AvatarGeneration != AvatarGeneration || IsYawLocked()) { return false; }
	const FRequest* Requested = Requests.Find(Handle.Id);
	const FRequest* Selected = Requests.Find(GetResolvedState().ActionRequestId);
	return Requested && Selected && Requested->Driver == EHodgeCharacterFacingDriver::ActionDirection &&
		Selected->Status == EHodgeFacingRequestStatus::Applied && Selected->Source == Requested->Source &&
		Selected->ExecutionId == Requested->ExecutionId &&
		FMath::Abs(FMath::FindDeltaAngleDegrees(Selected->Yaw, Requested->Yaw)) < .1f;
}

/** 设置请求状态。 */
void UHodgeCharacterRotationComponent::SetRequestStatus(const FGuid& Id, EHodgeFacingRequestStatus Status)
{
	FRequest* Request = Requests.Find(Id);
	if (!Request || Request->Status == Status) { return; }
	Request->Status = Status;
	FHodgeFacingRequestHandle Handle; Handle.Id = Id; Handle.AvatarGeneration = AvatarGeneration;
	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	OnActionFacingApplied.Broadcast(Handle, Status);
}

/** 解析Requests。 */
void UHodgeCharacterRotationComponent::ResolveRequests()
{
	if (bResolvingRequests || !IsFacingSystemReady()) { return; }
	TGuardValue<bool> Guard(bResolvingRequests, true);
	// 当前操作关联的Pawn角色。
	const APawn* Pawn = GetPawn<APawn>();
	// 按服务器权限区分本次处理分支。
	if (!Pawn || (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled())) { ApplyResolvedMode(); return; }
	const FHodgeCharacterRotationState Previous = LocalState;
	const FRequest* Base = nullptr; const FRequest* Action = nullptr; FGuid ActionId;
	for (const auto& Pair : Requests)
	{
		if (Pair.Value.bSuspended) { continue; }
		if (Pair.Value.Driver == EHodgeCharacterFacingDriver::ActionDirection)
		{ if (!Action || Pair.Value.Order > Action->Order) { Action = &Pair.Value; ActionId = Pair.Key; } }
		else if (!Base || Pair.Value.Order > Base->Order) { Base = &Pair.Value; }
	}
	const bool bAuthorityFallback = !Pawn->HasAuthority() && !HasPendingPrediction() && ReplicatedState.StateVersion > 0;
	const auto* Lock = GetOwner()->FindComponentByClass<UHodgeLockOnComponent>();
	const bool bLockFallback = bAuthorityFallback && ReplicatedState.bLockOnFacing && Lock && Lock->CanOwnControllerFacing();
	LocalState.BaseDriver = Base ? Base->Driver : bAuthorityFallback && (!ReplicatedState.bLockOnFacing || bLockFallback) ? ReplicatedState.BaseDriver : EHodgeCharacterFacingDriver::Movement;
	LocalState.bLockOnFacing = Base ? Base->Source.IsValid() && Base->Source->IsA<UHodgeLockOnComponent>() : bLockFallback;
	LocalState.Driver = Action ? Action->Driver : LocalState.BaseDriver;
	LocalState.ActionRequestId = Action ? ActionId : FGuid();
	LocalState.ActionYaw = Action ? Action->Yaw : 0.f;
	LocalState.ExecutionMode=Action ? Action->ExecutionMode : EHodgeFacingExecutionMode::PhysicalTurn;
	const auto* WarpAbility=Action ? Cast<UHodgeGameplayAbility_Definition>(Action->Source.Get()) : nullptr;
	LocalState.WarpDefinition=Action && Action->ExecutionMode==EHodgeFacingExecutionMode::MontageWarp && WarpAbility ? WarpAbility->GetDefinition() : nullptr;
	const auto* ActionAbility = Action ? Cast<UGameplayAbility>(Action->Source.Get()) : nullptr;
	LocalState.ActionAbilityHandle = ActionAbility ? ActionAbility->GetCurrentAbilitySpecHandle() : FGameplayAbilitySpecHandle();
	LocalState.ActionPredictionKey = ActionAbility ? ActionAbility->GetCurrentActivationInfo().GetActivationPredictionKey().Current : 0;
	if (!Action && bAuthorityFallback && ReplicatedState.Driver == EHodgeCharacterFacingDriver::ActionDirection && IsAuthorityActionCurrent())
	{
		LocalState.Driver = ReplicatedState.Driver;
		LocalState.ExecutionMode=ReplicatedState.ExecutionMode; LocalState.WarpDefinition=ReplicatedState.WarpDefinition;
		LocalState.ActionRequestId = ReplicatedState.ActionRequestId; LocalState.ActionYaw = ReplicatedState.ActionYaw;
		LocalState.ActionAbilityHandle = ReplicatedState.ActionAbilityHandle; LocalState.ActionPredictionKey = ReplicatedState.ActionPredictionKey;
	}
	LocalState.Style = LocalState.BaseDriver == EHodgeCharacterFacingDriver::Controller ?
		EHodgeLocomotionStyle::ReservedStrafe : EHodgeLocomotionStyle::FreeDirectional;
	LocalState.RequestSequence = Pawn->HasAuthority() ? LastServerRequestSequence : LocalRequestSequence;
	const bool bChanged = Previous.bLockOnFacing != LocalState.bLockOnFacing || Previous.BaseDriver != LocalState.BaseDriver || Previous.Driver != LocalState.Driver ||
		Previous.Style != LocalState.Style || Previous.ActionRequestId != LocalState.ActionRequestId ||
		Previous.ExecutionMode!=LocalState.ExecutionMode || Previous.WarpDefinition!=LocalState.WarpDefinition || Previous.ActionYaw!=LocalState.ActionYaw ||
		Previous.ActionAbilityHandle != LocalState.ActionAbilityHandle || Previous.ActionPredictionKey != LocalState.ActionPredictionKey ||
		Previous.RequestSequence != LocalState.RequestSequence;
	if (bChanged)
	{
		LocalState.bRecoveringFacing = !LocalState.bYawLocked && LocalState.ExecutionMode!=EHodgeFacingExecutionMode::MontageWarp;
		++LocalState.StateVersion;
	}
	ApplyResolvedMode();
	if (bChanged || ReplicatedState.StateVersion != LocalState.StateVersion) { PublishAuthorityState(); }
	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	if (bChanged) { OnFacingStateChanged.Broadcast(Previous, LocalState); }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (ActionId.IsValid())
	{
		const auto Status = Requests.Find(ActionId) ? Requests.Find(ActionId)->Status : EHodgeFacingRequestStatus::Invalid;
		if (LocalState.bYawLocked && Status == EHodgeFacingRequestStatus::Pending) { SetRequestStatus(ActionId, EHodgeFacingRequestStatus::Blocked); }
		else if (!LocalState.bYawLocked && Status == EHodgeFacingRequestStatus::Blocked) { SetRequestStatus(ActionId, EHodgeFacingRequestStatus::Pending); }
	}
	while (CompletedRequests.Num() > 64) { CompletedRequests.Remove(CompletedRequests.CreateConstIterator().Key()); }
}

/** 应用ResolvedMode。 */
void UHodgeCharacterRotationComponent::ApplyResolvedMode()
{
	if (!IsFacingSystemReady()) { return; }
	if (auto* Character = GetPawn<ACharacter>())
	{
		const auto State = GetResolvedState();
		Character->bUseControllerRotationYaw = State.Driver == EHodgeCharacterFacingDriver::Controller;
		Character->GetCharacterMovement()->bOrientRotationToMovement = State.Driver == EHodgeCharacterFacingDriver::Movement;
		Character->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
}

/** 获取朝向表现快照。 */
FHodgeFacingPresentationSnapshot UHodgeCharacterRotationComponent::GetFacingPresentationSnapshot() const
{
	const auto State = GetResolvedState(); FHodgeFacingPresentationSnapshot Result;
	Result.Style = State.Style; Result.Driver = State.Driver; Result.StateVersion = State.StateVersion;
	Result.bYawLocked = State.bYawLocked; Result.bRecoveringFacing = State.bRecoveringFacing;
	return Result;
}

/** 更新组件。 */
void UHodgeCharacterRotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* Tick)
{
	Super::TickComponent(DeltaTime, TickType, Tick);
	if (!IsFacingSystemReady()) { UninitializeFromAbilitySystem(); return; }
	// 当前操作关联的Pawn角色。
	const APawn* Pawn = GetPawn<APawn>();
	// 按服务器权限区分本次处理分支。
	if (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) { ApplyResolvedMode(); return; }
	TArray<FGuid> Expired;
	for (const auto& Pair : Requests)
	{
		const bool bDead = AbilitySystemComponent->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death);
		const auto* Definition = Cast<UHodgeGameplayAbility_Definition>(Pair.Value.Source.Get());
		if (bDead || !IsValidSource(Pair.Value.Source.Get(), Pair.Value.Driver == EHodgeCharacterFacingDriver::ActionDirection) ||
			(Pair.Value.Driver == EHodgeCharacterFacingDriver::Controller && !Pawn->Controller) ||
			(Definition && Pair.Value.Driver == EHodgeCharacterFacingDriver::ActionDirection && Definition->GetExecutionId() != Pair.Value.ExecutionId))
		{ Expired.Add(Pair.Key); }
	}
	for (const FGuid& Id : Expired)
	{ FHodgeFacingRequestHandle Handle; Handle.Id = Id; Handle.AvatarGeneration = AvatarGeneration; ReleaseFacingRequest(Handle); }
	RefreshLocalState();
	if (!LocalState.bYawLocked && LocalState.bRecoveringFacing && LocalState.ExecutionMode!=EHodgeFacingExecutionMode::MontageWarp)
	{
		float DesiredYaw = Pawn->GetActorRotation().Yaw;
		if (LocalState.Driver == EHodgeCharacterFacingDriver::ActionDirection) { DesiredYaw = LocalState.ActionYaw; }
		else if (LocalState.Driver == EHodgeCharacterFacingDriver::Controller && Pawn->Controller) { DesiredYaw = Pawn->Controller->GetControlRotation().Yaw; }
		else if (const auto* Character = Cast<ACharacter>(Pawn); Character && !Character->GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero())
		{ DesiredYaw = Character->GetCharacterMovement()->GetCurrentAcceleration().Rotation().Yaw; }
		NotifyFacingApplied(DesiredYaw);
	}
}

/** 沿现有复制入口提交当前请求及其身份。 */
void UHodgeCharacterRotationComponent::SendRequest(const FGuid& Id, bool bRelease)
{
	const FRequest* Request = Requests.Find(Id);
	if (!Request || !IsFacingSystemReady()) { return; }
	FGameplayAbilitySpecHandle AbilityHandle; int32 Key = 0;
	// 按预测状态或网络角色选择对应处理。
	if (const auto* Ability = Cast<UGameplayAbility>(Request->Source.Get()))
	{ AbilityHandle = Ability->GetCurrentAbilitySpecHandle(); Key = Ability->GetCurrentActivationInfo().GetActivationPredictionKey().Current; }
	ServerFacingRequest(GetOwner(), LocalRequestSequence, Id, bRelease, Request->Driver,
		Request->Yaw, Request->bInstant, AbilityHandle, Key, Request->ExecutionId);
}

/** 处理服务器侧的朝向请求消息，沿用当前身份与权限校验。 */
void UHodgeCharacterRotationComponent::ServerFacingRequest_Implementation(AActor* Avatar, uint32 Sequence, FGuid Id,
	bool bRelease, EHodgeCharacterFacingDriver Driver, float Yaw, bool bInstant, FGameplayAbilitySpecHandle AbilityHandle,
	int32 PredictionKey, FGuid ExecutionId)
{
	bool bAccepted = false;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (IsFacingSystemReady() && Avatar == GetOwner() && Id.IsValid() && Sequence > LastServerRequestSequence &&
		Sequence - LastServerRequestSequence <= 64)
	{
		LastServerRequestSequence = Sequence;
		if (bRelease)
		{
			if (const FRequest* Existing = Requests.Find(Id); Existing && Existing->bRemoteClient)
			{ Requests.Remove(Id); bAccepted = true; }
		}
		else if (!Requests.Contains(Id) && Requests.Num() < 32 && FMath::IsFinite(Yaw) &&
			!AbilitySystemComponent->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death))
		{
			UObject* Source = GetOwner();
			if (AbilityHandle.IsValid())
			{
				const auto* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(AbilityHandle);
				UGameplayAbility* Ability = Spec ? Spec->GetPrimaryInstance() : nullptr;
				Source = Ability && Ability->IsActive() && Ability->GetCurrentActivationInfo().GetActivationPredictionKey().Current == PredictionKey ? Ability : nullptr;
			}
			const auto* Definition = Cast<UHodgeGameplayAbility_Definition>(Source);
			const auto* Movement = Cast<UHodgeGameplayAbility_MovementAction>(Source);
			const bool bAction = Driver == EHodgeCharacterFacingDriver::ActionDirection;
			const bool bLegalDriver = HodgeFacing::IsBaseDriver(Driver) || (bAction && AbilityHandle.IsValid() && ExecutionId.IsValid() && !IsYawLocked());
			if (Source && bLegalDriver && IsValidSource(Source, bAction) &&
				(Driver != EHodgeCharacterFacingDriver::Controller || (bAllowControllerFacingRequests && GetPawn<APawn>()->Controller)) &&
				(!Definition || !bAction || (Definition->GetExecutionId().IsValid() && !Definition->IsExecutionEnding())))
			{
				FRequest& Request = Requests.Add(Id); Request.Source = Source; Request.Driver = Driver;
				Request.Yaw = FRotator::NormalizeAxis(Yaw); Request.Order = ++NextRequestOrder;
				// Definition 的本地 GUID 不跨端共享，服务器用已验证 Spec / PredictionKey 映射自己的执行。
				Request.ExecutionId = Definition && bAction ? Definition->GetExecutionId() :
					Movement && bAction ? Movement->GetMovementExecutionId() : ExecutionId;
				Request.bRemoteClient = true; Request.bInstant = bInstant;
				Request.Status = bAction ? EHodgeFacingRequestStatus::Pending : EHodgeFacingRequestStatus::Applied;
				bAccepted = true;
			}
		}
		ResolveRequests();
		if (bAccepted && !bRelease && bInstant && Driver == EHodgeCharacterFacingDriver::ActionDirection && !IsYawLocked())
		{
			if (auto* Character = GetPawn<ACharacter>())
			{ Character->GetCharacterMovement()->MoveUpdatedComponent(FVector::ZeroVector, FRotator(0.f, Yaw, 0.f), false); NotifyFacingApplied(Yaw); }
		}
	}
	ClientFacingResult(Sequence, Id, bAccepted, LocalState);
}

/** 处理客户端侧的朝向结果消息，沿用当前身份与权限校验。 */
void UHodgeCharacterRotationComponent::ClientFacingResult_Implementation(uint32 Sequence, FGuid Id, bool bAccepted,
	FHodgeCharacterRotationState State)
{
	if (!IsFacingSystemReady() || Sequence <= AcknowledgedSequence || Sequence > LocalRequestSequence) { return; }
	AcknowledgedSequence = Sequence;
	if (State.StateVersion >= ReplicatedState.StateVersion) { ReplicatedState = State; }
	if (!bAccepted && Requests.Contains(Id))
	{ Requests.Remove(Id); CompletedRequests.Add(Id, EHodgeFacingRequestStatus::Rejected); }
	ResolveRequests();
}

/** 开始服务器移动。 */
bool UHodgeCharacterRotationComponent::BeginServerMove(uint32 Sequence, int32 MontageWarpKey)
{
	// 按服务器权限区分本次处理分支。
	if (!GetOwner()->HasAuthority() || !IsFacingSystemReady()) { return false; }
	// 未获授权的移动包使用当前状态，不接受客户端直接携带的朝向模式。
	const auto* Historical = ServerRequestHistory.Find(Sequence);
	ServerMoveState = Historical ? *Historical : LocalState;
	// GAS 管理的 Warp 不依赖另一条组件 RPC 的到达顺序；权威活动执行优先。
	if (LocalState.ExecutionMode==EHodgeFacingExecutionMode::MontageWarp && IsAuthorityActionCurrent()) { ServerMoveState=LocalState; }
	else if (ServerMoveState.ExecutionMode==EHodgeFacingExecutionMode::MontageWarp && MontageWarpKey!=ServerMoveState.ActionPredictionKey)
	{ ServerMoveState=LocalState; }
	// 锁定不经朝向 RPC 排序，历史移动包不得复活已解除的锁定基础模式。
	if (ServerMoveState.bLockOnFacing || LocalState.bLockOnFacing)
	{
		ServerMoveState.BaseDriver = LocalState.BaseDriver; ServerMoveState.Style = LocalState.Style; ServerMoveState.bLockOnFacing = LocalState.bLockOnFacing;
		if (HodgeFacing::IsBaseDriver(ServerMoveState.Driver)) { ServerMoveState.Driver = ServerMoveState.BaseDriver; }
	}
	if (LocalState.bYawLocked) { ServerMoveState.bYawLocked = true; ServerMoveState.LockedYaw = LocalState.LockedYaw; }
	bServerMoveOverride = true;
	ApplyResolvedMode();
	return Historical != nullptr;
}

/** 结束服务器移动。 */
void UHodgeCharacterRotationComponent::EndServerMove()
{
	bServerMoveOverride = false;
	ApplyResolvedMode();
}

#if WITH_EDITOR
/** 在编辑器校验当前配置并收集诊断。 */
EDataValidationResult UHodgeCharacterRotationComponent::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult Parent = Super::IsDataValid(Context);
	if (!FMath::IsFinite(RecoveryTurnRate) || RecoveryTurnRate <= 0.f || !FMath::IsFinite(ActionTurnRate) || ActionTurnRate <= 0.f)
	{
		Context.AddError(NSLOCTEXT("HodgeFacing", "InvalidTurnRates", "Facing turn rates must be finite and greater than zero."));
		return EDataValidationResult::Invalid;
	}
	return Parent == EDataValidationResult::Invalid ? Parent : EDataValidationResult::Valid;
}
#endif
