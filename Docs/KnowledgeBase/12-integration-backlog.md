> 2026-10-07 更新：本文中旧 Timeline／HitWindows 的字段与制作步骤仅保留为历史；当前攻击入口以 [通知配置手册](../Guides/attack-ability-configuration.md) 和 [通知迁移更新](27-update-2026-10-07-anim-notify.md) 为准。

# 当前状态、断点与接通顺序

[返回知识库](README.md) · [本轮更新](26-update-2026-10-06.md)

[Lyra 迁移检查清单](../Design/lyra-migration-status.md) 单独记录死亡 GA、UI、Cue、消息、加载、阵营、库存和交互的迁移状态。

> 当前核对：2026-10-06；运行结论仅限已记录范围。

## 已接通，避免重复建设

- 角色等级/装备属性首版：成长 Profile、Instant 基础 GE、Infinite 装备加成、PlayerState 协调器、幂等与就绪/资源策略已接入。正式曲线为示例数据；完整经验、突破、后端存档和多角色切换仍是后续工作。

- KB-01～04：Experience 选择/加载、PawnData 注入、Hero 输入相机、PlayerState ASC→Pawn Avatar 已成立，正式角色可移动与攻击。
- KB-08：服务器检测、Melee GA、伤害规则、GE、Health 元属性结算已接通并由夹具验证；伤害交互倍率不再恒为零。
- KB-15/16/22：Timeline、Definition/Combo 与统一 Pawn CombatComponent 已实现。旧 ComboComponent 已删除；移动取消、连段记忆及末段后摇重开已有验证。
- KB-23：HodgeAbilityEditor 六面板编辑器与动画辅助库存在；编辑器预览不模拟完整 GAS/命中/网络。
- KB-24：EquipmentManager 已由 Experience AddComponents 注入，默认剑及 ASC 初始化链已挂载，来源句柄可撤销。
- Main 动画已迁移：无 Start 入口、保留 Stop、禁用 Pivot、固定层/FullBody/IK/转身修正。旋转锁和武器手持窗口已接入。
- 武器显隐/回背/悬浮/消隐已实现，BackSocket 默认 WeaponOnBack，根及检测 Mesh 留在手部。

## 剩余待办

### 正式攻击伤害

五个 DA_Attack 的 HitWindows 均为空；配置实际检测来源、命中 Profile、GE 和窗口，升级/确认 AbilityClass 具备 Melee 接入后，验证真实刀身采样、去重、友伤、旋转锁及手持窗口覆盖。不以夹具 100→70 代替正式五段扣血。

### KB-05：PawnData 授予撤销

PlayerState.GiveToAbilitySystem 仍未收集来源句柄；换配置、重生和重复授予的撤销策略需要补。装备管理器自身的 GrantedHandles 已实现，不再列为缺口。

### KB-06/12/14：生命周期与热卸载

专项覆盖换 Pawn、死亡/重生、Controller/Avatar 解除顺序、输入与镜头清理、GameFeature 停用/再激活、多世界隔离、网络相关性重新进入。不能仅以函数对称判断通过。

### KB-07：GameplayCue

路径观察者创建、移除及启动预加载仍不完整；显式 GameplayCueNotifyPaths 尚未配置，存在扫描全 Game 的警告。

### KB-09/10：敌人与专服

EnemyCharacter 尚未完成独立 ASC 初始化和完整敌人战斗。无 Server 专用 Target，Dedicated Server 登录分支仍有提前返回风险；独立进程、Cook/打包未验收。

### KB-19：UI

CommonUI 已启用，UIExtension/Indicator 等有有效代码；依赖 CommonGame、GameSettings、CommonUser 的停用迁移内容仍需接通。GameViewportClientClassName 未配置为 CommonGameViewportClient 派生类，完整 HUD/前端流程未完成。

### KB-20/21：输入与相机

下蹲绑定临时屏蔽；相机 GetBlendInfo 返回基础层而非当前主导层，当前无消费方，接入前明确契约。不要顺手把设计取舍改成代码修复。

### 后续动作系统

冲刺 GA、朝向运动切换与冲刺 Pivot 尚未制作；组合触发、Section 跳转/循环不在当前 Definition 第一版范围。延迟下窗口边界预测与服务器补偿需要专项设计，不能由既有后摇窗口测试推断覆盖。

## 操作入口

当前 PawnData 为 `/Game/Main/Data/PawnData/DA_Dafult_PawnData`，不要误改 AssetManager 的同名回退。普攻配置 `Main/Character/Hero/Ability/BasicAttack`，连段 `Main/Data/Combo`，窗口 `Main/Character/Hero/Ability/BasicAttack/Timeline`，武器 Profile `Main/Weapon/Presentation/DA_SwordPresentation`。

详见 [本轮更新](26-update-2026-10-06.md)、[验收范围](16-validation.md)。
