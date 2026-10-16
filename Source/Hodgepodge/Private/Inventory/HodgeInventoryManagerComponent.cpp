// Copyright Epic Games, Inc. All Rights Reserved.
#include "Inventory/HodgeInventoryManagerComponent.h"
#include "Inventory/HodgeInventoryItemDefinition.h"
#include "Inventory/HodgeInventoryItemInstance.h"
#include "Inventory/HodgeEquipmentSelectionComponent.h"
#include "Data/HodgePawnData.h"
#include "Equipment/HodgeEquipmentDefinition.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Character/HodgeCombatCharacter.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "UObject/StrongObjectPtr.h"
#include "TimerManager.h"
#include "Misc/ScopeExit.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeInventoryManagerComponent)

namespace HodgeInventory
{
	/** 预期失败返回诊断，不用断言或伪成功。 */
	FHodgeInventoryOperation Fail(EHodgeInventoryResult Code, const FString& Reason)
	{ FHodgeInventoryOperation Result; Result.Code = Code; Result.Reason = Reason; return Result; }
}

void FHodgeInventoryList::PostReplicatedReceive(const FFastArraySerializer::FPostReplicatedReceiveParameters& Parameters)
{
	if (Owner) { Owner->RefreshSnapshot(); }
}

UHodgeInventoryManagerComponent::UHodgeInventoryManagerComponent(const FObjectInitializer& Initializer) : Super(Initializer)
{
	SetIsReplicatedByDefault(true);
	InventoryList.Owner = this;
}

void UHodgeInventoryManagerComponent::OnRegister()
{
	Super::OnRegister();
	// 组件重新注册时恢复先前注销的子对象；ReadyForReplication 不保证再次调用。
	for (const auto& Entry : InventoryList.Entries) { RegisterItem(Entry.Instance); }
	// Experience 注入可能晚于 Pawn 初始化，下一帧补查一次而非无限轮询。
	if (GetOwner()->HasAuthority() && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			if (!IsRegistered()) { return; }
			if (const auto* PS = Cast<AHodgePlayerState>(GetOwner()))
			{ if (auto* Pawn = Cast<AHodgeCombatCharacter>(PS->GetPawn())) { Pawn->TryInitializeAttributesAndEquipment(); } }
		}));
	}
}

void UHodgeInventoryManagerComponent::OnUnregister()
{
	if (auto* Selection = GetOwner()->FindComponentByClass<UHodgeEquipmentSelectionComponent>()) { Selection->UnbindAvatar(Selection->GetBoundAvatar()); }
	for (const auto& Entry : InventoryList.Entries) { UnregisterItem(Entry.Instance); }
	Super::OnUnregister();
}

void UHodgeInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ThisClass, InventoryList, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, bInitialized, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, DefaultSelectedItemId, COND_OwnerOnly);
}

void UHodgeInventoryManagerComponent::RegisterItem(UHodgeInventoryItemInstance* Item)
{
	if (Item && IsUsingRegisteredSubObjectList() && IsReadyForReplication() && !IsReplicatedSubObjectRegistered(Item)) { AddReplicatedSubObject(Item, COND_OwnerOnly); }
}
void UHodgeInventoryManagerComponent::UnregisterItem(UHodgeInventoryItemInstance* Item)
{
	if (Item && IsUsingRegisteredSubObjectList()) { RemoveReplicatedSubObject(Item); }
}
void UHodgeInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
	for (const auto& Entry : InventoryList.Entries) { RegisterItem(Entry.Instance); }
}
bool UHodgeInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* Flags)
{
	bool Wrote = Super::ReplicateSubobjects(Channel, Bunch, Flags);
	if (Flags->bNetOwner)
	{ for (const auto& Entry : InventoryList.Entries) { if (Entry.Instance) { Wrote |= Channel->ReplicateSubobject(Entry.Instance, *Bunch, *Flags); } } }
	return Wrote;
}

FHodgeInventoryOperation UHodgeInventoryManagerComponent::CheckWrite() const
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) { return HodgeInventory::Fail(EHodgeInventoryResult::NoAuthority, TEXT("库存只能由服务器修改")); }
	if (!IsRegistered() || bMutating || bPublishing) { return HodgeInventory::Fail(EHodgeInventoryResult::Busy, TEXT("库存未注册或正在提交／通知")); }
	return {};
}

