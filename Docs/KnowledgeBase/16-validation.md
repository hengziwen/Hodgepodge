> 2026-10-08 UI 专项验收见 [UI 基础闭环报告](../Validation/ui-foundation-2026-10-08.md)，不将该报告扩大为所有系统或发布打包通过。

> 2026-10-07 更新：本文中旧 Timeline／HitWindows 的字段与制作步骤仅保留为历史；当前攻击入口以 [通知配置手册](../Guides/attack-ability-configuration.md) 和 [通知迁移更新](27-update-2026-10-07-anim-notify.md) 为准。

# 实际验证范围与验收步骤

> **2026-10-15 更新**：Timeline 已退役、伤害倍率已修复、敌人 AI／锁定／Dash／朝向／参考 HUD／共鸣／暗涌均已交付并有各自验证报告。下方 2026-10-06 条目保留历史，**"正式五段 HitWindows=0"与"完整敌人 ASC"两条已过时**（见"当前待验证"）。本轮未复跑任何 PIE。

[返回知识库](README.md) · [本轮更新](30-update-2026-10-15.md) · [当前任务与待办](29-current-work-and-backlog.md)

> 当前核对：**2026-10-15**（HEAD `b702482`）；运行结论仅限 `Docs/Validation/` 里对应日期报告的范围。

## 已保存证据，不冒充本次复跑

2026-10-06 随后执行正式资源迁移：39 个资产迁入 Main，12 个蓝图编译、39 项资产校验和冷启动重载通过；Editor/Game 常规构建、23 项原生回归及实际 Editor 的 DefinitionWorkflow 成功；单人 552 条验证六次出招、属性成长、材质与武器完整回收，双人 Listen Server 100ms 1908 条验证四个副本与拥有者属性复制。具体命令、修正过的测试预期及范围见 [本次迁移记录](../Validation/main-assets-migration-2026-10-06.md)。

2026-10-06 属性首版：最终 Editor/Game 常规构建通过，新增属性原生 4 项与相关回归 19 项 Success，18 条资产校验记录 VALID；单人 298 条、双人 100ms 716 条验证出生/等级/装备数值与复制。完整 Hodge 的 headless Editor UI 用例崩溃，不计通过；具体命令、警告和范围见 [成长设计第 13 节](../Design/character-attribute-growth.md)。本项不是完整经验/存档或死亡复活玩法验收。

- 2026-10-01：统一战斗组件、Experience 注入和 Melee 检测/既有 GE 夹具，验证输入→服务器命中→100 扣到 70 与 Listen Server。见 [GE 验证](../Design/melee-experience-ge-validation-20261001.md)、[统一组件验证](../Design/combat-component-unification-validation-20261001.md)。
- 2026-10-05：Main 动画迁移、Stop/连续转身/FullBody 下半身姿势与双人副本；[动画说明](../../Content/Main/Character/Hero/Anim/README.md)。
- 旋转：原生 ConstraintsAndReplay 与真实输入、RootMotion 对照、Authority/Autonomous/Simulated。模拟代理约束与移动复制不同包，存在暂时差值；[实施记录](../Design/character-rotation-policy.md)。
- 连段：默认 1 秒记忆、移动/能力取消、超时、末段 04 后摇接 01；单人/双人 100ms 与预测拒绝恢复。紧贴窗口边界的网络用例存在未通过记录；[连段记录](../Design/combo-retention.md)。
- 武器：2 项原生测试、资产校验、显隐/连续手持/取消/倍速、双人 100ms 和预测拒绝；插槽版本最终 Editor/Game 构建退出 0、原生 2 项 Success、单人 453 条、双人 1420 条验证 WeaponOnBack。见 [武器记录](../Design/weapon-presentation.md)。
- 同类 cpp 合并后 Editor/Game 构建与 Combo 5 项、Weapon 2 项通过；Combo 有现有 Cue 路径 warning，见 [构建流程](../AI_DEVELOPMENT.md)。

本次 2026-10-06 文档任务只执行静态复核、只读资产 Commandlet 和 kb.py/链接检查，没有重新执行 C++ 构建、蓝图编译或 PIE。保存的历史成功范围不能自动扩大到当前所有场景。

### 2026-10-07 ~ 10-15 新增验证（每项都有报告，多数带 `*-results.json`）

