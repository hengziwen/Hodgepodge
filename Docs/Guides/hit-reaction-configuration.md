# 受击配置手册：Body 判定与目标侧 Impact

> **完整制作与验收入口：**[战斗制作执行文档](combat-production-workflow.md)按当前 Compact/InPlace、普通轻重削韧与分段 Can Launch 路线，提供真实资产/窗口参数、迁移清单和单目标端到端步骤。本文后续带日期的旧布局说明按历史范围阅读，不覆盖执行文档中的当前契约。

> 2026-10-18 当前作者入口：Montage 每个命中分段的 `Hit.bCanLaunch`（Can Launch）独立决定击飞资格，默认关闭；默认 Reaction 或旧 Launch 参数不能替代这个开关。普通 `PoiseDriven` 削韧最多到重僵直；Profile 继续只负责目标动作。最新契约见[分段击飞设计](../Design/attack-segment-launch.md)与[验证记录](../Validation/attack-segment-launch-2026-10-18.md)。下方旧破韧、预留削韧与全根运动说明属于历史布局。

当前样例阈值为 1.5 秒内轻 20／重 55，同轮阈值只消费一次；削韧分数到期不取消现有控制。旧 100 点破韧和保护参数已隐藏且不再生效。正式五段削韧仍为 15／20／25／25／80；只有终结刀 04 与 R 的命中分段开启 Can Launch，击飞不依赖这 80 点或累计量。空中被开启开关的分段命中会重播 Fly，未开启则继续下落，真实接地后倒地起身。

目标控制时长与动作内容独立：`Compact.Light.ControlDuration=0.4s`、`Heavy.ControlDuration=0.55s` 为 Hero／假人样例；地面重受击距离在 `PawnData.Poise.HeavyDistance`（65cm）、`MoveDuration`（0.25s）与 `DisplacementScale` 调整。接触特效强度不再决定 S/B，普通轻硬直不移动胶囊。`B_L` 当前只播放前 1.10 秒以排除原片坏区间，不通过延长控制或放大权重掩盖坏帧。

> 2026-10-17：近期受击、动画衔接、连段／闪避与悟空参考统一见[总设计](../Design/act-combat-animation-feedback-design.md)和[实施／验收计划](../Design/act-combat-animation-feedback-plan.md)。本轮已新增强动画变体、持续／恢复段、分时恢复与视觉尾部；字段说明见本文末尾，实际验证结果单独登记，不能沿用 10-16 的通过结论。

> 2026-10-16：当前源码与正式资产的差异见[实现审计](../Design/hit-reaction-implementation-audit-2026-10-16.md)；需要长时间实测时使用[100 万血量假人](hit-reaction-dummy.md)。旧验证记录不替代当前五段配置核对。

> 2026-10-16 表现切片：新增 DirectionalLightFeedback、ContactEffect、ContactSound 与攻击 PresentationStrength；正式五段与原型变化、实测范围见[表现验证](../Validation/hit-presentation-2026-10-16.md)。DefaultDuration／ControlDuration 只对地面 HitStun／Knockback 展示和校验，MoveDuration 只对 Knockback 使用；预留削韧保留兼容但不在普通详情中编辑。公共 AirLoop／Landing／GetUp 引用尚未迁移。

日期：2026-10-08。适用 UE 5.5.4。本手册描述本次新增源码接口，不表示所有正式动画和怪物资产已配置或通过 PIE／联机验证。设计背景见[受击系统](../Design/hit-reaction-system.md)，已有伤害与通知配置见[攻击配置手册](attack-ability-configuration.md)。

## 1. 配置归属

- 攻击 Definition／GA：自己的 Body 状态。
- 攻击 Definition／命中通知：输出判定、Impacts 和预留削韧。
- 目标 HitReactionProfile：自己的受击、倒地、起身、腾空和落地动画及恢复限制。
- 目标 AbilitySet：授予 HodgeGameplayAbility_HitReaction。
- 目标 HitReactionComponent：原生默认组件，跟随当前 Pawn 的 ASC 初始化；不再通过 Experience 重复添加。

