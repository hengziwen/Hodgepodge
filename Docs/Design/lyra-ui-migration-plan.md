# Lyra UI 架构迁移执行文档（阶段一）

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
| `AHodgeHUDBase` | 空壳（`Public/Core/HUD/HodgeHUDBase.h:31`） | 本阶段不改，但它是 HUD 可见性链的一环 | — |
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
Source\Hodgepodge\Public\Core\HUD\HodgeHUDBase.h                      F10/R 相关（:31 空壳）
Config\DefaultEngine.ini                                             F13/F14
Source\Hodgepodge\Hodgepodge.Build.cs                                现有模块依赖
```

---

**变更记录**

| 日期 | 内容 |
|---|---|
| 2026-09-25 | 首版执行草案。基线来自对 Lyra 源码与本项目的只读实测；未修改任何代码。 |
