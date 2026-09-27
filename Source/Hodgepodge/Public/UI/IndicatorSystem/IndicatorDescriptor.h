// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// SceneComponent 定义。
// Indicator 使用 USceneComponent 作为世界空间中的跟随 / 投影目标。
#include "Components/SceneComponent.h"

// Slate 水平 / 垂直对齐枚举定义。
#include "Types/SlateEnums.h"

#include "IndicatorDescriptor.generated.h"

// Slate Widget 前向声明。
class SWidget;

// Indicator 描述对象前向声明。
class UIndicatorDescriptor;

// Indicator 管理组件前向声明。
class UHodgeIndicatorManagerComponent;

// UMG Widget 前向声明。
class UUserWidget;

struct FFrame;

// 场景视图投影数据。
// 用于将世界空间坐标转换到屏幕空间。
struct FSceneViewProjectionData;

// Indicator 世界空间 → 屏幕空间投影辅助结构。
// 根据 IndicatorDescriptor 中保存的目标组件、投影模式、偏移等信息，
// 计算 Indicator 最终应该位于屏幕上的位置。
struct FIndicatorProjection
{
	// 将指定 IndicatorDescriptor 描述的世界目标投影到屏幕空间。
	// InProjectionData 提供当前场景视图的投影信息，
	// ScreenSize 表示当前屏幕尺寸，
	// ScreenPositionWithDepth 输出最终屏幕位置以及深度信息。
	bool Project(const UIndicatorDescriptor& IndicatorDescriptor, const FSceneViewProjectionData& InProjectionData,
	             const FVector2f& ScreenSize, FVector& ScreenPositionWithDepth);
};

// Indicator 投影模式。
// 决定系统应该使用目标组件 / Actor 的哪个空间信息计算 Indicator 屏幕位置。
UENUM(BlueprintType)
enum class EActorCanvasProjectionMode : uint8
{
	// 使用 SceneComponent 自身的世界空间位置作为投影点。
	ComponentPoint,

	// 根据 SceneComponent 的世界空间包围盒计算 Indicator 位置。
	ComponentBoundingBox,

	// 根据 SceneComponent 投影到屏幕后形成的屏幕空间包围盒计算 Indicator 位置。
	ComponentScreenBoundingBox,

	// 根据 SceneComponent 所属 Actor 的世界空间包围盒计算 Indicator 位置。
	ActorBoundingBox,

	// 根据 SceneComponent 所属 Actor 投影后的屏幕空间包围盒计算 Indicator 位置。
	ActorScreenBoundingBox
};

/**
 * Describes and controls an active indicator.  It is highly recommended that your widget implements
 * IActorIndicatorWidget so that it can 'bind' to the associated data.
 *
 * 描述并控制一个当前处于活动状态的 Indicator。
 *
 * UIndicatorDescriptor 本身不是实际显示出来的 Widget，
 * 而是 Indicator 系统中一个 Indicator 的完整“描述数据”。
 *
 * 它负责描述：
 * - Indicator 绑定到哪个 SceneComponent；
 * - 使用哪个 Widget 类进行显示；
 * - Widget 需要绑定哪个业务数据对象；
 * - 世界目标应该如何投影到屏幕；
 * - Indicator 的对齐、偏移、屏幕边缘 Clamp 和箭头行为；
 * - Indicator 的排序优先级；
 * - 当前 Indicator 属于哪个 IndicatorManager。
 *
 * 推荐对应的 Widget 实现 IActorIndicatorWidget，
 * 从而能够与 Descriptor 中关联的数据进行绑定。
 */
UCLASS(BlueprintType)
class HODGEPODGE_API UIndicatorDescriptor : public UObject
{
	GENERATED_BODY()

public:
	// 构造 Indicator 描述对象。
	UIndicatorDescriptor()
	{
	}

public:
	// 获取当前 Indicator 关联的业务数据对象。
	// DataObject 与用于定位的 SceneComponent 相互独立，
	// 可以用于给 Indicator Widget 提供实际需要展示的数据。
	UFUNCTION(BlueprintCallable)
	UObject* GetDataObject() const { return DataObject; }

