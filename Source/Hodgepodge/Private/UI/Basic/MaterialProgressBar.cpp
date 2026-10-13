/** @file MaterialProgressBar.cpp
 * @brief 实现材质驱动进度条相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Basic/MaterialProgressBar.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MaterialProgressBar)

/** 将配置属性同步到现有控件或表现资源。 */
void UMaterialProgressBar::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Image_Bar)
	{
		Image_Bar->SetBrushFromMaterial(bUseStroke ? StrokeMaterial : NoStrokeMaterial);
		CachedMID = nullptr;
		CachedProgress = -1.0f;
		CachedStartProgress = -1.0f;

#if WITH_EDITORONLY_DATA
		if (IsDesignTime())
		{
			SetProgress_Internal(DesignTime_Progress);
		}
#endif

		if (UMaterialInstanceDynamic* MID = GetBarDynamicMaterial())
		{
			if (bOverrideDefaultSegmentEdge)
			{
				MID->SetScalarParameterValue(TEXT("SegmentEdge"), SegmentEdge);
			}

			if (bOverrideDefaultSegments)
			{
				MID->SetScalarParameterValue(TEXT("Segments"), (float)Segments);
			}

			if (bOverrideDefaultFillEdgeSoftness)
			{
				MID->SetScalarParameterValue(TEXT("FillEdgeSoftness"), FillEdgeSoftness);
			}

			if (bOverrideDefaultGlowEdge)
			{
				MID->SetScalarParameterValue(TEXT("GlowEdge"), GlowEdge);
			}

			if (bOverrideDefaultGlowSoftness)
			{
				MID->SetScalarParameterValue(TEXT("GlowSoftness"), GlowSoftness);
			}

			if (bOverrideDefaultOutlineScale)
			{
				MID->SetScalarParameterValue(TEXT("OutlineScale"), OutlineScale);
			}
		}

		if (bOverrideDefaultColorA)
		{
			SetColorA_Internal(CachedColorA);
		}

		if (bOverrideDefaultColorB)
		{
			SetColorB_Internal(CachedColorB);
		}

		if (bOverrideDefaultColorBackground)
		{
			SetColorBackground_Internal(CachedColorBackground);
		}
	}
}

#if WITH_EDITOR
/** 在控件重建后同步设计期或运行时表现。 */
void UMaterialProgressBar::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	if (IsDesignTime() && Image_Bar)
	{
		if (UMaterialInstanceDynamic* MID = GetBarDynamicMaterial())
		{
			if (!bOverrideDefaultColorA)
			{
				MID->GetVectorParameterValue(TEXT("ColorA"), CachedColorA);
			}

			if (!bOverrideDefaultColorB)
			{
				MID->GetVectorParameterValue(TEXT("ColorB"), CachedColorB);
			}

			if (!bOverrideDefaultColorBackground)
			{
				MID->GetVectorParameterValue(TEXT("Unfilled Color"), CachedColorBackground);
			}

			if (!bOverrideDefaultSegmentEdge)
			{
				MID->GetScalarParameterValue(TEXT("SegmentEdge"), SegmentEdge);
			}

			if (!bOverrideDefaultSegments)
			{
				float SegmentsFloat;
				MID->GetScalarParameterValue(TEXT("Segments"), SegmentsFloat);
				Segments = FMath::TruncToInt(SegmentsFloat);
			}

			if (!bOverrideDefaultFillEdgeSoftness)
			{
				MID->GetScalarParameterValue(TEXT("FillEdgeSoftness"), FillEdgeSoftness);
			}

			if (!bOverrideDefaultGlowEdge)
			{
				MID->GetScalarParameterValue(TEXT("GlowEdge"), GlowEdge);
			}

			if (!bOverrideDefaultGlowSoftness)
			{
				MID->GetScalarParameterValue(TEXT("GlowSoftness"), GlowSoftness);
			}

			if (!bOverrideDefaultOutlineScale)
			{
				MID->GetScalarParameterValue(TEXT("OutlineScale"), OutlineScale);
			}
		}
	}
}
#endif

/** 在指定UMG动画结束后执行对应的进度显示收尾。 */
void UMaterialProgressBar::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	if (BoundAnim_FillBar == Animation)
	{
		OnFillAnimationFinished.Broadcast();
	}
}

