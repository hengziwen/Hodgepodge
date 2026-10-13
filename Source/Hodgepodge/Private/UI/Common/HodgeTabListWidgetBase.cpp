/** @file HodgeTabListWidgetBase.cpp
 * @brief 实现标签页列表相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Common/HodgeTabListWidgetBase.h"

#include "CommonAnimatedSwitcher.h"
#include "CommonButtonBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeTabListWidgetBase)

/** 在控件实例初始化时建立必要的输入或页面绑定。 */
void UHodgeTabListWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

/** 在控件构建时接入界面数据和生命周期。 */
void UHodgeTabListWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	SetupTabs();
}

/** 在控件析构时解除界面监听与临时资源。 */
void UHodgeTabListWidgetBase::NativeDestruct()
{
	for (FHodgeTabDescriptor& TabInfo : PreregisteredTabInfoArray)
	{
		if (TabInfo.CreatedTabContentWidget)
		{
			TabInfo.CreatedTabContentWidget->RemoveFromParent();
			TabInfo.CreatedTabContentWidget = nullptr;
		}
	}

	Super::NativeDestruct();
}

/** 获取PreregisteredTab信息。 */
bool UHodgeTabListWidgetBase::GetPreregisteredTabInfo(const FName TabNameId, FHodgeTabDescriptor& OutTabInfo)
{
	const FHodgeTabDescriptor* const FoundTabInfo = PreregisteredTabInfoArray.FindByPredicate(
		[&](FHodgeTabDescriptor& TabInfo) -> bool
		{
			return TabInfo.TabId == TabNameId;
		});

	if (!FoundTabInfo)
	{
		return false;
	}

	OutTabInfo = *FoundTabInfo;
	return true;
}

/** 设置TabHidden状态。 */
void UHodgeTabListWidgetBase::SetTabHiddenState(FName TabNameId, bool bHidden)
{
	for (FHodgeTabDescriptor& TabInfo : PreregisteredTabInfoArray)
	{
		if (TabInfo.TabId == TabNameId)
		{
			TabInfo.bHidden = bHidden;
			break;
		}
	}
}

/** 注册DynamicTab。 */
bool UHodgeTabListWidgetBase::RegisterDynamicTab(const FHodgeTabDescriptor& TabDescriptor)
{
	// If it's hidden we just ignore it.
	if (TabDescriptor.bHidden)
	{
		return true;
	}

	PendingTabLabelInfoMap.Add(TabDescriptor.TabId, TabDescriptor);

	return RegisterTab(TabDescriptor.TabId, TabDescriptor.TabButtonType, TabDescriptor.CreatedTabContentWidget);
}

/** 处理PreLinkedSwitcher变化。 */
void UHodgeTabListWidgetBase::HandlePreLinkedSwitcherChanged()
{
	for (const FHodgeTabDescriptor& TabInfo : PreregisteredTabInfoArray)
	{
		// Remove tab content widget from linked switcher, as it is being disassociated.
		if (TabInfo.CreatedTabContentWidget)
		{
			TabInfo.CreatedTabContentWidget->RemoveFromParent();
		}
	}

	Super::HandlePreLinkedSwitcherChanged();
}

/** 处理PostLinkedSwitcher变化。 */
void UHodgeTabListWidgetBase::HandlePostLinkedSwitcherChanged()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!IsDesignTime() && GetCachedWidget().IsValid())
	{
		// Don't bother making tabs if we're in the designer or haven't been constructed yet
		SetupTabs();
	}

	Super::HandlePostLinkedSwitcherChanged();
}

/** 处理TabCreation。 */
void UHodgeTabListWidgetBase::HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton)
{
	FHodgeTabDescriptor* TabInfoPtr = nullptr;

	FHodgeTabDescriptor TabInfo;
	if (GetPreregisteredTabInfo(TabId, TabInfo))
	{
		TabInfoPtr = &TabInfo;
	}
	else
	{
		TabInfoPtr = PendingTabLabelInfoMap.Find(TabId);
	}

	if (TabButton->GetClass()->ImplementsInterface(UHodgeTabButtonInterface::StaticClass()))
	{
		if (ensureMsgf(
			TabInfoPtr,
			TEXT(
				"A tab button was created with id %s but no label info was specified. RegisterDynamicTab should be used over RegisterTab to provide label info."
			), *TabId.ToString()))
		{
			IHodgeTabButtonInterface::Execute_SetTabLabelInfo(TabButton, *TabInfoPtr);
		}
	}

	PendingTabLabelInfoMap.Remove(TabId);
}

/** 判断FirstTab活动。 */
bool UHodgeTabListWidgetBase::IsFirstTabActive() const
{
	if (PreregisteredTabInfoArray.Num() > 0)
	{
		return GetActiveTab() == PreregisteredTabInfoArray[0].TabId;
	}

	return false;
}

/** 判断上次Tab活动。 */
bool UHodgeTabListWidgetBase::IsLastTabActive() const
{
	if (PreregisteredTabInfoArray.Num() > 0)
	{
		return GetActiveTab() == PreregisteredTabInfoArray.Last().TabId;
	}

	return false;
}

/** 判断Tab可见。 */
bool UHodgeTabListWidgetBase::IsTabVisible(FName TabId)
{
	if (const UCommonButtonBase* Button = GetTabButtonBaseByID(TabId))
	{
		const ESlateVisibility TabVisibility = Button->GetVisibility();
		return (TabVisibility == ESlateVisibility::Visible
			|| TabVisibility == ESlateVisibility::HitTestInvisible
			|| TabVisibility == ESlateVisibility::SelfHitTestInvisible);
	}

	return false;
}

/** 获取可见Tab数量。 */
int32 UHodgeTabListWidgetBase::GetVisibleTabCount()
{
	int32 Result = 0;
	const int32 TabCount = GetTabCount();
	for (int32 Index = 0; Index < TabCount; Index++)
	{
		if (IsTabVisible(GetTabIdAtIndex(Index)))
		{
			Result++;
		}
	}

	return Result;
}

/** 设置upTabs。 */
void UHodgeTabListWidgetBase::SetupTabs()
{
	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	for (FHodgeTabDescriptor& TabInfo : PreregisteredTabInfoArray)
	{
		if (TabInfo.bHidden)
		{
			continue;
		}

		// If the tab content hasn't been created already, create it.
		if (!TabInfo.CreatedTabContentWidget && TabInfo.TabContentType)
		{
			TabInfo.CreatedTabContentWidget = CreateWidget<
				UCommonUserWidget>(GetOwningPlayer(), TabInfo.TabContentType);
			OnTabContentCreatedNative.Broadcast(TabInfo.TabId,
			                                    Cast<UCommonUserWidget>(TabInfo.CreatedTabContentWidget));
			OnTabContentCreated.Broadcast(TabInfo.TabId, Cast<UCommonUserWidget>(TabInfo.CreatedTabContentWidget));
		}

		if (UCommonAnimatedSwitcher* CurrentLinkedSwitcher = GetLinkedSwitcher())
		{
			// Add the tab content to the newly linked switcher.
			if (!CurrentLinkedSwitcher->HasChild(TabInfo.CreatedTabContentWidget))
			{
				CurrentLinkedSwitcher->AddChild(TabInfo.CreatedTabContentWidget);
			}
		}

		// If the tab is not already registered, register it.
		if (GetTabButtonBaseByID(TabInfo.TabId) == nullptr)
		{
			RegisterTab(TabInfo.TabId, TabInfo.TabButtonType, TabInfo.CreatedTabContentWidget);
		}
	}
}
