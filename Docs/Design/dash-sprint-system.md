# Dash 与 Sprint：动作时序、移动取消与奔跑策略

日期：2026-10-10。版本：1.7。适用 Hodgepodge、UE 5.5.4。

本修订按最新要求暂时移除体力系统及所有 Dash／Sprint 资源逻辑。v1.2 的体力属性、费用、恢复、持续预算和 UI 建议不再是当前开发契约。历史结果保留在[此前实施报告](../Validation/dash-sprint-implementation-2026-10-10.md)，不能作为本修订测试结果。

v1.4 滑步修复见[根运动与接地验证](../Validation/pivot-foot-slide-2026-10-10.md)。此前交付结果见[时序与 Pivot 验证](../Validation/dash-sprint-timing-2026-10-10.md)。实际资产、字段和调参步骤见[配置指南](../Guides/dash-sprint-configuration.md)。旋转基础沿用[角色旋转与移动动画模式调整](character-facing-modes.md)。完整锁定目标服务、锁定 CameraMode、攻击吸附不纳入本次实现。

## 1. 目标与确定规则

v1.5 将 Dash 的攻击中断交给既有 Tag Relationship Mapping。当前玩家映射取消所有攻击分类，Death 优先豁免；旧 AttackCancelWindow 不再约束 Dash。资产与扩展步骤见[中断配置指南](../Guides/dash-interrupt-configuration.md)。

v1.6 修正 Dash 到 Idle／移动／Sprint 的过渡。Dash 保留连续脚部贴地，不再随 FullBody 权重关闭 IK；默认后摇取消和 Sprint 开放与 0.35 秒位移结束对齐，主动衔接保留已有速度并限制到目标步态上限。

v1.7 为 Dash／Sprint／根运动 Pivot 补充脚步标记同步。Sprint 使用独立循环分支，保留普通 Locomotion；Pivot 在可配置的位移恢复点混出，跑步尾段不再阻止第二次掉头。

Dash 是一次有动画、定向位移和防御窗口的闪避动作；Sprint 是成功 Dash 后持续长按进入的高速奔跑状态。两者是现有 Hodgepodge Runtime 中的功能单元，不另建重复负责生命周期的 DashComponent，也不新增业务模块。

1. 两者没有冷却。当前也没有体力门槛、费用、恢复、耗尽退出或相关资源显示。
2. Shift Started 创建一次输入会话并尝试 Dash；Completed／Canceled 释放会话。短按释放不能结束正在播放的 Dash。
3. 一次按住只尝试一次 Dash 和一次合格 Sprint 衔接。动作失败、Sprint 中断后需重新松开并按下。
4. B 默认启用：角色起手后向锥内使用 B，沿角色正后方位移并保持起手面向；其余方向用 F。B 可关闭，无方向时用 F 朝前。
5. 当前旋转攻击未使用旋转锁。本实现不交接攻击旋转锁；已有受击和死亡限制仍有效。
6. RMS 位移时间和完整动画时间独立。位移结束不能作为蒙太奇或 GA 的正常结束条件。
7. 移动取消后摇和 Dash→Sprint 是显式、独立、可配置的退出路径。受击、死亡、离地、动画被其他合法动作替换仍可中断。
8. Sprint 正常使用朝运动方向旋转和 FreeDirectional。八向资源保留给 ReservedStrafe；不删除方向输入。
9. 退出释放本次句柄，再解析剩余策略。不能固定写回 ControllerYaw=True，也不能清除其他来源的减速和朝向请求。
10. 服务器批准动作、控制和防御；拥有者预测；观察者消费复制。GameplayCue 只处理表现。

## 2. 根因与修订

F／B 蒙太奇实际长度均为 1.333333 秒，RateScale=1。旧实现按 `MontageLength / Duration` 播放，将整段动作压缩到 0.35 秒，再在 Age>=Duration 结束 GA。这使位移、动画和后摇共用一个错误的结束条件。

旧衔接仅在 HandoffOpenTime 到达时尝试一次；当 HoldThreshold 晚于窗口开启，按住资格尚未成立就被消费。还有 0.3 秒的 Sprint 等待、0.5 秒的授权有效期隐藏在代码中，调参无法表达实际阶段。

Sprint Turn 被接入原八向 Pivot 的 DistanceMatchToTarget／AdvanceTimeByDistanceMatching，依赖原动画距离曲线和刹停映射；仅换资源不能使任意 Turn 正常推进。修订为 Sprint 独立的时间求值路径，保留普通分支。

