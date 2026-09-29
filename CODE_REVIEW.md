# Hodgepodge Code Review 标准

适用于本项目 C++、蓝图、数据资产、配置和插件接入变更。目标是先发现权威、生命周期、战斗结算和复制错误，再处理维护性问题。本文是审查标准，不是当前代码已通过审查的声明。

## 使用方法与证据要求

1. 先读 [AGENTS.md](AGENTS.md) 和 [AI 开发与验证流程](Docs/AI_DEVELOPMENT.md)，再检查当前源码、配置、资产与变更范围。
2. 明确审查对象：提交区间、暂存区、未暂存修改及相关未跟踪文件。先记录 `git status --short`；不得只读 `git diff` 就漏掉暂存或未跟踪实现。
3. 追踪入口、调用者、数据来源、执行端、退出和清理路径。本文共 **96 项**，按变更选择相关章节；勾选代表已有证据，未勾选不自动代表缺陷。不适用项写 `N/A + 原因`，无法确认写“待验证”。
4. 每条问题必须指出位置、触发条件、实际影响、依据和最小修复方向。检查清单是调查入口，不能仅因“看起来不符合模式”就报错。
5. 区分本次引入的问题、已有问题和后续设计建议。已有问题只有被本次改动触发或扩大时才作为本次阻塞项；其他问题单独记录。
6. 只读审查不自动修改、暂存、提交、重存资产或整理第三方插件。审查通过也不等于授权合并或发布。

## 当前项目定位（2026-09-28 工作区快照）

以下包括尚未提交的源码，只用于定位检查入口，后续审查需重新核实。

- `Hodgepodge.uproject` 关联 UE 5.5；本机开发约定为 UE 5.5.4。业务维持单一 `Hodgepodge` Runtime 模块；当前有 Game / Editor Target，没有专用 Server / Client Target。不得宣称 Dedicated Server 构建已获支持。
- 玩家 ASC 是 `AHodgePlayerState` 持有的 `UHodgeAbilitySystemComponent`，使用 Mixed；Avatar 是当前 Pawn。核对 PlayerState、HeroComponent、PawnExtension 和角色复制入口的实际协作，不另造一套并行初始化。
- 当前 `UHodgePawnData` 已暴露 `AbilitySets`、`ComboDefinition`、`TagRelationshipMapping`、`InputConfig`、`DefaultCameraMode`；GameMode 已调用 `PawnExtension.SetPawnData`。旧开发文档中的“仍注释”描述不再是当前实现。
- 战斗执行已存在 `UHodgeAbilityDefinition`、`UHodgeGameplayAbility_Definition`、`UHodgeComboDefinition`、`UHodgeComboComponent` 和 `UHodgeAbilityTask_PlayTimeline`。Definition 技能通过 ASC 的 SpecHandle 映射查定义，保留原 `SourceObject` 来源。
- Definition 路径使用本次 Montage 实例时钟；独立 Timeline Task 仍有自己的播放参数和时间推进路径。不能把两种路径的时长、倍率规则混为一谈。
- 已有 `Hodge.Timeline.*` 与 `Hodge.Combo.*` 自动化测试源码。存在测试不代表当前工作区已编译或运行通过。
- Build.cs 当前已有 UMG / Slate / CommonUI / CommonInput 依赖和 Editor 条件依赖。插件启用、目录存在或代码声明不等于端到端功能可用。
- 完整命中身份协议、延迟补偿、专用服务器发布等需逐项核实；本文相关条目是新增/修改该能力时的要求，不宣称它们已实现。

主要源码入口（头文件在对应 `Public` 目录）：

