> 2026-10-15：正文已按 HEAD `b702482` 重写。**Timeline 已彻底退役**（迁入 `Archive/Timeline/`，改动画通知驱动）、**伤害倍率恒 0 已修复**、**敌人 AI / 瞄准锁定 / Dash / 朝向 / 参考 HUD 均已交付并验证**。详见 [本轮更新](30-update-2026-10-15.md) 与 [当前任务与待办](29-current-work-and-backlog.md)。

> 2026-10-08：UI 基础闭环已接通；当前方案不引入 CommonGame／CommonUser／ModularGameplayActors，详见 [UI 更新](28-update-2026-10-08-ui.md)。
> 2026-10-07：旧 Timeline／HitWindows 的字段与制作步骤仅保留为历史；当前攻击入口以 [通知配置手册](../Guides/attack-ability-configuration.md) 和 [通知迁移更新](27-update-2026-10-07-anim-notify.md) 为准。

# 当前状态、断点与接通顺序

[返回知识库](README.md) · [本轮更新](30-update-2026-10-15.md) · [项目文档索引](../README.md)

[Lyra 迁移检查清单](../Design/lyra-migration-status.md) 单独记录死亡 GA、UI、Cue、消息、加载、阵营、库存和交互的迁移状态。

> 当前核对：**2026-10-15**，HEAD `b702482`，工作区干净；已执行 Editor 构建（`Target is up to date`）。运行结论仅限 `Docs/Validation/` 里对应日期报告的范围。

## 已接通，避免重复建设

