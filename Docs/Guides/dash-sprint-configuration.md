# Dash／Sprint 配置与接口（v1.7）

当前玩家的攻击中断改用 Tag Relationship Mapping，详见[中断配置指南](dash-interrupt-configuration.md)。以下 Dash 动画时序、移动后摇取消与 Sprint 衔接参数继续有效。

适用 UE 5.5.4。当前无冷却、无体力系统。设计见[Dash／Sprint 设计](../Design/dash-sprint-system.md)。

## Main 接入

- 配置：`/Game/Main/Character/Hero/Ability/Sprint/DA_Hero_SprintAbility`。
- GA：`/Game/Main/Character/Hero/Ability/GA_Hero_Dash`、`GA_Hero_Sprint`，两份 Profile 指向上述配置。
- 授予：`/Game/Main/Data/AbilitySet/DA_Pover`，各一份，InputTag 留空。Hero 统一路由 Started，防止重复启动。
- PawnData：`/Game/Main/Data/PawnData/DA_Dafult_PawnData.SprintAbilityProfile`。
- 输入：Main InputConfig 的 IA_Sprint→InputTag.Sprint；IMC_Default 的 LeftShift／Gamepad_FaceButton_Right 映射。Action 不添加 Hold／Tap Trigger 分别启动两个 GA。
- 动画：`/Game/Main/Character/Hero/Anim/Movement/AM_Hero_Dash_F`、`AM_Hero_Dash_B`、`A_Hero_Sprint_F`、`A_Hero_Sprint_Turn`。
- 图：`/Game/Main/Character/Hero/Anim/ABP_Pover_Base` 与 `Layer/ABP_Pover_LocomotionBase`。保留八向、FullBody、轻反馈 Group 和 IK。
- Cue：`/Game/Main/GameplayCues/Movement/GCN_Hero_Dash`、`GCN_Hero_Sprint`、`GCN_Hero_PerfectDodge`。

## Dash 与取消后摇

Started 尝试一次 Dash。Shift 松开只清长按资格、停止 Sprint，不能停止 Dash 蒙太奇。短按不输入移动取消时，两份 1.333333 秒蒙太奇按正常速率播完。

Duration=0.35 秒只决定 RMS 位移；Distance=280 cm。MontagePlayRate=1 决定动画倍率，仍需乘蒙太奇 RateScale。GA 在 OnCompleted 正常结束；动画被合法动作替换、受击、离地等可中断。

bAllowMoveCancel=True、MoveCancelOpenTime=0.35 秒。释放 Shift 后持续移动／重新输入移动，幅值达到 MoveIntentThreshold=0.1 且进入窗口，就取消恢复段并混出到普通移动。没有方向输入则完整播完。所有 Dash 时间窗从成功播放开始计时，单位为实际游戏秒，不是归一化比例，不包含朝向准备。

默认 MoveCancelOpenTime 和 HandoffOpenTime 与 Duration 对齐，避免位移结束先停止再起步。两者仍可手动调大；那会有意保留位移之后的恢复等待。主动衔接通过本次 RMS 的 ClampVelocity 继承已有速度，不移动角色或直接注入启动速度。

正式动画层的腿部 IK 和 FootPlacement Alpha 已读取主动画实例的 LocomotionFootIKAlpha。Dash 入／出混合保持连续脚部贴地，其他全身动作继续抑制 IK；UseFootPlacement 与 DisableLegIK 原条件保留。正常 Dash 混出开始时开放移动和朝向，GA 在动画完成后结束。不要再给 Mesh 添加固定 Z 补偿。

B 默认启用，BackwardConeAngle=45° 是角色后向锥半角。锥内用 B 沿正后方退、面向不变；其余方向用 F，关闭 B 后全部使用 F；无输入 F 朝前。

