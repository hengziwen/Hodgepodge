/**
 * @file HodgeAbilityTask_PlayTimeline.cpp
 * @brief UHodgeAbilityTask_PlayTimeline 类的实现
 *
 * 核心是"单一时间节点序列"：每次推进都在 (PreviousTime, CurrentTime] 内收集
 * WindowEnd / WindowBegin / PointFire 三种节点，排序后严格顺序消费，
 * 同一时刻固定为 WindowEnd -> WindowBegin -> Point。
 *
 * 初始化（接续）与 Tick 共用 CollectNodes / SortNodes，所以两条路径的调度顺序完全一致。
 *
 * 设计依据：Docs/Design/ability-timeline-stage1.md
 */

// 111屎山代码来袭

#include "AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h"

#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Data/HodgeAbilityTimeline.h"
#include "Engine/World.h"
#include "GameplayEffect.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAbilityTask_PlayTimeline)

namespace
{
	// 左开右闭，与推进区间一致：保证每个时刻恰好被消费一次，且起点时刻不丢。
	FORCEINLINE bool IsInAdvanceInterval(float Time, float PreviousTime, float CurrentTime)
	{
		return Time > PreviousTime && Time <= CurrentTime;
	}
}

UHodgeAbilityTask_PlayTimeline::UHodgeAbilityTask_PlayTimeline(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 不设置它，TickTask 不会被调用。
	bTickingTask = true;
}

UHodgeAbilityTask_PlayTimeline* UHodgeAbilityTask_PlayTimeline::PlayTimeline(
	UGameplayAbility* OwningAbility, UHodgeAbilityTimeline* Timeline,
	float StartOffset, float InitialPlayRate)
{
	UHodgeAbilityTask_PlayTimeline* Task = NewAbilityTask<UHodgeAbilityTask_PlayTimeline>(OwningAbility);

	Task->TimelineAsset = Timeline;
	Task->StartOffset = StartOffset;
	Task->InitialPlayRate = InitialPlayRate;

	return Task;
}

void UHodgeAbilityTask_PlayTimeline::Activate()
{
	Super::Activate();

	// 校验集中在唯一入口：C++ 与蓝图调用最终都会经过这里（C++ 需先 ReadyForActivation）。
	const bool bParamsValid =
		IsValid(TimelineAsset) &&
		FMath::IsFinite(TimelineAsset->Duration) &&
		TimelineAsset->Duration > 0.f &&
		FMath::IsFinite(InitialPlayRate) &&
		InitialPlayRate > 0.f &&
		FMath::IsFinite(StartOffset) &&
		StartOffset >= 0.f &&
		StartOffset < TimelineAsset->Duration;

	if (!bParamsValid)
	{
		UE_LOG(LogTemp, Error,
		       TEXT("[Hodge] PlayTimeline 参数非法：Timeline=%s Duration=%.3f StartOffset=%.3f PlayRate=%.3f"),
		       *GetNameSafe(TimelineAsset),
		       IsValid(TimelineAsset) ? TimelineAsset->Duration : -1.f,
		       StartOffset, InitialPlayRate);

		// 不留一个"活着但不动"的 Task。
		EndTask();
		return;
	}

	TArray<FText> ValidationErrors;
	if (!TimelineAsset->ValidateForPlayback(ValidationErrors))
	{
		for (const FText& Error : ValidationErrors)
		{
			UE_LOG(LogTemp, Error, TEXT("[Hodge] PlayTimeline invalid asset %s: %s"),
				*GetNameSafe(TimelineAsset), *Error.ToString());
		}
		EndTask();
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[Hodge] PlayTimeline 拿不到 World，Task 结束。"));
		EndTask();
		return;
	}

	LogicalElapsed = 0.f;
	ElapsedTime = StartOffset;

	// 采样点必须在第一次 Tick 之前就位，否则首帧会算出一个巨大的时间差。
	LastUpdateWorldTime = World->GetTimeSeconds();

	InitializeTimeline(StartOffset);
}