攻击方不引用具体怪物的蒙太奇。一个 Launch Impact 可以作用于不同骨骼的目标，由目标各自 Profile 选择自己的动作。

## 2. 四级状态与比较

Body 为 State.Combat.Body.Normal、Skill、SuperArmor、Vajra，对应 1、2、3、4。输出判定可使用同样的叶子 Tag，或 Combat.Attack.Judgement.Normal、Skill、SuperArmor、Vajra。

攻击等级大于等于目标 Body 等级才允许执行 Impact；低于时保留合法伤害，只做轻反馈。多个 Body 取最高，取消前保存快照。当前实现无 Body 按内部 0 处理，未知攻击 Tag 不作为合法判定。

## 3. 攻击 Definition 与每刀覆盖

正式普攻入口为 `/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1～5`。2026-10-08 已确认并保存五段 ExecutionBodyTag=State.Combat.Body.Normal；本轮保留用户现有判定、Impacts、伤害和通知配置，见[主角接入记录](../Validation/hero-hit-reaction-setup-2026-10-08.md)。

1. ExecutionBodyTag 填该动作自身的体状态，例如 State.Combat.Body.Normal。它独立于输出判定。
2. 展开 DefaultHitConfig.Reaction，AttackJudgementTag 例如填 Combat.Attack.Judgement.Skill。
3. 在 Impacts 加入一项，普通硬直选 HitStun；挑飞选 Launch。
4. ReservedPoiseDamage 可暂填 0。它只随结算结果传递，当前不会修改韧性属性或触发破韧。
5. 保留原 DamageEffect、倍率和检测配置。空 Reaction 保持原来的纯伤害行为；有判定但 Impacts 为空也不会隐式取消目标技能。

整段 Body 由 Definition GA 按执行身份添加／释放；独立 GA 也可使用其 ActivationOwnedTags。两种方式避免重复声明同一作用域。特殊霸体区间继续用 Montage 上的 Hodge 状态区间通知，填 Body.SuperArmor，窗口结束后恢复其他有效 Body。

命中通知 Hit 中新增 bUseDefaultReaction、ReactionOverride：

- bUseDefaultReaction=true 使用 Definition.DefaultHitConfig.Reaction。
- false 使用 ReactionOverride 整份配置，可只改变终结刀的判定／Impact。
- 与现有 bUseDefaultDamage 独立。通知 Damage.Reaction 不是独立第三配置入口。
- DamageScale 仍只影响生命伤害倍率，不提高判定等级，也不缩放运动或预留削韧。

## 4. Impact 参数

Type 支持 HitStun、Knockback、Launch、Knockdown、AirHit、Slam。

- ControlDuration：期望控制秒数；0 采用目标对应条目的 DefaultDuration，受目标 MaxControlDuration 限制。
- Direction：AwayFromSource 按本次命中来源位置向外；SourceForward 使用攻击者当前前向；WorldDirection 使用配置的世界方向。世界方向需有有效水平分量。
- KnockbackDistance／MoveDuration：地面击退距离及运动时长；实际位移受碰撞和速度限制。
- HorizontalSpeed／VerticalSpeed：抛飞／空中受击／下砸的速度幅值，单位厘米／秒；Slam 的竖直方向由执行器转成向下。
- bKnockdownOnLanding：空中效果落地后进入目标倒地流程；Slam 默认包含倒地落地结果，需要目标有 Knockdown 配置。

同一次命中最多一种主要运动，可组合 HitStun + Launch。不要组合 Knockback + Launch，也不要把立即 Knockdown 与运动并行；使用落地结果表达先飞后倒。

HitAcceptancePolicy 默认 AcceptedDamage，要求这次 Damage 输出进入 HealthSet 并被接受，再参与等级／Impact 判定；它检查消费是否被接受，不是简单判断生命减少量大于 0。GE 没有写出 Damage 时不会触发控制。ExplicitControl 明确允许没有 Damage 输出时尝试控制，但已明确拒绝的伤害、伤害免疫／GodMode、死亡和等级检查仍然生效。现有普通 Melee 仍需要合法伤害 GE，不因设置该策略自动支持缺 GE 的技能。

