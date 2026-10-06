> 2026-10-01 后续统一组件：连招协调已迁入 Pawn 上的 HodgeCombatComponentBase，PlayerState 不再创建 HodgeComboComponent。当前编码见第 17 节，合并后运行结果见第 18 节；此前运行记录对应合并前版本。

> 2026-10-06 状态同步：当前 CombatComponent 负责检测/协调，Melee GA 负责独立 Spec 和效果施加；DamageRules 已接通。正式普攻仍未配 HitWindows；同类实现统一主 cpp。 当前项目事实见 [本轮更新](../KnowledgeBase/26-update-2026-10-06.md)。

# 近战命中检测与 GA 效果应用：重构设计

日期：2026-10-01。状态：原生重构和攻击蓝图迁移已完成，三项新增原生测试通过；PIE 已验证三种来源及取消/重生，联机发现后加入玩家基础伤害初始化问题，尚未全部验收。本文是近战检测与伤害接入的编码依据。

原生代码已使用 CombatComponent 返回结果、近战 GA 提交效果的新链路。本轮按用户要求只编码和静态检查，没有执行构建、蓝图编译、PIE、联机、打包或自动化测试；不能把编码完成当作运行验证通过。

本文替代 [combat-hit-windows.md](combat-hit-windows.md) 中的职责划分、伤害提交方式与接入流程；旧文档保留为上一版实现和测试资产的历史记录。单段执行和连招协调仍遵循 [ability-definition-combo-graph.md](ability-definition-combo-graph.md)，Timeline 仍遵循 [ability-timeline-stage1.md](ability-timeline-stage1.md)。

## 1. 目标与已确定的边界

角色通过 Timeline Window 指定检测时段，通过来源 Tag 选择武器、本体或特定碰撞盒。CombatComponent 执行几何检测并返回命中结果；攻击 GA 决定目标是否有效、能命中几次、施加什么 GE，以及如何触发命中表现。

必须遵循以下约束：

- CombatComponent 不创建 GameplayEffectSpec，不应用 GE，不计算伤害，不保存伤害参数。
- CombatComponent 的几何检测部分不依赖 Timeline Task、AbilityLevel、EffectContext、CombatSet 或目标 HealthSet；连招协调部分绑定来源 ASC，通过当前 GA 读取自己的窗口和推进转移。
- 返回结果保留 FHitResult；只有 Actor 数组不足以支持命中位置、骨骼、物理材质和受击表现。
- 命中结果在窗口内每次采样后交给对应 GA，不积攒到窗口结束才应用伤害。
- 窗口级、攻击段级去重及重复命中间隔属于攻击规则，由 GA 持有。
- GA 构建和应用效果，Execution 计算数值，HealthSet 扣血、处理免疫和死亡。
- 第一版保持服务器权威检测与伤害，不新增客户端命中 RPC、服务器回溯或客户端伤害预测。

## 2. 当前事实与 Lyra 参考

### 2.1 当前项目的问题

以下为 2026-10-01 重构前 review 的问题记录；已实施结果见第 10、13 节。

1. `Private/Component/HodgeCombatComponentBase.cpp` 的 Sample 同时负责几何查询、目标 ASC/生命资格检查、去重、Spec 创建与 GE 提交。
2. BeginHitWindow 接收整个 FHodgeHitWindowBinding、ASC、Timeline、EffectContext 和 Level，并要求源 CombatSet 存在。
3. 组件 Tick 主动刷新 Timeline 的 Montage 时钟，造成检测器与技能执行器双向依赖。
4. 组件直接通过 ASC.MakeOutgoingSpec 创建效果，没有完整经过 GA 的 ApplyAbilityTagsToGameplayEffectSpec 和 AbilitySpec SetByCaller 参数复制流程。
5. UHodgeAbilityDefinition 的校验把自定义伤害效果限定为 Instant 且必须包含 HodgeDamageExecution，限制了检测的其他用途。
6. 新命中链只接入 HodgeGameplayAbility_Definition；旧 HodgeGameplayAbility_BasicAttack 仍使用独立 Timeline 时钟，没有接入新结果回调。

当前可保留的基础包括来源 Tag、Socket/Box 策略、独立几何历史、执行身份、Timeline 的 EventIndex 窗口通知，以及取消/解绑的清理保护。重构应改变职责和连接方式，避免无关重写。

### 2.2 Lyra 中已核实的内容

参考项目：`E:/Project/UProject/study/Epic/LyraStarterGame`。

- `Plugins/GameFeatures/ShooterCore/Content/Game/Melee/GA_Melee.uasset` 的序列化节点引用和注释包含 CapsuleTraceSingleForObjects、CompareTeams、遮挡 LineTraceSingle、MakeEffectContext、EffectContextAddHitResult、BP_ApplyGameplayEffectToTarget、GE_Damage_Melee 和命中 GameplayCue。
- 此资产使用 Single Trace，不能当作已经具备持续多目标窗口的框架。本文的多目标、Socket 扫掠和窗口会话是本项目需求。
- `Source/LyraGame/Weapons/LyraGameplayAbility_RangedWeapon.cpp` 的 OnTargetDataReadyCallback 将数据交给 OnRangedWeaponTargetDataReady，供能力蓝图处理效果；它是职责划分的参考，不是本项目近战网络实现的直接模板。
- `Source/LyraGame/AbilitySystem/Executions/LyraDamageExecution.cpp` 负责捕获属性、交互规则和最终伤害输出，说明“GA 应用 GE”不等于“GA 手写扣血公式”。

核实范围：读取本地资产保存的函数引用、属性名与注释，以及上述原生源码；没有通过编辑器导出 GA_Melee 的完整引脚连线。本文不声称复刻其完整执行顺序、数值或网络路径。

## 3. 最终调用链与各层职责

```text
当前 Pawn 的 CombatComponent 连招入口授权并启动一个 Definition 攻击 GA
  → Definition GA 播放 Montage、运行 Timeline
  → 近战 GA 根据 Window 通知建立/结束检测会话
  → GA 所属检测 Task 在采样阶段调用 CombatComponent
  → CombatComponent 返回本次采样的 FHitResult 批次
  → 近战 GA 过滤目标、登记命中次数、构建并应用 GE
  → DamageExecution 计算伤害
  → HealthSet 消费 Damage 并处理 Health/死亡
```

### 3.1 Timeline

