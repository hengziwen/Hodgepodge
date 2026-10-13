/** @file HodgeGameplayAbility_Jump.cpp
 * @brief 实现玩法能力跳跃相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h"

#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "Character/HodgeCombatCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_Jump)

struct FGameplayTagContainer;


/** 构造玩法能力跳跃并初始化其默认状态。 */
UHodgeGameplayAbility_Jump::UHodgeGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

/** 检查技能激活所需的上下文、标签与现有GAS限制。 */
bool UHodgeGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayTagContainer* SourceTags,
                                                    const FGameplayTagContainer* TargetTags,
                                                    FGameplayTagContainer* OptionalRelevantTags) const
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const AHodgeCombatCharacter* HodgeCharacter = Cast<AHodgeCombatCharacter>(ActorInfo->AvatarActor.Get());
	if (!HodgeCharacter || !HodgeCharacter->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

/** 结束本次技能并释放其拥有的资源，再交还GAS生命周期。 */
void UHodgeGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop jumping in case the ability blueprint doesn't call it.
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

/** 按当前角色状态开始跳跃。 */
void UHodgeGameplayAbility_Jump::CharacterJumpStart()
{
	if (AHodgeCombatCharacter* HodgeCharacter = GetHodgeCharacterFromActorInfo())
	{
		if (HodgeCharacter->IsLocallyControlled() && !HodgeCharacter->bPressedJump)
		{
			HodgeCharacter->UnCrouch();
			HodgeCharacter->Jump();
		}
	}
}

/** 停止当前跳跃输入或保持状态。 */
void UHodgeGameplayAbility_Jump::CharacterJumpStop()
{
	if (AHodgeCombatCharacter* HodgeCharacter = GetHodgeCharacterFromActorInfo())
	{
		if (HodgeCharacter->IsLocallyControlled() && HodgeCharacter->bPressedJump)
		{
			HodgeCharacter->StopJumping();
		}
	}
}
