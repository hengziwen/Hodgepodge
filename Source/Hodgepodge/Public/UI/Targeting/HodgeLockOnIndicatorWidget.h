/** @file HodgeLockOnIndicatorWidget.h @brief Indicator 系统使用的轻量锁定标记。 */
#pragma once
#include "Blueprint/UserWidget.h"
#include "HodgeLockOnIndicatorWidget.generated.h"
/** 原型采用可替换的菱形标记，位置由既有 IndicatorLayer 投影。 */
UCLASS()
class HODGEPODGE_API UHodgeLockOnIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
};
