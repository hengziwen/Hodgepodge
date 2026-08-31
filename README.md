# Hodgepodge

> 一个基于 **UE 5.5** 的个人游戏框架，目标是"ALS 的 locomotion + Lyra 的架构理念 + GAS 的战斗"，最终形态是 **UE Dedicated Server + 数据驱动的动作 RPG 底层框架**。

| 项目 | 值 |
|---|---|
| 引擎版本 | Unreal Engine **5.5** |
| 主模块 | `Hodgepodge`（Runtime，单模块） |
| 代码规模 | `Source/Hodgepodge` 共 89 个文件（44 `.h` + 44 `.cpp` + 1 `.cs`） |
| 核心依赖 | ALS-Refactored 4.15、GameplayAbilities、GameFeatures、EnhancedInput、**ModularGameplay** |
| 项目阶段 | **Experience 的 C++ 层已搬完，资产层尚未开始**（当前**无法启动**，见下方阻塞项） |
| 相关文档 | [`LYRA_LEARNING_GUIDE.md`](LYRA_LEARNING_GUIDE.md)（学什么）、[`LYRA_RUNTIME_FLOW.md`](LYRA_RUNTIME_FLOW.md)（怎么跑）、[`UE5 开放世界动作 RPG 架构方案 V2.md`](UE5%20开放世界动作%20RPG%20架构方案%20V2.md)（总体方案） |
| 代码基线 | 提交 `b8e708f` "夺舍Lyra重构了一堆Gameplay基类并接上Experience" |

---

## ⚠️ 当前最高优先级阻塞：项目跑不起来

**先澄清进度**：Experience 的 **C++ 代码层已经搬完了**（`b8e708f`，GameMode / GameState / PlayerState / 状态机全部就位）。
但 **资产层一步没做** —— `Content/` 里还没有任何一个 Experience 资产，`DefaultGame.ini` 也没注册扫描项。
代码写完 ≠ 能跑，现在启动必然崩溃：

```
GameMode::InitGame → HandleMatchAssignmentIfNotExpectingOne()
  → 所有来源都没有有效 Experience → 硬编码回退到 "B_HodgeDefaultExperience"
  → OnMatchAssignmentGiven() → ExperienceComponent->SetCurrentExperience()
  → UHodgeAssetManager::GetPrimaryAssetPath(ExperienceId)   ← 扫不到，路径无效
  → AssetPath.TryLoad() 返回 nullptr
  → check(AssetClass)    ← 💥 崩溃
```

**缺失的三样东西：**

| # | 缺失项 | 位置 |
|---|---|---|
| 1 | **Experience 数据资产本身** | 整个 `Content/` 下搜索 `*Experience*` 结果是 **0 个文件** |
| 2 | **`HodgeExperienceDefinition` 的 `PrimaryAssetTypesToScan`** | `Config/DefaultGame.ini` 里只有 `HodgeGameData` 和 `GameFeatureData` |
| 3 | **`UHodgeWorldSettings`**（可选，用于地图指定默认 Experience） | 代码里对应分支是注释状态 |

**修复办法**（在编辑器里做，约 10 分钟）：

1. 创建蓝图类，父类选 `HodgeExperienceDefinition`，命名为 **`B_HodgeDefaultExperience`**（必须这个名字，GameMode 里写死了），放到 `Content/Main/Data/`
2. 在 `DefaultGame.ini` 补上扫描项：
   ```ini
   +PrimaryAssetTypesToScan=(PrimaryAssetType="HodgeExperienceDefinition",AssetBaseClass="/Script/Hodgepodge.HodgeExperienceDefinition",bHasBlueprintClasses=True,bIsEditorOnly=False,Directories=((Path="/Game/Main/Data")),SpecificAssets=,Rules=(Priority=-1,ChunkId=-1,bApplyRecursively=True,CookRule=AlwaysCook))
   ```
3. 在该资产上配好 `DefaultPawnData`（指向 `DA_DafultPawnData`）

> 也可以用命令行临时绕过验证：`Hodgepodge.exe -Experience=xxx`，但资产不存在照样会崩，只是崩在不同的地方。

---

## 目录

