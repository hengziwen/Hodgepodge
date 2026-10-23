/** @file HodgeAttackRule.cpp @brief 固定执行身份并提供彼此独立的阶段账本。 */
#include "Combat/HodgeAttackRule.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAttackRule)

/** 占用先于 GE 广播，零伤害和无阶段信息不消耗机会。 */
bool FHodgeAttackPhaseLedger::TryReserve(FName Group, FName Phase, float AcceptedDamage)
{
	if (Group.IsNone() || Phase.IsNone() || !FMath::IsFinite(AcceptedDamage) || AcceptedDamage <= 0.f) { return false; }
	const FHodgeHitGroupKey Key{Group, Phase};
	if (Phases.Contains(Key)) { return false; }
	Phases.Add(Key); return true;
}

/** 规则生命周期只属于当前激活，不监听下一次 GA 执行。 */
void UHodgeAttackRule::Initialize(UHodgeGameplayAbility_Definition* InSource, const UHodgeAttackRuleConfig* InConfig)
{
	Source = InSource; Config = InConfig;
	ExecutionId = InSource ? InSource->GetExecutionId() : FGuid();
	BoundASC = InSource ? InSource->GetAbilitySystemComponentFromActorInfo() : nullptr;
	BoundAvatar = InSource ? InSource->GetAvatarActorFromActorInfo() : nullptr;
}

/** 旧执行、旧 Pawn、注销 ASC 和非权威端都不能提交收益。 */
bool UHodgeAttackRule::IsCurrent() const
{
	return Source.IsValid() && Source->IsActive() && !Source->IsExecutionEnding() && ExecutionId.IsValid() &&
		Source->GetExecutionId() == ExecutionId && BoundASC.IsValid() && BoundASC->IsRegistered() &&
		BoundASC->IsOwnerActorAuthoritative() && BoundAvatar.IsValid() && !BoundAvatar->IsActorBeingDestroyed() &&
		BoundASC->GetAvatarActor() == BoundAvatar.Get() && Source->GetAvatarActorFromActorInfo() == BoundAvatar.Get() &&
		Source->GetAbilitySystemComponentFromActorInfo() == BoundASC.Get();
}

/** 结算携带的执行及 Spec 身份必须同时匹配。 */
bool UHodgeAttackRule::Accepts(const FHodgeAttackSettlement& Settlement) const
{
	return IsCurrent() && Settlement.ExecutionId == ExecutionId && Settlement.SettlementId.IsValid() &&
		Settlement.AbilityHandle == Source->GetCurrentAbilitySpecHandle();
}
