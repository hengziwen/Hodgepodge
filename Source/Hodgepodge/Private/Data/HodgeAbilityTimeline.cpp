/**
 * @file HodgeAbilityTimeline.cpp
 * @brief UHodgeAbilityTimeline 类的实现
 *
 * 只含编辑期校验与列表整理。运行时的时间推进在 UHodgeAbilityTask_PlayTimeline 里，
 * 本类不含任何业务行为。
 *
 * 校验的原则：**发现问题，不替作者猜意图**。
 * 因此这里只报错 / 提示，绝不自动交换 StartTime 与 EndTime、也绝不自动钳制越界值。
 */

// 111屎山代码来袭

#include "Data/HodgeAbilityTimeline.h"

#include "GameplayEffect.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAbilityTimeline)

#define LOCTEXT_NAMESPACE "HodgeAbilityTimeline"

#if WITH_EDITOR

namespace
{
	// 读取 GE 类的默认时长策略。类无效时返回 false，由调用方决定"无法判定"怎么处理。
	bool TryGetEffectDurationPolicy(const TSubclassOf<UGameplayEffect>& EffectClass,
									EGameplayEffectDurationType& OutDurationPolicy)
	{
		const UGameplayEffect* EffectCDO = EffectClass ? EffectClass->GetDefaultObject<UGameplayEffect>() : nullptr;
		if (!EffectCDO)
		{
			return false;
		}

		OutDurationPolicy = EffectCDO->DurationPolicy;
		return true;
	}
}

