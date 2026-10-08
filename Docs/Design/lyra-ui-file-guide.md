> 2026-10-08：本文保留当时的源码导览与判断。当前自有根布局／HUD／菜单已接通，现行实现见 [Hodge UI 基础闭环](hodge-ui-foundation.md) 和 [配置指南](../Guides/ui-foundation-configuration.md)。

# Lyra UI 迁移文件导览（79 文件逐个说明 + 学习优先级）

> 2026-10-06 状态同步：UI 迁移/学习材料：CommonUI 已引入，部分 UIExtension/Indicator 代码有效；依赖 CommonGame/GameSettings/CommonUser 的停用内容与完整前端/HUD 尚未接通。本文不作为游戏 UI 已出画面的声明。 当前项目事实见 [本轮更新](../KnowledgeBase/26-update-2026-10-06.md)。

> **文档类型：参考索引，不是设计草案。** 基线 2026-09-26。
> 目录：`Source/Hodgepodge/Public/UI/**` + `Private/UI/**`，共 **79 个文件**（40 头文件 + 39 实现）。
> 图例：✅ 在编（53）｜⏸ 已整体注释、待复活（26，清单见 [迁移执行文档 §10.2](lyra-ui-migration-plan.md)）
> 行数格式：`头文件行数/实现文件行数`，为 **2026-09-26 实测总行数**。
> 注：T0 的 6 个文件此后又被补充了大量中文注释（`IndicatorDescriptor.h` 已达 394 行、`SActorCanvas.cpp` 已达 1184 行），行数仅供相对规模参考。

**相关文档**

- [Lyra UI 架构迁移执行文档](lyra-ui-migration-plan.md)（§10 待复活清单、§11 SActorCanvas 复活、§12 只引入 UIExtension 的方案）
- [UMG 与 Slate：最小心智模型](umg-slate-mental-model.md)（**卡在 `SPanel`/`Slot`/`OnPaint` 时先读这份**）
- [Indicator UI 系统：职责边界与完整流程](indicator-ui-system.md)（主线 B 的深入说明）
- [UIExtension：职责边界与完整流程](ui-extension-system.md)（A 轴的深入说明；§5 的四个轴里 A 轴的落地细节）
- [项目开发约定](../../AGENTS.md)

---

## 0. 先看这个：三条学习主线

**不要按目录顺序读**，按主线读。目录顺序（`Basic` → `Common` → `Foundation` → …）是 Lyra 的排列习惯，跟重要性无关。

| 主线 | 顺序 | 学时 | 学到什么 |
|---|---|---|---|
| **A. UI 基座** | `HodgeActivatableWidget` → `HodgeTaggedWidget` → `MaterialProgressBar` | 1~2 h | CommonUI 的输入模式契约、Tag 驱动显隐、材质驱动进度条 |
| **B. 世界标记（最有含金量）** | `IndicatorLibrary` → `HodgeIndicatorManagerComponent` → `IndicatorDescriptor` → `IActorIndicatorWidget` → `SActorCanvas` → `IndicatorLayer` | 4~6 h | 屏幕投影 + 视锥裁剪 + 屏幕边缘钳制 + 箭头指示 + 控件池 + Slate 自绘 |
| **C. HUD 挂载** | 引入 `UIExtension` → 复活 `HodgeHUDLayout` → `HodgeHUD` → 复活 `GameFeatureAction_AddWidget` → `HodgeWidgetFactory` | 3~5 h | Tag 作挂载点、Handle 生命周期、游戏功能动态注入 UI |
| D. 触屏输入（可选） | `HodgeSimulatedInputWidget` → `HodgeJoystickWidget` / `HodgeTouchRegion` | 1~2 h | 用 EnhancedInput 反向"模拟按键" |

主线 B 是这批文件里**唯一需要写 Slate 自绘、纯数学的部分**，也是别处学不到的；主线 A/C 是架构模式，别处也能见到，但这里是最小完整实现。

---

## 1. 分级总览

### T0 核心 —— 必读（15 文件）

