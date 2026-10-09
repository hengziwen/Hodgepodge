#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "Equipment/HodgeWeaponInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/GameStateBase.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_Definition)

UHodgeGameplayAbility_Definition::UHodgeGameplayAbility_Definition(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationGroup = EHodgeAbilityActivationGroup::Exclusive_Replaceable;
	bServerRespectsRemoteAbilityCancellation = true;
	bRequiresInitializedAttributes = true;
}

const UHodgeAbilityDefinition* UHodgeGameplayAbility_Definition::GetDefinition() const
{
	const auto* ASC = Cast<UHodgeAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	return ASC ? ASC->FindAbilityDefinition(GetCurrentAbilitySpecHandle()) : nullptr;
}

bool UHodgeGameplayAbility_Definition::IsComboCoordinated() const
{
	const auto* Definition = GetDefinition();
	return Definition && Definition->ExecutionRoute == EHodgeAbilityExecutionRoute::ComboCoordinated;
}

void UHodgeGameplayAbility_Definition::ActivateConfirmedDefinition(FGameplayAbilitySpecHandle Handle,
                                                                   const FGameplayAbilityActorInfo* Info,
                                                                   const FPredictionKey& Key,
                                                                   const FGameplayEventData& Payload)
{
	if (!Key.IsServerInitiatedKey() || IsActive())
	{
		return;
	}
	FGameplayAbilityActivationInfo ActivationInfo(Info->OwnerActor.Get());
	ActivationInfo.SetActivationConfirmed();
	ActivationInfo.ServerSetActivationPredictionKey(Key);
	CallActivateAbility(Handle, Info, ActivationInfo, nullptr, &Payload);
}

bool UHodgeGameplayAbility_Definition::CanActivateAbility(FGameplayAbilitySpecHandle Handle,
                                                          const FGameplayAbilityActorInfo* Info,
                                                          const FGameplayTagContainer* SourceTags,
                                                          const FGameplayTagContainer* TargetTags,
                                                          FGameplayTagContainer* RelevantTags) const
{
	const auto* ASC = Info ? Cast<UHodgeAbilitySystemComponent>(Info->AbilitySystemComponent.Get()) : nullptr;
	const auto* Combat = Info && Info->AvatarActor.IsValid()
		                    ? UHodgeCombatComponentBase::FindCombatComponent(Info->AvatarActor.Get())
		                    : nullptr;
	const auto* Definition = ASC ? ASC->FindAbilityDefinition(Handle) : nullptr;
	return ASC && Definition && Combat && Combat->CanExecuteAbilities() && Combat->GetOwner() == Info->AvatarActor.Get()
		&& !ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death)
		&& (Definition->ExecutionRoute == EHodgeAbilityExecutionRoute::Standalone || Combat->IsAuthorized(Handle))
		&& Super::CanActivateAbility(Handle, Info, SourceTags, TargetTags, RelevantTags);
}

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
	const auto* Definition = GetDefinition();
	TArray<FText> Errors;
	if (!Definition || !Definition->ValidateDefinition(Errors))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid execution definition [%s] for ability [%s]."), *GetNameSafe(Definition), *GetPathName());
		for (const FText& Error : Errors) { UE_LOG(LogTemp, Error, TEXT("%s"), *Error.ToString()); }
		FinishExecution(true, true);
		return;
	}
	// 连段继续使用协调器授权；独立技能按 GA 配置响应远端结束。
	CurrentActivationInfo.bCanBeEndedByOtherInstance = Definition->ExecutionRoute == EHodgeAbilityExecutionRoute::Standalone && bServerRespectsRemoteAbilityCancellation;
	if (!CommitAbility(Handle, Info, ActivationInfo))
	{
		FinishExecution(true, true);
		return;
	}
	auto* ASC = CastChecked<UHodgeAbilitySystemComponent>(Info->AbilitySystemComponent.Get());
	ExecutionBodyTag = Definition->ExecutionBodyTag;
	if (ExecutionBodyTag.IsValid()) { ASC->AddLooseGameplayTag(ExecutionBodyTag); }
	if (!IsActive() || bEnding) { return; }
	const auto& Config = Definition->ExecutionConfig;
	ExecutionCombat = UHodgeCombatComponentBase::FindCombatComponent(Info->AvatarActor.Get());
	if (ExecutionCombat.IsValid()) { PoseLease = ExecutionCombat->AcquirePoseLease(); }
	OnExecutionReady();
	if (!IsActive() || bEnding) { return; }
	if (ASC->PlayMontage(this, ActivationInfo, Config.Montage, Config.PlayRate) <= 0.f)
	{
		FinishExecution(true, true);
		return;
	}
	UAnimInstance* Anim = Info->GetAnimInstance();
	FAnimMontageInstance* Instance = Anim ? Anim->GetActiveInstanceForMontage(Config.Montage) : nullptr;
	if (!Instance)
	{
		FinishExecution(true, true);
		return;
	}
	MontageInstanceId = Instance->GetInstanceID();
	Instance->OnMontageBlendingOutStarted.BindUObject(this, &ThisClass::OnMontageBlendingOut, ExecutionId, MontageInstanceId);
	Instance->OnMontageEnded.BindUObject(this, &ThisClass::OnMontageEnded, ExecutionId, MontageInstanceId);
	if (IsComboCoordinated() && ExecutionCombat.IsValid()) { ExecutionCombat->ExecutionStarted(this); }

}

