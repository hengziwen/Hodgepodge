/** @file HodgeGameplayAbility_Interact.cpp
 * @brief 使用现有 GAS 按键激活流程提交一次服务器复核交互请求。
 */
// Copyright Epic Games, Inc. All Rights Reserved.
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Interact.h"

#include "Component/HodgeInteractionComponentBase.h"
#include "Core/PlayerController/HodgePlayerController.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_Interact)

UHodgeGameplayAbility_Interact::UHodgeGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 输入入口仅在拥有者执行，服务器业务通过组件 RPC 统一复核。
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	ActivationPolicy = EHodgeAbilityActivationPolicy::OnInputTriggered;
	ActivationGroup = EHodgeAbilityActivationGroup::Independent;
}

void UHodgeGameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	AHodgePlayerController* PC = GetHodgePlayerControllerFromActorInfo();
	// 旧 Avatar 能力即使迟到激活，也不能替当前 Pawn 发起交互。
	if (ActorInfo && ActorInfo->IsLocallyControlled() && PC && PC->GetPawn() == ActorInfo->AvatarActor.Get())
	{
		if (UHodgeInteractionComponentBase* Interaction = PC->FindComponentByClass<UHodgeInteractionComponentBase>())
		{
			Interaction->RequestNearestInteraction();
		}
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
