# Hodgepodge 战斗模块详细代码评审

日期：2026-10-18。审查基线：`dev-Combat`，`HEAD 891fd70`，**包含审查时的未提交工作区内容**。引擎约束：UE 5.5.4。

文档性质：对前次战斗代码审查的整理、证据复核与深化；不是重构实施记录，也不是新的战斗功能设计。本次只新增评审文档及文档索引，没有修改 C++、蓝图、DataAsset、Montage 或运行配置，没有提交或切换分支。

后续实施状态单独记录在[评审整改与验证记录](../Validation/combat-review-refactor-2026-10-18.md)。下文保留审查时的证据与行号，不将历史问题描述当作修复后的现状，也不将部分切片通过视为全部路线图完成。

> **总体判断：当前战斗模块存在明显的局部过度复杂和遗留化，主要集中在动作事务、受击裁决及兼容配置。既有 GAS 权威链、Experience／PawnData 初始化框架、独立命中检测和角色收益规则分离仍然有价值。建议先修正确性和诊断缺口，再逐步收敛职责与兼容面，不进行整体推倒重写。**

## 阅读导航

1. 概述与背景：评审目标、需求演变、判断边界。
2. 审查范围与方法：证据类型、规模及未验证项。
3. 核心实现逻辑：动作、检测、伤害、受击、动画运动及状态归属。
4. 优点与亮点：应保留的结构和必要复杂度。
5. 潜在问题与改进建议：十二项问题，逐项说明证据、触发条件、风险与验收标准。
6. 综合影响：功能、性能、维护成本和核心痛点。
7. 分阶段路线图：优先级、输出、依赖及停止条件。
8. 回归测试与风险控制：关键场景和证据要求。
9. 历史结论校正、阅读索引与总结。

## 1. 概述与背景

### 1.1 评审目标

本次评审回答四个问题：

- 当前战斗模块为什么难以阅读，问题属于正常业务复杂度还是可以降低的结构复杂度？
- 哪些代码存在可定位的正确性缺口，哪些只是维护风险或待测性能风险？
- 哪些既有成果应该继续保留，避免把已经完成的重构再次列为待办？
- 如何在保留现有玩法、服务器权威和资产兼容的前提下，分批降低复杂度？

“难维护”不以单个文件超过某个行数为判断标准。评审重点是：同一规则是否重复表达、一次状态变化需要跨多少职责理解、失败是否可诊断、兼容逻辑是否有退出条件，以及修改后能否用明确场景证明行为保持一致。

### 1.2 需求演变与当前约定

战斗系统经历过通知驱动命中、动作请求与连段协调、检测服务独立化、角色收益规则分离、方向性受击、阶段运动、削韧累计和分段击飞等迭代。新旧方案叠加是当前复杂度的重要背景，但不能把所有历史方案都视为仍应执行的需求。

本轮按下列最新约定评价代码：

- 保留现有 GAS、Experience、PawnData、GameFeature 和 Init State 主链。
- 玩家 ASC 由 PlayerState 持有、Pawn 作为 Avatar；普通怪物保留自身 ASC 的所有权方式。
- 普通削韧累计只推进轻僵直、重僵直，不能再由旧破韧阈值自动产生击飞。
- 攻击分段 `FHodgeAnimHitConfig.bCanLaunch` 是击飞资格的唯一作者入口；运行请求中的同名字段只是瞬时快照。
- 已处于受击 Airborne 链时，可击飞分段允许重新进入 Fly；不可击飞分段不重启 Fly／Push，等待真实落地后倒地、起身。
- 击飞资格仍受伤害接受、Body、目标资源、运动条件和空中预算限制，不是无条件强制控制。
- 普通削韧路线的动画表现与胶囊运动分离，继续使用现有 GA、CharacterMovement 和 RootMotionSource，不叠加竞争运动源。
- 继续使用项目现有动作素材；跑攻、切手及新共振度系统不纳入本次评审建议的实施范围。

相关现行约定见[分段击飞设计](E:/Project/Git/Hodgepodge/Docs/Design/attack-segment-launch.md)、[削韧受击设计](E:/Project/Git/Hodgepodge/Docs/Design/poise-driven-hit-reactions.md)及[ACT 总设计](E:/Project/Git/Hodgepodge/Docs/Design/act-combat-animation-feedback-design.md)。发生冲突时，应按最新需求及当前源码分别确认，不把旧方案正文直接当作当前执行事实。

### 1.3 总体质量判断

| 维度 | 判断 | 主要依据 |
| --- | --- | --- |
| 核心架构 | 可以继续演进，具备可保留的基础 | 权威结算、ASC 所有权、数据配置和既有初始化链已建立 |
| 职责边界 | 部分边界有效，协调层仍过重 | 检测和收益已有独立宿主；动作确认、恢复和受击分支仍跨多类交织 |
| 正确性 | 存在应优先处理的条件性缺口 | 武器 Overlap 半径、同步伤害重入、多边连段等待、无关配置阻断 |
| 可读性 | 高风险路径阅读成本较高 | 需要同时追踪 Combat、ASC、Definition、受击组件及 GA 的时序 |
| 可维护性 | 局部遗留化，扩展成本上升 | 兼容字段与新入口并存，部分作者字段不再决定实际执行 |
| 性能 | 有重复工作和轮询热点，尚不能认定已有瓶颈 | 本次没有 Profiling，不提供未经测量的帧率或耗时结论 |
| 测试保障 | 已有较多测试，但关键边界仍有空缺 | 怪物假人覆盖较多；真实玩家作为受击目标等路径未被现有专项证明 |

不建议把整个模块笼统称为“全部不可维护”。更准确的表述是：**底层机制有明确价值，但高层协调和配置兼容的复杂度已经影响理解、定位和安全修改，需要主动收敛。**

## 2. 审查范围与方法

### 2.1 实际审查范围

- 工程描述、模块依赖及战斗相关 Source／Config。
- Combat、ASC、Definition、Melee、HitDetection、HitReaction、Hero、EnemyInit 等主要运行入口。
- 攻击 Notify、目标 Profile／PawnData、角色收益规则及旧字段适配器。
- 相关原生测试、ACT／受击验证工具及已有验证记录的覆盖边界。
- 本机 Lyra 对应的伤害、能力、初始化、输入和清理链路。

审查中的“当前代码”指工作区文件，而非仅 `HEAD` 中的已提交版本。工作区已有大量相关资产、代码、工具和文档修改，本次不对其作回退、覆盖、自动提交或归属推断。

### 2.2 证据与严重度分开记录

本文采用以下证据分类：

- **源码确认**：可从当前实现直接读出行为或职责；不等于已在游戏中复现症状。
- **条件性缺陷**：可以给出完整触发条件和错误路径，但本轮未证明正式资产当前一定触发。
- **结构债务**：当前可能正确运行，但修改范围、理解成本或误用概率明显偏高。
- **待测风险**：存在合理风险来源，需要运行采样或性能测量后判断影响。
- **验证缺口**：现有测试没有覆盖某条必要路径；不等于该路径已损坏。

优先级含义：P1 为应先于大范围重构处理的正确性问题；P2 为高价值的条件性问题、结构收敛及关键测试缺口；P3 为可随切片推进的整理事项。本文没有证据足以认定必须阻断所有开发的 P0 故障。

### 2.3 规模与阅读负担

以下为审查快照的物理行数，仅用来定位阅读热点，不代表圈复杂度、有效代码量或质量评分：

