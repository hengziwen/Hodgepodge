/** @file HodgeTabButtonBase.h
 * @brief 声明标签页按钮相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "HodgeTabListWidgetBase.h"
#include "UI/Foundation/HodgeButtonBase.h"

#include "HodgeTabButtonBase.generated.h"

class UCommonLazyImage;
class UObject;
struct FFrame;
struct FSlateBrush;

/** 封装标签页按钮的状态与操作接口。 */
UCLASS(Abstract, Blueprintable, meta = (DisableNativeTick))
class HODGEPODGE_API UHodgeTabButtonBase : public UHodgeButtonBase, public IHodgeTabButtonInterface
{
	GENERATED_BODY()

public:
	/** 设置Icon来源Lazy对象。
	 * @param LazyObject Lazy对象，由调用方传入。
	 */
	void SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject);
	/** 设置Icon画刷。
	 * @param Brush 画刷，由调用方传入。
	 */
	void SetIconBrush(const FSlateBrush& Brush);

protected:
	/** 设置Tab标签信息。
	 * @param TabLabelInfo Tab标签信息，由调用方传入。
	 */
	UFUNCTION()
	virtual void SetTabLabelInfo_Implementation(const FHodgeTabDescriptor& TabLabelInfo) override;

private:
	// Lazy图像Icon的数据或对象引用。
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonLazyImage> LazyImage_Icon;
};
