// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/HodgeHUDLayout.h"

// 菜单统一进入当前本地玩家的根布局，不依赖 CommonGame。

// CommonUI 全局设置。
// 用于读取当前平台的 PlatformTraits。
#include "CommonUISettings.h"

// 输入设备 Subsystem。
// 用于根据 InputDeviceId 查询具体硬件设备类型，例如判断是否为 Gamepad。
#include "GameFramework/InputDeviceSubsystem.h"

// UE 输入设置。
#include "GameFramework/InputSettings.h"

// 平台输入设备映射接口。
// 用于查询设备连接状态、设备与 PlatformUser 的映射关系，
// 以及监听输入设备连接 / 配对状态变化。
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"

// CommonUI 输入类型。
// 包含 FUIActionTag、FBindUIActionArgs 等 CommonUI Action Binding 相关类型。
#include "Input/CommonUIInputTypes.h"

// CommonUI 模块接口。
// 用于访问 CommonUI Settings 以及当前平台的 PlatformTraits。
#include "ICommonUIModule.h"

// 原生 GameplayTag 定义支持。
// 当前文件使用 UE_DEFINE_GAMEPLAY_TAG_STATIC 定义 UI 和平台 Trait Tag。
#include "NativeGameplayTags.h"

// Hodge 的“控制器断开连接”界面。
#include "UI/Foundation/HodgeControllerDisconnectedScreen.h"

// Hodge CommonUI ActivatableWidget 基类。
#include "UI/HodgeActivatableWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Overlay.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#if WITH_EDITOR

// CommonUI 编辑器平台可见性模拟 Subsystem。
// 编辑器中可以通过模拟 Platform Tag 测试不同平台的 UI 行为。
#include "CommonUIVisibilitySubsystem.h"

#endif	// WITH_EDITOR

#include "UI/Foundation/HodgePrimaryGameLayout.h"
#include "UI/Subsystem/HodgeUIManagerSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHUDLayout)

// CommonUI 菜单层 GameplayTag。
// Escape Menu、Controller Disconnected Screen 等菜单级 UI
// 设计上会被 Push 到该 UI Layer。
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MENU, "UI.Layer.Menu");

// Escape / Pause 对应的 CommonUI Action Tag。
// HUD Layout 会监听这个 Action，并触发 HandleEscapeAction。
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_ACTION_ESCAPE, "UI.Action.Escape");

// 表示“当前平台主要使用 Controller 进行输入”的 Platform Trait Tag。
// 默认只有具备该 Trait 的平台才需要显示 Controller Disconnected Screen。
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_Input_PrimarlyController, "Platform.Trait.Input.PrimarlyController");

// HUD Layout 构造函数。
UHodgeHUDLayout::UHodgeHUDLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	  // 初始状态下没有正在显示的 Controller Disconnected Screen。
	  , SpawnedControllerDisconnectScreen(nullptr)
{
	// By default, only primarily controller platforms require a disconnect screen. 

	// 默认要求当前平台拥有“主要使用 Controller 输入”的 Trait，
	// 才启用控制器断开连接提示界面。
	PlatformRequiresControllerDisconnectScreen.AddTag(TAG_Platform_Trait_Input_PrimarlyController);
}

