/** @file HodgeGameplayAbility_Definition.cpp
 * @brief 实现玩法能力定义相关的类型、接口与现有处理流程。
 */
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Combat/HodgeAttackRule.h"
#include "UObject/StrongObjectPtr.h"
#include "Component/HodgeLockOnComponent.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Component/HodgeHeroComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Component/HodgeWingComponent.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "Equipment/HodgeWeaponInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/HodgeHeroComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeAttackMotionWarpingComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_Definition)

/** 构造玩法能力定义并初始化其默认状态。 */
UHodgeGameplayAbility_Definition::UHodgeGameplayAbility_Definition(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationGroup = EHodgeAbilityActivationGroup::Exclusive_Replaceable;
	bServerRespectsRemoteAbilityCancellation = true;
	bRequiresInitializedAttributes = true;
}

/** 获取定义。 */
const UHodgeAbilityDefinition* UHodgeGameplayAbility_Definition::GetDefinition() const
{
	// 后续GAS操作使用的能力系统组件。
	const auto* ASC = Cast<UHodgeAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	return ASC ? ASC->FindAbilityDefinition(GetCurrentAbilitySpecHandle()) : nullptr;
}

/** 判断连段Coordinated。 */
bool UHodgeGameplayAbility_Definition::IsComboCoordinated() const
{
	// 当前操作使用的定义配置。
	const auto* Definition = GetDefinition();
	return Definition && Definition->ExecutionRoute == EHodgeAbilityExecutionRoute::ComboCoordinated;
}

/** 激活Confirmed定义。 */
void UHodgeGameplayAbility_Definition::ActivateConfirmedDefinition(FGameplayAbilitySpecHandle Handle,
                                                                   const FGameplayAbilityActorInfo* Info,
                                                                   const FPredictionKey& Key,
                                                                   const FGameplayEventData& Payload)
{
	// 根据当前生命周期状态决定是否继续执行。
	if (!Key.IsServerInitiatedKey() || IsActive())
	{
		return;
	}
	FGameplayAbilityActivationInfo ActivationInfo(Info->OwnerActor.Get());
	ActivationInfo.SetActivationConfirmed();
	ActivationInfo.ServerSetActivationPredictionKey(Key);
	CallActivateAbility(Handle, Info, ActivationInfo, nullptr, &Payload);
}

/** 检查技能激活所需的上下文、标签与现有GAS限制。 */
bool UHodgeGameplayAbility_Definition::CanActivateAbility(FGameplayAbilitySpecHandle Handle,
                                                          const FGameplayAbilityActorInfo* Info,
                                                          const FGameplayTagContainer* SourceTags,
                                                          const FGameplayTagContainer* TargetTags,
                                                          FGameplayTagContainer* RelevantTags) const
{
	// 后续GAS操作使用的能力系统组件。
	const auto* ASC = Info ? Cast<UHodgeAbilitySystemComponent>(Info->AbilitySystemComponent.Get()) : nullptr;
	// 统一战斗组件。
	const auto* Combat = Info && Info->AvatarActor.IsValid()
		                     ? UHodgeCombatComponentBase::FindCombatComponent(Info->AvatarActor.Get())
		                     : nullptr;
	const auto* Definition = ASC ? ASC->FindAbilityDefinition(Handle) : nullptr;
	return ASC && Definition && Combat && Combat->IsCombatReadyForActions() && Combat->GetOwner() == Info->AvatarActor.
		Get()
		&& !ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death)
		&& ASC->GetOwnedGameplayTags().HasAll(Definition->RequiredSourceTags)
		&& !ASC->GetOwnedGameplayTags().HasAny(Definition->BlockedSourceTags)
		&& (Definition->ExecutionRoute == EHodgeAbilityExecutionRoute::Standalone || Combat->IsAuthorized(Handle))
		&& Super::CanActivateAbility(Handle, Info, SourceTags, TargetTags, RelevantTags);
}

