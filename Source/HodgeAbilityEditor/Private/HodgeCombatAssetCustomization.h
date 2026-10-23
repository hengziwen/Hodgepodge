/** @file HodgeCombatAssetCustomization.h @brief 既有战斗资产的制作流程与检查入口。 */
#pragma once

#include "IDetailCustomization.h"

namespace HodgeCombatDetails
{
	/** 按攻击意图和动画时机整理现有 Definition。 */
	TSharedRef<IDetailCustomization> MakeAbilityDefinitionCustomization();
	/** 按目标规则整理现有 PawnData。 */
	TSharedRef<IDetailCustomization> MakePawnDataCustomization();
	/** 按目标表现整理 Profile，并提供单资产迁移入口。 */
	TSharedRef<IDetailCustomization> MakeHitReactionProfileCustomization();
}
