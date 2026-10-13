/** @file HodgeDamageRules.h
 * @brief 声明伤害目标过滤规则相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"

class AActor;

// 检测与伤害执行共用同一套目标规则，避免过滤结果与实际结算不一致。
struct HODGEPODGE_API FHodgeDamageRules
{
	/** 检查是否允许伤害。
	 * @param Source 本次操作的来源。
	 * @param Target 本次操作的目标。
	 * @param bAllowFriendlyFire 是否Allow友方Fire的控制标记。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	static bool CanDamage(const AActor* Source, const AActor* Target, bool bAllowFriendlyFire);
};
