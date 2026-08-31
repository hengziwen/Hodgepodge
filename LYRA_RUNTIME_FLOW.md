# Lyra 运行时执行链路与生命周期详解

> 配套文档：`LYRA_LEARNING_GUIDE.md`（学习路线）
> 本篇专注回答三个问题：**代码按什么顺序执行？每个系统的生命周期是什么？我运行时到底跑的是哪个 Experience？**

---

## 目录

- [0. 先解决你的困惑：为什么找不到跑的 Experience](#0-先解决你的困惑为什么找不到跑的-experience)
- [1. Experience 资产全清单](#1-experience-资产全清单)
- [2. 地图与 Experience 对照表](#2-地图与-experience-对照表)
- [3. 如何确认当前跑的是哪个 Experience](#3-如何确认当前跑的是哪个-experience)
- [4. Experience 决策链路（5 级优先级）](#4-experience-决策链路5-级优先级)
- [5. Experience 加载生命周期（状态机）](#5-experience-加载生命周期状态机)
- [6. 完整启动时序：从进程启动到 Pawn 生成](#6-完整启动时序从进程启动到-pawn-生成)
- [7. Pawn 生命周期：Init State 链](#7-pawn-生命周期init-state-链)
- [8. 其他系统生命周期](#8-其他系统生命周期)
- [9. 调试技巧](#9-调试技巧)

---

## 0. 先解决你的困惑：为什么找不到跑的 Experience

**原因：Lyra 编辑器默认打开的地图不是玩法地图。**

看这两个配置（`Config/DefaultEngine.ini`）：

```ini
GameDefaultMap=/Game/System/FrontEnd/Maps/L_LyraFrontEnd.L_LyraFrontEnd
EditorStartupMap=/Game/System/DefaultEditorMap/L_DefaultEditorOverview.L_DefaultEditorOverview
```

你打开编辑器看到的第一个地图是 **`L_DefaultEditorOverview`**（总览地图）。它是一个**门户/展厅地图**，里面放的是：

| 资产 | 作用 |
|------|------|
| `B_ExperienceList3D` | 3D 体验列表（展示所有可玩玩法） |
| `B_TeleportToUserFacingExperience` | 传送门（踩上去传送到对应玩法） |

**这个地图本身跑的是 `B_LyraDefaultExperience` —— 一个几乎空白的 Experience。**

所以你在编辑器里点 Play，看到的就是"什么都没发生"——因为你在一个空壳展厅里，不是在实际玩法地图上。

### 怎么玩到真正的玩法

三种方式：

**方式 1：直接打开玩法地图**

编辑器里打开这些地图再点 Play：
- `/ShooterMaps/Content/Maps/L_Expanse` —— 射击（歼灭模式）
- `/ShooterCore/Content/Maps/L_ShooterGym` —— 射击训练场
- `/TopDownArena/Content/Maps/L_TopDownArenaGym` —— 俯视角竞技

**方式 2：在总览地图里踩传送门**

在 `L_DefaultEditorOverview` 里 Play，走到 3D 体验列表的传送门上，它会把你传送到对应玩法（走 `UserFacingExperience` / Playlist 流程）。

**方式 3：用命令行或 URL 参数强制指定**

```
# 命令行启动
UE5Editor.exe LyraStarterGame -game -Experience=B_ShooterGame_Elimination
```

---

## 1. Experience 资产全清单

Lyra 的 Experience 分散在**主工程 + 各个 GameFeature 插件**里。这就是为什么你"找不到"——它们不在一个目录。

### 1.1 主工程 Content

| 资产路径 | 用途 |
|----------|------|
| `/Game/System/Experiences/B_LyraDefaultExperience` | **空 Experience**，总览地图/兜底用（代码里硬编码的兜底就是这个） |
| `/Game/System/FrontEnd/B_LyraFrontEnd_Experience` | 前端大厅菜单 |

### 1.2 ShooterCore 插件（射击玩法）

路径：`Plugins/GameFeatures/ShooterCore/Content/Experiences/`

| 资产 | 类型 | 用途 |
|------|------|------|
| `B_ShooterGame_Elimination` | Experience | 歼灭模式（主要玩法） |
| `B_LyraShooterGame_ControlPoints` | Experience | 控制点模式 |
| `B_ShooterGame_Perf` | Experience | 性能测试 |
| `LAS_ShooterGame_SharedInput` | **ActionSet** | 共享输入配置 |
| `LAS_ShooterGame_StandardComponents` | **ActionSet** | 标准组件集 |
| `LAS_ShooterGame_StandardHUD` | **ActionSet** | 标准 HUD |
| `Phases/`（5 个） | GE + Phase 能力 | `Phase_Warmup`、`Phase_Playing`、`Phase_PostGame`、`GE_PregameLobby`、`GE_DamageImmunity_FromGameMode` |

> `LAS_` 前缀 = **L**yra **A**ction**S**et。这是 Lyra 的命名约定：`ULyraExperienceActionSet`。
> **注意 ActionSet 和 Experience 是两个不同的类**：Experience 可以引用多个 ActionSet 组合成完整玩法。

### 1.3 其他插件

| 插件 | 资产 | 用途 |
|------|------|------|
| TopDownArena | `B_TopDownArenaExperience` | 俯视角竞技（单人） |
| TopDownArena | `B_TopDownArena_Multiplayer_Experience` | 俯视角竞技（多人） |
| ShooterExplorer | `B_TestInventoryExperience` | 背包系统测试 |

### 1.4 UserFacingExperience（Playlist）

**这是另一层抽象**，不要把两者混淆：

| | `ULyraExperienceDefinition` | `ULyraUserFacingExperienceDefinition` |
|---|---|---|
| **是什么** | 玩法逻辑定义 | 面向玩家的"入口卡片" |
| **包含** | 插件、PawnData、Actions | **地图 ID + Experience ID** + 标题/图标/描述/最大玩家数 |
| **在哪选** | 由 Playlist 引用 | 在 UI 菜单里选 |
| **扫描类型** | `LyraExperienceDefinition` | `LyraUserFacingExperienceDefinition` |

清单：

| 资产 | 关联 |
|------|------|
| `/Game/System/Playlists/DA_Frontend` | 前端大厅 |
| `/Game/System/Playlists/DA_ExamplePlaylist` | 示例 |
| `ShooterCore/.../Playlists/DA_ShooterGame_ShooterGym` | 射击训练场 |
| `ShooterCore/.../Playlists/DA_ShooterGame_ShooterPerf` | 性能测试 |

---

## 2. 地图与 Experience 对照表

| 地图 | 所属 | 预期 Experience |
|------|------|-----------------|
| `L_DefaultEditorOverview` | 主工程 | `B_LyraDefaultExperience`（空） |
| `L_LyraFrontEnd` | 主工程 | `B_LyraFrontEnd_Experience` |
| `L_Expanse` | ShooterMaps | `B_ShooterGame_Elimination` |
| `L_Expanse_Blockout` | ShooterMaps | `B_ShooterGame_Elimination` |
| `L_Convolution_Blockout` | ShooterMaps | `B_ShooterGame_Elimination` |
| `L_FiringRange_WP` | ShooterMaps | `B_ShooterGame_Elimination` |
| `L_ShooterGym` | ShooterCore | `B_ShooterGame_Elimination` |
| `L_ShooterPerf` | ShooterCore | `B_ShooterGame_Perf` |
| `L_TopDownArenaGym` | TopDownArena | `B_TopDownArenaExperience` |
| `L_TopDown_LocalMultiplayer` | TopDownArena | `B_TopDownArena_Multiplayer_Experience` |
| `L_InventoryTestMap` | ShooterExplorer | `B_TestInventoryExperience` |

> ⚠️ 上表的 Experience 关联是**基于项目结构推断**的。地图的 WorldSettings 配置保存在 `.umap`（二进制）里，无法从文本直接确认。**在编辑器里打开地图 → 主菜单 `Window` → `World Settings` → `Default Gameplay Experience` 字段可以看到确切值。**

---

## 3. 如何确认当前跑的是哪个 Experience

### 方法 1：看日志（最可靠）

Lyra 在关键节点都打了日志。打开 Output Log，过滤 `LogLyraExperience`：

```
LogLyraExperience: Identified experience LyraExperienceDefinition:B_ShooterGame_Elimination (Source: WorldSettings)
LogLyraExperience: EXPERIENCE: StartExperienceLoad(CurrentExperience = LyraExperienceDefinition:B_ShooterGame_Elimination, ...)
LogLyraExperience: EXPERIENCE: OnExperienceLoadComplete(CurrentExperience = ...)
```

**`Source: XXX` 直接告诉你 Experience 是从哪个优先级来的**（OptionsString / DeveloperSettings / CommandLine / WorldSettings / Default）。

对应代码位置：

```cpp
// LyraGameMode.cpp:293
UE_LOG(LogLyraExperience, Log, TEXT("Identified experience %s (Source: %s)"),
    *ExperienceId.ToString(), *ExperienceIdSource);

// LyraExperienceManagerComponent.cpp:128
UE_LOG(LogLyraExperience, Log, TEXT("EXPERIENCE: StartExperienceLoad(CurrentExperience = %s, %s)"), ...);
```

### 方法 2：看 World Settings

编辑器打开地图 → `Window` → `World Settings` → 找 **Default Gameplay Experience** 字段。

### 方法 3：打断点

在 `LyraGameMode::OnMatchAssignmentGiven()` 下断点，看 `ExperienceId` 的值。

---

## 4. Experience 决策链路（5 级优先级）

入口：`ALyraGameMode::HandleMatchAssignmentIfNotExpectingOne()`
文件：`Source/LyraGame/GameModes/LyraGameMode.cpp:88`

**注意它被延迟了一帧调用**：

```cpp
// LyraGameMode.cpp:80
void ALyraGameMode::InitGame(...)
{
    Super::InitGame(MapName, Options, ErrorMessage);
    // Wait for the next frame to give time to initialize startup settings
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}
```

为什么要延迟一帧？注释说得很清楚：**给启动设置（Startup Settings）初始化的时间**。

### 决策流程（带代码行号）

```cpp
FPrimaryAssetId ExperienceId;   // 空
FString ExperienceIdSource;

// ① URL Options  —— 最高优先级
if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
{
    const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
    ExperienceId = FPrimaryAssetId(FPrimaryAssetType(ULyraExperienceDefinition::StaticClass()->GetFName()),
                                   FName(*ExperienceFromOptions));
    ExperienceIdSource = TEXT("OptionsString");
}

// ② DeveloperSettings —— 仅 PIE 生效
if (!ExperienceId.IsValid() && World->IsPlayInEditor())
{
    ExperienceId = GetDefault<ULyraDeveloperSettings>()->ExperienceOverride;
    ExperienceIdSource = TEXT("DeveloperSettings");
}

// ③ 命令行
if (!ExperienceId.IsValid())
{
    FString ExperienceFromCommandLine;
    if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
    {
        ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);
        if (!ExperienceId.PrimaryAssetType.IsValid())
        {
            ExperienceId = FPrimaryAssetId(FPrimaryAssetType(ULyraExperienceDefinition::StaticClass()->GetFName()),
                                           FName(*ExperienceFromCommandLine));
        }
        ExperienceIdSource = TEXT("CommandLine");
    }
}

// ④ WorldSettings —— 地图配置
if (!ExperienceId.IsValid())
{
    if (ALyraWorldSettings* TypedWorldSettings = Cast<ALyraWorldSettings>(GetWorldSettings()))
    {
        ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
        ExperienceIdSource = TEXT("WorldSettings");
    }
}

// 校验：这个 Experience 真的存在吗？
ULyraAssetManager& AssetManager = ULyraAssetManager::Get();
FAssetData Dummy;
if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
{
    UE_LOG(LogLyraExperience, Error, TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"), ...);
    ExperienceId = FPrimaryAssetId();   // 清空，走兜底
}

// ⑤ 硬编码兜底
if (!ExperienceId.IsValid())
{
    if (TryDedicatedServerLogin())   // 专用服务器特殊路径
    {
        return;
    }
    //@TODO: Pull this from a config setting or something
    ExperienceId = FPrimaryAssetId(FPrimaryAssetType("LyraExperienceDefinition"), FName("B_LyraDefaultExperience"));
    ExperienceIdSource = TEXT("Default");
}

OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
```

### 优先级速查表

| 顺序 | 来源 | 生效条件 | 典型场景 |
|------|------|----------|----------|
| 1 | **URL Options** | URL 里有 `?Experience=XXX` | 服务器跳转、Seamless Travel |
| 2 | **DeveloperSettings** | **仅 PIE** | 编辑器里临时切换玩法调试 |
| 3 | **命令行** | `-Experience=XXX` | 批处理启动、自动化测试 |
| 4 | **WorldSettings** | 地图配置了 | **最常见**，正常开图 |
| 5 | **硬编码兜底** | 以上都没有 | 总览地图 |

### 关于 ② DeveloperSettings（PIE 专用）

定义在 `Source/LyraGame/Development/LyraDeveloperSettings.h:54`：

```cpp
UCLASS(config=EditorPerProjectUserSettings, MinimalAPI)
class ULyraDeveloperSettings : public UDeveloperSettingsBackedByCVars
{
    // The experience override to use for Play in Editor (if not set, the default for the world settings of the open map will be used)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, config, Category=Lyra, meta=(AllowedTypes="LyraExperienceDefinition"))
    FPrimaryAssetId ExperienceOverride;
};
```

**配置文件是 `Config/DefaultEditorPerProjectUserSettings.ini`，当前项目里只配了 `CommonEditorMaps`，没有配 `ExperienceOverride`** —— 所以你的 PIE 会跳过这一级，走到 WorldSettings。

**实用技巧**：在编辑器 `Project Settings` → `Lyra` → `Experience Override` 里设置一个 Experience，可以让**所有地图**在 PIE 时都跑这个玩法。调试时非常方便。

---

## 5. Experience 加载生命周期（状态机）

### 5.1 状态定义

```cpp
// LyraExperienceManagerComponent.h:16
enum class ELyraExperienceLoadState
{
    Unloaded,                   // 未加载
    Loading,                    // 正在异步加载资源 Bundle
    LoadingGameFeatures,        // 正在加载并激活 GameFeature 插件
    LoadingChaosTestingDelay,   // 人为延迟（测试用）
    ExecutingActions,           // 正在执行 GameFeatureAction
    Loaded,                     // 完成
    Deactivating                // 正在反激活（清理）
};
```

### 5.2 完整流程

```
                    ┌─────────────────────────────────────────┐
                    │  SetCurrentExperience(ExperienceId)      │
                    │  LyraExperienceManagerComponent.cpp:56   │
                    └──────────────────┬──────────────────────┘
                                       │
                     AssetManager.GetPrimaryAssetPath(ExperienceId)
                     AssetPath.TryLoad()  ← 同步加载 Experience 类
                     GetDefault<ULyraExperienceDefinition>(AssetClass)
                                       │
                    ┌──────────────────▼──────────────────────┐
                    │  StartExperienceLoad()                   │
                    │  状态 → Loading                          │
                    │  :123                                    │
                    └──────────────────┬──────────────────────┘
                                       │
        收集 BundleAssetList：Experience 自身 + 所有 ActionSet
        BundlesToLoad = { Equipped, [Client], [Server] }
                                       │
        ChangeBundleStateForPrimaryAssets()  ← 异步加载
                                       │
                    ┌──────────────────▼──────────────────────┐
                    │  OnExperienceLoadComplete()              │
                    │  :214                                    │
                    └──────────────────┬──────────────────────┘
                                       │
        从 Experience.GameFeaturesToEnable 收集插件 URL
        （+ 所有 ActionSet 的 GameFeaturesToEnable）
        UGameFeaturesSubsystem::GetPluginURLByName()
                                       │
                    ┌──────────────────▼──────────────────────┐
                    │  状态 → LoadingGameFeatures               │
                    │  逐个 LoadAndActivateGameFeaturePlugin()  │
                    │  :269                                    │
                    └──────────────────┬──────────────────────┘
                                       │
                    NumGameFeaturePluginsLoading-- 计数归零
                                       │
                    ┌──────────────────▼──────────────────────┐
                    │  OnGameFeaturePluginLoadComplete()        │
                    │  :278                                     │
                    └──────────────────┬───────────────────────┘
                                       │
                    ┌──────────────────▼──────────────────────┐
                    │  OnExperienceFullLoadCompleted()          │
                    │  :289                                     │
                    └──────────────────┬───────────────────────┘
                                       │
        [可选] Chaos 延迟 → 状态 LoadingChaosTestingDelay → 定时器回调
                                       │
        状态 → ExecutingActions
        对每个 Action 依次调用：
            Action->OnGameFeatureRegistering();
            Action->OnGameFeatureLoading();
            Action->OnGameFeatureActivating(Context);
        （先 Experience.Actions，再各 ActionSet.Actions）
                                       │
        状态 → Loaded
                                       │
        广播三档委托（顺序保证）：
            ① OnExperienceLoaded_HighPriority
            ② OnExperienceLoaded
            ③ OnExperienceLoaded_LowPriority
                                       │
                    ┌──────────────────▼──────────────────────┐
                    │  GameMode::OnExperienceLoaded()           │
                    │  → RestartPlayer() ← 到这一步才生成 Pawn！ │
                    │  LyraGameMode.cpp:305                     │
                    └──────────────────────────────────────────┘
```

### 5.3 关键细节

**为什么有三档委托？**

```cpp
// 订阅 API
void CallOrRegister_OnExperienceLoaded_HighPriority(FOnLyraExperienceLoaded::FDelegate&& Delegate);
void CallOrRegister_OnExperienceLoaded(FOnLyraExperienceLoaded::FDelegate&& Delegate);
void CallOrRegister_OnExperienceLoaded_LowPriority(FOnLyraExperienceLoaded::FDelegate&& Delegate);
```

**解决初始化顺序依赖问题**。比如：
- HighPriority：能力系统先就绪
- Normal：常规玩法系统
- LowPriority：UI（依赖前两者的数据）

如果都挤在一个委托里，你就得靠"订阅顺序"这种脆弱的隐式约定。

**网络同步**：`CurrentExperience` 是复制属性，客户端通过 `OnRep_CurrentExperience()` 触发同样的加载流程：

```cpp
// :118
void ULyraExperienceManagerComponent::OnRep_CurrentExperience()
{
    StartExperienceLoad();
}
```

**加载状态与加载屏联动**：实现了 `ILoadingProcessInterface`：

```cpp
// :447
bool ULyraExperienceManagerComponent::ShouldShowLoadingScreen(FString& OutReason) const
{
    if (LoadState != ELyraExperienceLoadState::Loaded)
    {
        OutReason = TEXT("Experience still loading");
        return true;   // 只要没加载完，就显示加载屏
    }
    return false;
}
```

**这解释了 Lyra 加载屏为什么能自动出现和消失** —— `CommonLoadingScreen` 插件会轮询所有实现了这个接口的对象。

### 5.4 卸载流程（EndPlay）

```
EndPlay()
  → 逐个 DeactivateGameFeaturePlugin()
  → 状态 → Deactivating
  → 对每个 Action：OnGameFeatureDeactivating() → OnGameFeatureUnregistering()
  → OnAllActionsDeactivated() → 状态 → Unloaded
```

> ⚠️ 源码注释里明确标注了这块不完善：
> `//@TODO: We actually only deactivated and didn't fully unload...`
> `//@TODO: Handle deactivating game features, right now we 'leak' them enabled`

---

## 6. 完整启动时序：从进程启动到 Pawn 生成

```
═══ 阶段 A：引擎与资源管理器 ═══

进程启动
  ↓
ULyraAssetManager::StartInitialLoading()          [LyraAssetManager.cpp:106]
  ├─ Super::StartInitialLoading()                 扫描所有 PrimaryAssetType
  ├─ STARTUP_JOB(InitializeGameplayCueManager())  权重 1
  ├─ STARTUP_JOB_WEIGHTED(GetGameData(), 25.f)    权重 25
  └─ DoAllStartupJobs()                           [按权重计算进度，喂给加载屏]
       ‼️ 这是"启动早期加载屏"的进度来源


═══ 阶段 B：地图与 GameMode ═══

地图加载
  ↓
ALyraGameMode::InitGame()                        [LyraGameMode.cpp:80]
  └─ SetTimerForNextTick(HandleMatchAssignmentIfNotExpectingOne)
       ‼️ 延迟一帧，等启动设置初始化
  ↓
ALyraGameState::InitGameState()  （基类流程）
  └─ 创建 ExperienceManagerComponent（复制属性 CurrentExperience）
  ↓
ALyraGameMode::InitGameState()                   [LyraGameMode.cpp:452]
  └─ ExperienceComponent->CallOrRegister_OnExperienceLoaded(
         FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded))
       ‼️ 先订阅，后面 Experience 加载完会回调


═══ 阶段 C：确定 Experience ═══

ALyraGameMode::HandleMatchAssignmentIfNotExpectingOne()   [:88]
  ↓ 5 级优先级决策（见第 4 章）
ALyraGameMode::OnMatchAssignmentGiven()                   [:289]
  └─ ExperienceComponent->SetCurrentExperience(ExperienceId)


═══ 阶段 D：Experience 加载 ═══

（见第 5 章状态机：Loading → LoadingGameFeatures → ExecutingActions → Loaded）


═══ 阶段 E：生成 Pawn ═══

广播 OnExperienceLoaded
  ↓
ALyraGameMode::OnExperienceLoaded()              [LyraGameMode.cpp:305]
  └─ 遍历所有 PlayerController，对没 Pawn 的调用 RestartPlayer()
  ↓
ALyraGameMode::GetDefaultPawnClassForController_Implementation()   [:332]
  └─ GetPawnDataForController()
       ├─ 优先：PlayerState 上的 PawnData
       └─ 其次：Experience->DefaultPawnData
       └─ 兜底：AssetManager.GetDefaultPawnData()
  ↓
ALyraGameMode::SpawnDefaultPawnAtTransform_Implementation()       [:345]
  ├─ SpawnActor<Pawn>(PawnClass, SpawnInfo)   ← bDeferConstruction = true
  ├─ PawnExtComp->SetPawnData(PawnData)       ← 关键！
  └─ SpawnedPawn->FinishSpawning()


═══ 阶段 F：Pawn 初始化（Init State 链）═══

（见第 7 章）
```

### 关键点：为什么用 `bDeferConstruction = true`？

```cpp
// LyraGameMode.cpp:350
SpawnInfo.bDeferConstruction = true;
if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
{
    if (ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
    {
        PawnExtComp->SetPawnData(PawnData);      // ← 在构造完成前设置数据
    }
    SpawnedPawn->FinishSpawning(SpawnTransform); // ← 才真正 BeginPlay
}
```

**目的：让 PawnData 在 `BeginPlay` 之前就位**，这样 `BeginPlay` 里就能拿到完整配置。这是一个很实用的技巧。

---

## 7. Pawn 生命周期：Init State 链

### 7.1 为什么需要它

Lyra 的 Pawn 依赖多个**异步到达**的数据：

| 数据 | 何时到达 |
|------|----------|
| PawnData | GameMode 设置（服务端），或网络复制（客户端） |
| Controller | Possess 时机不确定 |
| PlayerState | 网络复制，比 Pawn 晚 |
| ASC | 在 PlayerState 上，需等待 |

如果都在 `BeginPlay` 里初始化，你永远不知道"谁的依赖先到"。**Init State 链就是把这些异步依赖变成有序的状态机。**

### 7.2 四个状态

定义在 `Source/LyraGame/LyraGameplayTags.cpp:27-30`：

```cpp
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_Spawned,         "InitState.Spawned",         "1: Actor/component has initially spawned and can be extended");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataAvailable,   "InitState.DataAvailable",   "2: All required data has been loaded/replicated and is ready for initialization");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataInitialized, "InitState.DataInitialized", "3: The available data has been initialized for this actor/component, but it is not ready for full gameplay");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_GameplayReady,   "InitState.GameplayReady",   "4: The actor/component is fully ready for active gameplay");
```

链定义（`LyraPawnExtensionComponent.cpp:218`）：

```cpp
static const TArray<FGameplayTag> StateChain = {
    LyraGameplayTags::InitState_Spawned,
    LyraGameplayTags::InitState_DataAvailable,
    LyraGameplayTags::InitState_DataInitialized,
    LyraGameplayTags::InitState_GameplayReady
};
```

### 7.3 准入条件（CanChangeInitState）

`LyraPawnExtensionComponent.cpp:224` —— **这是理解整个机制的核心**：

```cpp
// → Spawned
if (!CurrentState.IsValid() && DesiredState == InitState_Spawned)
    return Pawn != nullptr;   // 在有效 Pawn 上即可

// Spawned → DataAvailable
if (CurrentState == InitState_Spawned && DesiredState == InitState_DataAvailable)
{
    if (!PawnData)              return false;   // ① 必须有 PawnData
    if (bHasAuthority || bIsLocallyControlled)
        if (!GetController())   return false;   // ② 且必须有 Controller
    return true;
}

// DataAvailable → DataInitialized
if (CurrentState == InitState_DataAvailable && DesiredState == InitState_DataInitialized)
    // ③ 必须所有 feature 都到了 DataAvailable
    return Manager->HaveAllFeaturesReachedInitState(Pawn, InitState_DataAvailable);

// DataInitialized → GameplayReady
if (CurrentState == InitState_DataInitialized && DesiredState == InitState_GameplayReady)
    return true;
```

### 7.4 推进机制

**关键点：状态不是"自动"推进的，而是被事件触发重新检查的。**

```cpp
void ULyraPawnExtensionComponent::CheckDefaultInitialization()   // :213
{
    CheckDefaultInitializationForImplementers();   // 先帮别人推进
    ContinueInitStateChain(StateChain);           // 再推进自己
}
```

**哪些事件会触发 `CheckDefaultInitialization()`？**

| 触发点 | 代码位置 |
|--------|----------|
| `BeginPlay()` | :65 |
| `SetPawnData()` （服务端设置后） | :97 |
| `OnRep_PawnData()` （客户端复制到达后） | :102 |
| `HandleControllerChanged()` （Possess 变化） | :200 |
| `HandlePlayerStateReplicated()` | :205 |
| `SetupPlayerInputComponent()` | :210 |
| `OnActorInitStateChanged()` （**其他 feature 状态变化时**） | :287 |

最后一条是关键：

```cpp
void ULyraPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)   // :280
{
    if (Params.FeatureName != NAME_ActorFeatureName)   // 不是自己
    {
        if (Params.FeatureState == LyraGameplayTags::InitState_DataAvailable)
        {
            CheckDefaultInitialization();   // 别人到 DataAvailable 了，我再试一次
        }
    }
}
```

**这就是"协作式推进"**：每个组件只关心自己的条件，任何组件状态变化都会触发全局重检，直到所有组件都推进到位。

### 7.5 时序图

```
SpawnActor (bDeferConstruction=true)
  ↓
ULyraPawnExtensionComponent::OnRegister()
  └─ RegisterInitStateFeature()          ← 注册为 init state feature
  ↓
SetPawnData(PawnData)                    ← GameMode 调用
  └─ CheckDefaultInitialization()
  ↓
FinishSpawning() → BeginPlay()
  ├─ BindOnActorInitStateChanged(NAME_None, ...)  ← 监听所有 feature
  ├─ TryToChangeInitState(InitState_Spawned)      ✓→ Spawned
  └─ CheckDefaultInitialization()
  ↓
[异步等待] PawnData 复制到达 / Controller Possess
  ↓
→ DataAvailable
  ├─ 其他 feature（HeroComponent 等）也各自推进到 DataAvailable
  ├─ OnActorInitStateChanged 互相触发重检
  ↓
所有 feature 都到 DataAvailable
  → DataInitialized
  ├─ 各组件在此阶段做实际初始化（如 ASC 授予能力）
  ↓
→ GameplayReady
  ├─ 可以开始正常游戏逻辑
```

### 7.6 参与方

| 组件 | FeatureName | 说明 |
|------|-------------|------|
| `ULyraPawnExtensionComponent` | `PawnExtension` | 中枢，定义状态链 |
| `ULyraHeroComponent` | （自己的名字） | 玩家控制逻辑，监听 PawnExtension 的状态变化 |

```cpp
// LyraHeroComponent.cpp:211
BindOnActorInitStateChanged(ULyraPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);
ensure(TryToChangeInitState(LyraGameplayTags::InitState_Spawned));
```

---

## 8. 其他系统生命周期

### 8.1 ASC（AbilitySystemComponent）

**ASC 在哪里？**

| 类 | ASC 位置 | 用途 |
|----|----------|------|
| `ALyraCharacter` | **PlayerState 上** | 标准玩家（死亡后 ASC 保留，便于重生/观战） |
| `ALyraCharacterWithAbilities` | **Character 自身** | NPC / AI（没有 PlayerState） |

**初始化**（`LyraPawnExtensionComponent.cpp:105`）：

```cpp
void ULyraPawnExtensionComponent::InitializeAbilitySystem(ULyraAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
    if (AbilitySystemComponent == InASC) return;
    if (AbilitySystemComponent) UninitializeAbilitySystem();   // 先清理旧的

    // 处理"换 Pawn"的情况：把旧 Pawn 踢出
    AActor* ExistingAvatar = InASC->GetAvatarActor();
    if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
    {
        if (ULyraPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
            OtherExtensionComponent->UninitializeAbilitySystem();
    }

    AbilitySystemComponent = InASC;
    AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);   // Owner = PlayerState, Avatar = Pawn
    InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);

    OnAbilitySystemInitialized.Broadcast();   // 广播：能力系统就绪
}
```

**清理**（`UninitializeAbilitySystem()`，`:152`）：

```cpp
AbilityTypesToIgnore.AddTag(LyraGameplayTags::Ability_Behavior_SurvivesDeath);  // 死亡后仍保留的能力不取消
AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
AbilitySystemComponent->ClearAbilityInput();
AbilitySystemComponent->RemoveAllGameplayCues();
AbilitySystemComponent->SetAvatarActor(nullptr);   // 或 ClearActorInfo()
OnAbilitySystemUninitialized.Broadcast();
```

**"注册 + 立即回调"模式**（处理异步依赖的经典手法）：

```cpp
void ULyraPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)   // :292
{
    if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
        OnAbilitySystemInitialized.Add(Delegate);   // 注册

    if (AbilitySystemComponent)
        Delegate.Execute();                          // 如果已经就绪，立即回调
}
```

> 这个模式解决了"我订阅时，事件可能已经发生过了"的问题。Lyra 里到处都是这个模式（Experience 的 `CallOrRegister_OnExperienceLoaded` 也是同样思路）。

### 8.2 装备（Equipment）

```
玩家按切换槽位键
  ↓
ULyraQuickBarComponent::SetActiveSlotIndex()      [Server RPC]
  ├─ UnequipItemInSlot()  → EquipmentManager->UnequipItem()
  └─ EquipItemInSlot()
       ├─ 从槽位物品取 Fragment：FindFragmentByClass<UInventoryFragment_EquippableItem>()
       ├─ 拿到 EquipmentDefinition
       ├─ EquipmentManager->EquipItem(EquipDef)
       └─ EquippedItem->SetInstigator(SlotItem)   ← 装备回指背包物品的唯一纽带
  ↓
ULyraEquipmentManagerComponent::EquipItem()
  └─ EquipmentList.AddEntry(EquipmentClass)
  ↓
FLyraEquipmentList::AddEntry()  [服务端权威，5 步]
  ① check(OwnerComponent->GetOwner()->HasAuthority())
  ② 读 CDO：GetDefault<ULyraEquipmentDefinition>(Def)
  ③ 创建 Instance：NewObject<ULyraEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType)
       ‼️ Outer 是 Actor（不是 Component），因此 GetPawn() = Cast<APawn>(GetOuter())
  ④ 授予能力：AbilitySet->GiveToAbilitySystem(ASC, &GrantedHandles, Instance)
       ‼️ SourceObject 传入 Instance → 能力可通过 Spec->SourceObject 反查装备
  ⑤ 生成挂件 Actor：对 ActorsToSpawn 逐个 SpawnActor 并 Attach 到骨骼插槽
  ↓
[网络复制] FFastArraySerializer 增量复制到客户端
  ↓
[客户端] PostReplicatedAdd() → 触发 OnEquipped → 播放动画/特效
```

**卸载**是反向：回收 `GrantedHandles` 里的所有能力/GE/属性集，销毁 Instance 和挂件 Actor。

> `GrantedHandles` 是 `NotReplicated` 的 —— **只有服务端持有**，因为只有服务端需要回收。

### 8.3 游戏阶段（GamePhase）

由 `ULyraGamePhaseSubsystem`（WorldSubsystem）管理，用**嵌套 GameplayTag** 表示阶段。

ShooterCore 的阶段资产（`Plugins/GameFeatures/ShooterCore/Content/Experiences/Phases/`）：

```
Phase_Warmup        热身（不能开火，GE_DamageImmunity_FromGameMode 免疫伤害）
  ↓
Phase_Playing       正式游戏
  ↓
Phase_PostGame      结算
```

阶段用 `ULyraGamePhaseAbility` 表示，激活/结束时通知 PhaseSubsystem，子系统再通知所有观察者。

---

## 9. 调试技巧

### 9.1 日志

```
Log LogLyraExperience Verbose     ← 看 Experience 全流程（最有用）
Log LogLyra Verbose               ← 看 PawnData / ASC 相关
Log LogLyraAbilitySystem Verbose  ← 看能力相关
```

**关键日志文本**（可直接搜索）：

| 日志 | 位置 |
|------|------|
| `Identified experience XXX (Source: YYY)` | Experience 已选定，能看到来源 |
| `EXPERIENCE: StartExperienceLoad(...)` | 开始加载 |
| `EXPERIENCE: OnExperienceLoadComplete(...)` | 资源加载完，开始加载插件 |
| `Failed to find plugin URL from PluginName` | 插件名写错了 |
| `Trying to set PawnData [...] that already has valid PawnData` | 重复设置 PawnData |

### 9.2 控制台命令

| 命令 | 作用 |
|------|------|
| `Lyra.DumpLoadedAssets` | 列出 AssetManager 常驻内存的资源（排查内存） |
| `lyra.chaos.ExperienceDelayLoad.MinSecs 2` | Experience 加载延迟 2 秒 |
| `lyra.chaos.ExperienceDelayLoad.RandomSecs 3` | 随机延迟 0~3 秒 |
| `GameplayMessageSubsystem.LogMessages 1` | 打印所有消息总线消息 |
| `-LogAssetLoads`（启动参数） | 打印每个资源的同步加载耗时 |

### 9.3 断点速查

| 想看什么 | 断点位置 |
|----------|----------|
| 选了哪个 Experience | `LyraGameMode::OnMatchAssignmentGiven()` |
| Bundle 加载了什么 | `LyraExperienceManagerComponent::StartExperienceLoad()` |
| 插件何时激活 | `LyraExperienceManagerComponent::OnExperienceLoadComplete()` |
| Actions 何时执行 | `LyraExperienceManagerComponent::OnExperienceFullLoadCompleted()` |
| PawnData 谁设置的 | `LyraPawnExtensionComponent::SetPawnData()` |
| init state 为何卡住 | `LyraPawnExtensionComponent::CanChangeInitState()` ← **调试初始化问题首选** |
| ASC 何时就绪 | `LyraPawnExtensionComponent::InitializeAbilitySystem()` |
| 装备流程 | `FLyraEquipmentList::AddEntry()` |
| 伤害计算 | `LyraDamageExecution::Execute_Implementation()` |

### 9.4 诊断"Pawn 没生成"的流程

如果你遇到 Pawn 不出现，按这个顺序查：

1. **Experience 找到了吗？** → 看日志有没有 `Identified experience`
2. **加载卡在哪个状态？** → 看 `EXPERIENCE:` 系列日志走到哪一步
3. **PawnData 有了吗？** → 断点 `SetPawnData()`
4. **init state 卡在哪？** → 断点 `CanChangeInitState()`，看返回 false 的原因
   - 没有 PawnData → 检查 Experience 的 `DefaultPawnData` 配置
   - 没有 Controller → 检查 Possess 流程
   - 有 feature 没到 DataAvailable → 看是哪个 feature

---

## 附：一句话总结各系统生命周期

| 系统 | 生命周期概括 |
|------|-------------|
| **Experience** | 5 级决策 → 加载 Bundle → 激活插件 → 执行 Action → 广播三档委托 → 生成 Pawn |
| **Pawn** | Spawned → DataAvailable（要 PawnData+Controller）→ DataInitialized（所有 feature 就绪）→ GameplayReady |
| **ASC** | 从 PlayerState 获取 → `InitAbilityActorInfo(Owner=PS, Avatar=Pawn)` → 广播就绪 → 死亡时取消能力但保留 ASC |
| **装备** | 定义 CDO → 创建 Instance（Outer=Pawn）→ 授予能力（SourceObject=Instance）→ 复制 → 客户端特效 |
| **阶段** | Warmup → Playing → PostGame，用嵌套 GameplayTag 由 PhaseSubsystem 管理 |

---

*本文档基于 UE 5.5 版本 Lyra Starter Game 整理。*
