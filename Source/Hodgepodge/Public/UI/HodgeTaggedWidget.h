// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// CommonUI 提供的基础 UserWidget。
// 相比普通 UUserWidget，可以接入 CommonUI 的 UI 体系。
#include "CommonUserWidget.h"

// GameplayTag 和 GameplayTagContainer 定义。
// 当前 Widget 会根据玩家身上的 GameplayTag 自动决定是否隐藏。
#include "GameplayTagContainer.h"

#include "HodgeTaggedWidget.generated.h"

class UObject;

/**
 * An widget in a layout that has been tagged (can be hidden or shown via tags on the owning player)
 *
 * Hodge 项目中支持 GameplayTag 控制可见性的基础 Widget。
 *
 * 主要用于 HUD 中需要根据玩家当前状态自动显示 / 隐藏的 UI 控件。
 *
 * Widget 可以配置一组 HiddenByTags，
 * 当所属玩家拥有其中任意 GameplayTag 时，
 * Widget 会自动切换到 HiddenVisibility；
 * 当这些 Tag 不再存在时，则恢复到 ShownVisibility。
 */
UCLASS(Abstract, Blueprintable)
class UHodgeTaggedWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	// 构造支持 GameplayTag 可见性控制的 Widget。
	UHodgeTaggedWidget(const FObjectInitializer& ObjectInitializer);

	//~UWidget interface

	// 设置当前 Widget 希望使用的可见性状态。
	// 除了记录外部代码希望设置的 Visibility，
	// 最终显示状态还会受到 HiddenByTags 中 GameplayTag 的影响。
	virtual void SetVisibility(ESlateVisibility InVisibility) override;

	//~End of UWidget interface

	//~UUserWidget interface

	// Widget 构造并进入实际使用状态时调用。
	// 通常会在这里开始监听玩家身上与 HiddenByTags 相关的 GameplayTag 变化。
	virtual void NativeConstruct() override;

	// Widget 被销毁 / 移出使用状态时调用。
	// 通常会在这里解除之前注册的 GameplayTag 监听，避免残留无效回调。
	virtual void NativeDestruct() override;

	//~End of UUserWidget interface

protected:
	/** If the owning player has any of these tags, this widget will be hidden (using HiddenVisibility) */
	// 控制当前 Widget 隐藏的一组 GameplayTag。
	// 如果所属玩家拥有其中任意 Tag，
	// 当前 Widget 将使用 HiddenVisibility 作为最终可见性。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD")
	FGameplayTagContainer HiddenByTags;

	/** The visibility to use when this widget is shown (not hidden by gameplay tags). */
	// 当前 Widget 没有被 GameplayTag 隐藏时使用的可见性状态。
	// 默认使用 Visible。
	UPROPERTY(EditAnywhere, Category = "HUD")
	ESlateVisibility ShownVisibility = ESlateVisibility::Visible;

	/** The visibility to use when this widget is hidden by gameplay tags. */
	// 当前 Widget 因 HiddenByTags 中的 GameplayTag 而被隐藏时使用的可见性状态。
	// 默认使用 Collapsed，使 Widget 不显示并且不占用布局空间。
	UPROPERTY(EditAnywhere, Category = "HUD")
	ESlateVisibility HiddenVisibility = ESlateVisibility::Collapsed;

	/** Do we want to be visible (ignoring tags)? */
	// 记录当前 Widget 在“不考虑 GameplayTag”的情况下是否希望保持可见。
	// 用于区分：
	// 1. Widget 本身就希望隐藏；
	// 2. Widget 本身希望显示，但因为 GameplayTag 被临时隐藏。
	bool bWantsToBeVisible = true;

private:
	// 当当前 Widget 所监听的 GameplayTag 状态发生变化时调用。
	// 根据 HiddenByTags 重新计算并刷新 Widget 的最终 Visibility。
	void OnWatchedTagsChanged();
};
