# 实际验证范围与验收步骤

[返回知识库](README.md) · [本轮更新](26-update-2026-10-06.md)

> 当前核对：2026-10-06；运行结论仅限已记录范围。

## 已保存证据，不冒充本次复跑

- 2026-10-01：统一战斗组件、Experience 注入和 Melee 检测/既有 GE 夹具，验证输入→服务器命中→100 扣到 70 与 Listen Server。见 [GE 验证](../Design/melee-experience-ge-validation-20261001.md)、[统一组件验证](../Design/combat-component-unification-validation-20261001.md)。
- 2026-10-05：Main 动画迁移、Stop/连续转身/FullBody 下半身姿势与双人副本；[动画说明](../../Content/Main/Character/Hero/Anim/README.md)。
- 旋转：原生 ConstraintsAndReplay 与真实输入、RootMotion 对照、Authority/Autonomous/Simulated。模拟代理约束与移动复制不同包，存在暂时差值；[实施记录](../Design/character-rotation-policy.md)。
- 连段：默认 1 秒记忆、移动/能力取消、超时、末段 04 后摇接 01；单人/双人 100ms 与预测拒绝恢复。紧贴窗口边界的网络用例存在未通过记录；[连段记录](../Design/combo-retention.md)。
- 武器：2 项原生测试、资产校验、显隐/连续手持/取消/倍速、双人 100ms 和预测拒绝；插槽版本最终 Editor/Game 构建退出 0、原生 2 项 Success、单人 453 条、双人 1420 条验证 WeaponOnBack。见 [武器记录](../Design/weapon-presentation.md)。
- 同类 cpp 合并后 Editor/Game 构建与 Combo 5 项、Weapon 2 项通过；Combo 有现有 Cue 路径 warning，见 [构建流程](../AI_DEVELOPMENT.md)。

本次 2026-10-06 文档任务只执行静态复核、只读资产 Commandlet 和 kb.py/链接检查，没有重新执行 C++ 构建、蓝图编译或 PIE。保存的历史成功范围不能自动扩大到当前所有场景。

## 当前待验证

正式五段 HitWindows=0；配置真实命中后补五段伤害、去重、友伤、旋转和手持窗口覆盖。完整敌人 ASC、死亡/重生、卸装/热卸载、相关性重新进入、Cue 与 UI 仍需专项回归。

未执行 Cook/打包、独立进程客户端、Dedicated Server。没有 Server Target，不能将 Listen Server 测试写成专服验证。

## 可复现验收顺序

1. 在 ThirdPersonMap 核对实际 Experience、PawnData、Hero、ASC Owner/Avatar，验证移动、视角和相机。
2. Idle→Cycle 无 Start；松开进入 Stop；连续双向镜头转动无停顿跳变；Pivot 保持禁用。
3. FullBody 攻击下半身随源动作；RotationLock 前允许调整，窗口内镜头可转而胶囊 Yaw 锁定，退出平滑恢复。
4. 前四段移动取消/中断后 1 秒内接下一段，超时回第一段；按 1→2→3→5→4，末段 NextAttack 窗口点击直接回 1。
5. WeaponHand 开始显现，最后请求释放后宽限→回背→悬浮→消隐；连续连段不回背，回收中攻击回手。悬浮/消隐可见 Mesh 挂 WeaponOnBack，逻辑来源仍 WeaponOnHand。
6. 双人 100ms 检查 Authority、拥有者、模拟代理；再分别增加边界、失败、死亡和重新进入场景，不一并宣称已通过。

每次记录命令、退出码、资产路径、输入时刻、网络模式、预测/权威差异、首个有效错误。Saved 中原始记录为本机证据，不纳入源码。