| 主要实现文件 | 物理行数 | 主要阅读负担 |
| --- | ---: | --- |
| `HodgeCombatComponentBase.cpp` | 1188 | 连段、授权、动作请求、移动取消、依赖协调 |
| `HodgeHitDetectionComponent.cpp` | 474 | 检测会话、来源、历史几何、采样与清理 |
| `HodgeHitReactionComponent.cpp` | 955 | 命中事务、削韧、裁决、控制、复制与表现 |
| `HodgeHeroComponent.cpp` | 1392 | 初始化、输入、角色长按、冲刺和电影会话 |
| `HodgeAbilitySystemComponent.cpp` | 1295 | GAS 扩展、输入、激活、预测和连段等待 |
| `HodgeGameplayAbility_Definition.cpp` | 880 | 动作执行、恢复、通知资源和清理 |
| `HodgeGameplayAbility_Melee.cpp` | 557 | 检测请求、伤害结算、受击与规则事件 |
| `HodgeGameplayAbility_HitReaction.cpp` | 262 | 受击执行、Montage、RMS／Launch 与退出 |
| `HodgeHitReactionProfile.cpp` | 285 | 动画解析、变体、阶段、曲线和校验 |

ASC 等部分源文件具有不同编码；统计按 BOM 感知方式读取，避免把 UTF-16 文件误当 UTF-8，产生错误行数或搜索结论。

### 2.4 本轮未执行事项

本轮没有重新执行 Editor／Game 构建、原生测试、蓝图编译、PIE、联机、Cook、打包、动画逐帧检查或性能采样；没有读取全部正式二进制资产的内部字段。

已有[分段击飞验证记录](E:/Project/Git/Hodgepodge/Docs/Validation/attack-segment-launch-2026-10-18.md)记载的 172 项原生测试、48 次 PIE 场景执行等，是先前工作的历史证据。本文引用其范围，不把它们描述为本次重新运行，也不把用例数当作分支覆盖率。

### 2.5 本次文档交付检查

本次实际执行了 `git status --porcelain=v1`、`git branch --show-current`、`git rev-parse --short HEAD`，并通过 PowerShell 检查本文本地链接目标、行号范围、必需章节、十二项问题、代码围栏、冲突标记和行尾空白。索引差异执行 `git diff --check -- Docs/README.md`，未报告格式错误；新增文档另作内容与空白检查。

索引只在已有工作区内容上新增一条评审入口；移除本次新增条目后的 SHA-256 与任务开始时一致。全工作区 `git diff --check` 曾因既有资产触发 Git LFS 的 `.git/lfs/tmp` 写权限限制而未完成，随后将检查限定为文档；不将该环境限制解释为资产损坏或战斗构建失败。

## 3. 核心实现逻辑分析

### 3.1 主调用链

```text
玩家输入／AI 请求
  → Combat：连段选择、请求、授权与取消协调
  → ASC：GAS 激活、预测确认及服务器等待适配
  → Definition GA：动作执行、Montage、通知及资源生命周期
  → 攻击 Notify：命中来源、窗口、HitGroup、AttackPhase、分段击飞资格
  → HitDetection：几何采集、Sweep／Overlap、过滤与结果批次
  → Melee：命中去重、GE 提交、实际伤害确认及结算事件
  → DamageExecution／HealthSet：服务器计算、生命夹取、接受结果
  → HitReactionComponent：目标侧裁决与已解析受击计划
  → HitReaction GA／CMC：动画和权威运动执行
  → 角色规则：按已确认结算和执行身份处理对应收益
```

这是一条顺序说明，不代表全部调用都是单向异步。当前存在同步委托、能力取消和回调重入，必须结合执行身份和清理路径理解。

### 3.2 状态事实来源与执行归属

| 状态／数据 | 配置或事实来源 | 主要协调／修改入口 | 最终执行者与边界 |
| --- | --- | --- | --- |
| 玩家能力与持久属性 | PlayerState ASC、PawnData、授予来源句柄 | PlayerState／PawnExtension／现有授予链 | 服务器授权，客户端按复制与预测使用 |
| 连段图、输入意图、窗口 | ComboDefinition、当前 GA 的通知窗口 | Combat，ASC 适配激活与确认 | Definition GA 执行动作 |
| 当前动作资源 | 当前 ExecutionId、Montage／Notify 实例 | Definition GA 的进入、取消和结束 | 武器、姿势、相机等现有资源宿主 |
| 命中几何与采样历史 | 检测请求、武器来源、当前骨骼／组件 | HitDetection 会话 | 查询策略只输出候选接触 |
| 实际生命损失 | 服务器 HealthSet 扣血并夹取后的差值 | Damage GE／HealthSet | HealthSet，不由表现或受击重算 |
| 削韧进度 | 目标 PawnData 规则、合法命中事务 | HitReactionComponent | 目标侧裁决，累计只到 Heavy |
| 击飞作者资格 | 当前攻击分段 `bCanLaunch` | Notify 解析为运行请求 | 服务器仍检查 Body、接受条件和目标支持 |
| 当前受击状态 | 已解析 `FHodgeHitReactionState` | HitReactionComponent | 受击 GA／CMC 执行动画与运动 |
| 暗流／共鸣收益 | 对应角色规则配置与结算身份 | AttackRule 及其宿主 | 对应 GAS 资源效果，不由公共 Melee 硬编码发放 |

目前的问题并非上述归属全部错误，而是部分协调阶段互相调用，导致准备、确认、失败和清理边界分散。

### 3.3 动作请求、连段与预测

Combat 通过当前节点、输入意图、窗口和来源标签选择转换边；ASC 在服务器激活入口中判断是否应等待通知窗口；Definition GA 再处理目标数据、恢复等待、朝向、Montage 和通知资源。

这三个角色具有合理分工，但一次动作事务需要跨三个类追踪。尤其在“先取消旧动作，再授权新动作”“客户端已预测、服务器尚未收到窗口”“目标数据晚到”“受击恢复期间等待”时，成功、失败及延迟确认的关系不够集中。

源码入口：[Combat 转换选择](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp:212)、[ASC 延迟连段请求](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp:959)、[Definition 动作执行](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp:98)。

Avatar、SpecHandle、PredictionKey 和 ExecutionId 各自保护不同边界。它们是必要复杂度，不能为了字段少而合成一个通用 ID，也不能删除服务器检查来降低阅读量。

### 3.4 检测来源、查询与结算

命中检测已独立于 Combat 的主要业务协调：会话维护来源、上一帧几何、查询条件、采样状态和姿势占用，几何策略负责采集及查询。Melee 接受结果后继续处理目标过滤、去重、GE 和结算身份。

这一边界总体正确。需要重点关注的契约是：**同一份来源在 Sweep 和 Overlap 中必须具有相同的有效几何语义。** 来源半径、形状和采样点不能只在某一种策略里有效，换查询模式后悄悄丢失。

持续命中与单次命中共享主要链路；单次命中未指定 Profile 时，会临时选择 `Overlap + OnceOnEnter`，因此不能只验证武器 Sweep 就认定全部武器命中路径可靠。

### 3.5 伤害接受、结算身份与角色收益

HealthSet 使用 Damage Meta Attribute 消费伤害，扣除并夹取 Health，再广播实际接受的生命损失。对剩余 10 生命、输入 100 伤害的情况，实际接受值是 10；该语义应继续保持，不能由受击或表现再计算一次。

Melee 通过命中事务和实际伤害结果构造类型化结算，再交给独立角色规则。当前 R3 已将 Havoc／共鸣收益从公共 Melee 直接执行中分离，这是有效成果。新旧规则适配器仍存在，但会优先使用同类新配置，不能将其误报为必然双倍回能。

需要改进的是同步回调期间的 Meta Attribute 消费顺序，以及作者配置中旧字段和新规则的可见优先级，详见 F02、F10。

### 3.6 目标侧受击裁决

当前命中事务大体按以下顺序处理：