TArray<FHodgeInventoryItemView> UHodgeInventoryManagerComponent::GetItems() const
{
	TArray<FHodgeInventoryItemView> Views;
	for (const auto& Entry : InventoryList.Entries)
	{
		if (!Entry.Instance || !Entry.Instance->GetItemId().IsValid() || !Entry.Instance->GetItemDef()) { continue; }
		auto& View = Views.AddDefaulted_GetRef(); View.ItemId = Entry.Instance->GetItemId(); View.Instance = Entry.Instance; View.Count = Entry.StackCount;
	}
	return Views;
}
UHodgeInventoryItemInstance* UHodgeInventoryManagerComponent::FindItemById(FGuid Id) const
{
	if (!Id.IsValid()) { return nullptr; }
	for (const auto& Entry : InventoryList.Entries) { if (Entry.Instance && Entry.Instance->GetItemId() == Id) { return Entry.Instance; } }
	return nullptr;
}
int64 UHodgeInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<UHodgeInventoryItemDefinition> Definition) const
{
	int64 Total = 0;
	for (const auto& Entry : InventoryList.Entries) { if (Entry.Instance && Entry.Instance->GetItemDef() == Definition) { Total += Entry.StackCount; } }
	return Total;
}

FHodgeInventoryOperation UHodgeInventoryManagerComponent::BuildPlan(const TArray<FGrant>& Grants, int32 Capacity, TArray<FHodgeInventoryEntry>& Plan)
{
	if (Capacity < 1) { return HodgeInventory::Fail(EHodgeInventoryResult::InvalidConfiguration, TEXT("容量必须大于零")); }
	Plan = InventoryList.Entries;
	for (const auto& Grant : Grants)
	{
		if (Grant.Count < 1) { return HodgeInventory::Fail(EHodgeInventoryResult::InvalidQuantity, TEXT("数量必须为正整数")); }
		FString Error;
		if (!Grant.Definition || Grant.Definition->HasAnyClassFlags(CLASS_Abstract) || !GetDefault<UHodgeInventoryItemDefinition>(Grant.Definition)->Validate(Grant.Level, Error))
		{ return HodgeInventory::Fail(EHodgeInventoryResult::InvalidConfiguration, Error.IsEmpty() ? TEXT("物品定义为空或抽象") : Error); }
		const auto* Definition = GetDefault<UHodgeInventoryItemDefinition>(Grant.Definition);
		int64 Remaining = Grant.Count, Total = 0;
		for (auto& Entry : Plan)
		{
			if (Entry.Instance->GetItemDef() != Grant.Definition) { continue; }
			Total += Entry.StackCount;
			if (!Grant.ExistingId.IsValid() && Entry.Instance->GetItemLevel() == Grant.Level && Definition->MaxStackCount > 1)
			{
				const int32 Added = int32(FMath::Min<int64>(Remaining, Definition->MaxStackCount - Entry.StackCount));
				Entry.StackCount += Added; Remaining -= Added;
			}
		}
		if (Total + Grant.Count > MAX_int32) { return HodgeInventory::Fail(EHodgeInventoryResult::InvalidQuantity, TEXT("同类物品总数量溢出")); }
		const int64 Required = (Remaining + Definition->MaxStackCount - 1) / Definition->MaxStackCount;
		if (Required + Plan.Num() > Capacity) { return HodgeInventory::Fail(EHodgeInventoryResult::CapacityExceeded, TEXT("背包剩余条目不足")); }
		if (Grant.ExistingId.IsValid() && (Grant.Count != 1 || Plan.ContainsByPredicate([&](const auto& E) { return E.Instance->GetItemId() == Grant.ExistingId; })))
		{ return HodgeInventory::Fail(EHodgeInventoryResult::InvalidConfiguration, TEXT("旧装备身份冲突")); }
		while (Remaining > 0)
		{
			// 待提交条目也由反射结构引用；调用方保活整个计划，片段禁止外部副作用。
			auto* Item = NewObject<UHodgeInventoryItemInstance>(GetOwner());
			PendingItems.Add(Item);
			Item->Initialize(Grant.Definition, Grant.ExistingId.IsValid() ? Grant.ExistingId : FGuid::NewGuid(), Grant.Level);
			auto& Entry = Plan.AddDefaulted_GetRef(); Entry.Instance = Item;
			Entry.StackCount = int32(FMath::Min<int64>(Remaining, Definition->MaxStackCount)); Remaining -= Entry.StackCount;
		}
	}
	return {};
}

