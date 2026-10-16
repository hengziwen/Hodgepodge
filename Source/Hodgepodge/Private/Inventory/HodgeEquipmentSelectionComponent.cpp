// Copyright Epic Games, Inc. All Rights Reserved.
#include "Inventory/HodgeEquipmentSelectionComponent.h"
#include "Inventory/HodgeInventoryManagerComponent.h"
#include "Inventory/HodgeInventoryItemDefinition.h"
#include "Inventory/HodgeInventoryItemInstance.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Character/HodgeCombatCharacter.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h"
#include "AbilitySystem/Stats/HodgeAttributeCoordinator.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Equipment/HodgeEquipmentManagerComponent.h"
#include "Equipment/HodgeEquipmentInstance.h"
#include "Data/HodgePawnData.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Misc/ScopeExit.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEquipmentSelectionComponent)

namespace HodgeSelection
{
	/** 操作失败与依赖等待显式区分。 */
	FHodgeInventoryOperation Result(EHodgeInventoryResult Code, const FString& Reason)
	{ FHodgeInventoryOperation Value; Value.Code = Code; Value.Reason = Reason; return Value; }
}
UHodgeEquipmentSelectionComponent::UHodgeEquipmentSelectionComponent(const FObjectInitializer& Initializer) : Super(Initializer)
{ SetIsReplicatedByDefault(true); }

UHodgeInventoryManagerComponent* UHodgeEquipmentSelectionComponent::Inventory() const
{ return GetOwner() ? GetOwner()->FindComponentByClass<UHodgeInventoryManagerComponent>() : nullptr; }

void UHodgeEquipmentSelectionComponent::OnRegister()
{
	Super::OnRegister();
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
void UHodgeEquipmentSelectionComponent::OnUnregister()
{ UnbindAvatar(BoundAvatar.Get()); Super::OnUnregister(); }

void UHodgeEquipmentSelectionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ThisClass, SelectedItemId, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, bApplied, COND_OwnerOnly);
}

void UHodgeEquipmentSelectionComponent::OnRep_Selection() { NotifySelectionChanged(); }
void UHodgeEquipmentSelectionComponent::NotifySelectionChanged()
{
	if (!SelectedItemId.IsValid() || (Inventory() && Inventory()->FindItemById(SelectedItemId)))
	{ OnEquipmentSelectionChanged.Broadcast(); }
}
bool UHodgeEquipmentSelectionComponent::IsSelectionApplied() const
{
	if (!Inventory() || !Inventory()->IsRegistered() || !Inventory()->IsInitialized()) { return false; }
	if (!SelectedItemId.IsValid()) { return bInitialized || (Inventory() && Inventory()->IsInitialized()); }
	if (!GetOwner()->HasAuthority()) { return bApplied && Inventory() && Inventory()->FindItemById(SelectedItemId); }
	const auto* Manager = BoundAvatar.IsValid() ? BoundAvatar->FindComponentByClass<UHodgeEquipmentManagerComponent>() : nullptr;
	return bApplied && Manager && AppliedInstance && AppliedInstance->GetEquipmentId() == SelectedItemId && Manager->IsEquipmentReady(AppliedInstance);
}

FHodgeInventoryOperation UHodgeEquipmentSelectionComponent::InitializeFromPawnData(const UHodgePawnData* Data)
{
	if (!Inventory() || !Inventory()->IsRegistered()) { return HodgeSelection::Result(EHodgeInventoryResult::Waiting, TEXT("等待库存组件")); }
	auto Result = Inventory()->InitializeFromPawnData(Data); if (!Result.Succeeded()) { return Result; }
	if (!bInitialized) { bInitialized = true; SelectedItemId = Inventory()->GetDefaultSelectedItemId(); GetOwner()->ForceNetUpdate(); }
	return {};
}

FHodgeInventoryOperation UHodgeEquipmentSelectionComponent::CheckState(bool bManual) const
{
	const auto* PS = Cast<AHodgePlayerState>(GetOwner());
	if (!PS || !PS->HasAuthority()) { return HodgeSelection::Result(EHodgeInventoryResult::NoAuthority, TEXT("装备选择只能由服务器修改")); }
	if (!IsRegistered() || bChanging) { return HodgeSelection::Result(EHodgeInventoryResult::Busy, TEXT("装备选择正在修改或已停用")); }
	const auto* ASC = PS->GetHodgeAbilitySystemComponent();
	if (!Inventory() || !Inventory()->IsRegistered() || !Inventory()->IsInitialized() || !BoundAvatar.IsValid() || BoundAvatar->IsActorBeingDestroyed() ||
		ASC->GetAvatarActor() != BoundAvatar.Get() || !BoundAvatar->FindComponentByClass<UHodgeEquipmentManagerComponent>())
	{ return HodgeSelection::Result(EHodgeInventoryResult::Waiting, TEXT("等待当前 Avatar／ASC／装备管理器")); }
	if (!bManual) { return {}; }
	const auto* Health = ASC->GetSet<UHodgeHealthSet>();
	if (!PS->AreAttributesReadyFor(BoundAvatar.Get()) || ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) || (Health && Health->GetHealth() <= 0.f))
	{ return HodgeSelection::Result(EHodgeInventoryResult::ActionBlocked, TEXT("属性未就绪或角色死亡")); }
	const auto* Combat = BoundAvatar->FindComponentByClass<UHodgeCombatComponentBase>();
	if (ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Attack) || (Combat && Combat->IsEquipmentChangeBlocked()))
	{ return HodgeSelection::Result(EHodgeInventoryResult::ActionBlocked, TEXT("战斗动作执行或等待期间不能换装")); }
	for (const auto& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.IsActive() && Spec.Ability && (Spec.Ability->IsA<UHodgeGameplayAbility_Dash>() || Spec.Ability->IsA<UHodgeGameplayAbility_Sprint>() || Spec.Ability->IsA<UHodgeGameplayAbility_HitReaction>()))
		{ return HodgeSelection::Result(EHodgeInventoryResult::ActionBlocked, TEXT("Dash／Sprint／强受击期间不能换装")); }
	}
	return {};
}

