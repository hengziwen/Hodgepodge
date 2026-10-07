# 技能检测体扩展方案

日期：2026-10-06。状态：配置检测体、多段攻击和独立 Definition 技能入口已实现；独立技能 Actor 仍为后续设计。下文保留原设计思路，实际字段与配置以[使用说明](skill-hit-volumes-usage.md)为准。尺寸、时间和倍率是设计示例，不是《鬼泣5》的实际参数。

## 1. 建议与范围

推荐组合：短暂攻击使用“配置驱动的空间查询”，需要独立存续的领域、次元斩实体或投射物使用“技能 Actor”。二者复用几何查询、目标规则和伤害提交契约。不要为每个技能在角色身上新增常驻碰撞组件。

一次范围攻击至少需要区分：何时命中、形状、尺寸、中心位置、位置是否跟随、目标过滤、命中次数、伤害参数、取消行为。大范围并不天然需要 Actor；独立生命周期才是引入 Actor 的主要理由。

本提案不修改当前普攻资产，不推测《鬼泣5》内部代码。现有 BoxComponent 路径保留兼容，但不作为新增技能的默认配置方式。

## 2. 已核实的项目事实与缺口

- `UHodgeAbilityDefinition.HitWindows` 按精确 WindowTag 匹配 Timeline Window；同标签不同时间段共享绑定，不按数组下标匹配。
- `UHodgeGameplayAbility_Melee` 接收窗口回调，服务器创建检测会话，按目标 ASC 去重，然后应用显式配置的伤害 GE。
- `UHodgeCombatComponentBase` 由 Experience 注入 Pawn，解析角色／装备来源并保存检测会话；采样由技能 Task 调度，不由组件连段 Tick 驱动。
- `FHodgeHitDetectionSession` 已有 ExecutionId、EventIndex、句柄、上一帧几何；同一技能实例再次激活会获得新的执行身份。
- 当前 SocketSweep 使用来源半径；BoxSweep 必须接收 UBoxComponent，尺寸来自 GetScaledBoxExtent。
- 当前半角和视线过滤以 GetOwner() 的位置／朝向为基准。远处次元斩需要可配置的过滤基准，不能只添加世界位置。
- 当前 WaitHitResults 绑定活动 GA、当前 Avatar 和活动 Timeline，GA 结束、死亡或换 Pawn 后停止。不能用它直接承载“GA 结束后继续存在”的领域。
- Timeline Point 已存在，但只广播 Tag，Definition 的 OnPoint 目前将其转交连段协调器；没有 Point 命中绑定、带条目下标的 Point 命中回调。
- **Definition 当前不是通用独立技能入口。** CanActivateAbility 要求 Combat.IsAuthorized；ASC 的服务端激活也对 Definition 子类执行连段请求校验。独立技能不能直接复制普攻 GA 后绑定一个新按键就视为可用。
- 当前 DamageEffect 必须显式配置。Overlap 返回目标组件，不提供真实的武器接触点；结果归一化时不能编造骨骼、物理材质和阻挡法线。

事实依据：

- [检测类型与策略](../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)
- [检测策略实现](../../Source/Hodgepodge/Private/Combat/HodgeHitDetection.cpp)
- [组件与检测会话](../../Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp)
- [命中 Task](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.cpp)
- [近战结果处理](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp)
- [Definition 执行](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp)
- [ASC 激活入口](../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp)
- [Definition 校验](../../Source/Hodgepodge/Private/Data/HodgeAbilityDefinition.cpp)

## 3. 方案一：Definition 配置检测体，推荐作为主线

### 3.1 职责与数据位置

Timeline 只保存触发时间、窗口／消息标签和事件顺序；几何尺寸不放到 Timeline。Definition 保存本技能的空间配置、伤害与命中次数；Profile 保存可复用的查询和过滤政策；CombatComponent 保存独立的运行会话。

第一阶段不新增空间配置 DataAsset，直接在命中绑定中嵌入结构，避免“每招多建一个资产”。确实存在多技能共享的几何模板时，再增加模板引用和显式覆盖规则。

拟在 `FHodgeHitWindowBinding` 中增加 `Volume`，类型为 `FHodgeHitVolumeConfig`。旧资产默认 `GeometryMode=ExistingSource`，保留武器采样和组件盒体行为。