1. 验证服务器、事务身份、Avatar 和请求合法性。
2. 排除死亡、明确拒绝、免疫及不满足伤害接受策略的命中。
3. 已在受击 Airborne 且当前分段不可击飞：保留现有空中执行，按资格标记接地后倒地。
4. 当前分段可击飞：进入独立 Launch／Fly 裁决，不再由累计破韧阈值决定。
5. 普通削韧输入：按目标窗口和已消费等级推进 Light／Heavy。
6. 其余显式／兼容请求：检查 Body、Impact、当前姿态、回退及目标支持，构建计划。
7. 提交计划，发布状态，由现有 GA 和移动组件执行。

源码入口：[FinishHit](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp:179)、[ResolveSegmentLaunch](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp:368)、[BuildPlan](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp:398)。

必须区分受击状态 `Phase == Airborne` 与 CMC `IsFalling()`。自然跳跃／下落并不必然已处于受击击飞链，两者目前会进入不同兼容分支。这是后续整理作者语义时必须保留并明确选择的边界。

### 3.7 动画、运动和恢复

Profile 将逻辑类型、方向、强度和阶段解析成 Montage、Loop、Landing、GetUp 及运动来源；运行状态保留已经解析的引用和运动计划，受击 GA 根据状态执行 Montage、RMS 或 Launch。

普通削韧路线要求动画和外部运动遵守既定互斥契约；兼容／显式路线仍保留部分旧动画根运动选择。不能把所有路径描述成已经全面统一，也不能为了清理配置直接删除运行状态中的已解析字段。

地面 Knockback 已有曲线积分归一化及峰值限制，不是完全未经处理的匀速推移。动画与胶囊的过程是否自然、网络中是否存在瞬时修正，仍需运行观察，不能仅凭终点收敛或编译通过判断。

### 3.8 本机 Lyra 对照及复用边界

参考根目录：`E:/Project/UProject/study/Epic/LyraStarterGame`。以下为本轮审查采用的具体对照，不将“同名类存在”视为完整核查。

| 参考实现 | 对照内容 | 对 Hodge 的结论与适配边界 |
| --- | --- | --- |
| [LyraHealthSet::PostGameplayEffectExecute](E:/Project/UProject/study/Epic/LyraStarterGame/Source/LyraGame/AbilitySystem/Attributes/LyraHealthSet.cpp:148) | Damage 转 Health、Meta 清零、HealthChanged／OutOfHealth | 参考先消费 Meta 再发后续生命观察回调的顺序；Lyra 更早的 VerbMessage 仍在清零前，不能概括为其全部通知都安全重入 |
| [LyraDamageExecution](E:/Project/UProject/study/Epic/LyraStarterGame/Source/LyraGame/AbilitySystem/Executions/LyraDamageExecution.cpp:36) | 服务器计算、Source／Target、EffectContext | 继续复用已迁入的权威结算边界，不另造伤害流程 |
| [LyraAbilitySystemComponent](E:/Project/UProject/study/Epic/LyraStarterGame/Source/LyraGame/AbilitySystem/LyraAbilitySystemComponent.cpp) | 输入、能力组、取消和 GAS 生命周期 | 参考成熟边界；本地 ACT 连段等待是 Hodge 扩展，不能直接用 Lyra 输入循环替换 |
| [LyraGameplayAbility 标签要求](E:/Project/UProject/study/Epic/LyraStarterGame/Source/LyraGame/AbilitySystem/Abilities/LyraGameplayAbility.cpp:316) | 标签关系扩展、临时容器 | Hodge 相似实现并非新发现的“擅改 GAS 核心”证据；若调整重入性，应单独证明触发路径 |
| [LyraPawnExtension 初始化／解绑](E:/Project/UProject/study/Epic/LyraStarterGame/Source/LyraGame/Character/LyraPawnExtensionComponent.cpp:105) | ASC Owner／Avatar、旧 Avatar、取消与解绑 | 继续遵守生命周期契约；玩家和敌人所有权差异保持现状，不给普通敌人强加 PlayerState |
| [LyraHeroComponent 输入](E:/Project/UProject/study/Epic/LyraStarterGame/Source/LyraGame/Character/LyraHeroComponent.cpp:225) | 通用输入和相机桥接 | 为收窄 Hero 提供职责参照，不要求照搬项目不采用的上层框架 |
| [LyraAbilitySet](E:/Project/UProject/study/Epic/LyraStarterGame/Source/LyraGame/AbilitySystem/LyraAbilitySet.cpp:73) | 授予、句柄及撤销 | 继续复用 Hodge 已加强的受检与分来源授予，不重复建设 |
| [Lyra 远程武器目标数据](E:/Project/UProject/study/Epic/LyraStarterGame/Source/LyraGame/Weapons/LyraGameplayAbility_RangedWeapon.cpp:477) | TargetData、预测身份、回调解绑 | 可参考身份和清理，不把子弹追踪整体搬入近战几何与削韧系统 |

本机 Lyra 没有查到可以直接替换 Hodge 整条 ACT 连段、削韧、分段续飞和阶段恢复链的完整实现。因此复用重点是已经成熟的权限、数据、生命周期和资源归属，不引入新模块、引擎版本或平行战斗框架。

## 4. 优点与亮点

### 4.1 核心权威链没有失去统一入口

伤害仍由 GE／DamageExecution／HealthSet 处理，受击使用实际接受结果；客户端表现不直接写生命。受击组件和 GA 继续围绕 GAS 执行，而非各自生成一套伤害与控制逻辑。这为局部修复和回归提供了稳定边界。

### 4.2 R1 检测独立化已有实际成果

当前已经存在独立检测会话与几何策略，不能继续按早期文档描述成 Combat 仍包办全部几何采样。后续重点应是修复几何契约和收窄接口，不再重新建设一层同职责检测服务。

检测组件没有常驻 Tick；持续窗口由相应任务在有效区间采样。没有证据支持“每个检测实例都永久空转”的判断。

### 4.3 R3 公共攻击与角色收益分离有效

公共 Melee 发布结算，角色规则负责特定资源；已有执行身份、阶段及一次性消费约束。仍有兼容债务和公共伤害公式的 Havoc 专属捕获，但这与“公共 Melee 直接承担所有角色奖励”是不同问题。

### 4.4 生命周期已有保护措施

当前实现已包含弱 Avatar、代次验证、重复 Finish 防护、有限历史、死亡／解绑清理和空中次数限制。EnemyInit 也存在重入防护及有界追加检查。

因此不能把问题概括为“没有清理”“无限重试”或“所有异步回调都可能恢复旧 Pawn”。需要逐条指出哪个入口缺少哪项保障，避免为了批评架构忽视已经完成的防护。

### 4.5 配置和执行已有分层基础

攻击分段、目标 PawnData、动画 Profile、已解析运行状态和最终 GA／CMC 执行者已有基本区分。最新分段击飞作者字段也已经具有唯一来源。问题主要是解析、校验及兼容优先级尚未完全统一，而非完全没有数据驱动结构。

### 4.6 已有测试资产和运行工具可继续利用

现有测试覆盖削韧、分段击飞、阶段、取消、资源清理、角色规则身份和部分联机场景；验证记录也明确说明了假人目标、素材限制、独立专服和主观观感等边界。

后续应补齐具体缺口并让测试按身份与场景分类，不应因发现漏测而否定全部既有测试价值。

### 4.7 必须保留的正确复杂度

- Avatar、SpecHandle、PredictionKey、ExecutionId、Notify occurrence 和 Montage 实例不能随意互换。
- 客户端预测、服务器确认、拒绝恢复和模拟代理播放不是同一条执行路径。
- 命中接触、实际伤害、削韧控制和运动计划不能合为一个“播放受击动画”布尔量。
- 初始化完成、当前依赖有效、当前允许行动是不同概念，不能用一个通用 Ready 包揽。
- 资产兼容、动画骨架和权限检查必须保留；减少分支不能以删除正确约束为代价。