- **KB-01～04**：Experience 选择/加载、PawnData 注入、Hero 输入相机、PlayerState ASC→Pawn Avatar 已成立，正式角色可移动、可攻击、可锁定。
- **KB-08：伤害不再恒 0** ✅：`HodgeDamageExecution.cpp:186` 现在按 `FHodgeDamageRules::CanDamage(...)` 取 `1.f / 0.f`，判敌我在 `HodgeDamageRules.cpp:31-45`（`IGenericTeamAgentInterface`，未接队伍按 NoTeam），**未注释、启用中**，并参与最终 `DamageDone`。
- **KB-15/16/22：`HodgeComboComponent` 已删除**，统一为单一复制的 Pawn `CombatComponent`（见 `combat-component-unification-validation-20261001.md`）；**Timeline 已退役**，攻击窗口/事件改由 Montage 动画通知驱动（见 [30 号记录](30-update-2026-10-15.md)）。移动取消、连段记忆、末段后摇重开、独立技能移动取消均有验证。
- **KB-17：受击与死亡** ✅：`UHodgeHitReactionComponent`（`UPawnComponent`）挂在 `AHodgeCombatCharacter`；链路 = 攻击方 Melee GA `BeginHit` → `HealthSet::RecordDamage` → 目标侧 `UHodgeGameplayAbility_HitReaction`。**通知驱动**。46 项 PIE/联机用例通过（`hit-reaction-pie-network-2026-10-08.md`）。死亡流程（`DeathState` 状态机 + `UninitAndDestroy`）已串联。
- **KB-18/19：UI 已出画面** ✅：UI 基础闭环 + UMG 可视化重构完成（截图 `Saved/UIFoundation/UI-HUD-final.png`）。**明确不引入 CommonGame / CommonUser / ModularGameplayActors**（与 KB-19 旧描述的"需引入 CommonGame"相反，这是用户决策）。`[UI-MIGRATION-PENDING]` 标记从 24 处降到 **20 处**（10 头 + 10 cpp）。
- **KB-23**：`HodgeAbilityEditor` 六面板编辑器仍存在（注意：它编辑的是 Definition，而**执行配置已从 Timeline 换成 AnimNotify**，编辑器里的时间轴面板需要按新模型复核）。
- **KB-24：装备已挂载** ✅：`EquipmentManager` 由 Experience `AddComponents` 注入，默认剑与 ASC 初始化链已挂载，来源句柄可撤销。
- **敌人 AI（原 KB-09）** ✅ v0.1 已接入主流程：`AIC_Guardian` + `BT_Guardian/BB_Guardian` + `HodgeBTTask_ExecuteAbility` + `HodgeEnemyInitComponent` + `GA_Guardian_Death` + `DA_Guardian_Stats`（350HP / 15 伤害）；原生 57、蓝图 0 错、单人 14/14、Listen 18/18（`enemy-ai-implementation-2026-10-11.md`）。
- **目标锁定（新）** ✅：`UHodgeLockOnComponent`（Hero PawnComponent）+ 短 GA + `DA_HeroLockOn` + `UHodgeCameraMode_LockOn`；中键获取/解除、鼠标甩动切换、服务器确认与拒绝恢复、Dash/Sprint 暂让与恢复。
- **Dash / Sprint（新）** ✅：`UHodgeGameplayAbility_Dash` / `_Sprint`（基类 `HodgeGameplayAbility_MovementAction`）+ `UHodgeSprintAbilityProfile` + `UHodgeLocomotionPolicyComponent`；离地续冲、Pivot 滑步修复、技能中断（标签映射）均有验证。**体力系统按用户要求暂时移除**。
- **角色朝向（新）** ✅：`UHodgeCharacterRotationComponent` + `EHodgeCharacterFacingDriver{Movement,Controller,ActionDirection}` / `EHodgeLocomotionStyle{FreeDirectional,ReservedStrafe}`；`bUseControllerRotationYaw` / `bOrientRotationToMovement` 只是执行结果，不是配置入口。
- **攻击 Warp（新）** ✅ 首版：`HodgeAttackMotionWarpingComponent` + `HodgeRootMotionModifier_AttackFacing`；目前仅 `DA_Attack_1` 开启。
- **翅膀（新）** ✅ 攻击限定表现：`UHodgeAnimNotifyState_WingAction` + `HodgeWingComponent`（同步/淡出），命中用通知内 Box（WingMesh NoCollision）。⚠️ **旧 Pover 翅膀仍禁用**，不能一概而论"翅膀不可用"。
- **共鸣 / 暗涌（新）** ✅ 首版：`UHodgeResonanceSet` / `HodgeHavocSet` / `HodgeAbilityCost_ResonanceEnergy` / 电影镜头通知；E 键共鸣技能、R 键共鸣解放、暗流燃料与强化。⚠️ 参数是**原型初值**，`HodgeFill*` 是调试命令。
- **属性成长（原已完成）**：成长 Profile、Instant 基础 GE、Infinite 装备加成、PlayerState 协调器已接入；曲线为示例数据。
- **攻击命中检测**：`UHodgeHitDetectionProfile` + 三种策略（`UHodgeSocketSweepStrategy` / `UHodgeBoxSweepStrategy` / `UHodgeShapeQueryStrategy`），检测体配置为 `FHodgeHitVolumeConfig`（Sphere/Box/Capsule + 锚点）；调试绘制 CVar `Hodge.Combat.DebugDraw.Weapon/Body/HitBox`（仅服务器绘制）。

## 剩余待办

### 正式五段普攻的通知配置（🔴 最需要编辑器确认）

命中链路本身已通（Melee GA `HodgeGameplayAbility_Melee.cpp:406` 构造并提交伤害、伤害规则已启用），但**正式 `DA_Attack_1~5` 的 AnimNotify 命中配置是否逐段填全**仍只能在编辑器确认。旧结论"五个 DA_Attack 的 HitWindows 均为空"属 Timeline 时代，读到时要用新模型重新核对，别直接照抄。

### KB-05：PawnData 授予撤销

`PlayerState::GiveToAbilitySystem` 仍未收集来源句柄；换配置、重生、重复授予的撤销策略需要补（装备管理器自身的 `GrantedHandles` 已实现，可直接参照）。

### KB-06/12/14：生命周期与热卸载

专项覆盖换 Pawn、死亡/重生、Controller/Avatar 解除顺序、输入与镜头清理、GameFeature 停用/再激活、多世界隔离、网络相关性重新进入。**不能仅以函数对称判断通过**。

