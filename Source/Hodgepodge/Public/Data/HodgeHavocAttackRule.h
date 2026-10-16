/** @file HodgeHavocAttackRule.h @brief 暗流命中／施放收益适配既有 HavocRules 宿主。 */
#pragma once
#include "Combat/HodgeAttackRule.h"
#include "HodgeHavocAttackRule.generated.h"

/** 角色专属参数从公共 Definition 移到可选配置。 */
UCLASS()
class HODGEPODGE_API UHodgeHavocAttackRuleConfig : public UHodgeAttackRuleConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float DarkFlowGainPerPhase = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float DarkFlowGainOnCast = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bEnhancedNormalAttack = false;
	virtual UHodgeAttackRule* CreateRule(UObject* Outer) const override;
	virtual void ValidateRule(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const override;
};

/** 只持有单次执行的阶段和施放账本，角色级拢音限流仍属于 HavocRules。 */
UCLASS()
class HODGEPODGE_API UHodgeHavocAttackRule : public UHodgeAttackRule
{
	GENERATED_BODY()
public:
	virtual void OnSettlement(const FHodgeAttackSettlement& Settlement) override;
	virtual void OnExecutionEvent(FGameplayTag Tag) override;
private:
	FHodgeAttackPhaseLedger Phases;
	bool bCastCommitted = false;
};
