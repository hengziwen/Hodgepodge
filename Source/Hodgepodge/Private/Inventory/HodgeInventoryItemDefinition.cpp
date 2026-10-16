// Copyright Epic Games, Inc. All Rights Reserved.
#include "Inventory/HodgeInventoryItemDefinition.h"
#include "Inventory/HodgeInventoryItemInstance.h"
#include "Equipment/HodgeEquipmentDefinition.h"
#include "Equipment/HodgeWeaponInstance.h"
#include "Data/HodgeEquipmentStatProfile.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeInventoryItemDefinition)

void UHodgeInventoryFragment_SetStats::OnInstanceCreated(UHodgeInventoryItemInstance* Instance) const
{
	// 片段不改变角色、能力或其他物品。
	for (const auto& Pair : InitialStats) { Instance->InitializeStatTag(Pair.Key, Pair.Value); }
}

const UHodgeInventoryItemFragment* UHodgeInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UHodgeInventoryItemFragment> Type) const
{
	for (const UHodgeInventoryItemFragment* Fragment : Fragments) { if (Fragment && Fragment->IsA(Type)) { return Fragment; } }
	return nullptr;
}

bool UHodgeInventoryItemDefinition::Validate(int32 Level, FString& Error) const
{
	if (MaxStackCount < 1 || Level < 1) { Error = TEXT("物品堆叠上限或等级必须为正数"); return false; }
	TSet<UClass*> Types;
	int32 EquipmentFragments = 0, StatFragments = 0;
	for (const UHodgeInventoryItemFragment* Fragment : Fragments)
	{
		if (!Fragment || Types.Contains(Fragment->GetClass())) { Error = TEXT("物品片段为空或重复"); return false; }
		Types.Add(Fragment->GetClass());
		if (const auto* Equip = Cast<UHodgeInventoryFragment_EquippableItem>(Fragment))
		{
			const auto* Definition = Equip->EquipmentDefinition ? GetDefault<UHodgeEquipmentDefinition>(Equip->EquipmentDefinition) : nullptr;
			if (++EquipmentFragments > 1 || MaxStackCount != 1 || !Definition || Equip->EquipmentDefinition->HasAnyClassFlags(CLASS_Abstract) ||
				!Definition->InstanceType || !Definition->InstanceType->IsChildOf(UHodgeWeaponInstance::StaticClass()))
			{ Error = TEXT("单武器位要求唯一装备片段、非抽象武器定义且堆叠上限为一"); return false; }
			if (Definition->StatProfile)
			{
				TArray<FText> Errors; float Health, Damage;
				if (!Definition->StatProfile->Validate(Errors) || !Definition->StatProfile->Evaluate(Level, Health, Damage))
				{ Error = TEXT("装备等级或属性配置无效"); return false; }
			}
		}
		if (const auto* Stats = Cast<UHodgeInventoryFragment_SetStats>(Fragment))
		{
			if (++StatFragments > 1) { Error = TEXT("初始物品数值片段重复"); return false; }
			for (const auto& Pair : Stats->InitialStats)
			{ if (!Pair.Key.IsValid() || Pair.Value < 1) { Error = TEXT("初始物品 Tag 或数值无效"); return false; } }
		}
	}
	return true;
}

#if WITH_EDITOR
EDataValidationResult UHodgeInventoryItemDefinition::IsDataValid(FDataValidationContext& Context) const
{
	FString Error;
	if (!Validate(1, Error)) { Context.AddError(FText::FromString(Error)); return EDataValidationResult::Invalid; }
	return Super::IsDataValid(Context) == EDataValidationResult::Invalid ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif
