/**
 * @file HodgeAbilityTask_PlayTimeline.h
 * @brief 驱动 UHodgeAbilityTimeline 的 AbilityTask（AbilityTimeline 第一阶段）
 *
 * 职责边界（严格）：
 *   - 推进逻辑时间；
 *   - Window 进入 / 退出时授予 / 撤销 WindowTag（non-replicated loose tag），并按需施加 / 移除 WindowEffectClass；
 *   - Point 到达时向 ASC 派发 PointEventTag，并按需施加 PointEffectClass；
 *   - 时间走完时自动结束自己，并保证任何销毁路径都清理干净（tag 与 GE 两个账本都不残留）。
 * 不负责：播放 / 停止 Montage、推进连击段数、结束 Ability、决定条目含义。
 *
 * 结构：一条时间轴被展开成"时间节点"（WindowEnd / WindowBegin / PointFire），
 * 由 CollectNodes + SortNodes 统一收集排序后消费。initialization 与 Tick 共用这一套，
 * 因此"从 0 跑"与"从 StartOffset 接续"的调度顺序一致（但**不保证历史副作用等价**，见设计文档 §3.5）。
 *
 * 设计依据：Docs/Design/ability-timeline-stage1.md
 */

// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"

#include "HodgeAbilityTask_PlayTimeline.generated.h"

class UGameplayEffect;
class UHodgeAbilityTimeline;
struct FHodgeTimelineEvent;

/**
 * 时间轴的停止原因。
 *
 * 它决定要不要广播停止事件，因此由调用方显式传入，不允许由回调反推。
 */
UENUM(BlueprintType)
enum class EHodgeTimelineStopReason : uint8
{
	// 尚未声明。第一阶段 Task 自身不保存停止原因，该值留给后续"攻击段执行"使用。
	None,

	// 时间走完（ElapsedTime >= Duration），由 Task 自动产生，广播 Timeline.End。
	NaturalEnd,

	// 被外力真正抢占，广播 Interrupted。
	Interrupted,

	// Ability 取消 / Task 随 Ability 销毁。不广播任何事件。
	AbilityCancelled
};

/**
 * 把一条时间轴展开之后的"节点"。
 *
 * 枚举值顺序 = 同一时刻的执行顺序，这是设计的一部分而不是巧合：
 * Window 是左闭右开区间 [StartTime, EndTime)，因此 EndTime 时刻该窗口已经退出（先 Exit）、
 * StartTime 时刻该窗口已经生效（后 Enter），而 Point 应当看到该时刻的完整状态（最后派发）。
 *
 * 放在头文件里，是因为 CollectNodes / SortNodes 要作为成员函数被 InitializeTimeline
 * 与 AdvanceTimeline 共用 —— 那正是"统一 Scheduler"的落点。
 */
enum class EHodgeTimelineNodeKind : uint8
{
	WindowEnd = 0,
	WindowBegin = 1,
	PointFire = 2
};

struct FHodgeTimelineNode
{
	// 节点发生的时刻。恢复"起点处已生效的窗口"时取该 Window 的 StartTime，以便与正常推进同序。
	float Time = 0.f;

	EHodgeTimelineNodeKind Kind = EHodgeTimelineNodeKind::PointFire;

	// 对应 TimelineAsset->Events 的下标。
	int32 EventIndex = INDEX_NONE;
};

/**
 * 播放技能逻辑时间轴。
 */
UCLASS()
class HODGEPODGE_API UHodgeAbilityTask_PlayTimeline : public UAbilityTask
{
	GENERATED_BODY()

public:
	UHodgeAbilityTask_PlayTimeline(const FObjectInitializer& ObjectInitializer);

