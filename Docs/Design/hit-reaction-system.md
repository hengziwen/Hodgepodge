# 受击系统设计：Body 等级判定与 Impact 执行

日期：2026-10-08。第三版，整合四级体状态、独立攻击判定、Impact、目标侧动画与当前项目接入方案。适用 UE 5.5.4 与现有 Hodgepodge Runtime 模块。

**状态：核心 C++ 已接入，正式动画／目标资产与联机手感尚待配置验证。** 本文替代上一版以反应意图、独立控制等级和完整韧性系统为中心的方案。阶段 A～C 的类型、字段、Tag 和执行入口已创建；玩家追击目前提供确认结果接口，不自动配置空中连段。实施前基线见第 2 节，当前边界见第 16 节；实际作者字段以[受击配置手册](../Guides/hit-reaction-configuration.md)为准。

关联设计：[近战索敌与攻击吸附](melee-attack-assist.md)。当前攻击操作以[攻击能力配置手册](../Guides/attack-ability-configuration.md)为准，动画时机继续使用 Montage 通知，不恢复自建 Timeline。

阅读顺序：第 1、3 节看判定规则；第 5 节看配置职责；第 8 节看项目接入；第 16 节看实际实施边界。正文保留设计过程中的提议和历史基线，当前已实现字段与操作以配置手册为准，不能把后续资产制作或玩法验收视为已经完成。

**配置归属明确为：攻击方配置判定及 Impact 参数，目标方配置自己的受击动画、控制／恢复流程，目标方受击 GA／组件执行。怪物倒地、起身和腾空动画不放进玩家攻击 GA。**

## 1. 核心模型与评审结论

一次命中以三个核心参数描述受击：

1. `AttackJudgementTag`：这一击的攻击判定等级。
2. `Impacts`：判定通过后执行的受击效果配置。
3. `ReservedPoiseDamage`：预留削韧值，当前不消费、不产生破韧或修改等级。

目标 GA／状态效果独立持有 `State.Combat.Body.*`，表达当前承受攻击时的体状态。服务器比较本次攻击判定等级与目标的有效 Body 等级；通过后执行配置的 Impact，否则只产生不改变玩法控制的轻反馈。

```text
Body Tag：我现在处于什么体状态，别人多强的判定才能打断我？
AttackJudgementTag：我这一刀具有什么判定，可以突破对方什么体状态？
Impacts：突破后具体施加什么效果？
ReservedPoiseDamage：为未来韧性系统预留多少削韧输入？
```

Body 与攻击判定不是同一个数据来源。普攻 GA 可以给自身 Normal，同时这一刀携带 Skill 判定；不能默认复制攻击者自己的 Body Tag 作为攻击判定，更不能把命中携带的判定 Tag 添加为目标的 Body 状态。

**用户已确认：同等级也允许打断并施加 Impact，因此比较规则为 `AttackRank >= TargetBodyRank`。** 低于目标 Body 才不通过。本文不再使用上一轮暂议的严格大于规则。

本版取消外层 ReactionIntent 和第二套通用控制强度。位移、期望控制时长等攻击参数归入 Impact；受击动画资源和恢复规则归入目标的受击 Profile，执行器按 Impact 类型选择目标资源。四级判定不依赖扣血量，也不依赖尚未实现的韧性槽。

**当前实现：无 Body Tag 解析为内部等级 0，不新增第五个 Tag。** 此默认值按本设计建议实施；四种合法判定均能作用于无体目标，与同级可通过规则下回退 Normal 的通过集合相同。

## 2. 实施前代码基础与缺口（历史基线）

本节记录实施前的源码及 Config 检查，不覆盖本次新增代码；当前实施边界见第 16 节。检查时未读取二进制蓝图内部逻辑，也未执行构建或运行验证。

- [HodgeGameplayAbility_Melee](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp) 已有服务器命中处理、目标 ASC 归并、执行身份、去重、Spec 创建／应用，以及包含 HitResult 的 EffectContext。
- [FHodgeHitEffectConfig](../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h) 已有 DamageEffect、DamageMultiplier、DamageType 和命中规则，尚无本设计的攻击判定、Impact 或削韧预留字段。
- [HodgeCombatAnimNotifies](../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h) 已有状态区间通知；[Definition GA](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp) 按通知 occurrence 持有 Tag 计数，并在窗口结束／执行结束时释放。
- Definition GA 默认 LocalPredicted、Exclusive_Replaceable，蒙太奇中断会结束执行，Melee 结束时关闭检测 Task。新受击不能直接复制其攻击事件及互斥契约。
- [HodgeDamageExecution](../../Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeDamageExecution.cpp) 计算生命伤害，[HodgeHealthSet](../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp) 消费 Damage、处理免疫和生命耗尽；没有韧性属性。
- [HealthComponent](../../Source/Hodgepodge/Private/Component/HodgeHealthComponent.cpp) 的 HealthChanged 不提供完整命中载荷；死亡入口会发送死亡 GameplayEvent。不能用“生命减少”替代四级判定。
- [ASC](../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp) 使用普通 GAS 当前蒙太奇状态并扩展 Definition 设置，不是多通道并行蒙太奇复制系统。
- [CharacterMovement](../../Source/Hodgepodge/Private/Component/HodgeCharacterMovementComponent.cpp) 已有最终 Yaw 约束，[PawnExtension](../../Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp) 有 ASC 初始化／解绑入口。
- [PawnData](../../Source/Hodgepodge/Public/Data/HodgePawnData.h) 已引用 AbilitySets 和 TagRelationshipMapping，没有 HitReactionProfile；[AbilitySet](../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h) 分别支持普通 GA 与攻击 Definition 授予，受击 GA 可以使用普通 GA 列表。
- [CombatCharacter](../../Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp) 已有 OnAbilitySystemInitialized／OnAbilitySystemUninitialized；玩家的 ASC 由 [PlayerState](../../Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp) 持有，经 HeroComponent／PawnExtension 接入当前 Pawn。
- [EnemyCharacter](../../Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp) 当前原生实现主要设置 AutoPossessAI，不能仅据此确认正式怪物已有 ASC、PawnData 或受击能力授予。敌人侧初始化需单独核实。

