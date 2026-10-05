# 项目定位、目录与文档可信度

> 最近源码核对：2026-09-29（HEAD `abb9224`）。源码接入状态与运行验收分开记录。
[返回首页](README.md) · [本轮记录](25-update-2026-09-29.md)

## 工程是什么

项目描述的长期目标是 Dedicated Server、数据驱动、可扩展的动作 RPG 底层。现在是 `Hodgepodge`（Runtime）+ `HodgeAbilityEditor`（Editor）两个模块，引擎关联为 5.5。框架大量借鉴 Lyra：角色、GAS、输入、相机已完成迁移，**UI 层于 2026-09-28 整体搬入**（81 个文件 / 24 个待复活），**连击系统与技能编辑器于 2026-09-29 落地**。早期曾引入第三方动画插件作参考，主模块依赖里已不含它，但仓库中仍留有插件目录。

"框架有这些类"与"游戏具备这些能力"必须分开。Experience、ASC、AbilitySet、相机模式栈、连击系统、UI 组件类有代码；可移动玩家、完整战斗循环（**打中掉血仍不成立**）、AI、装备背包、实际出画面、后端不能据此自动认定完成。

## 目录阅读地图

- `Source/Hodgepodge/Public`：反射类型、组件接口、配置字段。理解一个系统先从这里确定契约。
- `Source/Hodgepodge/Private`：函数体和真实执行路径。确认实现必须看这里，尤其要排除注释代码。
- `Source/Hodgepodge/Hodgepodge.Build.cs`：模块依赖及 Iris 支持设置（2026-09-28 新增 `CommonUI` / `CommonInput` / `Slate`，Private 新增 `ApplicationCore`）。
- **`Source/HodgeAbilityEditor/`**（2026-09-29 新增，9 文件，**独立 Editor 模块**）：技能编辑器，编辑 `UHodgeAbilityDefinition`。已登记进 `HodgepodgeEditor.Target.cs`，不进 Runtime 构建。详见 [本轮记录](25-update-2026-09-29.md)。
- **`Source/Hodgepodge/Public/Equipment/`**（2026-09-29 新增，4 个 `.h`）：装备四件套，`Definition` / `Instance` / `ManagerComponent` / `WeaponInstance`。**编译通过但零挂载**（无 `CreateDefaultSubobject`、非 `BlueprintSpawnableComponent`），别把它算进"已实现"。
- `Source/Hodgepodge/Public/Data/` 包含 `HodgeAbilityDefinition` / `HodgeComboDefinition`；`UHodgePawnData::IsDataValid` 校验统一实现在 `Private/Data/HodgePawnData.cpp`（原 `_Validation.cpp` 于 2026-10-05 合并）。
- `Source/Hodgepodge/Public/UI`（2026-09-28 新增，41 个 `.h`）：Lyra UI 移植层，分 Basic / Common / Extension / Foundation / Frontend / IndicatorSystem / PerformanceStats / Subsystem / Weapons。⚠️ **其中 24 个文件是"待复活"状态** —— 逐行注释保留、文件头带 `[UI-MIGRATION-PENDING]` 标记，**不是有效代码**，别把它们算进"已实现"。详见 [本轮记录](24-update-2026-09-28.md)。
- `Source/*.Target.cs`：已有 Game / Editor 目标。独立 Server Target 需要另行补充。
- `Config`：默认地图、类重定向、AssetManager 扫描及输入设置。
- `Content/Main`：自有玩法数据、输入资源与角色动画；详见资产清单。
- `Content/qiuyuan`、`Content/Wuwa`、`Content/Assets`：模型、动画、音效等资源集合。资源存在不证明已经被 Pawn 使用。
- `Content/CodexText` 与 `Source/Hodgepodge/*/CodexText`：独立的 ALS 风格动画实验与 Survivor 小玩法；除 `AHodgeLocomotionLabMode` 复用 HodgeGameModeBase 外不继承主角色体系，主模块不引用它们。实验区持续精修（上半身 Overlay、左手握持、步幅、地形 IK），`Hodgepodge.Build.cs` 的**编辑器专用块**因此新增 `AnimationWarpingRuntime` / `AnimationWarpingEditor` 依赖；属实验、未走主 GAS 链，详见 [CodexText 地面运动实验说明](../../Tools/LocomotionLab/README.md)。
- **`/Game/CodexText/DefinitionCombo/`**（2026-09-29，当前连招资产所在地）：`GA_Attack_1~5` + `DA_Attack_1~5` + 对应 Timeline + `DT_LightCombo`（六行跳转表）+ `DA_LightCombo` + `AS_LightCombo`；`DA_Dafult_PawnData` 已存 `ComboDefinition = DA_LightCombo`。存在不等于已接线 —— Montage ↔ Timeline 时长配对需在编辑器确认。
- ⚠️ `Content/Main/Character/Hero/GA_BasicAttack.uasset` 与 `UHodgeGameplayAbility_BasicAttack` 已成**孤儿**（`DA_Pover` 已移除该授予、C++ 零引用），新链路走 Definition + Combo；`DA_TimeLineText.uasset` 是旧字段模型的废数据，旧的 `Hero/Ability/` 目录与 `GA_Melee` **已不存在**。
- `Content/Wuwa/Weapon/`：仇远武器（骨骼 + 材质已修复，**无动画**），详见 `Tools/Weapon/README.md`。
- `Plugins/Developer/RiderLink`：开发工具插件，不计入游戏核心逻辑索引。
- 仓库中另有一个未写进 `.uproject` 的第三方动画插件目录（其 `.uplugin` 声明 `EnabledByDefault=true`），主模块不依赖它；既不能称它已禁用，也不能当成在用。
- `Plugins/UnrealMCP`、`Plugins/McpAutomationBridge`：两个 Editor-only 自动化 / MCP 插件；连接与工具调用已于 2026-09-19 实测通过，坑见 [排障手册](14-troubleshooting.md)。
- `Saved`、`Intermediate`、`Binaries`：运行或构建产物，不作为架构事实来源。

