// Copyright Epic Games, Inc. All Rights Reserved.

// UHodgeTaggedWidget 类定义。
#include "UI/HodgeTaggedWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeTaggedWidget)

//@TODO: The other TODOs in this file are all related to tag-based showing/hiding of widgets, see UE-142237
// TODO：当前文件中的其他 TODO 都与“根据 GameplayTag 自动显示 / 隐藏 Widget”有关。
// 目前这套 Tag 驱动可见性的机制还没有真正接入完成。

// 构造支持 GameplayTag 控制可见性的 Widget。
UHodgeTaggedWidget::UHodgeTaggedWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// Widget 完成构造并进入实际运行状态时调用。
void UHodgeTaggedWidget::NativeConstruct()
{
	// 先执行 UCommonUserWidget 的构造逻辑。
	Super::NativeConstruct();

	// Design Time 指 UMG 编辑器中的设计预览状态。
	// 只有真正运行游戏时才需要监听玩家 GameplayTag。
	if (!IsDesignTime())
	{
		// Listen for tag changes on our hidden tags
		// 设计目标：监听 HiddenByTags 中相关 GameplayTag 的变化，
		// 当玩家获得或移除这些 Tag 时自动重新计算 Widget 的可见性。

		//@TODO: That thing I said
		// TODO：这里目前还没有真正注册 GameplayTag 变化监听。

		// Set our initial visibility value (checking the tags, etc...)
		// 使用当前 Visibility 主动执行一次 SetVisibility，
		// 用于初始化 bWantsToBeVisible、ShownVisibility、HiddenVisibility，
		// 并按照当前 GameplayTag 状态计算最终应该使用的 Visibility。
		SetVisibility(GetVisibility());
	}
}

// Widget 被销毁 / 移出实际运行状态时调用。
void UHodgeTaggedWidget::NativeDestruct()
{
	// 只有运行时才存在需要解除的 GameplayTag 监听。
	if (!IsDesignTime())
	{
		//@TODO: Stop listening for tag changes
		// TODO：未来在 NativeConstruct 注册 GameplayTag 监听后，
		// 应该在这里解除对应监听，避免 Widget 销毁后仍然收到 Tag 变化回调。
	}

	// 执行 UCommonUserWidget 的销毁逻辑。
	Super::NativeDestruct();
}

// 设置当前 Widget 的可见性。
// 除了保存调用者希望设置的 Visibility，
// 还会结合 HiddenByTags 的状态计算最终实际应用到 Widget 上的 Visibility。
void UHodgeTaggedWidget::SetVisibility(ESlateVisibility InVisibility)
{
#if WITH_EDITORONLY_DATA
	// UMG 编辑器设计预览期间不执行 GameplayTag 驱动的可见性逻辑。
	if (IsDesignTime())
	{
		// 直接按照编辑器设置的 Visibility 更新 Widget，
		// 保证设计器中的预览行为不会受到运行时 Tag 逻辑影响。
		Super::SetVisibility(InVisibility);

		// Design Time 下处理到这里结束。
		return;
	}
#endif

	// Remember what the caller requested; even if we're currently being
	// suppressed by a tag we should respect this call when we're done
	// 记录调用者真正希望 Widget 处于什么可见状态。
	// 即使当前 Widget 因 GameplayTag 被临时隐藏，
	// 当对应 Tag 消失后也应该恢复到调用者原本请求的状态。
	bWantsToBeVisible = ConvertSerializedVisibilityToRuntime(InVisibility).IsVisible();

	// 如果调用者希望 Widget 可见。
	if (bWantsToBeVisible)
	{
		// 保存调用者请求的可见状态。
		// 例如可能是 Visible、HitTestInvisible、SelfHitTestInvisible 等。
		ShownVisibility = InVisibility;
	}
	else
	{
		// 如果调用者希望 Widget 隐藏，
		// 保存对应的隐藏状态，例如 Hidden 或 Collapsed。
		HiddenVisibility = InVisibility;
	}

	// 查询所属玩家当前是否拥有 HiddenByTags 中任意 GameplayTag。
	// 当前实现尚未接入实际 Tag 来源，因此这里暂时始终为 false。
	const bool bHasHiddenTags = false; //@TODO: Foo->HasAnyTags(HiddenByTags);

	// Actually apply the visibility
	// 根据“Widget 自己是否希望显示”和“GameplayTag 是否要求隐藏”
	// 共同决定最终实际应用的 Visibility。
	const ESlateVisibility DesiredVisibility = (bWantsToBeVisible && !bHasHiddenTags)
		                                           ? ShownVisibility
		                                           : HiddenVisibility;

	// 只有最终 Visibility 与当前状态不同才真正修改，
	// 避免没有必要的重复 SetVisibility。
	if (GetVisibility() != DesiredVisibility)
	{
		// 这里调用 Super，避免再次进入当前重写的 SetVisibility，
		// 直接把最终计算结果应用到底层 Widget。
		Super::SetVisibility(DesiredVisibility);
	}
}

// 所监听的 GameplayTag 状态发生变化时调用。
// 设计目标是重新检查 HiddenByTags，并刷新 Widget 的最终可见性。
void UHodgeTaggedWidget::OnWatchedTagsChanged()
{
	// 查询玩家当前是否拥有 HiddenByTags 中任意 GameplayTag。
	// 当前实际 Tag 查询尚未实现，因此这里暂时始终返回 false。
	const bool bHasHiddenTags = false; //@TODO: Foo->HasAnyTags(HiddenByTags);

	// Actually apply the visibility
	// 根据 Widget 自身显示意愿以及 GameplayTag 状态，
	// 重新计算当前真正应该使用的 Visibility。
	const ESlateVisibility DesiredVisibility = (bWantsToBeVisible && !bHasHiddenTags)
		                                           ? ShownVisibility
		                                           : HiddenVisibility;

	// 只有可见性真正发生变化时才更新 Widget。
	if (GetVisibility() != DesiredVisibility)
	{
		// 直接调用父类实现应用最终结果，
		// 避免重新进入当前类的 SetVisibility 并修改 bWantsToBeVisible。
		Super::SetVisibility(DesiredVisibility);
	}
}