当前 Source／Config 未找到这四个 Body Tag 及完整的判定／Impact 系统。沿用现有命中和伤害链路，在单一业务 Runtime 模块内扩展，不新增模块，不要求通过尚不存在的编辑器字段完成配置。

## 3. 四级 Body 与攻击判定

### 3.1 用户定义的 Body 等级

- `State.Combat.Body.Normal`：普攻体，等级 1。
- `State.Combat.Body.Skill`：技能体／硬体，等级 2。
- `State.Combat.Body.SuperArmor`：霸体，等级 3。
- `State.Combat.Body.Vajra`：金刚体，等级 4。

比较顺序固定为 `Vajra > SuperArmor > Skill > Normal`。GameplayTag 自身没有数值大小或兄弟节点强弱关系，需在共享解析器中显式映射；不能比较名字、注册顺序或容器遍历顺序。

### 3.2 攻击判定使用独立载荷

允许命中载荷使用上述四个 Tag，也允许独立命名空间。建议作者界面使用拟议的 `Combat.Attack.Judgement.Normal / Skill / SuperArmor / Vajra`，与 Body 分别映射到同样的 1～4。

此独立命名空间是建议，尚未注册；两类映射集中维护。合法叶子 Tag 精确解析，父 Tag、未知子 Tag 或缺失攻击判定视为配置错误，不默认提升等级。缺失判定可以继续结算合法伤害，但不执行强 Impact，并输出可定位的诊断。

例如一段普攻：

```text
GA 给自身：State.Combat.Body.Normal（1）
这一刀携带：Combat.Attack.Judgement.Skill（2）
目标当前：State.Combat.Body.Skill（2）
比较：2 >= 2，通过
执行这一刀配置的 Impacts
```

攻击者的 Normal 只影响别人攻击它时的判断。目标若为 SuperArmor（3），同一刀 Skill（2）只能产生轻反馈，不执行击退／挑飞 Impact；生命伤害仍按现有伤害规则独立结算。

### 3.3 已确认的比较规则与推论

- Normal 可以突破 Normal。
- Skill 可以突破 Normal 和 Skill。
- SuperArmor 可以突破 Normal、Skill 和 SuperArmor。
- Vajra 可以突破全部四种 Body，包括 Vajra。
- 无体若采用内部等级 0，四种合法判定都能突破。

在此规则下，金刚体抵抗前三种判定，但并非绝对不可打断；金刚判定可以突破金刚体。这与用户确认的同等级可通过一致，不添加隐藏的金刚免疫例外。

无 Body Tag 建议使用内部 `NoBody=0`，不新增第五个 GameplayTag。若用户选择回退 Normal，则仅改变无标签目标的默认防御语义；目前最低合法判定为 Normal，同样可以通过。待用户确认后固定一个回退值，不同时保留两套默认规则。

### 3.4 多来源状态与命中快照

同时存在多个合法 Body Tag 时取最高有效等级。只移除自身持有的计数；霸体窗口结束时恢复剩余的 Skill／Normal，不能把其他 GA 或 GE 的状态清空。

同一作用域同一时刻建议只声明一种 Body，跨作用域可以叠加。例如 GA 全程给 Normal，特殊通知窗口给 SuperArmor，窗口内有效值为 3，结束后回到 1。同级多份也必须最后一个持有者释放才消失。

在本次命中进入效果回调、取消目标 GA 之前捕获目标 Body 快照。否则取消 GA 清掉 Skill 后再比较，会错误地把它当成无体。普通同帧多次命中按服务器实际处理顺序分别快照；不承诺所有攻击共享一个帧起始状态。

## 4. Body Tag 的赋予与生命周期

独立 GA 自身的 Body 可用 GAS 激活期间 Owned Tag 表达；动作中特定区间可复用现有状态通知。整个动作恒定 Body 不要求为此新增专用通知类。

本项目多个 Definition 可以共用同一个 Melee GA，因此建议新增 Definition.ExecutionBodyTag，作为共用 GA 的逐动作 Body 配置。Definition GA 在 Commit 成功后、执行准备和 Montage 开始前按当前 ExecutionId 获取 Body 作用域，并在 EndAbility 释放；动作中更高的 Body 窗口仍使用已有状态通知。

同一动作使用 Definition Body 或 GA Owned Body 二者之一，不能重复授予同一作用域。现有状态通知只表达动作内窗口，不能当作整段 Definition Body 的第二个默认来源。ExecutionBodyTag 与攻击判定独立，不在 BeginNotifyHit 中互相推导。

服务器根据实际活跃能力／窗口解析 Body；拥有者可本地预测相同状态，但不能向服务器声明自己临时变成金刚体。普通 Loose Tag 不被假设为自动复制；观察者需要的 Body 展示可以复制解析值，权威比较始终读取服务器有效持有关系。

进入强受击不会自动获得 SuperArmor。受击期间若没有其他 Body 来源，按基础／反应 Body 配置解析；首版建议默认无额外 Body，以便连击。需要受身或保护窗口时显式赋予状态，不能悄悄提高防御等级。

结束、取消、连段切换、死亡、ASC 解绑和 Pawn 更换均释放各自状态。玩家 ASC Owner 保持 PlayerState，Body 持有作用域绑定当前 Pawn Avatar，旧 Pawn 的标签与回调不得继续控制新 Pawn。

## 5. 命中配置：最小外层与覆盖

