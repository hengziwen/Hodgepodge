// 111 屎山代码来袭

#include "Component/HodgeCharacterMovementComponent.h"
#include "Combat/HodgeHitReactionTypes.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/CapsuleComponent.h"
#include "Character/HodgeCombatCharacter.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeLocomotionPolicyComponent.h"
#include "Data/HodgeSprintAbilityProfile.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component/HodgeCombatComponentBase.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCharacterMovementComponent)

// 定义“移动停止”GameplayTag，用于通过 GAS 禁止角色移动和旋转
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_MovementStopped, "Gameplay.MovementStopped");

namespace HodgeCharacter
{
	// 地面检测向下追踪的最大距离
	static float GroundTraceDistance = 100000.0f;

	// 控制地面检测射线长度的控制台变量，可在运行时通过控制台修改
	FAutoConsoleVariableRef CVar_GroundTraceDistance(
		TEXT("HodgeCharacter.GroundTraceDistance"), GroundTraceDistance,
		TEXT("Distance to trace down when generating ground information."), ECVF_Cheat);
};

namespace HodgeRotationPrediction
{
	UHodgeCharacterRotationComponent* FindRotation(const ACharacter* Character)
	{
		const AHodgeCombatCharacter* CombatCharacter = Cast<AHodgeCombatCharacter>(Character);
		return CombatCharacter ? CombatCharacter->GetCharacterRotationComponent() : nullptr;
	}

	class FSavedMove final : public FSavedMove_Character
	{
	public:
		FHodgeCharacterRotationState RotationState;
		FHodgeLocomotionState LocomotionState;
		bool bReactionControlled = false;

		virtual void Clear() override
		{
			Super::Clear();
			RotationState = {};
			LocomotionState = {};
			bReactionControlled = false;
		}

		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, const FVector& NewAcceleration,
			FNetworkPredictionData_Client_Character& ClientData) override
		{
			Super::SetMoveFor(Character, InDeltaTime, NewAcceleration, ClientData);
			if (const auto* Policy = Character->FindComponentByClass<UHodgeLocomotionPolicyComponent>()) { LocomotionState = Policy->GetResolvedPolicy(); }
			const auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character);
			bReactionControlled = ASC && ASC->HasMatchingGameplayTag(HodgeHitReactionTags::Controlled);
			if (const UHodgeCharacterRotationComponent* Rotation = FindRotation(Character))
			{
				RotationState = Rotation->GetResolvedState();
			}
		}

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override
		{
			const FSavedMove* Other = static_cast<const FSavedMove*>(NewMove.Get());
			if (!Other || RotationState.Driver != Other->RotationState.Driver || RotationState.Style != Other->RotationState.Style
				|| LocomotionState.ActivationKey != Other->LocomotionState.ActivationKey || LocomotionState.Version != Other->LocomotionState.Version
				|| LocomotionState.bSprinting != Other->LocomotionState.bSprinting
				|| RotationState.RequestSequence != Other->RotationState.RequestSequence || RotationState.ActionRequestId != Other->RotationState.ActionRequestId
				|| bReactionControlled != Other->bReactionControlled || RotationState.bYawLocked != Other->RotationState.bYawLocked
				|| RotationState.bRecoveringFacing != Other->RotationState.bRecoveringFacing
				|| (RotationState.bYawLocked && FMath::Abs(FMath::FindDeltaAngleDegrees(
					RotationState.LockedYaw, Other->RotationState.LockedYaw)) > KINDA_SMALL_NUMBER))
			{
				return false;
			}
			return Super::CanCombineWith(NewMove, Character, MaxDelta);
		}

		virtual void PrepMoveFor(ACharacter* Character) override
		{
			Super::PrepMoveFor(Character);
			if (auto* Policy = Character->FindComponentByClass<UHodgeLocomotionPolicyComponent>()) { Policy->SetMoveReplayState(LocomotionState); }
			if (auto* Movement = Cast<UHodgeCharacterMovementComponent>(Character->GetCharacterMovement()))
			{ Movement->SetHitReactionMoveReplay(bReactionControlled); }
			if (UHodgeCharacterRotationComponent* Rotation = FindRotation(Character))
			{
				Rotation->SetMoveReplayState(RotationState);
			}
		}

	private:
		using Super = FSavedMove_Character;
	};

	class FClientPredictionData final : public FNetworkPredictionData_Client_Character
	{
	public:
		explicit FClientPredictionData(const UCharacterMovementComponent& Movement)
			: FNetworkPredictionData_Client_Character(Movement) {}

		virtual FSavedMovePtr AllocateNewMove() override { return FSavedMovePtr(new FSavedMove()); }
	};

	struct FMoveData final : FCharacterNetworkMoveData
	{
		uint32 FacingSequence = 0;
		int32 SprintActivationKey = 0;
		virtual void ClientFillNetworkMoveData(const FSavedMove_Character& Move, ENetworkMoveType Type) override
		{
			FCharacterNetworkMoveData::ClientFillNetworkMoveData(Move, Type);
			FacingSequence = static_cast<const FSavedMove&>(Move).RotationState.RequestSequence;
			SprintActivationKey = static_cast<const FSavedMove&>(Move).LocomotionState.bSprinting ? static_cast<const FSavedMove&>(Move).LocomotionState.ActivationKey : 0;
		}
		virtual bool Serialize(UCharacterMovementComponent& Movement, FArchive& Ar, UPackageMap* Map, ENetworkMoveType Type) override
		{
			const bool bValid = FCharacterNetworkMoveData::Serialize(Movement, Ar, Map, Type);
			Ar.SerializeIntPacked(FacingSequence);
			Ar << SprintActivationKey;
			return bValid && !Ar.IsError();
		}
	};

	struct FMoveDataContainer final : FCharacterNetworkMoveDataContainer
	{
		FMoveData Moves[3];
		FMoveDataContainer() { NewMoveData = &Moves[0]; PendingMoveData = &Moves[1]; OldMoveData = &Moves[2]; }
	};
}