> 行数为 **2026-09-26 实测总行数**（口径 `头文件/实现文件`）。这些文件此后被补充了大量中文注释，行数会继续增长；引用具体位置时请以文件内实际内容为准。

| 文件 | 总行数 | 状态 | 干什么 / 关键 API |
|---|---|---|---|
| `HodgeActivatableWidget.h/.cpp` | 75/100 | ✅ | **全项目 UI 基类**。`EHodgeWidgetInputMode{Default,Game,Menu,All}` 决定激活时如何接管输入；`GetDesiredInputConfig()` 返回鼠标捕获/光标显隐；编辑器期校验控件树 |
| `HodgeTaggedWidget.h/.cpp` | 90/140 | ✅ | **Tag 驱动的显隐**。监听拥有者的 Tag 容器变化，命中 `HiddenByTags` 就隐藏；`bWantsToBeVisible` 记录"我想不想显示"，重写 `SetVisibility` 防止被外部改坏 |
| `IndicatorSystem/IndicatorLibrary.h/.cpp` | 39/27 | ✅ | 蓝图函数库，唯一入口 `GetIndicatorManagerComponent(AController)`。极简，适合当"读代码热身" |
| `IndicatorSystem/HodgeIndicatorManagerComponent.h/.cpp` | 77/74 | ✅ | 挂在 Controller 上的 `UControllerComponent`（`bAutoRegister` + `bAutoActivate`），`AddIndicator/RemoveIndicator` + `OnIndicatorAdded/Removed` 事件。指示器生命周期的所有者 |
| `IndicatorSystem/IndicatorDescriptor.h/.cpp` | 394/276 | ✅ | **指示器数据契约（一张"UI 工单"）**。目标组件/Socket、业务 `DataObject`、5 种投影模式、HAlign/VAlign、世界/屏幕两段偏移、包围盒锚点、Clamp 与箭头开关、`Priority`、自动移除；`IndicatorWidget`/`Content`/`CanvasHost` 全是弱引用。**它不做坐标转换**，转换在 `FIndicatorProjection::Project` |
| `IndicatorSystem/IActorIndicatorWidget.h` | 54 | ✅ | 控件侧接口 `UIndicatorWidgetInterface`：`BindIndicator` / `UnbindIndicator`（均为 `BlueprintNativeEvent`）。**成对设计的原因见 [indicator-ui-system §12.4](indicator-ui-system.md)**：控件会被池复用 |
| `IndicatorSystem/SActorCanvas.h/.cpp` | 505/1184 | ✅ | **Indicator 系统的客户端 UI Runtime（Slate 自绘）**。连接 Manager、异步加载控件类、控件池取用、Bind、投影、排序、Clamp、箭头、布局、Paint、Unbind、归还。15 项职责与完整运行链见 [indicator-ui-system §12](indicator-ui-system.md) |
| `IndicatorSystem/IndicatorLayer.h/.cpp` | 52/82 | ✅ | **UWidget ↔ Slate 的适配层**。`UWidget` 子类，`RebuildWidget()` 里 `SNew(SActorCanvas, ...)`，把纯 Slate 画布暴露给 UMG 设计器 |

**T0 的深入学习路径**：先读 [umg-slate-mental-model.md](umg-slate-mental-model.md)（UMG↔Slate 两层关系、5 个概念、两条树与绑定链），再读 [indicator-ui-system.md](indicator-ui-system.md)（职责边界 → 数据流 → 逐类 API → §12 深读），然后回到代码按 §12.2 的出生链走一遍 `SActorCanvas.cpp`。

**不必为 Slate 全库焦虑**：79 个文件里真正需要 Slate 知识的只有 4 个 —— `SActorCanvas`（自定义 `SPanel`）、`UIndicatorLayer`（适配层）、`SCircumferenceMarkerWidget`（`SLeafWidget` 自绘）、`SHitMarkerConfirmationWidget`（⏸）。其余全是纯 UMG 层。完整谱系见 [umg-slate-mental-model.md §13](umg-slate-mental-model.md)。

### T1 有用 —— 值得读（36 文件）