FHodgeInventoryOperation UHodgeInventoryManagerComponent::CommitPlan(TArray<FHodgeInventoryEntry>&& Plan)
{
	for (const auto& Old : InventoryList.Entries)
	{ if (!Plan.ContainsByPredicate([&](const auto& New) { return New.Instance == Old.Instance; })) { UnregisterItem(Old.Instance); } }
	InventoryList.Entries = MoveTemp(Plan);
	InventoryList.MarkArrayDirty();
	for (auto& Entry : InventoryList.Entries) { InventoryList.MarkItemDirty(Entry); RegisterItem(Entry.Instance); }
	// 旧拥有装备记录仅镜像完整库存，删除物品后不能由旧记录将其复活。
	if (auto* PS = Cast<AHodgePlayerState>(GetOwner()))
	{
		PS->OwnedEquipmentStates.Reset();
		for (const auto& Entry : InventoryList.Entries)
		{
			const auto* Fragment = GetDefault<UHodgeInventoryItemDefinition>(Entry.Instance->GetItemDef())->FindFragment<UHodgeInventoryFragment_EquippableItem>();
			if (!Fragment) { continue; }
			FHodgeOwnedEquipmentState State; State.InstanceId = Entry.Instance->GetItemId(); State.Level = Entry.Instance->GetItemLevel(); State.Definition = Fragment->EquipmentDefinition;
			PS->OwnedEquipmentStates.Add(State);
		}
	}
	// 先提交全部状态，再通过同一差异路径通知主机和复制客户端。
	const auto Before = LastSnapshot;
	RefreshSnapshot();
	FHodgeInventoryOperation Result;
	for (const auto& Old : Before)
	{
		const auto* New = LastSnapshot.FindByPredicate([&](const auto& V) { return V.ItemId == Old.ItemId; });
		if (!New || New->Count != Old.Count) { auto& C = Result.Changes.AddDefaulted_GetRef(); C.ItemId = Old.ItemId; C.OldCount = Old.Count; C.NewCount = New ? New->Count : 0; }
	}
	for (const auto& New : LastSnapshot)
	{ if (!Before.ContainsByPredicate([&](const auto& V) { return V.ItemId == New.ItemId; })) { auto& C = Result.Changes.AddDefaulted_GetRef(); C.ItemId = New.ItemId; C.NewCount = New.Count; } }
	GetOwner()->ForceNetUpdate();
	return Result;
}

void UHodgeInventoryManagerComponent::RefreshSnapshot()
{
	if (bPublishing || !IsRegistered()) { return; }
	const auto Current = GetItems();
	if (Current.Num() != InventoryList.Entries.Num()) { return; }
	TArray<FHodgeInventoryChange> Changes;
	for (const auto& Old : LastSnapshot)
	{
		const auto* New = Current.FindByPredicate([&](const auto& V) { return V.ItemId == Old.ItemId; });
		if (!New || New->Count != Old.Count) { auto& C = Changes.AddDefaulted_GetRef(); C.ItemId = Old.ItemId; C.OldCount = Old.Count; C.NewCount = New ? New->Count : 0; }
	}
	for (const auto& New : Current)
	{ if (!LastSnapshot.ContainsByPredicate([&](const auto& V) { return V.ItemId == New.ItemId; })) { auto& C = Changes.AddDefaulted_GetRef(); C.ItemId = New.ItemId; C.NewCount = New.Count; } }
	LastSnapshot = Current;
	TGuardValue<bool> PublishingGuard(bPublishing, true);
	if (auto* Selection = GetOwner()->FindComponentByClass<UHodgeEquipmentSelectionComponent>()) { Selection->NotifySelectionChanged(); }
	if (!Changes.IsEmpty()) { ++Revision; OnInventoryChanged.Broadcast(Changes, Revision); }
}

