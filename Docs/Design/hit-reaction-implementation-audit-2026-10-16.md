# 当前受击实现记录与假人测试边界

日期：2026-10-16；UE 5.5.4。记录对象为当前未提交的 R9/R5/R1/R3 工作区，不能用 HEAD 代替。本文记录现状，不实施 ACT 表现层重做；最新假人使用见[操作手册](../Guides/hit-reaction-dummy.md)。

> 本文保留表现切片修改前的审计快照。其后已开始按修订计划制作，正式五段、方向反馈、动作重播和工具行为的最新变化见[表现验证](../Validation/hit-presentation-2026-10-16.md)，不以本文的“第一段配置／编译权重 1.0”等旧快照覆盖后续结果。

## 1. 真实调用链与身份

1. `HodgeGameplayAbility_Melee::BeginNotifyHit` 从 Montage 的 `FHodgeAnimHitConfig` 选择伤害配置和受击配置。`bUseDefaultReaction` 独立于伤害继承，决定使用 Definition 的 `DefaultHitConfig.Reaction` 或通知覆盖。
2. R1 的 `WaitHitResults` 保留通知窗口和 PostPhysics 节拍，独立 `HitDetectionComponent` 保存来源、会话、历史、姿势占用；它不提交伤害或选择受击动画。
3. `MakeMeleeHitContext` 已经把完整 HitResult 和 SourceOrigin 放进 EffectContext。表现入口缺少接触信息，不等于伤害 Context 丢失 HitResult。
4. `SubmitMeleeHitEffects` 先处理目标 Defense；允许后调用目标 `BeginHit`，把得到的 HitId 写为 `HitSettlementId`。没有 Reaction 请求时 BeginHit 不创建事务，但伤害仍可正常提交。
5. DamageExecution 提交 Damage 元属性；HealthSet 执行免疫和 GodMode 检查，将 Damage 转为实际 Health 扣除，并通过相同身份记录受击事务结果。`OnDamageAccepted` 广播正的实际生命扣除。
6. R3 角色规则观察同一 SettlementId 的实际伤害，使用独立资源阶段账本；其共鸣、暗流结算不是公共命中表现总线。
7. `FinishHit` 按死亡、拒绝、伤害接受、AttackRank/BodyRank、Impact 支持情况裁决；成功提交目标受击 State。受击 GA 执行动画、RootMotionSource 或 LaunchCharacter。

玩家 ASC 归 PlayerState，普通怪物及假人 ASC 归 Pawn。实际授予、扣血和控制由服务器执行。强受击 State 复制与 GAS Montage 复制共同服务不同网络角色；不能为了减少入口直接删除其一。

## 2. 配置与执行契约

- `HodgeHitReactionProfile::FindAnimation` 只按 ImpactType 找一条记录，校验禁止同类型重复。方向/力度/姿态变体尚未实现。
- AttackRank 与 BodyRank 有 Normal、Skill、SuperArmor、Vajra 四级；当前允许攻击判定使用对应 Body 标签作为兼容输入，因此第一段的 Body.Normal 能被解析成普通攻击判定。
- Profile 的 LightFeedback 只有一个 Montage 与一个可选 Cue。LightFeedback 原生播放没有方向选择或连续命中抑制。
- `FinishHit` 当前只对 LowJudgement、Unsupported、NoImpact 触发轻反馈；Applied、CannotInterrupt、Dead 等不走这条分支。未配置请求时也不会进入。
- `MulticastLightFeedback` 为 Unreliable Multicast，发送 Direction，没有完整接触身份。它保留 Group 冲突检查和 Montage_Play 的 `bStopAllMontages=false`。
- `SubmitPlan` 为每个新计划分配 Sequence。`Synchronize` 对相同状态提前返回，但新 Sequence 即使使用相同 Montage，仍可能以 0.05 秒停止旧动作后重新播放。正常退出使用 Montage 自身混出，不是 0.05 秒。
- 地面计划会停止当前运动；Knockback 使用 ConstantForce，速度为距离/时长，结束速度为零。没有配置 StrengthOverTime。引擎 5.5.4 支持该曲线，但未来加入时须按曲线积分校准理想位移，并处理碰撞/限速/迟到同步。
- 空中普通 HitStun 已能转选 AirHit 动画；并非所有空中衔接都缺失。当前动画选择与逻辑 Type 不完全等价，MaxAirHits 的计数语义须单独审查。
- Downed/GettingUp 接新命中的替换策略不完整；表现过滤与是否重置控制时间属于不同决策，未来不能混改。

## 3. 最新正式资产事实

本轮前序只读审计使用引擎加载磁盘资产并导出文本和编译默认对象，原始结果在忽略目录 `Saved/HitReactionAudit`。源数据受保护字段的直接 Python 查询曾失败，后由 ObjectExporterT3D 补充核实；不能把查询错误当成空字段。