	// 设置当前 Indicator 关联的业务数据对象。
	UFUNCTION(BlueprintCallable)
	void SetDataObject(UObject* InDataObject) { DataObject = InDataObject; }

	// 获取当前 Indicator 跟随 / 投影使用的 SceneComponent。
	UFUNCTION(BlueprintCallable)
	USceneComponent* GetSceneComponent() const { return Component; }

	// 设置当前 Indicator 跟随 / 投影使用的 SceneComponent。
	UFUNCTION(BlueprintCallable)
	void SetSceneComponent(USceneComponent* InComponent) { Component = InComponent; }

	// 获取 Indicator 使用的 Component Socket 名称。
	// 可以让 Indicator 定位到 SceneComponent 的指定 Socket。
	UFUNCTION(BlueprintCallable)
	FName GetComponentSocketName() const { return ComponentSocketName; }

	// 设置 Indicator 使用的 Component Socket 名称。
	UFUNCTION(BlueprintCallable)
	void SetComponentSocketName(FName SocketName) { ComponentSocketName = SocketName; }

	// 获取当前 Indicator 应该使用的 UMG Widget 类。
	// 使用软类引用，避免 Descriptor 创建时立即同步加载对应 Widget 类。
	UFUNCTION(BlueprintCallable)
	TSoftClassPtr<UUserWidget> GetIndicatorClass() const { return IndicatorWidgetClass; }

	// 设置当前 Indicator 对应的 UMG Widget 类。
	UFUNCTION(BlueprintCallable)
	void SetIndicatorClass(TSoftClassPtr<UUserWidget> InIndicatorWidgetClass)
	{
		IndicatorWidgetClass = InIndicatorWidgetClass;
	}

public:
	// TODO Organize this better.
	// 当前 Descriptor 实际对应的 Indicator Widget 实例。
	// 使用弱引用，Descriptor 不通过该字段强制维持 Widget 生命周期。
	TWeakObjectPtr<UUserWidget> IndicatorWidget;

public:
	// 设置是否允许在 Indicator 对应的 SceneComponent 失效后自动移除当前 Indicator。
	UFUNCTION(BlueprintCallable)
	void SetAutoRemoveWhenIndicatorComponentIsNull(bool CanAutomaticallyRemove)
	{
		bAutoRemoveWhenIndicatorComponentIsNull = CanAutomaticallyRemove;
	}

	// 获取是否允许在 SceneComponent 失效后自动移除 Indicator。
	UFUNCTION(BlueprintCallable)
	bool GetAutoRemoveWhenIndicatorComponentIsNull() const { return bAutoRemoveWhenIndicatorComponentIsNull; }

	// 判断当前 Indicator 是否满足自动移除条件。
	bool CanAutomaticallyRemove() const
	{
		// 必须开启自动移除，并且当前 SceneComponent 已经无效。
		return bAutoRemoveWhenIndicatorComponentIsNull && !IsValid(GetSceneComponent());
	}

public:
	// Layout Properties
	//=======================
	// Indicator 布局相关配置。
	// 用于控制可见性、投影方式、对齐方式、屏幕 Clamp 和位置偏移等行为。

	// 获取当前 Indicator 是否应该显示。
	UFUNCTION(BlueprintCallable)
	bool GetIsVisible() const { return IsValid(GetSceneComponent()) && bVisible; }

	// 设置当前 Indicator 期望的显示状态。
	UFUNCTION(BlueprintCallable)
	void SetDesiredVisibility(bool InVisible)
	{
		bVisible = InVisible;
	}

	// 获取当前 Indicator 使用的世界 → 屏幕投影模式。
	UFUNCTION(BlueprintCallable)
	EActorCanvasProjectionMode GetProjectionMode() const { return ProjectionMode; }

	// 设置当前 Indicator 使用的投影模式。
	UFUNCTION(BlueprintCallable)
	void SetProjectionMode(EActorCanvasProjectionMode InProjectionMode)
	{
		ProjectionMode = InProjectionMode;
	}

	// Horizontal alignment to the point in space to place the indicator at.
	// 获取 Indicator 相对于最终屏幕投影点的水平对齐方式。
	UFUNCTION(BlueprintCallable)
	EHorizontalAlignment GetHAlign() const { return HAlignment; }