## 5. 目标 Profile

主角现有 Profile 为 `/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction`，由当前 `/Game/Main/Data/PawnData/DA_Dafult_PawnData` 引用。其他目标可以创建自己的 HodgeHitReactionProfile 数据资产。

2026-10-09 主角默认 HitStun 段调整为 0.4 秒，预留 0.18 秒混出，避免把完整 4.33 秒素材在中途强停。受击 GA 退出使用蒙太奇自己的 BlendOut。自定义攻击若要求不同控制时长，仍需选合适的目标动作和恢复策略，不能把任意长素材配任意短控制值当作观感已完成。

Animations 每种 Type 配一条：

- Montage：目标当前强受击动作，必须匹配目标骨骼、长度有效、无根运动。
- AirLoopMontage：空中保持动作，可选；执行器在等待落地期间重复播放。
- LandingMontage：实际落地后的动作，可选。
- GetUpMontage：倒地后的起身动作，可选。
- DefaultDuration：本类效果未指定 ControlDuration 时的默认值。
- bAllowWithoutMontage：明确允许只执行玩法而不播放动作。默认关闭；原生测试才使用无动画配置，不建议以此掩盖正式动画缺失。

旧版主运动由 Impact／CharacterMovement 执行并要求原地 Montage；当前已扩展为逐阶段运动来源，见第 12 节。Impact 路线继续要求原地，AnimationRootMotion 路线由动画提供位移，两者不能叠加。使用项目副本／重定向结果，不修改第三方源资产。

全局参数：

- FallbackToHitStun：列出不支持／不适用时允许降级的类型，并配置 HitStun 条目。降级仍要求本次等级已通过。
- DownedDuration、bAutoGetUp：目标倒地保持时间与自动起身策略。关闭自动起身时由服务器调用组件 RequestGetUp；仍有 MaxControlDuration 超时兜底。
- MaxControlDuration：一次连续控制链的最大秒数，重复命中不会无限重新计时。
- MaxAirborneDuration、MaxLaunchHeight、MaxLaunchSpeed、MaxAirHits：连续浮空的时长、高度、速度及 AirHit 托举次数限制。
- InterruptibleAbilityTags：除默认攻击 Definition／Jump 外，需要打断的其他能力标识；不要包含被动或 UI 等无关能力。

击倒使用目标的倒地动作并保持受控，满足条件后播放自己的 GetUp。Launch 使用目标腾空动作，实际落地后播放 Landing；需要倒地时衔接 Knockdown 条目。Falling 本身不等于受控。

目前强动画按 Type 映射，未提供每个方向的多条原生强动画选择表；轻反馈已有 DirectionalLightFeedback 与 FindLightFeedback 按目标局部来源选片，缺条目回退 LightFeedbackMontage。OnLightFeedback 继续提供方向事件，不需要为现有四向轻反馈重复创建选择系统。

## 6. 轻反馈

目标 Profile 的 LightFeedbackMontage 可选，需无根运动，并与主动作属于不同 Slot Group。Slot 接法和 Additive／骨骼混合由目标 AnimBP 配置，名称本身不代表 Additive。

执行器发现与 ASC 当前主蒙太奇同 Group 时会跳过该轻反馈蒙太奇，避免打断攻击；OnLightFeedback 仍提供方向供蓝图表现使用。LightFeedbackCue 可接现有 GameplayCue 音效／特效资源。

轻反馈不使用普通 ASC 主蒙太奇播放接口，不启动强受击 GA，不移动胶囊，不产生控制限制。

主角现已使用 `FullBody`（DefaultGroup）播放强受击；`AdditiveHitReact` 属于独立 `HodgeHitFeedback` Group。主图把轻反馈移动到 FullBody、Inertialization 和 RotateRootBone 之后，在加法 Identity Pose 上取轻反馈 delta，叠加到缓存的主姿势，再从 Bip001Spine 开始按 0.35 权重混合，最后进入原骨骼控制层。轻反馈序列使用 Local Space Additive、当前动画第 0 帧为基准，无根运动；后续调手感可修改蒙太奇和主图混合权重。

