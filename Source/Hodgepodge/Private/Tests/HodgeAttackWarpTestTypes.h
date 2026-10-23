/** @file HodgeAttackWarpTestTypes.h @brief 使用真实 GAS 激活测试朝向占用，播放链由 PIE 验证。 */
#pragma once
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
#include "Character/HodgeEnemyCharacter.h"
#include "GameFramework/PlayerController.h"
#include "HodgeAttackWarpTestTypes.generated.h"
UCLASS(Transient, NotBlueprintable)
class UHodgeAttackWarpTestAbility : public UHodgeGameplayAbility_Melee
{
	GENERATED_BODY()
public:
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		const FGameplayTagContainer* Source, const FGameplayTagContainer* Target, FGameplayTagContainer* Relevant) const override;
protected:
	/** 朝向用例隔离近战来源准备，Definition 的数据消费和真实播放仍照常执行。 */
	virtual void OnExecutionReady() override;
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo Activation, const FGameplayEventData* Payload) override;
};

/** 提供两个可区分的真实接口锚点，避免确认点测试误用表现点仍然通过。 */
UCLASS(Transient, NotBlueprintable)
class AHodgeAttackWarpTestTarget : public AHodgeEnemyCharacter
{
	GENERATED_BODY()
public:
	virtual bool GetLockPoint_Implementation(FName LockPointId, FVector& OutLocation) const override;
};

/** 在原生权威测试世界模拟远端玩家所有者，不绕过 GAS 的实际远端方向等待分支。 */
UCLASS(Transient, NotBlueprintable)
class AHodgeDeferredFacingTestController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual bool IsLocalController() const override;
};
