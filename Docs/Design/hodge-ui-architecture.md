# Hodge UI 模块化注入链：架构总览与逐条核实

> **文档性质**：架构总览 + 逐条实测核实（非草案，但与知识库的"已核实当前状态"仍有区别——本文核实的是**代码是否成立**，不包含资产侧接线）。
> **核实日期**：2026-09-27。
> **核实方式**：直接读源码 + 已有构建产物 + 资产字节粗查；本文所有结论均带行号证据（见 §8）。
> **配套文档**：[Lyra UI 架构迁移执行文档](lyra-ui-migration-plan.md)（§13 是待办清单）、[UIExtension：职责边界与完整流程](ui-extension-system.md)、[Indicator UI 系统：职责边界与完整流程](indicator-ui-system.md)

---

## 1. 全景图

```text
                    Hodge Experience
        （Content/Main/Experiences/Exp_HodgeDefaultExperience.uasset）
                           │  Actions / ActionSets
                           │  ⚠️ 代码可达，但资产侧尚未配置任何 AddWidgets
                           ↓
              UGameFeatureAction_AddWidgets
        （类名复数；文件 GameFeatureAction_AddWidget.cpp 单数）

        AddToWorld()
             │  ① 注册"要监听的 Actor 类型"
             │     AddExtensionHandler(AHodgeHUD::StaticClass(), HandleActorExtension)
             ↓
        UGameFrameworkComponentManager  ←──────── 订阅＋广播的中枢
             ↑                                │
             │ ② 注册 Receiver                 │ ④ 回调 ExtensionHandler
             │                                ↓
        AHodgeHUD                        HandleActorExtension
   PreInitializeComponents                     │
             │ AddGameFrameworkComponentReceiver
             │                             ┌────┴────────────────────┐
   BeginPlay │                             │                         │
             │ ③ 广播 NAME_GameActorReady   │ NAME_ExtensionAdded     │ NAME_ExtensionRemoved
             │  ※ 这一句在 Super::BeginPlay │ NAME_GameActorReady     │ NAME_ReceiverRemoved
             │    之前执行                  ↓                         ↓
             └────────────────────►     AddWidgets             RemoveWidgets
                                            │                         │
                                     LocalPlayer（ULocalPlayer）  LayoutsAdded[i]
                                            │                    ->DeactivateWidget()
                            ┌───────────────┴───────────────┐     （容器在过渡后自行移除）
                            │                               │
                        Layout[]                        Widgets[]
                            │                               │
                            ↓                               ↓
              需"根布局 + 层"（本项目尚未落地）   UUIExtensionSubsystem::
              → §13.2 todo ①                  RegisterExtensionAsWidgetForContext(
              ⚠️ 当前 :298 被注释，             SlotID, LocalPlayer, WidgetClass, -1)
                 Layout[] 实际不生效                    │
                            │                          ↓
                            │                 FUIExtension
                            │        （ExtensionPointTag / Priority / Context / Data）
                            │                          │
                            │            通知匹配到的 ExtensionPoint（:444）
                            │                          ↓
                            │      Tag 匹配 + DataClass 白名单 + Context 校验
                            │        （不匹配 = 静默不显示，见 §5）
                            │                          ↓
                            │        UUIExtensionPointWidget::OnAddOrRemoveExtension（:214）
                            │                          │
                            │            ┌─────────────┴─────────────┐
                            │         Added                       Removed
                            │            ↓                           ↓
                            │     CreateEntryInternal           RemoveEntryInternal
                            │      （:233 / :264）                （:286，经
                            │            │                         ExtensionMapping
                            │            ↓                         查回 Widget 实例）
                            │      具体业务 Widget
                            ↓
             具体层容器 UCommonActivatableWidgetStack
                 （引擎 CommonUI；Lyra 里由 UPrimaryGameLayout 持有）
                            │
             ┌──────────────┼──────────────────────┐
             ↓              ↓                      ↓
        固定 HUD 控件   UUIExtensionPointWidget   UIndicatorLayer
                          （槽位宿主编排）          （UWidget 容器层）
                                                       │ RebuildWidget()
                                                       ↓
                                                  SActorCanvas
                                       （真正负责 Indicator 投影 / 布局 /
                                         排序 / 屏幕边缘 Clamp）
```

---

## 2. 安装链逐步说明