原生播放显式传入 `bStopAllMontages=false`。不同 Group 必须配合这个参数，才能避免引擎默认停止所有蒙太奇。

## 7. 目标 PawnData／AbilitySet 与敌人初始化

1. PawnData.HitReactionProfile 引用目标 Profile。
2. PawnData.AbilitySets 中的一份 AbilitySet，在普通 GrantedGameplayAbilities 添加 HodgeGameplayAbility_HitReaction 或其蓝图子类，InputTag 留空。只授予一份，不放进 GrantedAbilityDefinitions。
3. 玩家保持 PlayerState ASC 所有权，现有 Experience／Hero／PawnExtension 接入不替换。

4. 对原生 HodgeEnemyCharacter 派生目标，可以在类默认值设置 EnemyPawnData，InitialLevel 在 StatProfile 合法范围。原生入口创建 Pawn 所有权 ASC、HealthSet／CombatSet，并按 PawnData 授予能力；重复调用幂等。
5. 已通过其他入口绑定 ASC 的怪物继续使用原入口，不由 EnemyPawnData 覆盖。没有使用 PawnData 的已有目标，可在 HitReactionComponent.OverrideProfile 指定 Profile，但仍需初始化合法 ASC 并授予受击 GA。

当前主角 `/Game/Main/Data/AbilitySet/DA_Pover` 已在普通授予列表中添加 `/Game/Main/Character/Hero/Ability/GA_Hero_HitReaction`；父类为 HodgeGameplayAbility_HitReaction，等级 1，InputTag 留空。原跳跃／死亡能力保留。

EnemyPawnData 的 StatProfile 用现有 InitializationEffect 初始化等级基础生命／伤害，初始化后满血。它只提供敌人首次绑定，不代替玩家成长／装备协调器，也不实现敌人存档和升级系统。

敌人 GAS 接口在 PawnData 绑定前也能返回其原生 ASC，供先到达的属性复制解析所属组件；外部已绑定 ASC 优先。业务就绪仍需确认 ASC 的 Avatar、PawnExtension 绑定和所需配置，不能仅用 ASC 指针非空判断。

敌人需要正常死亡流程时，继续在其能力包配置现有死亡能力和对应表现；受击不会把死亡动画或销毁责任塞进玩家攻击 GA。

## 8. 运行结果、取消与移动

目标组件暴露 GetReactionState、GetLastResult、OnReactionResolved、OnReactionStateChanged、OnLightFeedback。Melee GA 的 OnMeleeReactionResolved(Target, Result) 在共同提交入口完成后提供确认结果。

Outcome=Applied 且 AppliedImpacts 包含 Launch 才表示实际挑飞计划成立；Unsupported、CannotInterrupt、NotReady、Rejected、LowJudgement 都不是成功挑飞。命中受击接口不会自动启动玩家追击或空中连段图，后续攻击技能应消费该结果。

强受击添加 State.Combat.HitReaction.Controlled。普通非 OnSpawn 能力默认不能在该状态激活；死亡和受击 GA 允许。专门的受身／逃脱技能可明确打开 bAllowWhileHitReacting，不要给所有攻击打开。

取消前先建立限制，攻击走原来的 EndAbility 关闭检测、Body、武器和旋转资源。不能取消的目标能力会报告 CannotInterrupt，不靠覆盖动画伪装打断成功。受击禁止自主输入／寻路，不 DisableMovement，因此外部运动仍可执行。

地面 Knockback 使用 CharacterMovement RootMotionSource；Launch／AirHit／Slam 使用受限 Launch 速度。结束时移除自己的运动源，并清理尚未消费的自有 PendingLaunch。已经形成的物理下落继续由移动组件处理。

## 9. 最小验收步骤

保存当前资产并关闭编辑器完成常规构建后，再用 UE 5.5.4 打开项目，编译受影响角色／AnimBP。