| 文件 | 有效行 | 状态 | 干什么 / 关键 API |
|---|---|---|---|
| `Basic/MaterialProgressBar.h/.cpp` | 98/219 | ✅ | **材质驱动的进度条**（比 `UProgressBar` 可控）。`SetProgress` / `SetStartProgress` / `SetColorA/B/Background` / `AnimateProgressFromStart/Current` + `OnFillAnimationFinished`。分段/描边/发光/边缘柔化的参数全部走材质；蓝图里必须有名为 `Image_Bar` 的 Image 和可选的 `BoundAnim_FillBar` 动画 |
| `HodgeHUD.h/.cpp` | 26/53 | ✅ | `AHUD` 子类：自身注册为 GameFrameworkComponent 接收器（`PreInitializeComponents`/`BeginPlay`/`EndPlay` 三件套），并把所有带 ASC 的 Actor 塞进 GAS 调试列表 |
| `HodgeHUDLayout.h/.cpp` | 0/0 ⏸ | ⏸ 待复活 | **HUD 根控件**。输入动作绑定（Esc → 推菜单）、控制器断连屏的显示/隐藏。复活时需换掉 `HodgeLogChannels.h`/`LogHodge`（项目不存在）与 2 处 `UCommonUIExtensions::Push*` |
| `Common/HodgeTabListWidgetBase.h/.cpp` | 92/172 | ✅ | 标签页容器。`FHodgeTabDescriptor`（TabId/显示名/图标/隐藏态）、`IHodgeTabButtonInterface`、`RegisterDynamicTab` / `SetTabHiddenState` / `GetVisibleTabCount` / 前后切换。动态注册 + 可见性管理的完整例子 |
| `Common/HodgeTabButtonBase.h/.cpp` | 22/25 | ✅ | 标签按钮。`SetTabLabelInfo_Implementation` 接收 `FHodgeTabDescriptor`；`SetIconFromLazyObject` 做图标软加载 |
| `Common/HodgeListView.h/.cpp` | 22/42 | ✅ | 列表视图，覆写 `UCommonListView` 的 entry 生成，支持按条目类型指定工厂 |
| `Common/HodgeWidgetFactory.h/.cpp` | 18/8 | ✅ | **数据 → 控件类 的工厂基类**（`EditInlineNew`），`FindWidgetClassForData(const UObject*)`。与 UIExtension 的 `DataClasses` 生态配套 |
| `Common/HodgeWidgetFactory_Class.h/.cpp` | 20/15 | ✅ | 上面的默认实现：直接给一个 `TSubclassOf<UUserWidget>` |
| `Common/HodgeBoundActionButton.h/.cpp` | 23/34 | ✅ | 绑定 EnhancedInput 动作的按钮：自动取到"该动作当前绑定的按键"并显示图标/文字 |
| `Foundation/HodgeButtonBase.h/.cpp` | 27/42 | ✅ | 按钮基类，`UCommonButtonBase` 扩展（音效、悬停态） |
| `Foundation/HodgeActionWidget.h/.cpp` | 18/33 | ✅ | 输入动作提示控件（"按 X 键"），监听输入映射变化并刷新图标 |
| `Foundation/HodgeControllerDisconnectedScreen.h/.cpp` | 54/75 | ✅ | 手柄断连提示屏。**全项目唯一依赖 `ApplicationCore` 的文件**（`IPlatformInputDeviceMapper`），也是 `Build.cs` 补 `ApplicationCore` 的原因 |
| `Foundation/HodgeLoadingScreenSubsystem.h/.cpp` | 29/19 | ✅ | 极简 `GameInstanceSubsystem`（约 40 行）：存"加载屏里显示哪个控件"，带 `OnLoadingScreenWidgetChanged` 委托。**适合当"写第一个子系统"的模板** |
| `HodgeSimulatedInputWidget.h/.cpp` | 66/118 | ✅ | **用 EnhancedInput 反向模拟输入**：把触摸/摇杆的连续值写进 `UEnhancedPlayerInput` 模拟成按键。`InputKeyValue` / `InputKeyValue2D` / `FlushSimulatedInput`；支持按住刷新频率 |
| `HodgeJoystickWidget.h/.cpp` | 56/81 | ✅ | 虚拟摇杆，继承 `HodgeSimulatedInputWidget`；`GetPaletteCategory` 让它出现在 UMG 面板的专属分类里 |
| `HodgeTouchRegion.h/.cpp` | 26/27 | ✅ | 触屏区域，继承 `HodgeSimulatedInputWidget`；`NativeOnTouchEnded` 收触摸事件 |
| `Weapons/CircumferenceMarkerWidget.h/.cpp` | 40/36 | ✅ | 环形标记（瞄准/命中的外圈），`UWidget` 包 `SCircumferenceMarkerWidget` |
| `Weapons/SCircumferenceMarkerWidget.h/.cpp` | 60/88 | ✅ | 上面的 Slate 实现：`FCircumferenceMarkerEntry` 数组 + `SLeafWidget` 自绘。**与 `SActorCanvas` 一起构成"两种 Slate 自绘范式"（Leaf 单体 vs Panel 容器）** |

