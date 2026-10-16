# Hodgepodge 本地知识库

> **2026-10-16**：[R1 检测独立／R3 角色收益隔离](../Validation/combat-detection-role-rules-2026-10-16.md) 已实施，13 个正式 Definition 已迁移；Editor/Game、149 项原生、资产与单人／Listen 客户端／PIE Dedicated 的指定回归通过。Client 普攻窗口疑似异常已记录，按用户要求暂缓处理。

> **2026-10-15**：攻击链路已完成 **Timeline → Montage 动画通知** 的彻底迁移；伤害倍率、敌人 AI、目标锁定、Dash/Sprint、角色朝向、参考 HUD、共鸣／暗涌均已交付并各自有验证报告。当前任务与待办见 [最新复核](29-current-work-and-backlog.md)，本轮详情见 [30 号更新](30-update-2026-10-15.md)。

> 2026-10-07：dev-AN 的攻击时机已改为 Montage 原生通知。迁移前的人工核对保持历史日期，本次实现与实际验证见 [通知迁移更新](27-update-2026-10-07-anim-notify.md)。

## 从这里开始

- **第一次上手**：[项目地图](01-project-map.md) → [所有权](02-architecture.md) → [启动链](03-runtime-startup.md) → [Pawn 初始化](04-pawn-initialization.md) → [当前接通状态](12-integration-backlog.md)。
- **当前在做/待办**：[当前任务与待办复核](29-current-work-and-backlog.md)（更新至 10-15，含已完成范围、后续优先级与用户暂缓事项）。
- **本轮详情**：[30 号更新](30-update-2026-10-15.md)（共鸣／暗涌、技能配置工具、架构评审、Timeline 退役）。

项目已接入 Experience/Init State、PlayerState ASC、输入相机、Main 固定移动动画层、统一 CombatComponent、默认剑、旋转锁、连段记忆与武器显隐。**攻击窗口与事件由 Montage 原生通知（`HodgeAnimNotify*`）承载**；正式五段已使用命中 Notify／NotifyState，第四段直接读取 `Bip001LHand`；仍需按美术动作调整覆盖与半径 —— 配置与数值测试不代表逐帧视觉打磨完成。

- [攻击能力配置手册](../Guides/attack-ability-configuration.md)：Definition、AnimNotify、HitCheck、Profile、连段表、授予与输入的统一字段说明；后续配置问答复用此入口。
- [通知驱动迁移](27-update-2026-10-07-anim-notify.md)、[迁移报告](../Validation/anim-notify-migration-2026-10-07.md)：旧系统退役、资源账本、作者入口与验证。
- [UI 基础闭环](28-update-2026-10-08-ui.md)、[UI 配置指南](../Guides/ui-foundation-configuration.md)：正式 Main HUD、血条／技能栏、菜单输入与玩家生命周期。⚠️ 方案**明确不引入 CommonGame / CommonUser**。
- [属性初始化／成长／装备加成](../Design/character-attribute-growth.md)：首版已实现；经验／存档后端未实现。
- [Lyra 迁移现状与检查清单](../Design/lyra-migration-status.md)：已迁移一部分的系统与逐项验收。
- [架构审查与分阶段重构方案](../Design/architecture-refactor-review-2026-10-15.md)：R9／R5 已实施，[对应记录](../Validation/game-data-grant-contracts-2026-10-15.md)；R1／R3 已实施，[149 项原生与指定玩法回归记录](../Validation/combat-detection-role-rules-2026-10-16.md)；其他阶段待实施。
- [项目 README](../../README.md)、[设计文档目录](../README.md)。

## 按系统阅读

- [项目地图](01-project-map.md)、[所有权](02-architecture.md)、[启动](03-runtime-startup.md)、[Pawn 初始化](04-pawn-initialization.md)。
- [数据资产](05-data-assets.md)、[输入](06-input.md)、[GAS 与连段](07-gas.md)、[命中伤害和装备](08-combat-health.md)。
- [相机/动画/旋转](09-camera-animation.md)（含目标锁定、软锁定镜头、Dash/Sprint、朝向模式、电影镜头）、[GameFeature](10-game-features.md)、[复制和服务器](11-network.md)。
- [构建配置](13-build-config.md)、[排障](14-troubleshooting.md)、[开发步骤](15-development-recipes.md)。
- [维护术语](17-maintenance-glossary.md)、[FAQ](18-faq.md)、[评审材料](20-code-review.md)。
- [源码、Tag、资产、配置参考](Reference/README.md)、[扫描快照](Reference/snapshot.md)、[工具说明](tools/README.md)、[项目文档索引](../README.md)。

## 文档可信度与历史

当前状态以 01～18 章节、本轮更新和实际代码/资产为准；Reference 是静态索引。Design / Guides / Validation 中既有规划也有已实施方案，状态在每篇开头注明。历史评审和验证只说明当时的基线，不能当作当前缺陷清单或当前测试通过证据。

- **按日更新**：[09-13](19-update-2026-09-13.md)、[09-17](21-update-2026-09-17.md)、[09-19](22-update-2026-09-19.md)、[09-22](23-update-2026-09-22.md)、[09-28](24-update-2026-09-28.md)、[09-29](25-update-2026-09-29.md)、[10-06](26-update-2026-10-06.md)、[10-07 通知迁移](27-update-2026-10-07-anim-notify.md)、[10-08 UI](28-update-2026-10-08-ui.md)、[**10-15**](30-update-2026-10-15.md)。
- **常驻页**：[当前任务与待办](29-current-work-and-backlog.md)、[接通清单](12-integration-backlog.md)、[验证范围](16-validation.md)。
- [更新前快照](History/snapshot-before-20261006.json) 保留原日期/hash；当前 `snapshot.json` 由维护工具重新生成。
- [更新前长版 README](../History/README-before-20261006.md) 保留旧架构解说。

check → 核对变化 → 修改人工章节 → refresh → check。不要清理默认玩法仍引用的 CodexText 资产，也不要把本轮静态核对说成新的游戏回归。