- 维护 Montage 实例时钟、窗口标签/GE 账本、Point 与窗口进入/退出通知。
- 保留 EventIndex，不能仅靠 ASC 聚合 Tag 计数区分不同窗口。
- 不查询碰撞，不引用 CombatComponent，不构建攻击伤害效果。
- 窗口 GE 的通用状态职责保持原契约；近战伤害不借用 WindowEffectClass 执行。

### 3.2 CombatComponent 与检测策略

- 解析角色 HitSources 和当前装备 WeaponInstance.HitSources。
- 建立只含检测信息的会话，持有来源、Profile 与上一帧几何。
- 根据策略执行扫掠，忽略自身和自身装备外观，应用 Profile 的几何角度与遮挡条件。
- 返回原始命中数据和本次解析的来源信息。
- 来源失效、武器卸下或组件销毁时结束对应会话。
- 不判断队伍、无敌、生命值，不要求命中 Actor 实现 AbilitySystemInterface。

Socket/Box 策略继续只执行 Capture 与 Detect。来源解析失败必须报错，不能自动退化为角色本体。来源 Tag 精确匹配且唯一。

### 3.3 检测 AbilityTask

拟新增 `UHodgeAbilityTask_WaitHitResults`，作为 GA 所有的生命周期与采样适配器。它不包含伤害规则。

- 持有本次 GA 执行身份、检测会话句柄、弱 Avatar/CombatComponent 引用。
- 调用组件的显式采样接口，再将结果交给所属 GA。
- Task 结束时移除回调/调度并关闭所拥有的会话；不会关闭其他 GA 的会话。
- 仅作为时序和生命周期适配层，不成为第二个连招或伤害管理器。

### 3.4 GA、Execution 与 HealthSet

- 通用 `UHodgeGameplayAbility_Definition` 保留单段执行、Timeline/Montage 和 CombatComponent 连招通知。
- 拟新增 `UHodgeGameplayAbility_Melee : UHodgeGameplayAbility_Definition` 承担近战配置、窗口会话编排、命中规则、GE 与命中表现。
- 通用 Definition 基类不再直接执行近战命中业务；提供受保护的窗口/执行生命周期扩展入口，供子类在 Timeline 激活前接入。
- GE 的选择、等级、倍率、DamageType 和友伤策略由近战 GA 决定。
- HodgeDamageExecution 保持数值计算和伤害交互的最后检查；HealthSet 保持扣血、免疫与死亡处理。

## 4. 数据与接口契约

以下名称是拟实施接口，不是当前可调用 API。编码时保持 Hodge 命名及 Public/Private 对应目录，不引入新 Runtime 模块。

### 4.1 检测请求

拟引入 `FHodgeHitDetectionRequest`，只包含：

- SourceTag。
- UHodgeHitDetectionProfile 引用。
- 几何查询确实需要的额外忽略 Actor 列表，如有。

ExecutionId、EventIndex 是关联与生命周期信息，不作为算法分支依据。请求中不得出现 DamageEffect、DamageMultiplier、DamageType、友伤策略、目标 ASC、EffectContext 或 AbilityLevel。

现有 FHodgeHitSource 和 UHodgeHitDetectionProfile 保留，避免无必要的资产布局迁移。ObjectTypes、Radius、Socket、角度、遮挡和传送阈值仍属于检测参数。

### 4.2 会话接口

组件提供以下能力，具体 C++ 签名在实施阶段落实：

1. CreateDetectionSession：校验并解析来源，分配句柄，建立初始几何；不触发命中回调。
2. SampleDetection：显式采样一个会话，返回命中批次；无有效会话返回明确状态。
3. ResetDetectionHistory：只重置几何历史，用于暂停/回退后的恢复。
4. EndDetectionSession：幂等关闭一个会话，不产生伤害或自动补采样。
5. EndDetectionSessionsForExecution / EndAllDetectionSessions：处理指定执行及 Avatar 级清理。

句柄由组件分配，不复用活跃句柄；与 ExecutionId 联合校验。一个执行内，EventIndex 到会话句柄的映射由 GA 所属 Task 维护。会话只持有必要的 UObject 强/弱引用，Profile 强引用须参与 GC 跟踪。

组件的几何检测不自动 Tick 采样；连招 Tick 仅负责输入缓存、事件队列和移动取消。旧组件中自动刷新 Timeline 并采样、直接提交效果的 Tick 逻辑应移除。

### 4.3 命中批次与回调

拟引入 `FHodgeHitDetectionBatch`，包含：

- ExecutionId、EventIndex、SessionHandle。
- 本次采样的 `TArray<FHitResult>`。
- 来源信息：弱来源组件、弱 WeaponInstance（本体可为空）、采样来源位置。
- 采样时间或递增 SampleSequence，用于诊断和防止错误的重复消费。

检测组件不认识接收 GA。Task 将批次通过自身委托交给唯一所属 GA，禁止 Pawn 级全局广播让所有活跃 GA 同时处理。

绑定回调、登记 Task 与会话句柄之后才能首次采样。解决方式固定为“建立会话 → 登记身份和句柄 → 绑定回调 → 启动采样”，不通过返回后补登记来补救首次同步回调。

回调执行前后都要重新检查 Task/GA 是否仍有效、本次 ExecutionId 是否仍匹配。处理数组过程中，如果一次 GE 引起技能结束、死亡或换段，后续旧批次立即停止处理。

## 5. 采样阶段与 Timeline 同步

### 5.1 唯一协调入口

检测 Task 在 Avatar 所在 Level 注册独立的 `TG_PostPhysics` TickFunction，前置依赖角色 Mesh，并在 Task 销毁时注销。不要为了一个攻击任务修改 PlayerState ASC/GameplayTasksComponent 的全局 TickGroup。

该原生 TickFunction 的生命周期和暂停规则必须明确实现；普通 AbilityTask.TickTask 默认阶段不能冒充已满足 PostPhysics 要求。

每次检测 Task 更新按以下顺序执行：

1. 通过所属 GA 的执行入口刷新对应 Timeline/Montage 时钟。
2. 时钟刷新可能同步关闭窗口、结束 Task、结束技能或切段；重新检查执行身份和活跃句柄。
3. 对仍活跃的窗口调用组件 SampleDetection。
4. 将每个有效批次回调给该 GA。

Timeline 原有 Tick 可以继续推进逻辑，但高水位调度必须保证同一时间不重复发事件；每个检测会话的周期采样只由该检测 Task 调度，不再有组件与 Task 同时采样。

### 5.2 窗口进入、退出与暂停