### KB-07：GameplayCue

路径观察者创建、移除及启动预加载仍不完整；显式 `GameplayCueNotifyPaths` 未配置，存在扫描全 Game 的警告。这是 [29 号待办](29-current-work-and-backlog.md) 里的 P2。

### KB-10：专服与独立进程

**无 Server 专用 Target**；`TryDedicatedServerLogin` 仍有提前返回风险；独立进程、Cook/打包未验收。P1（约 3~7 人日）。

### KB-20：下蹲仍被屏蔽

`HodgeHeroComponent` 的 Crouch 绑定仍是注释状态，`HodgeLocomotionLab.cpp` 另有 `bCanCrouch=false`。按用户暂缓处理。

### KB-21：`GetBlendInfo` —— ✅ 已修复（2026-10-13）

软锁定镜头那轮已"修正 CameraMode 栈顶查询"（见 [29 号记录](29-current-work-and-backlog.md)）。本条关闭；引用旧描述时注意。

### 结构重构（新增，[评审报告](../Design/architecture-refactor-review-2026-10-15.md)）

只读审查，**尚未实施**。优先 R1：把命中检测从 `CombatComponent` 抽成独立 `UHodgeHitDetectionComponent`（`HodgeCombatComponentBase.cpp` 已 1525 行，公共逻辑在吸收角色专属规则）。另含 R2 电影/暗流会话解耦、R3 剥离角色收益规则、R5 统一授予契约、R7 Editor 工具按域拆分；分阶段 0~5，估 14~24 工作日。

### 明确"只设计不实现"的项

- [Inventory 集成](../Design/inventory-equipment-integration.md)：用户明确只设计；C++ **无任何 `UHodgeInventory*`**。
- [近战索敌与攻击吸附](../Design/melee-attack-assist.md)：草案，接口尚未创建。
- 加载屏、战斗消息 Router、交互：缺执行器 / 未接 / 是占位。

## 操作入口（更新）

- PawnData：`/Game/Main/Data/PawnData/DA_Dafult_PawnData`（**不要误改 AssetManager 的同名回退**）
- **攻击配置统一入口**：[`Docs/Guides/attack-ability-configuration.md`](../Guides/attack-ability-configuration.md)（Definition、AnimNotify、HitCheck、Profile、连段表、能力授予、输入）
- 锁定：`DA_HeroLockOn` + [`lock-on-configuration.md`](../Guides/lock-on-configuration.md)
- Dash / Sprint：[`dash-sprint-configuration.md`](../Guides/dash-sprint-configuration.md)、[`dash-interrupt-configuration.md`](../Guides/dash-interrupt-configuration.md)
- 朝向：[`character-facing-configuration.md`](../Guides/character-facing-configuration.md)
- 受击：[`hit-reaction-configuration.md`](../Guides/hit-reaction-configuration.md)
- 共鸣 / 暗涌：[`resonance-skill-configuration.md`](../Guides/resonance-skill-configuration.md)、[`resonance-liberation-configuration.md`](../Guides/resonance-liberation-configuration.md)、[`resonance-ability-icons.md`](../Guides/resonance-ability-icons.md)
- 翅膀：[`wing-notify-montage-configuration.md`](../Guides/wing-notify-montage-configuration.md)
- HUD：[`main-hud-reference-configuration.md`](../Guides/main-hud-reference-configuration.md)、[`ui-foundation-configuration.md`](../Guides/ui-foundation-configuration.md)
- 敌人：[`enemy-ai-configuration.md`](../Guides/enemy-ai-configuration.md)
- **归档**：`Archive/Timeline/`（旧 Timeline 代码与资产，**不参与构建**）、`Archive/UI/NativePrototype/`
- **Editor 工具**：`Tools/*/README.md`（均为 Editor-only Python，经本机 MCP 网关执行，不是运行时代码）

详见 [本轮更新](30-update-2026-10-15.md)、[验收范围](16-validation.md)。
