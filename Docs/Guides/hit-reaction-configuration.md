# 受击配置手册：Body 判定与目标侧 Impact

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

本版主运动由 Impact／CharacterMovement 执行，受击蒙太奇使用原地动作，不能同时再提供完整根运动位移。已有第三方动画若不符合要求，制作项目自有副本／重定向结果，不修改第三方源资产。

全局参数：

- FallbackToHitStun：列出不支持／不适用时允许降级的类型，并配置 HitStun 条目。降级仍要求本次等级已通过。
- DownedDuration、bAutoGetUp：目标倒地保持时间与自动起身策略。关闭自动起身时由服务器调用组件 RequestGetUp；仍有 MaxControlDuration 超时兜底。
- MaxControlDuration：一次连续控制链的最大秒数，重复命中不会无限重新计时。
- MaxAirborneDuration、MaxLaunchHeight、MaxLaunchSpeed、MaxAirHits：连续浮空的时长、高度、速度及 AirHit 托举次数限制。
- InterruptibleAbilityTags：除默认攻击 Definition／Jump 外，需要打断的其他能力标识；不要包含被动或 UI 等无关能力。

击倒使用目标的倒地动作并保持受控，满足条件后播放自己的 GetUp。Launch 使用目标腾空动作，实际落地后播放 Landing；需要倒地时衔接 Knockdown 条目。Falling 本身不等于受控。

目前强动画按 Type 映射，未提供每个方向的多条原生强动画选择表；轻反馈方向通过 OnLightFeedback 提供，方向变体可在目标表现层扩展。

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
