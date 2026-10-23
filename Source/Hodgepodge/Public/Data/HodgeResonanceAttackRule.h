/** @file HodgeResonanceAttackRule.h @brief 共鸣阶段收益的配置和执行级宿主。 */
#pragma once
#include "Combat/HodgeAttackRule.h"
#include "ScalableFloat.h"
#include "HodgeResonanceAttackRule.generated.h"
class UGameplayEffect;

/** 共鸣配置按动作可选；不改变暗流或属性成长模型。 */
UCLASS()
class HODGEPODGE_API UHodgeResonanceAttackRuleConfig : public UHodgeAttackRuleConfig
{
	GENERATED_BODY()
public:
	UHodgeResonanceAttackRuleConfig();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FScalableFloat GainPerPhase = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UGameplayEffect> GainEffect;
	virtual UHodgeAttackRule* CreateRule(UObject* Outer) const override;
	virtual void ValidateRule(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const override;
};

/** 每执行独立阶段账本；NPC 缺资源 Set 时保持无收益。 */
UCLASS()
class HODGEPODGE_API UHodgeResonanceAttackRule : public UHodgeAttackRule
{
	GENERATED_BODY()
public:
	virtual void OnSettlement(const FHodgeAttackSettlement& Settlement) override;
private:
	FHodgeAttackPhaseLedger Phases;
};
