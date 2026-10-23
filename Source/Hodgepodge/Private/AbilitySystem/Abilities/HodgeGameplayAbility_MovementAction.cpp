/** @file HodgeGameplayAbility_MovementAction.cpp
 * @brief 实现玩法能力移动动作相关的类型、接口与现有处理流程。
 */
#include "AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgeHeroComponent.h"
#include "Component/HodgeLocomotionPolicyComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Component/HodgeHitReactionComponent.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "Data/HodgeSprintAbilityProfile.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_MovementAction)

/** 按既有协议读写网络载荷及对象引用。 */
bool FHodgeMovementActionTargetData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bSuccess)
{
	Ar << Input << ControlYaw << ActorYawAtStart << SessionId;
	bSuccess = !Ar.IsError();
	return bSuccess;
}

/** 构造玩法能力移动动作并初始化其默认状态。 */
UHodgeGameplayAbility_MovementAction::UHodgeGameplayAbility_MovementAction()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationGroup = EHodgeAbilityActivationGroup::Independent;
	bServerRespectsRemoteAbilityCancellation = true;
	bRequiresInitializedAttributes = true;
}

/** 解析配置。 */
const UHodgeSprintAbilityProfile* UHodgeGameplayAbility_MovementAction::ResolveProfile(
	const FGameplayAbilityActorInfo* Info) const
{
	if (Profile) { return Profile; }
	const auto* Component = Info && Info->AvatarActor.IsValid()
		                        ? Info->AvatarActor->FindComponentByClass<UHodgeLocomotionPolicyComponent>()
		                        : nullptr;
	return Component ? Component->GetProfile() : nullptr;
}

/** 检查技能激活所需的上下文、标签与现有GAS限制。 */
bool UHodgeGameplayAbility_MovementAction::CanActivateAbility(FGameplayAbilitySpecHandle Handle,
                                                              const FGameplayAbilityActorInfo* Info,
                                                              const FGameplayTagContainer* SourceTags,
                                                              const FGameplayTagContainer* TargetTags,
                                                              FGameplayTagContainer* RelevantTags) const
{
	const auto* Config = ResolveProfile(Info);
	TArray<FText> Errors;
	const auto* Character = Info ? Cast<ACharacter>(Info->AvatarActor.Get()) : nullptr;
	// 后续GAS操作使用的能力系统组件。
	const auto* ASC = Info ? Cast<UHodgeAbilitySystemComponent>(Info->AbilitySystemComponent.Get()) : nullptr;
	return Character && !Character->bIsCrouched && Character->GetCharacterMovement()->IsMovingOnGround() && Config &&
		Config->Validate(Errors) &&
		ASC && !ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) &&
		!ASC->HasActiveAbilityTag(HodgeGameplayTags::Ability_Type_StatusChange_Death) &&
		!ASC->HasMatchingGameplayTag(HodgeMovementTags::Sprinting) &&
		!ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped) &&
		Super::CanActivateAbility(Handle, Info, SourceTags, TargetTags, RelevantTags);
}