建议新增拟议 `FHodgeHitReactionConfig`，由现有 `FHodgeHitEffectConfig` 嵌入为 Reaction 字段；现有 DamageEffect、伤害倍率与命中去重保持职责。所有字段均为未来新增，当前源码没有这些接口。

外层核心字段为 AttackJudgementTag、Impacts、ReservedPoiseDamage。再补充可选 HitAcceptancePolicy，区分普通伤害命中与明确允许的零伤害控制命中，以及免疫／拒绝和“接受但生命伤害为零”。这不是新的强度等级。

轻反馈动画与默认声音／特效放在目标 HitReactionProfile。攻击方需要特殊命中特效时可以带纯表现标识，但不在命中配置中保存某一种怪物的动画资源。

期望控制时长、方向、距离、速度和期望落地结果放在 Impact 中；目标的默认控制时长、恢复上限、倒地保持、起身许可和动画映射放在目标 Profile，不在攻击请求外层重复配置。

Definition.DefaultHitConfig.Reaction 提供默认整份配置，命中通知建议新增 Hit.bUseDefaultReaction 和 Hit.ReactionOverride，允许显式整份覆盖。它们独立于现有 bUseDefaultDamage，避免只改终结刀 Impact 时被迫复制伤害配置。

BeginNotifyHit 在解析现有伤害配置后，单独解析本刀 Reaction 并写入运行 Effect 快照。通知 Damage 内可能随结构嵌入而出现的 Reaction 不作为第三个作者入口，避免默认／Override／Damage 三处相互冲突。默认继承不得从攻击者当前 Body 推导攻击判定。

Impacts 为空表示本次没有强效果；即使等级通过也不自动取消 GA 或产生隐式硬直。希望“通过即普通硬直”的攻击，应显式配置一个 HitStun Impact，或者引用带该 Impact 的默认配置。此为空配置的补充建议，非用户已确认规则。

ReservedPoiseDamage 仅为有限、非负数值。当前可随命中记录／诊断传递，但不创建 Attribute、不扣韧性、不触发破韧、不降低 Body、不绕过比较。未来如何消费、是否改变 Body 与破韧后的控制规则另立设计。

### 5.1 攻击方配置什么

- 攻击 GA 蓝图或 Definition：自身 Body；共用 GA 优先用拟议 ExecutionBodyTag。
- Definition.DefaultHitConfig.Reaction：默认攻击判定、Impact 列表和预留削韧值。
- Montage 的 Hodge 命中通知：保留现有几何／命中组，可通过拟议独立反应覆盖改变某一刀。
- Impact：希望对目标施加的效果类型、控制时长请求、方向／运动参数及落地结果，不引用具体怪物蒙太奇。

例如玩家普攻自身 Normal，命中判定 Skill，Impact 为 Launch；它不包含人形怪或四足怪的腾空动画。

### 5.2 目标方配置什么

建议新增 `UHodgeHitReactionProfile`，并在 PawnData 新增 HitReactionProfile 引用。每种角色／骨骼可以有自己的 Profile，多个使用相同骨骼和规则的角色可共享。此数据资产和字段尚未创建。

Profile 保存：

- 支持的 Impact 类型，及不支持时的显式回退。
- LightFeedback、HitStun、Knockback、Launch、AirHit、Landing、Knockdown、GetUp 的动画映射；可按方向或姿态变体选择。
- 对应目标骨骼和 AnimGraph 的 Slot／Additive 配置要求，默认表现资源。
- 默认硬直时长与限制、倒地保持时长、是否允许自动起身、起身结束条件、空中／落地恢复规则。
- 已在空中／倒地时如何处理重复 Impact，姿势重播与控制刷新策略，浮空／托举上限。

攻击方的期望值与目标默认值分开。请求未填写可选时长时采用目标默认；目标允许限制时长时显式配置限幅。不得用动画总长度暗中决定全部玩法控制时间。

Profile 不重复授予能力。目标的 AbilitySet 普通 GA 列表配置共享 HitReaction GA，可用统一原生逻辑配通用蓝图；不同怪物的动画从各自 Profile 查找，不复制每一种攻击的受击 GA。

### 5.3 目标方谁执行

建议 `UHodgeHitReactionComponent` 负责等级快照、裁决、Profile 查询和当前计划管理；`UHodgeGameplayAbility_HitReaction` 负责强受击的控制生命周期及动画／运动 Task。

轻反馈由组件的表现入口／GameplayCue 触发，不启动会抢占主攻击蒙太奇的强受击 GA。强受击由目标自己的 ASC 启动目标自己的受击 GA，玩家攻击 GA 不直接对怪物调用 PlayMontage。

同一个 Launch Impact 命中人形怪和四足怪时：参数可以相同，目标 Profile 选出的腾空、空中循环、落地动画不同。玩家攻击资产不需要知道两种骨骼。

## 6. Impact 的定义与组合

### 6.1 Impact 不只是物理冲量

这里的 Impact 是一个“通过判定后施加到目标的受击效果”。请求指定效果类型和参数，目标执行器据此控制行为、选择目标动画并执行运动。若仅定义一个 FVector 冲量，无法完整表达击倒、起身或落地后恢复；这不意味着玩家攻击 GA 需要持有怪物的受击动画。

建议先采用一组受限的类型化配置及共享执行器，必要时支持配置资产复用；不要求为每一刀新增 C++ 类，也不为首版引入任意脚本调度框架。

首版可支持的效果语义：

- HitStun：取消目标可打断动作，持有控制限制，播放方向性受击，按时限恢复。
- Knockback：附带地面位移、控制和受击姿势，配置方向、距离／时长及结束速度。
- Launch：取消动作、空中受控、抛飞轨迹／姿势，配置初速度或运动曲线、下落和落地结果。
- Knockdown：进入倒地及起身流程；可以是当前施加或另一个 Impact 的落地子阶段。
- AirHit：空中受击姿势、有限托举／下落速度修正以及控制时限更新。
- Slam：终止托举、向下运动、实际撞地后进入对应姿势／控制阶段。

