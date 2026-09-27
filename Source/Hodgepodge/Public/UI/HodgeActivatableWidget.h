// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// CommonUI 提供的可激活 Widget 基类。
// 支持 Activate / Deactivate 生命周期以及 CommonUI 的输入路由机制。
#include "CommonActivatableWidget.h"

#include "HodgeActivatableWidget.generated.h"

// CommonUI 输入配置结构体。
// 用于描述当前 UI 激活后应该采用什么输入模式以及鼠标捕获方式。
struct FUIInputConfig;

// Hodge 项目对 UI 输入模式的进一步抽象。
// 用于决定当前 ActivatableWidget 激活时，输入应该如何在 Game 和 UI 之间分配。
UENUM(BlueprintType)
enum class EHodgeWidgetInputMode : uint8
{
	// 不主动指定输入模式，继续使用 CommonUI / 当前系统已有的默认输入配置。
	Default,

	// 游戏和 UI 都可以接收输入。
	GameAndMenu,

	// 主要使用游戏输入。
	Game,

	// 主要使用 UI / 菜单输入。
	Menu
};

// An activatable widget that automatically drives the desired input config when activated
// Hodge 项目的 CommonActivatableWidget 基类。
// Widget 激活后会根据自身配置自动向 CommonUI 提供期望的输入模式，
// 从而控制当前输入应该交给游戏、UI，还是两者共同处理。
UCLASS(Abstract, Blueprintable)
class UHodgeActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	// 构造 Hodge ActivatableWidget。
	UHodgeActivatableWidget(const FObjectInitializer& ObjectInitializer);

public:
	//~UCommonActivatableWidget interface

	// 返回当前 Widget 激活时希望使用的 UI 输入配置。
	// CommonUI 会根据这个配置调整 Game / Menu 输入模式以及鼠标捕获行为。
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	//~End of UCommonActivatableWidget interface

#if WITH_EDITOR
	// 编辑器编译 Widget Blueprint 时执行额外验证。
	// 可以用于检查当前 WidgetTree 是否满足项目规定的 UI 结构或配置要求。
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree,
	                                        class IWidgetCompilerLog& CompileLog) const override;
#endif

protected:
	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	// 当前 UI 激活期间希望使用的输入模式。
	// 例如打开背包后，可以决定 WASD 是否仍然能够传递给游戏角色，
	// 或者输入是否应该完全交给当前菜单 UI。
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EHodgeWidgetInputMode InputConfig = EHodgeWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	// 游戏接收输入时希望采用的鼠标捕获模式。
	// 默认永久捕获鼠标，常用于第三人称视角旋转等游戏输入场景。
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
