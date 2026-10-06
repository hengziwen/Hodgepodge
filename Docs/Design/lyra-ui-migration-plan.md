# Lyra UI 架构迁移执行文档（阶段一）

> 2026-10-06 状态同步：UI 迁移/学习材料：CommonUI 已引入，部分 UIExtension/Indicator 代码有效；依赖 CommonGame/GameSettings/CommonUser 的停用内容与完整前端/HUD 尚未接通。本文不作为游戏 UI 已出画面的声明。 当前项目事实见 [本轮更新](../KnowledgeBase/26-update-2026-10-06.md)。

> **文档状态：执行草案，尚未实现。** 最近更新 2026-09-25。
> 本文档只做计划，**未修改任何代码、资产、插件或配置**；文中涉及的阶段①~④全部未编译、未 PIE、未做蓝图验证。
> 不得把本文档描述成"已迁移 / 已验证"。
> 目标不是复刻 Lyra 的完整前端，而是**学清楚 Lyra UI 的分层架构**：加载屏判定链、UI 扩展点、层栈与输入挂起。

**相关文档**

- [AI 开发与验证流程](../AI_DEVELOPMENT.md)
- [项目开发约定](../../AGENTS.md)
- [当前状态、断点与接通顺序](../KnowledgeBase/12-integration-backlog.md)
- [GameFeature 现状](../KnowledgeBase/10-game-features.md)

**外部参考实现**

```text
Lyra 源码：E:\Project\UProject\study\Epic\LyraStarterGame   （本文所有行号均指向此目录）
本机引擎：E:\UE\UE_5.5
项目：    e:\Project\Git\Hodgepodge
```

---

## 0. 事实基线

### 0.1 已核实事实（实测日期 2026-09-25）

| 编号 | 事实 | 证据 |
|---|---|---|
| F1 | 引擎 5.5 的 CommonUI 插件只含 `CommonUI` / `CommonInput` / `CommonUIEditor` 三个模块 | `E:\UE\UE_5.5\Engine\Plugins\Runtime\CommonUI` |
| F2 | `CommonGame` / `UIExtension` / `CommonLoadingScreen` / `CommonUser` / `GameSubtitles` / `GameplayMessageRouter` / `ModularGameplayActors` 均**不在引擎**，全部来自 Lyra | 引擎 `Plugins\Runtime` 全列表比对 |
| F3 | 引擎只有 `ModularGameplay` 插件且只有**一个模块**；`ModularGameplayActors` 是 Lyra 插件 | `Engine\Plugins\Runtime\ModularGameplay\ModularGameplay.uplugin` |
| F4 | **CommonGame 编译期强依赖 CommonUser**：`CommonGame.Build.cs` 的 Public 里有 `"CommonUser"`；`CommonGameInstance.cpp:8` include `CommonUserSubsystem.h`，`:89/:98` 取 `UCommonUserSubsystem` / `UCommonSessionSubsystem` | 见左列文件行号 |
| F5 | `CommonGame.uplugin` 另外声明依赖 `OnlineFramework` 插件 | `Plugins\CommonGame\CommonGame.uplugin` |
| F6 | `UIExtension.Build.cs` 的 Public 依赖里写了 `"CommonGame"`，但**源码 0 处引用** CommonGame | `UIExtensionSystem.h:5-9` 只有 GameplayTagContainer / BlueprintFunctionLibrary / WorldSubsystem |
| F7 | `UIExtension.uplugin` 的 `"Plugins"` 键**写了两次**（先 CommonUI、后 CommonGame），JSON 后者覆盖前者 | `Plugins\UIExtension\UIExtension.uplugin` |
| F8 | 项目已存在 `ILoadingProcessInterface`，且 `UHodgeExperienceManagerComponent` 已实现 `ShouldShowLoadingScreen` | `Source/Hodgepodge/Public/Interface/LoadingProcessInterface.h:11,21,27,31`；`Public/Component/HodgeExperienceManagerComponent.h:37,52`；`Private/Component/HodgeExperienceManagerComponent.cpp:627` |
| F9 | 项目的 `LoadingProcessInterface.cpp` **只有 8 行**，**静态辅助函数 `ShouldShowLoadingScreen(UObject*, FString&)` 已声明但无实现** | `Private/Interface/LoadingProcessInterface.cpp:8` 仅剩一句上游注释 |
| F10 | `GameFeatureAction_AddWidget.h/.cpp` 在项目里**整文件被注释**，无有效声明/实现 | `Public/GameFeatures/GameFeatureAction_AddWidget.h` 全文 `//` 前缀 |
| F11 | 项目已有 `UHodgeLocalPlayerBase`（`: public ULocalPlayer`），具备 Lyra `UCommonLocalPlayer` 的全部委托与 `CallAndRegister_*`，`GetRootUILayout()` 被注释为"预留接口,待实现" | `Public/Core/LocalPlayer/HodgeLocalPlayerBase.h:31,52-109,143-144` |
| F12 | 项目已有 `UHodgeGameInstanceBase`（`: public UGameInstance`） | `Public/Core/GameInstance/HodgeGameInstanceBase.h:23` |
| F13 | ini 已把 `GameInstanceClass` / `LocalPlayerClassName` 指向上述两个类；**没有 `GameViewportClientClassName`** | `Config/DefaultEngine.ini` `[/Script/EngineSettings.GameMapsSettings]`、`[/Script/Engine.Engine]` |
| F14 | ini 已有 20+ 条 `ClassRedirects`（`LyraXxx` → `HodgeXxx`），项目既定做法是"改名 + 重定向" | `Config/DefaultEngine.ini` |
| F15 | Lyra 侧对 CommonGame 的 include 调用面为 **11 个文件 / 12 处** | `Source/LyraGame` 全量 include 扫描 |
| F16 | `ULyraAudioMixEffectsSubsystem` 是 LyraGame 里唯一使用 LoadingScreen 的类型 | `LyraAudioMixEffectsSubsystem.cpp:28,30,214,216,217` |
| F17 | LyraGame 中**没有任何地方调用 `RegisterLoadingProcessor`**；管理器靠遍历 GameState / 其组件 / PlayerController / 其组件来发现处理器 | `LoadingScreenManager.cpp:326-349` |
| F18 | `ULyraUIManagerSubsystem` 用 `FTSTicker` 每帧轮询同步 `AHUD::bShowHUD` → 根布局可见性 | `LyraUIManagerSubsystem.cpp:23,33-38,40-68` |

### 0.2 对既有外部评估的三条更正

| 外部说法 | 实际情况 | 影响 |
|---|---|---|
| "CommonUser 按需 / 单机可砍" | ❌ 不成立，见 F4、F5。要 CommonGame 就绕不开 CommonUser 与在线子系统 | 直接改变依赖图与工作量 |
| "UIExtension 零 Lyra 依赖，先做练手" | ⚠️ 只对一半：源码确实零引用，但 `Build.cs` 声明了 CommonGame（F6）。需先删该依赖才能独立 | 影响顺序与独立编译可行性 |
| "ModularGameplayActors 引擎/自带" | ❌ 错，见 F3。它是 Lyra 插件（15 文件 / 356 行） | 需一并纳入或规避 |

一条**顺序更正**：真正的"零项目依赖"起点是 **CommonLoadingScreen**，不是 UIExtension。

### 0.3 规模实测（已排除 `Intermediate` 生成文件）

| 插件 / 目录 | 文件 | 行数 |
|---|---|---|
| CommonGame（全量） | 29 | 2389 |
| └ CommonGame（本计划内核子集） | 16 | 1318 |
| UIExtension | 7 | 751 |
| CommonLoadingScreen | 13 | 891 |
| CommonUser | 12 | 5029 |
| GameSettings | 59 | 4336 |
| GameSubtitles | 10 | 541 |
| GameplayMessageRouter | 9 | 777 |
| ModularGameplayActors | 15 | 356 |
| **Source/LyraGame/UI** | **79** | **4924** |
| Source/LyraGame/Settings | 34 | 5730 |

---

## 1. 范围与前置决策

### 1.1 本计划做

| 阶段 | 内容 | 规模 | 学习点 |
|---|---|---|---|
| ① | CommonLoadingScreen | ~750 行 | 多来源加载状态汇聚、`IInputProcessor` 吞输入、最短显示时长、加载挂起心跳 |
| ② | UIExtension | 751 行 | Tag 作挂载点、Handle 生命周期、手动 GC 引用、`DynamicEntryBox` 槽位 |
| ③ | CommonGame 内核：`PrimaryGameLayout` + `GameUIPolicy` + `GameUIManagerSubsystem` + `CommonUIExtensions` | ~1300 行 | 层栈、异步推 + 输入挂起 token、每玩家根布局、`Within`/`GetOuter` 强绑定 |
| ④ | Hodge 侧换父类 + ini 绑定 | 小 | 反射布局变更与配置联动 |
| ⑤（顺带） | 让 `GameFeatureAction_AddWidget` 的注释版复活 | — | 阶段③④的端到端验收物 |

### 1.2 本计划明确不做

| 不做 | 理由 |
|---|---|
| CommonUser / GameSettings / `Source/LyraGame/Settings` | 合计约 1.5 万行，耦合在线账号与平台；与"UI 分层架构"这一学习目标无关 |
| `CommonStartupLoadingScreen` 模块（5 文件 118 行） | 服务 `PreLoadingScreen` 阶段，依赖 MoviePlayer；与运行时加载屏是两件事 |
| `CommonGame` 的会话/用户分支（`CommonGameInstance` 的 session 相关部分）、`CommonPlayerInputKey`、三个蓝图异步节点 | 学习价值低、成本高；阶段③只取内核 |
| GameplayMessageRouter | UI 链路不依赖（F15 的调用面里没有它） |
| 迁移 `Source/LyraGame/UI/**` 全目录 | 79 文件 4924 行，且会牵入 Settings；学习阶段只按需取 `LyraActivatableWidget` / `LyraTaggedWidget` / `LyraHUDLayout` 三个基座 |

### 1.3 开工前必须拍板的决策

