/** @file CircumferenceMarkerWidget.h
 * @brief 声明圆周标记控件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/Widget.h"
#include "UI/Weapons/SCircumferenceMarkerWidget.h"

#include "CircumferenceMarkerWidget.generated.h"

class SWidget;
class UObject;
struct FFrame;

/** 封装圆周标记控件的状态与操作接口。 */
UCLASS()
class UCircumferenceMarkerWidget : public UWidget
{
	GENERATED_BODY()

public:
	/** 构造圆周标记控件并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UCircumferenceMarkerWidget(const FObjectInitializer& ObjectInitializer);

	//~UWidget interface
public:
	/** 将配置属性同步到现有控件或表现资源。 */
	virtual void SynchronizeProperties() override;
protected:
	/** 根据当前属性创建对应的Slate控件。
	 * @return 本次计算、查询或创建得到的T共享RefS控件。
	 */
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~End of UWidget interface

	//~UVisual interface
public:
	/** 释放Slate界面Resources。
	 * @param bReleaseChildren 是否释放子项的控制标记。
	 */
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~End of UVisual interface
	
public:
	/** The list of positions/orientations to draw the markers at. */
	// 标记列表的记录集合。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	TArray<FCircumferenceMarkerEntry> MarkerList;

	/** The radius of the circle. */
	// 半径的数值或阈值。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, meta=(ClampMin=0.0))
	float Radius = 48.0f;

	/** The marker image to place around the circle. */
	// 标记图像的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FSlateBrush MarkerImage;

	/** Whether reticle corner images are placed outside the spread radius */
	//@TODO: Make this a 0-1 float alignment instead (e.g., inside/on/outside the radius)?
	// 控制是否准星角点OutsideSpread半径的状态标记。
	UPROPERTY(EditAnywhere, Category=Corner)
	uint8 bReticleCornerOutsideSpreadRadius : 1;

public:
	/** Sets the radius of the circle. */
	/** 设置半径。
	 * @param InRadius 半径，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
	void SetRadius(float InRadius);

private:
	/** Internal slate widget representing the actual marker visuals */
	// My标记控件的数据或对象引用。
	TSharedPtr<SCircumferenceMarkerWidget> MyMarkerWidget;
};