// HUD Layout 初始化完成时调用。
void UHodgeHUDLayout::NativeOnInitialized()
{
	// 先执行 UHodgeActivatableWidget / CommonUI 的初始化逻辑。
	Super::NativeOnInitialized();

	// 准备菜单层容器。
	//
	// Escape 菜单与 Controller Disconnected Screen 都要 Push 到这一层；
	// 这里获取当前玩家根布局已经注册的 Menu 层。
	EnsureMenuLayerStack();

	// 注册 CommonUI Escape Action。
	//
	// 将 GameplayTag：
	// UI.Action.Escape
	//
	// 转换为 CommonUI 使用的 FUIActionTag，
	// 当该 Action 被触发时调用 HandleEscapeAction。
	FBindUIActionArgs EscapeBinding(FUIActionTag::ConvertChecked(TAG_UI_ACTION_ESCAPE), false,
	    FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction));
	EscapeBinding.InputMode = ECommonInputMode::Game;
	RegisterUIActionBinding(EscapeBinding);

	// If we can display a controller disconnect screen, then listen for the controller state change delegates

	// 只有当前平台确实需要 Controller Disconnected Screen 时，
	// 才注册底层输入设备变化事件。
	//
	// PC 键鼠为主的平台没有必要承担这套监听逻辑。
	if (ShouldPlatformDisplayControllerDisconnectScreen())
	{
		// Bind to when input device connections change

		// 获取平台级输入设备映射器。
		IPlatformInputDeviceMapper& DeviceMapper = IPlatformInputDeviceMapper::Get();

		// 监听输入设备连接状态变化：
		// 例如手柄插入、断开、无线连接丢失等。
		DeviceMapper.GetOnInputDeviceConnectionChange().
		             AddUObject(this, &ThisClass::HandleInputDeviceConnectionChanged);

		// 监听输入设备所属 PlatformUser 的配对关系变化。
		//
		// 即使手柄没有物理断开，
		// 如果它从当前玩家重新配对给其他玩家，
		// 对当前玩家而言同样可能变成“没有可用手柄”。
		DeviceMapper.GetOnInputDevicePairingChange().AddUObject(this, &ThisClass::HandleInputDevicePairingChanged);
	}
}

// HUD Layout 销毁时调用。
void UHodgeHUDLayout::NativeDestruct()
{
    CloseOwnedMenu();

	// 先执行父类销毁逻辑。
	Super::NativeDestruct();

	// Remove bindings to input device connection changing

	// 获取平台输入设备映射器。
	IPlatformInputDeviceMapper& DeviceMapper = IPlatformInputDeviceMapper::Get();

	// 移除当前 HUD Layout 注册的所有设备连接状态回调，
	// 防止 Widget 销毁后继续收到设备事件。
	DeviceMapper.GetOnInputDeviceConnectionChange().RemoveAll(this);

	// 移除设备配对关系变化回调。
	DeviceMapper.GetOnInputDevicePairingChange().RemoveAll(this);

	// 如果之前已经安排了下一 Tick 的 Controller 状态检查，
	// HUD 销毁时需要主动取消。
	if (RequestProcessControllerStateHandle.IsValid())
	{
		// 从 CoreTicker 中移除对应任务。
		FTSTicker::GetCoreTicker().RemoveTicker(RequestProcessControllerStateHandle);

		// 清空本地保存的 Ticker Handle。
		RequestProcessControllerStateHandle.Reset();
	}
}

void UHodgeHUDLayout::CloseOwnedMenu()
{
    const FGuid Pending = PendingEscapeMenu;
    auto Menu = EscapeMenuInstance;
    PendingEscapeMenu.Invalidate(); EscapeMenuInstance.Reset();
    if (auto* Root = UHodgeUIManagerSubsystem::GetRootLayoutForController(GetOwningPlayer()))
    { Root->CancelPush(Pending); Root->Pop(Menu.Get()); }
}
void UHodgeHUDLayout::NativeOnDeactivated()
{
    // 停用时立即撤销所属菜单，不等待 Slate 切换结束后才 Destruct。
    CloseOwnedMenu();
    Super::NativeOnDeactivated();
}

// 确保菜单层栈（对应 UI.Layer.Menu）可用。
void UHodgeHUDLayout::EnsureMenuLayerStack()
{
    auto* GI = GetGameInstance();
    auto* UI = GI ? GI->GetSubsystem<UHodgeUIManagerSubsystem>() : nullptr;
    auto* Root = UI ? UI->GetRootLayout(GetOwningLocalPlayer()) : nullptr;
    MenuLayerStack = Root ? Cast<UCommonActivatableWidgetStack>(Root->GetLayer(TAG_UI_LAYER_MENU)) : nullptr;
}

