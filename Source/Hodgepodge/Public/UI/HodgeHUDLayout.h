//Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// Hodge 项目的 CommonUI ActivatableWidget 基类。
// UHodgeHUDLayout 本身是一个可激活 / 可停用的 CommonUI Widget。
#include "HodgeActivatableWidget.h"

// UE Core Ticker。
// 用于把手柄状态检查延迟到下一 Tick 统一处理，避免同一帧重复处理多次设备变化事件。
#include "Containers/Ticker.h"

// GameplayTag 容器。
// 用于根据当前平台配置的 GameplayTag 判断是否需要显示“手柄断开”界面。
#include "GameplayTagContainer.h"

#include "HodgeHUDLayout.generated.h"

// CommonUI 可激活 Widget。
// Escape 菜单和手柄断连菜单最终都会以 ActivatableWidget 的形式加入 UI Layer。
class UCommonActivatableWidget;

// UObject 前向声明。
class UObject;

// Hodge 的“控制器断开连接”界面。
class UHodgeControllerDisconnectedScreen;

// CommonUI 自带的“可激活控件栈”。
class UCommonActivatableWidgetStack;

/**
 * UHodgeHUDLayout
 *
 *	Widget used to lay out the player's HUD (typically specified by an Add Widgets action in the experience)
 *
 * 玩家 HUD 的主布局 Widget。
 *
 * 通常不会由 AHodgeHUD 直接创建，
 * 而是由当前 Experience 中配置的 Add Widgets Action 动态添加。
 *
 * 该 Widget 作为玩家 HUD 的主要布局入口，
 * 同时负责 Escape / Pause 菜单以及控制器断连界面等 HUD 级全局交互。
 */
UCLASS(Abstract, BlueprintType, Blueprintable, Meta = (DisplayName = "Hodge HUD Layout", Category = "Hodge|HUD"))
class UHodgeHUDLayout : public UHodgeActivatableWidget
{
	GENERATED_BODY()

public:
	// 构造函数。
	UHodgeHUDLayout(const FObjectInitializer& ObjectInitializer);

	// Widget 完成初始化时调用。
	//
	// 适合在这里注册输入动作、输入设备连接状态变化等
	// 与 HUD 整体生命周期一致的监听。
	virtual void NativeOnInitialized() override;

	// Widget 销毁时调用。
	//
	// 用于解除事件监听、Ticker 等当前 HUD Layout 持有的运行时状态。
	virtual void NativeDestruct() override;

protected:
	// 处理 Escape / Pause 输入。
	//
	// 通常用于将 EscapeMenuClass 对应的菜单
	// Push 到 CommonUI 的菜单 Layer 中。
	void HandleEscapeAction();

	// 确保菜单层栈（对应 UI.Layer.Menu）可用。
	void EnsureMenuLayerStack();

	/** 
	* Callback for when controllers are disconnected. This will check if the player now has 
	* no mapped input devices to them, which would mean that they can't play the game.
	* 
	* If this is the case, then call DisplayControllerDisconnectedMenu.
	*/

	// 输入设备连接状态发生变化时的回调。
	//
	// 当手柄连接 / 断开时进入这里，
	// 用于判断当前玩家是否已经失去了所有可用的输入设备。
	//
	// 如果玩家已经没有可用控制器，
	// 后续会触发“Controller Disconnected”界面的显示检查。
	void HandleInputDeviceConnectionChanged(EInputDeviceConnectionState NewConnectionState,
	                                        FPlatformUserId PlatformUserId, FInputDeviceId InputDeviceId);

	/**
	* Callback for when controllers change their owning platform user. We will use this to check
	* if we no longer need to display the "Controller Disconnected" menu
	*/

	// 输入设备与 PlatformUser 的配对关系发生变化时调用。
	//
	// 一个手柄可能从某个平台用户重新配对给另一个用户，
	// 因此即使设备本身没有发生物理连接 / 断开，
	// 当前玩家拥有的可用控制器集合也可能发生变化。
	//
	// 这里会重新检查是否仍然需要显示“Controller Disconnected”界面。
	void HandleInputDevicePairingChanged(FInputDeviceId InputDeviceId, FPlatformUserId NewUserPlatformId,
	                                     FPlatformUserId OldUserPlatformId);

	/**
	* Notify this widget that the state of controllers for the player have changed. Queue a timer for next tick to 
	* process them and see if we need to show/hide the "controller disconnected" widget.
	*/