## 3. 核心职责

- UHodgeHeroComponent：Started／Completed／Canceled、原始移动意图、按键会话、单次衔接路由。按下时同步读 Enhanced Input 当前移动值，避免同帧回调顺序改变 Dash 方向。
- UHodgeGameplayAbility_MovementAction：LocalPredicted GAS 生命周期、方向 TargetData、约束监听、公共 Cue 和退出清理。没有资源或冷却提交。
- UHodgeGameplayAbility_Dash：方向变体、动作朝向准备、蒙太奇、RMS、防御窗口、后摇取消和衔接授权；攻击取消由 ASC 的标签映射入口处理。
- UHodgeAbilityTagRelationshipMapping／UHodgeAbilitySystemComponent：按来源和目标标签匹配取消／阻塞规则，先判 Death 与配置豁免，再判断强制取消授权；Dash 保持 Independent，避免激活组取消无关技能。
- UHodgeGameplayAbility_Sprint：检查成功 Dash 授权和持续输入，持有 Movement 基础朝向与速度策略，处理松开、停步、伤害和攻击退出。
- UHodgeLocomotionPolicyComponent：步态、高速参数、独立减速来源、输入会话时间、服务器移动授权和 SavedMove 回放。没有资源预算。
- UHodgeCharacterRotationComponent：既有朝向请求仲裁、准备、预测及复制。动画和 Cue 不写 Actor Yaw。
- UHodgeCharacterMovementComponent：RMS 位移、Dash 期间抑制普通输入加速度、Sprint 最大速度／加速度／制动／转向与网络移动协议。
- UHodgeAnimInstance 与正式 Main 动画蓝图：缓存策略、选择 SprintCycle／SprintTurn、Pivot 准入、时间求值及原八向分支隔离。
- UHodgeDefenseComponent：服务器防御窗口与最多一次完美奖励，先于生命伤害和 Impact 提交。
- AHodgeGameplayCue_Movement：Dash、Sprint、PerfectDodge 的 Niagara 表现与清理。

ASC 继续由 PlayerState 持有；Pawn 服务沿 PawnExtension 初始化／解绑。重复初始化、换 Pawn、AvatarGeneration 改变后旧会话和请求不能继续生效。

## 4. Dash 数据与时间规范

共享配置为 UHodgeSprintAbilityProfile。时间单位是实际游戏秒数，受游戏时间膨胀影响。防御、衔接、后摇窗口从 Dash 成功提交并启动蒙太奇计时，不包含朝向准备。

- Duration：RMS 位移持续时间，默认 0.35 秒。
- Distance：恒定位移距离，默认 280 cm；速度为 Distance／Duration。
- MontagePlayRate：完整蒙太奇播放倍率，默认 1；有效倍率还乘资产 RateScale。
- ForwardMontage／BackwardMontage：同骨架、原地、单 FullBody Slot，B 启用时两份必须存在。
- bEnableBackwardVariant=True；BackwardConeAngle=45°，是后向锥半角且包含边界。
- bInstantFacingAtStart=False；FacingPreparationTimeout=0.5 秒。
- AttackCancelWindow：保留兼容旧资产；Dash 不再读取此字段。攻击中断由当前 PawnData 的 TagRelationshipMapping 决定，不要求进入移动取消窗口。
- bAllowMoveCancel=True；MoveCancelOpenTime=0.35 秒。
- HandoffOpenTime=0.35 秒；HandoffCloseTime=1.2 秒。
- HoldThreshold=0.22 秒，从物理 Started 计时；可以晚于衔接开启时间。
- HandoffNetworkGrace=0.5 秒：异步预测请求到达后的有限等待／授权余量，不改变拥有者的窗口触发点。
- MoveIntentThreshold=0.1：有效移动输入幅值阈值，Dash 移动取消和 Sprint 准入共用。

配置校验要求有限数值、合理窗口、RMS 和防御不越过位移阶段；取消／衔接必须在每个启用蒙太奇按配置倍率播放完之前。HandoffOpenTime>=Duration；MoveCancelOpenTime>=Duration；HandoffOpenTime<HandoffCloseTime。改变动画或播放倍率必须同步调窗口，不自动偷偷缩短动画。

## 5. Dash 流程与移动取消

Idle → DashRequested → FacingPreparing → DashActive → AnimationRecovery → Completed。