/** 设置进度。 */
void UMaterialProgressBar::SetProgress(float Progress)
{
	if (CachedProgress != Progress)
	{
		SetProgress_Internal(Progress);
	}
}

/** 设置起始进度。 */
void UMaterialProgressBar::SetStartProgress(float StartProgress)
{
	if (CachedStartProgress != StartProgress)
	{
		SetStartProgress_Internal(StartProgress);
	}
}

/** 设置颜色A。 */
void UMaterialProgressBar::SetColorA(FLinearColor ColorA)
{
	if (CachedColorA != ColorA)
	{
		SetColorA_Internal(ColorA);
	}
}

/** 设置颜色B。 */
void UMaterialProgressBar::SetColorB(FLinearColor ColorB)
{
	if (CachedColorB != ColorB)
	{
		SetColorB_Internal(ColorB);
	}
}

/** 设置颜色背景。 */
void UMaterialProgressBar::SetColorBackground(FLinearColor ColorBackground)
{
	if (CachedColorBackground != ColorBackground)
	{
		SetColorBackground_Internal(ColorBackground);
	}
}

/** 从指定起始进度更新材质进度动画。 */
void UMaterialProgressBar::AnimateProgressFromStart(float Start, float End, float AnimSpeed)
{
	SetStartProgress(Start);
	SetProgress(End);
	PlayAnimation(BoundAnim_FillBar, 0.0f, 1, EUMGSequencePlayMode::Forward, AnimSpeed);
}

/** 从当前显示进度更新材质进度动画。 */
void UMaterialProgressBar::AnimateProgressFromCurrent(float End, float AnimSpeed)
{
	if (UMaterialInstanceDynamic* MID = GetBarDynamicMaterial())
	{
		const float CurrentStart = MID->K2_GetScalarParameterValue(TEXT("StartProgress"));
		const float CurrentEnd = MID->K2_GetScalarParameterValue(TEXT("Progress"));
		const float CurrentFill = MID->K2_GetScalarParameterValue(TEXT("FillAmount"));
		const float NewStart = FMath::Lerp(CurrentStart, CurrentEnd, CurrentFill);
		AnimateProgressFromStart(NewStart, End, AnimSpeed);
	}
}

/** 设置进度内部。 */
void UMaterialProgressBar::SetProgress_Internal(float Progress)
{
	if (UMaterialInstanceDynamic* MID = GetBarDynamicMaterial())
	{
		CachedProgress = Progress;
		MID->SetScalarParameterValue(TEXT("Progress"), CachedProgress);
	}
}

/** 设置起始进度内部。 */
void UMaterialProgressBar::SetStartProgress_Internal(float StartProgress)
{
	if (UMaterialInstanceDynamic* MID = GetBarDynamicMaterial())
	{
		CachedStartProgress = StartProgress;
		MID->SetScalarParameterValue(TEXT("StartProgress"), CachedStartProgress);
	}
}

/** 设置颜色A内部。 */
void UMaterialProgressBar::SetColorA_Internal(FLinearColor ColorA)
{
	if (UMaterialInstanceDynamic* MID = GetBarDynamicMaterial())
	{
		CachedColorA = ColorA;
		MID->SetVectorParameterValue(TEXT("ColorA"), CachedColorA);
	}
}

/** 设置颜色B内部。 */
void UMaterialProgressBar::SetColorB_Internal(FLinearColor ColorB)
{
	if (UMaterialInstanceDynamic* MID = GetBarDynamicMaterial())
	{
		CachedColorB = ColorB;
		MID->SetVectorParameterValue(TEXT("ColorB"), CachedColorB);
	}
}

/** 设置颜色背景内部。 */
void UMaterialProgressBar::SetColorBackground_Internal(FLinearColor ColorBackground)
{
	if (UMaterialInstanceDynamic* MID = GetBarDynamicMaterial())
	{
		CachedColorBackground = ColorBackground;
		MID->SetVectorParameterValue(TEXT("Unfilled Color"), CachedColorBackground);
	}
}

/** 获取条形显示Dynamic材质。 */
UMaterialInstanceDynamic* UMaterialProgressBar::GetBarDynamicMaterial() const
{
	if (!CachedMID)
	{
		CachedMID = Image_Bar->GetDynamicMaterial();
	}

	return CachedMID;
}
