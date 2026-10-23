# 受击纯计划解析提取记录

日期：2026-10-19；续验：2026-10-20，Asia/Shanghai；引擎保持UE5.5.4。依据：[职责迁移方案第3节](../Design/combat-refactor-next-boundaries-2026-10-18.md)。

> 最终状态：第3节纯计划解析提取及另行获批补充修复已完成。最终Editor/Game、205项原生、转向两模式各11场景83项、正式单人五段7项与Dedicated0/100ms各7项流程/11项三端、确定时序假人12场景89项通过。五段按真实削韧窗口验收，正式参数不改。下文保留按时间记录的失败、待确认与未完成阶段；最新结论以文末“最终收口”及其明确边界为准，第4～5节仍未批准。

## 范围与已确认契约

用户单独批准 `FHodgeHitReactionPlanBuilder` 提取。完整复制来源位置、有效性与方向、目标位置与方向，保持原回退算法；失败保留原部分Plan/Applied输出，仅true可提交；后续阶段动态读取Profile保持原状。现有外层调用方仍清理失败对外结果，不把内部Applied列表当作实际执行结果。

新类位于现有Runtime的 `Private/Combat/HodgeHitReactionPlanBuilder.h/.cpp`，不反射、不Tick、不持久化、不新增模块/依赖。输入只保存同步计算需要的值与本次调用有效的只读Profile/Mesh借用；不持有Actor、ASC、Controller或GA。计算输出沿用原State和Impact类型；日志和具体对象身份诊断仍由组件负责。

组件保留资格/依赖检查、读取快照、`SubmitPlan`、复制、序号、状态转换、Timer及清理；受击GA保留Montage和CMC/RMS执行。`TickPlan`、`RequestGetUp`、恢复查询、GA与恢复尾部的Profile读取均不迁移。计划提取本身不恢复第4～5节、Client共鸣回路窗口异常或完整生命周期网络矩阵；本批正式五段再次失败后，用户另行恢复终结刀问题，见下文独立范围，不与纯计算提取混称。

## 迁移前事实与Lyra依据

当前三条调用路径为普通显式请求、`ResolvePoise` 和 `ResolveSegmentLaunch`，均先计算、成功后才调用SubmitPlan；失败仍由原调用方清空对外AppliedImpacts。原 `BuildPlan` 仅在最终主片确定后读取AnimInstance是否可用，不能将该条件提前为所有兼容路径的强制要求。

已查本机Lyra `Source/LyraGame/AbilitySystem/Executions/LyraDamageExecution.cpp::Execute_Implementation`、`LyraHealthSet.cpp::PostGameplayEffectExecute`、`LyraGameplayAbility.cpp::CheckCost/ApplyCost`、`LyraAbilitySystemComponent.cpp`的激活组查询与取消、`LyraPawnExtensionComponent.cpp::InitializeAbilitySystem/UninitializeAbilitySystem`。复用其计算/提交分工、Owner/Avatar和清理归属；未找到可直接迁入的完整削韧/分段续飞PlanBuilder，因此提取现有Hodge算法，不引入另一套战斗框架。

改前组件源和头文件备份位于 `Saved/PlanBuilderRefactor/20261019_145531/Before/`；本轮开始工作区已有223条状态记录。备份、生成报告和日志不纳入源码。

## 实施结果

已先保留原组件BuildPlan，增加新计算类与同输入差分测试；实际比较旧计算与新类的返回值、完整State和Applied列表通过后，再切换组件门面。差分测试不复制旧算法作为第二份长期实现。

组件 `BuildPlan` 现在只保留原前置依赖检查、同步快照采样、调用Builder和日志身份补充。快照包含原 `IsFalling || (Airborne && !bSawAirborne)` 语义，不提前推进序号；动画可执行性只在最终主片非空时消费，不拦截合法无动画兼容路径。Builder不自动清空Plan/Applied，仅清空本次诊断。

只读复核确认：排除 `BuildPlan` 整段及新增include，组件其余内容与本轮备份一致；公开组件头文件、受击GA、复制类型、Profile实现和Build.cs均保持本轮基线。没有新增反射字段、模块、RPC或第二套执行入口。

## 算法与原生回归证据

静态计算主体对照已通过：规范化后的新旧主体均为2368个token，SHA-256相同；仅使用明确列出的快照名称映射，并严格单列日志迁出的适配。证据脚本为 `Saved/PlanBuilderRefactor/20261019_145531/verify_extraction.py`，结果为同目录 `algorithm-equivalence.json`。此证据不代替组件采样接线和运行时验证。

切换前在旧生产BuildPlan仍未修改时，运行 `Hodge.HitReaction.PlanBuilder`：两项测试通过，进程退出0。`OldNewComparison/index.json` 和 `OldNewComparison.log` 保留独立旧新比较证据；当时组件SHA-256为 `D72C3361930B556CF0FA247AAD067EA1283834B1F69F790B7176535B47C29492`。

- `DifferentialMatrix`：61个场景，包括36个双布局/空中状态/类型组合、7个方向与退化回退场景，以及正式Hero/Guardian/Dummy三份Profile的18个选择场景。
- `GuardsAndPartialOutputs`：21个场景，包括无动画兼容、当前路径坏资源、无关配置错误、移动禁用与回退、控制链期限、空中次数、分段资格、主片动画依赖、跨阶段选择、续飞策略及失败部分输出。
- 每个场景比较返回值、所有反射State属性及Applied顺序；重复计算比较诊断及输出，并检查Profile内容/dirty、组件状态/身份、GAS标签、生命、Montage、角色变换和运动未被计算修改。

组件切换后，全量 `Hodge` 自动化196项通过，0失败、0未运行，其中受击40项、Editor8项；进程退出0。报告中119项无警告、77项带警告，保留原始诊断，不将带警告结果写成无警告。报告位于 `FinalAutomation/index.json`，日志为 `FinalAutomation.log`。取消/解绑包含既有 `CancellationAndInput`、`UpdateExpiryAndDetach`、`Poise.LifecycleAndSubmissionFailure` 等回归。

