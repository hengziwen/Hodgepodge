#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgeComboComponent.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
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
	const FGameplayAbilityActorInfo* Info, const FPredictionKey& Key, const FGameplayEventData& Payload)
{
	if (!Key.IsServerInitiatedKey() || IsActive()) { return; }
	FGameplayAbilityActivationInfo ActivationInfo(Info->OwnerActor.Get());
	ActivationInfo.SetActivationConfirmed();
	ActivationInfo.ServerSetActivationPredictionKey(Key);
	CallActivateAbility(Handle, Info, ActivationInfo, nullptr, &Payload);
}

bool UHodgeGameplayAbility_Definition::CanActivateAbility(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* Info, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* RelevantTags) const
{
	const auto* ASC = Info ? Cast<UHodgeAbilitySystemComponent>(Info->AbilitySystemComponent.Get()) : nullptr;
	const auto* Combo = Info && Info->OwnerActor.IsValid() ? Info->OwnerActor->FindComponentByClass<UHodgeComboComponent>() : nullptr;
	return ASC && ASC->FindAbilityDefinition(Handle) && Combo && Combo->IsAuthorized(Handle)
		&& Super::CanActivateAbility(Handle, Info, SourceTags, TargetTags, RelevantTags);
}

void UHodgeGameplayAbility_Definition::ActivateAbility(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Payload)
{
	bEnding = false;
	const auto* Definition = GetDefinition();
	TArray<FText> Errors;
	if (!Definition || !Definition->ValidateDefinition(Errors) || !CommitAbility(Handle, Info, ActivationInfo))
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
	if (!Instance) { FinishExecution(true, true); return; }
	TimelineTask = UHodgeAbilityTask_PlayTimeline::PlayMontageTimeline(this, Config.TimelineTaskConfig.Timeline,
		Anim, Config.Montage, Instance->GetInstanceID());
	TimelineTask->OnFinished.AddUObject(this, &ThisClass::OnTimelineFinished);
	TimelineTask->OnWindowsChanged.AddUObject(this, &ThisClass::OnWindowsChanged);
	TimelineTask->OnPoint.AddUObject(this, &ThisClass::OnPoint);
	if (auto* Combo = Info->OwnerActor->FindComponentByClass<UHodgeComboComponent>()) { Combo->ExecutionStarted(this); }
	if (IsActive() && TimelineTask) { TimelineTask->ReadyForActivation(); }
}

void UHodgeGameplayAbility_Definition::FinishExecution(bool bCancelled, bool bReplicate)
{
	if (IsActive()) { EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicate, bCancelled); }
}

void UHodgeGameplayAbility_Definition::EndAbility(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled)
{
	if (bEnding || !IsActive()) { return; }
	TGuardValue<bool> Guard(bEnding, true);
	if (TimelineTask)
	{
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
	if (auto* Combo = Info->OwnerActor->FindComponentByClass<UHodgeComboComponent>()) { Combo->ExecutionEnded(this); }
	Super::EndAbility(Handle, Info, ActivationInfo, bReplicate, bCancelled);
}

FGameplayTagContainer UHodgeGameplayAbility_Definition::GetExecutionWindows() const
{
	return TimelineTask ? TimelineTask->GetActiveWindowTags() : FGameplayTagContainer();
}
void UHodgeGameplayAbility_Definition::RefreshExecutionClock()
{
	if (TimelineTask) { TimelineTask->RefreshMontageClock(); }
}
void UHodgeGameplayAbility_Definition::OnTimelineFinished(EHodgeTimelineStopReason Reason)
{
	const auto* FinishedTask = TimelineTask.Get();
	if (Reason == EHodgeTimelineStopReason::NaturalEnd) { OnPoint(HodgeGameplayTags::GameplayEvent_Attack_Timeline_End); }
	if (TimelineTask == FinishedTask) { FinishExecution(Reason != EHodgeTimelineStopReason::NaturalEnd, true); }
}
void UHodgeGameplayAbility_Definition::OnWindowsChanged()
{
	if (auto* Combo = GetOwningActorFromActorInfo()->FindComponentByClass<UHodgeComboComponent>()) { Combo->WindowsChanged(this); }
}
void UHodgeGameplayAbility_Definition::OnPoint(FGameplayTag Tag)
{
	if (auto* Combo = GetOwningActorFromActorInfo()->FindComponentByClass<UHodgeComboComponent>()) { Combo->TimelineEvent(this, Tag); }
}