/** 启动本次技能，按现有配置提交消耗并建立执行资源。 */
void UHodgeGameplayAbility_MovementAction::ActivateAbility(FGameplayAbilitySpecHandle Handle,
                                                           const FGameplayAbilityActorInfo* Info,
                                                           FGameplayAbilityActivationInfo ActivationInfo,
                                                           const FGameplayEventData* Event)
{
	Super::ActivateAbility(Handle, Info, ActivationInfo, Event);
	ExecutionId = FGuid::NewGuid();
	bCommitted = bReceivedData = false;
	bRecoveryCostCommitted = false;
	const auto* Reaction = Info->AvatarActor->FindComponentByClass<UHodgeHitReactionComponent>();
	RecoverySequence = Reaction && Reaction->IsControlled() ? Reaction->GetReactionState().Sequence : 0;
	SessionId = 0;
	ExecutionProfile = ResolveProfile(Info);
	PreparingAt = GetWorld()->GetTimeSeconds();
	Policy = Info->AvatarActor->FindComponentByClass<UHodgeLocomotionPolicyComponent>();
	Rotation = Info->AvatarActor->FindComponentByClass<UHodgeCharacterRotationComponent>();
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!ExecutionProfile || !Policy.IsValid() || !Rotation.IsValid() || !Rotation->IsFacingSystemReady())
	{
		Finish();
		return;
	}
	if (GetAssetTags().HasTag(HodgeGameplayTags::Ability_Type_Action_Dash))
	{
		SetMovementTag(HodgeMovementTags::DashPreparing, true);
	}
	// 后续GAS操作使用的能力系统组件。
	auto* ASC = Info->AbilitySystemComponent.Get();
	const FPredictionKey Key = ActivationInfo.GetActivationPredictionKey();
	const FGameplayTag Constraints[] = {
		HodgeGameplayTags::Status_Death, HodgeHitReactionTags::Controlled, TAG_Gameplay_MovementStopped
	};
	// 按当前对象生命周期建立状态监听。
	for (int32 Index = 0; Index < 3; ++Index)
	{
		ConstraintHandles[Index] = ASC->RegisterGameplayTagEvent(Constraints[Index]).AddUObject(
			this, &ThisClass::OnConstraintChanged);
	}
	DataHandle = ASC->AbilityTargetDataSetDelegate(Handle, Key).AddUObject(this, &ThisClass::OnTargetData);
	GetWorld()->GetTimerManager().SetTimer(TickHandle, this, &ThisClass::TickMovement, .01f, true);
	if (Info->IsLocallyControlled())
	{
		const auto* Hero = GetHeroComponentFromActorInfo();
		if (!Hero)
		{
			Finish();
			return;
		}
		auto* Payload = new FHodgeMovementActionTargetData();
		const auto Snapshot = Hero->GetMoveIntentSnapshot();
		Payload->Input = Snapshot.RawInput2D;
		Payload->ControlYaw = Snapshot.ControllerYawAtSample;
		Payload->ActorYawAtStart = Info->AvatarActor->GetActorRotation().Yaw;
		Payload->SessionId = Hero->GetSprintInputSession().SessionId;
		FGameplayAbilityTargetDataHandle Data(Payload);
		FScopedPredictionWindow Prediction(ASC, true);
		if (!Info->IsNetAuthority())
		{
			ASC->ServerSetReplicatedTargetData(Handle, Key, Data, {}, ASC->ScopedPredictionKey);
		}
		OnTargetData(Data, {});
	}
	else { ASC->CallReplicatedTargetDataDelegatesIfSet(Handle, Key); }
}

/** 处理目标数据。 */
void UHodgeGameplayAbility_MovementAction::OnTargetData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag Tag)
{
	// 根据当前生命周期状态决定是否继续执行。
	if (!IsActive()) { return; }
	if (bReceivedData)
	{
		const FGameplayAbilityTargetDataHandle Copy = Data;
		HandleMovementFollowUp(Copy);
		GetAbilitySystemComponentFromActorInfo()->ConsumeClientReplicatedTargetData(
			CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
		return;
	}
	if (Data.Num() != 1 || !Data.Get(0) ||
		Data.Get(0)->GetScriptStruct() != FHodgeMovementActionTargetData::StaticStruct())
	{
		if (IsActive()) { Finish(); }
		return;
	}
	const auto* Payload = static_cast<const FHodgeMovementActionTargetData*>(Data.Get(0));
	const FVector2D Input = Payload->Input;
	const float Yaw = Payload->ControlYaw;
	SessionId = Payload->SessionId;
	ActorYawAtStart = Payload->ActorYawAtStart;
	GetAbilitySystemComponentFromActorInfo()->ConsumeClientReplicatedTargetData(
		CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
	if (SessionId <= 0 || Input.ContainsNaN() || Input.SizeSquared() > 2.01f || !FMath::IsFinite(Yaw) ||
		!FMath::IsFinite(ActorYawAtStart) || FMath::Abs(
			FMath::FindDeltaAngleDegrees(GetAvatarActorFromActorInfo()->GetActorRotation().Yaw, ActorYawAtStart)) >
		90.f)
	{
		Finish();
		return;
	}
	bReceivedData = true;
	const auto Intent = HodgeFacing::MakeMoveIntent(Input, Yaw, GetWorld()->GetTimeSeconds(),
	                                                Rotation->GetAvatarGeneration());
	BeginMovement(Intent);
}

/** 判断移动有效。 */
bool UHodgeGameplayAbility_MovementAction::IsMovementValid() const
{
	const auto* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	// 后续GAS操作使用的能力系统组件。
	const auto* ASC = GetAbilitySystemComponentFromActorInfo();
	const auto* Reaction = Character ? Character->FindComponentByClass<UHodgeHitReactionComponent>() : nullptr;
	const bool bAllowedRecovery = RecoverySequence != 0 && Reaction && Reaction->GetReactionState().Sequence == RecoverySequence &&
		CanRecoverFromHit(CurrentSpecHandle, CurrentActorInfo);
	return Character && !Character->bIsCrouched && ASC && ASC->GetAvatarActor() == Character && Policy.IsValid() &&
		Rotation.IsValid() &&
		Rotation->IsFacingSystemReady() && IsExecutionMovementModeValid(Character->GetCharacterMovement()) &&
		!ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) && (!ASC->HasMatchingGameplayTag(
			HodgeHitReactionTags::Controlled) || bAllowedRecovery) &&
		!ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped);
}