/** 启动本次技能，按现有配置提交消耗并建立执行资源。 */
void UHodgeGameplayAbility_Definition::ActivateAbility(FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* Info,
                                                       FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* Payload)
{
	bEnding = false;
	bLifecycleEventSent = false;
	bPredictedMoveCancel = false;
	PresentationWeapon.Reset();
	NotifyResources.Reset();
	StateCounts.Reset();
	NextOccurrence = 0;
	MontageInstanceId = INDEX_NONE;
	ExecutionId = FGuid::NewGuid();
	ExecutionLockTarget.Reset();
	if (const auto* Lock = Info && Info->AvatarActor.IsValid()
		                       ? Info->AvatarActor->FindComponentByClass<UHodgeLockOnComponent>()
		                       : nullptr)
	{
		ExecutionLockTarget = Lock->IsConfirmedTargetValid() ? Lock->GetConfirmedTarget() : nullptr;
	}
	// 当前操作使用的定义配置。
	const auto* Definition = GetDefinition();
	TArray<FText> Errors;
	if (!Definition || !Definition->ValidateDefinition(Errors))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid execution definition [%s] for ability [%s]."), *GetNameSafe(Definition),
		       *GetPathName());
		for (const FText& Error : Errors) { UE_LOG(LogTemp, Error, TEXT("%s"), *Error.ToString()); }
		FinishExecution(true, true);
		return;
	}
	// 连段继续使用协调器授权；独立技能按 GA 配置响应远端结束。
	// 客户端必须接受服务器结束；服务器上的连段仍由协调器授权结束。
	CurrentActivationInfo.bCanBeEndedByOtherInstance = !Info->IsNetAuthority() ||
	(Definition->ExecutionRoute == EHodgeAbilityExecutionRoute::Standalone &&
		bServerRespectsRemoteAbilityCancellation);
	if (!CommitAbility(Handle, Info, ActivationInfo))
	{
		FinishExecution(true, true);
		return;
	}
	// 后续GAS操作使用的能力系统组件。
	auto* ASC = CastChecked<UHodgeAbilitySystemComponent>(Info->AbilitySystemComponent.Get());
	ExecutionBodyTag = Definition->ExecutionBodyTag;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (ExecutionBodyTag.IsValid()) { ASC->AddLooseGameplayTag(ExecutionBodyTag); }
	// 根据当前生命周期状态决定是否继续执行。
	if (!IsActive() || bEnding) { return; }
	InitializeAttackFacing(Payload);
	bActivationReturned = true;
}

/** 目标就绪后按原路径启动；不额外等待 Warp 的 Applied。 */
void UHodgeGameplayAbility_Definition::StartExecutionMontage()
{
	if (!IsActive() || bEnding || MontageInstanceId != INDEX_NONE || !CurrentActorInfo) { return; }
	const auto* Definition = GetDefinition();
	if (!Definition)
	{
		FinishExecution(true, true);
		return;
	}
	const auto& Config = Definition->ExecutionConfig;
	auto* ASC = CastChecked<UHodgeAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	ExecutionCombat = UHodgeCombatComponentBase::FindCombatComponent(CurrentActorInfo->AvatarActor.Get());
	if (ExecutionCombat.IsValid()) { PoseLease = ExecutionCombat->AcquirePoseLease(); }
	if (CurrentActorInfo->IsNetAuthority() && !Definition->CreateAttackRules(this, AttackRules))
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot create execution attack rules for %s"), *GetNameSafe(Definition));
		FinishExecution(true, true);
		return;
	}
	OnExecutionReady();
	if (!IsActive() || bEnding) { return; }
	if (ASC->PlayMontage(this, CurrentActivationInfo, Config.Montage, Config.PlayRate) <= 0.f)
	{
		FinishExecution(true, true);
		return;
	}
	UAnimInstance* Anim = CurrentActorInfo->GetAnimInstance();
	FAnimMontageInstance* Instance = Anim ? Anim->GetActiveInstanceForMontage(Config.Montage) : nullptr;
	if (!Instance)
	{
		FinishExecution(true, true);
		return;
	}
	MontageInstanceId = Instance->GetInstanceID();
	Instance->OnMontageBlendingOutStarted.BindUObject(this, &ThisClass::OnMontageBlendingOut, ExecutionId,
	                                                  MontageInstanceId);
	Instance->OnMontageEnded.BindUObject(this, &ThisClass::OnMontageEnded, ExecutionId, MontageInstanceId);
	if (IsComboCoordinated() && ExecutionCombat.IsValid()) { ExecutionCombat->ExecutionStarted(this); }

	if (bActivationReturned && IsComboCoordinated() && ExecutionCombat.IsValid())
	{
		ExecutionCombat->CompleteServerActivation();
	}
}

/** 锁定目标优先，移动意图其次，零输入使用镜头水平朝向。 */
bool UHodgeGameplayAbility_Definition::BuildAttackFacingData(const FGameplayAbilityActorInfo* Info,
                                                             FHodgeAttackFacingTargetData& Out)
{
	const auto* Pawn = Info && Info->AvatarActor.IsValid() ? Cast<APawn>(Info->AvatarActor.Get()) : nullptr;
	if (!Pawn) { return false; }
	Out.SourceAvatar = const_cast<APawn*>(Pawn);
	Out.StartYaw = FRotator::NormalizeAxis(Pawn->GetActorRotation().Yaw);
	Out.DesiredYaw = Out.StartYaw;
	Out.Source = EHodgeAttackFacingSource::Actor;
	if (const auto* Lock = Pawn->FindComponentByClass<UHodgeLockOnComponent>(); Lock && Lock->IsConfirmedTargetValid())
	{
		FVector Point;
		if (Lock->GetPresentationLockPoint(Point))
		{
			const FVector Direction = (Point - Pawn->GetActorLocation()).GetSafeNormal2D();
			if (!Direction.IsNearlyZero())
			{
				Out.LockedTarget = Lock->GetConfirmedTarget();
				Out.DesiredYaw = Direction.Rotation().Yaw;
				Out.Source = EHodgeAttackFacingSource::LockedTarget;
				return true;
			}
		}
	}
	if (const auto* Hero = Pawn->FindComponentByClass<UHodgeHeroComponent>())
	{
		const auto Intent = Hero->GetMoveIntentSnapshot();
		if (Intent.InputMagnitude > KINDA_SMALL_NUMBER && !Intent.DesiredDirectionWorld.IsNearlyZero())
		{
			Out.DesiredYaw = Intent.DesiredDirectionWorld.Rotation().Yaw;
			Out.Source = EHodgeAttackFacingSource::MoveInput;
			return true;
		}
	}
	// 服务器发起的连段没有本地输入事件，使用已经验证的移动加速度。
	if (Info->IsNetAuthority() && !Info->IsLocallyControlled())
	{
		const auto* Character = Cast<ACharacter>(Pawn);
		const FVector Acceleration = Character
			                             ? Character->GetCharacterMovement()->GetCurrentAcceleration()
			                             : FVector::ZeroVector;
		if (!Acceleration.IsNearlyZero())
		{
			Out.DesiredYaw = Acceleration.Rotation().Yaw;
			Out.Source = EHodgeAttackFacingSource::MoveInput;
			return true;
		}
	}
	if (Pawn->Controller)
	{
		Out.DesiredYaw = FRotator::NormalizeAxis(Pawn->GetControlRotation().Yaw);
		Out.Source = EHodgeAttackFacingSource::Camera;
	}
	return FMath::IsFinite(Out.StartYaw) && FMath::IsFinite(Out.DesiredYaw);
}

