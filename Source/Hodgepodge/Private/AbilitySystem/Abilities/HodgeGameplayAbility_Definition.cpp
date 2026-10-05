#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Equipment/HodgeWeaponInstance.h"
#include "GameFramework/Pawn.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_Definition)

UHodgeGameplayAbility_Definition::UHodgeGameplayAbility_Definition(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationGroup = EHodgeAbilityActivationGroup::Exclusive_Replaceable;
	bServerRespectsRemoteAbilityCancellation = false;
}

const UHodgeAbilityDefinition* UHodgeGameplayAbility_Definition::GetDefinition() const
{
	const auto* ASC = Cast<UHodgeAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	return ASC ? ASC->FindAbilityDefinition(GetCurrentAbilitySpecHandle()) : nullptr;
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
	if (Definition && Definition->WeaponUseWindowTag.IsValid()
		&& !UHodgeWeaponInstance::ResolvePresentationWeapon(Cast<APawn>(Info->AvatarActor.Get()))) { return false; }
	return ASC && ASC->FindAbilityDefinition(Handle) && Combat && Combat->IsAuthorized(Handle)
		&& Super::CanActivateAbility(Handle, Info, SourceTags, TargetTags, RelevantTags);
}

void UHodgeGameplayAbility_Definition::ActivateAbility(FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* Info,
                                                       FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* Payload)
{
	bEnding = false;
	PresentationWeapon.Reset();
	WeaponUseHandles.Reset();
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
	if (Definition->WeaponUseWindowTag.IsValid())
	{
		PresentationWeapon = UHodgeWeaponInstance::ResolvePresentationWeapon(Cast<APawn>(Info->AvatarActor.Get()), GetCurrentSourceObject());
		if (!PresentationWeapon.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Weapon presentation requires one equipped configured weapon: %s"), *GetPathName());
			FinishExecution(true, true);
			return;
		}
	}
	if (!CommitAbility(Handle, Info, ActivationInfo))
	{
		FinishExecution(true, true);
		return;
	}
	auto* ASC = CastChecked<UHodgeAbilitySystemComponent>(Info->AbilitySystemComponent.Get());
	const auto& Config = Definition->ExecutionConfig;
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
	TimelineTask = UHodgeAbilityTask_PlayTimeline::PlayMontageTimeline(this, Config.TimelineTaskConfig.Timeline,
	                                                                   Anim, Config.Montage, Instance->GetInstanceID());
	TimelineTask->OnFinished.AddUObject(this, &ThisClass::OnTimelineFinished);
	TimelineTask->OnWindowsChanged.AddUObject(this, &ThisClass::OnWindowsChanged);
	TimelineTask->OnPoint.AddUObject(this, &ThisClass::OnPoint);
	TimelineTask->OnWindowEntered.AddUObject(this, &ThisClass::HandleExecutionWindowEntered);
	TimelineTask->OnWindowExited.AddUObject(this, &ThisClass::HandleExecutionWindowExited);
	OnExecutionReady();
	if (!IsActive() || bEnding || !TimelineTask) { return; }
	if (auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(Info->AvatarActor.Get()))
	{
		Combat->ExecutionStarted(this);
	}
	if (IsActive() && TimelineTask)
	{
		TimelineTask->ReadyForActivation();
	}
}