/** 默认维持地面执行，Dash 只在已经提交后扩展 Falling。 */
bool UHodgeGameplayAbility_MovementAction::IsExecutionMovementModeValid(
	const UCharacterMovementComponent* Movement) const
{
	return Movement && Movement->IsMovingOnGround();
}

/** 更新移动。 */
void UHodgeGameplayAbility_MovementAction::TickMovement()
{
	// 根据当前生命周期状态决定是否继续执行。
	if (!IsActive()) { return; }
	FScopedPredictionWindow Prediction(GetAbilitySystemComponentFromActorInfo(), IsLocallyControlled());
	if (!IsMovementValid() || (!bReceivedData && GetWorld()->GetTimeSeconds() - PreparingAt > 2.))
	{
		Finish();
		return;
	}
	if (bReceivedData) { UpdateMovement(GetWorld()->GetDeltaSeconds()); }
}

/** 设置移动标签。 */
void UHodgeGameplayAbility_MovementAction::SetMovementTag(FGameplayTag Tag, bool bAdd)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (auto* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (bAdd)
		{
			if (MovementTag.IsValid()) { ASC->RemoveLooseGameplayTag(MovementTag); }
			MovementTag = Tag;
			ASC->AddLooseGameplayTag(Tag);
		}
		else if (MovementTag.IsValid())
		{
			ASC->RemoveLooseGameplayTag(MovementTag);
			MovementTag = {};
		}
	}
}

/** 按当前移动动作配置开始GameplayCue表现。 */
void UHodgeGameplayAbility_MovementAction::StartCue(FGameplayTag Tag)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (ActiveCue.IsValid()) { return; }
	ActiveCue = Tag;
	FGameplayCueParameters Parameters;
	Parameters.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	GetAbilitySystemComponentFromActorInfo()->AddGameplayCue(Tag, Parameters);
}

/** 结束当前动作拥有的GameplayCue表现。 */
void UHodgeGameplayAbility_MovementAction::StopCue()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (ActiveCue.IsValid())
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveGameplayCue(ActiveCue);
		ActiveCue = {};
	}
}

/** 结束当前任务阶段并完成其收尾。 */
void UHodgeGameplayAbility_MovementAction::Finish(bool bCancelled)
{
	if (IsActive()) { EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bCancelled); }
}

/** 处理约束变化。 */
void UHodgeGameplayAbility_MovementAction::OnConstraintChanged(FGameplayTag Tag, int32 Count)
{
	if (Count > 0) { Finish(); }
}

/** 结束本次技能并释放其拥有的资源，再交还GAS生命周期。 */
void UHodgeGameplayAbility_MovementAction::EndAbility(FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* Info,
                                                      FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate,
                                                      bool bCancelled)
{
	if (GetWorld()) { GetWorld()->GetTimerManager().ClearTimer(TickHandle); }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Info && Info->AbilitySystemComponent.IsValid())
	{
		const FGameplayTag Constraints[] = {
			HodgeGameplayTags::Status_Death, HodgeHitReactionTags::Controlled, TAG_Gameplay_MovementStopped
		};
		for (int32 Index = 0; Index < 3; ++Index)
		{
			Info->AbilitySystemComponent->RegisterGameplayTagEvent(Constraints[Index]).Remove(ConstraintHandles[Index]);
		}
		Info->AbilitySystemComponent->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey()).
		      Remove(DataHandle);
		Info->AbilitySystemComponent->ConsumeClientReplicatedTargetData(
			Handle, ActivationInfo.GetActivationPredictionKey());
		SetMovementTag({}, false);
		StopCue();
	}
	bCommitted = false;
	Super::EndAbility(Handle, Info, ActivationInfo, bReplicate, bCancelled);
}
