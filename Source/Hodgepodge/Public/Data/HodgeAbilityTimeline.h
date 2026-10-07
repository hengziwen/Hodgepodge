/**
 * @file HodgeAbilityTimeline.h
 * @brief 技能逻辑时间轴数据资产（AbilityTimeline 第一阶段）
 *
 * 只描述"什么时候发生什么"，不包含任何业务判断：
 * 区间（Window）负责驱动 WindowTag 与 WindowEffectClass，
 * 时刻（Point）负责派发 PointEventTag 并按需施加 PointEffectClass。
 *
 * 一条时间轴只有一张 Events 列表：作者在一处配完，运行期由统一 Scheduler
 * （CollectNodes → SortNodes → EnterWindow / ExitWindow / FirePointEvent）展开后消费。
 * 与动画资产的两类 Notify 一一对应：Window ← AnimNotifyState，Point ← AnimNotify。
 *
 * 设计依据：Docs/Design/ability-timeline-stage1.md
 */

// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"

#include "HodgeAbilityTimeline.generated.h"

class UGameplayEffect;

/**
 * Timeline 条目的本端执行策略。
 *
 * 三态直接决定条目走哪条执行通道，因此必须在派发时真实分派，不能声明了却当纯分类使用。
 * 只对 Point 有效：Window 的 tag 各端各自维护、GE 固定权威端，都不受它控制。
 */
UENUM(BlueprintType)
enum class EHodgeTimelineEventNetPolicy : uint8
{
	// 服务器与本地控制端各自执行。用于两端都需要的事件。
	LocalAndAuthority,

	// 仅服务器执行。用于命中判定、施加伤害等游戏事实。
	AuthorityOnly,

	// 仅本地控制端执行。用于相机抖动、本地 UI、本地音效等表现。
	// 注意：与 PointEffectClass 组合是非法的 —— 远程客户端的 GE 永远不会被施加（见 IsDataValid）。
	LocallyControlledOnly
};

/**
 * 条目类型。
 *
 * 与动画资产的两类 Notify 一一对应，这就是本层存在的意义：把逻辑从动画里搬出来。
 */
UENUM(BlueprintType)
enum class EHodgeTimelineEventKind : uint8
{
	// 时间区间 [StartTime, EndTime)，对应 AnimNotifyState：区间内持续生效。
	Window,

	// 时间点，对应 AnimNotify：只在 StartTime 触发一次。
	Point
};

/**
 * 时间轴上的一个条目（Timeline Entry）。
 *
 * 所有条目放在同一个数组里（统一 Authoring List / 统一身份 / 统一校验 / 统一 Scheduler 输入），
 * 但**不为"字段统一"而牺牲语义**：两种 Kind 的字段用 EditConditionHides 互相隐藏。
 *
 * 为什么 Tag 与效果类都要按 Kind 拆开：
 *   Window 的 tag 走 AddLooseGameplayTag / RemoveLooseGameplayTag，是"状态"；
 *   Point  的 tag 走 HandleGameplayEvent，是"消息"。
 *   两者的运行期语义、Tag 命名空间、GE 生命周期契约都不同，共用一个属性只会把差异藏起来。
 */
USTRUCT(BlueprintType)
struct FHodgeTimelineEvent
{
	GENERATED_BODY()

	// 条目类型，决定哪些字段生效。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EHodgeTimelineEventKind Kind = EHodgeTimelineEventKind::Window;

	// 资产内唯一标识。
	// 只用于：编辑器显示与查找、日志、Trace、跨端断言。
	// 禁止在 Gameplay 逻辑里按它分支 —— 要判断状态看 WindowTag，要判断消息看 PointEventTag。
	// 它同时是列表的显示名（TitleProperty），所以必须起得可读：ComboWindow / HitCheck / SuperArmor。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName EventID;

	// 区间起点（秒）；Point 用它表示触发时刻。
	// 条目起点，单位动画源秒；改变播放倍率后不用改写这个时间。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.0, UIMin=0.0, Units="s"))
	float StartTime = 0.f;