`Volume` 的拟议字段：

- `GeometryMode`：ExistingSource / ConfiguredShape。前者沿用当前来源；后者从本绑定生成虚拟检测体。
- `Shape`：Sphere / Box / Capsule，仅在 ConfiguredShape 下显示。扇形后续用基础形状查询加夹角过滤，不先引入复杂多边形碰撞。
- `SphereRadius`：球体半径，厘米。
- `BoxHalfExtent`：盒体三个半尺寸，厘米；(200,100,150) 表示完整尺寸 400×200×300。
- `CapsuleRadius`、`CapsuleHalfHeight`：胶囊参数，半高包含端帽且不得小于半径。
- `LocalTransform`：相对空间锚点的位移和旋转。配置检测体默认不继承锚点缩放，尺寸直接使用厘米值；旧组件盒体继续读取组件缩放尺寸。
- `AnchorKind`：AvatarRoot / RegisteredSource / ExecutionTransform。
- `AnchorSocket`：RegisteredSource 可选的单个 Socket；与武器刀根刀尖采样列表分开。
- `AnchorKey`：ExecutionTransform 使用的运行期锚点标识，如选定的次元斩中心；由当前 GA 执行提供，不能写进共享资产。
- `TransformPolicy`：Follow / SnapshotOnEventEnter。Follow 每次采样读取；Snapshot 在本窗口或 Point 开始时解析一次，之后固定。

世界形状由“解析锚点 → 应用局部变换 → 使用配置尺寸”生成。角色根组件与 Mesh 的局部轴可能不同，因此来源必须明确；不默认为 Mesh 的 X 轴就是角色前向。

Profile 拟议扩展：

- `QueryMode`：Sweep / Overlap。Sweep 覆盖上一帧到当前帧；Overlap 只查询本次位置。
- `SampleMode`：EveryFrame / OnceOnEnter；固定周期采样留到持续领域方案。
- `FilterFrame`：Caster / DetectionAnchor。半角与视线查询使用选择的原点和朝向。AOE 一般使用 DetectionAnchor；是否穿墙由 bRequireLineOfSight 显式决定。

`Radius` 和 `SegmentSamples` 继续属于武器来源采样配置，ConfiguredShape 使用 Volume 尺寸，不修改共享 WeaponInstance 的 HitSources。每个技能独立配置，因此不存在两个技能抢着修改同一个 Box 的尺寸。

### 3.2 空间锚点与检测会话

执行期拟增加 `FHodgeHitExecutionContext`，由当前 GA 持有：ExecutionId、弱引用 SourceASC／SourceAvatar、经过服务器验证的锚点映射、锁定时间。目标位置进入此上下文，不进入 Definition 或策略 CDO。

阶段一至少提供原生虚函数 `PrepareHitExecutionContext`，在 Timeline 激活之前调用。后续提供受限的蓝图接口设置锚点；已被固定会话消费的锚点不被原地修改。需要多个独立中心时，使用不同 AnchorKey 或不同 Actor 实例。

会话增加：已解析的 Volume、固定变换／当前锚点、过滤变换、源身份及采样状态。会话仍以 ExecutionId + EventIndex + SessionHandle 验证，不以 EventID 名称决定技能语义。

修改 Capture 接口，使其消费解析后的空间输入，而非强制以 USceneComponent 作为所有请求的前提。ExistingSource 分支仍解析角色或武器组件；ExecutionTransform 分支不要求 SourceComponent 非空。对应调整 IsSourceValid 和 Task 的 Tick 前置依赖；只有存在组件时才添加组件更新依赖。

配置检测体至少添加一个统一形状策略。球体／盒体／胶囊是配置类型，不必每种形状新增一个组件。策略默认对象保持无状态，上一帧几何仍属于会话。

### 3.3 Window 与 Point 的触发方式

短时间持续检测用现有 HitWindows。例如挥刀窗口 EveryFrame + Sweep；固定爆发窗口 OnceOnEnter + Overlap。OnceOnEnter 是新行为，不能仅用 RepeatHitInterval=0 代替：后者仍会逐帧查询，也会命中稍后进入范围的敌人。

