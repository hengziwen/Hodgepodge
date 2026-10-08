// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/IndicatorSystem/SActorCanvas.h"

// AssetManager，用于获取全局 StreamableManager，异步加载 Indicator WidgetClass。
#include "Engine/AssetManager.h"

// GameViewportClient，用于获取当前 LocalPlayer 对应的 Viewport。
#include "Engine/GameViewportClient.h"

// StreamableManager，用于异步加载 Descriptor 中保存的软引用 WidgetClass。
#include "Engine/StreamableManager.h"

// Indicator Widget 统一绑定接口。
// Widget 创建后通过 BindIndicator 接收 Descriptor，回收前通过 UnbindIndicator 清理绑定。
#include "UI/IndicatorSystem/IActorIndicatorWidget.h"

// Slate 排列后的子 Widget 集合。
#include "Layout/ArrangedChildren.h"

// 当前 PlayerController 对应的 Indicator 管理组件。
// SActorCanvas 会监听它的 Indicator Added / Removed 事件。
#include "UI/IndicatorSystem/HodgeIndicatorManagerComponent.h"

// SceneView 投影相关数据。
// 用于获取当前玩家视图对应的 FSceneViewProjectionData。
#include "SceneView.h"

// Indicator 描述对象。
// 提供目标 Component、WidgetClass、投影模式、Clamp、Priority 等配置。
#include "UI/IndicatorSystem/IndicatorDescriptor.h"

// SBox 用作 Indicator UserWidget 的 Slate 宿主容器。
#include "Widgets/Layout/SBox.h"

// SLeafWidget 用作屏幕边缘箭头这种没有子节点的轻量 Slate Widget。
#include "Widgets/SLeafWidget.h"

class FSlateRect;

// Indicator 被 Clamp 到屏幕边缘时所在的方向。
namespace EArrowDirection
{
	enum Type
	{
		// 屏幕左侧。
		Left,

		// 屏幕上侧。
		Top,

		// 屏幕右侧。
		Right,

		// 屏幕下侧。
		Bottom,

		// 方向数量，同时作为“没有 Clamp 方向”的无效值使用。
		MAX
	};
}

// Angles for the direction of the arrow to display
// 每一个屏幕边缘方向对应的箭头旋转角度。
// 默认箭头图片按向上方向制作，因此：
// Left = 270°、Top = 0°、Right = 90°、Bottom = 180°。
const float ArrowRotations[EArrowDirection::MAX] =
{
	270.0f,
	0.0f,
	90.0f,
	180.0f
};

// Offsets for the each direction that the arrow can point
// 每一个 Clamp 方向对应的二维单位偏移方向。
// 后续会利用该方向把箭头放到 Indicator 对应的一侧。
const FVector2D ArrowOffsets[EArrowDirection::MAX] =
{
	FVector2D(-1.0f, 0.0f),
	FVector2D(0.0f, -1.0f),
	FVector2D(1.0f, 0.0f),
	FVector2D(0.0f, 1.0f)
};


// Indicator 被 Clamp 到屏幕边缘时使用的箭头 Slate Widget。
// 这是一个轻量 SLeafWidget，只负责绘制并旋转指定的 Arrow Brush。
class SActorCanvasArrowWidget : public SLeafWidget
{
public:
	// Slate 构造参数。
	SLATE_BEGIN_ARGS(SActorCanvasArrowWidget)
		{
		}

		/** always goes at the end */
	SLATE_END_ARGS()

	/** Ctor */
	// 默认旋转角度为 0，并且尚未绑定 Arrow Brush。
	SActorCanvasArrowWidget()
		: Rotation(0.0f)
		  , Arrow(nullptr)
	{
	}

	/** Every widget needs one of these */
	// 初始化箭头 Widget。
	void Construct(const FArguments& InArgs, const FSlateBrush* ActorCanvasArrowBrush)
	{
		// 保存 UIndicatorLayer 传递下来的默认箭头 Brush。
		Arrow = ActorCanvasArrowBrush;

		// 箭头本身不需要 Tick。
		SetCanTick(false);
	}

	// 绘制箭头。
	virtual int32 OnPaint(const FPaintArgs& Args,
	                      const FGeometry& AllottedGeometry,
	                      const FSlateRect& MyClippingRect,
	                      FSlateWindowElementList& OutDrawElements,
	                      int32 LayerId,
	                      const FWidgetStyle& InWidgetStyle,
	                      bool bParentEnabled) const override
	{
		// 当前已经使用到的最大 Slate Layer。
		int32 MaxLayerId = LayerId;

		// 只有存在有效 Arrow Brush 时才真正绘制。
		if (Arrow)
		{
			// 根据父级 Enabled 状态决定当前箭头是否启用。
			const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);

			// Disabled 时使用 Slate 默认禁用绘制效果。
			const ESlateDrawEffect DrawEffects = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

			// 合并父 WidgetStyle 与 Arrow Brush 自身的 Tint，得到最终颜色。
			const FColor FinalColorAndOpacity = (InWidgetStyle.GetColorAndOpacityTint() * Arrow->GetTint(InWidgetStyle))
				.ToFColor(true);

			// 以当前 Rotation 角度绘制旋转后的箭头 Brush。
			FSlateDrawElement::MakeRotatedBox(
				OutDrawElements,
				MaxLayerId++,
				AllottedGeometry.ToPaintGeometry(Arrow->ImageSize, FSlateLayoutTransform()),
				Arrow,
				DrawEffects,

				// Rotation 内部使用角度，这里转换成 Slate 绘制需要的弧度。
				FMath::DegreesToRadians(GetRotation()),

				// 不指定自定义旋转中心。
				TOptional<FVector2D>(),

				// 旋转坐标相对于当前绘制元素。
				FSlateDrawElement::RelativeToElement,

				// 最终颜色和透明度。
				FinalColorAndOpacity
			);
		}

		// 返回当前 Widget 绘制所使用到的最大 LayerId。
		return MaxLayerId;
	}

	// 设置箭头旋转角度。
	FORCEINLINE void SetRotation(float InRotation)
	{
		// 将角度限制在一个 360° 周期内。
		Rotation = FMath::Fmod(InRotation, 360.0f);
	}

	// 获取当前箭头旋转角度。
	FORCEINLINE float GetRotation() const
	{
		return Rotation;
	}

	// 箭头 Widget 的期望尺寸直接使用 Brush 图片尺寸。
	virtual FVector2D ComputeDesiredSize(float) const override
	{
		if (Arrow)
		{
			return Arrow->ImageSize;
		}
		else
		{
			// 没有 Brush 时没有期望尺寸。
			return FVector2D::ZeroVector;
		}
	}

private:
	// 当前箭头的旋转角度。
	float Rotation;

	// 当前箭头使用的 Slate Brush。
	// Brush 实际由外部 UIndicatorLayer 持有。
	const FSlateBrush* Arrow;
};

// 初始化 SActorCanvas。
void SActorCanvas::Construct(const FArguments& InArgs, const FLocalPlayerContext& InLocalPlayerContext,
                             const FSlateBrush* InActorCanvasArrowBrush)
{
	// 保存当前 Canvas 所属的 LocalPlayer 上下文。
	LocalPlayerContext = InLocalPlayerContext;

	// 保存屏幕边缘箭头使用的 Brush。
	ActorCanvasArrowBrush = InActorCanvasArrowBrush;

	// WidgetPool 创建 UUserWidget 时需要知道对应 World。
	IndicatorPool.SetWorld(LocalPlayerContext.GetWorld());

	// 不使用普通 Slate Tick。
	// Indicator 更新由后面的 ActiveTimer 驱动。
	SetCanTick(false);

	// Canvas 自身可见，但不拦截 HitTest。
	SetVisibility(EVisibility::SelfHitTestInvisible);

	// Create 10 arrows for starters
	// 预创建 10 个屏幕边缘箭头 Widget，
	// 后续布局时直接复用，避免每帧动态创建箭头。
	for (int32 i = 0; i < 10; ++i)
	{
		// 创建一个箭头 Slate Widget。
		TSharedRef<SActorCanvasArrowWidget> ArrowWidget = SNew(SActorCanvasArrowWidget, ActorCanvasArrowBrush);

		// 默认隐藏，只有某个 Indicator 真正发生 Clamp 时才显示。
		ArrowWidget->SetVisibility(EVisibility::Collapsed);

		// 将箭头加入独立的 ArrowChildren。
		ArrowChildren.AddSlot(MoveTemp(
			FArrowSlot::FSlotArguments(MakeUnique<FArrowSlot>())
			[
				ArrowWidget
			]
		));
	}

	// 根据当前状态决定是否启动 ActiveTimer。
	UpdateActiveTimer();
}

