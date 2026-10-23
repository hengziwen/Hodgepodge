/** @file HodgeCharacterMovementComponent.cpp
 * @brief 实现角色移动组件相关的类型、接口与现有处理流程。
 */
// 111 屎山代码来袭

#include "Component/HodgeCharacterMovementComponent.h"
#include "Component/HodgeLockOnComponent.h"
#include "Combat/HodgeHitReactionTypes.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/CapsuleComponent.h"
#include "Character/HodgeCombatCharacter.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeHitReactionComponent.h"
#include "Component/HodgeLocomotionPolicyComponent.h"
#include "Data/HodgeSprintAbilityProfile.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component/HodgeCombatComponentBase.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Misc/ScopeExit.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCharacterMovementComponent)

// 校正与普通限速分别留证，历史重放不重复伪造实时转向日志。
DEFINE_LOG_CATEGORY_STATIC(LogHodgeHitReactionFacing, Log, All);

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
	/** 查找旋转。
	 * @param Character 角色，由调用方传入。
	 * @return 旋转的当前值或引用。
	 */
	UHodgeCharacterRotationComponent* FindRotation(const ACharacter* Character)
	{
		const AHodgeCombatCharacter* CombatCharacter = Cast<AHodgeCombatCharacter>(Character);
		return CombatCharacter ? CombatCharacter->GetCharacterRotationComponent() : nullptr;
	}

	/** 封装Saved移动的状态与操作接口。 */
	class FSavedMove final : public FSavedMove_Character
	{
	public:
		// 旋转状态的数据或对象引用。
		FHodgeCharacterRotationState RotationState;
		// 移动表现状态的数据或对象引用。
		FHodgeLocomotionState LocomotionState;
		// 控制是否受击Controlled的状态标记。
		bool bReactionControlled = false;
		UHodgeCharacterMovementComponent::FHitReactionFacingMoveSnapshot HitReactionFacing;
		// 起始身份与下一移动步序号只用于回填实际消费Yaw，不进入网络移动协议。
		TWeakObjectPtr<ACharacter> AttackWarpAvatar;
		int32 AttackWarpAvatarGeneration = INDEX_NONE;
		uint64 AttackWarpEvaluationSequence = 0;

		/** 清空当前容器或缓存保存的数据。 */
		virtual void Clear() override
		{
			Super::Clear();
			RotationState = {};
			LocomotionState = {};
			bReactionControlled = false;
			HitReactionFacing = {};
			AttackWarpAvatar.Reset();
			AttackWarpAvatarGeneration = INDEX_NONE;
			AttackWarpEvaluationSequence = 0;
		}

		/** 设置移动关联。
		 * @param Character 角色，由调用方传入。
		 * @param InDeltaTime 本次更新的时间步长，单位秒。
		 * @param NewAcceleration 新加速度，由调用方传入。
		 * @param ClientData 客户端数据，由调用方传入。
		 */
		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, const FVector& NewAcceleration,
			FNetworkPredictionData_Client_Character& ClientData) override
		{
			Super::SetMoveFor(Character, InDeltaTime, NewAcceleration, ClientData);
			AttackWarpAvatar = Character;
			AttackWarpAvatarGeneration = INDEX_NONE;
			AttackWarpEvaluationSequence = 0;
			if (const auto* Movement = Cast<UHodgeCharacterMovementComponent>(Character->GetCharacterMovement()))
			{
				AttackWarpEvaluationSequence = Movement->GetNextAttackWarpEvaluationSequence();
				HitReactionFacing = Movement->CaptureHitReactionFacing(InDeltaTime);
			}
			if (const auto* Policy = Character->FindComponentByClass<UHodgeLocomotionPolicyComponent>()) { LocomotionState = Policy->GetResolvedPolicy(); }
			// 后续GAS操作使用的能力系统组件。
			const auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character);
			bReactionControlled = ASC && ASC->HasMatchingGameplayTag(HodgeHitReactionTags::Controlled);
			if (const UHodgeCharacterRotationComponent* Rotation = FindRotation(Character))
			{
				RotationState = Rotation->GetResolvedState();
				AttackWarpAvatarGeneration = Rotation->GetAvatarGeneration();
			}
		}

		/** 原生根运动录制后仅回填本步实际消费Yaw，回放不重新采样或改录。 */
		virtual void PostUpdate(ACharacter* Character, EPostUpdateMode PostUpdateMode) override
		{
			Super::PostUpdate(Character, PostUpdateMode);
			if (PostUpdateMode != PostUpdate_Record || AttackWarpAvatar.Get() != Character) { return; }
			// 末态移动保存实际持有Yaw，历史恢复不重新排入一次权威校正。
			if (HitReactionFacing.bCurrent)
			{
				HitReactionFacing.RecordedYaw = Character->GetActorRotation().Yaw;
				HitReactionFacing.bHasRecordedYaw = true;
			}
			if (auto* Movement = Cast<UHodgeCharacterMovementComponent>(Character->GetCharacterMovement()))
			{
				float EvaluatedYaw = 0.f;
				if (Movement->ConsumeAttackWarpEvaluation(AttackWarpEvaluationSequence, AttackWarpAvatarGeneration,
					RotationState, RootMotionMontage.Get(), EvaluatedYaw))
				{ RotationState.ActionYaw = EvaluatedYaw; }
			}
		}

		/** 检查是否允许Combine关联。
		 * @param NewMove 新移动，由调用方传入。
		 * @param Character 角色，由调用方传入。
		 * @param MaxDelta 最大增量，由调用方传入。
		 * @return true表示满足该查询条件，false表示不满足。
		 */
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override
		{
			const FSavedMove* Other = static_cast<const FSavedMove*>(NewMove.Get());
			// 活动补转及状态交界不能把较早移动并入新时段；稳定冻结仍沿用原生合并。
			if (Other && (HitReactionFacing.Facing.IsTurning() || Other->HitReactionFacing.Facing.IsTurning() ||
				HitReactionFacing.bCorrectionPending || Other->HitReactionFacing.bCorrectionPending ||
				HitReactionFacing.bCurrent != Other->HitReactionFacing.bCurrent ||
				HitReactionFacing.Facing.ChainId != Other->HitReactionFacing.Facing.ChainId ||
				HitReactionFacing.Facing.Segment != Other->HitReactionFacing.Facing.Segment ||
				HitReactionFacing.Facing.Revision != Other->HitReactionFacing.Facing.Revision ||
				HitReactionFacing.bPaused != Other->HitReactionFacing.bPaused ||
				HitReactionFacing.bExternalLock != Other->HitReactionFacing.bExternalLock)) { return false; }
			if (!Other || RotationState.Driver != Other->RotationState.Driver || RotationState.ExecutionMode!=Other->RotationState.ExecutionMode ||
				RotationState.WarpDefinition!=Other->RotationState.WarpDefinition || RotationState.ActionYaw!=Other->RotationState.ActionYaw || RotationState.Style != Other->RotationState.Style
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

		/** 准备p移动关联。
		 * @param Character 角色，由调用方传入。
		 */
		virtual void PrepMoveFor(ACharacter* Character) override
		{
			Super::PrepMoveFor(Character);
			const auto* CurrentRotation = FindRotation(Character);
			// 受击历史跨Avatar代后连同旧控制约束一并失效，不能只跳过补转而留下旧锁。
			if (HitReactionFacing.Facing.HasIdentity() && (!CurrentRotation || !CurrentRotation->IsFacingSystemReady() ||
				HitReactionFacing.Avatar.Get() != Character || HitReactionFacing.AvatarGeneration != CurrentRotation->GetAvatarGeneration()))
			{
				if (auto* Policy = Character->FindComponentByClass<UHodgeLocomotionPolicyComponent>()) { Policy->ClearMoveReplayState(); }
				if (auto* Movement = Cast<UHodgeCharacterMovementComponent>(Character->GetCharacterMovement()))
				{ Movement->bHasReactionReplay = false; Movement->bReactionReplayControlled = false; Movement->HitReactionReplay.Reset(); }
				if (auto* Rotation = FindRotation(Character)) { Rotation->ClearMoveReplayState(); }
				return;
			}
			if (auto* Policy = Character->FindComponentByClass<UHodgeLocomotionPolicyComponent>()) { Policy->SetMoveReplayState(LocomotionState); }
			if (auto* Movement = Cast<UHodgeCharacterMovementComponent>(Character->GetCharacterMovement()))
			{
				Movement->SetHitReactionMoveReplay(bReactionControlled);
				Movement->HitReactionReplay = HitReactionFacing;
			}
			if (UHodgeCharacterRotationComponent* Rotation = FindRotation(Character))
			{
				Rotation->SetMoveReplayState(RotationState, HitReactionFacing.bExternalLock);
			}
		}

	private:
		using Super = FSavedMove_Character;
	};

	/** 封装客户端预测数据的状态与操作接口。 */
	class FClientPredictionData final : public FNetworkPredictionData_Client_Character
	{
	public:
		/** 构造客户端预测数据并初始化其默认状态。
		 * @param Movement 角色移动组件或移动数据。
		 */
		explicit FClientPredictionData(const UCharacterMovementComponent& Movement)
			: FNetworkPredictionData_Client_Character(Movement) {}

		/** 分配本项目带扩展预测状态的SavedMove。
		 * @return 本次计算、查询或创建得到的Saved移动Ptr。
		 */
		virtual FSavedMovePtr AllocateNewMove() override { return FSavedMovePtr(new FSavedMove()); }
	};

	/** 保存移动数据的数据和接口契约。 */
	struct FMoveData final : FCharacterNetworkMoveData
	{
		// 朝向序号的数值或阈值。
		uint32 FacingSequence = 0;
		int32 MontageWarpKey = 0;
		// Sprint奔跑Activation键的数值或阈值。
		int32 SprintActivationKey = 0;
		/** 处理客户端侧的填充网络移动数据消息，沿用当前身份与权限校验。
		 * @param Move 移动，由调用方传入。
		 * @param Type 类型，由调用方传入。
		 */
		virtual void ClientFillNetworkMoveData(const FSavedMove_Character& Move, ENetworkMoveType Type) override
		{
			FCharacterNetworkMoveData::ClientFillNetworkMoveData(Move, Type);
			FacingSequence = static_cast<const FSavedMove&>(Move).RotationState.RequestSequence;
			const auto& Facing=static_cast<const FSavedMove&>(Move).RotationState;
			MontageWarpKey=Facing.ExecutionMode==EHodgeFacingExecutionMode::MontageWarp ? Facing.ActionPredictionKey : 0;
			SprintActivationKey = static_cast<const FSavedMove&>(Move).LocomotionState.bSprinting ? static_cast<const FSavedMove&>(Move).LocomotionState.ActivationKey : 0;
		}
		/** 读取或写入当前对象的序列化数据。
		 * @param Movement 角色移动组件或移动数据。
		 * @param Ar 读取或写入数据的归档对象。
		 * @param Map 序列化使用的对象映射或当前映射表。
		 * @param Type 类型，由调用方传入。
		 * @return true表示本次检查或操作成功，false表示失败。
		 */
		virtual bool Serialize(UCharacterMovementComponent& Movement, FArchive& Ar, UPackageMap* Map, ENetworkMoveType Type) override
		{
			const bool bValid = FCharacterNetworkMoveData::Serialize(Movement, Ar, Map, Type);
			Ar.SerializeIntPacked(FacingSequence);
			Ar << MontageWarpKey;
			Ar << SprintActivationKey;
			return bValid && !Ar.IsError();
		}
	};

	/** 保存移动数据Container的数据和接口契约。 */
	struct FMoveDataContainer final : FCharacterNetworkMoveDataContainer
	{
		// Moves的数据或对象引用。
		FMoveData Moves[3];
		/** 构造移动数据Container并初始化其默认状态。 */
		FMoveDataContainer() { NewMoveData = &Moves[0]; PendingMoveData = &Moves[1]; OldMoveData = &Moves[2]; }
	};
}


