/** @file MaterialProgressBar.h
 * @brief 声明材质驱动进度条相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"

#include "MaterialProgressBar.generated.h"

class UImage;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UWidgetAnimation;

/** 使用动态材质参数显示与动画化进度的CommonUI控件。 */
UCLASS(Abstract, meta = (DisableNativeTick))
class UMaterialProgressBar : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	/** 将配置属性同步到现有控件或表现资源。 */
	virtual void SynchronizeProperties() override;

#if WITH_EDITOR
	/** 在控件重建后同步设计期或运行时表现。 */
	virtual void OnWidgetRebuilt() override;
#endif

	/** 在指定UMG动画结束后执行对应的进度显示收尾。
	 * @param Animation 动画，由调用方传入。
	 */
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

public:
	/** 设置进度。
	 * @param Progress 进度，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable)
	void SetProgress(float Progress);

	/** 设置起始进度。
	 * @param StartProgress 起始进度，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable)
	void SetStartProgress(float StartProgress);

	/** 设置颜色A。
	 * @param ColorA 颜色A，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable)
	void SetColorA(FLinearColor ColorA);

	/** 设置颜色B。
	 * @param ColorB 颜色B，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable)
	void SetColorB(FLinearColor ColorB);

	/** 设置颜色背景。
	 * @param ColorBackground 颜色背景，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable)
	void SetColorBackground(FLinearColor ColorBackground);

	/** 从指定起始进度更新材质进度动画。
	 * @param Start 起始，由调用方传入。
	 * @param End 结束，由调用方传入。
	 * @param AnimSpeed 动画速度，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable)
	void AnimateProgressFromStart(float Start, float End, float AnimSpeed = 1.0f);

	/** 从当前显示进度更新材质进度动画。
	 * @param End 结束，由调用方传入。
	 * @param AnimSpeed 动画速度，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable)
	void AnimateProgressFromCurrent(float End, float AnimSpeed = 1.0f);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFillAnimationFinished);

	// 进度填充动画完成后的通知。
	UPROPERTY(BlueprintAssignable)
	FOnFillAnimationFinished OnFillAnimationFinished;

private:
	/** 设置进度内部。
	 * @param Progress 进度，由调用方传入。
	 */
	void SetProgress_Internal(float Progress);
	/** 设置起始进度内部。
	 * @param StartProgress 起始进度，由调用方传入。
	 */
	void SetStartProgress_Internal(float StartProgress);
	/** 设置颜色A内部。
	 * @param ColorA 颜色A，由调用方传入。
	 */
	void SetColorA_Internal(FLinearColor ColorA);
	/** 设置颜色B内部。
	 * @param ColorB 颜色B，由调用方传入。
	 */
	void SetColorB_Internal(FLinearColor ColorB);
	/** 设置颜色背景内部。
	 * @param ColorBackground 颜色背景，由调用方传入。
	 */
	void SetColorBackground_Internal(FLinearColor ColorBackground);

	/** 获取条形显示Dynamic材质。
	 * @return 条形显示Dynamic材质的当前值或引用。
	 */
	UMaterialInstanceDynamic* GetBarDynamicMaterial() const;

	// 控制是否Override默认颜色A的状态标记。
	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "CachedColorA"))
	bool bOverrideDefaultColorA = false;

	// 当前材质使用的第一组颜色缓存。
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Color A", EditCondition = "bOverrideDefaultColorA"))
	FLinearColor CachedColorA;

	// 控制是否Override默认颜色B的状态标记。
	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "CachedColorB"))
	bool bOverrideDefaultColorB = false;

	// 当前材质使用的第二组颜色缓存。
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Color B", EditCondition = "bOverrideDefaultColorB"))
	FLinearColor CachedColorB;

	// 控制是否Override默认颜色背景的状态标记。
	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "CachedColorBackground"))
	bool bOverrideDefaultColorBackground;

	UPROPERTY(EditAnywhere,
		meta = (DisplayName = "Color Background", EditCondition = "bOverrideDefaultColorBackground"))
	// 当前进度材质的背景颜色缓存。
	FLinearColor CachedColorBackground;

	// 控制是否Override默认分段数量的状态标记。
	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "Segments"))
	bool bOverrideDefaultSegments = false;

	// 进度条材质的分段数量。
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultSegments"))
	int32 Segments;

	// 控制是否Override默认分段边缘的状态标记。
	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "SegmentEdge"))
	bool bOverrideDefaultSegmentEdge = false;

	// 分段边缘的材质参数。
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultSegmentEdge"))
	float SegmentEdge;

	// 控制是否Override默认填充边缘柔和度的状态标记。
	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "FillEdgeSoftness"))
	bool bOverrideDefaultFillEdgeSoftness;

	// 填充边缘的柔和程度。
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultFillEdgeSoftness"))
	float FillEdgeSoftness;

	// 控制是否Override默认发光边缘的状态标记。
	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "GlowEdge"))
	bool bOverrideDefaultGlowEdge = false;

	// 发光边缘的材质参数。
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultGlowEdge"))
	float GlowEdge;

	// 控制是否Override默认发光柔和度的状态标记。
	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "GlowSoftness"))
	bool bOverrideDefaultGlowSoftness = false;

	// 发光区域的柔和程度。
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultGlowSoftness"))
	float GlowSoftness;

	// 控制是否Override默认轮廓缩放的状态标记。
	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "OutlineScale"))
	bool bOverrideDefaultOutlineScale = false;

	// 材质轮廓的缩放参数。
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultOutlineScale"))
	float OutlineScale;

	// 控制是否Use描边的状态标记。
	UPROPERTY(EditAnywhere)
	bool bUseStroke = true;

	// 带描边的进度条材质。
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> StrokeMaterial;

	// 不带描边的进度条材质。
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> NoStrokeMaterial;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Design Time Progress"))
	float DesignTime_Progress = 1.0f;
#endif

	// 显示进度条材质的绑定Image控件。
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UImage> Image_Bar;

	// 已绑定动画填充条形显示的数据或对象引用。
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim, AllowPrivateAccess))
	TObjectPtr<UWidgetAnimation> BoundAnim_FillBar;

	// 当前进度条使用的动态材质实例缓存。
	UPROPERTY(Transient)
	mutable TObjectPtr<UMaterialInstanceDynamic> CachedMID;

	// 最近一次已提交到材质的进度缓存。
	float CachedProgress = -1.0f;
	// 最近一次已提交到材质的起始进度缓存。
	float CachedStartProgress = -1.0f;
};
