#include "Data/HodgePawnData.h"
#if WITH_EDITOR
#include "Data/HodgeAbilityDefinition.h"
#include "Data/HodgeAbilitySet.h"
#include "Data/HodgeAbilityTimeline.h"
#include "Data/HodgeComboDefinition.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Misc/DataValidation.h"

EDataValidationResult UHodgePawnData::IsDataValid(FDataValidationContext& Context) const
{
	const auto Parent = Super::IsDataValid(Context);
	if (!ComboDefinition) { return Parent; }
	TArray<FText> Errors;
	ComboDefinition->ValidateDefinition(Errors);
	TMap<FGameplayTag, TArray<const UHodgeAbilityDefinition*>> Definitions;
	for (const UHodgeAbilitySet* Set : AbilitySets)
	{
		if (!Set) { continue; }
		for (const auto& Entry : Set->GetGrantedDefinitions())
		{
			if (!Entry.Definition) { Errors.Add(FText::FromString(TEXT("AbilitySet has an empty Definition entry"))); continue; }
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
				Errors.Add(FText::FromString(Pair.Key.ToString() + TEXT(": AbilityTag must resolve to exactly one granted Definition")));
				continue;
			}
			const auto* Timeline = (*Found)[0]->ExecutionConfig.TimelineTaskConfig.Timeline.Get();
			if (!Timeline) { continue; }
			FGameplayTagContainer Windows;
			FGameplayTagContainer Events;
			Events.AddTag(HodgeGameplayTags::GameplayEvent_Attack_Timeline_End);
			for (const auto& Event : Timeline->Events)
			{
				if (Event.Kind == EHodgeTimelineEventKind::Window) { Windows.AddTag(Event.WindowTag); }
				else if (Event.NetPolicy != EHodgeTimelineEventNetPolicy::LocallyControlledOnly) { Events.AddTag(Event.PointEventTag); }
			}
			for (int32 Index = 0; Index < Node.Transitions.Num(); ++Index)
			{
				const auto& Edge = Node.Transitions[Index];
				const FString Prefix = FString::Printf(TEXT("%s.Transitions[%d]: "), *Pair.Key.ToString(), Index);
				if (!Windows.HasAll(Edge.RequiredWindowTags))
				{ Errors.Add(FText::FromString(Prefix + TEXT("RequiredWindowTags are not provided by this execution"))); }
				if (Edge.TriggerEventTag.IsValid() && !Events.HasTagExact(Edge.TriggerEventTag))
				{ Errors.Add(FText::FromString(Prefix + TEXT("TriggerEventTag requires an authority Timeline point or natural-end event"))); }
			}
		}
	}
	for (const auto& Error : Errors) { Context.AddError(Error); }
	return Errors.IsEmpty() && Parent != EDataValidationResult::Invalid ? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}
#endif