/** 构造角色移动组件并初始化其默认状态。 */
UHodgeCharacterMovementComponent::UHodgeCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FacingMoveData = MakeUnique<HodgeRotationPrediction::FMoveDataContainer>();
	SetNetworkMoveDataContainer(*FacingMoveData);
}

// 模拟角色移动，存在服务端同步加速度时需要保护复制过来的加速度不被父类逻辑覆盖
void UHodgeCharacterMovementComponent::PerformMovement(float DeltaTime)
{
	BeginAttackWarpEvaluation();
	ON_SCOPE_EXIT { EndAttackWarpEvaluation(); };
	// 每个真实/回放移动只消费一份快照，正常输入和旧攻击不能争写本次Yaw。
	if (HasValidData() && IsActive() && MovementMode != MOVE_None && FMath::IsFinite(DeltaTime) && DeltaTime > 0.f)
	{
		const bool bReplay = CharacterOwner && CharacterOwner->bClientUpdating && HitReactionReplay.IsSet();
		ApplyHitReactionFacing(bReplay ? HitReactionReplay.GetValue() : CaptureHitReactionFacing(DeltaTime), DeltaTime);
	}
	const auto* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	// 后续GAS操作使用的能力系统组件。
	const auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CharacterOwner);
	const bool bStopped = IsHitReactionControlled() || (ASC && (ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) ||
		ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped)));
	if (Policy && !bStopped)
	{
		// 保存帧可能带回交接前的 RMS，按来源恢复成功退出策略再执行引擎清理。
		for (const auto* Sources : {&CurrentRootMotion.RootMotionSources, &CurrentRootMotion.PendingAddRootMotionSources})
		{
			for (const auto& Source : *Sources)
			{
				float MaximumSpeed = 0.f;
				bool bClamp = false;
				if (Source && Policy->GetDashFinishVelocity(Source->InstanceName, IsFalling(), MaximumSpeed, bClamp))
				{
					// 活动源与待加入源使用同一策略，避免首个物理步保留过时收尾模式。
					Source->FinishVelocityParams.Mode = bClamp ? ERootMotionFinishVelocityMode::ClampVelocity : ERootMotionFinishVelocityMode::SetVelocity;
					Source->FinishVelocityParams.ClampVelocity = MaximumSpeed;
					if (!bClamp) { Source->FinishVelocityParams.SetVelocity = FVector::ZeroVector; }
				}
			}
		}
	}
	Super::PerformMovement(DeltaTime);
}

/** 解绑只退役本组件的补转缓存，不能旋回旧朝向或移除共享标签。 */
void UHodgeCharacterMovementComponent::ResetHitReactionFacing()
{
	HitReactionReplay.Reset();
	HitReactionReplayAnchor.Reset();
	bHasReactionReplay = false;
	bReactionReplayControlled = false;
	LiveFacingChainId.Invalidate();
	LiveFacingSegment = 0;
	LiveFacingAvatarGeneration = INDEX_NONE;
	LastFacingEvaluatedAt = -MAX_flt;
	bFacingLocallyFrozen = false;
	LocallyFrozenFacingYaw = 0.f;
	CorrectedFacingChainId.Invalidate();
	CorrectedFacingSegment = CorrectedFacingRevision = 0;
	CorrectedFacingYaw = 0.f;
	bNativeFacingFinalRestorePending = false;
}

/** 当前资格只读受击组件，通用FacingSequence不能重新授权旧段。 */
bool UHodgeCharacterMovementComponent::IsLiveHitReactionFacing(const FHodgeHitReactionFacingState& Facing) const
{
	const auto* Reaction = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeHitReactionComponent>() : nullptr;
	return Facing.HasIdentity() && Reaction && Reaction->IsFacingStateCurrent(Facing.ChainId, Facing.Segment);
}

/** 在录制点冻结时间和锁原因，服务器不消费客户端上报的Yaw或Elapsed。 */
UHodgeCharacterMovementComponent::FHitReactionFacingMoveSnapshot UHodgeCharacterMovementComponent::CaptureHitReactionFacing(float DeltaTime) const
{
	FHitReactionFacingMoveSnapshot Snapshot;
	const auto* Reaction = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeHitReactionComponent>() : nullptr;
	const auto* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	if (!Reaction || !Rotation || !Rotation->IsFacingSystemReady()) { return Snapshot; }
	Snapshot.Facing = Reaction->GetFacingState();
	Snapshot.Avatar = CharacterOwner;
	Snapshot.AvatarGeneration = Rotation->GetAvatarGeneration();
	Snapshot.bCurrent = IsLiveHitReactionFacing(Snapshot.Facing);
	Snapshot.bPaused = Reaction->IsFacingPaused();
	Snapshot.bExternalLock = Rotation->IsExternallyYawLocked();
	Snapshot.EvaluatedAt = Reaction->GetServerTime();
	const bool bSameSegment = LiveFacingChainId == Snapshot.Facing.ChainId && LiveFacingSegment == Snapshot.Facing.Segment &&
		LiveFacingAvatarGeneration == Snapshot.AvatarGeneration;
	Snapshot.EvaluationStartedAt = FMath::Max(Snapshot.EvaluatedAt - FMath::Max(0.f, DeltaTime),
		bSameSegment ? LastFacingEvaluatedAt : Snapshot.Facing.StartedAt);
	Snapshot.bLocallyFrozen = bSameSegment && bFacingLocallyFrozen;
	Snapshot.bCorrectionPending = Snapshot.bCurrent && Snapshot.Facing.bHasTarget && Snapshot.Facing.bFinal && !CharacterOwner->HasAuthority() &&
		(CorrectedFacingChainId != Snapshot.Facing.ChainId || CorrectedFacingSegment != Snapshot.Facing.Segment ||
		 CorrectedFacingRevision != Snapshot.Facing.Revision || bNativeFacingFinalRestorePending);
	return Snapshot;
}