Started 采集世界方向、控制 Yaw、起手 Actor Yaw、SessionId、AvatarGeneration。默认平滑准备朝向；准备期间没有无敌，也没有 RMS 位移。服务器复检合法数据、朝向已应用、当前攻击允许取消与 ActivationGroup。成功时切为 Exclusive_Replaceable，启动动画、RMS、防御和 Cue。

RMS 结束后将自己的结束速度归零，动作继续播放恢复段。Dash 活跃时普通输入加速度被抑制，但 Hero 的移动意图仍被记录。没有退出请求时由本次蒙太奇 OnCompleted 正常结束；有限 watchdog 仅处理丢失回调的异常，不代替正常完成。

释放 Shift 只取消 Sprint 资格。释放后，移动意图达到阈值且进入 MoveCancelOpenTime，结束自己的 Dash、RMS 和朝向请求，以蒙太奇 BlendOut 接回当前移动姿势。原本一直按住的方向输入也允许取消后摇；不要求重新敲方向键。短按并在后摇前停止方向输入时完整播完动画。

持续按 Shift 时优先等待配置的 Sprint 衔接。衔接窗口尚有效时不让移动取消抢先结束 Dash；关闭窗口后仍可通过移动取消退出。Sprint 衔接或合法移动取消是主动截断恢复段，不属于短按提前结束缺陷。

EndAbility 用重入保护处理蒙太奇停止回调，清理自身防御、RMS、确认监听、Cue 和请求。不得停止所有蒙太奇、写死 Idle，或释放他人的句柄。

正常蒙太奇混出开始时先释放本次 Dashing 移动限制与动作朝向，能力仍等 OnCompleted；无输入短按不提前截断动画。主动移动取消或 Sprint 交接只调整本次 RMS 的结束策略为 ClampVelocity，避免再把已有速度写成零；自然无输入结束和取消路径保持原有停止规则。RMS 名称由复制的 SpecHandle 与 SessionId 组成，使拥有者和服务器匹配同一来源。

成功交接的来源结束策略由 LocomotionPolicy 保留一秒，供 CMC 恢复最近保存帧的 RMS 时再次施加；记录到期或 Pawn 解绑即清空。只有已记录的同名来源生效，死亡／强控制时不应用，不改其他技能来源，不直接注入速度。此重放缓存不改变手动配置的技能窗口。

Packed 服务器纠正包含已退出的同名 Dash 来源时，CMC 沿用包内完整权威速度进入原位置纠正流程，避免旧 RMS 桥接只保留垂直分量后清零平面速度。时间戳拒绝、服务器位置和重放仍由引擎处理；无关 RMS、动画根运动、相对基座速度和控制状态不应用此补充，不增加猜测速度或网络协议字段。

Main 原脚部控制将腿部 IK／FootPlacement Alpha 设置为 `1-FullBodyWeight`，导致 Dash 的脚部贴地在结束时才重新启用。改为读取 NativeUpdateAnimation 缓存的 LocomotionFootIKAlpha：Dash 权重不削弱贴地，其他全身动作权重照常抑制；混出实例仍参与计算。保留 UseFootPlacement 和 DisableLegIK 条件，不移动胶囊或模型组件，不修改素材 Root／骨盆轨迹。

## 6. Sprint 衔接与退出

Dash 每次更新检查窗口 `[HandoffOpenTime, HandoffCloseTime]`。只有本次成功 Dash、相同 SessionId、Shift 仍按住、实际按住时间>=HoldThreshold、移动意图>=MoveIntentThreshold，才由拥有者发出一次 Sprint 激活。

窗口开启但长按未满足时继续等，不消费会话。没有移动输入时也允许等到窗口内出现移动。没有长按或窗口已经关闭不能补进入 Sprint。服务器收到请求后再次检查自己的 Dash 授权、按住时间、状态和方向数据；网络宽限只允许服务器等待到达顺序和本地长按计时，不绕过按键释放或状态限制。

远端服务器可在 `[HandoffOpenTime, HandoffCloseTime+HandoffNetworkGrace]` 建立有限授权，避免网络或长帧越过普通关闭点时丢失在途请求。拥有者仍只在 `[HandoffOpenTime,HandoffCloseTime]` 发起，不能提前或在本地窗口关闭后补请求；仍要求活动 Dash 和同一按住会话。

成功 Sprint 释放自己的 Dash 动作朝向、混出 Dash 蒙太奇，持有 Movement 基础请求和 Sprint 速度策略。参数为 MaxSpeed=720、Acceleration=2400、Braking=2000、TurnRate=540°/s；只提高本次策略，保留有效减速乘数。

