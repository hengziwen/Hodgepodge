// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// 上游用 FAsyncMixin 做指示器控件的异步加载；本项目不引入 AsyncMixin 插件，
// 改为 FStreamableManager 直接发起异步加载，句柄由 IndicatorLoadHandles 持有。

// UMG Widget 对象池。
// Indicator Widget 可以通过对象池复用，避免频繁创建 / 销毁 UUserWidget。
#include "Blueprint/UserWidgetPool.h"

// Slate Panel 基类。
// SActorCanvas 本质上是一个自定义 Slate 容器，负责排列多个 Indicator Widget。
#include "Widgets/SPanel.h"

// Slate ActiveTimer 句柄。
// 用于驱动 Indicator Canvas 的持续更新。
class FActiveTimerHandle;

// Slate 子控件排列结果集合。
class FArrangedChildren;

// Slate 子控件集合接口。
class FChildren;

// Slate 绘制参数。
class FPaintArgs;

// GC 引用收集器。
// SActorCanvas 通过 FGCObject 主动向 UE GC 报告自己持有的 UObject。
class FReferenceCollector;

// Slate 裁剪矩形。
class FSlateRect;

// Slate 绘制元素列表。
class FSlateWindowElementList;

// Slate Widget 样式。
class FWidgetStyle;

// StreamableManager 异步加载句柄。
// 用于持有 Indicator WidgetClass 的异步加载任务。
struct FStreamableHandle;

// 单个 Indicator 的描述对象。
class UIndicatorDescriptor;

// 当前玩家对应的 Indicator 管理组件。
class UHodgeIndicatorManagerComponent;

// Slate Brush。
// 当前 Canvas 使用它绘制屏幕边缘 Indicator 的方向箭头。
struct FSlateBrush;

// Indicator 系统真正的 Slate 布局容器。
//
// SPanel：
// 负责管理、排列和绘制多个 Indicator Slate Widget。
//
// FGCObject：
// 由于 SActorCanvas 本身不是 UObject，
// 但内部持有 UIndicatorDescriptor 等 UObject，
// 因此通过 FGCObject 主动参与 UE GC 引用收集，避免这些对象被错误回收。
class SActorCanvas : public SPanel, public FGCObject
{
public:
	/** ActorCanvas-specific slot class */
	// 每一个实际 Indicator Widget 在 SActorCanvas 中对应的 Slot。
	//
	// Slot 除了保存 Slate Child Widget，
	// 还缓存该 Indicator 的屏幕位置、深度、优先级、可见性、
	// 是否位于摄像机前方以及是否被 Clamp 等运行时布局状态。
	class FSlot : public TSlotBase<FSlot>
	{
	public:
		// 为指定 IndicatorDescriptor 创建对应的 Canvas Slot。
		FSlot(UIndicatorDescriptor* InIndicator)
			: TSlotBase<FSlot>()
			  // 保存当前 Slot 对应的 IndicatorDescriptor。
			  , Indicator(InIndicator)
			  // 默认屏幕位置为 (0, 0)。
			  , ScreenPosition(FVector2D::ZeroVector)
			  // 默认深度为 0。
			  , Depth(0)
			  // 默认排序优先级为 0。
			  , Priority(0.f)
			  // 默认允许显示 Indicator。
			  , bIsIndicatorVisible(true)
			  // 默认认为目标位于摄像机前方。
			  , bInFrontOfCamera(true)
			  // 创建时还没有有效的屏幕投影位置。
			  , bHasValidScreenPosition(false)
			  // 新创建 Slot 默认标记为 Dirty，需要进行布局更新。
			  , bDirty(true)
			  // 上一帧默认没有发生屏幕边缘 Clamp。
			  , bWasIndicatorClamped(false)
			  // 初始 Clamp 状态没有发生变化。
			  , bWasIndicatorClampedStatusChanged(false)
		{
		}

		// 声明当前自定义 Slate Slot 的参数结构。
		SLATE_SLOT_BEGIN_ARGS(FSlot, TSlotBase<FSlot>)
		SLATE_SLOT_END_ARGS()

		// 继续使用 TSlotBase 提供的 Construct。
		using TSlotBase<FSlot>::Construct;

		// 获取当前 Indicator 自身是否期望显示。
		bool GetIsIndicatorVisible() const { return bIsIndicatorVisible; }