FHodgeInventoryOperation UHodgeEquipmentSelectionComponent::CanEquipItem(FGuid Id) const
{
	const auto Check = CheckState(true); if (!Check.Succeeded()) { return Check; }
	const auto* Item = Inventory()->FindItemById(Id);
	if (!Item) { return HodgeSelection::Result(EHodgeInventoryResult::NotOwned, TEXT("该物品不属于当前玩家")); }
	const auto* Definition = GetDefault<UHodgeInventoryItemDefinition>(Item->GetItemDef());
	const auto* Equip = Definition->FindFragment<UHodgeInventoryFragment_EquippableItem>();
	if (!Equip) { return HodgeSelection::Result(EHodgeInventoryResult::NotEquippable, TEXT("物品没有装备片段")); }
	FString Error; TArray<FText> Errors;
	if (!Definition->Validate(Item->GetItemLevel(), Error) || !UHodgeEquipmentManagerComponent::ValidateEquipmentForGrant(Equip->EquipmentDefinition,
		CastChecked<AHodgePlayerState>(GetOwner())->GetHodgeAbilitySystemComponent(), Item->GetItemLevel(), Errors))
	{ return HodgeSelection::Result(EHodgeInventoryResult::InvalidConfiguration, Error.IsEmpty() ? TEXT("装备授予配置无效") : Error); }
	return {};
}

bool UHodgeEquipmentSelectionComponent::RemoveApplied()
{
	auto* Pawn = BoundAvatar.Get();
	auto* Manager = Pawn ? Pawn->FindComponentByClass<UHodgeEquipmentManagerComponent>() : nullptr;
	if (AppliedInstance && Manager && Manager->GetEquipmentInstancesOfType(UHodgeEquipmentInstance::StaticClass()).Contains(AppliedInstance))
	{
		auto* Old = AppliedInstance.Get(); Manager->UnequipItem(Old);
		if (Manager->GetEquipmentInstancesOfType(UHodgeEquipmentInstance::StaticClass()).Contains(Old)) { return false; }
	}
	AppliedInstance = nullptr; bApplied = false;
	return true;
}

void UHodgeEquipmentSelectionComponent::UnbindAvatar(APawn* Pawn)
{
	if (!GetOwner()->HasAuthority() || !Pawn || BoundAvatar.Get() != Pawn) { return; }
	// 同步授予回调触发解绑时，先使操作失效，返回后再清理候选实例。
	if (bChanging) { bUnbindPending = true; return; }
	TGuardValue<bool> Guard(bChanging, true);
	if (RemoveApplied()) { BoundAvatar.Reset(); }
	else { bUnbindPending = true; }
	GetOwner()->ForceNetUpdate(); NotifySelectionChanged();
}

FHodgeInventoryOperation UHodgeEquipmentSelectionComponent::BindAvatar(APawn* Pawn)
{
	if (!GetOwner()->HasAuthority() || !bInitialized || !IsValid(Pawn)) { return HodgeSelection::Result(EHodgeInventoryResult::Waiting, TEXT("等待库存或 Avatar")); }
	if (bChanging) { return HodgeSelection::Result(EHodgeInventoryResult::Busy, TEXT("当前装备切换尚未结束")); }
	if (BoundAvatar.Get() != Pawn)
	{
		UnbindAvatar(BoundAvatar.Get());
		if (BoundAvatar.IsValid()) { return HodgeSelection::Result(EHodgeInventoryResult::Busy, TEXT("旧 Avatar 装备尚未清理")); }
		BoundAvatar = Pawn;
	}
	if (!SelectedItemId.IsValid() || IsSelectionApplied()) { return {}; }
	return ApplySelection(SelectedItemId, false);
}

