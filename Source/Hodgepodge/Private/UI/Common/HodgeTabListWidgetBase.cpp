// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Common/HodgeTabListWidgetBase.h"

#include "CommonAnimatedSwitcher.h"
#include "CommonButtonBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeTabListWidgetBase)

void UHodgeTabListWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UHodgeTabListWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	SetupTabs();
}

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

void UHodgeTabListWidgetBase::HandlePostLinkedSwitcherChanged()
{
	if (!IsDesignTime() && GetCachedWidget().IsValid())
	{
		// Don't bother making tabs if we're in the designer or haven't been constructed yet
		SetupTabs();
	}

	Super::HandlePostLinkedSwitcherChanged();
}

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

bool UHodgeTabListWidgetBase::IsFirstTabActive() const
{
	if (PreregisteredTabInfoArray.Num() > 0)
	{
		return GetActiveTab() == PreregisteredTabInfoArray[0].TabId;
	}

	return false;
}

bool UHodgeTabListWidgetBase::IsLastTabActive() const
{
	if (PreregisteredTabInfoArray.Num() > 0)
	{
		return GetActiveTab() == PreregisteredTabInfoArray.Last().TabId;
	}

	return false;
}

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

void UHodgeTabListWidgetBase::SetupTabs()
{
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