| 步 | 动作 | 代码位置 |
|---|---|---|
| ① | Action 监听"`AHodgeHUD` 这个类型"，而不是监听某个具体实例 | `GameFeatureAction_AddWidget.cpp:160-205`（`AddToWorld`） |
| ② | HUD 把自己注册成 GameFramework Component Receiver | `HodgeHUD.cpp:46-56`（`PreInitializeComponents`） |
| ③ | HUD 广播 `NAME_GameActorReady`，**注意在 `Super::BeginPlay()` 之前** | `HodgeHUD.cpp:59-70` |
| ④ | Manager 找到匹配的 ExtensionHandler，回调 `HandleActorExtension` | `GameFeatureAction_AddWidget.cpp:234-255` |
| ⑤ | 按事件名分流：Added/Ready → `AddWidgets`；Removed/ReceiverRemoved → `RemoveWidgets` | 同上 |
| ⑥ | 取 `LocalPlayer`，然后走 `Layout[]` 与 `Widgets[]` 两条独立支路 | `GameFeatureAction_AddWidget.cpp:257-330` |

> ⚠️ **步 ③ 的时序是这条链最容易踩的坑**：`SendGameFrameworkComponentExtensionEvent` 排在 `Super::BeginPlay()` 之前，所以 `AddWidgets` 执行时"根布局是否已经存在"完全取决于你在 HUD 生命周期里把它创建在哪一句之前。若创建代码放在发事件之后，`PushContentToLayer_ForPlayer` 会因为找不到层而返回空（且不打日志）。

---

## 3. 两条分发支路

### 3.1 `Layout[]`：安装"层里的界面"

| 项 | 内容 |
|---|---|
| 数据结构 | `FHodgeHUDLayoutRequest`：`TSoftClassPtr<UCommonActivatableWidget> LayoutClass` + `FGameplayTag LayerID`（`GameFeatureAction_AddWidget.h:16-27`） |
| 语义 | `{LayerID} -> {LayoutClass}`，即"把某个 ActivatableWidget 放进指定层" |
| 产物 | `FPerActorData::LayoutsAdded`（`TArray<TWeakObjectPtr<UCommonActivatableWidget>>`，`.h:81`） |
| Lyra 实现 | `UCommonUIExtensions::PushContentToLayer_ForPlayer` → `LocalPlayer → GameUIManagerSubsystem → GameUIPolicy → GetRootLayout() → UPrimaryGameLayout::PushWidgetToLayerStack()` |
| 本项目现状 | **`:298` 已被注释**，`Layout[]` 不生效 → HUD Layout 不会被创建 → §13.2 todo ① 未落地 |

### 3.2 `Widgets[]`：安装"槽位里的界面"

| 项 | 内容 |
|---|---|
| 数据结构 | `FHodgeHUDElementEntry`：`TSoftClassPtr<UUserWidget> WidgetClass` + `FGameplayTag SlotID`（`.h:31-42`） |
| 语义 | `{SlotID} -> {WidgetClass}`，即"把某个 UUserWidget 注册到某个扩展点" |
| 调用 | `RegisterExtensionAsWidgetForContext(SlotID, LocalPlayer, WidgetClass, -1)`（`:327`） |
| 产物 | `FUIExtensionHandle`，存进 `FPerActorData::ExtensionHandles`（`.h:82`） |
| 接收端 | `UUIExtensionPointWidget` 用 `ExtensionPointTag` 匹配，`DataClasses` 白名单 + Context 校验通过后才 `CreateEntryInternal` 创建实例 |
| 关键设计 | 扩展点控件维护 `ExtensionMapping`（`ExtensionHandle → Widget 实例`，`UIExtensionPointWidget.cpp:240`），Removed 时凭 Handle 精确删除（`:286`） |

> 这条支路是**解耦的核心**：业务 GameFeature 只声明"我要挂到 `SlotID`"，完全不需要知道 HUD 的 Widget Tree 长什么样。

---

## 4. 卸载链（两条，务必区分）

### 4.1 GameFeature / Experience 停用 → `Reset`

```text
GameFeature Deactivate（或 Experience 切换）
        ↓
UGameFeatureAction::OnGameFeatureDeactivating（HodgeExperienceManagerComponent.cpp:588 调用）
        ↓
UGameFeatureAction_AddWidgets::OnGameFeatureDeactivating（:46）
        ↓
Reset(ActiveData)（:207）
        ├── ActiveData.ComponentRequests.Empty()          ← 释放对 Manager 的订阅
        ├── 每个 FUIExtensionHandle.Unregister()（:225）
        │        ↓
        │   UUIExtensionSubsystem::UnregisterExtension（UIExtensionSystem.cpp:412）
        │        ↓
        │   NotifyExtensionPointsOfExtension(EUIExtensionAction::Removed, ...)（:444）
        │        ↓
        │   UUIExtensionPointWidget::OnAddOrRemoveExtension（:214）→ RemoveEntryInternal（:286）
        └── ActiveData.ActorData.Empty()
```