// Indicator Canvas 的持续更新函数。
// 负责连接 IndicatorManager、获取玩家投影数据、
// 更新每个 Indicator Slot 的屏幕位置、可见性、深度和 Priority。
EActiveTimerReturnType SActorCanvas::UpdateCanvas(double InCurrentTime, float InDeltaTime)
{
	// Unreal 性能统计标记，用于分析 UpdateCanvas 的 CPU 开销。
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SActorCanvas_UpdateCanvas);

	// OnPaint 尚未执行时，还不知道当前 Canvas 的实际 Geometry / Size，
	// 因此暂时无法进行正确的屏幕空间投影。
	if (!OptionalPaintGeometry.IsSet())
	{
		return EActiveTimerReturnType::Continue;
	}

	// Grab the local player
	// 获取当前 Canvas 所属的 LocalPlayer。
	ULocalPlayer* LocalPlayer = LocalPlayerContext.GetLocalPlayer();

	// 尝试取得之前缓存的 IndicatorManagerComponent。
	UHodgeIndicatorManagerComponent* IndicatorComponent = IndicatorComponentPtr.Get();

	// 尚未找到 Manager，或者之前缓存的 Manager 已经失效。
	if (IndicatorComponent == nullptr)
	{
		// 从当前 LocalPlayer 对应的 PlayerController 上寻找 IndicatorManagerComponent。
		IndicatorComponent = UHodgeIndicatorManagerComponent::GetComponent(LocalPlayerContext.GetPlayerController());

		if (IndicatorComponent)
		{
			// World may have changed
			// Player / World 可能发生过切换，
			// 因此重新设置 WidgetPool 当前使用的 World。
			IndicatorPool.SetWorld(LocalPlayerContext.GetWorld());

			// 缓存当前 Manager。
			IndicatorComponentPtr = IndicatorComponent;

			// 监听之后新增的 Indicator。
			IndicatorComponent->OnIndicatorAdded.AddSP(this, &SActorCanvas::OnIndicatorAdded);

			// 监听之后移除的 Indicator。
			IndicatorComponent->OnIndicatorRemoved.AddSP(this, &SActorCanvas::OnIndicatorRemoved);

			// Canvas 开始监听 Manager 之前，
			// Manager 可能已经存在一些 Indicator，
			// 因此这里进行一次全量同步，避免遗漏已有 Indicator。
			for (UIndicatorDescriptor* Indicator : IndicatorComponent->GetIndicators())
			{
				OnIndicatorAdded(Indicator);
			}
		}
		else
		{
			//TODO HIDE EVERYTHING
			// 当前还没有找到 IndicatorManager，
			// 暂时继续运行 ActiveTimer，等待之后 Manager 出现。
			return EActiveTimerReturnType::Continue;
		}
	}

	//Make sure we have a player. If we don't, we can't project anything
	// 只有存在有效 LocalPlayer 时才能进行 World → Screen 投影。
	if (LocalPlayer)
	{
		// 获取最近一次 OnPaint 保存的 Canvas Geometry。
		const FGeometry PaintGeometry = OptionalPaintGeometry.GetValue();

		// 当前玩家视图对应的投影数据。
		FSceneViewProjectionData ProjectionData;

		// 从当前 LocalPlayer / Viewport 获取真正的视图投影信息。
		if (LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, /*out*/ ProjectionData))
		{
			// 当前拥有有效投影环境，允许显示 Indicator。
			SetShowAnyIndicators(true);

			// 记录本次更新过程中是否有 Indicator 状态发生变化。
			bool IndicatorsChanged = false;

			// 遍历所有已经拥有实际 Slate Slot 的 Indicator。
			for (int32 ChildIndex = 0; ChildIndex < CanvasChildren.Num(); ++ChildIndex)
			{
				// 当前 Indicator 对应的运行时 Slot。
				SActorCanvas::FSlot& CurChild = CanvasChildren[ChildIndex];

				// 当前 Slot 对应的 Descriptor。
				UIndicatorDescriptor* Indicator = CurChild.Indicator;

				// If the slot content is invalid and we have permission to remove it
				// 如果 Descriptor 配置为“目标 Component 失效时自动移除”，
				// 并且目标 Component 当前已经无效，则自动注销该 Indicator。
				if (Indicator->CanAutomaticallyRemove())
				{
					IndicatorsChanged = true;

					// 清理对应 Widget / Slot。
					RemoveIndicatorForEntry(Indicator);

					// Decrement the current index to account for the removal
					// 当前数组发生了 Remove，后面的元素已经前移，
					// 因此索引减一，避免跳过下一个 Slot。
					--ChildIndex;
					continue;
				}

				// 将 Descriptor 当前期望的显示状态同步给 Slot。
				CurChild.SetIsIndicatorVisible(Indicator->GetIsVisible());

				// 当前 Indicator 不需要显示时，不再进行投影计算。
				if (!CurChild.GetIsIndicatorVisible())
				{
					// 如果 Slot 状态发生变化，则记录本次 Canvas 有变化。
					IndicatorsChanged |= CurChild.bIsDirty();

					// 当前变化已经处理完成。
					CurChild.ClearDirtyFlag();
					continue;
				}

				// If the indicator changed clamp status between updates, alert the indicator and mark the indicators as changed
				// 如果上一轮 Paint / Arrange 导致 Indicator 的 Clamp 状态发生变化，
				// 则在这一轮 Update 中处理这个变化。
				if (CurChild.WasIndicatorClampedStatusChanged())
				{
					// 原实现预留：
					// 可以通知 Descriptor “当前是否开始 / 停止被 Clamp”。
					//Indicator->OnIndicatorClampedStatusChanged(CurChild.WasIndicatorClamped());

					// 清除状态变化标记。
					CurChild.ClearIndicatorClampedStatusChangedFlag();

					// Clamp 状态变化意味着需要重新绘制。
					IndicatorsChanged = true;
				}

				// 用于接收投影后的：
				// X = 屏幕 X
				// Y = 屏幕 Y
				// Z = 深度 / 距离信息。
				FVector ScreenPositionWithDepth;

				// 创建 Indicator 投影器。
				FIndicatorProjection Projector;

				// 根据 Descriptor 的 ProjectionMode、Component、Socket、Offset 等信息，
				// 将世界空间目标投影到当前 LocalPlayer 的屏幕空间。
				const bool Success = Projector.Project(*Indicator, ProjectionData, PaintGeometry.Size,
				                                       OUT ScreenPositionWithDepth);

				// 投影失败。
				if (!Success)
				{
					// 当前没有有效屏幕坐标。
					CurChild.SetHasValidScreenPosition(false);

					// 当前实现同时将其视为不在摄像机前方。
					CurChild.SetInFrontOfCamera(false);

					// 收集 Dirty 状态。
					IndicatorsChanged |= CurChild.bIsDirty();

					// 当前变化已经处理。
					CurChild.ClearDirtyFlag();
					continue;
				}

				// 当前实现根据 Project() 是否成功设置摄像机前方状态。
				CurChild.SetInFrontOfCamera(Success);

				// 如果目标在摄像机前方，或者 Descriptor 允许 Clamp 到屏幕，
				// 则认为这个 Indicator 仍然拥有可以用于布局的有效屏幕位置。
				CurChild.SetHasValidScreenPosition(CurChild.GetInFrontOfCamera() || Indicator->GetClampToScreen());

				// 只有确实可以显示 Indicator 时才更新它的屏幕位置。
				if (CurChild.HasValidScreenPosition())
				{
					// Only dirty the screen position if we can actually show this indicator.
					// 保存 Project() 得到的 XY 屏幕位置。
					CurChild.SetScreenPosition(FVector2D(ScreenPositionWithDepth));

					// TODO(待验证)：Project() 的输出是 X=屏幕横坐标、Z=到摄像机距离，
					// 此处取 .X 会让排序次键变成屏幕横坐标；上游 Lyra 同样取 .X。见 indicator-ui-system.md §10。
					CurChild.SetDepth(ScreenPositionWithDepth.X);
				}

				// 同步 Descriptor 的 Priority。
				CurChild.SetPriority(Indicator->GetPriority());

				// 收集当前 Slot 是否发生变化。
				IndicatorsChanged |= CurChild.bIsDirty();

				// 本轮变化处理完成。
				CurChild.ClearDirtyFlag();
			}

			// 只有 Indicator 状态真正发生变化时，
			// 才通知 Slate 当前 Widget 的 Paint 已失效，需要重新绘制。
			if (IndicatorsChanged)
			{
				Invalidate(EInvalidateWidget::Paint);
			}
		}
		else
		{
			// 当前玩家无法取得有效投影数据，
			// 暂时隐藏所有 Indicator。
			SetShowAnyIndicators(false);
		}
	}
	else
	{
		// 没有有效 LocalPlayer 时无法投影，隐藏所有 Indicator。
		SetShowAnyIndicators(false);
	}

	// 当前已经不存在任何 Indicator，
	// Canvas 不再需要持续更新。
	if (AllIndicators.Num() == 0)
	{
		// 清空 ActiveTimer 句柄。
		TickHandle.Reset();

		// 告诉 Slate 停止当前 ActiveTimer。
		return EActiveTimerReturnType::Stop;
	}
	else
	{
		// 仍然存在 Indicator，继续下一轮更新。
		return EActiveTimerReturnType::Continue;
	}
}

