#include "AbilitySystem/HodgeTimelineEvaluator.h"

float FHodgeTimelineEvaluator::WindowEnd(const FHodgeTimelineEvent& Event, float Duration)
{
    // 只吸收窗口终点的舍入误差；Point 和窗口起点必须严格可达。
    return Event.EndTime > Duration && Event.EndTime <= Duration + WindowEndTolerance ? Duration : Event.EndTime;
}

void FHodgeTimelineEvaluator::Collect(TConstArrayView<FHodgeTimelineEvent> Events, float Duration,
    float PreviousTime, float CurrentTime, bool bInitialize, TArray<FHodgeTimelineNode>& OutNodes)
{
    OutNodes.Reset();
    if (!FMath::IsFinite(Duration) || Duration <= 0.f || !FMath::IsFinite(CurrentTime)
        || (!bInitialize && (!FMath::IsFinite(PreviousTime) || CurrentTime <= PreviousTime))) { return; }
    CurrentTime = FMath::Clamp(CurrentTime, 0.f, Duration);
    OutNodes.Reserve(Events.Num() * 2);
    auto Crossed = [PreviousTime, CurrentTime](float Time) { return PreviousTime < Time && Time <= CurrentTime; };
    for (int32 Index = 0; Index < Events.Num(); ++Index)
    {
        const auto& Event = Events[Index];
        if (Event.Kind == EHodgeTimelineEventKind::Window)
        {
            const float End = WindowEnd(Event, Duration);
            if (bInitialize)
            {
                if (Event.StartTime <= CurrentTime && CurrentTime < End)
                { OutNodes.Add({Event.StartTime, EHodgeTimelineNodeKind::WindowBegin, Index}); }
            }
            else
            {
                if (Crossed(End)) { OutNodes.Add({End, EHodgeTimelineNodeKind::WindowEnd, Index}); }
                if (Crossed(Event.StartTime)) { OutNodes.Add({Event.StartTime, EHodgeTimelineNodeKind::WindowBegin, Index}); }
            }
        }
        else if (Event.Kind == EHodgeTimelineEventKind::Point
            && (bInitialize ? Event.StartTime == CurrentTime : Crossed(Event.StartTime)))
        { OutNodes.Add({Event.StartTime, EHodgeTimelineNodeKind::PointFire, Index}); }
    }
}

void FHodgeTimelineEvaluator::Sort(TConstArrayView<FHodgeTimelineEvent> Events, TArray<FHodgeTimelineNode>& Nodes)
{
    Nodes.Sort([Events](const auto& A, const auto& B)
    {
        if (A.Time != B.Time) { return A.Time < B.Time; }
        if (A.Kind != B.Kind) { return uint8(A.Kind) < uint8(B.Kind); }
        if (Events[A.EventIndex].Priority != Events[B.EventIndex].Priority)
        { return Events[A.EventIndex].Priority < Events[B.EventIndex].Priority; }
        return A.EventIndex < B.EventIndex;
    });
}

void FHodgeTimelineEvaluator::EvaluateRange(TConstArrayView<FHodgeTimelineEvent> Events, float Duration,
    float PreviousTime, float CurrentTime, TArray<FHodgeTimelineNode>& OutNodes)
{
    Collect(Events, Duration, PreviousTime, CurrentTime, false, OutNodes);
    Sort(Events, OutNodes);
}

void FHodgeTimelineEvaluator::EvaluateAt(TConstArrayView<FHodgeTimelineEvent> Events, float Duration,
    float Time, TArray<int32>& OutActiveWindows)
{
    OutActiveWindows.Reset();
    if (!FMath::IsFinite(Time) || !FMath::IsFinite(Duration) || Duration <= 0.f || Time < 0.f || Time > Duration) { return; }
    for (int32 Index = 0; Index < Events.Num(); ++Index)
    {
        const auto& Event = Events[Index];
        if (Event.Kind == EHodgeTimelineEventKind::Window && Event.StartTime <= Time && Time < WindowEnd(Event, Duration))
        { OutActiveWindows.Add(Index); }
    }
}