### T2 次要 —— 需要时再看（14 文件）

| 文件 | 有效行 | 状态 | 干什么 / 为什么要等 |
|---|---|---|---|
| `Subsystem/HodgeUIManagerSubsystem.h/.cpp` | 0/0 ⏸ | ⏸ | 上游是"每玩家根布局 + HUD 可见性同步"，**依赖 `UGameUIManagerSubsystem`（CommonGame）**。§12 方案已决定不搬 CommonGame → 优先级最低 |
| `Subsystem/HodgeUIMessaging.h/.cpp` | 0/0 ⏸ | ⏸ | 确认框/错误框入口，**父类 `UCommonMessagingSubsystem`（CommonGame）** |
| `Foundation/HodgeConfirmationScreen.h/.cpp` | 0/0 ⏸ | ⏸ | 确认框控件，**父类 `UCommonGameDialog`（CommonGame）** |
| `HodgeSettingScreen.h/.cpp` | 0/0 ⏸ | ⏸ | 设置界面，**父类 `UGameSettingScreen`（GameSettings 插件，59 文件）** |
| `HodgeGameViewportClient.h/.cpp` | 0/0 ⏸ | ⏸ | 平台光标/鼠标行为，**父类 `UCommonGameViewportClient`（CommonGame）** |
| `Frontend/ApplyFrontendPerfSettingsAction.h/.cpp` | 19/20 | ✅ | GameFeatureAction：激活时套用"前端性能设置"。**唯一一个"小而完整"的 GameFeatureAction 例子**，可选读 |
| `Frontend/HodgeLobbyBackground.h/.cpp` | 16/2 | ✅ | 大厅背景图数据资产（`UPrimaryDataAsset` 装一个软纹理）。共 2 行实现，属于"看了就懂" |

### T3 暂缓 —— 现在别碰（14 文件）

依赖**没有搬迁的玩法系统**，读它只会被牵着去补装备/武器/性能统计。

| 文件 | 有效行 | 状态 | 卡在哪 |
|---|---|---|---|
| `Frontend/HodgeFrontendStateComponent.h/.cpp` | 0/0 ⏸ | ⏸ | 依赖 CommonUser（`UCommonUserSubsystem`/`ECommonUser*`）+ ControlFlows（`FControlFlow`）+ PrimaryGameLayout。**依赖最多的一个** |
| `PerformanceStats/HodgePerfStatContainerBase.h/.cpp` | 0/0 ⏸ | ⏸ | 依赖未搬迁的 `Performance/HodgePerformanceStatTypes.h`（`EHodgeStatDisplayMode`） |
| `PerformanceStats/HodgePerfStatWidgetBase.h/.cpp` | 0/0 ⏸ | ⏸ | 依赖 `UHodgePerformanceStatSubsystem`（未搬迁） |
| `Weapons/HodgeReticleWidgetBase.h/.cpp` | 0/0 ⏸ | ⏸ | 依赖 `HodgeWeaponInstance` / `HodgeRangedWeaponInstance` / `HodgeInventoryItemInstance`（项目无 `Weapons/`、`Inventory/` 目录） |
| `Weapons/HodgeWeaponUserInterface.h/.cpp` | 0/0 ⏸ | ⏸ | 依赖 `HodgeEquipmentManagerComponent`（项目无 `Equipment/`） |
| `Weapons/HitMarkerConfirmationWidget.h/.cpp` | 0/0 ⏸ | ⏸ | 连带依赖 `SHitMarkerConfirmationWidget` |
| `Weapons/SHitMarkerConfirmationWidget.h/.cpp` | 0/0 ⏸ | ⏸ | 依赖 `HodgeWeaponStateComponent`（未搬迁） |

