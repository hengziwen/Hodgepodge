// Copyright Epic Games, Inc. All Rights Reserved.

// UHodgeActivatableWidget 类定义。
#include "UI/HodgeActivatableWidget.h"

// Widget Blueprint 编译日志。
// 用于在编辑器编译 Widget Blueprint 时输出 Warning / Note 等提示信息。
#include "Editor/WidgetCompilerLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeActivatableWidget)

// 定义当前文件使用的本地化文本命名空间。
// 后面的 LOCTEXT 会归属于 "Hodge" 命名空间。
#define LOCTEXT_NAMESPACE "Hodge"

// 构造 Hodge ActivatableWidget。
UHodgeActivatableWidget::UHodgeActivatableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// 根据当前 Widget 配置的 InputConfig，
// 向 CommonUI 返回该 Widget 激活期间希望使用的输入配置。
TOptional<FUIInputConfig> UHodgeActivatableWidget::GetDesiredInputConfig() const
{
	// 根据项目层定义的 EHodgeWidgetInputMode，
	// 转换为 CommonUI 真正使用的 FUIInputConfig。
	switch (InputConfig)
	{
	// GameAndMenu 表示游戏和 UI 都可以参与输入处理。
	case EHodgeWidgetInputMode::GameAndMenu:
		// ECommonInputMode::All 允许 Game 和 Menu 输入同时存在。
		// 鼠标捕获方式使用当前 Widget 配置的 GameMouseCaptureMode。
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);

	// Game 表示当前 Widget 激活期间主要保持游戏输入模式。
	case EHodgeWidgetInputMode::Game:
		// 使用 CommonUI 的 Game 输入模式。
		// 鼠标继续按照 GameMouseCaptureMode 配置进行捕获。
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);

	// Menu 表示当前 Widget 激活期间主要由 UI / 菜单处理输入。
	case EHodgeWidgetInputMode::Menu:
		// 使用 CommonUI 的 Menu 输入模式。
		// 菜单模式下不捕获鼠标，使鼠标可以自由操作 UI。
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);

	// Default 表示当前 Widget 不主动指定新的输入配置。
	case EHodgeWidgetInputMode::Default:
	default:
		// 返回空的 TOptional。
		// 表示当前 Widget 没有提供自己的 FUIInputConfig，
		// 让 CommonUI 继续按照已有输入配置 / 默认行为进行处理。
		return TOptional<FUIInputConfig>();
	}
}

#if WITH_EDITOR

// Widget Blueprint 编译期间执行额外的项目级检查。
// 这部分代码只存在于编辑器构建中，不会进入正常运行时版本。
void UHodgeActivatableWidget::ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree,
                                                         class IWidgetCompilerLog& CompileLog) const
{
	// 先执行 UCommonActivatableWidget 自身的 WidgetTree 编译检查。
	Super::ValidateCompiledWidgetTree(BlueprintWidgetTree, CompileLog);

	// 检查当前 Widget Blueprint 是否在蓝图脚本中实现了 BP_GetDesiredFocusTarget。
	// 该函数用于告诉 CommonUI：当前页面激活后，默认应该把输入焦点放在哪个 Widget 上。
	if (!GetClass()->IsFunctionImplementedInScript(
		GET_FUNCTION_NAME_CHECKED(UHodgeActivatableWidget, BP_GetDesiredFocusTarget)))
	{
		// 如果当前类的直接原生父类就是 UHodgeActivatableWidget，
		// 基本可以确定当前 Widget 自己没有提供默认焦点目标。
		if (GetParentNativeClass(GetClass()) == UHodgeActivatableWidget::StaticClass())
		{
			// 输出编译警告。
			// 没有 DesiredFocusTarget 时，键鼠可能仍然可以操作，
			// 但手柄依赖 UI Focus 进行导航，因此很容易出现页面打开后手柄无法正常操作的问题。
			CompileLog.Warning(LOCTEXT("ValidateGetDesiredFocusTarget_Warning",
			                           "GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen."));
		}
		else
		{
			//TODO - Note for now, because we can't guarantee it isn't implemented in a native subclass of this one.
			// TODO：这里只输出 Note，因为当前 Widget 还存在其他原生父类层级，
			// 无法完全确定 BP_GetDesiredFocusTarget 是否已经由某个原生子类 / 父类逻辑进行了处理。

			// 输出普通提示而不是 Warning。
			// 如果默认焦点逻辑已经在 Native Base Class 中实现，则可以忽略该提示。
			CompileLog.Note(LOCTEXT("ValidateGetDesiredFocusTarget_Note",
			                        "GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen.  If it was implemented in the native base class you can ignore this message."));
		}
	}
}

#endif

// 清除当前文件定义的本地化文本命名空间，避免影响后续代码。
#undef LOCTEXT_NAMESPACE