这些是可选 Impact 配置，不再是外层必选的 ReactionIntent。轻反馈不属于这组强 Impact，否则会绕过“判定失败不施加强效果”的边界。

### 6.2 每个 Impact 的必要内部参数

- 适用条件：地面／空中／倒地及目标能力支持。
- 是否需要打断、取消哪些可打断能力、持有哪些控制限制。
- 效果类型／可选表现键，供目标 Profile 选择自己的姿势；失败回退必须明确。
- 时间或实际状态结束条件，以及超时／取消清理策略。
- 若含运动：方向来源、速度或距离／时长、主运动方式、碰撞和结束速度。
- 若含阶段：期望的落地／倒地结果；具体动画与起身流程由目标 Profile 和执行器提供。
- 对重复命中的请求语义；目标执行规则决定刷新／替换／忽略及限幅，避免多段命中反复卡第一帧。

不是每项 Impact 都暴露所有参数。地面击退只展示其运动参数；纯 HitStun 不要求填写挑飞速度。公共控制／姿势／运动可以在配置内部复用。

### 6.3 先构建执行计划，再提交

等级通过后，对 Impacts 做适用性与组合校验，构建本次执行计划。不要按数组逐项立刻取消／移动目标，再发现后面的配置冲突。

同一阶段最多一个主胶囊运动来源，最多一个主要受击姿势和一份合并控制生命周期。HitStun + Launch 可以组合；两个互斥的主运动不能同时争夺位置。Launch 落地后 Knockdown 属于阶段衔接，不是起飞瞬间并行倒地。

多个 Impact 需要打断时只取消一次。冲突配置在资产校验时报错，不用任意数组顺序决定赢家。重复命中可更新／替换当前计划，清理旧运动后再取得新运动资源。

目标能力支持是等级通过后的执行限制。例如不可腾空的大体型目标可以明确拒绝 Launch，或使用该 Impact 显式配置的 HitStun 回退。回退仍需本次等级已经通过，绝不能让低判定通过“回退硬直”绕过 Body。

没有可执行 Impact 时不取消目标 GA，只产生允许的轻反馈。缺动画不是隐式等级提升；默认姿势／回退资源应在实施前校验。

## 7. 服务器命中处理顺序

```text
已有检测、目标归并与命中去重
  → 固定本刀配置与目标当前 Body 快照
  → 解析攻击判定与目标 Body 的等级
  → 应用合法伤害，确认命中接受／免疫／死亡结果
  → 活着且命中接受：执行 AttackRank >= TargetBodyRank
      未通过：轻反馈，不取消 GA，不施加强 Impact
      通过：校验并提交 Impact 计划
  → 建立控制限制，取消相关动作，施加姿势与运动
  → 按阶段／恢复／取消／死亡清理本次资源
```

等级比较不决定扣血。金刚体受到低等级攻击仍可以正常受伤，除非另有伤害免疫；“未打断”不能直接退掉伤害 GE。

GE 提交成功不等于最终接受命中。现有 HealthSet 可能拒绝 Damage，且没有完整结果回传；实施必须补齐结算结果／等价接受契约，不能在 ApplyMeleeHitEffects 之后无条件 Launch。

首版建议普通伤害命中被伤害免疫拒绝时不执行强 Impact；独立零伤害控制须由 HitAcceptancePolicy 明确允许，仍需正常等级比较。生命伤害恰为零但命中被接受与“被免疫拒绝”分别记录，不只判断 Health 差值。

死亡优先接管。致死后的普通 Impact 不再启动；需要死亡抛飞时使用死亡流程的独立配置。结算作用域支持回调重入和 Avatar 重新验证，不跨帧持有回调中的 FGameplayEffectSpec 指针。

源去重继续沿用现有命中机会记录，目标消费以服务器命中／Impact 身份防重。目标 Body 快照与比较结果不会在目标 GA 被取消后重新计算，以免同一次命中先轻反馈又强受击。

## 8. 执行职责与 GAS 契约

建议目标侧新增拟议 `UHodgeHitReactionComponent`，协调 Body 解析、命中结果、Impact 计划、身份和复制；不查询新的武器碰撞，不成为第二套伤害／连段管理器。

共享执行器或受击 GA 负责计划生命周期。首版建议继承 UHodgeGameplayAbility，服务器发起、Independent 组，避免被目标现有 Exclusive_Blocking 的入口阻止；只有已通过等级与接受校验的计划才能启动。

目标受击 GA 不继承攻击专用 HodgeGameplayAbility_Definition，不加入 GrantedAbilityDefinitions，也不触发 GameplayEvent.Attack.Completed／Interrupted。否则会受攻击 Definition 的配置校验、连段授权和事件语义影响；受击自己的事件另行定义。

目标同一时刻由一份受控计划管理强受击，重复命中更新／替换该计划。Independent 不代表绕过 Body，也不代表多份 GA 可以同时控制同一胶囊。

参与此规则的攻击 GA 必须允许按约定取消。若把 Skill／SuperArmor 攻击设为永久不可取消，等级通过仍可能取消失败；这是契约错误，不应再引入隐式第二套免打断规则。取消失败不能宣称打断成功，也不能强行靠播放蒙太奇伪装结果。

死亡／阶段特殊技能另有明确终止契约，不通过四级普通命中控制它们。受击执行自身保持可取消，不带 SurvivesDeath，死亡能清理它。

进入计划时先建立所需控制限制，再走现有 Cancel／EndAbility，阻止中断事件、连段缓存或 AI 同帧重新启动攻击。等级快照在此之前已固定。根据 Impact 定义取消相关能力，不取消全部被动、UI 等无关能力。