/** 唯一合法写入仍经过CMC移动接口，世界位移和速度保持原值。 */
void UHodgeCharacterMovementComponent::MoveHitReactionFacingYaw(float Yaw)
{
	if (!CharacterOwner || !UpdatedComponent || !FMath::IsFinite(Yaw)) { return; }
	FRotator Facing = UpdatedComponent->GetComponentRotation();
	Facing.Yaw = FRotator::NormalizeAxis(Yaw);
	TGuardValue<bool> Guard(bApplyingHitReactionFacing, true);
	MoveUpdatedComponent(FVector::ZeroVector, Facing, false);
	if (auto* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner))
	{ Rotation->UpdateHitReactionLockedYaw(UpdatedComponent->GetComponentRotation().Yaw); }
}

/** 标签回调先记录当前Yaw；移除硬锁不清除此段的本地退役事实。 */
void UHodgeCharacterMovementComponent::FreezeHitReactionFacing()
{
	if (!CharacterOwner || CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy || CharacterOwner->bClientUpdating) { return; }
	auto* Reaction = CharacterOwner->FindComponentByClass<UHodgeHitReactionComponent>();
	auto* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	if (!Reaction || !Rotation) { return; }
	const FHodgeHitReactionFacingState Facing = Reaction->GetFacingState();
	if (!IsLiveHitReactionFacing(Facing)) { return; }
	if (LiveFacingChainId != Facing.ChainId || LiveFacingSegment != Facing.Segment ||
		LiveFacingAvatarGeneration != Rotation->GetAvatarGeneration())
	{
		LiveFacingChainId = Facing.ChainId;
		LiveFacingSegment = Facing.Segment;
		LiveFacingAvatarGeneration = Rotation->GetAvatarGeneration();
		LastFacingEvaluatedAt = Facing.StartedAt;
		bFacingLocallyFrozen = false;
	}
	if (!bFacingLocallyFrozen)
	{
		bFacingLocallyFrozen = true;
		LocallyFrozenFacingYaw = CharacterOwner->GetActorRotation().Yaw;
		UE_LOG(LogHodgeHitReactionFacing, Log, TEXT("Event=HardFreeze Avatar=%s Role=%d Chain=%s Segment=%u Revision=%u ServerTime=%.9f WorldTime=%.9f Yaw=%.9f Authority=%d"),
			*GetPathNameSafe(CharacterOwner), int32(CharacterOwner->GetLocalRole()), *Facing.ChainId.ToString(), Facing.Segment,
			Facing.Revision, Reaction->GetServerTime(), GetWorld()->GetTimeSeconds(), LocallyFrozenFacingYaw, CharacterOwner->HasAuthority());
	}
	Rotation->UpdateHitReactionLockedYaw(LocallyFrozenFacingYaw);
	if (CharacterOwner->HasAuthority())
	{ Reaction->FinishFacingSegment(Facing.ChainId, Facing.Segment, Facing.Revision, LocallyFrozenFacingYaw, true); }
}

/** 正常补转只执行本步与世界时窗的交集，历史回放不读取当前时间或来源。 */
void UHodgeCharacterMovementComponent::ApplyHitReactionFacing(const FHitReactionFacingMoveSnapshot& Snapshot, float DeltaTime)
{
	if (!CharacterOwner || CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy) { return; }
	const auto* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	const bool bReplay = CharacterOwner->bClientUpdating && HitReactionReplay.IsSet();
	if (!Snapshot.bCurrent || Snapshot.Avatar.Get() != CharacterOwner || !Rotation || !Rotation->IsFacingSystemReady() ||
		Snapshot.AvatarGeneration != Rotation->GetAvatarGeneration())
	{
		if (!bReplay && LiveFacingChainId.IsValid()) { ResetHitReactionFacing(); }
		return;
	}
	const FHodgeHitReactionFacingState& Facing = Snapshot.Facing;
	const float BeforeYaw = CharacterOwner->GetActorRotation().Yaw;
	float EffectiveDelta = 0.f;
	// 历史持有姿态恢复单独留证，不能把它记作正常补转的角速率。
	const auto RestoreRecordedYaw = [&]()
	{
		const float BeforeRestore = CharacterOwner->GetActorRotation().Yaw;
		MoveHitReactionFacingYaw(Snapshot.RecordedYaw);
		UE_LOG(LogHodgeHitReactionFacing, Log, TEXT("Event=HistoricalRestore Avatar=%s Role=%d Chain=%s Segment=%u Revision=%u ServerTime=%.9f WorldTime=%.9f DeltaTime=0.000000000 BeforeYaw=%.9f AfterYaw=%.9f RecordedYaw=%.9f"),
			*GetPathNameSafe(CharacterOwner), int32(CharacterOwner->GetLocalRole()), *Facing.ChainId.ToString(), Facing.Segment,
			Facing.Revision, Snapshot.EvaluatedAt, GetWorld()->GetTimeSeconds(), BeforeRestore,
			CharacterOwner->GetActorRotation().Yaw, Snapshot.RecordedYaw);
	};
	ON_SCOPE_EXIT
	{
		UE_LOG(LogHodgeHitReactionFacing, Log, TEXT("Event=Step Avatar=%s Role=%d Chain=%s Segment=%u Revision=%u Replay=%d Paused=%d Hard=%d Final=%d ServerTime=%.9f WorldTime=%.9f From=%.9f DeltaTime=%.9f EffectiveDeltaTime=%.9f BeforeYaw=%.9f AfterYaw=%.9f TargetYaw=%.9f MaxYawSpeed=%.9f"),
			*GetPathNameSafe(CharacterOwner), int32(CharacterOwner->GetLocalRole()), *Facing.ChainId.ToString(), Facing.Segment,
			Facing.Revision, bReplay, Snapshot.bPaused, Snapshot.bExternalLock, Facing.bFinal, Snapshot.EvaluatedAt,
			GetWorld()->GetTimeSeconds(), Snapshot.EvaluationStartedAt, DeltaTime, EffectiveDelta, BeforeYaw,
			CharacterOwner->GetActorRotation().Yaw, Facing.TargetYaw, Facing.MaxYawSpeed);
	};
	if (!bReplay)
	{
		if (LiveFacingChainId != Facing.ChainId || LiveFacingSegment != Facing.Segment ||
			LiveFacingAvatarGeneration != Snapshot.AvatarGeneration)
		{
			// 迟到Final可能先在原生回放收尾消费；首次建立Live段不能重复消费同一最终修订。
			const bool bKeepConsumedFinal = Facing.bFinal && CorrectedFacingChainId == Facing.ChainId &&
				CorrectedFacingSegment == Facing.Segment && CorrectedFacingRevision == Facing.Revision;
			const float ConsumedYaw = CorrectedFacingYaw;
			const bool bKeepNativeRestore = bKeepConsumedFinal && bNativeFacingFinalRestorePending;
			ResetHitReactionFacing();
			if (bKeepConsumedFinal)
			{
				CorrectedFacingChainId = Facing.ChainId; CorrectedFacingSegment = Facing.Segment;
				CorrectedFacingRevision = Facing.Revision; CorrectedFacingYaw = ConsumedYaw;
				bNativeFacingFinalRestorePending = bKeepNativeRestore;
			}
			LiveFacingChainId = Facing.ChainId;
			LiveFacingSegment = Facing.Segment;
			LiveFacingAvatarGeneration = Snapshot.AvatarGeneration;
		}
		LastFacingEvaluatedAt = FMath::Max(LastFacingEvaluatedAt, Snapshot.EvaluatedAt);
		if (Snapshot.bExternalLock) { FreezeHitReactionFacing(); }
	}
	if (Facing.bFinal)
	{
		if (!Snapshot.bPaused)
		{
			if (bReplay && Snapshot.bHasRecordedYaw) { RestoreRecordedYaw(); }
			else if (!bReplay) { ApplyHitReactionFacingCorrection(); }
		}
		return;
	}
	if (!bReplay && CharacterOwner->HasAuthority() && (Snapshot.bExternalLock || bFacingLocallyFrozen ||
		!Facing.bHasTarget || (Snapshot.bPaused && Snapshot.EvaluatedAt >= Facing.EndsAt)))
	{
		if (auto* Reaction = CharacterOwner->FindComponentByClass<UHodgeHitReactionComponent>())
		{ Reaction->FinishFacingSegment(Facing.ChainId, Facing.Segment, Facing.Revision, BeforeYaw, Snapshot.bExternalLock || bFacingLocallyFrozen); }
		return;
	}
	if (Snapshot.bPaused || Snapshot.bExternalLock || Snapshot.bLocallyFrozen || (!bReplay && bFacingLocallyFrozen))
	{
		if (bReplay && Snapshot.bLocallyFrozen && Snapshot.bHasRecordedYaw && !Snapshot.bPaused)
		{ RestoreRecordedYaw(); }
		return;
	}
	if (!Facing.bHasTarget || !FMath::IsFinite(Facing.StartedAt) || !FMath::IsFinite(Facing.EndsAt) ||
		!FMath::IsFinite(Facing.TargetYaw) || !FMath::IsFinite(Facing.MaxYawSpeed) || Facing.MaxYawSpeed <= 0.f ||
		!FMath::IsFinite(Snapshot.EvaluatedAt) || !FMath::IsFinite(Snapshot.EvaluationStartedAt)) { return; }
	EffectiveDelta = FMath::Min(DeltaTime, FMath::Max(0.f, FMath::Min(Snapshot.EvaluatedAt, Facing.EndsAt) -
		FMath::Max(Snapshot.EvaluationStartedAt, Facing.StartedAt)));
	float Difference = FMath::FindDeltaAngleDegrees(BeforeYaw, Facing.TargetYaw);
	if (FMath::IsNearlyEqual(FMath::Abs(Difference), 180.f, UE_KINDA_SMALL_NUMBER)) { Difference = 180.f; }
	const double MaximumStep = double(Facing.MaxYawSpeed) * double(EffectiveDelta);
	const float Step = FMath::Sign(Difference) * float(FMath::Min(double(FMath::Abs(Difference)), MaximumStep));
	if (EffectiveDelta > 0.f && !FMath::IsNearlyZero(Step)) { MoveHitReactionFacingYaw(BeforeYaw + Step); }
	else if (auto* CurrentRotation = HodgeRotationPrediction::FindRotation(CharacterOwner))
	{
		// 零步长或已到目标也要同步本步锁参考，Super不能回填SavedMove录制前的旧Yaw。
		CurrentRotation->UpdateHitReactionLockedYaw(BeforeYaw);
	}
	if (!bReplay && CharacterOwner->HasAuthority() && (Snapshot.EvaluatedAt >= Facing.EndsAt ||
		FMath::Abs(FMath::FindDeltaAngleDegrees(CharacterOwner->GetActorRotation().Yaw, Facing.TargetYaw)) <= UE_KINDA_SMALL_NUMBER))
	{
		if (auto* Reaction = CharacterOwner->FindComponentByClass<UHodgeHitReactionComponent>())
		{ Reaction->FinishFacingSegment(Facing.ChainId, Facing.Segment, Facing.Revision, CharacterOwner->GetActorRotation().Yaw, false); }
	}
}