### 4.2 HUD 消失 / Receiver 移除 → `RemoveWidgets`

```text
NAME_ExtensionRemoved / NAME_ReceiverRemoved
        ↓
RemoveWidgets(:333)
        ├── LayoutsAdded 里的每个控件 → DeactivateWidget()（:356）
        │        ↓
        │   引擎 CommonActivatableWidgetContainer.cpp:286
        │   (DisplayedWidget->OnDeactivated() 绑定 HandleActiveWidgetDeactivated)
        │   → 容器在过渡结束后自行移除该控件
        └── ExtensionHandles 里每个 Handle.Unregister()（:367）
```

> **两条链的分工必须记清**：
> - `Reset` **不负责卸载 `Layout[]` 推送出来的控件**，它只断开订阅 + 注销 Extension，然后 `ActorData.Empty()` 把记录全丢掉。
> - 因此 `Layout` 的撤销**只能靠 `RemoveWidgets`**（由 HUD 侧的 Removed 事件驱动）。
> - 另一处细节：`RemoveWidgets` 用的是 `DeactivateWidget()` 而**不是** `Stack->RemoveWidget()`。前者是"通知控件停用，容器接手移除"，后者是"直接命令容器移除"（会先把 active widget 停用再移除）。两种都成立，但语义不同。

---

## 5. 三张连接契约

整套解耦靠三样东西连接，任何一处不匹配都是**静默失效**（不崩、不报错）：

| 契约 | 连接什么 | 不匹配的后果 |
|---|---|---|
| **Layer Tag**（`UI.Layer.*`） | `Layout[i].LayerID` ↔ 根布局里的层容器 | 找不到层 → Push 返回空（`PushContentToLayer_ForPlayer` 内部 `return nullptr`，无日志） |
| **ExtensionPoint Tag + Context**（`SlotID` ↔ `ExtensionPointTag`） | `Widgets[i].SlotID` ↔ `UUIExtensionPointWidget` | 匹配失败 → 业务 Widget 永远不创建，界面"少一块"但无任何报错 |
| **Handle 生命周期**（`FUIExtensionHandle` / `FUIExtensionPointHandle`） | 注册方 ↔ 接收方 | Handle 必须**手动** `Unregister()`；漏掉就是 UI 残留或野引用 |

> 第三张契约的 `DataClasses` 白名单 + `EUIExtensionPointMatch`（Exact / Partial）也属于匹配条件，详见 `UIExtensionSystem.h:42-76` 与 [UIExtension 文档](ui-extension-system.md)。

---

## 6. 六处需要修正的说法

流传的版本整体方向正确（函数名 `CreateEntryInternal` / `RemoveEntryInternal` 核实无误），以下 6 处需要修正：

| # | 原表述 | 修正 |
|---|---|---|
| 1 | `Layout[] → CommonUI Layer → UHodgeHUDLayout` | `LayoutClass` 是 `TSoftClassPtr<UCommonActivatableWidget>`，**可以是任何 ActivatableWidget**，不一定是 `UHodgeHUDLayout`；而且当前项目里它**不能**是 `UHodgeHUDLayout`（该类是 `UCLASS(Abstract)`，见 `HodgeHUDLayout.h:42`，必须指向具体 WBP 子类）。 |
| 2 | 同上（层与 HUDLayout 的上下游） | **当前实现里层与 HUDLayout 的持有关系是反的**：Lyra 是"根布局（`UPrimaryGameLayout` / `W_OverallUILayout`）持有层，HUDLayout 被 push 进层"；本项目目前是"`UHodgeHUDLayout` 自己持有 `MenuLayerStack`"。等 §13.2 todo ① 落地后才回到 Lyra 的结构。 |
| 3 | 卸载只画了 `Reset` | 漏了 `RemoveWidgets` 这条支路，而 `Layout[]` 的撤销恰好**只能**靠它（`Reset` 只释放订阅 + 注销 Extension + 清记录）。见 §4。 |
| 4 | 顶部 `Experience → ActionSet → AddWidgets` | 代码链成立，但**资产侧完全没接线**：项目里没有任何 GameFeature 插件，`Exp_HodgeDefaultExperience.uasset` 中也搜不到 `AddWidgets` / `HodgeHUDElementEntry` / `HodgeHUDLayoutRequest`。 |
| 5 | `GameFrameworkComponentManager` 画成单向 ↑ | Manager 是**双向中枢**：Action 先向它注册 ExtensionHandler（①），HUD 再向它注册 Receiver（②）并广播 Ready（③），它才回调 Action（④）。图上单向箭头容易读成"HUD 直接通知 Action"。 |
| 6 | "业务 GameFeature 不需要知道 HUD" | 对 Widget 层成立，但**注入器本身硬编码了 `AHodgeHUD::StaticClass()`**（`AddToWorld`，`:160-205`）。解耦发生在"Widget 与 HUD 结构"之间，不在"HUD Actor 类型"这一层。 |