void UHodgeGameplayAbility_Definition::InitializeAttackFacing(const FGameplayEventData* Payload)
{
	bFacingDataWaiting = false;
	bFacingDataProcessed = false;
	bActivationReturned = false;
	bAttackFacingRetired = false;
	AttackFacingHandle = {};
	const auto* Data = GetDefinition();
	const auto Key = CurrentActivationInfo.GetActivationPredictionKey();
	if (!Data->FacingWarp.bEnabled)
	{
		StartExecutionMontage();
		return;
	}
	if (Key.IsServerInitiatedKey() && Payload && Payload->TargetData.Num() == 1)
	{
		OnAttackFacingData(Payload->TargetData, {}, ExecutionId);
		return;
	}
	if (CurrentActorInfo->IsLocallyControlled() || Key.IsServerInitiatedKey() || !Cast<APawn>(
		GetAvatarActorFromActorInfo())->IsPlayerControlled())
	{
		FHodgeAttackFacingTargetData Target;
		if (!BuildAttackFacingData(CurrentActorInfo, Target))
		{
			FinishExecution(true, true);
			return;
		}
		FGameplayAbilityTargetDataHandle Handle(new FHodgeAttackFacingTargetData(Target));
		if (!CurrentActorInfo->IsNetAuthority())
		{
			auto* ASC = GetAbilitySystemComponentFromActorInfo();
			FScopedPredictionWindow Scope(ASC, true);
			ASC->ServerSetReplicatedTargetData(CurrentSpecHandle, Key, Handle, {}, ASC->ScopedPredictionKey);
		}
		OnAttackFacingData(Handle, {}, ExecutionId);
		return;
	}
	auto* ASC = GetAbilitySystemComponentFromActorInfo();
	bFacingDataWaiting = true;
	// 等待方向期间只占用当前姿态，避免服务器被普通移动转向提前改变起手 Yaw。
	if (auto* Rotation = GetAvatarActorFromActorInfo()->FindComponentByClass<UHodgeCharacterRotationComponent>())
	{
		AttackFacingHandle = Rotation->RequestMontageWarpFacing(GetAvatarActorFromActorInfo()->GetActorRotation().Yaw,
		                                                        this, ExecutionId);
	}
	if (!AttackFacingHandle.IsValid())
	{
		FinishExecution(true, true);
		return;
	}
	FacingDataDelegate = ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, Key).AddUObject(
		this, &ThisClass::OnAttackFacingData, ExecutionId);
	GetWorld()->GetTimerManager().SetTimer(FacingDataTimer,
	                                       FTimerDelegate::CreateUObject(
		                                       this, &ThisClass::OnAttackFacingTimeout, ExecutionId),
	                                       Data->FacingWarp.TargetDataTimeout, false);
	ASC->CallReplicatedTargetDataDelegatesIfSet(CurrentSpecHandle, Key);
}

void UHodgeGameplayAbility_Definition::OnAttackFacingData(const FGameplayAbilityTargetDataHandle& Handle, FGameplayTag,
                                                          FGuid ForExecution)
{
	if (!IsActive() || bEnding || ForExecution != ExecutionId) { return; }
	auto* ASC = Cast<UHodgeAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
	if (bFacingDataProcessed) { return; }
	bFacingDataProcessed = true;
	bFacingDataWaiting = false;
	GetWorld()->GetTimerManager().ClearTimer(FacingDataTimer);
	ASC->RetireAttackFacingData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
	if (Handle.Num() != 1 || !Handle.Get(0) || Handle.Get(0)->GetScriptStruct() !=
		FHodgeAttackFacingTargetData::StaticStruct() ||
		!ApplyAttackFacing(*static_cast<const FHodgeAttackFacingTargetData*>(Handle.Get(0))))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack facing rejected for %s"), *GetName());
		FinishExecution(true, true);
		return;
	}
	StartExecutionMontage();
}