精确单次爆发建议进一步新增 `HitPoints`，以 `PointEventTag` 绑定 Timeline Point；共享几何／效果参数结构，Trigger 字段按 Window 与 Point 分开。示例消息标签 `GameplayEvent.Attack.Hit.Pulse` 需要正式注册，不能使用状态标签假装一次性消息。

Point 接线必须补齐：

1. 保留现有 OnPoint(Tag)，增加带 EventIndex 的内部回调，避免破坏现有消费者。
2. Definition 提供 `OnExecutionPoint(EventIndex, Tag)`，先检查本次身份；现有连段消息转交逻辑保留并明确调用顺序。
3. 攻击执行者按 PointEventTag 找到 HitPoints 绑定，创建一次性会话。
4. 登记句柄和结果接收状态后只采样一次，回调处理结束后释放会话。不能先删除会话，再调用现有 IsMeleeBatchCurrent 验证结果。
5. 用 EventIndex 区分同标签的多个 Point；每个条目最多消费一次。帧跨过多个触发时刻时逐项处理，不丢中间段。
6. 开启 Point 命中时只在权威端提交伤害，独立防重复；预测端可以播放表现。不得直接向 Source ASC 应用 PointEffectClass 来冒充范围伤害。

### 3.4 独立技能的激活契约

建议在 Definition 增加 `ExecutionRoute=ComboCoordinated / Standalone`，旧资产默认为 ComboCoordinated。空间能力可以先用连段测试节点验证，独立按键技能必须完成本节后再接入。

ComboCoordinated 保持当前 AuthorizedHandle、连段 Payload、确认及记忆流程。

Standalone 通过已授予 AbilitySpec、正常 GAS 输入／激活、CommitAbility、消耗／冷却、ActivationGroup 和所需标签控制。客户端提交目标位置走目标数据通道，由服务器验证；不能仅删除 IsAuthorized。

需要同时修改：

- Definition.CanActivateAbility 的路由判断。
- ASC.InternalServerTryActivateAbility 的 Definition 专用连段校验分支，只对 ComboCoordinated 执行。
- 服务端主动激活的客户端确认处理；Standalone 使用正常 GAS 确认，不套用连段专用确认。
- ClientActivateAbilityFailed 的连段记忆纠正，只针对 ComboCoordinated；武器预测请求的拒绝恢复仍覆盖两类技能。
- Definition 的 ExecutionStarted／Ended、WindowsChanged、OnPoint 等连段协调通知，只对 ComboCoordinated 转交。
- 编辑器校验禁止把 Standalone Definition 作为连段节点；技能授予仍通过已有 Definition 与 Spec 关联。

Standalone 攻击按 ActivationGroup 中断正在进行的普攻后，普攻正常 EndAbility 保留连段记忆。Standalone 自己不写入 CurrentComboTag，不擦除记忆。

### 3.5 审判斩示例：固定大范围、多段爆发

以下参数仅供方案评估：

- GeometryMode=ConfiguredShape，Shape=Sphere，SphereRadius=800。
- AnchorKind=AvatarRoot，LocalTransform.Location=(0,0,-90)，根据实际胶囊中心调整到地面附近。
- QueryMode=Overlap，FilterFrame=DetectionAnchor，HalfAngleDegrees=180。
- 在施法准备时将施法中心写入 AnchorKey=CastCenter，后续三个爆发都使用同一 ExecutionTransform。不能让三个 Point 分别重新读取移动后的角色位置。
- Timeline 在 0.8、1.1、1.4 秒各放一个 Point，PointEventTag 相同即可复用一个绑定；EventIndex 不同。
- HitGroup 留空：每段对每个目标最多命中一次，三个 Point 最多三次；若三段只允许总共命中一次，使用相同 HitGroup 且 RepeatHitInterval=0。
- DamageMultiplier 根据每段需求配置；不同倍率使用不同 PointEventTag 与不同绑定。
- 视觉上几十道斩击由 Cue 表现，伤害只在这三个明确时刻查询；特效数量不决定碰撞查询数量。

不需要常驻 Box，也不需要生成几十个伤害 Actor。若技能被取消，未执行的 Point 不再造成伤害。

### 3.6 次元斩示例：锁定目标位置、延迟命中