/** 同段Final只校正一次；原生新ACK或回放收尾仅恢复已消费约束，不再排校正。 */
void UHodgeCharacterMovementComponent::ApplyHitReactionFacingCorrection(bool bRestoreAfterReplay)
{
	if (!HasValidData() || !IsActive() || MovementMode == MOVE_None || !CharacterOwner ||
		CharacterOwner->HasAuthority() || !CharacterOwner->IsLocallyControlled() || CharacterOwner->bClientUpdating) { return; }
	const auto* Reaction = CharacterOwner->FindComponentByClass<UHodgeHitReactionComponent>();
	const auto* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	if (!Reaction || !Rotation || !Rotation->IsFacingSystemReady() || Reaction->IsFacingPaused()) { return; }
	const FHodgeHitReactionFacingState Facing = Reaction->GetFacingState();
	if (!IsLiveHitReactionFacing(Facing) || !Facing.bHasTarget || !Facing.bFinal || !FMath::IsFinite(Facing.FinalYaw)) { return; }
	const bool bConsumed = CorrectedFacingChainId == Facing.ChainId && CorrectedFacingSegment == Facing.Segment &&
		CorrectedFacingRevision == Facing.Revision;
	if (bConsumed && !bRestoreAfterReplay && !bNativeFacingFinalRestorePending) { return; }
	const float BeforeYaw = CharacterOwner->GetActorRotation().Yaw;
	MoveHitReactionFacingYaw(bConsumed ? CorrectedFacingYaw : Facing.FinalYaw);
	// 同段权威校正同时更新本地冻结参考，后续硬锁求值不能恢复校正前的Yaw。
	if (bFacingLocallyFrozen && LiveFacingChainId == Facing.ChainId && LiveFacingSegment == Facing.Segment &&
		LiveFacingAvatarGeneration == Rotation->GetAvatarGeneration())
	{ LocallyFrozenFacingYaw = CharacterOwner->GetActorRotation().Yaw; }
	bNativeFacingFinalRestorePending = false;
	if (!bConsumed)
	{
		CorrectedFacingChainId = Facing.ChainId;
		CorrectedFacingSegment = Facing.Segment;
		CorrectedFacingRevision = Facing.Revision;
		CorrectedFacingYaw = Facing.FinalYaw;
	}
	UE_LOG(LogHodgeHitReactionFacing, Log, TEXT("Event=%s Avatar=%s Role=%d Chain=%s Segment=%u Revision=%u ServerTime=%.9f WorldTime=%.9f DeltaTime=0.000000000 BeforeYaw=%.9f AfterYaw=%.9f FinalYaw=%.9f"),
		bConsumed ? TEXT("FrozenRestore") : TEXT("Correction"), *GetPathNameSafe(CharacterOwner), int32(CharacterOwner->GetLocalRole()),
		*Facing.ChainId.ToString(), Facing.Segment, Facing.Revision, Reaction->GetServerTime(), GetWorld()->GetTimeSeconds(),
		BeforeYaw, CharacterOwner->GetActorRotation().Yaw, Facing.FinalYaw);
}

/** 同Avatar代的历史参考与原生响应身份共用值快照，不改变旧攻击Warp回放。 */
UHodgeCharacterMovementComponent::FHitReactionReplayAnchor UHodgeCharacterMovementComponent::CaptureHitReactionReplayAnchor(
	bool bMayRestore, bool bRequireFacingHistory) const
{
	FHitReactionReplayAnchor Anchor;
	const auto* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	const auto* Prediction = GetPredictionData_Client_Character();
	if (!CharacterOwner || !Rotation || !Rotation->IsFacingSystemReady() || !Prediction) { return Anchor; }
	bool bHasFacingHistory = false;
	for (const FSavedMovePtr& Move : Prediction->SavedMoves)
	{
		const auto* Saved = static_cast<const HodgeRotationPrediction::FSavedMove*>(Move.Get());
		if (Saved && Saved->HitReactionFacing.bCurrent && Saved->HitReactionFacing.Avatar.Get() == CharacterOwner &&
			Saved->HitReactionFacing.AvatarGeneration == Rotation->GetAvatarGeneration()) { bHasFacingHistory = true; break; }
	}
	if (bRequireFacingHistory && !bHasFacingHistory) { return Anchor; }
	const auto* Reaction = CharacterOwner->FindComponentByClass<UHodgeHitReactionComponent>();
	const FHodgeHitReactionFacingState Facing = Reaction ? Reaction->GetFacingState() : FHodgeHitReactionFacingState();
	const FHodgeCharacterRotationState RotationState = Rotation->GetResolvedState();
	Anchor.Avatar = CharacterOwner;
	Anchor.AvatarGeneration = Rotation->GetAvatarGeneration();
	Anchor.ChainId = Facing.ChainId;
	Anchor.Segment = Facing.Segment;
	Anchor.ActionRequestId = RotationState.ActionRequestId;
	Anchor.Driver = uint8(RotationState.Driver);
	Anchor.VisibleYaw = CharacterOwner->GetActorRotation().Yaw;
	Anchor.bMayRestore = bMayRestore;
	Anchor.bFacingCurrent = IsLiveHitReactionFacing(Facing);
	return Anchor;
}

/** 时间戳可能重置，确认身份依赖Super前实际存在的未确认移动对象。 */
FSavedMovePtr UHodgeCharacterMovementComponent::FindHitReactionCorrectionMove(float TimeStamp) const
{
	if (!HasValidData() || !IsActive() || !FMath::IsFinite(TimeStamp) || CharacterOwner->HasAuthority() ||
		!CharacterOwner->IsLocallyControlled() || CharacterOwner->bClientUpdating) { return {}; }
	const auto* Prediction = GetPredictionData_Client_Character();
	const int32 Index = Prediction ? Prediction->GetSavedMoveIndex(TimeStamp) : INDEX_NONE;
	return Index != INDEX_NONE ? Prediction->SavedMoves[Index] : FSavedMovePtr();
}