	// 通知当前 HUD Layout：玩家的控制器状态已经发生变化。
	//
	// 这里不会立刻执行完整检查，
	// 而是通过 Ticker 将处理请求安排到下一 Tick，
	// 再统一判断是否应该显示 / 隐藏控制器断连界面。
	void NotifyControllerStateChangeForDisconnectScreen();

	/**
	 * This will check the state of the connected controllers to the player. If they do not have
	 * any controllers connected to them, then we should display the Disconnect menu. If they do have
	 * controllers connected to them, then we can hide the disconnect menu if its showing.
	 */

	// 真正处理控制器状态变化的函数。
	//
	// 检查当前玩家实际拥有的控制器设备：
	//
	// - 如果已经没有可用控制器，则显示 Controller Disconnected 菜单；
	// - 如果重新拥有可用控制器，则隐藏已经显示的断连菜单。
	//
	// virtual 允许具体项目 / 平台进一步覆盖自己的设备判断逻辑。
	virtual void ProcessControllerDevicesHavingChangedForDisconnectScreen();

	/**
     * Returns true if this platform supports a "controller disconnected" screen. 
     */

	// 判断当前运行平台是否应该启用“控制器断开”提示功能。
	//
	// 不同平台对控制器断开的处理要求不同，
	// 因此这里通过平台配置决定是否启用该机制。
	virtual bool ShouldPlatformDisplayControllerDisconnectScreen() const;

	/**
	* Pushes the ControllerDisconnectedMenuClass to the Menu layer (UI.Layer.Menu)
	*/

	// 显示“控制器断开连接”菜单。
	//
	// 默认设计意图是将 ControllerDisconnectedScreen
	// Push 到 CommonUI 的 UI.Layer.Menu 层。
	//
	// BlueprintNativeEvent 允许 C++ 提供默认实现，
	// 同时允许蓝图覆盖具体显示行为。
	UFUNCTION(BlueprintNativeEvent, Category="Controller Disconnect Menu")
	void DisplayControllerDisconnectedMenu();

	/**
	* Hides the controller disconnected menu if it is active.
	*/

	// 如果当前“控制器断开连接”菜单已经显示，则将其隐藏。
	//
	// BlueprintNativeEvent 允许蓝图覆盖具体关闭行为。
	UFUNCTION(BlueprintNativeEvent, Category="Controller Disconnect Menu")
	void HideControllerDisconnectedMenu();

	/**
	 * The menu to be displayed when the user presses the "Pause" or "Escape" button 
	 */

	// 玩家按下 Pause / Escape 时需要显示的菜单类型。
	//
	// 使用 SoftClass 引用，避免 HUD Layout 加载时
	// 强制同步加载对应菜单 Widget 资源。
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;

	/** 
	* The widget which should be presented to the user if all of their controllers are disconnected.
	*/

	// 当当前玩家所有控制器都断开时需要显示的 Widget Class。
	UPROPERTY(EditDefaultsOnly, Category="Controller Disconnect Menu")
	TSubclassOf<UHodgeControllerDisconnectedScreen> ControllerDisconnectedScreen;

	/**
	 * The platform tags that are required in order to show the "Controller Disconnected" screen.
	 *
	 * If these tags are not set in the INI file for this platform, then the controller disconnect screen
	 * will not ever be displayed. 
	 */

	// 启用“Controller Disconnected”界面所要求的平台 GameplayTag。
	//
	// 当前平台的配置必须满足这里声明的 Tag，
	// 才会启用控制器断连提示。
	//
	// 如果平台 INI 中没有配置这些要求的 Tag，
	// 则该平台永远不会显示控制器断连界面。
	UPROPERTY(EditDefaultsOnly, Category="Controller Disconnect Menu")
	FGameplayTagContainer PlatformRequiresControllerDisconnectScreen;

	/** Pointer to the active "Controller Disconnected" menu if there is one. */

	// 当前已经显示的“Controller Disconnected”菜单实例。
	//
	// 没有显示断连菜单时为空。
	// 保存实例后可以在控制器恢复时精确关闭对应界面。
	UPROPERTY(Transient)
	TObjectPtr<UCommonActivatableWidget> SpawnedControllerDisconnectScreen;

	// 菜单层容器（对应 CommonUI 的 UI.Layer.Menu）。
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonActivatableWidgetStack> MenuLayerStack;

	/** Handle from the FSTicker for when we want to process the controller state of our player */

	// 延迟处理玩家控制器状态时对应的 Ticker Handle。
	//
	// 用于跟踪当前是否已经安排了一次下一 Tick 的设备状态处理，
	// 并方便在 Widget 生命周期结束时进行清理。
	FTSTicker::FDelegateHandle RequestProcessControllerStateHandle;
};