## 5. 潜在问题与改进建议

### 5.1 问题总览

| 编号 | 优先级 | 类型 | 核心问题 | 当前影响判断 |
| --- | --- | --- | --- | --- |
| F01 | P1 | 源码确认的条件性缺陷 | 武器 ExistingSource／Overlap 未携带球半径 | 特定查询路径可能漏命中 |
| F02 | P1 | 条件性缺陷 | 接受伤害广播早于 Damage Meta 清零 | 同步嵌套伤害可能重复消费外层数值 |
| F03 | P2 | 条件性缺陷 | 连段选边和服务器等待判定不一致 | 特定多边配置可能错误等待／超时 |
| F04 | P2 | 配置与诊断缺陷 | 隐藏参数仍被无条件校验，部分可编辑值被覆盖 | 无关参数可阻断检测，错误不易定位 |
| F05 | P2 | 配置耦合缺陷 | 独立击飞预设仍调用整份削韧校验 | 无关削韧参数错误可拒绝合法 Fly |
| F06 | P2 | 失败范围与性能风险 | 每次构建计划校验整个 Profile | 无关动画配置错误扩散，重复工作 |
| F07 | P2 | 结构债务 | 动作事务入口跨 Combat／ASC／Definition | 修改和理解需要跨类追踪时序 |
| F08 | P2 | 结构债务 | 受击组件多职责，兼容路径语义分叉 | 状态、计划、执行和表现耦合较高 |
| F09 | P2 | 结构债务 | Hero 角色专属会话及初始化具体依赖 | 新角色与初始化调整扩大公共修改面 |
| F10 | P2 | 兼容债务 | 新旧作者入口长期并存，退出条件不清 | 配置优先级和实际生效面难理解 |
| F11 | P2 | 待测性能风险 | 重复来源查询、LOS、曲线和轮询 | 尚无可量化性能损失结论 |
| F12 | P2 | 验证缺口 | 玩家作为受击目标等关键场景未被专项覆盖 | 现有通过结果不能代表这些路径可靠 |

以下建议只整理此前评审涉及的改进方向，不新增玩法、技术栈或通用规则框架。

### F01 · 武器 Overlap 路径丢失球形检测半径

**证据与逻辑。** [SocketSweep::Capture](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Combat/HodgeHitDetection.cpp:252)检查 `Source.Radius`，重置并填写 Geometry 的 Transform、Points，却没有写入其 Radius；`FHodgeHitGeometry.Radius` 默认值为 0。[ExistingSource／Overlap 分支](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeHitDetectionComponent.cpp:419)把此 Geometry 当球体查询，[ShapeQuery](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Combat/HodgeHitDetection.cpp:373)使用的是 `Current.Radius`。

**触发条件。** 使用 socket／武器既有来源、查询为 Overlap、最终按球体处理。单次命中没有显式 Profile 时，Melee 会选择 Overlap，见[Melee 单次查询默认值](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp:237)。

**影响。** 声明的刀刃厚度不进入查询，处于采样点附近但未覆盖零半径点的目标可能漏检。正常 Sweep 直接使用 `Source.Radius`，不受同样问题影响；不能描述成全部普攻失效。

**测试边界。** 当前几何测试及 ConfiguredShape 的 Overlap 检查没有覆盖“武器来源捕获后再切到 Overlap”的完整路径。本轮没有运行实际漏命中复现。

**建议。** 修复来源到 Geometry 的契约，使两种查询模式一致；保留现有策略边界，不新增检测框架。

**完成标准。** 同一武器来源下，位于声明半径内、但不直接覆盖采样点的目标应被 Overlap 检出；半径外不应误命中。保留 Box、ConfiguredShape、正常 Sweep、清理和去重回归。

### F02 · 同步接受伤害回调存在 Meta Attribute 重复消费风险

**证据与逻辑。** [HealthSet 扣血与广播](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp:264)在扣血、记录和 `OnDamageAccepted.Broadcast` 后才 `SetDamage(0)`。广播期间，外层 Damage 仍保留在 Meta Attribute 中。

**触发条件。** 同步订阅者对同一目标 ASC 再应用一份累加 Damage 的 GE。若目标从 100 生命接受外层 10 点伤害，再在回调中追加 5 点，内层可能读取 10＋5，而不是仅 5；最终可能为 75，而正确顺序应为 85。该示例用于说明静态路径，不是本轮运行测量结果。

**影响边界。** 当前已识别的 Melee 观察者主要累计实际伤害，Sprint 观察者主要取消冲刺，没有查到它们直接对同目标追加伤害。因此不宣称正式普攻目前一定重复扣血；这是公开同步事件不满足安全重入契约的问题。

**建议。** 在发放可重入通知前消费本次 Meta 输入，保存本次结果快照，并明确后续 HealthChanged／OutOfHealth 使用的数值与顺序。不要用全局重入锁丢掉合法嵌套伤害，也不要改动实际接受伤害的定义。

**完成标准。** 外层和内层伤害分别只结算一次；最终生命、Accepted 值、致死事件和结算身份正确；免疫、生命夹取、死亡和无嵌套的普通伤害不改变。现有 `RuleSynchronousReentry` 只覆盖规则收益重入，不能代替该测试。

### F03 · 多条转换边下，服务器等待与实际选边结果可能冲突

**证据与逻辑。** [SelectTransition](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp:212)按满足窗口的边及优先级选择；[CanBufferServerActivation](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp:434)发现任意同意图、同目标能力的未开窗口边就返回等待。ASC 在正式准备前先缓存，并在[延迟处理](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp:968)中可能一直等到超时。

**触发条件。** 同一意图／同一目标能力配置多条边，一条已满足窗口，另一条未满足；其余来源标签与身份合法。现有图校验没有禁止这类配置，多边优先级也已有测试。

**影响。** 原本有可执行转换边的请求可能被延迟甚至拒绝，表现为输入不跟手或预测回滚。本轮没有构造运行复现，也没有读取全部正式连段资产，不能据此认定它就是历史 Client 连段故障的原因。

**建议。** 统一边选择与等待规则，形成同一次判定的 Ready／Wait／Reject 及选中边；客户端、正式服务器准备与缓存流程复用同一语义，保留身份与优先级约束。

**完成标准。** 开闭边并存时不会被无关闭边阻塞；全部闭边时仍有界等待；相同优先级继续保持稳定数组顺序；错误 Avatar、预测键、源节点和未授予目标不得等待或执行。

### F04 · 作者界面、参数校验和实际消费存在错位

**问题一：隐藏字段仍参与失败判定。** [Notify 作者字段](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h:32)已按来源和形状隐藏参数，但[Validate](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Animation/HodgeCombatAnimNotifies.cpp:78)仍无条件检查 LocalTransform、Radius、BoxHalfExtent、CapsuleHalfHeight 等。球体或武器来源仍可能因不可见的无关 Box 数据被拒绝。

**问题二：错误容易被静默吞掉。** [Melee 创建请求](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp:215)收到 Errors 后直接返回 false。作者可能只看到“这一刀没伤害”，难以得知是哪个隐藏参数非法。

**问题三：可编辑数据不等于生效数据。** Notify 的显式 Damage 使用完整 `FHodgeHitEffectConfig`，其中包含 Reaction，但 Melee 随后总是使用独立默认／覆盖入口重新赋值 `Effect.Reaction`。该内嵌 Reaction 作者面可能看似可配、实际被覆盖。

**边界。** 当前默认值合法，不证明正式资产已遭遇上述失败。分段 `bCanLaunch` 与运行快照同名不是双作者源，不能把它列为这类问题。

**建议。** 使用同一配置解释规则决定字段展示、相关参数校验和运行消费；保留必要跨字段约束。诊断至少包含技能／Montage、分段或 occurrence、来源／形状及失败字段。明确唯一 Reaction 作者面，旧嵌套数据保留兼容但不再误导编辑。

