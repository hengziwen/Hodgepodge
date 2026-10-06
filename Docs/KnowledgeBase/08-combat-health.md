# 属性、命中、装备与死亡

[返回知识库](README.md) · [本轮更新](26-update-2026-10-06.md)

> 当前核对：2026-10-06；运行结论仅限已记录范围。

## 当前分工

Experience 通过 AddComponents 给 Pawn 添加 HodgeCombatComponentBase 与 EquipmentManager。CombatComponent 协调连段和服务器检测会话、去重及命中结果；HodgeGameplayAbility_Melee 接收结果并构建/施加 GE，组件不直接写 Health。

检测支持身体、武器组件/socket 和 HitBox 配置。策略选择、采样和目标规则位于 Combat 目录；共享 FHodgeDamageRules 同时用于检测与伤害 Execution。

## 伤害已不恒为零

DamageExecution 捕获 Source CombatSet.BaseDamage，乘 SetByCaller.DamageMultiplier（缺省 1）、距离、物理材质及目标交互倍率。CanDamage 拒绝无效、自身、同一 ASC、死亡/零血目标；队伍和友伤标记决定合法性，未接队伍接口按无队伍处理。

HealthSet 管 Health/MaxHealth、Damage/Healing 元属性与夹取/通知；CombatSet 管 BaseDamage/BaseHeal。HealthComponent 已挂角色，监听耗尽并推进 DeathState，角色禁移动/碰撞并执行销毁清理。完整击杀消息、敌人 ASC 与重生策略仍待补。

2026-10-01 夹具已验证真实攻击输入→Timeline→服务器检测→既有 GE→100 扣到 70，以及 Listen Server 路径。正式五段普攻 HitWindows 目前均为空，需要填写来源、Profile 与 GE 后补完整回归；夹具通过不能代替正式资产接入。

## 默认装备与表现

PawnData.DefaultWeaponDefinition 指向 BP_Equipment_Sword，ASC 就绪后服务器装备；实例、Actor 引用复制并补迟到绑定。卸装按来源撤销技能/GE/属性句柄。

WeaponInstance 持有手持请求、阶段、计时器和预测/权威状态。可见 WeaponVisualMesh 与隐藏 SkeletalMesh 检测来源分离，检测 Mesh 保持 WeaponOnHand。闲置表现回到角色 Mesh 的 WeaponOnBack，BackTransform 是插槽偏移；悬浮/消隐中重新攻击可回手，不新建角色常驻武器组件。

## 仍需验证

正式伤害、完整敌人角色、死亡/重生、真实刀身覆盖、卸装/热卸载、相关性重新进入与独立专服。回背外观路径不是飞剑/投掷的伤害路径。

参考：[命中接入](../Design/combat-hit-windows.md)、[Melee 实现](../Design/melee-detection-ga-effects.md)、[伤害夹具验收](../Design/melee-experience-ge-validation-20261001.md)、[武器表现](../Design/weapon-presentation.md)。