控制 Tag 必须由能力条件、TagRelationshipMapping、移动输入和 AI 实际消费。Falling 与空中受控分开表达；AI 的自主请求暂停与受击运动分别处理，不能用 DisableMovement 把挑飞运动也一起禁止。

清理只移除计划持有的 Tag、Task、运动源、朝向请求与委托。状态／动画长度解耦，时限、实际落地和起身转换按各 Impact 结束条件推进。

### 8.1 接入文件和最小新增类型

以下为设计接入位置，本次已创建对应核心类型；实际字段和当前支持范围见配置手册与第 16 节：

- Public／Private/Combat/HodgeHitReactionTypes：集中定义四级解析、请求、Impact 参数、结算结果和计划等值类型，避免在各 GA 复制比较逻辑。
- Public／Private/Data/HodgeHitReactionProfile：目标动画和受击／恢复配置。
- Public／Private/Component/HodgeHitReactionComponent：目标侧协调、ASC 生命周期、Profile、结果与状态同步。
- Public／Private/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction：目标强受击的共享执行 GA。

不为每一刀新增类；首版 Impact 使用受限类型化数据。每个项目自有类的成员实现集中在其主 cpp，Editor 校验在主文件 WITH_EDITOR 内实现，沿用 Public／Private 目录与 UE_INLINE_GENERATED_CPP_BY_NAME，不新增业务 Runtime 模块。

### 8.2 启动与授予：沿用 PawnData／AbilitySet

建议将 HitReactionComponent 作为 AHodgeCombatCharacter 的默认子对象，与现有 Health／Rotation 一样在创建 ASC 回调前创建。受击是所有战斗角色的基础能力；不同时通过 Experience 注入同一个组件。Experience 继续选择 PawnData 并注入现有 Combat／Equipment，不复制或替换它们。

在 CombatCharacter.OnAbilitySystemInitialized 绑定反应组件，读取 PawnExtension 当前 PawnData 的拟议 Profile，校验 ASC.Avatar 与本 Pawn 一致。在 OnAbilitySystemUninitialized 解绑并清理；反应资源应在旧 ASC／Avatar 尚可访问时释放。

玩家继续使用 PlayerState.ASC；PawnData.AbilitySets 在现有授予流程中把 HitReaction GA 放进 GrantedGameplayAbilities，InputTag 留空，触发来自服务器结果。共用攻击 GA 仍在 GrantedAbilityDefinitions，不能混入受击 GA。

受击组件不自行重复 GiveAbility，不移除其他来源授予的技能。若 AbilitySet 已授予长期存在的 PlayerState 能力，换 Pawn 时更新其 Avatar／Profile 和结束旧执行，不擅自清掉 PlayerState 的长期能力；临时 GameFeature 授予按原有句柄生命周期移除。

敌人侧实施前单独确认：ASC 的实际 Owner、Avatar 初始化、PawnData 来源、AbilitySet 授予及 HealthSet。当前 EnemyCharacter 原生代码不提供完整证明。如果怪物蓝图已接通这些入口就沿用；若未接通，需先补齐，不能仅加 Profile 就宣称受击可用，也不强制将所有 AI 改成玩家 PlayerState 所有权。

初始化依赖可能不同步到位。组件先绑定 ASC，在 PawnData／Profile 和受击 GA 就绪后激活功能；重复尝试幂等，缺配置输出诊断。未就绪时不阻断已有合法伤害，也不能误报强受击成功。

### 8.3 每刀请求：改在已有通知和命中快照入口

在 HodgeGameplayAbility_Melee.BeginNotifyHit 中解析拟议的默认／覆盖 Reaction，保存到已有 FHodgeMeleeHitState.Binding。持续／单次通知继续沿用已有 occurrence、HitGroup、AttackPhase 和去重，不新增碰撞查询，也不在共享 Notify 对象上保存运行目标。

ProcessMeleeHitResults 保留目标 ASC／Avatar、阵营、执行身份和重复命中检查。取得目标当前反应组件后，ApplyMeleeHitEffects 的共同提交入口在实际 GE 应用前建立一份目标结算作用域，固定本刀请求、来源、HitResult 和 Body 快照。

BuildMeleeHitSpec 继续构建已有伤害 GE、标签及 SetByCaller。攻击判定不复用 DamageType，不添加为目标拥有状态，不根据 Source ASC 当前 Body 自动推导。普通攻击仍必须有现行配置要求的 DamageEffect，不能将 Impact 当作缺失伤害 GE 的隐式替代。

蓝图派生的效果处理必须通过共同提交入口，避免覆盖 ApplyMeleeHitEffects 后绕过结果、快照和去重。实施可将不可绕过的原生结算包装与允许扩展的实际效果 Hook 分开，但不拆同类 cpp。

### 8.4 伤害结果：补齐真正接受／拒绝的回传

建议为 FHodgeGameplayEffectContext 增加服务器本地的命中结算身份，以便匹配本次提交作用域；请求与 Body 快照由目标组件作用域持有，不把所有 Impact 数组和怪物动画塞入 Context。

HealthSet.PreGameplayEffectExecute 拒绝普通 Damage 时，将免疫／拒绝结果标记给对应作用域；PostGameplayEffectExecute 消费 Damage 后记录实际伤害和最终生命／死亡结果。对没有 Damage 输出的 Execution，作用域关闭时给出 NoDamageOutput／不接受普通强控制的明确结果，不假设 Post 一定执行。

HodgeDamageExecution 继续只计算数值，不做等级裁决、播放动画或移动角色。HealthChanged 保持 UI／资源变化职责，不挂通用受击逻辑。HealthComponent 继续原有死亡入口；伤害作用域结束后若已死亡，不再提交普通 Impact。

首版普通命中按接受的 Damage 路径触发，零伤害独立控制是显式扩展，不能仅把 DamageMultiplier 设为 0 就宣称现有路径支持它。回传允许同一作用域内多个属性回调，结果只提交一次；不使用一个全局 LastHit 缓存，也不跨帧保存 Spec 指针。