void UHodgeAbilityTask_PlayTimeline::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (bStopped)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float Now = World->GetTimeSeconds();
	const float WorldDelta = Now - LastUpdateWorldTime;

	// 先更新采样点，再判方向。
	// 若把它挪到下面的 return 之后，后续每一帧都会重新算出同一个负差值，Timeline 会被永久冻死。
	LastUpdateWorldTime = Now;

	// 负差值必须在**累加之前**拦掉。
	// 只靠 AdvanceTimeline 里的 CurrentTime <= PreviousTime 是拦不住的：那时 LogicalElapsed /
	// ElapsedTime 已经被污染成负数，而下一帧起 CurrentTime 又开始单调增长，于是早期节点会被
	// 重新跨过（Point 重复派发），并且 ElapsedTime 要很久才能爬回 Duration —— Ability 会远超预期时长挂着。
	if (WorldDelta <= 0.f)
	{
		return;
	}

	// 用世界时间差累加，而不是用 TickTask 的 DeltaTime：
	// 卡顿后能一次性把区间追上（这正是"跨帧消费"要覆盖的情形）。
	LogicalElapsed += WorldDelta * InitialPlayRate;

	const float PreviousTime = ElapsedTime;

	// 时钟不允许越过 Duration。
	ElapsedTime = FMath::Min(StartOffset + LogicalElapsed, TimelineAsset->Duration);

	// 先把区间内的全部时间节点消费完……
	AdvanceTimeline(PreviousTime, ElapsedTime);

	// ……其间任意一个节点都可能已经结束了 Ability（Task 已被 OnDestroy，bStopped 已被置位）。
	if (bStopped)
	{
		return;
	}

	// ……再判定自然结束，这样 StartTime == Duration 的 Point 不会被提前吃掉。
	if (ElapsedTime >= TimelineAsset->Duration)
	{
		StopTimeline(EHodgeTimelineStopReason::NaturalEnd);
	}
}

// 收集时间节点。
//
// 两种模式共用同一个比较器，这是"从 0 跑"与"从 StartOffset 接续"调度顺序一致的基础：
//   bInitializing == false：收集落在 (PreviousTime, CurrentTime] 内的全部边界与时刻。
//   bInitializing == true ：只做两件事 —— 补"起点时仍然生效"的窗口（节点时刻取它的 StartTime，
//                           于是它落在与正常推进相同的位置上），以及"恰好落在起点"的 Point。
//                           历史（StartTime < StartOffset）一律不补：这是接续语义，不是历史重放。
void UHodgeAbilityTask_PlayTimeline::CollectNodes(float PreviousTime, float CurrentTime,
												  bool bInitializing, TArray<FHodgeTimelineNode>& OutNodes) const
{
	OutNodes.Reset();

	if (!IsValid(TimelineAsset))
	{
		return;
	}

	const TArray<FHodgeTimelineEvent>& Events = TimelineAsset->Events;
	OutNodes.Reserve(Events.Num() * 2);

	for (int32 EventIndex = 0; EventIndex < Events.Num(); ++EventIndex)
	{
		const FHodgeTimelineEvent& Event = Events[EventIndex];

		if (Event.Kind == EHodgeTimelineEventKind::Window)
		{
			if (bInitializing)
			{
				// 起点恢复：只在"起点时仍然生效"（Start <= T < End）时补一次 WindowBegin。
				// 只影响调度顺序 —— 窗口 GE 的实际施加时刻仍然是 StartOffset。
				if (Event.StartTime <= CurrentTime && CurrentTime < Event.EndTime)
				{
					OutNodes.Add({Event.StartTime, EHodgeTimelineNodeKind::WindowBegin, EventIndex});
				}
			}
			else
			{
				if (IsInAdvanceInterval(Event.EndTime, PreviousTime, CurrentTime))
				{
					OutNodes.Add({Event.EndTime, EHodgeTimelineNodeKind::WindowEnd, EventIndex});
				}

				if (IsInAdvanceInterval(Event.StartTime, PreviousTime, CurrentTime))
				{
					OutNodes.Add({Event.StartTime, EHodgeTimelineNodeKind::WindowBegin, EventIndex});
				}
			}
		}
		else
		{
			const bool bCollect = bInitializing
				? (Event.StartTime == CurrentTime)
				: IsInAdvanceInterval(Event.StartTime, PreviousTime, CurrentTime);

			if (bCollect)
			{
				OutNodes.Add({Event.StartTime, EHodgeTimelineNodeKind::PointFire, EventIndex});
			}
		}
	}
}