bool UHodgeGameplayAbility_Definition::ApplyAttackFacing(const FHodgeAttackFacingTargetData& Target)
{
	const auto* Data = GetDefinition();
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Data || !Avatar || Target.SourceAvatar.Get() != Avatar || !FMath::IsFinite(Target.StartYaw) || !
		FMath::IsFinite(Target.DesiredYaw) ||
		FMath::Abs(Target.StartYaw) > 180.f || FMath::Abs(Target.DesiredYaw) > 180.f || static_cast<uint8>(Target.
			Source) > static_cast<uint8>(EHodgeAttackFacingSource::LockedTarget)) { return false; }
	if (CurrentActorInfo->IsNetAuthority() && FMath::Abs(
			FMath::FindDeltaAngleDegrees(Avatar->GetActorRotation().Yaw, Target.StartYaw)) > Data->FacingWarp.
		StartYawTolerance) { return false; }
	if (Target.Source == EHodgeAttackFacingSource::LockedTarget)
	{
		const auto* Lock = Avatar->FindComponentByClass<UHodgeLockOnComponent>();
		FVector Point;
		if (!Lock || !Lock->IsConfirmedTargetValid() || Lock->GetConfirmedTarget() != Target.LockedTarget.Get() || !Lock
			->GetPresentationLockPoint(Point)) { return false; }
		const FVector Direction = (Point - Avatar->GetActorLocation()).GetSafeNormal2D();
		if (Direction.IsNearlyZero() || FMath::Abs(
				FMath::FindDeltaAngleDegrees(Direction.Rotation().Yaw, Target.DesiredYaw)) > Data->FacingWarp.
			TargetYawTolerance) { return false; }
	}
	else if (Target.LockedTarget.IsValid()) { return false; }
	if (!HodgeAttackFacing::ResolveYaw(Target.StartYaw, Target.DesiredYaw, Data->FacingWarp.MaxCorrectionAngle,
	                                   FacingTargetYaw)) { return false; }
	auto* Rotation = Avatar->FindComponentByClass<UHodgeCharacterRotationComponent>();
	if (!Rotation || !Avatar->FindComponentByClass<UHodgeAttackMotionWarpingComponent>()) { return false; }
	ReleaseAttackFacing();
	AttackFacingHandle = Rotation->RequestMontageWarpFacing(FacingTargetYaw, this, ExecutionId);
	bAttackFacingRetired = false;
	return Rotation->IsFacingRequestSelected(AttackFacingHandle);
}

void UHodgeGameplayAbility_Definition::OnAttackFacingTimeout(FGuid ForExecution)
{
	if (IsActive() && ForExecution == ExecutionId && bFacingDataWaiting)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack facing data timed out for %s"), *GetName());
		FinishExecution(true, true);
	}
}

void UHodgeGameplayAbility_Definition::UpdateAttackFacingLifecycle()
{
	if (!IsActive() || bEnding || bPredictedMoveCancel || !AttackFacingHandle.IsValid() || !CurrentActorInfo)
	{
		return;
	}
	const auto* Data = GetDefinition();
	auto* Anim = CurrentActorInfo->GetAnimInstance();
	const auto* Instance = Anim ? Anim->GetMontageInstanceForID(MontageInstanceId) : nullptr;
	if (!Data || !Instance || Instance->Montage != Data->ExecutionConfig.Montage) { return; }
	for (const auto& Event : Data->ExecutionConfig.Montage->Notifies)
	{
		const auto* Tag = Cast<UHodgeAnimNotifyState_GameplayTag>(Event.NotifyStateClass);
		if (Tag && Tag->StateTag == HodgeGameplayTags::Status_Attack_Recovery && Instance->GetPosition() >= Event.
			GetTriggerTime())
		{
			ReleaseAttackFacing();
			return;
		}
	}
}

void UHodgeGameplayAbility_Definition::ReleaseAttackFacing()
{
	if (!AttackFacingHandle.IsValid()) { return; }
	if (auto* Avatar = GetAvatarActorFromActorInfo())
	{
		if (auto* Warp = Avatar->FindComponentByClass<UHodgeAttackMotionWarpingComponent>())
		{
			Warp->ClearAttackWarpForRequest(AttackFacingHandle.Id);
		}
		if (auto* Rotation = Avatar->FindComponentByClass<UHodgeCharacterRotationComponent>())
		{
			Rotation->ReleaseFacingRequest(AttackFacingHandle);
		}
	}
	AttackFacingHandle = {};
	bAttackFacingRetired = true;
}

/** 在仍活动时按指定结束方式结束当前执行。 */
void UHodgeGameplayAbility_Definition::FinishExecution(bool bCancelled, bool bReplicate)
{
	// 根据当前生命周期状态决定是否继续执行。
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicate, bCancelled);
	}
}