bInstantFacingAtStart=False、FacingPreparationTimeout=0.5 秒。当前旋转攻击不使用旋转锁，也不进行锁交接。Dash 的攻击中断由 PawnData.TagRelationshipMapping 决定，AttackCancelWindow 保留兼容但不再由 Dash 消费；角色移动取消攻击仍使用原有执行窗口。

## Dash→Sprint 调参

在 DA_Hero_SprintAbility 分别调整：

- HandoffOpenTime=0.35：最早允许衔接的 Dash 阶段，默认与位移结束对齐。
- HandoffCloseTime=1.2：最后允许发起衔接的 Dash 阶段。
- HoldThreshold=0.22：从 Shift Started 起计算的最低按住时长，可晚于窗口开启。
- MoveIntentThreshold=0.1：必须有有效方向输入。
- HandoffNetworkGrace=0.5：服务端有限等待／授权余量，不改变拥有者的衔接阶段。

触发要求窗口、长按、方向、成功 Dash 同时成立。窗口开启时尚未长按达标会继续等；成立后尝试一次。释放、状态拒绝、会话结束后不能自动补进 Sprint。持续按 Shift 时衔接优先，窗口有效期内移动取消不会抢先结束 Dash。

例：希望最早 0.8 秒衔接，将 HandoffOpenTime 改为 0.8，CloseTime 保持 1.2；希望至少长按 0.85 秒，将 HoldThreshold 改为 0.85。两项可以独立调节。更换动画或改变倍率后必须让所有启用 F／B 动画容纳位移、取消和衔接窗口；资产验证会拒绝越界配置。

## Sprint 与 Pivot

SprintMaxSpeed=720、SprintAcceleration=2400、SprintBraking=2000、SprintTurnRate=540。NoMoveIntentGrace=0.08 秒，BlockedExitDelay=0.4 秒，bAllowSprintPivot=True。

SprintCycle 为原地高速循环。SprintCyclePlayRate=1 是基准倍率，实际倍率还乘当前平面速度／SprintCycleReferenceSpeed；Main 的参考速度约 599.07 cm/s，Min／MaxPlayRate=0.5／1.5。720 cm/s 时实际约 1.20 倍，避免结束掉头后回循环重新滑步。

Pivot 当前通过 SprintPivotMontage 指向 `/Game/Main/Character/Hero/Anim/Movement/AM_Hero_Sprint_Pivot`，使用新复制的 `A_Hero_Sprint_Pivot_RootMotion`，RootMotionFromMontagesOnly。原有 A_Hero_Sprint_Turn 保留。配置根运动蒙太奇后，旧状态机原地 Pivot 关闭准入，避免两套姿势同时播放。

PivotTriggerAngle=150°、PivotMinimumSpeed=250 cm/s、PivotReentryDelay=0.25 秒；SprintTurnPlayRate=1。间隔从 Pivot 起手计时，恢复阶段仍禁止再次 Pivot，不在尾段结束后追加等待。PivotRecoveryEndTime=0.9 为素材时间：保留旋转、制动和反向起步，到点后以 SprintTurnBlendOutTime=0.16 秒混出至循环。素材的 1.6 秒长度保留，末尾跑步不必播完。蒙太奇 BlendIn=0.12、BlendOut=0.16；松开、攻击、伤害和控制仍停止自己的 Pivot 蒙太奇。

Sprint 循环新增独立播放分支，使用 HodgeSprint 同步组；普通分支仍为 Locomotion／AlwaysFollower。Dash F／B 和根运动 Pivot 蒙太奇也属于 HodgeSprint，作为组领导，循环按 Hodge.LeftFoot／Hodge.RightFoot 标记对齐脚步，动作时间与根运动不被循环拉伸。标记位于四份 Main 序列的 HodgeSprintSync 通知轨道；Slot 仍是 FullBody，Slot Group 与动画 Sync Group 含义不同。Dash BlendOut=0.18 秒，循环分支进入／退出混合为 0.12／0.16 秒。