void UHodgeGameplayAbility_Definition::FinishExecution(bool bCancelled, bool bReplicate)
{
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicate, bCancelled);
	}
}

bool UHodgeGameplayAbility_Definition::BeginPredictedMoveCancel()
{
	if (!IsActive() || bEnding || bPredictedMoveCancel || !CanBeCanceled() || !CurrentActorInfo ||
		CurrentActorInfo->IsNetAuthority() || !CurrentActorInfo->IsLocallyControlled()) { return false; }
	auto* ASC = Cast<UHodgeAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (!ASC || ASC->GetAnimatingAbility() != this || !GetDefinition()) { return false; }
	// 先预测姿势退出，保留执行身份等待服务器接受或恢复。
	bPredictedMoveCancel = true;
	ASC->StopDefinitionMontage(this, false);
	return IsActive() && bPredictedMoveCancel;
}

void UHodgeGameplayAbility_Definition::ResolvePredictedMoveCancel(bool bEnd, float ServerPosition, float ServerTime, float ServerPlayRate)
{
	if (!bPredictedMoveCancel || !IsActive() || !CurrentActorInfo) { return; }
	if (bEnd) { FinishExecution(true, false); return; }
	const auto* Definition = GetDefinition();
	auto* ASC = Cast<UHodgeAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	UAnimInstance* Anim = CurrentActorInfo->GetAnimInstance();
	if (!Definition || !ASC || !Anim || ASC->GetAnimatingAbility() != this ||
		!FMath::IsFinite(ServerPosition) || !FMath::IsFinite(ServerTime) || !FMath::IsFinite(ServerPlayRate) || ServerPlayRate < 0.f)
	{ FinishExecution(true, false); return; }
	if (FAnimMontageInstance* Previous = Anim->GetMontageInstanceForID(MontageInstanceId))
	{
		Previous->OnMontageBlendingOutStarted.Unbind();
		Previous->OnMontageEnded.Unbind();
	}
	const auto* State = GetWorld()->GetGameState();
	const float Now = State ? State->GetServerWorldTimeSeconds() : GetWorld()->GetTimeSeconds();
	const float Position = FMath::Clamp(ServerPosition + FMath::Max(0.f, Now - ServerTime) * ServerPlayRate,
		0.f, FMath::Max(0.f, Definition->ExecutionConfig.Montage->GetPlayLength() - .01f));
	if (ASC->PlayMontage(this, CurrentActivationInfo, Definition->ExecutionConfig.Montage, ServerPlayRate, NAME_None, Position) <= 0.f)
	{ FinishExecution(true, false); return; }
	FAnimMontageInstance* Instance = Anim->GetActiveInstanceForMontage(Definition->ExecutionConfig.Montage);
	if (!Instance) { FinishExecution(true, false); return; }
	MontageInstanceId = Instance->GetInstanceID();
	Instance->OnMontageBlendingOutStarted.BindUObject(this, &ThisClass::OnMontageBlendingOut, ExecutionId, MontageInstanceId);
	Instance->OnMontageEnded.BindUObject(this, &ThisClass::OnMontageEnded, ExecutionId, MontageInstanceId);
	bPredictedMoveCancel = false;
}