1. 先配目标 HitStun、授予受击 GA，选择一招配置 Skill 判定。目标 Body.Normal／Skill 时应中断当前可取消动作；SuperArmor／Vajra 时应扣血并轻反馈。
2. 改为 Vajra 判定，Vajra 目标同级也应通过；未配置 Reaction 的原技能应保持原行为。
3. 逐步加入 Knockback、Launch、AirHit、Knockdown、Slam 和对应目标动画，测试墙体、天花板、地面、落地、起身与重复命中上限。
4. 测试取消、死亡、重生／Pawn 更换，无 Body 或 Controlled 残留；将目标能力设为不可取消时检查明确失败结果。
5. 两玩家 Listen Server 验证拥有者、服务器、模拟代理与延迟校正。客户端不能只因自己的动画播完就解除服务器控制。

原生测试和常规构建不代替上述资产、PIE、联机或打包验证。本次实际执行结果见[验证报告](../Validation/hit-reaction-2026-10-08.md)。

## 10. 2026-10-17 ACT 持续段与恢复配置

`FHodgeHitReactionAnimation.Variants` 在同一 Impact 下按目标局部来源方向与 `MinimumStrength` 选择动作：只使用满足力度阈值的同方向最高档，找不到时使用原 `Montage`。阈值范围 0.1～2，同方向同阈值不能重复。方向、力度只影响表现，不提高 AttackJudgement、不绕过 Body，也不修改伤害。

`SustainSection`／`RecoverySection` 可留空保留原片段行为；设置时必须成对，并存在于默认及每个变体 Montage 中，仅支持地面 HitStun／Knockback。强受击进入持续段后可继续保持姿态，服务器释放控制时跳到恢复段。`StartedAt` 是动画起播时钟，`RecoveryStartedAt` 是最新强受击的恢复等待起点，续播时两者不会合并。`ReplacementBlendOut` 默认 0.05 秒，范围为有限非负值，只控制强受击替换，不修改普通自然退出混出。

`bSustainGroundHitStun` 控制地面硬直是否保持姿态至控制结束。`bAllowNaturalRecoveryTail` 默认关闭；开启后只保留具体原地 Montage 实例的视觉恢复尾部，先释放受控标签、运动、姿势与能力占用。移动、新动作、死亡、解绑、新强受击或有限期限到达时退出该尾部，不能清理同资产的新实例。它不是延长硬直，也不延长伤害或收益窗口。

`KnockbackStrengthCurve` 留空保留恒速；配置后横轴为 0～1 归一化移动时间，纵轴为非负速度权重。曲线面积必须大于零且值、切线有限；按积分归一化期望位移，再按 `MaxLaunchSpeed` 限制峰值。`FHodgeRootMotionSource_Knockback` 继承原生 ConstantForce，只对当前物理步求曲线平均值，继续复用其复制、时间推进、模拟追赶和结束策略，不另行移动胶囊。此适配针对实测的单点采样偏差；碰撞、峰值限制及网络校正仍可能使位移偏离期望值，必须结合实际轨迹检查，不把曲线本身当作脚步动画。

`bEnableGroundRecovery` 默认关闭，旧敌人不自动获得恢复资格。玩家实验值为 `DodgeRecoveryDelay=0.2`、`AttackRecoveryDelay=0.3`、`MovementRecoveryDelay=0.4` 秒，允许有限非负值。只作用于存活、同 Avatar、地面、无位移的 HitStun；击退、空中、倒地、起身不能套用。控制本身提前自然结束时，恢复延时不会额外施加一层禁止行动。

反击还要求该攻击 Definition 的 `bAllowHitRecoveryAttack=true`，本轮只为正式五段普攻设置，E／R 不因此开放。闪避与普攻仍必须满足原 GAS 费用、冷却、状态、授予与请求授权。提交失败不解除旧硬直；服务器只交接当前 Sequence，客户端等待权威控制复制，不通过提前删除服务器受击 GA 实现预测免控。连续新强受击重开恢复等待，即使视觉上继续同一个动作。

恢复普攻等待权威解锁期间保留原连段准备授权；解锁后仍通过同一方向 TargetData 和 Montage 路径接管节点。播放攻击动画与连段节点有效是两项独立检查，不能只观察动画。独立旋转锁／MovementStopped 仍可阻止恢复，100ms 延迟时动作可能晚于自然释放，不保证与单机相同响应。

