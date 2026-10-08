> 2026-10-07 更新：本文中旧 Timeline／HitWindows 的字段与制作步骤仅保留为历史；当前攻击入口以 [通知配置手册](../Guides/attack-ability-configuration.md) 和 [通知迁移更新](27-update-2026-10-07-anim-notify.md) 为准。

# 属性、命中、装备与死亡

攻击字段含义与配置步骤统一见[攻击能力配置手册](../Guides/attack-ability-configuration.md)；后续配置问答优先引用手册相应章节，本章说明系统职责和状态。

[返回知识库](README.md) · [本轮更新](26-update-2026-10-06.md)

> 当前核对：2026-10-06；运行结论仅限已记录范围。

## 当前分工

属性初始化/成长首版已实施：PlayerState 拥有属性协调器和等级/装备来源记录，HealthComponent 只绑定/通知；出生等待默认装备属性后补满，角色升级保持比例，装备变化保留绝对值。重算期间上限夹取延后，真实资源变化仍记录；现有属性名和伤害结算入口保持。死亡 GA 已由用户接入，真实致死 GE 能触发死亡事件并完成 8 秒死亡阶段与 Pawn 销毁，但互斥计数和 PIE 清理仍有错误；复活玩法待接入。见 [死亡验证](../Validation/jump-death-2026-10-06.md) 与 [成长方案](../Design/character-attribute-growth.md)。

Experience 通过 AddComponents 给 Pawn 添加 HodgeCombatComponentBase 与 EquipmentManager。CombatComponent 协调连段和服务器检测会话、去重及命中结果；HodgeGameplayAbility_Melee 接收结果并构建/施加 GE，组件不直接写 Health。

检测支持身体、武器组件/socket 和 HitBox 配置。策略选择、采样和目标规则位于 Combat 目录；共享 FHodgeDamageRules 同时用于检测与伤害 Execution。

2026-10-06 本轮扩展：Definition 可配置无需常驻组件的球／盒／胶囊检测体，支持服务器锚点、固定／跟随、Window／Point 多段、共享组作用域与指定目标。Standalone 走正常 GAS 激活和远端取消，ComboCoordinated 保留连段授权。Actor 独立存续、通用客户端 TargetData、锁定系统及演出尚未实现。配置与验证见[使用说明](../Design/skill-hit-volumes-usage.md)和[本轮验收](../Validation/skill-hit-volumes-2026-10-06.md)。

## 伤害已不恒为零

DamageExecution 捕获 Source CombatSet.BaseDamage，乘 SetByCaller.DamageMultiplier（缺省 1）、距离、物理材质及目标交互倍率。CanDamage 拒绝无效、自身、同一 ASC、死亡/零血目标；队伍和友伤标记决定合法性，未接队伍接口按无队伍处理。

HealthSet 管 Health/MaxHealth、Damage/Healing 元属性与夹取/通知；CombatSet 管 BaseDamage/BaseHeal。HealthComponent 已挂角色，监听耗尽并推进 DeathState，角色禁移动/碰撞并执行销毁清理。完整击杀消息、敌人 ASC 与重生策略仍待补。

2026-10-01 夹具已验证真实攻击输入→Timeline→服务器检测→既有 GE→100 扣到 70，以及 Listen Server 路径。2026-10-06 本轮实际读取正式五段 Definition，用户已填写 HitWindows；本轮保留其配置，并验证真实第一段输入被独立技能打断时连段记忆保持。仍未完成五段实际刀刃覆盖的完整验收，测试例子通过不能代替所有正式动作接入。

## 默认装备与表现

PawnData.DefaultWeaponDefinition 指向 BP_Equipment_Sword，ASC 就绪后服务器装备；实例、Actor 引用复制并补迟到绑定。卸装按来源撤销技能/GE/属性句柄。

WeaponInstance 持有手持请求、阶段、计时器和预测/权威状态。可见 WeaponVisualMesh 与隐藏 SkeletalMesh 检测来源分离，检测 Mesh 保持 WeaponOnHand。闲置表现回到角色 Mesh 的 WeaponOnBack，BackTransform 是插槽偏移；悬浮/消隐中重新攻击可回手，不新建角色常驻武器组件。

## 仍需验证

正式伤害、完整敌人角色、死亡/重生、真实刀身覆盖、卸装/热卸载、相关性重新进入与独立专服。回背外观路径不是飞剑/投掷的伤害路径。

参考：[命中接入](../Design/combat-hit-windows.md)、[Melee 实现](../Design/melee-detection-ga-effects.md)、[伤害夹具验收](../Design/melee-experience-ge-validation-20261001.md)、[武器表现](../Design/weapon-presentation.md)。