- GA 从锁定系统／瞄准获得候选目标，服务器验证后记录位置至 SlashCenter。
- Shape=Sphere，SphereRadius=150；或使用 BoxHalfExtent=(120,120,180) 表示较高区域。
- AnchorKind=ExecutionTransform，AnchorKey=SlashCenter；目标移动后中心是否跟随由技能明确决定。
- 例如 0.45 秒播放位置特效，0.65 秒 Point 执行 Overlap。此例“固定位置”不跟随目标，也不跟随施法角色。
- 如果要追踪目标直到命中，则上下文保存弱引用目标，由服务器在命中时解析最新位置；目标失效采取取消或固定最后位置，禁止退回角色原点。
- 目标资格仍走 HodgeDamageRules，伤害仍使用显式 Instant GE + HodgeDamageExecution。

若该次元斩在 GA 结束前完成，方案一足够。若要连发三个次元斩且前一个在下一次 GA 激活后继续存在，采用方案二承载它们。

## 4. 方案二：独立技能 Actor，作为方案一的补充

### 4.1 何时使用

适用于独立飞行的剑气、发射后延迟命中的次元斩、持续几秒的领域，以及需要同时存在多个独立实例的效果。角色身上仍不增加新的常驻组件。

拟新增 `AHodgeAttackVolumeActor` 和 `UHodgeAttackVolumeDefinition`。前者只承载独立位置、计时和执行身份；后者保存形状、持续时间、脉冲间隔、过滤、伤害配置和表现配置。移动投射物确实需要物理碰撞／运动组件时再派生，不让所有静态范围攻击承担其开销。

### 4.2 执行与生命周期

1. GA 校验、Commit 成功并取得目标位置后，服务器生成 Actor。
2. 在 Actor 启动之前配置 SourceASC、SourceAvatar、独立 VolumeId、世界变换、出生时间、效果数据。使用延迟生成或显式 Initialize→Start，避免 BeginPlay 在配置完成前造成伤害。
3. Actor 用独立的计时和命中记录执行一次爆发或周期脉冲，调用同一几何查询契约；不继续依赖原 GA 的活动 Timeline。
4. 客户端由复制参数／GameplayCue 播放表现；不提交伤害。晚加入客户端根据服务器时间恢复当前阶段，不能重新从头播放。
5. Lifetime 到期、取消、来源失效时，统一停止计时、查询和表现并释放对象。

拟议 `LifetimePolicy`：

- BoundToAbility：GA 结束就终止。
- Independent：成功生成后允许 GA 正常结束，Actor 按自己的 Lifetime 运行；是否受 GA 取消影响另设明确政策。

默认角色死亡、Pawn 更换或卸载相关 GameFeature 时停止。**第一版不承诺来源角色销毁后继续造成伤害**：当前 HodgeDamageRules 和 GE 来源依赖有效的 Source Avatar；若未来要支持死亡后投射物命中，需要另设持续来源和阵营快照契约。

### 4.3 共享查询与伤害，不复制整套近战 GA

当前 Task 活动检查强依赖 GA，不能让 Actor 假装 GA 活着。Actor 需要独立执行身份和采样驱动；公共几何服务接受解析好的世界形状，不以组件必须属于角色为前提。

把可复用的命中归并、效果 Context／Spec 构建和服务器提交整理成无状态的项目自有辅助类型，例如 `FHodgeHitEffectExecutor`。调用方持有命中历史；GA 保留现有 CanApplyMeleeHit／ApplyMeleeHitEffects 覆盖入口，Actor 也能调用基础契约。不能让 Actor 持有一个已经结束的 GA 作为长期伤害执行者。

若 Actor 范围也由 CombatComponent 保存会话，需要增加受控的 RuntimeActor 锚点：仅允许当前服务器创建且绑定当前来源身份的实例，不接受客户端随意传入组件。组件死亡／解绑清理与 Actor 清理幂等配合。

需要对象池时，只池化 Actor，不共享执行状态。复用前重置 VolumeId、来源、计时器、命中历史、委托、可见性和网络阶段；远端收到的新身份必须重置旧表现。未测量生成压力前先不引入池。

### 4.4 连发次元斩示例

第一次生成 Actor A，中心在目标位置 P1；第二次生成 B，中心 P2；第三次生成 C，中心 P3。各自 0.2 秒后造成一次伤害，再结束表现。

A／B／C 拥有自己的 VolumeId 和中心，彼此不会覆盖。施法 GA 结束后它们是否继续由 LifetimePolicy 决定。角色蓝图里仍没有三个额外 BoxComponent。

领域示例：Lifetime=3 秒，PulseInterval=0.5 秒。每次脉冲独立去重，或显式共享整段历史决定总次数。命中间隔 RepeatHitInterval 与查询间隔 PulseInterval 分开：前者限制某个目标的受击频率，后者控制实际查询频率。

## 5. 方案三：复用一个运行期 BoxComponent，仅适合作为过渡

在技能开始时调整一个通用 Box 的位置和尺寸，结束后归还控制权，确实比“一招一个 Box”少组件。

但两个技能／检测窗口同时存在时会争用变换和尺寸；需要租用句柄、配置恢复和池化，远处多个区域还要多个实例。最终仍接近方案二，而且当前会话只复制来源信息，每次 Capture 仍读取组件尺寸，运行中改 Box 会改变旧会话。

因此不推荐作为正式主线。只有明确所有攻击完全互斥、已有编辑器组件调试需求时才考虑；既有 Box 路径继续兼容即可。

## 6. 共同需要解决的细节

### 6.1 命中结果与过滤

Overlap 结果需包含真实目标 Actor／Component、检测原点和采样身份。可尝试目标碰撞最近点生成用于表现的代表位置；失败时使用目标位置。标明 HitKind=Overlap，骨骼、材质和法线缺失保持缺失，不伪称真实扫掠接触。

GA 默认路径按目标 ASC 合并多个碰撞组件。远处攻击使用检测中心进行半角和 LOS 过滤；能否穿墙、能否打不同楼层由配置决定。使用窄高度盒体或增加高度过滤可避免球体把楼上目标一起命中。

### 6.2 连段去重与并发

同一标签不同时间段可复用绑定，但每个 EventIndex 独立保存会话和历史。HitGroup 是同一执行内的共享去重组，不自动跨 GA 或 Actor。

不要用每目标的冷却间隔模拟严格的三段伤害，尤其当敌人在范围内中途进入时。严格多段用明确的 Point／脉冲，每段有独立身份。未来若要跨 Actor 合计命中次数，需要额外显式共享 GroupId，而不是复用一个字符串组名当全局命中记录。

### 6.3 武器手持与远程攻击

当前 WeaponUseWindowTag 会要求所有 HitWindows 都被手持窗口覆盖。这对挥刀合理，对已经放出的远程次元斩不一定合理。

拟为绑定增加 `RequiresWeaponInHand`：默认 true 保留既有覆盖校验；虚拟空间攻击可显式 false。WeaponUseWindowTag 仍控制拔刀／收刀表现，既有武器来源解析不依赖可见模型回背。

HitPoints 也需使用同一检查：要求手持的 Point 必须落在有效手持窗口内；独立领域不要求手持。不得为了通过校验而把武器一直锁在手里直到领域结束。

### 6.4 服务器与目标数据

客户端可以预测蒙太奇、次元斩预览与特效；最终位置和伤害由服务器确认。TargetData 只提供候选目标／位置，服务器验证范围、合法目标、墙体、当前 Avatar 和技能状态，不信任客户端传入的命中名单、半径或伤害倍率。

同一次执行中多段坐标按执行身份存储；单机也使用同一契约。帧率低、倍速播放、网络延迟、连续取消均不能重复提交同一个触发。

### 6.5 调试与内容校验

拟沿用 HitBox 类别的 Debug 开关显示配置范围，固定区域画实际中心，跟随区域画实际轨迹。新增日志记录技能资产、ExecutionId／VolumeId、EventIndex、锚点、查询次数和最终有效目标数。

编辑器预览可显示半透明几何／调试线和时间轴游标；这些属于 HodgeAbilityEditor，不在 Runtime 引入 Editor 依赖。无需把预览几何序列化成角色碰撞组件。

校验需覆盖：正有限尺寸、胶囊尺寸关系、缺失锚点、非法触发组合、标签匹配、Point／Window 重复绑定、共享组间隔一致、手持覆盖、独立路由与连段节点冲突。缺失运行锚点明确失败并结束本次查询，不退回坐标零点。

