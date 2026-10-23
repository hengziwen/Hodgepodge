/** @file HodgeCombatPropertyCustomization.h @brief 提供战斗结构的常规与高级详情分组，不改变配置来源。 */
#pragma once

#include "IPropertyTypeCustomization.h"

namespace HodgeCombatDetails
{
	/** 提示模式归属并保留唯一反应入口，将显式冲击移到高级区。 */
	TSharedRef<IPropertyTypeCustomization> MakeHitReactionConfigCustomization();
	/** 保留目标基础运动，将容量、霸体积累与距离上限放到高级区。 */
	TSharedRef<IPropertyTypeCustomization> MakePoiseSettingsCustomization();
	/** 保留通知几何及继承入口，将查询与命中去重放到高级区。 */
	TSharedRef<IPropertyTypeCustomization> MakeAnimHitConfigCustomization();
	/** 旧布局保留动作入口，将方向变体、分段及运动来源折叠到高级区。 */
	TSharedRef<IPropertyTypeCustomization> MakeLegacyReactionAnimationCustomization();
}
