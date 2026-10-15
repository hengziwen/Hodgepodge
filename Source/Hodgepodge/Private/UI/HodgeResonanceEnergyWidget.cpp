/** @file HodgeResonanceEnergyWidget.cpp @brief 设置大招图标默认值，原蓝图路径保持有效。 */
#include "UI/HodgeResonanceEnergyWidget.h"
#include "AbilitySystem/AttributeSet/HodgeResonanceSet.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeResonanceEnergyWidget)

UHodgeResonanceEnergyWidget::UHodgeResonanceEnergyWidget()
{
	InputTag=HodgeResonanceTags::Input; bShowEnergyRing=true;
	Caption=FText::FromString(TEXT("共鸣解放")); KeyLabel=FText::FromString(TEXT("R"));
}