/** 开始Predicted移动取消。 */
bool UHodgeGameplayAbility_Definition::BeginPredictedMoveCancel()
{
	// 按服务器权限区分本次处理分支。
	if (!IsActive() || bEnding || bPredictedMoveCancel || !CanBeCanceled() || !CurrentActorInfo ||
		CurrentActorInfo->IsNetAuthority() || !CurrentActorInfo->IsLocallyControlled()) { return false; }
	// 后续GAS操作使用的能力系统组件。
	auto* ASC = Cast<UHodgeAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!ASC || ASC->GetAnimatingAbility() != this || !GetDefinition()) { return false; }
	// 先预测姿势退出，保留执行身份等待服务器接受或恢复。
	bPredictedMoveCancel = true;
	if (auto* Avatar = GetAvatarActorFromActorInfo())
	{
		if (auto* Rotation = Avatar->FindComponentByClass<UHodgeCharacterRotationComponent>())
		{
			Rotation->SetFacingRequestSuspended(AttackFacingHandle, true);
		}
		if (auto* Warp = Avatar->FindComponentByClass<UHodgeAttackMotionWarpingComponent>())
		{
			Warp->ClearAttackWarpForRequest(AttackFacingHandle.Id);
		}
	}
	ASC->StopDefinitionMontage(this, false);
	return IsActive() && bPredictedMoveCancel;
}

/** 解析Predicted移动取消。 */
void UHodgeGameplayAbility_Definition::ResolvePredictedMoveCancel(bool bEnd, float ServerPosition, float ServerTime,
                                                                  float ServerPlayRate)
{
	// 根据当前生命周期状态决定是否继续执行。
	if (!bPredictedMoveCancel || !IsActive() || !CurrentActorInfo) { return; }
	if (bEnd)
	{
		FinishExecution(true, false);
		return;
	}
	// 当前操作使用的定义配置。
	const auto* Definition = GetDefinition();
	// 后续GAS操作使用的能力系统组件。
	auto* ASC = Cast<UHodgeAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	UAnimInstance* Anim = CurrentActorInfo->GetAnimInstance();
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Definition || !ASC || !Anim || ASC->GetAnimatingAbility() != this ||
		!FMath::IsFinite(ServerPosition) || !FMath::IsFinite(ServerTime) || !FMath::IsFinite(ServerPlayRate) ||
		ServerPlayRate < 0.f)
	{
		FinishExecution(true, false);
		return;
	}
	if (FAnimMontageInstance* Previous = Anim->GetMontageInstanceForID(MontageInstanceId))
	{
		Previous->OnMontageBlendingOutStarted.Unbind();
		Previous->OnMontageEnded.Unbind();
	}
	const auto* State = GetWorld()->GetGameState();
	const float Now = State ? State->GetServerWorldTimeSeconds() : GetWorld()->GetTimeSeconds();
	const float Position = FMath::Clamp(ServerPosition + FMath::Max(0.f, Now - ServerTime) * ServerPlayRate,
	                                    0.f, FMath::Max(
		                                    0.f, Definition->ExecutionConfig.Montage->GetPlayLength() - .01f));
	if (ASC->PlayMontage(this, CurrentActivationInfo, Definition->ExecutionConfig.Montage, ServerPlayRate, NAME_None,
	                     Position) <= 0.f)
	{
		FinishExecution(true, false);
		return;
	}
	FAnimMontageInstance* Instance = Anim->GetActiveInstanceForMontage(Definition->ExecutionConfig.Montage);
	if (!Instance)
	{
		FinishExecution(true, false);
		return;
	}
	MontageInstanceId = Instance->GetInstanceID();
	Instance->OnMontageBlendingOutStarted.BindUObject(this, &ThisClass::OnMontageBlendingOut, ExecutionId,
	                                                  MontageInstanceId);
	Instance->OnMontageEnded.BindUObject(this, &ThisClass::OnMontageEnded, ExecutionId, MontageInstanceId);
	bPredictedMoveCancel = false;
	float Recovery = MAX_flt;
	for (const auto& Notify : Definition->ExecutionConfig.Montage->Notifies)
	{
		const auto* Tag = Cast<UHodgeAnimNotifyState_GameplayTag>(Notify.NotifyStateClass);
		if (Tag && Tag->StateTag == HodgeGameplayTags::Status_Attack_Recovery)
		{
			Recovery = FMath::Min(Recovery, Notify.GetTriggerTime());
		}
	}
	if (Position >= Recovery) { ReleaseAttackFacing(); }
	else if (auto* Rotation = GetAvatarActorFromActorInfo()->FindComponentByClass<UHodgeCharacterRotationComponent>())
	{
		Rotation->SetFacingRequestSuspended(AttackFacingHandle, false);
	}
}