		// 设置当前 Indicator 自身是否期望显示。
		void SetIsIndicatorVisible(bool bVisible)
		{
			// 只有状态发生变化时才标记 Slot 为 Dirty。
			if (bIsIndicatorVisible != bVisible)
			{
				bIsIndicatorVisible = bVisible;
				bDirty = true;
			}

			// 根据最新状态刷新真正 Slate Widget 的 Visibility。
			RefreshVisibility();
		}

		// 获取当前 Indicator 投影后的屏幕坐标。
		FVector2D GetScreenPosition() const { return ScreenPosition; }

		// 更新当前 Indicator 的屏幕坐标。
		void SetScreenPosition(FVector2D InScreenPosition)
		{
			// 坐标发生变化时标记 Slot 为 Dirty。
			if (ScreenPosition != InScreenPosition)
			{
				ScreenPosition = InScreenPosition;
				bDirty = true;
			}
		}

		// 获取当前 Indicator 相对于摄像机的深度 / 距离信息。
		double GetDepth() const { return Depth; }

		// 更新当前 Indicator 的深度。
		void SetDepth(double InDepth)
		{
			// 深度发生变化时标记 Slot 为 Dirty，
			// 后续可能需要重新排序。
			if (Depth != InDepth)
			{
				Depth = InDepth;
				bDirty = true;
			}
		}

		// 获取当前 Indicator 的排序优先级。
		int32 GetPriority() const { return Priority; }

		// 设置当前 Indicator 的排序优先级。
		void SetPriority(int32 InPriority)
		{
			// Priority 发生变化时标记 Slot 为 Dirty，
			// 后续可能需要重新调整 Indicator 前后顺序。
			if (Priority != InPriority)
			{
				Priority = InPriority;
				bDirty = true;
			}
		}

		// 获取 Indicator 对应目标是否位于摄像机前方。
		bool GetInFrontOfCamera() const { return bInFrontOfCamera; }

		// 更新目标是否位于摄像机前方。
		void SetInFrontOfCamera(bool bInFront)
		{
			// 状态变化时标记 Slot 为 Dirty。
			if (bInFrontOfCamera != bInFront)
			{
				bInFrontOfCamera = bInFront;
				bDirty = true;
			}

			// 根据最新状态重新刷新 Widget 可见性。
			RefreshVisibility();
		}

		// 当前 Indicator 是否已经拥有有效的屏幕投影坐标。
		bool HasValidScreenPosition() const { return bHasValidScreenPosition; }

		// 设置当前 Indicator 是否拥有有效屏幕坐标。
		void SetHasValidScreenPosition(bool bValidScreenPosition)
		{
			// 投影有效状态变化时标记 Slot 为 Dirty。
			if (bHasValidScreenPosition != bValidScreenPosition)
			{
				bHasValidScreenPosition = bValidScreenPosition;
				bDirty = true;
			}

			// 投影无效时需要同步隐藏对应 Widget。
			RefreshVisibility();
		}

		// 当前 Slot 是否存在需要重新处理的状态变化。
		bool bIsDirty() const { return bDirty; }

		// 清除 Dirty 标记，表示当前状态已经处理完成。
		void ClearDirtyFlag()
		{
			bDirty = false;
		}

		// 获取上一帧 Indicator 是否被 Clamp 到屏幕边缘。
		bool WasIndicatorClamped() const { return bWasIndicatorClamped; }

		// 缓存当前 Indicator 是否发生了屏幕边缘 Clamp。
		void SetWasIndicatorClamped(bool bWasClamped) const
		{
			// Clamp 状态发生变化时，同时记录“Clamp 状态发生变化”。
			if (bWasClamped != bWasIndicatorClamped)
			{
				bWasIndicatorClamped = bWasClamped;
				bWasIndicatorClampedStatusChanged = true;
			}
		}

		// 获取本次更新中 Clamp 状态是否发生变化。
		bool WasIndicatorClampedStatusChanged() const { return bWasIndicatorClampedStatusChanged; }

		// 清除 Clamp 状态变化标记。
		void ClearIndicatorClampedStatusChangedFlag()
		{
			bWasIndicatorClampedStatusChanged = false;
		}