**完成标准。** 修改无关隐藏字段不阻断合法请求；非法相关字段仍被拒绝且可以定位；默认伤害／显式伤害和默认反应／覆盖反应的组合行为清楚；不通过忽略全部校验掩盖资产错误。

### F05 · 目标击飞速度预设仍被无关削韧配置约束

**证据与逻辑。** [ResolveSegmentLaunch](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp:378)优先使用请求中的 Launch／AirHit 参数；没有显式参数时，从 PawnData 取目标速度前调用整份 `Poise.Validate`。[该校验](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Combat/HodgeHitReactionTypes.cpp:157)在削韧开启时也检查窗口、轻重阈值、样本容量和地面位移等。

**触发条件。** 分段允许击飞、请求没有显式速度、目标削韧已开启且某项与当前击飞无关的参数非法。合法目标击飞预设也会被返回 Unsupported。

**准确边界。** 不是“关闭削韧就不能击飞”，也不是所有 Fly 都走此检查；已有显式运动参数时会走另一分支。LaunchSpeed 和 DisplacementScale 本身仍应被验证。

**建议。** 将轻重累计规则与目标运动预设的相关校验分开，保留现有轻量配置，不增加大型规则资产或新执行系统。

**完成标准。** 仅无关 Window／Threshold／MaxSamples 错误时，不阻断合法的独立 Fly；非法速度／抵抗仍拒绝。覆盖削韧开关、显式参数有无、Body、目标素材缺失和次数上限。

### F06 · 全量 Profile 校验扩大单次命中的失败范围

**证据与逻辑。** [BuildPlan](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp:401)每次先执行整份 Profile.Validate；[Profile 校验](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Data/HodgeHitReactionProfile.cpp:175)检查全部支持条目、变体、阶段家族、轻反馈及曲线。

**功能风险。** 当前命中只需 Light／Heavy／Fly，但无关 Push 家族缺少 Landing、其他轻反馈非法等，也可能使本次计划整体失败。全资产合法性检查是必要的，但不应让与当前路径无关的坏配置在运行中不加区分地扩散。

**性能风险。** 校验可能解析并复制多个条目，读取 Montage／Section 等。存在击退曲线时，校验与实际 Knockback 构建又可能分别积分。其成本尚未测量，不能写成已经造成可见卡顿。

**建议。** 把配置加载／变更时的完整验证与运行时本次路径的必要检查分开；稳定配置可保存解析结果，但必须有身份和失效规则。Avatar、目标骨架、输入合法性及当前必要阶段仍需检查。

**完成标准。** 完整资产检查继续报告所有错误；运行时能指出本次所需家族为何不可用；无关家族错误不误伤合法路径。迁移与缓存前后比较 Montage、时长、MotionSource、Landing／GetUp 和运动数值，而非只比较字段数量。

### F07 · 动作事务分散，跨类完成与失败边界不集中

**证据。** Combat [准备与切换](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp:326)、ASC [等待与激活](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp:959)、Definition [执行与恢复等待](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp:98)及[通知资源清理](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp:561)共同完成一次动作。

**核心痛点。** 三者不是三套完全重复的状态机；各自职责有必要，但准备、提交、确认、拒绝、等待和结束的相互调用要求维护者掌握完整时序。修改一个恢复窗口或取消入口，可能同时影响预测、连段记忆、当前动画和占用释放。

**建议。** 先集中纯判定，再明确一次动作事务的入口、状态和唯一最终结果；Combat 保留协调，ASC 保留 GAS／网络适配，Definition 保留执行和资源生命周期。只有出现独立职责后才引入独立类，不按文件行数拆函数片段。

**完成标准。** 正常、等待、取消、失败和迟到确认均能从一个清楚的事务说明追踪；每个请求只有一次最终结果；保留 Avatar／Spec／PredictionKey／ExecutionId 的具体语义，旧回调不能清理新动作。

### F08 · 受击组件同时承担裁决、协调、控制与表现

**证据。** 受击组件包含 Begin／Finish 命中事务、伤害聚合、削韧样本、目标配置、Body、计划解析、能力扫描与取消、AI 停止移动、控制标签、阶段 Tick、复制、动画尾段／姿势租约、接触特效和轻反馈。主要入口包括[SubmitPlan](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp:544)、[RetirePresentation](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp:657)、[接触反馈](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp:919)。

**核心痛点。** 一次改动难以只影响其中一层；组件与 GA 的 Attach／Synchronize／Finish／Detach／Complete 又形成时序约束。结构复杂度主要来自多职责共同拥有执行边界，而非单纯 955 行太长。

**兼容路径边界。** 已处于受击 Airborne 时，分段分流统一决定 Fly 或接地恢复；但自然下落且未进入该状态时，显式 Slam 仍可到 BuildPlan，空中 Knockdown 仍有 AirHit／Push 表现分支。不能写成 Push／Slam 全部不可达，也不能把最新统一续飞要求称为擅自改变玩法。

**建议。** 先分清目标规则解析、已解析计划、权威状态、GA／CMC 执行及接触表现之间的接口；将无副作用的解析和独立表现职责收窄，继续使用同一受击状态。列清兼容入口和状态矩阵，避免继续往同一函数追加特例。

**完成标准。** 每一层输入输出和失败语义清楚；普通接触、Light／Heavy、独立 Fly、实际落地、Downed／GetUp 保持现有约定；不出现第二套状态机、重复控制或重复接触表现。

### F09 · 公共 Hero 残留角色规则，初始化存在具体双向依赖

**角色输入。** [Hero 长按会话](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp:1358)直接读取 HavocSet／HavocRules，使用 20ms Timer 检查回路触发。普通攻击输入又会启动该会话。Hero 同时承载电影镜头和冲刺输入状态，新增角色规则仍容易修改公共入口。

**初始化依赖。** [Combat 依赖协调](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp:899)主动查找 EnemyInit，并按 HeroCharacter／PlayerState／EnemyInit 判断就绪；[EnemyInit](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeEnemyInitComponent.cpp:255)又检查 Combat 的依赖。

**准确边界。** 双向具体依赖已经成立，但 EnemyInit 有 `bTrying` 和最多四次追加检查，不能据此认定无限递归或无界初始化重试。

**建议。** 角色长按规则回到已有角色规则宿主或由 PawnData 选择的窄策略；电影会话作为独立职责整理。初始化继续使用 Init State，分别表达阶段完成、当前绑定有效、当前可行动，减少彼此识别具体角色类的需要。

**完成标准。** 新角色不必为了专属长按修改通用 Hero；动态注入、重复注册、ASC 解绑重绑、死亡和换 Pawn 不重复授予／装备／补满资源。玩家、敌人继续保留各自所有权，不新建第二初始化协调器。

### F10 · 兼容字段和当前作者入口没有明确收口

**角色规则兼容。** Definition 的 AttackRules 与旧暗流／共鸣收益字段同时存在。[适配器](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Combat/HodgeLegacyAttackRuleAdapter.cpp:13)只在缺少同类新配置时使用旧字段，因此不会按该实现必然重复执行。但旧字段仍可编辑，在新规则存在时可能调整后没有效果。

**受击兼容。** Compact、旧 Animations 数组、显式 Impact、旧破韧字段、Fallback、旧根运动路径共同提高理解成本。旧字段有些已隐藏、不再消费，有些仍在特殊入口生效；它们不能统一按“零引用”处理。

**扩展耦合。** DamageExecution 仍有 Havoc 专属属性捕获。当前单元素需求下这不是已确认公式错误；将来扩展时需要修改公共公式，应记录为修改面风险，不趁本次评审引入通用元素框架。

