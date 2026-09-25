# Lyra 运行时执行链路与生命周期详解

> 配套文档：`LYRA_LEARNING_GUIDE.md`（学习路线）
> 本篇专注回答四个问题：**代码按什么顺序执行？每个系统的生命周期是什么？我运行时到底跑的是哪个 Experience？从开火到造成伤害这条链路是怎么走的？**

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
- [9. 造成伤害完整链路（从开火到死亡）](#9-造成伤害完整链路从开火到死亡)
- [10. 调试技巧](#10-调试技巧)

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

## 9. 造成伤害完整链路（从开火到死亡）

> 本章回答：**从"捡到一把枪"到"敌人掉血死亡"，代码到底按什么顺序跑？**
> 先把这张总览图记住，后面每一节对应其中一段：

```
Experience 加载 / GameFeature 激活
  → PawnData 授予起始能力 + HealthSet
  → 拾取武器：Inventory → QuickBar → EquipmentManager.EquipItem
  → AddEntry：创建装备实例 + 授予武器能力集（带 InputTag）+ Spawn 武器 Actor
  → 输入：InputAction → InputTag → ASC.AbilityInputTagPressed → TryActivateAbility
  → GA_Weapon_Fire.ActivateAbility → StartRangedWeaponTargeting → 本地 Trace → TargetData
  → 预测上报服务器 + 服务器 ClientConfirmTargetData（命中标记）
  → 蓝图 OnRangedWeaponTargetDataReady → MakeOutgoingSpec(DamageGE) → ApplyGameplayEffectSpecToTarget
  → LyraDamageExecution：基础伤害 × 距离衰减 × 材质衰减 × 友伤判定 → 写入 HealthSet.Damage
  → HealthSet.PostGameplayEffectExecute：Health − Damage → OnOutOfHealth
  → HealthComponent.HandleOutOfHealth：GameplayEvent.Death + Elim 消息
  → GA_Death → StartDeath / FinishDeath → Status.Death.Dying / Dead
  → Cue 飘字 + ShooterCore 计分（ElimStreak / ElimChain / Assist）
```

> ⚠️ **两个 C++ / 蓝图边界**（读源码时会"断链"，先记下来）：
> 1. "构造伤害 GE Spec + `ApplyGameplayEffectSpecToTarget`"在蓝图 `GA_Weapon_Fire` 里，C++ 只暴露钩子 `OnRangedWeaponTargetDataReady`。
> 2. 伤害飘字 `AddNumberPop` 的调用者是伤害 GameplayCue（蓝图资产），C++ 里没有 native 调用点。

### 9.1 阶段 1：Experience 加载与能力/属性注入

World 由 **Experience** 驱动（见第 4、5 章）。`UGameFeatureAction_AddAbilities` 在目标 Actor 就绪时，把 Ability / AttributeSet 授予其 ASC —— 触发时机监听 `ALyraPlayerState::NAME_LyraAbilityReady`（`GameFeatureAction_AddAbilities.cpp:144-159, 161-231`）。这是"全局规则型"能力/属性的注入通道。

### 9.2 阶段 2：Pawn 就位 —— PawnData 授予起始能力与 HealthSet

`ULyraPawnData` 配置了 `AbilitySets` / `InputConfig` / `PawnClass` / `DefaultCameraMode`。PlayerState 收到 PawnData 时授予起始能力集：

```cpp
// LyraPlayerState.cpp:185
void ALyraPlayerState::SetPawnData(const ULyraPawnData* InPawnData)
{
    ...
    for (const ULyraAbilitySet* AbilitySet : PawnData->AbilitySets)
    {
        if (AbilitySet)
        {
            AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);   // :207
        }
    }

    UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_LyraAbilityReady);  // :211
    ForceNetUpdate();
}
```

`GiveToAbilitySystem` 做三件事：加 AttributeSet、授予 Ability、应用起始 GE。**其中埋了两个关键点**：

```cpp
// LyraAbilitySet.cpp:114
ULyraGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<ULyraGameplayAbility>();

FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
AbilitySpec.SourceObject = SourceObject;                                       // ① 能力知道"我属于哪个装备实例"
AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);        // ② 输入标签藏在 Spec 动态标签里

const FGameplayAbilitySpecHandle AbilitySpecHandle = LyraASC->GiveAbility(AbilitySpec);
```

`ULyraHealthSet` 作为 AttributeSet 被加到 ASC；`ULyraHealthComponent` 在 `InitializeWithAbilitySystem` 里缓存 HealthSet 并订阅 `OnHealthChanged` / `OnMaxHealthChanged` / `OnOutOfHealth`（`LyraHealthComponent.cpp:70-80`），完成 GAS 与玩法层的桥接（详见 `LYRA_LEARNING_GUIDE.md` 对 HealthSet / HealthComponent 分工的说明）。

### 9.3 阶段 3：获得武器 —— Inventory → QuickBar → Equipment

**拾取/生成**：世界武器走 `ALyraWeaponSpawner::AttemptPickUpWeapon_Implementation`（`LyraWeaponSpawner.cpp:124-142`），拿到 `WeaponDefinition->InventoryItemDefinition` 后调用蓝图事件 `GiveWeapon`。蓝图把武器做成 `ULyraInventoryItemInstance` 并放进 QuickBar 槽位。

武器的关键配置是物品上的 **`UInventoryFragment_EquippableItem`**，它把"物品实例"映射到"装备定义"：

```cpp
// LyraQuickBarComponent.cpp:86
void ULyraQuickBarComponent::EquipItemInSlot()
{
    if (ULyraInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
    {
        if (const UInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UInventoryFragment_EquippableItem>())
        {
            TSubclassOf<ULyraEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
            if (EquipDef != nullptr)
            {
                if (ULyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
                {
                    EquippedItem = EquipmentManager->EquipItem(EquipDef);   // :100
                    if (EquippedItem != nullptr)
                    {
                        EquippedItem->SetInstigator(SlotItem);               // :103
                    }
                }
            }
        }
    }
}
```

**真正"装备"的一步是 `FLyraEquipmentList::AddEntry`（服务端权威）**，它一次完成三件事：

```cpp
// LyraEquipmentManagerComponent.cpp:68
ULyraEquipmentInstance* FLyraEquipmentList::AddEntry(TSubclassOf<ULyraEquipmentDefinition> EquipmentDefinition)
{
    ...
    FLyraAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
    NewEntry.EquipmentDefinition = EquipmentDefinition;
    NewEntry.Instance = NewObject<ULyraEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);  // ① 创建装备实例
    Result = NewEntry.Instance;

    if (ULyraAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        for (const TObjectPtr<const ULyraAbilitySet>& AbilitySet : EquipmentCDO->AbilitySetsToGrant)
        {
            AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);  // ② 授予武器能力集，SourceObject=Result
        }
    }

    Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);   // ③ Spawn 出武器 Actor 并 Attach 到骨骼插槽

    MarkItemDirty(NewEntry);
    return Result;
}
```

| 步骤 | 作用 | 为什么重要 |
|------|------|-----------|
| ① `NewObject<ULyraEquipmentInstance>` | 创建装备实例（实际类型是 `ULyraWeaponInstance` / `ULyraRangedWeaponInstance` 子类） | 能力的运行期数据载体 |
| ② `GiveToAbilitySystem(..., Result)` | 授予该武器自带能力集（`GA_Weapon_Fire` 就在这被授予） | **`SourceObject` = 装备实例**，能力靠它反查武器 |
| ③ `SpawnEquipmentActors` | spawn 出真正的武器 Actor（`B_WeaponActor`） | 视觉/枪口/挂点 |

> **能力如何拿回武器？** `ULyraGameplayAbility_FromEquipment::GetAssociatedEquipment()` 读 `Spec->SourceObject`（`LyraGameplayAbility_FromEquipment.cpp:18-26`），返回的就是这里的装备实例。
>
> **网络**：`AddEntry` 里有 `check(...HasAuthority())`（`:74`），装备只在服务端执行；客户端通过 `FLyraEquipmentList` 的 FastArray 复制，在 `PostReplicatedAdd` → `OnEquipped()` 做表现（`:47`）。`GrantedHandles` 是 `NotReplicated`，只有服务端持有用于回收。

### 9.4 阶段 4：输入绑定 —— InputAction ↔ InputTag ↔ 能力

`ULyraHeroComponent::InitializePlayerInput` 在本地玩家就绪时，把输入资产里每个 InputAction 绑到对应 InputTag：

```cpp
// LyraHeroComponent.cpp:280
// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
// be triggered directly by these input actions Triggered events.
TArray<uint32> BindHandles;
LyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);   // :283
```

按键回调转发给 ASC：

```cpp
// LyraHeroComponent.cpp:343
void ULyraHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
    ...
    if (ULyraAbilitySystemComponent* LyraASC = PawnExtComp->GetLyraAbilitySystemComponent())
    {
        LyraASC->AbilityInputTagPressed(InputTag);   // :351
    }
}
```

之后由 PlayerController 每帧驱动 ASC 处理：

```cpp
// LyraPlayerController.cpp:359
void ALyraPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
    ...
    if (ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent())
    {
        LyraASC->ProcessAbilityInput(DeltaTime, bGamePaused);   // :363
    }
}
```

**三点合一是"装备 → 能力可被输入激活"的完整纽带**：
1. 授予能力时把 InputTag 塞进 `AbilitySpec.GetDynamicSpecSourceTags()`（`LyraAbilitySet.cpp:118`）；
2. 能力的 `SourceObject` = 装备实例（`LyraEquipmentManagerComponent.cpp:93`）；
3. HeroComponent 把 InputAction 绑到 InputTag → `AbilityInputTagPressed` → `ProcessAbilityInput` 里 `TryActivateAbility`。

> 能力默认 `ActivationPolicy = OnInputTriggered`（`LyraGameplayAbility.cpp:44`），枚举定义见 `LyraGameplayAbility.h:37-47`（`OnInputTriggered` / `WhileInputActive` / `OnSpawn`）。**项目里没有 `bAutoActivateOnInput` 这个符号**。
>
> ⚠️ 本工作区副本里 `Source/LyraGame/AbilitySystem/LyraAbilitySystemComponent.cpp` 是 1 字节空文件，ASC 输入分派（Tag 匹配 Spec → 收进 `InputPressedSpecHandles` → `TryActivateAbility`）的实现体不在源码内，只能看到头文件声明（`LyraAbilitySystemComponent.h:45-49, 96-102`）。

### 9.5 阶段 5：开火 —— Trace 打点

`GA_Weapon_Fire` 激活后进 C++ 基类 `ULyraGameplayAbility_RangedWeapon::ActivateAbility`：

```cpp
// LyraGameplayAbility_RangedWeapon.cpp:440
void ULyraGameplayAbility_RangedWeapon::ActivateAbility(...)
{
    // Bind target data callback
    UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
    OnTargetDataReadyCallbackDelegateHandle = MyAbilityComponent->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataReadyCallback);   // :446

    // Update the last firing time
    ULyraRangedWeaponInstance* WeaponData = GetWeaponInstance();
    WeaponData->UpdateFiringTime();   // :451

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
```

**⚠️ 关键：Lyra 的远程武器不走 `AGameplayAbilityTargetActor_Trace` / `WaitTargetData` 那一套**，而是在能力内部自建 Trace。蓝图在输入触发时调 `StartRangedWeaponTargeting()`。

**① 算弹道起点/终点**（起点用相机→焦点变换，不是枪口，这是第三人称射击手感的关键）：

```cpp
// LyraGameplayAbility_RangedWeapon.cpp:352
void ULyraGameplayAbility_RangedWeapon::PerformLocalTargeting(OUT TArray<FHitResult>& OutHits)
{
    APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
    ULyraRangedWeaponInstance* WeaponData = GetWeaponInstance();
    if (AvatarPawn && AvatarPawn->IsLocallyControlled() && WeaponData)
    {
        FRangedWeaponFiringInput InputData;
        InputData.WeaponData = WeaponData;
        InputData.bCanPlayBulletFX = (AvatarPawn->GetNetMode() != NM_DedicatedServer);

        const FTransform TargetTransform = GetTargetingTransform(AvatarPawn, ELyraAbilityTargetingSource::CameraTowardsFocus);   // :364
        InputData.AimDir = TargetTransform.GetUnitAxis(EAxis::X);
        InputData.StartTrace = TargetTransform.GetTranslation();
        InputData.EndAim = InputData.StartTrace + InputData.AimDir * WeaponData->GetMaxDamageRange();

        TraceBulletsInCartridge(InputData, /*out*/ OutHits);   // :378
    }
}
```

**② 逐弹丸 + 扩散**（霰弹靠 `BulletsPerCartridge`，扩散角由 Heat/Spread 状态机给出）：

```cpp
// LyraGameplayAbility_RangedWeapon.cpp:382
for (int32 BulletIndex = 0; BulletIndex < BulletsPerCartridge; ++BulletIndex)
{
    const float BaseSpreadAngle = WeaponData->GetCalculatedSpreadAngle();
    const float SpreadAngleMultiplier = WeaponData->GetCalculatedSpreadAngleMultiplier();
    const float ActualSpreadAngle = BaseSpreadAngle * SpreadAngleMultiplier;

    const float HalfSpreadAngleInRadians = FMath::DegreesToRadians(ActualSpreadAngle * 0.5f);
    const FVector BulletDir = VRandConeNormalDistribution(InputData.AimDir, HalfSpreadAngleInRadians, WeaponData->GetSpreadExponent());   // :397

    const FVector EndTrace = InputData.StartTrace + (BulletDir * WeaponData->GetMaxDamageRange());
    ...
    FHitResult Impact = DoSingleBulletTrace(InputData.StartTrace, EndTrace, WeaponData->GetBulletTraceSweepRadius(), /*bIsSimulated=*/ false, /*out*/ AllImpacts);   // :404
```

**扩散 / 后坐力 / 精度**都在 `ULyraRangedWeaponInstance` 里：每帧 `Tick` 驱动 `UpdateSpread` / `UpdateMultipliers`，开火时 `AddSpread` 加热：

```cpp
// LyraRangedWeaponInstance.cpp:111
void ULyraRangedWeaponInstance::AddSpread()
{
    const float HeatPerShot = HeatToHeatPerShotCurve.GetRichCurveConst()->Eval(CurrentHeat);
    CurrentHeat = ClampHeat(CurrentHeat + HeatPerShot);
    CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);
}
```

| 关注点 | 在哪实现 |
|--------|----------|
| Heat / Spread / 后坐力 / 精度乘子 | **C++** `ULyraRangedWeaponInstance`（`:73-123, 148-217`） |
| 单发 / 连发 / 射速 | **蓝图** `GA_Weapon_Fire`（用循环/延迟调 `StartRangedWeaponTargeting`） |
| Trace 通道 | `Lyra_TraceChannel_Weapon`（`LyraGameplayAbility_RangedWeapon.cpp:138-141`） |

**③ 打包 TargetData**（每条弹道一个 `SingleTargetHit`，带命中结果 + CartridgeID）：

```cpp
// LyraGameplayAbility_RangedWeapon.cpp:552
void ULyraGameplayAbility_RangedWeapon::StartRangedWeaponTargeting()
{
    ...
    FScopedPredictionWindow ScopedPrediction(MyAbilityComponent, CurrentActivationInfo.GetActivationPredictionKey());   // :566

    TArray<FHitResult> FoundHits;
    PerformLocalTargeting(/*out*/ FoundHits);

    FGameplayAbilityTargetDataHandle TargetData;
    TargetData.UniqueId = WeaponStateComponent ? WeaponStateComponent->GetUnconfirmedServerSideHitMarkerCount() : 0;   // :573

    if (FoundHits.Num() > 0)
    {
        const int32 CartridgeID = FMath::Rand();
        for (const FHitResult& FoundHit : FoundHits)
        {
            FLyraGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FLyraGameplayAbilityTargetData_SingleTargetHit();
            NewTargetData->HitResult = FoundHit;
            NewTargetData->CartridgeID = CartridgeID;
            TargetData.Add(NewTargetData);
        }
    }
    ...
    OnTargetDataReadyCallback(TargetData, FGameplayTag());   // :597
}
```

### 9.6 阶段 6：命中确认 —— 客户端预测 + 服务端验证

```cpp
// LyraGameplayAbility_RangedWeapon.cpp:477
void ULyraGameplayAbility_RangedWeapon::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
{
    ...
    FScopedPredictionWindow ScopedPrediction(MyAbilityComponent);   // :484
    FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData)));

    const bool bShouldNotifyServer = CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority();
    if (bShouldNotifyServer)
    {
        MyAbilityComponent->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), LocalTargetDataHandle, ApplicationTag, MyAbilityComponent->ScopedPredictionKey);   // :492
    }
    ...
#if WITH_SERVER_CODE
    // 服务端：把验证结果与"被替换的命中"回投客户端
    WeaponStateComponent->ClientConfirmTargetData(LocalTargetDataHandle.UniqueId, bIsTargetDataValid, HitReplaces);   // :521
#endif

    // See if we still have ammo
    if (bIsTargetDataValid && CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))   // :531
    {
        // We fired the weapon, add spread
        WeaponData->AddSpread();                          // :536
        // Let the blueprint do stuff like apply effects to the targets
        OnRangedWeaponTargetDataReady(LocalTargetDataHandle);   // :539
    }
}
```

| 环节 | 说明 |
|------|------|
| 本地玩家 | 用预测键把 TargetData 上报服务器（`CallServerSetReplicatedTargetData`） |
| 服务端 | 验证后回调 `ULyraWeaponStateComponent::ClientConfirmTargetData`（`LyraWeaponStateComponent.cpp:66-101`） |
| 客户端表现 | 命中标记：`SHitMarkerConfirmationWidget` 从 `GetLastWeaponDamageScreenLocations()` 取屏幕点绘制 |
| 扣弹药 | **`CommitAbility` 通过后才算真正开火**（消耗/冷却在这里） |

### 9.7 阶段 7：应用伤害 GE —— C++ 与蓝图的边界

`OnRangedWeaponTargetDataReady` 是 `BlueprintImplementableEvent`。**蓝图里**做：

1. 从 `ULyraGameData::DamageGameplayEffect_SetByCaller` 取伤害 GE 类（`LyraGameData.h:32-33`）；
2. `MakeOutgoingSpec` 创建 Spec，用 **`SetByCaller.Damage`**（`LyraGameplayTags.cpp:36`）写入伤害量；
3. `ApplyGameplayEffectSpecToTarget` 施加到目标。

**C++ 为这一步提供的支撑**：

| 支撑点 | 位置 | 作用 |
|--------|------|------|
| 把命中结果注入 EffectContext | `LyraGameplayAbilityTargetData_SingleTargetHit.cpp:13-22` | Execution 里能拿到 `HitResult` 算距离、取物理材质 |
| 把武器实例写成 AbilitySource | `LyraGameplayAbility.cpp:278-301, 428-440` | 距离/材质衰减的来源 |
| 武器实现 `ILyraAbilitySourceInterface` | `LyraRangedWeaponInstance.cpp:125-146` | 提供衰减曲线查询 |

> 一句话：**C++ 负责"打到哪"和"把武器信息塞进 Context"，蓝图负责"施加哪个 GE、扣多少血"。**

### 9.8 阶段 8：伤害结算 —— `ULyraDamageExecution`

GE 执行时进 `ULyraDamageExecution::Execute_Implementation`（`#if WITH_SERVER_CODE`，仅服务端）：

```cpp
// LyraDamageExecution.cpp:50
float BaseDamage = 0.0f;
ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);   // ① 捕获 CombatSet.BaseDamage

const AActor* EffectCauser = TypedContext->GetEffectCauser();
const FHitResult* HitActorResult = TypedContext->GetHitResult();      // ② 取命中结果（无则回退目标位置）
...
// ③ 友伤判定：同队 → 乘 0
float DamageInteractionAllowedMultiplier = 0.0f;
if (HitActor)
{
    ULyraTeamSubsystem* TeamSubsystem = HitActor->GetWorld()->GetSubsystem<ULyraTeamSubsystem>();
    DamageInteractionAllowedMultiplier = TeamSubsystem->CanCauseDamage(EffectCauser, HitActor) ? 1.0 : 0.0;   // :96
}
...
// ④ 距离衰减 + 物理材质衰减
if (const ILyraAbilitySourceInterface* AbilitySource = TypedContext->GetAbilitySource())
{
    if (const UPhysicalMaterial* PhysMat = TypedContext->GetPhysicalMaterial())
    {
        PhysicalMaterialAttenuation = AbilitySource->GetPhysicalMaterialAttenuation(PhysMat, SourceTags, TargetTags);
    }
    DistanceAttenuation = AbilitySource->GetDistanceAttenuation(Distance, SourceTags, TargetTags);   // :126
}
...
const float DamageDone = FMath::Max(BaseDamage * DistanceAttenuation * PhysicalMaterialAttenuation * DamageInteractionAllowedMultiplier, 0.0f);   // :131

if (DamageDone > 0.0f)
{
    // Apply a damage modifier, this gets turned into - health on the target
    OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULyraHealthSet::GetDamageAttribute(), EGameplayModOp::Additive, DamageDone));   // :136
}
```

**注意：这里只是把 `DamageDone` 写进 `ULyraHealthSet::Damage` 这个 meta 属性**（注释：*"this gets turned into -health on the target"*），并不是直接扣 Health。护甲减伤 Lyra 默认没做，只做了距离/材质衰减。

### 9.9 阶段 9：扣血 —— `ULyraHealthSet`

**① 结算前拦截**（伤害免疫 / GodMode 的统一入口）：

```cpp
// LyraHealthSet.cpp:68
bool ULyraHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data)
{
    if (Data.EvaluatedData.Attribute == GetDamageAttribute())
    {
        if (Data.EvaluatedData.Magnitude > 0.0f)
        {
            const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);

            if (Data.Target.HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
            {
                Data.EvaluatedData.Magnitude = 0.0f;   // 免疫：不扣血
                return false;
            }
#if !UE_BUILD_SHIPPING
            if (Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Cheat_GodMode) && !bIsDamageFromSelfDestruct)
            {
                Data.EvaluatedData.Magnitude = 0.0f;   // 作弊无敌
                return false;
            }
#endif
        }
    }
    ...
    return true;
}
```

**② meta `Damage` → `Health`**（血才真正扣掉）：

```cpp
// LyraHealthSet.cpp:128
if (Data.EvaluatedData.Attribute == GetDamageAttribute())
{
    // Send a standardized verb message that other systems can observe
    if (Data.EvaluatedData.Magnitude > 0.0f)
    {
        FLyraVerbMessage Message;
        Message.Verb = TAG_Lyra_Damage_Message;   // "Lyra.Damage.Message"
        ...
        MessageSystem.BroadcastMessage(Message.Verb, Message);
    }

    // Convert into -Health and then clamp
    SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));   // :148
    SetDamage(0.0f);
}
...
if (GetHealth() != HealthBeforeAttributeChange)
{
    OnHealthChanged.Broadcast(...);   // :173
}

if ((GetHealth() <= 0.0f) && !bOutOfHealth)
{
    OnOutOfHealth.Broadcast(...);     // :178  ← 死亡流程的入口
}
```

### 9.10 阶段 10：死亡 —— `HealthComponent` + `GA_Death`

`ULyraHealthComponent` 订阅了 `OnOutOfHealth`，回调解锁死亡流程：

```cpp
// LyraHealthComponent.cpp:148
void ULyraHealthComponent::HandleOutOfHealth(...)
{
#if WITH_SERVER_CODE
    if (AbilitySystemComponent && DamageEffectSpec)
    {
        // ① 发 "GameplayEvent.Death" → 自动激活 GA_Death
        {
            FGameplayEventData Payload;
            Payload.EventTag = LyraGameplayTags::GameplayEvent_Death;   // :156
            ...
            FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
            AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);   // :166
        }

        // ② 广播 "Lyra.Elimination.Message" → 计分/连杀/助攻
        {
            FLyraVerbMessage Message;
            Message.Verb = TAG_Lyra_Elimination_Message;   // :172
            ...
            MessageSystem.BroadcastMessage(Message.Verb, Message);
        }
    }
#endif
}
```

`GA_Death` 的 TriggerTag 就是 `GameplayEvent.Death`，被上面的 `HandleGameplayEvent` 自动激活：

```cpp
// LyraGameplayAbility_Death.cpp:22
if (HasAnyFlags(RF_ClassDefaultObject))
{
    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = LyraGameplayTags::GameplayEvent_Death;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
}
```

`GA_Death` → `HealthComponent` 状态机 → 打 tag / 广播委托：

```cpp
// LyraGameplayAbility_Death.cpp:70
void ULyraGameplayAbility_Death::StartDeath()
{
    if (ULyraHealthComponent* HealthComponent = ULyraHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
    {
        if (HealthComponent->GetDeathState() == ELyraDeathState::NotDead)
        {
            HealthComponent->StartDeath();
        }
    }
}

// LyraHealthComponent.cpp:235
void ULyraHealthComponent::StartDeath()
{
    ...
    DeathState = ELyraDeathState::DeathStarted;
    AbilitySystemComponent->SetLooseGameplayTagCount(LyraGameplayTags::Status_Death_Dying, 1);   // :246
    OnDeathStarted.Broadcast(Owner);
    Owner->ForceNetUpdate();
}
```

> **复制的真相**：死亡状态靠 `UPROPERTY(ReplicatedUsing = OnRep_DeathState) ELyraDeathState DeathState` 复制（`LyraHealthComponent.h:128-130`）；`Status.Death.Dying/Dead` 这两个 **loose tag 是本地派生的镜像**，客户端在 `OnRep_DeathState` 里跑同一套 `StartDeath`/`FinishDeath` 得到，不是靠 tag 自身复制。`ClearGameplayTags()` 的存在，是因为 ASC 挂在 PlayerState 上、跨 Pawn 复用，必须在初始化/反初始化时清掉残留 tag（`LyraHealthComponent.cpp:106-113, 85, 93`）。

### 9.11 阶段 11：反馈与计分

| 反馈 | 触发点 |
|------|--------|
| 伤害飘字 / 特效 / 音效 | 伤害 GameplayCue（蓝图）→ `ULyraNumberPopComponent::AddNumberPop`（`LyraNumberPopComponent.h:54-56`），实现见 `LyraNumberPopComponent_NiagaraText` / `_MeshText` |
| 命中标记 / 准星 | `SHitMarkerConfirmationWidget`、`ULyraReticleWidgetBase`（消费 `ULyraWeaponStateComponent`） |
| 伤害统计 | `Lyra.Damage.Message` ← `LyraDamageLogDebuggerComponent.cpp:26, 84-98` |
| 击杀 / 连杀 / 助攻 | `Lyra.Elimination.Message` ← ShooterCore 的 `ElimStreakProcessor` / `ElimChainProcessor` / `AssistProcessor` |
| 消息复制到 HUD | `LyraPlayerState::ClientBroadcastMessage` / `LyraVerbMessageReplication::RebroadcastMessage` |

### 9.12 客户端 / 服务端职责划分

| 环节 | 本地客户端 | 服务端 |
|------|-----------|--------|
| 装备武器 | 收 FastArray 复制 → `OnEquipped` 表现 | 真正执行 `AddEntry`（`HasAuthority`） |
| Trace 打点 | **本地预测执行**（`IsLocallyControlled`） | 收到上报后验证 |
| 上报 / 确认 | `CallServerSetReplicatedTargetData` | `ClientConfirmTargetData` 回投命中标记 |
| 伤害 GE | 预测施加（蓝图） | 权威施加 |
| 伤害 Execution | —— | `#if WITH_SERVER_CODE` 仅服务端结算 |
| 扣血 / 死亡事件 | `OnRep_Health` / `OnRep_DeathState` 本地派生 | 权威结算 + 复制 |
| 飘字 / 命中标记 / HUD | 客户端表现 | —— |

### 9.13 需要到蓝图确认的环节

1. **`GA_Weapon_Fire`**：单发/连发/射速控制，以及 `OnRangedWeaponTargetDataReady` 里"构造 GE Spec + `ApplyGameplayEffectSpecToTarget`"。
2. **伤害 GameplayCue（GCN_Damage 之类）**：`AddNumberPop` 的调用者。

其余逻辑全部在 C++，按上述 11 个阶段即可。

---

## 10. 调试技巧

### 10.1 日志

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

### 10.2 控制台命令

| 命令 | 作用 |
|------|------|
| `Lyra.DumpLoadedAssets` | 列出 AssetManager 常驻内存的资源（排查内存） |
| `lyra.chaos.ExperienceDelayLoad.MinSecs 2` | Experience 加载延迟 2 秒 |
| `lyra.chaos.ExperienceDelayLoad.RandomSecs 3` | 随机延迟 0~3 秒 |
| `GameplayMessageSubsystem.LogMessages 1` | 打印所有消息总线消息 |
| `-LogAssetLoads`（启动参数） | 打印每个资源的同步加载耗时 |

### 10.3 断点速查

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
| 装备授予了哪些能力 | `ULyraAbilitySet::GiveToAbilitySystem()` |
| 输入有没有传到 ASC | `ULyraHeroComponent::Input_AbilityInputTagPressed()` |
| 开火 Trace 打了哪 | `ULyraGameplayAbility_RangedWeapon::PerformLocalTargeting()` |
| 命中确认走没走通 | `ULyraGameplayAbility_RangedWeapon::OnTargetDataReadyCallback()` |
| 伤害计算 | `ULyraDamageExecution::Execute_Implementation()` |
| 血是怎么扣的 | `ULyraHealthSet::PostGameplayEffectExecute()` |
| 死亡事件从哪来 | `ULyraHealthComponent::HandleOutOfHealth()` |

### 10.4 诊断"Pawn 没生成"的流程

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
| **伤害链路** | 装备武器 → 输入激活 GA → 本地 Trace → 预测上报 → 伤害 GE → 距离/材质衰减 → HealthSet 扣血 → 死亡事件 → Cue/计分 |

---

*本文档基于 UE 5.5 版本 Lyra Starter Game 整理。*