void UHodgeGameplayAbility_Definition::EndAbility(FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* Info,
                                                  FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate,
                                                  bool bCancelled)
{
	if (bEnding || !IsActive())
	{
		return;
	}
	const FGuid RequestedExecution = ExecutionId;
	if (!bLifecycleEventSent)
	{
		bLifecycleEventSent = true;
		SendExecutionEvent(bCancelled ? HodgeGameplayTags::GameplayEvent_Attack_Interrupted : HodgeGameplayTags::GameplayEvent_Attack_Completed);
		if (ExecutionId != RequestedExecution || !IsActive()) { return; }
	}
	TGuardValue<bool> Guard(bEnding, true);
	bPredictedMoveCancel = false;
	// 先失效旧身份，清理期间的回调不能消费上一段结果。
	const FGuid EndingExecutionId = ExecutionId;
	const int32 EndingMontageId = MontageInstanceId;
	ExecutionId.Invalidate();
	OnExecutionEnding(EndingExecutionId);
	MontageInstanceId = INDEX_NONE;
	if (UAnimInstance* Anim = Info ? Info->GetAnimInstance() : nullptr)
	{
		if (FAnimMontageInstance* Instance = Anim->GetMontageInstanceForID(EndingMontageId))
		{
			if (Instance->OnMontageBlendingOutStarted.IsBoundToObject(this)) { Instance->OnMontageBlendingOutStarted.Unbind(); }
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
	if (auto* Combat = IsComboCoordinated() ? UHodgeCombatComponentBase::FindCombatComponent(Info->AvatarActor.Get()) : nullptr) { Combat->ExecutionEnded(this); }
	// GAS 的 EndAbility 只复制正常结束，取消需要明确的取消消息。
	if (bCancelled && bReplicate && Info && Info->AbilitySystemComponent.IsValid())
	{
		Info->AbilitySystemComponent->ReplicateEndOrCancelAbility(Handle, ActivationInfo, this, true);
		bReplicate = false;
	}
	Super::EndAbility(Handle, Info, ActivationInfo, bReplicate, bCancelled);
}

FGameplayTagContainer UHodgeGameplayAbility_Definition::GetExecutionWindows() const
{
	FGameplayTagContainer Tags;
	for (const auto& Pair : StateCounts) { if (Pair.Value > 0) { Tags.AddTag(Pair.Key); } }
	return Tags;
}

bool UHodgeGameplayAbility_Definition::AcceptsNotify(const FBranchingPointNotifyPayload& Payload) const
{
	return IsActive() && !bEnding && ExecutionId.IsValid() && CurrentActorInfo &&
		Payload.SkelMeshComponent == CurrentActorInfo->SkeletalMeshComponent.Get() &&
		Payload.SkelMeshComponent && Payload.SkelMeshComponent->GetOwner() == GetAvatarActorFromActorInfo() &&
		Payload.MontageInstanceID == MontageInstanceId && Payload.SequenceAsset == GetDefinition()->ExecutionConfig.Montage;
}

int32 UHodgeGameplayAbility_Definition::AllocateNotifyOccurrence()
{
	return NextOccurrence < MAX_int32 ? ++NextOccurrence : INDEX_NONE;
}

int32 UHodgeGameplayAbility_Definition::BeginNotifyResource(const FBranchingPointNotifyPayload& Payload)
{
	if (!AcceptsNotify(Payload) || !Payload.NotifyEvent || NotifyResources.Contains(Payload.NotifyEvent->NotifyStateClass.Get())) { return INDEX_NONE; }
	FNotifyResource Resource;
	Resource.OccurrenceId = AllocateNotifyOccurrence();
	NotifyResources.Add(Payload.NotifyEvent->NotifyStateClass.Get(), Resource);
	return Resource.OccurrenceId;
}

void UHodgeGameplayAbility_Definition::EndNotifyResource(const FBranchingPointNotifyPayload& Payload)
{
	if (!AcceptsNotify(Payload)) { return; }
	FNotifyResource Resource;
	if (!NotifyResources.RemoveAndCopyValue(Payload.NotifyEvent->NotifyStateClass.Get(), Resource)) { return; }
	const FGuid Execution = ExecutionId;
	OnNotifyResourceEnded(Resource.OccurrenceId);
	if (Execution != ExecutionId) { return; }
	if (Resource.WeaponHandle.IsValid() && PresentationWeapon.IsValid()) { PresentationWeapon->ReleaseHandUse(Resource.WeaponHandle); }
	if (Resource.Tag.IsValid())
	{
		int32* Count = StateCounts.Find(Resource.Tag);
		if (Count && --*Count == 0) { StateCounts.Remove(Resource.Tag); }
		if (auto* ASC = GetAbilitySystemComponentFromActorInfo()) { ASC->RemoveLooseGameplayTag(Resource.Tag); }
		if (Execution == ExecutionId) { NotifyWindowsChanged(); }
	}
}

void UHodgeGameplayAbility_Definition::AcquireNotifyTag(int32 OccurrenceId, FGameplayTag Tag)
{
	if (!Tag.IsValid()) { return; }
	for (auto& Pair : NotifyResources)
	{
		if (Pair.Value.OccurrenceId != OccurrenceId || Pair.Value.Tag.IsValid()) { continue; }
		Pair.Value.Tag = Tag;
		++StateCounts.FindOrAdd(Tag);
		const FGuid Execution = ExecutionId;
		if (auto* ASC = GetAbilitySystemComponentFromActorInfo()) { ASC->AddLooseGameplayTag(Tag); }
		if (Execution == ExecutionId) { NotifyWindowsChanged(); }
		return;
	}
}

void UHodgeGameplayAbility_Definition::AcquireNotifyWeapon(int32 OccurrenceId)
{
	for (auto& Pair : NotifyResources)
	{
		if (Pair.Value.OccurrenceId != OccurrenceId || Pair.Value.WeaponHandle.IsValid()) { continue; }
		if (!PresentationWeapon.IsValid()) { PresentationWeapon = UHodgeWeaponInstance::ResolvePresentationWeapon(Cast<APawn>(GetAvatarActorFromActorInfo()), GetCurrentSourceObject()); }
		if (PresentationWeapon.IsValid())
		{
			const auto Key = CurrentActivationInfo.GetActivationPredictionKey();
			Pair.Value.WeaponHandle = PresentationWeapon->AcquireHandUse(ExecutionId, OccurrenceId, Key.IsServerInitiatedKey() ? -Key.Current : Key.Current);
		}
		return;
	}
}

void UHodgeGameplayAbility_Definition::NotifyWindowsChanged()
{
	if (auto* Combat = IsComboCoordinated() ? UHodgeCombatComponentBase::FindCombatComponent(GetAvatarActorFromActorInfo()) : nullptr) { Combat->WindowsChanged(this); }
}

void UHodgeGameplayAbility_Definition::SendExecutionEvent(FGameplayTag Tag)
{
	if (!Tag.IsValid() || !IsActive() || bEnding) { return; }
	const FGuid Execution = ExecutionId;
	FGameplayEventData Data;
	Data.EventTag = Tag;
	Data.Instigator = GetAvatarActorFromActorInfo();
	if (auto* ASC = GetAbilitySystemComponentFromActorInfo()) { ASC->HandleGameplayEvent(Tag, &Data); }
	if (Execution != ExecutionId) { return; }
	if (auto* Combat = IsComboCoordinated() ? UHodgeCombatComponentBase::FindCombatComponent(GetAvatarActorFromActorInfo()) : nullptr) { Combat->ExecutionEvent(this, Tag); }
}

void UHodgeGameplayAbility_Definition::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted, FGuid Execution, int32 InstanceId)
{
	if (ExecutionId != Execution || MontageInstanceId != InstanceId || bEnding || bPredictedMoveCancel) { return; }
	bLifecycleEventSent = true;
	SendExecutionEvent(bInterrupted ? HodgeGameplayTags::GameplayEvent_Attack_Interrupted : HodgeGameplayTags::GameplayEvent_Attack_Completed);
	if (ExecutionId == Execution) { FinishExecution(bInterrupted, true); }
}

void UHodgeGameplayAbility_Definition::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, FGuid Execution, int32 InstanceId)
{
	if (ExecutionId == Execution && MontageInstanceId == InstanceId && !bEnding) { OnMontageBlendingOut(Montage, bInterrupted, Execution, InstanceId); }
}

void UHodgeGameplayAbility_Definition::ValidateExecutionConfiguration(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const
{
	if (!Definition.ExecutionConfig.Montage) { return; }
	if (Definition.ExecutionConfig.Montage->Notifies.ContainsByPredicate([](const FAnimNotifyEvent& Event)
	{ return (Event.Notify && Event.Notify->IsA<UHodgeAnimNotify_Hit>()) || (Event.NotifyStateClass && Event.NotifyStateClass->IsA<UHodgeAnimNotifyState_HitCheck>()); }))
	{ Errors.Add(FText::FromString(TEXT("Hit notifications require HodgeGameplayAbility_Melee or a subclass implementing their contract."))); }
}
