# 受击根运动复评与共鸣解放 Fly 测试

日期：2026-10-17；UE 5.5.4。关联[受击设计](hit-reaction-system.md)、[ACT 总设计](act-combat-animation-feedback-design.md)和[配置手册](../Guides/hit-reaction-configuration.md)。

> 后续实施更新：用户已授权按阶段切换根运动，当前 Hero／假人已接入完整现有 S/B、Fly/Push 和 StandUp。本文下方“未全量切换”“执行器要求原地”等句子保留为首次评估的历史状态，不再代表当前实现。当前契约和验证见[Behit 实施记录](../Validation/authored-behit-root-motion-2026-10-17.md)及配置手册第 12 节。空中仍由 CMC 处理弹道与重力，这属于用户确认的分阶段方案。

## 1. 修正此前的约束表述

“受击必须使用原地动画”只是当前 Hodge 执行器的约定，并不是 UE 的限制，也不是所有受击表现的最佳选择。动画 Root Motion 和 RootMotionSource 最终都由 CharacterMovement 处理碰撞、移动和网络校正；采用动画根运动不等于绕开 CMC。

对于具有完整脚步、重心转移的地面 S/B 受击、踉跄、退步和落地滑动，保留动画根运动值得优先验证。程序生成脊柱动作不再作为后续制作路线。复用现有 Behit 素材，仅制作必要的骨架适配与 Montage，不覆盖源动画。

本轮按用户请求重新评估根运动，实施共鸣解放 Launch 和调试开关；**未将全部受击切换为动画根运动**。现有 Profile 对根运动的校验仍然生效，不能通过单独勾选 Enable Root Motion 绕过执行器约束。

## 2. 本机依据与技术边界

- Lyra `Source/LyraGame/Character/LyraCharacterMovementComponent.cpp`：继续扩展 CMC，GroundInfo 区分 Walking、NavWalking 和地面距离，没有提供可直接迁入的完整 Behit 飞行状态机。
- Lyra `Source/LyraGame/Weapons/LyraGameplayAbility_RangedWeapon.cpp`：用控制台变量切换调试绘制，与真实命中结果分离；本轮继续沿用项目已有同类机制。
- UE `CharacterMovementComponent.cpp::ApplyRootMotionToVelocity`：动画根运动优先于 RootMotionSource 速度覆盖，两者不是可任意同时启用的两条独立位移链。
- UE `CharacterMovementComponent.cpp::ConstrainAnimRootMotionVelocity`：Falling 下保留当前重力方向速度；不能仅靠勾选动画根运动就要求 Falling 沿动画完整的垂直轨迹运动。
- Hodge `HodgeGameplayAbility_HitReaction::Synchronize`：现有 Launch 使用 LaunchCharacter，Knockback 使用 RMS；Montage 通过 ASC 播放。`HodgeHitReactionProfile::Validate` 和实例检查目前要求原地片段。

## 3. 建议按阶段选择运动来源

1. 地面 HitStun/退步：优先评估动画根运动；该次阶段不再同时申请外部击退 RMS。需要检查移动方向、片段完整性、恢复动作取消和多端校正。
2. 地面参数化 Knockback：保留 RMS 路线供指定距离/时长的攻击使用，匹配原地退步动作；与动画根运动路线显式互斥。
3. Fly_Start/Loop、Push_Start/Loop：先保留 CMC 弹道和真实碰撞。已读源片段的 root 在首/中/尾采样位置无平移，开启标记本身不会产生击飞高度。不能据有限采样断言整段无根运动，实施前还需完整提取轨迹。
4. Fly_Fall：用户确认从触地冲击开始，必须在真实落地后进入。可评估接地后的动画根运动缓冲；源片段存在明显根骨水平平移，不能直接开启后就认为位移合理。
5. 空中、落地或恢复阶段切换时，由服务器选择当前动作及运动来源；清理旧来源和具体 Montage 实例，保持真实落地、控制截止和动画退出各自的语义。

实现根运动接入时需补齐逐阶段运动来源配置、校验、GAS Montage 生命周期、复制和取消验证。根运动尾部仍在移动胶囊，不能复用当前“不持有运动责任”的纯视觉尾部策略。不得切换 Flying 来躲开重力后沿固定轨迹假落地。

## 4. 本轮可测试的共鸣解放

- 正式资产：`/Game/Main/Character/Hero/Ability/ResonanceLiberation/DA_Resonance_Liberation`。
- 保留原 Vajra 判定、伤害 GE、倍率、充能消耗、冷却、镜头、翼动作和攻击通知，只给 `DefaultHitConfig.Reaction.Impacts` 增加 Launch。
- 初始测试值：HorizontalSpeed=180cm/s，VerticalSpeed=800cm/s，AwayFromSource，落地不追加 Knockdown。数值用于观察现有 Fly 链，可手调；仍受目标高度、速度、控制和 Body 限制。
- `AM_Burst01` 已有单次 Box 命中通知，`bUseDefaultReaction=true`，不新增第二次伤害，也不移动命中时刻。
- Hero/百万血假人已有 Fly_Start→Fly_Loop→Fly_Fall 对应 Montage。当前 Fly_Fall 由 `TickPlan` 在已经观察到 Falling 且真实回到地面后切换，不按动画长度猜测触地。
- Guardian 当前未配置 Launch，不强行复用不兼容骨架。第一轮使用 `/Game/Tests/Combat/L_HitReactionDummy` 的百万血假人验证，不能把 Guardian 的 Unsupported 当成攻击未命中。
- 配置前备份：`/Game/Tests/Combat/ResonanceLaunch/Backups/DA_Resonance_Liberation_BeforeLaunch`。脚本已有备份或现有 Impact 时停止，不覆盖手动修改。

## 5. 攻击判定体调试开关

`Hodge.Combat.DebugDraw.ConfiguredShape 1/0` 控制共鸣解放这类配置形状的线框，覆盖 Box/Sphere/Capsule，与武器/身体来源分类无关。`Hodge.Combat.DebugDraw 1/0` 是总开关。默认保持此前启用行为，不写用户 ini。

`Hodge.Combat.DebugDraw.Duration` 继续控制线条保留秒数。关闭只停止新绘制，已绘制线条按原寿命消失，不清除其他系统的线条。调试命令只影响运行查询所在进程，不复制服务器调试线到独立客户端；专服不绘制，Shipping 不保证调试命令可用。

## 6. 验证要求

- Editor/Game 常规构建；实际命中查询验证显示开关不改变目标命中集合。
- 资产读回：只增加一个 Launch，原伤害/执行/通知保持一致；目标 Fly 引用合法。
- 真实 R 输入：通过原 GAS 消耗，确认目标接受伤害、Launch、离地、实际落地后 Fly_Fall、结束无控制残留；失败保留记录。
- 本轮不据此宣称动画根运动迁移或最终动作手感验收完成。