现有 Context.NetSerialize 与 Iris 转发并不自动覆盖新增自定义字段。若结算身份只用于服务器同步调用，不依赖复制它；解析后的受击计划走组件／能力同步。将来确实需要网络传输新 Context 字段时，必须同时处理普通序列化、Duplicate 和 Iris 方案，并验证实际复制配置。

### 8.5 目标执行：提交一份计划，不在攻击 GA 播怪物动画

服务器关闭结算作用域后，目标反应组件依次执行等级比较、Impact 适用性和资源校验，从当前 Profile 解析动画与恢复规则，再提交计划。计划是运行快照，不能修改共享 Impact／Profile 资产。

首次强受击激活目标的 HitReaction GA；已经存在强受击时由组件更新／替换当前计划，不能依靠反复 TryActivate 同一个 InstancedPerActor GA 来重入。更新带新计划序号，只清理旧计划持有的资源。

计划先取得控制限制，取消受影响攻击，经已有 Definition.EndAbility 和 Melee.OnExecutionEnding 关闭命中／状态／武器资源，然后受击 GA 使用目标自己的 ASC 播放 Profile 选出的主蒙太奇，并启动必要运动 Task。

取消失败或受击 GA 缺失／激活失败时，不标记打断或 Launch 成功，释放新计划已取得的限制并报告原因。接管旧受击计划时须保留连续控制，不能在资源交接中开放一帧输入。

动画循环和落地姿势可以由目标蒙太奇 Section 或目标 AnimBP 实现。首版按目标 Profile 选择一种明确方式；阶段由服务器受击逻辑推进，AnimBP 只消费只读状态，动画线程不访问可变 ASC／组件。

### 8.6 输入、连段、AI 与朝向接管

拟议受控状态应阻止普通攻击／技能／跳跃的激活，并让玩家 Input_Move 停止自主移动。现有 Input_Move 只检查 Status.Attack，不能因为受击 GA 结束了攻击就让移动输入重新驱动受控角色。需区分原始输入采集、自主行动许可与受击运动。

不要把受击 Tag 冒充 Status.Attack。扩展相应激活条件／关系映射及移动入口，AI 消费同一受控状态，暂停主动 MoveTo／攻击请求；真实击退或挑飞仍由 CharacterMovement 运行。

CombatComponent.ExecutionEnded 已有连段／输入清理，ExecutionEvent 又可能处理攻击中断。应在取消前建立受控阻塞，并复查 Interrupted 转移与缓存清理，防止受击期间旧输入启动下一刀；恢复时不无条件重放之前的攻击请求。

攻击 EndAbility 释放自己的 Body、旋转锁和吸附资源，受击接管目标朝向。受击不能通过缓存并恢复几个旋转布尔值覆盖其他持有者，不能绕开现有最终 Yaw 约束直接旋转胶囊。

### 8.7 目标同步与迟到状态

强受击在服务器裁决后启动，目标拥有者与模拟代理消费确认结果。组件同步当前计划序号、类型／阶段、必要时间与运动参数；GA 激活和组件复制可能不同步到达，消费方等待匹配身份数据就绪再启动表现，旧序号丢弃。

普通 ASC 主蒙太奇复制用于一份主要受击动作。轻反馈使用独立 Cue／表现同步，不改主 AnimatingAbility。控制结束由服务器决定，不因某客户端蒙太奇播放完毕就提前解除玩法状态。

胶囊移动由 CharacterMovement 同步，服务器发起的玩家位移和拥有者校正／重播分别验证。新加入相关性的客户端重建当前受控／空中／倒地阶段，不依赖一次性的命中 Cue 恢复整个状态。

### 8.8 现有文件的后续改动清单

- HodgeAbilityDefinition.h/.cpp：新增逐动作 Body、默认命中反应配置校验，保持 Standalone／ComboCoordinated 原契约。
- HodgeHitDetection.h、HodgeCombatAnimNotifies.h/.cpp：加入 Reaction 配置与独立覆盖，检测请求仍只描述几何／来源。
- HodgeGameplayAbility_Definition.cpp：获取／释放 Execution Body 作用域，不新增受击专用攻击事件。
- HodgeGameplayAbility_Melee.cpp：通知快照、目标结算提交和确认结果，保留已有去重、SourceObject 与权限检查。
- HodgeGameplayEffectContext.h/.cpp、HodgeHealthSet.h/.cpp：服务器结算身份和接受／拒绝回传，不将控制逻辑放进 DamageExecution。
- HodgePawnData.h/.cpp、HodgeCombatCharacter.h/.cpp：目标 Profile 引用、反应组件和 ASC 生命周期。
- HodgeHeroComponent、能力条件／TagRelationshipMapping、实际 AI 入口：消费受控限制，保留输入取消及现有联机授权逻辑。
- HodgeGameplayTags、受影响目标 Profile／AbilitySet／AnimBP：注册新标签并配置目标资源；此前均属于计划内容。

公开类型与 Build.cs 可见性按实际使用检查；Runtime 不无条件依赖 Editor。敌人 ASC 初始化是否需要改动，先核实现有蓝图／生成入口，不能在本设计中视为已经完成。

## 9. 动画、位移和轻反馈

等级失败的轻反馈建议采用 Additive 局部抖动、音效和特效，不移动玩法胶囊、不停止主技能、不获得控制限制。局部骨骼偏移限制对持武器链的影响，避免明显改变主攻击真实检测轨迹。

若轻反馈使用蒙太奇，与主动作采用不同 Slot Group 和独立表现同步入口；只改 Slot 名称不能保证并行，普通 ASC 当前蒙太奇接口也不能被轻反馈覆盖。轻反馈不带根运动。