覆盖复核后再补一项 `LegacyLayoutWithExplicitImpactMode`：明确使用旧动画布局加 `ExplicitImpact` 枚举，避免把Legacy默认输入加Impacts数组误称为该枚举的覆盖；同时补齐所有失败必须提供本次诊断的断言。最终守卫为22个场景，矩阵仍为61个。重新执行Editor/Game增量构建均退出0，再跑全量196项仍全通过，119项无警告、77项带警告、0失败/未运行；最终证据为 `FinalAutomationExpanded/index.json` 和 `.log`，构建日志为 `EditorBuild-final.log`、`GameBuild-final.log`。切换前的61+21独立对照证据保持原样。

警告包含现有初始化调试输出、坏配置用例诊断，以及 `AM_Attack04_Montage` 的分支点重叠提示；本切片不修改这些正式资源或暂缓玩法问题。

切换后的同名差分测试用于验证组件采样门面和Builder输入输出一致性，不能单独代替切换前的独立旧算法对照；两阶段证据分别保留。

## 构建与运行时验收进度

切换前与切换后Editor常规构建均退出0；日志分别为 `EditorBuild-before-switch.log`、`EditorBuild-after-switch.log`。首次Game构建亦退出0，日志为 `GameBuild-after-switch.log`。补测试后的最终构建见上一节。本批PIE使用新批次 `Saved/CombatProduction/PlanBuilder_20261019_145531/`，各专项结果如下。

- 单机假人：12场景、89检查通过，12份最终释放快照、2442条逐帧样本；`poise-single-0ms-dummy/runtime-single-0ms.json`，batch.error=null，cleanup标记完整。
- 正式五段：失败。锁定已确认、动作顺序正确；前四刀各31伤害，终结刀3秒内没有真实伤害，后续移动、闪避和闪避后普攻未执行。原报告 `action-single-0ms-dummy/action-loop.json` 和batch均保留失败。
- R：9项检查通过，`r-single-0ms-dummy/r-input-smoke.json`，batch.error=null，cleanup标记完整。
- PIE Dedicated玩家目标：4场景、58检查通过；实际PlayerState-owned ASC、拥有者与独立观察客户端参与，三端合计12份释放快照、3117条样本，控制/运动/动画无残留。报告 `poise-dedicated-0ms-player/runtime-dedicated-0ms.json`，batch.error=null，cleanup标记完整。

原串行启动器在正式五段失败后立即停止；后两项在读取失败并询问范围后，使用 `run_gameplay.ps1 -StartIndex 2` 启动各自全新实例独立执行，没有跳过错误后宣告整个批次成功。

本批未执行完整Listen/100ms矩阵、玩家网络取消/解绑/死亡/换Pawn、独立进程专服、全面蓝图Compile、Cook或打包。正式三份Profile的原生解析测试不称为三个角色蓝图的完整实战验收。

以上为转向修复前基线。2026-10-20单机正式五段及后续动作闭环已通过，见下节；新增攻击转向的联机回归仍须独立通过，不能以单机结果关闭全部验收。

## 终结刀问题的另行授权与依赖

用户已在本批失败后明确授权单独排查修复终结刀，并确认锁定五段应通过现有转向机制保持可命中；随后已明确批准[补充方案](../Design/locked-combo-finisher-fix-2026-10-19.md)的具体规则，以及Queued/Pending继续原确认目标、确认切换/失效/解锁后本刀永久冻结的边界。受控转向更新与移动回放保存已实现；前述196项和PIE结果属于纯计划提取完成后的基线，新增规则以以下独立结果为准。

后续优先级与依赖：完成已批准规则的现有转向链修正及其原生测试，关闭编辑器执行Editor/Game常规构建；备份并只为正式五段启用作者开关，再完成五段实测、原受击基线和受影响的网络转向回归，最后核对工作区、记录全部结果并关闭本切片。第4～5节拆分和Client共鸣窗口问题仍不在该授权内。新增代码修改前备份与哈希另存 `FinisherBefore/`、`finisher-before-sha256.csv`，不覆盖纯计划提取前证据。

## 2026-10-20 转向修复后的独立验收

现有Definition能力在激活时记录本刀已确认目标并配对订阅确认状态；仅在原Warp窗口、最早伤害通知之前更新自己的同身份朝向请求。Rotation仍持有唯一Yaw事实，同步当前同身份服务器移动求值视图；MotionWarping仍执行旋转。CMC在本次移动中记录实际被Warp使用的输入，SavedMove在PostUpdate_Record只回填匹配执行、Avatar代次及Montage的Yaw单字段；回放不重采样，移动序列化字段不变。

正式 `/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1/2/3/5/4` 已启用 `FacingWarp.bTrackExecutionLockTargetDuringWarp`。先读取并保存五份完整备份到 `/Game/Tests/Combat/LockedFinisher/Backups/DA_Attack_<n>_BeforeTracking`，全部备份成功后才写源资产。`TrackingConfiguration/configuration.json` 的error=null、backups=5、saved=5；五组前后T3D去掉新增true字段后逐字一致，原角度135°、速率1080°/s、位移、通知和伤害均未调整。