// 设置当前 Canvas 是否允许显示 Indicator。
void SActorCanvas::SetShowAnyIndicators(bool bIndicators)
{
	// 只有状态真正变化时才处理。
	if (bShowAnyIndicators != bIndicators)
	{
		bShowAnyIndicators = bIndicators;

		// 当前无法显示任何 Indicator 时，
		// 将 Canvas 中所有子 Widget 统一隐藏。
		if (!bShowAnyIndicators)
		{
			for (int32 ChildIndex = 0; ChildIndex < AllChildren.Num(); ChildIndex++)
			{
				AllChildren.GetChildAt(ChildIndex)->SetVisibility(EVisibility::Collapsed);
			}
		}
	}
}

// Slate 布局阶段。
// 根据每个 Indicator Slot 的 ScreenPosition、Alignment、Clamp、Priority 等信息，
// 计算每个 Indicator Widget 和 Arrow Widget 最终应该放到哪里。
void SActorCanvas::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	// 性能统计标记。
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SActorCanvas_OnArrangeChildren);

	// 当前这一轮布局还没有使用任何 Arrow。
	NextArrowIndex = 0;

	//Make sure we have a player. If we don't, we can't project anything
	// 只有当前 Canvas 允许显示 Indicator 时才进行布局。
	if (bShowAnyIndicators)
	{
		// 获取箭头 Brush 的实际尺寸。
		const FVector2D ArrowWidgetSize = ActorCanvasArrowBrush->GetImageSize();

		// Clamp 时在屏幕边缘额外预留：
		// 固定 10 像素 + 一个 Arrow 的尺寸。
		const FIntPoint FixedPadding = FIntPoint(10.0f, 10.0f) + FIntPoint(ArrowWidgetSize.X, ArrowWidgetSize.Y);

		// 当前 Canvas 的屏幕中心。
		// 后续会从屏幕中心向目标屏幕位置发射线段，求与 ClampRect 边缘的交点。
		const FVector Center = FVector(AllottedGeometry.Size * 0.5f, 0.0f);

		// Sort the children
		// 临时收集所有 Indicator Slot，用于布局前排序。
		TArray<const SActorCanvas::FSlot*> SortedSlots;

		for (int32 ChildIndex = 0; ChildIndex < CanvasChildren.Num(); ++ChildIndex)
		{
			SortedSlots.Add(&CanvasChildren[ChildIndex]);
		}

		// 先根据 Priority 排序；
		// Priority 相同时，再根据 Depth 排序。
		//
		// 使用 StableSort 可以在排序条件相等时尽量保持原来的相对顺序。
		SortedSlots.StableSort([](const SActorCanvas::FSlot& A, const SActorCanvas::FSlot& B)
		{
			return A.GetPriority() == B.GetPriority() ? A.GetDepth() > B.GetDepth() : A.GetPriority() < B.GetPriority();
		});

		// Go through all the sorted children
		// 按排序后的顺序逐个进行布局。
		for (int32 ChildIndex = 0; ChildIndex < SortedSlots.Num(); ++ChildIndex)
		{
			//grab a child
			// 当前 Indicator Slot。
			const SActorCanvas::FSlot& CurChild = *SortedSlots[ChildIndex];

			// 当前 Slot 对应的 Descriptor。
			const UIndicatorDescriptor* Indicator = CurChild.Indicator;

			// Skip this indicator if it's invalid or has an invalid world position
			// 当前 Widget 的 Visibility 不被 ArrangedChildren 接受时，
			// 不需要继续进行布局。
			if (!ArrangedChildren.Accepts(CurChild.GetWidget()->GetVisibility()))
			{
				// 当前没有真正进行 Clamp。
				CurChild.SetWasIndicatorClamped(false);
				continue;
			}

			// 取得投影阶段计算好的初始屏幕坐标。
			FVector2D ScreenPosition = CurChild.GetScreenPosition();

			// 取得目标是否位于摄像机前方。
			const bool bInFrontOfCamera = CurChild.GetInFrontOfCamera();

			// Don't bother if we can't project the position and the indicator doesn't want to be clamped
			// Descriptor 是否要求将超出屏幕范围的 Indicator Clamp 到屏幕边缘。
			const bool bShouldClamp = Indicator->GetClampToScreen();

			//get the offset and final size of the slot
			// 根据 Widget DesiredSize 和 Descriptor 的 HAlign / VAlign，
			// 计算最终 Widget 尺寸、位置偏移和 Clamp Padding。
			FVector2D SlotSize, SlotOffset, SlotPaddingMin, SlotPaddingMax;
			GetOffsetAndSize(Indicator, SlotSize, SlotOffset, SlotPaddingMin, SlotPaddingMax);

			// 记录当前 Indicator 在这一轮布局中是否真的发生了 Clamp。
			bool bWasIndicatorClamped = false;

			// If we don't have to clamp this thing, we can skip a lot of work
			// 只有 Descriptor 开启 ClampToScreen 时才执行屏幕边缘限制逻辑。
			if (bShouldClamp)
			{
				//figure out if we clamped to any edge of the screen
				// 默认没有 Clamp 到任何方向。
				EArrowDirection::Type ClampDir = EArrowDirection::MAX;

				// Determine the size of inner screen rect to clamp within
				// 根据 Widget 自身尺寸、对齐方式、箭头尺寸和固定 Padding，
				// 计算真正允许 Indicator 中心点活动的屏幕内部矩形。
				const FIntPoint RectMin = FIntPoint(SlotPaddingMin.X, SlotPaddingMin.Y) + FixedPadding;
				const FIntPoint RectMax = FIntPoint(AllottedGeometry.Size.X - SlotPaddingMax.X,
				                                    AllottedGeometry.Size.Y - SlotPaddingMax.Y) - FixedPadding;

				// Indicator 最终需要被限制在这个矩形内部。
				const FIntRect ClampRect(RectMin, RectMax);

				// Make sure the screen position is within the clamp rect
				// 当前投影位置已经超出允许显示区域。
				if (!ClampRect.Contains(FIntPoint(ScreenPosition.X, ScreenPosition.Y)))
				{
					// 使用四个平面描述 ClampRect 的四条边界。
					const FPlane Planes[] =
					{
						FPlane(FVector(1.0f, 0.0f, 0.0f), ClampRect.Min.X), // Left
						FPlane(FVector(0.0f, 1.0f, 0.0f), ClampRect.Min.Y), // Top
						FPlane(FVector(-1.0f, 0.0f, 0.0f), -ClampRect.Max.X), // Right
						FPlane(FVector(0.0f, -1.0f, 0.0f), -ClampRect.Max.Y) // Bottom
					};

					// 从屏幕中心到目标屏幕位置形成一条线段，
					// 找它与 ClampRect 四条边的交点，
					// 从而把 Indicator 沿目标方向压到屏幕边缘。
					for (int32 i = 0; i < EArrowDirection::MAX; ++i)
					{
						FVector NewPoint;

						// 判断 Center → ScreenPosition 是否与当前边界平面相交。
						if (FMath::SegmentPlaneIntersection(Center, FVector(ScreenPosition, 0.0f), Planes[i], NewPoint))
						{
							// 记录当前 Clamp 到哪一条边。
							ClampDir = (EArrowDirection::Type)i;

							// 使用交点作为新的屏幕位置。
							ScreenPosition = FVector2D(NewPoint);
						}
					}
				}
				// 当前 XY 看起来仍在 ClampRect 内，
				// 但目标实际位于摄像机后方时，
				// 仍然需要将它钉到屏幕某一侧。
				else if (!bInFrontOfCamera)
				{
					// 将当前屏幕位置归一化，用于判断更接近哪个屏幕边缘。
					const float ScreenXNorm = ScreenPosition.X / (RectMax.X - RectMin.X);
					const float ScreenYNorm = ScreenPosition.Y / (RectMax.Y - RectMin.Y);

					//we need to pin this thing to the side of the screen
					// 根据归一化坐标所在区域决定应该 Clamp 到 Left / Top / Right / Bottom。
					if (ScreenXNorm < ScreenYNorm)
					{
						if (ScreenXNorm < (-ScreenYNorm + 1.0f))
						{
							// 更靠近左侧。
							ClampDir = EArrowDirection::Left;
							ScreenPosition.X = ClampRect.Min.X;
						}
						else
						{
							// 更靠近下侧。
							ClampDir = EArrowDirection::Bottom;
							ScreenPosition.Y = ClampRect.Max.Y;
						}
					}
					else
					{
						if (ScreenXNorm < (-ScreenYNorm + 1.0f))
						{
							// 更靠近上侧。
							ClampDir = EArrowDirection::Top;
							ScreenPosition.Y = ClampRect.Min.Y;
						}
						else
						{
							// 更靠近右侧。
							ClampDir = EArrowDirection::Right;
							ScreenPosition.X = ClampRect.Max.X;
						}
					}
				}

				// 只要最终得到了有效 Clamp 方向，
				// 就说明当前 Indicator 被限制到了屏幕边缘。
				bWasIndicatorClamped = (ClampDir != EArrowDirection::MAX);

				// should we show an arrow
				// Descriptor 要求显示屏幕边缘箭头，
				// 当前确实发生了 Clamp，
				// 并且预创建的 ArrowChildren 中还有可用箭头时，才显示箭头。
				if (Indicator->GetShowClampToScreenArrow() &&
					bWasIndicatorClamped &&
					ArrowChildren.IsValidIndex(NextArrowIndex))
				{
					// 当前 Clamp 方向对应的箭头偏移方向。
					const FVector2D ArrowOffsetDirection = ArrowOffsets[ClampDir];

					// 当前 Clamp 方向对应的箭头旋转角度。
					const float ArrowRotation = ArrowRotations[ClampDir];

					//grab an arrow widget
					// 从预创建的箭头池中取出当前需要使用的 Arrow Widget。
					TSharedRef<SActorCanvasArrowWidget> ArrowWidgetToUse = StaticCastSharedRef<SActorCanvasArrowWidget>(
						ArrowChildren.GetChildAt(NextArrowIndex));

					// 下一个 Indicator 使用下一个箭头。
					NextArrowIndex++;

					//set the rotation of the arrow
					// 根据 Clamp 方向旋转箭头。
					ArrowWidgetToUse->SetRotation(ArrowRotation);

					//figure out the magnitude of the offset
					// Indicator Widget 与箭头之间需要错开的距离，
					// 使用两者尺寸的一半之和，避免箭头直接压在 Widget 中心。
					const FVector2D OffsetMagnitude = (SlotSize + ArrowWidgetSize) * 0.5f;

					//used to center the arrow on the position
					// 将箭头自身中心对齐到计算出来的位置。
					const FVector2D ArrowCenteringOffset = -(ArrowWidgetSize * 0.5f);

					// 根据 Indicator 的垂直对齐方式，
					// 对箭头位置再进行一次补偿。
					FVector2D ArrowAlignmentOffset = FVector2D::ZeroVector;

					switch (Indicator->VAlignment)
					{
					case VAlign_Top:
						ArrowAlignmentOffset = SlotSize * FVector2D(0.0f, 0.5f);
						break;

					case VAlign_Bottom:
						ArrowAlignmentOffset = SlotSize * FVector2D(0.0f, -0.5f);
						break;
					}

					//figure out the offset for the arrow
					// 根据方向和偏移距离得到箭头相对于 Indicator 的方向偏移。
					const FVector2D WidgetOffset = (OffsetMagnitude * ArrowOffsetDirection);

					// 合并方向偏移、对齐补偿和箭头中心补偿。
					const FVector2D FinalOffset = (WidgetOffset + ArrowAlignmentOffset + ArrowCenteringOffset);

					//get the final position
					// 得到箭头最终屏幕位置。
					const FVector2D FinalPosition = (ScreenPosition + FinalOffset);

					// 当前箭头需要显示，并且不参与 HitTest。
					ArrowWidgetToUse->SetVisibility(EVisibility::HitTestInvisible);

					// Inject the arrow on top of the indicator
					// 将箭头作为排列后的 Slate Widget 注入 ArrangedChildren。
					ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(
						ArrowWidgetToUse, // The child widget being arranged
						FinalPosition, // Child's local position (i.e. position within parent)
						ArrowWidgetSize, // Child's size
						1.f // Child's scale
					));
				}
			}

			// 将当前布局阶段实际得到的 Clamp 状态缓存回 Slot。
			// 如果和上一帧不同，会设置 ClampedStatusChanged 标记，
			// 下一轮 UpdateCanvas 可以处理这个变化。
			CurChild.SetWasIndicatorClamped(bWasIndicatorClamped);

			// Add the information about this child to the output list (ArrangedChildren)
			// 最后把真正的 Indicator Widget 加入 Slate 排列结果。
			ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(
				CurChild.GetWidget(),

				// 投影位置 + 根据 HAlign / VAlign 得到的 Widget 偏移。
				ScreenPosition + SlotOffset,

				// Widget DesiredSize。
				SlotSize,

				// 当前 Child Scale。
				1.f
			));
		}
	}

	// 如果上一轮使用的箭头数量比这一轮更多，
	// 将本轮没有继续使用的旧箭头重新隐藏。
	if (NextArrowIndex < ArrowIndexLastUpdate)
	{
		for (int32 ArrowRemovedIndex = NextArrowIndex; ArrowRemovedIndex < ArrowIndexLastUpdate; ArrowRemovedIndex++)
		{
			ArrowChildren.GetChildAt(ArrowRemovedIndex)->SetVisibility(EVisibility::Collapsed);
		}
	}

	// 保存这一轮实际使用到的箭头数量，
	// 下一轮用于判断哪些旧箭头需要隐藏。
	ArrowIndexLastUpdate = NextArrowIndex;
}