/** 结束本次技能并释放其拥有的资源，再交还GAS生命周期。 */
void UHodgeGameplayAbility_Definition::EndAbility(FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* Info,
                                                  FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate,
                                                  bool bCancelled)
{
	// 根据当前生命周期状态决定是否继续执行。
	if (bEnding || !IsActive())
	{
		return;
	}
	const FGuid RequestedExecution = ExecutionId;
	if (!bLifecycleEventSent)
	{
		bLifecycleEventSent = true;
		SendExecutionEvent(bCancelled
			                   ? HodgeGameplayTags::GameplayEvent_Attack_Interrupted
			                   : HodgeGameplayTags::GameplayEvent_Attack_Completed);
		if (ExecutionId != RequestedExecution || !IsActive()) { return; }
	}
	const bool bCompletePreparing = bActivationReturned && IsComboCoordinated() && MontageInstanceId == INDEX_NONE;
	TGuardValue<bool> Guard(bEnding, true);
	bPredictedMoveCancel = false;
	bFacingDataWaiting = false;
	if (GetWorld()) { GetWorld()->GetTimerManager().ClearTimer(FacingDataTimer); }
	if (Info && Info->AbilitySystemComponent.IsValid())
	{
		Info->AbilitySystemComponent->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey()).
		      Remove(FacingDataDelegate);
		if (GetDefinition() && GetDefinition()->FacingWarp.bEnabled)
		{
			Info->AbilitySystemComponent->ConsumeClientReplicatedTargetData(
				Handle, ActivationInfo.GetActivationPredictionKey());
			Cast<UHodgeAbilitySystemComponent>(Info->AbilitySystemComponent.Get())->RetireAttackFacingData(
				Handle, ActivationInfo.GetActivationPredictionKey());
		}
	}
	FacingDataDelegate.Reset();
	ReleaseAttackFacing();
	// 先失效旧身份，清理期间的回调不能消费上一段结果。
	const FGuid EndingExecutionId = ExecutionId;
	const int32 EndingMontageId = MontageInstanceId;
	// 先释放本次电影镜头，不能等待已经失效的NotifyEnd。
	if (auto* Hero = GetHeroComponentFromActorInfo()) { Hero->EndCinematicCameraForExecution(EndingExecutionId); }
	ExecutionId.Invalidate();
	AttackRules.Reset();
	OnExecutionEnding(EndingExecutionId);
	if (const AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		if (auto* Wing = Avatar->FindComponentByClass<UHodgeWingComponent>())
		{
			Wing->EndWingForExecution(EndingExecutionId);
		}
	}
	MontageInstanceId = INDEX_NONE;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (UAnimInstance* Anim = Info ? Info->GetAnimInstance() : nullptr)
	{
		if (FAnimMontageInstance* Instance = Anim->GetMontageInstanceForID(EndingMontageId))
		{
			if (Instance->OnMontageBlendingOutStarted.IsBoundToObject(this))
			{
				Instance->OnMontageBlendingOutStarted.Unbind();
			}
			if (Instance->OnMontageEnded.IsBoundToObject(this)) { Instance->OnMontageEnded.Unbind(); }
		}
	}
	if (PresentationWeapon.IsValid()) { PresentationWeapon->ReleaseHandUsesForExecution(EndingExecutionId); }
	NotifyResources.Reset();
	PresentationWeapon.Reset();
	const TMap<FGameplayTag, int32> EndingCounts = MoveTemp(StateCounts);
	StateCounts.Reset();
	if (auto* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		for (const auto& Pair : EndingCounts) { ASC->RemoveLooseGameplayTag(Pair.Key, Pair.Value); }
		if (ExecutionBodyTag.IsValid()) { ASC->RemoveLooseGameplayTag(ExecutionBodyTag); }
	}
	ExecutionBodyTag = FGameplayTag();
	if (ExecutionCombat.IsValid()) { ExecutionCombat->ReleasePoseLease(PoseLease); }
	PoseLease.Invalidate();
	ExecutionCombat.Reset();
	if (auto* ASC = Cast<UHodgeAbilitySystemComponent>(Info->AbilitySystemComponent.Get()))
	{
		ASC->StopDefinitionMontage(this, !bCancelled);
		ASC->ClearAnimatingAbility(this);
	}
	if (auto* Combat = IsComboCoordinated()
		                   ? UHodgeCombatComponentBase::FindCombatComponent(Info->AvatarActor.Get())
		                   : nullptr) { Combat->ExecutionEnded(this); }
	// GAS 的 EndAbility 只复制正常结束，取消需要明确的取消消息。
	if (bCancelled && bReplicate && Info && Info->AbilitySystemComponent.IsValid())
	{
		Info->AbilitySystemComponent->ReplicateEndOrCancelAbility(Handle, ActivationInfo, this, true);
		bReplicate = false;
	}
	Super::EndAbility(Handle, Info, ActivationInfo, bReplicate, bCancelled);
	if (Info && bCompletePreparing)
	{
		if (auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(Info->AvatarActor.Get()))
		{
			Combat->CompleteServerActivation();
		}
	}
}

/** 获取执行Windows。 */
FGameplayTagContainer UHodgeGameplayAbility_Definition::GetExecutionWindows() const
{
	FGameplayTagContainer Tags;
	for (const auto& Pair : StateCounts) { if (Pair.Value > 0) { Tags.AddTag(Pair.Key); } }
	return Tags;
}

