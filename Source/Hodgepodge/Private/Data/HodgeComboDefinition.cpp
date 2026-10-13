/** @file HodgeComboDefinition.cpp
 * @brief 实现连段图配置相关的类型、接口与现有处理流程。
 */
#include "Data/HodgeComboDefinition.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeComboDefinition)

/** 查找节点。 */
const FHodgeComboRow* UHodgeComboDefinition::FindNode(FGameplayTag Tag) const
{
	return ComboTable && ComboTable->GetRowStruct() == FHodgeComboRow::StaticStruct()
		       ? ComboTable->FindRow<FHodgeComboRow>(Tag.GetTagName(), TEXT("Combo"), false)
		       : nullptr;
}

/** 检查定义配置及其引用资源是否满足执行契约。 */
bool UHodgeComboDefinition::ValidateDefinition(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	auto Error = [&Errors](const FString& Message) { Errors.Add(FText::FromString(Message)); };
	if (!ComboTable || ComboTable->GetRowStruct() != FHodgeComboRow::StaticStruct())
	{
		Error(TEXT("ComboTable must use HodgeComboRow"));
		return false;
	}
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!EntryComboTag.IsValid() || !FindNode(EntryComboTag))
	{
		Error(TEXT("Missing EntryComboTag row"));
	}
	if (!FMath::IsFinite(InputBufferSeconds) || InputBufferSeconds <= 0.f)
	{
		Error(TEXT("Invalid input buffer duration"));
	}
	if (!FMath::IsFinite(MoveIntentThreshold) || MoveIntentThreshold < 0.f)
	{
		Error(TEXT("Invalid movement threshold"));
	}
	if (!FMath::IsFinite(ComboRetentionSeconds) || ComboRetentionSeconds < 0.f)
	{
		Error(TEXT("Invalid combo retention duration"));
	}
	TSet<FGameplayTag> BoundInputs;
	for (const auto& Binding : InputBindings)
	{
		if (!Binding.InputTag.IsValid() || !Binding.IntentTag.IsValid() || BoundInputs.Contains(Binding.InputTag))
		{
			Error(TEXT("Invalid or duplicate input binding"));
		}
		BoundInputs.Add(Binding.InputTag);
	}
	for (const auto& Pair : ComboTable->GetRowMap())
	{
		const auto& Row = *reinterpret_cast<const FHodgeComboRow*>(Pair.Value);
		const FString Prefix = Pair.Key.ToString() + TEXT(": ");
		if (!Row.ComboTag.IsValid() || Row.ComboTag.GetTagName() != Pair.Key)
		{
			Error(Prefix + TEXT("RowName must equal ComboTag"));
		}
		if ((Row.ComboTag == EntryComboTag) == Row.AbilityTag.IsValid())
		{
			Error(Prefix + TEXT("Only Entry must have an empty AbilityTag"));
		}
		if (Row.ComboTag == EntryComboTag && !Row.GrantedTags.IsEmpty())
		{
			Error(Prefix + TEXT("Entry must not grant tags"));
		}
		for (int32 Index = 0; Index < Row.Transitions.Num(); ++Index)
		{
			const auto& Edge = Row.Transitions[Index];
			const FString Context = Prefix + FString::Printf(TEXT("Transitions[%d]: "), Index);
			if (Edge.TriggerInputIntentTag.IsValid() == Edge.TriggerEventTag.IsValid())
			{
				Error(Context + TEXT("Exactly one input/event trigger is required"));
			}
			if (Edge.bAllowAfterExecutionEnded && (!Edge.TriggerInputIntentTag.IsValid() || Row.ComboTag == EntryComboTag))
			{
				Error(Context + TEXT("Resume requires an input transition from an attack node"));
			}
			if (!Edge.TargetComboTag.IsValid() || !FindNode(Edge.TargetComboTag))
			{
				Error(Context + TEXT("Missing target node"));
			}
			if (Edge.RequiredSourceTags.HasAny(Edge.BlockedSourceTags))
			{
				Error(Context + TEXT("Required source state is blocked"));
			}
			if (Row.ComboTag == EntryComboTag && !Edge.RequiredWindowTags.IsEmpty())
			{
				Error(Context + TEXT("Entry has no execution windows"));
			}
		}
	}
	return Before == Errors.Num();
}
#if WITH_EDITOR
/** 在编辑器校验当前配置并收集诊断。 */
EDataValidationResult UHodgeComboDefinition::IsDataValid(FDataValidationContext& Context) const
{
	const auto Parent = Super::IsDataValid(Context);
	TArray<FText> Errors;
	ValidateDefinition(Errors);
	for (const FText& Error : Errors) { Context.AddError(Error); }
	return Errors.IsEmpty() && Parent != EDataValidationResult::Invalid
		       ? EDataValidationResult::Valid
		       : EDataValidationResult::Invalid;
}
#endif
