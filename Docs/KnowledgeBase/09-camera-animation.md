> 2026-10-07 更新：本文中旧 Timeline／HitWindows 的字段与制作步骤仅保留为历史；当前攻击入口以 [通知配置手册](../Guides/attack-ability-configuration.md) 和 [通知迁移更新](27-update-2026-10-07-anim-notify.md) 为准。
> **2026-10-15 更新**：本文涉及相机与动画的部分已大幅扩充 —— 新增**目标锁定**（`UHodgeLockOnComponent` + `UHodgeCameraMode_LockOn`）、**软锁定镜头**（死区/停手宽限/分轴响应）、**角色朝向模式**（`UHodgeCharacterRotationComponent`，Free/ReservedStrafe）、**Dash/Sprint 动画同步与 Pivot 修复**、**攻击前摇 Motion Warping**、**电影镜头**（`UHodgeAnimNotifyState_CinematicCamera`）。`GetBlendInfo` 栈顶查询缺陷**已修复**。详见 [本轮更新](30-update-2026-10-15.md)。

# 相机、移动、动画与旋转约束

> 2026-10-17：近期动作衔接、连段／闪避派生、受击恢复、根运动交接与悟空参考见[ACT 总设计](../Design/act-combat-animation-feedback-design.md)和[实施／验收计划](../Design/act-combat-animation-feedback-plan.md)。本轮已制作方向中／重原型、持续与恢复段并接入分时恢复及曲线运动；实际构建、资产读回和测试边界见[实现验证](../Validation/act-combat-animation-feedback-2026-10-17.md)，不以本章历史快照代替当前事实。

> 后续已改用现有 Behit，地面 S/B、Fall、StandUp 由动画根运动执行，空中 Fly/Push Start／Loop 由 CMC 弹道执行，实际落地才进入 Fall。原程序原型退出 Hero／假人引用但保留备份；当前实现见[Behit 实施记录](../Validation/authored-behit-root-motion-2026-10-17.md)。

[返回知识库](README.md) · [本轮更新](30-update-2026-10-15.md) · [接通清单](12-integration-backlog.md)

> 当前核对：**2026-10-15**（HEAD `b702482`）；运行结论仅限 `Docs/Validation/` 里对应日期报告的范围。

## 相机与移动

HeroComponent 在初始化时绑定 CameraComponent 的模式委托；默认 PawnData 指向 CM_ThirdPerson，技能临时模式按 SpecHandle 归属清理。CameraModeStack 复用按类缓存的实例，Push 在下标 0。

> ✅ **`GetBlendInfo` 栈顶查询缺陷已于 2026-10-13 修复**（此前取 `Last()` 基础层、权重恒 1.0，无法反映当前主导模式）。旧章节里"不应当作当前主导层使用"的描述已过时。

CharacterBase 使用 HodgeCharacterMovementComponent，提供 GroundInfo、加速度与网络入口。正式 BP_Hero_Pover 使用控制器 Yaw，常态不朝向运动；默认步速 420。Hero 移动意图在阻断位移前采集，用于后摇取消。

## 正式动画资产

主图 `/Game/Main/Character/Hero/Anim/ABP_Pover_Base`，父类 HodgeAnimInstance；固定层 `Layer/ABP_Pover_LocomotionBase`，接口 `ALI_Pover_LocomotionInterface`。主图已从最初迁入的 ABP_Pover_Lyra 重命名，旧名称不能作为当前路径。

骨架与网格在 `Anim/Model`；移动序列、BlendSpaces、Types 与五段攻击在 Main/Anim。DefaultSlot 已移除，攻击使用 FullBody。正式动画不因换武器切换移动层；手持武器表现与动画层是两个职责。

Idle/Stop 起移动直接进入 Cycle，Start 无入口；保留 Stop，EnablePivot=False。连续转身与腿部 IK 已修正，FullBody 权重抑制程序性腿部修正，避免攻击下半身被固定。**冲刺 GA 已于 2026-10-10 实现并验证**（见下文 Dash / Sprint 一节）。

## 攻击朝向

