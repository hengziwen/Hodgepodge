# Hodgepodge

> 一个基于 **UE 5.5** 的个人游戏框架，目标是"**Lyra 的架构理念 + GAS 的战斗**"，最终形态是 **UE Dedicated Server + 数据驱动的动作 RPG 底层框架**。

| 项目 | 值 |
|---|---|
| 引擎版本 | Unreal Engine **5.5**（本机核实 5.5.4） |
| 主模块 | `Hodgepodge`（Runtime，单模块；另有 `CodexText` 实验代码同模块内） |
| 代码规模 | `Source/Hodgepodge` 共 **158 个 C++ 文件**（79 `.h` + 79 `.cpp`）+ 3 个 `.cs`，约 **23100 行** |
| 核心依赖 | GameplayAbilities、GameFeatures、EnhancedInput、**ModularGameplay**、AnimationWarping、ControlRig、**UMG / SlateCore** |
| 项目阶段 | 🚧 **战斗 MVP 已成型**：普攻连招 / 逻辑时间轴 / 生命与死亡链路已落地，`Docs/Validation/` 有两份 PIE 验证记录；伤害数值管线仍有缺口 |
| 相关文档 | [`Docs/KnowledgeBase/README.md`](Docs/KnowledgeBase/README.md)（**已核对的当前事实、断点与验证边界**）、[`Docs/Design/`](Docs/Design)（设计草案）、[`Docs/Validation/`](Docs/Validation)（验证记录）、[`LYRA_LEARNING_GUIDE.md`](LYRA_LEARNING_GUIDE.md)、[`LYRA_RUNTIME_FLOW.md`](LYRA_RUNTIME_FLOW.md) |
| 代码基线 | 提交 `10305c2`（工作区干净） |

---

## 当前状态

项目处于 **战斗 MVP 已成型** 的阶段。框架层（Experience / AssetManager / GAS / 角色 / 相机 / 输入 / PlayerController）是 Lyra 的等价实现，**战斗链路（普攻连招 → 逻辑时间轴 → 伤害 → 死亡）也已落地**，并有两份 PIE 验证记录。

**编译状态**：✅ 通过（`10305c2`，Development Editor，UBT 报 `Target is up to date`）。

### 本轮变化（`4db0536` → `10305c2`，10 个提交）