- 进入：句柄和回调全部就绪后立即采样一次。
- 活跃：骨骼更新后的采样阶段持续查询。
- 正常退出：在窗口通知链中显式补最后一次采样，然后关闭会话。保留现有退出通知早于该窗口 Tag/GE 清理的时序，确保末次处理仍具有正确窗口状态。
- 取消、Task 销毁、死亡、解绑、Pawn 更换、主动终止：直接关闭，不补采样。
- Montage 暂停：保持窗口并检测当前位置；RepeatHitInterval 使用世界时间，暂停不会冻结该间隔。
- Montage 位置回退到窗口前：暂停该窗口的结果输出，只重置几何历史；恢复后继续检测，不重新创建窗口或重复发送进入事件。

初次采样和正常退出采样可能同步结束技能，不能持有容器元素引用跨回调。Task 更新和窗口通知需要重入保护，采样不得递归刷新 Timeline。

单帧跨完整窗口使用当前可取得的姿态进行进入/退出采样，不声称恢复历史轨迹。保留现有有限采样/盒体旋转近似的局限。

## 6. GA 目标规则与去重

### 6.1 目标过滤

近战 GA 默认 GE 路径依次检查：

1. 批次身份正确、当前 GA 活跃、Avatar 未更换且当前端有服务器权限。
2. HitResult 的 Actor/组件有效；检测来源仍有效。
3. 通过现有 GAS 查询方式取得目标 ASC；其 Avatar 有效且不是本次攻击者，也不是相同源 ASC。
4. 对默认生命伤害确认目标可受伤，按攻击配置检查队伍等资格。
5. 根据命中记录判断是否允许提交本次效果。

没有 ASC 的 Actor 仍能作为几何结果返回。默认伤害 GA 可以跳过；派生能力可用于可破坏物或交互，不由检测组件提前抹掉。

GA 层目标策略提供可覆写入口。不要以“检测可复用”为理由把全部业务规则再次堆到组件；通用伤害交互的最终保护由 Execution 保持。

### 6.2 命中次数

- RepeatHitInterval=0：每个有效目标在本窗口内最多提交一次。
- RepeatHitInterval>0：按世界时间允许该目标重复命中，不是几何采样频率。
- HitGroup 为空：以 EventIndex 区分窗口；同 Tag 的顺序窗口各有独立记录。
- HitGroup 非空：同一次 GA 执行的同组窗口共享记录，直至该次执行结束。
- 同组 RepeatHitInterval 必须一致。

默认伤害按目标 ASC 去重，将同目标胶囊/网格等碰撞结果归并。跨窗口/跨采样记录全部由 GA 保存；组件不得记录“该目标已经吃过伤害”。

同批次候选默认按有效结果原顺序处理，一个目标取第一条可用 HitResult；保证结果携带空间信息，不把多个刀身采样点造成的重复接触变成多次伤害。

必须先通过资格检查并成功构建有效 Spec，随后登记命中机会，再进入 Apply 调用。默认规定：GE 被免疫或拒绝仍消耗本次接触机会；队伍不符、没有 ASC、Spec 构建失败不消耗。不要根据 Instant GE 的 ActiveGameplayEffectHandle 是否有效来推断有没有造成伤害。

如果先提交效果再记账，同步 GAS 回调可能重入造成重复伤害；本次设计禁止这种顺序。

## 7. GA 构建与应用 GE

### 7.1 来源与 Context

- 从 GA.MakeEffectContext 建立上下文，保留当前能力、授予来源和正确 Instigator/EffectCauser 语义。
- 为每个目标建立独立上下文，写入其 FHitResult 和本次攻击 Origin，避免多个目标共用一个可变 Context。
- 检测批次中的 WeaponInstance 只是可供 GA 使用的来源信息；组件不得覆盖 Context.SourceObject 或 AbilitySource。
- 武器 GA 若要将武器用作效果来源，应在自己的 Context 构建入口中明确处理，并保留原 AbilitySpec.SourceObject。不能假设任意 WeaponInstance 都实现 AbilitySourceInterface，也不能用一次失败的接口转换抹掉已有来源。
- 本体/碰撞盒攻击使用其实际来源位置；EffectCauser 的业务归属由 GA 决定，不自动改成碰撞组件 Owner。

### 7.2 Spec 构建顺序

通过 GA 的构建路径保留 ApplyAbilityTagsToGameplayEffectSpec 和 AbilitySpec.SetByCallerTagMagnitudes。不得只把现有 ASC.MakeOutgoingSpec 调用机械搬进 GA 函数。

具体可采用 GA 内部的带 HitResult 构建帮助函数：先建立每目标 Context，再创建 Spec，然后执行与 GA.MakeOutgoingGameplayEffectSpec 一致的能力标签与 SetByCaller 初始化，最后写入该窗口的倍率和伤害类型。另一种实现可以调用原生 GA 构建 API 后补独立 Context，但必须保证依赖 HitResult 的标签扩展在 Context 完整后执行。

实施时选一种并在代码中集中实现，不维护两套行为不同的 Spec 构建路径。相关测试必须证明 AbilityTag、动态 Spec 来源标签和授予的 SetByCaller 均被保留。

顺序为：完整 Context → 能力基础参数 → 本窗口参数覆盖 → 有效性检查 → 登记命中机会 → 通过 GA 的效果应用路径提交目标。

### 7.3 伤害配置

现有 FHodgeHitWindowBinding 的伤害字段可以暂时保留在 Definition.HitWindows 中，作为近战 GA 读取的配置；传给组件时构造纯检测请求。保持字段名以降低资产迁移成本，不再让整个 Binding 进入检测会话。

- DamageEffect 由窗口显式配置，常规伤害选择 `/Game/GameplayEffects/Damage/GameplayEffectParent_Damage_Basic` 或其子类；留空时报配置错误，不提供原生替代 GE。
- 当前默认公式仍为 BaseDamage × DamageMultiplier × 距离/材质及交互倍率，不在本次重构中新增伤害模型。
- CombatSet 是默认伤害路径的来源属性需求，不是检测会话的启动条件。
- 将“Instant 且使用 HodgeDamageExecution”的限定移出通用检测校验。默认生命伤害可以维持此明确配置契约；其他效果由相应能力校验自己的需求。
- 默认效果只写一次 Damage，避免 Modifier 和 Execution 对同次攻击重复扣血。

友伤/自伤等通用策略需要区分攻击筛选与最终伤害规则。复查 FHodgeDamageRules，不能让近战新增规则无条件改变所有其他 GE 的行为；自毁、环境伤害等须按实际 GE 路径单独核实，不能仅根据源码推断已回归或已通过。

### 7.4 命中表现