	/**
	 * 创建时间轴驱动。
	 *
	 * 注意：工厂只创建并赋值，不会触发 Task。C++ 调用方拿到返回值后必须自己调用
	 * ReadyForActivation()，否则 Activate() 永远不会执行（且不会有任何报错）。
	 * 蓝图侧的 latent AbilityTask 节点会自动完成这一步。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Ability|Tasks",
		meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility",
		      BlueprintInternalUseOnly="TRUE"))
	static UHodgeAbilityTask_PlayTimeline* PlayTimeline(
		UGameplayAbility* OwningAbility,
		UHodgeAbilityTimeline* Timeline,
		float StartOffset = 0.f,
		float InitialPlayRate = 1.f);

	/**
	 * 停止时间轴。必须带原因，它决定要不要广播停止事件。
	 * 语义 = 停止推进 + 清理窗口状态（tag 与 GE）+ 结束 Task。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Ability|Tasks")
	void StopTimeline(EHodgeTimelineStopReason Reason);

	// 是否已经停止推进。已销毁必然是已停止，反之不成立。
	UFUNCTION(BlueprintPure, Category="Hodge|Ability|Tasks")
	bool IsTimelineStopped() const { return bStopped; }

protected:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	// 建立起点状态：恢复起点处已生效的 Window，并消费起点时刻的节点。
	// 与 AdvanceTimeline 共用收集与排序，保证"需要恢复的窗口"与"恰好落在起点的 Point"
	// 之间的调度顺序遵循同一套规则（接续语义，不是历史重放）。
	void InitializeTimeline(float InStartOffset);

	// 消费 (PreviousTime, CurrentTime] 内的全部时间节点。
	void AdvanceTimeline(float PreviousTime, float CurrentTime);

	// 收集时间节点。bInitializing 为 true 时只收集"起点恢复"与"恰好落在起点"的节点，
	// 此时 PreviousTime 被忽略。
	void CollectNodes(float PreviousTime, float CurrentTime, bool bInitializing,
					  TArray<FHodgeTimelineNode>& OutNodes) const;

	// 节点排序：Time → NodeKind → Priority → EventIndex（EventIndex 兜底保证跨端确定）。
	void SortNodes(TArray<FHodgeTimelineNode>& Nodes) const;

	// Window 进入：先加 WindowTag，再按需施加 WindowEffectClass。
	void EnterWindow(int32 EventIndex);

	// Window 退出：先移除 GE，再减 WindowTag。
	void ExitWindow(int32 EventIndex);

	// Point：按 NetPolicy 决定本端是否派发，然后"先派发事件、再施加 GE"。
	void FirePointEvent(const FHodgeTimelineEvent& Event);

	// 派发 Task 自身产生的系统事件（Timeline.End / Interrupted），复用 FirePointEvent。
	void FireSystemEvent(const FGameplayTag& EventTag);

	// 撤销全部已授予的 WindowTag 与已施加的 GE。必须幂等，且对"改 ASC 引发的重入"安全。
	void ClearAllWindowState();

	// 施加一条条目配置的 GE，返回句柄；无法施加时返回无效句柄。
	// 名字不叫 ApplyWindowEffect：Point 也在用它。
	FActiveGameplayEffectHandle ApplyTimelineEffect(UAbilitySystemComponent* ASC,
													TSubclassOf<UGameplayEffect> EffectClass);

	// 本端是否有权威（Avatar 上的权威）。Window 的 GE 与 Point 的 GE 都靠它决定是否施加。
	bool HasAuthorityOnAvatar() const;

private:
	friend struct FHodgeTimelineTestAccess;

	UPROPERTY()
	TObjectPtr<UHodgeAbilityTimeline> TimelineAsset;

	// 逻辑时间起点（秒），用于接续段。
	float StartOffset = 0.f;

	// 逻辑播放速率。第一阶段启动后不变。
	float InitialPlayRate = 1.f;

	// 上一次推进时的世界时间，用于分段累积。
	float LastUpdateWorldTime = 0.f;

	// 从 StartOffset 起累计的逻辑时间。只增不减（时钟倒退在 TickTask 里就被拦掉）。
	float LogicalElapsed = 0.f;

	// 当前逻辑时间 = StartOffset + LogicalElapsed，并夹在 Duration 以内。单调不减。
	float ElapsedTime = 0.f;

	// tag 账本：当前处于激活状态的 Window 下标。WindowTag 由配置推出，所以"下标集合"本身就是账本。
	TArray<int32> ActiveWindowIndices;

	// GE 账本：Window 下标 → 已施加的 Infinite GE 句柄。没配 GE 的窗口不在此表里。
	TMap<int32, FActiveGameplayEffectHandle> WindowEffectHandles;

	// 是否已停止推进。StopTimeline 与 OnDestroy 都会置位，它同时是重入防护的唯一哨兵。
	bool bStopped = false;

	// 账本是否已封存。ClearAllWindowState 一开始（把两个账本 MoveTemp 走之后）就置位：
	// 从那一刻起新施加的 GE 不再入账，必须由 EnterWindow 自补偿。
	bool bCleanedUp = false;
};