- 常规Editor构建退出0：`EditorBuild-tracking-fixture-20261020.log`。常规Game构建退出0：`GameBuild-tracking-final-20261020.log`。保留已有插件描述、IncludeOrder等警告，没有修改构建设置来消除它们。
- 新增7组追踪回归，加原10组，共17组AttackWarp全通过：`TrackingAutomation-fixture-20261020/index.json`，0失败/未运行。覆盖默认关闭、确认点与预测点隔离、永久冻结、等待TargetData期间切换、固定起始角限幅、原窗口/伤害边界、YawLocked、旧执行及旧Avatar、服务器移动视图和SavedMove身份/回放边界。
- 正式资产启用后再跑全量Hodge：203项成功，0失败/未运行；119项无警告、84项带警告。AttackWarp17项、HitReaction40项、Editor8项。报告 `TrackingFullAutomation-20261020/index.json`；进程退出0。
- 单机正式五段：`Saved/CombatProduction/TrackingPostFix_20261020/action-single-0ms-dummy/action-loop.json`，error=null，7项检查全通过。真实顺序1→2→3→5→4，五刀各产生31实际伤害，第二刀Light、第三刀Heavy，仅第四号终结刀产生Launch；移动退出、方向闪避、闪避后普攻均完成。整段只在起始重置一次，不在连招中瞬移、放大检测盒或改通知。
- 同批单机假人受击复验：12场景、89检查、12份释放快照、2397条逐帧样本通过，`poise-single-0ms-dummy/runtime-single-0ms.json`。
- 同批R复验：9项检查通过，`r-single-0ms-dummy/r-input-smoke.json`。
- 同批Dedicated玩家目标受击复验：4场景、58检查、12份三端释放快照、2967条样本通过，`poise-dedicated-0ms-player/runtime-dedicated-0ms.json`。目标为真实PlayerState-owned ASC，远端拥有者和独立观察客户端参与；最终无控制、受击Montage或RMS残留。上述三份batch.error均为null，cleanup标记完整，进程退出0。该结果不代替攻击拥有者的联机连段。

首次原生追踪批次曾因测试源角色未提供HealthSet导致7项失败；补齐测试夹具真实依赖后重新构建并独立全量重跑，没有抑制错误或修改产品判断。原失败报告 `TrackingAutomation-20261020/` 保留。等待TargetData及预测暂停的原生夹具分别为本地GAS投递和显式暂停注入；SavedMove使用真实保存/恢复入口，但原生用例的求值作用域由测试建立，不能称为真实联机验证。

新增Dedicated PIE三端正式连招0ms首轮尚未通过：拥有者真实输入、独立观察者及服务器身份匹配，只有锁定检查通过，服务器始终重复第一刀，拥有者曾播放第二刀并随后收到激活拒绝；最终动作闭环超时。该结果位于 `Saved/CombatProduction/TrackingNetwork_20261020/action-dedicated-0ms-dummy/`，不以拥有者动画代替服务器伤害，不将原生203项或单机通过写成网络通过。当前正在定位最早分歧；本节暂不关闭全部验收。

### 联机失败对照与已暂停的契约

0ms首轮2359份三端采样中，服务器和观察者只有第一刀，拥有者第二刀有434帧有效动画权重；服务器19次31伤害均关联第一刀。原生日志出现145次能力预测拒绝，最早明显拒绝为预测第三刀；当前日志不足以确定最初第二刀在服务器激活等待、TargetData到达/缓存/消费中的具体Spec-Key分支。采集到41条匹配本批攻击者名字的原生位置纠正，最大11.826cm；回放和Warp输入歧义日志均未采集到，不能据空日志宣称SavedMove重放已实测或全程无纠正。

独立诊断 `TrackingNetworkToggleOffRetry1_20261020/action-dedicated-0ms-dummy/` 保持同一输入、45秒期限和七项断言，只在PIE前将五份Definition的新开关在内存true→false。仍只通过锁定检查：服务器序列仅第一刀，19次真实伤害均来自第一刀；拥有者可见第二刀，2474份三端采样后超时。诊断不算验收通过，也不能单独证明整个旧版本一定存在同一故障；它说明关闭新规则仍复现，尚不能将故障归因于动态追踪本身。

对照前后磁盘SHA一致，结束仅重载明确五包丢弃临时值，原true恢复、无dirty包、没有保存。首次 `TrackingNetworkToggleOff_20261020/` 在PIE前因Saved诊断脚本向独立结构体调用EditDefaultsOnly字段set接口而停止；改用完整文本唯一字段替换导入后另开Retry1，旧失败保留，未放宽生产反射限制。

剩余工作按依赖排序：

1. 确认是否单独恢复1→2激活/TargetData交接的定位与修复范围。未确认前不放宽原校验、不延长窗口、不改RPC或移动格式；第3节总验收保持未关闭。
2. 旧 `Tools/AttackWarp/run_e2e.py` 的angle_limit场景固定预期90°，而正式DA_Attack_1本轮备份前已为135°；原120°输入也已不触顶，仅改期望为120°会失去限幅覆盖。另有locked用例仍将结束朝向与首次Yaw比较，与获批的窗口内追踪契约不一致。已向用户补充完整测试方案，替换最初仅按配置计算期望的选项：限幅输入改165°以验证当前135°上限；仅locked用例改用原窗口结束时最后合法Yaw及同一目标身份核对实际朝向，原6°/8°容差及其余用例保持。该适配尚未确认，因此未改动或运行旧专项，不为使旧脚本通过修改正式资产。
3. 相应修复及测试契约明确后，才执行受影响构建/原生回归、原有网络转向专项和正式联机五段0/100ms；保留本次全部失败报告，最后再判断能否关闭第3节。

上述两个确认随后已获用户明确批准：授权定位并修复1→2交接，保留原校验、输入窗口与网络格式，必要契约变化再确认；旧专项按完整补充方案适配，替换最初不完整的120°期望方案。受击基线、R、单机正式五段与源码/资产范围核对均已完成。Dedicated/Listen均仅指同进程PIE世界，物理键鼠、独立OS客户端、Server可执行文件、完整生命周期、全面蓝图Compile和Cook/打包仍未验证。

等待确认期间已在新干净专用实例只读核实当前首刀Montage：Warp源时间0～0.118086、最早HitCheck开始0.133333、Recovery开始0.600000，通知和Definition导出位于 `ExistingWarpCurrent_20261020/`；inspection.error=null、saved=false、前后磁盘哈希一致、无dirty包，进程退出0。Montage SHA-256为 `576e655ee65e0fa1ab1db724d51f8e51fa1e80fd78e9347c3344c8e5dcd19c7d`。原inside的0.025～0.09、after的0.25～0.55以及服务器>0.12、Recovery的0.6/0.62与当前源时间未发现新的数值冲突；没有据此运行或宣称专项通过。

### 获批后的1→2交接定位与修复边界

