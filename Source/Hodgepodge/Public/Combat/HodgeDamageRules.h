#pragma once

#include "CoreMinimal.h"

class AActor;

// 检测与伤害执行共用同一套目标规则，避免过滤结果与实际结算不一致。
struct HODGEPODGE_API FHodgeDamageRules
{
	static bool CanDamage(const AActor* Source, const AActor* Target, bool bAllowFriendlyFire);
};