GA 根据有效命中和攻击配置触发 GameplayCue、受击事件或额外效果；组件不生成 Niagara 或 Cue。首次重构不自动创建这些表现资产。

命中接触、效果提交和实际 Health 下降是不同事实。需要“造成伤害才播放”的表现时，应由效果/属性变化事实驱动，不能把几何命中或有效 Spec 当作伤害成功证明。服务器 Cue 与本地预测 Cue 另行设计去重；第一版不双端重复执行同一命中 Cue。

## 8. 生命周期、权限与扩展入口

- PlayerState 继续拥有玩家 ASC；连招会话与几何检测统一位于 Avatar Pawn 的 CombatComponent，不改变 GAS 所有权。
- CombatComponent 通过 Experience 的 AddComponents Action 添加，角色构造函数不创建默认判定组件；组件自身 EndPlay 清理会话，角色死亡和 ASC 解绑按需查找组件后清理。
- 第一版检测 Task 只在服务器建立有效会话，伤害 GA 在结果消费和 Apply 前继续检查权限。
- 客户端 Timeline 可以维护本地窗口表现，但不建立服务器伤害会话。
- 每次 GA 激活生成新的本地 ExecutionId；它不是跨端预测/网络身份，不能用于伪装 TargetData 验证。
- GA EndAbility 清理本次执行的 Task、记录和会话；必须在可能发出末次旧回调之前失效旧执行身份。
- Task.OnDestroy、组件 OnUnregister/EndPlay、死亡开始、ASC 解绑与 Avatar 更换提供幂等清理。默认武器的装备/解绑/重生生命周期继续使用已验证的原有链路。
- 保留 Unreal GC 持有规则，Task/组件和 GA 间的委托采用可解绑的 UObject 弱绑定，不留下捕获裸 this 的异步回调。

近战子类可以提供受保护的 C++ 策略入口及蓝图扩展事件，用于过滤目标、构建命中效果和附加表现。无效批次及客户端回调不能通过蓝图入口绕过原生权限和执行身份检查；蓝图若需替换默认伤害，必须明确“替换”与“附加”语义，避免默认 GE 和蓝图各应用一次。

不添加全局 PendingDamageRequest，不通过 GA_QuickbarSlots 管理常驻武器，不以聚合 LooseTag 注册回调代替窗口身份。

## 9. 文件与资产迁移范围

主要源码落点：

- Public/Private/Combat/HodgeHitDetection：检测请求、批次/来源数据及已有策略。
- Public/Private/Component/HodgeCombatComponentBase：统一连招协调入口、纯几何检测会话与显式查询接口。
- Public/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition：通用生命周期扩展，移出近战业务。
- 对应目录 HodgeGameplayAbility_Melee：近战 GA 消费结果和应用效果。
- 对应目录 HodgeAbilityTask_WaitHitResults：GA 所属的采样与生命周期适配。
- Public/Private/Data/HodgeAbilityDefinition：检测校验与近战配置校验边界。
- 相关 DamageExecution/DamageRules：仅修正已确认关联的问题，保留统一数值/生命链。
- Private/Tests：保留已有 Timeline/几何测试，补结果路由及 GA 效果路径的针对性测试。

迁移原则：

1. 先核实当前 Experience → PawnData → AbilitySet 的真实引用和 GA_Attack_1～5 的实际父类。
2. 新近战 GA 继承现有 Definition 执行链。核对原攻击蓝图的变量/事件后，再迁移父类或 AbilityClass；不能只换 Class 让旧蓝图扩展失效。
3. 旧 HodgeGameplayAbility_BasicAttack 和使用它的资产保留为兼容基线，核对引用后再决定迁移；本轮不再发展第二套命中/伤害系统。
4. 保留现有 HitSources、Profile、WindowTag 和 HitWindows 字段名；确需反射变更时附带资产迁移办法与必要重定向。
5. `/Game/CodexText/CombatHitWindows` 现有八个资产是旧配置样例，应在新路径接通后重新校验和运行，不能当作新设计已验证。
6. 原 DA_Attack_1 在上轮检查中实际引用 BasicAttack/DA_Attack01_Timeline；实施时从 ExecutionConfig 展开引用重新确认，不按附近文件名猜测。

## 10. 编码顺序与验收门槛

以下状态区分源码完成与资产/运行验证；不以静态检查替代编译和玩法验收。

- [x] A. 固定源码与资产序列化引用基线；没有在编辑器核对完整蓝图引脚。
- [x] B. 引入纯检测请求和命中批次，建立显式创建/采样/结束会话接口，保留来源及策略。
- [x] C. 引入 GA 所属检测 Task，落实 PostPhysics 调度、两阶段启动、窗口边界和幂等清理。
- [x] D. 建立近战 GA 与通用 Definition 的扩展边界，把目标筛选、去重、Context/Spec 和效果应用迁入 GA。
- [x] E. 移除组件中的 GE、伤害属性、目标生命规则及 Timeline 时钟依赖；一次批次只能由所属 GA 消费。
- [x] F. 调整配置校验和迁移攻击资产入口，旧 BasicAttack 保持兼容，不双重授予相同 AbilityTag。
- [ ] G. 执行或交付下节验证，记录真实结果与未验证项，再更新旧文档和测试配置。

每步只做相关改动，不自动提交，不修改引擎/插件版本，不重构第三方插件。编码保持中文注释一行说明一件事，遵守项目注释块长度约定。

F 已通过 MCP 迁移五个攻击蓝图并编译保存；G 已执行原生测试、单机 PIE 和双玩家 Listen Server，仍有初始化失败与未覆盖项，详见第 15 节。旧 BasicAttack 没有增加第二套检测或伤害提交实现。

## 11. 验证计划与完成定义

### 11.1 结构与原生回归

- 组件不再引用伤害 GE、CombatSet、HealthSet、GameplayEffectContext 或 Timeline Task，不含 MakeOutgoingSpec/ApplyGameplayEffect 调用。
- 返回 Actor 没有 ASC 的命中仍可被 GA 收到；默认伤害 GA 可以跳过该目标。
- 两个并发 GA 的批次互不串用；旧执行取消后不会污染同一 GA 实例的下一次激活。
- 首次立即采样与正常退出采样均已绑定接收者；同步回调结束 GA 后不再处理旧数组剩余目标。
- 同窗口多组件、多刀身采样点只提交一次；不同窗口独立；同 HitGroup 共享；重复间隔正确。
- SourceObject、能力标签、动态来源标签及 AbilitySpec SetByCaller 保留；两个目标的 HitResult/Context 不相互覆盖。
- 检测会话在没有 CombatSet 时仍能返回结果；默认伤害路径缺少所需属性时给出可定位诊断。
- 所有 Task 销毁和清理路径注销独立 TickFunction，无滞留回调、失效对象访问或重复采样。
- 保留已有 Timeline 的跨帧、重入和清理测试；增加结果消费测试，不用原八项测试宣称完整伤害链已通过。

