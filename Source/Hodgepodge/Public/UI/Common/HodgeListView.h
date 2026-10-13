/** @file HodgeListView.h
 * @brief 声明列表控件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonListView.h"

#include "HodgeListView.generated.h"

class UUserWidget;
class ULocalPlayer;
class UHodgeWidgetFactory;

/** 封装列表控件的状态与操作接口。 */
UCLASS(meta = (DisableNativeTick))
class HODGEPODGE_API UHodgeListView : public UCommonListView
{
	GENERATED_BODY()

public:
	/** 构造列表控件并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeListView(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	/** 校验已编译Defaults。
	 * @param InCompileLog 编译Log，由调用方传入。
	 */
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const override;
#endif

protected:
	/** 处理Generate条目控件内部。
	 * @param Item Item，由调用方传入。
	 * @param DesiredEntryClass 期望条目类型，由调用方传入。
	 * @param OwnerTable 所属对象数据表，由调用方传入。
	 * @return 本次计算、查询或创建得到的用户控件。
	 */
	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass,
	                                                   const TSharedRef<STableViewBase>& OwnerTable) override;
	//virtual bool OnIsSelectableOrNavigableInternal(UObject* SelectedItem) override;

protected:
	// Factory规则的记录集合。
	UPROPERTY(EditAnywhere, Instanced, Category="Entry Creation")
	TArray<TObjectPtr<UHodgeWidgetFactory>> FactoryRules;
};
