#pragma once
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "HodgeRelationshipTestAbility.generated.h"

/** 关系测试使用的持久能力，无动画或场景副作用。 */
UCLASS(Transient, NotBlueprintable)
class UHodgeRelationshipTestAbility : public UHodgeGameplayAbility
{
	GENERATED_BODY()
public:
	UHodgeRelationshipTestAbility();
protected:
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
};

/** 验证动作朝向的预测激活生命周期，无动画与资源副作用。 */
UCLASS(Transient, NotBlueprintable)
class UHodgeFacingPredictionTestAbility : public UHodgeRelationshipTestAbility
{
	GENERATED_BODY()
public:
	UHodgeFacingPredictionTestAbility();
};