### 6.6 多次挥击的大招：一段技能执行，多段独立命中

以“围绕目标连续挥剑、最后重击”的表现作为设计目标，不声明《最终幻想7》的内部算法或实际段数。

同一次 GA 执行中，可以让 Timeline 包含多个命中条目。假设有八次普通挥击和一次终结：八个不重叠的 Window 使用同一 WindowTag、同一个普通挥击绑定，HitGroup 留空、RepeatHitInterval=0；每个 EventIndex 创建独立历史，因此同一个敌人最多受到八次普通挥击。终结使用另一标签，绑定较大的区域和更高倍率。

命中窗口逐次打开和关闭，不能用覆盖整个大招的长窗口加固定间隔，替代与挥击同步的离散命中。八次挥击不需要八个 GA，也不需要八个 Box；时间条目可以多，配置按实际类型复用。

两种目标契约需明确选择：

- 自由战斗：每段按武器轨迹或配置范围查询。锁定只负责接近和瞄准，敌人可以躲避，其他进入范围的敌人可以被击中。
- 锁定演出：首次命中后，服务器确认并保存目标。后续 Point 只提交该确认目标，逐段验证目标有效、未死亡以及演出保持条件；这是拟议 `ConfirmedTarget` 结果来源，不是几何形状。若设计为保证命中，不再依赖视觉剑尖逐帧碰撞；若设计允许逃脱，逐段检查范围／控制状态。

第一版也可以用“几何查询后只保留锁定目标”的过滤政策，无需立即支持保证命中的 ConfirmedTarget 路径。该过滤不会给范围外目标造成伤害。

锁定目标需要保存 SourceAvatar、目标 ASC／Avatar 的弱引用、ExecutionId 和确认状态。目标死亡／销毁／更换 Pawn 时，取消后续命中；重新选敌必须是显式的服务器策略。控制目标移动、霸体、镜头和攻击者位移是独立功能，由能力生命周期管理，不写到几何策略中。

对于同一段既有剑刃检测又有辅助范围的攻击，需要共享该段的命中记录，避免同一个目标受两次伤害。建议增加去重作用域 `PerOccurrence / ExecutionGroup`：旧配置保持现有 HitGroup 契约；PerOccurrence 的共享标识由执行身份和明确的段身份组合得到，不能把整个大招的所有段放进同一个只命中一次的组。

### 6.7 凤凰位移与多段斩击：轨迹检测和脉冲命中分开

按“角色以凤凰表现突进，途中多段伤害，最后落地爆发”的目标设计，不依据粒子数量推断《鸣潮》的实际伤害段数。

若凤凰是角色自身的技能表现，角色 Pawn 仍承载服务器实际位置。配置盒体／胶囊体以 AvatarRoot 或经过验证的技能轨迹为锚点；角色 Mesh 暂时隐藏不影响此锚点。凤凰 Niagara／残影提供视觉，不作为服务器伤害坐标来源。

示例：0.20～0.32、0.45～0.57、0.70～0.82 秒三个突进命中 Window，最后 0.95 秒一个落地 Point。具体时间和大小须按实际动作调试。每段独立去重；落地点使用本次执行确定的 LandingCenter，配置更大的 Overlap 区域。

需要区分两种玩法：

- 每次穿过目标算一段：每个突进 Window 使用 Sweep，覆盖该段真实运动路径，同段只命中一次。
- 凤凰持续贴着目标，按固定节奏斩击：明确设置多个 Point 或脉冲。各段分别判断范围，玩家未在该段范围内则不受该段伤害；不能由帧率决定伤害次数。

若凤凰独立飞出，而角色留在原地，使用方案二的 Actor 承载权威轨迹，检测锚点跟随 Actor；单纯视觉上的模型变化不需要独立 Actor。

快速位移必须区分连续冲刺与瞬移：连续冲刺应检测沿途；瞬移应清空旧轨迹历史，只检测落点。当前 MaxSweepDistance 会把过大的单帧位移视为传送，只查询新位置，这个保护不能直接承担所有高速冲刺的路径检测。应由实际位移系统提供可采样轨迹或运动分段，在允许的查询预算内执行，不能简单放大阈值让跨场景瞬移扫伤沿途目标。

