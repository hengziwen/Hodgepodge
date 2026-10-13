/** @file HodgeDamageRules.cpp
 * @brief 实现伤害目标过滤规则相关的类型、接口与现有处理流程。
 */
#include "Combat/HodgeDamageRules.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GenericTeamAgentInterface.h"

namespace
{
	/** 获取队伍。
	 * @param Actor 本次操作关联的Actor。
	 * @return 队伍的当前值或引用。
	 */
	FGenericTeamId GetTeam(const AActor* Actor)
	{
		if (const auto* Agent = Cast<IGenericTeamAgentInterface>(Actor)) { return Agent->GetGenericTeamId(); }
		if (const APawn* Pawn = Cast<APawn>(Actor))
		{
			if (const auto* Agent = Cast<IGenericTeamAgentInterface>(Pawn->GetController())) { return Agent->GetGenericTeamId(); }
		}
		return FGenericTeamId::NoTeam;
	}
}

/** 检查是否允许伤害。 */
bool FHodgeDamageRules::CanDamage(const AActor* Source, const AActor* Target, bool bAllowFriendlyFire)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!IsValid(Source) || !IsValid(Target) || Source == Target) { return false; }
	const UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Source);
	const UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
	if (!SourceASC || !TargetASC || SourceASC == TargetASC ||
		TargetASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death)) { return false; }
	const UHodgeHealthSet* Health = TargetASC->GetSet<UHodgeHealthSet>();
	if (!Health || Health->GetHealth() <= 0.f) { return false; }
	const FGenericTeamId SourceTeam = GetTeam(Source);
	const FGenericTeamId TargetTeam = GetTeam(Target);
	// 未接入队伍接口的角色视为无队伍，不把所有默认角色误判成友军。
	return bAllowFriendlyFire || SourceTeam == FGenericTeamId::NoTeam || TargetTeam == FGenericTeamId::NoTeam || SourceTeam != TargetTeam;
}