// 节点排序：时间 → 节点种类 → Priority → EventIndex。
//
// Priority 刻意排在"节点种类"之后：语义顺序（区间 [Start, End) 的数学定义）优先于作者配置，
// 否则"某一刻的状态"会变成可由 Priority 改写的，跨端一致与区间定义都会破。
void UHodgeAbilityTask_PlayTimeline::SortNodes(TArray<FHodgeTimelineNode>& Nodes) const
{
	if (!IsValid(TimelineAsset))
	{
		return;
	}

	const TArray<FHodgeTimelineEvent>& Events = TimelineAsset->Events;

	Nodes.Sort([&Events](const FHodgeTimelineNode& Left, const FHodgeTimelineNode& Right)
	{
		if (Left.Time != Right.Time)
		{
			return Left.Time < Right.Time;
		}

		if (Left.Kind != Right.Kind)
		{
			return static_cast<uint8>(Left.Kind) < static_cast<uint8>(Right.Kind);
		}

		const int32 LeftPriority = Events.IsValidIndex(Left.EventIndex) ? Events[Left.EventIndex].Priority : 0;
		const int32 RightPriority = Events.IsValidIndex(Right.EventIndex) ? Events[Right.EventIndex].Priority : 0;

		if (LeftPriority != RightPriority)
		{
			return LeftPriority < RightPriority;
		}

		// 同刻同类同优先级：用下标兜底，保证跨端严格确定。
		return Left.EventIndex < Right.EventIndex;
	});
}

void UHodgeAbilityTask_PlayTimeline::InitializeTimeline(float InStartOffset)
{
	// 关键：初始化不自己发明一套顺序，而是走与 Tick 完全相同的收集 + 排序 + 消费。
	// 否则"接续运行时"的调度顺序会与"正常推进"不一致。
	//
	// 语义边界：这只保证当前状态重建的调度顺序一致，不保证历史副作用等价 ——
	// 窗口 GE 的 OnActive / Cue / 周期都以 StartOffset 为实际发生时刻。
	TArray<FHodgeTimelineNode> Nodes;
	CollectNodes(0.f, InStartOffset, /*bInitializing=*/true, Nodes);
	SortNodes(Nodes);

	for (const FHodgeTimelineNode& Node : Nodes)
	{
		// 前一个节点可能已经把 Ability 结束掉了。
		if (bStopped)
		{
			return;
		}

		switch (Node.Kind)
		{
		case EHodgeTimelineNodeKind::WindowBegin:
			EnterWindow(Node.EventIndex);
			break;

		case EHodgeTimelineNodeKind::PointFire:
			FirePointEvent(TimelineAsset->Events[Node.EventIndex]);
			break;

		case EHodgeTimelineNodeKind::WindowEnd:
			// 不会出现：EndTime <= StartOffset 的窗口在起点就已经退出，CollectNodes 不会收集它。
			break;
		}
	}
}

void UHodgeAbilityTask_PlayTimeline::AdvanceTimeline(float PreviousTime, float CurrentTime)
{
	// 廉价的不变式校验。注意它**不是**时钟倒退的防线（真正的防线在 TickTask 里，
	// 必须在污染 LogicalElapsed 之前拦），这里只是防止把空区间或倒序区间喂给节点收集。
	if (CurrentTime <= PreviousTime)
	{
		return;
	}

	const TArray<FHodgeTimelineEvent>& Events = TimelineAsset->Events;

	// 1) 收集 + 2) 排序：与 InitializeTimeline 共用同一套规则。
	TArray<FHodgeTimelineNode> Nodes;
	CollectNodes(PreviousTime, CurrentTime, /*bInitializing=*/false, Nodes);
	SortNodes(Nodes);

	// 3) 严格按序消费。
	for (const FHodgeTimelineNode& Node : Nodes)
	{
		// 三种节点都会写 ASC（加 tag / 施 GE / 派发事件），都可能同步结束 Ability 并销毁本 Task。
		// 一旦停止就必须立刻放弃剩余节点：否则会在清理之后重新把窗口状态加上去，永久残留。
		if (bStopped)
		{
			return;
		}

		const FHodgeTimelineEvent& Event = Events[Node.EventIndex];

		switch (Node.Kind)
		{
		case EHodgeTimelineNodeKind::WindowEnd:
			ExitWindow(Node.EventIndex);
			break;

		case EHodgeTimelineNodeKind::WindowBegin:
			EnterWindow(Node.EventIndex);
			break;

		case EHodgeTimelineNodeKind::PointFire:
			FirePointEvent(Event);
			break;
		}

		// 关键：上面的调用返回后可能已经销毁，游标必须立刻失效。
		if (bStopped)
		{
			return;
		}
	}
}