| 领域 | 报告（`Docs/Validation/`） | 关键数字 |
|---|---|---|
| 动画通知迁移 | `anim-notify-migration-2026-10-07` | 攻击链路从 Timeline 切到 Montage 通知 |
| 客户端连段窗口修复 | `client-combo-window-2026-10-07`、`client-combo-refusal-investigation-2026-10-08` | 首帧服务器授权竞态 |
| UI 基础闭环 / UMG 重构 | `ui-foundation-2026-10-08`、`umg-authoring-refactor-2026-10-08` | 20 项原生、9 资产、单人/双人各 11 阶段 |
| 受击系统 | `hit-reaction-2026-10-08`、`hit-reaction-pie-network-2026-10-08` | 46 项 PIE/联机 |
| 动画同步 / Pivot | `animation-sync-pivot-2026-10-10`、`pivot-foot-slide-2026-10-10` | 原生 36 / PIE 78；脚踝 XY 降 86.7%/92.1% |
| 朝向模式 | `character-facing-implementation-2026-10-10` | 原生 24、PIE 24+6，覆盖 91.86% |
| Dash / Sprint | `dash-sprint-implementation` / `-timing` / `-transitions` / `-interrupt` / `dash-air-continuation-2026-10-13` | air 原生 45/45、PIE 227（含 dedicated/listen-lag） |
| 敌人 AI | `enemy-ai-implementation-2026-10-11`、`enemy-attack-target-key-2026-10-11` | 原生 57、蓝图 0 错、单人 14/14、Listen 18/18 |
| 参考 HUD | `main-hud-reference-2026-10-11` | 冷启动单人/Listen 各 12/12 |
| 装备导入 | `pover-equipment-import-2026-10-11`（+ csv/json） | 材质与贴图引用修复 |
| 翅膀 | `attack-wing-presentation-2026-10-12`、`wing-fade`、`wing-box-hit`、`wing-notify-montage-2026-10-14` | 后者原生 8/8、资产 16/16、PIE 86 |
| 武器通知回背 | `weapon-notify-end-behavior-2026-10-12` | 默认保留旧策略 |
| 锁定 / 软镜头 | `lock-on-implementation-2026-10-12`、`soft-lock-camera-2026-10-13`、`soft-lock-tracking-2026-10-13`、`lock-on-indicator-dash-start-2026-10-13` | 原生 29/31/36、四模式各 20~24 |
| 攻击 Warp | `attack-facing-motion-warp-2026-10-13` | 首版仅 `DA_Attack_1` 开启 |
| 独立技能移动取消 | `definition-move-cancel-2026-10-14` | 动作 12 + 连段 4、PIE 151 |
| 共鸣 / 暗涌 | `resonance-liberation-2026-10-15`、`havoc-dark-surge-2026-10-15`、`resonance-ability-icons`、`resonance-ability-layout`、`*-debug-fill` | 解放原生 81（0 失败）、暗涌原生 39；⚠️ 参数为原型初值，debug-fill 是调试命令 |

本次 2026-10-15 文档任务只执行静态复核、Editor 构建（`Target is up to date`）与 `kb.py check`/`refresh`，**没有**重新执行蓝图编译或 PIE。保存的历史成功范围不能自动扩大到当前所有场景，也不能把别人的 `*-results.json` 当作本次复跑结果。

## 当前待验证

**正式五段普攻的 AnimNotify 命中配置是否逐段填全**（旧结论"HitWindows=0"属 Timeline 时代，须按通知模型在编辑器复核）；配置后补五段伤害、去重、友伤、旋转与手持窗口覆盖。

仍待专项回归：死亡/重生、卸装/热卸载、相关性重新进入、GameplayCue（注册/预加载缺口）、换 Pawn 生命周期、Cook/打包与独立进程。**敌人 ASC 已补齐**（Guardian 已接入并有 10-11 报告），**UI 已出画面**（不引入 CommonGame）。

未执行 Cook/打包、独立进程客户端、Dedicated Server。没有 Server Target，不能将 Listen Server 测试写成专服验证。

## 可复现验收顺序

1. 在 ThirdPersonMap 核对实际 Experience、PawnData、Hero、ASC Owner/Avatar，验证移动、视角和相机。
2. Idle→Cycle 无 Start；松开进入 Stop；连续双向镜头转动无停顿跳变；Pivot 保持禁用。
3. FullBody 攻击下半身随源动作；RotationLock 前允许调整，窗口内镜头可转而胶囊 Yaw 锁定，退出平滑恢复。
4. 前四段移动取消/中断后 1 秒内接下一段，超时回第一段；按 1→2→3→5→4，末段 NextAttack 窗口点击直接回 1。
5. WeaponHand 开始显现，最后请求释放后宽限→回背→悬浮→消隐；连续连段不回背，回收中攻击回手。悬浮/消隐可见 Mesh 挂 WeaponOnBack，逻辑来源仍 WeaponOnHand。
6. 双人 100ms 检查 Authority、拥有者、模拟代理；再分别增加边界、失败、死亡和重新进入场景，不一并宣称已通过。
7. 锁定：中键获取/解除、鼠标甩动切换、Dash/Sprint 暂让与恢复、镜头死区与停手宽限；攻击自动索敌**应保持关闭**。
8. Dash/Sprint：Shift 起停、离地续冲、Pivot 滑步、被攻击打断；⚠️ 体力系统已按用户要求移除，别按旧用例去找它。
9. 共鸣/暗涌：E 技能（8 秒 CD）、R 解放（满充、12 秒 CD、前半段电影镜头）；⚠️ 不要用 `HodgeFillResonance` / `HodgeFillDarkFlow` 的调试回满当作正式充能验证。

每次记录命令、退出码、资产路径、输入时刻、网络模式、预测/权威差异、首个有效错误。Saved 中原始记录为本机证据，不纳入源码。
