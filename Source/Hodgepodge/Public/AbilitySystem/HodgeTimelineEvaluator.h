#pragma once

#include "CoreMinimal.h"
#include "Data/HodgeAbilityTimeline.h"

// 顺序定义同刻消费语义：先退出窗口，再进入窗口，最后触发 Point。
enum class EHodgeTimelineNodeKind : uint8 { WindowEnd, WindowBegin, PointFire };

struct FHodgeTimelineNode
{
	float Time = 0.f;
	EHodgeTimelineNodeKind Kind = EHodgeTimelineNodeKind::PointFire;
	int32 EventIndex = INDEX_NONE;
};

/** 无时钟、无副作用的时间求值；游戏和编辑器以相同源动画时间调用。 */
class HODGEPODGE_API FHodgeTimelineEvaluator
{
public:
	static constexpr float WindowEndTolerance = 1.e-3f;
	static float WindowEnd(const FHodgeTimelineEvent& Event, float Duration);
	static void Collect(TConstArrayView<FHodgeTimelineEvent> Events, float Duration,
	                    float PreviousTime, float CurrentTime, bool bInitialize, TArray<FHodgeTimelineNode>& OutNodes);
	static void Sort(TConstArrayView<FHodgeTimelineEvent> Events, TArray<FHodgeTimelineNode>& Nodes);
	static void EvaluateRange(TConstArrayView<FHodgeTimelineEvent> Events, float Duration,
	                          float PreviousTime, float CurrentTime, TArray<FHodgeTimelineNode>& OutNodes);
	static void EvaluateAt(TConstArrayView<FHodgeTimelineEvent> Events, float Duration,
	                       float Time, TArray<int32>& OutActiveWindows);
};