---

## 2. T0 精读指引

每个文件给出：**读什么 → 学到什么**。

### 2.1 `HodgeActivatableWidget`（31 行头 + 45 行实现）

**读什么**：枚举 `EHodgeWidgetInputMode` 的四个取值 → `GetDesiredInputConfig()` 的 `switch` → `ValidateCompiledWidgetTree`。

**学到什么**：CommonUI 的核心契约——控件激活时**声明自己想要的输入配置**（鼠标是否捕获、光标是否隐藏、移动输入是否可用），而不是去改全局输入模式。这是"UI 状态机"与"输入模式"解耦的标准做法。

**注意**：这是 `UCLASS(Abstract, Blueprintable)`，实际用的是它的蓝图子类。

### 2.2 `HodgeTaggedWidget`（32 行头 + 59 行实现）

**读什么**：`NativeConstruct` 里如何订阅"拥有者的 Tag 容器变化" → `OnWatchedTagsChanged` → `SetVisibility` 被重写的原因（保存 `bWantsToBeVisible`，外部改可见性时不算数）。

**学到什么**：**用 Tag 表达"什么时候该出现"**——和 UIExtension 用 Tag 表达"挂在哪"是同一思路的两种应用。你可以照它的模式做"进入战斗才显示血条"。

### 2.3 `IndicatorSystem`（6 文件，主线 B 的全部内容）

| 文件 | 读什么 | 学到什么 |
|---|---|---|
| `IndicatorLibrary` | 唯一函数 `GetIndicatorManagerComponent` | 蓝图函数库怎么当"入口门面"（11 行） |
| `HodgeIndicatorManagerComponent` | `AddIndicator` / `RemoveIndicator` 与两个 `DECLARE_EVENT` | `UControllerComponent` 挂 Controller、事件广播、以及"谁拥有指示器列表" |
| `IndicatorDescriptor` | 属性块 + `FIndicatorProjection` + `GetIndicatorClass()` | 一份"UI 需要什么数据"的完整契约：目标、偏移、优先级、投影参数、控件类 |
| `IActorIndicatorWidget` | `BindIndicator` / `UnbindIndicator` 两个 `BlueprintNativeEvent` | 控件与数据之间只通过接口耦合，控件可以完全不认识游戏系统 |
| `SActorCanvas` | **这是重头**：`OnPaint` / `OnArrangeChildren` / `UpdateCanvas` / `FSlot::SetWasIndicatorClamped` / `AddReferencedObjects` | ① Slate 自定义 Panel 的完整骨架；② 世界坐标 → 屏幕坐标 → 视锥判定 → 屏幕边缘钳制 + 箭头方向计算；③ `FUserWidgetPool` 复用 UMG 控件；④ Slate 非 UObject 的 GC 上报方式 |
| `IndicatorLayer` | 29 行的 `RebuildWidget` | UWidget 与 Slate 的分工：UWidget 管设计器可见性/属性，Slate 管每帧绘制 |

**读完应能回答**：指示器被屏幕边缘钳制时，为什么可见性计算要放在 `OnPaint` 里（const 函数）而不是 `Tick` 里？`bWasIndicatorClamped` 这类 `mutable` 缓存是怎么用的？

### 2.4 主线 C 的四个关键点（跨文件）