换 Pivot 素材后先找完整转向及反向起步的恢复点，再配置 PivotRecoveryEndTime（必须大于 0 且小于素材长度），同步校准脚步标记。它不包含播放倍率：GA 直接读取蒙太奇实例位置；修改 SprintTurnPlayRate 会相应改变实际秒数。根运动 Pivot 与 Dash 都保持连续脚部贴地，攻击蒙太奇仍按原权重退出 IK。

服务器复检关联 Sprint 的 SessionId／SequenceId、方向、速度、状态和 StartingYaw。拥有者不瞬间回正 Actor，观察客户端消费 GAS／Character 根运动复制。新路径只支持地面近反向的单一掉头，不把一份 180° 动作当作所有角度的完备素材。

松开、停止移动、阻挡、实际生命伤害、控制、死亡、MovementStopped、攻击或离地结束 Sprint。释放本次朝向和速度句柄后恢复当前合法策略，保留独立减速来源。

## 防御配置

InvulnerabilityStart／End=0.04／0.22 秒；PerfectStart／End=0.04／0.10 秒，窗口均为 `[Start, End)`，相对成功提交。每次 Dash 最多一个完美奖励。PerfectRewardEffect 可选，当前不提供体力奖励。Cue 只负责 Niagara 表现。

## 体力与 HUD

没有体力条资产或正式显示绑定。Main HUD 的左下角为生命面板：`WBP_HodgeHUD.PlayerVitalsPoint` 是 Overlay 下左下对齐，Padding=32；`WBP_PlayerVitals.VitalsSize` 宽 280，Border Padding=16，纵排 HealthText／HealthBar。这些是 UMG 逻辑单位，受 DPI 缩放影响。不要把生命条当作体力条。

已移除 StaminaSet、Stamina／MaxStamina、StaminaDelta GE／Tag、DashCost、SprintStartRequiredStamina、SprintDrainPerSecond、NaturalRegenPerSecond、GetStamina、恢复阻塞和 SavedMove 资源预算。编辑器不再有这些字段；现阶段 Dash／Sprint 仅受动作和状态限制。

## 接口和网络

Hero 管理 BeginSprintInput／EndSprintInput／GetSprintInputSession／RequestSprintHandoff。MovementAction 复用 GAS TargetData、预测、约束监听和清理；Dash／Sprint 分别管理自己的动作与步态。

Policy 提供 GetResolvedPolicy／GetProfile、AcquireSprint／ReleasePolicy、会话与衔接授权、独立速度修正句柄。OnSprintAuthorityEnded 处理服务器状态结束。CMC 保存运动策略和合法激活键，不再保存／结算资源；服务器不接收客户端任意速度。

AnimInstance 缓存 Sprint 状态、资源及 Turn 倍率；UpdateSprintPivot 只负责表现时间。RotationComponent 仍是 Actor 朝向唯一仲裁入口。所有退出释放自己拥有的句柄，不清全体蒙太奇，不写固定 Idle。

## 验证工具

`Tools/DashSprint/configure_hero.py` 用于首次创建 Main 接入，不用于覆盖已调整图。`configure_pivot_root_motion.py` 创建指定根运动副本／蒙太奇、绑定 Profile 并估算循环参考速度；正式接入已完成。`refine_timing.py` 只更新本修订指定配置／Pivot 接线并编译指定动画蓝图。

原生组 Hodge.Movement；run_e2e.py 验证短按、移动取消、配置阈值、配置衔接时间和实际 Pivot 求值时间；run_combat_integration.py／run_hit_exit.py 复核攻击与受击退出。结果存于 Saved/DashTimingRefinement。检查 error 为空及场景完整才算通过；summarize_timing_results.py 同时核对三个视角的 Pivot 播放／退出与覆盖率。停止 PIE 后运行 cleanup_validation.py，只重载临时测试 Profile／探针，不保存测试覆盖值。

本次未实现空中 Dash、完整锁定、相机吸附、位移曲线、重新启用资源系统或完善多角度 Turn 素材。