独立批次 `Saved/CombatProduction/HandoffDiagnostic_20261020/` 仅增加Verbose身份日志，保留原真实三端输入和七项断言，仍失败。最早第二刀为Spec8、OriginalKey5、CurrentKey6：先进入Deferred等待并收到一笔方向数据；约0.021秒后恢复时，进入引擎激活前Cached=1，返回后Cached=0、Active=1且FacingWait=1。这是实际缓存丢失证据，不再只根据PIE结束时超时推断。

本机UE5.5.4 `AbilitySystemComponent_Abilities.cpp::InternalServerTryActivateAbility` 在真实激活前调用 `ConsumeAllReplicatedData(Handle, PredictionKey)`；`FAbilityReplicatedDataCache::Reset` 清TargetData、confirmed/cancelled、ApplicationTag和缓存数据预测键。原Hodge延后激活把本应在激活后到达的数据提前缓存，随后该清理将本笔数据清空。Lyra RangedWeapon的同步激活/按Spec-Key订阅与清理机制继续复用，但没有本项目Deferred开窗职责；不通过改RPC batch、引擎源码或窗口掩盖问题。

修复局限于已有ASC的实际延后激活边界：只为当前Deferred请求的同Avatar/Spec/原预测键、已确认且类型/来源正确的一笔方向数据保存局部值；引擎原清理及激活照常执行；成功激活的同身份实例仍在等方向数据时，使用原 `ServerSetReplicatedTargetData_Implementation` 重投，继续全部原校验、回调消费和retire。保留原CurrentKey与ApplicationTag，不恢复GenericEvent、取消状态或整份缓存，不新增长期队列、RPC或字段。回调后重新查询Spec/实例，再执行原延后时间确认和完成授权收尾。失败、旧Avatar、不同预测键或已结束执行不得被重投复活。

修改前源码与哈希单列 `Saved/HandoffFix_20261020/Before/`、`source-before.csv`，保留ASC原UTF-16编码；测试原文件单列 `TestsBefore/`。旧转向专项保留83条具名断言；按批准调整限幅输入和locked基准，原倍率、其他输入、容差及取消恢复步骤保持。新构建、原生与PIE结果在后续记录前均不预判通过。

交接修复后的首轮实际网络批次为 `HandoffFixed_20261020/`：Dedicated0ms的原七项和十一项三端检查均通过、退出0；五刀各31真实伤害，仅终结刀Launch。Dedicated100ms的玩法及三端检查也全部通过，采集到24次原生移动回放、无Warp输入歧义日志，但编辑器在完整关闭日志后返回 `-1073740791`，因此该轮100ms整批仍判异常，必须保留后另批复验，不能仅看batch.error=null宣告整个进程成功。0/100ms分别采集34/91条原生位置纠正，不将实际发生的纠正写成“没有回弹/纠正”。

新增 `Hodge.Combo.DeferredFacingTargetDataHandoff` 原生回归使用临时世界、测试专用非本地PlayerController、真实非Bot PlayerState、原生产Deferred Timer、Super激活及方向RPC实现，核对提前数据、实际Montage/Yaw、消费、重复、取消、到期及旧Avatar。首轮夹具缺少PlayerState，导致原生 `APawn::IsPlayerControlled` 前提失败；补真实依赖后另批复验，未放宽远端判定。原报告 `FocusedAutomation/` 保留，不将进程退出0误称为该测试通过。

补齐PlayerState后的首刀还须使用带预测键的原服务器激活入口，不能用本地无键输入代替远端客户端；该夹具修正后 `FocusedAutomation-remote-request/` 一项成功（带警告）、0失败，实际覆盖Open/Cancel/Timeout/Avatar四场景。完整 `FullAutomation/` 随后204项成功，119项无警告、85项带警告，0失败/未运行/inProcess，进程退出0。Native中的ASC仍是该临时Pawn自有，PlayerState-owned ASC的真实网络归属由正式Hero的Dedicated三端批次验证，二者不混称。

交接修复最终Editor构建为 `Saved/HandoffFix_20261020/EditorBuild-fixture-retry3.log`，退出0；Game常规构建为同目录 `GameBuild-final.log`，退出0。保留前面的诊断API误用、NativeTag编译修正及夹具前提失败，不隐藏失败或改引擎/模块设置。该交接批次生产代码仅ASC主cpp变化；ASC公开头、Definition头/实现及Combat实现均仍匹配 `HandoffFix_20261020/source-before.csv`，没有趁机迁移其他职责。

### 旧转向专项的新边界冲突

旧专项的首次启动批 `TrackingExistingWarp_20261020/` 在PIE前读取受保护的可选Play偏好失败，尚未运行玩法；启动器随后按既有可选不可读契约记录unavailable，保留旧批并新开 `TrackingExistingWarpRetry1_20261020/`，未修改必需偏好或断言。Retry1 Dedicated0ms实际运行：前两场景通过，angle_limit的135°目标限幅通过，但第19条检查要求实际角色到达135°±6°，实测为101.176941°，因此专项失败并在此停止，Listen100ms尚未执行。

该场景拥有者首个采样位置约0.030578s、Yaw0；后续帧Yaw32.060853/67.830107/101.176941，对应真实帧时间差约0.029686/0.033120/0.030877秒，均与1080°/s上限一致；窗口后保持101.176941°。这里的“允许135°方向修正”不自动证明短窗口内一定能到达135°，不能放宽容差、拉长通知或改速率来假装测试通过。

已就新的物理验收契约再次明确提问并暂停该用例修改：是否仅将大角度物理检查改为向合法Yaw推进、不越过目标、逐帧转速受现有上限约束，并保留服务器/拥有者实际朝向8°一致性；其余45°等场景仍要求6°/8°到达。正式参数保持。确认前不移除失败断言、不宣告83项通过；独立100ms正式五段复验继续进行。

