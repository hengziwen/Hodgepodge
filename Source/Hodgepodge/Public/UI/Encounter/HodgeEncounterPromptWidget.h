/** @file HodgeEncounterPromptWidget.h @brief 仅显示刷怪开关当前交互文字，不持有奖励或运行状态。 */
#pragma once

#include "Blueprint/UserWidget.h"
#include "HodgeEncounterPromptWidget.generated.h"

class UTextBlock;

/** 使用 UMG 的复合字体显示中文，避免世界 TextRender 缺字。 */
UCLASS()
class HODGEPODGE_API UHodgeEncounterPromptWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetPrompt(const FText& InPrompt);
	/** 读取实际 UMG 文本供表现与联机验证，不提供业务执行捷径。 */
	UFUNCTION(BlueprintPure, Category="Hodge|Encounter") FText GetPromptText() const;
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
private:
	UPROPERTY(Transient) TObjectPtr<UTextBlock> Label;
	FText Prompt;
};
