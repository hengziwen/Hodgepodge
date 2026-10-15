/** @file HodgeResonanceEnergyWidget.h @brief 保留现有大招类和资产引用，复用技能图标表现。 */
#pragma once
#include "UI/HodgeAbilityIconWidget.h"
#include "HodgeResonanceEnergyWidget.generated.h"

/** 大招专属默认值，不另建资源或冷却逻辑。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeResonanceEnergyWidget : public UHodgeAbilityIconWidget
{
	GENERATED_BODY()
public:
	/** 维持既有R键资产入口并启用独立能量环。 */
	UHodgeResonanceEnergyWidget();
};