补充两个容易记混的命名：

- 类名 `UGameFeatureAction_AddWidgets`（**复数**）、文件名 `GameFeatureAction_AddWidget.cpp`（**单数**）、`DisplayName` 是 "Add Widgets"。
- `FUIExtensionHandle::Unregister()` **不自动调用**，必须由持有者在 `Reset` / `RemoveWidgets` 里逐个调用（`UIExtensionSystem.cpp:412`）。

---

## 7. 当前项目现状：代码成立 vs 资产未接线

### ✅ 已在 C++ 侧成立

| 环节 | 状态 |
|---|---|
| HUD 作为 GameFramework Receiver | ✅ `HodgeHUD.cpp:46-56` |
| HUD Ready 事件广播 | ✅ `HodgeHUD.cpp:59-70`（顺序已在 §2 标注） |
| Action 监听 HUD 类型 | ✅ `GameFeatureAction_AddWidget.cpp:160-205` |
| 事件分流 | ✅ `:234-255` |
| `Widgets[]` → UIExtension 注册 | ✅ `:309-328`（唯一**未被注释**的分支） |
| 扩展点接收 → 创建/移除条目 | ✅ `UIExtensionPointWidget.cpp:214-296` |
| `Handle.Unregister()` → 通知扩展点 | ✅ `UIExtensionSystem.cpp:412/444` |
| Indicator 层 → SActorCanvas | ✅ `IndicatorLayer.h`（`RebuildWidget()` 内创建） |
| Experience 停用 → `OnGameFeatureDeactivating` | ✅ `HodgeExperienceManagerComponent.cpp:580-606` |

### ❌ 尚未接线的部分

| 项 | 现状 | 证据 |
|---|---|---|
| `Layout[]` 支路 | `:298` 调用被注释 → **HUD Layout 不会被创建** | `GameFeatureAction_AddWidget.cpp:298` |
| `CommonUIExtensions.h` | `:22` 已注释（CommonGame 插件不存在） | 同文件 `:22` |
| GameFeature 插件 | 项目里**一个都没有** | `Plugins/` 只有 ALS、RiderLink、McpAutomationBridge、UnrealMCP |
| `AddWidgets` Action 配置 | 无任何资产配置过 | `Content/Main` 全量扫描未命中 `AddWidgets` |
| HUD 布局 WBP / 槽位蓝图 | 不存在 | `Content` 下仅 5 个 `WBP_*`，全在 `CodexText` |
| CommonUI Action 输入映射 | `Config/*.ini` 中 `CommonUI` 匹配 0 处 | 实测 |
| Experience 卸载完整性 | 只完成"停用"，未完成资源卸载 | `HodgeExperienceManagerComponent.cpp:23,25,647` 的 TODO |

**结论**：这是一条"**代码已画好、资产还没插上电**"的链。要让它真正跑起来（按依赖顺序）：

```text
① 层栈 + 根布局落地（§13.2 todo ①）→ 打开 Layout[] 支路
② 建 GameFeature 插件 / 或在 Experience 上直接配 Actions
③ 建 HUD Layout WBP（含 4 个 ActivatableWidgetStack + RegisterLayer 连线）
④ 用 UUIExtensionPointWidget 做槽位，配 SlotID
⑤ Config/DefaultInput.ini 补 CommonUI 的 UI.Action.* 映射
```

---

## 8. 证据索引