| 你要搞懂的 | 在哪 |
|---|---|
| HUD 根控件怎么被创建 | `HodgeHUDLayout`（待复活）+ `AHodgeHUD::BeginPlay` |
| 控件怎么被"挂"到槽位 | `GameFeatureAction_AddWidget` 的 `Widgets[]` 分支 → `UUIExtensionSubsystem::RegisterExtensionAsWidgetForContext` |
| 槽位怎么接收 | `UUIExtensionPointWidget`（待引入，属于 UIExtension） |
| 数据怎么变成控件类 | `UUIExtensionPointWidget::OnAddOrRemoveExtension` + `HodgeWidgetFactory::FindWidgetClassForData` |

---

## 3. 全量速查（按目录）

> 只列尚未在上文表格出现的信息，便于对照磁盘目录。

| 目录 | 文件 | 现状 |
|---|---|---|
| `UI/`（顶层） | `HodgeActivatableWidget`、`HodgeTaggedWidget`、`HodgeJoystickWidget`、`HodgeSimulatedInputWidget`、`HodgeTouchRegion` | ✅ 5 组在编 |
| | `HodgeHUDLayout`、`HodgeSettingScreen`、`HodgeGameViewportClient` | ⏸ 3 组待复活 |
| `Core/HUD/` | `HodgeHUD` | ✅ 已从 `UI/` 迁入 |
| `UI/Basic/` | `MaterialProgressBar` | ✅ |
| `UI/Common/` | `HodgeBoundActionButton`、`HodgeListView`、`HodgeTabButtonBase`、`HodgeTabListWidgetBase`、`HodgeWidgetFactory`、`HodgeWidgetFactory_Class` | ✅ 全部在编 |
| `UI/Foundation/` | `HodgeActionWidget`、`HodgeButtonBase`、`HodgeControllerDisconnectedScreen`、`HodgeLoadingScreenSubsystem` | ✅ 4 组在编 |
| | `HodgeConfirmationScreen` | ⏸ |
| `UI/Frontend/` | `ApplyFrontendPerfSettingsAction`、`HodgeLobbyBackground` | ✅ 2 组在编 |
| | `HodgeFrontendStateComponent` | ⏸ |
| `UI/IndicatorSystem/` | `HodgeIndicatorManagerComponent`、`IActorIndicatorWidget`、`IndicatorDescriptor`、`IndicatorLayer`、`IndicatorLibrary`、`SActorCanvas` | ✅ **全部在编（已完整）** |
| `UI/PerformanceStats/` | `HodgePerfStatContainerBase`、`HodgePerfStatWidgetBase` | ⏸ 2 组 |
| `UI/Subsystem/` | `HodgeUIManagerSubsystem`、`HodgeUIMessaging` | ⏸ 2 组 |
| `UI/Weapons/` | `CircumferenceMarkerWidget`、`SCircumferenceMarkerWidget` | ✅ 2 组在编 |
| | `HitMarkerConfirmationWidget`、`SHitMarkerConfirmationWidget`、`HodgeReticleWidgetBase`、`HodgeWeaponUserInterface` | ⏸ 4 组 |

**统计**：79 文件 = ✅ 53 在编（含整个 IndicatorSystem 11 文件）｜⏸ 26 待复活（其中 25 个是"父类在未引入插件里"或"依赖未搬迁的玩法系统"，另一个是同组连带的 Slate 实现）

---

## 4. 三个"为什么它值钱"的注脚

1. **`SActorCanvas` 是这批文件里唯一的"硬技术"**：屏幕投影、视锥判定、边缘钳制、箭头角度、控件池——这些在任何游戏里都要重写一遍，而且写法与引擎版本无关。602 行也是全部 79 个文件里最长的实现。
2. **`HodgeTaggedWidget` + `UUIExtensionPointWidget` 是同一思想的两次应用**（Tag 决定"何时显示" / Tag 决定"挂在哪里"）。理解一个就能类推另一个，这是 Lyra 的架构底色。
3. **最"小"的两个文件反而最实用**：`HodgeLoadingScreenSubsystem`（40 行）是写子系统的模板；`HodgeWidgetFactory`（8 行实现）是"数据 → 控件"的工厂模式模板。**先照这两个写自己的东西，再回头读大文件**，效率更高。