FHodgeInventoryOperation UHodgeInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UHodgeInventoryItemDefinition> Definition, int32 Count, int32 Level) const
{
	const auto Check = CheckWrite(); if (!Check.Succeeded()) { return Check; }
	if (!bInitialized) { return HodgeInventory::Fail(EHodgeInventoryResult::Waiting, TEXT("库存尚未初始化")); }
	// 只读容量预检不创建 UObject，也不执行片段。
	FString Error;
	if (Count < 1) { return HodgeInventory::Fail(EHodgeInventoryResult::InvalidQuantity, TEXT("数量必须为正数")); }
	if (!Definition || Definition->HasAnyClassFlags(CLASS_Abstract) || !GetDefault<UHodgeInventoryItemDefinition>(Definition)->Validate(Level, Error))
	{ return HodgeInventory::Fail(EHodgeInventoryResult::InvalidConfiguration, Error.IsEmpty() ? TEXT("无效物品定义") : Error); }
	const int32 Limit = GetDefault<UHodgeInventoryItemDefinition>(Definition)->MaxStackCount;
	int64 Remaining = Count;
	for (const auto& E : InventoryList.Entries)
	{ if (E.Instance->GetItemDef() == Definition && E.Instance->GetItemLevel() == Level) { Remaining -= FMath::Min<int64>(Remaining, Limit - E.StackCount); } }
	if (GetTotalItemCountByDefinition(Definition) + Count > MAX_int32) { return HodgeInventory::Fail(EHodgeInventoryResult::InvalidQuantity, TEXT("数量溢出")); }
	return (Remaining + Limit - 1) / Limit + InventoryList.Entries.Num() <= MaxEntries ? FHodgeInventoryOperation() : HodgeInventory::Fail(EHodgeInventoryResult::CapacityExceeded, TEXT("背包容量不足"));
}

FHodgeInventoryOperation UHodgeInventoryManagerComponent::AddItemDefinition(TSubclassOf<UHodgeInventoryItemDefinition> Definition, int32 Count, int32 Level)
{
	auto Check = CanAddItemDefinition(Definition, Count, Level); if (!Check.Succeeded()) { return Check; }
	TGuardValue<bool> Guard(bMutating, true);
	TArray<FHodgeInventoryEntry> Plan;
	ON_SCOPE_EXIT { PendingItems.Reset(); };
	Check = BuildPlan({{Definition, Count, Level, {}, false}}, MaxEntries, Plan);
	return Check.Succeeded() ? CommitPlan(MoveTemp(Plan)) : Check;
}

FHodgeInventoryOperation UHodgeInventoryManagerComponent::RemoveItemById(FGuid Id)
{
	const auto Check = CheckWrite(); if (!Check.Succeeded()) { return Check; }
	if (!FindItemById(Id)) { return HodgeInventory::Fail(EHodgeInventoryResult::NotOwned, TEXT("物品不属于该背包")); }
	TGuardValue<bool> Guard(bMutating, true);
	if (auto* Selection = GetOwner()->FindComponentByClass<UHodgeEquipmentSelectionComponent>())
	{
		if (Selection->GetSelectedItemId() == Id) { const auto Result = Selection->RequestUnequip(); if (!Result.Succeeded()) { return Result; } }
	}
	TArray<FHodgeInventoryEntry> Plan = InventoryList.Entries;
	Plan.RemoveAll([&](const auto& E) { return E.Instance->GetItemId() == Id; });
	return CommitPlan(MoveTemp(Plan));
}

FHodgeInventoryOperation UHodgeInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UHodgeInventoryItemDefinition> Definition, int32 Count)
{
	const auto Check = CheckWrite(); if (!Check.Succeeded()) { return Check; }
	if (!Definition || Count < 1) { return HodgeInventory::Fail(EHodgeInventoryResult::InvalidQuantity, TEXT("消耗定义和正数量必需")); }
	if (GetTotalItemCountByDefinition(Definition) < Count) { return HodgeInventory::Fail(EHodgeInventoryResult::InsufficientQuantity, TEXT("物品数量不足")); }
	const auto* Selection = GetOwner()->FindComponentByClass<UHodgeEquipmentSelectionComponent>();
	TArray<FHodgeInventoryEntry> Plan = InventoryList.Entries;
	int32 Remaining = Count;
	for (auto& Entry : Plan)
	{
		if (!Remaining || Entry.Instance->GetItemDef() != Definition) { continue; }
		if (Selection && Selection->GetSelectedItemId() == Entry.Instance->GetItemId())
		{ return HodgeInventory::Fail(EHodgeInventoryResult::ActionBlocked, TEXT("通用消耗不能移除已选择装备")); }
		const int32 Taken = FMath::Min(Remaining, Entry.StackCount); Entry.StackCount -= Taken; Remaining -= Taken;
	}
	Plan.RemoveAll([](const auto& E) { return E.StackCount == 0; });
	TGuardValue<bool> Guard(bMutating, true);
	return CommitPlan(MoveTemp(Plan));
}