// 处理玩家触发 Escape / Pause UI Action。
void UHodgeHUDLayout::HandleEscapeAction()
{
    auto* GI = GetGameInstance();
    auto* UI = GI ? GI->GetSubsystem<UHodgeUIManagerSubsystem>() : nullptr;
    auto* Root = UI ? UI->GetRootLayout(GetOwningLocalPlayer()) : nullptr;
    if (!Root || EscapeMenuClass.IsNull()) { return; }
    if (PendingEscapeMenu.IsValid()) { Root->CancelPush(PendingEscapeMenu); PendingEscapeMenu.Invalidate(); return; }
    if (EscapeMenuInstance.IsValid() && EscapeMenuInstance->IsActivated())
    { Root->Pop(EscapeMenuInstance.Get()); EscapeMenuInstance.Reset(); return; }
    TWeakObjectPtr<UHodgeHUDLayout> Weak = this;
    PendingEscapeMenu = Root->PushAsync(TAG_UI_LAYER_MENU, EscapeMenuClass, [Weak](UCommonActivatableWidget* Menu)
    {
        if (Weak.IsValid()) { Weak->PendingEscapeMenu.Invalidate(); Weak->EscapeMenuInstance = Menu; }
    });
}

// 输入设备连接状态发生变化时调用。
void UHodgeHUDLayout::HandleInputDeviceConnectionChanged(EInputDeviceConnectionState NewConnectionState,
                                                         FPlatformUserId PlatformUserId, FInputDeviceId InputDeviceId)
{
	// 获取当前 HUD Layout 所属 LocalPlayer 对应的平台用户 ID。
	const FPlatformUserId OwningLocalPlayerId = GetOwningLocalPlayer()->GetPlatformUserId();

	// 正常情况下 LocalPlayer 应该拥有有效 PlatformUserId。
	ensure(OwningLocalPlayerId.IsValid());

	// This device connection change happened to a different player, ignore it for us.

	// 本次发生连接变化的设备不属于当前玩家，
	// 则当前 HUD 不需要处理。
	if (PlatformUserId != OwningLocalPlayerId)
	{
		return;
	}

	// 当前玩家的设备状态发生变化。
	// 不立即处理，而是安排到下一 Tick 统一检查最终状态。
	NotifyControllerStateChangeForDisconnectScreen();
}

// 输入设备与 PlatformUser 的配对关系发生变化时调用。
void UHodgeHUDLayout::HandleInputDevicePairingChanged(FInputDeviceId InputDeviceId, FPlatformUserId NewUserPlatformId,
                                                      FPlatformUserId OldUserPlatformId)
{
	// 获取当前 HUD 所属 LocalPlayer 的平台用户 ID。
	const FPlatformUserId OwningLocalPlayerId = GetOwningLocalPlayer()->GetPlatformUserId();

	// 确保当前 LocalPlayer 的 PlatformUserId 有效。
	ensure(OwningLocalPlayerId.IsValid());

	// If this pairing change was related to our local player, notify of a change.

	// 如果当前设备：
	// - 新配对给了当前玩家；
	// - 或之前属于当前玩家，现在被配走；
	//
	// 都意味着当前玩家的可用 Controller 集合可能发生变化。
	if (NewUserPlatformId == OwningLocalPlayerId || OldUserPlatformId == OwningLocalPlayerId)
	{
		// 安排下一 Tick 重新检查当前玩家的 Controller 状态。
		NotifyControllerStateChangeForDisconnectScreen();
	}
}

// 判断当前平台是否需要显示 Controller Disconnected Screen。
bool UHodgeHUDLayout::ShouldPlatformDisplayControllerDisconnectScreen() const
{
	// We only want this menu on primarily controller platforms

	// 获取 CommonUI 当前平台的 PlatformTraits，
	// 检查是否满足 PlatformRequiresControllerDisconnectScreen 中要求的全部 Tag。
	//
	// 默认要求：
	// Platform.Trait.Input.PrimarlyController
	bool bHasAllRequiredTags = ICommonUIModule::GetSettings().GetPlatformTraits().HasAll(
		PlatformRequiresControllerDisconnectScreen);

	// Check the tags that we may be emulating in the editor too
#if WITH_EDITOR

	// 编辑器下还需要考虑 CommonUI 的平台 Trait 模拟功能。
	//
	// 即使当前实际运行平台没有这些 Trait，
	// 开发者也可以在编辑器中模拟其他平台进行 UI 测试。
	const FGameplayTagContainer& PlatformEmulationTags = UCommonUIVisibilitySubsystem::Get(GetOwningLocalPlayer())->
		GetVisibilityTags();

	// 真实平台满足要求，或者编辑器模拟的平台 Tag 满足要求，
	// 都认为当前应该启用 Controller Disconnect Screen。
	bHasAllRequiredTags |= PlatformEmulationTags.HasAll(PlatformRequiresControllerDisconnectScreen);

#endif	// WITH_EDITOR

	// 返回最终平台判断结果。
	return bHasAllRequiredTags;
}