bool UHodgeAbilityTask_PlayTimeline::HasAuthorityOnAvatar() const
{
	const AActor* Avatar = GetAvatarActor();
	return Avatar != nullptr && Avatar->HasAuthority();
}

void UHodgeAbilityTask_PlayTimeline::EnterWindow(int32 EventIndex)
{
	if (!IsValid(TimelineAsset) || !TimelineAsset->Events.IsValidIndex(EventIndex))
	{
		return;
	}

	// UE 5.5 的 UAbilityTask::AbilitySystemComponent 是 TWeakObjectPtr，取一次局部指针并判空。
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC)
	{
		return;
	}

	// 防御：同一窗口只允许进入一次，否则 tag 会多加、GE 会多施。
	if (ActiveWindowIndices.Contains(EventIndex))
	{
		return;
	}

	const FHodgeTimelineEvent& Event = TimelineAsset->Events[EventIndex];

	// 先记账，再改 ASC。引擎是先更新标签计数、再广播回调，
	// 所以重入进来的清理一定看得见这一条并配对移除。
	ActiveWindowIndices.Add(EventIndex);
	ASC->AddLooseGameplayTag(Event.WindowTag, 1);

	// ---- 防线一：tag 回调可能已经结束了 Ability 并完成清理 ----
	// 此时绝不能再去施加 GE。"施加后立刻移除"只能撤销最终状态，
	// 撤销不了 Apply 期间已经发生的副作用（属性变化、Cue、OnActive、Tag 变化、能力阻断重算）。
	if (bStopped || bCleanedUp)
	{
		return;
	}

	// GE 无法"先记账"：句柄要等施加完才拿得到。
	// Window 的 GE 只在权威端施加，并依赖 GE 复制 —— 两端各施一个 Infinite GE 会产生两个实例，
	// 取消时序一旦错开就永久残留。
	if (Event.WindowEffectClass && HasAuthorityOnAvatar())
	{
		const FActiveGameplayEffectHandle Handle = ApplyTimelineEffect(ASC, Event.WindowEffectClass);

		// 施加失败（Tag 阻断 / 免疫 / 类无效）不入账，否则账本会失真。
		if (!Handle.IsValid())
		{
			return;
		}

		// ---- 防线二：Apply 内部发生重入 ----
		// 账本里没有这个句柄，不补偿就永久残留。
		if (bCleanedUp)
		{
			ASC->RemoveActiveGameplayEffect(Handle);
			return;
		}

		WindowEffectHandles.Add(EventIndex, Handle);
	}
}