松开立即退出；零移动意图超过 NoMoveIntentGrace=0.08 秒退出；持续有输入但阻挡超过 BlockedExitDelay=0.4 秒退出。Pivot 经过低速或朝向恢复期间不立即视为阻挡。接受真实生命伤害、强控制、死亡、MovementStopped、开始攻击或离地均退出；被无敌拒绝的命中不退出。

Sprint 自己的策略结束后重新解析合法基础状态。默认恢复自由行走；如已有合法 ReservedStrafe 来源则恢复八向和对应 Controller 朝向。

## 7. Pivot、根运动与脚步匹配

Main 的掉头使用 Sprint GA 内的独立根运动蒙太奇，不再通过原地 Turn 求值器同时叠加普通加速。旧图保留给兼容／对照；提供 SprintPivotMontage 时，bSprintPivotAllowed 关闭该旧路径。

素材原始 Root 轨迹先前进约 216 cm 至 0.5 秒刹停，再反向起步，1.6 秒结束。普通 CMC 反向输入会提前刹停／加速，固定时间播完原地 Turn 会明显滑步。新资源 A_Hero_Sprint_Pivot_RootMotion／AM_Hero_Sprint_Pivot 保留该位移与转向，使用 RootMotionFromMontagesOnly。CMC 在这个蒙太奇提供根运动时不额外应用普通朝向旋转；不改变其他攻击的旋转规则。

默认 PivotTriggerAngle=150°、PivotMinimumSpeed=250 cm/s、PivotReentryDelay=0.25 秒。拥有者检测当前速度与移动意图的近反向夹角，发送本次 Sprint 关联的 TargetData：IncomingDirection、DesiredDirection、StartingYaw、SessionId、SequenceId。服务器复检活动能力、当前速度、地面、按键、窗口／重入间隔、有限值、方向与起手朝向偏差。重复或越序请求不再播放一次。

拥有者保留起手 Actor Yaw，避免回正瞬跳；服务器对齐通过验证的起手快照。SprintPivotMontage 的倍率沿用 SprintTurnPlayRate，入／出混合分别为 0.12／0.16 秒。松开、攻击、真实伤害、控制、死亡、离地等沿 Sprint 生命周期结束并停止自身蒙太奇。Pivot 刹停阶段不作为阻挡退出；不会授予无敌或完美闪避。

PivotRecoveryEndTime=0.9 为素材时间，GA 根据本次蒙太奇实例的位置结束制动／反向起步控制，并混回循环，不等待 1.6 秒素材的跑步尾段。PivotReentryDelay 从起手计算，控制阶段仍以 bPivoting 防重复；恢复后不追加尾段冷却。新动作前移除旧任务的回调，避免旧混出完成清掉新 Pivot 状态。远端下一序号可有限等待旧恢复点，最多 HandoffNetworkGrace 秒；恢复后重新检查会话、速度、方向和朝向，不能提前跳过当前转身。

朝向复制状态携带动作来源的 GAS SpecHandle 和激活 PredictionKey。拥有者本地结束预测 Dash 后，迟到的权威动作租约不再覆盖当前 Sprint 的 Movement 驱动；只接受仍活动、Avatar 和激活相符的预测动作。服务器专用动作及组件来源保留既有权威回退，不用定时宽限掩盖陈旧动作状态。

循环的原地序列使用独立播放分支 HodgeSprintCyclePlayer，与 Dash F／B 和根运动 Pivot 蒙太奇同属 HodgeSprint 同步组。蒙太奇作为领导保留动作速率，循环以 Hodge.LeftFoot／Hodge.RightFoot 标记跟随脚步相位；同步不作为控制授权，也不改变根运动阶段时间。普通分支的 Locomotion／AlwaysFollower 保留，进入／退出新分支混合为 0.12／0.16 秒。Dash 的混出为 0.18 秒；Dash 和根运动 Pivot 都保持连续脚部贴地，其他全身动作继续抑制 IK。

移动输入提交给 CMC 前，Hero 通知活动 Sprint 检查 Pivot，使用制动／转向之前的速度与起手朝向。定时更新保留补充检查；同一活动 Pivot 和序号规则避免重复启动。不能只在移动计算后检查速度，否则低帧率下可能已减到门槛以下，跳过动画直接转向。