`Core/PlayState` 的拼写是项目现状。知识库保留实际路径，不擅自改成 PlayerState。

## 文档冲突怎么处理

本地源码和配置回答“现在实现了什么”；资产编辑器回答“现在配了什么”；运行日志和验收回答“实际跑起来怎样”。项目 README 是历史状态摘要，Lyra 两篇文档是上游参考，V2 方案是目标设计。

2026-09-10 曾核对到 HeroComponent 全部注释、相机委托无调用者；2026-09-13 源码已启用两者。2026-09-17 进一步确认：ASC 接入收敛到单一组件路径、Receiver 生命周期已配对、AbilitySet 基础授予与额外输入移除已实现。2026-09-19 时间轴按统一事件模型重写并部分实测；2026-09-22 新增"移动取消后摇"消费方（`WaitMoveCancel` + 移动意图 + 取消窗口标签，**当时未验证**）。**2026-09-28 该消费方已被攻击 Ability 接入并在 09-24 的 PIE 验证中通过**；同轮还落地了 UI 源码迁移（81 文件 / 24 待复活）、HUD 换代、伤害 Execution、`UHodgeCombatSet` 与 `UHodgeHealthComponent`。**2026-09-29 连招链路换代**：`UHodgeAbilityDefinition` + `UHodgeComboDefinition` + `UHodgeComboComponent`（已挂 PlayerState）+ `UHodgeGameplayAbility_Definition` 成为主链路并在 09-28 的 `definition-combo` 验证中通过（不含命中伤害），旧 `BasicAttack` 成孤儿；同轮新增独立编辑器模块 `HodgeAbilityEditor`、装备四件套（未挂载）、仇远武器模型与 `CODE_REVIEW.md`。必须更新旧结论，不能继续沿用上一轮知识库的缺口描述。当前真实运行效果仍需独立验证。

## 当前工作区与提交基线

知识库以读取时的磁盘内容为准。**当前 HEAD 为 `abb9224`**（"Merge branch 'main'"，2026-09-29），工作区有 **2 个 `CodexText` 资产改动未提交**（`DA_Attack01_Timeline`、`DA_Attack_1`）。

相对上一轮记录（`6eec094`）之后入库的主要提交：`dc3fb9f` README 与知识库、`722ccbc` 技能编辑器与连击系统 + 导入仇远武器模型、`4db52d9` 修复仇远武器骨骼材质、`1be25a8` 夺舍 Lyra 装备系统四件套（未挂载）+ 装备设计文档、`abb9224` Merge。

源码规模：**`Source/Hodgepodge` 129 个 `.h` + 132 个 `.cpp`（约 33900 行）**（其中 `UI/` 41+40、`Equipment/` 4+4），另有独立模块 `Source/HodgeAbilityEditor/`（9 文件）。精确 Git HEAD、文件数量与 SHA-256 见 [扫描快照](Reference/snapshot.md)。

> ⚠️ **旧回退补记已作废**：2026-09-17 那版**双数组**（`Phases[]` + `Events[]`）Timeline / `HodgeComboSet` / `GA_Melee` 确实曾被丢弃；但"当前工作区没有 Timeline 源码、没有 `Attack.*` 标签"的结论**已不成立** —— Timeline 已按统一事件模型重写并提交，`Status.Attack.*` / `GameplayEvent.Attack.*` 也已集中声明。**仍然不存在**的是 Bundle 预加载（`PreloadPrimaryAssetBundles` / `PreloadPrimaryAssetsOnGrant`）与旧设计的 `Attack.Entry.*` / `Attack.Transition.*` / `Status.AttackMode.*`。

2026-09-29 更新：① **`HodgeComboSet` 不再是缺口** —— 连招数据已由 `UHodgeComboDefinition`（DataTable 跳转表）实现并挂载验证；② 攻击侧主链路换成 `UHodgeGameplayAbility_Definition`（由 `UHodgeAbilityDefinition` + Combo 驱动），旧 `UHodgeGameplayAbility_BasicAttack` 成孤儿待清理；🔴 **"打中掉血"始终不成立**（`DamageInteractionAllowedMultiplier` 恒 `0.0f`，连招能跑完但打不掉血）。详见 [2026-09-19 记录](22-update-2026-09-19.md)、[2026-09-28 记录](24-update-2026-09-28.md) 与 [2026-09-29 记录](25-update-2026-09-29.md)。

这意味着 checkout 到旧提交后，人工章节可能描述得比代码超前；从磁盘删除或启用草稿后，旧索引也可能失效。维护工具可以提示文件变化，但不能替代语义复核。

## 如何有效阅读代码

1. 先看头文件继承和 UPROPERTY，确定所有权与数据入口。
2. 看构造函数，确认组件是否真正 `CreateDefaultSubobject`。
3. 看生命周期，确认注册、初始化、复制通知和销毁清理。
4. 全局查函数调用，不只看函数定义。
5. 检查调用是否在注释中、条件分支是否可达。
6. 最后查资产配置和实际运行日志。

例如当前 InitializeAbilitySystem 已由有效 HeroComponent 调用，因此不再属于“零调用者”；仍需检查状态是否到达、组件是否挂载和实际生成的蓝图类型。