bool UHodgeGameplayAbility_Definition::GetCinematicSource(UAnimMontage*& OutMontage, int32& OutInstanceId,
                                                          float& OutPosition) const
{
	OutMontage = nullptr;
	OutInstanceId = INDEX_NONE;
	OutPosition = 0.f;
	const auto* Data = GetDefinition();
	auto* ASC = GetAbilitySystemComponentFromActorInfo();
	auto* Anim = CurrentActorInfo ? CurrentActorInfo->GetAnimInstance() : nullptr;
	const auto* Instance = Anim ? Anim->GetMontageInstanceForID(MontageInstanceId) : nullptr;
	if (!IsActive() || bEnding || bPredictedMoveCancel || !ExecutionId.IsValid() || !Data || !Instance ||
		!ASC || ASC->GetAnimatingAbility() != this || ASC->GetAvatarActor() != GetAvatarActorFromActorInfo() || Instance
		->Montage != Data->ExecutionConfig.Montage) { return false; }
	OutMontage = Data->ExecutionConfig.Montage;
	OutInstanceId = MontageInstanceId;
	OutPosition = Instance->GetPosition();
	return FMath::IsFinite(OutPosition);
}

/** 校验通知是否属于当前Avatar、蒙太奇实例与执行身份。 */
bool UHodgeGameplayAbility_Definition::AcceptsNotify(const FBranchingPointNotifyPayload& Payload) const
{
	return IsActive() && !bEnding && ExecutionId.IsValid() && CurrentActorInfo &&
		Payload.SkelMeshComponent == CurrentActorInfo->SkeletalMeshComponent.Get() &&
		Payload.SkelMeshComponent && Payload.SkelMeshComponent->GetOwner() == GetAvatarActorFromActorInfo() &&
		Payload.MontageInstanceID == MontageInstanceId && Payload.SequenceAsset == GetDefinition()->ExecutionConfig.
		Montage;
}

/** 为本次执行分配通知次数身份。 */
int32 UHodgeGameplayAbility_Definition::AllocateNotifyOccurrence()
{
	return NextOccurrence < MAX_int32 ? ++NextOccurrence : INDEX_NONE;
}

/** 为被当前执行接受的通知建立资源记录。 */
int32 UHodgeGameplayAbility_Definition::BeginNotifyResource(const FBranchingPointNotifyPayload& Payload)
{
	if (!AcceptsNotify(Payload) || !Payload.NotifyEvent || NotifyResources.Contains(
		Payload.NotifyEvent->NotifyStateClass.Get())) { return INDEX_NONE; }
	FNotifyResource Resource;
	Resource.OccurrenceId = AllocateNotifyOccurrence();
	NotifyResources.Add(Payload.NotifyEvent->NotifyStateClass.Get(), Resource);
	return Resource.OccurrenceId;
}

/** 按通知记录释放其标签、手持或检测资源。 */
void UHodgeGameplayAbility_Definition::EndNotifyResource(const FBranchingPointNotifyPayload& Payload)
{
	if (!AcceptsNotify(Payload)) { return; }
	FNotifyResource Resource;
	if (!NotifyResources.RemoveAndCopyValue(Payload.NotifyEvent->NotifyStateClass.Get(), Resource)) { return; }
	const FGuid Execution = ExecutionId;
	OnNotifyResourceEnded(Resource.OccurrenceId);
	if (Execution != ExecutionId) { return; }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Resource.WeaponHandle.IsValid() && PresentationWeapon.IsValid())
	{
		PresentationWeapon->ReleaseHandUse(Resource.WeaponHandle);
	}
	if (Resource.Tag.IsValid())
	{
		int32* Count = StateCounts.Find(Resource.Tag);
		if (Count && --*Count == 0) { StateCounts.Remove(Resource.Tag); }
		if (auto* ASC = GetAbilitySystemComponentFromActorInfo()) { ASC->RemoveLooseGameplayTag(Resource.Tag); }
		if (Execution == ExecutionId) { NotifyWindowsChanged(); }
	}
}

/** 为本次通知申请执行期间的状态Tag。 */
void UHodgeGameplayAbility_Definition::AcquireNotifyTag(int32 OccurrenceId, FGameplayTag Tag)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Tag.IsValid()) { return; }
	for (auto& Pair : NotifyResources)
	{
		if (Pair.Value.OccurrenceId != OccurrenceId || Pair.Value.Tag.IsValid()) { continue; }
		Pair.Value.Tag = Tag;
		++StateCounts.FindOrAdd(Tag);
		const FGuid Execution = ExecutionId;
		if (auto* ASC = GetAbilitySystemComponentFromActorInfo()) { ASC->AddLooseGameplayTag(Tag); }
		if (Execution == ExecutionId)
		{
			if (Tag == HodgeGameplayTags::Status_Attack_Recovery) { ReleaseAttackFacing(); }
			NotifyWindowsChanged();
		}
		return;
	}
}

/** 为本次通知申请武器手持资源。 */
void UHodgeGameplayAbility_Definition::AcquireNotifyWeapon(int32 OccurrenceId, EHodgeWeaponHandEndBehavior EndBehavior)
{
	for (auto& Pair : NotifyResources)
	{
		if (Pair.Value.OccurrenceId != OccurrenceId || Pair.Value.WeaponHandle.IsValid()) { continue; }
		if (!PresentationWeapon.IsValid())
		{
			PresentationWeapon = UHodgeWeaponInstance::ResolvePresentationWeapon(
				Cast<APawn>(GetAvatarActorFromActorInfo()), GetCurrentSourceObject());
		}
		if (PresentationWeapon.IsValid())
		{
			const auto Key = CurrentActivationInfo.GetActivationPredictionKey();
			Pair.Value.WeaponHandle = PresentationWeapon->AcquireHandUse(
				ExecutionId, OccurrenceId, Key.IsServerInitiatedKey() ? -Key.Current : Key.Current, EndBehavior);
		}
		return;
	}
}