HodgeCombatCharacter 原生创建 RotationComponent，PawnExtension 生命周期注入/解绑 ASC。**动画通知区间**授予的 `Status.Rotation.Locked` 锁角色 Yaw，不锁镜头；退出后按 `RecoveryTurnRate` 默认 360 度/秒恢复。（该窗口此前由 Timeline 授予 —— Timeline 已退役。）

FaceRotation、PhysicsRotation、RootMotion/移动最终旋转、SavedMove 重放和平滑校正共同消费策略。HodgeAnimInstance 在游戏线程缓存 bSuppressLocomotionYaw、bResetLocomotionYaw、LocomotionRootYawScale，蓝图动画线程只读，按 FullBody 权重退出程序性根 Yaw 修正。

## 配置与排查

## 目标锁定与软锁定镜头（2026-10-12 ~ 10-13）

- **硬锁定（持久目标选择）**：`UHodgeLockOnComponent`（Hero 上的原生 PawnComponent，实现 `IGameFrameworkInitStateInterface`）+ 短命令 GA `UHodgeGameplayAbility_LockOn` + Profile `DA_HeroLockOn` + `UHodgeLockOnStateEffect` + 接口 `HodgeLockOnTargetInterface` + 标记 `UIndicatorDescriptor`。中键获取/解除、鼠标左右甩动切换、服务器资格与版本确认、拒绝恢复、旧请求隔离与有限超时；Dash/Sprint 期间暂让再恢复。
- **锁定镜头**：`UHodgeCameraMode_LockOn : UHodgeCameraMode_ThirdPerson`（复用避障）；链路 = GA `SetCameraMode` → Hero → `DetermineCameraModeDelegate` → `HodgeCameraComponent` → CameraModeStack → PAM POV。
- **软锁定（镜头辅助，不是自动索敌）**：`FHodgeSoftLockCamera` —— 死区 20°/12°、停手宽限 0.75 秒、指数锐度、分轴响应、鼠标抑制与渐进恢复。
- **验证**：10-12 原生 29/29、四种 PIE 模式各 20/20；10-13 软镜头原生 31/31、镜头各 24、回归各 20；10-13 tracking 原生 36/36、镜头各 22、回归各 20。
- ⚠️ **攻击自动索敌未启用**（[melee-attack-assist](../Design/melee-attack-assist.md) 仍是草案，接口未创建）。

## Dash / Sprint 与动画同步（2026-10-10）

- 技能：`UHodgeGameplayAbility_Dash` / `UHodgeGameplayAbility_Sprint`（基类 `UHodgeGameplayAbility_MovementAction`）+ 配置 `UHodgeSprintAbilityProfile` + 策略组件 `UHodgeLocomotionPolicyComponent`（步态 / 高速 / 减速 / 输入会话 / SavedMove）
- 输入：`UHodgeHeroComponent` 中 Shift Started 建会话、Completed/Canceled 释放，`RequestSprintHandoff` 单次衔接
- 网络：LocalPredicted；TargetData 只带原始输入、采样 ControlYaw、起手 ActorYaw、SessionId；根运动位移由 `UHodgeCharacterMovementComponent` 应用
- 中断：改走标签映射（`UHodgeAbilityTagRelationshipMapping`），`bForceCancel` + Death 豁免；**不再依赖旧攻击后摇窗口**
- 离地续冲：`MovementAction::IsMovementValid` 新增执行模式虚接口，Dash 允许 Falling 并用 `ClampVelocity` 保留负 Z
- 动画：`UHodgeAnimInstance` 新增 `bSprinting` / `bSprintPivotAllowed` / `SprintCycle` / `SprintTurn` / `SprintCyclePlayRate` / `SprintTurnPlayRate` / `SprintTurnBlendOutTime` / `LocomotionFootIKAlpha`，函数 `UpdateSprintPivot` / `UpdateSprintCycle` / `ShouldExitSprintPivot` / `MatchCycleRate` / `ResolveFootIKAlpha` / `ResolveCycleSyncGroup`
- 验证：implementation 原生 30 / PIE 31；timing 原生 31 / PIE 40；transitions 原生 35 / PIE 60（骨盆差 −0.10/−0.08/−0.12 cm，最低速度 720）；interrupt 原生 34 / PIE 69；air 原生 45/45 / PIE 227（含 single/listen/dedicated/listen-lag）；pivot 原生 32 / PIE 37（脚踝 XY 速度降 86.70%/92.14%）
- ⚠️ **体力系统按用户要求暂时移除**，不要按旧文档去找它

