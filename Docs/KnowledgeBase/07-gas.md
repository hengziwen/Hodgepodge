> 2026-10-07 更新：本文中旧 Timeline／HitWindows 的字段与制作步骤仅保留为历史；当前攻击入口以 [通知配置手册](../Guides/attack-ability-configuration.md) 和 [通知迁移更新](27-update-2026-10-07-anim-notify.md) 为准。

# GAS、AbilitySet 与技能生命周期

[返回知识库](README.md) · [本轮更新](26-update-2026-10-06.md)

> 当前核对：2026-10-06；运行结论仅限已记录范围。

## 玩家 ASC 与输入

PlayerState 创建玩家 ASC、HealthSet/CombatSet，PawnExtension 以 PS 为 Owner、当前身体为 Avatar 接入；HeroComponent 绑定输入和默认相机。PlayerController.PostProcessInput 消费 ASC 输入，换 Pawn 要同时考虑解绑、重绑及输入清理。

PawnData 的 AbilitySets 在权威端授予，但 PlayerState 仍未保存完整 GrantedHandles 撤销账本。EquipmentManager 为装备来源保存句柄，卸装可精确撤销，不能混同两条路径。

## 现有普攻主链

Input → CombatComponent 选择跳转 → GAS 激活 Definition/Melee GA → 播放对应 FullBody Montage → 原生通知直接请求状态、手持与命中 → PostPhysics 采样 → Montage 生命周期／EndAbility 统一清理。

当前协调者是 Pawn 上的 HodgeCombatComponentBase，Experience 注入；旧 HodgeComboComponent 已移除，不在 PlayerState 另挂连段组件。旧 BasicAttack 类／Timeline 夹具已集中归档，退出活动 Source 和 Content。

CurrentComboTag 表示当前动作，ComboMemory.Node 记最近成功启动段。前四段自然结束/中断/移动取消后开始默认 1 秒保留计时，其他技能不刷新也不暂停它；记忆不保留攻击、旋转锁或命中标签。动作结束后仅允许 bAllowAfterExecutionEnded 的输入边续接；动作播放中继续检查窗口条件。末段后摇直接接第一段，末段结束不保留记忆。

输入缓存仍为单槽 0.3 秒，与连段记忆不同。客户端预测与服务器重新选边；服务器不接受客户端直接指定目标段，也不做历史窗口补偿。失败不得推进已确认节点，拥有者通过权威记忆校正恢复。

## 窗口与生命周期

Status.Rotation.Locked 由角色旋转组件消费，Status.Weapon.Hand 由 Definition 的 WeaponUseWindowTag 消费。手持请求用独立句柄，EndAbility 只释放自己的执行；命中窗口清理先于手持释放。

HitWindows 配置时，手持窗口必须覆盖实际命中并更早进入。现有正式五段 HitWindows 为空；不能凭蒙太奇和窗口标签宣称已施加伤害。

## Context 与效果

HodgeAbilitySystemGlobals 分配 FHodgeGameplayEffectContext；Melee GA 每次命中独立构建 Spec，写来源、HitResult、Origin、SetByCaller 倍率等，再由服务器施加。关系映射、Cost、激活组、失败标签沿用 GAS 主体系；GameplayCue 仍有注册/预加载缺口。

源码：[CombatComponent](../../Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp)、[Definition GA](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp)、[Melee GA](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp)、[ASC](../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp)。
