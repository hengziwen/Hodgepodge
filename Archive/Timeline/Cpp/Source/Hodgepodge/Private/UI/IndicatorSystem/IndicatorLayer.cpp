// Copyright Epic Games, Inc. All Rights Reserved.

// UIndicatorLayer 定义。
// 作为 Indicator 系统在 UMG Widget Tree 中的入口和容器。
#include "UI/IndicatorSystem/IndicatorLayer.h"

// Indicator 系统底层的 Slate Canvas。
// 真正负责当前 LocalPlayer 的 Indicator 布局与显示。
#include "UI/IndicatorSystem/SActorCanvas.h"

// Slate 基础布局控件 SBox。
// 当处于设计器模式或无法创建 SActorCanvas 时，作为安全的占位 Widget 返回。
#include "Widgets/Layout/SBox.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IndicatorLayer)

// Slate Widget 前向声明。
class SWidget;

/////////////////////////////////////////////////////
// UIndicatorLayer

// 构造 Indicator Layer。
UIndicatorLayer::UIndicatorLayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 将当前 Widget 标记为变量，
	// 允许在 Widget Blueprint 等环境中将该 Widget 作为变量进行访问。
	bIsVariable = true;

	// IndicatorLayer 自身以及其子内容不参与鼠标 / 点击等 Hit Test，
	// 避免世界 Indicator UI 阻挡玩家对其他 UI 或游戏世界的输入。
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

// 释放 UIndicatorLayer 持有的底层 Slate 资源。
void UIndicatorLayer::ReleaseSlateResources(bool bReleaseChildren)
{
	// 先执行 UWidget 基类的 Slate 资源释放逻辑。
	Super::ReleaseSlateResources(bReleaseChildren);

	// 释放当前 UIndicatorLayer 对 SActorCanvas 的共享引用。
	// 当 Widget 被销毁或重建时，避免继续持有旧的 Slate Canvas。
	MyActorCanvas.Reset();
}

// 根据当前 UIndicatorLayer 构建真正用于显示的底层 Slate Widget。
TSharedRef<SWidget> UIndicatorLayer::RebuildWidget()
{
	// 运行时才创建真正的 SActorCanvas。
	// Widget Blueprint 设计器环境中不执行依赖 LocalPlayer 的运行时逻辑。
	if (!IsDesignTime())
	{
		// 获取当前 UIndicatorLayer 所属的 LocalPlayer。
		// Indicator 是与具体玩家视角相关的 UI，因此需要明确当前 Canvas 属于哪个本地玩家。
		ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();

		// 确保当前 Widget 拥有有效的 LocalPlayer。
		// 没有 LocalPlayer 时无法正确建立当前玩家对应的 Indicator Canvas。
		if (ensureMsgf(LocalPlayer, TEXT("Attempting to rebuild a UActorCanvas without a valid LocalPlayer!")))
		{
			// 创建真正负责 Indicator 显示和布局的 SActorCanvas。
			//
			// FLocalPlayerContext(LocalPlayer)：
			// 将当前本地玩家上下文传递给 SActorCanvas，
			// 使其能够获取该玩家对应的 Controller、Viewport、投影数据等运行时信息。
			//
			// &ArrowBrush：
			// 将 UIndicatorLayer 配置的默认屏幕边缘箭头样式传递给 SActorCanvas。
			MyActorCanvas = SNew(SActorCanvas, FLocalPlayerContext(LocalPlayer), &ArrowBrush);

			// 将创建完成的 SActorCanvas 作为当前 UWidget 对应的底层 Slate Widget 返回。
			return MyActorCanvas.ToSharedRef();
		}
	}

	// Give it a trivial box, NullWidget isn't safe to use from a UWidget
	// 设计器环境或无法获得有效 LocalPlayer 时，
	// 返回一个简单的 SBox 作为安全占位 Widget。
	// UWidget 的 RebuildWidget 不适合直接返回 NullWidget。
	return SNew(SBox);
}