	private:
		// 根据 Indicator 自身显示状态和投影有效性，
		// 刷新实际 Slate Widget 的 Visibility。
		void RefreshVisibility()
		{
			// Indicator 必须自身允许显示，并且已经拥有有效屏幕坐标，
			// 才允许真正显示对应 Widget。
			const bool bIsVisible = bIsIndicatorVisible && bHasValidScreenPosition;

			// 可见时使用 SelfHitTestInvisible：
			// Widget 自身不参与 HitTest，但不会因此隐藏。
			//
			// 不可见时使用 Collapsed：
			// 完全隐藏并且不参与布局。
			GetWidget()->SetVisibility(bIsVisible ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed);
		}

		//Kept Alive by SActorCanvas::AddReferencedObjects
		// 当前 Slot 对应的 IndicatorDescriptor。
		// 该字段是裸 UObject 指针，
		// 生命周期由 SActorCanvas::AddReferencedObjects 主动报告给 UE GC 进行保护。
		UIndicatorDescriptor* Indicator;

		// 当前 Indicator 投影后的屏幕位置。
		FVector2D ScreenPosition;

		// 当前 Indicator 与摄像机之间的深度 / 距离。
		double Depth;

		// 当前 Indicator 的排序优先级。
		int32 Priority;

		// Indicator 自身是否期望显示。
		uint8 bIsIndicatorVisible : 1;

		// Indicator 对应目标是否位于摄像机前方。
		uint8 bInFrontOfCamera : 1;

		// 当前 Indicator 是否拥有有效屏幕投影位置。
		uint8 bHasValidScreenPosition : 1;

		// 当前 Slot 数据是否发生变化，需要重新处理布局 / 排序等逻辑。
		uint8 bDirty : 1;

		/** 
		 * Cached & frame-deferred value of whether the indicator was visually screen clamped last frame or not; 
		 * Semi-hacky mutable implementation as it is cached during a const paint operation
		 */
		// 缓存上一帧 Indicator 是否因为超出屏幕范围而被 Clamp 到屏幕边缘。
		//
		// 由于该值会在 const 的 Paint 阶段被更新，
		// 因此这里使用 mutable 允许在 const 函数中修改缓存状态。
		mutable uint8 bWasIndicatorClamped : 1;

		// 标记当前帧 Clamp 状态相对于上一帧是否发生变化。
		mutable uint8 bWasIndicatorClampedStatusChanged : 1;

		// 允许 SActorCanvas 直接访问 Slot 内部运行时状态。
		friend class SActorCanvas;
	};

	/** ActorCanvas-specific slot class */
	// 屏幕边缘方向箭头对应的专用 Slate Slot。
	// 与普通 Indicator Slot 分开维护。
	class FArrowSlot : public TSlotBase<FArrowSlot>
	{
	};

	/** Begin the arguments for this slate widget */
	// 声明 SActorCanvas 的 Slate 构造参数。
	SLATE_BEGIN_ARGS(SActorCanvas)
		{
			// 默认整个 Canvas 不参与 HitTest，
			// 避免世界空间 Indicator 层阻挡其他 UI / Gameplay 输入。
			_Visibility = EVisibility::HitTestInvisible;
		}

		/** Indicates that we have a slot that this widget supports */
		// 允许通过 Slate 参数向 SActorCanvas 添加自定义 FSlot。
		SLATE_SLOT_ARGUMENT(SActorCanvas::FSlot, Slots)

		/** This always goes at the end */
	SLATE_END_ARGS()

	// 构造 SActorCanvas。
	SActorCanvas()
	// 普通 Indicator Widget 子节点集合。
		: CanvasChildren(this)
		  // 屏幕边缘方向箭头子节点集合。
		  , ArrowChildren(this)
		  // 对外统一暴露的组合子节点集合。
		  , AllChildren(this)
	{
		// 将普通 Indicator 子节点加入统一 Children 集合。
		AllChildren.AddChildren(CanvasChildren);

		// 将屏幕边缘箭头子节点加入统一 Children 集合。
		AllChildren.AddChildren(ArrowChildren);
	}

	/** 取消所有进行中的指示器控件异步加载，避免句柄在 Canvas 销毁后继续存活。 */
	// SActorCanvas 析构时负责清理异步加载等运行时资源。
	~SActorCanvas();