独立 `HandoffFixedRepeat_20261020/action-dedicated-100ms-dummy/` 现已完整通过：原七项流程、十一项三端检查、五刀各31伤害且仅终结刀Launch，原输入与正式资产磁盘哈希保持；batch.error=null，PIE清理完整，编辑器退出0。采集到76条原生位置纠正及23次回放、0条Warp输入歧义日志。首次100ms非零关闭的原因未从日志/Crash目录或应用事件取得证据，保留该次异常，不将正常复验宣称为已修复某个已定位的引擎退出故障。当前剩余为上述大角度物理验收确认及之后的两模式转向专项收口。

## 本机执行命令

以下命令在项目根目录执行；未调整项目构建设置。日志/报告根为 `Saved/PlanBuilderRefactor/20261019_145531/`。

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.HitReaction.PlanBuilder' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/PlanBuilderRefactor/20261019_145531/OldNewComparison' '-abslog=E:/Project/Git/Hodgepodge/Saved/PlanBuilderRefactor/20261019_145531/OldNewComparison.log'
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/PlanBuilderRefactor/20261019_145531/FinalAutomationExpanded' '-abslog=E:/Project/Git/Hodgepodge/Saved/PlanBuilderRefactor/20261019_145531/FinalAutomationExpanded.log'
& './Saved/PlanBuilderRefactor/20261019_145531/run_gameplay.ps1'
```

最后一条为本批生成的串行启动器，内部逐次使用 `Start-Process -WindowStyle Hidden -Wait` 启动 `UnrealEditor.exe`，传入 `-HodgeCombatValidationExclusive`、独立 `-ExecutePythonScript` 与 `-abslog`。Python来自制作教程第14.3节模板，仅设置本批JOB/MODE/PLAYER_TARGET和新报告目录；启动前检查无既有PIE/dirty包，收尾核对cleanup标记和报告后才退出自有实例，不保存临时资产修改。

2026-10-20继续使用上面的Editor/Game常规构建参数，新增资产配置及验收入口如下。每条在前一条自有编辑器结束后执行；网络启动器首个失败即停止，0ms失败后未将100ms视为已运行。

```powershell
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -nosplash -nosound '-ExecutePythonScript=E:/Project/Git/Hodgepodge/Saved/PlanBuilderRefactor/20261019_145531/configure_tracking.py' '-abslog=E:/Project/Git/Hodgepodge/Saved/PlanBuilderRefactor/20261019_145531/ConfigureTracking-20261020.log'
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/PlanBuilderRefactor/20261019_145531/TrackingFullAutomation-20261020' '-abslog=E:/Project/Git/Hodgepodge/Saved/PlanBuilderRefactor/20261019_145531/TrackingFullAutomation-20261020.log'
& './Saved/PlanBuilderRefactor/20261019_145531/run_tracking_gameplay.ps1' -StartIndex 1 -Count 1
& './Saved/PlanBuilderRefactor/20261019_145531/run_tracking_network.ps1'
& './Saved/PlanBuilderRefactor/20261019_145531/run_tracking_gameplay.ps1' -StartIndex 0 -Count 1
& './Saved/PlanBuilderRefactor/20261019_145531/run_tracking_gameplay.ps1' -StartIndex 2 -Count 2
& './Saved/PlanBuilderRefactor/20261019_145531/run_tracking_network_toggle_off.ps1'
& 'E:/Python/python.exe' './Saved/PlanBuilderRefactor/20261019_145531/verify_extraction.py'
git diff --check
```

资产配置前先核对没有既有本项目编辑器，实际通过 `Start-Process -WindowStyle Hidden -Wait -PassThru` 启动上述参数，避免打开额外可见窗口；配置脚本有干净会话和备份存在性保护，不能将记录中的命令当作可重复覆盖按钮。算法摘要再次核对一致；受击组件公开头、受击GA、复制类型、Profile实现及Build.cs仍匹配纯提取前哈希。114份既有资产快照仅五份预期Definition发生变化，其余109份未变，另新增五份明确备份。工作区保留原用户改动，不提交、不推送。

关闭开关诊断启动器前后运行过两次：首轮在PIE前因测试脚本错误停止，修正后另批次复现联机失败；两次均退出1并保留证据，不能当成通过命令。只读首刀核查同样通过隐藏 `Start-Process -Wait -PassThru` 执行 `UnrealEditor.exe`，参数为项目路径、`-unattended -nosplash -nosound -HodgeCombatValidationExclusive`、`-ExecutePythonScript=E:/Project/Git/Hodgepodge/Saved/PlanBuilderRefactor/20261019_145531/inspect_existing_warp_current.py` 和 `-abslog=E:/Project/Git/Hodgepodge/Saved/PlanBuilderRefactor/20261019_145531/ExistingWarpCurrent-20261020.log`。

后续获得交接与测试适配授权后，实际执行入口如下；各报告结果按上文单列，不能将这份命令清单解释为每条都通过。既有批次拒绝覆盖，后续复验需另用明确新批。

```powershell
& './Saved/HandoffFix_20261020/run_handoff_network.ps1' -Batch HandoffDiagnostic_20261020 -LagMs 0
& './Saved/HandoffFix_20261020/run_handoff_network.ps1' -Batch HandoffFixed_20261020 -LagMs 0,100
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Combo.DeferredFacingTargetDataHandoff' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/HandoffFix_20261020/FocusedAutomation-remote-request' '-abslog=E:/Project/Git/Hodgepodge/Saved/HandoffFix_20261020/FocusedAutomation-remote-request.log'
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/HandoffFix_20261020/FullAutomation' '-abslog=E:/Project/Git/Hodgepodge/Saved/HandoffFix_20261020/FullAutomation.log'
& './Saved/PlanBuilderRefactor/20261019_145531/run_tracking_existing_warp.ps1'
& './Saved/PlanBuilderRefactor/20261019_145531/run_tracking_existing_warp.ps1' -Batch TrackingExistingWarpRetry1_20261020
& './Saved/HandoffFix_20261020/run_handoff_network.ps1' -Batch HandoffFixedRepeat_20261020 -LagMs 100
```

Editor/Game仍执行前述普通UBT命令，非Live Coding；编译失败及修正的每次日志保留于 `Saved/HandoffFix_20261020/`。全部临时制作、探针、报告和日志留在Saved；本批没有再修改正式二进制资产，没有自动提交或推送。

## 2026-10-20 大角度契约获批与Recovery释放顺序修复（验证中）

用户已明确批准新的大角度物理验收契约：仅该场景按向合法Yaw推进、不越过目标、逐帧转速受现有上限约束验收，并保留服务器/拥有者实际朝向8°一致性；其余场景继续使用原6°/8°到达要求。正式角度、速率、Warp/伤害窗口及网络格式保持不变。上文“物理验收待确认”保留为当时阶段的历史，不再表示当前仍等待这项批准。

随后实际运行 `Saved/CombatProduction/TrackingExistingWarpRate_20261020/`。Dedicated0ms在未改动物理到达或Recovery要求的 `camera-45` 场景失败：报告共15项检查，14项通过、1项失败，失败项为 `camera-45_AW03_Recovery释放占用`。报告位于 `dedicated-0ms/dedicated-attack-warp-e2e.json`，对应日志为批次根目录 `Editor-dedicated-0ms.log`；batch保留失败。测试在此停止，angle_limit尚未运行，Listen100ms尚未启动，不能将本批写成两模式11场景83检查通过。

失败采样中，拥有者Montage位置为0.6228534s，服务器为0.599418879s；拥有者已越过原0.6s Recovery边界，服务器仍未越过。拥有者的预测朝向请求 `03AE9A7D494CF1B29D23D98AB59CC124` 随后变为同一动作的权威请求 `CDDFE2B64B8605233E97A6B59977E72C`。这些是当前真实采样，不通过增加等待、放宽容差或推迟Recovery来消除失败。

修复前源码顺序为 `UHodgeGameplayAbility_Definition::ReleaseAttackFacing` 先调用 `Rotation->ReleaseFacingRequest`，最后才将 `bAttackFacingRetired` 置true。前者移除请求后会同步调用 `ResolveRequests`；此时GA尚未标记Retired，权威回退仍可能把旧动作租约回填到本地状态。该顺序与本次预测请求被旧权威请求替换的证据一致。

用户已批准最小顺序修复：GA必须在 `ReleaseFacingRequest` 触发同步解析之前先标记本刀Retired，保留原释放时机、RPC及容差。当前正在新增同步释放的原生回归并执行构建；本阶段尚无该修复后的新测试结果，不预写通过。第3节及另行获批补充修复的整体验收仍未关闭，需以修复后的实际构建、回归及两模式转向专项结果另行收口；第4～5节、Client共鸣窗口和完整生命周期矩阵继续保持原授权边界。

## 2026-10-20 Recovery顺序修复结果与纠正后Warp回放缺口（验证中）

Recovery最小顺序修复已完成：`ReleaseAttackFacing` 在同步释放朝向请求之前标记本刀Retired，原释放时机、RPC和容差保持。证据根为 `Saved/RecoveryRetirement_20261020/`；`EditorBuild-after-fix.log` 与 `GameBuild-after-fix.log` 的常规构建均退出0。修复前 `BeforeFixAutomation/` 确实有1项测试失败，包含两个“释放当下”断言失败；对应进程虽退出0，不能据此写成回归通过。修复后 `FullAutomation/` 共204项成功，其中119项无警告、85项带警告，0失败/未运行/inProcess；增强后的 `Hodge.AttackWarp.LateAuthorityDoesNotRevive` 也已通过。此前失败报告和源码顺序证据全部保留。

随后 `Saved/CombatProduction/TrackingExistingWarpRecovery_20261020/` 的Dedicated0ms通过49项检查，已包含大角度物理及Recovery检查，之后因工具读取受保护的 `ActionAbilityHandle` 报错而停止，不能把这轮计作11场景83项完成。工具随后仅改用现有ASC `GetAllAbilities`、`FindAbilityDefinition`、BlueprintLibrary当前active GA及Avatar的唯一匹配查询；Spec、请求、Montage实例和目标身份断言均保留，没有通过开放生产属性、移除身份校验或修改玩法资产绕过错误。

`Saved/CombatProduction/TrackingExistingWarpPublicQuery_20261020/` 的Dedicated0ms随后完成11场景83项检查，全部通过，batch的error为null、passed为true，PIE结束、无dirty包和偏好恢复等cleanup证据完整；但自有编辑器PID17928在本地15:49:17写出 `Exiting`、`Log file closed` 后，PowerShell捕获退出 `-1073740791`（`0xC0000409`），该轮仍保留为进程退出异常。窄范围只读诊断未取得故障模块或调用栈：项目 `Saved/Crashes` 最近记录仍为10月18日，同时间Windows Application Error/WER无匹配证据，也没有该PID的本地CrashDumps文件；缺少这些记录不能证明未崩溃，原因仍未确定。

另批 `Saved/CombatProduction/TrackingExistingWarpExitRepeat_20261020/` 使用相同代码和输入复验：Dedicated0ms再次通过83项检查，编辑器进程退出0。Listen100ms则在angle_limit第19项失败并停止：拥有者已推进到113.800678°，随后单帧退回74.615925°，差值约-39.18475°；同阶段服务器保持113.800678°。正常退出的Dedicated复验不自动解释上一批退出异常，也不能覆盖Listen的真实物理失败。

用户已另行批准单独定位并修复此次纠正/回放链路，协议参数8°保持，规则或验收契约的进一步变化须另行确认。源码与日志已形成一致证据：原生 `UMotionWarpingComponent::UpdateWithContext` 先扫描窗口并创建modifier，再广播 `OnPreUpdate`；项目在该回调内发现时间倒回后执行 `ClearAttackWarp`，将刚创建的项目modifier一并删除。`.111448→.147731` 这一段原始正常求值为16.64cm和39.19°旋转，纠正后的首个回放步仍有16.64cm平移，但旋转变为0°；后续步起点已越过原Warp窗口，不能等待下一帧补算。本机Lyra没有相同职责的自定义回放修复，继续复用UE5.5.4 CMC、CharacterAdapter和原Notify窗口机制。

当前已新增 `Hodge.AttackWarp.ReplayWindowRebuild`，覆盖原生实际CMC回放与CharacterAdapter的调用顺序；相应构建已通过，修复前的失败对照正在运行，尚无本轮测试结果。拟在倒回清理之后，只对当前真实Notify中包含合法历史位置的原窗口调用 `OnBecomeRelevant` 重建项目modifier，保留原生委托和加工链路，不增加接口、网络字段或玩法参数；本记录不预写修复已完成或该回归已通过。

用户还已明确批准验证包装器保留全部原生纠正日志和观测样本，以真实回放回归、逐帧可见Yaw速率、单调推进、不越界及同刻跨端8°共同验收，不要求“零纠正”。时钟倒退、身份变化或任何物理检查失败仍须停止，不能跳过样本后宣告成功。正式三模式的最新复验脚本已备，尚未运行；第3节及本轮补充修复的整体验收继续未关闭，第4～5节、Client共鸣窗口、完整生命周期矩阵及此前未验证项保持原边界。

## 2026-10-20 最终收口：回放修复、真实窗口与完整复验

### 1. 已完成的修复及原生证据

回放窗口重建已按获批边界实施于 `UHodgeAttackMotionWarpingComponent` 主cpp。执行身份、目标名或Montage位置倒回触发原有清理后，只遍历当前Montage中实际存在、目标名相同且包含当前历史位置的Hodge MotionWarping Notify；使用原生裁剪的起止时刻和 `OnBecomeRelevant` 重建缺失modifier。保持模板、原生回调、历史Yaw与平移，窗口外不重建；Montage缺失直接清理退出，不读取空对象。没有新增接口、RPC、移动网络字段、当前目标重采样或修改引擎纠正规则。

新增 `Hodge.AttackWarp.ReplayWindowRebuild` 使用项目真实SavedMove的SetMoveFor/PrepMoveFor及CMC `ConvertLocalRootMotionToWorld`，经过CharacterAdapter、原生窗口发现和项目OnPreUpdate，比较正常求值与倒回重放的旋转、平移及窗口外结果，并覆盖空Montage。`Saved/ReplayWindowRebuild_20261020/BeforeFixAutomation/` 实际1项失败，两条倒回旋转断言失败；不能把该进程退出0当作通过。修复后该回归、增强的 `LateAuthorityDoesNotRevive` 与 `DeferredFacingTargetDataHandoff` 均通过。原生夹具本身不等于完整网络纠正RPC，下面Listen实测另行覆盖真实纠正。

最终普通Editor/Game构建日志分别为 `Saved/ReplayWindowRebuild_20261020/EditorBuild-final.log` 与 `GameBuild-final.log`，均退出0。全量 `FullAutomation/index.json` 共205项成功：119项无警告、86项带警告，失败、未运行、运行中均为0；进程退出0。既有弃用等构建警告保留，未通过升级引擎、插件或改构建设置消除。之后只改Python报告断言与文档，没有再次修改C++，不为文档收尾重复编译。

### 2. 最终转向专项：两模式均完成

报告根：`Saved/CombatProduction/TrackingExistingWarpReplayFix_20261020/`。

- Dedicated0ms：11场景83项全部通过，1094帧、三端采样；大角度拥有者和服务器实际Yaw均约87.06734°，分别有11/10个合法相邻运动区间，原生求值记录57条。该批没有记录原生纠正。
- Listen100ms：11场景83项全部通过，1173帧、2346份两端记录；大角度拥有者72.53395184°、服务器72.53499706°，差0.00104522°，小于原8°；分别13/8个合法样本、12/7个相邻运动区间。原生求值58条，4条纠正相关日志对应两次实际纠正，全部保留，不能宣称零纠正。
- 两批batch.error=null、passed=true，均已结束PIE、无dirty包、未保存资产、恢复原地图及可读偏好，进程退出0。`LastExecutedPlayModeType/Location` 等受保护可选偏好仍记录unavailable，不能声称它们已被读取或恢复。

速率检查按同一Montage的有效运动时间核对，逐样本验证实例倍率、资产RateScale和Mesh全局倍率的乘积确为1；服务器World Tick间隔不冒充ServerMove模拟步长。所有同阶段有效采样及跨原窗口末尾的相邻区间均保留，检查身份、世界时间、Montage时间、实际可见Yaw的单调/速率/不越界；没有为通过跳过纠正帧。135°目标限幅、1080°/s与原短窗口不改，大角度不再要求短窗内必达135°，其余场景原6°/8°到达和跨端要求保持。locked通过已有ASC公共查询取得唯一当前GA，核对同Spec、同请求、同Montage实例和同确认目标；仍比较原窗口末最后合法Yaw，不开放生产受保护属性。

### 3. 正式五段：按真实削韧窗口验收

第一轮回放修复后的 `ReplayWindowFinal_20261020/action-single-0ms-dummy/` 已通过七项正式流程、清理完成、进程退出0。但随后 `ReplayWindowFinalNetwork_20261020/action-dedicated-0ms-dummy/` 被旧工具的“每轮必须Heavy”断言停止：五刀均已各扣31生命、终结刀已Launch，首刀到第三刀实际间隔1.5017s，首刀15点已超出原1.5s窗口，第三刀实际累计45未达55。该失败仍保留，后续移动/闪避当时未执行，不能事后改写为整轮通过。

用户明确确认参数保持，正式五段依实际窗口验收，重击由确定时序专项另行覆盖。`Tools/ACTCombat/validate_action_loop.py` 因而读取目标实际PawnData.Poise，仍要求Light、禁止Break，并核对普通刀首次Heavy前均低于实际阈值；实际Heavy必须达到阈值且包含Knockback。Heavy消费后允许同轮后续None，不要求固定第三刀Heavy；五段顺序、每刀真实伤害、普通刀不Launch、仅终结刀Launch、移动退出、方向闪避和返回普攻均保持。改动的是报告验收，不是Runtime判定、输入窗口、通知或正式参数。

最终网络批次为 `Saved/CombatProduction/PoiseWindowFinalNetwork_20261020/`：

- Dedicated0ms：七项流程和十一项三端检查全部通过，166份三端采样行。普通刀依次为15/None、35/Light、60/Heavy、50/None；终结刀单独Launch。
- Dedicated100ms：七项流程和十一项三端检查全部通过，185份三端采样行。普通刀依次为15/None、35/Light、45/None、70/Heavy，Heavy在第四刀出现；终结刀单独Launch。
- 两批均为实际拥有者Enhanced Input注入、服务器结算和独立观察者；顺序1→2→3→5→4、每刀31真实伤害，仅终结刀Launch，后续动作完成。batch.error=null、清理完整、进程退出0，正式资产前后哈希一致；目标 `/Game/CodexText/HitReactionValidation/Dummy/DA_DummyPawnData` 的Window=1.5、HeavyThreshold=55保持。

最终确定时序专项位于 `Saved/CombatProduction/PoiseWindowAcceptance_20261020/poise-single-0ms-dummy/`：原12场景89项全部通过，2394份采样、12份释放快照；HeavyR/HeavyL各7项通过，另覆盖Light、HugePoise、Knockback、Cancel、Armor、空中追加及次数、DenseNoRefresh和DenseThresholds。使用现有装备授予、Notify及GE与仅PIE有效的夹具配置，未保存正式资产；清理完成、进程退出0。没有因正式窗口表现去提高削韧或修改动画。

最终报告断言又加强了“首次Heavy之前不能已达阈值却漏Heavy”的前缀检查，防止较晚Heavy掩盖较早漏判。此项只加强纯Python验收函数，未再启动PIE。`Saved/PoiseWindowAcceptance_20261020/verify_prefix.py` 实际离线执行当前函数，复核上述两份最终网络报告、最终单人报告及保留的旧无Heavy失败报告，共4份真实数据；6种漏阈值、非有限值、Heavy无Knockback等反例均被拒绝，消费Heavy后的None合法。结果及当前源码SHA保存为 `final-prefix-audit.json`。旧失败只复核削韧数据，原overall error保持；没有补造未执行的后续动作或覆盖报告，离线检查不能称为新的PIE运行。

### 4. 实际命令与证据保留

在项目根执行，普通构建参数沿用前文：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/ReplayWindowRebuild_20261020/FullAutomation' '-abslog=E:/Project/Git/Hodgepodge/Saved/ReplayWindowRebuild_20261020/FullAutomation.log'
& './Saved/PlanBuilderRefactor/20261019_145531/run_tracking_existing_warp.ps1' -Batch TrackingExistingWarpReplayFix_20261020
& './Saved/RecoveryRetirement_20261020/run_final_gameplay.ps1'
& './Saved/ReplayWindowFinal_20261020/run_final_gameplay.ps1'
& './Saved/PoiseWindowAcceptance_20261020/run_acceptance.ps1'
& 'E:/Python/python.exe' './Saved/PoiseWindowAcceptance_20261020/verify_prefix.py'
```