/** 通知Windows变化。 */
void UHodgeGameplayAbility_Definition::NotifyWindowsChanged()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (auto* Combat = IsComboCoordinated()
		                   ? UHodgeCombatComponentBase::FindCombatComponent(GetAvatarActorFromActorInfo())
		                   : nullptr) { Combat->WindowsChanged(this); }
}

/** 按当前执行身份发送动作语义事件。 */
void UHodgeGameplayAbility_Definition::SendExecutionEvent(FGameplayTag Tag)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Tag.IsValid() || !IsActive() || bEnding) { return; }
	const FGuid Execution = ExecutionId;
	FGameplayEventData Data;
	OnExecutionEvent(Tag);
	if (Execution != ExecutionId || !IsActive() || bEnding) { return; }
	DispatchAttackRuleEvent(Tag);
	if (Execution != ExecutionId || !IsActive() || bEnding) { return; }
	Data.EventTag = Tag;
	Data.Instigator = GetAvatarActorFromActorInfo();
	if (auto* ASC = GetAbilitySystemComponentFromActorInfo()) { ASC->HandleGameplayEvent(Tag, &Data); }
	if (Execution != ExecutionId) { return; }
	if (auto* Combat = IsComboCoordinated()
		                   ? UHodgeCombatComponentBase::FindCombatComponent(GetAvatarActorFromActorInfo())
		                   : nullptr) { Combat->ExecutionEvent(this, Tag); }
}

/** 处理蒙太奇Blending输出。 */
void UHodgeGameplayAbility_Definition::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted, FGuid Execution,
                                                            int32 InstanceId)
{
	// 根据当前生命周期状态决定是否继续执行。
	if (ExecutionId != Execution || MontageInstanceId != InstanceId || bEnding || bPredictedMoveCancel) { return; }
	bLifecycleEventSent = true;
	SendExecutionEvent(bInterrupted
		                   ? HodgeGameplayTags::GameplayEvent_Attack_Interrupted
		                   : HodgeGameplayTags::GameplayEvent_Attack_Completed);
	if (ExecutionId == Execution) { FinishExecution(bInterrupted, true); }
}

/** 处理蒙太奇结束。 */
void UHodgeGameplayAbility_Definition::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, FGuid Execution,
                                                      int32 InstanceId)
{
	// 根据当前生命周期状态决定是否继续执行。
	if (ExecutionId == Execution && MontageInstanceId == InstanceId && !bEnding)
	{
		OnMontageBlendingOut(Montage, bInterrupted, Execution, InstanceId);
	}
}

/** 校验执行Configuration。 */
void UHodgeGameplayAbility_Definition::ValidateExecutionConfiguration(const UHodgeAbilityDefinition& Definition,
                                                                      TArray<FText>& Errors) const
{
	if (!Definition.ExecutionConfig.Montage) { return; }
	if (Definition.ExecutionConfig.Montage->Notifies.ContainsByPredicate([](const FAnimNotifyEvent& Event)
	{
		return (Event.Notify && Event.Notify->IsA<UHodgeAnimNotify_Hit>()) || (Event.NotifyStateClass && Event.
			NotifyStateClass->IsA<UHodgeAnimNotifyState_HitCheck>());
	}))
	{
		Errors.Add(FText::FromString(TEXT(
			"Hit notifications require HodgeGameplayAbility_Melee or a subclass implementing their contract.")));
	}
}

/** 同步回调可结束执行并触发 GC，快照强持有规则且每次重新检查身份。 */
void UHodgeGameplayAbility_Definition::PublishAttackSettlement(const FHodgeAttackSettlement& Settlement)
{
	TStrongObjectPtr<UHodgeGameplayAbility_Definition> KeepSource(this);
	TArray<TStrongObjectPtr<UHodgeAttackRule>> Snapshot;
	for (UHodgeAttackRule* Rule : AttackRules) { Snapshot.Emplace(Rule); }
	for (const auto& Rule : Snapshot)
	{
		if (!IsActive() || bEnding || GetExecutionId() != Settlement.ExecutionId) { break; }
		if (Rule.Get()) { Rule->OnSettlement(Settlement); }
	}
}

/** 施放事件走同一批执行规则，结束或换执行后不分发旧事件。 */
void UHodgeGameplayAbility_Definition::DispatchAttackRuleEvent(FGameplayTag Tag)
{
	TStrongObjectPtr<UHodgeGameplayAbility_Definition> KeepSource(this);
	const FGuid Id = GetExecutionId();
	TArray<TStrongObjectPtr<UHodgeAttackRule>> Snapshot;
	for (UHodgeAttackRule* Rule : AttackRules) { Snapshot.Emplace(Rule); }
	for (const auto& Rule : Snapshot)
	{
		if (!IsActive() || bEnding || GetExecutionId() != Id) { break; }
		if (Rule.Get()) { Rule->OnExecutionEvent(Tag); }
	}
}