### 11.2 编辑器与玩法验收

- 对迁移的近战 GA 蓝图核对父类、失效引脚与实际 AbilityClass，确认只授予一份对应能力。
- 本体、武器、指定 Box 三种来源各能在窗口内返回结果并由 GA 造成预期伤害。
- 在来源 BaseDamage=20、窗口倍率=1.5、无其他修正时，目标 Health=100 → 70；同窗口保持接触不再扣血。
- 范围外、遮挡、友军/无敌等情况分别符合检测、攻击规则和效果规则，不混淆失败原因。
- 取消前未命中无伤害；取消后无尾伤；卸武器、死亡、解绑、换 Pawn 无残留；重生恢复且不重复扣血。
- Listen Server 双玩家双方发起攻击均由服务器应用伤害，两端 Health 一致。没有受击预测不应被报告为实现了预测/回溯。

完成定义：职责检查、授权范围内的构建/测试与资产迁移检查均有明确结果，未执行项目单独标注；没有出现 CombatComponent 与 GA 同时提交同次伤害的过渡状态。

## 12. 首版暂不实现的扩展

客户端 TargetData 上传与服务器验证、历史姿态回溯、精确旋转连续碰撞、独立伤害类型公式、格挡/暴击、自动 Niagara/Cue 资产、独立服务器可执行文件均不在本轮范围。

扩展算法继续派生检测策略；扩展命中行为通过 GA；扩展伤害数值通过 GE/Execution。禁止以新增算法或效果为理由把伤害提交重新放回检测组件。

## 13. 本次实现与编辑器迁移

### 13.1 已编码接口

组件通过 CreateDetectionSession、SampleDetection、ResetDetectionHistory、EndDetectionSession 和按执行/全量清理返回或管理纯几何会话；GetDetectionSourceComponent 提供实际来源组件供 Task 建立 Tick 前置依赖。几何检测不自行 Tick，组件 Tick 只推进连招缓存和事件，几何查询也不要求任何 ASC/CombatSet 存在；连招入口需要绑定当前 Avatar 的 ASC。

WaitHitResults Task 绑定单个执行的结果接收者，持有 EventIndex 到句柄映射。在角色 Mesh 和实际来源组件更新后执行周期采样；即时进入/退出采样使用通知当时可用姿态。Task 暂停会关闭独立 Tick，恢复前重置几何历史；Montage 暂停仍允许活跃窗口检测当前位置。OnDestroy 和 BeginDestroy 都幂等注销 Tick 并仅释放所属执行。

Definition GA 增加 OnExecutionReady、OnExecutionEnding、OnExecutionWindowEntered、OnExecutionWindowExited 通用扩展入口。没有 HitWindows 的通用技能保持原有执行链；配置了 HitWindows 但仍直接继承 Definition 的资产会明确校验失败，不再静默转交组件施加伤害。

Melee GA 提供以下替换入口：

- ProcessMeleeHitResults：替换整个批次处理，可读取没有 ASC 的命中。
- CanApplyMeleeHit：替换默认目标资格过滤。
- ApplyMeleeHitEffects：替换单目标默认效果提交；若调用父实现，不要再次提交同一个 GE。
- C++ MakeMeleeHitContext / BuildMeleeHitSpec：扩展来源和 Spec 构建。
- C++ ValidateExecutionConfiguration：为不使用默认生命伤害 Execution 的派生能力定义自己的效果契约。

默认处理保留每目标独立 HitResult/Context、GA 能力标签、动态 Spec 来源标签和授予的 SetByCaller；窗口倍率只覆盖 SetByCaller.DamageMultiplier。TargetData 使用目标 ASC 的 Avatar，保留原碰撞信息，并显式写入来源 Transform，避免 ActorArray 将 Context 的 Origin 覆盖为零坐标。

新增测试源文件 Private/Tests/HodgeMeleeRoutingTests.cpp，注册 Hodge.Combat.DetectionRouting、Hodge.Combat.MeleeHitHistory、Hodge.Combat.MeleeSpecContext。覆盖无 ASC 几何命中、会话身份隔离、Task 重复释放、来源销毁、独立/共享记录、重复间隔、授予参数与动态标签保留、SourceObject 和每目标 Context 隔离；这些测试本轮未执行。完整取消重入、Tick 时序和实际扣血仍需第 11 节验证。

### 13.2 当前真实资产入口

离线读取的序列化引用显示 Experience 使用 `/Game/Main/Data/PawnData/DA_Dafult_PawnData`；该 PawnData 关联 BP_Hero_Pover 及 DefinitionCombo 中的 AS_LightCombo、DA_LightCombo。不要根据 AI_DEVELOPMENT.md 的历史路径配置旧的 `/Game/Main/Data/DA_Dafult_PawnData`。

`/Game/CodexText/DefinitionCombo/GA_Attack_1` 至 `GA_Attack_5` 在迁移前直接继承 HodgeGameplayAbility_Definition，现已通过 MCP 改为 HodgeGameplayAbility_Melee。`DA_Attack_1` 的实际 Timeline 引用仍为 `/Game/CodexText/BasicAttack/DA_Attack01_Timeline`，不是旁边的 DA_Attack_1_Timeline。运行前必须在每个 Definition 的 ExecutionConfig 展开确认实际 Timeline。

原生编码阶段只读取资产；用户编译完成后的 MCP 阶段修改了上述五个 GA，并新增第 14 节的测试资产。实际 Timeline 的 C++ 数据校验实现 `Source/Hodgepodge/Private/Data/HodgeAbilityTimeline.cpp` 未在这两个阶段被修改。

### 13.3 编译后迁移步骤