	// 设置 Indicator 相对于最终屏幕投影点的水平对齐方式。
	UFUNCTION(BlueprintCallable)
	void SetHAlign(EHorizontalAlignment InHAlignment)
	{
		HAlignment = InHAlignment;
	}

	// Vertical alignment to the point in space to place the indicator at.
	// 获取 Indicator 相对于最终屏幕投影点的垂直对齐方式。
	UFUNCTION(BlueprintCallable)
	EVerticalAlignment GetVAlign() const { return VAlignment; }

	// 设置 Indicator 相对于最终屏幕投影点的垂直对齐方式。
	UFUNCTION(BlueprintCallable)
	void SetVAlign(EVerticalAlignment InVAlignment)
	{
		VAlignment = InVAlignment;
	}

	// Clamp the indicator to the edge of the screen?
	// 获取目标离开屏幕范围后，Indicator 是否需要被限制在屏幕边缘。
	UFUNCTION(BlueprintCallable)
	bool GetClampToScreen() const { return bClampToScreen; }

	// 设置目标离开屏幕范围后，Indicator 是否需要被限制在屏幕边缘。
	UFUNCTION(BlueprintCallable)
	void SetClampToScreen(bool bValue)
	{
		bClampToScreen = bValue;
	}

	// Show the arrow if clamping to the edge of the screen?
	// 获取 Indicator 被 Clamp 到屏幕边缘时是否显示方向箭头。
	UFUNCTION(BlueprintCallable)
	bool GetShowClampToScreenArrow() const { return bShowClampToScreenArrow; }

	// 设置 Indicator 被 Clamp 到屏幕边缘时是否显示方向箭头。
	UFUNCTION(BlueprintCallable)
	void SetShowClampToScreenArrow(bool bValue)
	{
		bShowClampToScreenArrow = bValue;
	}

	// The position offset for the indicator in world space.
	// 获取 Indicator 在世界空间中的位置偏移。
	// 该偏移通常在执行 World → Screen 投影之前应用。
	UFUNCTION(BlueprintCallable)
	FVector GetWorldPositionOffset() const { return WorldPositionOffset; }

	// 设置 Indicator 的世界空间位置偏移。
	UFUNCTION(BlueprintCallable)
	void SetWorldPositionOffset(FVector Offset)
	{
		WorldPositionOffset = Offset;
	}

	// The position offset for the indicator in screen space.
	// 获取 Indicator 在屏幕空间中的位置偏移。
	// 该偏移通常在完成 World → Screen 投影之后应用。
	UFUNCTION(BlueprintCallable)
	FVector2D GetScreenSpaceOffset() const { return ScreenSpaceOffset; }

	// 设置 Indicator 的屏幕空间位置偏移。
	UFUNCTION(BlueprintCallable)
	void SetScreenSpaceOffset(FVector2D Offset)
	{
		ScreenSpaceOffset = Offset;
	}

	// 获取包围盒锚点。
	// 当使用 BoundingBox 类型投影模式时，
	// 该值用于描述 Indicator 应该取包围盒中的哪个相对位置作为锚点。
	UFUNCTION(BlueprintCallable)
	FVector GetBoundingBoxAnchor() const { return BoundingBoxAnchor; }

	// 设置包围盒锚点。
	UFUNCTION(BlueprintCallable)
	void SetBoundingBoxAnchor(FVector InBoundingBoxAnchor)
	{
		BoundingBoxAnchor = InBoundingBoxAnchor;
	}

public:
	// Sorting Properties
	//=======================
	// Indicator 排序相关配置。

	// Allows sorting the indicators (after they are sorted by depth), to allow some group of indicators
	// to always be in front of others.
	// 获取 Indicator 的显示优先级。
	// 在完成深度排序之后，可以继续利用 Priority 调整不同 Indicator 之间的前后显示关系。
	UFUNCTION(BlueprintCallable)
	int32 GetPriority() const { return Priority; }

	// 设置 Indicator 的显示优先级。
	UFUNCTION(BlueprintCallable)
	void SetPriority(int32 InPriority)
	{
		Priority = InPriority;
	}

public:
	// 获取当前 Descriptor 所属的 IndicatorManagerComponent。
	UHodgeIndicatorManagerComponent* GetIndicatorManagerComponent() { return ManagerPtr.Get(); }

