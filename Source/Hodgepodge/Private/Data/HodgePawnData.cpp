/** @file HodgePawnData.cpp
 * @brief 实现Pawn能力/输入/相机配置相关的类型、接口与现有处理流程。
 */
// 111屎山代码来袭


#include "Data/HodgePawnData.h"
#include "Data/HodgeCharacterStatProfile.h"
#include "Data/HodgeHitReactionProfile.h"
#include "Inventory/HodgeInventoryItemDefinition.h"

#if WITH_EDITOR
#include "Data/HodgeAbilityDefinition.h"
#include "Data/HodgeAbilitySet.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "Data/HodgeComboDefinition.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgePawnData)

/** 构造Pawn能力/输入/相机配置并初始化其默认状态。 */
UHodgePawnData::UHodgePawnData(const FObjectInitializer& ObjectInitializer)
{
	PawnClass = nullptr;
	InputConfig = nullptr;
	DefaultCameraMode = nullptr;
}

// 编辑器资产校验

#if WITH_EDITOR
/** 在编辑器校验当前配置并收集诊断。 */
EDataValidationResult UHodgePawnData::IsDataValid(FDataValidationContext& Context) const
{
	const auto Parent = Super::IsDataValid(Context);
	if (bUseInventory)
	{
		bool bValid = InventoryMaxEntries > 0;
		int32 Selections = 0;
		for (const auto& Item : DefaultInventoryItems)
		{
			FString Error;
			const auto* Definition = Item.Definition ? GetDefault<UHodgeInventoryItemDefinition>(Item.Definition) : nullptr;
			if (!Definition || Item.Count < 1 || !Definition->Validate(Item.Level, Error))
			{ Context.AddError(FText::FromString(Error.IsEmpty() ? TEXT("库存默认物品定义或数量非法") : Error)); bValid = false; }
			if (Item.bEquip && (++Selections > 1 || Item.Count != 1 || !Definition || !Definition->FindFragment<UHodgeInventoryFragment_EquippableItem>()))
			{ Context.AddError(FText::FromString(TEXT("库存只能指定一个可装备的默认物品"))); bValid = false; }
		}
		if (!bValid) { return EDataValidationResult::Invalid; }
	}
	if (StatProfile)
	{
		TArray<FText> Errors;
		StatProfile->Validate(Errors);
		for (const auto& Error : Errors) { Context.AddError(Error); }
		if (!Errors.IsEmpty()) { return EDataValidationResult::Invalid; }
	}
	if (HitReactionProfile)
	{
		TArray<FText> ReactionErrors;
		HitReactionProfile->Validate(ReactionErrors);
		for (const auto& Error : ReactionErrors) { Context.AddError(Error); }
		if (!ReactionErrors.IsEmpty()) { return EDataValidationResult::Invalid; }
	}
	if (!ComboDefinition) { return Parent; }
	TArray<FText> Errors;
	ComboDefinition->ValidateDefinition(Errors);
	TMap<FGameplayTag, TArray<const UHodgeAbilityDefinition*>> Definitions;
	for (const UHodgeAbilitySet* Set : AbilitySets)
	{
		if (!Set) { continue; }
		for (const auto& Entry : Set->GetGrantedDefinitions())
		{
			if (!Entry.Definition)
			{
				Errors.Add(FText::FromString(TEXT("AbilitySet has an empty Definition entry")));
				continue;
			}
			Entry.Definition->ValidateDefinition(Errors);
			Definitions.FindOrAdd(Entry.Definition->AbilityTag).Add(Entry.Definition);
		}
	}
	if (ComboDefinition->ComboTable && ComboDefinition->ComboTable->GetRowStruct() == FHodgeComboRow::StaticStruct())
	{
		for (const auto& Pair : ComboDefinition->ComboTable->GetRowMap())
		{
			const auto& Node = *reinterpret_cast<const FHodgeComboRow*>(Pair.Value);
			if (Node.ComboTag == ComboDefinition->EntryComboTag) { continue; }
			const auto* Found = Definitions.Find(Node.AbilityTag);
			if (!Found || Found->Num() != 1)
			{
				Errors.Add(FText::FromString(
					Pair.Key.ToString() + TEXT(": AbilityTag must resolve to exactly one granted Definition")));
				continue;
			}
			const auto* Montage = (*Found)[0]->ExecutionConfig.Montage.Get();
			if ((*Found)[0]->ExecutionRoute != EHodgeAbilityExecutionRoute::ComboCoordinated)
			{
				Errors.Add(FText::FromString(
					Pair.Key.ToString() + TEXT(": Standalone definitions cannot be combo nodes")));
				continue;
			}
			if (!Montage) { continue; }
			FGameplayTagContainer Windows;
			FGameplayTagContainer Events;
			Events.AddTag(HodgeGameplayTags::GameplayEvent_Attack_Completed);
			Events.AddTag(HodgeGameplayTags::GameplayEvent_Attack_Interrupted);
			for (const auto& Event : Montage->Notifies)
			{
				if (const auto* Notify = Cast<UHodgeAnimNotifyState_GameplayTag>(Event.NotifyStateClass))
				{
					Windows.AddTag(Notify->StateTag);
				}
				if (const auto* Notify = Cast<UHodgeAnimNotify_GameplayEvent>(Event.Notify))
				{
					Events.AddTag(Notify->EventTag);
				}
			}
			for (int32 Index = 0; Index < Node.Transitions.Num(); ++Index)
			{
				const auto& Edge = Node.Transitions[Index];
				const FString Prefix = FString::Printf(TEXT("%s.Transitions[%d]: "), *Pair.Key.ToString(), Index);
				if (!Windows.HasAll(Edge.RequiredWindowTags))
				{
					Errors.Add(
						FText::FromString(Prefix + TEXT("RequiredWindowTags are not provided by this execution")));
				}
				if (Edge.TriggerEventTag.IsValid() && !Events.HasTagExact(Edge.TriggerEventTag))
				{
					Errors.Add(FText::FromString(
						Prefix + TEXT("TriggerEventTag requires a Montage gameplay-event notify or completion event")));
				}
			}
		}
	}
	for (const auto& Error : Errors) { Context.AddError(Error); }
	return Errors.IsEmpty() && Parent != EDataValidationResult::Invalid
		       ? EDataValidationResult::Valid
		       : EDataValidationResult::Invalid;
}
#endif