- 初始化与授予：[PlayerState](Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp)、[GameMode](Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp)、[HeroComponent](Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp)、[PawnExtension](Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp)、[AbilitySet](Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp)。
- 技能与连击：[ASC](Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp)、[Definition Ability](Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp)、[ComboComponent](Source/Hodgepodge/Private/Component/HodgeComboComponent.cpp)、[Timeline Task](Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.cpp)、[Montage 扩展](Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent_Montage.cpp)。
- 数据契约：[AbilityDefinition](Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[ComboDefinition](Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)、[Timeline](Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)、[PawnData](Source/Hodgepodge/Public/Data/HodgePawnData.h)。
- 属性与伤害：[HealthSet](Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp)、[DamageExecution](Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeDamageExecution.cpp)、[EffectContext](Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayEffectContext.cpp)。

## 严重程度与结论

按可证明的影响定级，不按某个关键词机械定级。

- **BLOCKER**：可触发崩溃、客户端伪造权威结算、严重状态损坏/永久失控，或必需目标无法构建。合入前必须修复。
- **MAJOR**：正常玩法路径失效、取消后残留效果、预测拒绝不能恢复、重复命中结算、可证明的显著性能退化等。默认修复后合入；延期需维护者明确接受影响和补救计划。
- **MINOR**：局部维护性、接口或健壮性问题；说明实际影响，不借机要求大范围重构。
- **NIT**：不影响行为的命名、注释、排版建议。单独列出，不淹没重要问题，不独立阻塞合入。

结论使用“通过 / 需修改 / 待验证”。存在未解决 BLOCKER 时只能“需修改”；关键验证缺失时不能以“没看到问题”代替“通过”。

## 01 · UE C++ 与构建边界

- [ ] **CPP-01**：保持 UE 5.5 兼容；新增 API 对照本机引擎头文件/实现核实，不为照搬示例升级引擎、插件或构建设置。
- [ ] **CPP-02**：业务维持 `Hodge` 命名、UE 类型前缀与 Public / Private 对应布局，不仅为整理目录新增模块。
- [ ] **CPP-03**：`.generated.h` 是头文件最后一个 include；反射实现沿用 `UE_INLINE_GENERATED_CPP_BY_NAME`，导出宏与反射声明匹配。
- [ ] **CPP-04**：include 自足，前置声明适用；公开接口暴露的依赖在 Build.cs 中可见，不靠 PCH 或间接 include 偶然编译。
- [ ] **CPP-05**：Editor 类型、函数及模块依赖有正确条件边界，非编辑器 Game 构建能覆盖相关路径。
- [ ] **CPP-06**：const、参数传递、访问权限与返回值表达真实契约；Blueprint 可写属性和可调用函数只暴露必要权限。
- [ ] **CPP-07**：函数职责可解释，重复逻辑和业务常量有明确归属；不以任意行数上限或个人排版偏好代替审查。
- [ ] **CPP-08**：函数体内中文 `//` 注释每块最多 3 行，一行说明一件事；不逐行翻译代码。类头/文件头 Doxygen 不受此限，保留已有 `Dafult`、`PlayState` 等引用相关拼写。

## 02 · UObject、生命周期与重入

- [ ] **LIFE-01**：跨帧 UObject 引用有明确保活或弱引用策略；裸指针、Outer、非反射容器不被误当作自动 GC 保活保证。
- [ ] **LIFE-02**：弱引用在使用时验证；Actor 销毁、EndPlay、World 结束和 Pawn 替换后不访问旧对象。
- [ ] **LIFE-03**：Delegate、Timer、输入绑定、组件请求和异步句柄记录所属实例，正常结束、取消、失败和卸载均可释放。
- [ ] **LIFE-04**：Lambda/异步回调不无保护地捕获会销毁的 `this`；回调核对对象、World 和本次执行身份。
- [ ] **LIFE-05**：初始化、解绑和清理幂等；重复回调不重复授予、重复注销、重复 End 或扣除别人持有的计数。
- [ ] **LIFE-06**：广播事件、应用 GE、结束技能等可重入调用之后重新确认活动状态；旧调用栈不能继续修改新技能实例的状态。
- [ ] **LIFE-07**：CDO、共享 DataAsset、DataTable 行和 Notify 对象不存放玩家的可变执行状态；每次执行状态归属清晰。
- [ ] **LIFE-08**：工作线程不直接操作不安全的 UObject/GAS/World 状态；属性声明本身不提供线程同步保证。

## 03 · Experience、PawnData、GameFeature 与初始化

- [ ] **INIT-01**：从实际地图/GameMode Override 追到 Experience、PawnData 和 PawnClass，不把 README 或 Lyra 示例当作调用链。
- [ ] **INIT-02**：异步加载、插件激活完成前不提前生成依赖玩法；加载失败有明确终态和可定位日志。
- [ ] **INIT-03**：Init State 的依赖条件、推进与重试完整；不靠固定 Delay 猜 PlayerState、Controller、PawnData 或输入何时就绪。
- [ ] **INIT-04**：服务器 possession 与客户端复制入口最终收敛到同一初始化契约；处理数据不同到达顺序和重复通知。
- [ ] **INIT-05**：玩家 ASC Owner 仍是 PlayerState、Avatar 是当前 Pawn；旧 Pawn 清理前确认所有权，不清掉新 Pawn 的 ActorInfo。
- [ ] **INIT-06**：AbilitySet/Definition 授予由权威端负责；区分 PlayerState 持久资源和 Pawn/装备临时资源，必要时保留可撤销句柄。
- [ ] **INIT-07**：GameFeature 注入和撤销成对，按 World/激活上下文隔离；多 PIE World、重复激活和异步停用无泄漏或串用。
- [ ] **INIT-08**：新配置从反射字段、数据校验、资产赋值到消费入口均接通；不要求用户填写不存在的字段，不把启用插件等同于功能完成。

## 04 · GAS Ability 与 Task

- [ ] **GAS-01**：每个能力说明谁发起、谁预测、谁判定、谁表现；InstancingPolicy、NetExecutionPolicy 和取消策略符合该契约。
- [ ] **GAS-02**：CanActivate 的父类检查、项目 TagRelationshipMapping、ActivationGroup 与业务条件协作，不绕过阻塞/互斥规则。
- [ ] **GAS-03**：成本和冷却明确何时提交；Commit 失败停止副作用，重入不重复扣费；分离提交或无成本能力有明确依据。
- [ ] **GAS-04**：激活后的早退、无目标、无动画、数据非法、取消和死亡路径均能结束活动能力或明确移交生命周期。
- [ ] **GAS-05**：自定义 End/Cancel 保留必要父类语义；结束顺序避免回调再次启动旧流程，且不误停止其他技能。
- [ ] **GAS-06**：Task 在配置/绑定后激活，OnDestroy 释放资源；Task 结束与 Ability 结束的责任清楚，无永远活动的等待。
- [ ] **GAS-07**：InstancedPerActor 再激活重置本次状态；查找使用实际 SpecHandle/执行身份，不仅按类名选取同类多个授予。
- [ ] **GAS-08**：Definition 授予/复制/移除与 ASC 映射同步；原 SourceObject、来源等级和 EffectContext 来源语义不被执行数据覆盖。

## 05 · 网络权威、复制与预测

- [ ] **NET-01**：最终伤害、资源、授予和权威状态由服务器决定；客户端提供意图/候选信息，不直接提供可信最终结果。
- [ ] **NET-02**：Server RPC 校验连接所属对象、当前 Avatar、能力句柄、执行身份、目标合法性和必要的范围/频率，不以 RPC 标记代替校验。
- [ ] **NET-03**：重复、延迟、乱序或上一轮执行的请求不影响当前执行；换 Pawn 后旧请求被拒绝，序号回绕/复用有边界。
- [ ] **NET-04**：PredictionKey 的来源和有效范围正确；延迟回调不盲目复用旧预测窗口，客户端预测与服务器发起执行分支清楚。
- [ ] **NET-05**：拒绝/纠正预测后，Montage、窗口、Tag、输入缓冲和可预测成本能恢复；不假定所有 GE/ExecCalc/伤害均受 GAS 自动回滚。
- [ ] **NET-06**：玩家 Mixed 模式的 owning connection 链正确；修改 Full/Mixed/Minimal 有接收方需求和带宽依据。GE 复制模式不代替 AttributeSet 的属性复制配置。
- [ ] **NET-07**：权威端、自主代理和模拟代理各有明确消费路径；只给 Owner 的数据不成为观察者表现的隐含依赖，OnRep 不假定到达顺序。
- [ ] **NET-08**：Listen Server 同时具备本地与权威身份时不重复执行；可靠 RPC 有流量控制，掉线、晚加入和相关性恢复有相应验证。

## 06 · GameplayTag、GE、Attribute 与 Cue

- [ ] **DATA-01**：沿用项目实际命名空间，如 `Status.*`、`GameplayEvent.*`、`InputTag.*`、`InputIntent.*`、`Combo.*`；不机械替换成示例中的 `State.*`/`Event.*`。
- [ ] **DATA-02**：区分 Tag 的父子匹配与精确匹配、状态与消息；注册、资产引用和重命名迁移一致，编辑器 Categories 过滤不是运行时校验。
- [ ] **DATA-03**：Loose Tag 的本地维护、显式复制和 GE 授予职责清楚；按来源加减计数，不假定 AddLooseGameplayTag 自动同步或用归零删除他人状态。
- [ ] **DATA-04**：Attribute 的 BaseValue/CurrentValue、临时 Buff 和 Damage Meta Attribute 语义清楚；业务修改走项目 GAS 流程，初始化及 AttributeSet 内结算 setter 按具体语义审查。
- [ ] **DATA-05**：数值范围、非有限值、最大值变化和聚合后重算有处理；PreAttributeChange 不能被当作覆盖所有路径的唯一约束。
- [ ] **DATA-06**：AttributeSet 注册、复制声明、OnRep 与 `GAMEPLAYATTRIBUTE_REPNOTIFY` 匹配；属性变化通知驱动 UI，死亡结算不重复。
- [ ] **DATA-07**：GE Duration/Stacking/Period、捕获时机和 SetByCaller 缺失行为明确；ExecCalc 负责计算，不夹带动画/UI；效果上下文来源和失效处理正确。
- [ ] **DATA-08**：Cue 只负责表现，不能成为伤害成立的必要条件；持续 Cue 成对清理，预测与复制去重，专用服务器路径不依赖本地 UI/音效。

## 07 · Combat Timeline 调度契约

本节描述当前 Hodge Timeline 约束；有意修改契约时，需同步实现、校验、资产迁移和回归用例。

- [ ] **TIME-01**：区分独立 Task 时钟与 Definition 的 Montage 实例时钟，Duration/有效时长、PlayRate 和起始偏移只换算一次。
- [ ] **TIME-02**：Montage 以实例 ID 绑定；暂停、倍率变化、同资产重播、被替换、倒退或跳 Section 有明确定义，不能跟上另一轮动画。
- [ ] **TIME-03**：Window 使用 `[StartTime, EndTime)`；同刻按 WindowEnd → WindowBegin → Point，随后同类节点按 Priority 小者优先及稳定索引排序。
- [ ] **TIME-04**：覆盖 0 时刻、精确边界、终点 Point、起始偏移恢复和低帧率跨多个节点；不能因先判断结束而漏掉末尾事件。
- [ ] **TIME-05**：播放前拒绝非有限/非法时长、重复 EventID、非法区间、冲突窗口和不支持的 GE 配置；运行时不能仅依赖编辑器校验。
- [ ] **TIME-06**：EventID 只作身份/诊断，不作业务分支；WindowTag 是状态，PointEventTag 是消息，NetPolicy 只控制 Point，且真实执行分流。
- [ ] **TIME-07**：Window GE 维持 Infinite、无堆叠、独占句柄契约并由权威端施加/清理；Point GE 允许 Instant/HasDuration、禁止 Infinite，且不由窗口清理回收。
- [ ] **TIME-08**：Point 先发事件再应用 GE；事件回调可能结束/替换执行，后续不得继续副作用。Stop/OnDestroy 只释放本 Task 持有的 Tag/GE，清理本身可安全重入。

## 08 · Definition、连击图与输入窗口

- [ ] **COMBO-01**：Definition 的 AbilityTag/AbilityClass/Montage/Timeline 和 Combo 表行引用可解析；校验入口节点、重复标识、悬空边和未授予技能。
- [ ] **COMBO-02**：Ability 负责本段执行、ComboComponent 负责节点/转移/缓冲、Timeline 负责调度；不以资产名硬编码几十套技能流程。
- [ ] **COMBO-03**：InputTag → IntentTag 映射清楚；输入按下/松开、消费/透传规则不会同时触发旧 BasicAttack 路径与 Definition 路径。
- [ ] **COMBO-04**：输入缓冲明确到期时钟、覆盖和消费规则；输入被阻塞、切换 Pawn、死亡、返回 Entry 后无残留意图。
- [ ] **COMBO-05**：转移检查当前执行的窗口、Required/Blocked SourceTags；TransitionPriority 当前为大者优先，不能混同 Timeline Priority，平局行为确定。
- [ ] **COMBO-06**：服务器基于自己的节点/窗口重选转移并核对执行身份；客户端不能通过声称 Timeline 事件已发生来绕过转移条件。
- [ ] **COMBO-07**：切段前预检、结束旧技能、激活新技能和失败回退的次序明确；成本失败、Montage 失败和预测拒绝不留下半切换状态。
- [ ] **COMBO-08**：事件驱动切段、移动取消和返回 Entry 校验身份；事件队列防重入及无界循环，节点 GrantedTags、观察者状态和清理保持一致。

## 09 · 命中、伤害与反应（相关功能变更时适用）

- [ ] **HIT-01**：命中上下文能区分一次攻击执行、攻击段、命中实例和目标；标识的生成端、作用域及生命周期明确，不强制固定字段名。
- [ ] **HIT-02**：明确一次/多次命中规则和去重键；同帧多 Trace、多骨骼 Overlap、多个剑气及重发请求不会产生非预期重复伤害。
- [ ] **HIT-03**：服务器验证目标存在、阵营、距离、碰撞通道及必要遮挡/窗口；客户端 HitResult、伤害值或时间戳不能直接作为可信结论。
- [ ] **HIT-04**：客户端预测 HitSet 与服务器权威 HitSet 职责分开；允许暂时差异，但最终结算和表现需收敛，不要求两个本地集合天然相同。
- [ ] **HIT-05**：碰撞采样覆盖高速运动和低帧率跨越；使用延迟补偿时定义时间边界和校验策略，不能把未实现的补偿当作已有保证。
- [ ] **HIT-06**：取消/死亡/卸载后关闭命中窗口、清理碰撞与任务；脱手弹道如允许继续存在，明确伤害来源与寿命，不一律随 Ability 销毁。
- [ ] **HIT-07**：伤害计算、护盾/生命/削韧处理、反应和表现有明确边界；无敌/格挡/死亡等交互顺序与恰好归零情况可测试。
- [ ] **HIT-08**：新增 EffectContext 字段核对复制需求、Duplicate/深拷贝、NetSerialize 和 Iris 路径；当前 CartridgeID 未额外序列化，不能推定新增命中 ID 自动跨端可用。

## 10 · 动画、Root Motion 与表现

- [ ] **ANIM-01**：GAS 战斗 Montage 沿用 ASC 播放/停止及项目复制扩展；直接 AnimInstance 调用需说明如何保持能力状态和网络同步。
- [ ] **ANIM-02**：检查 Mesh、Skeleton、AnimInstance、Slot/Group 和 Linked Layer 的实际资产配置；编译通过不代表角色能播放正确动作。
- [ ] **ANIM-03**：区分自然结束、BlendOut、打断和主动停止；绑定实际 Montage 实例，旧回调不能结束新技能，丢失回调仍能收尾。
- [ ] **ANIM-04**：BlendIn、NaturalBlendOut、StopBlendOut 和曲线应用于预期实例；不为单次技能修改所有角色共享的 Montage 资产。
- [ ] **ANIM-05**：本地玩家、服务器与远端观察者能重建所需动画参数；Definition/Montage 状态乱序到达时可补齐，不能只验证 Owner。
- [ ] **ANIM-06**：Root Motion、移动组件预测/纠正和 Motion Warping 的权威来源清楚；取消后恢复缩放/运动状态，不额外叠加一套相互竞争的位移。
- [ ] **ANIM-07**：Notify、Timeline 与 Linked Layer 通知传播不重复派发同一玩法事件；离屏/服务器动画 Tick 策略不能悄悄停止必要的权威时钟或命中采样。
- [ ] **ANIM-08**：动画线程使用安全快照/代理；Tag 属性映射在 ASC/Pawn 更换时重新绑定并释放旧关联，表现失败不阻断权威玩法。

## 11 · 性能、资源与错误处理

- [ ] **PERF-01**：Tick 有必要性、活跃范围和成本依据；Timeline/命中采样可用 Tick，但避免空闲轮询、每帧全场搜索和重复昂贵组件查找。
- [ ] **PERF-02**：高频路径不做无依据的同步加载、对象分配或 Tag 字符串查找；缓存有失效规则，不缓存已销毁 Pawn/组件。
- [ ] **PERF-03**：Timeline 节点收集/排序、连击图查询、事件队列和命中集合有规模边界；性能结论给出测量场景，不能仅凭代码外观判“严重卡顿”。
- [ ] **PERF-04**：Primary Asset/软引用有扫描、加载、Bundle/Cook 可达性证据；编辑器内存中已加载不等于新进程和打包后可用。
- [ ] **PERF-05**：异步加载有完成/失败/取消处理，切地图、退出 PIE、Feature 卸载后回调不能恢复过期玩法。
- [ ] **PERF-06**：复制属性和 RPC 按接收方需求控制频率/体积；不为表现广播完整冗余状态，持续日志/Cue 不产生无界开销。
- [ ] **PERF-07**：check 用于程序不变量，外部/资产/网络非法输入有运行时拒绝路径；ensure 后不继续无效解引用，断言表达式不承担不可省略副作用。
- [ ] **PERF-08**：错误日志包含可定位资产/对象、角色或执行身份及原因；失败后进入安全状态，不静默吞错、不每帧刷屏，也不输出敏感信息。

## 12 · 兼容、回归与交付

- [ ] **QA-01**：类/反射属性/Tag/资产路径变化检查蓝图、配置、CoreRedirects 或 Tag 重定向；迁移步骤和兼容边界明确。
- [ ] **QA-02**：二进制资产记录具体路径、父类/属性/图的改动与验证；LFS 指针、文件存在或解析器局部输出不能充当完整资产审查。
- [ ] **QA-03**：C++ 修改按开发流程执行常规 Editor 构建；Runtime/依赖/插件变更再执行 Game 构建，记录目标、配置、退出码和首个有效错误。
- [ ] **QA-04**：测试覆盖本次风险及至少一个关键失败/边界场景；优先扩展现有 Hodge.Timeline / Hodge.Combo 用例，不用镜像实现的测试凑数量。
- [ ] **QA-05**：反射/继承/默认子对象/构建配置变化保存工作并关闭编辑器后常规构建，再打开编译受影响蓝图；不以 Live Coding 成功替代。
- [ ] **QA-06**：网络/GAS 变更至少核对 Listen Server 双玩家的服务器、本地玩家和远端观察者；按风险补独立进程、延迟/丢包、晚加入与重生验证。
- [ ] **QA-07**：交付区分 C++ 构建、自动化测试、蓝图编译、PIE、联机与 Cook/打包的通过/失败/未执行；缺少 Server Target 时不得声称独立服务器发布通过。
- [ ] **QA-08**：结束检查 Git 状态和目标 diff，保留用户已有修改，不自动提交或纳入生成物；文档变更只检查内容、链接与差异，无需启动 UE 构建。

## 验证执行指引

常规构建命令、编辑器关闭要求和环境错误处理以 [AI_DEVELOPMENT.md 的编译验证](Docs/AI_DEVELOPMENT.md#编译验证) 为准。先核实本机引擎路径与实际 Target；其“核实基线”是历史快照，不是永久能力清单。

- **Timeline/Definition/Combo**：构建后在编辑器 Session Frontend 的 Automation 中查找 `Hodge.Timeline` 和 `Hodge.Combo`，运行相关测试并记录完整测试名、结果与日志。源码入口是 [Timeline 测试](Source/Hodgepodge/Private/Tests/HodgeAbilityTimelineTests.cpp) 和 [Definition/Combo 测试](Source/Hodgepodge/Private/Tests/HodgeAbilityDefinitionTests.cpp)。
- **玩法回归**：记录实际地图与 GameMode Override、PawnData、AbilitySet、Definition、Timeline、Combo 表和 Montage 的资产路径；按变更选择正常连段、窗口前/边界/后输入、按住与松开、移动取消、资源不足、动画失败、受击/死亡、换 Pawn 和重复启动 PIE。
- **网络回归**：先以 2 玩家 Listen Server 验证双方发起攻击与对方观察结果；再按风险增加独立进程和网络模拟。明确记录延迟/丢包设置与测试时长，核对服务器窗口、拒绝后的恢复、Tag/GE 残留和过期取消请求。
- **服务器专项**：若本次目标确实包含 Dedicated Server，另外核实 Target、工具链、服务器资产与无渲染环境下的动画/时钟行为；PIE 的服务器模式不能替代服务器可执行文件构建验证。
- **资产交付**：需要手动操作时提供“资产路径 → 属性/节点 → 操作 → 预期结果”。未读取资产属性时标为未确认，不能要求配置源码中尚不存在的字段。

## Review 输出模板

```text
审查范围：提交区间/暂存/工作区/未跟踪文件；相关资产
结论：通过 / 需修改 / 待验证

[MAJOR][TIME-08] 路径:行号 — 简短问题标题
触发：具体输入、时序、网络角色或数据条件。
影响：实际错误及受影响对象。
证据：调用链、代码分支、复现或测试结果；未知部分明确说明。
建议：最小可行修复方向及回归场景。

验证：实际命令/编辑器步骤、目标/模式、退出码或结果、日志位置。
未验证：具体缺口与所需步骤。
已有问题/非阻塞建议：与本次引入问题分开。
```

可直接用于 AI 审查的提示词：

> 阅读 AGENTS.md、Docs/AI_DEVELOPMENT.md 和 CODE_REVIEW.md，审查我指定的变更范围。先核实当前源码和资产调用链，覆盖相关清单项，优先检查权威、预测、生命周期、Timeline/Combo 和重复结算。仅报告有具体证据及实际影响的问题，给出严重级别、检查项编号、文件位置、触发条件和最小修复建议。区分新增缺陷、已有问题和待验证项；没有发现问题就明确说明，不凑数。不要修改文件、提交或重存资产，不把未执行的测试写成通过。

## 参考资料与适用边界

以下用于核对概念和设计依据；本文具体审查契约来自 Hodgepodge 当前实现及项目约定，不能标称为 Epic 官方完整 Review 标准。

- [Epic C++ Coding Standard](https://dev.epicgames.com/documentation/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine)：C++ 风格和类型/接口约定参考；项目相邻风格与明确约定优先。
- [Epic Using Gameplay Abilities](https://dev.epicgames.com/documentation/unreal-engine/using-gameplay-abilities-in-unreal-engine) 与 [Gameplay Attributes / Attribute Sets](https://dev.epicgames.com/documentation/unreal-engine/gameplay-attributes-and-attribute-sets-for-the-gameplay-ability-system-in-unreal-engine)：能力和属性职责参考，具体 API 回到 UE 5.5 源码核实。
- [Epic Abilities in Lyra](https://dev.epicgames.com/documentation/unreal-engine/abilities-in-lyra-in-unreal-engine)：能力、Tag 与项目扩展的官方样例；Lyra 的存在不证明本项目已接入对应系统。
- [tranek/GASDocumentation](https://github.com/tranek/GASDocumentation)：社区作者的 GAS 理解与多人示例，作为补充阅读，不作为覆盖本机版本语义的官方规定。
- [Epic Gameplay Abilities in Action RPG（4.27）](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-abilities-in-action-rpg?application_version=4.27)：历史动作 RPG 样例，只参考职责组织，不直接照搬旧 API。

在线资料可能默认显示更新引擎版本。审查时以 UE 5.5.4 本机 API 和项目事实为准，不因资料版本变化升级项目。不一律禁止 Tick、bool、Attribute setter 或 Full 复制；对共享玩法状态、性能热路径、属性修改及复制策略分别验证其正确性。也不为符合清单强行引入尚无需求的完整战斗框架。