本轮样例位于 `/Game/Tests/Combat/ACTPrototype`，包含四向 Medium／Heavy、持续／恢复片段及击退曲线；原正式配置备份保存在该目录的 `Backups`。这些是可替换动作原型，不是最终动画质量证明，跑攻和切手尚未制作。

## 11. 共鸣解放 Fly 测试与判定框开关

2026-10-17 用户确认 `Behit_Fly_Fall` 从触地冲击开始，只在真实落地时播放。共鸣解放 `DA_Resonance_Liberation.DefaultHitConfig.Reaction.Impacts` 新增 Launch，初始水平/竖直速度为 180/800cm/s，保留已有伤害、Vajra 判定和单次 Box 通知。目标仍需支持 Launch；可用 `/Game/Tests/Combat/L_HitReactionDummy` 测试，Guardian 当前没有 Launch 配置。

- `Hodge.Combat.DebugDraw.ConfiguredShape 1`：显示配置攻击判定体，包括 R 的 Box；设为 `0` 关闭新绘制。
- `Hodge.Combat.DebugDraw 1/0`：全部命中检测调试绘制的总开关，开启后仍遵守各分类开关。
- `Hodge.Combat.DebugDraw.Duration 0.2`：把后续线条的寿命设为 0.2 秒；已经生成的线条仍按生成时的寿命消失。

命令只影响本进程实际执行的检测，不修改碰撞、伤害或命中次数。单人/Listen 主机可观察服务器绘制，独立客户端不会自动收到服务器线条。原 Weapon/Body/HitBox 分类继续适用于组件来源；ConfiguredShape 不再依赖默认 WeaponSourceTag 决定显示。

原地动画是旧执行器约束，并非 UE 限制。地面受击与落地动作采用动画根运动的价值、与 RMS 的互斥及空中重力边界见[根运动复评](../Design/hit-reaction-root-motion-evaluation-2026-10-17.md)。后续已按用户授权实施第 12 节方案；不要单独修改第三方源动画标记来绕过逐阶段校验。

## 12. 2026-10-17 现有 Behit 与分阶段根运动

### 12.1 当前资源与选择

正式 Hero Profile：`/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction`。百万血假人 Profile：`/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction`。两者引用的当前动作在 `/Game/Main/Character/Hero/Anim/HitReactions/Behit`，每个 `AM_` 对应同目录 `A_` Sequence。

- HitStun：`AM_Behit_S_L/R` 为小幅、`AM_Behit_B_L/R` 为大幅；Variants 中 MinimumStrength 初值分别 0.1 和 1.0，选择满足阈值的最高档。完整片长约 3.37／3.93／4.33 秒，控制时长不自动裁剪动画。
- Knockback：`AM_Behit_B_L/R`，使用地面重受击，不再引用 Push_Start。现有 B 动作包含失衡和恢复，不能把它称作专门制作、任意距离匹配的退步动画。
- Launch：`AM_Behit_Fly_Start`→`AM_Behit_Fly_Loop`→实际落地后 `AM_Behit_Fly_Fall`。
- AirHit：`AM_Behit_Push_Start`→`AM_Behit_Push_Loop`→实际落地后 `AM_Behit_Push_Fall`。
- Slam：借用 Push 三段，真实下砸仍由原速度和落地倒地规则决定；没有新增专门的下砸动画。
- Knockdown：`AM_Behit_Fly_Fall`；已有倒地接地素材的复用，不代表单独的地面倒下起始已制作。GetUp：现有 `AM_StandUp`，完整约 2.5 秒。
- 不打断时的局部反馈：`AM_Local_S_L/R`，来源为现有 S 动作的前 0.3 秒，加工为 LocalSpace Additive；不生成骨骼关键帧。只有左右两套，前／后方向回退到其中一套，不宣称四向原创素材齐全。

旧 ACTPrototype 八个强动作、旧 PresentationPrototype 动作仍在磁盘，但 Hero／假人 Profile 不再引用。迁移前 Profile 与五段攻击副本位于 `/Game/Tests/Combat/BehitMigration/Backups`，不重复运行迁移脚本覆盖手调。

