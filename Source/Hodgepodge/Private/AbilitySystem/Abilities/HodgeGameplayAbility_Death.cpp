/** @file HodgeGameplayAbility_Death.cpp
 * @brief 实现玩法能力死亡相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Abilities/HodgeGameplayAbility_Death.h"

#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "Component/HodgeHealthComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Trace/Trace.inl"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_Death)

/** 构造玩法能力死亡并初始化其默认状态。 */
UHodgeGameplayAbility_Death::UHodgeGameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	bAutoStartDeath = true;
	bAllowWhileHitReacting = true;
	FGameplayTagContainer Tags = GetAssetTags(); Tags.AddTag(HodgeGameplayTags::Ability_Type_StatusChange_Death); SetAssetTags(Tags);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// Add the ability trigger tag as default to the CDO.
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = HodgeGameplayTags::GameplayEvent_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

/** 启动本次技能，按现有配置提交消耗并建立执行资源。 */
void UHodgeGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayAbilityActivationInfo ActivationInfo,
                                                  const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);

	UHodgeAbilitySystemComponent* HodgeASC = CastChecked<UHodgeAbilitySystemComponent>(
		ActorInfo->AbilitySystemComponent.Get());

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(HodgeGameplayTags::Ability_Behavior_SurvivesDeath);

	// Cancel all abilities and block others from starting.
	HodgeASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

	SetCanBeCanceled(false);

	if (!ChangeActivationGroup(EHodgeAbilityActivationGroup::Exclusive_Blocking))
	{
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "UHodgeGameplayAbility_Death::ActivateAbility: Ability [%s] failed to change activation group to blocking."
		       ), *GetName());
	}

	if (bAutoStartDeath)
	{
		StartDeath();
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

/** 结束本次技能并释放其拥有的资源，再交还GAS生命周期。 */
void UHodgeGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);

	// Always try to finish the death when the ability ends in case the ability doesn't.
	// This won't do anything if the death hasn't been started.
	FinishDeath();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

/** 进入死亡开始阶段并应用既有死亡限制。 */
void UHodgeGameplayAbility_Death::StartDeath()
{
	if (UHodgeHealthComponent* HealthComponent = UHodgeHealthComponent::FindHealthComponent(
		GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EHodgeDeathState::NotDead)
		{
			HealthComponent->StartDeath();
		}
	}
}

/** 结束死亡。 */
void UHodgeGameplayAbility_Death::FinishDeath()
{
	if (UHodgeHealthComponent* HealthComponent = UHodgeHealthComponent::FindHealthComponent(
		GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EHodgeDeathState::DeathStarted)
		{
			HealthComponent->FinishDeath();
		}
	}
}
