#if WITH_DEV_AUTOMATION_TESTS
#include "HodgeAbilityEditorToolkit.h"
#include "SHodgeAbilityPreview.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Components/SkeletalMeshComponent.h"
#include "Editor.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAbilityEditorTest,"Hodge.Editor.DefinitionWorkflow",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeAbilityEditorTest::RunTest(const FString& Parameters)
{
    auto* Source=LoadObject<UHodgeAbilityDefinition>(nullptr,TEXT("/Game/CodexText/DefinitionCombo/DA_Attack_5.DA_Attack_5"));
    if(!TestNotNull(TEXT("Existing definition fixture"),Source)){return false;}
    auto* Definition=DuplicateObject<UHodgeAbilityDefinition>(Source,GetTransientPackage());
    auto* Timeline=DuplicateObject<UHodgeAbilityTimeline>(Source->ExecutionConfig.TimelineTaskConfig.Timeline,GetTransientPackage());
    Definition->ExecutionConfig.TimelineTaskConfig.Timeline=Timeline;
    auto Toolkit=MakeShared<FHodgeAbilityEditorToolkit>();
    Toolkit->Init(Definition,nullptr);
    const int32 Before=Timeline->Events.Num();
    Toolkit->Seek(.25f);
    Toolkit->AddEvent(false);
    TestEqual(TEXT("Add edits referenced timeline"),Timeline->Events.Num(),Before+1);
    const FName PointID=Toolkit->SelectedID;
    GEditor->UndoTransaction();
    TestEqual(TEXT("Undo restores timeline"),Timeline->Events.Num(),Before);
    GEditor->RedoTransaction();
    TestEqual(TEXT("Redo restores event"),Timeline->Events.Num(),Before+1);
    Toolkit->SelectedID=PointID;
    Toolkit->Select(Toolkit->SelectedIndex());
    Toolkit->DuplicateEvent();
    TestEqual(TEXT("Duplicate adds a unique event"),Timeline->Events.Num(),Before+2);
    TestTrue(TEXT("Duplicate has a different identity"),Toolkit->SelectedID!=PointID);
    Toolkit->DeleteEvent();
    Toolkit->SelectedID=PointID;
    Toolkit->Select(Toolkit->SelectedIndex());
    Toolkit->DeleteEvent();
    TestEqual(TEXT("Delete restores original size"),Timeline->Events.Num(),Before);
    Toolkit->Seek(0.f);
    Toolkit->TogglePlay();
    Toolkit->TickPreview(.1f);
    TestTrue(TEXT("Forward playback advances source time"),Toolkit->Time>0.f);
    Toolkit->Seek(0.f);
    TestFalse(TEXT("Seek stops playback"),Toolkit->bPlaying);
    TestTrue(TEXT("Viewport exists"),Toolkit->Preview.IsValid());
    if(Toolkit->Preview)
    {
        auto* Component=Toolkit->Preview->Component;
        TestNotNull(TEXT("Montage has preview mesh"),Component->GetSkeletalMeshAsset());
        for(float ScrubTime : {.1f, .3f, .6f, .2f})
        {
            Toolkit->Preview->Client->bNeedsRedraw=false;
            Toolkit->Seek(ScrubTime);
            TestTrue(TEXT("Every scrub requests a viewport redraw without viewport focus"),Toolkit->Preview->Client->bNeedsRedraw);
        }
        Toolkit->Seek(0.f);
        const auto PoseA=Component->GetComponentSpaceTransforms();
        Toolkit->Seek(.7f);
        const auto PoseB=Component->GetComponentSpaceTransforms();
        bool bChanged=false;
        for(int32 Index=0;Index<FMath::Min(PoseA.Num(),PoseB.Num());++Index)
        { bChanged |= !PoseA[Index].Equals(PoseB[Index],.001f); }
        TestTrue(TEXT("Explicit time updates montage pose"),bChanged);
        TestFalse(TEXT("Scrubbing disables realtime rendering"),Toolkit->Preview->Client->IsRealtime());
    }
    Toolkit->Seek(Toolkit->Duration());
    TestEqual(TEXT("End has no active windows"),Toolkit->ActiveWindows.Num(),0);
    Toolkit->CloseWindow(EAssetEditorCloseReason::AssetEditorHostClosed);
    return true;
}
#endif
