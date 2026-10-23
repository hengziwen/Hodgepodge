# 战斗评审后续职责迁移方案（第3节已完成，其余待确认）

日期：2026-10-18。基线：`dev-Combat / 891fd70` 与当前工作区。

本文承接[战斗代码评审](combat-code-review-2026-10-18.md)。用户先确认在现有类内收敛入口，后单独批准并完成第3节。**第4～5节仍是待批准方案，不表示其中的新类、接口或资产接线已经实现。** 原类整改见[整改记录](../Validation/combat-review-refactor-2026-10-18.md)，第3节及另行获批补充修复见[计划解析提取记录](../Validation/hit-reaction-plan-builder-2026-10-19.md)。

> 2026-10-18 后续确认：用户选择“本轮只完成修复，拆类继续暂缓”。第 3～5 节均不在当前实施范围；复审发现项及修复结果见[跟进清单](combat-refactor-follow-up-review-2026-10-18.md)与[本轮验证](../Validation/combat-review-follow-up-2026-10-18.md)。

> 2026-10-19 本次确认：用户已单独批准第3节的纯计划解析提取，并确认完整来源快照、保留部分失败输出、后续Profile读取保持原状三项契约。仅恢复第3节，第4～5节和两项暂缓玩法问题保持原范围。实际实施与验证见[计划解析提取记录](../Validation/hit-reaction-plan-builder-2026-10-19.md)。

> 2026-10-20 实施进度：第3节计算提取已完成，独立旧新对照、Editor/Game及全量203项原生回归通过。正式终结刀问题随后获得单独排查/修复授权及[具体转向方案](locked-combo-finisher-fix-2026-10-19.md)批准，单机五段七项、R九项及玩家受击三端基线通过；新增攻击连招联机回归在1→2权威激活处失败，整体验收尚未关闭。该补充行为不是纯计算提取的一部分；第4～5节、Client共鸣窗口及完整生命周期矩阵仍未恢复。

> 同日继续授权：用户已批准独立修复1→2交接，并批准旧转向专项的完整适配。实测确认Deferred激活被引擎清掉已先到达的同Spec/预测键方向数据；现有ASC窄修复保留原校验、窗口及网络格式。新增真实Timer/Super交接原生回归及全量204项通过，Dedicated0ms正式五段通过；100ms玩法检查也通过，但首批编辑器关闭非零退出，仍待另批复验及原有转向专项收口。详细结果仍以[本批验证记录](../Validation/hit-reaction-plan-builder-2026-10-19.md)为准。

> 同日阶段记录：100ms独立复验七项流程与十一项三端检查通过，进程退出0；当时大角度验收仍待确认。此前进度和失败均为历史阶段，不能作为下面最终状态的替代证据。

> 2026-10-20 最终收口：第3节及另行批准的终结刀追踪、Deferred交接、Recovery释放顺序和Warp回放重建修复均已完成。最终Editor/Game构建与205项原生回归通过；转向Dedicated0ms、Listen100ms各11场景83项通过；正式单人五段7项、Dedicated0/100ms各7项流程及11项三端检查通过；确定时序假人专项12场景89项通过。削韧窗口1.5s、Heavy阈值55及其他正式参数保持，五段按真实窗口验收，Heavy左右片由专项覆盖。最终批次均正常退出；此前三次0xC0000409退出原因仍未知并保留。第4～5节、Client共鸣窗口与完整生命周期矩阵仍未恢复，不把本次完成扩大为这些范围通过。

## 1. 已固定的约束

- 一个 `Hodgepodge` Runtime 模块，沿用现有 Editor 模块；每个自有类的非内联实现集中于自己的主 `.cpp`。
- 保留 GAS、Experience、PawnData、GameFeature、Init State、玩家 PlayerState ASC 与敌人 Pawn ASC。
- 不修改伤害公式、普通连段／派生等待时限、恢复参数、当前分段击飞资格或正式动画资源。
- 不删除反射字段、旧枚举、旧资产引用；不新增另一套权威状态、RPC 或命中／受击流程。
- 迁移每个副作用时，先核对唯一所有者，再一次切换执行入口。不能让旧路径与新路径同时播放或清理。
- 先完成真实玩家受击基线，再收口相应结构迁移；未覆盖的恢复输入、死亡、换 Pawn 不以假人测试替代。

Lyra 对照仍为本机 `E:/Project/UProject/study/Epic/LyraStarterGame` 的 `LyraAbilitySystemComponent`、`LyraGameplayAbility`、`LyraHealthSet` 与 `LyraPawnExtensionComponent`。复用其 GAS／生命周期归属；未查到可直接替换 Hodge 连段、削韧与分段续飞的完整实现。以下是对现有 Hodge 职责的提取，不引入外部战斗框架。

## 2. 当前已收敛的原类入口

### 2.1 Combat