1. 保存编辑器工作，关闭本项目编辑器，按项目流程由用户执行常规 Editor/Game 构建，再打开项目。
2. 打开 `/Game/CodexText/DefinitionCombo/GA_Attack_1` 至 `GA_Attack_5`，通过 File → Reparent Blueprint 将父类改为 HodgeGameplayAbility_Melee。保留现有类默认值、变量及事件；逐个检查失效节点后编译保存。不要另建同 AbilityTag 的能力再重复授予。
3. 打开同目录的 DA_Attack_1 至 DA_Attack_5，确认 AbilityClass 仍指向对应已换父类的 GA。没有 HitWindows 的段仍只播放原逻辑；要检测的段配置 HitWindows，并保证 WindowTag 在实际引用 Timeline 中有 Window 条目。
4. 首先用已有 `/Game/CodexText/CombatHitWindows/DA_Test_Body` 验证。确认 AbilityClass 是 GA_Attack_1 或自己的 Melee 子类；Timeline 引用 DA_Test_Body_Timeline，绑定 SourceTag=Combat.Source.Body.Origin、Profile=DA_Hit_SocketSweep，DamageEffect=GameplayEffectParent_Damage_Basic、DamageMultiplier=1.5、RepeatHitInterval=0、HitGroup 留空。
5. 在 Experience 的 AddComponents 中选择判定组件蓝图，在该组件蓝图的 HitSources 配置唯一的 Combat.Source.Body.Origin 来源，Sockets 留空，LocalOffset 指向前方，Radius 覆盖测试目标。不要在角色构造函数或角色蓝图再添加第二份组件；测试来源迁移见第 16 节。
6. 武器样例使用 DA_Test_Weapon / DA_Test_Weapon_Timeline，在当前默认武器 Instance 的 HitSources 填相同来源 Tag、正确 WeaponActorIndex、实际组件名和存在的 Socket。Box 样例使用 DA_Test_HitBox / DA_Test_HitBox_Timeline、DA_Hit_BoxSweep，在判定组件蓝图的 HitSources 配实际 BoxComponent 名称。以样例资产现有 SourceTag 为配对依据，不改已有 Tag 名称。
7. 当前 HodgePlayerState 已创建 CombatSet 默认子对象，不要通过 AbilitySet 再添加第二份同类型属性集。将测试来源 BaseDamage 设置为 20，并确认目标 ASC 有 HealthSet、Health=100；若保留窗口倍率 1.5、无其他修正且队伍允许受伤，按本轮已验证的现有伤害 GE 配置，首次接触预期 100→70，同窗口持续接触不再次扣血。
8. 若通过 AS_LightCombo 临时替换第一段 Definition 为 DA_Test_Body，先记录原引用，保证该 AbilityTag 只授予一次，测试后还原。此样例 Timeline 只有命中窗口，没有连招过渡窗口，不能据此验证完整连招。
9. 先执行新增三项 Hodge.Combat 测试，再按第 11 节检查窗口边界、取消、换 Pawn 和双玩家。原八项自动化历史结果不代表重构后通过。

没有迁移攻击蓝图时，新默认命中配置不能运行；有 HitWindows 的旧 Definition 父类会输出迁移错误。这是明确的迁移门槛，不允许临时把伤害提交恢复到 CombatComponent 绕过。

## 14. MCP 接入结果与当前测试入口

### 14.1 实际执行结果

用户报告原生编译完成后，连接了当前 Hodgepodge 的原生 MCP HTTP 接口，核实项目绝对路径和引擎版本 5.5.4。项目 `.codex/config.toml` 仍指向历史 D 盘目录，本次未修改该配置；实际使用当前项目 Saved/MCP 的凭据连接当前编辑器，不向日志输出凭据。

读取了五个攻击 EventGraph：均只有未连接的激活/结束事件，未发现需要迁移的 EventGraph 伤害提交链。逐个备份到 `/Game/CodexText/Backups/MeleeMigration_20261001/GA_Attack_N_BeforeMelee` 后，迁移父类、使用 MCP `manage_blueprint.compile(saveAfterCompile=true)` 编译保存。五个蓝图均返回 UpToDate、saved=true。

五个正式 DA_Attack 和八个原有 CombatHitWindows 样例均通过 EditorValidatorSubsystem 的 SCRIPT 数据校验。正式 DA_Attack_1～5 的 HitWindows 仍为空，实际 BasicAttack Timeline 目前只有恢复/取消窗口；本次没有凭空设置正式动作的命中时间，也没有切换正式 Experience。

执行命令及结果：

- `automation RunTests Hodge.Combat.DetectionRouting`：日志显示成功，覆盖无 ASC 命中和会话隔离等。
- `automation RunTests Hodge.Combat.MeleeHitHistory`：日志显示成功。
- `automation RunTests Hodge.Combat.MeleeSpecContext`：首次失败，因测试创建抽象 UObject 基类触发 Ensure；改为既有场景组件后，用户重新编译，MCP 复跑通过。日志于 2026-10-01 04:12:58 UTC 记录 `Test Completed. Result={成功}`，测试事件区间无错误。
- `git diff --check` 及新增脚本的 Python 语法检查：作为源码/文档静态检查，不能代替 UE 构建或 PIE。

测试启动时编辑器后台降频导致等待帧率门槛，临时在内存关闭后台降频后运行测试；测试结束已恢复原值 True，没有保存该设置。没有启动 PIE、联机、打包或新的 C++ 构建。

### 14.2 新建独立测试资产

目录：`/Game/CodexText/CombatHitWindows/Fixture`。共十二个资产，四个蓝图编译保存成功，全部十二个资产最终数据校验为 Valid、无错误或警告。

- BP_MeleeTestHero：复制当前正式角色，保留动画；配置本体和 Box 来源，新增 MeleeTestHitBox。没有给正式角色追加组件或来源。
- BP_MeleeTestWeaponInstance、BP_MeleeTestEquipment：复制正式剑的实例及装备定义，只在副本配置来源并替换 InstanceType。
- GE_MeleeTestAttributes：Infinite，Override CombatSet.BaseDamage=20；测试 AbilitySet 不新增 CombatSet。
- DT_MeleeTestCombo、DA_MeleeTestCombo：只保留 Entry → Light.01 的单段入口，没有后续过渡。避免样例 Timeline 缺少 NextAttack 窗口而使 PawnData 配置无效。
- AS_MeleeTest_Body、AS_MeleeTest_Weapon、AS_MeleeTest_HitBox：分别只授予对应 DA_Test 和基础伤害效果。
- DA_MeleeTestPawn_Body、DA_MeleeTestPawn_Weapon、DA_MeleeTestPawn_HitBox：复制当前实际 PawnData，引用测试角色/装备、相应 AbilitySet 和单段 Combo。保留其他原有非攻击 AbilitySet，替换原 AS_LightCombo，不重复授予同 AbilityTag。