void UHodgeAbilityTask_PlayTimeline::ExitWindow(int32 EventIndex)
{
	if (!IsValid(TimelineAsset) || !TimelineAsset->Events.IsValidIndex(EventIndex))
	{
		return;
	}

	// 先记账移除，再改 ASC：重入进来的清理不会重复处理这一条。
	if (!ActiveWindowIndices.Remove(EventIndex))
	{
		return;
	}

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC)
	{
		return;
	}

	const FHodgeTimelineEvent& Event = TimelineAsset->Events[EventIndex];

	// GE 先移除（可能触发属性变化与回调），再减 tag。
	if (const FActiveGameplayEffectHandle* Found = WindowEffectHandles.Find(EventIndex))
	{
		const FActiveGameplayEffectHandle Handle = *Found;
		WindowEffectHandles.Remove(EventIndex);

		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	// ⚠️ 这里绝对不能加 `if (bStopped) return;`，和 EnterWindow 正相反：
	//    EnterWindow 是"先记账再改 ASC"，重入清理会把这一条一起撤掉，提前返回是安全的；
	//    ExitWindow 已经先把账本条目摘掉了，清理不会再管这个窗口的 tag，
	//    在这里提前返回 = WindowTag 计数永久多 1。
	//    也就是说"给所有 ASC 调用后面统一加守卫"这个做法是错的，判断依据是"这个操作是否已被记进账本"。
	//
	// 用入参时取到的那个局部 ASC 指针，而不是重新 AbilitySystemComponent.Get()：
	// 当前调用栈已经接手了这个窗口的清理责任，就不该因为 Task 内部引用状态变化而半途甩手。
	// IsValid 只为对付"ASC 自身已被销毁"这一种情况 —— 那时它的 tag 容器也一起没了，
	// 跳过是正确行为；同时也避免拿一个悬垂裸指针去调成员函数。
	if (IsValid(ASC))
	{
		ASC->RemoveLooseGameplayTag(Event.WindowTag, 1);
	}
}

FActiveGameplayEffectHandle UHodgeAbilityTask_PlayTimeline::ApplyTimelineEffect(
	UAbilitySystemComponent* ASC, TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!ASC || !EffectClass)
	{
		return FActiveGameplayEffectHandle();
	}

	// 只建一个"干净"的 Context：不要 AddSourceObject(this)。
	// 本 Task 是调度实现细节，不是战斗语义上的"来源"；而且 PointEffectClass 允许 HasDuration，
	// GE 完全可能在本 Task 销毁之后还活着，让一个已经结束的调度 Task 当 SourceObject 只会误导。
	// 需要 Instigator / EffectCauser / Ability 时，应由 GA 侧提供 Context。
	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();

	const FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(EffectClass, /*Level=*/1.f, Context);
	if (!Spec.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[Hodge] ApplyTimelineEffect 构造 Spec 失败：%s"), *GetNameSafe(EffectClass));
		return FActiveGameplayEffectHandle();
	}

	return ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
}

void UHodgeAbilityTask_PlayTimeline::FirePointEvent(const FHodgeTimelineEvent& Event)
{
	// 先算出"本端是否该执行"，再派发一次。
	// listen server 上本地玩家的 Ability 会同时满足 HasAuthority 与 IsLocallyControlled，
	// 写成两个独立 if 会让同一个条目执行两次。
	const AActor* Avatar = GetAvatarActor();
	const bool bIsAuthority = Avatar != nullptr && Avatar->HasAuthority();
	const bool bIsLocallyControlled = IsLocallyControlled();

	bool bShouldFire = false;
	switch (Event.NetPolicy)
	{
	case EHodgeTimelineEventNetPolicy::LocalAndAuthority:
		bShouldFire = bIsAuthority || bIsLocallyControlled;
		break;

	case EHodgeTimelineEventNetPolicy::AuthorityOnly:
		bShouldFire = bIsAuthority;
		break;

	case EHodgeTimelineEventNetPolicy::LocallyControlledOnly:
		bShouldFire = bIsLocallyControlled;
		break;
	}

	if (!bShouldFire)
	{
		return;
	}

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC)
	{
		return;
	}

	// 载荷带身份：OptionalObject 指向本 Task，便于订阅方确认"这是我那条 Timeline"。
	FGameplayEventData Payload;
	Payload.EventTag = Event.PointEventTag;
	Payload.Instigator = Avatar;
	Payload.OptionalObject = this;
	Payload.EventMagnitude = Event.StartTime;

	// 固定顺序 ①：先派发语义事件（同步，可能结束 Ability / 停止 Timeline）。
	ASC->HandleGameplayEvent(Event.PointEventTag, &Payload);

	// 事件的处理者若结束了这一帧，就不再施加效果 —— 这是契约，不是 bug。
	if (bStopped)
	{
		return;
	}

	// 固定顺序 ②：再施加本条目配置的 GE。
	//
	// 只在权威端施加：客户端预测施加 + 服务器施加会得到两份实例，取消时序错开就会残留。
	// 契约：Point 只负责 Apply 这一次，此后生命周期（Instant 立即结算、HasDuration 自行到期）
	// 完全不属于 Timeline —— 不记账、也不移除它。所以校验禁止配 Infinite。
	if (Event.PointEffectClass && bIsAuthority)
	{
		ApplyTimelineEffect(ASC, Event.PointEffectClass);
	}
}