- `DT_LightCombo` 当前顺序：01 → 02 → 03 → 05 → 04。
- `DA_Attack_1` 配置普通判定和默认 HitStun；2/3/5/4 的 Reaction 判定与 Impact 均为空。全部正式命中通知继承默认 Reaction，没有覆盖补足。因此现有五段不是完整的受击力度递进。
- 第二段有两个命中窗口，第五段有三个，分别共享自身 HitGroup 和 Strike 阶段；终结段使用 PoverWingFinisher。资源阶段去重不能直接复用为接触表现去重。
- Hero Profile 支持 HitStun/Knockback/Launch/Knockdown/AirHit/Slam；默认控制时长 0.4 秒，倒地 1 秒，整链上限 6 秒，空中上限 4 秒、最大追击 6 次。LightFeedbackCue 为空。
- Guardian 正式 Profile 只配置 HitStun/Knockback/Knockdown，默认 0.45 秒；Launch/AirHit 未配置，FallbackToHitStun 为空。不能以 Hero 的支持范围代替 Guardian。
- Hero LightFeedback 是约 3.367 秒的 Local Space Additive，使用自身第 0 帧作为基准；Guardian 轻反馈约 0.6 秒，是上半身 Slot 中的非 Additive 动作。
- Hero HitStun Montage 已约为 0.4 秒，混入 0.08 秒、混出 0.18 秒；正常退出已有改进，不能重新描述成统一短混出。
- Hero 上半身节点结构保存 0.35，但暴露引脚默认值和编译默认对象均为 **1.0**。后续需校准实际编译/运行权重，不能继续把工具写入值当成最终值。
- Hero 图的关键顺序：FullBody Slot → Inertialization → RotateRootBone → 缓存基础姿态 → AdditiveHitReact/ApplyAdditive → 上半身 BranchFilter → SkeletalControls → 输出。局部反馈位于惯性化之后。
- 近战伤害 GE 已配置 `GameplayCue.Character.DamageTaken`，但 Tag 不证明已有对应音画执行；当前静态搜索尚未确认接触特效/声音接线。正式 Profile 的轻反馈 Cue 为空。

## 4. 当前假人怎样保持有效基线

假人使用独立 Stats、PawnData、AbilitySet、Profile 和蓝图；Profile 是正式 Hero Profile 的独立快照，动画/骨架仍引用当前正式资源，权重等已知问题原样保留。它不是优化后的演示。

最大/当前生命由 EnemyInit 的现有初始化链设为 1,000,000，不使用无敌、伤害免疫、恢复生命 Tick 或假扣血。数值在 float 可区分当前小额伤害的范围内。普通 AIController 只负责 Possess；不运行行为树，不锁死 CharacterMovement，允许击退、挑飞和落地。

测试攻击是独立装备来源，通过真实原生 Melee、通知和 GE 触发；不直接调用 FinishHit 或伪造结果。Light 为有效判定但空 Impact，预期 NoImpact 轻反馈；六种 Impact 分别有独立资产。同一时间只装备一种测试来源，移除只清理自己的句柄。

## 5. Lyra 核查与复用边界

- `Source/LyraGame/AbilitySystem/Attributes/LyraHealthSet.cpp`：沿用真实 Health/MaxHealth、服务器数值与属性复制；不通过客户端显示血量制造假人耐久。
- `Source/LyraGame/Character/LyraPawnExtensionComponent.cpp`：保留 ASC Owner/Avatar 绑定及初始化生命周期。本项目怪物通过既有 EnemyInit 适配 Pawn 自有 ASC，不引入 PlayerState。
- `Source/LyraGame/AbilitySystem/Executions/LyraDamageExecution.cpp`：已有 HitResult、物理材质及缺接触信息回退。公共表现扩展应复用现有 Context。
- `Source/LyraGame/Animation/LyraAnimInstance.cpp`：沿用 ASC/TagPropertyMap 动画接入；本机 Lyra 没有可直接替代 Hodge 近战 Impact 阶段、翼来源或完整受击演出的同职责实现。

## 6. 后续顺序与验证边界

后续按[修订实施计划](hit-reaction-system.md#17-2026-10-16-受击表现实施计划修订)执行：P0 优先正式五段普攻的可视化垂直切片，重新读取正式资产，处理短方向性轻反馈、重复强受击重启和公共接触音画；配置整理作为 P1 辅助。解除工具按控制时长隐式裁剪动画的默认行为在 P0 动画编辑前处理。公共恢复动画引用迁移后置 P2，完整运动打磨后置 P3；HitStop 不属于本次假人任务。原始审计快照不代表新计划已实施。

假人测试不自动修复 Client 普攻窗口、正式攻击空请求或动画素材质量。构建、蓝图编译、真实 PIE 数值/阶段与主观画面评价分别记录；NullRHI 运行验证不代表视觉手感合格，也不是 Listen/Dedicated 联机验收。