	// 初始化 SActorCanvas。
	//
	// InArgs：
	// Slate 构造参数。
	//
	// InCtx：
	// 当前 Canvas 所属 LocalPlayer 的上下文，
	// 用于找到 PlayerController、IndicatorManager 以及玩家对应的视图信息。
	//
	// ActorCanvasArrowBrush：
	// Indicator 被 Clamp 到屏幕边缘时使用的默认箭头样式。
	void Construct(const FArguments& InArgs, const FLocalPlayerContext& InCtx,
	               const FSlateBrush* ActorCanvasArrowBrush);

	// SWidget Interface

	// 根据当前 Canvas 尺寸、Indicator 屏幕位置、对齐、Clamp 等信息，
	// 计算并排列所有普通 Indicator 和箭头子 Widget 的最终 Slate Geometry。
	virtual void
	OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;

	// Canvas 自身不主动请求固定 DesiredSize，
	// 实际大小由父级布局决定。
	virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D::ZeroVector; }

	// 返回当前 Canvas 的所有子节点。
	// 包括普通 Indicator 和屏幕边缘箭头。
	virtual FChildren* GetChildren() override { return &AllChildren; }

	// 绘制当前 Canvas 中的 Indicator。
	// 可以在这里根据深度、优先级、Clamp 状态等决定最终绘制顺序和附加表现。
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	                      FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	                      bool bParentEnabled) const;

	// End SWidget

	// 设置是否严格按照 Indicator 被加入 Canvas 的顺序生成 Draw Element。
	// 开启后可以控制绘制顺序，但会关闭部分 Slate Batching，从而增加 DrawCall。
	void SetDrawElementsInOrder(bool bInDrawElementsInOrder) { bDrawElementsInOrder = bInDrawElementsInOrder; }

	// FGCObject 接口：
	// 返回当前 GC 引用对象的调试名称。
	virtual FString GetReferencerName() const override;

	// FGCObject 接口：
	// 将 SActorCanvas 持有的 UObject 引用主动报告给 UE GC，
	// 防止 Slate 中使用的 IndicatorDescriptor 被错误回收。
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
	// IndicatorManager 新增 Descriptor 时的事件回调。
	void OnIndicatorAdded(UIndicatorDescriptor* Indicator);

	// IndicatorManager 移除 Descriptor 时的事件回调。
	void OnIndicatorRemoved(UIndicatorDescriptor* Indicator);

	// 为指定 Descriptor 真正创建 / 加载对应 Indicator Widget，
	// 并将其加入当前 ActorCanvas。
	void AddIndicatorForEntry(UIndicatorDescriptor* Indicator);

	// 从当前 ActorCanvas 中移除指定 Descriptor 对应的 Indicator Widget / Slot。
	void RemoveIndicatorForEntry(UIndicatorDescriptor* Indicator);

	/** 指示器控件类异步加载完成后的回调，由 FStreamableManager 在加载完成时触发。 */
	// IndicatorWidgetClass 异步加载完成后继续执行 Indicator Widget 创建 / 加入 Canvas 的逻辑。
	void OnIndicatorClassLoaded(TWeakObjectPtr<UIndicatorDescriptor> IndicatorPtr);

	// Slate Panel 添加 FSlot 时使用的作用域 Slot 参数类型。
	using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;

	// 为指定 Descriptor 在 CanvasChildren 中创建一个新的普通 Indicator Slot。
	FScopedWidgetSlotArguments AddActorSlot(UIndicatorDescriptor* Indicator);

	// 根据 Slate Widget 从 CanvasChildren 中移除对应的 Indicator Slot。
	int32 RemoveActorSlot(const TSharedRef<SWidget>& SlotWidget);

	// 设置当前 Canvas 是否存在需要显示的 Indicator。
	// 用于控制 ActiveTimer 等更新逻辑是否需要持续运行。
	void SetShowAnyIndicators(bool bIndicators);

	// ActiveTimer 更新函数。
	// 负责持续更新 Indicator 的投影位置、可见性、深度、优先级等运行时状态。
	EActiveTimerReturnType UpdateCanvas(double InCurrentTime, float InDeltaTime);

	/** Helper function for calculating the offset */
	// 根据 Descriptor 的对齐方式、Widget DesiredSize 等信息，
	// 计算 Indicator 最终布局需要使用的尺寸、位置偏移以及屏幕边缘 Padding。
	void GetOffsetAndSize(const UIndicatorDescriptor* Indicator,
	                      FVector2D& OutSize,
	                      FVector2D& OutOffset,
	                      FVector2D& OutPaddingMin,
	                      FVector2D& OutPaddingMax) const;

	// 根据当前是否存在 Indicator，
	// 注册、更新或停止用于驱动 Canvas 的 Slate ActiveTimer。
	void UpdateActiveTimer();