**建议。** 建立兼容登记：当前作者入口、旧字段适用范围、实际资产依赖、迁移检查、移除条件。正式资产完成迁移后先关闭旧编辑面，再按证据决定是否移除运行适配。反射属性和枚举不能直接删除以免损失序列化数据。

**完成标准。** 同一配置问题有唯一可编辑答案；新配置优先级可查；每个保留兼容入口都能说明用途；Hero／Dummy／Guardian 迁移前后已解析计划保持预期，备份和引用检查完成。

### F11 · 热路径存在可减少的重复工作，但尚无性能定论

**已观察到的成本来源：**

- 来源有效性检查会枚举装备实例，采样、Capture、批次有效性和忽略 Actor 构建阶段可能重复执行。
- 刀刃多个采样点可产出同目标多条候选；LOS 在原始候选阶段逐条检查，ASC 级最终去重较晚。
- 击退曲线积分使用 128 段、129 个采样点；全量 Profile 校验与实际计划可能重复求值。
- Combat 在有效服务器／本地所有者路径中逐帧处理事件、记忆和移动取消；MovementAction 有 10ms Timer，Hero 回路长按有 20ms Timer；服务器连段缓存按下一帧重查。
- 开发调试绘制默认保留一段时间，高频采样时会增加调试图元。

来源：[检测来源检查](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeHitDetectionComponent.cpp:172)、[检测候选过滤](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeHitDetectionComponent.cpp:449)、[曲线积分](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Data/HodgeHitReactionProfile.cpp:97)。

**必须保留的限定。** 本轮没有 CPU／GameThread 采样，不能声称这些调用已导致掉帧；部分开销受活动窗口约束。调试绘制受 `ENABLE_DRAW_DEBUG` 和非 Dedicated 等条件限制，不能直接推导 Shipping／服务器存在相同绘制开销。延迟请求有容量和时限，不能写成无界队列。

**建议。** 先测调用次数和同负载耗时；稳定来源随装备变化失效，同批 LOS 仅在语义允许时复用，曲线与解析配置按资产身份缓存。候选去重不能丢掉更合适的接触点，Timer 事件化不能缩减合法恢复／输入窗口。

**完成标准。** 在同一怪物数、命中频率、网络模式及调试状态下比较前后数据，同时验证命中、动画、网络和清理不变。没有测量支持时，不以“减少 Tick 数量”作为性能完成标准。

### F12 · 测试数量较多，但角色身份和边界组合覆盖不足

**玩家受击路径。** [现有运行工具](E:/Project/Git/Hodgepodge/Tools/ACTCombat/validate_poise_runtime.py:195)取玩家作为攻击者，目标固定为敌人假人；不同 World 中的 owner 观察不等于拥有受击 Pawn 的客户端。受击 GA [Synchronize](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.cpp:120)会对 Authority／LocallyControlled 执行运动，真实玩家 AutonomousProxy、PlayerState ASC 和恢复输入需要单独验证。

**逻辑组合。** 当前原生测试尚未完整覆盖 F01 的来源切换查询、F02 同目标嵌套 Damage、F03 开闭多边混合以及 F05 无关削韧配置非法但独立 Fly 合法。

**证据误读风险。** 最终位置收敛不能证明过程中没有网络纠正；状态到达不能证明动画观感自然；Dummy 通过不能代表玩家受击通过；PIE Dedicated 也不能代表独立进程专服或打包通过。

**建议。** 保留现有套件，按“攻击者角色、受击者角色、网络角色、初始化所有权、输入恢复、生命周期和配置边界”补齐覆盖。为每项回归说明它证明什么、没有证明什么。

**完成标准。** 测试报告能分别列出原生、Dummy PIE、真实玩家受击 PIE、独立进程、性能和手感验证。缺少场景时明确未执行，不用总用例数量代替结论。

## 6. 综合影响与核心维护痛点

### 6.1 对现有功能的影响

F01 直接影响特定来源／查询的命中可靠性；F02 影响同步扩展伤害的正确性；F03 影响特定连段图的服务器接受时机；F04～F06 使配置错误的实际影响范围超出作者预期。

F07～F10 更多体现为修改成本：功能当前可能可用，但调整输入、恢复、角色规则或动画家族时，要同时理解网络、资源清理和兼容优先级，更容易发生局部修复破坏其他入口。

本轮没有证据证明所有正式攻击均异常、玩家受击一定损坏、所有怪物都具备 Fly 资源，或所有动画回弹都来自同一个原因。

### 6.2 对性能的影响

合理怀疑的主要是群怪、高频多段命中和复杂 Profile 下的 CPU 重复工作，而非伤害公式本身。目标数量、刀刃采样点、候选重复、LOS 和表现配置可共同放大成本。

但实际瓶颈可能仍在骨骼更新、动画、Niagara 或其他游戏逻辑。本报告不提供未经采样的收益百分比、FPS 承诺或毫秒估计。优化应先证明热点，再避免牺牲命中和时序正确性。

### 6.3 对可维护性的影响

最严重的维护痛点按影响排序如下：

1. **一次行为跨多个协调者。** 读懂函数不足以确定状态归属，必须追踪同步回调、服务器确认和旧动作清理。
2. **当前规则与兼容规则交错。** 同一字段在不同姿态、入口或新旧布局下可能不生效、回退或改变优先级。
3. **相同决策多次独立表达。** 选边与等待、编辑展示与校验、削韧资格与击飞预设之间出现不一致。
4. **失败诊断不够接近作者对象。** 最终只能看到 Unsupported、无伤害或动作未执行，不能直接定位分段、资产和失败规则。
5. **测试覆盖与实际运行身份不完全对应。** 已有大量断言，但维护者仍无法据此判断某些玩家和预测路径是否安全。

### 6.4 成因判断

主要成因是持续迭代时保留旧行为，又将新的特例加入原协调入口；为保障联机和取消正确性新增了必要身份，但没有同步把事务边界、作者面和迁移条件整理清楚。

文档与测试已经积累，薄弱点不在完全没有规范，而在规范尚未充分转化为唯一解析规则、清楚的失败语义和针对边界组合的回归。增加更多文档或更多类本身都不能自动解决该问题。

## 7. 分阶段重构路线图

### 7.1 总体约束与优先顺序

建议顺序：**正确性与边界基线 → 配置解释收敛 → 动作／受击事务边界 → 角色输入与初始化依赖 → 实测优化与工程治理。**

全过程保持一个 Hodgepodge Runtime 模块、现有 Editor 模块及同类主 `.cpp` 约定。不新增战斗框架，不改伤害公式和已确认玩法，不为文件变短拆成 `_Combo.cpp`／`_Presentation.cpp` 等实现片段。

### 阶段 1：先修正确性，建立有针对性的基线

**范围：** F01、F02、F03，以及后续切片必需的现状记录；F12 作为跨阶段验收任务，在本阶段建立真实玩家受击基线，后续随相应职责迁移持续回归。

**输出：** 三类边界用例、最小修复、当前正式五段／R／移动取消／受击恢复的回归结果；明确已有失败与本次新增失败。

**处理顺序：** 武器 Overlap 几何契约 → Damage Meta 重入顺序 → 多边选择／等待一致性。F02 虽未发现当前正式订阅者触发，也应在扩展同步伤害规则前处理。

**完成标准：** 每项修复有针对错误条件的回归；正常 Sweep、普通伤害、单边有界等待不退化；运行身份和清理保持现有约定。

**停止条件：** 非目标场景的伤害数值或伤害公式发生变化，或出现重复结算、原有输入窗口改变、预测身份失配及清理破坏时，停止扩大切片，先定位差异。目标缺陷场景的数值变化应符合修复回归预期，例如 F02 的示例应从错误的 75 恢复为 85。

### 阶段 2：统一当前配置的解释和相关校验

**范围：** F04、F05、F06、F10 中与当前作者面直接相关的部分。

