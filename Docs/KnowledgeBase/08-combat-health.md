> 2026-10-07 更新：本文中旧 Timeline／HitWindows 的字段与制作步骤仅保留为历史；当前攻击入口以 [通知配置手册](../Guides/attack-ability-configuration.md) 和 [通知迁移更新](27-update-2026-10-07-anim-notify.md) 为准。
> **2026-10-15 更新：伤害交互倍率不再恒为零**（`HodgeDamageExecution.cpp:186` 按 `FHodgeDamageRules::CanDamage` 取 `1.f / 0.f`，判敌我已启用）；**受击系统**（`UHodgeHitReactionComponent`）与**属性成长/装备加成**已接入并有验证。详见 [本轮更新](30-update-2026-10-15.md)。

# 属性、命中、装备与死亡

> 2026-10-24 合并说明：main 的背包／伤害跳字与 dev-Combat 的受击改造共同保留；下列记录仍按原日期和验证范围解读。

> 2026-10-16：新增 [Lyra 本地伤害跳字](../Design/damage-numbers-lyra.md)，正式输入为 `OnDamageAccepted` 的实际 HP 损失，经攻击者 Controller 的拥有者 RPC 送往本地 NiagaraText，不从 Health 复制差值反推每刀伤害。配置见 [指南](../Guides/damage-number-configuration.md)，真实验证边界见 [报告](../Validation/damage-numbers-lyra.md)。

2026-10-17：[ACT 总设计](../Design/act-combat-animation-feedback-design.md)与[实施／验收计划](../Design/act-combat-animation-feedback-plan.md)已进入实现，方向强动画、持续／恢复、玩家恢复与曲线移动见[本轮验证](../Validation/act-combat-animation-feedback-2026-10-17.md)。10-16 [审计](../Design/hit-reaction-implementation-audit-2026-10-16.md)保留切片前快照，[表现验证](../Validation/hit-presentation-2026-10-16.md)保留当日结果；[假人](../Guides/hit-reaction-dummy.md)不代替正式 Guardian、物理输入与最终演出验收。Client 后续连段问题继续独立保留。

后续已按用户要求退出八个程序强动作的正式引用，Hero／假人接入完整现有 Behit S/B、Fly/Push、StandUp；地面／落地根运动与空中 CMC 分阶段互斥，当前资产和实际测试见[Behit 实施记录](../Validation/authored-behit-root-motion-2026-10-17.md)。不据此宣称 Guardian 专属素材或最终手感完成。

攻击字段含义与配置步骤统一见[攻击能力配置手册](../Guides/attack-ability-configuration.md)；后续配置问答优先引用手册相应章节，本章说明系统职责和状态。

[返回知识库](README.md) · [本轮更新](30-update-2026-10-15.md) · [接通清单](12-integration-backlog.md)

> 当前核对：**2026-10-15**（HEAD `b702482`）；运行结论仅限 `Docs/Validation/` 里对应日期报告的范围。

## 当前分工

属性初始化/成长首版已实施：PlayerState 拥有属性协调器和等级/装备来源记录，HealthComponent 只绑定/通知；出生等待默认装备属性后补满，角色升级保持比例，装备变化保留绝对值。重算期间上限夹取延后，真实资源变化仍记录；现有属性名和伤害结算入口保持。死亡 GA 已由用户接入，真实致死 GE 能触发死亡事件并完成 8 秒死亡阶段与 Pawn 销毁，但互斥计数和 PIE 清理仍有错误；复活玩法待接入。见 [死亡验证](../Validation/jump-death-2026-10-06.md) 与 [成长方案](../Design/character-attribute-growth.md)。

Experience 通过 AddComponents 给 Pawn 添加 HodgeCombatComponentBase 与 EquipmentManager。2026-10-16 R1：Combat 只协调连段／请求／网络，旧检测入口代理唯一的 HodgeHitDetectionComponent；检测服务保存会话、来源、几何历史及姿势账本，既有能力 Task 保留 PostPhysics 节拍。Melee 负责目标资格、每目标命中去重、GE 提交及实际伤害确认，检测组件不直接写 Health。

2026-10-16 R3：Melee 发布带 Execution／Spec／SettlementId 的强类型结算，Definition 的 AttackRules 持有可选共鸣／暗流配置，每执行创建独立规则及阶段账本；角色级拢音仍由 HavocRules 限流。正式 13 个资产已迁移并清空旧收益值，新旧路径不并行。[实现与完整验证](../Validation/combat-detection-role-rules-2026-10-16.md)。

检测支持身体、武器组件/socket 和 HitBox 配置。策略选择、采样和目标规则位于 Combat 目录；共享 FHodgeDamageRules 同时用于检测与伤害 Execution。