// 收到 Controller 状态变化后，请求下一 Tick 统一处理。
void UHodgeHUDLayout::NotifyControllerStateChangeForDisconnectScreen()
{
	// We should only ever get here if we have bound to the controller state change delegates

	// 理论上只有支持 Controller Disconnect Screen 的平台
	// 才会绑定前面的设备状态变化 Delegate，因此这里进行状态验证。
	ensure(ShouldPlatformDisplayControllerDisconnectScreen());

	// If we haven't already, queue the processing of device state for next tick.

	// 如果当前还没有安排设备状态检查，
	// 则注册一个下一 Tick 执行的 Ticker。
	//
	// 如果同一帧连续发生多个设备事件，
	// 因为 Handle 已经有效，所以不会重复注册多个检查任务。
	if (!RequestProcessControllerStateHandle.IsValid())
	{
		// 向 CoreTicker 注册一个弱引用 Lambda。
		//
		// 使用 WeakLambda 可以避免 HUD Layout 已经销毁时
		// Ticker 继续强行访问无效对象。
		RequestProcessControllerStateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateWeakLambda(
			this, [this](float DeltaTime)
			{
				// Ticker 已经开始执行，
				// 先把 Handle 重置为无效状态。
				RequestProcessControllerStateHandle.Reset();

				// 根据这一 Tick 的最终设备状态，
				// 判断应该显示还是隐藏 Controller Disconnect Screen。
				ProcessControllerDevicesHavingChangedForDisconnectScreen();

				// 返回 false 表示这个 Ticker 只执行一次，
				// 不需要继续周期性调用。
				return false;
			}));
	}
}

// 真正检查当前玩家 Controller 设备状态，
// 并决定显示 / 隐藏 Controller Disconnected Screen。
void UHodgeHUDLayout::ProcessControllerDevicesHavingChangedForDisconnectScreen()
{
	// We should only ever get here if we have bound to the controller state change delegates

	// 只有启用了 Controller Disconnect Screen 的平台
	// 才应该进入这个处理流程。
	ensure(ShouldPlatformDisplayControllerDisconnectScreen());

	// 获取当前 LocalPlayer 对应的平台用户 ID。
	const FPlatformUserId OwningLocalPlayerId = GetOwningLocalPlayer()->GetPlatformUserId();

	// 确保 PlatformUserId 有效。
	ensure(OwningLocalPlayerId.IsValid());

	// Get all input devices mapped to our player

	// 获取平台输入设备映射器。
	const IPlatformInputDeviceMapper& DeviceMapper = IPlatformInputDeviceMapper::Get();

	// 保存当前 PlatformUser 映射到的所有输入设备。
	TArray<FInputDeviceId> MappedInputDevices;

	// 查询所有属于当前玩家的 InputDeviceId。
	const int32 NumDevicesMappedToUser = DeviceMapper.GetAllInputDevicesForUser(
		OwningLocalPlayerId, OUT MappedInputDevices);

	// Check if there are any other connected GAMEPAD devices mapped to this platform user. 

	// 默认认为当前玩家没有已连接的 Gamepad。
	bool bHasConnectedController = false;

	// 遍历当前玩家拥有的所有输入设备。
	for (const FInputDeviceId MappedDevice : MappedInputDevices)
	{
		// 首先要求该设备当前处于 Connected 状态。
		if (DeviceMapper.GetInputDeviceConnectionState(MappedDevice) == EInputDeviceConnectionState::Connected)
		{
			// 查询该 InputDevice 对应的实际硬件设备信息。
			const FHardwareDeviceIdentifier HardwareInfo = UInputDeviceSubsystem::Get()->
				GetInputDeviceHardwareIdentifier(MappedDevice);

			// 只有 Gamepad 类型才被视为这里需要检查的 Controller。
			//
			// 键盘、鼠标等设备不会让 bHasConnectedController 变成 true。
			if (HardwareInfo.PrimaryDeviceType == EHardwareDevicePrimaryType::Gamepad)
			{
				bHasConnectedController = true;
			}
		}
	}

	// If there are no gamepad input devices mapped to this user, then we want to pop the toast saying to re-connect them

	// 当前玩家已经没有任何已连接的 Gamepad，
	// 显示“请重新连接控制器”界面。
	if (!bHasConnectedController)
	{
		DisplayControllerDisconnectedMenu();
	}

	// Otherwise we can hide the screen if it is currently being shown

	// 如果已经重新拥有可用 Gamepad，
	// 并且当前 Controller Disconnected Screen 正在显示，
	// 则关闭该界面。
	else if (SpawnedControllerDisconnectScreen)
	{
		HideControllerDisconnectedMenu();
	}
}

