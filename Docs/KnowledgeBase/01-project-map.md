# 项目定位、目录与文档可信度

[返回知识库](README.md) · [本轮更新](30-update-2026-10-15.md) · [接通清单](12-integration-backlog.md)

> 当前核对：**2026-10-15**（HEAD `b702482`，工作区干净）；运行结论仅限 `Docs/Validation/` 里对应日期报告的范围。

## 当前基线

引擎 UE 5.5.4，单一业务 Runtime Hodgepodge，加 Editor 工具模块 HodgeAbilityEditor。CodexText 位于主模块与 Content 内，不能称独立模块。

默认地图 ThirdPersonMap，编辑器启动 MainMenu。默认 Experience 使用 `/Game/Main/Data/PawnData/DA_Dafult_PawnData`，创建 BP_Hero_Pover；配置回退 PawnData 仍在 Main/Data 根目录。

## 目录职责

- Source/Hodgepodge/Public 与 Private：反射接口、实现；同类实现集中一个主 cpp。
- Source/HodgeAbilityEditor：Definition 编辑器、动画文本导入/引用修正/编译诊断，不进入 Game 目标。
- Content/Main/Character/Hero/Anim：正式主图 ABP_Pover_Base、固定层、骨架、移动和 FullBody 攻击资源。
- Content/Main/Character/Hero/Ability/BasicAttack、Main/Data/Combo、Main/Weapon/Presentation、Main/Data/CharacterStats：正式技能、窗口、连段、武器表现与成长配置。
- Content/CodexText：菜单、测试夹具、旧资源和历史备份；正式角色依赖的 39 个资产已迁入 Main。
- Content/CodexText/LyraAnimation：实验地图、测试角色和研究入口；旧 ALS 风格 CodexText 源码仍是另一套实验。
- Equipment、Combat、Component：默认装备与表现、检测规则与数据、连段/检测协调和旋转约束。
- UI：CommonUI 迁移层；有停用代码和未接前端依赖，文件数量不代表完成度。
- Config、uproject、Build.cs/Target：配置与模块事实；Plugins 的默认启用与主模块直接依赖是不同层级。**Tag 注册已按域拆分**到 `Config/Tags/*.ini`（`ResonanceSkillTags` / `HavocTags` / `AbilityHUDTags`）。
- **`Archive/`**：从活动源码/内容退出的归档（`Archive/Timeline/` = 退役的 Timeline C++ 与资产、`Archive/UI/NativePrototype/`），**不参与 UBT/UHT/Cook**，其中的反射类型当前 Runtime 不提供。
- **`Tools/`**：Editor-only Python 工具（`Tools/*/README.md`，如 `DashSprint` / `LockOn` / `CharacterFacing` / `AttackWarp` / `Havoc` / `ResonanceSkill` / `Wing*` / `HUDReference` / `UIFoundation` / `ModelRepair` / `PoverEquipment` / `AnimNotifyCombat` / `ChiR24MCP`），经本机 MCP 网关驱动编辑器，**不是运行时代码**。
- `Content/CodexText/DefinitionCombo`、`Content/Main/Data/CharacterStats`、`Content/Main/Character/Hero/Anim`：各有自己的 README，是当前正式内容侧的主要入口。

## 已纠正的旧结论

CombatComponent 不再是占位；不存在旧独立 ComboComponent，职责已统一到 Pawn 的 HodgeCombatComponentBase。装备通过 Experience AddComponents 注入，不能以没有 CreateDefaultSubobject 判断未挂载。伤害倍率已按目标规则计算且**不再恒为零**（`HodgeDamageExecution.cpp:186` 走 `FHodgeDamageRules::CanDamage`）。

**2026-10-15 起新增的纠正**：

- **Timeline 已彻底退役**（`HodgeAbilityTimeline` / `PlayTimeline` / `TimelineEvaluator` 移出 `Source/`，迁入 `Archive/Timeline/`）；攻击窗口与事件改由 **Montage 动画通知**驱动。凡旧章节说"Timeline 锁 Yaw / Timeline 派发窗口"的，都是历史。
- **"正式五段 HitWindows 仍为空"已不再适用** —— 命中配置改由 AnimNotify 承载；但正式 `DA_Attack_1~5` 的通知是否逐段填全**仍需编辑器确认**。
- **敌人 AI 不再是缺口**：Guardian（AIC / BT / BB + GAS + EnemyInit）已接入主流程，有 10-11 验证报告。
- **UI 已出画面**，且方案**明确不引入 CommonGame / CommonUser / ModularGameplayActors**。
- **`GetBlendInfo` 取栈底层**的缺陷已修复（2026-10-13）。
- 新增玩法：目标锁定（含软锁定镜头）、Dash/Sprint、角色朝向模式、攻击前摇 Motion Warping、翅膀攻击表现、共鸣（Resonance）与暗涌（Havoc）。**体力系统按用户要求移除**。

精确文件数、HEAD、hash 用 [快照](Reference/snapshot.md)，行为与配置用 [本轮更新](26-update-2026-10-06.md)。README 为当前入口，旧 README、评审和每日更新均标记历史。保留 Dafult、PlayState 等实际拼写。