/** 原生拒绝、重复或仍未移除的移动均不能产生新的受击朝向授权。 */
bool UHodgeCharacterMovementComponent::IsHitReactionCorrectionAcknowledged(const FSavedMovePtr& ExpectedAck,
	const FSavedMovePtr& PreviousAck, float TimeStamp) const
{
	if (!ExpectedAck.IsValid() || ExpectedAck == PreviousAck) { return false; }
	const auto* Prediction = GetPredictionData_Client_Character();
	return Prediction && Prediction->LastAckedMove == ExpectedAck && ExpectedAck->TimeStamp == TimeStamp &&
		!Prediction->SavedMoves.Contains(ExpectedAck);
}

/** 原生响应只在真正新ACK后更新当前锁；历史回放与Final去重仍分别处理。 */
void UHodgeCharacterMovementComponent::CommitHitReactionNativeCorrection(const FHitReactionReplayAnchor& Before,
	const FSavedMovePtr& ExpectedAck, const FSavedMovePtr& PreviousAck, float TimeStamp,
	const TCHAR* NativeType, float BeforeYaw, float ServerYaw)
{
	if (!HasValidData() || !IsActive() || !IsHitReactionCorrectionAcknowledged(ExpectedAck, PreviousAck, TimeStamp) ||
		MovementMode == MOVE_None || CharacterOwner->HasAuthority() || !CharacterOwner->IsLocallyControlled() ||
		CharacterOwner->bClientUpdating || !FMath::IsFinite(BeforeYaw) || !FMath::IsFinite(ServerYaw)) { return; }
	auto* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	const auto* Reaction = CharacterOwner->FindComponentByClass<UHodgeHitReactionComponent>();
	if (!Rotation || !Rotation->IsFacingSystemReady() || !Reaction || Before.Avatar.Get() != CharacterOwner ||
		Before.AvatarGeneration != Rotation->GetAvatarGeneration()) { return; }
	const FHodgeHitReactionFacingState Facing = Reaction->GetFacingState();
	const FHodgeCharacterRotationState Current = Rotation->GetResolvedState();
	if (!Before.bFacingCurrent || !IsLiveHitReactionFacing(Facing) || Before.ChainId != Facing.ChainId || Before.Segment != Facing.Segment ||
		Before.ActionRequestId != Current.ActionRequestId || Before.Driver != uint8(Current.Driver)) { return; }
	const float NativeYaw = CharacterOwner->GetActorRotation().Yaw;
	if (!FMath::IsFinite(NativeYaw)) { return; }
	Rotation->UpdateHitReactionLockedYaw(NativeYaw);
	if (bFacingLocallyFrozen && LiveFacingChainId == Facing.ChainId && LiveFacingSegment == Facing.Segment &&
		LiveFacingAvatarGeneration == Rotation->GetAvatarGeneration()) { LocallyFrozenFacingYaw = NativeYaw; }
	FHitReactionReplayAnchor NativeAnchor = Before;
	NativeAnchor.VisibleYaw = NativeYaw;
	NativeAnchor.bMayRestore = NativeAnchor.bNativeCorrection = true;
	HitReactionReplayAnchor = NativeAnchor;
	bNativeFacingFinalRestorePending = Facing.bFinal && CorrectedFacingChainId == Facing.ChainId &&
		CorrectedFacingSegment == Facing.Segment && CorrectedFacingRevision == Facing.Revision;
	UE_LOG(LogHodgeHitReactionFacing, Log, TEXT("Event=NativeCorrection NativeType=%s MoveTimeStamp=%.9f Avatar=%s Role=%d Chain=%s Segment=%u Revision=%u ServerTime=%.9f WorldTime=%.9f DeltaTime=0.000000000 BeforeYaw=%.9f AfterYaw=%.9f NativeYaw=%.9f ServerYaw=%.9f"),
		NativeType, TimeStamp, *GetPathNameSafe(CharacterOwner), int32(CharacterOwner->GetLocalRole()),
		*Facing.ChainId.ToString(), Facing.Segment, Facing.Revision, Reaction->GetServerTime(), GetWorld()->GetTimeSeconds(),
		BeforeYaw, NativeYaw, NativeYaw, FRotator::NormalizeAxis(ServerYaw));
}

/** RMS尾部可能误用相等LastAcked时间戳写Yaw，局部恢复不改原生ACK或源同步。 */
void UHodgeCharacterMovementComponent::RestoreRejectedHitReactionNativeCorrection(const FHitReactionReplayAnchor& Before,
	const FSavedMovePtr& ExpectedAck, const FSavedMovePtr& PreviousAck, float TimeStamp)
{
	if (!HasValidData() || !IsActive() || CharacterOwner->HasAuthority() || !CharacterOwner->IsLocallyControlled() ||
		CharacterOwner->bClientUpdating || IsHitReactionCorrectionAcknowledged(ExpectedAck, PreviousAck, TimeStamp)) { return; }
	const auto* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	const auto* Reaction = CharacterOwner->FindComponentByClass<UHodgeHitReactionComponent>();
	if (!Rotation || !Rotation->IsFacingSystemReady() || !Reaction || Before.Avatar.Get() != CharacterOwner ||
		Before.AvatarGeneration != Rotation->GetAvatarGeneration()) { return; }
	const FHodgeHitReactionFacingState Facing = Reaction->GetFacingState();
	const FHodgeCharacterRotationState Current = Rotation->GetResolvedState();
	if (!Before.bFacingCurrent || !IsLiveHitReactionFacing(Facing) || Before.ChainId != Facing.ChainId || Before.Segment != Facing.Segment ||
		Before.ActionRequestId != Current.ActionRequestId || Before.Driver != uint8(Current.Driver)) { return; }
	const float RejectedYaw = CharacterOwner->GetActorRotation().Yaw;
	if (!FMath::IsFinite(Before.VisibleYaw) || !FMath::IsFinite(RejectedYaw) ||
		FMath::IsNearlyZero(FMath::FindDeltaAngleDegrees(Before.VisibleYaw, RejectedYaw), UE_SMALL_NUMBER)) { return; }
	MoveHitReactionFacingYaw(Before.VisibleYaw);
	UE_LOG(LogHodgeHitReactionFacing, Log, TEXT("Event=NativeRejectedRestore NativeType=RMS MoveTimeStamp=%.9f Avatar=%s Role=%d Chain=%s Segment=%u Revision=%u ServerTime=%.9f WorldTime=%.9f BeforeYaw=%.9f RejectedYaw=%.9f AfterYaw=%.9f"),
		TimeStamp, *GetPathNameSafe(CharacterOwner), int32(CharacterOwner->GetLocalRole()), *Facing.ChainId.ToString(),
		Facing.Segment, Facing.Revision, Reaction->GetServerTime(), GetWorld()->GetTimeSeconds(), Before.VisibleYaw,
		RejectedYaw, CharacterOwner->GetActorRotation().Yaw);
}