// SActorCanvas 的 Slate 绘制入口。
int32 SActorCanvas::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
                            FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
                            bool bParentEnabled) const
{
	// 性能统计标记。
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SActorCanvas_OnPaint);

	// 保存当前实际 Paint Geometry。
	// UpdateCanvas 后续需要使用这里的 Size 进行 World → Screen 投影。
	OptionalPaintGeometry = AllottedGeometry;

	// 创建当前需要绘制的排列结果集合。
	FArrangedChildren ArrangedChildren(EVisibility::Visible);

	// 调用 OnArrangeChildren，
	// 根据当前 Indicator 状态计算每个 Indicator / Arrow 的最终 Geometry。
	ArrangeChildren(AllottedGeometry, ArrangedChildren);

	// 当前已经使用的最大 Slate Layer。
	int32 MaxLayerId = LayerId;

	// 当前 Canvas 成为这些 Child Paint 调用的新 Parent。
	const FPaintArgs NewArgs = Args.WithNewParent(this);

	// 根据父级状态计算当前 Canvas 是否 Enabled。
	const bool bShouldBeEnabled = ShouldBeEnabled(bParentEnabled);

	// 按排列后的顺序绘制所有 Indicator 和 Arrow。
	for (const FArrangedWidget& CurWidget : ArrangedChildren.GetInternalArray())
	{
		// 当前 Widget 没有被裁剪区域完全剔除时才真正绘制。
		if (!IsChildWidgetCulled(MyCullingRect, CurWidget))
		{
			// 取得可修改的 Widget 指针。
			SWidget* MutableWidget = const_cast<SWidget*>(&CurWidget.Widget.Get());

			// 调用 Child Widget 自己的 Paint。
			const int32 CurWidgetsMaxLayerId = CurWidget.Widget->Paint(NewArgs, CurWidget.Geometry, MyCullingRect,
			                                                           OutDrawElements,

			                                                           // 开启严格绘制顺序时，
			                                                           // 后一个 Widget 从当前 MaxLayerId 继续绘制；
			                                                           // 否则允许多个 Widget 从相同 LayerId 开始，以利于 Slate Batching。
			                                                           bDrawElementsInOrder ? MaxLayerId : LayerId,

			                                                           InWidgetStyle, bShouldBeEnabled);

			// 更新当前使用到的最大 Layer。
			MaxLayerId = FMath::Max(MaxLayerId, CurWidgetsMaxLayerId);
		}
		else
		{
			//SlateGI - RemoveContent
			// 当前 Widget 被裁剪，不执行实际绘制。
		}
	}

	// 返回本次 Canvas 绘制最终使用到的最大 LayerId。
	return MaxLayerId;
}