UHodgeCharacterMovementComponent::UHodgeCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FacingMoveData = MakeUnique<HodgeRotationPrediction::FMoveDataContainer>();
	SetNetworkMoveDataContainer(*FacingMoveData);
}

// 模拟角色移动，存在服务端同步加速度时需要保护复制过来的加速度不被父类逻辑覆盖
void UHodgeCharacterMovementComponent::PerformMovement(float DeltaTime)
{
	const auto* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	const auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CharacterOwner);
	const bool bStopped = IsHitReactionControlled() || (ASC && (ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) ||
		ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped)));
	if (Policy && !bStopped)
	{
		// 保存帧可能带回交接前的 RMS，按来源恢复成功退出策略再执行引擎清理。
		for (const auto& Source : CurrentRootMotion.RootMotionSources)
		{
			float MaximumSpeed = 0.f;
			if (Source && Policy->GetDashExitVelocity(Source->InstanceName, MaximumSpeed))
			{ Source->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::ClampVelocity; Source->FinishVelocityParams.ClampVelocity = MaximumSpeed; }
		}
	}
	Super::PerformMovement(DeltaTime);
}

void UHodgeCharacterMovementComponent::ClientHandleMoveResponse(const FCharacterMoveResponseDataContainer& MoveResponse)
{
	TOptional<FVector> CorrectionVelocity;
	const auto* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	const auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CharacterOwner);
	const bool bStopped = IsHitReactionControlled() || (ASC && (ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) ||
		ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped)));
	if (Policy && !bStopped && MoveResponse.IsCorrection() && MoveResponse.bRootMotionSourceCorrection &&
		!MoveResponse.bRootMotionMontageCorrection && !MoveResponse.ClientAdjustment.bBaseRelativeVelocity &&
		!MoveResponse.ClientAdjustment.NewVel.ContainsNaN())
	{
		if (const auto* Sources = MoveResponse.GetRootMotionSourceGroup(*this))
		{
			for (const auto& Source : Sources->RootMotionSources)
			{
				float MaximumSpeed = 0.f;
				if (Source && Policy->GetDashExitVelocity(Source->InstanceName, MaximumSpeed))
				{ CorrectionVelocity = FVector(MoveResponse.ClientAdjustment.NewVel); break; }
			}
		}
	}
	TGuardValue<TOptional<FVector>> CorrectionGuard(DashCorrectionVelocity, CorrectionVelocity);
	Super::ClientHandleMoveResponse(MoveResponse);
}