来源参数：本体将角色前方 `(120,0,0)` 转换为 Mesh 局部偏移，当前 Mesh 旋转 -90°、Z=-88，因此偏移约为 `(0,120,88)`，球半径 55。Box 使用实际组件 MeleeTestHitBox，位于胶囊前方 `(120,0,0)`，半尺寸 `(50,40,60)`，组件自身 Collision 关闭但策略读取形状。武器使用实际 SkeletalMesh 组件和已核实存在的 Sword_Bone01 骨骼、球半径 25。武器配置只用于验证来源路由，不代表已精调完整刀身覆盖；正式刀身需要根据模型配置刀根/刀尖 Socket。

创建脚本位于 `Tools/Combat/create_melee_fixtures.py`。通过 MCP 的 execute_python 使用项目相对路径执行；脚本拒绝覆盖已有同名资产，不应直接重跑覆盖用户调整后的样例。

### 14.3 用户下一步操作

1. 原生测试阶段已完成：用户重新编译后，第三项测试 MCP 复跑通过。结果见 `Saved/Tests/MeleeMigration/migration_summary.json`，本次日志片段见同目录 `spec_rerun_log.txt`。下一步执行以下玩法验证。
2. 保存并记录 `/Game/Main/Experiences/Exp_HodgeDefaultExperience` 的 DefaultPawnData 当前引用。测试本体时临时选择 `/Game/CodexText/CombatHitWindows/Fixture/DA_MeleeTestPawn_Body`，武器/Box 时分别选择后缀 Weapon/HitBox 的资产；每次重新启动 PIE 让新 PawnData 生效。测试完成恢复原引用。
3. 使用项目已有的带 ASC/HealthSet 的目标；来源 BaseDamage 应为 20，样例窗口为 0.30～0.42 秒、倍率 1.5。无其他修正、无免疫且队伍允许受伤时，目标 Health=100 应变成 70，同一窗口保持接触不再次扣血。只摆放没有 ASC 的静态物体不会触发默认生命伤害。
4. 测试角色仍拥有默认装备。Body 和 Box 来源按配置生效，Weapon 模式只使用装备副本的来源；切换样例不是修改快捷栏或重新授予第二个攻击能力。
5. 正式连招接入需要在 DA_Attack 实际引用的 Timeline 增加独立命中 Window，再给各 Definition 添加同 Tag 的 HitWindows。先由动画确定窗口时间与刀身参数，再按第 11 节验证；当前正式连招不会因仅迁移父类就自动造成伤害。

完整命中判定、实际扣血、取消/重入、重生与多人结果仍未验收；数据校验和蓝图编译不证明这些玩法通过。

## 15. 后续进入游戏验收

用户随后授权完整进入游戏测试，已通过 MCP 执行真实 PIE 与双玩家 Listen Server。上述第 14 节描述的是此前仅完成资产配置和原生回归时的状态；当前玩法结果以 [2026-10-01 PIE 验收记录](melee-pie-validation-20261001.md) 为准。

本体、武器、Box 实际造成预期一次伤害；范围外、窗口结束后进入、取消、免疫、遮挡、Pawn 销毁及重生均有逐帧记录。联机主机攻击通过，客户端首次攻击失败：后加入玩家的服务器 BaseDamage=0。运行时重新应用同一个初始化 GE 后客户端攻击及两端 Health 一致性通过，但这是诊断结果，不代表初始化修复。G 保持未完全验收，本轮没有新增 C++ 修改或构建。


## 16. 复用伤害 GE 与 Experience 组件装配更正

本次移除原生 HodgeMeleeDamageEffect；常规伤害统一引用 `/Game/GameplayEffects/Damage/GameplayEffectParent_Damage_Basic` 或其子类。MCP 已核实该资产是 Instant、包含 HodgeDamageExecution；原资产本身未被修改。DA_Test_Body、DA_Test_Weapon、DA_Test_HitBox 的 HitWindows.DamageEffect 已显式指向它，留空不再自动替代为新 GE。

原伤害 GE 保留原有配置。本轮实际 PIE 核实 BaseDamage=20、倍率=1.5 时，目标 Health 为 100→70。此前仅从序列化 CalculationModifiers 推断额外 +1、预期 68.5 的结论不正确；实际结果还受属性聚合及计算修饰影响，不能仅凭配置文本推断最终伤害；后加入玩家服务器 BaseDamage=0 时，本轮实际造成 1.5 伤害。

正式 `/Game/Main/Experiences/Exp_HodgeDefaultExperience` 的现有 AddComponents Action 保留装备条目，新增 ActorClass=HodgeCombatCharacter、ComponentClass=HodgeCombatComponentBase、Client/Server=true、AdditionFlags=AddUnique。CharacterBase 已注册/注销组件 Receiver，Experience 激活/卸载管理请求；角色构造函数不创建判定组件，死亡、ASC 解绑和 Pawn 销毁时按需查找后清理。检测 Task 校验组件仍已注册，组件卸载后停止采样。GA 继续负责过滤目标、构建和应用 GE，组件只返回几何结果。

HitSources 应配置在 Experience 所选的判定组件蓝图默认值上；CombatComponent 已声明 Blueprintable。更换组件配置时替换对应 AddComponents 条目的 ComponentClass，不同时添加同一职责的基础组件和派生组件。

当前编辑器仍加载修改前 DLL，无法创建该组件的蓝图子类；本次没有执行构建、蓝图编译或 PIE。测试角色原来存在于原生默认子对象上的两条来源配置已保存在 `Tools/Combat/melee_fixture_hit_sources.json`。保存并关闭编辑器、常规编译、重新打开后，执行：

```powershell
python Tools/Combat/configure_melee_fixture_experience.py
```

脚本通过本机 MCP 创建 `/Game/CodexText/CombatHitWindows/Fixture/BP_MeleeTestCombatComponent` 并恢复来源配置，创建独立 `/Game/Main/Experiences/CodexText/Exp_MeleeValidation`，将其中判定组件条目的 ComponentClass 替换为该测试组件蓝图，保留其他 Actions；默认使用 DA_MeleeTestPawn_Body。它不切换正式体验、不启动 PIE。该迁移脚本本轮仅通过语法检查，尚未在重新编译后的编辑器执行。

当前 GameMode 的 WorldSettings Experience 入口仍被注释，不能通过地图的 Default Gameplay Experience 选择测试体验；AssetManager 只扫描 Main/Experiences，因此测试体验放在该目录，并通过地图 URL `?Experience=Exp_MeleeValidation` 选择。用该测试 Experience 的 DefaultPawnData 在 Body、Weapon、HitBox 测试 PawnData 间切换。本轮 Standalone PIE 桥接 play 接口不传 URL，采用临时内存覆盖默认体验的 Actions/PawnData，结束后恢复，不保存正式资产。原第 15 节修改正式 Experience 的方式不再适用于带专用来源配置的测试。先检查每个 Pawn 只有一个判定组件，再复测检测、取消、卸载、死亡和重生；此前 PIE 报告对应旧版本，不能证明本轮更正已通过。后加入玩家基础伤害初始化问题仍待修复。