// SActorCanvas 析构。
SActorCanvas::~SActorCanvas()
{
	// 取消未完成的加载；回调体持有 this 的弱引用，句柄继续存活只会白等。
	// 遍历所有仍然存在的 Indicator WidgetClass 异步加载任务。
	for (const TSharedPtr<FStreamableHandle>& Handle : IndicatorLoadHandles)
	{
		if (Handle.IsValid())
		{
			// Canvas 已经销毁，不再需要等待对应资源加载完成。
			Handle->CancelHandle();
		}
	}

	// 清空所有加载句柄。
	IndicatorLoadHandles.Reset();
}

// FGCObject 调试名称。
FString SActorCanvas::GetReferencerName() const
{
	return TEXT("SActorCanvas");
}

// 向 UE GC 报告当前 SActorCanvas 持有的 UObject 引用。
void SActorCanvas::AddReferencedObjects(FReferenceCollector& Collector)
{
	// SActorCanvas 本身不是 UObject，
	// 因此需要主动告诉 GC：AllIndicators 中的 Descriptor 仍然正在被使用。
	Collector.AddReferencedObjects(AllIndicators);
}

// IndicatorManager 新增 Indicator 时的回调。
void SActorCanvas::OnIndicatorAdded(UIndicatorDescriptor* Indicator)
{
	// 将 Descriptor 加入当前 Canvas 的完整 Indicator 集合。
	AllIndicators.Add(Indicator);

	// 此时对应 Widget 尚未完成创建 / 加载，
	// 先放入 InactiveIndicators。
	InactiveIndicators.Add(Indicator);

	// 开始为这个 Descriptor 创建真正的 UI。
	AddIndicatorForEntry(Indicator);
}

