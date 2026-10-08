#if WITH_DEV_AUTOMATION_TESTS
#include "AbilitySystem/HodgeTimelineEvaluator.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEvaluatorTest,"Hodge.Timeline.SharedEvaluator",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeEvaluatorTest::RunTest(const FString& Parameters)
{
    TArray<FHodgeTimelineEvent> Events;
    FHodgeTimelineEvent Window;
    Window.EventID="First";Window.StartTime=0.f;Window.EndTime=.5f;Window.Priority=100;
    Window.WindowTag=HodgeGameplayTags::Status_Attack_Active;
    Events.Add(Window);
    Window.EventID="Second";Window.StartTime=.5f;Window.EndTime=1.0005f;Window.Priority=-100;
    Events.Add(Window);
    FHodgeTimelineEvent Point;
    Point.Kind=EHodgeTimelineEventKind::Point;Point.EventID="Point";Point.StartTime=.5f;Point.Priority=-200;
    Point.PointEventTag=HodgeGameplayTags::GameplayEvent_Attack_Test;
    Events.Add(Point);
    TArray<FHodgeTimelineNode> Nodes;
    FHodgeTimelineEvaluator::EvaluateRange(Events,1.f,0.f,1.f,Nodes);
    TestEqual(TEXT("A skipped frame collects every crossed edge"),Nodes.Num(),4);
    if(Nodes.Num()==4)
    {
        TestTrue(TEXT("End precedes begin regardless of priority"),Nodes[0].Kind==EHodgeTimelineNodeKind::WindowEnd);
        TestTrue(TEXT("Begin precedes point"),Nodes[1].Kind==EHodgeTimelineNodeKind::WindowBegin);
        TestTrue(TEXT("Point sees boundary state"),Nodes[2].Kind==EHodgeTimelineNodeKind::PointFire);
        TestEqual(TEXT("Rounded window end is reachable"),Nodes[3].Time,1.f);
    }
    TArray<int32> Active;
    FHodgeTimelineEvaluator::EvaluateAt(Events,1.f,.75f,Active);
    TestTrue(TEXT("Seek reconstructs only active windows"),Active.Num()==1 && Active[0]==1);
    FHodgeTimelineEvaluator::EvaluateAt(Events,1.f,.25f,Active);
    TestTrue(TEXT("Backward seek restores earlier window"),Active.Num()==1 && Active[0]==0);
    FHodgeTimelineEvaluator::EvaluateAt(Events,1.f,1.f,Active);
    TestEqual(TEXT("End is exclusive even with rounding"),Active.Num(),0);
    FHodgeTimelineEvaluator::EvaluateRange(Events,1.f,.5f,.5f,Nodes);
    TestEqual(TEXT("Pause emits no nodes"),Nodes.Num(),0);
    FHodgeTimelineEvaluator::EvaluateRange(Events,1.f,.75f,.25f,Nodes);
    TestEqual(TEXT("Runtime reverse interval emits no nodes"),Nodes.Num(),0);
    FHodgeTimelineEvaluator::Collect(Events,1.f,0.f,.5f,true,Nodes);
    FHodgeTimelineEvaluator::Sort(Events,Nodes);
    TestTrue(TEXT("Start offset restores window then exact point"),Nodes.Num()==2
        && Nodes[0].Kind==EHodgeTimelineNodeKind::WindowBegin && Nodes[1].Kind==EHodgeTimelineNodeKind::PointFire);

    auto* Timeline=NewObject<UHodgeAbilityTimeline>();
    Timeline->Events.Add(Point);
    for(float InvalidTime:{-.0005f,1.0005f})
    {
        Timeline->Events[0].StartTime=InvalidTime;
        TArray<FText> Errors;
        TestFalse(TEXT("Unreachable points rejected despite former tolerance"),Timeline->ValidateForPlayback(Errors));
    }
    for(float Endpoint:{0.f,1.f})
    {
        Timeline->Events[0].StartTime=Endpoint;
        TArray<FText> Errors;
        TestTrue(TEXT("Exact point endpoints remain valid"),Timeline->ValidateForPlayback(Errors));
        FHodgeTimelineEvaluator::Collect(Timeline->Events,1.f,0.f,Endpoint,Endpoint==0.f,Nodes);
        TestEqual(TEXT("Endpoint point is consumed exactly once"),Nodes.Num(),1);
    }
    return true;
}
#endif