void UHodgeGameplayAbility_Definition::FinishExecution(bool bCancelled, bool bReplicate)
{
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicate, bCancelled);
	}
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
	TGuardValue<bool> Guard(bEnding, true);
	// 先失效旧身份，清理期间的回调不能消费上一段结果。
	const FGuid EndingExecutionId = ExecutionId;
	ExecutionId.Invalidate();
	OnExecutionEnding(EndingExecutionId);
	if (PresentationWeapon.IsValid()) { PresentationWeapon->ReleaseHandUsesForExecution(EndingExecutionId); }
	WeaponUseHandles.Reset();
	PresentationWeapon.Reset();
	if (TimelineTask)
	{
		TimelineTask->OnWindowEntered.RemoveAll(this);
		TimelineTask->OnWindowExited.RemoveAll(this);
		TimelineTask->OnFinished.RemoveAll(this);
		TimelineTask->OnWindowsChanged.RemoveAll(this);
		TimelineTask->OnPoint.RemoveAll(this);
		TimelineTask->StopTimeline(EHodgeTimelineStopReason::AbilityCancelled);
		TimelineTask = nullptr;
	}
	if (auto* ASC = Cast<UHodgeAbilitySystemComponent>(Info->AbilitySystemComponent.Get()))
	{
		ASC->StopDefinitionMontage(this, !bCancelled);
		ASC->ClearAnimatingAbility(this);
	}
	if (auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(Info->AvatarActor.Get())) { Combat->ExecutionEnded(this); }
	Super::EndAbility(Handle, Info, ActivationInfo, bReplicate, bCancelled);
}

FGameplayTagContainer UHodgeGameplayAbility_Definition::GetExecutionWindows() const
{
	return TimelineTask ? TimelineTask->GetActiveWindowTags() : FGameplayTagContainer();
}

void UHodgeGameplayAbility_Definition::RefreshExecutionClock()
{
	if (TimelineTask)
	{
		TimelineTask->RefreshMontageClock();
	}
}

void UHodgeGameplayAbility_Definition::OnTimelineFinished(EHodgeTimelineStopReason Reason)
{
	const auto* FinishedTask = TimelineTask.Get();
	if (Reason == EHodgeTimelineStopReason::NaturalEnd)
	{
		OnPoint(HodgeGameplayTags::GameplayEvent_Attack_Timeline_End);
	}
	if (TimelineTask == FinishedTask)
	{
		FinishExecution(Reason != EHodgeTimelineStopReason::NaturalEnd, true);
	}
}

void UHodgeGameplayAbility_Definition::OnWindowsChanged()
{
	if (auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(GetAvatarActorFromActorInfo()))
	{
		Combat->WindowsChanged(this);
	}
}

void UHodgeGameplayAbility_Definition::OnPoint(FGameplayTag Tag)
{
	if (auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(GetAvatarActorFromActorInfo()))
	{
		Combat->TimelineEvent(this, Tag);
	}
}

void UHodgeGameplayAbility_Definition::HandleExecutionWindowEntered(int32 EventIndex, FGameplayTag WindowTag)
{
	const auto* Definition = GetDefinition();
	if (Definition && WindowTag == Definition->WeaponUseWindowTag && PresentationWeapon.IsValid())
	{
		const auto Key = CurrentActivationInfo.GetActivationPredictionKey();
		const int32 Id = Key.IsServerInitiatedKey() ? -Key.Current : Key.Current;
		WeaponUseHandles.Add(EventIndex, PresentationWeapon->AcquireHandUse(ExecutionId, EventIndex, Id));
	}
	OnExecutionWindowEntered(EventIndex, WindowTag);
}

void UHodgeGameplayAbility_Definition::HandleExecutionWindowExited(int32 EventIndex, bool bSampleFinal)
{
	// 先结束几何采样，释放表现时不再有该窗口的命中会话。
	OnExecutionWindowExited(EventIndex, bSampleFinal);
	if (FGuid* Handle = WeaponUseHandles.Find(EventIndex))
	{
		if (PresentationWeapon.IsValid()) { PresentationWeapon->ReleaseHandUse(*Handle); }
		WeaponUseHandles.Remove(EventIndex);
	}
}

void UHodgeGameplayAbility_Definition::ValidateExecutionConfiguration(
	const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const
{
	if (!Definition.HitWindows.IsEmpty())
	{
		Errors.Add(FText::FromString(TEXT("HitWindows require a melee ability or a subclass implementing hit-window behavior; migrate AbilityClass to HodgeGameplayAbility_Melee.")));
	}
}