/** 历史计算结束再恢复当前占用；旧段Yaw只参与历史模拟，不能留到可见末态。 */
void UHodgeCharacterMovementComponent::RestoreHitReactionFacingAfterReplay(const FHitReactionReplayAnchor& Anchor)
{
	auto* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	if (!Anchor.bMayRestore || !CharacterOwner || Anchor.Avatar.Get() != CharacterOwner || !Rotation ||
		!Rotation->IsFacingSystemReady() || Rotation->GetAvatarGeneration() != Anchor.AvatarGeneration) { return; }
	const auto* Reaction = CharacterOwner->FindComponentByClass<UHodgeHitReactionComponent>();
	const FHodgeHitReactionFacingState Facing = Reaction ? Reaction->GetFacingState() : FHodgeHitReactionFacingState();
	// 根运动接管或显式退役后只保留旧段记录，任何旧补转锚点都不能覆盖当前原生朝向。
	if (Facing.bRetired || (Reaction && Reaction->GetReactionState().IsActive() &&
		Reaction->GetReactionState().MotionSource == EHodgeReactionMotionSource::AnimationRootMotion)) { return; }
	const FHodgeCharacterRotationState Current = Rotation->GetResolvedState();
	if (Anchor.ChainId != Facing.ChainId || Anchor.Segment != Facing.Segment ||
		Anchor.ActionRequestId != Current.ActionRequestId || Anchor.Driver != uint8(Current.Driver)) { return; }
	if (!IsLiveHitReactionFacing(Facing) && !Current.bYawLocked && !IsHitReactionControlled())
	{
		const auto* Prediction = GetPredictionData_Client_Character();
		bool bHasFacingHistory = false;
		const HodgeRotationPrediction::FSavedMove* Last = nullptr;
		for (const FSavedMovePtr& Move : Prediction->SavedMoves)
		{
			const auto* Saved = static_cast<const HodgeRotationPrediction::FSavedMove*>(Move.Get());
			Last = Saved;
			if (Saved && Saved->HitReactionFacing.bCurrent && Saved->HitReactionFacing.Avatar.Get() == CharacterOwner &&
				Saved->HitReactionFacing.AvatarGeneration == Rotation->GetAvatarGeneration()) { bHasFacingHistory = true; }
		}
		// GoodMove已移除受击历史，或尾步已合法重放当前新动作/基础驱动时，不覆盖其原生输出。
		if (!bHasFacingHistory) { return; }
		if (Last && Last->AttackWarpAvatar.Get() == CharacterOwner &&
			Last->AttackWarpAvatarGeneration == Rotation->GetAvatarGeneration() && !Last->HitReactionFacing.bCurrent &&
			!Last->bReactionControlled && !Last->RotationState.bYawLocked &&
			Last->RotationState.Driver == Current.Driver && Last->RotationState.ExecutionMode == Current.ExecutionMode &&
			Last->RotationState.ActionRequestId == Current.ActionRequestId &&
			Last->RotationState.ActionAbilityHandle == Current.ActionAbilityHandle &&
			Last->RotationState.ActionPredictionKey == Current.ActionPredictionKey &&
			Last->RotationState.RequestSequence == Current.RequestSequence) { return; }
	}
	// 最终有源段由一次权威校正负责；暂停时只恢复当前冻结参考，不消费最终校正。
	if (IsLiveHitReactionFacing(Facing) && Facing.bFinal && Facing.bHasTarget && !Reaction->IsFacingPaused()) { return; }
	const float BeforeYaw = CharacterOwner->GetActorRotation().Yaw;
	const float RestoreYaw = Anchor.bNativeCorrection ? Anchor.VisibleYaw : Current.bYawLocked ? Current.LockedYaw : Anchor.VisibleYaw;
	MoveHitReactionFacingYaw(RestoreYaw);
	UE_LOG(LogHodgeHitReactionFacing, Log, TEXT("Event=CurrentRestore Avatar=%s Role=%d Chain=%s Segment=%u Revision=%u ServerTime=%.9f WorldTime=%.9f DeltaTime=0.000000000 BeforeYaw=%.9f AfterYaw=%.9f AnchorYaw=%.9f"),
		*GetPathNameSafe(CharacterOwner), int32(CharacterOwner->GetLocalRole()), *Facing.ChainId.ToString(), Facing.Segment,
		Facing.Revision, Reaction ? Reaction->GetServerTime() : GetWorld()->GetTimeSeconds(), GetWorld()->GetTimeSeconds(),
		BeforeYaw, CharacterOwner->GetActorRotation().Yaw, RestoreYaw);
}

/** 开始移动步并清理旧求值，回放不开放实时录入。 */
void UHodgeCharacterMovementComponent::BeginAttackWarpEvaluation()
{
	++AttackWarpEvaluationSequence;
	ActiveAttackWarpEvaluation.Reset();
	CompletedAttackWarpEvaluation.Reset();
	bAttackWarpEvaluationAmbiguous = false;
	bAttackWarpEvaluationActive = CharacterOwner && !CharacterOwner->bClientUpdating;
}

/** 关闭录入并移交本步完成结果，歧义输入不进入回填。 */
void UHodgeCharacterMovementComponent::EndAttackWarpEvaluation()
{
	if (bAttackWarpEvaluationActive && !bAttackWarpEvaluationAmbiguous)
	{ CompletedAttackWarpEvaluation = ActiveAttackWarpEvaluation; }
	ActiveAttackWarpEvaluation.Reset();
	bAttackWarpEvaluationActive = false;
	bAttackWarpEvaluationAmbiguous = false;
}

/** 为 SetMoveFor 预记下一移动求值序号。 */
uint64 UHodgeCharacterMovementComponent::GetNextAttackWarpEvaluationSequence() const
{
	return AttackWarpEvaluationSequence + 1;
}

/** 捕获实际 Warp 输入，同一步不同身份或方向不互相覆盖。 */
void UHodgeCharacterMovementComponent::RecordAttackWarpEvaluation(
	const FHodgeCharacterRotationState& State, const UAnimMontage* Montage)
{
	if (!bAttackWarpEvaluationActive || bAttackWarpEvaluationAmbiguous || !CharacterOwner || CharacterOwner->bClientUpdating)
	{ return; }
	const auto* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	const auto* Definition = State.WarpDefinition.Get();
	if (!Rotation || !Definition || !Definition->FacingWarp.bTrackExecutionLockTargetDuringWarp ||
		!Montage || Definition->ExecutionConfig.Montage != Montage ||
		!State.ActionRequestId.IsValid() || !State.ActionAbilityHandle.IsValid() || !FMath::IsFinite(State.ActionYaw) ||
		State.Driver != EHodgeCharacterFacingDriver::ActionDirection || State.ExecutionMode != EHodgeFacingExecutionMode::MontageWarp || State.bYawLocked)
	{ return; }
	FAttackWarpEvaluation Evaluation;
	Evaluation.Sequence = AttackWarpEvaluationSequence;
	Evaluation.RequestId = State.ActionRequestId;
	Evaluation.AbilityHandle = State.ActionAbilityHandle;
	Evaluation.PredictionKey = State.ActionPredictionKey;
	Evaluation.AvatarGeneration = Rotation->GetAvatarGeneration();
	Evaluation.Avatar = CharacterOwner;
	Evaluation.Definition = Definition;
	Evaluation.Montage = Montage;
	Evaluation.ActionYaw = State.ActionYaw;
	if (ActiveAttackWarpEvaluation.IsSet())
	{
		const auto& Previous = ActiveAttackWarpEvaluation.GetValue();
		if (Previous.RequestId != Evaluation.RequestId || Previous.AbilityHandle != Evaluation.AbilityHandle ||
			Previous.PredictionKey != Evaluation.PredictionKey || Previous.AvatarGeneration != Evaluation.AvatarGeneration ||
			Previous.Avatar != Evaluation.Avatar || Previous.Definition != Evaluation.Definition ||
			Previous.Montage != Evaluation.Montage || Previous.ActionYaw != Evaluation.ActionYaw)
		{
			// 单个SavedMove不能表达同一步内多套输入，发现歧义后不再接受后续刀的覆盖。
			UE_LOG(LogTemp, Verbose, TEXT("AttackWarp saved input ambiguous Owner=%s Evaluation=%llu PreviousRequest=%s CurrentRequest=%s PreviousYaw=%.3f CurrentYaw=%.3f; no yaw refill"),
				*GetNameSafe(CharacterOwner), Evaluation.Sequence, *Previous.RequestId.ToString(),
				*Evaluation.RequestId.ToString(), Previous.ActionYaw, Evaluation.ActionYaw);
			ActiveAttackWarpEvaluation.Reset();
			bAttackWarpEvaluationAmbiguous = true;
		}
		return;
	}
	ActiveAttackWarpEvaluation = Evaluation;
}

/** 核对同一步与完整执行身份后，仅返回可回填的方向。 */
bool UHodgeCharacterMovementComponent::ConsumeAttackWarpEvaluation(uint64 Sequence, int32 AvatarGeneration,
	const FHodgeCharacterRotationState& InitialState, const UAnimMontage* Montage, float& OutYaw)
{
	if (!CompletedAttackWarpEvaluation.IsSet() || CompletedAttackWarpEvaluation->Sequence != Sequence) { return false; }
	const FAttackWarpEvaluation Evaluation = CompletedAttackWarpEvaluation.GetValue();
	CompletedAttackWarpEvaluation.Reset();
	const auto* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	if (!CharacterOwner || CharacterOwner->bClientUpdating || !Rotation || Rotation->GetAvatarGeneration() != AvatarGeneration ||
		Evaluation.AvatarGeneration != AvatarGeneration || Evaluation.Avatar.Get() != CharacterOwner ||
		!Evaluation.Definition.IsValid() || !Evaluation.Montage.IsValid() || Evaluation.Montage.Get() != Montage ||
		InitialState.WarpDefinition.Get() != Evaluation.Definition.Get() || InitialState.ActionRequestId != Evaluation.RequestId ||
		InitialState.ActionAbilityHandle != Evaluation.AbilityHandle || InitialState.ActionPredictionKey != Evaluation.PredictionKey ||
		InitialState.Driver != EHodgeCharacterFacingDriver::ActionDirection || InitialState.bYawLocked ||
		InitialState.ExecutionMode != EHodgeFacingExecutionMode::MontageWarp)
	{ return false; }
	OutYaw = Evaluation.ActionYaw;
	return true;
}