| 变更 | 说明 |
|---|---|
| **普攻连招** 🆕 | `UHodgeGameplayAbility_BasicAttack`：5 段连招；输入缓冲（`WaitInputPress`）、窗口驱动接段（`Status.Attack.Cancel.NextAttack`）、移动取消、后摇取消。见 [§6.17](#617-战斗系统普攻与逻辑时间轴) |
| **逻辑时间轴** 🆕 | `UHodgeAbilityTimeline`（DataAsset）+ `UHodgeAbilityTask_PlayTimeline`（AbilityTask）：把"逻辑时刻"（窗口 Tag / GameplayEvent / GE）与动画表现分离 —— **Montage 只管动画，时间轴管逻辑**。含 3 个自动化测试 |
| **等待移动取消** 🆕 | `UHodgeAbilityTask_WaitMoveCancel`：双信号（取消窗口 Tag + HeroComponent 的移动意图）同时成立才取消 |
| **伤害 / 治疗管线** 🆕 | `HodgeDamageExecution` / `HodgeHealExecution`（GE Execution）+ `UHodgeCombatSet`（`BaseDamage` / `BaseHeal`）。见 [§6.18](#618-伤害与死亡) |
| **生命组件与死亡流程** ✅ | `UHodgeHealthComponent`（`UGameFrameworkComponent`）已挂到 `AHodgeCombatCharacter`：监听 HealthSet → `DeathState` 状态机（NotDead → DeathStarted → DeathFinished）→ 禁用移动/碰撞 → 下一帧销毁 |
| **Tag 体系大扩充** | 新增 `Config/DefaultGameplayTags.ini`（286 条 Lyra 对照迁移，登记 **123 条非原生 Tag**）；C++ 原生 Tag 新增 `Status.Attack.*`、`GameplayEvent.Attack/Timeline.End/Interrupted`、`Status.Death.*`、`SetByCaller.Damage/Heal` 等 |
| **GE 配置进 GameData** | `UHodgeGameData` 新增 `DamageGameplayEffect_SetByCaller` / `HealGameplayEffect_SetByCaller` / `DynamicTagGameplayEffect` 字段，`DA_Dafult_GameData` 里已配 |
| **相机曲线资产** | `Content/Characters/Cameras/` 新增 `ThirdPersonOffsetCurve` / `ThirdPersonDeathOffsetCurve` |
| **本地知识库 + 设计/验证文档** 🆕 | `Docs/KnowledgeBase/`（45 个 md + `Tools/kb.py`）、`Docs/Design/`（4 篇设计）、`Docs/Validation/`（2 篇 PIE 验证记录） |
| **第二个 MCP 插件** | `Plugins/McpAutomationBridge`（ChiR24，HTTP MCP `127.0.0.1:3016`）与 `UnrealMCP` 并行共存 |
| **CodexText 扩充** | 6 个实验类；`Content/CodexText/` 增加大量动画资产（含 6 向移动与攻击 Montage） |

### 已验证 ✅（见 [`Docs/Validation/`](Docs/Validation)）

| 验证记录 | 结论 |
|---|---|
| `basic-attack-2026-09-24.md` | `GA_BasicAttack` 五段连击：单人 PIE 12 项 + Listen Server（客户端 36 项 / 主机 16 项）全部通过。**该次验证不含命中与伤害** |
| `timeline-2026-09-24.md` | `Hodge.Timeline` 三项自动化测试 + PIE 九项断言通过（窗口 GE 0→1→0、Point / End 派发、取消清理幂等），Editor / Game 构建退出码 0 |

### 已通电 / 待验证 / 未建

| 系统 | 状态 | 说明 |
|---|---|---|
| Experience 全链路 | ✅ | 状态机、Bundle 加载、GameFeature 激活、GameMode 流程；资产 `Exp_HodgeDefaultExperience` |
| AssetManager | ✅ | StartupJob 进度、GameData 缓存、常驻资源池、PIE 预加载 |
| 玩家 ASC（单入口） | ✅ | `AHodgePlayerState` 持有；Avatar 由 **HeroComponent → PawnExtension** 单一入口绑定 |
| 游戏级 ASC | ✅ | `AHodgeGameState` 持有（Owner = Avatar = GameState），暂无使用者 |
| Init State 链 | ✅ | `PawnExtension` 与 `Hero` 两个 Feature **均已挂载**（HeroComponent 由 C++ 创建），PawnData 由 GameMode 注入 |
| AbilitySystemGlobals | ✅ | 自定义 Globals 已配，EffectContext 的 `check` 崩溃已消除 |
| 主 PlayerController | ✅ | `AHodgePlayerController` 为默认 PC；相机管理器 / 观战 / Replay / `ProcessAbilityInput` 就位 |
| 输入主链 | ✅ | HeroComponent 绑定原生 / 能力输入；`PostProcessInput → ProcessAbilityInput` 打通（❓ 资产配置待编辑器确认） |
| 相机主链 | ✅ | 委托由 HeroComponent 绑定；模式资产 `CM_ThirdPerson` / `CM_ThirdPerson_Death` + 两条曲线资产（❓ PawnData 实际指向待确认） |
| AbilitySet 基础授予 | ✅ | 权威端遍历 `PawnData->AbilitySets` 授予（**无句柄，不可撤销**） |
| **普攻连招** | ✅ 已验证 | `UHodgeGameplayAbility_BasicAttack` 五段连击，见 `basic-attack-2026-09-24.md` |
| **逻辑时间轴** | ✅ 已验证 | `HodgeAbilityTimeline` + `PlayTimeline` Task + 自动化测试，见 `timeline-2026-09-24.md` |
| **生命 / 死亡流程** | ✅ | `UHodgeHealthComponent` 已挂载；`OnOutOfHealth → StartDeath → FinishDeath → UninitAndDestroy` 串联 |
| **伤害 / 治疗结算** | ⚠️ | HealthSet 的 Meta 属性结算闭环 ✅；但 `DamageExecution` 的敌我判定整段被注释 → **当前算出来的伤害恒为 0**（见下） |
| Pawn 生成 | ❓ | `DefaultPawnClass` 回退仍是 `AHodgeCharacterBase`；最终看 `PawnData->PawnClass`（二进制，需编辑器确认） |
| 敌人 | ❌ | `AHodgeEnemyCharacter` 只设置 AI 自动控制，无 ASC、无战斗 |
| Cue 路径增删 | ❌ | Observer 注册行仍是注释，预加载钩子空实现 |
| UI / Load Screen / AI / 背包 / DS | ❌ | 完全没有（CodexText 的 UMG 是独立实验） |

### 当前剩余的关键缺口

| 缺口 | 说明 |
|---|---|
| **伤害恒为 0** 🔴 | `HodgeDamageExecution` 里用 TeamSubsystem 判敌我的整段被注释，`DamageInteractionAllowedMultiplier` 恒为 `0.0f` → **任何攻击都打不掉血**。这是"战斗闭环"目前最大的实际缺口 |
| **AbilitySet 只授不撤** 🔴 | `GiveToAbilitySystem(ASC, nullptr)` 没收集 `GrantedHandles`；换装 / 重生 / 撤销前必须补上按来源保存的句柄 |
| **编辑器确认资产接线** 🔴 | `DA_Dafult_PawnData`（`PawnClass` / `InputConfig` / `DefaultCameraMode` / `AbilitySets`）、`BP_Hero_Pover` 的 `DefaultInputMappings`、`DA_HodgeInputConfig` 的映射、`GA_BasicAttack` 的 `AttackSteps`。文本无法读取 `.uasset` 内部值 |
| **无防御 / 暴击 / 伤害类型** 🟠 | `DamageExecution` 目前只做 `BaseDamage × 距离衰减 × 物理材质衰减`，没有 Defense、暴击、`DamageType.*` 分支 |
| **敌人 ASC 缺初始化** 🟠 | 先定 ASC 所有者（PlayerState 还是 Character），再验服务器伤害目标与远端状态 |
| **Cue 路径没注册** 🟠 | `HodgeGameFeaturePolicy.cpp:39` 创建 `UHodgeGameFeature_AddGameplayCuePaths` Observer 的那行仍注释；`OnGameFeatureUnregistering` 与 `UHodgeAssetManager::InitializeGameplayCueManager()` 仍是空实现 |
| **淘汰 / 击杀消息未接** 🟠 | `UHodgeHealthComponent::HandleOutOfHealth` 里 Elimination / Verb Message 广播整段被注释 |

> Experience 链路本身完好，默认 Experience 是 `Exp_HodgeDefaultExperience`，换玩法用 `-Experience=<资产名>`。

> 研发过程记录见 [`Docs/KnowledgeBase/`](Docs/KnowledgeBase/README.md) 的 19~23 号"每日更新"，以及 `Docs/Design/` 的 4 篇设计草案。

> ⚠️ 关于"攻击时间轴 / 连击（AbilityTimeline / ComboSet）"：`Docs/KnowledgeBase/21-update-2026-09-17.md` 记录过这套实现，但它属于**当时工作区的未提交改动**，当前工作区已不存在对应源码与配置扫描项（全仓库搜不到 `HodgeAbilityTimeline` / `HodgeComboSet` / `PlayTimeline`）。本 README 以**实际工作区**为准；设计意图另见 [`Docs/Design/ability-timeline.md`](Docs/Design/ability-timeline.md)。

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
- **不是稳定可运行的状态** —— 能跑但有不少技术债，见 [§7 进度](#7-当前进度) 和 [§11 技术债](#11-已知问题与技术债)

### 1.3 从 Lyra 借来的设计信条

这些是本项目的"宪法"。完整推导见 [`LYRA_LEARNING_GUIDE.md` 第 4 章](LYRA_LEARNING_GUIDE.md#4-lyra-的十大核心设计理念)。

| # | 信条 | 本项目对应实现 | 状态 |
|---|---|---|---|
| 1 | **一套代码，多种玩法** | `UHodgeExperienceDefinition` + `AHodgeGameModeBase` 完整流程 | ✅ 已跑通 |
| 2 | **数据驱动** | `UHodgePawnData`（`PawnClass` / `AbilitySets` / `TagRelationshipMapping` / `InputConfig` / `DefaultCameraMode`）/ `UHodgeGameData` / `UHodgeInputConfig` | 🚧 结构齐了，`AbilitySets` 也真的会被授予；资产内部赋值待编辑器确认 |
| 3 | **插件化扩展** | `UHodgeGameFeaturePolicy` + `GameFeatureAction_*` | 🚧 Policy 完成，5 个 Action 可用（`_AddWidget` 仍注释），还没有插件实例 |
| 4 | **GameplayTag 作万能胶水** | `HodgeGameplayTags.h` + `FGameplayTagStackContainer` | ✅ |
| 5 | **组合优于继承** | `PawnData` 决定 Pawn 类、ModularGameplay 组件化 | ✅ |
| 6 | **服务器权威** | ASC 放 PlayerState、`Mixed` 复制模式 | ✅ |
| 7 | **Base / Concrete 分层** | `HodgeGameStateBase → HodgeGameState`、`HodgePlayerStateBase → HodgePlayerState` | ✅ |
| 8 | **Init State 链解耦异步依赖** | `UGameFrameworkComponentManager` + 4 个 InitState Tag + `PawnExtensionComponent` / `HeroComponent` | ✅ 两个 Feature 都已挂载，角色侧 ASC 走单一入口 |

### 1.4 与 Lyra 的关键分歧

| 维度 | Lyra | Hodgepodge | 原因 |
|---|---|---|---|
| **Locomotion** | 自研 `LyraCharacterMovementComponent` + 完整动画层 | `UHodgeCharacterMovementComponent` 只做地面信息缓存与移动扩展；动画层靠 `UHodgeAnimInstance` 的 Tag 驱动重建，另有 `CodexText` 的 6 向移动实验（见 [§6.16](#616-codextext-实验模块)） | 移动与动画是自研重点，目前仍是引擎默认物理 + 少量扩展 |
| **Camera** | 相机模式栈（`LyraCamera` 模块） | **完整移植**：`UHodgeCameraComponent` + `UHodgeCameraMode` + `UHodgeCameraMode_ThirdPerson` + `AHodgePlayerCameraManager`；默认偏移改由蓝图曲线提供 | 相机是 Lyra 里最独立、最好移植的部分之一 |
| **Pawn 与 GAS 的协调** | `ULyraPawnExtensionComponent` + `ULyraHeroComponent` 双 Feature | 两个 Feature 都已在 C++ 挂载，HeroComponent 统一负责输入 / 相机 / ASC 入口 | 已与 Lyra 对齐，仅换 Pawn 与重生的清理顺序待运行验证 |
| **UI / 设置 / 登录** | CommonUI + UIExtension + GameSettings + CommonUser 全家桶 | **框架层全部没有**；`CodexText` 实验模块自带一套纯 UMG 界面 | 体量大、非核心矛盾，延后引入 |

---

## 2. 技术栈与依赖

### 2.1 引擎与模块依赖

`Source/Hodgepodge/Hodgepodge.Build.cs`：

```csharp
PublicDependencyModuleNames:  Core, CoreUObject, Engine, InputCore,
                              GameplayAbilities, GameplayTags, GameplayTasks,
                              ModularGameplay, GameFeatures,
                              AIModule, EngineSettings, NetCore,
                              AnimGraphRuntime, RigVM, ControlRig,
                              UMG, SlateCore
PrivateDependencyModuleNames: EnhancedInput, PhysicsCore, Niagara, SignificanceManager
Editor 专用（Target.bBuildEditor）: UnrealEd, AnimGraph, BlueprintGraph
```

另外 `SetupIrisSupport(Target)` 只在构建层打开 Iris 支持，**不代表运行时已启用 Iris**。

`Hodgepodge.uproject` 启用的插件：`GameplayAbilities`、`GameFeatures`、`AnimationLocomotionLibrary`、`AnimationWarping`、`ModelingToolsEditorMode`（仅 Editor）、`McpAutomationBridge`（仅 Editor）；并显式 **禁用 `UNTLink`**。

> ⚠️ 一个小提示（不影响编译）：模块依赖 `ModularGameplay` / `SignificanceManager`，但 `.uproject` 的 `Plugins` 段没把这两个**引擎插件**声明出来，UBT 可能提示。

### 2.2 插件清单

| 插件 | 版本 | 位置 | 说明 |
|---|---|---|---|
| **GameplayAbilities** | 引擎自带 | 引擎插件 | ✅ GAS 本体 |
| **GameFeatures** | 引擎自带 | 引擎插件 | ✅ 玩法热插拔 |
| **AnimationWarping** | 引擎自带 | 引擎插件 | ✅ 动画变形 / 步法 |
| **AnimationLocomotionLibrary** | 引擎自带 | 引擎插件 | ✅ 动画 locomotion 工具库 |

| **McpAutomationBridge** 🆕 | 第三方 | `Plugins/McpAutomationBridge/` | 编辑器 MCP 桥（Editor-only）。见 [§12.6](#126-ai-辅助开发工具链) |
| **RiderLink** | 2025.2.2.1 | `Plugins/Developer/RiderLink/` | Rider 联动，不参与游戏逻辑 |
| **UnrealMCP** | 第三方（MIT） | `Plugins/UnrealMCP/` | 编辑器 MCP 服务端（`127.0.0.1:55557`，仅 Editor，主模块不依赖）。见 [§12.6](#126-ai-辅助开发工具链) |
| **UNTLink** | — | — | ⚠️ `.uproject` 中 `Enabled: false`，已显式禁用 |

### 2.3 未引入的 Lyra 子系统

以下在 Lyra 里是标配，但**本项目一个都没有**。看到 `#include` 它们的代码，一定是编译不过的死代码。

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
    ├── Public/       ← 79 个头文件
    └── Private/      ← 79 个实现文件，与 Public 大体镜像
```

| 目录 | 文件数 | 职责 | 重要度 |
|---|---|---|---|
| `AbilitySystem/` | 18 | ASC、**Ability + AbilityCost + BasicAttack**、**AbilityTask（PlayTimeline / WaitMoveCancel）**、**Executions 2 个**、AttributeSet 3 个、TagRelationship、**AbilitySystemGlobals**、GameplayCueManager、EffectContext、GlobalAbilitySystem、GameplayTags、GameplayTagStack | ★★★★★ |
| `Component/` | 9 | **PawnExtensionComponent** / **HeroComponent** / **HealthComponent** / ExperienceManagerComponent / 角色移动组件 + 4 个组件基类 | ★★★★★ |
| `Core/` | 10 | GameInstance / GameMode / **GameState + GameStateBase** / **PlayerController + Base** / **PlayerState + PlayerStateBase** / HUD / LocalPlayer | ★★★★★ |
| `Data/` | 9 | AssetManager、GameData、PawnData、**AbilitySet、AbilityTimeline**、Experience 三件套、ExperienceManager | ★★★★★ |
| `Camera/` | 7 | 相机模式栈整套（Lyra 移植） | ★★★★ |
| `Character/` | 4 | 角色继承链（Base → Combat → Hero / Enemy） | ★★★★★ |
| `Input/` | 6 | InputConfig（数据）+ InputComponent（绑定）+ UserSettings / MappableKeyProfile / Modifiers / AimSensitivity | ★★★★ |
| `GameFeatures/` | 8 | 7 个 GameFeatureAction + Policy | ★★（4 个可用） |
| `CodexText/` 🆕 | 4 | **隔离实验模块**：ALS 6 向动画 / 平地起停脚锁 / LocomotionLab / Survivor 小玩法，直接继承引擎类，主体系无引用（见 [§6.16](#616-codextext-实验模块)） | ★（实验） |
| `Interface/` | 2 | `AbilitySourceInterface` + `LoadingProcessInterface` | ★ |
| `Animation/` | 1 | `UHodgeAnimInstance`（GameplayTag 映射动画变量） | ★★★★ |
| `Actor/` | 1 | Actor 基类 | ★ |

### 3.2 内容目录

```
Content/
├── Main/                    ★ 项目自有内容，新东西放这里
│   ├── Experiences/         Exp_HodgeDefaultExperience（当前默认）
│   ├── Data/                DA_Dafult_GameData（含 Damage/Heal/DynamicTag GE）/ DA_Dafult_PawnData / DA_Pover
│   ├── Camera/              CM_ThirdPerson / CM_ThirdPerson_Death
│   ├── Input/               DA_HodgeInputConfig / IMC_Default / IMC_UI
│   │   └── InputAction/     IA_Move / IA_Look / IA_Aim / IA_Crouch / IA_Sprint / … / IA_Attack
│   └── Character/           BP_HeroBase / BP_Hero_Pover / GA_BasicAttack
│       ├── Hero/            Anim/ABP_Pover_Base（漂泊者动画蓝图）+ Anim/Layer/
│       └── EnemyBase/       ABP_Enemy_Base / ALI_Enemy
├── CodexText/               ⚠️ AI / MCP 演练 + 动画实验产物（含 6 向移动与攻击 Montage，见 §6.16）
│                            L_MainMenu / L_Character / L_SurvivalHUD / L_Terminal + WBP_* UMG + Montage/
├── qiuyuan/                 漂泊者角色资源（285）
├── Wuwa/                    鸣潮风格资源包（675，含 322 个 fbx 模型）
├── Assets/                  通用资产：Enemies / HeroCharacter / Weapons /
│                            Niagara / Sounds / Textures / Meshes / MaterialFunctions（645）
├── Characters/              角色资源（148）+ Cameras/ 相机偏移曲线（ThirdPersonOffsetCurve / Death 版）
├── Collections/ Developers/ LevelPrototyping/   编辑器辅助目录
└── ThirdPerson/             UE 模板内容（游戏默认地图在这里）
```

> `__ExternalActors__/`、`__ExternalObjects__/` 是启用世界分区（World Partition）后关卡对象外置产生的，别手动改。

### 3.3 仓库根目录的其他内容

```
Plugins/
├── McpAutomationBridge/   编辑器 MCP 桥（ChiR24，Editor-only，HTTP MCP 3016）
├── UnrealMCP/             编辑器 MCP 插件（第三方，MIT，TCP 55557）
└── Developer/RiderLink/   Rider 联动
Tools/                     MCP 与实验工具前端（UnrealMCP / ChiR24MCP / LocomotionLab / ModelRepair）
Docs/
├── KnowledgeBase/         ★ 已核对的本地知识库（01~23 章节 + Reference/ 自动索引 + tools/kb.py）
├── Design/                设计草案（ability-timeline / stage1 / combo-graph / combo 实现计划）
├── Validation/            PIE 验证记录（basic-attack / timeline，2026-09-24）
└── AI_DEVELOPMENT.md      AI 开发与验证流程
TagMigration/              Lyra GameplayTag 对照表 + 可直接导入的 ini
AGENTS.md                  AI 协作规则（项目级）
.agents/                   工程事实快照（给 AI 读）
.cursor/rules/             Cursor 规则指针
.codex/config.toml         Codex 的 MCP 服务端注册
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

以下命令取自 [`Docs/AI_DEVELOPMENT.md`](Docs/AI_DEVELOPMENT.md) 的验证流程，**路径按本机实测填写（`D:\UE_5.5` + `D:\Hodgepodge`），换机器必须改**。外部构建前先保存并关闭本项目编辑器，避免 DLL 占用。

```powershell
$ueRoot      = 'D:\UE_5.5'
$projectFile = 'D:\Hodgepodge\Hodgepodge.uproject'

# 1) Editor 构建（日常改 C++ 走这条，按需跑 UHT / 编译 / 链接）
& "$ueRoot\Engine\Build\BatchFiles\Build.bat" HodgepodgeEditor Win64 Development "-Project=$projectFile" -WaitMutex -architecture=x64
if ($LASTEXITCODE -ne 0) { throw "Editor build failed: $LASTEXITCODE" }

# 2) Game 构建（改动 Runtime 代码 / 模块依赖 / 插件 / 平台条件后再跑，避免 Editor 环境掩盖依赖问题）
& "$ueRoot\Engine\Build\BatchFiles\Build.bat" Hodgepodge Win64 Development "-Project=$projectFile" -WaitMutex -architecture=x64
if ($LASTEXITCODE -ne 0) { throw "Game build failed: $LASTEXITCODE" }

# 3) 打开编辑器
& "$ueRoot\Engine\Binaries\Win64\UnrealEditor.exe" $projectFile
```

> 新增 / 删除 C++ 文件、Target、模块或插件后，用 `.uproject` 的 “Generate Visual Studio project files” 刷新 IDE 工程（本机未发现可用的 `GenerateProjectFiles.bat` 脚本路径，别照抄旧命令）。现有 `.sln` 与 `Intermediate` 是生成物，不手工改。

### 4.3 运行检查清单

**编译已验证**（`10305c2`，Development Editor）。如果启动异常，按这个顺序查：

1. **Experience 扫得到吗？** 日志搜 `Identified experience ... (Source: Default)`。
   扫不到就检查 `DefaultGame.ini` 里 `HodgeExperienceDefinition` 的扫描目录是否是 `/Game/Main/Experiences`，以及 `Exp_HodgeDefaultExperience.uasset` 是否在那儿。

2. **`DefaultEngine.ini` 的 `AssetManagerClassName`** 必须是 `/Script/Hodgepodge.HodgeAssetManager`，错了会直接 `UE_LOG(Fatal)` 退出。

3. **`DefaultGame.ini` 的 `[/Script/HodgePodge.HodgeAssetManager]` 段**：
   ```ini
   HodgeGameDataPath=/Game/Main/Data/DA_Dafult_GameData.DA_Dafult_GameData
   DefaultPawnData=/Game/Main/Data/DA_Dafult_PawnData.DA_Dafult_PawnData
   ```
   GameData 加载失败是 **Fatal**，不是 Warning。

4. **`DefaultGame.ini` 的 `[/Script/GameplayAbilities.AbilitySystemGlobals]` 段**：
   ```ini
   AbilitySystemGlobalsClassName=/Script/Hodgepodge.HodgeAbilitySystemGlobals
   GlobalGameplayCueManagerClass=/Script/Hodgepodge.HodgeGameplayCueManager
   GameFeaturesManagerClassName=/Script/Hodgepodge.HodgeGameFeaturePolicy
   ```

5. **`DefaultInput.ini` 的输入组件类** 必须是 `/Script/Hodgepodge.HodgeInputComponent`（HeroComponent 里 `CastChecked<UHodgeInputComponent>` 依赖它）。

6. **默认 PlayerController**：`AHodgeGameModeBase` 构造里设了 `PlayerControllerClass = AHodgePlayerController`；`DefaultEngine.ini` 另有 `[/Script/Engine.Engine] LocalPlayerClassName=/Script/Hodgepodge.HodgeLocalPlayerBase`。

7. **`GlobalDefaultGameMode` 用的是旧类名** `/Script/Hodgepodge.HodgepodgeGameModeBase`，靠 `[CoreRedirects]` 生效。能跑，但建议改成 `HodgeGameModeBase`。

8. **地图**：`GameDefaultMap` = `/Game/ThirdPerson/Maps/ThirdPersonMap`（仍是 UE 模板图，项目还没有自己的正式关卡）；`EditorStartupMap` = `/Game/CodexText/L_MainMenu`（CodexText 实验关卡，**两者已经不一致**）。

9. **`Config/DefaultGameplayTags.ini`** 已登记 123 条非原生 Tag（从 Lyra 286 条对照迁移而来）；C++ 侧原生 Tag 走 `UE_DECLARE/DEFINE_GAMEPLAY_TAG`。缺 Tag 时先查这个文件。

10. **MCP 插件配置**（`DefaultGame.ini` 新增节，可选功能）：`[/Script/McpAutomationBridge.McpAutomationBridgeSettings]` —— `bEnableNativeMCP` / `NativeMCPPort=3016` / `ListenPorts=8116` / `bRequireCapabilityToken=True`。不跑 AI 工具链时可以忽略。

11. **HeroComponent 不需要在蓝图里加**：`UHodgeHeroComponent` 已由 `AHodgeHeroCharacter` 构造在 C++ 挂载。若蓝图 `BP_HeroBase` / `BP_Hero_Pover` 里也手工加过，会出现两个组件，需在编辑器里清掉一个。

### 4.4 切换玩法

现在只有 `Exp_HodgeDefaultExperience` 一个 Experience。要试多种玩法：

```powershell
# 命令行指定（优先级高于 WorldSettings）
UnrealEditor.exe Hodgepodge.uproject -game -Experience=Exp_HodgeDefaultExperience
```

或在编辑器里新建：`Content/Main/Experiences/` 下右键 → Blueprint Class → 父类选 `HodgeExperienceDefinition` → 配 `DefaultPawnData` / `GameFeaturesToEnable` / `Actions`。

> 新建的 Experience 会**自动被扫描到**（扫描项是整目录 `/Game/Main/Experiences`，`bHasBlueprintClasses=True`），不用改配置。

**验证是否生效**：日志过滤 `Identified experience`，`Source:` 字段会告诉你是从 OptionsString / CommandLine / WorldSettings / Default 哪一级来的。

---

## 5. 架构总览

### 5.1 分层图

```
┌──────────────────────────────────────────────────────────────┐
│  内容层    Content/Main  +  GameFeature 插件（尚未建立）        │
│           数据资产组合出的玩法，不改 C++ 代码                    │
├──────────────────────────────────────────────────────────────┤
│  玩法层    AbilitySystem/  Character/  Camera/  Animation/     │
│           Component/  GameFeatures/  Input/                    │
│           （战斗、装备、背包 — 大部分待建）                       │
├──────────────────────────────────────────────────────────────┤
│  框架层    Core/                                              │
│           ├─ Base 层   GameStateBase / PlayerStateBase         │
│           │            （生命周期 + ModularGameplay 接收者）     │
│           └─ 实例层   GameMode / GameState / PlayerState       │
│                       （Experience + GAS + 玩家状态）           │
│           Data/（AssetManager + Experience 三件套）            │
├──────────────────────────────────────────────────────────────┤
│  基础层    引擎: GAS / EnhancedInput / GameFeatures /           │
│             ModularGameplay / AnimationWarping / AssetManager   │
└──────────────────────────────────────────────────────────────┘
```

### 5.2 启动时序

```
① 引擎启动
   UHodgeAssetManager::StartInitialLoading()
     ├─ Super::StartInitialLoading()              扫描 PrimaryAssetTypesToScan
     ├─ STARTUP_JOB(InitializeGameplayCueManager())       [空实现]
     ├─ STARTUP_JOB_WEIGHTED(GetGameData(), 25.f)        同步加载 GameData
     └─ DoAllStartupJobs()                        权重进度

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
     7. 硬编码默认               "Exp_HodgeDefaultExperience"   ← 目前总会走到这里

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
   SpawnDefaultPawnAtTransform_Implementation()
     └─ PawnExtComp->SetPawnData(PawnData)        ← PawnData 注入点

⑨ AHodgePlayerState::OnExperienceLoaded()   （非客户端，注册于 PreInitializeComponents）
     └─ SetPawnData(GameMode->GetPawnDataForController(...))
          ├─ 判重：已有 PawnData → 报错并返回（目前唯一的防重手段）
          ├─ MARK_PROPERTY_DIRTY_FROM_NAME + 赋值
          ├─ 遍历 PawnData->AbilitySets → GiveToAbilitySystem(ASC, nullptr)   ← 基础授予（无句柄）
          ├─ SendGameFrameworkComponentExtensionEvent(NAME_HodgeAbilityReady)
          └─ ForceNetUpdate()
```

### 5.3 为什么 Pawn 要等 Experience 加载完才生成

只有 Experience 加载完成后，才知道该用哪个 `PawnData`、该授予哪些能力。所以：

- `HandleStartingNewPlayer_Implementation()` 里加了 `IsExperienceLoaded()` 守卫
- `GetPawnDataForController()` 在 Experience 未加载时直接返回 `nullptr`
- `OnExperienceLoaded()` 负责给"已连接但还没 Pawn"的玩家补一次 `RestartPlayer()`

---

## 6. 核心系统详解

### 6.1 AssetManager ✅

`Data/HodgeAssetManager.h/.cpp` —— 项目完成度最高的部分，Lyra `LyraAssetManager` 的等价实现。

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

### 6.2 Experience 系统 ✅

本项目 Experience 是 Lyra 同名系统的移植，代码结构与 Lyra 高度一致。想深入理解每一步，建议对照 [`LYRA_RUNTIME_FLOW.md`](LYRA_RUNTIME_FLOW.md) 第 4~6 章阅读。

#### 已落地的资产

| 资产 | 路径 | 说明 |
|---|---|---|
| `Exp_HodgeDefaultExperience` | `/Game/Main/Experiences/` | 默认 Experience，GameMode 兜底硬编码引用它 |
| `DA_Dafult_PawnData` | `/Game/Main/Data/` | 已配进 Experience 的 `DefaultPawnData` |

新建玩法 Experience：在 `/Game/Main/Experiences/` 建蓝图，父类选 `HodgeExperienceDefinition`，**不需要改任何配置** —— 建完即被 AssetManager 发现，用 `-Experience=<名字>` 切换即可。

#### 三个数据资产

| 类 | 文件 | 内容 |
|---|---|---|
| `UHodgeExperienceDefinition` | `Data/HodgeExperienceDefinition.h` | `GameFeaturesToEnable[]`、`DefaultPawnData`、`Actions[]`（Instanced）、`ActionSets[]` |
| `UHodgeExperienceActionSet` | `Data/HodgeExperienceActionSet.h` | 可复用的 `Actions[]` + `GameFeaturesToEnable[]` |
| `UHodgePawnData` | `Data/HodgePawnData.h` | **5 个字段**：`PawnClass` / `AbilitySets[]` / `TagRelationshipMapping` / `InputConfig` / `DefaultCameraMode`。字段均 `EditDefaultsOnly` 且全部生效；`AbilitySets` 现在真的会被授予（见 [§6.4](#64-playerstate玩家状态主体)）。资产内部是否填完整**需编辑器确认** |

#### 加载状态机

`Component/HodgeExperienceManagerComponent.h/.cpp`，挂在 `AHodgeGameState` 上（继承 `UGameStateComponent` + `ILoadingProcessInterface`）。

```
Unloaded → Loading → LoadingGameFeatures → LoadingChaosTestingDelay
                                                    ↓
                                          ExecutingActions → Loaded → Deactivating → Unloaded
```

| 方法 | 职责 |
|---|---|
| `SetCurrentExperience(FPrimaryAssetId)` | 服务端入口，`CurrentExperience` 复制到客户端 |
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
| `SpawnDefaultPawnAtTransform_Implementation()` | `bDeferConstruction = true` 延迟构造，**并调用 `PawnExtComp->SetPawnData(PawnData)`** |
| `RequestPlayerRestartNextFrame()` | 下一帧重生，`bForceReset` 可强制放弃当前 Pawn |
| `FailedToRestartPlayer()` | 重生失败后按条件下一帧重试，避免无限循环 |
| `TryDedicatedServerLogin()` | DS 专用，仅占位 |
| `OnGameModePlayerInitialized` | 玩家完成 GameMode 层初始化的多播委托 |

### 6.3 GameState（游戏级 GAS + Experience 宿主）

`Core/GameState/HodgeGameState.h/.cpp`，继承 `AHodgeGameStateBase` + `IAbilitySystemInterface`。

| 成员 | 说明 |
|---|---|
| `ExperienceManagerComponent` | **Experience 的宿主**，构造函数里 `CreateDefaultSubobject` |
| `AbilitySystemComponent` | **游戏级 ASC**，`PostInitializeComponents` 里 `InitAbilityActorInfo(this, this)` |
| `ServerFPS` | 服务器 Tick 里更新，`DOREPLIFETIME` 复制 |
| `RecorderPlayerState` | 回放录制者，用 `COND_ReplayOnly` 只在回放流复制 |
| `OnRecorderPlayerStateChangedEvent` | 录制者变化的委托 |

游戏级 ASC 的典型用途：全局 GameplayCue、全场 Buff、不隶属于任何玩家的效果。目前还没使用者。

`AHodgeGameStateBase` 已瘦身为纯基类（只剩 `PreInitializeComponents` / `BeginPlay` / `EndPlay` 三个空扩展点）。

### 6.4 PlayerState（玩家状态主体）

`Core/PlayState/HodgePlayerState.h/.cpp`，继承 `AHodgePlayerStateBase` + `IAbilitySystemInterface`。

| 成员 | 说明 |
|---|---|
| `AbilitySystemComponent` | 玩家 ASC，构造时创建，`Mixed` 模式，`NetUpdateFrequency = 100.f` |
| `HealthSet` | 构造时 `CreateDefaultSubobject` |
| `PawnData` | `ReplicatedUsing = OnRep_PawnData`，由 `OnExperienceLoaded` → `SetPawnData()` 设置；同时在该函数里**授予 `PawnData->AbilitySets`** |
| `MyPlayerConnectionType` | `EHodgePlayerConnectionType`：Player / LiveSpectator / ReplaySpectator / InactivePlayer |
| `MyTeamID` / `MySquadID` | 队伍 / 小队 ID（`FGenericTeamId`），复制回调目前是空的 |
| `StatTags` | `FGameplayTagStackContainer`，Tag + 数量的统计容器 |
| `ReplicatedViewRotation` | 观战用视角旋转，`COND_SkipOwner` |
| `NAME_HodgeAbilityReady` | 静态 `FName("HodgeAbilitiesReady")`，SetPawnData 后发送该扩展事件 |

**`SetPawnData()` 是玩家侧的数据与能力总入口**（权威端）：

```cpp
// 先判重：已有 PawnData 就直接报错返回，这也是目前唯一的"防重"手段
if (PawnData) { UE_LOG(..., Error, TEXT("Trying to set PawnData [...] that already has valid PawnData...")); return; }
MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
PawnData = InPawnData;
for (const UHodgeAbilitySet* AbilitySet : PawnData->AbilitySets)   // ← 基础授予
    if (AbilitySet) AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);  // ⚠️ 未收集 GrantedHandles
UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_HodgeAbilityReady);
ForceNetUpdate();
```

**生命周期要点**：

```cpp
// PreInitializeComponents
AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
// ⚠️ 这行只是占位兜底：PlayerState 创建早于 Pawn，GetPawn() 此时是 nullptr。
//    真正的 Avatar 绑定由 HeroComponent → PawnExtension::InitializeAbilitySystem 完成（见 §6.6 / §6.8）。
ExperienceComponent->CallOrRegister_OnExperienceLoaded(...);
```

复制全部用 **Push Model**（`bIsPushBased = true` + `MARK_PROPERTY_DIRTY_FROM_NAME`），比传统轮询复制省 CPU。

### 6.5 GameplayTagStack（Tag + 数量的容器）

`AbilitySystem/GameplayTagStack.h/.cpp`，Lyra 同名文件移植。

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

用 `FFastArraySerializer` 做**增量复制**，并提供 `PostReplicatedAdd` / `PreReplicatedRemove` / `PostReplicatedChange` 三个钩子。目前只有 `StatTags` 一处使用。

### 6.6 Init State 链（PawnExtension + HeroComponent）

链上有**两个 Feature 类**，都是 `UPawnComponent` + `IGameFrameworkInitStateInterface`：

| Feature | FeatureName | 挂载情况 |
|---|---|---|
| `UHodgePawnExtensionComponent` | `PawnExtension` | ✅ 由 `AHodgeCombatCharacter` 构造时创建 |
| `UHodgeHeroComponent` | `Hero` | ✅ 由 `AHodgeHeroCharacter` 构造时创建（Hero 层，与 Lyra 一致） |

**状态注册**（`UHodgeGameInstanceBase::Init()`）：

```cpp
ComponentManager->RegisterInitState(InitState_Spawned,         false, FGameplayTag());
ComponentManager->RegisterInitState(InitState_DataAvailable,   false, InitState_Spawned);
ComponentManager->RegisterInitState(InitState_DataInitialized, false, InitState_DataAvailable);
ComponentManager->RegisterInitState(InitState_GameplayReady,   false, InitState_DataInitialized);
```

#### Feature ① PawnExtensionComponent

```cpp
OnRegister() → RegisterInitStateFeature()
BeginPlay()  → TryToChangeInitState(InitState_Spawned) → CheckDefaultInitialization()
EndPlay()    → UninitializeAbilitySystem() + UnregisterInitStateFeature()
```

| 目标状态 | 准入条件（`CanChangeInitState`） |
|---|---|
| `Spawned` | Pawn 有效 |
| `DataAvailable` | `PawnData != nullptr`；Authority / 本地控制时还必须有 Controller |
| `DataInitialized` | `HaveAllFeaturesReachedInitState(Pawn, DataAvailable)` |
| `GameplayReady` | 直接 `true` |

关键 API：`FindPawnExtensionComponent(Actor)`、`SetPawnData` / `GetPawnData<T>()`、`InitializeAbilitySystem` / `UninitializeAbilitySystem`、`HandleControllerChanged` / `HandlePlayerStateReplicated`、`SetupPlayerInputComponent`、`OnAbilitySystemInitialized_RegisterAndCall` / `OnAbilitySystemUninitialized_Register`。

（Lyra 里的 `IsReadyToInitialize()` 和 `OnPawnReadyToInitialize` 没移植过来。）

#### Feature ② HeroComponent

| 目标状态 | 准入条件 |
|---|---|
| `Spawned` | Pawn 有效 |
| `DataAvailable` | 有 `AHodgePlayerState`；非 SimulatedProxy 时 Controller.PlayerState 归属正确；本地控制还需 `Pawn->InputComponent` + PC + LocalPlayer |
| `DataInitialized` | 有 PS，且 **PawnExtension 已到 `DataInitialized`** |
| `GameplayReady` | 直接 `true` |

它在 `HandleChangeInitState`（`DataAvailable` → `DataInitialized`）里做三件事 —— **这三件事就是整条链的意义所在**：

```cpp
PawnExtComp->InitializeAbilitySystem(PS->GetHodgeAbilitySystemComponent(), PS);  // ① 角色侧 ASC
InitializePlayerInput(Pawn->InputComponent);                                     // ② 输入绑定
DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);   // ③ 相机模式选择
```

✅ 这三行现在都会执行（HeroComponent 已由 `AHodgeHeroCharacter` 构造挂载）。但**"执行"不等于"运行正确"**：换 Pawn / 重生 / 观战时的清理顺序仍需运行验证。

> ⚠️ 一个已知条件：`InitializePlayerInput` 里 `AddMappingContext` 被 `bRegisterWithSettings` 相关条件包住，且会先 `ClearAllMappings()`。若资产里该开关为 false，IMC 会被跳过，**不能只凭"函数被调用"就断定输入已生效**。

#### PlayerState 侧的 Receiver

```cpp
// AHodgePlayerStateBase
PreInitializeComponents() → AddGameFrameworkComponentReceiver(this);
BeginPlay()               → SendGameFrameworkComponentExtensionEvent(NAME_GameActorReady);
EndPlay()                 → RemoveGameFrameworkComponentReceiver(this);
Reset()                   → 转发给所有 UPlayerStateComponent
CopyProperties()          → 按类型+名字匹配，逐个复制 UPlayerStateComponent 的数据
```

#### 还剩两个坑

```
① 默认 PawnClass 仍是基类（GameMode 兜底路径）
   AHodgeGameModeBase 构造里 DefaultPawnClass = AHodgeCharacterBase::StaticClass()
   → 基类没有 PawnExtensionComponent（只在 CombatCharacter 里建）
   → FindPawnExtensionComponent() 返回 nullptr → SetPawnData 静默跳过
   ✔ 实际用哪个 Pawn 由 PawnData->PawnClass 决定，需在编辑器确认指向 Hero 蓝图

② 换 Pawn / 重生的清理顺序未验证
   → HeroComponent 的初始化已闭环，但 UHodgePlayerController::OnUnPossess
     会先清 ASC Avatar，可能让 PawnExtension 中依赖 Avatar 匹配的清理分支跳过
```

> 调试提示：HeroComponent 里加了 `[HODGE-DBG]` 日志（`CanChangeInitState` 各分支、IMC 条目、`BLOCKED`、`CALLBACK`、相机绑定），运行时过滤 `HODGE-DBG` 就能看到链走到了哪一步、被什么条件挡住（这些日志是临时诊断，定位完成后应删除）。

### 6.7 角色体系

```
ACharacter
    ↓
AHodgeCharacterBase              ← 构造时 SetDefaultSubobjectClass 换装移动组件
    ↓
AHodgeCombatCharacter            ← Lyra Character 移植（相机 / FastShared / 死亡 / Team）
    ↓                             并创建 UHodgePawnExtensionComponent
    ├── AHodgeHeroCharacter      ← 玩家：构造挂 UHodgeHeroComponent（输入 / 相机 / ASC 入口）
    └── AHodgeEnemyCharacter     ← 敌人：骨架实现
```

| 类 | 文件 | 关键点 |
|---|---|---|
| `AHodgeCharacterBase` | `Character/HodgeCharacterBase.h` | 生命周期扩展点基类。构造里 `SetDefaultSubobjectClass` 把默认 `CharacterMovement` 换成 `UHodgeCharacterMovementComponent`；移动参数交给蓝图 / 数据资产。**刻意不依赖 GAS**。<br>Receiver 生命周期：`PreInitializeComponents` 注册 → `BeginPlay` 发 `GameActorReady` → `EndPlay` 移除 |
| `AHodgeCombatCharacter` | `Character/HodgeCombatCharacter.h` | Lyra `ALyraCharacter` 移植：`UHodgeCameraComponent`（摆位 `-300, 0, 75`）、`bUseControllerRotationYaw`、`FSharedRepMovement`、死亡流程（`OnDeathStarted/Finished` + `UninitAndDestroy`，HealthComponent 仍注释）、移动模式→GameplayTag、`IGenericTeamAgentInterface`。<br>并创建 `UHodgePawnExtensionComponent`，把 `PossessedBy`/`UnPossessed`/`OnRep_Controller` → `HandleControllerChanged()`、`OnRep_PlayerState` → `HandlePlayerStateReplicated()`、`SetupPlayerInputComponent` 都转发给它 |
| `AHodgeHeroCharacter` | `Character/HodgeHeroCharacter.h` | 玩家角色。**构造里 `CreateDefaultSubobject<UHodgeHeroComponent>(TEXT("HeroComponent"))`**（`HeroComponent` 为 `VisibleAnywhere` 私有属性），输入 / 相机 / ASC 三件事从此有确定入口；`PossessedBy` / `OnRep_PlayerState` 已退化为只调 `Super`（头文件注释尚未同步，属遗留） |
| `AHodgeEnemyCharacter` | `Character/HodgeEnemyCharacter.h` | 继承 `AHodgeCombatCharacter`。只设置 AI 自动控制（`AutoPossessAI`）；碰撞盒 / 血条 / 战斗组件全部注释，**没有自己的 ASC 初始化** |
| `UHodgeHeroComponent` | `Component/HodgeHeroComponent.h` | `ULyraHeroComponent` 移植。已实现：`InitializePlayerInput`（Move / Look_Mouse / Look_Stick / Crouch / AutoRun + AbilityInputTag）、`AbilityCameraMode`、`DetermineCameraMode`（能力相机优先，回落 `PawnData->DefaultCameraMode`）、`AddAdditionalInputConfig`、**`RemoveAdditionalInputConfig`**（按 InputConfig 解绑）、`IsReadyToBindInputs`。<br>剩余：`Input_AutoRun` 内部逻辑、**没有死亡重生** |

要点：

- **相机与 PawnExtension 都在 CombatCharacter 层**：Hero / Enemy 共享。
- **HeroComponent 落在 HeroCharacter 层**（与 Lyra 一致），并由 C++ 挂载 → 输入、相机模式选择、角色侧 ASC 三项一起生效。
- **输入绑定入口**：`SetupPlayerInputComponent()` 转发给 PawnExtension 后只调 `CheckDefaultInitialization()`；真正的绑定逻辑在 HeroComponent 的 `InitializePlayerInput()` 里（见 [§6.9](#69-输入)）。

### 6.8 GAS

GAS 整套是 Lyra 实现的移植。先给类表，再看初始化链路与结算闭环。

| 类 | Lyra 原型 | 职责 | 状态 |
|---|---|---|---|
| `UHodgeAbilitySystemComponent` | `ULyraAbilitySystemComponent` | 项目 ASC：输入缓冲（`ProcessAbilityInput` / `AbilityInputTagPressed/Released`）、`ActivationGroup`、`CancelAbilitiesByFunc`、TagRelationship 查询、`TryActivateAbilitiesOnSpawn` | ✅ PlayerState 构造；初始化由 **HeroComponent → PawnExtension 单入口**完成；`ProcessAbilityInput` 由 `AHodgePlayerController::PostProcessInput` 每帧驱动 |
| `UHodgeGameplayAbility` | `ULyraGameplayAbility` | Ability 基类：`ActivationPolicy` / `ActivationGroup`、`OnPawnAvatarSet`、`MakeEffectContext` 重写；`ActiveCameraMode` + `Set/ClearCameraMode`；`AdditionalCosts`（命中才付费 `ShouldOnlyApplyCostOnHit`）；`GetHeroComponentFromActorInfo` | ✅ 已接入；**C++ 子类有 `UHodgeGameplayAbility_BasicAttack`**（见 [§6.17](#617-战斗系统普攻与逻辑时间轴)），`Main/Character/Hero/GA_BasicAttack` 是它的蓝图子类 |
| `UHodgeAbilityCost` | `ULyraAbilityCost` | 可插拔消耗项（`CheckCost` / `ApplyCost`） | ⚠️ 循环已解开，但无子类 |
| `UHodgeAbilityTagRelationshipMapping` | 同名 | DataAsset：AbilityTag 的 Block / Cancel / Required 关系 | ⚠️ ASC 会查询，但 `SetTagRelationshipMapping` 调用点被注释 |
| `UHodgeAbilitySet` | `ULyraAbilitySet` | "能力包"：Ability + Effect + AttributeSet 一次性授予/回收 | ✅ GameFeature 与 `SetPawnData` 两条路径均已授予；**授予时未记录句柄，回收/撤销不可用** |
| `UHodgeGlobalAbilitySystem` | `ULyraGlobalAbilitySystem` | `UWorldSubsystem`，对世界内所有 ASC 批量授予/移除 | ✅ 自动实例化，但 `Apply*ToAll` 无调用者 |
| `UHodgeAbilitySystemGlobals` | —（Lyra 无） | `UAbilitySystemGlobals` 子类，重写 `AllocGameplayEffectContext()` | ✅ ini 已配，见下 |
| `FHodgeGameplayEffectContext` | `FLyraGameplayEffectContext` | 自定义 EffectContext，携带 AbilitySource + 等级 | ✅ 由上面的 Globals 分配 |
| `IHodgeAbilitySourceInterface` | 同名 | 武器/投射物提供距离衰减、物理材质衰减 | ❌ 无实现类 |
| `UHodgeGameplayCueManager` | `ULyraGameplayCueManager` | Cue 预加载 / 延迟加载 / 常驻管理 | ⚠️ ini 已指向它，但增删路径没通（见下） |
| `UHodgeAttributeSet` / `UHodgeHealthSet` / `UHodgeCombatSet` | `ULyraAttributeSet` / `HealthSet` / `CombatSet` | 属性集基类 / 生命属性（`Health` / `MaxHealth` / `Healing` / `Damage`）/ 战斗属性（`BaseDamage` / `BaseHeal`） | ✅ 已接入 |

#### AbilitySystemGlobals（`8df7f52` 新增）

```cpp
// Public/AbilitySystem/HodgeAbilitySystemGlobals.h
UCLASS(Config=Game)
class UHodgeAbilitySystemGlobals : public UAbilitySystemGlobals
{
    virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};

// 实现：返回项目自定义的上下文
FGameplayEffectContext* UHodgeAbilitySystemGlobals::AllocGameplayEffectContext() const
{
    return new FHodgeGameplayEffectContext();
}
```

`Config/DefaultGame.ini` 新增段：

```ini
[/Script/GameplayAbilities.AbilitySystemGlobals]
AbilitySystemGlobalsClassName=/Script/Hodgepodge.HodgeAbilitySystemGlobals
GlobalGameplayCueManagerClass=/Script/Hodgepodge.HodgeGameplayCueManager
GameFeaturesManagerClassName=/Script/Hodgepodge.HodgeGameFeaturePolicy
; 另有 6 个 ActivateFail*Tag、bUseDebugTargetFromHud、PredictTargetGameplayEffects 等
```

**效果**：`UHodgeGameplayAbility::MakeEffectContext` 里的 `check(EffectContext)` 不再崩（底层分配到的已是自定义类型）；`UHodgeGameplayCueManager::Get()` 的强转也不再返回 nullptr。

**仍未通**：Cue 路径的增删 —— `HodgeGameFeaturePolicy.cpp:39` 注册 `UHodgeGameFeature_AddGameplayCuePaths` Observer 的那行仍是注释，导致写好的 `OnGameFeatureRegistering` 不执行；`OnGameFeatureUnregistering` 整段注释；`UHodgeAssetManager::InitializeGameplayCueManager()` 仍是空实现。

#### 初始化链路：单入口 + 一个占位首绑

```cpp
// ① AHodgePlayerState::PreInitializeComponents() —— GetPawn() 还是 nullptr，只算占位兜底
AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

// ② 真正的入口：HeroComponent 在 PawnExtension 到达 DataInitialized 后触发
PawnExtComp->InitializeAbilitySystem(PS->GetHodgeAbilitySystemComponent(), PS);

// ③ AHodgeHeroCharacter::PossessedBy / OnRep_PlayerState —— 现在只调 Super，不再各自绑 Avatar
void AHodgeHeroCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
}
void AHodgeHeroCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
}
```

- Lyra 的统一入口 `UHodgePawnExtensionComponent::InitializeAbilitySystem()` 现在真的会被调用（HeroComponent 已挂载），角色侧 `GetAbilitySystemComponent()` 与 `OnAbilitySystemInitialized` 均有确定来源。
- ① 仍保留：它是 Pawn 尚未就绪时的占位首绑，**不是**最终 Avatar。
- 待运行验证：换 Pawn / 重生时 `UninitializeAbilitySystem` 的清理顺序；控制器 `OnUnPossess` 会先清 ASC Avatar，可能让 PawnExtension 中依赖 Avatar 匹配的清理分支跳过。

#### 三套 ASC 现状

| ASC | 宿主 | 状态 |
|---|---|---|
| `AHodgePlayerState::AbilitySystemComponent` | 玩家 | ✅ 有效，Avatar 由 HeroComponent → PawnExtension 绑定 |
| `AHodgeGameState::AbilitySystemComponent` | 游戏全局 | ✅ 有效，暂无使用者 |
| 角色侧 `AHodgeCombatCharacter::GetAbilitySystemComponent()` | — | ✅ 转发给 `PawnExtComponent`，PawnExtension 初始化后不再为空 |

#### 伤害 / 治疗结算 ✅

属性分在两个 AttributeSet 里：

- `UHodgeHealthSet`：`Health`（HideFromModifiers）/ `MaxHealth` / `Healing` / `Damage`（HideFromModifiers）—— 全是结算用的运行态属性
- `UHodgeCombatSet`：`BaseDamage` / `BaseHeal` —— 能力 / GE 配置用的基础值

```
PreGameplayEffectExecute（仅处理 Damage）
  ├─ 非自毁时：Gameplay.DamageImmunity / Cheat.GodMode → Magnitude 归零并 return false
  └─ 缓存 HealthBeforeAttributeChange / MaxHealthBeforeAttributeChange

PostGameplayEffectExecute
  ├─ Damage   → Health = Clamp(Health - Damage, 0, MaxHealth)；Damage 清零
  ├─ Healing  → Health = Clamp(Health + Healing, 0, MaxHealth)；Healing 清零
  ├─ Health   → 直接改血（Clamp）
  ├─ MaxHealth→ 广播 OnMaxHealthChanged
  └─ 统一死亡判定：血量变化 → OnHealthChanged；血量 ≤ 0 且 !bOutOfHealth → OnOutOfHealth
     （bOutOfHealth 边沿触发，回调里改血后会重算；客户端 OnRep_Health 有一份同样逻辑）
```

配套：`ClampAttribute`（Health ∈ [0, MaxHealth]、MaxHealth ≥ 1）在 `PreAttributeBaseChange` / `PreAttributeChange` 里调用；`PostAttributeChange` 在 MaxHealth 下降时用 `ApplyModToAttribute(Override)` 压 Health，血量回正时复位 `bOutOfHealth`。

为什么要走 Damage / Healing 这两个 **Meta 属性**绕一圈？因为要在扣减前做统一处理（死亡判定、护盾、溢出治疗转护盾），并拿到完整的 `FGameplayEffectModCallbackData` 上下文。

> 简化之处：目前没有护盾、没有溢出治疗转护盾；伤害类 Tag（`Gameplay.Damage` / `DamageImmunity` / `FellOutOfWorld` 等）定义在 `HodgeHealthSet.cpp` 顶部，没并入统一 Tag 表。
>
> `OnHealthChanged` / `OnOutOfHealth` **已有消费者**：`UHodgeHealthComponent`（见 [§6.18](#618-伤害与死亡)）监听它们驱动死亡流程。**但"谁能打出伤害"这条链还有缺口** —— 见 [§6.17](#617-战斗系统普攻与逻辑时间轴) 的伤害说明。

#### Tag 体系

`HodgeGameplayTags.h/.cpp` 的分组：`InitState.*`、`InputTag.*`、`Ability.ActivateFail.*`（IsDead / Cooldown / Cost / TagsBlocked / TagsMissing / Networking / ActivationGroup）、`Ability.Type.Action.*`（Dash / Melee / Grenade / Jump / ADS / Reload / WeaponFire…）与 `Ability.Type.Passive.*`、`GameplayCue.*`、`GameplayEffect.DamageType.*`、`SetByCaller.Damage/Heal`、`Cheat.GodMode/UnlimitedHealth`、`Status.Death.*`、`Movement.Mode.*`。

> `Gameplay.MovementStopped` 定义在 `HodgeCharacterMovementComponent` 里，不在这张表。

### 6.9 输入

```
UHodgeInputConfig  (DataAsset)          UHodgeInputComponent  (: UEnhancedInputComponent)
├─ NativeInputActions[]                 ├─ BindNativeAction<T>(Tag, TriggerEvent, Obj, Func)
│   （InputAction + InputTag）           │     └─ InputConfig->FindNativeInputActionForTag()
└─ AbilityInputActions[]                ├─ BindAbilityActions<T>(...)
   FindNativeInputActionForTag()        │     └─ Triggered→Pressed / Completed→Released
   FindAbilityInputActionForTag()       ├─ RemoveBinds(Handles)
                                        └─ AddInputMappings / RemoveInputMappings（⚠️ 仍是空壳）
```

**绑定逻辑写在 `UHodgeHeroComponent::InitializePlayerInput()` 里**（已实现完整）：

```cpp
UHodgeInputComponent* HodgeIC = CastChecked<UHodgeInputComponent>(IC);
InputSubsystem->ClearAllMappings();
InputSubsystem->AddMappingContext(DefaultInputMappings, ...);
HodgeIC->AddInputMappings(InputConfig, InputSubsystem);
HodgeIC->BindAbilityActions(InputConfig, this, &Pressed, &Released, ...);   // InputTag → ASC
HodgeIC->BindNativeAction(InputConfig, InputTag_Move,       Triggered, this, &Input_Move);
HodgeIC->BindNativeAction(InputConfig, InputTag_Look_Mouse, Triggered, this, &Input_LookMouse);
HodgeIC->BindNativeAction(InputConfig, InputTag_Look_Stick, Triggered, this, &Input_LookStick);
HodgeIC->BindNativeAction(InputConfig, InputTag_Crouch,     Triggered, this, &Input_Crouch);
HodgeIC->BindNativeAction(InputConfig, InputTag_AutoRun,    Triggered, this, &Input_AutoRun);
```

**它现在会执行**（HeroComponent 已挂载）。从 Pawn 到绑定的转发路径：

```
AHodgeCombatCharacter::SetupPlayerInputComponent()
  └─ PawnExtComponent->SetupPlayerInputComponent()
       └─ 只调 CheckDefaultInitialization()     ← 不绑任何 Action
          ↓ HeroComponent 在 DataInitialized 时接管
          InitializePlayerInput(Pawn->InputComponent)   ← 真正的绑定
```

当前状态与剩余待确认项：

1. ✅ `Config/DefaultInput.ini:94` 的 `DefaultInputComponentClass` 已指向 `/Script/Hodgepodge.HodgeInputComponent`（`CastChecked` 不会失败）
2. ✅ 默认 PlayerController 已换成 `AHodgePlayerController` —— 它的 `PostProcessInput → ASC->ProcessAbilityInput` 是把缓冲输入喂给 GAS 的最后一环（见 [§6.15](#615-playercontroller)）
3. ✅ `UHodgeHeroComponent` 已由 C++ 挂在 Hero 层
4. ✅ `RemoveAdditionalInputConfig` 已实现：按 InputConfig 精确解绑，句柄存在 `AdditionalInputConfigHandles`，`EndPlay` 统一清理
5. ❓ `PawnData->InputConfig`（`DA_HodgeInputConfig`）是否配齐 Move / Look / Crouch / AutoRun 等标签，以及 `DefaultInputMappings` 是否填了 `IMC_Default`
6. ⚠️ `AddMappingContext` 被 `Mapping.bRegisterWithSettings` 条件包住：**该开关为 false 时对应 IMC 会被跳过**；并且函数开头 `ClearAllMappings()` 会清掉其他来源的 Mapping
7. ⚠️ `HodgeInputComponent::AddInputMappings` / `RemoveInputMappings` **仍是空壳**（只有 `check` + 注释），IMC 增删目前只有 HeroComponent 这一条路径

**其它输入类**（Lyra 移植，目前均无人使用）：

| 类 | 职责 |
|---|---|
| `UHodgeInputUserSettings` | `UEnhancedInputUserSettings` 子类，重写 `ApplySettings()` |
| `UHodgePlayerMappableKeyProfile` | `UEnhancedPlayerMappableKeyProfile` 子类（键位方案） |
| `HodgeInputModifiers.h` | 4 个 Modifier：`SettingBasedScalar`、`DeadZone`、`GamepadSensitivity`、`AimInversion` |
| `UHodgeAimSensitivityData` | 瞄准灵敏度曲线资产（空壳，`SensitivityMap` 全注释） |

### 6.10 GameFeature

`GameFeatures/HodgeGameFeaturePolicy.h` 实现 3 个 Observer：`UHodgeGameFeaturePolicy` / `UHodgeGameFeature_HotfixManager` / `UHodgeGameFeature_AddGameplayCuePaths`。

**7 个 GameFeatureAction 的真实状态**：

| 文件 | 状态 | 说明 |
|---|---|---|
| `GameFeatureAction_WorldActionBase.h` | ✅ 可用 | Lyra 原版 |
| `GameFeatureAction_SplitscreenConfig.h` | ✅ 可用 | Lyra 原版 |
| `GameFeatureAction_AddAbilities.h` | ✅ 可用 | 依赖的 `UHodgeAbilitySet` 已建，授予/回收逻辑完整 |
| `GameFeatureAction_AddGameplayCuePath.h` | ✅ 可用 | 只有构造 + `IsDataValid` |
| `GameFeatureAction_AddInputContextMapping.h` | ✅ 可用 | 用 `UEnhancedInputLocalPlayerSubsystem` + `UHodgeAssetManager`；注册 / 注销均已实现。⚠️ 源码里有 TODO：`ControllersAddedTo` 与 LocalPlayer 混用、该列表实际没被写入记录，撤销路径需复核 |
| `GameFeatureAction_AddInputBinding.h` | ✅ 已可用 | `AddInputMappingForPlayer` 主体与 `HandlePawnExtension` 的 `NAME_ExtensionAdded` / `NAME_BindInputsNow` 分支均已启用，撤销分支（`NAME_ExtensionRemoved` / `NAME_BindInputsNow 失效`）走 `RemoveInputMapping` |
| `GameFeatureAction_AddWidget.h` | ❌ 仍注释 | 需要 UIExtension + CommonUI |

**结论**：除 `_AddWidget` 外基本可用，但**还没有任何 GameFeature 插件实例**去用它们（`Content/` 下无 `.uplugin`），所以这些 Action 目前都没有运行验证。

### 6.11 其他框架类

| 类 | 文件 | 说明 |
|---|---|---|
| `UHodgeGameInstanceBase` | `Core/GameInstance/` | 注册 Init State 链；持有 `DebugTestEncryptionKey`（硬编码测试密钥） |
| `AHodgePlayerControllerBase` | `Core/PlayerController/` | **事件桥梁**（Base 层）：把引擎回调转成 LocalPlayer 的多播委托 |
| `AHodgePlayerController` 🆕 | `Core/PlayerController/` | **主 PlayerController**（Concrete 层），相机 / 观战 / Replay / `ProcessAbilityInput`，见 [§6.15](#615-playercontroller) |
| `AHodgeReplayPlayerController` 🆕 | `Core/PlayerController/` | Replay 专用 PC：跟随 `FollowedPlayerState`、平滑观察视角 |
| `UHodgeLocalPlayerBase` | `Core/LocalPlayer/` | 三个 `CallAndRegister_On*Set` 委托 + `bIsPlayerViewEnabled` |
| `AHodgeHUDBase` | `Core/HUD/` | HUD 占位 |
| `UHodgeActorComponentBase` | `Component/` | 组件基类，默认**开 Tick** |
| `UHodgeCombatComponentBase` | `Component/` | 战斗组件，默认**关 Tick**；空实现 |
| `UHodgeInteractionComponentBase` | `Component/` | 交互组件；空实现 |
| `UHodgeMovementComponentBase` | `Component/` | 历史遗留空壳，**别用**；真正的移动组件是 `UHodgeCharacterMovementComponent` |
| `ILoadingProcessInterface` | `Interface/` | `ShouldShowLoadingScreen()`，目前无消费者 |

### 6.12 移动组件

`Component/HodgeCharacterMovementComponent.h/.cpp`，继承 `UCharacterMovementComponent`（Lyra `ULyraCharacterMovementComponent` 移植）。

**接入方式**：`AHodgeCharacterBase` 构造时 `SetDefaultSubobjectClass<UHodgeCharacterMovementComponent>(TEXT("CharacterMovement0"))` 换掉引擎默认组件。

| 能力 | 说明 |
|---|---|
| `GetGroundInfo()` | 地面信息缓存（`FHodgeCharacterGroundInfo`：帧号 + 命中结果 + 离地距离）。Walking 复用 `CurrentFloor`；其它模式向下打射线（`HodgeCharacter.GroundTraceDistance` CVar）。每帧只算一次 |
| `GetDeltaRotation()` / `GetMaxSpeed()` | ASC 带 `Gameplay.MovementStopped` Tag 时**锁旋转 / 速度归零** —— GAS 用 Tag 操纵移动的入口 |
| `SetReplicatedAcceleration()` + `SimulateMovement()` | 配合 FastShared 移动复制，保护服务端同步下来的加速度 |
| `CanAttemptJump()` | 允许空中起跳（不检查蹲伏），给二段跳留口子 |

> 边界要说清：这不是完整的 locomotion 系统 —— 没有步态/旋转模式状态机，那部分要靠动画蓝图重建（见 [§6.14](#614-动画实例)）。

### 6.13 Camera 系统

`Camera/` 7 个文件，是 Lyra `LyraCamera` 模块的逐类移植：

| 类 | Lyra 原型 | 职责 |
|---|---|---|
| `UHodgeCameraComponent` | `ULyraCameraComponent` | 相机组件，挂在 `AHodgeCombatCharacter`（相对 `-300,0,75`）。持有 `CameraModeStack`，`DetermineCameraModeDelegate` 回答"现在该用哪个 CameraMode 类" |
| `UHodgeCameraMode` | `ULyraCameraMode` | 模式抽象基类；`FHodgeCameraModeView`（位置/旋转/FOV/控制旋转）支持按权重混合 |
| `UHodgeCameraModeStack` | `ULyraCameraModeStack` | 模式栈：Push/Pop、按 BlendTime 混合（`BlendStack` 自栈底向栈顶叠加） |
| `UHodgeCameraMode_ThirdPerson` | `ULyraCameraMode_ThirdPerson` | **Abstract + Blueprintable**。第三人称：`TargetOffsetCurve`（按俯仰查曲线）、蹲伏平滑（`CrouchOffsetBlendMultiplier=5`）、防穿透（多射线 + 预测避让） |
| `AHodgePlayerCameraManager` | `ALyraPlayerCameraManager` | 相机管理器。FOV 80°、Pitch ±89°；`UpdateViewTarget` 允许 UI 相机接管 |
| `UHodgeUICameraManagerComponent` | `ULyraUICameraManagerComponent` | `Within=AHodgePlayerCameraManager`，UI 用 `SetViewTarget` 临时接管 |
| `UHodgeCameraAssistInterface` / `FHodgePenetrationAvoidanceFeeler` | 同名 | 视角补充接口 / 防穿透射线参数 |

#### 每帧时序

相机是「被动被问、主动算」：引擎每帧通过 `APlayerCameraManager` 调 `UHodgeCameraComponent::GetCameraView()`。

```mermaid
sequenceDiagram
    autonumber
    participant Eng as 引擎 Tick
    participant PCM as HodgePlayerCameraManager
    participant CC as HodgeCameraComponent
    participant Del as DetermineCameraModeDelegate
    participant Stack as CameraModeStack
    participant M as CameraMode(们)

    Eng->>PCM: UpdateViewTarget(OutVT, Δt)
    PCM->>CC: GetCameraView(Δt, DesiredView)
    CC->>CC: UpdateCameraModes()
    CC->>Del: Execute() 问"当前该用哪个Mode类?"
    Del-->>CC: 返回 TSubclassOf<CameraMode>
    CC->>Stack: PushCameraMode(ModeClass)
    CC->>Stack: EvaluateStack(Δt, CameraModeView)
    loop 每个 Mode
        Stack->>M: UpdateCameraMode(Δt)
        M->>M: UpdateView() + UpdateBlending()
    end
    Stack->>Stack: BlendStack() 按权重叠成最终视图
    Stack-->>CC: 最终 CameraModeView
    CC->>CC: PC->SetControlRotation + SetWorldLocationAndRotation
    CC-->>PCM: DesiredView
```

要点：

- `UpdateCameraModes()` 只在栈激活**且委托已绑定**时才 Push 模式；
- 最终视图回写两处：`PlayerController->SetControlRotation`（让输入方向与相机一致）与相机组件自身位姿/FOV。

#### 多模式怎么混合

`BlendStack()` 的两条铁律：

1. **栈底永远权重 1.0**（地基）；
2. **顺序叠加**：从栈底往栈顶，逐个按 `BlendWeight` 叠上去。

```
Result = 栈底.View
Result = Lerp(Result, 上一层.View, 上一层.BlendWeight)
Result = Lerp(Result, 栈顶.View,   栈顶.BlendWeight)
```

例：第三人称下按瞄准 → 栈变成 `[AimMode(栈顶, 0→1 淡入), ThirdPerson(栈底, 1.0)]`，淡入完成后 UpdateStack 会把下层的 ThirdPerson 移除。

> 多个 Mode 的权重**不会相加超过 1** —— 它是层叠覆盖，不是平均。权重描述"我覆盖下面多少"。

**当前状态：委托已绑定，剩下的是资产与运行验证**：

- ✅ `DetermineCameraModeDelegate.BindUObject(...)` 在 `UHodgeHeroComponent::HandleChangeInitState` 里执行（HeroComponent 已挂载），`CameraModeStack` 会真正出画面
- ✅ 模式资产为 `Content/Main/Camera/CM_ThirdPerson` / `CM_ThirdPerson_Death`；旧 `CM_Default` 已删除
- ✅ `UHodgeCameraMode_ThirdPerson` 构造函数**不再写默认偏移曲线**（`TargetOffsetCurve = nullptr`，仅保留默认穿透射线）；偏移改由蓝图 `bUseRuntimeFloatCurves` / 曲线资产提供 → 第三人称模式现在**依赖蓝图子类或曲线资产**
- ✅ `DetermineCameraMode()` 逻辑：能力相机（`AbilityCameraMode`）优先，否则回落 `PawnData->DefaultCameraMode`
- ❓ 剩余：`DA_Dafult_PawnData.DefaultCameraMode` 实际指向哪个资产（应为 `CM_ThirdPerson`）、模式资产里的曲线数值是否合意，需在编辑器 / PIE 确认

### 6.14 动画实例

`Animation/HodgeAnimInstance.h/.cpp`，Lyra `ULyraAnimInstance` 移植，是 `ABP_Pover_Base` / `ABP_Enemy_Base` 的基类。

- `FGameplayTagBlueprintPropertyMap`：把 **GameplayTag ↔ 动画蓝图变量**绑定。Tag 加/移除时自动写变量 —— 动画层不用轮询 GAS，**Tag 即状态**。
- `GroundDistance`：`NativeUpdateAnimation` 里从 `UHodgeCharacterMovementComponent::GetGroundInfo()` 同步，供落地/脚部逻辑用。
- `NativeInitializeAnimation`：自动找 Owner 的 ASC 并 `InitializeWithAbilitySystem`。
- 编辑器 `IsDataValid`：校验映射表，防止运行时才炸。

> 待补：动画蓝图要在 `ABP_Pover_Base` 里用 Tag 映射 + 引擎原生动画系统重建整套 locomotion 状态节点。`CodexText` 里另有一套 ALS 风格实验实现（见 [§6.16](#616-codextext-实验模块)），**与 `UHodgeAnimInstance` 的 Tag 驱动路线是两套东西，不要混用**。

### 6.15 PlayerController

`Core/PlayerController/HodgePlayerController.h/.cpp`（约 1100 行），Lyra `ALyraPlayerController` 的移植。

| 能力 | 说明 |
|---|---|
| 相机管理器 | 构造里 `PlayerCameraManagerClass = AHodgePlayerCameraManager`；实现 `IHodgeCameraAssistInterface`（`OnCameraPenetratingTarget`） |
| **GAS 输入的最后一环** | `PostProcessInput()` → `ASC->ProcessAbilityInput()`。HeroComponent 缓冲下来的输入，最终在这里变成 Ability 激活 |
| 观战视角 | `PlayerTick` 用 `PlayerState->ReplicatedViewRotation` 同步观察方向，并禁用引擎默认的 `TargetViewRotation` 复制 |
| 客户端 Replay | `TryToRecordClientReplay` / `ShouldRecordClientReplay`（策略目前恒 false）；`AHodgeReplayPlayerController` 负责回放时跟随录制者 |
| 自动奔跑 | `SetIsAutoRunning` / `GetIsAutoRunning` + `K2_OnStartAutoRun` / `K2_OnEndAutoRun`（输入触发目前注释） |
| 相机遮挡处理 | `UpdateHiddenComponents` + `bHideViewTargetPawnNextFrame` |
| PlayerState 变化 | `LastSeenPlayerState` 缓存 + `OnPlayerStateChanged` / `BroadcastOnPlayerStateChanged` |
| 便捷访问 | `GetHodgePlayerState` / `GetHodgeAbilitySystemComponent` / `GetHodgeHUD` |
| Cheat | `ServerCheat` / `ServerCheatAll`（在服务器执行控制台命令） |

**Base / Concrete 分工**（延续 [§8.2](#82-base--concrete-分层)）：

```
AHodgePlayerControllerBase     ← 事件桥梁：ReceivedPlayer / SetPawn / OnPossess /
                                 OnUnPossess / OnRep_PlayerState → UHodgeLocalPlayerBase 多播委托
AHodgePlayerController         ← 具体逻辑：相机 / 观战 / Replay / 自动奔跑 / ProcessAbilityInput
    └── AHodgeReplayPlayerController   ← 回放专用
```

**接法**：`AHodgeGameModeBase` 构造里 `PlayerControllerClass = AHodgePlayerController`；`DefaultEngine.ini` 里配了 `LocalPlayerClassName = UHodgeLocalPlayerBase`。

> 注意：`AHodgeCombatCharacter::GetHodgePlayerController()` 与 `AHodgePlayerState::GetHodgePlayerController()` 已从 `CastChecked<AHodgePlayerControllerBase>` 收敛成 `CastChecked<AHodgePlayerController>` —— 这些接口现在**要求必须是 Concrete 层**，自定义的非 Hodge 子类会 Cast 失败。

### 6.16 CodexText 实验模块

`Source/Hodgepodge/{Public,Private}/CodexText/` 是一块**隔离实验区**：直接继承引擎类（`ACharacter` / `AGameModeBase` / `UUserWidget`），**不接入 Hodge 的 Experience / GAS / PlayerState**，主体系也没有任何 C++ 引用。

| 类 | 文件 | 用途 |
|---|---|---|
| `UHodgeALSLocomotion` | `CodexText/HodgeALSLocomotion.*` | ALS 风格 6 向动画（方向 / 步态 / 脚锁等变量驱动） |
| `UHodgeGroundedLocomotion` + `UHodgeGroundedAuthoring` | `CodexText/HodgeGroundedLocomotion.*`、`HodgeGroundedAuthoring.cpp` | 平地起停 / 脚锁；Authoring 是编辑器专用脚本库 |
| `UHodgeLocomotionLabComponent` / `AHodgeLocomotionLabMode` / `UHodgeLocomotionLabAuthoring` | `CodexText/HodgeLocomotionLab.*` | 移动实验组件 + **复用 `AHodgeGameModeBase`** 的独立实验关卡 GameMode + 编辑器工具 |
| `AHodgeSurvivorHero` / `AHodgeSurvivorMode` / `UHodgeSurvivorHUD` | `CodexText/HodgeSurvivor.*` | 一个自包含的"幸存者类"小玩法（自建输入、刷怪、升级、纯 UMG HUD） |

配套内容在 `Content/CodexText/`（469 个资产 + 6 个关卡，含 `L_MainMenu` / `L_Character` / `L_SurvivalHUD` / `L_Terminal` 与对应 `WBP_*` UMG）；`Config/DefaultEngine.ini` 的 `EditorStartupMap` 就指向 `L_MainMenu`。

> 定位：**AI / MCP 演练 + 动画实验**产物，不是正式玩法。要清理时先确认 `L_*` 关卡与 `BP_*Host` / `WBP_*` 没有被引用，再整体删除。

### 6.17 战斗系统：普攻与逻辑时间轴

这是本项目的**战斗核心设计**：把"表现（动画）"和"逻辑（时机）"彻底分开。

#### 三段式结构

```
UHodgeGameplayAbility_BasicAttack（连招编排）
  └─ 每段 FHodgeBasicAttackStep = { UAnimMontage（表现）, UHodgeAbilityTimeline（逻辑） }
       ├─ UAbilityTask_PlayMontageAndWait   → 只负责播动画
       └─ UHodgeAbilityTask_PlayTimeline    → 只负责逻辑时刻
            └─ UHodgeAbilityTimeline（DataAsset）
                 └─ FHodgeTimelineEvent[] { Kind = Window | Point,
                                            StartTime / EndTime,
                                            WindowTag / PointEventTag,
                                            WindowEffectClass / PointEffectClass,
                                            Priority, NetPolicy }
```

**为什么不直接用 AnimNotify**：动画会被重定向、换皮、调速、被蒙太奇混合，但"第 12 帧开始有判定、第 20 帧开取消窗口"这种逻辑必须稳定、可复现、可同步、可数据驱动。所以逻辑时间轴独立于动画存在。

#### Timeline 的两种事件

| 类型 | 语义 | 进入时 | 退出时 |
|---|---|---|---|
| `Window` | 一段时间区间（判定窗口 / 取消窗口） | 授予 loose tag（如 `Status.Attack.Active` / `.Cancel`）+ 权威端施加 `WindowEffectClass`（必须 Infinite） | 收回 tag + 移除该 GE |
| `Point` | 某个时刻（命中判定帧 / 结束帧） | 派发 `PointEventTag` 的 GameplayEvent + 施加一次性 `PointEffectClass` | — |

节点按 `Time → Kind → Priority → Index` 排序后由 `PlayTimeline` 的 TickTask 驱动；跨帧只派发一次，重复 `Stop` 与 TaskOwnerEnded 都幂等。数据资产自带 `ValidateForPlayback` 与编辑器 `IsDataValid`（时长、EventID 唯一、同 Tag 窗口重叠、GE 时长契约）。

#### 普攻连招（BasicAttack）

| 机制 | 实现 |
|---|---|
| 连招段配置 | `TArray<FHodgeBasicAttackStep> AttackSteps`，每段一份 `Montage + Timeline`；激活时校验 Montage 长度与 `Timeline.Duration` 对齐，不一致直接拒绝激活 |
| 输入缓冲 | `UAbilityTask_WaitInputPress` 缓存一次点击（`bBufferedAttack`），不丢输入也不连点叠加 |
| 接段（取消到下一段） | 监听 `Status.Attack.Cancel.NextAttack` 的计数变化，在窗口内且有缓冲输入时 `TryAdvance` 进入下一段 |
| 移动取消 | `UHodgeAbilityTask_WaitMoveCancel`：**双信号** —— 取消窗口 Tag 生效 **且** HeroComponent 的移动意图超过阈值，才取消 |
| 后摇取消 | `OnInterrupted` → `EndAbility`；`ClearStep` 逐段清理（含手工解绑 `WaitInputPress`） |
| 互斥 | `ActivationGroup = Exclusive_Replaceable`，拥有 / 阻塞 `Status.Attack` |

#### 自动化测试与验证记录

- `Private/Tests/HodgeAbilityTimelineTests.cpp`：3 个测试 —— 校验规则（窗口必须 Infinite GE、Point 禁 Infinite、NaN / 无穷拒绝）、非法配置不产生副作用、清理与排序（跨帧只派发一次、外部 GE 存活、重复 Stop 幂等）
- `Docs/Validation/timeline-2026-09-24.md`、`Docs/Validation/basic-attack-2026-09-24.md`（详见 [§12.5 参考文档](#125-参考文档)）

#### 衔接：伤害缺口

连招本身已验证可跑，但"打中敌人要掉血"这条链还有缺口 —— `HodgeDamageExecution` 的敌我判定被注释，倍率恒 `0.0f`，见 [§6.18](#618-伤害与死亡)。

### 6.18 伤害与死亡

#### 管线全景

```
技能 / GE 配置
  └─ UHodgeCombatSet.BaseDamage / BaseHeal        （配置驱动的基础值）
       ↓
  GE（SetByCaller.Damage / SetByCaller.Heal）      ← UHodgeGameData 里的
       ↓                                             DamageGameplayEffect_SetByCaller 等
  UHodgeDamageExecution / UHodgeHealExecution      （GE Execution 计算）
       ↓  写入 Meta 属性
  UHodgeHealthSet.Damage / Healing
       ↓  PostGameplayEffectExecute 结算
  UHodgeHealthSet.Health  →  广播 OnHealthChanged / OnOutOfHealth
       ↓
  UHodgeHealthComponent（消费事件，驱动死亡状态机）
```

#### 两个 Execution

| Execution | 计算 | 状态 |
|---|---|---|
| `UHodgeDamageExecution` | 捕获 Source 的 `BaseDamage`（snapshot）→ × 距离衰减 × 物理材质衰减 × `DamageInteractionAllowedMultiplier` → 输出到 `HealthSet.Damage` | ⚠️ **敌我倍率恒 0**（TeamSubsystem 段被注释）→ 伤害恒 0；且无防御 / 暴击 / 伤害类型分支 |
| `UHodgeHealExecution` | 捕获 `BaseHeal` → clamp ≥ 0 → 输出到 `HealthSet.Healing` | ✅ 简单完整 |

#### 死亡流程（已打通）

`UHodgeHealthComponent`（`UGameFrameworkComponent`，由 `AHodgeCombatCharacter` 构造创建）是生命系统的**逻辑层**，与"存数值"的 `UHodgeHealthSet` 分工：

| 层 | 负责 |
|---|---|
| `HealthSet` | 保存 `Health` / `MaxHealth` 等 Attribute，结算 Meta 属性，广播属性事件 |
| `HealthComponent` | 监听属性事件，维护 `DeathState` 状态机，广播 `OnDeathStarted` / `OnDeathFinished`，执行死亡后的表现与销毁 |

```
Health 归零 → HealthSet 广播 OnOutOfHealth
  → HealthComponent::HandleOutOfHealth：派发 GameplayEvent.Death
  → StartDeath()：DeathState = DeathStarted，广播 OnDeathStarted
       → CombatCharacter：禁用移动与碰撞、关闭输入
  → FinishDeath()：DeathState = DeathFinished，广播 OnDeathFinished
       → 下一帧 DestroyDueToDeath / K2_OnDeathFinished → UninitAndDestroy
```

- `DeathState` 是 `ReplicatedUsing = OnRep_DeathState` 的属性，客户端通过 OnRep **补播**缺失的阶段事件（服务器先走一步也不会漏）
- `DamageSelfDestruct(bFellOutOfWorld)` 供自毁 / 掉出世界场景使用
- 待补：`HandleOutOfHealth` 里 Elimination / Verb Message 广播整段被注释（淘汰 / 击杀提示链未接）

---

## 7. 当前进度

对照 `UE5 开放世界动作 RPG 架构方案 V2.md` 的 Phase 划分。

### 7.1 已完成 ✅

| 模块 | 内容 |
|---|---|
| **Experience 系统** | 数据资产三件套 + 状态机 + Bundle 加载 + GameFeature 激活/停用 + 三档委托 + GameMode 全流程（7 级优先级、延迟生成 Pawn、PawnData 三级回退、重生重试）；资产 `Exp_HodgeDefaultExperience` |
| **AssetManager** | StartupJob 权重进度、GameData 缓存、软引用同步加载、常驻资源池、PIE 预加载 |
| **Base / Concrete 分层** | `GameStateBase → GameState`、`PlayerStateBase → PlayerState`、`PlayerControllerBase → PlayerController` |
| **玩家状态主体** | PawnData 复制、连接类型枚举、队伍/小队、StatTags、观战视角、Push Model |
| **游戏级 ASC** | `AHodgeGameState` 持有全局 ASC |
| **玩家 ASC（单入口）** | PlayerState 持有 ASC；Avatar 由 HeroComponent → `PawnExtension::InitializeAbilitySystem` 初始化 |
| **主 PlayerController** | `AHodgePlayerController`（`ALyraPlayerController` 移植）：相机管理器 / 观战 / Replay / 自动奔跑 / `PostProcessInput → ProcessAbilityInput`；已设为默认 PC |
| **GameplayTagStack** | Tag + 数量的 FastArray 增量复制容器 |
| **ModularGameplay 基础设施** | 4 个 Init State 注册；Character / PlayerState 双侧 Receiver（生命周期成对）；PawnExtension + HeroComponent 双 Feature 均已挂载；PawnData 注入 |
| **HeroComponent** ✅ | `UHodgeHeroComponent` 完整实现（输入 / 相机 / ASC 入口）并由 C++ 挂载；额外输入增删闭环 |
| **AbilitySet 基础授予** | `PlayerState::SetPawnData` 在权威端遍历 `PawnData->AbilitySets` 授予（**未记录句柄，不可撤销**） |
| **GAS 全套 Lyra 化** | ASC / Ability / AbilityCost / AbilitySet / GlobalAbilitySystem / EffectContext / CueManager / TagRelationship / AbilitySourceInterface |
| **AbilitySystemGlobals** | 自定义 Globals + ini 配置，自定义 EffectContext 生效 |
| **伤害 / 治疗管线** | `UHodgeCombatSet`（`BaseDamage` / `BaseHeal`）+ `HodgeDamageExecution` / `HodgeHealExecution` + `UHodgeHealthSet` 的 Pre/Post 结算（⚠️ 伤害倍率恒 0，见 [§6.18](#618-伤害与死亡)） |
| **生命组件与死亡流程** 🆕 | `UHodgeHealthComponent` 已挂到 CombatCharacter：`OnOutOfHealth → StartDeath → FinishDeath → UninitAndDestroy` 已串联 |
| **普攻连招** 🆕 | `UHodgeGameplayAbility_BasicAttack`：五段连招、输入缓冲、窗口接段、移动 / 后摇取消（**已验证**） |
| **逻辑时间轴** 🆕 | `UHodgeAbilityTimeline`（DataAsset）+ `UHodgeAbilityTask_PlayTimeline` + `UHodgeAbilityTask_WaitMoveCancel`；含 3 个自动化测试（**已验证**） |
| **验证记录** 🆕 | `Docs/Validation/`：basic-attack（五段连击 PIE + Listen Server）、timeline（自动化 + PIE 断言） |
| **相机系统** | `Camera/` 7 类完整移植；模式资产 `CM_ThirdPerson` / `CM_ThirdPerson_Death`，委托由 HeroComponent 绑定（默认偏移改由蓝图曲线提供） |
| **输入框架** | `UHodgeInputComponent`（`DefaultInputComponentClass` 已指向它）+ InputConfig + HeroComponent 绑定 + 额外输入增删（❓ 资产配置待验证） |
| **移动组件 / 动画实例** | `UHodgeCharacterMovementComponent` + `UHodgeAnimInstance` |
| **GameFeatureAction** | `_AddAbilities` / `_AddGameplayCuePath` / `_AddInputContextMapping` / `_AddInputBinding` 可用（`_AddWidget` 仍注释），但无插件实例、未运行验证 |
| **GameplayTag 体系** | `HodgeGameplayTags.h` 原生 Tag（含 `Status.Attack.*` / `GameplayEvent.*` / `Status.Death.*`）+ `Config/DefaultGameplayTags.ini`（123 条非原生 Tag，自 Lyra 286 条对照迁移） |
| **AI 工具链** | `UnrealMCP` + `McpAutomationBridge` 两个 Editor-only 插件 + `Tools/` 前端 + `AGENTS.md` / `.cursor/rules` / `.codex/config.toml` |
| **本地知识库** | `Docs/KnowledgeBase/`（含 `kb.py` 的 `check` / `search` / `refresh` 工具） |
| **CodexText 实验模块** | ALS 风格动画、LocomotionLab、Survivor 小玩法（隔离，不影响主体系） |
| **编译优化** | 全项目 `.gen.cpp` 改 `#include UE_INLINE_GENERATED_CPP_BY_NAME(ClassName)` |

### 7.2 未完成 🚧

| 优先级 | 事项 | 说明 |
|---|---|---|
| 🔴 P0 | **修伤害倍率恒 0** | `HodgeDamageExecution` 的 TeamSubsystem 判敌我整段被注释，`DamageInteractionAllowedMultiplier` 恒 `0.0f` → 战斗"打到人但不掉血"。恢复敌我判定是打通战斗闭环的第一件事 |
| 🔴 P0 | **AbilitySet 授予补句柄** | 现在只授不撤；换装 / 重生 / 撤销前必须有按来源保存的 `GrantedHandles`，否则无法回收也无法真正防重 |
| 🔴 P0 | **编辑器确认资产接线** | `DA_Dafult_PawnData`（`PawnClass` / `InputConfig` / `DefaultCameraMode` / `AbilitySets`）、`BP_Hero_Pover` 的 `DefaultInputMappings`、`DA_HodgeInputConfig` 的映射、`GA_BasicAttack` 的 `AttackSteps`（Montage + Timeline 配对）—— 文本无法读取 `.uasset` 内部值 |
| 🟠 P1 | **接上 Cue 路径增删** | `HodgeGameFeaturePolicy.cpp:39` 的 Observer 注册是注释；`OnGameFeatureUnregistering` 与 `InitializeGameplayCueManager()` 空实现 |
| 🟠 P1 | **伤害系统补维度** | `DamageExecution` 目前只看 BaseDamage × 距离 / 物理材质衰减；缺防御、暴击、`DamageType.*` 分支 |
| 🟠 P1 | **淘汰 / 击杀消息** | `UHodgeHealthComponent::HandleOutOfHealth` 里 Elimination / Verb Message 广播整段注释 |
| 🟠 P1 | **敌人 ASC** | 先定 ASC 所有者（PlayerState 还是 Character），再验服务器伤害目标与远端状态 |
| 🟠 P1 | **建立独立 Log Category** | 全部用 `LogTemp`（还靠 `[HODGE-DBG]` 临时日志），`HodgeLogChannels.h` 不存在 |
| 🟠 P1 | **清理过时注释与临时日志** | `HodgeHeroCharacter` 的双入口注释、`[HODGE-DBG]` 诊断日志、已删函数名的残留说明 |
| 🟡 P2 | **换 Pawn / 重生清理顺序运行验证** | 控制器 `OnUnPossess` 先清 ASC Avatar，可能跳过 PawnExtension 中依赖 Avatar 匹配的分支 |
| 🟡 P2 | 建 `UHodgeWorldSettings` | 让地图能指定默认 Experience |
| 🟡 P2 | 建第一个 GameFeature 插件实例 | 4 个 Action 已可用 |
| 🟡 P2 | 战斗系统 | `HodgeCombatComponentBase` 空的；`HodgeEnemyCharacter` 只有 AI 自动控制 |
| 🟡 P2 | Loading Screen | `ILoadingProcessInterface` 与 `UpdateInitialGameContentLoadPercent` 无消费者 |
| 🟡 P2 | UI 系统 | 框架层完全没有（CodexText 的 UMG 是独立实验） |
| 🟡 P2 | 统一 `EditorStartupMap` 与 `GameDefaultMap` | 前者指向 `CodexText/L_MainMenu`，后者仍是 UE 模板图 |
| 🟡 P2 | 清理 `Content/CodexText/` | AI 演练 + 动画实验产物，确认无引用后清理 |
| 🟢 P3 | Equipment / Inventory / Weapon 三段式模型 | 待建 |
| 🟢 P3 | Teams 阵营系统 | `MyTeamID` 有了，但子系统没有 |
| 🟢 P3 | AI（AIController / BehaviorTree / EQS） | 待建（AIModule 依赖已加） |
| 🟢 P3 | ReplicationGraph / SignificanceManager | 大规模 Actor 时才需要 |
| 🟢 P3 | 开放世界 / 后端 | 按方案最后做 |

### 7.3 路线图

```
阶段 A（已完成）：把角色接电
  ├─ ✅ UHodgeHeroComponent 由 C++ 挂载（输入 / 相机 / ASC 三件事有确定入口）
  ├─ ✅ ASC 单入口 + AbilitySet 基础授予 + 额外输入增删闭环
  ├─ ✅ Character / PlayerState 双侧 Receiver 生命周期成对
  └─ ✅ 普攻连招 + 逻辑时间轴（含自动化测试与 PIE 验证）

阶段 B（当前）：战斗闭环 —— 从"能打动作"到"能打死人"
  ├─ ★ 修 HodgeDamageExecution 的敌我倍率（当前恒 0，打不掉血）
  ├─ 编辑器确认 GA_BasicAttack 的 AttackSteps（Montage↔Timeline 配对）与命中判定
  ├─ 伤害系统补防御 / 暴击 / DamageType 维度
  ├─ AbilitySet 引入 GrantedHandles（可撤销、可防重）
  └─ 接上 Cue 路径 Observer + 淘汰/击杀消息

阶段 C：联机验证 + 内容层
  ├─ Dedicated Server + 2 Client 验证同步（需先补 Server Target）
  ├─ 建 GameFeature 插件，验证激活 / 停用 / 再次激活
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

### 8.2 Base / Concrete 分层

| 层 | 职责 | 例子 |
|---|---|---|
| **Base 层** | 只放生命周期扩展点和跨项目通用机制，**不放具体业务逻辑** | `AHodgeGameStateBase`（3 个生命周期方法）、`AHodgePlayerStateBase`（ModularGameplay Receiver 封装） |
| **Concrete 层** | 放具体游戏逻辑 | `AHodgeGameState`（Experience + 全局 ASC）、`AHodgePlayerState`（PawnData + 玩家 ASC） |

好处：Base 层可随时替换复用，Concrete 层随便改不影响底层。

### 8.3 目录与文件

- `Public/Private` 严格镜像，一个 `.h` 对应一个同路径 `.cpp`
- include 用**完整相对路径**（`#include "Character/HodgeHeroCharacter.h"`）
- 每个 `.cpp` 顶部用 `#include UE_INLINE_GENERATED_CPP_BY_NAME(ClassName)` 加速编译

### 8.4 注释

新写的代码统一用**中文 Doxygen 风格**（`@file` / `@brief` / `@param` / `@return`）。从 Lyra 拷贝的代码保留原版英文注释，再叠加中文说明。

> ⚠️ 部分注释存在**机器翻译痕迹**（`Actor`→"演员"、`GameplayAbility`→"能力"）。看到不要困惑，逐步修正即可。

### 8.5 网络编程

- **一切默认服务器权威**。永远不要在客户端直接改属性（`HP -= 50` 是错的），走 `GameplayEffect`
- 新增可复制属性记得在 `GetLifetimeReplicatedProps` 里注册
- **Push Model**：PlayerState 已启用（`bIsPushBased = true`）。改属性前必须 `MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, 属性名, this)`，否则**客户端收不到更新**
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

| 文档 | 回答什么问题 | 什么时候读 |
|---|---|---|
| [`Docs/KnowledgeBase/README.md`](Docs/KnowledgeBase/README.md) | **本项目当前到底是什么状态**、断点在哪、怎么验收 | **每次动手前先读**，它区分"源码已实现 / 未接通 / 待编辑器验证 / 建议" |
| [`LYRA_LEARNING_GUIDE.md`](LYRA_LEARNING_GUIDE.md) | **学什么**、按什么顺序学、哪些重要 | 规划学习路线时 |
| [`LYRA_RUNTIME_FLOW.md`](LYRA_RUNTIME_FLOW.md) | **怎么跑**、执行顺序、各系统生命周期、调试技巧 | 读 Experience / Init State 代码时边读边对照 |

> 读 §6.2 Experience 和 §6.6 Init State 链时，强烈建议把 `LYRA_RUNTIME_FLOW.md` 第 4~7 章打开对照 —— 本项目这两块基本是 Lyra 的直接移植。

### 9.1 第一天：建立整体认知（约 5 小时）

| # | 文件 | 时间 | 收获 |
|---|---|---|---|
| 1 | `Data/HodgeAssetManagerStartupJob.h` | 15 min | 零依赖，理解启动加载进度系统 |
| 2 | `Data/HodgeExperienceDefinition.h` | 10 min | 理解数据驱动 |
| 3 | `Component/HodgeExperienceManagerComponent.cpp` | 1.5 h | **核心中的核心**，逐行走一遍状态机 |
| 4 | `Core/GameMode/HodgeGameModeBase.cpp` | 1.5 h | Experience 如何被选中、如何触发 Pawn 生成 |
| 5 | `Data/HodgePawnData.h` + `AbilitySystem/HodgeGameplayTags.h` | 30 min | 数据契约与 Tag 体系 |
| 6 | [§6.17 战斗系统](#617-战斗系统普攻与逻辑时间轴) + `HodgeAbilityTimeline.h` + `HodgeGameplayAbility_BasicAttack.h` | 1 h | **战斗核心设计**：表现（Montage）与逻辑（Timeline）为什么分开 |

然后按 [§4.3](#43-运行检查清单) 把项目跑起来，跑通后对照 [§10.6](#106-诊断pawn-没生成--卡住) 排查异常。

### 9.2 第一周：玩家状态与角色

| 天 | 内容 | 目标问题 |
|---|---|---|
| 1-2 | `HodgePlayerState.h/.cpp` + `HodgePlayerStateBase.h/.cpp` | Base/Concrete 分层各自负责什么？PawnData 什么时候被设置？ |
| 3-4 | `HodgeGameState.h/.cpp` | 为什么需要游戏级 ASC？ExperienceManagerComponent 挂在哪？ |
| 5 | `AbilitySystem/GameplayTagStack.h` | FastArray 增量复制相比直接复制 TArray 好在哪？ |
| 6-7 | `HodgePawnExtensionComponent.cpp` + `HodgeHeroComponent.cpp` | 两个 Feature 的准入条件各是什么？HeroComponent 在 `DataInitialized` 做的三件事点亮了什么？ |

### 9.3 第一个月：参与修复与扩展

- 推进 [§7.3 路线图](#73-路线图) 阶段 B（**战斗闭环**）：先修 `HodgeDamageExecution` 的敌我倍率（当前恒 0，打不掉血），再把 `GA_BasicAttack` 的 `AttackSteps`（Montage ↔ Timeline 配对）在编辑器里配好，然后补防御 / 暴击 / 伤害类型
- 验证手段现成的：`Docs/Validation/` 里有两份 PIE 验证记录可照抄流程；`HodgeAbilityTimelineTests.cpp` 的 3 个自动化测试可作为改动后的回归
- 先读 [`Docs/KnowledgeBase/12-integration-backlog.md`](Docs/KnowledgeBase/12-integration-backlog.md)（当前接通清单）与 [`Docs/KnowledgeBase/16-validation.md`](Docs/KnowledgeBase/16-validation.md)（验收场景），再动代码 —— 它们区分了"源码已实现 / 待编辑器验证 / 建议"，能省掉大量猜测
- 配套阅读：`LYRA_RUNTIME_FLOW.md` 第 7 章（四个状态、`CanChangeInitState`、协作式推进）和第 8.1 节（ASC 初始化）—— 本项目的 PawnExtension / HeroComponent 就是照它写的
- 对照阅读 Lyra 源码：`LyraCharacter` / `LyraCamera` / `LyraHeroComponent` / `LyraAnimInstance` —— 本项目新代码基本是它们的移植，有疑问回原版查最准

### 9.4 前置知识自检

| 知识点 | 重要度 | 说明 |
|---|---|---|
| **GAS** | 🔴 极高 | ASC / GameplayAbility / GameplayEffect / AttributeSet / GameplayCue / GameplayTag |
| **网络复制基础** | 🔴 极高 | RPC、属性复制、`OnRep`、**Push Model**、FastArray、服务端权威 |
| **GameplayTag** | 🔴 极高 | 输入通道、状态标记、能力分类，"万能胶水" |
| **Enhanced Input** | 🟠 高 | InputAction / InputMappingContext / Trigger |
| **AssetManager / 软引用 / PrimaryDataAsset** | 🟠 高 | 资源加载体系 |
| **ModularGameplay / GameFrameworkComponentManager** | 🟠 高 | 本项目大量使用，必须懂 |
| **GameFeatures** | 🟡 中 | 边用边学 |
| **相机模式栈 / 动画蓝图** | 🟡 中 | 做相机与动画时再深入 |

### 9.5 进度自查表

- [ ] 能画出 Experience 的状态流转图
- [ ] 能说出 Experience 的 7 级选择优先级
- [ ] 能解释为什么 Pawn 要等 Experience 加载完才生成
- [ ] 能说出 `GetPawnDataForController` 的三级回退顺序
- [ ] 能解释 Base / Concrete 分层的好处
- [ ] 能说出 HeroComponent 在 `DataInitialized` 做的三件事（角色侧 ASC / 输入绑定 / 相机模式），以及 Init State 链为什么优于各处手动初始化
- [ ] 能解释 `AbilitySet->GiveToAbilitySystem(ASC, nullptr)` 里"没有句柄"会带来什么问题
- [ ] 能解释 HealthSet 里 Damage 为什么是 Meta 属性
- [ ] 能说出 Push Model 下改属性必须做什么
- [ ] 能解释 GameplayTagStack 为什么用 FastArray

---

## 10. 调试与验证工具箱

### 10.1 控制台命令

| 命令 | 作用 |
|---|---|
| `Hodge.DumpLoadedAssets` | 列出 AssetManager 常驻内存的所有资源（查内存泄漏） |
| `Hodge.chaos.ExperienceDelayLoad.MinSecs 2` | 人为延迟 Experience 加载 2 秒 |
| `Hodge.chaos.ExperienceDelayLoad.RandomSecs 3` | 随机延迟 0~3 秒 |
| `Automation RunTests Hodge.Timeline` | 跑 Timeline 的 3 个自动化测试（也可在编辑器 Session Frontend → Automation 里点） |

### 10.2 启动参数

| 参数 | 作用 |
|---|---|
| `-LogAssetLoads` | 打印每个资源的同步加载耗时 |
| `-Experience=<Name>` | 指定要加载的 Experience |

### 10.3 日志

⚠️ 项目**没有独立 Log Category**，全部走 `LogTemp`（`HodgeLogChannels.h` 不存在）。在此之前靠断点 + `LogTemp` 过滤：

| 日志 | 位置 | 用途 |
|---|---|---|
| `Identified experience %s (Source: %s)` | `OnMatchAssignmentGiven` | **确认 Experience 选择结果**，排查启动问题第一步 |
| `Failed to identify experience, loading screen will stay up forever` | `OnMatchAssignmentGiven` | Experience Id 无效 |
| `EXPERIENCE: Wanted to use %s but couldn't find it` | `HandleMatchAssignmentIfNotExpectingOne` | 指定的 Experience 找不到，已回退默认 |
| `OW GameInstance Init` | `UHodgeGameInstanceBase::Init` | 确认 GameInstance 初始化 |
| `HODGE-DBG` 系列 | `UHodgeHeroComponent`（**临时诊断日志**） | 过滤 `HODGE-DBG` 可见：逐条 IMC 条目与 `RegisterWithSettings`（false 表示该 IMC 不会生效）、缺失的 native tag、`CanChangeInitState` 的 `BLOCKED` 分支、相机绑定 `CALLBACK`。定位完成后应删除这些日志 |

### 10.4 关键断点位置

| 想知道什么 | 在哪打断点 |
|---|---|
| 最终选定了哪个 Experience | `AHodgeGameModeBase::HandleMatchAssignmentIfNotExpectingOne()` |
| Experience 何时开始加载 | `AHodgeGameModeBase::OnMatchAssignmentGiven()` |
| Bundle 加载了什么 | `UHodgeExperienceManagerComponent::StartExperienceLoad()` |
| 插件何时激活 | `UHodgeExperienceManagerComponent::OnExperienceLoadComplete()` |
| Actions 何时执行 | `UHodgeExperienceManagerComponent::OnExperienceFullLoadCompleted()` |
| Pawn 何时生成 / 用哪个类 | `AHodgeGameModeBase::OnExperienceLoaded()` / `GetDefaultPawnClassForController_Implementation()` |
| PawnData 何时设置 | `AHodgePlayerState::SetPawnData()` |
| PawnExtension 拿到 PawnData 了吗 | `AHodgeGameModeBase::SpawnDefaultPawnAtTransform_Implementation()` |
| ASC 的 Avatar 何时绑定 | `UHodgeHeroComponent::HandleChangeInitState()` → `UHodgePawnExtensionComponent::InitializeAbilitySystem()`（`PossessedBy` / `OnRep_PlayerState` 现在只调 Super，不适合下断点） |
| GameData 何时加载 | `UHodgeAssetManager::LoadGameDataOfClass()` |
| 伤害如何结算 | `UHodgeHealthSet::PostGameplayEffectExecute()` |
| Init State 链推进到哪 | `UHodgePawnExtensionComponent::CanChangeInitState()` / `UHodgeHeroComponent::CanChangeInitState()` |
| 输入绑定是否执行 | `UHodgeHeroComponent::InitializePlayerInput()`（不命中说明 Hero 的 Init State 没走到 `DataInitialized`） |
| 角色侧 ASC 为什么是空 | `UHodgePawnExtensionComponent::InitializeAbilitySystem()` 是否被调到 |
| AbilitySet 授予是否发生 | `AHodgePlayerState::SetPawnData()` 里的 `GiveToAbilitySystem` 循环 |
| HeroComponent 链卡在哪 | 过滤日志 `HODGE-DBG`，看 `CanChangeInitState` 是哪个条件返回了 false |
| 连招走到第几段 | `UHodgeGameplayAbility_BasicAttack::CurrentAttackStep`（蓝图可读）/ `TryAdvance()` |
| Timeline 事件何时派发 | `UHodgeAbilityTask_PlayTimeline` 的 `EnterWindow` / `ExitWindow` / `FirePointEvent` |
| 移动取消是否触发 | `UHodgeAbilityTask_WaitMoveCancel::Evaluate()`（看双信号哪一半没满足） |
| 死亡流程走到哪 | `UHodgeHealthComponent::StartDeath()` / `FinishDeath()` / `OnRep_DeathState()` |
| **伤害为什么是 0** | `UHodgeDamageExecution::Execute_Implementation()`，断点看 `DamageInteractionAllowedMultiplier`（当前恒 0） |

### 10.5 推荐实验

1. **建两个 Experience 资产，用 `-Experience=` 切换**，观察 PawnData 不同导致的角色行为变化
2. **用 `Hodge.chaos.ExperienceDelayLoad.MinSecs 5`** 拖慢加载，观察状态机中间状态
3. **给 `AHodgePlayerState` 加一个 StatTag**，服务端 `AddStatTagStack`，客户端验证复制
4. **给 `PossessedBy()` 打断点**，对比 `PreInitializeComponents` 里 `GetPawn()` 还是 nullptr 的时序差异

### 10.6 诊断"Pawn 没生成 / 卡住"

| 步骤 | 查什么 | 怎么看 |
|---|---|---|
| 1 | Experience 找到了吗？ | 日志搜 `Identified experience`，看 `Source:` |
| 2 | 加载卡在哪个状态？ | 日志搜 `EXPERIENCE:` |
| 3 | 插件名写错了吗？ | 日志搜 `Failed to find plugin URL from PluginName` |
| 4 | PawnData 有了吗？ | 断点 `AHodgePlayerState::SetPawnData()` |
| 5 | Pawn 类对不对？ | 断点 `GetDefaultPawnClassForController_Implementation()` |
| 6 | PawnExtension 拿到 PawnData 了吗？ | 看 `SpawnDefaultPawnAtTransform` 里 `FindPawnExtensionComponent` 是否为 nullptr |
| 7 | ASC 的 Avatar 绑上了吗？ | 断点 `UHodgePawnExtensionComponent::InitializeAbilitySystem()`；确认 HeroComponent 走到了 `DataInitialized` |
| 8 | 输入 / 相机为什么没反应？ | 过滤 `HODGE-DBG`：IMC 是否因 `RegisterWithSettings=false` 被跳过、native tag 是否缺配置、相机 `CALLBACK` 有没有打出来 |

---

## 11. 已知问题与技术债

按影响面排序。改这里的东西前先看一眼。

### 11.1 🔴 功能性缺陷

| 问题 | 影响 | 位置 |
|---|---|---|
| **AbilitySet 只授不撤** 🔴 | `GiveToAbilitySystem(ASC, nullptr)` 未收集 `GrantedHandles` → 无法撤销、无法换装、无法按来源防重（目前只靠 `SetPawnData` 的提前返回） | `HodgePlayerState.cpp::SetPawnData` |
| **伤害倍率恒 0** 🔴 | `HodgeDamageExecution` 的 TeamSubsystem 判敌我整段被注释 → `DamageInteractionAllowedMultiplier` 恒 `0.0f`，**打中也不掉血** | `HodgeDamageExecution.cpp` |
| **伤害缺维度** | 只做 BaseDamage × 距离衰减 × 物理材质衰减；无防御、无暴击、无 `DamageType.*` 分支 | `HodgeDamageExecution.cpp` |
| **淘汰 / 击杀消息未接** | `HandleOutOfHealth` 里 Elimination / Verb Message 广播整段注释 | `HodgeHealthComponent.cpp` |
| **Cue 路径增删没生效** | Observer 注册行是注释；`OnGameFeatureUnregistering` 与 `InitializeGameplayCueManager()` 空实现 | `HodgeGameFeaturePolicy.cpp:39`、`HodgeAssetManager.cpp` |
| **`DefaultPawnClass` 回退是基类** | GameMode 构造里仍是 `AHodgeCharacterBase`（没有 PawnExtensionComponent）→ 若 `PawnData->PawnClass` 也没配，`SetPawnData` 静默跳过 | `HodgeGameModeBase.cpp` |
| **敌人没有 ASC 初始化** | `AHodgeEnemyCharacter` 只设置 `AutoPossessAI`，没有 ASC 创建 / 关联 → 服务器伤害目标与远端状态无从谈起 | `HodgeEnemyCharacter.cpp` |
| **`UHodgeAbilityCost` 无子类** | 消耗框架的循环已解开，但没有具体 Cost 实现（弹药 / 能量等） | `AbilitySystem/Abilities/HodgeAbilityCost.h` |
| **IMC 会被条件跳过** | `AddMappingContext` 被 `bRegisterWithSettings` 包住，为 false 时 IMC 不生效；且开头 `ClearAllMappings()` 会清掉其他来源的 Mapping | `HodgeHeroComponent.cpp::InitializePlayerInput` |
| **`AddInputMappings` / `RemoveInputMappings` 是空壳** | IMC 增删没地方做（只剩 HeroComponent 一条路径） | `Input/HodgeInputComponent.cpp` |
| **属性集不是配置驱动** | 靠 `CreateDefaultSubobject` 写死，不再由 `AttributeSetClasses` 配置 | `AHodgePlayerState` 构造 |
| `OnRep_PawnData()` / `OnRep_MyTeamID()` / `OnRep_MySquadID()` 是空的 | 复制后无响应 | `HodgePlayerState.cpp` |
| 4 个 Lyra 输入类是空壳 | `AimSensitivityData` / `InputUserSettings` / `MappableKeyProfile` / `InputModifiers` 均无使用者 | `Input/` |
| **PawnData 字段是否填全未知** | 资产是二进制，`PawnClass` / `InputConfig` / `DefaultCameraMode` / `AbilitySets` 的实际赋值只能在编辑器确认 | `DA_Dafult_PawnData.uasset` |
| **诊断日志未清理** | HeroComponent / CombatCharacter 里的 `[HODGE-DBG]` 是临时定位用日志 | `HodgeHeroComponent.cpp` 等 |
| **换 Pawn / 重生清理顺序未验证** | 控制器 `OnUnPossess` 先清 ASC Avatar，可能让 PawnExtension 中依赖 Avatar 匹配的清理分支跳过 | `HodgePlayerController.cpp`、`HodgePawnExtensionComponent.cpp` |

### 11.2 配置与命名

| 问题 | 说明 |
|---|---|
| `GlobalDefaultGameMode` 用旧类名 | `/Script/Hodgepodge.HodgepodgeGameModeBase`，靠 `[CoreRedirects]` 生效（`GameInstanceClass` 已改成新类名 `HodgeGameInstanceBase`） |
| 资产名拼写错误 | `DA_Dafult_GameData` / `DA_Dafult_PawnData`（`Dafult` 应为 `Default`），已写进 ini / 代码，改名要同步 |
| 目录名拼写错误 | `Core/PlayState/` 应为 `Core/PlayerState/` |
| 引擎插件依赖没写全 | 模块用了 `ModularGameplay` / `SignificanceManager` 但 `.uproject` 的 `Plugins` 段没声明（只是 UBT 警告） |
| `UNTLink` 状态 | `.uproject` 中 `Enabled: false`（显式禁用），但插件自身的 `EnabledByDefault` 仍为 true，别误以为还在使用 |
| 编辑器 / 游戏启动图不一致 | `EditorStartupMap=/Game/CodexText/L_MainMenu`，`GameDefaultMap=/Game/ThirdPerson/Maps/ThirdPersonMap` |
| `.codex/config.toml` 路径写死 | 指向 `D:\Hodgepodge\...`，换机器要改 |

### 11.3 安全隐患

| 问题 | 说明 |
|---|---|
| **硬编码调试密钥** | `UHodgeGameInstanceBase::Init()` 里 `DebugTestEncryptionKey` 用固定递增数据填充。发布前记得移除 |

### 11.4 代码卫生

| 问题 | 说明 |
|---|---|
| 日志全部用 `LogTemp` | 无独立 Log Category，`HodgeLogChannels.h` 不存在 |
| 注释机器翻译痕迹 | `Actor`→"演员"、`GameplayAbility`→"能力" 等中英混排 |
| 过时注释 | `HodgeHeroCharacter.h/.cpp` 仍写着"GAS 双初始化入口"，实际已只调 Super |
| `[HODGE-DBG]` 临时日志 | HeroComponent 等处的诊断日志，定位后应清理 |
| 死代码未清理 | `HodgeEnemyCharacter` 大段注释、`MotionWarpingComponent` 注释、`HostDedicatedServerMatch` 整段注释 |
| `Content/CodexText/` 是演练 + 实验产物 | 469 个资产 + 6 个关卡（含 `WBP_*` UMG）与 `Source` 里的 `CodexText` 模块，主体系无引用 |
| 部分输入资产命名偏旧 | `Main/Input/InputAction/` 里 `IA_Ragdoll` / `IA_Slomo` / `IA_OverlayModeMenu` 等当前无绑定，是否被 `DA_HodgeInputConfig` 引用需在编辑器确认 |
| Lyra 遗留 Tag | `HodgeGameplayTags.h` 里有 `Lyra_*`、`ShooterGame_*` 前缀的 Tag |
| `// 111屎山代码来袭` | 多个文件顶部的自嘲注释，无害 |

### 11.5 明确的设计取舍（不是 bug）

| 取舍 | 理由 |
|---|---|
| Locomotion 先留白 | 不立即自研全套；先用引擎默认移动 + `UHodgeAnimInstance` 的 Tag 驱动重建动画层 |
| 不引入 CommonUI / UIExtension / GameSettings / CommonUser | 体量大、非核心矛盾，延后 |
| 保留注释掉的 `_AddWidget` | 作为参考实现，启用前需先补依赖 |
| 走 Init State 链而非各处手动初始化 | 依赖会越来越多（PawnData / InputConfig / AbilitySet），手动入口会失控。现已按 Lyra 落地，旧的 ASC 双入口已移除 |
| 战斗逻辑与动画分离 | 用 `HodgeAbilityTimeline` 承载时机逻辑，Montage 只做表现 —— 动画可换皮 / 重定向而不动逻辑，见 [§6.17](#617-战斗系统普攻与逻辑时间轴) |
| CodexText 用独立实现而非接入框架 | 实验内容（含 UMG、自建输入）刻意绕开 Experience / GAS，避免污染主体系 |

---

## 12. 附录：文件速查索引

### 12.1 架构核心（必读）

```
Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h                启动任务（零依赖，先读这个）
Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h                  Experience 数据结构
Source/Hodgepodge/Private/Component/HodgeExperienceManagerComponent.cpp    状态机实现 ★★★
Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp              Experience 接入流程 ★★★
Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp              玩家状态主体 ★★★
Source/Hodgepodge/Private/Core/GameState/HodgeGameState.cpp                Experience 宿主 + 全局 ASC
Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp      Init State 链注册
Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp        Init State 节点 + ASC 容器 ★★★
Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp                 输入 / 相机 / ASC 入口（✅ 已挂载）★★★
Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp                 玩家角色：挂 HeroComponent（原双入口已退化）★★★
Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerController.cpp  主 PC：相机 / 观战 / ProcessAbilityInput ★★★
Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp    伤害治疗结算 + 死亡判定 ★★★
Source/Hodgepodge/Private/Component/HodgeHealthComponent.cpp               生命逻辑层 + 死亡状态机 ★★★
Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_BasicAttack.h  普攻连招编排 ★★★
Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h                       逻辑时间轴数据资产 ★★★
Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.cpp  时间轴驱动（窗口 / Point）★★★
Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeDamageExecution.cpp  伤害计算（⚠️ 倍率恒 0，见 §6.18）
Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h       Lyra ASC：输入缓冲 / ActivationGroup
Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemGlobals.h         自定义 GAS 全局配置
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
                             GameInstanceClass（新类名）、
                             LocalPlayerClassName=/Script/Hodgepodge.HodgeLocalPlayerBase、
                             EditorStartupMap=/Game/CodexText/L_MainMenu、
                             [CoreRedirects]（勿删）
Config/DefaultGame.ini       [/Script/HodgePodge.HodgeAssetManager] 数据路径、
                             PrimaryAssetTypesToScan ★ 共 7 项：
                               Map / PrimaryAssetLabel / HodgeGameData /
                               GameFeatureData / HodgeExperienceDefinition /
                               HodgePawnData / HodgeExperienceActionSet
                             [/Script/GameplayAbilities.AbilitySystemGlobals]：
                               AbilitySystemGlobalsClassName / GlobalGameplayCueManagerClass /
                               GameFeaturesManagerClassName / ActivateFail*Tag 等
                             [/Script/GameFeatures.GameFeaturesSubsystemSettings]：
                               GameFeaturesManagerClassName
                             [/Script/McpAutomationBridge.McpAutomationBridgeSettings]：
                               NativeMCPPort=3016 / ListenPorts=8116
Config/DefaultInput.ini      DefaultInputComponentClass=/Script/Hodgepodge.HodgeInputComponent
Config/DefaultGameplayTags.ini  🆕 GameplayTagList（123 条非原生 Tag，自 Lyra 286 条对照迁移；
                             原生 Tag 在 C++ 里用 UE_DECLARE/DEFINE_GAMEPLAY_TAG 注册）
```

> 新建 Experience 蓝图放在 `/Game/Main/Experiences/` 即自动被扫到，无需改 ini。

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
└── AHodgePlayerControllerBase             （事件桥梁，Base 层）
    └── AHodgePlayerController             （主 PC：相机 / 观战 / Replay / ProcessAbilityInput）
        └── AHodgeReplayPlayerController   （回放专用）

ULocalPlayer
└── UHodgeLocalPlayerBase                  （三套 CallAndRegister 委托）

ACharacter
└── AHodgeCharacterBase                  ← 构造换装 UHodgeCharacterMovementComponent
    └── AHodgeCombatCharacter            ← Lyra Character 移植（相机 / FastShared / 死亡 / Team）
        ├── AHodgeHeroCharacter          ← 玩家（构造挂 UHodgeHeroComponent）
        └── AHodgeEnemyCharacter         ← 骨架

UActorComponent
└── UHodgeActorComponentBase
    ├── UHodgeCombatComponentBase
    ├── UHodgeInteractionComponentBase
    ├── UHodgeLocomotionLabComponent       （CodexText 实验）
    └── UHodgeMovementComponentBase        （空壳，别用）

UPawnComponent + IGameFrameworkInitStateInterface
├── UHodgePawnExtensionComponent         （Feature ①：PawnData + ASC 容器）
└── UHodgeHeroComponent                  （Feature ②：输入 + 相机 + ASC 入口）

UCharacterMovementComponent
└── UHodgeCharacterMovementComponent     （GroundInfo / MovementStopped）

UCameraComponent
└── UHodgeCameraComponent                （相机模式栈）

UAnimInstance
└── UHodgeAnimInstance                   （Tag 映射 + GroundDistance）

UEnhancedInputComponent
└── UHodgeInputComponent                 （原生 + 能力绑定）

UAbilitySystemComponent
└── UHodgeAbilitySystemComponent         （输入缓冲 / ActivationGroup）

UAbilitySystemGlobals
└── UHodgeAbilitySystemGlobals           （AllocGameplayEffectContext）

UAttributeSet
└── UHodgeAttributeSet
    ├── UHodgeHealthSet                  （Health / MaxHealth / Healing / Damage）
    └── UHodgeCombatSet                  （BaseDamage / BaseHeal）

UGameplayAbility
└── UHodgeGameplayAbility                （Ability 基类）
    └── UHodgeGameplayAbility_BasicAttack（五段连招编排）

UAbilityTask
├── UHodgeAbilityTask_PlayTimeline       （驱动 HodgeAbilityTimeline：窗口 / Point）
└── UHodgeAbilityTask_WaitMoveCancel     （取消窗口 + 移动意图 双信号）

UGameplayEffectExecutionCalculation
├── UHodgeDamageExecution                （⚠️ 敌我倍率恒 0，见 §6.18）
└── UHodgeHealExecution

UGameFrameworkComponent
└── UHodgeHealthComponent                （生命逻辑层 + DeathState 状态机）

UWorldSubsystem
└── UHodgeGlobalAbilitySystem            （对所有 ASC 批量授予）

UPrimaryDataAsset
├── UHodgeGameData
├── UHodgePawnData
├── UHodgeExperienceDefinition
├── UHodgeExperienceActionSet
├── UHodgeAbilitySet
└── UHodgeAbilityTimeline                （逻辑时间轴：Window / Point 事件表）

UDataAsset
├── UHodgeInputConfig                    （InputAction ↔ InputTag）
└── UHodgeAbilityTagRelationshipMapping  （AbilityTag 的 Block/Cancel 关系）

UHodgeAnimInstance
├── UHodgeALSLocomotion                  （CodexText：ALS 风格 6 向动画）
│   └── UHodgeGroundedLocomotion         （CodexText：平地起停 / 脚锁）
ACharacter
└── AHodgeSurvivorHero                   （CodexText：自包含小玩法英雄）
AGameModeBase
└── AHodgeSurvivorMode                   （CodexText：不接 Hodge 框架）
AHodgeGameModeBase
└── AHodgeLocomotionLabMode              （CodexText：复用 Hodge 框架的独立实验关卡）
UActorComponent
└── UHodgeLocomotionLabComponent         （CodexText）
UUserWidget
└── UHodgeSurvivorHUD                    （CodexText）
```

### 12.5 参考文档

> 前两份是 **Lyra 原版**的学习文档（讲 Lyra 自己怎么跑）。本项目大量直接移植 Lyra，把类名前缀 `Lyra` 换成 `Hodge` 基本就能对应上。**本项目自身的现状**以 `Docs/KnowledgeBase/` 为准。

| 文档 | 内容 | 本项目对应关系 |
|---|---|---|
| [`Docs/KnowledgeBase/README.md`](Docs/KnowledgeBase/README.md) | **本项目**本地知识库：架构、启动链、初始化、输入 / GAS / 战斗 / 相机 / 网络、验收手册、源码与配置索引 | 全篇；[`12-integration-backlog.md`](Docs/KnowledgeBase/12-integration-backlog.md) 是当前断点清单，[`16-validation.md`](Docs/KnowledgeBase/16-validation.md) 是验收手册 |
| [`LYRA_LEARNING_GUIDE.md`](LYRA_LEARNING_GUIDE.md) | Lyra 架构学习指南：学什么、按什么顺序学。第 4 章的十大理念是本项目的设计宪法 | 全部理念的来源 |
| [`LYRA_RUNTIME_FLOW.md`](LYRA_RUNTIME_FLOW.md) | Lyra 运行时执行链路：Experience 决策、加载状态机、启动时序、Pawn Init State 链、调试技巧 | §6.2 Experience（第 4~6 章）、§6.6 Init State（第 7 章）、§6.8 ASC 初始化（第 8.1 节） |
| [`UE5 开放世界动作 RPG 架构方案 V2.md`](UE5%20开放世界动作%20RPG%20架构方案%20V2.md) | 总体方案与 Phase 划分，含角色职责、组件设计、DS 路线 | §7 进度的 Phase 依据 |
| [`Docs/Design/`](Docs/Design) | 4 篇设计文档：`ability-timeline.md`（时间轴全量设计）、`ability-timeline-stage1.md`（第一阶段事件模型）、`ability-definition-combo-graph.md`（单段技能 Definition 与连招跳转表）、`ability-definition-combo-implementation-plan.md` | 时间轴已实现，见 [§6.17](#617-战斗系统普攻与逻辑时间轴)；Definition / 连招图仍是设计草案 |
| [`Docs/Validation/`](Docs/Validation) | 2 篇 PIE 验证记录：`basic-attack-2026-09-24.md`、`timeline-2026-09-24.md` | 复现步骤与断言清单，改战斗代码后照跑一遍 |
| [`Docs/AI_DEVELOPMENT.md`](Docs/AI_DEVELOPMENT.md) | AI 开发与验证流程（构建命令、验证边界） | §4.2 构建命令的来源 |

### 12.6 AI 辅助开发工具链

本仓库引入了两套"让 AI 直接操作 UE 编辑器"的插件链。**不影响游戏运行时**，主模块 `Hodgepodge` 的 `Build.cs` 里没有它们。

```
AI 客户端（Codex / Cursor / 其它 MCP 客户端）
   ↓ stdio
Tools/UnrealMCP/server.py        Python MCP 前端（FastMCP），只暴露 15 个"安全"工具
   ↓ TCP 127.0.0.1:55557（换行分隔的 JSON 行）
Plugins/UnrealMCP/               UE 编辑器插件（UEditorSubsystem），命令派发到游戏线程
   ↓
编辑器：创建蓝图 / 加组件 / 编译 / UMG / 资产查看 …

另一条独立链路：Plugins/McpAutomationBridge/（Editor-only；端口在 DefaultGame.ini 配置：
NativeMCPPort=3016 / ListenPorts=8116，默认只监听回环且要求能力令牌）
```

| 组成 | 说明 |
|---|---|
| `Plugins/UnrealMCP/` | **第三方开源**（`github.com/voodoofox/unreal-mcp`，MIT），本地做过 UE5.5 适配：修 include、关掉 Niagara 能力、`EnabledByDefault=true`。`Type: Editor`，只在编辑器生效 |
| `Plugins/McpAutomationBridge/` 🆕 | 另一套编辑器 MCP 桥（Editor-only，`.uproject` 中 `TargetAllowList: ["Editor"]`）。开关与端口见 `Config/DefaultGame.ini` 的 `[/Script/McpAutomationBridge.McpAutomationBridgeSettings]` |
| `Tools/UnrealMCP/server.py` | Python MCP 服务端。**刻意不暴露任意 Python / 控制台执行**；每次调用先 `identity()` 校验是不是本工程；README 强制"改动前先 duplicate 备份、先读 Pin 再连线、编译成功才保存" |
| `Tools/`（其它） | `ChiR24MCP` / `LocomotionLab` / `ModelRepair` 等脚本与说明 |
| `AGENTS.md` | 项目级 AI 协作规则：以现有源码为准、UE 5.5 不改引擎、单模块 + `Hodge` 命名、GAS/PlayerState 所有权、交付需说明验证情况 |
| `.agents/ue-project-context.md` | 英文工程事实快照（模块 / 类 / UI / 输入 / 动画现状），给 AI 读的"当前状态说明书" |
| `.cursor/rules/project.mdc` | `alwaysApply: true` 的指针文件，指向 `AGENTS.md` |
| `.codex/config.toml` | 注册 `mcp_servers.hodge_blueprints`，用 venv 的 python 启动 `server.py` |
| `Docs/KnowledgeBase/` | AI 维护的本地知识库，附带 `kb.py` 的 `check` / `search` / `refresh` 工具 |

> ⚠️ 注意：`Content/CodexText/` 与 `Source/Hodgepodge/{Public,Private}/CodexText/` 是这套工具链 + 动画实验的产物（示例关卡、UMG、独立玩法），主体系无引用；删除前先确认 `L_*` / `BP_*Host` / `WBP_*` 没有被引用。

---

*本 README 基于 UE 5.5 + Hodgepodge 提交 `10305c2` 整理（工作区干净，编译已验证）。项目处于活跃的 Lyra 化重构中，**[§7 进度](#7-当前进度) 与 [`Docs/KnowledgeBase`](Docs/KnowledgeBase/README.md) 请优先关注并定期更新**。战斗相关结论以 [`Docs/Validation/`](Docs/Validation) 的验证记录为准；文中"已接通"指源码与配置已存在，未逐一重跑 PIE。*