// IndicatorManager 移除 Indicator 时的回调。
void SActorCanvas::OnIndicatorRemoved(UIndicatorDescriptor* Indicator)
{
	// 先移除 / 回收对应 Widget 和 Slate Slot。
	RemoveIndicatorForEntry(Indicator);

	// 从完整 Indicator 集合中移除。
	AllIndicators.Remove(Indicator);

	// 如果它仍处于异步加载等未激活状态，
	// 同时从 InactiveIndicators 移除。
	InactiveIndicators.Remove(Indicator);
}

// 根据 Descriptor 开始准备对应 Indicator Widget。
void SActorCanvas::AddIndicatorForEntry(UIndicatorDescriptor* Indicator)
{
	// 异步加载指示器控件类，再从控件池取实例复用。
	// 取得 Descriptor 配置的软引用 WidgetClass。
	TSoftClassPtr<UUserWidget> IndicatorClass = Indicator->GetIndicatorClass();

	// 只有配置了有效软引用时才开始加载。
	if (!IndicatorClass.IsNull())
	{
		// 使用弱 UObject 指针传入异步回调，
		// 避免加载期间 Descriptor 被销毁后继续访问无效对象。
		TWeakObjectPtr<UIndicatorDescriptor> IndicatorPtr(Indicator);

		// 用 CreateSP 绑定，句柄死亡后回调不会执行；Slate 控件不是 UObject，不能用 CreateWeakLambda。
		// 获取 AssetManager 的全局 StreamableManager。
		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();

		// 异步请求加载 Indicator WidgetClass。
		const TSharedPtr<FStreamableHandle> LoadHandle = StreamableManager.RequestAsyncLoad(
			IndicatorClass.ToSoftObjectPath(),

			// SActorCanvas 是 Slate SharedRef 对象，
			// 使用 CreateSP 将异步完成回调绑定到当前 Canvas。
			FStreamableDelegate::CreateSP(StaticCastSharedRef<SActorCanvas>(AsShared()),
			                              &SActorCanvas::OnIndicatorClassLoaded, IndicatorPtr)
		);

		// 保存有效加载句柄，
		// Canvas 析构时可以统一取消尚未完成的任务。
		if (LoadHandle.IsValid())
		{
			IndicatorLoadHandles.Add(LoadHandle);
		}
	}
}

// Indicator WidgetClass 异步加载完成后的回调。
void SActorCanvas::OnIndicatorClassLoaded(TWeakObjectPtr<UIndicatorDescriptor> IndicatorPtr)
{
	// 尝试重新取得 Descriptor。
	UIndicatorDescriptor* LoadedIndicator = IndicatorPtr.Get();

	// 加载期间 Descriptor 已经失效，则不再继续。
	if (LoadedIndicator == nullptr)
	{
		return;
	}

	// 异步加载期间该指示器可能已被移除。
	// Descriptor 虽然 UObject 仍然有效，
	// 但业务上已经不属于当前 Canvas 时，同样不能继续创建 Widget。
	if (!AllIndicators.Contains(LoadedIndicator))
	{
		return;
	}

	// 从 Indicator WidgetPool 中取得可复用实例；
	// 如果对象池没有可用实例，则创建新的 UUserWidget。
	UUserWidget* IndicatorWidget = IndicatorPool.GetOrCreateInstance(
		TSubclassOf<UUserWidget>(LoadedIndicator->GetIndicatorClass().Get()));

	// 创建 / 获取失败则终止。
	if (IndicatorWidget == nullptr)
	{
		return;
	}

	// 如果具体 Widget 实现了统一 Indicator 接口，
	// 将 Descriptor 绑定给 Widget。
	if (IndicatorWidget->GetClass()->ImplementsInterface(UIndicatorWidgetInterface::StaticClass()))
	{
		IIndicatorWidgetInterface::Execute_BindIndicator(IndicatorWidget, LoadedIndicator);
	}

	// Descriptor 保存当前实际对应的 Widget 弱引用。
	LoadedIndicator->IndicatorWidget = IndicatorWidget;

	// Widget 已经准备完成，不再属于 Inactive 状态。
	InactiveIndicators.Remove(LoadedIndicator);

	// 为当前 Descriptor 创建一个 SActorCanvas::FSlot，
	// 并创建 SBox 作为 CanvasHost。
	AddActorSlot(LoadedIndicator)
	[
		// Descriptor 保存 CanvasHost 的弱引用，
		// 后续移除 Indicator 时可以根据它找到对应 Slot。
		SAssignNew(LoadedIndicator->CanvasHost, SBox)
		[
			// 将 UUserWidget 转换 / 获取对应 Slate Widget，
			// 作为 CanvasHost 的真正显示内容。
			IndicatorWidget->TakeWidget()
		]
	];
}

// 移除指定 Descriptor 对应的 Widget 和 Slate Slot。
void SActorCanvas::RemoveIndicatorForEntry(UIndicatorDescriptor* Indicator)
{
	// Descriptor 当前已经拥有实际 Widget 时进行解绑和回收。
	if (UUserWidget* IndicatorWidget = Indicator->IndicatorWidget.Get())
	{
		// 如果 Widget 实现了 Indicator 接口，
		// 在回收到对象池之前通知它解除 Descriptor / Gameplay 数据绑定。
		if (IndicatorWidget->GetClass()->ImplementsInterface(UIndicatorWidgetInterface::StaticClass()))
		{
			IIndicatorWidgetInterface::Execute_UnbindIndicator(IndicatorWidget, Indicator);
		}

		// Descriptor 不再指向这个 Widget。
		Indicator->IndicatorWidget = nullptr;

		// 将 Widget 放回对象池，而不是直接销毁。
		IndicatorPool.Release(IndicatorWidget);
	}

	// 尝试取得当前 Indicator 在 Canvas 中对应的 Slate Host。
	TSharedPtr<SWidget> CanvasHost = Indicator->CanvasHost.Pin();

	if (CanvasHost.IsValid())
	{
		// 从 CanvasChildren 中移除对应 Slot。
		RemoveActorSlot(CanvasHost.ToSharedRef());

		// 清空 Descriptor 对 CanvasHost 的弱引用。
		Indicator->CanvasHost.Reset();
	}
}

