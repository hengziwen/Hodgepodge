# 攻击前摇 Motion Warping 设计与首版实施契约

日期：2026-10-13；版本 v1.0；UE 5.5.4；单一 Hodgepodge Runtime 模块。

## 1. 背景、目标与范围

自由模式不再由镜头直接驱动角色 Yaw；五段普攻已有根运动，动画根运动期间的 PhysicsRotation 默认关闭。仅预留前摇时间不能完成攻击定向。

AW-01：攻击立即播放，在前摇窗口内修正根运动旋转，不新增物理转身等待。
AW-02：方向固定采样一次；锁定目标优先，其次有效移动意图，自由无移动输入按镜头水平朝向。最后一项已由用户确认。
AW-03：Warp 窗口与朝向占用独立，窗口在首刀前结束，占用到 Recovery 或动作退出才释放。
AW-04：旧请求、目标、回调不影响新执行，强约束优先；合法 Dash 取消仍按 GAS 标签关系执行。
AW-05：拥有者预测、服务器验证、服务器发起事件、模拟代理及 SavedMove 均有一致的执行上下文。
AW-06：预测移动取消采用暂停／恢复／最终清理，拒绝后不重复播放已经结束的 Warp。
AW-07：配置、非法载荷、迟到数据、倍率、连段及原系统回归有可追溯测试。

首版只为 DA_Attack_1 配置旋转 Warp，其余动作默认关闭。不开自动搜敌、平移吸附、动态目标追踪、攻击旋转锁或空中追击，不修改相机和 Dash RMS。旧吸附草案的后续阶段不属于本次授权范围。

## 2. 模块职责

- HodgeCharacterRotationComponent：保持 Movement／Controller／ActionDirection 驱动，增加 PhysicalTurn／MontageWarp 执行策略，管理动作占用与本地预测暂停。
- HodgeCharacterMovementComponent：MontageWarp 占用期间不执行物理动作转向；保持最终 Yaw 约束、移动校正和原始 Root Motion。SavedMove 保留策略、Definition、目标 Yaw 与身份。
- HodgeGameplayAbility_Definition：建立固定方向、发送和消费单次 TargetData、获准后沿原路径播放 Montage，管理恢复点与结束清理。
- HodgeCombatComponentBase：等待方向数据时保留当前连段授权；数据就绪或失败后完成同一次授权。服务器发起的事件载荷携带固定方向。
- HodgeAttackMotionWarpingComponent：原生 MotionWarpingComponent 的轻量适配，根运动求值前读取正常／回放快照，准备本次目标，仅清理项目攻击修正器和自己拥有的目标。
- HodgeRootMotionModifier_AttackFacing：旋转 Skew Warp，执行 Default＋SlerpWithClampedRate，按实际 PlayRate 转换旋转剩余时间，禁止平移扭曲。
- HodgeAbilityDefinition：每招显式启用与参数，原生 Montage Notify 是唯一窗口时间来源。

不新增 Motion Warping Manager、持续锁定服务或 Runtime 模块。Actor Transform 继续由 CMC 应用。

## 3. 数据结构与接口

FHodgeAttackFacingConfig：bEnabled=false；WarpTargetName=HodgeAttackFacing；MaxCorrectionAngle=90°；MaxRotationRate=1080°/s；RotationTimeMultiplier=1；TargetDataTimeout=1s；StartYawTolerance=45°；TargetYawTolerance=30°。正式数值为可编辑原型基线，限速导致未完全对准时不补 Snap。

FHodgeAttackFacingTargetData：SourceAvatar、LockedTarget、StartYaw、DesiredYaw、Source（Actor／Camera／MoveInput／LockedTarget）。只提交方向，不能提交落点或任意 Transform；SourceAvatar 使用 PackageMap 序列化。

FHodgeCharacterRotationState 扩展 ExecutionMode、WarpDefinition，复用 ActionYaw、ActionRequestId、ActionAbilityHandle、ActionPredictionKey。

RequestMontageWarpFacing(TargetYaw, Source, ExecutionId) 返回当前 Avatar 的句柄；GAS 已校验方向，不另外发送一条组件方向 RPC。IsFacingRequestSelected 查询当前执行许可；不改 Applied 原语义。SetFacingRequestSuspended 只暂停本地解析，不提前撤销权威请求。ReleaseFacingRequest 仍按来源清理。

