/** @file HodgeUIDataSourceTests.cpp
 * @brief 实现界面数据来源自动化测试相关的类型、接口与现有处理流程。
 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UI/Data/HodgeGameplayUIDataSource.h"
#include "UI/Subsystem/HodgeUIManagerSubsystem.h"
#include "AbilitySystem/HodgeGameplayTags.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeUIInactiveDataTest,"Hodge.UI.DataSourceInactive",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeUIInactiveDataTest::RunTest(const FString& Parameters)
{
    // 本次操作的来源。
    auto* Source = NewObject<UHodgeGameplayUIDataSource>();
    Source->Initialize(nullptr,nullptr);
    TestFalse(TEXT("Invalid owner does not publish ready data"),Source->GetVitals().bReady);
    TestFalse(TEXT("Invalid owner cannot submit gameplay input"),Source->SubmitInput(HodgeGameplayTags::InputTag_Ability_Melee));
    Source->Shutdown(); Source->Shutdown();
    TestFalse(TEXT("Retained references to a closed source cannot submit"),Source->SubmitInput(HodgeGameplayTags::InputTag_Ability_Melee));
    TestFalse(TEXT("Closed source has no granted display entry"),Source->GetAbilityDisplayState(HodgeGameplayTags::InputTag_Ability_Melee).bGranted);
    TestNull(TEXT("No controller resolves no root"),UHodgeUIManagerSubsystem::GetRootLayoutForController(nullptr));
    TestNull(TEXT("No controller resolves no data source"),UHodgeUIManagerSubsystem::GetGameplayDataForController(nullptr));
    return true;
}
#endif
