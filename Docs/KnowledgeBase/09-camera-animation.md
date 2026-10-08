> 2026-10-07 更新：本文中旧 Timeline／HitWindows 的字段与制作步骤仅保留为历史；当前攻击入口以 [通知配置手册](../Guides/attack-ability-configuration.md) 和 [通知迁移更新](27-update-2026-10-07-anim-notify.md) 为准。

# 相机、移动、动画与旋转约束

[返回知识库](README.md) · [本轮更新](26-update-2026-10-06.md)

> 当前核对：2026-10-06；运行结论仅限已记录范围。

## 相机与移动

HeroComponent 在初始化时绑定 CameraComponent 的模式委托；默认 PawnData 指向 CM_ThirdPerson，技能临时模式按 SpecHandle 归属清理。CameraModeStack 复用按类缓存的实例，Push 在下标 0；GetBlendInfo 仍取 Last 基础层，不应当作当前主导层使用。

CharacterBase 使用 HodgeCharacterMovementComponent，提供 GroundInfo、加速度与网络入口。正式 BP_Hero_Pover 使用控制器 Yaw，常态不朝向运动；默认步速 420。Hero 移动意图在阻断位移前采集，用于后摇取消。

## 正式动画资产

主图 `/Game/Main/Character/Hero/Anim/ABP_Pover_Base`，父类 HodgeAnimInstance；固定层 `Layer/ABP_Pover_LocomotionBase`，接口 `ALI_Pover_LocomotionInterface`。主图已从最初迁入的 ABP_Pover_Lyra 重命名，旧名称不能作为当前路径。

骨架与网格在 `Anim/Model`；移动序列、BlendSpaces、Types 与五段攻击在 Main/Anim。DefaultSlot 已移除，攻击使用 FullBody。正式动画不因换武器切换移动层；手持武器表现与动画层是两个职责。

Idle/Stop 起移动直接进入 Cycle，Start 无入口；保留 Stop，EnablePivot=False，后续冲刺 GA 尚未实现。连续转身与腿部 IK 已修正，FullBody 权重抑制程序性腿部修正，避免攻击下半身被固定。

## 攻击朝向

HodgeCombatCharacter 原生创建 RotationComponent，PawnExtension 生命周期注入/解绑 ASC。Timeline 的 Status.Rotation.Locked 窗口锁角色 Yaw，不锁镜头；退出后按 RecoveryTurnRate 默认 360 度/秒恢复。

FaceRotation、PhysicsRotation、RootMotion/移动最终旋转、SavedMove 重放和平滑校正共同消费策略。HodgeAnimInstance 在游戏线程缓存 bSuppressLocomotionYaw、bResetLocomotionYaw、LocomotionRootYawScale，蓝图动画线程只读，按 FullBody 权重退出程序性根 Yaw 修正。

## 配置与排查

主图/层引用、Stop、Pivot 和 FullBody 见 [正式动画说明](../../Content/Main/Character/Hero/Anim/README.md)。旋转窗口见 BasicAttack 的五段 Timeline；锁区间应覆盖要求固定方向的命中，开头可留调整朝向的时间。

武器回背位置由 Skeleton/Mesh 的 WeaponOnBack 插槽控制，DA_SwordPresentation.BackTransform 只提供局部偏移，不再是 Pawn 根坐标。

旧 CodexText ALS/Grounded C++ 与 Survivor 仍是实验；默认连段和武器配置仍在 CodexText，不能用实验身份概括整目录。完整重生、技能镜头覆盖与网络边界仍需专项验证。

参考：[旋转方案](../Design/character-rotation-policy.md)、[本轮状态](26-update-2026-10-06.md)。