`EvaluateTransition` 提供本地选边和服务器等待使用的共同判定。`SetActionRequestStatus` 继续是本地请求记录与结果发布的唯一写入口，不为本次重构再建立事务管理器。

保留以下身份的区别：

- `RequestId`：本地战斗请求历史与通知。
- `AbilitySpecHandle`：授予的能力。
- `PredictionKey`：GAS 网络激活及预测。
- `ExecutionId`：同一 GA 实例的本次动作。
- Avatar：这些身份当前适用的 Pawn。

不能用一个新 ID 取代上述全部语义，也不能把本地请求历史视为服务器复制状态。

### 2.2 Definition

`EndAbility` 仍是技能结束入口。原清理块已集中到同类私有 `ReleaseExecutionResources`，仅从结束保护内调用，保留清理内容和顺序。

资源释放与 GAS 最终结束分别负责：前者释放本次执行拥有的资源，后者保持网络取消、动画能力交接、Combat 通知及 Super 生命周期。不得在资源释放函数内再次结束 GA 或重新发布请求结果。

### 2.3 受击

`ResolvePresentation` 产生本次选择的值副本；`ValidatePlanPresentation` 检查当前实际计划；`Validate` 继续承担完整资产检查。

当前组件仍唯一持有权威 `FHodgeHitReactionState`；受击 GA 负责动画和运动。下面的迁移不改变这项归属。

## 3. F08 第一切片：无副作用计划解析（已完成）

### 3.1 类和文件

已新增一个不反射、不 Tick、不持久化的独立 C++ 类 `FHodgeHitReactionPlanBuilder`，文件位于现有 Runtime 的 `Private/Combat/HodgeHitReactionPlanBuilder.h/.cpp`。

这是独立类的完整实现，不是将 `UHodgeHitReactionComponent` 的成员函数挪到另一个实现片段。组件原 `BuildPlan` 保留为调用门面。

### 3.2 输入

输入为一次调用期间有效的上下文快照，包含：

- 已通过命中接受与 Body 资格判定的请求、表现方向／强度与目标受击等级。
- 当前受击状态、当前服务器时间、下一动画变体序号。
- 目标位置和朝向、来源朝向、当前实际空中／移动可执行状态。
- 已确认的 Profile 与目标 SkeletalMesh，只读用于资源选择和兼容校验。
- 当前是否具备播放主动画的有效 AnimInstance。

2026-10-19 已确认补齐来源契约：组件在每次同步调用时复制 `Hit.Origin`、来源是否仍有效及其当前前向、目标当前位置及前向，保持原方向回退算法；并复制请求、表现、目标等级和当前状态。输入还包含原排队Launch判定后的空中状态、移动可执行性、当前服务器时间和 `NextSequence + 1` 变体值，不提前推进序号。来源失效仍使用目标位置减命中Origin；方向退化仍回退目标前向。新类不能通过保存Actor或ASC再延后读取。

输入不持有 ASC、Actor、Controller 或 GA 实例，不创建长期 UObject 引用，不保存跨帧数组元素指针。Profile／Mesh 只在游戏线程的同步调用中读取，不宣称线程安全。

### 3.3 输出及失败

唯一计算接口为 `Build(Context, OutPlan, OutAppliedImpacts, OutErrors)`，返回本次是否可生成有效计划。

- `OutPlan` 继续使用现有 `FHodgeHitReactionState`，不增加复制格式。
- `OutAppliedImpacts` 沿用当前结果语义。
- `OutErrors` 只提供可定位诊断；原调用者仍将计划不可用映射为既有结果，不引入新的拒绝类型。
- Profile 选择、时长、位移、空中次数、落地／起身、续播策略和本次路径检查在同一个解析边界内完成。
- 依赖失效、Avatar／ASC 身份及请求接受资格由组件在构建快照前检查，不能由静态解析类代为授权。

2026-10-19 已确认失败契约：保留原计算对 `OutPlan`、`OutAppliedImpacts` 的增量写入及失败时部分输出，不在新接口内自动清空或回滚；只有返回true才可提交。`OutErrors` 每次只返回本次计算的诊断。现有三个调用方继续在未实际应用时清空对外结果的AppliedImpacts，不能将内部部分输出误报为已施加控制。

### 3.4 禁止的副作用

解析类不能取消能力、设置标签、播放 Montage、申请 RMS、Launch、移动胶囊、发送 RPC、修改 Profile 或登记 Timer。

组件继续执行 `SubmitPlan`、阶段更新和状态复制；受击 GA 继续执行运动及 Montage。一次重构只替换计算入口，不同时迁移提交与执行。

2026-10-19 已确认后续读取契约：`TickPlan`、`RequestGetUp`、`ChangePhase`、恢复查询以及受击GA对当前Profile的读取继续保留，包括真实落地后的Knockdown解析、起身、曲线、混出和恢复尾部；本次不扩展为整个控制链的配置冻结，也不新增长期快照或另一份权威状态。

### 3.5 完成标准