第 16 节更正后的实际运行结果见 [Experience 与既有伤害 GE 复跑记录](melee-experience-ge-validation-20261001.md)：正式玩法 14/15，通过运行时补应用初始化 GE 的诊断通过；原生自动化 4/5，测试缺少显式 GE 的源文件已修正但未重新编译。组件蓝图及扫描目录问题已完成实际资产修正，客户端数值初始化仍待修复。


## 17. 统一 CombatComponent 的实施

外部仅通过 Pawn 的 HodgeCombatComponentBase 处理连招输入、授权、网络确认和命中检测。保留现有类名与 HitSources 属性，既有组件蓝图和 Experience AddComponents 继续使用；删除 PlayerState 的默认 HodgeComboComponent 及原类文件。连招状态是当前 Pawn 的执行状态，换 Pawn 后重新开始，不跨重生延续输入缓存。

统一组件的实现集中在 HodgeCombatComponentBase.cpp，包含几何会话、连招协调、RPC、观察者标签及 ASC 绑定。2026-10-05 已合并原 `_Combo.cpp`；后续禁止将同类成员实现按功能拆分到多个文件。不新增挂载组件或 Runtime 模块；连招状态与检测历史各自保存为私有状态。只有当前角色上的 CombatComponent 是对外战斗入口。

输入链路：HeroComponent→ASC→ASC 当前 Avatar 的 CombatComponent.InputPressed→连招转移→Definition/Melee GA。GA 的 CanActivateAbility、执行开始/结束、窗口变化和 Point 通知，以及 ASC 的服务器激活验证、服务器发起激活确认，全部查找当前 Avatar 上的组件，不再查找 ASC Owner/PlayerState。

检测链路不变：Timeline 窗口→GA→WaitHitResults Task→组件几何会话→FHitResult 批次→GA 目标资格/去重/Spec/GE。合并后组件仍不应用伤害 GE；所有已有伤害资产、AbilityDefinition、ComboDefinition 和 Timeline 数据结构保留。

组件注册和 BeginPlay 通过 PawnExtension 的 RegisterAndCall 订阅 ASC 初始化，并从 PawnData.ComboDefinition 配置连招；已初始化 ASC 可以立即绑定，先添加组件时则等待回调。重复绑定幂等，未配置 ComboDefinition 时仍可作为几何检测器。没有 PawnExtension 的其他角色可以显式调用 Configure，但传入 ASC 的 Avatar 必须是组件 Owner。

ASC 换 Avatar 前清理旧 Pawn 的组件；死亡、ASC 解绑、组件注销和 EndPlay 清理输入、当前攻击、节点标签、授权及全部检测会话。PawnExtension 新增 UnregisterAbilitySystemDelegates，组件注销时只移除自己的回调。组件拒绝操作已被其他 Pawn 接管的 ASC；同步激活回调若销毁或更换 Pawn，ASC 不会继续完成旧组件的确认。

复制与 RPC：组件默认开启复制，ServerMoveCancel、ServerReturnToEntry 和 ClientMoveCancelResult 迁入该组件，继续校验 Avatar、节点、能力句柄及预测键；ObserverTags 使用 COND_SkipOwner。拥有者使用预测节点标签，模拟代理应用服务器标签。复制先于 ASC 绑定时缓存 ObserverTags，绑定完成后补应用，并记录实际应用的计数以避免重复添加；旧组件解绑只撤销自身持有的标签。

UE 5.5 的 GameFrameworkComponentManager 对复制组件仅在服务器创建，客户端经 Actor 组件复制接收，因此 Experience 的客户端条目不会再自行生成第二份复制组件。正式 Experience 及测试体验都需要保留服务器 AddComponents 条目、AddUnique；组件蓝图编译后确认 Component Replicates=true，不要额外手工挂载另一份 CombatComponent。

组件 Tick 仅处理输入超时、连招事件和移动取消，按有效连招配置启停；几何检测仍由 GA 所属 Task 在 PostPhysics 调度。不能把统一组件 Tick 当作攻击窗口的碰撞采样时钟。

已迁移现有 Hodge.Combo.SessionRules 测试到 Avatar 上的 CombatComponent，增加当前 Avatar 查找、ASC Owner 无第二组件、旧 Pawn 拒绝新 Avatar 输入的断言。Tools/BasicAttack 的七个既有回归脚本也改为从 PlayerState 对应 Pawn 获取 CombatComponent。静态检查确认 Source/Tools 无旧组件类引用、14 个 BasicAttack Python 脚本语法可解析、两个组件 cpp 的 46 个成员实现与头文件匹配且无重复，以及本轮相关文件 git diff --check 通过；Content/Config 的原始类名扫描未发现旧组件名，不等于蓝图编译验证。本轮只编码和文档更新，未执行构建、蓝图编译、自动化、PIE、联机或打包；前两轮玩法结果不能证明合并后通过。

用户下一步：保存并关闭编辑器，常规编译后重新打开；编译受影响的角色/PlayerState/组件蓝图，检查旧类或失效节点，并核实每个 Pawn 只有一个复制 CombatComponent。执行 Hodge.Combo 与 Hodge.Combat 回归，再验证完整连招的输入缓存、连续转移、移动取消、服务器事件转移、主机/客户端、观察者标签、重生与组件卸载。已有后加入玩家 BaseDamage 初始化问题仍需单独修复，本次未改其时序。


## 18. 统一组件后的运行回归（2026-10-01）

用户编译并重启编辑器后通过 MCP 实际复跑：Hodge.Combo 4 项与 Hodge.Combat 5 项全部通过，两个测试蓝图 compile/save 成功。单人连招 13 个断言、客户端连招及网络重生 17 个断言、攻击中移除组件 3 个检查、服务器 Point/自然结束转移 3 个检查全部通过。

本体、武器、命名 Box、取消及重生等 10 项伤害用例通过；Listen Server 主机伤害通过，客户端首次伤害仍失败，服务器 BaseDamage=0、拥有端=20、目标两端均为 98.5。组件统一不能视为解决该属性初始化缺陷。运行设置与临时配置均已恢复，完整证据及未验证边界见 [统一 CombatComponent 回归报告](combat-component-unification-validation-20261001.md)。