## 角色朝向模式（2026-10-10 实现并验证）

项目没有 `HodgeRotationMode` 这类单一枚举，而是拆成三层：

| 类型 | 取值 |
|---|---|
| `EHodgeCharacterFacingDriver` | `Movement` / `Controller` / `ActionDirection` |
| `EHodgeLocomotionStyle` | `FreeDirectional` / `ReservedStrafe` |
| `EHodgeFacingExecutionMode` | `PhysicalTurn` / `MontageWarp` |

- 组件：`UHodgeCharacterRotationComponent`（`UPawnComponent`）做**仲裁 + 复制 + SavedMove 回放**
- `bUseControllerRotationYaw` / `bOrientRotationToMovement` **只是执行结果**（`Character.FaceRotation = ControllerFacing`、`CMC.PhysicsRotation = MovementFacing`），**不是配置入口**
- 与 Lyra / ALS 的关系：项目明确不重构第三方 ALS，而是把 Lyra 的 RotationMode 概念拆成"基础朝向模式 + 动画风格 + 动作覆盖"
- 动画侧：`UHodgeAnimInstance` 新增 `bUseStrafeLocomotion` / `StrafeLocomotionWeight` / `bAllowLocomotionPivot` / `FacingPresentation` / `FacingModeBlendTime` / `bSuppressLocomotionYaw` / `bResetLocomotionYaw` / `LocomotionRootYawScale`
- 正式主角默认 **Free**，原方向图保留为 ReservedStrafe，Pivot 禁用（见 `Content/Main/Character/Hero/Anim/README.md`）
- 验证：原生 24（15+9）、PIE 24（各 8）+ 6 连招，覆盖 282/307 = **91.86%**
- 配置：[`character-facing-configuration.md`](../Guides/character-facing-configuration.md)

## 电影镜头（2026-10-15）

`UHodgeAnimNotifyState_CinematicCamera`（通知窗口）+ `HodgeCinematicCameraProfile` / `HodgeCinematicCameraShots` + `UHodgeHeroComponent::RefreshCinematicCamera()` + `UHodgeCameraMode_Cinematic`。当前用于共鸣解放前半段；取舍依据见 [`resonance-liberation-cinematic-review-2026-10-14.md`](../Design/resonance-liberation-cinematic-review-2026-10-14.md)。⚠️ 该会话逻辑目前挂在 HeroComponent 上，[架构审查](../Design/architecture-refactor-review-2026-10-15.md) R2 建议解耦。

## 配置与排查

主图/层引用、Stop、Pivot 和 FullBody 见 [正式动画说明](../../Content/Main/Character/Hero/Anim/README.md)。旋转窗口见五段普攻的**动画通知**（`Status.Rotation.Locked` 区间）；锁区间应覆盖要求固定方向的命中，开头可留调整朝向的时间。

武器回背位置由 Skeleton/Mesh 的 WeaponOnBack 插槽控制，DA_SwordPresentation.BackTransform 只提供局部偏移，不再是 Pawn 根坐标。

旧 CodexText ALS/Grounded C++ 与 Survivor 仍是实验；默认连段和武器配置仍在 CodexText，不能用实验身份概括整目录。完整重生、技能镜头覆盖与网络边界仍需专项验证。

参考：[旋转方案](../Design/character-rotation-policy.md)、[朝向模式](../Design/character-facing-modes.md)、[锁定系统](../Design/lock-on-system.md)、[软锁定调研](../Design/soft-lock-camera-research-2026-10-13.md)、[Dash/Sprint 设计](../Design/dash-sprint-system.md)、[攻击 Warp](../Design/attack-facing-motion-warp.md)、[本轮更新](30-update-2026-10-15.md)。
