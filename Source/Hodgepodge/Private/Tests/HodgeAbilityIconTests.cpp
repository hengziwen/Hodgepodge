/** @file HodgeAbilityIconTests.cpp @brief 验证冷却恢复方向、能量独立性和未就绪表现边界。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UI/HodgeAbilityIconWidget.h"
#include "UI/Data/HodgeGameplayUIDataSource.h"
#include <limits>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAbilityIconCooldownTest,"Hodge.UI.AbilityIcon.Cooldown",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeAbilityIconCooldownTest::RunTest(const FString& Parameters)
{
	FHodgeUIAbilityDisplayState Ability; Ability.bGranted=true; Ability.bInputAllowed=true; Ability.CooldownDuration=8.f;
	FHodgeUIResonanceSnapshot Resource;
	Ability.CooldownRemaining=8.f;
	auto View=UHodgeAbilityIconWidget::EvaluateState(Ability,Resource,false);
	TestEqual(TEXT("CD开始时亮色尚未从底部恢复"),View.CooldownReadyPercent,0.f);
	TestTrue(TEXT("普通技能不等待大招资源"),View.bReady);
	TestFalse(TEXT("冷却期间不能提交技能"),View.bCanActivate);
	Ability.CooldownRemaining=4.f; View=UHodgeAbilityIconWidget::EvaluateState(Ability,Resource,false);
	TestEqual(TEXT("8秒CD过4秒后亮色恢复一半"),View.CooldownReadyPercent,.5f);
	Ability.CooldownRemaining=0.f; View=UHodgeAbilityIconWidget::EvaluateState(Ability,Resource,false);
	TestEqual(TEXT("冷却结束时亮色到达顶部"),View.CooldownReadyPercent,1.f);
	TestTrue(TEXT("CD结束且其他条件满足时可用"),View.bCanActivate);
	Ability.CooldownRemaining=4.f; Ability.CooldownDuration=0.f;
	TestEqual(TEXT("未获有效总时长时不伪装成恢复完成"),UHodgeAbilityIconWidget::EvaluateState(Ability,Resource,false).CooldownReadyPercent,0.f);
	Ability.CooldownDuration=2.f;
	TestEqual(TEXT("网络修正后剩余超过总长也约束在零"),UHodgeAbilityIconWidget::EvaluateState(Ability,Resource,false).CooldownReadyPercent,0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAbilityIconEnergyTest,"Hodge.UI.AbilityIcon.EnergyIndependent",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeAbilityIconEnergyTest::RunTest(const FString& Parameters)
{
	FHodgeUIAbilityDisplayState Ability; Ability.bGranted=true; Ability.CooldownDuration=12.f; Ability.CooldownRemaining=6.f;
	FHodgeUIResonanceSnapshot Resource; Resource.bReady=true; Resource.Percent=.75f;
	auto View=UHodgeAbilityIconWidget::EvaluateState(Ability,Resource,true);
	TestEqual(TEXT("充能75不受半冷却影响"),View.EnergyPercent,.75f);
	TestEqual(TEXT("12秒CD过6秒时恢复一半"),View.CooldownReadyPercent,.5f);
	Resource.Percent=1.f; View=UHodgeAbilityIconWidget::EvaluateState(Ability,Resource,true);
	TestFalse(TEXT("满能量不清除CD或放开输入"),View.bCanActivate);
	Resource.Percent=std::numeric_limits<float>::quiet_NaN();
	TestEqual(TEXT("非法能量不向材质提交NaN"),UHodgeAbilityIconWidget::EvaluateState(Ability,Resource,true).EnergyPercent,0.f);
	Resource.bReady=false;
	TestFalse(TEXT("大招资源晚到时不显示假可用状态"),UHodgeAbilityIconWidget::EvaluateState(Ability,Resource,true).bReady);
	Ability.bGranted=false;
	TestFalse(TEXT("技能撤销后不保留已就绪显示"),UHodgeAbilityIconWidget::EvaluateState(Ability,Resource,false).bReady);
	return true;
}
#endif