// 为指定 Descriptor 创建一个新的 SActorCanvas::FSlot。
SActorCanvas::FScopedWidgetSlotArguments SActorCanvas::AddActorSlot(UIndicatorDescriptor* Indicator)
{
	// 保存当前 Canvas 的弱引用，
	// 避免 Slot 添加完成回调反向强持有整个 Canvas。
	TWeakPtr<SActorCanvas> WeakCanvas = SharedThis(this);

	// 创建新的 FSlot 并加入 CanvasChildren。
	return FScopedWidgetSlotArguments{
		MakeUnique<FSlot>(Indicator), this->CanvasChildren, INDEX_NONE, [WeakCanvas](const FSlot*, int32)
		{
			// Slot 真正加入完成后，如果 Canvas 仍然存在，
			// 更新 ActiveTimer 状态。
			if (TSharedPtr<SActorCanvas> Canvas = WeakCanvas.Pin())
			{
				Canvas->UpdateActiveTimer();
			}
		}
	};
}

// 根据 Slate Widget 找到并移除对应的 Actor Slot。
int32 SActorCanvas::RemoveActorSlot(const TSharedRef<SWidget>& SlotWidget)
{
	// 遍历当前所有普通 Indicator Slot。
	for (int32 SlotIdx = 0; SlotIdx < CanvasChildren.Num(); ++SlotIdx)
	{
		// 找到拥有目标 Slate Widget 的 Slot。
		if (SlotWidget == CanvasChildren[SlotIdx].GetWidget())
		{
			// 从 CanvasChildren 中移除。
			CanvasChildren.RemoveAt(SlotIdx);

			// Indicator 数量发生变化，重新判断 ActiveTimer 是否还需要运行。
			UpdateActiveTimer();

			// 返回被移除 Slot 的索引。
			return SlotIdx;
		}
	}

	// 没找到对应 Slot。
	return -1;
}

// 根据 Indicator Widget 的尺寸和对齐方式，
// 计算 Widget 最终尺寸、相对于投影点的 Offset，以及 Clamp 所需 Padding。
void SActorCanvas::GetOffsetAndSize(const UIndicatorDescriptor* Indicator,
                                    FVector2D& OutSize,
                                    FVector2D& OutOffset,
                                    FVector2D& OutPaddingMin,
                                    FVector2D& OutPaddingMax) const
{
	//This might get used one day
	// 当前没有实际使用父级分配尺寸，
	// 保留该变量用于统一 Alignment 计算形式。
	FVector2D AllottedSize = FVector2D::ZeroVector;

	//grab the desired size of the child widget
	// 取得 Descriptor 对应的 CanvasHost。
	TSharedPtr<SWidget> CanvasHost = Indicator->CanvasHost.Pin();

	if (CanvasHost.IsValid())
	{
		// 使用 Widget 自身 DesiredSize 作为最终 Indicator Slot 尺寸。
		OutSize = CanvasHost->GetDesiredSize();
	}

	//handle horizontal alignment
	// 根据 Descriptor 的水平对齐方式，
	// 决定“投影点”对应 Widget 的左边、中心还是右边。
	switch (Indicator->GetHAlign())
	{
	case HAlign_Left: // same as Align_Top
		// 投影点对应 Widget 左侧。
		OutOffset.X = 0.0f;
		OutPaddingMin.X = 0.0f;
		OutPaddingMax.X = OutSize.X;
		break;

	case HAlign_Center:
		// 投影点对应 Widget 水平中心。
		OutOffset.X = (AllottedSize.X - OutSize.X) / 2.0f;
		OutPaddingMin.X = OutSize.X / 2.0f;
		OutPaddingMax.X = OutPaddingMin.X;
		break;

	case HAlign_Right: // same as Align_Bottom
		// 投影点对应 Widget 右侧。
		OutOffset.X = AllottedSize.X - OutSize.X;
		OutPaddingMin.X = OutSize.X;
		OutPaddingMax.X = 0.0f;
		break;
	}

	//Now, handle vertical alignment
	// 根据 Descriptor 的垂直对齐方式，
	// 决定“投影点”对应 Widget 的顶部、中心还是底部。
	switch (Indicator->GetVAlign())
	{
	case VAlign_Top:
		// 投影点对应 Widget 顶部。
		OutOffset.Y = 0.0f;
		OutPaddingMin.Y = 0.0f;
		OutPaddingMax.Y = OutSize.Y;
		break;

	case VAlign_Center:
		// 投影点对应 Widget 垂直中心。
		OutOffset.Y = (AllottedSize.Y - OutSize.Y) / 2.0f;
		OutPaddingMin.Y = OutSize.Y / 2.0f;
		OutPaddingMax.Y = OutPaddingMin.Y;
		break;

	case VAlign_Bottom:
		// 投影点对应 Widget 底部。
		OutOffset.Y = AllottedSize.Y - OutSize.Y;
		OutPaddingMin.Y = OutSize.Y;
		OutPaddingMax.Y = 0.0f;
		break;
	}
}

// 根据当前 Indicator 状态决定是否需要启动 ActiveTimer。
void SActorCanvas::UpdateActiveTimer()
{
	// 两种情况下需要持续更新：
	//
	// 1. 当前已经存在 Indicator，需要持续更新它们的投影位置；
	// 2. 还没有找到 IndicatorManager，需要持续尝试获取 Manager。
	const bool NeedsTicks = AllIndicators.Num() > 0 || !IndicatorComponentPtr.IsValid();

	// 需要更新，并且当前还没有注册 ActiveTimer。
	if (NeedsTicks && !TickHandle.IsValid())
	{
		// 以 0 间隔注册 ActiveTimer，
		// 使用 UpdateCanvas 驱动 Indicator 系统的持续更新。
		TickHandle = RegisterActiveTimer(0, FWidgetActiveTimerDelegate::CreateSP(this, &SActorCanvas::UpdateCanvas));
	}
}