---

## 5. 为什么会有"这么多种 UI"：四个正交的轴

这是看这批文件时最容易产生的困惑：`IndicatorSystem` 和 `UIExtension` / `HodgeActivatableWidget` / `HodgeTaggedWidget` **看起来毫无关系**，为什么要分这么多类？

**答案是：它们不是同一层的并列物，而是回答四个不同问题的四条轴。**

### 5.1 四条轴

| 轴 | 回答的问题 | 组件 | 典型用例 |
|---|---|---|---|
| **A. 挂载机制** | "这个控件怎么出现在 HUD 上？" | **UIExtension**（`UUIExtensionPointWidget` 槽位 + `UUIExtensionSubsystem`）✅ **已于 2026-09-27 并入模块并编译通过**，细节见 [ui-extension-system.md](ui-extension-system.md) | 某个 GameFeature / 模块想往 HUD 里塞一个控件，又不想让 HUD 认识它 |
| **B. 行为契约** | "它出现之后怎么表现、怎么面对输入？" | **`HodgeActivatableWidget`**（输入模式）／**`HodgeTaggedWidget`**（Tag 驱动显隐） | Esc 菜单要接管输入；血条只在战斗时出现 |
| **C. 承载与根** | "HUD 的根从哪来、谁来创建？" | Lyra 用 **CommonGame 层栈**（`UPrimaryGameLayout` + `GameUIPolicy`）；**本项目决定不用**，改用 `AHodgeHUD` + `CreateWidget` | HUD Layout 的创建与层级 |
| **D. 世界跟随** | "跟随 Actor 的标记怎么画出来？" | **IndicatorSystem**（`Descriptor` → `Manager` → `SActorCanvas`） | 敌人血条、锁定框、任务点、交互提示、队友标记、Ping |

一句话对照：

```text
A 解决"怎么挂"       —— 横切机制
B 解决"挂上之后是什么" —— 类的基类选择
C 解决"挂在哪张 HUD 上" —— 宿主与层级
D 解决"世界跟谁、画在哪" —— 一整套自给自足的子系统
```

### 5.2 它们确实互相不依赖（实测）

在项目里搜过：**IndicatorSystem 的 11 个文件对 `UIExtension` / `HodgeActivatableWidget` / `HodgeTaggedWidget` 的引用数 = 0**。整套 IndicatorSystem 只要求"某张 HUD 蓝图里放了一个 `UIndicatorLayer`"，此外不依赖任何东西。

### 5.3 唯一的交接点在**资产层**（Lyra 实证）

在 Lyra 工程里全盘搜过资产引用，链路是唯一的：

```text
Experience (B_LyraShooterGame_ControlPoints)
   └─ 激活 GameFeature 插件 ShooterCore
        └─ LAS_ShooterGame_StandardHUD （GameFeatureActionSet）
             └─ GameFeatureAction_AddWidget
                  ├─ Layout[]  → PushContentToLayer_ForPlayer(UI.Layer.Game, W_ShooterHUDLayout)
                  │                ↑ 这一步需要 CommonGame 的 UCommonUIExtensions
                  └─ Widgets[] → RegisterExtensionAsWidgetForContext(槽位 Tag, LocalPlayer, 控件类)
                                 ↑ 这一步只需要 UIExtension
                                   │
W_ShooterHUDLayout（同一张蓝图里同时有）
   ├── UUIExtensionPointWidget × N   ← 接收上面 Widgets[] 注入的控件（UIExtension 的落点）
   └── UIndicatorLayer              ← IndicatorSystem 的入口（→ SActorCanvas）
```

**资产层证据**（`findstr` 全工程 `.uasset` 内容）：