强 Impact 才使用主要受击姿势和运动。地面 Knockback 选择合适根运动或 Root Motion Source；简单 Launch 可先用 LaunchCharacter，精确轨迹接入 CharacterMovement。每阶段明确主运动来源，避免动画根运动与完整额外位移重复叠加。

倒地的典型目标流程为：Profile 的倒地动画 → 倒地保持 → 满足 Profile 的起身许可 → 播放该目标的起身动画 → 恢复自主行动。挑飞流程为：目标腾空姿势／实际向上运动 → 空中受控 → 实际落地 → Profile 的落地／倒地姿势 → 按规则恢复。攻击者只指定 Knockdown 或 Launch 及参数，不提供这些目标动画。

LaunchCharacter 会进入 Falling，不能当作所有贴地击退的默认实现。移动保留 Sweep 与真实碰撞，遇墙、地面和天花板按实际结果结束／转换，不用瞬移穿过阻挡来满足配置距离。

Impact 的方向策略可用攻击者到目标、攻击方向、武器方向或径向中心等；ImpactNormal 不直接等同于受力方向。已有 Yaw 锁也会限制根运动旋转，受击接管时必须清理旧攻击约束或申请明确朝向权限。

## 10. 空中连段与吸附衔接

挑飞来自等级通过后实际成立的 Launch Impact，而不是仅有攻击判定 Tag 或申请了挑飞。结果回传成功施加的 Impact 及其阶段，攻击方据此决定追击。

目标被挑飞与玩家升空是两份独立运动。范围攻击可以施加多个 Launch，但追击只从实际成功目标中选一个主目标，优先本刀辅助／锁定目标，再按稳定规则选择；见[吸附设计](melee-attack-assist.md)。

AirHit 同样需要本次等级通过。首版建议受击目标没有隐式高 Body，因此正常攻击可继续托举；特殊空中保护窗口显式赋予 Body。不能因目标已在空中就默认跳过等级比较。

AirHit 可限制下落速度或给有限向上补偿；限制高度、连续控制时间和托举次数，避免无条件累加速度。Slam 提交后移除旧托举运动，实际落地再执行撞地／Knockdown 子阶段。

落地使用真实地面接触，不能只等腾空动画播完。玩家是否自动追击、是否允许落空升空、空中连段分支由攻击技能设计，不由受击组件代为激活。

## 11. 预留削韧的边界

本版只预留 ReservedPoiseDamage，不实现韧性上限、恢复、破韧事件或破韧后保护。不能在说明中把预留值写成已经扣除了目标韧性。

未来韧性系统消费该值时需另行决定：是否在等级失败时也削韧、破韧是否改变 Body、如何与金刚体和控制免疫相处、属性初始化／重建与复制方式。没有明确新规则前，削韧不得改变本版等级比较。

## 12. 复制、生命周期与可观测结果

服务器裁决攻击判定、目标 Body 快照、命中接受、Impact 执行与取消。首版不预测目标权威强受击；拥有者攻击预测继续沿用现有契约，客户端不能授权自己任意提高判定。

建议结果包含服务器命中身份、源执行关联、当前 Avatar、攻击判定 Tag／等级、目标 Body 快照／等级、通过与否、接受／拒绝原因、实际执行的 Impacts、执行阶段与预留削韧值。请求 Launch 与 Launch 成功必须区分。

GameplayEvent 只是执行入口，不自动复制载荷。强计划使用明确状态和必要运动参数同步，轻反馈使用 Cue／表现通道；持续状态支持晚到相关性的客户端重建，模拟代理不自行重新比较后修改服务器位置。

胶囊运动沿用 CharacterMovement，必要参数和 SavedMove／运动源重播要明确接入，不新增每帧位置 RPC。普通 Loose Tag、EffectContext 新字段或使用引擎 Task 都不等于自动完成本项目复制。

Body 持有关系、Impact 句柄和目标引用绑定当前 Pawn。重复初始化幂等，解绑、取消、死亡和 Pawn 更换释放自身资源；UObject 引用考虑 GC，跨帧目标用弱引用验证，旧结果不能落到新 Avatar。

调试至少显示两个不同等级来源、取最高后的 Body、比较结论、Impact 跳过／执行原因、取消结果与运动阶段。不能只打印“播放受击成功”掩盖等级或取消失败。

## 13. 示例与需要确认的规则

- 普攻自身 Normal，判定 Skill，Impacts=[HitStun]：打中 Normal／Skill 动作可打断并硬直；打中 SuperArmor／Vajra 动作只轻反馈。
- 普攻自身 Normal，判定 SuperArmor，Impacts=[HitStun, Launch]：可突破 Skill／SuperArmor 并挑飞，自身仍可被 Normal 及以上判定打断。
- 技能自身 SuperArmor，判定 Skill，Impacts=[Knockback]：自身霸体不代表输出霸体判定；攻击目标 SuperArmor 时不能击退。
- Vajra 判定命中 Vajra：同级通过，可执行适用的 Impact；不是永远只轻反馈。
- 任意合法伤害、低判定打中 Vajra：照常按伤害规则扣血，只轻反馈；不施加已配置 Launch。
- 等级通过但 Impacts 为空：只结算伤害／表现，不隐式取消动作。
- 等级通过但目标不支持 Launch：执行配置明确的回退；没有回退则跳过，不报告成功挑飞。

同等级可通过已获用户确认。当前采用无 Body 内部 0、Impacts 为空无控制、冲突配置校验和不支持效果的显式回退；不注册第五种体状态。

### 13.1 未来制作示例：一招挑飞，两种怪物

以下是制作流程示意。相关原生字段已经创建，具体示例资产尚未自动制作；当前操作以配置手册为准，不能把示意路径视为现成资产。