Sprint 循环原素材没有 Root 位移，因此不能调用 SetPlayRateToMatchSpeed 依赖它自动求速。配置 SprintCycleReferenceSpeed 为参考步幅速度，Main 从脚部近地姿势区间估算约 599.07 cm/s。实际播放倍率为实际平面速度／参考速度 × SprintCyclePlayRate，再限制到 SprintCycleMinPlayRate／MaxPlayRate（默认 0.5～1.5）。720 cm/s 下约 1.20 倍，掉头结束时按实际速度连续接回循环。

旧原地 Pivot 的显式时间与完成保护仍保留；它只保证不会卡帧，不能替代上述根运动接入的脚步匹配。八向锁敌资源、完整目标锁定服务和 CameraMode 的边界保持不变。

## 8. 防御与 Cue

无敌默认 `[0.04, 0.22)`，完美默认 `[0.04, 0.10)`，相对成功提交；每 Dash 最多一次完美奖励。普通无敌不自动授予完美。

Melee 在提交伤害和附带 Impact 之前调用 ResolveIncomingHit，返回 Allowed／Dodged／PerfectDodge。不可闪避与不允许完美的攻击独立配置。PerfectRewardEffect 可选，当前没有体力奖励。

GameplayCue_Character_Dash、SprintCue、PerfectDodgeCue 各自处理表现，停止只清本次 Cue。未来投射物／范围伤害需在自己的提交入口接入防御服务。

## 9. 输入、网络与接口

TargetData 只携带原始输入、采样控制 Yaw、起手 Actor Yaw、SessionId，不接收客户端任意速度、防御窗口或费用。服务器拒绝非有限值、越界幅值、无效会话和过大起手面向误差。

CMC SavedMove 保留运动策略快照和 ActivationKey；上传合法 Sprint 激活键，服务器读取自己的配置。未知／过期键不授予高速。保留已有 FacingSequence、受击字段和回放入口；移除 RemainingBudget、DrainPerSecond 与分段资金结算。

Hero：BeginSprintInput、EndSprintInput、GetSprintInputSession、RequestSprintHandoff。GA：HasCommittedMovement、GetInputSessionId、Dash.GetDashDirections。Policy：AcquireSprint／ReleasePolicy、GetResolvedPolicy／GetProfile、SetSprintInput／IsSessionHeld／SessionHeldTime、AuthorizeHandoff／CanHandoff／ConsumeHandoff、AcquireSpeedModifier／ReleaseSpeedModifier。OnSprintAuthorityEnded 用于拥有者响应服务器策略结束，语义与资源无关。

## 10. UI 事实与体力移除范围

Main 的 WBP_HodgeHUD 以 Overlay 承载扩展点。PlayerVitalsPoint 位于左下角，Padding 四边 32；WBP_PlayerVitals 为宽 280 的 SizeBox、Border 内边距 16，纵排 HealthText 和 HealthBar。右下角是 AbilityBarPoint，右上角是菜单提示。数值为 UMG 逻辑单位，屏幕像素受 DPI 缩放影响。

当前没有体力条、体力 Widget 或体力属性绑定，因此不存在可报告的体力条屏幕位置。上述左下条是生命条。移除 StaminaSet 默认子对象、MaxStamina 配置、初始化／回滚、原生 StaminaDelta GE／Tag、Dash费用、Sprint门槛／持续消耗、自然恢复／恢复阻塞、资源预算／预测／SavedMove字段、相关 API 和验证入口。

CodexText Survivor 示例里名为 StaminaBar／StaminaValue 的控件显示等级／经验等示例数据，未绑定本次体力属性，且不属于 Main HUD；不能按名字误删独立示例功能。

## 11. 边界、异常与验证

准备失败、朝向拒绝、未授权取消、骨架不匹配、窗口越界、动画不合法等拒绝当前动作并消费按住会话。失去 Avatar、重新初始化或 EndPlay 解绑所有当前监听和句柄。保持 PlayerState ASC 权限模型。

验证覆盖 F／默认B／侧向F短按完整动画、后摇前禁止移动取消与窗口后取消、长按默认／延后阈值／延后窗口、松开／停步／攻击／伤害／控制退出、实际 Pivot 求值时间、蓝图编译、Main UI 和双客户端 0／100ms 延迟观察。必须分别报告常规 Editor、Game、原生测试、PIE、联机结果；本次不把这些等同于 Cook／打包。

后续可扩展：按动画通知或曲线定义窗口、位移速度曲线、专用镜像／多角度 Turn、空中 Dash、锁定摄像机。重新启用体力需重新评审独立资源模块与 UI，不把本修订删除的字段悄悄恢复。