上述RecoveryRetirement脚本的单人七项通过但关闭异常，未继续网络；ReplayWindowFinal脚本的单人七项正常退出、随后旧Heavy断言导致网络停止，不能把这两条整个命令写成成功。最终 `run_acceptance.ps1` 依次运行89项专项及新的网络0/100ms，整条退出0。验证包装器拒绝同名已有批次；复现时须使用新批次输出，不能直接覆盖这些历史目录。备份、日志与报告只留Saved，不纳入源码。

### 5. 完成范围、遗留问题与后续优先级

第3节的纯计算提取已完成：独立旧新比较61场景及最终22个守卫通过；组件提交/复制、受击GA运动/动画、后续Profile读取和失败部分输出语义保持。R九项、玩家受击Dedicated三端4场景58项及12份释放快照为此前TrackingPostFix基线，最终仅涉及攻击Warp和报告的窄修复后未重复运行，不能称为这一最终批次重新通过。Details正常/高级/兼容整理、检查与无损迁移仍以原实施记录为准。

历史HandoffFixed、TrackingExistingWarpPublicQuery及RecoveryRetirement三批曾在玩法/清理后返回0xC0000409，原因仍未知；窄范围日志、Crash目录和Windows事件未取得故障栈。最终原生、两模式转向、单人五段、12场景专项及网络0/100ms均正常退出，不能据此声称已修复某个已定位的引擎退出故障。若再复现，优先采集该次退出转储/故障模块后单独定位，不擅自改引擎、插件或渲染退出链。