void UHodgeCharacterMovementComponent::ClientAdjustPosition_Implementation(float TimeStamp, FVector NewLoc, FVector NewVel,
	UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition,
	uint8 ServerMovementMode, TOptional<FRotator> OptionalRotation)
{
	// Packed 包已有完整权威速度；成功退出的 Dash 不走旧 RMS 入口的平面归零。
	Super::ClientAdjustPosition_Implementation(TimeStamp, NewLoc, DashCorrectionVelocity.Get(NewVel), NewBase,
		NewBaseBoneName, bHasBase, bBaseRelativePosition, ServerMovementMode, OptionalRotation);
}

void UHodgeCharacterMovementComponent::SimulateMovement(float DeltaTime)
{
	if (bHasReplicatedAcceleration)
	{
		// 保存网络同步过来的加速度，防止父类模拟过程中修改它
		const FVector OriginalAcceleration = Acceleration;

		// 执行 CharacterMovementComponent 原本的移动模拟逻辑
		Super::SimulateMovement(DeltaTime);

		// 恢复复制过来的加速度，保证本次移动使用的是服务端同步值
		Acceleration = OriginalAcceleration;
	}
	else
	{
		// 没有复制加速度时，直接使用父类默认移动模拟逻辑
		Super::SimulateMovement(DeltaTime);
	}
}

// 判断当前角色是否允许尝试跳跃
bool UHodgeCharacterMovementComponent::CanAttemptJump() const
{
	// 与 UCharacterMovementComponent 默认实现类似，但这里不检查蹲伏状态
	return IsJumpAllowed() &&
		(IsMovingOnGround() || IsFalling());

	// Falling 状态用于支持二段跳以及跳跃持续按压等情况，具体是否合法由 Character 再进行校验
}

void UHodgeCharacterMovementComponent::InitializeComponent()
{
	// 执行父类移动组件的初始化逻辑
	Super::InitializeComponent();
}