FHodgeInventoryOperation UHodgeInventoryManagerComponent::InitializeFromPawnData(const UHodgePawnData* Data)
{
	const auto Check = CheckWrite(); if (!Check.Succeeded()) { return Check; }
	if (bInitialized) { return InitializationData == Data ? FHodgeInventoryOperation() : HodgeInventory::Fail(EHodgeInventoryResult::InvalidConfiguration, TEXT("不能用不同配置覆盖已有库存")); }
	if (!Data || !Data->bUseInventory) { return HodgeInventory::Fail(EHodgeInventoryResult::InvalidConfiguration, TEXT("PawnData 未启用库存")); }
	TGuardValue<bool> Guard(bMutating, true);
	TArray<FGrant> Grants;
	ON_SCOPE_EXIT { PendingItems.Reset(); };
	int32 SelectedDefinitions = 0;
	const auto* PS = Cast<AHodgePlayerState>(GetOwner());
	TSet<TSubclassOf<UHodgeEquipmentDefinition>> MappedEquipment;
	for (const auto& Default : Data->DefaultInventoryItems)
	{
		if (Default.bEquip && (++SelectedDefinitions > 1 || Default.Count != 1)) { return HodgeInventory::Fail(EHodgeInventoryResult::InvalidConfiguration, TEXT("只能配置一个默认武器")); }
		const auto* Definition = Default.Definition ? GetDefault<UHodgeInventoryItemDefinition>(Default.Definition) : nullptr;
		const auto* Equip = Definition ? Definition->FindFragment<UHodgeInventoryFragment_EquippableItem>() : nullptr;
		if (Default.bEquip && !Equip) { return HodgeInventory::Fail(EHodgeInventoryResult::InvalidConfiguration, TEXT("默认选择物品不可装备")); }
		int32 ExistingCount = 0;
		if (Equip)
		{
			if (MappedEquipment.Contains(Equip->EquipmentDefinition)) { return HodgeInventory::Fail(EHodgeInventoryResult::InvalidConfiguration, TEXT("装备定义的默认物品映射不唯一")); }
			MappedEquipment.Add(Equip->EquipmentDefinition);
			if (PS)
			{
				for (const auto& State : PS->GetOwnedEquipmentStates())
				{
					if (State.Definition != Equip->EquipmentDefinition) { continue; }
					if (!State.InstanceId.IsValid()) { return HodgeInventory::Fail(EHodgeInventoryResult::InvalidConfiguration, TEXT("旧装备身份无效")); }
					Grants.Add({Default.Definition, 1, State.Level, State.InstanceId, Default.bEquip && ExistingCount == 0}); ++ExistingCount;
				}
			}
		}
		if (Default.Count - ExistingCount > 0) { Grants.Add({Default.Definition, Default.Count - ExistingCount, Default.Level, {}, Default.bEquip && !ExistingCount}); }
		else if (Default.Count < 1) { return HodgeInventory::Fail(EHodgeInventoryResult::InvalidQuantity, TEXT("默认物品数量非法")); }
	}
	TArray<FHodgeInventoryEntry> Plan;
	if (PS)
	{ for (const auto& Old : PS->GetOwnedEquipmentStates())
		{ if (!MappedEquipment.Contains(Old.Definition)) { return HodgeInventory::Fail(EHodgeInventoryResult::InvalidConfiguration, TEXT("旧装备缺少显式物品映射")); } } }
	auto Result = BuildPlan(Grants, Data->InventoryMaxEntries, Plan); if (!Result.Succeeded()) { return Result; }
	for (const auto& Grant : Grants)
	{
		if (!Grant.bSelect) { continue; }
		const auto* Selected = Plan.FindByPredicate([&](const auto& E) { return E.Instance->GetItemDef() == Grant.Definition && (!Grant.ExistingId.IsValid() || E.Instance->GetItemId() == Grant.ExistingId); });
		if (Selected) { DefaultSelectedItemId = Selected->Instance->GetItemId(); }
	}
	MaxEntries = Data->InventoryMaxEntries; InitializationData = Data; bInitialized = true;
	return CommitPlan(MoveTemp(Plan));
}