void UHodgeAbilityTask_PlayTimeline::FireSystemEvent(const FGameplayTag& EventTag)
{
	// 系统事件没有资产条目，构造一个临时条目复用同一条派发路径，避免出现第二套逻辑。
	// 它是"消息"而不是"状态"，所以只填 Point 侧的字段。
	FHodgeTimelineEvent Event;
	Event.Kind = EHodgeTimelineEventKind::Point;
	Event.StartTime = ElapsedTime;
	Event.PointEventTag = EventTag;
	Event.NetPolicy = EHodgeTimelineEventNetPolicy::LocalAndAuthority;

	FirePointEvent(Event);
}

void UHodgeAbilityTask_PlayTimeline::ClearAllWindowState()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();

	// 先搬走并清空两个账本，再动 ASC。
	// 原因：移除 loose tag / 移除 GE 都会触发回调，回调可能在调用栈上重入本函数；
	// 若此时外层还在遍历账本，内层的 Reset() 会让外层读到已释放的内存。
	TArray<int32> IndicesToRemove = MoveTemp(ActiveWindowIndices);
	TMap<int32, FActiveGameplayEffectHandle> HandlesToRemove = MoveTemp(WindowEffectHandles);
	ActiveWindowIndices.Reset();
	WindowEffectHandles.Reset();

	// 账本已封存：从这一刻起新施加的 GE 不会再入账，必须由 EnterWindow 自补偿，
	// 所以这里立刻置位（而不是等函数返回）。
	bCleanedUp = true;

	if (!ASC)
	{
		return;
	}

	// 先撤 GE，再撤 tag：GE 可能带属性变化与更强的回调。
	for (const TPair<int32, FActiveGameplayEffectHandle>& Pair : HandlesToRemove)
	{
		if (Pair.Value.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Pair.Value);
		}
	}

	// TimelineAsset 为空时账本必然也是空的（没有任何窗口进过），所以这里的判断只是兜底。
	if (IsValid(TimelineAsset))
	{
		for (int32 EventIndex : IndicesToRemove)
		{
			if (TimelineAsset->Events.IsValidIndex(EventIndex))
			{
				ASC->RemoveLooseGameplayTag(TimelineAsset->Events[EventIndex].WindowTag, 1);
			}
		}
	}
}

void UHodgeAbilityTask_PlayTimeline::StopTimeline(EHodgeTimelineStopReason Reason)
{
	if (bStopped)
	{
		return;
	}

	bStopped = true;

	// 先清理本端状态，再广播，避免订阅者读到即将失效的旧状态。
	ClearAllWindowState();

	switch (Reason)
	{
	case EHodgeTimelineStopReason::NaturalEnd:
		FireSystemEvent(HodgeGameplayTags::GameplayEvent_Attack_Timeline_End);
		break;

	case EHodgeTimelineStopReason::Interrupted:
		FireSystemEvent(HodgeGameplayTags::GameplayEvent_Attack_Interrupted);
		break;

	default:
		// AbilityCancelled / None 不广播任何事件。
		break;
	}

	// 停止必须真正结束 Task，否则会留下一个"不再推进但仍然活着"的对象。
	// 重复 EndTask 由引擎的 TaskState 守卫，这里不需要额外判断。
	EndTask();
}

void UHodgeAbilityTask_PlayTimeline::OnDestroy(bool bInOwnerFinished)
{
	// 头等规则：从这一刻起，任何仍在调用栈里的时间轴推进都必须立刻停止。
	// 不设这一行，AdvanceTimeline 的循环会在清理之后继续执行，重新把窗口状态加上去并永久残留。
	bStopped = true;

	// 无条件兜底：Ability 被外部取消、Pawn 销毁等路径都不会经过 StopTimeline。
	ClearAllWindowState();

	Super::OnDestroy(bInOwnerFinished);
}
