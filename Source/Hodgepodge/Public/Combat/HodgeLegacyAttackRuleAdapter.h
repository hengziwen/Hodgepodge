/** @file HodgeLegacyAttackRuleAdapter.h @brief 旧 Definition 字段的临时只读适配，正式资产迁移后仅服务旧夹具。 */
#pragma once
#include "CoreMinimal.h"
class UHodgeAbilityDefinition;
class UHodgeGameplayAbility_Definition;
class UHodgeAttackRule;
namespace HodgeLegacyAttackRules
{
	/** 保留旧资产缺省 GE，不让公共 Definition 依赖角色资源实现。 */
	void InitializeDefaults(UHodgeAbilityDefinition& Definition);
	/** 新配置优先，同一规则绝不同时执行新旧路径。 */
	void CreateRules(const UHodgeAbilityDefinition& Definition, UHodgeGameplayAbility_Definition* Source, TArray<TObjectPtr<UHodgeAttackRule>>& OutRules);
	/** 旧字段的验证集中在适配层；全部引用迁移后可整体移除此层。 */
	void Validate(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors);
}