	// 区间终点（秒），开区间：EndTime 时刻本条已经退出。仅 Window 有效。
	// 窗口结束的动画源时间，必须晚于起点；0 不代表自动延续到动作末尾。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		meta=(ClampMin=0.0, UIMin=0.0, Units="s",
			  EditCondition="Kind==EHodgeTimelineEventKind::Window", EditConditionHides))
	float EndTime = 0.f;

	// 同一时刻的执行次序，数值小的先执行。
	// 只在"同一时刻、同一类节点"之间比较，不覆盖 WindowEnd → WindowBegin → Point 的固有顺序。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Priority = 0;

	// ==================== Window 专用：区间内持续生效的"状态" ====================

	// 区间内自动 Add / Remove 的 loose tag。
	// Categories 只是编辑器 Tag Picker 的层级过滤，不参与校验、不影响运行期。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		meta=(Categories="Status",
			  EditCondition="Kind==EHodgeTimelineEventKind::Window", EditConditionHides))
	FGameplayTag WindowTag;

	// 区间内施加、退出时移除的 GE，必须是 Infinite 且不堆叠，以保证句柄独占。
	// 只在权威端施加并依赖 GE 复制。留空表示这段区间只驱动 WindowTag。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		meta=(EditCondition="Kind==EHodgeTimelineEventKind::Window", EditConditionHides))
	TSubclassOf<UGameplayEffect> WindowEffectClass;

	// ==================== Point 专用：某一刻派发的"消息" ====================

	// 随 GameplayEvent 广播的事件标签。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		meta=(Categories="GameplayEvent",
			  EditCondition="Kind==EHodgeTimelineEventKind::Point", EditConditionHides))
	FGameplayTag PointEventTag;

	// 时刻施加一次的 GE。只在拥有权威时施加；**Timeline 此后不再追踪它的生命周期**。
	// 允许 Instant / HasDuration，禁止 Infinite（见 IsDataValid）。留空表示这个时刻只派发 PointEventTag。
	//
	// 固定顺序（写死在实现里，不依赖调用方）：先派发 PointEventTag，再施加本 GE。
	// 因此事件的处理者若结束了 Ability / 停止了 Timeline，本 GE 就不会被施加。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		meta=(EditCondition="Kind==EHodgeTimelineEventKind::Point", EditConditionHides))
	TSubclassOf<UGameplayEffect> PointEffectClass;

	// 本条目在哪些端派发（见 EHodgeTimelineEventNetPolicy）。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		meta=(EditCondition="Kind==EHodgeTimelineEventKind::Point", EditConditionHides))
	EHodgeTimelineEventNetPolicy NetPolicy = EHodgeTimelineEventNetPolicy::LocalAndAuthority;
};

/**
 * 技能逻辑时间轴。
 *
 * 蒙太奇模式以调用方的 Montage 为时长和时钟来源；独立模式使用 Duration。
 *
 * 本阶段不提供"查询某时刻处于哪些窗口"的 API：它唯一的消费者曾是初始化，
 * 而初始化现在改走与 Tick 共用的节点收集；真要判断当前状态，直接查 ASC 上的 loose tag 更准。
 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeAbilityTimeline : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Definition 使用蒙太奇时长；独立 Timeline 保留手动时长，兼容已有资产。
	// Definition 必须开启，时长来自实际蒙太奇；独立 Timeline Task 可以关闭并使用 Duration。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bUseMontageDuration = false;

	// 仅用于独立计时；蒙太奇模式忽略此值，不需要同步动画长度。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.01, Units="s", EditCondition="!bUseMontageDuration", EditConditionHides))
	float Duration = 1.0f;

	// 条目列表（Window 与 Point 混排）。
	// 保存后按 StartTime 稳定排序；TitleProperty 直接指向 EventID。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty=EventID))
	TArray<FHodgeTimelineEvent> Events;

	// 蒙太奇模式必须传入动画长度；独立模式默认使用手动时长。
	bool ValidateForPlayback(TArray<FText>& OutErrors, float DurationOverride = -1.f) const;

#if WITH_EDITOR
	//~Begin UObject 接口重写
	// 编辑器数据校验：时长、按 Kind 分流的字段合法性、EventID 唯一性、同标签窗口重叠、GE 时长契约等。
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

	// 属性在编辑器中改变时调用：只把 Events 按 StartTime 稳定排序，不修改任何配置语义。
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	//~End UObject 接口重写
#endif

private:
	bool ValidateEntries(TArray<FText>& OutErrors, float EffectiveDuration, bool bCheckDuration) const;
};