低帧率跨过多个伤害 Point 时，按各段身份执行且只执行一次；多个事件共享当前角色位置不等于还原了它们各自的历史位置。需要轨迹命中的技能，应从服务器轨迹按事件时间解析位置／分段查询。当前 Socket 采样没有完整姿势回放，不能声称已保证任意低帧率下的每次剑刃接触。

## 7. 推荐实施顺序与改动范围

### 第一阶段：配置形状与静态范围

改动 HodgeHitDetection.h/.cpp、HodgeCombatComponentBase.h/.cpp、HodgeGameplayAbility_Melee.h/.cpp、HodgeAbilityTask_WaitHitResults、HodgeAbilityDefinition 校验。

先实现 ConfiguredShape、AvatarRoot／RegisteredSource／ExecutionTransform、Follow／Snapshot、Overlap／Sweep、OnceOnEnter、过滤原点。通过连段测试节点验证范围，不立即把独立技能硬接进当前连段授权。

验收：原武器／Body／组件 Box 结果保持；同一角色同时两个范围不互相改变；固定范围不随镜头或角色移动；远程范围使用正确 LOS 原点；Overlap 正确合并目标 ASC；结束／死亡／换 Pawn 清理会话。

### 第二阶段：Point 与独立 Definition 技能

增加 HitPoints、带条目身份的 Point 回调、一次性结果处理；实现 ExecutionRoute 在 Definition、ASC 和 Combo 协调上的完整分流。增加准备运行锚点的扩展入口。

验收：低帧率跨三个 Point 仍只发生三段；同标签复用绑定但不漏段；不同倍率独立绑定；Standalone 正常输入／服务端验证／预测拒绝；技能中断普攻后连段记忆保持；原普攻仍不能绕过连段授权。

### 第三阶段：仅在确有独立存续需求时加入技能 Actor

新增独立 Actor／配置类型、公共效果提交辅助、受控的 RuntimeActor 锚点、生命周期与复制。先验证静态延迟爆发，再验证移动剑气；最后有性能证据时增加对象池。

验收：同时三个次元斩；BoundToAbility 与 Independent 两种结束行为；Source Avatar 失效及时停止；100ms 延迟下服务器只扣一次血、表现中心一致；池化复用不继承旧命中历史；持续领域按脉冲而非每帧查询。

所有业务代码仍在 Hodgepodge Runtime。每个项目自有类的非内联实现集中到一个主 .cpp；新增独立类型拥有自己的主文件，不拆出同类功能片段。

## 8. 原设计阶段的交付与验证边界

只新增本设计提案。已读取项目开发约定、现有源码、检测会话、Timeline、Definition 和 ASC／连段授权链；通过 git status 保留已有工作区改动。没有修改 C++、蓝图、GameplayTag 或资产配置，没有执行构建、PIE、联机或打包。

当前可以配置的仅为旧来源／SocketSweep／组件 BoxSweep；本文 Volume、HitPoints、ExecutionRoute、AttackVolumeActor 等均需要后续实现，不应现在在编辑器里寻找这些字段。

## 9. 实施阶段更新

已实现 Volume、HitPoints、ExecutionRoute、服务器锚点／目标、ConfiguredShape、Overlap／Sweep、OnceOnEnter、过滤原点和共享组作用域。旧字段默认保持 ExistingSource／ComboCoordinated，原 SocketSweep、组件 BoxSweep 和连段授权链保留。

实际共用效果配置为 FHodgeHitEffectConfig；Window／Point 各自保留触发标签。Capture 的旧组件接口保留兼容，配置几何在会话内解析后交给 ShapeQueryStrategy。多来源逐段共享去重使用 HitGroupScope=TriggerTime，按相同 StartTime 归并。

已确认目标支持 LockedTargetInVolume／ConfirmedTarget；蓝图通过 PrepareHitExecutionContext、SetHitAnchor、SetHitTarget 提供服务器上下文。未实现通用客户端 TargetData 入口、任意轨迹回放、锁定组件、动画／镜头演出或独立技能 Actor。

已新增 Main 下四个可复用 Profile，以及 CodexText 下独立教学／验证资产。没有替换正式五段普攻的配置。详见[配置说明](skill-hit-volumes-usage.md)与[实际验证记录](../Validation/skill-hit-volumes-2026-10-06.md)。