/** 处理客户端侧的句柄移动响应消息，沿用当前身份与权限校验。 */
void UHodgeCharacterMovementComponent::ClientHandleMoveResponse(const FCharacterMoveResponseDataContainer& MoveResponse)
{
	const FSavedMovePtr ExpectedAck = MoveResponse.IsCorrection() ? FindHitReactionCorrectionMove(MoveResponse.ClientAdjustment.TimeStamp) : FSavedMovePtr();
	const FSavedMovePtr PreviousAck = ExpectedAck.IsValid() ? GetPredictionData_Client_Character()->LastAckedMove : FSavedMovePtr();
	const auto PreviousAnchor = HitReactionReplayAnchor;
	if (ExpectedAck.IsValid())
	{
		// 无Rotation可能回退LastAckedMove；显式原生Yaw待底层确认新ACK后再授权锚点。
		HitReactionReplayAnchor = CaptureHitReactionReplayAnchor(!MoveResponse.bHasRotation &&
			!MoveResponse.bRootMotionSourceCorrection && !MoveResponse.bRootMotionMontageCorrection);
	}
	TOptional<FVector> CorrectionVelocity;
	const auto* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	// 后续GAS操作使用的能力系统组件。
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
	TGuardValue<bool> FacingResponseGuard(bHitReactionFacingResponseScope, true);
	Super::ClientHandleMoveResponse(MoveResponse);
	// 同一帧后到的拒绝包不能擦除前一个已接受回包的待重放锚点。
	if (ExpectedAck.IsValid() && !IsHitReactionCorrectionAcknowledged(ExpectedAck, PreviousAck, MoveResponse.ClientAdjustment.TimeStamp))
	{ HitReactionReplayAnchor = PreviousAnchor; }
}

/** 处理客户端侧的Adjust位置消息，沿用当前身份与权限校验。 */
void UHodgeCharacterMovementComponent::ClientAdjustPosition_Implementation(float TimeStamp, FVector NewLoc, FVector NewVel,
	UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition,
	uint8 ServerMovementMode, TOptional<FRotator> OptionalRotation)
{
	const FSavedMovePtr ExpectedAck = FindHitReactionCorrectionMove(TimeStamp);
	const FSavedMovePtr PreviousAck = ExpectedAck.IsValid() ? GetPredictionData_Client_Character()->LastAckedMove : FSavedMovePtr();
	const auto PreviousAnchor = HitReactionReplayAnchor;
	const FHitReactionReplayAnchor NativeBefore = OptionalRotation.IsSet() ? CaptureHitReactionReplayAnchor(false, false) : FHitReactionReplayAnchor();
	const float BeforeYaw = CharacterOwner ? CharacterOwner->GetActorRotation().Yaw : 0.f;
	// 直接legacy回包每次重新记录，Packed/根运动的嵌套入口沿用外层本次响应标记。
	if (!bHitReactionFacingResponseScope && ExpectedAck.IsValid())
	{ HitReactionReplayAnchor = CaptureHitReactionReplayAnchor(!OptionalRotation.IsSet()); }
	// Packed 包已有完整权威速度；成功退出的 Dash 不走旧 RMS 入口的平面归零。
	Super::ClientAdjustPosition_Implementation(TimeStamp, NewLoc, DashCorrectionVelocity.Get(NewVel), NewBase,
		NewBaseBoneName, bHasBase, bBaseRelativePosition, ServerMovementMode, OptionalRotation);
	if (OptionalRotation.IsSet())
	{ CommitHitReactionNativeCorrection(NativeBefore, ExpectedAck, PreviousAck, TimeStamp, TEXT("Rotation"), BeforeYaw, OptionalRotation->Yaw); }
	if (ExpectedAck.IsValid() && !IsHitReactionCorrectionAcknowledged(ExpectedAck, PreviousAck, TimeStamp))
	{ HitReactionReplayAnchor = PreviousAnchor; }
}

/** UE5.5先经无OptionalRotation的位置纠正，再写入ServerRotation，因此由外层保留不可恢复标记。 */
void UHodgeCharacterMovementComponent::ClientAdjustRootMotionPosition_Implementation(float TimeStamp,
	float ServerMontageTrackPosition, FVector ServerLoc, FVector_NetQuantizeNormal ServerRotation, float ServerVelZ,
	UPrimitiveComponent* ServerBase, FName ServerBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode)
{
	const FSavedMovePtr ExpectedAck = FindHitReactionCorrectionMove(TimeStamp);
	const FSavedMovePtr PreviousAck = ExpectedAck.IsValid() ? GetPredictionData_Client_Character()->LastAckedMove : FSavedMovePtr();
	const auto PreviousAnchor = HitReactionReplayAnchor;
	if (!bHitReactionFacingResponseScope && ExpectedAck.IsValid())
	{ HitReactionReplayAnchor = CaptureHitReactionReplayAnchor(false); }
	TGuardValue<bool> FacingResponseGuard(bHitReactionFacingResponseScope, true);
	Super::ClientAdjustRootMotionPosition_Implementation(TimeStamp, ServerMontageTrackPosition, ServerLoc,
		ServerRotation, ServerVelZ, ServerBase, ServerBoneName, bHasBase, bBaseRelativePosition, ServerMovementMode);
	if (ExpectedAck.IsValid() && !IsHitReactionCorrectionAcknowledged(ExpectedAck, PreviousAck, TimeStamp))
	{ HitReactionReplayAnchor = PreviousAnchor; }
}

/** 保留原生RMS朝向与重放规则，嵌套位置纠正不能误标成无朝向回包。 */
void UHodgeCharacterMovementComponent::ClientAdjustRootMotionSourcePosition_Implementation(float TimeStamp,
	FRootMotionSourceGroup ServerRootMotion, bool bHasAnimRootMotion, float ServerMontageTrackPosition, FVector ServerLoc,
	FVector_NetQuantizeNormal ServerRotation, float ServerVelZ, UPrimitiveComponent* ServerBase, FName ServerBoneName,
	bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode)
{
	const FSavedMovePtr ExpectedAck = FindHitReactionCorrectionMove(TimeStamp);
	const FSavedMovePtr PreviousAck = ExpectedAck.IsValid() ? GetPredictionData_Client_Character()->LastAckedMove : FSavedMovePtr();
	const auto PreviousAnchor = HitReactionReplayAnchor;
	const FHitReactionReplayAnchor NativeBefore = CaptureHitReactionReplayAnchor(false, false);
	const float BeforeYaw = CharacterOwner ? CharacterOwner->GetActorRotation().Yaw : 0.f;
	if (!bHitReactionFacingResponseScope && ExpectedAck.IsValid())
	{ HitReactionReplayAnchor = CaptureHitReactionReplayAnchor(false); }
	TGuardValue<bool> FacingResponseGuard(bHitReactionFacingResponseScope, true);
	Super::ClientAdjustRootMotionSourcePosition_Implementation(TimeStamp, MoveTemp(ServerRootMotion), bHasAnimRootMotion,
		ServerMontageTrackPosition, ServerLoc, ServerRotation, ServerVelZ, ServerBase, ServerBoneName, bHasBase,
		bBaseRelativePosition, ServerMovementMode);
	if (!bHasAnimRootMotion)
	{
		CommitHitReactionNativeCorrection(NativeBefore, ExpectedAck, PreviousAck, TimeStamp, TEXT("RMS"), BeforeYaw, ServerRotation.Y * 180.f);
		RestoreRejectedHitReactionNativeCorrection(NativeBefore, ExpectedAck, PreviousAck, TimeStamp);
	}
	if (ExpectedAck.IsValid() && !IsHitReactionCorrectionAcknowledged(ExpectedAck, PreviousAck, TimeStamp))
	{ HitReactionReplayAnchor = PreviousAnchor; }
}

/** 按当前复制状态更新模拟代理的角色移动。 */
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

/** 初始化组件。 */
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
	// 朝向组件或旋转数据。
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