// 编辑器中校验 Timeline 数据是否合法。
EDataValidationResult UHodgeAbilityTimeline::IsDataValid(FDataValidationContext& Context) const
{
	// 先执行父类校验，再合并当前类的结果。
	EDataValidationResult Result = CombineDataValidationResults(
		Super::IsDataValid(Context),
		EDataValidationResult::Valid
	);

	// 逻辑时长必须为正，否则整个时间轴无法推进。
	if (Duration <= 0.f)
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(FText::Format(
			LOCTEXT("DurationIsNotPositive", "Duration 必须大于 0（当前 {0}）"),
			FText::AsNumber(Duration)
		));
	}

	// EventID 唯一性。记录首次出现的下标，报错时能把冲突双方都点出来。
	TMap<FName, int32> EventIdToIndex;

	// ---------------- 逐条校验 ----------------
	for (int32 EventIndex = 0; EventIndex < Events.Num(); ++EventIndex)
	{
		const FHodgeTimelineEvent& Event = Events[EventIndex];
		const FText IndexText = FText::AsNumber(EventIndex);

		// ---- 与 Kind 无关的字段 ----

		if (Event.EventID.IsNone())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(
				LOCTEXT("EventIDIsEmpty", "Events[{0}] 的 EventID 为空（它同时是列表显示名）"),
				IndexText
			));
		}
		else if (const int32* ExistingIndex = EventIdToIndex.Find(Event.EventID))
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(
				LOCTEXT("EventIDIsDuplicated", "Events[{0}] 的 EventID \"{1}\" 与 Events[{2}] 重复"),
				IndexText,
				FText::FromName(Event.EventID),
				FText::AsNumber(*ExistingIndex)
			));
		}
		else
		{
			EventIdToIndex.Add(Event.EventID, EventIndex);
		}

		// 推进被 Min(StartOffset + LogicalElapsed, Duration) 截断，
		// 因此越界的条目永远不可能被消费 —— 这是不可达配置，不是"配得不太合理"。
		if (Event.StartTime < 0.f || Event.StartTime > Duration)
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(
				LOCTEXT("StartTimeOutOfRange", "Events[{0}] 的 StartTime 必须在 [0, Duration] 内"),
				IndexText
			));
		}

		// ---- 按 Kind 分流 ----
		// 隐藏字段（例如 Kind == Point 时的 WindowTag）可能残留切换前填过的值，一律不校验，
		// 否则只会误导。
		if (Event.Kind == EHodgeTimelineEventKind::Window)
		{
			if (Event.EndTime <= Event.StartTime)
			{
				Result = EDataValidationResult::Invalid;
				Context.AddError(FText::Format(
					LOCTEXT("WindowRangeIsReversed", "Events[{0}] 是 Window，StartTime 必须小于 EndTime"),
					IndexText
				));
			}

			// 终点超过 Duration 的区间永远不会正常退出。
			if (Event.EndTime > Duration)
			{
				Result = EDataValidationResult::Invalid;
				Context.AddError(FText::Format(
					LOCTEXT("WindowEndTimeExceedsDuration", "Events[{0}] 是 Window，EndTime 超过了 Duration（该区间永远无法正常退出）"),
					IndexText
				));
			}

			if (!Event.WindowTag.IsValid())
			{
				Result = EDataValidationResult::Invalid;
				Context.AddError(FText::Format(
					LOCTEXT("WindowTagIsInvalid", "Events[{0}] 是 Window，WindowTag 为空"),
					IndexText
				));
			}

			// Window 靠 ExitWindow 撤销 GE，所以只有 Infinite 才能覆盖整个区间。
			// 若配成 HasDuration 且短于窗口，GE 会在窗口中途自行到期 —— 那段时间的状态空洞
			// 直接违反 Window 的数据语义，因此这里是 Error 而不是 Warning。
			if (Event.WindowEffectClass)
			{
				EGameplayEffectDurationType DurationPolicy = EGameplayEffectDurationType::HasDuration;
				if (TryGetEffectDurationPolicy(Event.WindowEffectClass, DurationPolicy)
					&& DurationPolicy != EGameplayEffectDurationType::Infinite)
				{
					Result = EDataValidationResult::Invalid;
					Context.AddError(FText::Format(
						LOCTEXT("WindowEffectMustBeInfinite", "Events[{0}] 是 Window，WindowEffectClass 必须是 Infinite（否则 GE 会在窗口结束前自行到期）"),
						IndexText
					));
				}
			}
		}
		else
		{
			if (!Event.PointEventTag.IsValid())
			{
				Result = EDataValidationResult::Invalid;
				Context.AddError(FText::Format(
					LOCTEXT("PointEventTagIsInvalid", "Events[{0}] 是 Point，PointEventTag 为空"),
					IndexText
				));
			}

			if (Event.PointEffectClass)
			{
				// Point 只负责 Apply 这一次，此后生命周期不归 Timeline 管：
				// Infinite 会留下一个没人回收的永久状态，所以禁止。
				EGameplayEffectDurationType DurationPolicy = EGameplayEffectDurationType::HasDuration;
				if (TryGetEffectDurationPolicy(Event.PointEffectClass, DurationPolicy)
					&& DurationPolicy == EGameplayEffectDurationType::Infinite)
				{
					Result = EDataValidationResult::Invalid;
					Context.AddError(FText::Format(
						LOCTEXT("PointEffectMustNotBeInfinite", "Events[{0}] 是 Point，PointEffectClass 不能是 Infinite（Timeline 不会再回收它）"),
						IndexText
					));
				}

				// LocallyControlledOnly + GE 在多人下没有一致语义：
				// 远程玩家的 Owning Client 事件会执行但 GE 不施加（无权威），
				// 服务器则因为 bShouldFire 为假直接返回，也不施加，
				// 而 listen server 主机自己控制的角色两者同时为真 → GE 会施加。
				// 结果就是 Host 有、Remote 没有。
				if (Event.NetPolicy == EHodgeTimelineEventNetPolicy::LocallyControlledOnly)
				{
					Result = EDataValidationResult::Invalid;
					Context.AddError(FText::Format(
						LOCTEXT("PointEffectWithLocallyControlledOnly", "Events[{0}] 配了 PointEffectClass，NetPolicy 就不能是 LocallyControlledOnly（远程客户端上这个 GE 永远不会施加；请改用 AuthorityOnly 或 LocalAndAuthority）"),
						IndexText
					));
				}
			}
		}
	}

	// ---------------- 条目之间的组合校验 ----------------
	// 只比较同为 Window 的条目；非 Window 一律跳过。
	for (int32 LeftIndex = 0; LeftIndex < Events.Num(); ++LeftIndex)
	{
		const FHodgeTimelineEvent& Left = Events[LeftIndex];
		if (Left.Kind != EHodgeTimelineEventKind::Window)
		{
			continue;
		}

		for (int32 RightIndex = LeftIndex + 1; RightIndex < Events.Num(); ++RightIndex)
		{
			const FHodgeTimelineEvent& Right = Events[RightIndex];
			if (Right.Kind != EHodgeTimelineEventKind::Window)
			{
				continue;
			}

			const bool bSameTag =
				Left.WindowTag.IsValid() && Left.WindowTag == Right.WindowTag;

			// 1) 同一个 WindowTag 的区间不允许重叠：标签按计数维护，重叠会让计数语义含混。
			if (bSameTag)
			{
				const bool bOverlapped =
					Left.StartTime < Right.EndTime && Right.StartTime < Left.EndTime;

				if (bOverlapped)
				{
					Result = EDataValidationResult::Invalid;
					Context.AddError(FText::Format(
						LOCTEXT("WindowTagOverlapped", "Events[{0}] 与 Events[{1}] 使用了同一个 WindowTag 且区间重叠"),
						FText::AsNumber(LeftIndex),
						FText::AsNumber(RightIndex)
					));
				}
			}

			// 2) 首尾相接：边界时刻会先 End 再 Begin，于是标签走 1 → 0 → 1，
			//    订阅方会真的收到一次 Removed / Added；相同 GE 相邻还会重新触发 Cue / OnActive。
			//    若本意是一段连续区间，应该合并成一条 —— 但作者也可能故意要这个边界跳变，所以只提示。
			const bool bAdjacent =
				FMath::IsNearlyEqual(Left.EndTime, Right.StartTime) ||
				FMath::IsNearlyEqual(Right.EndTime, Left.StartTime);

			if (bAdjacent)
			{
				if (bSameTag)
				{
					Context.AddWarning(FText::Format(
						LOCTEXT("WindowTagIsAdjacent", "Events[{0}] 与 Events[{1}] 的 WindowTag 相同且首尾相接：边界处该标签会经历一次移除再添加。若本意是一段连续区间，请合并为一条 Window"),
						FText::AsNumber(LeftIndex),
						FText::AsNumber(RightIndex)
					));
				}
				else if (Left.WindowEffectClass && Left.WindowEffectClass == Right.WindowEffectClass)
				{
					Context.AddWarning(FText::Format(
						LOCTEXT("WindowEffectIsAdjacent", "Events[{0}] 与 Events[{1}] 的 WindowEffectClass 相同且首尾相接：边界处会先移除再重新施加，GameplayCue / OnActive 会被重新触发"),
						FText::AsNumber(LeftIndex),
						FText::AsNumber(RightIndex)
					));
				}
			}
		}
	}

	// 同一时刻、同一 Priority 的多条 Point：执行顺序只能靠数组下标决定，提示作者显式区分。
	for (int32 LeftIndex = 0; LeftIndex < Events.Num(); ++LeftIndex)
	{
		const FHodgeTimelineEvent& Left = Events[LeftIndex];
		if (Left.Kind != EHodgeTimelineEventKind::Point)
		{
			continue;
		}

		for (int32 RightIndex = LeftIndex + 1; RightIndex < Events.Num(); ++RightIndex)
		{
			const FHodgeTimelineEvent& Right = Events[RightIndex];
			if (Right.Kind != EHodgeTimelineEventKind::Point)
			{
				continue;
			}

			if (FMath::IsNearlyEqual(Left.StartTime, Right.StartTime)
				&& Left.Priority == Right.Priority)
			{
				Context.AddWarning(FText::Format(
					LOCTEXT("PointsShareTimeAndPriority", "Events[{0}] 与 Events[{1}] 的 StartTime 和 Priority 完全相同：执行顺序只能由数组下标决定，建议显式区分 Priority"),
					FText::AsNumber(LeftIndex),
					FText::AsNumber(RightIndex)
				));
			}
		}
	}

	// 运行期已不依赖数组有序，但升序更便于人工核对。
	// 注意：FDataValidationContext 只有 Error / Warning 两级（UE 5.5 没有 Info），因此这里用 Warning 表达提示。
	bool bSortedByStartTime = true;
	for (int32 EventIndex = 1; EventIndex < Events.Num(); ++EventIndex)
	{
		if (Events[EventIndex].StartTime < Events[EventIndex - 1].StartTime)
		{
			bSortedByStartTime = false;
			break;
		}
	}

	if (!bSortedByStartTime)
	{
		Context.AddWarning(LOCTEXT("EventsNotSortedByStartTime", "Events 未按 StartTime 升序排列，建议重新编辑一次属性以触发自动排序"));
	}

	return Result;
}

// 属性变化后只做一件事：把 Events 按 StartTime 稳定排序。
// 刻意不做"自动交换 StartTime / EndTime""自动把 EndTime 钳到 Duration"这类纠错 ——
// 那会和 IsDataValid 把这些情况定为 Error 自相矛盾，也会把配置错误悄悄改掉。
void UHodgeAbilityTimeline::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 必须用稳定排序：Index 是运行期节点排序的最终兜底键，
	// 同刻、同 Priority 的两条条目谁在数组里靠前谁先执行 —— 作者在列表里的先后
	// 就是这个兜底顺序的表达方式，整理列表时不能把它换序（普通 Sort 是不稳定的）。
	Events.StableSort([](const FHodgeTimelineEvent& Left, const FHodgeTimelineEvent& Right)
	{
		return Left.StartTime < Right.StartTime;
	});
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