HodgeAttackMotionWarpingComponent.PrepareAttackTarget(State, Montage, Position) 接收与当前求值一致的快照。ClearAttackWarpForRequest 只清理匹配执行的目标，不能盲目 RemoveAllWarpTargets。

Definition.GetFacingWarpWindow 返回唯一原生窗口；窗口必须使用 Hodge 专用旋转修正器。配置校验拒绝跨越首刀、无根运动、重复窗口、与 Rotation.Locked 重叠等问题。

## 4. 时序与权限

拥有者激活 → 采样固定方向并通过当前 Spec／PredictionKey 的 GAS TargetData 上传 → 本地取得预测许可并立即播放。

服务器激活 → 暂时占用当前姿态（不执行未验证目标） → 绑定缓存消费和有界等待 → 验证 Avatar、方向、起手误差、有效锁定目标、配置及强约束 → 取得 MontageWarp 占用 → 播放 Montage。

服务器发起的连段事件在原事件 TargetData 中携带固定方向，拥有者按该载荷执行，不重新取另一方向。

Warp 通知有效时修正旋转；通知结束后恢复原动画根运动，继续持有占用。进入既有 Status.Attack.Recovery，或真实取消／结束时释放占用，解析当前基础模式，不恢复旧开关。全程保持 bAllowPhysicsRotationDuringAnimRootMotion=false。

Recovery 通知之外，根运动求值前按当前 Montage 源位置补偿检查恢复边界，避免低帧率或网络位置校正跳过通知。已经释放的执行标记为退役；迟到的权威朝向快照不能在 GA 仍活动时重新建立旧占用。

网络身份使用 Avatar＋Spec＋PredictionKey；ExecutionId 是本地资源身份，服务器映射自身执行。目标只消费一次，ASC 有界退役记录拒绝重复和迟到数据。无数据超时／非法载荷取消本次激活，不能无限等待或使用未验证的目标。

## 5. 取消、回放和异常

预测移动取消暂停本地朝向许可并清理当前修正器，但保存本次方向和句柄；服务器接受后最终结束，拒绝后按权威 Montage 位置恢复。恢复到窗口后不重新 Warp；恢复到 Recovery 后不重新取得动作占用。

SavedMove 的 ExecutionMode、WarpDefinition、ActionYaw 及身份参与合并限制与 PrepMoveFor。Warp 组件在回放时使用历史 Montage 和快照，遇到执行切换／位置回退重建修正器，避免复用下一刀缓存。服务器使用已授权活动 Warp 状态，不信任移动包直接声明策略。

目标重叠、非法方向、过大起手差、死亡和强控制不会绕过约束。强约束禁用修正，最终 CMC Yaw 约束仍保留。新动作、解绑或销毁不保留旧请求。默认关闭的技能保持原路径。

第一版不保证背后大角度完全对准。关闭 Translation Warp 保留原动画平移。播放倍率与帧率变化需实测，时间使用 Montage 源时间而非墙钟 Timer；Timer 只限制网络数据等待。

## 6. 验收与实施顺序

1. Editor／Game 常规构建，冷启动编译受影响蓝图和加载新默认组件。
2. 原生配置、固定 Yaw、窗口、请求／暂停、根运动执行、协议退役、历史快照和载荷测试。
3. 第 1 段普攻在自由／锁定模式验证即时播放、前摇平滑转向、窗口后不抢转、Recovery 释放。
4. 两客户端 Listen、PIE Dedicated、100ms 延迟验证拥有者／权威／代理、连续连段、取消接受与拒绝、回放及强控制。
5. Dash、Sprint、Pivot、锁定镜头和既有攻击回归。未验证项如独立专服构建、Cook、打包单独列明。

具体结果写入独立验证报告，不能以脚本启动或 C++ 构建代替 PIE 通过。工程既有未提交改动保留，不自动提交。

本轮结果与复跑命令见 [2026-10-13 验证报告](../Validation/attack-facing-motion-warp-2026-10-13.md)。
