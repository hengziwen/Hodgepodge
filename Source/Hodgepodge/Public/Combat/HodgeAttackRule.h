/** @file HodgeAttackRule.h @brief 攻击只发布实际结算；可选规则独立持有本次执行的收益状态。 */
#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "HodgeAttackRule.generated.h"

class UHodgeGameplayAbility_Definition;
class UHodgeAbilityDefinition;
class UAbilitySystemComponent;
class UHodgeAttackRuleConfig;

/** 阶段身份不含目标，同一阶段命中多个敌人也只占用一次收益。 */
struct HODGEPODGE_API FHodgeHitGroupKey
{
	FName Group;
	FName Phase;
	bool operator==(const FHodgeHitGroupKey& Other) const { return Group == Other.Group && Phase == Other.Phase; }
	friend uint32 GetTypeHash(const FHodgeHitGroupKey& Key) { return HashCombine(GetTypeHash(Key.Group), GetTypeHash(Key.Phase)); }
};

/** 每个规则实例保存自己的阶段账本，不与命中次数或其他资源共享。 */
struct HODGEPODGE_API FHodgeAttackPhaseLedger
{
	bool TryReserve(FName Group, FName Phase, float AcceptedDamage);
	/** 测试和显式结束可清空账本；生产实例随执行直接释放。 */
	void Reset() { Phases.Reset(); }
private:
	TSet<FHodgeHitGroupKey> Phases;
};

/** 同步值载荷不保留 Spec 指针，SettlementId 隔离嵌套伤害的实际生命扣除。 */
struct HODGEPODGE_API FHodgeAttackSettlement
{
	FGuid ExecutionId;
	FGuid SettlementId;
	FGameplayAbilitySpecHandle AbilityHandle;
	FName HitGroup;
	FName AttackPhase;
	TWeakObjectPtr<UAbilitySystemComponent> TargetASC;
	float AcceptedDamage = 0.f;
};

/** 执行级小对象；仅消费当前服务器执行，不自行检测或创建玩家资源。 */
UCLASS(Abstract)
class HODGEPODGE_API UHodgeAttackRule : public UObject
{
	GENERATED_BODY()
public:
	/** 一次初始化固定能力、ASC、Avatar 和执行身份，结束后不能跟随 GA 复用。 */
	void Initialize(UHodgeGameplayAbility_Definition* InSource, const UHodgeAttackRuleConfig* InConfig);
	/** 原生默认空处理，具体收益由独立规则实现。 */
	virtual void OnSettlement(const FHodgeAttackSettlement& Settlement) {}
	virtual void OnExecutionEvent(FGameplayTag Tag) {}
protected:
	bool IsCurrent() const;
	bool Accepts(const FHodgeAttackSettlement& Settlement) const;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeGameplayAbility_Definition> Source;
	UPROPERTY(Transient) TObjectPtr<const UHodgeAttackRuleConfig> Config;
private:
	FGuid ExecutionId;
	TWeakObjectPtr<UAbilitySystemComponent> BoundASC;
	TWeakObjectPtr<AActor> BoundAvatar;
};

/** Definition 只引用可选配置；新角色增加配置和规则，不改公共 Melee。 */
UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class HODGEPODGE_API UHodgeAttackRuleConfig : public UObject
{
	GENERATED_BODY()
public:
	/** 工厂只创建本次执行对象，不在创建期间应用玩法副作用。 */
	virtual UHodgeAttackRule* CreateRule(UObject* Outer) const PURE_VIRTUAL(UHodgeAttackRuleConfig::CreateRule, return nullptr;);
	/** 规则自己验证专属字段和通知契约。 */
	virtual void ValidateRule(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const {}
};
