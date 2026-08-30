# Hodgepodge

> 一个基于 **UE 5.5** 的个人游戏框架，目标是"ALS 的 locomotion + Lyra 的架构理念 + GAS 的战斗"，最终形态是 **UE Dedicated Server + 数据驱动的动作 RPG 底层框架**。

| 项目 | 值 |
|---|---|
| 引擎版本 | Unreal Engine **5.5** |
| 主模块 | `Hodgepodge`（Runtime，单模块） |
| 代码规模 | `Source/Hodgepodge` 共 83 个文件（41 `.h` + 41 `.cpp` + 1 `.cs`） |
| 核心依赖 | ALS-Refactored 4.15、GameplayAbilities、GameFeatures、EnhancedInput |
| 项目阶段 | **框架搭建期**（Phase 1~2 之间，跑得动但玩法未成型） |
| 相关文档 | [`LYRA_LEARNING_GUIDE.md`](LYRA_LEARNING_GUIDE.md)、[`UE5 开放世界动作 RPG 架构方案 V2.md`](UE5%20开放世界动作%20RPG%20架构方案%20V2.md) |

> **给新人的一句话**：这个项目跑得起来，但 **Experience 系统是"写好却没通电"的状态**（数据层 + 状态机完成，未接入 GameMode）。读代码前先看 [§7 当前进度](#7-当前进度)，能省下大量困惑。

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

- **不是游戏** —— 目前没有可玩的玩法循环，没有 UI，没有关卡内容
- **不是 Lyra 的复制品** —— 只吸收理念，**不整体照搬代码**（Lyra 耦合了 CommonUI / CommonUser / GameSettings 等一堆子系统，本项目目前一个都没引入）
- **不是最佳实践的终点** —— 源码里有大量 `// TODO` 和整体注释掉的死代码，这是**故意保留的参考素材**，见 [§11 技术债](#11-已知问题与技术债)

### 1.3 从 Lyra 借来的六条设计信条

这些是本项目的"宪法"。写任何新系统前，先问自己是否符合它们。完整推导见 [`LYRA_LEARNING_GUIDE.md` 第 4 章](LYRA_LEARNING_GUIDE.md#4-lyra-的十大核心设计理念)。

| # | 信条 | 本项目对应实现 | 状态 |
|---|---|---|---|
| 1 | **一套代码，多种玩法** —— 用数据 + 插件组合替代 GameMode 继承 | `UHodgeExperienceDefinition` | 🚧 数据层完成，未接入 GameMode |
| 2 | **数据驱动** —— 玩法配置全部是数据资产 | `UHodgePawnData` / `UHodgeGameData` / `UHodgeInputConfig` | 🚧 PawnData 只剩 1 个字段 |
| 3 | **插件化扩展** —— 不改主工程代码就能加玩法 | `UHodgeGameFeaturePolicy` + `GameFeatureAction_*` | 🚧 Policy 完成，Action 基本是死代码 |
| 4 | **GameplayTag 作万能胶水** —— 零编译依赖的模块间解耦 | `HodgeGameplayTags.h`（100+ 原生 Tag） | ✅ |
| 5 | **组合优于继承** —— 换数据胜过写子类 | 角色继承链 + `AttributeSetClasses` 数组 | ✅ |
| 6 | **服务器权威** —— 从第一天就按 Listen/Dedicated Server 写 | ASC 放 PlayerState、`Mixed` 复制模式、双初始化 | ✅ |

### 1.4 与 Lyra 的关键分歧

本项目**不是** Lyra 的移植，有三处刻意的路线差异，务必理解，否则会看不懂代码：

| 维度 | Lyra | Hodgepodge | 原因 |
|---|---|---|---|
| **Locomotion / Camera** | 自研 `LyraCharacterMovementComponent` + 相机模式栈 | 直接继承 **ALS** 的 `AAlsCharacter` / `UAlsCameraComponent` | 自研一套高质量 locomotion 的成本极高，ALS 在移动、动画、相机上已经做得足够好，先借用 |
| **ASC 归属** | `PlayerState` 持有，通过 `PawnExtensionComponent` + **Init State 链**协调 | `PlayerState` 持有，通过 `PossessedBy` / `OnRep_PlayerState` **双入口**直接初始化 | Init State 链是为"大量异步依赖"准备的；当前依赖只有 PlayerState 和 Character 两个，双入口足够，复杂度低一个数量级 |
| **UI / 设置 / 登录** | CommonUI + UIExtension + GameSettings + CommonUser 全家桶 | **全部没有** | 这些插件体量大、学习曲线陡，且不是框架的核心矛盾，延后引入 |

---

## 2. 技术栈与依赖

### 2.1 引擎与模块依赖

`Hodgepodge.uproject` 声明的模块与插件：

| 类别 | 名称 | 说明 |
|---|---|---|
| Module | `Hodgepodge` (Runtime) | 项目主模块，LoadingPhase = Default |
| AdditionalDependencies | `Engine` / `GameplayAbilities` / `EnhancedInput` / `ModularGameplay` | |
| Plugin | `GameFeatures` | Experience 系统的运行时依赖 |
| Plugin | `GameplayAbilities` | GAS |
| Plugin | `ModelingToolsEditorMode` | 仅 Editor |
| Plugin | `UNTLink` | ⚠️ 在 `.uproject` 里启用了，但项目内**搜不到该插件的任何文件**。不会致命报错，但建议确认后移除 |

`Source/Hodgepodge/Hodgepodge.Build.cs` 的实际依赖：

```csharp
PublicDependencyModuleNames:  Core, CoreUObject, Engine, InputCore,
                              GameplayAbilities, GameplayTags, GameplayTasks,
                              ALS, GameFeatures
PrivateDependencyModuleNames: ALSCamera, EnhancedInput
```

> **注意**：`ModularGameplay` 在 `.uproject` 里声明了，但 `Build.cs` **没有**加。目前代码里也没有用到 `GameFrameworkComponentManager`。如果后面要做组件热插拔，记得补上这个依赖。

### 2.2 插件清单

| 插件 | 版本 | 位置 | 说明 |
|---|---|---|---|
| **ALS-Refactored** | 4.15（UE5.5 对应版本） | `Plugins/ALS-Refactored-4.15/ALS-Refactored-4.15/` | Sixze 重写的 ALS V4 C++ 版，MIT 许可。4 个模块：`ALS`（Runtime）、`ALSCamera`（Runtime）、`ALSExtras`（Runtime 示例）、`ALSEditor`（UncookedOnly） |
| **RiderLink** | — | `Plugins/Developer/RiderLink/` | Rider IDE 联动插件，**不参与游戏逻辑** |

**ALS 核心类**（写代码时最常打交道的）：

| 类 | 位置 | 职责 |
|---|---|---|
| `AAlsCharacter` | `ALS/Public/AlsCharacter.h` | 主角色类。所有状态（LocomotionMode / RotationMode / Stance / Gait / ViewMode / OverlayMode）**用 GameplayTag 而非枚举**表示，天然可复制 |
| `UAlsCharacterMovementComponent` | `ALS/Public/AlsCharacterMovementComponent.h` | 为网络同步重写的移动组件 |
| `UAlsAnimationInstance` | `ALS/Public/AlsAnimationInstance.h` | 动画主线程实例，聚合 20+ 状态结构 |
| `UAlsCameraComponent` | `ALSCamera/Public/AlsCameraComponent.h` | 相机组件，注意它继承 `USkeletalMeshComponent`，**不需要自定义 PlayerCameraManager** |

> **重要**：ALS 的 Tag 命名空间是独立的（`AlsGaitTags::`、`AlsStanceTags::`、`AlsRotationModeTags::`、`AlsViewModeTags::` 等），和项目自己的 `HodgeGameplayTags` 是两套，不要混用。

### 2.3 未引入的 Lyra 插件

以下 Lyra 插件在 `LYRA_LEARNING_GUIDE.md` 里被标为"强烈推荐"，但**本项目目前一个都没有**。如果看到代码里 `#include` 它们，那一定是编译不过的死代码。

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
    ├── Public/                      ← 头文件（40 个）
    └── Private/                     ← 实现文件（41 个），与 Public 完全同构
```

`Public/Private` 镜像的九个子目录：

| 目录 | 文件数 | 职责 | 重要度 |
|---|---|---|---|
| `Data/` | 7 | AssetManager、GameData、PawnData、**Experience 三件套** | ★★★★★ |
| `Component/` | 5 | 组件基类 + ExperienceManagerComponent | ★★★★★ |
| `Core/` | 7 | GameInstance / GameMode / GameState / PlayerController / PlayerState / HUD / LocalPlayer | ★★★★★ |
| `Character/` | 4 | 角色继承链 | ★★★★★ |
| `AbilitySystem/` | 5 | ASC、Ability 基类、GameplayTags、AttributeSet、HealthSet | ★★★★ |
| `Input/` | 2 | InputConfig（数据）+ InputComponent（绑定） | ★★★★ |
| `GameFeatures/` | 8 | 7 个 GameFeatureAction + Policy | ★★（多数是死代码） |
| `Actor/` | 1 | Actor 基类 | ★ |
| `Interface/` | 1 | `ILoadingProcessInterface` | ★ |

> `Core/` 下有个历史遗留拼写错误：目录名是 `PlayState/`，应为 `PlayerState/`。**暂不修改**（改目录会动一堆 include 和 git 历史），但新代码请按正确拼写理解。

### 3.2 内容目录

```
Content/
├── Main/                    ★ 项目自有内容，新东西放这里
│   ├── Data/                DA_Dafult_GameData / DA_DafultPawnData / DA_DafultGameData
│   ├── Input/               DA_HodgeInputConfig / IMC_Default / IMC_UI / InputAction/(18)
│   └── Character/EnemyBase/
├── Assets/                  美术资源：Enemies / HeroCharacter / Weapons /
│                            Niagara / Sounds / Textures / Meshes / MaterialFunctions
├── Characters/              角色资源（145）
├── ALS/ ALSCamera/ ALSExtras/   插件自带内容，不要改
├── AdvancedLocomotionV4/    ALS V4 原始内容（历史遗留，345 个资产）
├── piaobozhe/ qiuyuan/      第三方角色资源（246 / 285 个资产）
└── ThirdPerson/             UE 模板内容
```

### 3.3 配置文件

| 文件 | 关键内容 |
|---|---|
| `Config/DefaultEngine.ini` | `AssetManagerClassName=/Script/Hodgepodge.HodgeAssetManager`、`GlobalDefaultGameMode`、`GameInstanceClass`、**`[CoreRedirects]` 类名重命名表** |
| `Config/DefaultGame.ini` | `[/Script/HodgePodge.HodgeAssetManager]` 的 GameData / PawnData 路径、`PrimaryAssetTypesToScan` |
| `Config/DefaultInput.ini` | Enhanced Input 的按键映射 |
| `Config/DefaultEditor.ini` | 编辑器设置 |

---

## 4. 快速开始

### 4.1 环境要求

| 项 | 要求 |
|---|---|
| 引擎 | Unreal Engine **5.5**（源码版或安装版均可） |
| IDE | Visual Studio 2022（Windows）或 Rider for Unreal |
| 平台 | Windows（当前只验证过 Win64，DX12） |
| 渲染 | 项目开启了 DX12 / SM6 / 虚拟阴影贴图 / Lumen（`r.DynamicGlobalIlluminationMethod=1`），显卡要求偏高 |

### 4.2 编译与运行

```powershell
# 1) 生成解决方案（引擎目录下的 BatchFiles，路径按你的安装位置调整）
& "D:\Epic\UE_5.5\Engine\Build\BatchFiles\GenerateProjectFiles.bat" `
    -projectfiles -project="e:\Project\Git\Hodgepodge\Hodgepodge.uproject" -game -engine

# 2) 编译（或用 IDE 打开 Hodgepodge.sln 编译 Development Editor | Win64）
& "D:\Epic\UE_5.5\Engine\Build\BatchFiles\Build.bat" `
    HodgepodgeEditor Win64 Development -project="e:\Project\Git\Hodgepodge\Hodgepodge.uproject"

# 3) 打开编辑器
& "D:\Epic\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe" `
    "e:\Project\Git\Hodgepodge\Hodgepodge.uproject"
```

### 4.3 首次运行检查清单

跑不起来 90% 是配置问题，按这个顺序查：

1. **`DefaultEngine.ini` 的 `AssetManagerClassName`** 必须是 `/Script/Hodgepodge.HodgeAssetManager`。
   错了会直接 `UE_LOG(Fatal)` 退出（`HodgeAssetManager.cpp` 的 `Get()` 里写死了这个检查）。

2. **`DefaultGame.ini` 的 `[/Script/HodgePodge.HodgeAssetManager]` 段**必须配好 GameData 路径：
   ```ini
   [/Script/HodgePodge.HodgeAssetManager]
   HodgeGameDataPath=/Game/Main/Data/DA_Dafult_GameData.DA_Dafult_GameData
   DefaultPawnData=/Game/Main/Data/DA_DafultPawnData.DA_Dafult_PawnData
   ```
   GameData 加载失败是 **Fatal**，不是 Warning。

3. **`PrimaryAssetTypesToScan`** 至少要包含 `HodgeGameData`（当前有）。
   > ⚠️ 目前**没有**注册 `HodgeExperienceDefinition` 的扫描项，这是 Experience 系统接不上的直接原因之一，见 [§11](#11-已知问题与技术债)。

4. **启动地图**：`GameDefaultMap=/Game/ALSExtras/Levels/L_Als_Grid`，`EditorStartupMap=/Game/ThirdPerson/Maps/ThirdPersonMap`。

5. **`GlobalDefaultGameMode` 用的是旧类名** `/Script/Hodgepodge.HodgepodgeGameModeBase`，靠 `DefaultEngine.ini` 的 `[CoreRedirects]` 生效。能跑，但建议改成 `HodgeGameModeBase`。

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
│  框架层    Core/（GameInstance / GameMode / GameState /        │
│           PlayerController / PlayerState / LocalPlayer / HUD） │
│           Data/（AssetManager + Experience 三件套）            │
├──────────────────────────────────────────────────────────────┤
│  基础层    Plugins: ALS-Refactored（ALS / ALSCamera /          │
│           ALSExtras）  +  引擎: GAS / EnhancedInput /          │
│           GameFeatures / AssetManager                          │
└──────────────────────────────────────────────────────────────┘
```

### 5.2 启动时序（当前实际）

```
引擎启动
  └─ UHodgeAssetManager::StartInitialLoading()
       ├─ Super::StartInitialLoading()          扫描 PrimaryAssetTypesToScan
       ├─ STARTUP_JOB(InitializeGameplayCueManager())   [当前是空实现]
       ├─ STARTUP_JOB_WEIGHTED(GetGameData(), 25.f)     同步加载 GameData
       └─ DoAllStartupJobs()                    权重进度 → 空实现

地图加载
  └─ AHodgeGameModeBase::InitGame()
       └─ SpawnDefaultPawn → AHodgeCharacterBase（ALS 角色）
            ├─ PossessedBy()            [服务端] → PS->InitializeAbilitySystemForCharacter(this)
            │                                      ├─ ASC->InitAbilityActorInfo(PS, Character)
            │                                      └─ 创建 AttributeSetClasses 里的属性集（仅一次）
            ├─ OnRep_PlayerState()      [客户端] → PS->InitializeAbilitySystemForCharacter(this)
            ├─ NotifyControllerChanged()          → 向本地玩家添加 InputMappingContext
            └─ SetupPlayerInputComponent()        → 按 InputTag 绑定 ALS 输入

Experience 流程     ❌ 当前未被调用（见 §6.2）
```

### 5.3 目标时序（Lyra 式，尚未接通）

作为对照，把 `LYRA_LEARNING_GUIDE.md` 里的目标流程贴在这里。**这是 Experience 系统接好之后应该有的样子**：

```
① 引擎启动 → AssetManager::StartInitialLoading() → 加载 GameData
② 地图加载 → GameMode::InitGame() → 延迟一帧 → HandleMatchAssignmentIfNotExpectingOne()
③ 确定 Experience（优先级：URL Options > DeveloperSettings(仅PIE) > 命令行 > WorldSettings > 硬编码默认）
④ → ExperienceManagerComponent->SetCurrentExperience(Id)
⑤ → StartExperienceLoad()                              [状态: Loading]
⑥ → ChangeBundleStateForPrimaryAssets()                异步加载 Bundle 资源
⑦ → OnExperienceLoadComplete() → 收集 GameFeature 插件   [状态: LoadingGameFeatures]
⑧ → 逐个 LoadAndActivateGameFeaturePlugin() → 计数归零
⑨ → OnExperienceFullLoadCompleted() → 执行 Actions      [状态: ExecutingActions → Loaded]
⑩ → 广播三档委托：HighPriority → Normal → LowPriority
⑪ → GameMode::OnExperienceLoaded() → RestartPlayer()    ← 到这一步才生成 Pawn
```

**关键洞察**：Pawn 的生成要推迟到 Experience 加载完成之后，因为只有那时才知道用哪个 PawnData、授予哪些能力。

---

## 6. 核心系统详解

### 6.1 AssetManager ✅ 已完成度最高

`Data/HodgeAssetManager.h/.cpp`

整个项目完成度最高的部分，基本是 Lyra `LyraAssetManager` 的等价实现。

| 能力 | 说明 |
|---|---|
| **StartupJob 权重进度系统** | `STARTUP_JOB(func)` / `STARTUP_JOB_WEIGHTED(func, weight)` 宏注册任务，按权重换算总体进度 |
| **GameData 类型化缓存** | `GameDataMap` 以 Class 为 Key，不同类型 GameData 分别缓存，避免重复加载 |
| **软引用同步加载** | `GetAsset<T>()` / `GetSubclass<T>()`，`bKeepInMemory` 控制是否常驻 |
| **常驻资源池** | `LoadedAssets` + `FCriticalSection` 持强引用，防止 GC |
| **PIE 预加载** | `PreBeginPIE()` 里确保进入 PIE 前 GameData 已就绪 |
| **调试命令** | `Hodge.DumpLoadedAssets` |

**零依赖的必读文件**：`Data/HodgeAssetManagerStartupJob.h`

这是全项目最值得先读的文件 —— 46 行左右、零依赖、把"多个异步加载 → 按序回调 → 汇报加权进度"的样板完整封装了一遍。理解它，就理解了整个启动加载体系。

```cpp
// 用法示意（HodgeAssetManager.cpp）
STARTUP_JOB(InitializeGameplayCueManager());        // 权重 1
STARTUP_JOB_WEIGHTED(GetGameData(), 25.f);          // 权重 25
```

**空实现（待办）**：

```cpp
void UHodgeAssetManager::InitializeGameplayCueManager()
{
    // UHodgeGameplayCueManager* GCM = UHodgeGameplayCueManager::Get();
    // check(GCM);
    // GCM->LoadAlwaysLoadedCues();
}
void UHodgeAssetManager::UpdateInitialGameContentLoadPercent(float) { /* 预留 */ }
```

### 6.2 Experience 系统 🚧 数据层 + 状态机完成，未接入流程

这是本项目的架构核心，也是**最需要补齐的一块**。

#### 已有的三个数据资产

| 类 | 文件 | 内容 |
|---|---|---|
| `UHodgeExperienceDefinition` | `Data/HodgeExperienceDefinition.h` | `GameFeaturesToEnable[]`、`DefaultPawnData`、`Actions[]`（Instanced）、`ActionSets[]` |
| `UHodgeExperienceActionSet` | `Data/HodgeExperienceActionSet.h` | 可复用的一组 `Actions[]` + `GameFeaturesToEnable[]` |
| `UHodgePawnData` | `Data/HodgePawnData.h` | **目前只剩 `PawnClass` 一个字段**，其余 4 个全被注释 |

#### 加载状态机

`Component/HodgeExperienceManagerComponent.h/.cpp` 挂在 GameState 上（继承 `UGameStateComponent` + `ILoadingProcessInterface`）。

```
Unloaded → Loading → LoadingGameFeatures → LoadingChaosTestingDelay
                                                    ↓
                                          ExecutingActions → Loaded → Deactivating → Unloaded
```

| 方法 | 职责 |
|---|---|
| `SetCurrentExperience(FPrimaryAssetId)` | 入口，服务端调用，`CurrentExperience` 通过 `DOREPLIFETIME` 复制到客户端 |
| `OnRep_CurrentExperience()` | 客户端收到后自行启动加载 |
| `StartExperienceLoad()` | 状态 → Loading，收集 Bundle 资源，按客户端/服务端区分 Bundle 后异步加载 |
| `OnExperienceLoadComplete()` | 状态 → LoadingGameFeatures，收集插件 URL 并 `LoadAndActivateGameFeaturePlugin` |
| `OnGameFeaturePluginLoadComplete()` | 计数归零后进入下一步 |
| `OnExperienceFullLoadCompleted()` | 状态 → ExecutingActions → Loaded，执行 Actions 并按三档广播委托 |
| `EndPlay()` | 逆序停用插件 + 执行 Deactivating 流程 |
| `ShouldShowLoadingScreen()` | 给 Loading Screen 用（目前无消费者） |

**三档委托**（`CallOrRegister_OnExperienceLoaded_HighPriority` / `_` / `_LowPriority`）解决了异步依赖的经典问题：**"如果已经加载完就立即回调，否则注册等通知"**。这个模式在 `UHodgeLocalPlayerBase` 里也用了，是本项目的标准手法。

#### 缺的三块（要接通 Experience，必须补齐）

1. **`AHodgeGameModeBase` 没有触发加载** —— 缺 Lyra 的 `HandleMatchAssignmentIfNotExpectingOne()`、`OnExperienceLoaded()`、`GetPawnDataForController()`，也没有延迟生成 Pawn。
2. **`AHodgeGameStateBase` 没挂 `UHodgeExperienceManagerComponent`** —— 组件写了但没人创建，所以 `SetCurrentExperience()` **从未被调用**。
3. **`DefaultGame.ini` 缺 `HodgeExperienceDefinition` 的 `PrimaryAssetTypesToScan`** —— 即使写了代码，AssetManager 也扫不到 Experience 资产。另外也缺 `LyraWorldSettings` 那样的 `UHodgeWorldSettings` 来指定地图默认 Experience。

`Data/HodgeExperienceManager.h/.cpp` 是 PIE 环境下的插件引用计数辅助（`NotifyOfPluginActivation` / `RequestToDeactivatePlugin`），**整个实现包在 `#if WITH_EDITOR` 里**，非编辑器下是空壳。

### 6.3 角色体系（ALS 接入）✅

```
ACharacter
    ↓
AAlsCharacter                    ← ALS 插件：移动、动画、状态机
    ↓
AHodgeCharacterBase              ← 生命周期扩展点，不依赖 GAS
    ↓
AHodgeCombatCharacter            ← + IAbilitySystemInterface，持有 ASC
    ↓
    ├── AHodgeHeroCharacter      ← 玩家：ALS 相机 + 输入 + GAS 双初始化入口
    └── AHodgeEnemyCharacter     ← 敌人：几乎空实现
```

| 类 | 文件 | 关键点 |
|---|---|---|
| `AHodgeCharacterBase` | `Character/HodgeCharacterBase.h` | 继承 `AAlsCharacter`，只提供 `PreInitializeComponents` / `BeginPlay` / `EndPlay` 扩展点。**刻意不放 GAS** |
| `AHodgeCombatCharacter` | `Character/HodgeCombatCharacter.h` | 实现 `IAbilitySystemInterface`，持有 `UHodgeAbilitySystemComponentBase`。`MotionWarpingComponent` 被注释待接 |
| `AHodgeHeroCharacter` | `Character/HodgeHeroCharacter.h` | 重载 `CreatePlayerInputComponent()` 返回 `UHodgeInputComponentBase`；`NotifyControllerChanged()` 管理 IMC 增删；`CalcCamera()` 委托给 ALS 相机组件 |
| `AHodgeEnemyCharacter` | `Character/HodgeEnemyCharacter.h` | 骨架，双手碰撞盒 / 血条 Widget / 战斗组件全部注释待实现 |

**ALS 实现的 12 个输入动作**（`HodgeHeroCharacter::SetupPlayerInputComponent`）：
Look、Move、Sprint、Walk、Crouch、Jump、Aim、Ragdoll、Roll、RotationMode、ViewMode、SwitchShoulder。

### 6.4 输入框架 ✅

两个文件，设计干净，值得作为模板。

```
UHodgeInputConfig  (DataAsset)          UHodgeInputComponentBase  (: UEnhancedInputComponent)
├─ NativeInputActions[]                 ├─ AddInputMappings / RemoveInputMappings
│   （FHodgeInputAction:                ├─ BindNativeAction<T>(Tag, TriggerEvent, Obj, Func)
│     InputAction + InputTag）          ├─ BindAbilityActions<T>(Config, Obj, Pressed, Released, Handles)
└─ AbilityInputActions[]                └─ RemoveBinds(Handles)
```

| 类 | 文件 | 说明 |
|---|---|---|
| `UHodgeInputConfig` | `Input/HodgeInputConfig.h` | 数据资产，`NativeInputActions` 手动绑，`AbilityInputActions` 自动绑 GAS |
| `FHodgeInputAction` | 同上 | `{ const UInputAction* InputAction; FGameplayTag InputTag; }`，Tag 限定 `Categories = "InputTag"` |
| `UHodgeInputComponentBase` | `Input/HodgeInputComponentBase.h` | 模板方法 `BindNativeAction` / `BindAbilityActions`，绑定句柄存进 `TArray<uint32>` 便于统一清理 |

内容资产：`Content/Main/Input/DA_HodgeInputConfig`、`IMC_Default`、`IMC_UI`、`InputAction/`（18 个）。

> `AddInputMappings` / `RemoveInputMappings` 目前是**空实现**。IMC 的增删实际在 `AHodgeHeroCharacter::NotifyControllerChanged()` 里手工做的，没走这两个方法。

### 6.5 GAS ✅ 骨架完成

| 类 | 文件 | 说明 |
|---|---|---|
| `UHodgeAbilitySystemComponentBase` | `AbilitySystem/HodgeAbilitySystemComponentBase.h` | 构造函数里 `SetIsReplicatedByDefault(true)` + `SetReplicationMode(EGameplayEffectReplicationMode::Mixed)` |
| `UHodgeAttributeSet` | `AbilitySystem/AttributeSet/HodgeAttributeSet.h` | 基类，定义 `ATTRIBUTE_ACCESSORS` 宏和 `FHodgeAttributeEvent` 六参委托，提供 `GetHodgeAbilitySystemComponent()` |
| `UHodgeHealthSet` | `AbilitySystem/AttributeSet/HodgeHealthSet.h` | Health / MaxHealth / **Healing** / **Damage**（Meta 属性）/ BaseDamage / BaseHeal |
| `UHodgeGameplayAbilityBase` | `AbilitySystem/HodgeGameplayAbilityBase.h` | 继承 `UGameplayAbility`，**目前是空壳**，等 Lyra 的激活策略、激活组、附加 Cost 搬过来 |
| `HodgeGameplayTags` | `AbilitySystem/HodgeGameplayTags.h` | 100+ 个 `UE_DECLARE_GAMEPLAY_TAG_EXTERN` 原生 Tag |

#### ASC 归属：PlayerState

`AHodgePlayerStateBase`（`Core/PlayState/`）持有 ASC，`NetUpdateFrequency = 100.f`。

```cpp
// Core/PlayState/HodgePlayerStateBase.cpp
void AHodgePlayerStateBase::InitializeAbilitySystemForCharacter(ACharacter* Avatar)
{
    if (!AbilitySystemComponent || !Avatar) return;

    if (AbilitySystemComponent->GetAvatarActor() != Avatar)
        AbilitySystemComponent->InitAbilityActorInfo(this, Avatar);

    if (HasAuthority() && !bASCInitialized)
    {
        for (TSubclassOf<UAttributeSet> AttrClass : AttributeSetClasses)
        {
            UAttributeSet* NewAS = NewObject<UAttributeSet>(AbilitySystemComponent, AttrClass);
            AbilitySystemComponent->AddAttributeSetSubobject(NewAS);
            AddedAttributeSets.Add(NewAS);
        }
        bASCInitialized = true;
    }
}
```

**双初始化入口**（Lyra 用 Init State 链解决的问题，这里用双入口简化）：

| 端 | 入口 | 时机 |
|---|---|---|
| 服务端 | `AHodgeHeroCharacter::PossessedBy()` | Controller 拥有角色时 |
| 客户端 | `AHodgeHeroCharacter::OnRep_PlayerState()` | PlayerState 复制到位时 |

`bASCInitialized` 用独立 bool 而非 `AddedAttributeSets.Num() == 0` 判断，因为**死亡重生 / 重新 Possess 时数组可能为空但 ASC 已初始化过**。这个细节别改掉。

**AttributeSet 是配置驱动的**：在 `AHodgePlayerStateBase::AttributeSetClasses`（`EditDefaultsOnly`）里配，服务端运行时动态创建。不需要写死在 C++ 里。

**HealthSet 的 Meta 属性设计**（来自 Lyra，必须理解）：

```
GE 施加伤害 → Damage（Meta 属性，一次性）
            → PostGameplayEffectExecute 里 Health -= Damage
            → Damage 清零，广播 OnHealthChanged
```

为什么不直接改 Health？因为要**在扣减前做统一处理**（死亡判定、护盾、溢出治疗转护盾等），并拿到完整的 `FGameplayEffectModCallbackData` 上下文（Instigator / Causer / Spec）。细节见 `LYRA_LEARNING_GUIDE.md` 阶段 3 的关键问题。

### 6.6 GameFeature 🚧 Policy 完成，Action 基本是死代码

`GameFeatures/HodgeGameFeaturePolicy.h` 实现了 3 个 Observer：

| 类 | 职责 |
|---|---|
| `UHodgeGameFeaturePolicy` | 继承 `UDefaultGameFeaturesProjectPolicies`，重写 `InitGameFeatureManager` / `GetPreloadAssetListForGameFeature` / `IsPluginAllowed` / `GetPreloadBundleStateForGameFeature` / `GetGameFeatureLoadingMode` |
| `UHodgeGameFeature_HotfixManager` | `IGameFeatureStateChangeObserver`，监听 `OnGameFeatureLoading` 处理 Hotfix |
| `UHodgeGameFeature_AddGameplayCuePaths` | 监听 `OnGameFeatureRegistering` / `Unregistering`，动态管理 GameplayCue 搜索路径 |

**7 个 GameFeatureAction 的真实状态**：

| 文件 | 状态 |
|---|---|
| `GameFeatureAction_WorldActionBase.h` | ✅ 可用（Lyra 原版，世界相关 Action 的基类） |
| `GameFeatureAction_SplitscreenConfig.h` | ✅ 可用（Lyra 原版） |
| `GameFeatureAction_AddAbilities.h` | ❌ **整体注释**（依赖 `ULyraAbilitySet`，本项目没有） |
| `GameFeatureAction_AddWidget.h` | ❌ **整体注释**（依赖 UIExtension 插件，本项目没有） |
| `GameFeatureAction_AddInputBinding.h` | ❌ **整体注释** |
| `GameFeatureAction_AddInputContextMapping.h` | ❌ **整体注释** |
| `GameFeatureAction_AddGameplayCuePath.h` | ❌ **整体注释**（功能已由 Policy 里的 Observer 覆盖） |

这 5 个注释掉的文件是**故意保留的 Lyra 参考实现**，不要删，但也不要指望它们能编译。想启用任何一 个，得先把对应的 Lyra 依赖（AbilitySet / UIExtension 等）补进来。

> 用 `Search` 搜 `^class HODGEPODGE_API` 在 `Public/GameFeatures/` 下只有 0 个结果，就能确认这一点。

### 6.7 其他框架类

| 类 | 文件 | 说明 |
|---|---|---|
| `AHodgeGameInstanceBase` | `Core/GameInstance/` | 游戏实例 |
| `AHodgeGameModeBase` | `Core/GameMode/` | 构造函数里配齐 5 个默认类；`ControllerCanRestart()` 供子类重写 |
| `AHodgeGameStateBase` | `Core/GameState/` | 服务器 FPS 复制（1 秒更新一次）、SeamlessTravel 清理 Bot/Inactive、Replay 录制者追踪 + 复制回调委托 |
| `AHodgePlayerControllerBase` | `Core/PlayerController/` | **事件桥梁**：把引擎回调（`ReceivedPlayer` / `SetPawn` / `OnPossess` / `OnUnPossess` / `OnRep_PlayerState`）转成 LocalPlayer 的多播委托 |
| `UHodgeLocalPlayerBase` | `Core/LocalPlayer/` | 三个 `CallAndRegister_On*Set` 委托 + `bIsPlayerViewEnabled` 视图开关 |
| `AHodgeHUDBase` | `Core/HUD/` | HUD 占位 |
| `AHodgeActorBase` | `Actor/` | Actor 基类 |
| `UHodgeActorComponentBase` | `Component/` | 组件基类：`BeginPlay` / `TickComponent` / `InitializeComponent` / `OnReady` 扩展点 + `IsServer()`。**默认开 Tick** |
| `UHodgeCombatComponentBase` | `Component/` | 战斗组件，**默认关 Tick**（事件驱动）；空实现 |
| `UHodgeInteractionComponentBase` | `Component/` | 交互组件；空实现 |
| `UHodgeMovementComponentBase` | `Component/` | 移动组件；空实现（实际移动用 ALS 的） |
| `ILoadingProcessInterface` | `Interface/` | `ShouldShowLoadingScreen(FString&)`，目前无消费者 |

---

## 7. 当前进度

对照 `UE5 开放世界动作 RPG 架构方案 V2.md` 的 Phase 划分。

### 7.1 已完成 ✅

| 模块 | 内容 |
|---|---|
| **工程骨架** | C++ 模块、`Build.cs` 依赖、Target 文件、Git 仓库 |
| **游戏框架类全家桶** | GameInstance / GameMode / GameState / PlayerController / PlayerState / HUD / LocalPlayer 全部就位，配置进 `DefaultEngine.ini` |
| **AssetManager** | StartupJob 权重进度系统、GameData 类型化缓存、软引用同步加载、常驻资源池、PIE 预加载、`DumpLoadedAssets` 命令 |
| **Experience 数据层** | `ExperienceDefinition` / `ExperienceActionSet` / `PawnData` 三个数据资产，含编辑器 `IsDataValid` 校验 |
| **Experience 状态机** | 7 状态机、Bundle 异步加载（区分客户端/服务端）、GameFeature 激活/停用、三档委托广播、Chaos 延迟测试钩子 |
| **GameFeaturePolicy** | Policy + HotfixManager + AddGameplayCuePaths 两个 Observer |
| **ALS 接入** | 角色继承链、移动组件、动画实例、相机组件、12 个 ALS 输入动作全部绑定 |
| **输入框架** | `HodgeInputConfig`（Tag 映射数据资产）+ `HodgeInputComponentBase`（模板绑定方法） |
| **GAS 骨架** | ASC（Mixed 复制）+ AttributeSet 基类 + HealthSet（Meta 属性 + Clamp）+ PlayerState 持有 ASC + 双初始化 + AttributeSet 配置驱动 |
| **GameplayTag 体系** | `HodgeGameplayTags.h` 100+ 原生 Tag，覆盖 Ability / InputTag / GameplayCue / Status / InitState / UI / Movement |
| **组件基类层级** | `ActorComponentBase` → Combat / Interaction / Movement |
| **LocalPlayer 事件系统** | PlayerController / PlayerState / Pawn 三套 `CallAndRegister` 委托 |

### 7.2 未完成 🚧❌

| 优先级 | 事项 | 说明 |
|---|---|---|
| 🔴 P0 | **Experience 接入 GameMode** | 缺 `HandleMatchAssignmentIfNotExpectingOne` / `OnExperienceLoaded` / 延迟生成 Pawn；GameState 未挂 ExperienceManagerComponent |
| 🔴 P0 | **注册 Experience 的 `PrimaryAssetTypesToScan`** | `DefaultGame.ini` 里只有 `HodgeGameData` 和 `GameFeatureData` |
| 🔴 P0 | **建 `UHodgeWorldSettings`** | 用于地图指定默认 Experience，并在 `DefaultEngine.ini` 配 `WorldSettingsClassName` |
| 🟠 P1 | **补 `UHodgePawnData` 字段** | AbilitySets / TagRelationshipMapping / InputConfig / DefaultCameraMode 四个字段全被注释 |
| 🟠 P1 | **实现 `UHodgeAbilitySet`** | 能力批量授予/回收 + `GrantedHandles` 统一回收（防能力泄漏）。这是 Lyra 理念 5，目前完全没有 |
| 🟠 P1 | **实现 `UHodgeGameplayCueManager`** | `InitializeGameplayCueManager()` 是空实现 |
| 🟠 P1 | **建立独立的 Log Category** | 现在全部用 `LogTemp`。`HodgeLogChannels.h` **文件本身并不存在**，只有 `HodgeExperienceManagerComponent.cpp` 里留了一行被注释的 `#include "HodgeLogChannels.h"`，需要新建 |
| 🟡 P2 | 解注释并实现 5 个 GameFeatureAction | 需先补 Lyra 依赖（AbilitySet / UIExtension / CommonUI） |
| 🟡 P2 | 建第一个 GameFeature 插件实例 | 目前 `Content/` 下没有任何 `.uplugin` |
| 🟡 P2 | 战斗系统 | `HodgeCombatComponentBase` 是空的；`HodgeEnemyCharacter` 的碰撞盒、血条、战斗组件全注释 |
| 🟡 P2 | Loading Screen 系统 | `ILoadingProcessInterface` 和 `UpdateInitialGameContentLoadPercent` 都无消费者 |
| 🟡 P2 | UI 系统 | 完全没有。要引入的话先引入 CommonUI + UIExtension |
| 🟢 P3 | Equipment / Inventory / Weapon 三段式模型 | Lyra 理念 2，待建 |
| 🟢 P3 | Teams 阵营系统 | 待建 |
| 🟢 P3 | AI（AIController / BehaviorTree / EQS） | 待建 |
| 🟢 P3 | ReplicationGraph / SignificanceManager | 大规模 Actor 时才需要 |
| 🟢 P3 | Hotfix 实际接入 | Policy 里的 Observer 是骨架 |
| 🟢 P3 | 开放世界（World Partition / Data Layer / HLOD） | 按方案是最后做 |
| 🟢 P3 | 后端（LoginServer / DB / Redis） | 按方案是最后做 |

### 7.3 路线图（建议顺序）

```
阶段 A：接通 Experience          ← 当前在这里
  ├─ 建 UHodgeWorldSettings，配 WorldSettingsClassName
  ├─ DefaultGame.ini 补 HodgeExperienceDefinition 扫描项
  ├─ GameState 挂 UHodgeExperienceManagerComponent
  ├─ GameMode 补 HandleMatchAssignmentIfNotExpectingOne + OnExperienceLoaded + 延迟生成 Pawn
  └─ 建第一个 Experience 数据资产，验证状态机跑通

阶段 B：数据驱动补全
  ├─ 补齐 UHodgePawnData 的 4 个字段
  ├─ 实现 UHodgeAbilitySet（授予/回收）
  └─ 实现 UHodgeGameplayCueManager

阶段 C：战斗闭环
  ├─ 第一个 GameplayAbility（近战攻击）
  ├─ CombatComponent：Hit 检测 → GE 伤害 → 死亡
  └─ 敌人 AI 基础

阶段 D：联机验证
  └─ Dedicated Server + 2 Client：移动 / 技能 / 属性 / 死亡同步

阶段 E：内容层
  ├─ 建 GameFeature 插件，验证热插拔
  └─ Equipment / Inventory 三段式模型
```

---

## 8. 代码规范与约定

### 8.1 命名

| 类别 | 前缀 | 示例 |
|---|---|---|
| UObject 派生类 | 引擎前缀（`U` / `A` / `S` / `F` 等） | `UHodgePawnData`、`AHodgeHeroCharacter` |
| 项目标识 | `Hodge` | `UHodgeAssetManager`、`HodgeGameplayTags` |
| Blueprint 资产 | 类型前缀 | `DA_`（DataAsset）、`IMC_`（InputMappingContext）、`IA_`（InputAction）、`GA_`（GameplayAbility）、`GE_`（GameplayEffect）、`WBP_` |
| 模块导出宏 | `HODGEPODGE_API` | |

> **历史遗留**：类名曾从 `Hodgepodge` 前缀改名为 `Hodge` 前缀（如 `HodgepodgeCharacterBase` → `HodgeCharacterBase`）。`DefaultEngine.ini` 的 `[CoreRedirects]` 段保留了全部重命名映射，**不要删除**，否则旧蓝图资产会失效。

### 8.2 目录与文件

- `Public/Private` 严格镜像，一个 `.h` 对应一个同路径 `.cpp`
- 头文件 include 用**完整相对路径**（`#include "Character/HodgeHeroCharacter.h"`），不用相对 `..`
- 每个 `.cpp` 顶部用 `#include UE_INLINE_GENERATED_CPP_BY_NAME(ClassName)` 加速编译（项目已在用，新文件请照做）

### 8.3 注释

项目里两种注释风格并存：

- **新代码**：中文 Doxygen 风格，`@file` / `@brief` / `@param` / `@return`
- **从 Lyra 拷贝的代码**：保留 Epic 原版英文注释，再叠加中文说明

新写的代码请统一用**中文 Doxygen 风格**。

> ⚠️ 部分注释存在**机器翻译痕迹**（例如 `Actor` 被译成"演员"、`GameplayAbility` 被译成"能力"、`InputAction` 被译成"输入动作"，中英混排）。看到不要困惑，逐步手动修正即可。

### 8.4 网络编程

- **一切默认服务器权威**。永远不要在客户端直接改属性（`HP -= 50` 是错的），走 `GameplayEffect`
- 新增可复制属性记得在 `GetLifetimeReplicatedProps` 里 `DOREPLIFETIME`
- ASC 相关的初始化一律走 `InitializeAbilitySystemForCharacter`，不要自己调 `InitAbilityActorInfo`

### 8.5 异步依赖

统一用 **"Register + 若已完成立即回调"** 模式，项目里已有两处范例：

```cpp
// UHodgeExperienceManagerComponent
CallOrRegister_OnExperienceLoaded_HighPriority(...)
// UHodgeLocalPlayerBase
CallAndRegister_OnPlayerControllerSet(...)
```

新写异步系统时照抄这个模式，不要让调用方自己判断"是不是已经初始化完了"。

---

## 9. 新人上手路径

按 `LYRA_LEARNING_GUIDE.md` 的经验，**不要从 UI 或玩法目录开始读**，从框架层开始性价比最高。

### 9.1 第一天：建立整体认知（约 4 小时）

按顺序读这 5 个文件，读完能建立完整认知：

| # | 文件 | 时间 | 收获 |
|---|---|---|---|
| 1 | `Data/HodgeAssetManagerStartupJob.h` | 15 min | 零依赖，理解启动加载进度系统 |
| 2 | `Data/HodgeExperienceDefinition.h` | 10 min | 理解数据驱动 |
| 3 | `Component/HodgeExperienceManagerComponent.cpp` | 1.5 h | **核心中的核心**，逐行走一遍状态机 |
| 4 | `Data/HodgePawnData.h` | 10 min | 理解角色数据契约 |
| 5 | `AbilitySystem/HodgeGameplayTags.h` | 30 min | 理解 Tag 体系覆盖面 |

然后跑一遍编辑器，确认能进 PIE 并操控角色。

### 9.2 第一周：角色与战斗

| 天 | 内容 | 目标问题 |
|---|---|---|
| 1-2 | `Character/` 四个类 + `Core/PlayState/HodgePlayerStateBase.cpp` | 一个 Pawn 从生成到"可用"经历了什么？GAS 双初始化为什么是必要的？ |
| 3-4 | `Input/` 两个文件 + `HodgeHeroCharacter::SetupPlayerInputComponent` | InputTag 是怎么把按键连到函数和 GAS 能力的？ |
| 5 | `AbilitySystem/AttributeSet/` 两个文件 | 为什么 Damage / Healing 是 Meta 属性而不是直接改 Health？ |
| 6-7 | ALS 的 `AAlsCharacter.h`（不要求全懂） | ALS 用 GameplayTag 表示状态，和我们的 `HodgeGameplayTags` 是两套，边界在哪？ |

### 9.3 第一个月：补齐与扩展

- 完成 [§7.3 阶段 A](#73-路线图建议顺序)：接通 Experience
- 读 `Plugins/ALS-Refactored-4.15/` 的 `AAlsCharacter` 和 `UAlsAnimationInstance`
- 对照 `LYRA_LEARNING_GUIDE.md` 第 4 章的十大理念，逐条确认本项目实现到哪一步

### 9.4 前置知识自检

有任意一项完全没接触过，先补，否则读代码会很吃力：

| 知识点 | 重要度 | 说明 |
|---|---|---|
| **GAS** | 🔴 极高 | ASC / GameplayAbility / GameplayEffect / AttributeSet / GameplayCue / GameplayTag |
| **网络复制基础** | 🔴 极高 | RPC、属性复制、`OnRep`、服务端权威 |
| **GameplayTag** | 🔴 极高 | 本项目用 Tag 做输入通道、状态标记、能力分类，是"万能胶水" |
| **Enhanced Input** | 🟠 高 | InputAction / InputMappingContext / Trigger |
| **AssetManager / 软引用 / PrimaryDataAsset** | 🟠 高 | 资源加载体系 |
| **Subsystem** | 🟡 中 | 项目用了 EngineSubsystem（`UHodgeExperienceManager`）和 LocalPlayerSubsystem |
| **GameFeatures / ModularGameplay** | 🟡 中 | 边用边学 |
| **ALS** | 🟡 中 |  locomotion 层，用到了再深入 |

### 9.5 进度自查表

- [ ] 能画出 Experience 的 7 个状态流转图
- [ ] 能解释为什么 Lyra 要把 Pawn 生成推迟到 Experience 加载完成之后
- [ ] 能说出 GAS 双初始化的两个入口分别在哪个端、什么时机
- [ ] 能解释 HealthSet 里 Damage 为什么是 Meta 属性
- [ ] 能说出 `HodgeGameplayTags` 里至少 4 类 Tag 的用途
- [ ] 能追踪一次按键输入 → ALS 移动 的完整调用链
- [ ] 能解释 AssetManager 的 StartupJob 权重是怎么换算成总体进度的
- [ ] 能说清本项目和 Lyra 在三处路线分歧上的取舍理由

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

### 10.3 日志

⚠️ 项目目前**没有独立 Log Category**，全部走 `LogTemp`。`HodgeLogChannels.h` 文件并不存在（只在 `HodgeExperienceManagerComponent.cpp` 里留了一行注释掉的 `#include`），需要新建。这是待办项（§7.2 P1）。

在此之前，调试靠断点 + `LogTemp` 过滤。`HodgeHeroCharacter` 的 `NotifyControllerChanged` 和 `SetupPlayerInputComponent` 里有 `LogTemp Warning` 级别的诊断输出，输入不生效时先看这两条。

### 10.4 关键断点位置

| 想知道什么 | 在哪打断点 |
|---|---|
| Experience 资源加载了什么 | `UHodgeExperienceManagerComponent::StartExperienceLoad()` |
| 插件何时激活 | `UHodgeExperienceManagerComponent::OnExperienceLoadComplete()` |
| Actions 何时执行 | `UHodgeExperienceManagerComponent::OnExperienceFullLoadCompleted()` |
| GameData 何时加载 | `UHodgeAssetManager::LoadGameDataOfClass()` |
| ASC 何时就绪（服务端） | `AHodgeHeroCharacter::PossessedBy()` |
| ASC 何时就绪（客户端） | `AHodgeHeroCharacter::OnRep_PlayerState()` |
| 属性集何时创建 | `AHodgePlayerStateBase::InitializeAbilitySystemForCharacter()` |
| 伤害如何结算 | `UHodgeHealthSet::PostGameplayEffectExecute()` |
| 输入绑定是否执行 | `AHodgeHeroCharacter::SetupPlayerInputComponent()` |
| IMC 何时切换 | `AHodgeHeroCharacter::NotifyControllerChanged()` |

### 10.5 推荐实验

1. **新建一个 Experience 数据资产**，改 `DefaultPawnData`，观察角色行为变化（需先接通 §7.2 P0）
2. **新建一个 GameFeatureAction**，继承 `UGameFeatureAction_WorldActionBase`，只打印日志，观察它在加载流程中的位置
3. **给 HealthSet 加一个 Mana 属性**，验证 AttributeSet 的配置驱动创建是否生效
4. **加一个 InputTag**，在 `DA_HodgeInputConfig` 里配好，在 `SetupPlayerInputComponent` 里绑定，跑通完整链路

---

## 11. 已知问题与技术债

按影响面排序。改这里的东西前先看一眼，别踩坑。

### 11.1 功能性缺口

| 问题 | 影响 | 位置 |
|---|---|---|
| Experience 未接入 GameMode | `SetCurrentExperience()` 从未被调用，整个 Experience 系统是"写好但没通电" | `Core/GameMode/HodgeGameModeBase.cpp`、`Core/GameState/HodgeGameStateBase.h` |
| `DefaultGame.ini` 缺 Experience 扫描项 | AssetManager 扫不到 Experience 资产，会静默失败 | `Config/DefaultGame.ini` |
| `UHodgePawnData` 只剩 1 个字段 | 数据驱动能力基本失效 | `Data/HodgePawnData.h` |
| 无 `UHodgeWorldSettings` | 地图没法指定默认 Experience | — |
| `InitializeGameplayCueManager()` 空实现 | GameplayCue 不会预加载，首次触发会卡顿 | `Data/HodgeAssetManager.cpp` |
| 5 个 GameFeatureAction 整体注释 | 指望它们能工作会浪费时间 | `GameFeatures/GameFeatureAction_*.h` |
| `Content/` 下无任何 `.uplugin` | 没有 GameFeature 插件实例，热插拔未经验证 | `Config/DefaultGame.ini` 指向 `/Game/Unused` |
| `AddInputMappings` / `RemoveInputMappings` 空实现 | IMC 管理实际在 `NotifyControllerChanged` 里手工做 | `Input/HodgeInputComponentBase.cpp` |

### 11.2 配置与命名

| 问题 | 说明 |
|---|---|
| `GlobalDefaultGameMode` 用旧类名 | `/Script/Hodgepodge.HodgepodgeGameModeBase`，靠 `[CoreRedirects]` 生效。能跑但建议改成 `HodgeGameModeBase` |
| 资产名拼写错误 | `DA_Dafult_GameData` / `DA_DafultPawnData`（`Dafult` 应为 `Default`），已写进 `DefaultGame.ini`，改名要同步改 ini |
| GameData 资产重复 | `Content/Main/Data/` 下同时有 `DA_Dafult_GameData` 和 `DA_DafultGameData` |
| 目录名拼写错误 | `Core/PlayState/` 应为 `Core/PlayerState/` |
| `ModularGameplay` 依赖缺失 | `.uproject` 声明了但 `Build.cs` 没加，代码也没用到 |

### 11.3 代码卫生

| 问题 | 说明 |
|---|---|
| 日志全部用 `LogTemp` | 无独立 Log Category，日志难过滤 |
| 注释机器翻译痕迹 | `Actor`→"演员"、`GameplayAbility`→"能力" 等中英混排 |
| 死代码未清理 | `HodgeEnemyCharacter` 大段注释、`MotionWarpingComponent` 注释、Lyra 遗留 Tag |
| Lyra 遗留 Tag | `HodgeGameplayTags.h` 里有 `Lyra_*`、`ShooterGame_*` 前缀的 Tag，是直接从 Lyra 拷的，应改名或删除 |
| 预留但无用的 Tag | `InitState_Spawned` / `DataAvailable` / `DataInitialized` / `GameplayReady` 四个 Tag 已声明，但项目**没有实现 Init State 链**（走的是双入口方案） |
| `// 111屎山代码来袭` | 多个文件顶部的自嘲注释，无害但会随时间失效 |

### 11.4 明确的设计取舍（不是 bug）

| 取舍 | 理由 |
|---|---|
| 不用 Lyra 的 Init State 链，改用双入口 | 当前异步依赖只有 PlayerState 和 Character 两个，Init State 链的复杂度不划算。等依赖变多（加 PawnData、InputConfig、AbilitySet 之后）再考虑引入 |
| 不引入 CommonUI / UIExtension / GameSettings / CommonUser | 体量大、非核心矛盾，延后 |
| 保留 ALS 原版的移动组件和相机组件 | 不自研 locomotion，先借用 |
| 保留 5 个注释掉的 Lyra GameFeatureAction | 作为参考实现，启用前需先补依赖 |

---

## 12. 附录：文件速查索引

### 12.1 架构核心（必读）

```
Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h              启动任务（零依赖，先读这个）
Source/Hodgepodge/Public/Data/HodgeAssetManager.h                       资源管理器
Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp                    StartupJob 注册与执行
Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h              Experience 数据结构
Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h    加载状态机 ★★★
Source/Hodgepodge/Private/Component/HodgeExperienceManagerComponent.cpp 状态机实现 ★★★
Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h                 玩家角色 + GAS 双初始化
Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h          ASC 归属 + 属性集动态创建
Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h    Meta 属性设计范例
Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h              Tag 总表
```

### 12.2 小而美的文件（值得精读）

```
Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h     ~46 行，进度系统设计
Source/Hodgepodge/Public/Input/HodgeInputConfig.h               Tag → InputAction 的数据驱动
Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h CallAndRegister 模式范例
Source/Hodgepodge/Public/Interface/LoadingProcessInterface.h    极简接口设计
```

### 12.3 配置文件

```
Config/DefaultEngine.ini     AssetManagerClassName、GlobalDefaultGameMode、
                             GameInstanceClass、[CoreRedirects]（勿删）
Config/DefaultGame.ini       [/Script/HodgePodge.HodgeAssetManager] 数据路径、
                             PrimaryAssetTypesToScan ★ 最容易踩坑
Config/DefaultInput.ini      Enhanced Input 按键映射
```

### 12.4 类继承关系速查

```
UAssetManager
└── UHodgeAssetManager

AGameModeBase
└── AHodgeGameModeBase

AGameStateBase
└── AHodgeGameStateBase

APlayerState + IAbilitySystemInterface
└── AHodgePlayerStateBase              （持有 ASC）

APlayerController
└── AHodgePlayerControllerBase         （事件桥梁）

ULocalPlayer
└── UHodgeLocalPlayerBase              （三套 CallAndRegister 委托）

ACharacter → AAlsCharacter
└── AHodgeCharacterBase
    └── AHodgeCombatCharacter (+ IAbilitySystemInterface)
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
└── UHodgeGameplayAbilityBase          （当前空壳）

UPrimaryDataAsset
├── UHodgeGameData
├── UHodgePawnData
├── UHodgeExperienceDefinition
└── UHodgeExperienceActionSet

UDataAsset
└── UHodgeInputConfig

UDefaultGameFeaturesProjectPolicies
└── UHodgeGameFeaturePolicy
```

### 12.5 参考文档

| 文档 | 用途 |
|---|---|
| [`LYRA_LEARNING_GUIDE.md`](LYRA_LEARNING_GUIDE.md) | Lyra 架构学习指南。本项目大部分理念的来源，**新系统设计的第一参考** |
| [`UE5 开放世界动作 RPG 架构方案 V2.md`](UE5%20开放世界动作%20RPG%20架构方案%20V2.md) | 本项目的总体方案与 Phase 划分，含角色职责划分、组件设计、DS 路线 |
| ALS 插件 README | `Plugins/ALS-Refactored-4.15/ALS-Refactored-4.15/README.md`，含版本对照表 |

---

*本 README 基于 UE 5.5 + ALS-Refactored 4.15 + Hodgepodge 当前代码状态整理。项目处于活跃开发中，进度表请定期更新。*