2026-10-06 本轮扩展：Definition 可配置无需常驻组件的球／盒／胶囊检测体，支持服务器锚点、固定／跟随、Window／Point 多段、共享组作用域与指定目标。Standalone 走正常 GAS 激活和远端取消，ComboCoordinated 保留连段授权。Actor 独立存续、通用客户端 TargetData、锁定系统及演出尚未实现。配置与验证见[使用说明](../Design/skill-hit-volumes-usage.md)和[本轮验收](../Validation/skill-hit-volumes-2026-10-06.md)。

## 伤害已不恒为零

DamageExecution 捕获 Source CombatSet.BaseDamage，乘 SetByCaller.DamageMultiplier（缺省 1）、距离、物理材质及目标交互倍率。CanDamage 拒绝无效、自身、同一 ASC、死亡/零血目标；队伍和友伤标记决定合法性，未接队伍接口按无队伍处理。

HealthSet 管 Health/MaxHealth、Damage/Healing 元属性与夹取/通知；CombatSet 管 BaseDamage/BaseHeal。HealthComponent 已挂角色，监听耗尽并推进 DeathState，角色禁移动/碰撞并执行销毁清理。完整击杀消息与重生策略仍待补（**敌人 ASC 已补齐**，见下）。

### 受击系统（2026-10-08 接入并验证）

2026-10-16 新增[受击表现切片](../Validation/hit-presentation-2026-10-16.md)：四方向短 Additive、独立接触音画与去重、重复地面硬直续播、正式五段 Reaction 编排及配置校验。已提供离线画面对照与实际回归；素材和主观手感仍需验收，公共动画字段未迁移，旧基线不代表新版本验证。

`UHodgeHitReactionComponent`（`UPawnComponent`）挂在 `AHodgeCombatCharacter`（支持 `OverrideProfile`）。链路：攻击方 Melee GA `BeginHit` → `HealthSet::RecordDamage` → **目标侧自己的** `UHodgeGameplayAbility_HitReaction` 执行（**动画通知驱动，非 Timeline**）。四级 Body / 独立判定 / Impact / 目标动画与恢复。验证 46 项 PIE/联机用例（[报告](../Validation/hit-reaction-pie-network-2026-10-08.md)）；正式动画与联机手感仍待调。

### 属性成长与装备加成（首版已接入）

PlayerState 持有属性协调器；三层职责 = 成长（Instant Override）/ 装备（Infinite Additive）/ 资源。正式资产 4 个：`DA_Pover_Stats` / `CT_Pover_Growth` / `DA_Sword_Stats` / `CT_Sword_Growth`（1~90 级）。API：`SetCharacterLevel` / `SetEquipmentLevel` / `InitializeCharacterProgression` / `RestoreCharacterHealth`。曲线是**示例数据**。见 [`character-attribute-growth.md`](../Design/character-attribute-growth.md) 与 [08 参考索引](Reference/source-abilitysystem.md)。

2026-10-01 夹具已验证真实攻击输入→服务器检测→既有 GE→100 扣到 70，以及 Listen Server 路径（当时经 Timeline 派发，**Timeline 现已退役**）。2026-10-06 曾读取正式五段 Definition 的 HitWindows 配置；10-07 起命中改由 **Montage 动画通知**承载，旧字段结论不能直接沿用。

⚠️ **仍待编辑器确认**：正式 `DA_Attack_1~5` 的 AnimNotify 命中配置是否逐段填全。夹具通过不能代替所有正式动作接入。共鸣解放 / 暗涌的验证报告（10-15）已含命中与伤害项。

## 默认装备与表现

PawnData.DefaultWeaponDefinition 指向 BP_Equipment_Sword，ASC 就绪后服务器装备；实例、Actor 引用复制并补迟到绑定。卸装按来源撤销技能/GE/属性句柄。

WeaponInstance 持有手持请求、阶段、计时器和预测/权威状态。可见 WeaponVisualMesh 与隐藏 SkeletalMesh 检测来源分离，检测 Mesh 保持 WeaponOnHand。闲置表现回到角色 Mesh 的 WeaponOnBack，BackTransform 是插槽偏移；悬浮/消隐中重新攻击可回手，不新建角色常驻武器组件。

## 仍需验证

真实刀身覆盖（正式五段的通知配置）、死亡/重生、卸装/热卸载、相关性重新进入、独立专服与 Cook/打包。**敌人角色已不再是缺口**（Guardian 已接入主流程，见 [敌人 AI 实施报告](../Validation/enemy-ai-implementation-2026-10-11.md)），**伤害交互倍率也已修复**。回背外观路径不是飞剑/投掷的伤害路径。

参考：[命中接入](../Design/combat-hit-windows.md)、[Melee 实现](../Design/melee-detection-ga-effects.md)、[伤害夹具验收](../Design/melee-experience-ge-validation-20261001.md)、[武器表现](../Design/weapon-presentation.md)。
