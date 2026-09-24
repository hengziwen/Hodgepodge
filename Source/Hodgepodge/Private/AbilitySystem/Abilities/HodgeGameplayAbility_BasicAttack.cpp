#include "AbilitySystem/Abilities/HodgeGameplayAbility_BasicAttack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h"
#include "AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Data/HodgeAbilityTimeline.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_BasicAttack)

UHodgeGameplayAbility_BasicAttack::UHodgeGameplayAbility_BasicAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationGroup = EHodgeAbilityActivationGroup::Exclusive_Replaceable;
	ActivationOwnedTags.AddTag(HodgeGameplayTags::Status_Attack);
	ActivationBlockedTags.AddTag(HodgeGameplayTags::Status_Attack);
	bServerRespectsRemoteAbilityCancellation = true;
}

void UHodgeGameplayAbility_BasicAttack::ActivateAbility(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	bEndingAttack = false;
	bChangingStep = false;
	bBufferedAttack = false;
	bool bValid = !AttackSteps.IsEmpty() && FMath::IsFinite(PlayRate) && PlayRate > 0.f;
	for (const FHodgeBasicAttackStep& Step : AttackSteps)
	{
		TArray<FText> Errors;
		bValid &= Step.Montage && Step.Timeline && Step.Timeline->ValidateForPlayback(Errors)
			&& FMath::IsNearlyEqual(Step.Montage->GetPlayLength(), Step.Timeline->Duration, 0.01f);
	}
	if (!bValid || !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (!bValid)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Hodge] BasicAttack rejected invalid steps/rate or montage-timeline duration mismatch: %s"), *GetPathName());
		}
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!IsActive()) { return; }
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ComboHandle = ASC->RegisterGameplayTagEvent(HodgeGameplayTags::Status_Attack_Cancel_NextAttack)
		.AddUObject(this, &ThisClass::OnComboWindowChanged);
	TimelineEndHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(HodgeGameplayTags::GameplayEvent_Attack_Timeline_End)
		.AddUObject(this, &ThisClass::OnTimelineEnded);
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->ConsumeMovementInputVector();
		Character->GetCharacterMovement()->StopMovementImmediately();
	}
	CurrentAttackStep = 1;
	StartStep();
}

void UHodgeGameplayAbility_BasicAttack::StartStep()
{
	bBufferedAttack = false;
	const FHodgeBasicAttackStep& Step = AttackSteps[CurrentAttackStep - 1];
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, Step.Montage, PlayRate);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnInterrupted);
	MontageTask->ReadyForActivation();
	if (!IsActive()) { return; }
	if (CurrentAttackStep < AttackSteps.Num())
	{
		InputTask = UAbilityTask_WaitInputPress::WaitInputPress(this, false);
		InputTask->OnPress.AddDynamic(this, &ThisClass::OnAttackPressed);
		InputTask->ReadyForActivation();
	}
	MoveTask = UHodgeAbilityTask_WaitMoveCancel::WaitMoveCancel(this,
		HodgeGameplayTags::Status_Attack_Cancel_Move, MoveIntentThreshold);
	MoveTask->OnMoveCancel.AddDynamic(this, &ThisClass::OnInterrupted);
	MoveTask->ReadyForActivation();
	if (!IsActive()) { return; }
	TimelineTask = UHodgeAbilityTask_PlayTimeline::PlayTimeline(this, Step.Timeline, 0.f, PlayRate);
	TimelineTask->ReadyForActivation();
}

void UHodgeGameplayAbility_BasicAttack::OnAttackPressed(float TimeWaited)
{
	bBufferedAttack = true;
	TryAdvance();
}

void UHodgeGameplayAbility_BasicAttack::OnComboWindowChanged(FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0) { TryAdvance(); }
}

void UHodgeGameplayAbility_BasicAttack::TryAdvance()
{
	if (!IsActive() || bEndingAttack || bChangingStep || !bBufferedAttack
		|| CurrentAttackStep >= AttackSteps.Num()
		|| !GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(HodgeGameplayTags::Status_Attack_Cancel_NextAttack))
	{
		return;
	}
	bChangingStep = true;
	ClearStep();
	++CurrentAttackStep;
	bChangingStep = false;
	StartStep();
}

void UHodgeGameplayAbility_BasicAttack::OnTimelineEnded(const FGameplayEventData* Payload)
{
	if (Payload && Payload->OptionalObject == TimelineTask) { OnCompleted(); }
}

void UHodgeGameplayAbility_BasicAttack::OnCompleted()
{
	if (IsActive() && !bChangingStep && !bEndingAttack)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UHodgeGameplayAbility_BasicAttack::OnInterrupted()
{
	if (IsActive() && !bChangingStep && !bEndingAttack)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UHodgeGameplayAbility_BasicAttack::ClearStep()
{
	if (InputTask)
	{
		// 引擎 WaitInputPress 只在收到输入时解绑；提前取消或换段也必须释放旧监听。
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputPressed,
				CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).RemoveAll(InputTask);
		}
		InputTask->OnPress.RemoveAll(this);
		InputTask->EndTask();
		InputTask = nullptr;
	}
	if (MoveTask) { MoveTask->OnMoveCancel.RemoveAll(this); MoveTask->EndTask(); MoveTask = nullptr; }
	if (TimelineTask) { TimelineTask->EndTask(); TimelineTask = nullptr; }
	if (MontageTask)
	{
		MontageTask->OnCompleted.RemoveAll(this);
		MontageTask->OnInterrupted.RemoveAll(this);
		MontageTask->OnCancelled.RemoveAll(this);
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (ASC && ASC->GetAnimatingAbility() == this)
		{
			ASC->CurrentMontageStop(FMath::Max(0.f, CancelBlendOutTime));
		}
		MontageTask->EndTask();
		MontageTask = nullptr;
	}
}

void UHodgeGameplayAbility_BasicAttack::EndAbility(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bEndingAttack || !IsActive()) { return; }
	bEndingAttack = true;
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->UnregisterGameplayTagEvent(ComboHandle, HodgeGameplayTags::Status_Attack_Cancel_NextAttack);
		if (auto* Delegate = ASC->GenericGameplayEventCallbacks.Find(HodgeGameplayTags::GameplayEvent_Attack_Timeline_End))
		{
			Delegate->Remove(TimelineEndHandle);
		}
	}
	ComboHandle.Reset();
	TimelineEndHandle.Reset();
	ClearStep();
	CurrentAttackStep = 0;
	bBufferedAttack = false;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