// 获取角色当前脚下的地面信息，必要时会重新进行地面检测
const FHodgeCharacterGroundInfo& UHodgeCharacterMovementComponent::GetGroundInfo()
{
	// 没有角色或者本帧已经更新过地面信息时，直接返回缓存
	if (!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
	{
		return CachedGroundInfo;
	}

	// Walking 状态下 CharacterMovementComponent 已经计算出了地面信息，直接复用
	if (MovementMode == MOVE_Walking)
	{
		// 使用当前移动组件计算好的地面碰撞结果
		CachedGroundInfo.GroundHitResult = CurrentFloor.HitResult;

		// Walking 状态下认为角色已经站在地面上，因此距离为 0
		CachedGroundInfo.GroundDistance = 0.0f;
	}
	else
	{
		// 获取角色胶囊体，用于计算角色与地面的实际距离
		const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
		check(CapsuleComp);

		// 获取未缩放的胶囊体半高
		const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();

		// 使用 UpdatedComponent 的碰撞类型作为地面检测通道，没有时默认使用 Pawn 通道
		const ECollisionChannel CollisionChannel = (UpdatedComponent
			                                            ? UpdatedComponent->GetCollisionObjectType()
			                                            : ECC_Pawn);

		// 从角色当前位置开始进行地面检测
		const FVector TraceStart(GetActorLocation());

		// 向角色下方进行超长距离射线检测
		const FVector TraceEnd(TraceStart.X, TraceStart.Y,
		                       (TraceStart.Z - HodgeCharacter::GroundTraceDistance - CapsuleHalfHeight));

		// 配置射线检测参数，并忽略角色自身
		FCollisionQueryParams QueryParams(
			SCENE_QUERY_STAT(HodgeCharacterMovementComponent_GetGroundInfo), false, CharacterOwner);

		// 配置碰撞响应参数
		FCollisionResponseParams ResponseParam;

		// 根据当前移动组件初始化碰撞查询参数
		InitCollisionParams(QueryParams, ResponseParam);

		// 保存射线检测结果
		FHitResult HitResult;

		// 从角色位置向下发射射线，检测角色下方的地面
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams,
		                                     ResponseParam);

		// 保存本次地面检测的碰撞结果
		CachedGroundInfo.GroundHitResult = HitResult;

		// 默认认为地面距离非常远，后续如果检测到地面会重新计算
		CachedGroundInfo.GroundDistance = HodgeCharacter::GroundTraceDistance;

		// NavWalking 状态下直接认为角色贴着导航地面
		if (MovementMode == MOVE_NavWalking)
		{
			CachedGroundInfo.GroundDistance = 0.0f;
		}
		// 检测到阻挡地面后，根据碰撞距离和胶囊体半高计算角色到地面的实际距离
		else if (HitResult.bBlockingHit)
		{
			CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
		}
	}

	// 记录本次地面信息更新发生在哪一帧，用于避免同一帧重复检测
	CachedGroundInfo.LastUpdateFrame = GFrameCounter;

	// 返回当前缓存的地面信息
	return CachedGroundInfo;
}

// 接收服务端同步过来的角色加速度
void UHodgeCharacterMovementComponent::SetReplicatedAcceleration(const FVector& InAcceleration)
{
	// 标记已经收到服务端复制的加速度
	bHasReplicatedAcceleration = true;

	// 使用服务端同步过来的加速度
	Acceleration = InAcceleration;
}

// 获取角色在当前帧允许产生的旋转变化
FRotator UHodgeCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	const UHodgeCharacterRotationComponent* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	const bool bApplyLocalConstraint = CharacterOwner && CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy;
	if (Rotation && bApplyLocalConstraint && Rotation->IsYawLocked()) { return FRotator::ZeroRotator; }
	// 从角色 Owner 上获取对应的 AbilitySystemComponent
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		// 如果角色拥有 MovementStopped 标签，则禁止角色旋转
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped))
		{
			// 返回零旋转，表示本帧不允许发生任何旋转
			return FRotator(0, 0, 0);
		}
	}

	// 没有禁止移动时使用 CharacterMovementComponent 默认的旋转计算
	FRotator Delta = Super::GetDeltaRotation(DeltaTime);
	const auto* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	if (Policy && Policy->GetResolvedPolicy().bSprinting) { Delta.Yaw = Policy->GetResolvedPolicy().TurnRate * FMath::Max(0.f, DeltaTime); }
	if (Rotation && bApplyLocalConstraint && Rotation->IsRecoveringFacing())
	{
		Delta.Yaw = FMath::Min(FMath::Abs(Delta.Yaw), Rotation->GetRecoveryTurnRate() * FMath::Max(0.f, DeltaTime));
	}
	return Delta;
}

void UHodgeCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	UHodgeCharacterRotationComponent* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	if (Rotation && Rotation->IsFacingSystemReady()) { Rotation->ApplyResolvedMode(); }
	if (Rotation && Rotation->IsYawLocked() && CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy) { return; }
	// 根运动蒙太奇提供唯一旋转增量，动作朝向请求不能逐帧把 Yaw 改回起点。
	const auto* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	const auto* Profile = Policy ? Policy->GetProfile() : nullptr;
	const auto* Anim = CharacterOwner && CharacterOwner->GetMesh() ? CharacterOwner->GetMesh()->GetAnimInstance() : nullptr;
	const auto* RootMontage = Anim ? Anim->GetRootMotionMontageInstance() : nullptr;
	if (HasAnimRootMotion() && RootMontage && Profile && RootMontage->Montage == Profile->SprintPivotMontage) { return; }
	if (Rotation && Rotation->IsFacingSystemReady() && CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy &&
		Rotation->GetResolvedState().Driver == EHodgeCharacterFacingDriver::ActionDirection)
	{
		const float TargetYaw = Rotation->GetResolvedState().ActionYaw;
		FRotator Facing = CharacterOwner->GetActorRotation();
		Facing.Yaw = FMath::FixedTurn(Facing.Yaw, TargetYaw, FMath::Abs(GetDeltaRotation(DeltaTime).Yaw));
		MoveUpdatedComponent(FVector::ZeroVector, Facing, false);
		Rotation->NotifyFacingApplied(TargetYaw);
		return;
	}
	FRotator DesiredRotation = CharacterOwner ? CharacterOwner->GetActorRotation() : FRotator::ZeroRotator;
	if (Rotation && Rotation->IsRecoveringFacing() && CharacterOwner)
	{
		if (bOrientRotationToMovement)
		{
			FRotator Delta = GetDeltaRotation(DeltaTime);
			DesiredRotation = ComputeOrientToMovementRotation(DesiredRotation, DeltaTime, Delta);
		}
		else if (bUseControllerDesiredRotation && CharacterOwner->Controller)
		{
			DesiredRotation = CharacterOwner->Controller->GetDesiredRotation();
		}
	}
	Super::PhysicsRotation(DeltaTime);
	if (Rotation && (bOrientRotationToMovement || bUseControllerDesiredRotation))
	{
		Rotation->NotifyFacingApplied(DesiredRotation.Yaw);
	}
}

void UHodgeCharacterMovementComponent::ServerMove_PerformMovement(const FCharacterNetworkMoveData& MoveData)
{
	UHodgeLocomotionPolicyComponent* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	if (Policy) { Policy->BeginServerMove(static_cast<const HodgeRotationPrediction::FMoveData&>(MoveData).SprintActivationKey); }
	UHodgeCharacterRotationComponent* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	const bool bServerOverride = CharacterOwner && CharacterOwner->HasAuthority() && Rotation;
	if (bServerOverride)
	{ Rotation->BeginServerMove(static_cast<const HodgeRotationPrediction::FMoveData&>(MoveData).FacingSequence); }
	Super::ServerMove_PerformMovement(MoveData);
	if (Policy) { Policy->EndServerMove(); }
	if (bServerOverride) { Rotation->EndServerMove(); }
}

bool UHodgeCharacterMovementComponent::MoveUpdatedComponentImpl(const FVector& Delta, const FQuat& NewRotation,
	bool bSweep, FHitResult* OutHit, ETeleportType Teleport)
{
	FQuat AppliedRotation = NewRotation;
	const UHodgeCharacterRotationComponent* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	if (Rotation && Rotation->IsYawLocked() && CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy
		&& Teleport == ETeleportType::None && !bApplyingRotationCorrection && UpdatedComponent
		&& !UpdatedComponent->IsSimulatingPhysics())
	{
		// 约束最终胶囊 Yaw，保留根运动位移和引擎允许的 Pitch/Roll。
		FRotator Constrained = NewRotation.Rotator();
		Constrained.Yaw = Rotation->GetLockedYaw();
		AppliedRotation = Constrained.Quaternion();
	}
	return Super::MoveUpdatedComponentImpl(Delta, AppliedRotation, bSweep, OutHit, Teleport);
}

