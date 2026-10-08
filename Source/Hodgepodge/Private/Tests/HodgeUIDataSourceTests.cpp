#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UI/Data/HodgeGameplayUIDataSource.h"
#include "UI/Subsystem/HodgeUIManagerSubsystem.h"
#include "AbilitySystem/HodgeGameplayTags.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeUIInactiveDataTest,"Hodge.UI.DataSourceInactive",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeUIInactiveDataTest::RunTest(const FString& Parameters)
{
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