FHodgeInventoryOperation UHodgeEquipmentSelectionComponent::ApplySelection(FGuid Id, bool bManual)
{
	if (!bChanging && IsRegistered() && GetOwner()->HasAuthority() && SelectedItemId == Id && IsSelectionApplied()) { return {}; }
	auto Check = bManual ? CanEquipItem(Id) : CheckState(false); if (!Check.Succeeded()) { return Check; }
	auto* Item = Inventory()->FindItemById(Id);
	const auto* Definition = Item ? GetDefault<UHodgeInventoryItemDefinition>(Item->GetItemDef()) : nullptr;
	const auto* Equip = Definition ? Definition->FindFragment<UHodgeInventoryFragment_EquippableItem>() : nullptr;
	if (!Equip) { return HodgeSelection::Result(EHodgeInventoryResult::NotEquippable, TEXT("物品不存在或不可装备")); }
	if (SelectedItemId == Id && IsSelectionApplied()) { return {}; }
	TGuardValue<bool> Guard(bChanging, true);
	ON_SCOPE_EXIT { if (bUnbindPending) { bUnbindPending = false; RemoveApplied(); BoundAvatar.Reset(); bApplied = false; GetOwner()->ForceNetUpdate(); } };
	auto* Pawn = BoundAvatar.Get();
	auto* Manager = Pawn->FindComponentByClass<UHodgeEquipmentManagerComponent>();
	const FGuid OldId = SelectedItemId;
	auto* OldItem = Inventory()->FindItemById(OldId);
	const auto* PS = CastChecked<AHodgePlayerState>(GetOwner());
	if (!RemoveApplied()) { return HodgeSelection::Result(EHodgeInventoryResult::Busy, TEXT("旧装备暂时不能撤销")); }
	if (bUnbindPending || !IsRegistered() || !Inventory()->IsRegistered() || PS->GetHodgeAbilitySystemComponent()->GetAvatarActor() != Pawn)
	{ return HodgeSelection::Result(EHodgeInventoryResult::Waiting, TEXT("卸装回调中 Avatar 已失效")); }
	AppliedInstance = Manager->EquipItemWithState(Equip->EquipmentDefinition, Id, Item->GetItemLevel(), Item);
	if (AppliedInstance && !bUnbindPending && IsRegistered() && Inventory()->IsRegistered() && Inventory()->FindItemById(Id) == Item && !Pawn->IsActorBeingDestroyed() && PS->GetHodgeAbilitySystemComponent()->GetAvatarActor() == Pawn)
	{
		SelectedItemId = Id; bApplied = true;
		GetOwner()->ForceNetUpdate(); NotifySelectionChanged();
		return bUnbindPending ? HodgeSelection::Result(EHodgeInventoryResult::Waiting, TEXT("装备通知期间 Avatar 解绑")) : FHodgeInventoryOperation();
	}
	const bool bIrreversible = Manager->DidEquipHaveIrreversibleEffects();
	RemoveApplied();
	// 只能补偿可撤销失败，不对 Instant GE 或激活回调的效果承诺回滚。
	if (!bUnbindPending && !bIrreversible && IsRegistered() && OldItem && PS->GetHodgeAbilitySystemComponent()->GetAvatarActor() == Pawn)
	{
		const auto* OldEquip = GetDefault<UHodgeInventoryItemDefinition>(OldItem->GetItemDef())->FindFragment<UHodgeInventoryFragment_EquippableItem>();
		if (OldEquip) { AppliedInstance = Manager->EquipItemWithState(OldEquip->EquipmentDefinition, OldId, OldItem->GetItemLevel(), OldItem); }
	}
	bApplied = AppliedInstance != nullptr; SelectedItemId = bApplied ? OldId : FGuid();
	GetOwner()->ForceNetUpdate(); NotifySelectionChanged();
	return HodgeSelection::Result(bApplied || !OldItem ? EHodgeInventoryResult::EquipmentFailed : EHodgeInventoryResult::RecoveryFailed, Manager->GetLastEquipError());
}

FHodgeInventoryOperation UHodgeEquipmentSelectionComponent::RequestEquipItem(FGuid Id) { return ApplySelection(Id, true); }
FHodgeInventoryOperation UHodgeEquipmentSelectionComponent::RequestUnequip()
{
	// 无 Avatar 时仍可显式清空保留选择，不报告已经存在实际装备。
	if (!GetOwner()->HasAuthority()) { return HodgeSelection::Result(EHodgeInventoryResult::NoAuthority, TEXT("只允许服务器卸装")); }
	if (!IsRegistered() || bChanging) { return HodgeSelection::Result(EHodgeInventoryResult::Busy, TEXT("装备正在修改")); }
	if (BoundAvatar.IsValid()) { const auto Check = CheckState(true); if (!Check.Succeeded()) { return Check; } }
	TGuardValue<bool> Guard(bChanging, true);
	if (!RemoveApplied()) { return HodgeSelection::Result(EHodgeInventoryResult::Busy, TEXT("装备仍在执行撤销")); }
	SelectedItemId.Invalidate(); GetOwner()->ForceNetUpdate(); NotifySelectionChanged(); return {};
}