```text
Source\Hodgepodge\Public\GameFeatures\GameFeatureAction_AddWidget.h
    :16-27   FHodgeHUDLayoutRequest（LayoutClass + LayerID）
    :31-42   FHodgeHUDElementEntry（WidgetClass + SlotID）
    :50-51   UGameFeatureAction_AddWidgets  声明
    :72,:76  Layout / Widgets 配置数组
    :81-82   FPerActorData::LayoutsAdded / ExtensionHandles

Source\Hodgepodge\Private\GameFeatures\GameFeatureAction_AddWidget.cpp
    :22      //#include "CommonUIExtensions.h"（已注释）
    :46-63   OnGameFeatureDeactivating
    :65-81   AddAdditionalAssetBundleData（编辑器）
    :83-158  IsDataValid（编辑器）
    :160-205 AddToWorld → AddExtensionHandler
    :207-232 Reset（:225 Handle.Unregister）
    :234-255 HandleActorExtension（事件分流）
    :257-330 AddWidgets（:298 已注释的 Push；:327 UIExtension 注册）
    :333-... RemoveWidgets（:356 DeactivateWidget；:367 Handle.Unregister）

Source\Hodgepodge\Private\Core\HUD\HodgeHUD.cpp
    :46-56   PreInitializeComponents → AddGameFrameworkComponentReceiver
    :59-70   BeginPlay（SendGameFrameworkComponentExtensionEvent 在 Super::BeginPlay 之前）
    :73-82   EndPlay → RemoveGameFrameworkComponentReceiver

Source\Hodgepodge\Private\UI\Extension\UIExtensionPointWidget.cpp
    :45      ReleaseSlateResources
    :123     ResetExtensionPoint
    :142     RegisterExtensionPoint
    :186     RegisterExtensionPointForPlayerState
    :214-296 OnAddOrRemoveExtension（:233,:264 CreateEntryInternal；:240 ExtensionMapping；:286 RemoveEntryInternal）
    :297     ValidateCompiledDefaults

Source\Hodgepodge\Private\UI\Extension\UIExtensionSystem.cpp
    :341     NotifyExtensionPointsOfExtension(Added, ...)
    :400     ExtensionPoint->Callback.ExecuteIfBound(Action, Request)
    :412-464 UnregisterExtension（:444 通知 Removed）
    :466     UnregisterExtensionPoint
    :539     ExtensionCallback.ExecuteIfBound(Action, Request)

Source\Hodgepodge\Public\UI\Extension\UIExtensionSystem.h
    :388-399 RegisterExtensionAsWidget / ForContext / AsData
    :403-407 UnregisterExtension / UnregisterExtensionPoint

Source\Hodgepodge\Public\UI\IndicatorSystem\IndicatorLayer.h
    UCLASS() UIndicatorLayer : public UWidget；RebuildWidget() 内创建 SActorCanvas

Source\Hodgepodge\Public\UI\HodgeHUDLayout.h
    :42      UCLASS(Abstract, BlueprintType, Blueprintable, ...)  ← LayoutClass 必须指向 WBP 子类
    成员      MenuLayerStack（当前实现：HUDLayout 自己持有层）

Source\Hodgepodge\Private\Component\HodgeExperienceManagerComponent.cpp
    :23,:25  TODO：停用不完整 / GameFeature 只激活不完整处理停用
    :580-606 DeactivateListOfActions（:588 Action->OnGameFeatureDeactivating）
    :645-650 OnAllActionsDeactivated（:647 TODO：只停用未卸载资源）

引擎（UE 5.5）
    Engine\Plugins\Runtime\CommonUI\Source\CommonUI\Private\Widgets\CommonActivatableWidgetContainer.cpp
        :286 DisplayedWidget->OnDeactivated() 绑定 HandleActiveWidgetDeactivated
        :187 RegisterInstanceInternal
    Engine\Plugins\Runtime\CommonUI\Source\CommonUI\Public\Widgets\CommonActivatableWidgetContainer.h
        :31,:47 AddWidget ; :68 RemoveWidget ; :73 GetWidgetList ; :187-188 UCommonActivatableWidgetStack

项目侧未接线的资产证据（2026-09-27 实测）
    Plugins\                无任何 GameFeature 插件（仅 ALS / RiderLink / McpAutomationBridge / UnrealMCP）
    Content\Main\Experiences\Exp_HodgeDefaultExperience.uasset   6484 字节，未命中 AddWidgets /
                                                                 HodgeHUDElementEntry / HodgeHUDLayoutRequest
    Config\*.ini            CommonUI / CommonInput 匹配 0 处
    Content\                仅 5 个 WBP_*（全在 CodexText），无 HUD 布局或槽位蓝图
```

---

**变更记录**

| 日期 | 内容 |
|---|---|
| 2026-09-27 | 首版。对"AddWidgets 注入链全景图"逐条实测核实，修正 6 处表述，补 `RemoveWidgets` 卸载支路与三张连接契约。本轮仅写文档，未改代码。 |
