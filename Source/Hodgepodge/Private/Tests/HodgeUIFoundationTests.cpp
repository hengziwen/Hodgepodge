#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UI/Foundation/HodgePrimaryGameLayout.h"
#include "UI/Subsystem/HodgeUIManagerSubsystem.h"
#include "CommonActivatableWidget.h"
#include "Engine/GameInstance.h"
#include "AbilitySystem/HodgeGameplayTags.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeUIClosedRootTest, "Hodge.UI.ClosedRootAndInvalidRequests",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeUIClosedRootTest::RunTest(const FString& Parameters)
{
    auto* Root = NewObject<UHodgePrimaryGameLayout>();
    int32 Completed = 0;
    auto Finish = [&Completed, this](UCommonActivatableWidget* Widget)
    { TestNull(TEXT("Invalid layer does not create a page"), Widget); ++Completed; };
    const auto Invalid = Root->PushAsync(HodgeGameplayTags::UI_Layer_Menu,
        TSoftClassPtr<UCommonActivatableWidget>(UCommonActivatableWidget::StaticClass()), Finish);
    TestFalse(TEXT("Invalid layer has no retained request"), Invalid.IsValid());
    TestEqual(TEXT("Failure completion runs once"), Completed, 1);
    TestEqual(TEXT("No failed request blocks input"), Root->GetPendingRequestCount(), 0);
    Root->CancelPush(FGuid::NewGuid());
    TestEqual(TEXT("Unknown cancellation is idempotent"), Completed, 1);
    Root->ReleaseLayout(); Root->ReleaseLayout();
    Root->PushAsync(HodgeGameplayTags::UI_Layer_Menu,
        TSoftClassPtr<UCommonActivatableWidget>(UCommonActivatableWidget::StaticClass()), Finish);
    TestEqual(TEXT("Released root refuses new asynchronous UI"), Completed, 2);
    TestFalse(TEXT("Released empty root has no blocking page"), Root->HasBlockingPage());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeUINonHodgeInstanceTest, "Hodge.UI.ManagerScope",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeUINonHodgeInstanceTest::RunTest(const FString& Parameters)
{
    auto* Instance = NewObject<UGameInstance>();
    auto* UI = NewObject<UHodgeUIManagerSubsystem>(Instance);
    TestFalse(TEXT("UI manager is not injected into foreign game instances"), UI->ShouldCreateSubsystem(Instance));
    TestNull(TEXT("Unknown player has no root"), UI->GetRootLayout(nullptr));
    UI->SuspendInput(nullptr, TEXT("Unknown")); UI->ResumeInput(nullptr, TEXT("Unknown")); UI->PlayerRemoved(nullptr);
    TestEqual(TEXT("Invalid lifecycle calls do not create player state"), UI->GetPlayerLayoutCount(), 0);
    TestTrue(TEXT("No local controller retains normal authority execution"), UHodgeUIManagerSubsystem::AllowsGameplayInput(nullptr));
    return true;
}
#endif