/** 通过当前旋转模式和约束更新角色实际朝向。 */
void UHodgeCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	// 朝向组件或旋转数据。
	UHodgeCharacterRotationComponent* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	if (Rotation && Rotation->IsFacingSystemReady()) { Rotation->ApplyResolvedMode(); }
	// 按预测状态或网络角色选择对应处理。
	if (Rotation && Rotation->IsYawLocked() && CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy) { return; }
	// Warp 占用持续到合法恢复阶段，不因窗口结束而重新执行物理插值。
	if (Rotation && Rotation->GetResolvedState().ExecutionMode==EHodgeFacingExecutionMode::MontageWarp) { return; }
	// 根运动蒙太奇提供唯一旋转增量，动作朝向请求不能逐帧把 Yaw 改回起点。
	const auto* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	// 当前操作使用的配置对象。
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
	// 角色旋转在 CMC 内执行，专服使用确认目标而不依赖渲染镜头。
	if (CharacterOwner && CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy && Rotation && Rotation->GetResolvedState().Driver == EHodgeCharacterFacingDriver::Controller)
	{
		if (const auto* Lock = CharacterOwner->FindComponentByClass<UHodgeLockOnComponent>())
		{
			FRotator Facing = CharacterOwner->GetActorRotation();
			if (Lock->ResolveControlRotation(Facing,DeltaTime)) { MoveUpdatedComponent(FVector::ZeroVector,FRotator(0.f,Facing.Yaw,0.f),false); Rotation->NotifyFacingApplied(Facing.Yaw); return; }
		}
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

/** 处理服务器侧的移动Perform移动消息，沿用当前身份与权限校验。 */
void UHodgeCharacterMovementComponent::ServerMove_PerformMovement(const FCharacterNetworkMoveData& MoveData)
{
	UHodgeLocomotionPolicyComponent* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	// 按预测状态或网络角色选择对应处理。
	if (Policy) { Policy->BeginServerMove(static_cast<const HodgeRotationPrediction::FMoveData&>(MoveData).SprintActivationKey); }
	// 朝向组件或旋转数据。
	UHodgeCharacterRotationComponent* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	const bool bServerOverride = CharacterOwner && CharacterOwner->HasAuthority() && Rotation;
	if (bServerOverride)
	{ Rotation->BeginServerMove(static_cast<const HodgeRotationPrediction::FMoveData&>(MoveData).FacingSequence,static_cast<const HodgeRotationPrediction::FMoveData&>(MoveData).MontageWarpKey); }
	Super::ServerMove_PerformMovement(MoveData);
	if (Policy) { Policy->EndServerMove(); }
	if (bServerOverride) { Rotation->EndServerMove(); }
}

/** 按当前组件约束执行实际移动与碰撞处理。 */
bool UHodgeCharacterMovementComponent::MoveUpdatedComponentImpl(const FVector& Delta, const FQuat& NewRotation,
	bool bSweep, FHitResult* OutHit, ETeleportType Teleport)
{
	FQuat AppliedRotation = NewRotation;
	// 朝向组件或旋转数据。
	const UHodgeCharacterRotationComponent* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner);
	// 按预测状态或网络角色选择对应处理。
	if (Rotation && Rotation->IsYawLocked() && CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy
		&& Teleport == ETeleportType::None && !bApplyingRotationCorrection && !bApplyingHitReactionFacing && UpdatedComponent
		&& !UpdatedComponent->IsSimulatingPhysics())
	{
		// 约束最终胶囊 Yaw，保留根运动位移和引擎允许的 Pitch/Roll。
		FRotator Constrained = NewRotation.Rotator();
		Constrained.Yaw = Rotation->GetLockedYaw();
		AppliedRotation = Constrained.Quaternion();
	}
	return Super::MoveUpdatedComponentImpl(Delta, AppliedRotation, bSweep, OutHit, Teleport);
}

/** 获取预测数据客户端。 */
FNetworkPredictionData_Client* UHodgeCharacterMovementComponent::GetPredictionData_Client() const
{
	// 按预测状态或网络角色选择对应处理。
	if (!ClientPredictionData)
	{
		UHodgeCharacterMovementComponent* MutableThis = const_cast<UHodgeCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new HodgeRotationPrediction::FClientPredictionData(*this);
	}
	return ClientPredictionData;
}

/** 处理客户端侧的更新位置After服务器更新消息，沿用当前身份与权限校验。 */
bool UHodgeCharacterMovementComponent::ClientUpdatePositionAfterServerUpdate()
{
	const auto* Prediction = GetPredictionData_Client_Character();
	const bool bWillReplay = CharacterOwner && Prediction && Prediction->bUpdatePosition && !Prediction->SavedMoves.IsEmpty();
	const FHitReactionReplayAnchor Anchor = HitReactionReplayAnchor.IsSet() ? HitReactionReplayAnchor.GetValue() :
		CaptureHitReactionReplayAnchor(true);
	HitReactionReplayAnchor.Reset();
	const auto* Reaction = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeHitReactionComponent>() : nullptr;
	const FHodgeHitReactionFacingState Facing = Reaction ? Reaction->GetFacingState() : FHodgeHitReactionFacingState();
	const bool bLogReplay = bWillReplay && (Facing.HasIdentity() || Anchor.Avatar.IsValid());
	if (bLogReplay)
	{
		UE_LOG(LogHodgeHitReactionFacing, Log, TEXT("Event=ReplayBegin Avatar=%s Role=%d Chain=%s Segment=%u Revision=%u ServerTime=%.9f WorldTime=%.9f DeltaTime=0.000000000 BeforeYaw=%.9f Moves=%d"),
			*GetPathNameSafe(CharacterOwner), int32(CharacterOwner->GetLocalRole()), *Facing.ChainId.ToString(), Facing.Segment,
			Facing.Revision, Reaction->GetServerTime(), GetWorld()->GetTimeSeconds(), CharacterOwner->GetActorRotation().Yaw,
			Prediction->SavedMoves.Num());
	}
	const bool bResult = Super::ClientUpdatePositionAfterServerUpdate();
	bHasReactionReplay = false;
	HitReactionReplay.Reset();
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (auto* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr) { Policy->ClearMoveReplayState(); }
	// 按预测状态或网络角色选择对应处理。
	if (UHodgeCharacterRotationComponent* Rotation = HodgeRotationPrediction::FindRotation(CharacterOwner))
	{
		Rotation->ClearMoveReplayState();
	}
	if (bResult) { RestoreHitReactionFacingAfterReplay(Anchor); }
	// 原生回放结束才消费当前段校正，不能把最终Yaw套进某个旧历史移动。
	ApplyHitReactionFacingCorrection(bResult);
	if (bLogReplay)
	{
		UE_LOG(LogHodgeHitReactionFacing, Log, TEXT("Event=ReplayEnd Avatar=%s Role=%d Chain=%s Segment=%u Revision=%u ServerTime=%.9f WorldTime=%.9f DeltaTime=0.000000000 AfterYaw=%.9f Replayed=%d"),
			*GetPathNameSafe(CharacterOwner), int32(CharacterOwner->GetLocalRole()), *Facing.ChainId.ToString(), Facing.Segment,
			Facing.Revision, Reaction->GetServerTime(), GetWorld()->GetTimeSeconds(), CharacterOwner->GetActorRotation().Yaw, bResult);
	}
	return bResult;
}

/** 按现有网络策略平滑本次位置修正。 */
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

/** 获取最大加速度。 */
float UHodgeCharacterMovementComponent::GetMaxAcceleration() const
{
	const auto* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	const auto State = Policy ? Policy->GetResolvedPolicy() : FHodgeLocomotionState();
	return State.bSprinting ? State.Acceleration : Super::GetMaxAcceleration();
}
/** 获取最大BrakingDeceleration。 */
float UHodgeCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	const auto* Policy = CharacterOwner ? CharacterOwner->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	const auto State = Policy ? Policy->GetResolvedPolicy() : FHodgeLocomotionState();
	return State.bSprinting ? State.Braking : Super::GetMaxBrakingDeceleration();
}
/** 按当前朝向与动作策略约束输入加速度。 */
FVector UHodgeCharacterMovementComponent::ConstrainInputAcceleration(const FVector& InputAcceleration) const
{
	if (IsHitReactionControlled()) { return FVector::ZeroVector; }
	// 后续GAS操作使用的能力系统组件。
	const auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (ASC && (ASC->HasMatchingGameplayTag(HodgeMovementTags::DashPreparing) || ASC->HasMatchingGameplayTag(HodgeMovementTags::Dashing))) { return FVector::ZeroVector; }
	// 统一战斗组件。
	const auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(GetOwner());
	if (ASC && ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Attack) && (!Combat || !Combat->IsMoveCancelPredicted()))
	{ return FVector::ZeroVector; }
	return Super::ConstrainInputAcceleration(InputAcceleration);
}

/** 判断命中受击Controlled。 */
bool UHodgeCharacterMovementComponent::IsHitReactionControlled() const
{
	if (CharacterOwner && CharacterOwner->bClientUpdating && bHasReactionReplay) { return bReactionReplayControlled; }
	// 后续GAS操作使用的能力系统组件。
	const auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	return ASC && ASC->HasMatchingGameplayTag(HodgeHitReactionTags::Controlled);
}

/** 应用Requested移动。 */
bool UHodgeCharacterMovementComponent::ApplyRequestedMove(float DeltaTime, float MaxAccel, float MaxSpeed, float Friction,
	float BrakingDeceleration, FVector& OutAcceleration, float& OutRequestedSpeed)
{
	if (IsHitReactionControlled()) { OutAcceleration = FVector::ZeroVector; OutRequestedSpeed = 0.f; return false; }
	return Super::ApplyRequestedMove(DeltaTime, MaxAccel, MaxSpeed, Friction, BrakingDeceleration, OutAcceleration, OutRequestedSpeed);
}