1. 在现有正式攻击目录 `/Game/Main/Character/Hero/Ability/BasicAttack` 的相应 Definition 中，拟议 ExecutionBodyTag 配 Normal；拟议 DefaultHitConfig.Reaction 配 Skill 判定、Launch Impact 和预留削韧值。现有伤害 GE、倍率继续按配置手册。
2. 在该 Definition 引用的 Montage 保留已有 Hodge 命中通知；仅某一刀需要挑飞时，用拟议 bUseDefaultReaction=false 和 ReactionOverride 配这一刀，其他刀可以继承默认 HitStun。
3. 在拟议 `/Game/Main/Combat/HitReactions` 创建人形和四足两个目标 Profile，分别配置自己的轻反馈、硬直、腾空、空中姿势、落地、倒地与起身资源；该路径是建议的新资产目录，不代表已经存在。
4. 在各目标 PawnData 的拟议 HitReactionProfile 字段引用对应 Profile，并通过其 AbilitySet 普通 GA 列表授予同一个 HitReaction GA。不同骨骼的动画不填到玩家 Definition 或攻击 GA 中。
5. 核实各目标 ASC 的初始化／Avatar、HealthSet、碰撞对象类型及实际目标 AnimBP Slot 接法；不同怪物不能仅凭父类名称假定这些配置一致。

运行时，玩家对自身保持 Normal，命中携带 Skill 与 Launch 参数。目标当前 Skill 则同级通过，从目标自己的 Profile 选择腾空动作并执行挑飞；目标当前 SuperArmor 则只从自己的 Profile 播轻反馈。伤害仍按合法 GE 结算。

玩家的上升追击必须等到该目标的 Launch 实际成功结果，不能在动画通知进入时提前假定怪物已经腾空。人形和四足使用相同攻击参数但不同目标动画，不需要创建两份玩家攻击 GA。

## 14. 实施阶段与验收

### 阶段 A：四级比较与普通受击

- 建立集中等级映射、Body 作用域、命中配置与结果；实现 LightFeedback 和 HitStun Impact。
- 验收：四级 16 种组合按大于等于比较；输出判定与自身 Body 独立；无状态回退、多来源最高值和取消前快照正确；轻反馈不中断主技能。

### 阶段 B：Impact 计划与地面击退

- 加入组合校验、控制合并、Knockback 及重复命中更新。
- 验收：多 Impact 只取消一次；无合法效果不取消；取消失败被报告；互斥运动配置报错；墙体阻挡、结束速度和打断清理正确。

### 阶段 C：Launch、AirHit、Slam 与 Knockdown

- 加入空中运动、落地子阶段、倒地／恢复和限幅。
- 验收：各次空中命中仍比较等级；不支持挑飞不伪造成功；多次托举有界；撞墙／天花板、死亡和换 Pawn 能清理。

### 阶段 D：玩家追击

- 联合吸附设计接入成功 Launch 结果、主目标选择和玩家独立升空。
- 验收：多目标只追一个；等级失败、免疫／拒绝、Impact 跳过不触发成功追击；下砸或失效后能结束。

韧性系统不纳入上述阶段，本版仅验证预留值有限非负并原样传递，且不改变判定／目标属性。

实施后原生验证覆盖等级映射、Body 持有、命中快照、组合与清理；按[开发流程](../AI_DEVELOPMENT.md)执行 Editor／Game 常规构建，再分别验证蓝图、单人 PIE、双人 Listen Server、延迟校正和 Pawn 更换。Cook／打包独立报告。

本次核心实现已按该设计接入；玩法资产、PIE、联机、Cook／打包的实际验证范围单独报告，不将原生测试视为这些验收全部通过。

## 15. 参考

- [UE 5.5 Animation Slots](https://dev.epicgames.com/documentation/en-us/unreal-engine/animation-slots-in-unreal-engine?application_version=5.5)：Slot 与 Group 的作用及组内蒙太奇互斥。
- [旋转约束设计](character-rotation-policy.md)：已有组件职责与复制背景；旧 Timeline 内容为历史记录，当前窗口采用 Montage 通知。

## 16. 2026-10-08 实施边界

- 四级 Body／独立判定及大于等于比较已实现，Definition 执行 Body 与通知窗口按自身作用域清理。
- DefaultHitConfig.Reaction、bUseDefaultReaction、ReactionOverride 已接入原命中快照和共同效果提交入口，未配置 Reaction 的原技能保持原有伤害行为。
- HealthSet 接受／拒绝与 Context 的服务器本地结算身份已接入，保留伤害、免疫和死亡的独立责任。
- 目标 Profile、默认反应组件和 ServerInitiated／Independent 受击 GA 已实现，普通 AbilitySet 授予，不使用攻击 Definition／连段事件。
- HitStun、Knockback、Launch、Knockdown、AirHit、Slam 的计划、阶段、限制和清理已接入；地面击退使用 RootMotionSource，空中运动使用受限 Launch。首版受击蒙太奇要求原地动作，动画映射按 Type，方向强动画表未实现。
- 倒地姿势保持、起身、空中循环／落地和超时恢复由目标侧执行。连续控制／空中链限幅不代替完整的受身保护或防无限连机制。
- 输入、能力激活、AI 请求、旋转约束和 SavedMove 的受控快照已接入；移动校正和真实联机手感仍需运行验证。
- HodgeEnemyCharacter 增加由 EnemyPawnData 启用的 Pawn 所有权 ASC、基础属性和 AbilitySet 初始化，已有外部 ASC 入口保留。
- OnMeleeReactionResolved 和目标侧结果事件提供实际成功 Impact 的接口。玩家自动升空追击、空中连段资产和完整吸附系统仍属于攻击技能／吸附后续实施范围。
- 削韧仅预留非负值；没有实现韧性属性、恢复或破韧。没有自动修改正式普攻、怪物、动画、Experience 或插件资产。

当前字段和具体制作步骤见[受击配置手册](../Guides/hit-reaction-configuration.md)。