| 编号 | 决策 | 选项 | 影响 |
|---|---|---|---|
| **D1** | 落点：独立插件 还是 并入 `Hodgepodge` 模块 | A. `Plugins/<Name>/` 保持与 Lyra 一致的插件形态<br>B. 全部并入 `Source/Hodgepodge`（沿用项目单模块约定） | A 便于与 Lyra 逐文件对照；B 与 AGENTS.md"单一 Runtime 模块"一致。**注意阶段①的接口已在 Hodgepodge 模块内（F8），选 A 会产生同名类型冲突** |
| **D2** | `Common*` 前缀改不改 | A. 保留 `UCommonLoadingScreenManager` 等原名<br>B. 改成 `UHodge*`（沿用 F14 的既定做法） | B 需要同步写 CoreRedirects 并核对蓝图父类；A 与"全 Hodge 前缀"风格不一致 |
| **D3** | 阶段①接入哪个消费者 | A. 只接 `UHodgeExperienceManagerComponent`（已有 `ShouldShowLoadingScreen`）<br>B. 同时评估音频混音（F16 在 Lyra 是唯一消费者） | A 就能验证主链；B 需要有对应的混音资产 |

---

## 2. 阶段 ①：CommonLoadingScreen

### 2.1 移植清单

源目录：`LyraStarterGame\Plugins\CommonLoadingScreen\Source\`

| 源文件 | 行数 | 处理 |
|---|---|---|
| `CommonLoadingScreen\Public\LoadingScreenManager.h` | 95 | 移植。契约：`UGameInstanceSubsystem` + `FTickableGameObject`；`GetLoadingScreenDisplayStatus()`、`OnLoadingScreenVisibilityChangedDelegate()`、`RegisterLoadingProcessor` / `UnregisterLoadingProcessor` |
| `CommonLoadingScreen\Private\LoadingScreenManager.cpp` | 533 | 移植，**核心**。含静态辅助函数实现（`:46`）、判定链（`:255-405`）、最短时长（`:407-465`）、显示/隐藏与输入吞噬（`:473`+） |
| `CommonLoadingScreen\Private\CommonLoadingScreenSettings.h` | 51 | 移植。`UDeveloperSettingsBackedByCVars`，`config=Game, defaultconfig`，`LoadingScreenWidget` 软类 + `LoadingScreenZOrder` + `HoldLoadingScreenAdditionalSecs` + 心跳与调试开关 |
| `CommonLoadingScreen\Private\CommonLoadingScreenSettings.cpp` | 7 | 移植 |
| `CommonLoadingScreen\Public\LoadingProcessTask.h` / `.cpp` | 24 / 37 | 移植（可选）。`ULoadingProcessTask` 是"临时挂起加载屏"的入口，自己注册/注销 |
| `CommonLoadingScreen\Private\CommonLoadingScreenModule.cpp` | 3 | 按 D1 决定：并入主模块则不需要 |
| `CommonLoadingScreen\Public\LoadingProcessInterface.h` | 23 | **不带入**，改用项目已有的同名接口（见 2.2） |
| `CommonStartupLoadingScreen\**`（5 文件） | 118 | 不做（见 1.2） |

### 2.2 冲突与前置修复

**C1（必做前置）**：项目的静态辅助函数缺实现。Lyra 在 `LoadingScreenManager.cpp:46-63` 提供：

```cpp
bool ILoadingProcessInterface::ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason)
```

它内部 `Cast<ILoadingProcessInterface>` 后转发，并用 `ensureMsgf` 强制"要显示时必须给出原因"。**判定链完全依赖这个函数**（`:326/:334/:345/:364/:372`），所以移植前必须先补上项目版实现，否则阶段① 编译可以通过但链接调用点会失败。

**C2**：项目已有的 `ILoadingProcessInterface` 与插件的**完全同名**。必须二选一，不要两个都留。建议沿用项目版本，把插件那份的**实现体**并入项目 `.cpp`。

**C3**：`UCommonLoadingScreenSettings` 是 `defaultconfig`，ini 段落为 `[/Script/<模块名>.CommonLoadingScreenSettings]`（模块名取决于 D1/D2）。`LoadingScreenWidget` 必须指向一张实际存在的 Widget 蓝图，否则会回退到 `SThrobber` 占位（`:520`）。

### 2.3 关键学习点（按 Lyra 行号）

| 主题 | 位置 | 要点 |
|---|---|---|
| 判定链 | `LoadingScreenManager.cpp:255-405` | 顺序：Force CVar → WorldContext → World → GameState 存在 → `bCurrentlyInLoadMap` → `TravelURL` 非空 → `PendingNetGame` → `HasBegunPlay` → 无缝传送 → 问 GameState → 问 GameState 的组件 → 问外部注册的处理器 → 问 PlayerController 及其组件 → 分屏/非分屏的本地 PC 计数兜底。**每一步都写 `DebugReasonForShowingOrHidingLoadingScreen`**，这是可观测性的关键 |
| 最短显示时长 | `:407-465` | 需求消失后仍保持 `HoldLoadingScreenAdditionalSecs`，期间把 `bDisableWorldRendering = false` 让贴图真正流进来；编辑器中默认不生效（`HoldLoadingScreenAdditionalSecsEvenInEditor`） |
| 输入吞噬 | `:473-533`、`StartBlockingInput` | 用 `IInputProcessor`（不是 `SetInputMode`）吞掉全部输入；同时 `FSlateApplication::Get().Tick()` 保证首帧立刻显示 |
| 加载挂起检测 | `:232`、`:246` | `FThreadHeartBeat::MonitorCheckpointStart/End` + `LoadingScreenHeartbeatHangDuration` |
| 地图加载事件 | `:201-221` | `FCoreDelegates::PreLoadMap` / `PostLoadMap`（注册位置在 `Initialize`） |
| 首屏守卫 | `:467-484` | `FPreLoadScreenManager::HasValidActivePreLoadScreen()`：引擎自己的加载屏还在时不要抢屏 |
| 发现机制 | `:326-349` | **不依赖注册**：主动遍历世界对象；`ExternalLoadingProcessors` 只是补充通道（纠正了外部评估"ExperienceManager 会注册处理器"的说法） |

### 2.4 落地步骤

1. 补 `ILoadingProcessInterface::ShouldShowLoadingScreen(UObject*, FString&)` 实现（C1），并核对 `UHodgeExperienceManagerComponent.cpp:627` 的返回值语义与之一致。
2. 按 D1 建立落点目录；按 D2 处理类名与日志类别名（`LogLoadingScreen`）。
3. 先只移植"设置 + 管理器 + 任务"三个单元，**不带 Startup 模块、不带接口文件**。
4. 按实际 include 收敛 Build.cs 依赖。Lyra 的公开依赖是 `Core`，私有依赖是 `CoreUObject` / `Engine` / `Slate` / `SlateCore` / `InputCore` / `PreLoadScreen` / `RenderCore` / `DeveloperSettings` / `UMG`；实测 `LoadingScreenManager.cpp:5-28` 需要 `HAL/ThreadHeartBeat.h`、`PreLoadScreen(.Manager).h`、`ShaderPipelineCache.h`、`SlateApplication.h`、`IInputProcessor.h`、`SThrobber.h`。
5. 在编辑器里创建加载屏 Widget 蓝图，填进设置类的 `LoadingScreenWidget`。
6. 按 AI_DEVELOPMENT.md 做 Editor 常规构建，再补 Game 构建。

### 2.5 验收标准

| # | 检查 | 期望 | 失败特征 |
|---|---|---|---|
| V1-1 | Editor 构建 | 退出码 0 | 链接错误 → 多半是 C1 未补 |
| V1-2 | PIE 启动 | 初始加载阶段出现加载屏，GameState 就绪后消失 | 一直不消失 → 判定链某步恒为 true（看 `DebugReasonForShowingOrHidingLoadingScreen`） |
| V1-3 | 日志 | 出现 `LogLoadingScreen` 的显示/隐藏原因 | 类别名不匹配会导致"看不到任何日志" |
| V1-4 | 输入 | 加载屏期间点击/按键不进入游戏 | 未装 `IInputProcessor` |
| V1-5 | 地图切换 | 切换地图时重新出现 | PreLoadMap/PostLoadMap 未注册 |

**不得声称**：V1-2~V1-5 未实际执行前，只能说"已编译"。

---

## 3. 阶段 ②：UIExtension

### 3.1 移植清单

源目录：`LyraStarterGame\Plugins\UIExtension\Source\`

| 源文件 | 行数 |
|---|---|
| `Public\UIExtensionSystem.h` | 216 |
| `Private\UIExtensionSystem.cpp` | 311 |
| `Public\Widgets\UIExtensionPointWidget.h` | 48 |
| `Private\Widgets\UIExtensionPointWidget.cpp` | 149 |
| `Private\UIExtensionModule.cpp` | 20 |
| `Private\LogUIExtension.h` / `.cpp` | 4 / 3 |

### 3.2 前置修复

删掉 `UIExtension.Build.cs` 里 `PublicDependencyModuleNames` 的 `"CommonGame"`（F6）。源码零引用，删掉即可独立编译。

### 3.3 关键学习点

| 主题 | 要点 |
|---|---|
| 扩展点匹配 | `EUIExtensionPointMatch::ExactMatch` / `PartialMatch`：用 Tag 层级决定"注册 A.B 能否收到 A.B.C" |
| Handle 语义 | `FUIExtensionHandle` / `FUIExtensionPointHandle` 是不透明句柄，**必须写 `TStructOpsTypeTraits`**（`WithCopy`、`WithIdenticalViaEquality`），否则蓝图变量会出问题 |
| GC | 子系统用 `TSharedPtr` 持有内部结构，必须实现 `AddReferencedObjects` 手工上报引用 |
| 订阅方 | `UUIExtensionPointWidget : UDynamicEntryBoxBase`，通过 `FOnGetWidgetClassForData` / `FOnConfigureWidgetForData` 两个可绑定事件把"数据 → 控件"的决定权交给蓝图 |
| 与项目的关系 | 项目侧 C++ 目前只用得到 `FUIExtensionHandle` + `RegisterExtensionAsWidgetForContext` + `Handle.Unregister()`（阶段③⑤会用到）；真正的价值在蓝图槽位机制 |

### 3.4 验收标准

| # | 检查 | 期望 |
|---|---|---|
| V2-1 | 独立构建（不引入 CommonGame） | 通过，证明 F6 的依赖是形式依赖 |
| V2-2 | 蓝图 | 新建一个 `UUIExtensionPointWidget` 子类，Details 面板出现 `ExtensionPointTag` / `ExtensionPointTagMatch` / `DataClasses` 三个属性 |
| V2-3 | 运行时 | 注册一个 Widget 扩展后，扩展点控件里出现该控件；注销后消失 |

---

## 4. 阶段 ③：CommonGame 内核

### 4.1 移植清单（有意裁剪）

源目录：`LyraStarterGame\Plugins\CommonGame\Source\`

| 源文件 | 行数 | 是否入本阶段 |
|---|---|---|
| `Public\PrimaryGameLayout.h` | 110 | ✅ 层栈核心 |
| `Private\PrimaryGameLayout.cpp` | 113 | ✅ |
| `Public\GameUIPolicy.h` | 77 | ✅ 每玩家根布局策略 |
| `Private\GameUIPolicy.cpp` | 176 | ✅ |
| `Public\GameUIManagerSubsystem.h` | 41 | ✅ 策略持有者 |
| `Private\GameUIManagerSubsystem.cpp` | 60 | ✅ |
| `Public\CommonUIExtensions.h` | 46 | ✅ 6 个静态函数 |
| `Private\CommonUIExtensions.cpp` | 145 | ✅ |
| `Public\CommonLocalPlayer.h` / `Private\CommonLocalPlayer.cpp` | 37 / 64 | ✅ 需与项目 `UHodgeLocalPlayerBase` 合并（阶段④） |
| `Public\CommonGameInstance.h` / `Private\CommonGameInstance.cpp` | 64 / 181 | ⚠️ 只取 `Init` / `AddLocalPlayer` / `RemoveLocalPlayer` / `ReturnToMainMenu`；**剔除 session / CommonUser 分支**（这决定能否绕开 F4 的 CommonUser 依赖） |
| `Public\Messaging\CommonMessagingSubsystem.h` / `.cpp` | 38 / 36 | ⚠️ 可延后 |
| `Public\Messaging\CommonGameDialog.h` / `.cpp` | 55 / 75 | ⚠️ 可延后（对应确认框） |
| `CommonPlayerController.h` / `.cpp` | 42 / 88 | ❌ 本阶段不做 |
| `CommonPlayerInputKey.h` / `.cpp` | 69 / 1117(生成) | ❌ 不做 |
| `AsyncAction_*.h/.cpp`（3 组） | — | ❌ 不做 |

**关键点**：`CommonGameInstance` 是 CommonUser 依赖的唯一入口（F4）。只要剔掉它的 session/用户分支，阶段③就能在不引入 CommonUser 的前提下成立。这一点必须在动手前用一次"只编译这三个单元"的构建验证，不能假设。

### 4.2 关键学习点

| 主题 | 要点 |
|---|---|
| 层栈 | `UPrimaryGameLayout` 用 `TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*>` 存层，`RegisterLayer` 由蓝图 `W_OverallUILayout` 连线注册 |
| 模板与护栏 | `PushWidgetToLayerStack` / `PushWidgetToLayerStackAsync` 是模板，**实现必须在头文件**（`PrimaryGameLayout.h:53-110`）；两处 `static_assert(TIsDerivedFrom<..., UCommonActivatableWidget>)`（`:62`、`:102`）是唯一类型护栏 |
| 异步 + 输入挂起 | `:59-91`：`RequestAsyncLoad` 前 `SuspendInputForPlayer`，加载完成/取消两条路径都要 `ResumeInputForPlayer`，**绑定 Cancel 委托是易漏点** |
| 每玩家根布局 | `UGameUIPolicy` 用 `Within = GameUIManagerSubsystem`，`GetOwningUIManager()` 是 `CastChecked<UGameUIManagerSubsystem>(GetOuter())` → `NewObject` 时必须传 Manager 作 Outer |
| 工具函数 | `UCommonUIExtensions` 的输入挂起用全局静态计数器 `InputSuspensions` + `FName::SetNumber` 生成唯一 token，**不能用 bool 标志位** |
| 配置入口 | `UGameUIManagerSubsystem` 的 `DefaultUIPolicyClass` 是 `config, EditAnywhere`（ini 绑定）；`UGameUIPolicy` 的 `LayoutClass` 在策略蓝图里配 |
| 已知设计瑕疵 | `LyraUIManagerSubsystem.cpp:23,33-68` 用 `FTSTicker` 每帧轮询 `bShowHUD`。可改为监听 HUD 变化，但**这属于上游设计改造，应与迁移拆成两次改动**（AGENTS.md：不顺手重构） |

### 4.3 验收标准

| # | 检查 | 期望 | 失败特征 |
|---|---|---|---|
| V3-1 | 构建 | 通过，且**未引入 CommonUser 模块** | 出现 `CommonUser` 相关符号 → 裁剪不彻底 |
| V3-2 | PIE 启动 | 每玩家生成根布局，不崩 | 崩在 `CastChecked<UCommonLocalPlayer>` → 阶段④未完成 |
| V3-3 | 根布局日志 | 出现"为玩家添加根布局"类日志 | 没出现 → Manager 未监听 LocalPlayer |
| V3-4 | Esc 菜单 | 推到 Menu 层 | 层 Tag 或 Async 推链断 |
| V3-5 | 输入挂起 | 异步推 UI 期间输入被临时屏蔽，推完恢复 | token 冲突 → 用了 bool 而非计数器 |

---

## 5. 阶段 ④：Hodge 侧换父类 + ini

### 5.1 改动清单

| 目标 | 现状（F 编号） | 目标形态 | 风险 |
|---|---|---|---|
| `UHodgeLocalPlayerBase` | `: public ULocalPlayer`（F11），已有 Lyra 同款三组委托 + `CallAndRegister_*`，`GetRootUILayout()` 注释 | `: public UCommonLocalPlayer`，实现 `GetRootUILayout()` | **反射布局变更**，必须关编辑器常规构建；委托成员可能与基类重复，需逐一比对 |
| `UHodgeGameInstanceBase` | `: public UGameInstance`，覆写 `Init` / `Shutdown`（F12） | `: public UCommonGameInstance` | 同上；`UCommonGameInstance` 已覆写 `Init` / `AddLocalPlayer` / `RemoveLocalPlayer` / `ReturnToMainMenu`，需确认调用 `Super` |
| `AHodgeHUD` | Lyra `ALyraHUD` 移植（`Public/Core/HUD/HodgeHUD.h:23`）：注册 GameFrameworkComponent 接收器 + GAS 调试 Actor 列表 | 本阶段不改，但它是 HUD 可见性链的一环 | — |
| `DefaultEngine.ini` | 无 `GameViewportClientClassName`（F13） | 若要 Lyra 的十字准星/光标链，需补该配置并实现 ViewportClient | 属于阶段③之外的独立项，**建议本阶段不做**，只登记 |

### 5.2 为什么这一步不能省

`UGameUIPolicy::GetRootLayout(const UCommonLocalPlayer*)` 与 `UGameUIManagerSubsystem::NotifyPlayerAdded(UCommonLocalPlayer*)` 都要求 `UCommonLocalPlayer`。项目 ini 当前把 `LocalPlayerClassName` 指向 `UHodgeLocalPlayerBase`（F13），所以**只有换父类这一条路**，否则 `CastChecked` 直接崩（对应 V3-2）。

### 5.3 验收标准

| # | 检查 | 期望 |
|---|---|---|
| V4-1 | Editor + Game 双构建 | 退出码 0 |
| V4-2 | PIE 里 `UHodgeLocalPlayerBase` 仍能拿到 `PlayerController` | 委托回调有值（这是**先于一切 UI 验证**的第 0 步） |
| V4-3 | `AddLocalPlayer` / `RemoveLocalPlayer` | 分屏或第二个本地玩家加入/退出时行为不回归 |
| V4-4 | 蓝图 | `BP_*` 角色/控制器蓝图 Compile 无父类相关错误 |

---

## 6. 顺序与依赖

```text
① CommonLoadingScreen ──────────────► 独立可完成，唯一的项目侧前置是补 C1
                                        （项目已实现 ILoadingProcessInterface，见 F8）