	// 设置当前 Descriptor 所属的 IndicatorManagerComponent。
	void SetIndicatorManagerComponent(UHodgeIndicatorManagerComponent* InManager);

	// 主动将当前 Indicator 从所属 IndicatorManager 中注销。
	UFUNCTION(BlueprintCallable)
	void UnregisterIndicator();

private:
	// 当前 Indicator 自身期望的显示状态。
	UPROPERTY()
	bool bVisible = true;

	// 目标位于屏幕外时，是否将 Indicator Clamp 到屏幕边缘。
	UPROPERTY()
	bool bClampToScreen = false;

	// Indicator 被 Clamp 到屏幕边缘时是否显示方向箭头。
	UPROPERTY()
	bool bShowClampToScreenArrow = false;

	// 是否覆盖系统正常计算得到的屏幕位置。
	UPROPERTY()
	bool bOverrideScreenPosition = false;

	// SceneComponent 失效时是否允许自动移除当前 Indicator。
	UPROPERTY()
	bool bAutoRemoveWhenIndicatorComponentIsNull = false;

	// 当前 Indicator 使用的投影模式。
	UPROPERTY()
	EActorCanvasProjectionMode ProjectionMode = EActorCanvasProjectionMode::ComponentPoint;

	// Indicator 相对于投影锚点的水平对齐方式。
	UPROPERTY()
	TEnumAsByte<EHorizontalAlignment> HAlignment = HAlign_Center;

	// Indicator 相对于投影锚点的垂直对齐方式。
	UPROPERTY()
	TEnumAsByte<EVerticalAlignment> VAlignment = VAlign_Center;

	// Indicator 排序优先级。
	UPROPERTY()
	int32 Priority = 0;

	// 包围盒投影时使用的归一化锚点。
	// 默认 (0.5, 0.5, 0.5) 表示包围盒中心位置。
	UPROPERTY()
	FVector BoundingBoxAnchor = FVector(0.5, 0.5, 0.5);

	// World → Screen 投影完成后额外应用的屏幕空间偏移。
	UPROPERTY()
	FVector2D ScreenSpaceOffset = FVector2D(0, 0);

	// World → Screen 投影之前额外应用的世界空间偏移。
	UPROPERTY()
	FVector WorldPositionOffset = FVector(0, 0, 0);

private:
	// SActorCanvas 可以直接访问当前 Descriptor 的私有成员。
	// 表明真正的 Slate Indicator 布局 / 投影层会直接读取这些描述数据。
	friend class SActorCanvas;

	// 当前 Indicator 关联的业务数据对象。
	// Widget 可以通过该对象获取需要显示的 Gameplay 数据。
	UPROPERTY()
	TObjectPtr<UObject> DataObject;

	// 当前 Indicator 跟随 / 投影使用的 SceneComponent。
	UPROPERTY()
	TObjectPtr<USceneComponent> Component;

	// 当前 Indicator 使用的 Component Socket 名称。
	// NAME_None 表示不指定 Socket。
	UPROPERTY()
	FName ComponentSocketName = NAME_None;

	// 当前 Indicator 应该实例化的 UMG Widget 类。
	// 使用 SoftClassPtr 延迟对具体 Widget 资源的加载依赖。
	UPROPERTY()
	TSoftClassPtr<UUserWidget> IndicatorWidgetClass;

	// 当前 Descriptor 所属的 IndicatorManagerComponent。
	// 使用弱引用避免 Descriptor 与 Manager 之间形成不必要的强生命周期依赖。
	UPROPERTY()
	TWeakObjectPtr<UHodgeIndicatorManagerComponent> ManagerPtr;

	// 当前 Indicator 对应的 Slate 内容 Widget。
	// 使用弱引用，不负责维持 Slate Widget 生命周期。
	TWeakPtr<SWidget> Content;

	// 当前 Indicator 所在 ActorCanvas 中对应的 Slate Host。
	// 使用弱引用保存，不负责维持 Canvas Host 生命周期。
	TWeakPtr<SWidget> CanvasHost;
};