**输出：** 当前支持模式表、来源／形状相关字段表、独立 Fly 校验边界、Profile 解析结果及兼容登记；明确旧字段是否仍读取。

**实施原则：** 先统一语义，再精简可编辑项。隐藏不是删除，完整资产校验和当前路径校验分别承担职责，错误应附带对象和阶段信息。

**完成标准：** 无关参数不阻断合法路径；错误相关参数仍被拒绝；目标运动、Body、资源缺失和限制不被放宽；Hero／Dummy／Guardian 解析结果可比较。

**停止条件：** 迁移丢失引用、产生第二个作者事实来源、旧序列化无法识别或计划数值未经批准改变。

### 阶段 3：收窄动作事务与受击执行边界

**范围：** F07、F08；复用阶段 1 的统一决策和阶段 2 的已解析配置。

**输出：** 动作事务说明、唯一完成／失败入口、无副作用的受击计划解析边界、独立接触表现职责和清理归属表。

**实施原则：** Combat 继续协调，ASC 继续适配 GAS／网络，Definition 和受击 GA 继续执行；只移动完整且有独立生命周期的职责。每个切片只切换一个实际副作用入口，避免新旧路径同时执行。

**完成标准：** 等待、取消、恢复、拒绝、目标数据迟到、同步回调重入都只产生一次最终结果；旧 Montage／RMS／请求不能影响新执行。F12 中真实玩家受击的拥有者、观察者及恢复输入回归通过后，才能收口对应事务迁移。

**停止条件：** 需要同时重写所有 RPC、运行状态复制或所有正式 Montage 才能继续。此时应缩小切片，而非一次性迁移整套系统。

### 阶段 4：处理角色输入和初始化具体依赖

**范围：** F09 及关联公共头依赖。

**输出：** 角色专属长按与电影会话的窄职责；初始化阶段、当前绑定与行动许可的分别查询；减少 Combat 对 EnemyInit／具体玩家类的识别。

**完成标准：** 现有输入手感和技能消耗保持；重复注册、动态注入、死亡、换 Pawn、ASC 解绑重绑均可靠；玩家和怪物的 ASC 所有权不改变。F12 中的玩家受击中换 Pawn、重生及解绑回归属于本阶段交付要求。

**停止条件：** 通用 Ready 查询开始混合死亡、初始化、资源驻留及临时输入限制，或为了统一而给普通敌人增加 PlayerState。

### 阶段 5：根据测量优化并清理工程债务

**范围：** F11，以及已完成切片中的重复日志、过期注释和文档。

**输出：** 同负载性能样本、稳定来源／解析缓存策略、必要的查询复用、轮询调整及兼容退出清单更新。

**完成标准：** 实测热点降低，命中和网络行为保持；日志可按域定位；当前文档与字段一致。维护收益用“改一类规则需修改哪些公共类、失败能否直达资产”衡量。

**停止条件：** 没有测量却批量关闭 Tick、降低采样、合并伤害，或缓存缺少装备／Avatar／资产变更失效机制。

本报告不提供固定总工期。正式资产盘点、运行基线和边界修复结果尚未充分支持可靠工期估算。

## 8. 必要回归测试与验收证据

### 8.1 正确性与配置矩阵

下表是后续实施的验收要求，**不是本次已执行结果**。

| 场景 | 主要断言 | 关联问题 |
| --- | --- | --- |
| 武器来源＋Overlap，目标位于半径内但不覆盖采样点 | 能检出，声明半径真实有效 | F01 |
| 同来源 Sweep／Overlap、ConfiguredShape／Box | 既有合法命中不退化，半径外不误检 | F01 |
| 同目标 OnDamageAccepted 中同步追加 Damage GE | 两份伤害各消费一次，最终生命与事件正确 | F02 |
| 嵌套致死、免疫、过量伤害 | Accepted、致死和清理身份正确，不复用外层 Meta | F02 |
| 同意图同目标，多边开闭窗口并存 | 有合法开放边时不错误等待 | F03 |
| 全部闭窗、优先级相同、错误身份、等待超时 | 有界等待、顺序稳定、非法请求拒绝 | F03 |
| Sphere／Box／Capsule／EquippedWeapon 隐藏参数 | 只校验相关数据，错误可定位 | F04 |
| 默认／显式伤害与默认／覆盖 Reaction 组合 | 唯一反应来源，编辑值与执行一致 | F04 |
| 削韧开关、无关阈值非法、显式速度有无 | 独立 Fly 不被无关配置阻断 | F05 |
| Fly 速度非法、Body 不足、无素材、次数上限 | 仍正确拒绝或按既有限制处理 | F05 |
| 无关 Push／轻反馈错误，本次仅需 Light／Heavy／Fly | 完整资产诊断保留，合法路径不被无关错误连带阻断 | F06 |
| Compact／旧布局、Hero／Dummy／Guardian | 解析计划、骨架与运动来源符合迁移前后约定 | F08、F10 |
| 实际自然下落与受击 Airborne 分别受到显式控制 | 当前分支差异有明确规则，不误判为同一种空中状态 | F08 |
| 取消、死亡、换 Pawn、ASC 解绑、旧回调迟到 | Tag／Timer／Montage／RMS／PoseLease 清理配对 | F07～F09 |

### 8.2 玩家作为受击目标的联机矩阵

保留现有玩家攻击 Dummy 的测试，同时补充：

- 怪物攻击真实玩家；玩家 A 攻击玩家 B 时使用现有合法攻击通道或隔离测试夹具，不要求新增 PvP／友伤规则或修改正式阵营配置。
- Listen 主机玩家受击、远端 AutonomousProxy 受击、第三方 SimulatedProxy 观察。
- 0ms／100ms 条件下的 Light、Heavy、Fly、可续飞／不可续飞、次数封顶和真实落地起身。
- 受击恢复窗口中闪避、普攻、移动；过早输入拒绝；预测动作被服务器拒绝后的正确恢复。
- 受击中死亡、重生、换 Pawn、解绑；旧运动和动画不能影响新 Pawn 或新动作。

同时记录目标 LocalRole、ASC Owner／Avatar、Sequence／Phase、Montage 实例、CMC 位置／速度、RMS／PendingLaunch 和控制标签。检查过程纠正和重复执行，不只比较最终坐标。

### 8.3 性能与手感的证据分离

性能评估需固定同屏目标数、命中频率、采样点数、网络模式及调试开关，记录检测、LOS、Profile 校验／积分、动画和 GameThread 成本。没有这些数据，不承诺 UMG、Niagara 或受击组件中的某一项必然是瓶颈。

手感验收仍需真实战斗画面，确认接触时机、动作连续性、脚步与胶囊、击飞落地、连续命中和目标攻击可读性。该主观验收属于原任务中已有要求，不能用本次文档、编译或状态断言替代；本轮没有执行，也没有宣称达到参考项目观感。

### 8.4 构建与报告要求

后续确实修改 C++ 时按项目规范执行常规 Editor 构建；Runtime／依赖变更再执行 Game 构建。反射布局变更前保存并关闭编辑器，随后冷启动核对受影响资产。Live Coding 不替代常规构建。

报告必须分开列出：C++、原生测试、资产检查、蓝图编译、单人 PIE、Listen、PIE Dedicated、独立进程、Cook／打包、性能和手感。通过、失败、未执行分别记录，保留首次有效失败和最终结果。

## 9. 重构挑战、约束与应对策略

### 9.1 资产和反射兼容

挑战是旧属性、枚举、布局仍可能被正式资产或测试引用。处理方式应是先盘点和备份，再比较解析结果，最后决定隐藏或移除；不以 C++ 文本零引用为删除蓝图／反射数据的依据。