- [1. 项目定位](#1-项目定位)
- [2. 技术栈与依赖](#2-技术栈与依赖)
- [3. 目录结构](#3-目录结构)
- [4. 快速开始](#4-快速开始)
- [5. 架构总览](#5-架构总览)
- [6. 核心系统详解](#6-核心系统详解)
- [7. 当前进度](#7-当前进度)
- [8. 代码规范与约定](#8-代码规范与约定)
- [9. 新人上手路径](#9-新人上手路径)
- [10. 调试与验证工具箱](#10-调试与验证工具箱)
- [11. 已知问题与技术债](#11-已知问题与技术债)
- [12. 附录：文件速查索引](#12-附录文件速查索引)

---

## 1. 项目定位

### 1.1 这是什么

Hodgepodge（大杂烩）是一个**用来长本事的框架工程**，不是一个要上线的游戏。它的定位与 Lyra 完全一致 —— 是 **bootstrapping framework（样板框架）**，不是 finished game。

它要回答的问题是：

> 如何围绕 UE 的核心技术，搭一套**能长出来**、**不推倒重来**、**从第一行代码就支持 Dedicated Server** 的动作 RPG 底层？

### 1.2 这不是什么

- **不是游戏** —— 没有可玩的玩法循环，没有 UI，没有关卡内容
- **不是 Lyra 的复制品** —— 只吸收理念，不整体照搬（Lyra 耦合的 CommonUI / CommonUser / GameSettings 等子系统，本项目一个都没引入）
- **不是稳定可运行的状态** —— 项目正处于一次大型重构的中途，见顶部阻塞项和 [§11 技术债](#11-已知问题与技术债)

### 1.3 从 Lyra 借来的设计信条

这些是本项目的"宪法"。完整推导见 [`LYRA_LEARNING_GUIDE.md` 第 4 章](LYRA_LEARNING_GUIDE.md#4-lyra-的十大核心设计理念)。

| # | 信条 | 本项目对应实现 | 状态 |
|---|---|---|---|
| 1 | **一套代码，多种玩法** | `UHodgeExperienceDefinition` + `AHodgeGameModeBase` 完整流程 | ✅ C++ 层接通<br>🚫 缺资产，见顶部阻塞项 |
| 2 | **数据驱动** | `UHodgePawnData` / `UHodgeGameData` / `UHodgeInputConfig` | 🚧 PawnData 只剩 1 个字段 |
| 3 | **插件化扩展** | `UHodgeGameFeaturePolicy` + `GameFeatureAction_*` | 🚧 Policy 完成，Action 基本是死代码 |
| 4 | **GameplayTag 作万能胶水** | `HodgeGameplayTags.h` + `FGameplayTagStackContainer` | ✅ |
| 5 | **组合优于继承** | `PawnData` 决定 Pawn 类、ModularGameplay 组件化 | ✅ |
| 6 | **服务器权威** | ASC 放 PlayerState、`Mixed` 复制模式 | ✅ |
| 7 | **Base / Concrete 分层** | `HodgeGameStateBase` → `HodgeGameState`、`HodgePlayerStateBase` → `HodgePlayerState` | ✅ **新增** |
| 8 | **Init State 链解耦异步依赖** | `UGameFrameworkComponentManager` + 4 个 InitState Tag | 🚧 链已注册，**无组件参与** |

### 1.4 与 Lyra 的关键分歧

读代码前务必理解这三点，否则会困惑"为什么这里和 Lyra 不一样"：

| 维度 | Lyra | Hodgepodge | 原因 |
|---|---|---|---|
| **Locomotion / Camera** | 自研 `LyraCharacterMovementComponent` + 相机模式栈 | 直接继承 **ALS** 的 `AAlsCharacter` / `UAlsCameraComponent` | 自研高质量 locomotion 成本极高，先借用 |
| **Pawn 与 GAS 的协调** | `ULyraPawnExtensionComponent` 驱动 Init State 链 | 目前**没有 PawnExtensionComponent**。Init State 链已注册但无组件参与，GAS 初始化改由 `AHodgePlayerState::PreInitializeComponents()` 完成 | 重构中间态，见 [§6.6](#66-modulargameplay-与-init-state-链--半成品) |
| **UI / 设置 / 登录** | CommonUI + UIExtension + GameSettings + CommonUser 全家桶 | **全部没有** | 体量大、非核心矛盾，延后引入 |

---

## 2. 技术栈与依赖

### 2.1 引擎与模块依赖

`Source/Hodgepodge/Hodgepodge.Build.cs`：

```csharp
PublicDependencyModuleNames:  Core, CoreUObject, Engine, InputCore,
                              GameplayAbilities, GameplayTags, GameplayTasks,
                              ModularGameplay,   // ← 本次重构新增
                              ALS, GameFeatures,
                              AIModule,          // ← 本次重构新增
                              EngineSettings,    // ← 本次重构新增
                              NetCore            // ← 本次重构新增
PrivateDependencyModuleNames: ALSCamera, EnhancedInput
```

`Hodgepodge.uproject` 启用的插件：`GameFeatures`、`GameplayAbilities`、`ModelingToolsEditorMode`（仅 Editor）、`UNTLink`。

> ⚠️ `UNTLink` 在 `.uproject` 里启用了，但项目内**搜不到该插件的任何文件**。不会致命报错，建议确认后移除。

### 2.2 插件清单

| 插件 | 版本 | 位置 | 说明 |
|---|---|---|---|
| **ALS-Refactored** | 4.15（UE5.5 对应版本） | `Plugins/ALS-Refactored-4.15/ALS-Refactored-4.15/` | Sixze 重写的 ALS V4 C++ 版，MIT 许可。4 个模块：`ALS`、`ALSCamera`、`ALSExtras`（示例）、`ALSEditor` |
| **RiderLink** | — | `Plugins/Developer/RiderLink/` | Rider IDE 联动，**不参与游戏逻辑** |

**ALS 核心类**：

| 类 | 位置 | 职责 |
|---|---|---|
| `AAlsCharacter` | `ALS/Public/AlsCharacter.h` | 主角色类。所有状态（LocomotionMode / RotationMode / Stance / Gait / ViewMode / OverlayMode）**用 GameplayTag 而非枚举**表示 |
| `UAlsCharacterMovementComponent` | `ALS/Public/AlsCharacterMovementComponent.h` | 为网络同步重写的移动组件 |
| `UAlsAnimationInstance` | `ALS/Public/AlsAnimationInstance.h` | 动画主线程实例，聚合 20+ 状态结构 |
| `UAlsCameraComponent` | `ALSCamera/Public/AlsCameraComponent.h` | 相机组件，继承 `USkeletalMeshComponent`，**不需要自定义 PlayerCameraManager** |

> **重要**：ALS 的 Tag 命名空间（`AlsGaitTags::`、`AlsStanceTags::`、`AlsRotationModeTags::` 等）和项目自己的 `HodgeGameplayTags` 是**两套**，不要混用。

### 2.3 未引入的 Lyra 插件

以下插件在 `LYRA_LEARNING_GUIDE.md` 里被标为"强烈推荐"，但**本项目一个都没有**。看到 `#include` 它们的代码，一定是编译不过的死代码。

`AsyncMixin`、`GameplayMessageRouter`、`UIExtension`、`ModularGameplayActors`、`CommonGame`、`GameSettings`、`CommonUser`、`CommonLoadingScreen`、`PocketWorlds`、`GameSubtitles`

---

## 3. 目录结构

### 3.1 源码目录

```
Source/
├── Hodgepodge.Target.cs
├── HodgepodgeEditor.Target.cs
└── Hodgepodge/
    ├── Hodgepodge.Build.cs
    ├── Hodgepodge.h / Hodgepodge.cpp
    ├── Public/       ← 43 个头文件
    └── Private/      ← 43 个实现文件，与 Public 严格镜像
```

| 目录 | 文件数 | 职责 | 重要度 |
|---|---|---|---|
| `Data/` | 7 | AssetManager、GameData、PawnData、**Experience 三件套** | ★★★★★ |
| `Core/` | 9 | GameInstance / GameMode / **GameState + GameStateBase** / PlayerController / **PlayerState + PlayerStateBase** / HUD / LocalPlayer | ★★★★★ |
| `Component/` | 5 | 组件基类 + ExperienceManagerComponent | ★★★★★ |
| `Character/` | 4 | 角色继承链 | ★★★★★ |
| `AbilitySystem/` | 6 | ASC、Ability 基类、GameplayTags、**GameplayTagStack**、AttributeSet、HealthSet | ★★★★ |
| `Input/` | 2 | InputConfig（数据）+ InputComponent（绑定） | ★★★★ |
| `GameFeatures/` | 8 | 7 个 GameFeatureAction + Policy | ★★（多数是死代码） |
| `Actor/` | 1 | Actor 基类 | ★ |
| `Interface/` | 1 | `ILoadingProcessInterface` | ★ |

**本次重构新增的文件**：

```
Public/AbilitySystem/GameplayTagStack.h        + Private/AbilitySystem/GameplayTagStack.cpp
Public/Core/GameState/HodgeGameState.h         + Private/Core/GameState/HodgeGameState.cpp
Public/Core/PlayState/HodgePlayerState.h       + Private/Core/PlayState/HodgePlayerState.cpp
```

> `Core/` 下有个历史遗留拼写错误：目录名是 `PlayState/`，应为 `PlayerState/`。**暂不修改**（改目录会动一堆 include 和 git 历史）。

### 3.2 内容目录

```
Content/
├── Main/                    ★ 项目自有内容，新东西放这里
│   ├── Data/                DA_Dafult_GameData / DA_DafultPawnData / DA_DafultGameData
│   │                        ⚠️ 缺 B_HodgeDefaultExperience（见顶部阻塞项）
│   ├── Input/               DA_HodgeInputConfig / IMC_Default / IMC_UI / InputAction/(18)
│   └── Character/EnemyBase/
├── Assets/                  Enemies / HeroCharacter / Weapons /
│                            Niagara / Sounds / Textures / Meshes / MaterialFunctions
├── Characters/              角色资源（145）
├── ALS/ ALSCamera/ ALSExtras/   插件自带内容，不要改
├── AdvancedLocomotionV4/    ALS V4 原始内容（历史遗留）
├── piaobozhe/ qiuyuan/      第三方角色资源
└── ThirdPerson/             UE 模板内容
```

---

## 4. 快速开始

### 4.1 环境要求

| 项 | 要求 |
|---|---|
| 引擎 | Unreal Engine **5.5** |
| IDE | Visual Studio 2022 或 Rider for Unreal |
| 平台 | Windows（当前只验证过 Win64，DX12） |
| 渲染 | 开启了 DX12 / SM6 / 虚拟阴影贴图 / Lumen，显卡要求偏高 |

### 4.2 编译与运行

```powershell
# 1) 生成解决方案（路径按你的引擎安装位置调整）
& "D:\Epic\UE_5.5\Engine\Build\BatchFiles\GenerateProjectFiles.bat" `
    -projectfiles -project="e:\Project\Git\Hodgepodge\Hodgepodge.uproject" -game -engine

# 2) 编译
& "D:\Epic\UE_5.5\Engine\Build\BatchFiles\Build.bat" `
    HodgepodgeEditor Win64 Development -project="e:\Project\Git\Hodgepodge\Hodgepodge.uproject"

# 3) 打开编辑器
& "D:\Epic\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe" `
    "e:\Project\Git\Hodgepodge\Hodgepodge.uproject"
```

### 4.3 首次运行检查清单

跑不起来 90% 是配置问题，按这个顺序查：

1. **先解决顶部阻塞项** —— 建 `B_HodgeDefaultExperience` 资产 + 补 `DefaultGame.ini` 扫描项。没做这步，后面全不用查。

2. **`DefaultEngine.ini` 的 `AssetManagerClassName`** 必须是 `/Script/Hodgepodge.HodgeAssetManager`。
   错了会直接 `UE_LOG(Fatal)` 退出。

3. **`DefaultGame.ini` 的 `[/Script/HodgePodge.HodgeAssetManager]` 段**：
   ```ini
   HodgeGameDataPath=/Game/Main/Data/DA_Dafult_GameData.DA_Dafult_GameData
   DefaultPawnData=/Game/Main/Data/DA_DafultPawnData.DA_Dafult_PawnData
   ```
   GameData 加载失败是 **Fatal**，不是 Warning。

4. **`GlobalDefaultGameMode` 用的是旧类名** `/Script/Hodgepodge.HodgepodgeGameModeBase`，靠 `DefaultEngine.ini` 的 `[CoreRedirects]` 生效。能跑，但建议改成 `HodgeGameModeBase`。

5. **启动地图**：`GameDefaultMap=/Game/ALSExtras/Levels/L_Als_Grid`，`EditorStartupMap=/Game/ThirdPerson/Maps/ThirdPersonMap`。

---

## 5. 架构总览

### 5.1 分层图

```
┌──────────────────────────────────────────────────────────────┐
│  内容层    Content/Main  +  GameFeature 插件（尚未建立）        │
│           数据资产组合出的玩法，不改 C++ 代码                    │
├──────────────────────────────────────────────────────────────┤
│  玩法层    AbilitySystem/  Character/  Component/             │
│           GameFeatures/   （战斗、装备、背包 — 大部分待建）      │
├──────────────────────────────────────────────────────────────┤
│  框架层    Core/                                              │
│           ├─ Base 层   GameStateBase / PlayerStateBase         │
│           │            （生命周期 + ModularGameplay 接收者）     │
│           └─ 实例层   GameMode / GameState / PlayerState       │
│                       （Experience + GAS + 玩家状态）           │
│           Data/（AssetManager + Experience 三件套）            │
├──────────────────────────────────────────────────────────────┤
│  基础层    Plugins: ALS-Refactored（ALS / ALSCamera）          │
│           + 引擎: GAS / EnhancedInput / GameFeatures /         │
│             ModularGameplay / AssetManager                     │
└──────────────────────────────────────────────────────────────┘
```

### 5.2 启动时序（当前实现）

Experience 已经接通，这是现在的完整流程：

```
① 引擎启动
   UHodgeAssetManager::StartInitialLoading()
     ├─ Super::StartInitialLoading()              扫描 PrimaryAssetTypesToScan
     ├─ STARTUP_JOB(InitializeGameplayCueManager())       [空实现]
     ├─ STARTUP_JOB_WEIGHTED(GetGameData(), 25.f)        同步加载 GameData
     └─ DoAllStartupJobs()                        权重进度 → [空实现]

   UHodgeGameInstanceBase::Init()
     └─ UGameFrameworkComponentManager 注册 4 个 Init State
        Spawned → DataAvailable → DataInitialized → GameplayReady

② 地图加载
   AHodgeGameModeBase::InitGame()
     └─ SetTimerForNextTick(HandleMatchAssignmentIfNotExpectingOne)

③ 下一帧 → HandleMatchAssignmentIfNotExpectingOne()
   按优先级确定 Experience：
     1. Matchmaking 分配        （未接入）
     2. URL Options             ?Experience=XXX
     3. Developer Settings      （仅 PIE，代码注释中）
     4. 命令行                  -Experience=XXX
     5. WorldSettings           （代码注释中，缺 UHodgeWorldSettings）
     6. Dedicated Server        TryDedicatedServerLogin()
     7. 硬编码默认               "B_HodgeDefaultExperience"   ← 目前总会走到这里

④ OnMatchAssignmentGiven(ExperienceId, Source)
     └─ ExperienceComponent->SetCurrentExperience(ExperienceId)

⑤ AHodgeGameModeBase::InitGameState()   （早于 ③④ 执行）
     └─ CallOrRegister_OnExperienceLoaded(→ this->OnExperienceLoaded)

⑥ UHodgeExperienceManagerComponent 状态机
   Unloaded → Loading（Bundle 异步加载，区分客户端/服务端）
            → LoadingGameFeatures（LoadAndActivateGameFeaturePlugin）
            → [LoadingChaosTestingDelay]（可选）
            → ExecutingActions（执行 Experience.Actions + ActionSets.Actions）
            → Loaded
   └─ 三档委托广播：HighPriority → Normal → LowPriority

⑦ AHodgeGameModeBase::OnExperienceLoaded()
     └─ 遍历所有 PlayerController，给还没有 Pawn 的玩家 RestartPlayer()

⑧ 生成 Pawn
   GetDefaultPawnClassForController()
     └─ GetPawnDataForController()
          ├─ PlayerState->GetPawnData()          （由 ⑨ 设置）
          ├─ Experience->DefaultPawnData
          └─ UHodgeAssetManager::GetDefaultPawnData()

⑨ AHodgePlayerState::OnExperienceLoaded()   （非客户端，注册于 PreInitializeComponents）
     └─ SetPawnData(GameMode->GetPawnDataForController(...))
          ├─ MARK_PROPERTY_DIRTY_FROM_NAME + 赋值
          ├─ [注释] AbilitySet->GiveToAbilitySystem()
          └─ SendGameFrameworkComponentExtensionEvent(NAME_HodgeAbilityReady)
```

### 5.3 关键设计：为什么 Pawn 要等 Experience 加载完才生成

只有 Experience 加载完成后，才知道该用哪个 `PawnData`、该授予哪些能力。所以：

- `HandleStartingNewPlayer_Implementation()` 里加了 `IsExperienceLoaded()` 守卫，没加载完就不处理新玩家
- `GetPawnDataForController()` 在 Experience 未加载时直接返回 `nullptr`
- `OnExperienceLoaded()` 负责给"已连接但还没 Pawn"的玩家补一次 `RestartPlayer()`

---

## 6. 核心系统详解

### 6.1 AssetManager ✅

`Data/HodgeAssetManager.h/.cpp` —— 整个项目完成度最高的部分，Lyra `LyraAssetManager` 的等价实现。

| 能力 | 说明 |
|---|---|
| **StartupJob 权重进度系统** | `STARTUP_JOB(func)` / `STARTUP_JOB_WEIGHTED(func, weight)` |
| **GameData 类型化缓存** | `GameDataMap` 以 Class 为 Key，避免重复加载 |
| **软引用同步加载** | `GetAsset<T>()` / `GetSubclass<T>()`，`bKeepInMemory` 控制常驻 |
| **常驻资源池** | `LoadedAssets` + `FCriticalSection` 持强引用防 GC |
| **PIE 预加载** | `PreBeginPIE()` 确保进入 PIE 前 GameData 就绪 |
| **调试命令** | `Hodge.DumpLoadedAssets` |

**零依赖的必读文件**：`Data/HodgeAssetManagerStartupJob.h` —— 全项目最值得先读的文件，把"多个异步加载 → 按序回调 → 汇报加权进度"完整封装了一遍。

**空实现（待办）**：`InitializeGameplayCueManager()`、`UpdateInitialGameContentLoadPercent()`

### 6.2 Experience 系统 ⚠️ C++ 层已接通，资产层缺失

本项目的 Experience 是 Lyra 同名系统的移植，**代码结构与 Lyra 高度一致**。
想深入理解每一步在干什么，强烈建议对照 [`LYRA_RUNTIME_FLOW.md`](LYRA_RUNTIME_FLOW.md) 的第 4~6 章阅读 —— 那篇文档把决策链路、状态机、启动时序逐行拆开讲了，把里面的 `Lyra` 前缀换成 `Hodge` 基本就是本项目的行为。

#### 三个数据资产

| 类 | 文件 | 内容 |
|---|---|---|
| `UHodgeExperienceDefinition` | `Data/HodgeExperienceDefinition.h` | `GameFeaturesToEnable[]`、`DefaultPawnData`、`Actions[]`（Instanced）、`ActionSets[]` |
| `UHodgeExperienceActionSet` | `Data/HodgeExperienceActionSet.h` | 可复用的 `Actions[]` + `GameFeaturesToEnable[]` |
| `UHodgePawnData` | `Data/HodgePawnData.h` | **目前只剩 `PawnClass` 一个字段**，其余 4 个仍被注释 |

#### 加载状态机

`Component/HodgeExperienceManagerComponent.h/.cpp`，挂在 `AHodgeGameState` 上（继承 `UGameStateComponent` + `ILoadingProcessInterface`）。

```
Unloaded → Loading → LoadingGameFeatures → LoadingChaosTestingDelay
                                                    ↓
                                          ExecutingActions → Loaded → Deactivating → Unloaded
```

| 方法 | 职责 |
|---|---|
| `SetCurrentExperience(FPrimaryAssetId)` | 服务端入口，`CurrentExperience` 通过 `DOREPLIFETIME` 复制到客户端 |
| `OnRep_CurrentExperience()` | 客户端收到后自行启动加载 |
| `StartExperienceLoad()` | 收集 Bundle 资源，按客户端/服务端区分后异步加载 |
| `OnExperienceLoadComplete()` | 收集插件 URL 并 `LoadAndActivateGameFeaturePlugin` |
| `OnGameFeaturePluginLoadComplete()` | 计数归零后进入下一步 |
| `OnExperienceFullLoadCompleted()` | 执行 Actions，按三档广播委托 |
| `EndPlay()` | 逆序停用插件 + Deactivating 流程 |
| `ShouldShowLoadingScreen()` | 给 Loading Screen 用（目前无消费者） |

**三档委托**（`CallOrRegister_OnExperienceLoaded_HighPriority` / `_` / `_LowPriority`）解决异步依赖的经典问题：**"如果已经加载完就立即回调，否则注册等通知"**。这个模式在 `UHodgeLocalPlayerBase` 里也用了，是本项目标准手法。

#### GameMode 侧的新增接口

| 方法 | 作用 |
|---|---|
| `HandleMatchAssignmentIfNotExpectingOne()` | 按 7 级优先级确定 Experience |
| `OnMatchAssignmentGiven()` | 触发 `SetCurrentExperience` |
| `OnExperienceLoaded()` | 给未生成 Pawn 的玩家补 `RestartPlayer()` |
| `GetPawnDataForController()` | PlayerState.PawnData → Experience.DefaultPawnData → AssetManager 默认 |
| `GetDefaultPawnClassForController_Implementation()` | 用 PawnData->PawnClass 决定 Pawn 类 |
| `SpawnDefaultPawnAtTransform_Implementation()` | `bDeferConstruction = true` 延迟构造，预留了设置 PawnData 的位置（注释中） |
| `RequestPlayerRestartNextFrame()` | 下一帧重生，`bForceReset` 可强制放弃当前 Pawn |
| `FailedToRestartPlayer()` | 重生失败后按条件下一帧重试，避免无限循环 |
| `TryDedicatedServerLogin()` | DS 专用，仅占位（CommonUser 逻辑注释中） |
| `OnGameModePlayerInitialized` | 玩家完成 GameMode 层初始化的多播委托 |

### 6.3 新增：`AHodgeGameState`（游戏级 GAS + Experience 宿主）

`Core/GameState/HodgeGameState.h/.cpp`，继承 `AHodgeGameStateBase` + `IAbilitySystemInterface`。

| 成员 | 说明 |
|---|---|
| `ExperienceManagerComponent` | **Experience 的宿主**，构造函数里 `CreateDefaultSubobject` |
| `AbilitySystemComponent` | **游戏级 ASC**，`PostInitializeComponents` 里 `InitAbilityActorInfo(this, this)`（Owner 和 Avatar 都是 GameState） |
| `ServerFPS` | 服务器 Tick 里 `GAverageFPS` 更新，`DOREPLIFETIME` 复制 |
| `RecorderPlayerState` | 回放录制者，用 `COND_ReplayOnly` 只在回放流复制 |
| `OnRecorderPlayerStateChangedEvent` | 录制者变化的委托 |

游戏级 ASC 的典型用途：全局 GameplayCue、全场 Buff、需要"不隶属于任何玩家"的效果。目前还没使用者。

`AHodgeGameStateBase` 已**瘦身为纯基类**（只剩 `PreInitializeComponents` / `BeginPlay` / `EndPlay` 三个空扩展点）。

### 6.4 新增：`AHodgePlayerState`（玩家状态主体）

`Core/PlayState/HodgePlayerState.h/.cpp`，继承 `AHodgePlayerStateBase` + `IAbilitySystemInterface`。

| 成员 | 说明 |
|---|---|
| `AbilitySystemComponent` | 玩家 ASC，构造时创建，`SetIsReplicated(true)` + `Mixed` 模式。`NetUpdateFrequency = 100.f` |
| `HealthSet` | 构造时 `CreateDefaultSubobject`，靠 ASC 的 `InitializeComponent` 自动检测 |
| `PawnData` | `ReplicatedUsing = OnRep_PawnData`，由 `OnExperienceLoaded` 设置 |
| `MyPlayerConnectionType` | `EHodgePlayerConnectionType` 枚举：Player / LiveSpectator / ReplaySpectator / InactivePlayer |
| `MyTeamID` / `MySquadID` | 队伍 / 小队 ID（`FGenericTeamId`），复制回调目前是空的 |
| `StatTags` | `FGameplayTagStackContainer`，玩家统计数据的 Tag+数量容器 |
| `ReplicatedViewRotation` | 观战用视角旋转，`COND_SkipOwner` |
| `NAME_HodgeAbilityReady` | 静态 `FName("HodgeAbilitiesReady")`，SetPawnData 后发送该扩展事件 |

**生命周期要点**：

```cpp
// PreInitializeComponents
AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());   // ⚠️ GetPawn() 此时大概率是 nullptr
// 非客户端：注册 Experience 加载完成回调
ExperienceComponent->CallOrRegister_OnExperienceLoaded(...);
```

复制全部用 **Push Model**（`bIsPushBased = true` + `MARK_PROPERTY_DIRTY_FROM_NAME`），比传统轮询复制省 CPU。

### 6.5 新增：`GameplayTagStack`（Tag + 数量的通用容器）

`AbilitySystem/GameplayTagStack.h/.cpp`，Lyra 同名文件的移植。

```cpp
USTRUCT(BlueprintType)
struct FGameplayTagStack : public FFastArraySerializerItem
{
    FGameplayTag Tag;
    int32 StackCount = 0;
};

USTRUCT(BlueprintType)
struct FGameplayTagStackContainer : public FFastArraySerializer
{
    void AddStack(FGameplayTag Tag, int32 StackCount);
    void RemoveStack(FGameplayTag Tag, int32 StackCount);
    int32 GetStackCount(FGameplayTag Tag) const;
    bool ContainsTag(FGameplayTag Tag) const;

    TArray<FGameplayTagStack> Stacks;        // 参与复制
    TMap<FGameplayTag, int32> TagToCountMap; // 查询缓存，不复制
};
```

用 `FFastArraySerializer` 做**增量复制**（改一项只传增量，不是整体替换），并提供 `PostReplicatedAdd` / `PreReplicatedRemove` / `PostReplicatedChange` 三个钩子。Lyra 用弹药、分数、统计都靠它，本项目目前只有 `StatTags` 一处使用。

### 6.6 ModularGameplay 与 Init State 链 🚧 半成品

本次重构开始接入，但**只做了一半**。

**已做的：**

```cpp
// UHodgeGameInstanceBase::Init()
UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);
ComponentManager->RegisterInitState(InitState_Spawned,         false, FGameplayTag());
ComponentManager->RegisterInitState(InitState_DataAvailable,   false, InitState_Spawned);
ComponentManager->RegisterInitState(InitState_DataInitialized, false, InitState_DataAvailable);
ComponentManager->RegisterInitState(InitState_GameplayReady,   false, InitState_DataInitialized);
```

```cpp
// AHodgePlayerStateBase
PreInitializeComponents() → UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
BeginPlay()               → SendGameFrameworkComponentExtensionEvent(NAME_GameActorReady);
EndPlay()                 → RemoveGameFrameworkComponentReceiver(this);
Reset()                   → 转发给所有 UPlayerStateComponent
CopyProperties()          → 按类型+名字匹配，逐个复制 UPlayerStateComponent 的数据
```

**没做的（关键）：**

- ❌ **没有任何组件调用 `RegisterInitStateFeature`**
- ❌ **没有任何类实现 `IGameFrameworkInitStateInterface`**
- ❌ **`UHodgePawnExtensionComponent` 根本不存在**（只在注释里出现：GameMode 的 `SpawnDefaultPawnAtTransform`、PlayerState 的 `ClientInitialize`、`GameFeatureAction_AddInputBinding`）

**结论**：链搭好了，节点没接上。在这个状态下 Init State 链不会起作用，所有 `InitState_*` Tag 只是被注册而已。

### 6.7 角色体系（ALS）✅

```
ACharacter
    ↓
AAlsCharacter                    ← ALS 插件：移动、动画、状态机
    ↓
AHodgeCharacterBase              ← 生命周期扩展点，不依赖 GAS
    ↓
AHodgeCombatCharacter            ← + IAbilitySystemInterface，持有 ASC（但从未初始化）
    ↓
    ├── AHodgeHeroCharacter      ← 玩家：ALS 相机 + 输入
    └── AHodgeEnemyCharacter     ← 敌人：几乎空实现
```

| 类 | 文件 | 关键点 |
|---|---|---|
| `AHodgeCharacterBase` | `Character/HodgeCharacterBase.h` | 继承 `AAlsCharacter`，只提供生命周期扩展点。**刻意不放 GAS** |
| `AHodgeCombatCharacter` | `Character/HodgeCombatCharacter.h` | 持有 `HodgeAbilitySystemComponent`，但 `PossessedBy` 里的 `InitAbilityActorInfo(this, this)` **是注释状态**，所以这个 ASC 实际上从未被初始化 |
| `AHodgeHeroCharacter` | `Character/HodgeHeroCharacter.h` | 重载 `CreatePlayerInputComponent()` 返回 `UHodgeInputComponentBase`；`NotifyControllerChanged()` 管理 IMC；12 个 ALS 输入动作 |
| `AHodgeEnemyCharacter` | `Character/HodgeEnemyCharacter.h` | 骨架，碰撞盒 / 血条 / 战斗组件全部注释 |

**ALS 实现的 12 个输入动作**：Look、Move、Sprint、Walk、Crouch、Jump、Aim、Ragdoll、Roll、RotationMode、ViewMode、SwitchShoulder。

### 6.8 GAS ⚠️ 初始化链路存在断裂

这是本次重构**最需要注意的地方**，改动前务必先读。

#### 旧方案（已废弃）

```
AHodgePlayerStateBase::InitializeAbilitySystemForCharacter(Avatar)
  ├─ ASC->InitAbilityActorInfo(PS, Avatar)
  └─ 服务端遍历 AttributeSetClasses 动态创建属性集
调用点：AHodgeHeroCharacter::PossessedBy() / OnRep_PlayerState()
```

`InitializeAbilitySystemForCharacter` **已在本次重构中删除**。

#### 新方案（当前）

```
AHodgePlayerState::PreInitializeComponents()
  └─ ASC->InitAbilityActorInfo(/*Owner=*/this, /*Avatar=*/GetPawn())
```

#### 断裂点

| 问题 | 说明 |
|---|---|
| **`GetPawn()` 大概率是 nullptr** | PlayerState 的创建早于 Pawn，`PreInitializeComponents` 时 Pawn 还没生成。Lyra 是在 Pawn 生成后由 `PawnExtensionComponent` 调用 `InitializeAbilitySystem(ASC, Pawn)` **重新绑定** Avatar 的，本项目没有这个组件 |
| **`HodgeHeroCharacter::PossessedBy` / `OnRep_PlayerState` 是空块** | 旧的调用被删了，新的没接上，留下两个空的 `if` 块： <br>`if (AHodgePlayerStateBase* PS = GetPlayerState<AHodgePlayerStateBase>()) { }` |
| **属性集不再动态创建** | 改成构造时 `CreateDefaultSubobject<UHodgeHealthSet>`，靠 ASC 的 `InitializeComponent` 自动检测。要加新属性集得改 C++，不再是配置驱动 |
| **`AHodgeCombatCharacter` 的 ASC 是孤儿** | 持有但从不初始化，`GetAbilitySystemComponent()` 返回它，可能误导调用方 |

**结论**：当前 ASC 的 Avatar 很可能是 nullptr，依赖 Avatar 的 GAS 功能（如需要 Avatar 的 Ability、GameplayCue）会异常。**修这个的最佳方式就是补上 `UHodgePawnExtensionComponent` + Init State 链**，这也是 Lyra 的原设计。

#### 三套 ASC 的现状

| ASC | 宿主 | 状态 |
|---|---|---|
| `AHodgePlayerState::AbilitySystemComponent` | 玩家 | ✅ 有效，待修 Avatar |
| `AHodgeGameState::AbilitySystemComponent` | 游戏全局 | ✅ 有效（Owner = Avatar = GameState），暂无使用者 |
| `AHodgeCombatCharacter::HodgeAbilitySystemComponent` | 角色 | ❌ 从未初始化，建议删除或明确用途 |

#### 属性集

| 类 | 文件 | 说明 |
|---|---|---|
| `UHodgeAttributeSet` | `AbilitySystem/AttributeSet/HodgeAttributeSet.h` | 基类，`ATTRIBUTE_ACCESSORS` 宏 + `FHodgeAttributeEvent` 六参委托 |
| `UHodgeHealthSet` | `AbilitySystem/AttributeSet/HodgeHealthSet.h` | Health / MaxHealth / **Healing** / **Damage**（Meta 属性）/ BaseDamage / BaseHeal |

**HealthSet 的 Meta 属性设计**（来自 Lyra，必须理解）：

```
GE 施加伤害 → Damage（Meta 属性，一次性）
            → PostGameplayEffectExecute 里 Health -= Damage
            → Damage 清零，广播 OnHealthChanged
```

为什么要绕一圈？因为要**在扣减前做统一处理**（死亡判定、护盾、溢出治疗转护盾等），并拿到完整的 `FGameplayEffectModCallbackData` 上下文。

### 6.9 输入框架 ✅

```
UHodgeInputConfig  (DataAsset)          UHodgeInputComponentBase  (: UEnhancedInputComponent)
├─ NativeInputActions[]                 ├─ AddInputMappings / RemoveInputMappings
│   （InputAction + InputTag）           ├─ BindNativeAction<T>(Tag, TriggerEvent, Obj, Func)
└─ AbilityInputActions[]                ├─ BindAbilityActions<T>(...)
                                        └─ RemoveBinds(Handles)
```

按 `InputTag` 查找 `InputAction` 后绑定，是本项目数据驱动输入的体现。内容资产：`DA_HodgeInputConfig`、`IMC_Default`、`IMC_UI`、`InputAction/`（18 个）。

> `AddInputMappings` / `RemoveInputMappings` 是**空实现**。IMC 增删实际在 `AHodgeHeroCharacter::NotifyControllerChanged()` 里手工做的。

### 6.10 GameFeature 🚧 Policy 完成，Action 基本是死代码

`GameFeatures/HodgeGameFeaturePolicy.h` 实现 3 个 Observer：`UHodgeGameFeaturePolicy` / `UHodgeGameFeature_HotfixManager` / `UHodgeGameFeature_AddGameplayCuePaths`。

**7 个 GameFeatureAction 的真实状态**：

| 文件 | 状态 |
|---|---|
| `GameFeatureAction_WorldActionBase.h` | ✅ 可用（Lyra 原版） |
| `GameFeatureAction_SplitscreenConfig.h` | ✅ 可用（Lyra 原版） |
| `GameFeatureAction_AddAbilities.h` | ❌ 整体注释（依赖 `ULyraAbilitySet`，本项目没有） |
| `GameFeatureAction_AddWidget.h` | ❌ 整体注释（依赖 UIExtension） |
| `GameFeatureAction_AddInputBinding.h` | ❌ 整体注释 |
| `GameFeatureAction_AddInputContextMapping.h` | ❌ 整体注释 |
| `GameFeatureAction_AddGameplayCuePath.h` | ❌ 整体注释（功能已由 Policy 的 Observer 覆盖） |

这 5 个注释掉的文件是**故意保留的 Lyra 参考实现**，不要删，但也不要指望它们能编译。

### 6.11 其他框架类

| 类 | 文件 | 说明 |
|---|---|---|
| `UHodgeGameInstanceBase` | `Core/GameInstance/` | 注册 Init State 链；持有 `DebugTestEncryptionKey`（硬编码测试密钥） |
| `AHodgePlayerControllerBase` | `Core/PlayerController/` | **事件桥梁**：把引擎回调转成 LocalPlayer 的多播委托 |
| `UHodgeLocalPlayerBase` | `Core/LocalPlayer/` | 三个 `CallAndRegister_On*Set` 委托 + `bIsPlayerViewEnabled` |
| `AHodgeHUDBase` | `Core/HUD/` | HUD 占位 |
| `UHodgeActorComponentBase` | `Component/` | 组件基类，默认**开 Tick** |
| `UHodgeCombatComponentBase` | `Component/` | 战斗组件，默认**关 Tick**；空实现 |
| `UHodgeInteractionComponentBase` | `Component/` | 交互组件；空实现 |
| `UHodgeMovementComponentBase` | `Component/` | 移动组件；空实现（实际用 ALS 的） |
| `ILoadingProcessInterface` | `Interface/` | `ShouldShowLoadingScreen()`，目前无消费者 |

---

## 7. 当前进度

对照 `UE5 开放世界动作 RPG 架构方案 V2.md` 的 Phase 划分。

### 7.1 已完成 ✅

| 模块 | 内容 |
|---|---|
| **Experience 系统（C++ 全链路）** | 数据资产三件套的**类定义** + 7 状态机 + Bundle 加载 + GameFeature 激活/停用 + 三档委托 + GameMode 全流程接通（7 级优先级、延迟生成 Pawn、PawnData 三级回退、重生重试）<br>⚠️ **只有 C++ 类，还没有任何数据资产实例** |
| **Base / Concrete 分层** | `GameStateBase → GameState`、`PlayerStateBase → PlayerState`，职责分离 |
| **游戏级 ASC** | `AHodgeGameState` 持有全局 ASC，Owner = Avatar = GameState |
| **玩家状态主体** | PawnData 复制、连接类型枚举、队伍/小队、StatTags、观战视角旋转、Push Model 复制 |
| **GameplayTagStack** | Tag + 数量的 FastArray 增量复制容器 |
| **ModularGameplay 基础设施** | Init State 链注册、PlayerState 的 Receiver 注册与事件转发、PlayerStateComponent 的 Reset/CopyProperties |
| **AssetManager** | StartupJob 权重进度、GameData 类型化缓存、软引用同步加载、常驻资源池、PIE 预加载 |
| **ALS 接入** | 角色继承链、移动、动画、相机、12 个输入动作 |
| **输入框架** | `HodgeInputConfig` + `HodgeInputComponentBase` |
| **GameplayTag 体系** | 100+ 原生 Tag |

### 7.2 阻塞中 🚫（不解决无法启动）

| # | 事项 | 说明 |
|---|---|---|
| 1 | **建 `B_HodgeDefaultExperience` 资产** | Content 下搜索 `*Experience*` 为 0 |
| 2 | **注册 `HodgeExperienceDefinition` 扫描项** | `DefaultGame.ini` 缺失，会导致 `check(AssetClass)` 崩溃 |

### 7.3 未完成 🚧

| 优先级 | 事项 | 说明 |
|---|---|---|
| 🔴 P0 | **修 GAS Avatar 断裂** | 补 `UHodgePawnExtensionComponent`，或用其他方式在 Pawn 生成后重绑 ASC 的 Avatar |
| 🔴 P0 | **清理 `AHodgeCombatCharacter` 的孤儿 ASC** | 持有但从不初始化，会误导调用方 |
| 🔴 P0 | **清理 `HodgeHeroCharacter` 的两个空 `if` 块** | 重构遗留的中间态 |
| 🟠 P1 | **让 Init State 链真正跑起来** | 需要有组件 `RegisterInitStateFeature` + 实现 `IGameFrameworkInitStateInterface` |
| 🟠 P1 | **补 `UHodgePawnData` 字段** | AbilitySets / TagRelationshipMapping / InputConfig / DefaultCameraMode 仍被注释 |
| 🟠 P1 | **实现 `UHodgeAbilitySet`** | 能力批量授予/回收（Lyra 理念 5），`SetPawnData` 里的调用是注释状态 |
| 🟠 P1 | **实现 `UHodgeGameplayCueManager`** | `InitializeGameplayCueManager()` 是空实现 |
| 🟠 P1 | **建立独立 Log Category** | 全部用 `LogTemp`。`HodgeLogChannels.h` **文件本身不存在** |
| 🟡 P2 | 建 `UHodgeWorldSettings` | 让地图能指定默认 Experience |
| 🟡 P2 | 解注释并实现 5 个 GameFeatureAction | 需先补 Lyra 依赖 |
| 🟡 P2 | 建第一个 GameFeature 插件实例 | `Content/` 下无 `.uplugin` |
| 🟡 P2 | 战斗系统 | `HodgeCombatComponentBase` 空的；`HodgeEnemyCharacter` 全注释 |
| 🟡 P2 | Loading Screen | `ILoadingProcessInterface` 和 `UpdateInitialGameContentLoadPercent` 都无消费者 |
| 🟡 P2 | UI 系统 | 完全没有 |
| 🟢 P3 | Equipment / Inventory / Weapon 三段式模型 | 待建 |
| 🟢 P3 | Teams 阵营系统 | `MyTeamID` 有了，但子系统没有 |
| 🟢 P3 | AI（AIController / BehaviorTree / EQS） | 待建（AIModule 依赖已加） |
| 🟢 P3 | ReplicationGraph / SignificanceManager | 大规模 Actor 时才需要 |
| 🟢 P3 | 开放世界 / 后端 | 按方案最后做 |

### 7.4 路线图（建议顺序）

```
阶段 A（1~2 天）：让项目跑起来        ← 当前在这里
  ├─ 建 B_HodgeDefaultExperience 资产
  ├─ DefaultGame.ini 补 HodgeExperienceDefinition 扫描项
  └─ 验证 Experience 状态机跑通（日志 + 断点）

阶段 B（1 周）：修复 GAS 初始化
  ├─ 补 UHodgePawnExtensionComponent，接上 Init State 链
  ├─ 验证 ASC 的 Avatar 正确指向 Pawn
  ├─ 清理 HodgeCombatCharacter 的孤儿 ASC 和 HeroCharacter 的空 if 块
  └─ 加独立 Log Category

阶段 C（2~3 周）：数据驱动补全
  ├─ 补齐 UHodgePawnData 的 4 个字段
  ├─ 实现 UHodgeAbilitySet（授予/回收）
  └─ 实现 UHodgeGameplayCueManager

阶段 D：战斗闭环 + 联机验证
  ├─ 第一个 GameplayAbility（近战攻击）
  ├─ CombatComponent：Hit 检测 → GE 伤害 → 死亡
  └─ Dedicated Server + 2 Client 验证同步

阶段 E：内容层
  ├─ 建 GameFeature 插件，验证热插拔
  └─ Equipment / Inventory 三段式模型
```

---

## 8. 代码规范与约定

### 8.1 命名

| 类别 | 前缀 | 示例 |
|---|---|---|
| UObject 派生类 | 引擎前缀 + `Hodge` | `UHodgePawnData`、`AHodgeHeroCharacter` |
| Blueprint 资产 | 类型前缀 | `DA_`（DataAsset）、`IMC_`、`IA_`、`GA_`、`GE_`、`WBP_`、`B_`（蓝图类） |
| 模块导出宏 | `HODGEPODGE_API` | |

> **历史遗留**：类名曾从 `Hodgepodge` 前缀改名为 `Hodge` 前缀。`DefaultEngine.ini` 的 `[CoreRedirects]` 段保留了全部重命名映射，**不要删除**，否则旧蓝图资产会失效。

### 8.2 Base / Concrete 分层（本次重构引入）

这是 Lyra 的重要模式，新写代码请遵守：

| 层 | 职责 | 例子 |
|---|---|---|
| **Base 层** | 只放生命周期扩展点和跨项目通用机制，**不放具体业务逻辑** | `AHodgeGameStateBase`（3 个生命周期方法）、`AHodgePlayerStateBase`（ModularGameplay Receiver 封装） |
| **Concrete 层** | 放具体游戏逻辑 | `AHodgeGameState`（Experience + 全局 ASC）、`AHodgePlayerState`（PawnData + 玩家 ASC） |

好处：Base 层可以随时被替换或复用，Concrete 层随便改不影响底层。

### 8.3 目录与文件

- `Public/Private` 严格镜像，一个 `.h` 对应一个同路径 `.cpp`
- include 用**完整相对路径**（`#include "Character/HodgeHeroCharacter.h"`）
- 每个 `.cpp` 顶部用 `#include UE_INLINE_GENERATED_CPP_BY_NAME(ClassName)` 加速编译

### 8.4 注释

新写的代码统一用**中文 Doxygen 风格**（`@file` / `@brief` / `@param` / `@return`）。从 Lyra 拷贝的代码保留原版英文注释，再叠加中文说明。

> ⚠️ 部分注释存在**机器翻译痕迹**（`Actor`→"演员"、`GameplayAbility`→"能力"、`InputAction`→"输入动作" 中英混排）。看到不要困惑，逐步修正即可。

### 8.5 网络编程

- **一切默认服务器权威**。永远不要在客户端直接改属性（`HP -= 50` 是错的），走 `GameplayEffect`
- 新增可复制属性记得在 `GetLifetimeReplicatedProps` 里注册
- **Push Model**：本项目 PlayerState 已启用（`bIsPushBased = true`）。修改属性前必须 `MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, 属性名, this)`，否则**客户端收不到更新**。这是很容易踩的坑
- 数组/容器复制优先用 `FFastArraySerializer`，不要直接复制 `TArray`

### 8.6 异步依赖

统一用 **"Register + 若已完成立即回调"** 模式，项目里已有三处范例：

```cpp
// UHodgeExperienceManagerComponent
CallOrRegister_OnExperienceLoaded_HighPriority(...)
// UHodgeLocalPlayerBase
CallAndRegister_OnPlayerControllerSet(...)
// AHodgePlayerState::PreInitializeComponents
ExperienceComponent->CallOrRegister_OnExperienceLoaded(...)
```

新写异步系统时照抄这个模式，不要让调用方自己判断"是不是已经初始化完了"。

---

## 9. 新人上手路径

按 `LYRA_LEARNING_GUIDE.md` 的经验，**不要从 UI 或玩法目录开始读**，从框架层开始性价比最高。

**两份 Lyra 文档怎么配合用**（都是讲 Lyra 原版，把 `Lyra` 前缀换成 `Hodge` 即可对应到本项目）：

| 文档 | 回答什么问题 | 什么时候读 |
|---|---|---|
| [`LYRA_LEARNING_GUIDE.md`](LYRA_LEARNING_GUIDE.md) | **学什么**、按什么顺序学、哪些重要 | 规划学习路线时 |
| [`LYRA_RUNTIME_FLOW.md`](LYRA_RUNTIME_FLOW.md) | **怎么跑**、执行顺序、各系统生命周期、调试技巧 | 读 Experience / Init State 代码时，边读边对照 |

> 读 [§6.2 Experience](#62-experience-系统--c-层已接通资产层缺失) 和 [§6.6 Init State 链](#66-modulargameplay-与-init-state-链--半成品) 时，强烈建议把 `LYRA_RUNTIME_FLOW.md` 的第 4~7 章打开对照着看 —— 本项目这两块基本是 Lyra 的直接移植，原版讲得更细。

### 9.1 第一天：建立整体认知（约 5 小时）

| # | 文件 | 时间 | 收获 |
|---|---|---|---|
| 1 | `Data/HodgeAssetManagerStartupJob.h` | 15 min | 零依赖，理解启动加载进度系统 |
| 2 | `Data/HodgeExperienceDefinition.h` | 10 min | 理解数据驱动 |
| 3 | `Component/HodgeExperienceManagerComponent.cpp` | 1.5 h | **核心中的核心**，逐行走一遍状态机（对照 Runtime Flow 第 5 章） |
| 4 | `Private/Core/GameMode/HodgeGameModeBase.cpp` | 1.5 h | Experience 如何被选中、如何触发 Pawn 生成（对照第 4 章） |
| 5 | `Data/HodgePawnData.h` + `AbilitySystem/HodgeGameplayTags.h` | 30 min | 数据契约与 Tag 体系 |

然后按 [§4.3](#43-首次运行检查清单) 把项目跑起来 —— **当前跑不起来，先修复阻塞项本身就是最好的学习**。

### 9.2 第一周：玩家状态与角色

| 天 | 内容 | 目标问题 |
|---|---|---|
| 1-2 | `Core/PlayState/HodgePlayerState.h/.cpp` + `HodgePlayerStateBase.h/.cpp` | Base/Concrete 分层各自负责什么？PawnData 什么时候被设置？ |
| 3-4 | `Core/GameState/HodgeGameState.h/.cpp` | 为什么需要游戏级 ASC？ExperienceManagerComponent 挂在哪？ |
| 5 | `AbilitySystem/GameplayTagStack.h` | FastArray 增量复制相比直接复制 TArray 好在哪？ |
| 6-7 | [§6.6](#66-modulargameplay-与-init-state-链--半成品) 的断裂点 | 为什么 ASC 的 Avatar 可能是 nullptr？Lyra 是怎么解决的？ |

### 9.3 第一个月：参与修复与扩展

- 完成 [§7.4 阶段 A / B](#74-路线图建议顺序)：跑通项目 + 修复 GAS 初始化
- 补 `UHodgePawnExtensionComponent`（这是理解 Lyra Init State 链最好的实践）
  - 照抄对象：`LYRA_RUNTIME_FLOW.md` 第 7 章（四个状态、准入条件 `CanChangeInitState`、协作式推进机制）和第 8.1 节（ASC 初始化的 `InitializeAbilitySystem`）
  - 本项目缺的正是这个组件，补全后 [§6.8](#68-gas--初始化链路存在断裂) 的断裂点会自动解决
- 读 `Plugins/ALS-Refactored-4.15/` 的 `AAlsCharacter` 和 `UAlsAnimationInstance`

### 9.4 前置知识自检

有任意一项完全没接触过，先补，否则读代码会很吃力：

| 知识点 | 重要度 | 说明 |
|---|---|---|
| **GAS** | 🔴 极高 | ASC / GameplayAbility / GameplayEffect / AttributeSet / GameplayCue / GameplayTag |
| **网络复制基础** | 🔴 极高 | RPC、属性复制、`OnRep`、**Push Model**、FastArray、服务端权威 |
| **GameplayTag** | 🔴 极高 | 输入通道、状态标记、能力分类，"万能胶水" |
| **Enhanced Input** | 🟠 高 | InputAction / InputMappingContext / Trigger |
| **AssetManager / 软引用 / PrimaryDataAsset** | 🟠 高 | 资源加载体系 |
| **ModularGameplay / GameFrameworkComponentManager** | 🟠 高 | 本次重构大量使用，必须懂 |
| **GameFeatures** | 🟡 中 | 边用边学 |
| **ALS** | 🟡 中 | locomotion 层，用到了再深入 |

### 9.5 进度自查表

- [ ] 能画出 Experience 的 7 个状态流转图
- [ ] 能说出 Experience 的 7 级选择优先级
- [ ] 能解释为什么 Pawn 要等 Experience 加载完才生成
- [ ] 能说出 `GetPawnDataForController` 的三级回退顺序
- [ ] 能解释 Base / Concrete 分层的好处
- [ ] 能指出当前 ASC Avatar 断裂的原因和修复方向
- [ ] 能解释 HealthSet 里 Damage 为什么是 Meta 属性
- [ ] 能说出 Push Model 下改属性必须做什么
- [ ] 能解释 GameplayTagStack 为什么用 FastArray 而不是直接复制 TArray

---

## 10. 调试与验证工具箱

### 10.1 控制台命令

| 命令 | 作用 |
|---|---|
| `Hodge.DumpLoadedAssets` | 列出 AssetManager 常驻内存的所有资源（查内存泄漏） |
| `Hodge.chaos.ExperienceDelayLoad.MinSecs 2` | 人为延迟 Experience 加载 2 秒，便于观察中间状态 |
| `Hodge.chaos.ExperienceDelayLoad.RandomSecs 3` | 随机延迟 0~3 秒 |

### 10.2 启动参数

| 参数 | 作用 |
|---|---|
| `-LogAssetLoads` | 打印每个资源的同步加载耗时 |
| `-Experience=<Name>` | 指定要加载的 Experience（绕过 WorldSettings/默认） |

### 10.3 日志

⚠️ 项目**没有独立 Log Category**，全部走 `LogTemp`。`HodgeLogChannels.h` **文件并不存在**（只在 `HodgeExperienceManagerComponent.cpp` 里留了一行注释掉的 `#include`），需要新建。

在此之前，调试靠断点 + `LogTemp` 过滤。几处有用的诊断输出：

| 日志 | 位置 | 用途 |
|---|---|---|
| `Identified experience %s (Source: %s)` | `OnMatchAssignmentGiven` | **确认 Experience 选择结果**，排查启动问题第一步 |
| `Failed to identify experience, loading screen will stay up forever` | `OnMatchAssignmentGiven` | Experience Id 无效 |
| `EXPERIENCE: Wanted to use %s but couldn't find it` | `HandleMatchAssignmentIfNotExpectingOne` | 指定的 Experience 找不到，已回退默认 |
| `OW GameInstance Init` | `UHodgeGameInstanceBase::Init` | 确认 GameInstance 初始化 |
| `[HodgeHeroCharacter] SetupPlayerInputComponent called...` | `SetupPlayerInputComponent` | 输入不生效时先看这个 |

### 10.4 关键断点位置

| 想知道什么 | 在哪打断点 |
|---|---|
| 最终选定了哪个 Experience | `AHodgeGameModeBase::HandleMatchAssignmentIfNotExpectingOne()` |
| Experience 何时开始加载 | `AHodgeGameModeBase::OnMatchAssignmentGiven()` |
| Bundle 加载了什么 | `UHodgeExperienceManagerComponent::StartExperienceLoad()` |
| 插件何时激活 | `UHodgeExperienceManagerComponent::OnExperienceLoadComplete()` |
| Actions 何时执行 | `UHodgeExperienceManagerComponent::OnExperienceFullLoadCompleted()` |
| Pawn 何时生成 | `AHodgeGameModeBase::OnExperienceLoaded()` |
| Pawn 用哪个类 | `AHodgeGameModeBase::GetDefaultPawnClassForController_Implementation()` |
| PawnData 何时设置到 PlayerState | `AHodgePlayerState::SetPawnData()` |
| ASC 何时初始化（**查 Avatar 断裂**） | `AHodgePlayerState::PreInitializeComponents()` |
| GameData 何时加载 | `UHodgeAssetManager::LoadGameDataOfClass()` |
| 伤害如何结算 | `UHodgeHealthSet::PostGameplayEffectExecute()` |
| 输入绑定是否执行 | `AHodgeHeroCharacter::SetupPlayerInputComponent()` |

### 10.5 推荐实验

1. **建两个 Experience 资产，用 `-Experience=` 切换**，观察 PawnData 不同导致的角色行为变化 —— 这是理解 Experience 价值最快的方式
2. **用 `Hodge.chaos.ExperienceDelayLoad.MinSecs 5`** 人为拖慢加载，观察状态机的中间状态
3. **给 `AHodgePlayerState` 加一个 StatTag**，在服务端 `AddStatTagStack`，客户端验证复制
4. **在 `AHodgePlayerState::PreInitializeComponents` 打断点**，看 `GetPawn()` 是不是 nullptr —— 亲眼看一次断裂点

### 10.6 诊断"Pawn 没生成 / 卡住"的标准流程

修好阻塞项之后如果 Pawn 还是不出现，按这个顺序排查（改编自 `LYRA_RUNTIME_FLOW.md` 第 9.4 节）：

| 步骤 | 查什么 | 怎么看 |
|---|---|---|
| 1 | Experience 找到了吗？ | 日志搜 `Identified experience`，看有没有、`Source:` 是什么 |
| 2 | 加载卡在哪个状态？ | 日志搜 `EXPERIENCE:`，看走到 `StartExperienceLoad` / `OnExperienceLoadComplete` 的哪一步 |
| 3 | 插件名写错了吗？ | 日志搜 `Failed to find plugin URL from PluginName` |
| 4 | PawnData 有了吗？ | 断点 `AHodgePlayerState::SetPawnData()` |
| 5 | Pawn 类对不对？ | 断点 `AHodgeGameModeBase::GetDefaultPawnClassForController_Implementation()` |
| 6 | ASC 的 Avatar 是 nullptr 吗？ | 断点 `AHodgePlayerState::PreInitializeComponents()`，看 `GetPawn()` |

> 第 6 步当前**必然是 nullptr**，这是已知断裂点（[§6.8](#68-gas--初始化链路存在断裂)），不是你配置错了。

---

## 11. 已知问题与技术债

按影响面排序。改这里的东西前先看一眼。

### 11.1 🚫 阻塞级（不解决无法启动）

| 问题 | 位置 |
|---|---|
| **没有任何 Experience 资产** | 全 `Content/` 搜索 `*Experience*` 为 0 个结果，但 GameMode 硬编码回退到 `B_HodgeDefaultExperience` |
| **`HodgeExperienceDefinition` 未注册扫描项** | `Config/DefaultGame.ini`。导致 `GetPrimaryAssetPath` 无效 → `TryLoad()` 返回 nullptr → `check(AssetClass)` 崩溃 |

### 11.2 🔴 功能性缺陷

| 问题 | 影响 | 位置 |
|---|---|---|
| **ASC 的 Avatar 大概率是 nullptr** | 依赖 Avatar 的 GAS 功能会异常 | `AHodgePlayerState::PreInitializeComponents` 用 `GetPawn()`，此时 Pawn 未生成 |
| **`UHodgePawnExtensionComponent` 不存在** | Init State 链无节点、Pawn 无法接收 PawnData、AbilitySet 无法授予 | 仅在注释中出现于 GameMode / PlayerState / AddInputBinding |
| **Init State 链无组件参与** | 4 个 InitState Tag 注册了但不会起作用 | 无 `RegisterInitStateFeature`、无 `IGameFrameworkInitStateInterface` |
| **`HodgeHeroCharacter::PossessedBy` / `OnRep_PlayerState` 是空块** | 重构中间态，死代码 | `Private/Character/HodgeHeroCharacter.cpp:388-414` |
| **`AHodgeCombatCharacter` 的 ASC 是孤儿** | 持有但从不初始化，`GetAbilitySystemComponent()` 会返回它，误导调用方 | `InitAbilityActorInfo` 是注释状态 |
| **属性集不再是配置驱动** | 原来通过 `AttributeSetClasses` 数组配置，现在 `CreateDefaultSubobject` 写死 | `AHodgePlayerState` 构造函数 |
| `UHodgePawnData` 只剩 1 个字段 | 数据驱动能力基本失效 | `Data/HodgePawnData.h` |
| `InitializeGameplayCueManager()` 空实现 | GameplayCue 不预加载，首次触发卡顿 | `Data/HodgeAssetManager.cpp` |
| `HodgePlayerState::OnRep_PawnData()` 是空的 | PawnData 复制到客户端后什么都不做 | `Private/Core/PlayState/HodgePlayerState.cpp:243` |
| `HodgePlayerState::OnRep_MyTeamID()` / `OnRep_MySquadID()` 是空的 | 队伍/小队变化无响应 | 同上 |
| `AddInputMappings` / `RemoveInputMappings` 空实现 | IMC 管理在 `NotifyControllerChanged` 里手工做 | `Input/HodgeInputComponentBase.cpp` |

### 11.3 配置与命名

| 问题 | 说明 |
|---|---|
| `GlobalDefaultGameMode` 用旧类名 | `/Script/Hodgepodge.HodgepodgeGameModeBase`，靠 `[CoreRedirects]` 生效 |
| `GameInstanceClass` 用旧类名 | `/Script/Hodgepodge.HodgepodgeGameInstanceBase` |
| 资产名拼写错误 | `DA_Dafult_GameData` / `DA_DafultPawnData`（`Dafult` 应为 `Default`），已写进 `DefaultGame.ini`，改名要同步 |
| GameData 资产重复 | `Content/Main/Data/` 下同时有 `DA_Dafult_GameData` 和 `DA_DafultGameData` |
| 目录名拼写错误 | `Core/PlayState/` 应为 `Core/PlayerState/` |
| `UNTLink` 插件启用但不存在 | `.uproject` 里启用，项目内搜不到任何文件 |

### 11.4 安全隐患

| 问题 | 说明 |
|---|---|
| **硬编码调试密钥** | `UHodgeGameInstanceBase::Init()` 里 `DebugTestEncryptionKey` 用固定递增数据填充。源码注释已标注"正式项目中不能使用"，但记得在发布前移除 |

### 11.5 代码卫生

| 问题 | 说明 |
|---|---|
| 日志全部用 `LogTemp` | 无独立 Log Category，`HodgeLogChannels.h` 不存在 |
| 注释机器翻译痕迹 | `Actor`→"演员"、`GameplayAbility`→"能力" 等中英混排 |
| 死代码未清理 | `HodgeEnemyCharacter` 大段注释、`MotionWarpingComponent` 注释、`HostDedicatedServerMatch` 整段注释 |
| Lyra 遗留 Tag | `HodgeGameplayTags.h` 里有 `Lyra_*`、`ShooterGame_*` 前缀的 Tag，应改名或删除 |
| `// 111屎山代码来袭` | 多个文件顶部的自嘲注释，无害但会随时间失效 |

### 11.6 明确的设计取舍（不是 bug）

| 取舍 | 理由 |
|---|---|
| 用 ALS 而非自研 locomotion | 自研高质量 locomotion 成本极高，先借用 |
| 不引入 CommonUI / UIExtension / GameSettings / CommonUser | 体量大、非核心矛盾，延后 |
| 保留 5 个注释掉的 Lyra GameFeatureAction | 作为参考实现，启用前需先补依赖 |
| 走 Init State 链而非简单双入口 | 依赖会越来越多（PawnData / InputConfig / AbilitySet），双入口会失控。当前处于迁移中途 |

---

## 12. 附录：文件速查索引

### 12.1 架构核心（必读）

```
Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h              启动任务（零依赖，先读这个）
Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h              Experience 数据结构
Source/Hodgepodge/Private/Component/HodgeExperienceManagerComponent.cpp  状态机实现 ★★★
Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp            Experience 接入流程 ★★★
Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp            玩家状态主体 ★★★
Source/Hodgepodge/Private/Core/GameState/HodgeGameState.cpp              Experience 宿主 + 全局 ASC
Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp    Init State 链注册
```

### 12.2 小而美的文件（值得精读）

```
Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h      ~46 行，进度系统设计
Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h        FastArray 增量复制范例
Source/Hodgepodge/Public/Input/HodgeInputConfig.h                Tag → InputAction 数据驱动
Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h CallAndRegister 模式范例
Source/Hodgepodge/Public/Interface/LoadingProcessInterface.h     极简接口设计
```

### 12.3 配置文件

```
Config/DefaultEngine.ini     AssetManagerClassName、GlobalDefaultGameMode（旧类名）、
                             GameInstanceClass（旧类名）、[CoreRedirects]（勿删）
Config/DefaultGame.ini       [/Script/HodgePodge.HodgeAssetManager] 数据路径、
                             PrimaryAssetTypesToScan ★★ 缺 Experience 扫描项（阻塞项）
Config/DefaultInput.ini      Enhanced Input 按键映射
```

### 12.4 类继承关系速查

```
UAssetManager
└── UHodgeAssetManager

UGameInstance
└── UHodgeGameInstanceBase                 （注册 Init State 链）

AGameModeBase
└── AHodgeGameModeBase                     （Experience 全流程）

AGameStateBase
└── AHodgeGameStateBase                    （纯生命周期基类）
    └── AHodgeGameState (+ IAbilitySystemInterface)
                                            ExperienceManagerComponent + 全局 ASC

APlayerState
└── AHodgePlayerStateBase                  （ModularGameplay Receiver 封装）
    └── AHodgePlayerState (+ IAbilitySystemInterface)
                                            PawnData + 玩家 ASC + HealthSet + StatTags

APlayerController
└── AHodgePlayerControllerBase             （事件桥梁）

ULocalPlayer
└── UHodgeLocalPlayerBase                  （三套 CallAndRegister 委托）

ACharacter → AAlsCharacter
└── AHodgeCharacterBase
    └── AHodgeCombatCharacter (+ IAbilitySystemInterface)  ← 持有未初始化的 ASC
        ├── AHodgeHeroCharacter
        └── AHodgeEnemyCharacter

UActorComponent
└── UHodgeActorComponentBase
    ├── UHodgeCombatComponentBase
    ├── UHodgeInteractionComponentBase
    └── UHodgeMovementComponentBase

UEnhancedInputComponent
└── UHodgeInputComponentBase

UGameStateComponent + ILoadingProcessInterface
└── UHodgeExperienceManagerComponent

UAbilitySystemComponent
└── UHodgeAbilitySystemComponentBase

UAttributeSet
└── UHodgeAttributeSet
    └── UHodgeHealthSet

UGameplayAbility
└── UHodgeGameplayAbilityBase              （空壳）

UPrimaryDataAsset
├── UHodgeGameData
├── UHodgePawnData
├── UHodgeExperienceDefinition
└── UHodgeExperienceActionSet

UDataAsset
└── UHodgeInputConfig

UDefaultGameFeaturesProjectPolicies
└── UHodgeGameFeaturePolicy

FFastArraySerializer
└── FGameplayTagStackContainer              （Tag + 数量，增量复制）
```

### 12.5 参考文档

> 前两份是 **Lyra 原版**的学习文档（讲 Lyra 自己怎么跑），不是本项目文档。但本项目大量直接移植 Lyra，把类名前缀 `Lyra` 换成 `Hodge` 基本就能对应上，价值极高。

| 文档 | 内容 | 本项目对应关系 |
|---|---|---|
| [`LYRA_LEARNING_GUIDE.md`](LYRA_LEARNING_GUIDE.md) | Lyra 架构学习指南：学什么、按什么顺序学、哪些重要。**新系统设计的第一参考**，第 4 章的十大理念是本项目的设计宪法 | 全部理念的来源 |
| [`LYRA_RUNTIME_FLOW.md`](LYRA_RUNTIME_FLOW.md) | Lyra 运行时执行链路：Experience 决策链路、加载状态机、完整启动时序、Pawn Init State 链、各系统生命周期、调试技巧 | [§6.2](#62-experience-系统--c-层已接通资产层缺失) Experience（第 4~6 章）<br>[§6.6](#66-modulargameplay-与-init-state-链--半成品) Init State 链（第 7 章）<br>[§6.8](#68-gas--初始化链路存在断裂) ASC 初始化（第 8.1 节） |
| [`UE5 开放世界动作 RPG 架构方案 V2.md`](UE5%20开放世界动作%20RPG%20架构方案%20V2.md) | 本项目的总体方案与 Phase 划分，含角色职责、组件设计、DS 路线 | [§7 当前进度](#7-当前进度) 的 Phase 依据 |
| ALS 插件 README | `Plugins/ALS-Refactored-4.15/ALS-Refactored-4.15/README.md`，含引擎版本对照表 | [§2.2](#22-插件清单) |

---

*本 README 基于 UE 5.5 + ALS-Refactored 4.15 + Hodgepodge 当前代码状态（提交 `b8e708f` 之后）整理。项目处于活跃的 Lyra 化重构中，**顶部的阻塞项和 [§7 进度表](#7-当前进度)请优先关注并定期更新**。*
