# 项目定位、目录与文档可信度

> 最近源码核对：2026-09-22。源码接入状态与运行验收分开记录。
[返回首页](README.md)

## 工程是什么

项目描述的长期目标是 Dedicated Server、数据驱动、可扩展的动作 RPG 底层。它现在是单 Runtime 模块 `Hodgepodge`，引擎关联为 5.5。框架大量借鉴 Lyra，角色移动曾依赖 ALS，目前模块依赖已经移除 ALS，但资源和插件参考目录仍在。

“框架有这些类”与“游戏具备这些能力”必须分开。Experience、ASC、AbilitySet、相机模式栈有代码；可移动玩家、完整战斗循环、AI、装备背包、UI、后端不能据此自动认定完成。

## 目录阅读地图

- `Source/Hodgepodge/Public`：反射类型、组件接口、配置字段。理解一个系统先从这里确定契约。
- `Source/Hodgepodge/Private`：函数体和真实执行路径。确认实现必须看这里，尤其要排除注释代码。
- `Source/Hodgepodge/Hodgepodge.Build.cs`：模块依赖及 Iris 支持设置。
- `Source/*.Target.cs`：已有 Game / Editor 目标。独立 Server Target 需要另行补充。
- `Config`：默认地图、类重定向、AssetManager 扫描及输入设置。
- `Content/Main`：自有玩法数据、输入资源与角色动画；详见资产清单。
- `Content/qiuyuan`、`Content/Wuwa`、`Content/Assets`：模型、动画、音效等资源集合。资源存在不证明已经被 Pawn 使用。
- `Content/CodexText` 与 `Source/Hodgepodge/*/CodexText`：独立的 ALS 风格动画实验与 Survivor 小玩法；除 `AHodgeLocomotionLabMode` 复用 HodgeGameModeBase 外不继承主角色体系，主模块不引用它们。实验区持续精修（上半身 Overlay、左手握持、步幅、地形 IK），`Hodgepodge.Build.cs` 的**编辑器专用块**因此新增 `AnimationWarpingRuntime` / `AnimationWarpingEditor` 依赖；属实验、未走主 GAS 链，详见 [CodexText 地面运动实验说明](../../Tools/LocomotionLab/README.md)。
- `Content/Main/Character/Hero/GA_Attack.uasset`（已随 HEAD `993a9eb` 提交）、`Content/Main/Character/Hero/DA_TimeLineText.uasset`（旧字段模型的废数据）、`Content/Main/Input/InputAction/IA_Attack.uasset` 与 `Content/Main/Data/DA_Pover.uasset`（`HodgeAbilitySet`）：攻击相关资产；⚠️ 旧的 `Content/Main/Character/Hero/Ability/` 目录与 `GA_Melee` **已不存在**。存在不等于已接线。
- `Plugins/Developer/RiderLink`：开发工具插件，不计入游戏核心逻辑索引。
- `Plugins/ALS-Refactored-4.15`：主模块已移除直接代码依赖，但 ALS.uplugin 声明 EnabledByDefault=true，不能称插件已禁用。
- `Plugins/UnrealMCP`、`Plugins/McpAutomationBridge`：两个 Editor-only 自动化/MCP 插件，默认启用；不能据此宣称编辑器连接或工具调用已验证。
- `Saved`、`Intermediate`、`Binaries`：运行或构建产物，不作为架构事实来源。

`Core/PlayState` 的拼写是项目现状。知识库保留实际路径，不擅自改成 PlayerState。

## 文档冲突怎么处理

本地源码和配置回答“现在实现了什么”；资产编辑器回答“现在配了什么”；运行日志和验收回答“实际跑起来怎样”。项目 README 是历史状态摘要，Lyra 两篇文档是上游参考，V2 方案是目标设计。

2026-09-10 曾核对到 HeroComponent 全部注释、相机委托无调用者；2026-09-13 源码已启用两者。2026-09-17 进一步确认：ASC 接入收敛到单一组件路径、Receiver 生命周期已配对、AbilitySet 基础授予与额外输入移除已实现。2026-09-19 时间轴按统一事件模型重写并部分实测；2026-09-22 新增"移动取消后摇"消费方（`WaitMoveCancel` + 移动意图 + 取消窗口标签，**未验证**）。必须更新旧结论，不能继续沿用上一轮知识库的缺口描述。当前真实运行效果仍需独立验证。

## 当前工作区与提交基线

知识库以读取时的磁盘内容为准。**当前 HEAD 为 `993a9eb`**（"update:timeline测试"，2026-09-20）；它在 `77b7dba`（按统一事件模型重写的四个时间轴源文件 + `HodgeGameplayTags.h/.cpp` 中的 `Status.Attack.*` / `GameplayEvent.Attack.*`）之上，把时间轴测试资产、`Config/DefaultGameplayTags.ini` 与相关文档一次入库。工作区仍有未提交改动：**新增** `Public|Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.*`；**修改** `Hodgepodge.Build.cs`、`HodgeGameplayTags.h/.cpp`（+ `Status.Attack.Cancel` / `.Move`）、`Component/HodgeHeroComponent.h/.cpp`（移动意图）、四个 `CodexText` 源文件；以及 `Content/CodexText/**` 的动画/地图/时间轴测试资产（**不在快照哈希范围**）。精确 Git HEAD、文件数量、SHA-256 和工作区列表见 [扫描快照](Reference/snapshot.md)。

> ⚠️ **旧回退补记已作废**：2026-09-17 那版**双数组**（`Phases[]` + `Events[]`）Timeline / `HodgeComboSet` / `GA_Melee` 确实曾被丢弃；但"当前工作区没有 Timeline 源码、没有 `Attack.*` 标签"的结论**已不成立** —— Timeline 已按统一事件模型重写并提交，`Status.Attack.*` / `GameplayEvent.Attack.*` 也已集中声明。**仍然不存在**的是 `HodgeComboSet`、Bundle 预加载（`PreloadPrimaryAssetBundles` / `PreloadPrimaryAssetsOnGrant`）与旧设计的 `Attack.Entry.*` / `Attack.Transition.*` / `Status.AttackMode.*`。详见 [2026-09-19 记录](22-update-2026-09-19.md)。

这意味着 checkout 到旧提交后，人工章节可能描述得比代码超前；从磁盘删除或启用草稿后，旧索引也可能失效。维护工具可以提示文件变化，但不能替代语义复核。

## 如何有效阅读代码

1. 先看头文件继承和 UPROPERTY，确定所有权与数据入口。
2. 看构造函数，确认组件是否真正 `CreateDefaultSubobject`。
3. 看生命周期，确认注册、初始化、复制通知和销毁清理。
4. 全局查函数调用，不只看函数定义。
5. 检查调用是否在注释中、条件分支是否可达。
6. 最后查资产配置和实际运行日志。

例如当前 InitializeAbilitySystem 已由有效 HeroComponent 调用，因此不再属于“零调用者”；仍需检查状态是否到达、组件是否挂载和实际生成的蓝图类型。