FNetworkPredictionData_Client* UHodgeCharacterMovementComponent::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		UHodgeCharacterMovementComponent* MutableThis = const_cast<UHodgeCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new HodgeRotationPrediction::FClientPredictionData(*this);
	}
	return ClientPredictionData;
}

bool UHodgeCharacterMovementComponent::ClientUpdatePositionAfterServerUpdate()
{
	const bool bResult = Super::ClientUpdatePositionAfterServerUpdate();
	bHasReactionReplay = false;
	if (auto* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr) { Policy->ClearMoveReplayState(); }
	if (UHodgeCharacterRotationComponent* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner))
	{
		Rotation->ClearMoveReplayState();
	}
	return bResult;
}

void UHodgeCharacterMovementComponent::SmoothCorrection(const FVector& OldLocation, const FQuat& OldRotation,
	const FVector& NewLocation, const FQuat& NewRotation)
{
	TGuardValue<bool> CorrectionGuard(bApplyingRotationCorrection, true);
	Super::SmoothCorrection(OldLocation, OldRotation, NewLocation, NewRotation);
}

// 获取角色当前最大移动速度
float UHodgeCharacterMovementComponent::GetMaxSpeed() const
{
	// 从角色 Owner 上获取对应的 AbilitySystemComponent
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		// 如果角色拥有 MovementStopped 标签，则禁止角色移动
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped))
		{
			// 返回 0 速度，使角色完全停止移动
			return 0;
		}
	}

	// 没有禁止移动时使用 CharacterMovementComponent 默认最大速度
	const auto* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	const FHodgeLocomotionState State = Policy ? Policy->GetResolvedPolicy() : FHodgeLocomotionState();
	return State.bSprinting ? State.MaxSpeed : Super::GetMaxSpeed() * State.SpeedScale;
}

float UHodgeCharacterMovementComponent::GetMaxAcceleration() const
{
	const auto* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	const auto State = Policy ? Policy->GetResolvedPolicy() : FHodgeLocomotionState();
	return State.bSprinting ? State.Acceleration : Super::GetMaxAcceleration();
}
float UHodgeCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	const auto* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	const auto State = Policy ? Policy->GetResolvedPolicy() : FHodgeLocomotionState();
	return State.bSprinting ? State.Braking : Super::GetMaxBrakingDeceleration();
}
FVector UHodgeCharacterMovementComponent::ConstrainInputAcceleration(const FVector& InputAcceleration) const
{
	if (IsHitReactionControlled()) { return FVector::ZeroVector; }
	const auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (ASC && (ASC->HasMatchingGameplayTag(HodgeMovementTags::DashPreparing) || ASC->HasMatchingGameplayTag(HodgeMovementTags::Dashing))) { return FVector::ZeroVector; }
	const auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(GetOwner());
	if (ASC && ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Attack) && (!Combat || !Combat->IsMoveCancelPredicted()))
	{ return FVector::ZeroVector; }
	return Super::ConstrainInputAcceleration(InputAcceleration);
}

bool UHodgeCharacterMovementComponent::IsHitReactionControlled() const
{
	if (CharacterOwner && CharacterOwner->bClientUpdating && bHasReactionReplay) { return bReactionReplayControlled; }
	const auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	return ASC && ASC->HasMatchingGameplayTag(HodgeHitReactionTags::Controlled);
}

bool UHodgeCharacterMovementComponent::ApplyRequestedMove(float DeltaTime, float MaxAccel, float MaxSpeed, float Friction,
	float BrakingDeceleration, FVector& OutAcceleration, float& OutRequestedSpeed)
{
	if (IsHitReactionControlled()) { OutAcceleration = FVector::ZeroVector; OutRequestedSpeed = 0.f; return false; }
	return Super::ApplyRequestedMove(DeltaTime, MaxAccel, MaxSpeed, Friction, BrakingDeceleration, OutAcceleration, OutRequestedSpeed);
}