| 结论 | 证据 |
|---|---|
| 全工程只有一张蓝图用了 `UIndicatorLayer` | 仅 `Plugins/GameFeatures/ShooterCore/Content/UserInterface/W_ShooterHUDLayout.uasset` |
| 这张 HUD Layout 由 GameFeature 注入 | `LAS_ShooterGame_StandardHUD.uasset`（含 `GameFeatureAction_AddWidget`）是唯一引用 `W_ShooterHUDLayout` 的资产 |
| 扩展点槽位与 Indicator 画布**在同一张蓝图里** | `W_ShooterHUDLayout.uasset` 同时含 `UIExtensionPointWidget` 与 `IndicatorLayer` |
| 其它 HUD Layout（前端的、TopDown 的）只有扩展点、没有 Indicator | `W_FrontEndHUDLayout` / `W_TopDownArenaHUDLayout` 含 `UIExtensionPointWidget`，不含 `IndicatorLayer` |

**所以要记住的只有一句话**：四个轴在**代码层完全解耦**，只在**蓝图里"同一个 HUD Layout 同时放槽位和一个 IndicatorLayer"**时汇合。

### 5.4 决策表：我要做 X → 用哪个

| 我要做的 | 用什么 | 为什么 |
|---|---|---|
| 敌人/队友头顶血条、锁定框、任务点、交互提示、Ping | **IndicatorSystem** | 需要"跟随世界目标 + 投影 + 贴边 + 排序"，这套它全包了 |
| 技能栏、小地图、弹药数这类**固定位置**的 HUD 元素 | 普通 `UUserWidget`，由 HUD 根直接持有（或经 UIExtension 挂进槽位） | 屏幕空间固定，不需要投影 |
| "让某个模块/插件往 HUD 里加一个控件" | **UIExtension** | Tag 槽位 + Handle 生命周期 + 可插拔 |
| "进入战斗才显示、脱离战斗就隐藏" | **`HodgeTaggedWidget`** | Tag 驱动显隐，不用自己写监听 |
| Esc 菜单、设置界面、确认框 | **`HodgeActivatableWidget`** | 需要接管输入 / 光标 / 输入模式切换 |
| 屏幕空间飘字、命中特效 | **都不是**（另起一个屏幕空间池） | 既不属于世界跟随，也不属于槽位 |

### 5.5 会不会管理混乱：三条纪律

1. **先问"哪个空间"**：屏幕空间（固定位置）还是世界空间（跟随 Actor）。世界空间的 UI 一律走 IndicatorSystem，不要另起炉灶。
2. **世界空间 UI 只准"提交工单"**：Gameplay 侧只 `NewObject<UIndicatorDescriptor>` + 填字段 + `AddIndicator`，**不准**自己 `CreateWidget` / 自己 `Tick` / 自己 `ProjectWorldToScreen`。这是这套结构不乱的根基。
3. **一个控件只选一种行为契约基类**：要输入就用 `HodgeActivatableWidget`，要 Tag 显隐就用 `HodgeTaggedWidget`，不要既继承一个又重写另一套可见性逻辑（真要组合，用 `HodgeTaggedWidget` 作为内层子控件）。

**补充**：本项目已决定**不引入 CommonGame**（见[迁移执行文档 §12](lyra-ui-migration-plan.md)），所以上面 C 轴换成"`AHodgeHUD` 里 `CreateWidget` + `AddToViewport`"，A/B/D 三条轴完全不受影响。

---

**变更记录**

| 日期 | 内容 |
|---|---|
| 2026-09-26 | 首版。按 T0/T1/T2/T3 四级覆盖全部 79 个文件；行数为实测有效行/总行。 |
| 2026-09-26 | T0 表格行数改按**当前总行数**重测（IndicatorSystem 与两个基座文件已被补充中文注释）；T0 增加"深入学习路径"，指向 [Indicator UI 系统说明](indicator-ui-system.md)。 |
| 2026-09-26 | 新增 §5「为什么会有'这么多种 UI'：四个正交的轴」：四条轴对照、实测零依赖、Lyra 资产层的唯一交接点（含 `findstr` 证据）、决策表、三条纪律。未修改代码。 |
| 2026-09-27 | §5 的 A 轴标注"已并入模块并编译通过"，链接 [UIExtension 子系统说明](ui-extension-system.md)。 |