// BlueprintNativeEvent 的 C++ 默认实现：
// 显示 Controller Disconnected Menu。
void UHodgeHUDLayout::DisplayControllerDisconnectedMenu_Implementation()
{
	// 输出调试日志。
	UE_LOG(LogTemp, Log, TEXT("[%hs] Display controller disconnected menu!"), __func__);

	// 确保配置了有效的 Controller Disconnected Screen Class，且层容器可用。
	if (!ControllerDisconnectedScreen || !MenuLayerStack)
	{
		return;
	}

	// 已经在显示时不要重复 Push，否则栈里会堆出多个断连界面。
	if (SpawnedControllerDisconnectScreen)
	{
		return;
	}

	// Push the "controller disconnected" widget to the menu layer

	// 设计意图：
	// 将 ControllerDisconnectedScreen Push 到 UI.Layer.Menu，
	// 并保存实际创建出来的 Widget 实例，便于控制器恢复后精确 Pop 掉它。
	//
	// 对应 Lyra 的：
	//   SpawnedControllerDisconnectScreen = UCommonUIExtensions::PushContentToLayer_ForPlayer(...)
	// 区别只是把“找到持有该层的对象”这一步换成了直接持有 Stack 引用。
	SpawnedControllerDisconnectScreen =
		MenuLayerStack->AddWidget<UCommonActivatableWidget>(ControllerDisconnectedScreen);

	// AddWidget 在传入的 Class 不是 ActivatableWidget 时会返回空，这里留个日志避免静默失败。
	if (!SpawnedControllerDisconnectScreen)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%hs] 断连界面 Push 失败：%s"), __func__,
		       *GetNameSafe(ControllerDisconnectedScreen));
	}
}

// BlueprintNativeEvent 的 C++ 默认实现：
// 隐藏 Controller Disconnected Menu。
void UHodgeHUDLayout::HideControllerDisconnectedMenu_Implementation()
{
	// 输出调试日志。
	UE_LOG(LogTemp, Log, TEXT("[%hs] Hide controller disconnected menu!"), __func__);

	// 设计意图：
	// 将当前显示的 Controller Disconnected Screen
	// 从菜单层 Stack 中 Pop 出去。
	//
	// 对应 Lyra 的 UCommonUIExtensions::PopContentFromLayer →
	// UPrimaryGameLayout::FindAndRemoveWidgetFromLayer →
	// UCommonActivatableWidgetContainerBase::RemoveWidget。
	//
	// 因为这里直接持有 MenuLayerStack 引用，不需要 Lyra 那样
	// “遍历所有层去找控件在哪一层”。
	if (MenuLayerStack && SpawnedControllerDisconnectScreen)
	{
		// RemoveWidget 内部会自动完成两件事：
		// - 该 Widget 是本层 active widget 时，调用它的 DeactivateWidget()；
		// - 把它从本层的 WidgetList 中移除并释放 Slate 资源。
		//
		// 所以这里不需要手动 DeactivateWidget() / RemoveFromParent()。
		MenuLayerStack->RemoveWidget(*SpawnedControllerDisconnectScreen);
	}

	// 清空当前断连界面实例引用，
	// 表示当前已经没有活动的 Controller Disconnect Screen。
	SpawnedControllerDisconnectScreen = nullptr;
}