对相同输入比较迁移前后 Type、Phase、选中 Montage、Loop／Landing／GetUp、MotionSource、时长、位移、控制链和续飞次数；浮点值使用既有容差。

覆盖正式 Hero／Guardian／Dummy、旧布局显式入口、普通削韧、分段 Fly、自然下落兼容路径、非法当前资源、无关错误和取消／解绑。Editor／Game、原生回归、正式五段／R、玩家受击拥有者与观察者均通过后再切换完成状态。

实际结果已达到本切片门槛：独立旧新比较61场景、最终失败/依赖守卫22场景通过；正式五段最新三模式通过，R九项和玩家受击三端58项既有基线通过，后两者未在最终Warp窄修复后重复运行。完整批次、未复跑项和退出异常边界见[最终验证记录](../Validation/hit-reaction-plan-builder-2026-10-19.md)。

## 4. F08 第二切片：局部接触表现（另行批准）

拟将纯客户端的接触音画及轻反馈收至窄组件 `UHodgeHitContactPresentationComponent`，仍在同一 Runtime 模块；它不负责强受击控制、主 Montage、运动或权威阶段。

- 现有组件继续接收唯一的接触 Multicast、检查绑定及 HitId，并保留有限去重记录和既有诊断计数。
- Dedicated 路径不创建新的表现组件。当前服务端诊断计数不能因表现组件不存在而消失。
- 客户端接收器通过一次本地调用传入已确认事件、当前阶段和 Profile，只执行 Niagara、Sound 与局部轻反馈。
- 新组件只拥有自己的轻反馈 Montage／强度记录及清理，不持有第二份受击状态，不新增 RPC。
- 同级轻反馈抑制、较强覆盖、倒地／起身禁止站立轻反馈、Slot 冲突与 Mesh 骨架检查全部保持。
- 当前恢复尾段及其权威姿势租约留在原组件，不能因名称含“表现”就一并搬走；这些对象还参与根运动和服务器骨骼更新。

风险是重复播放、绑错 Avatar、重复清理与 Dedicated 诊断变化。必须先将旧播放调用停用，再启用新调用；同一次 HitId 只能播放一次。

## 5. F09 的关键实现约束与待确认项

### 5.1 长按不能直接迁入当前规则 GA

本轮重新核查发现：`UHodgeGameplayAbility_HavocRules` 的 `NetExecutionPolicy` 为 **ServerOnly**；`BeginCircuitHold`／`UpdateCircuitHold` 当前则在本地控制 Pawn 上执行，并从授予 Spec 的配置对象读取 Hold 参数。

因此“把长按直接搬进已有 HavocRules 活动实例”不能保持当前客户端输入行为。不能为方便迁移擅自把该 GA 改成 LocalPredicted 或复制新的输入状态。

建议另拟一个本地 Havoc 长按职责，订阅通用输入通知；只从既有规则配置读取阈值，最终仍通过 `RequestStandaloneAbility` 走原网络激活。通用 Hero 不再读取 HavocSet／Havoc 标签。这个切片需要先确定订阅接口、组件注入来源与解绑归属，再批准编码。

必须保持：原普攻 Started 不延迟、松开不补发攻击、20ms 检查及当前有界等待不擅自调参、提交前关闭会话、防止旧 Pawn 的 Timer 触发新动作。

### 5.2 电影会话

先列清已有 Hero 电影 API、会话字段、Source Montage 身份、Timer、相机栈入口和退出顺序，再确定窄宿主。Definition 现有按 ExecutionId 结束相机的入口需要兼容，不重建 CameraModeStack。

在这些数据及接口确认前，只保留原类中的唯一清理调用，不新增镜头宿主或反射字段。

### 5.3 初始化

先分别固定“Init State 已达到”“ASC 当前绑定有效”“当前允许行动”三个查询的契约，再减少 Combat 对 EnemyInit／HeroCharacter／PlayerState 的具体识别。

不能将三个查询合成一个笼统 Ready，也不能删除对玩家属性初始化、怪物装备依赖或当前死亡的检查。现有不同 ASC 所有权继续保留。新的查询接口、实现者及调用顺序需列入下一份具体接口变更，再确认。

## 6. 其余工作与停止条件

- F10：当前作者入口与兼容登记已经写入整改记录；字段删除、旧资产迁移另行确认，不通过清空属性制造“简化”。
- F11：未获得同负载 CPU／调用计数前，不新增解析缓存、不合并 LOS、不改变 Tick、Timer、采样密度或攻击次数。
- F12：本轮补真实玩家受击拥有者／观察者基线；恢复输入、死亡、换 Pawn／重生、解绑及独立进程仍按各迁移阶段补齐，不以最终位置收敛证明全程没有网络纠正。
- 任何新方案若要求重写 RPC、复制状态、全套正式动画或伤害公式，立即停在该切片并再次确认。

**第3节已于2026-10-19单独批准。第4～5节尚未获编码批准，不随第3节自动实施。**