private:
	// 当前 SActorCanvas 已知的所有 IndicatorDescriptor。
	// TObjectPtr 让这些 UObject 引用可以被 UE 的对象系统正确追踪。
	TArray<TObjectPtr<UIndicatorDescriptor>> AllIndicators;

	// 当前暂时没有激活 / 显示的 IndicatorDescriptor。
	// 可用于记录尚未创建 Widget、等待加载或暂时不可用的 Indicator。
	TArray<UIndicatorDescriptor*> InactiveIndicators;

	// 当前 SActorCanvas 所属的本地玩家上下文。
	// 用于访问 LocalPlayer、PlayerController 以及当前玩家对应的 View。
	FLocalPlayerContext LocalPlayerContext;

	// 当前 LocalPlayer / Controller 对应的 IndicatorManagerComponent。
	// Canvas 通过它获取现有 Indicator，并监听 Indicator Added / Removed。
	TWeakObjectPtr<UHodgeIndicatorManagerComponent> IndicatorComponentPtr;

	/** All the slots in this canvas */
	// 当前所有普通 Indicator Widget 对应的 Slate Slot。
	TPanelChildren<FSlot> CanvasChildren;

	// 当前所有屏幕边缘方向箭头对应的 Slate Slot。
	// mutable 是因为部分箭头状态可能需要在 const 布局 / Paint 阶段更新。
	mutable TPanelChildren<FArrowSlot> ArrowChildren;

	// 将 CanvasChildren 与 ArrowChildren 合并成统一的 Slate Children 集合，
	// 供 GetChildren() 返回给 Slate 框架。
	FCombinedChildren AllChildren;

	// Indicator UUserWidget 对象池。
	// 用于复用 Indicator Widget，减少频繁 CreateWidget / Destroy 带来的开销。
	FUserWidgetPool IndicatorPool;

	// 屏幕边缘 Clamp Indicator 使用的默认箭头 Brush。
	// 该指针来自外部 UIndicatorLayer 的 ArrowBrush。
	const FSlateBrush* ActorCanvasArrowBrush = nullptr;

	// 下一次需要使用的 Arrow Slot 索引。
	// 用于在当前帧排列多个 Clamp Indicator 的方向箭头。
	mutable int32 NextArrowIndex = 0;

	// 上一次更新时使用到的 Arrow Slot 索引范围。
	// 用于管理和复用当前 Canvas 中已有的 Arrow Slot。
	mutable int32 ArrowIndexLastUpdate = 0;

	/** Whether to draw elements in the order they were added to canvas. Note: Enabling this will disable batching and will cause a greater number of drawcalls */
	// 是否严格按照元素加入 Canvas 的顺序绘制。
	// 开启后可以获得更明确的绘制顺序，
	// 但会破坏 Slate 的部分批处理能力并增加 DrawCall。
	bool bDrawElementsInOrder = false;

	// 当前 Canvas 是否存在任何需要显示的 Indicator。
	// 可以用于决定是否需要持续运行 ActiveTimer。
	bool bShowAnyIndicators = false;

	// 缓存最近一次 Paint 阶段使用的 Canvas Geometry。
	// UpdateCanvas 等逻辑可以利用该 Geometry 获得当前实际屏幕布局尺寸。
	mutable TOptional<FGeometry> OptionalPaintGeometry;

	// 当前用于驱动 Indicator Canvas 更新的 Slate ActiveTimer 句柄。
	TSharedPtr<FActiveTimerHandle> TickHandle;

	/** 进行中的指示器控件异步加载句柄，Canvas 销毁时统一取消。 */
	// 保存所有尚未完成的 IndicatorWidgetClass 异步加载任务。
	// Canvas 销毁时可以统一取消，避免异步回调访问已经失效的 Canvas。
	TArray<TSharedPtr<FStreamableHandle>> IndicatorLoadHandles;
};