保留 Hero 与 Guardian 的骨架差异。Guardian 没有兼容 Fly 动画时继续明确 Unsupported，不为了测试通过跨骨架套动作，也不把资源缺失误写成算法失败。

### 9.2 同步回调和不可逆副作用

伤害、取消能力、扣资源和广播可能在同一调用栈内重入。切片应明确“先消费什么、何时发布什么、失败后哪些状态可撤销”。只移动一处函数调用而不检查其前后观察者，可能引入更隐蔽的二次消费。

角色收益、伤害和通知表现应各有清楚身份，保留现有去重账本，不把“避免重入”实现成丢弃合法事件。

### 9.3 网络身份和更新顺序

准备、确认、Montage 复制、RMS 和物理更新可能跨帧到达。每一切片保留原身份校验、时间预算和迟到清理，按拥有者与观察者分别验收。

不能为了缩短协议代码删除等待或恢复机制，也不能用服务器直接覆盖客户端表现来掩盖预测不一致。

### 9.4 热路径优化和缓存失效

减少重复验证、枚举或积分后，必须明确装备、Avatar、配置和世界变化如何使缓存失效。否则性能改善可能换来旧来源继续命中、错骨架动画或旧计划复用。

缓存只承担已验证稳定事实，不成为新的作者入口，也不绕过本次命中和服务器权限检查。

### 9.5 工作区一致性和可回退性

当前基线包含未提交工作，实施前应保留用户资产与手工配置；每个切片独立记录修改文件、备份、验证和剩余项。本次文档整理不意味着自动授权执行所有重构，也不自动提交任何现有成果。

## 10. 历史结论校正与兼容登记

### 10.1 不能继续作为当前缺陷重复报告的事项

| 历史问题／容易误读的判断 | 当前核查结论 |
| --- | --- |
| R9 GameData 仍 NewObject 隐藏单例 | 当前 `Get()` 已转向 AssetManager.GetGameData，不再按旧缺陷安排修复 |
| R5 GameFeature 仍旧 void 授予且仅按 Actor 去重 | 当前使用受检批量授予并按 Actor／EntryIndex 记录；不能重复称其未实现 |
| R1 还没有独立检测组件 | 当前组件与几何层已存在，重点是补齐契约而非再抽一次 |
| R3 公共 Melee 仍直接执行全部 Havoc／共鸣收益 | 当前已由结算事件进入规则；旧字段适配仍待收敛 |
| Combat 的 ActionEndedHandle 一定漏解绑 | Shutdown 调用 CloseActionRequests，后者已移除并重置句柄 |
| EnemyInit 双向依赖必然无限递归 | 当前有重入防护和有界追加检查，问题是结构耦合 |
| HitReaction 没有旧 Pawn／Timer／死亡清理 | 当前已有相关保护，应针对具体缺口而非笼统否定 |
| 所有组件仍因共同基类默认 Tick 而空转 | 当前受击、旋转、Locomotion 等继承关系已变化，旧概括不适用 |
| Push／Slam 完全不可达 | 自然下落等兼容路径仍可达；已击飞再命中有新的分段优先级 |
| 本机 Lyra 标签要求实现可以直接证明 Hodge 擅改 GAS | 本机 Lyra 本身有对应扩展，必须按项目差异具体评估 |

源码核对入口：[GameData](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Data/HodgeGameData.cpp:20)、[GameFeature 受检授予](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddAbilities.cpp:380)、[动作请求清理](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp:1138)。

### 10.2 需要登记的兼容面

| 兼容面 | 当前保留原因／边界 | 后续收敛或移除条件 |
| --- | --- | --- |
| 旧角色收益字段 | 未迁移资产仍可生成同一规则实现；新同类配置优先 | 完成资产盘点、迁移与收益对照，再关闭旧编辑入口 |
| Compact 与旧 Animations | 当前与旧 Profile 布局及历史资产兼容 | 证明正式目标与必要测试全部使用当前布局，备份和重定向策略明确 |
| BreakThreshold／BreakImpact 等 | 序列化兼容，最新链路不再消费累计击飞 | 确认引用与序列化兼容后，以独立变更完成迁移审查和验证 |
| 显式 Impact 与普通 PoiseDriven | 特殊控制／旧技能和普通累计规则的区别 | 明确入口和适用状态，不让普通攻击重复执行两份控制 |
| 自然下落的 Slam／空中 Knockdown | 仍有可达兼容分支 | 先确定保留行为，再决定界面展示与迁移，不能直接当死代码删除 |
| 旧动画 Root Motion 选项 | 某些显式／Legacy 路线仍可选择 | 按实际资产和运动来源检查后收口，普通削韧互斥契约保持 |
| 早期程序生成原型与备份 | 历史验证或回退资料，不代表当前正式动作质量 | 仅在引用盘点后处理；本轮不删除资产 |

每项后续应记录实际负责范围、当前引用、迁移验证和退出条件；本报告不虚构负责人或截止日期。

## 11. 维护者阅读索引

建议按“状态与入口 → 查询和结算 → 目标裁决 → 执行和清理 → 测试”的顺序阅读，避免只从最大 cpp 从头读到尾。

- [Combat](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp)：转换边、动作授权、请求与移动取消。
- [ASC](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp)：激活确认、等待缓存、输入与能力组。
- [Definition GA](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp)：动作执行、恢复、通知和资源。
- [Notify](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Animation/HodgeCombatAnimNotifies.cpp)：分段配置解析与校验。
- [HitDetectionComponent](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeHitDetectionComponent.cpp) 与[几何策略](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Combat/HodgeHitDetection.cpp)：会话、来源、采样和查询。
- [Melee](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp)：目标去重、GE、受击事务与结算发布。
- [HealthSet](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp)：实际生命损失和同步回调。
- [HitReactionComponent](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp)、[受击类型](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Combat/HodgeHitReactionTypes.cpp)、[Profile](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Data/HodgeHitReactionProfile.cpp)：目标裁决、配置和已解析计划。
- [受击 GA](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.cpp)：Synchronize、ClearMotion、Montage 和退出。
- [连段测试](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Tests/HodgeAbilityDefinitionTests.cpp)、[检测测试](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Tests/HodgeHitDetectionTests.cpp)、[受击测试](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Tests/HodgeHitReactionTests.cpp)、[规则分离测试](E:/Project/Git/Hodgepodge/Source/Hodgepodge/Private/Tests/HodgeCombatSeparationTests.cpp)：已覆盖规则与待补边界。

历史与当前资料分别参阅[10-15 架构评审](E:/Project/Git/Hodgepodge/Docs/Design/architecture-refactor-review-2026-10-15.md)、[R1／R3 验证](E:/Project/Git/Hodgepodge/Docs/Validation/combat-detection-role-rules-2026-10-16.md)、[R5／R9 验证](E:/Project/Git/Hodgepodge/Docs/Validation/game-data-grant-contracts-2026-10-15.md)、[当前受击配置手册](E:/Project/Git/Hodgepodge/Docs/Guides/hit-reaction-configuration.md)。历史通过记录不能覆盖当前后续改动。

## 12. 总结

当前战斗模块的问题可以归纳为：**稳定机制仍在，但协调入口、配置解释和兼容路径没有与功能增长同步收敛。** 局部逻辑缺口已经可以从源码定位，长期风险则是继续通过公共类特例和兼容字段扩展功能。

优先修复武器 Overlap 几何契约、同步伤害 Meta 消费顺序和多边连段等待一致性；随后处理隐藏字段、独立击飞校验和 Profile 失败扩散，再逐步整理动作与受击事务、角色输入及初始化依赖。性能调整应有测量支持。

评审完成不等于系统已经修复。本次交付是详细 Markdown 评审及索引；没有新增玩法、技术或框架，也没有改变战斗代码。后续实施应按每个问题的触发条件和完成标准分批验证，保持既有权威、身份、资产和清理契约。