### 12.2 运动、控制和恢复

每条 Animations 新增 `MotionSource`、`LandingMotionSource`、`GetUpMotionSource`。枚举 `Impact` 使用原 Launch／外部击退 RMS；`AnimationRootMotion` 使用 Montage 位移。当前 S/B、Fall、StandUp 选择动画根运动；空中 Start／Loop 选择 Impact，源副本禁用根运动且锁根。Runtime State 保存已经解析的阶段来源与 Montage，各端不能独立挑片。

动画根运动地面 Knockback 不消费攻击请求的 KnockbackDistance／MoveDuration 来计算位移；这些字段仍服务其他目标的 Impact 路线。动画根运动也不受 MaxLaunchSpeed 对素材根轨迹的限速。实际位移受原轨迹、朝向、碰撞和网络校正影响；改变距离需要调整项目副本的内容／选片，不应同时增加一份外部推力。完整 Fall 的原轨迹有约 3.5～3.94 米水平位移，可能偏大，保留原动作供用户评估，未擅自缩短。

`AirLoopTransitionTime` 是有限非负秒数，当前 0.18。正值要求经过该时间且实际下落后才切 Loop；0 保持旧按 Start 片长切换的兼容行为。Fall 始终由真实落地触发。Loop 可重复姿态，空中不重复根位移。普通空中接触转为 Push 表现而保留弹道；只有显式 AirHit 托举才累计 MaxAirHits。

`DefaultDuration`／请求 `ControlDuration` 继续控制地面受控时间，不是片长。控制比完整动作短时可接管恢复尾部；比片长长时保持末姿态，不从头循环根位移。落地控制标记 `bLanding` 阻止把落地缓冲误判成可主动恢复的地面 HitStun。

`bAllowRootMotionRecoveryTail` 默认 false，当前 Hero／假人 true；与原地尾部开关独立。开启后自然解控仍允许剩余根位移和姿态完成，服务器继续保持骨骼更新。主动输入、新动作、死亡、新强受击、解绑或离地会停止旧实例根运动并混出。关闭后自然结束即清理根运动，但可能截掉脚步恢复。它不延长伤害窗口、硬直或收益。

Hero 地面恢复仍为 0.2 秒闪避、0.3 秒普攻、0.4 秒移动，假人不开主动恢复。资格中的 `bHasMovement=false` 指没有外部击退／Launch 请求，根运动 HitStun 仍可按上述时间交接；最终动作仍须通过 GAS 原费用和授权。

### 12.3 左右与重复命中

攻击 `Reaction.PresentationSide` 有 Automatic／Left／Right，明确指定优先；当前正式顺序 01→02→03→05→04 为 L/R/L/R/L。Automatic 在侧面来袭时用局部来源方向；开启条目 `bUseLateralVariants` 后正前／后来源按服务器反应身份奇偶交替，不在客户端随机。该初始编排尚未按武器逐帧轨迹做美术验收。

Profile `GroundReplayInterval` 为有限非负秒数，当前 0.16；0 保留旧持续策略。间隔内同级命中保留同一 Montage 起点；达到间隔的独立命中允许重新受力；更轻命中保持较重反馈，更重命中可替换。增大值减少重入但容易看起来没有逐刀反应；减小值更容易反复起始。恢复等待仍从最新命中计时，不从沿用的动画起点算。

五段强度仍为 0.65/0.8/1.15/1.0/1.6。01／02 新增 HitStun，ControlDuration 分别 0.16／0.18 秒；03／05 使用原 0 回退目标默认，04 保留 Knockback 请求。原伤害、执行 Montage／通知、判定等级与 Body 不改。R 的 Launch 仍为水平 180、竖直 800cm/s。全链上限可能提前结束很长的倒地／起身，不能只看某个 Montage 长度。

验证与待调边界见[实施记录](../Validation/authored-behit-root-motion-2026-10-17.md)。这份配置说明不宣称最终手感已达到参考项目。