② UIExtension ──────────────────────► 先删 Build.cs 里的 CommonGame 依赖即可独立
③ CommonGame 内核 ──────────────────► 依赖 ②（AddWidget 链路）与 ④（LocalPlayer 类型）
④ Hodge 换父类 + ini ───────────────► 依赖 ③ 的类定义存在
⑤ AddWidget 注释版复活 ─────────────► 依赖 ② + ③ + ④，作为端到端验收物
```

硬性约束：

- ①不依赖任何 Lyra 插件，可以立刻开工。
- ③ **不能拆成"先做一半"**：`CommonUIExtensions::PushContentToLayer_ForPlayer` → `UGameUIManagerSubsystem::GetCurrentUIPolicy` → `UGameUIPolicy::GetRootLayout` → `UPrimaryGameLayout::PushWidgetToLayerStack` 是一条完整的调用链，缺一环就断。
- ④必须在③之后、⑤之前。

---

## 7. 通用纪律

1. **构建**：C++ 改动按 AI_DEVELOPMENT.md 执行 Editor 构建；涉及模块依赖、插件、反射布局（阶段③④）再补 Game 构建。Live Coding 成功不替代常规构建；换父类后必须关编辑器编译再打开。
2. **编辑器操作**：关闭编辑器前保存工作；不强制结束含未保存内容的编辑器。
3. **工作区**：开工与收工都检查 `git status --short`，保留用户已有改动；不回退、不覆盖、不自动提交。
4. **诚实标注**：每次交付分别说明 C++ 构建 / 蓝图编译 / PIE / 联机 / 打包 的通过与未执行项。"C++ 编译通过"不等于"蓝图可用"。
5. **不顺手重构**：第三方插件（ALS、RiderLink）与二进制资产不动；上游设计瑕疵（如 F18 的轮询）单独开改动。
6. **文档路径**：`Docs/AI_DEVELOPMENT.md` 里的引擎/项目路径是历史值（`D:\...`），与当前实测路径（第 0 节）不一致；本计划按实测路径执行，**不修改那份文档**（另开任务处理）。
7. **Asset 与配置**：新增反射类型后，蓝图父类下拉需要一次常规构建 + 重开编辑器才能看到；`defaultconfig` 段落名与模块名绑定（C3）。

---

## 8. 风险与坑

| # | 风险 | 触发点 | 应对 |
|---|---|---|---|
| R1 | 模板函数实现放 `.cpp` | 阶段③的 `PushWidgetToLayerStack*` | 实现保持在头文件；`static_assert` 一并保留 |
| R2 | `Within` + `GetOuter()` | 阶段③的 `UGameUIPolicy` | `NewObject<T>(Manager, Class)`；不可用 `CreateDefaultSubobject` 或 `GetTransientPackage()` |
| R3 | 与项目既有接口/类重名 | 阶段①的 `ILoadingProcessInterface`（C2）、阶段④的 LocalPlayer | 二选一，不并存 |
| R4 | 静态辅助函数只声明未实现 | 阶段①（C1，F9 已实测） | 先补实现再移植调用方 |
| R5 | `CommonUser` 被隐性牵入 | 阶段③的 `CommonGameInstance` | 剔除 session/用户分支；用 V3-1 验证 |
| R6 | 反射布局变更未走常规构建 | 阶段④ | 关编辑器编译；不依赖 Live Coding |
| R7 | 蓝图父类报"缺失父类" | 阶段②③的 Widget 与策略蓝图 | 每次改动后让编辑器重扫；D2 决定改名时同步写 CoreRedirects |
| R8 | 输入挂起 token 冲突 | 阶段③的多异步 UI 同时加载 | 全局静态计数器 + `FName::SetNumber`，禁止 bool 标志 |
| R9 | 加载屏"永远不消失"却查不到原因 | 阶段① | 判定链每步都写 `DebugReason`；打开 `LogLoadingScreenReasonEveryFrame` |
| R10 | 上游 `UIExtension.uplugin` 重复键（F7） | 自研插件描述时照抄 | 写自己的 `.uplugin` 时合并成一个 `Plugins` 数组 |

---

## 9. 未决问题

| # | 问题 | 需要谁定 |
|---|---|---|
| Q1 | D1 落点：独立插件 还是 并入 Hodgepodge 模块 | 用户 |
| Q2 | D2 前缀：保留 `Common*` 还是改 `Hodge*` | 用户 |
| Q3 | D3 阶段①的消费者范围：只接 ExperienceManager 还是含音频混音 | 用户 |
| Q4 | 是否顺带补 `GameViewportClientClassName`（F13） | 用户（建议延后） |
| Q5 | 是否把 `Docs/AI_DEVELOPMENT.md` 的路径更新为实测值 | 用户（本计划不擅自改） |

---

## 10. 待复活文件清单（2026-09-26 实测）

### 10.1 为什么有这份清单

`Source/Hodgepodge/Public/UI/**` + `Private/UI/**` 共 **79 个文件**是从 `LyraStarterGame\Source\LyraGame\UI\**` 按"改文件名 + 改类名 + CoreRedirect"整体搬入的。但 Lyra 的 UI 不是自包含的：它依赖 5 个**本项目未引入**的插件。为了让其余文件先编过，**30 个文件被整体注释**（`// ` 前缀 + 文件头 `[UI-MIGRATION-PENDING]` 标记块）。

**约定：复活时必须 `.h` 与 `.cpp` 成对恢复**。只恢复一半会导致 UCLASS 的构造函数/vtable 缺定义，报 `LNK2019`。

### 10.2 分组清单

**A 组 — 依赖 `CommonGame` 插件（12 文件）**

| 文件 | 缺什么 | 复活前置 |
|---|---|---|
| `Subsystem/HodgeUIManagerSubsystem.h/.cpp` | 父类 `UGameUIManagerSubsystem` | 移植 CommonGame：`GameUIManagerSubsystem` + `GameUIPolicy` |
| `Subsystem/HodgeUIMessaging.h/.cpp` | 父类 `UCommonMessagingSubsystem` | 移植 CommonGame：`Messaging/CommonMessagingSubsystem` |
| `Foundation/HodgeConfirmationScreen.h/.cpp` | 父类 `UCommonGameDialog` | 移植 CommonGame：`Messaging/CommonGameDialog` + `CommonGameDialogDescriptor` |
| `HodgeGameViewportClient.h/.cpp` | 父类 `UCommonGameViewportClient` | 移植 CommonGame 的 ViewportClient；并补 `GameViewportClientClassName` |
| `HodgeHUDLayout.h/.cpp` | `UCommonUIExtensions` 的 3 个静态函数 | 移植 CommonGame：`CommonUIExtensions`（`PushContentToLayer_ForPlayer` / `PushStreamedContentToLayer_ForPlayer` / `PopContentFromLayer`） |
| `Frontend/HodgeFrontendStateComponent.h/.cpp` | `CommonUser`（`UCommonUserSubsystem` / `ECommonUser*` / `FCommonUserTags`）+ `ControlFlows`（`FControlFlow`）+ `PrimaryGameLayout` / `UCommonLocalPlayer` | 依赖最多，**建议最后复活**；单机流程可考虑改写成不依赖 CommonUser 的版本 |

**B 组 — 依赖 `GameSettings` 插件（2 文件）**

| 文件 | 缺什么 | 复活前置 |
|---|---|---|
| `HodgeSettingScreen.h/.cpp` | 父类 `UGameSettingScreen`；`CreateRegistry()` 返回 `UGameSettingRegistry*` | 移植 GameSettings 插件（59 文件 / 4336 行）。**这是整个迁移里最重的依赖，与"学 UI 架构"无关** |

**C 组 — 依赖 `AsyncMixin` 插件（4 文件）→ ✅ 已于 2026-09-26 复活，不再注释**

| 文件 | 原缺什么 | 复活方式 |
|---|---|---|
| `IndicatorSystem/SActorCanvas.h/.cpp` | `#include "AsyncMixin.h"` + 基类 `FAsyncMixin` | 不引入 AsyncMixin 插件：去掉 `FAsyncMixin` 基类，异步加载改用 `FStreamableManager::RequestAsyncLoad`。详见 §11 |
| `IndicatorSystem/IndicatorLayer.h/.cpp` | 仅因 `IndicatorLayer.cpp:36` 用 `SNew(SActorCanvas, ...)` 被连带 | 随 `SActorCanvas` 一起复原，无独立依赖 |

**D 组 — 依赖未搬迁的玩法系统（8 文件）**

| 文件 | 缺什么 | 复活前置 |
|---|---|---|
| `Weapons/HodgeReticleWidgetBase.h/.cpp` | `Weapons/HodgeWeaponInstance.h`、`Weapons/HodgeRangedWeaponInstance.h`、`Inventory/HodgeInventoryItemInstance.h` | 先移植装备 / 武器 / 库存系统（项目当前无 `Equipment/`、`Weapons/`、`Inventory/` 目录） |
| `Weapons/HodgeWeaponUserInterface.h/.cpp` | `Equipment/HodgeEquipmentManagerComponent.h`、`Weapons/HodgeWeaponInstance.h` | 同上 |
| `Weapons/SHitMarkerConfirmationWidget.h/.cpp` | `Weapons/HodgeWeaponStateComponent.h` | 同上 |
| `Weapons/HitMarkerConfirmationWidget.h/.cpp` | 仅因 `HitMarkerConfirmationWidget.cpp:35` 用 `SNew(SHitMarkerConfirmationWidget, ...)` 被连带 | 随 `SHitMarkerConfirmationWidget` 一起复活 |
| `PerformanceStats/HodgePerfStatContainerBase.h/.cpp`、`HodgePerfStatWidgetBase.h/.cpp` | `Performance/HodgePerformanceStatTypes.h`（`EHodgeStatDisplayMode` / `EHodgeDisplayablePerformanceStat`）+ `UHodgePerformanceStatSubsystem` | 补 `Performance/` 目录（Lyra 侧为 `LyraPerformanceStatTypes.h` + `LyraPerformanceStatSubsystem`），**约 2~3 个文件，成本低** |

### 10.3 本轮已做的非注释改动

| 文件 | 改动 | 原因 |
|---|---|---|
| `Private/UI/Frontend/ApplyFrontendPerfSettingsAction.cpp:3` | `"Ui/Frontend/..."` → `"UI/Frontend/..."` | 大小写：Windows 能过，Linux/Android 打包会挂 |
| `Private/UI/Common/HodgeListView.cpp:3` | `"Ui/Common/..."` → `"UI/Common/..."` | 同上 |
| `Hodgepodge.Build.cs` | 私有依赖新增 `ApplicationCore` | `IPlatformInputDeviceMapper::Get()` 定义在 `ApplicationCore`（`GenericPlatform/GenericPlatformInputDeviceMapper.h`），`HodgeControllerDisconnectedScreen.cpp:53,86` 使用 |

### 10.4 复活时必须一并修的已知问题（当前埋在注释体内）

| 文件 | 问题 | 修法 |
|---|---|---|
| `Public/UI/HodgeHUDLayout.h`（原第 5 行） | `#include "HogdeActivatableWidget.h"` —— **文件名拼错** | 改为 `"UI/HodgeActivatableWidget.h"` |
| `Public/UI/Frontend/HodgeFrontendStateComponent.h`（原第 7 行） | `#include "LoadingProcessInterface.h"` —— 项目的在 `Interface/` 子目录，本模块 include 根是 `Public/` | 改为 `"Interface/LoadingProcessInterface.h"` |
| `Public/UI/PerformanceStats/HodgePerfStatContainerBase.h`（原第 6 行） | `#include "Performance/HodgePerformanceStatTypes.h"` —— 该文件**从未被搬入** | 补 `Performance/` 目录后再复活 |

### 10.5 本轮构建验证结果

| 项 | 命令 | 结果 |
|---|---|---|
| Editor 构建 | `Build.bat HodgepodgeEditor Win64 Development -Project=...\Hodgepodge.uproject -WaitMutex -architecture=x64` | **EXIT=0**（UHT + 编译 + 链接全部通过） |
| Game 构建 | `Build.bat Hodgepodge Win64 Development -Project=...\Hodgepodge.uproject -WaitMutex -architecture=x64` | **EXIT=0** |
| C 组复活后（Editor） | 同上 Editor 命令 | **EXIT=0** |
| C 组复活后（Game） | 同上 Game 命令 | **EXIT=0** |

复活 C 组 4 个文件后：**已注释 26 个 / 在编 53 个**（总计 79）。

**未验证 / 不得声称**：未打开编辑器、未做蓝图编译、未 PIE。30 个已注释文件里的蓝图资产（如 `W_OverallUILayout`、`B_LyraUIPolicy`）现在会报"缺失父类"——这是预期的，因为类已不在构建内。**"C++ 编译通过"不等于"蓝图或 UI 可用"**。

---

## 11. SActorCanvas 复活记录（2026-09-26）

### 11.1 为什么它能第一个复活

它是 30 个待复活文件里**唯一不需要引入新插件**的：阻塞点只有 `FAsyncMixin` 一个基类，而它的异步加载语义（"加载一个 `TSoftClassPtr<UUserWidget>`，完成后创建控件并挂到 Canvas"）用引擎自带的 `FStreamableManager` 就能等价表达。`IndicatorLayer` 只是被它连带，随之一起复原。

### 11.2 相对上游的改动

| 位置 | 上游（Lyra） | 本项目 |
|---|---|---|
| `SActorCanvas.h` include | `#include "AsyncMixin.h"` | **删除**，改为前向声明 `struct FStreamableHandle;`（注意是 `struct`，写成 `class` 会触发 C4099） |
| 类声明 | `class SActorCanvas : public SPanel, public FAsyncMixin, public FGCObject` | `public SPanel, public FGCObject` |
| 异步加载 | `AsyncLoad(Class, Lambda)` + `StartAsyncLoading()`（FAsyncMixin 的排队语义） | `FStreamableManager::RequestAsyncLoad(SoftObjectPath, Delegate)`，逐次直发、无需 `StartAsyncLoading` |
| 回调绑定 | `CreateWeakLambda(this, ...)` | `CreateSP(StaticCastSharedRef<SActorCanvas>(AsShared()), &SActorCanvas::OnIndicatorClassLoaded, IndicatorPtr)`。**Slate 控件不是 UObject，`CreateWeakLambda` 会触发 `TStrongObjectPtr can only be constructed with UObject types` 断言**；`CreateSP` 用共享引用保活，语义等价 |
| 回调体 | 内联在 `AddIndicatorForEntry` 的 lambda 里 | 拆成成员函数 `OnIndicatorClassLoaded(TWeakObjectPtr<UIndicatorDescriptor>)`，控件类在回调内用 `LoadedIndicator->GetIndicatorClass()` 重新取 |
| 句柄生命周期 | FAsyncMixin 内部管理 | 新增成员 `TArray<TSharedPtr<FStreamableHandle>> IndicatorLoadHandles`，析构函数里统一 `CancelHandle()` |

### 11.3 保留的上游行为

- `AllIndicators.Contains(LoadedIndicator)` 的"加载期间已被移除"守卫**原样保留**——这是异步加载最容易漏的竞态检查。
- 控件池复用（`IndicatorPool.GetOrCreateInstance` + `Release`）逻辑不变。
- `IIndicatorWidgetInterface::Execute_BindIndicator` 的绑定时机不变（仍是控件创建后立刻绑定）。
- 箭头绘制、视锥裁剪、`AddReferencedObjects` 手动 GC 等 Slate 绘制逻辑**一行未动**。

### 11.4 验证结果与边界

| 项 | 结果 |
|---|---|
| Editor 构建 | **EXIT=0** |
| Game 构建 | **EXIT=0** |
| 注释文件计数 | 30 → **26**（在编 53） |

**未验证**：未打开编辑器、未 PIE、未做蓝图编译。因此 **"指示器实际能在屏幕上画出来"没有任何运行证据**——本次只证明"能编过、链接过"。若要做运行验证，需要一个 `UIndicatorLayer` 的子 Widget 蓝图 + 一个通过 `UHodgeIndicatorManagerComponent::AddIndicator` 注册的 `UIndicatorDescriptor`，并在 PIE 里观察箭头/图标是否跟随目标。

---

## 12. 只引入 UIExtension 的落地方案（不搬 CommonGame）

> 决策：**走 Lyra 的 HUD 挂载方式（UIExtension 扩展点），但不引入 CommonGame 插件。**
>
> **落地进度（2026-09-27）**：**Step 1 已完成** —— 4 个文件并入 `Hodgepodge` 模块，Editor / Game 双构建 **EXIT=0**（详见 [UIExtension 子系统说明](ui-extension-system.md) §9）；**Step 2 / 3 / 4 未开始**。
> 已记录 1 条**对上游的有意偏离**（注册日志极性修正），见 [ui-extension-system.md §10.1](ui-extension-system.md)。

### 12.1 对 §0.2 的一处更正

§0.2 与 §3.2 曾写"UIExtension 源码 0 处引用 CommonGame"——**这个说法不准确**。当时只扫了 `UIExtensionSystem.h/.cpp`。完整核实结果：

| 文件 | 行数 | CommonGame 依赖 |
|---|---|---|
| `Source/Public/UIExtensionSystem.h` + `Source/Private/UIExtensionSystem.cpp` | 216 + 311 | ✅ **零依赖**（只用 GameplayTags / Slate / UMG） |
| `Source/Public/Widgets/UIExtensionPointWidget.h` | 48 | ✅ 头文件不含 |
| `Source/Private/Widgets/UIExtensionPointWidget.cpp` | 149 | ⚠️ **有**：`:8` `#include "CommonLocalPlayer.h"`、`:38` `GetOwningLocalPlayer<UCommonLocalPlayer>()`、`:102` `RegisterExtensionPointForPlayerState(UCommonLocalPlayer*)` |
| `Source/Private/UIExtensionModule.cpp`、`LogUIExtension.h/.cpp` | 20 + 4 + 3 | ✅ 零依赖 |
| `UIExtension.Build.cs` | — | ⚠️ 声明 `"CommonGame"`（唯一原因就是上面那个文件） |

**修正后的结论**：扩展点内核（Subsystem / ExtensionPoint / Extension / Handle）完全独立；**只有现成的槽位控件 `UUIExtensionPointWidget` 粘了 `UCommonLocalPlayer`**。

### 12.2 唯一的障碍已经提前解决

`UCommonLocalPlayer` 在那 3 处只做一件事：**等 LocalPlayer 拿到 PlayerState，再注册一个带 Context 的扩展点**。项目里的 `UHodgeLocalPlayerBase` 就是同一个东西：

| 上游 | 项目 | 位置 |
|---|---|---|
| `UCommonLocalPlayer::CallAndRegister_OnPlayerStateSet` | `UHodgeLocalPlayerBase::CallAndRegister_OnPlayerStateSet` | `HodgeLocalPlayerBase.h:98` |
| `FPlayerStateSetDelegate`（`LocalPlayer*, APlayerState*`） | 同名同签名 | `HodgeLocalPlayerBase.h:63` |

`GetOwningLocalPlayer<T>()` 是 `UWidget` 的模板方法（`Engine/Source/Runtime/UMG/Public/Components/Widget.h:887`），换模板参数即可。

**改造量（2026-09-27 逐文件点数后的准确清单）**：

| 文件 | 改动 | 处数 |
|---|---|---|
| `Public/UIExtensionSystem.h` | `UIEXTENSION_API` → `HODGEPODGE_API` | 5 |
| `Public/Widgets/UIExtensionPointWidget.h` | `UIEXTENSION_API` → `HODGEPODGE_API` | 1 |
| `Public/Widgets/UIExtensionPointWidget.h` | `class UCommonLocalPlayer;` 前向声明 + `RegisterExtensionPointForPlayerState(UCommonLocalPlayer*, ...)` 签名 → `UHodgeLocalPlayerBase` | 2 |
| `Private/Widgets/UIExtensionPointWidget.cpp` | `#include "CommonLocalPlayer.h"` → `"Core/LocalPlayer/HodgeLocalPlayerBase.h"` | 1 |
| `Private/Widgets/UIExtensionPointWidget.cpp` | `GetOwningLocalPlayer<UCommonLocalPlayer>()` → `<UHodgeLocalPlayerBase>()`（`Widget.h:887` 的模板方法） | 1 |
| `Private/Widgets/UIExtensionPointWidget.cpp` | `RegisterExtensionPointForPlayerState(UCommonLocalPlayer* ...)` 定义 → `UHodgeLocalPlayerBase*` | 1 |
| 两个 `.cpp` | include 路径 → `UI/Extension/...` | 2 |
| `Private/UIExtensionSystem.cpp` | `LogUIExtension` 的处理（见下） | 14 |

**`LogUIExtension` 二选一**：
- ① **推荐**：在 `UIExtensionSystem.cpp` 内就地加 `DECLARE_LOG_CATEGORY_EXTERN` + `DEFINE_LOG_CATEGORY`（2 行），14 处调用不动 —— **保留 `log LogUIExtension Verbose` 这个排查手段**（扩展点匹配日志是 Verbose 级，"注册了但不显示"只能靠它查）。
- ② 全部改成 `LogTemp`，删掉 `LogUIExtension.h/.cpp` 与对应 include —— 符合项目现状（全项目 `DECLARE_LOG_CATEGORY` 为 0 处），但失去过滤能力。

### 12.3 失去什么 / 用什么替代

| CommonGame 能力 | 失去后 | 替代 |
|---|---|---|
| `UPrimaryGameLayout` 层栈（Game / Menu / Modal） | 没有"推到某层"的概念 | 第一步不需要。将来要分层：引擎自带 `UCommonActivatableWidgetStack`（`Engine/Plugins/Runtime/CommonUI/Source/CommonUI/Public/Widgets/CommonActivatableWidgetContainer.h`）+ 自己写 Tag→Stack 映射（约 30 行，等价于 `PrimaryGameLayout`） |
| `UGameUIPolicy` / `UGameUIManagerSubsystem` | HUD 根控件不再随玩家自动创建/销毁 | `AHodgeHUD::BeginPlay` 里 `CreateWidget + AddToViewport`（单机/单玩家足够；分屏要自己遍历 LocalPlayer） |
| `UCommonUIExtensions::Push/Pop/SuspendInput` | `HodgeHUDLayout` 里 2 处调用无实现 | 自己写等价函数，或局部用 `AddToViewport` |
| `CommonMessagingSubsystem` / `CommonGameDialog` | 没有统一确认框 | UMG 自己搭 |
| `UCommonGameInstance` / `UCommonGameViewportClient` | 无 | 不需要 |
| `UCommonLocalPlayer::GetRootUILayout()` | 无 | 不需要（HUD 由 HUD Actor 持有） |

**保留**：Tag 挂载、`FUIExtensionHandle` 生命周期、手动 GC 引用（`AddReferencedObjects`）、槽位控件、`DataClasses` 契约——即 UIExtension 的全部架构价值。

### 12.4 落地清单

**Step 1｜引入 UIExtension（7 文件 / 751 行）**

| 源文件（Lyra） | 行数 | 目标 |
|---|---|---|
| `Source/Public/UIExtensionSystem.h` | 216 | `Public/UI/Extension/UIExtensionSystem.h` |
| `Source/Private/UIExtensionSystem.cpp` | 311 | `Private/UI/Extension/UIExtensionSystem.cpp` |
| `Source/Public/Widgets/UIExtensionPointWidget.h` | 48 | `Public/UI/Extension/Widgets/UIExtensionPointWidget.h` |
| `Source/Private/Widgets/UIExtensionPointWidget.cpp` | 149 | `Private/UI/Extension/Widgets/UIExtensionPointWidget.cpp` |
| `Source/Private/UIExtensionModule.cpp` | 20 | **不要**（并入模块后无意义） |
| `Source/Private/LogUIExtension.h/.cpp` | 4 + 3 | **不要**，日志统一用 `LogTemp`（全项目 `DECLARE_LOG_CATEGORY` 为 0 处，尚无日志类别体系） |

- 建议**并入 `Hodgepodge` 模块**（不建插件、不建模块）：`CommonUI` / `CommonInput` / `UMG` / `Slate` / `SlateCore` / `GameplayTags` 已全在 `Hodgepodge.Build.cs` 中 → **零依赖改动**。
- 类名保留 `UUIExtensionSubsystem` / `FUIExtensionHandle` / `UUIExtensionPointWidget` 原名（不改成 Hodge 前缀），以便与上游文档对照。
- 若坚持做插件：需删掉 `UIExtension.Build.cs` 里的 `"CommonGame"`，并注意上游 `.uplugin` 的 `"Plugins"` 键重复写了两次（后者覆盖前者）。

**Step 2｜HUD 根控件 + 槽位**

- 复活 `HodgeHUDLayout.h/.cpp`（84 + 192 行，原 §10.2 A 组）。
- 复活时必须同时处理：`#include "HodgeLogChannels.h"` 与 `LogHodge` **都不存在**（`LogHodge` 只出现在注释里，项目无日志类别）→ 换 `LogTemp`；2 处 `UCommonUIExtensions::Push*` → 自己实现（见 12.3）。
- `AHodgeHUD::BeginPlay` 创建 HUD Layout 并 `AddToViewport`。
- HUD Layout 蓝图里放 `UUIExtensionPointWidget`，填 `ExtensionPointTag`。

**Step 3｜复活 `GameFeatureAction_AddWidget`（161 行，原 §10.2 A 组）**

- **保留 `Widgets[]` 分支**：`RegisterExtensionAsWidgetForContext(Entry.SlotID, LocalPlayer, Entry.WidgetClass.Get(), -1)` → 纯 UIExtension。
- **删除 `Layout[]` 分支**（`FHodgeHUDLayoutRequest`、`LayoutsAdded`、`UCommonUIExtensions::PushContentToLayer_ForPlayer`、`DeactivateWidget`）→ 这是全文件**唯一**需要 CommonGame 的地方（上游 `AddWidget.cpp:151-157`、`:176-182`）。

**Step 4｜资产**

- HUD Layout 蓝图（父类 `UHodgeHUDLayout`）→ 血条 Widget 蓝图 → Experience / GameFeature 里配 `AddWidget` 的 `Widgets[]`（`SlotID` + `WidgetClass`）。

### 12.5 扩展点匹配契约（不搞清会"静默不显示"）

`UIExtensionSystem.cpp:35-59` 的 `DoesExtensionPassContract` 要求**两个条件同时成立**：

1. **数据类匹配**：`RegisterExtensionAsWidget` 把 Data 设成**控件的 `UClass`**（`:138-141`），判定的是"该蓝图类 → `IsChildOf(AllowedDataClasses)` 中之一"。`UUIExtensionPointWidget::RegisterExtensionPoint` 会自动加入 `UUserWidget`（`:87`）→ 血条蓝图继承 `UUserWidget` 即可通过；但若在槽位 `DataClasses` 里填了具体类，则必须把血条类也加进去。
2. **Context 匹配**（`:39-41`）：`AddWidget` 传入的 `LocalPlayer` 必须等于槽位注册时的 Context。槽位会注册两次（`UIExtensionPointWidget.cpp:90` 无 Context、`:95` 带 `GetOwningLocalPlayer()`）。

排查手段：`UIExtension` 日志为 Verbose 级别；并入模块后用 `LogTemp` 过滤，或临时建一个 `LogUIExtension` 类别。

### 12.6 与原计划的关系

| 原计划 | 本方案 |
|---|---|
| §4 阶段③：移植 CommonGame 内核（16 文件 / 1318 行） | **不做** |
| §5 阶段④：Hodge 类换父类（`UCommonLocalPlayer` / `UCommonGameInstance`） | **不做**（改用 `CreateWidget` 建 HUD 根） |
| §4 的 `UPrimaryGameLayout` 层栈 | 降级为可选，需要时用引擎 `UCommonActivatableWidgetStack` 自建 |
| §2 阶段① CommonLoadingScreen | 不受影响，仍可独立推进 |
| §3 阶段② UIExtension | **升为本方案的核心（Step 1）** |

---

## 13. UI 骨架三块 todo（架构学习主线）

> 基线实测日期：2026-09-27。
> 与 §2~§5 的关系：§2~§5 是最初的阶段划分；§12 之后实际采用的路线是「只引入 UIExtension，其余用引擎 CommonUI + C++ 兜底自建」。
> **本节是当前有效**的推进清单，三块都不需要 CommonUser / GameSettings。
> 架构全景（安装链 / 卸载链 / 三张连接契约 / 已核实事实）见 [Hodge UI 模块化注入链：架构总览与逐条核实](hodge-ui-architecture.md)。

### 13.1 完成度实测（2026-09-27）

**① 源码文件维度：基本搬完**

| 项 | Lyra | 项目现状 | 说明 |
|---|---|---|---|
| `Source/LyraGame/UI/**` | 79 文件 / 4924 行 | **81 文件 / 7968 行** | 79 个文件 **100% 覆盖**（`LyraHUD.h/.cpp` 在 Lyra 侧位于 `Source/LyraGame/` 根目录，不属于 UI 子目录，故不计入）；额外多 4 个来自 UIExtension 插件：`UI/Extension/UIExtensionSystem.h/.cpp`、`UIExtensionPointWidget.h/.cpp` |
| 在编 / 仍注释 | — | **57 在编（6486 行）/ 24 注释（1482 行）** | 分组见 §10.2；复活对应关系见 §13.6 |
| `Content/UI/**` 蓝图 | **606 个 `.uasset`**（Credits / Foundation / FrontEnd / Hud / Indicators / Menu / PerfStats / Settings） | **0** | 项目 `Content` 下仅 5 个 `WBP_*`，全在 `CodexText`，与 Lyra 无关 |

**② 框架插件维度：几乎为零**

| 插件 | Lyra 规模 | 项目现状 | 完成度 |
|---|---|---|---|
| CommonUI（引擎自带） | — | 已启用 | 100% |
| UIExtension | 751 行 / 7 文件 | 4 个源文件搬入 `UI/Extension/` 且在编 | 源码 100%（无插件本体；无蓝图使用槽位） |
| **CommonGame** | **2389 行 / 29 文件** | 仅 `UHodgeHUDLayout::MenuLayerStack` 单层（≈100 行） | **~5%** |
| **CommonLoadingScreen** | **891 行 / 13 文件** | `LoadingProcessInterface.h`(30 行) + `HodgeLoadingScreenSubsystem`(19 行)；接口静态函数**未实现**（`.cpp` 仅 4 非空行） | **~10%** |
| CommonUser | 5029 行 | 0 | 0% |
| GameSubtitles | 541 行 | 0 | 0% |
| GameSettings | 4336 行 | 0 | 0% |
| `Plugins/` 下 5 个 Lyra UI 插件 | — | **全部缺失**（仅有 ALS-Refactored / Developer / McpAutomationBridge / UnrealMCP） | 0/5 |

**③ 为什么"搬了文件"≠"迁移了系统"**

`LyraGame/UI` 中的类多为插件基类的派生类，派生类全部搬入而基类所在插件全部未引入，因此只能整体注释或 C++ 兜底重写：

| 项目文件 | 父类 | 父类所在 |
|---|---|---|
| `HodgeUIManagerSubsystem` | `UGameUIManagerSubsystem` | CommonGame |
| `HodgeUIMessaging` | `UCommonMessagingSubsystem` | CommonGame |
| `HodgeConfirmationScreen` | `UCommonGameDialog` | CommonGame |
| `HodgeGameViewportClient` | `UCommonGameViewportClient` | CommonGame |
| `HodgeSettingScreen` | `UGameSettingScreen` | GameSettings |
| `HodgeFrontendStateComponent` | — | CommonUser + ControlFlows |

**④ 当前已通的链**

- 控件层：`HodgeActivatableWidget` / `HodgeTaggedWidget` / `HodgeButtonBase` / 列表 / 标签页 / 派生输入控件（Joystick、SimulatedInput、TouchRegion）/ `MaterialProgressBar`
- 指示器链：`IndicatorSystem` 全套（`SActorCanvas` 见 §11）
- 扩展点链：`UIExtensionSystem` + `UIExtensionPointWidget` 源码在编，但**暂无蓝图使用槽位**
- HUD 宿主：`HodgeGameModeBase.cpp:43` 已设 `HUDClass = AHodgeHUD`；但 `AHodgeHUD` 自身**不创建任何控件**
- 菜单层：`UHodgeHUDLayout` 已复活（Push/Pop 走引擎 `UCommonActivatableWidgetStack`：`AddWidget` / `RemoveWidget`），仅持有单个 `MenuLayerStack`；`GameFeatureAction_AddWidget.cpp` 仍为 0 有效行 → **HUDLayout 目前无人创建**

**⑤ 仍然断着的链**

| 链 | 断点 | 证据 |
|---|---|---|
| 层 / 每玩家根布局 | 无 `UPrimaryGameLayout` / `GameUIPolicy`，仅 1 个 Menu 层 | `UI/Subsystem/*` 两文件全注释 |
| 弹窗 / 消息 | 无 `UCommonMessagingSubsystem` / `UCommonGameDialog` | `HodgeUIMessaging`、`HodgeConfirmationScreen` 全注释 |
| 加载屏 | 无 `ULoadingScreenManager`；`LoadingProcessInterface.cpp` 仅 4 非空行，静态函数 `ShouldShowLoadingScreen(UObject*, FString&)` 声明未实现 | 实测 |
| HUD 注入 | `GameFeatureAction_AddWidget.cpp` 0 有效代码行 | 实测 |
| 输入路由 | `Config/*.ini` 中 `CommonUI` 匹配 **0 处** → `UI.Action.Escape` 无按键映射，`RegisterUIActionBinding` 注册后不触发 | 实测 |
| 用户 / 前端 / 设置 | `Public/UI/Settings`、`Public/Settings`、`Public/Performance`、`Public/Weapons`、`Public/Equipment`、`Public/Inventory` 目录均不存在 | 实测 |

---

### 13.2 todo ①：层栈 + 每玩家根布局（`PrimaryGameLayout` + `GameUIPolicy`）

**目标**：把 `HUDLayout` 里的单个 `MenuLayerStack` 扩展成 Lyra 的多层模型，并引入"每玩家根布局"的归属关系。

**参考实现**

```text
Plugins\CommonGame\Source\Public\PrimaryGameLayout.h        :35-137（:35-36 UCLASS(Abstract) ; :53-110 模板 ; :62,:102 static_assert ; :93-97 无 InitInstanceFunc 重载）
Plugins\CommonGame\Source\Public\GameUIManagerSubsystem.h   :22-50
Plugins\CommonGame\Source\Private\CommonUIExtensions.cpp    :55-74 Push ; :76-94 PushStreamed ; :96-117 Pop ; :129-150 Suspend ; :157+ Resume
Plugins\CommonGame\Source\Public\CommonUIExtensions.h       :21-22（COMMONGAME_API 函数库）, :39-58 函数声明
引擎：Plugins\Runtime\CommonUI\Source\CommonUI\Public\Widgets\CommonActivatableWidgetContainer.h
    :31,:47 AddWidget（两个重载）; :68 RemoveWidget ; :73 GetWidgetList ; :75 GetNumWidgets ; :78 ClearWidgets ; :187-188 UCommonActivatableWidgetStack
引擎：Plugins\Runtime\CommonUI\Source\CommonUI\Public\CommonActivatableWidget.h  :38 IsActivated ; :41 ActivateWidget ; :44 DeactivateWidget
引擎：Plugins\Runtime\CommonUI\Source\CommonUI\Private\Input\CommonUIActionRouterBase.cpp
    :174-212 RegisterUIActionBinding（无节点则延迟一帧）; :263 OnRebuilding 订阅 ; :984 root 创建 ; :1678-1684 HandleActivatableWidgetRebuilding
```

**要做的事**

1. 新类 `UHodgePrimaryGameLayout`（可基于 `UCommonUserWidget` 或 `UCommonActivatableWidget`）：
   - `TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers`
   - `UFUNCTION(BlueprintCallable) void RegisterLayer(UPARAM(meta=(Categories="UI.Layer")) FGameplayTag, UCommonActivatableWidgetContainerBase*)`（对应 WBP 里连线）
   - `template <typename T = UCommonActivatableWidget> T* PushWidgetToLayerStack(FGameplayTag, UClass*, TFunctionRef<void(T&)>)` —— **模板定义必须写在头文件**
   - `void FindAndRemoveWidgetFromLayer(UCommonActivatableWidget*)` —— 建议按 `GetWidgetList()` 判断归属，而不是照抄 Lyra 的盲遍历（理由见 §12.3）
   - `UCommonActivatableWidgetContainerBase* GetLayerWidget(FGameplayTag) const`
2. 层 Tag 约定：`UI.Layer.Game` / `UI.Layer.GameMenu` / `UI.Layer.Menu` / `UI.Layer.Modal`，补进 `HodgeGameplayTags`（当前只有 `UI_Action_Escape` / `UI_Action_Back`；`meta=(Categories="UI.Layer")` 需要 Tag 已注册，否则蓝图里选不到）
3. 根布局归属（二选一）：
   - 最小：在 `UHodgeLocalPlayerBase` 上挂 `GetRootUILayout()`（`HodgeLocalPlayerBase.h:143-144` 已有预留注释）
   - 贴近 Lyra：补 `UGameUIPolicy`（`Within = GameUIManagerSubsystem`）
4. `UHodgeUIExtensions`（函数库）：`PushContentToLayer_ForPlayer` / `PushStreamedContentToLayer_ForPlayer`（异步）/ `PopContentFromLayer` / `SuspendInputForPlayer` / `ResumeInputForPlayer`
   —— 后两组**只依赖引擎 `UCommonInputSubsystem`**（`CommonUIExtensions.cpp:129-150` 实测），可以先行落地
5. 把 `UHodgeHUDLayout` 里 C++ 兜底的 `EnsureMenuLayerStack()` 替换为向根布局注册/取层

**前置条件**：HUDLayout 需要一个实例化入口（`GameFeatureAction_AddWidget` 仍是 0 有效行）。

**学习点**：模板必须定义在头文件（否则 `unresolved external`）；`static_assert(TIsDerivedFrom<T, UCommonActivatableWidget>::IsDerived, ...)` 是唯一护栏；若引入 Policy，`GetOuter()` 与 `Within` 强绑定（`NewObject<UGameUIPolicy>(Manager, Class)`，传错 Outer 会崩）；输入挂起 token 必须全局唯一（`SuspendToken.SetNumber(++InputSuspensions)`）。

**验收**：Esc 菜单 Push 到 `UI.Layer.Menu` 后能被 `PopContentFromLayer` 移除；日志能打印层 Tag 与实际控件名；`AddWidget` 的模板重载对非 `UCommonActivatableWidget` 子类在编译期被 `static_assert` 拦住。

---

### 13.3 todo ②：加载屏（`LoadingScreenManager`）

**目标**：多来源加载状态汇聚 + 显示期间吞输入 + 最短显示时长。

**参考实现**

```text
Plugins\CommonLoadingScreen\Source\CommonLoadingScreen\Private\LoadingScreenManager.cpp
    :46-63   静态辅助函数实现（C1 的参照）
    :201-221 PreLoadMap / PostLoadMap
    :232,246 FThreadHeartBeat 检查点
    :255-405 判定链全量
    :407-465 最短显示时长 / 额外保持
    :467-484 首屏守卫（PreLoadScreen）
    :473-533 显示加载屏 + 输入吞噬 + Slate Tick
Plugins\CommonLoadingScreen\Source\CommonLoadingScreen\Public\LoadingScreenManager.h        :55-62 对外契约
Plugins\CommonLoadingScreen\Source\CommonLoadingScreen\Private\CommonLoadingScreenSettings.h :15-65 配置项
```

**硬前置（必须先做）**：`Source\Hodgepodge\Private\Interface\LoadingProcessInterface.cpp` 目前只有 4 非空行，静态函数 `ShouldShowLoadingScreen(UObject*, FString&)` 声明了但**没有实现**，而判定链有 5 处调用它。

**要做的事**

1. 补 `ILoadingProcessInterface::ShouldShowLoadingScreen(UObject*, FString&)` 的静态实现（遍历 `ILoadingProcessInterface` 实现者）
2. 新类 `UHodgeLoadingScreenManager : UGameInstanceSubsystem, FTickableGameObject`，对外契约对齐 `LoadingScreenManager.h:55-62`
3. 新类 `UHodgeLoadingScreenSettings`（`config`，含 `LoadingScreenWidget` 软类、最短显示时长、编辑器强制 Tick）
4. 显示期间用 `IInputProcessor` 吃掉全部输入
5. 与已有 `HodgeLoadingScreenSubsystem`（`UI/Foundation/`，19 行）合并或明确分工

**学习点**：判定链的发现策略 —— Lyra 是**主动遍历** GameState / 组件 / PlayerController 找处理器（`LoadingScreenManager.cpp:326-349`），`RegisterLoadingProcessor` 只是补充通道，并非唯一入口；最短显示时长用于防止一次快速加载导致界面闪烁。

**验收**：加载过程中显示加载屏、完成后隐藏；快速加载时不闪烁；加载屏显示期间键鼠输入被吞掉（角色不动）。

---

### 13.4 todo ③：消息 / 对话框（`CommonMessagingSubsystem` + `CommonGameDialog` + AsyncAction）

**目标**：对话框分发 + 蓝图异步节点。

**参考实现**：`Plugins\CommonGame\Source\Public\Messaging\*`（本轮未逐行核实，动工前先补精确行号）

**要做的事**

1. `UHodgeMessagingSubsystem : ULocalPlayerSubsystem`：`ShowConfirmation(UHodgeGameDialogDescriptor*, FCommonMessagingResultDelegate = {})` / `ShowError(...)`
   —— 用普通 `virtual` 代替 Lyra 的精确 override 签名（因为没有 CommonGame 基类）
2. `UHodgeGameDialogDescriptor`：`CreateConfirmationOk / OkCancel / YesNo / YesNoCancel` + `Header` / `Body` / `ButtonActions`
3. `EHodgeMessagingResult` + `FHodgeConfirmationDialogAction`（结构体需实现 `operator==`）
4. `UHodgeGameDialog : UCommonActivatableWidget`：`SetupDialog` / `KillDialog`，并 Push 到 `UI.Layer.Modal`
5. 可选：`UAsyncAction_ShowConfirmation`（`UBlueprintAsyncActionBase` + 多播委托 `OnResult`）
6. 复活 `Subsystem/HodgeUIMessaging.*` 与 `Foundation/HodgeConfirmationScreen.*`（当前 4 个文件全注释）

**前置条件**：需要 `UI.Layer.Modal` 层 → **依赖 todo ①**。

**验收**：调用 `ShowConfirmation` 能弹到 Modal 层；选择结果通过回调返回；层栈中该控件能被正确 Pop。

---

### 13.5 明确不做（本主线范围外）

| 项 | 行数 | 原因 |
|---|---|---|
| CommonUser | 5029 | 在线账号 / 平台会话耦合，与 UI 分层无关 |
| GameSettings | 4336 | 设置框架，牵出 CommonUser → 在线子系统 → 输入重映射 |
| `LyraGame/Settings/**` | 5730 | 同上 |
| `Content/UI/**` 606 个蓝图 | — | 全量搬运无学习价值；按需自建最小几张（HUD Layout / Menu / Confirmation）即可 |

---

### 13.6 与 §10 待复活清单的对应关系

当前仍注释 **24 个文件 / 1482 行**，按"做完哪一块能复活"归类：

| 前置 | 可复活文件 | 文件数 |
|---|---|---|
| todo ③（消息链）+ todo ① 的 Modal 层 | `Subsystem/HodgeUIMessaging.h/.cpp`、`Foundation/HodgeConfirmationScreen.h/.cpp` | 4 |
| todo ①（若采用 Policy 方案） | `Subsystem/HodgeUIManagerSubsystem.h/.cpp` | 2 |
| 独立小改（与三块无关） | `HodgeGameViewportClient.h/.cpp` —— 父类由 `UCommonGameViewportClient` 降为 `UGameViewportClient` 即可 | 2 |
| todo ① 完成后回改 | `HodgeHUDLayout.h/.cpp`（已复活，可去掉 C++ 兜底改回按层 Push） | 已复活 |
| 需额外前置（不在三块内） | `HodgeSettingScreen.h/.cpp`（GameSettings）、`Frontend/HodgeFrontendStateComponent.h/.cpp`（CommonUser + ControlFlows） | 4 |
| 需先补上游系统 | `PerformanceStats/*`（4，缺 `Performance/` 目录与性能统计子系统）、`Weapons/*`（8，缺武器 / 装备 / 库存系统） | 12 |

> 注：`Weapons/*` 的 8 个文件里，`HitMarkerConfirmationWidget.*` 只是因为 `SNew(SHitMarkerConfirmationWidget, ...)` 被连带，可与 `SHitMarkerConfirmationWidget.*` 一起处理。

---

### 13.7 建议顺序与纪律

```text
② 加载屏（891 行，零 Lyra 依赖，接口钩子已存在）
   ↓
① 层栈 + 每玩家根布局（~1200 行，是 ③ 的前置）
   ↓
③ 消息 / 对话框（~400 行）
```

纪律：

1. 每块落地后同时跑 Editor 与 Game 双构建，记录 EXIT 状态。
2. 构建期间不要让 IDE 保存被编译的文件 —— 曾出现 UHT 与编译器读到两个版本、报出 `缺少";"(在"<class-head>"的前面)` 之类假故障（详见 §13.1 对应的变更记录）。
3. 蓝图资产（`WBP_*`）由编辑器侧自建，代码侧只提供 `TSubclassOf` / `TSoftClassPtr` 配置位与 `meta=(BindWidgetOptional)`。
4. 未在 PIE 中验证过的运行时路径，一律标注"未验证"，不得写成"已完成"。

---

## 附录 A：证据索引

**Lyra 参考实现（绝对路径，行号已核实）**

```text
Plugins\CommonLoadingScreen\Source\CommonLoadingScreen\Private\LoadingScreenManager.cpp
    :5-28    依赖 include（用于收敛 Build.cs）
    :46-63   静态辅助函数实现（C1 的参照）
    :201-221 PreLoadMap / PostLoadMap
    :232,246 FThreadHeartBeat 检查点
    :255-405 判定链全量
    :407-465 最短显示时长 / 额外保持
    :467-484 首屏守卫（PreLoadScreen）
    :473-533 显示加载屏 + 输入吞噬 + Slate Tick
Plugins\CommonLoadingScreen\Source\CommonLoadingScreen\Public\LoadingScreenManager.h   :55-62 对外契约
Plugins\CommonLoadingScreen\Source\CommonLoadingScreen\Private\CommonLoadingScreenSettings.h :15-65 配置项
Plugins\UIExtension\Source\Public\UIExtensionSystem.h        :5-9 零 CommonGame 引用的证据
Plugins\UIExtension\Source\Public\Widgets\UIExtensionPointWidget.h
Plugins\CommonGame\Source\Public\PrimaryGameLayout.h         :35-137（:53-110 模板）
Plugins\CommonGame\Source\Public\GameUIManagerSubsystem.h    :22-50
Plugins\CommonGame\Source\Public\CommonGameInstance.h        :9-14,30-38,51-59,77
Plugins\CommonGame\Source\Private\CommonGameInstance.cpp     :8,89,98,108,114（CommonUser 耦合点）
Plugins\CommonGame\CommonGame.Build.cs                        （F4 的依赖证据）
Source\LyraGame\UI\Subsystem\LyraUIManagerSubsystem.cpp       :15-68（F18）
Source\LyraGame\Audio\LyraAudioMixEffectsSubsystem.cpp        :28,30,214-217（F16）
```

**项目侧现状（绝对路径）**

```text
Source\Hodgepodge\Public\Interface\LoadingProcessInterface.h        F8（:11,21,27,31）
Source\Hodgepodge\Private\Interface\LoadingProcessInterface.cpp     F9（仅 8 行，缺静态实现）
Source\Hodgepodge\Public\Component\HodgeExperienceManagerComponent.h  F8（:37,50-53）
Source\Hodgepodge\Private\Component\HodgeExperienceManagerComponent.cpp F8（:627）
Source\Hodgepodge\Public\GameFeatures\GameFeatureAction_AddWidget.h   F10（全文注释）
Source\Hodgepodge\Public\Core\LocalPlayer\HodgeLocalPlayerBase.h      F11（:31,52-109,143-144）
Source\Hodgepodge\Public\Core\GameInstance\HodgeGameInstanceBase.h    F12（:23,36,39）
Source\Hodgepodge\Public\Core\HUD\HodgeHUD.h                          F10/R 相关
Config\DefaultEngine.ini                                             F13/F14
Source\Hodgepodge\Hodgepodge.Build.cs                                现有模块依赖
```

---

**变更记录**

| 日期 | 内容 |
|---|---|
| 2026-09-25 | 首版执行草案。基线来自对 Lyra 源码与本项目的只读实测；未修改任何代码。 |
| 2026-09-26 | 新增 §10「待复活文件清单」：79 个 UI 文件中有 30 个被整体注释（见 §10.2 分组）；修正 2 处 `Ui/` 大小写；`Build.cs` 补 `ApplicationCore`。Editor / Game 双构建 EXIT=0。 |
| 2026-09-26 | C 组复活：`SActorCanvas.h/.cpp` + `IndicatorLayer.h/.cpp` 复原并改用 `FStreamableManager`（见 §11）。已注释 30 → 26 个，Editor / Game 双构建 EXIT=0。 |
| 2026-09-26 | 新增 §12「只引入 UIExtension 的落地方案（不搬 CommonGame）」；更正 §0.2/§3.2 关于"UIExtension 零依赖"的说法。配套新增 [文件导览](lyra-ui-file-guide.md)。本轮未改代码。 |
| 2026-09-26 | 新增 [Indicator UI 系统：职责边界与完整流程](indicator-ui-system.md)（学习笔记 + 源码核实）。本轮未改代码。 |
| 2026-09-27 | `UHodgeHUDLayout` 复活：Push/Pop 改走引擎 `UCommonActivatableWidgetStack`（`MenuLayerStack` + `EnsureMenuLayerStack()` C++ 兜底）；`HodgeGameModeBase.cpp:43` 的 `HUDClass` 指向 `AHodgeHUD`。已注释 26 → 24 个，Editor / Game 双构建 EXIT=0。 |
| 2026-09-27 | 新增 §13「UI 骨架三块 todo（架构学习主线）」与完成度实测：源码文件 100% 覆盖、框架插件 ~3%、`Content/UI` 蓝图 0%（606 vs 0）。本轮仅写文档。 |