已执行范围是同进程PIE、原生/真实CMC回放夹具及Enhanced Input注入；AW06包含编辑器探针注入，不等于真实服务器拒绝RPC。没有独立Server Target/进程、多进程、物理键盘、Cook/打包或完整死亡/换Pawn/生命周期网络矩阵的通过结论。这些边界仍需独立范围与验收；第4～5节职责拆分和Client共鸣窗口保持暂缓，不因当前切片完成自动开始。

本次收尾先同步文档，再核对源码/资产哈希、Python语法、本地链接与工作区差异；不自动提交或推送。五份正式攻击资产只在已批准范围启用追踪，其余源参数和Montage保持，历史失败不删改。最终审计结果在下面补记。

最终只读审计已通过，证据 `Saved/FinalClosure_20261020/closure-audit.json`：受击提取六份基线中只有组件门面改变，其余五份一致；交接五份基线中仅ASC实现和获批Recovery顺序所在Definition实现改变，公开头及Combat不变。114份资产中仅已批准五份攻击Definition改变，其余109份一致，五份显式备份存在，首刀Montage哈希保持。重跑旧新规范化计算对照仍为2368个token、相同SHA；4份Python语法检查、七份文档209个本地文件链接和无尾随空白检查通过，`git diff --check`退出0，只有既有LF/CRLF提示。当前工作区仍为244条状态，无本轮新增/移除的状态路径，用户已有改动保留；结束时无本项目UE编辑器残留进程。本次只同步文档和执行只读审计，未重跑UE、构建或保存资产。
