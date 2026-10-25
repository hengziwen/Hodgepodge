/** @file HodgeEncounterPromptWidget.cpp @brief 按服务器快照更新文字，不增加持续 Tick。 */
#include "UI/Encounter/HodgeEncounterPromptWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEncounterPromptWidget)

TSharedRef<SWidget> UHodgeEncounterPromptWidget::RebuildWidget()
{
	// 原生构造最小提示树，表现不改变目标的服务器授权。
	if (!WidgetTree) { WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree")); }
	if (!WidgetTree->RootWidget)
	{
		Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Prompt"));
		Label->SetJustification(ETextJustify::Center);
		Label->SetText(Prompt);
		WidgetTree->RootWidget = Label;
	}
	return Super::RebuildWidget();
}

void UHodgeEncounterPromptWidget::SetPrompt(const FText& InPrompt)
{
	Prompt = InPrompt;
	if (Label) { Label->SetText(Prompt); }
}

FText UHodgeEncounterPromptWidget::GetPromptText() const
{
	return Label ? Label->GetText() : Prompt;
}
