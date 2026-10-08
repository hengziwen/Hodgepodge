# dev-AN：彻底弃用自建 Timeline，迁移到动画通知

日期：2026-10-07。分支：`dev-AN`。核对基线：`b1482a5`（支持配置检测体、多段攻击与独立技能入口）。开始时工作区干净。

**状态：已实施并验证。** 2026-10-07 用户补充要求：旧文件不删除，集中归档；实施与验证结果以末尾记录为准。

## 1. 已确定的方向与最终边界

用户要求完全弃用项目自建 Timeline。最终分支交付中解除其 Runtime 调度器、数据资产类型、绑定模型、专用编辑器、测试夹具、生成脚本和当前配置说明中的运行依赖，并集中归档旧文件。Git 历史与明确标记的历史文档可记录旧设计，但不能保留一个会继续运行的兼容后端。

蒙太奇原生 Notify／NotifyState 是动画相关时机的唯一作者入口。命中、角色状态、接段权限、手持区间直接在动画轨道上编辑，不在另一张资产中重复填写开始／结束时间。

继续利用：GAS／PlayerState ASC、Experience／PawnData／AbilitySet、技能身份和连段图、输入缓存与连段记忆、伤害 Execution、几何查询算法、目标规则、武器实例／表现。它们需要的执行状态和句柄由当前 GA 负责，解除对 Timeline 的直接依赖。

本次迁移不引入独立技能 Actor，不重做移动动画、锁定系统、镜头演出、属性成长或 UI。不升级 UE 5.5.4，不增加业务 Runtime 模块或角色常驻组件。

## 2. 这次要解决的实际设计问题

当前链路把一段动作的时机拆为 Timeline 条目、标签、Definition 命中绑定和组件来源，制作一个左手攻击还会牵涉来源注册与 Experience 组件子类。

源码里存在实质耦合：

- Definition 强制 Montage／Timeline、单个不循环 Section，GA 自己启动 PlayTimeline 并依赖它结束。
- WaitHitResults 要求 TimelineTask 有效且未停止，并从 Timeline 查询窗口是否活动。
- Melee 的共享记录使用 Timeline.Events[EventIndex].StartTime，结果身份也使用 EventIndex。
- Combo 的接段和移动取消读取执行窗口，并通过 RefreshExecutionClock 强制推进旧时钟。
- 武器手持由 Timeline 窗口触发，Definition 又进行覆盖校验。
- PawnData 校验从 Timeline 推导可用状态／事件；自建编辑器维护第二份时机编辑和预览。
- ASC 对 Definition Montage 强制关闭自动 BlendOut，当前依靠 Timeline 到达末尾再停止，直接删除 Timeline 会使结束流程失效。

迁移必须同时拆开这些边界。只让 Notify 转发旧 WindowTag、再查旧 HitWindows 并启动旧 Timeline，不满足本设计。

## 3. 内容制作的验收目标

### 左手从上往下划

在 AM_Attack04_Montage 上添加一个命中 NotifyState，拖动有效区间，配置：

- Source=CharacterMeshSocket。
- BoneOrSocket=Bip001LHand。
- Shape=Sphere，Radius 例如 15cm。
- Damage 使用该动作的默认伤害，必要时覆盖倍率。

不需要新增 Body.LeftHand 来源标签，不需要创建 CombatComponent 子类，不需要改 Experience，不需要创建外部 Timeline／对应的命中绑定。当前 Notify 收到的 MeshComp 就是角色身体来源。

### 多段攻击

在动画上放多个命中区间或单次 Notify，每段拥有自己的运行身份；普通逐段命中无需手动取八个注册标签。最后一段可以在自己的 Notify 中覆盖范围／伤害倍率。

### 新业务

纯动画表现直接用 UE 已有声音／粒子通知。玩法业务由专用 Notify 或 GA 逻辑调用清晰的接口，不要求为每项功能修改通用时间轴、新增绑定数组及中央事件枚举。

## 4. 目标职责边界

### 4.1 GA 执行实例

管理本次 ExecutionId、当前 Montage 实例、当前 Avatar、预测键、正常／取消结束、资源句柄及服务器权限。执行账本直接属于现有 HodgeGameplayAbility_Definition／Melee 实例，不新增角色协调组件或一个通用时间调度器。

资源包括检测会话、作用域状态标签、可回收 GE、手持请求和骨骼更新租用。每项有独立句柄，统一在 GA.EndAbility 兜底回收。

### 4.2 Notify／NotifyState

保存作者需要的局部参数，接收 MeshComp 和原生通知上下文，向当前有效 GA 提交一次动作请求。Notify UObject 本身不保存当前角色、会话句柄、上一帧坐标或命中列表，因为同一动画资产可被多个角色同时使用。

NotifyState 的 Begin／End 表达区间；单次 Notify 表达一次触发。几何逐帧采样继续由独立 PostPhysics Task 驱动，不把完整检测或伤害算法塞进 NotifyTick。

### 4.3 CombatComponent／采样服务

保留来源解析、几何查询、会话句柄和目标过滤。请求包含世界几何／可解析锚点，完全不知道 Timeline、NotifyState 类型、条目下标或动画开始时间。

采样 Task 只关心 GA 执行是否有效、会话是否存活和是否需要继续采样；Notify 和 GA 负责开启／关闭。普通来源与骨骼更新前置依赖保留。

### 4.4 Combo 协调器

保留输入缓存、节点切换、授权、预测校正和执行结束后的记忆。所需窗口由当前 GA 的作用域状态账本提供，不能用其他能力在 ASC 上添加的同名标签冒充当前动作权限。

输入／消息到来时读取当前状态；状态变化时重新评估未过期缓存。删除为了推进 Timeline 而调用的 RefreshExecutionClock。

### 4.5 WeaponInstance

继续管理手持请求、返回插槽、悬浮／消隐和预测拒绝。新的 WeaponHand NotifyState 直接请求／释放手持，GA 记录句柄并兜底释放，不通过 Definition 的窗口匹配触发。

## 5. 首版通知类型与作者参数

### 5.1 UHodgeAnimNotifyState_HitCheck

拟新增持续命中通知。默认：当前角色 Mesh、单 Socket／骨骼球扫、逐帧、同段每目标一次。

基础面板只显示 Source、BoneOrSocket／武器来源、Shape、尺寸、默认伤害使用／倍率。高级参数放在高级分组，不让作者重复选择互相矛盾的 GeometryMode 与 Strategy。

Source 选择：

- CharacterMeshSocket：直接用通知 MeshComp，填写骨骼／Socket；无全局 SourceTag、无额外注册。
- EquippedWeapon：通过现有 WeaponInstance 解析武器来源；默认武器可直接选择 MainHand，保留已有刀根刀尖配置。
- AvatarRoot：以当前 Avatar 根变换建立区域。
- NamedComponent：确有需求时查找当前 Avatar 上的组件对象名，不作为普通手脚攻击默认方式。
- ExecutionAnchor／ExecutionTarget：由服务器 GA 提供世界锚点或确认目标，保留已有范围技能能力。

请求根据 Source／Shape 自动选几何算法。查询参数保留 ObjectTypes、LOS／通道、半角、采样方式、大位移保护和旋转子步，可内联填写或选用可选共享 Profile。Profile 的作者侧 Strategy 与 GeometryMode 双重选择退出新模型；旧算法实现继续复用。

默认没有 HitGroup，开启一次区间产生独立记录。需要多来源共享同一刀时填写显式 HitGroup 和 AttackPhase，见第 7 节。

### 5.2 UHodgeAnimNotify_Hit

拟新增单次命中通知，与持续命中使用同一种命中请求／结果契约。默认是当前位置一次 Overlap；可用于爆发、落地和 ConfirmedTarget。

每个通知触发产生一个独立 occurrence，不添加 Definition.HitPoints。通知可覆盖终结段倍率和尺寸。触发后不持有一个等待 End 的区间；结果消费完成即释放单次会话。

### 5.3 UHodgeAnimNotifyState_GameplayTag

拟新增作用域状态通知：配置 GameplayTag，如 Rotation.Locked、Attack.Recovery、Attack.Cancel.Move／NextAttack。

Begin 从当前 GA 获得状态句柄，End 释放该句柄。GA 内按标签计数，同标签通知可以按实际需求重叠；某个通知退出只撤销自己的贡献。原有 ASC 其他来源的标签计数不被清零。

旋转组件继续响应 ASC 标签；Combo 的权限读取本次 GA 的贡献，不读取所有来源的全局计数。

### 5.4 UHodgeAnimNotifyState_WeaponHand

拟新增专用手持区间通知，直接沿用 WeaponInstance.AcquireHandUse／ReleaseHandUse。默认武器选择与偏移来自现有表现配置。

是否需要武器由这个具体功能决定，左手 Body Hit 不被“武器手持必须覆盖”约束阻挡。武器来源解析失败明确停止该命中会话，不强制终止一切身体攻击。

### 5.5 UHodgeAnimNotify_GameplayEvent

拟新增可选消息通知，用于已有 Combo 消息边或能力事件订阅。只有业务需要消息时才填写 GameplayEventTag；普通命中通知直接提交命中请求，普通手持直接提交手持请求。

原生 Montage 生命周期提供默认自然结束／中断消息，不强制在每个动画末尾额外放一个 End 通知。

消息统一为中性的 GameplayEvent.Attack.Completed／Interrupted；旧 GameplayEvent.Attack.Timeline.End 的常量、Config 注册和 Combo 数据引用在迁移时替换并清除。每次执行至多发送一次对应结束消息；同步消息回调可能切换动作，返回后只清理原执行，不关闭新执行。

### 5.6 GE 与额外业务

首版迁移实际使用的状态、命中和手持。若引用审计发现仍在运行的旧 WindowEffectClass／PointEffectClass，按实际业务迁成作用域效果 NotifyState 或普通 GE／GameplayEvent 调用；不得为保留测试夹具而重新造一个通用 PointEffect 调度层。

项目允许在新专用 Notify 或普通 GA／AbilityTask 中扩展功能。通用状态／命中接口稳定，不建立“每个业务都必须增加中央注册枚举”的制度。

## 6. Definition 与数据配置的收敛

保留 UHodgeAbilityDefinition 类名及授予关联，避免无关蓝图父类与技能引用迁移。保留技能身份、AbilityClass、Montage、PlayRate、Blend 设置、ExecutionRoute、InputTag。

删除：FHodgeTimelineTaskConfig、ExecutionConfig.TimelineTaskConfig、HitWindows、HitPoints、WeaponUseWindowTag、基于 Timeline 的必填／匹配／覆盖校验。

伤害默认值使用一个可选的 DefaultHitConfig，共用 FHodgeHitEffectConfig 重构后的中性效果结构。Notify 默认继承本动作的伤害 GE／倍率／类型，可显式覆盖；没有选中有效 GE 时给出该通知的清晰错误，不创建隐式伤害效果。

只有需要复用时引用共享命中配置资产；普通手部检测可以直接在通知填写骨骼与半径。不要强制每个命中段再新建一个 DataAsset。

从中性效果结构移除 WindowTag、PointEventTag、EventIndex、开始时间相关字段；这些属于已弃用的绑定／调度模型。TargetPolicy、世界锚点、效果上下文、RepeatHitInterval 和友伤规则继续使用。

Profile 保留可复用查询／过滤参数，删除作者侧的策略配对陷阱；算法由真实请求的来源与形状确定。现有 Profile 迁移时明确转换参数，不在运行期悄悄忽略仍可编辑的旧字段。

## 7. 执行身份、去重与事件顺序

### 7.1 运行身份

执行身份由 ExecutionId + Mesh 弱引用 + Montage 实例 ID 验证；通知 occurrence 由原生回调中的事件／通知对象标识和本次进入序号在当前执行内分配。

会话只使用自己的 Handle／OccurrenceId，不使用旧 Timeline EventIndex。不要只取“当前活跃 Montage”来认领回调，否则旧 Montage 混出时的 End 可能关闭新动作的状态。

UE 5.5 FAnimNotifyEvent.Guid 位于 WITH_EDITORONLY_DATA，不能作为打包后运行身份。资产指针／原生通知事件引用可以用于同一端的匹配，但不能作为跨网络传输 ID；网络确认使用既有 Spec／PredictionKey／Avatar 身份。

首版关键战斗通知直接放在 Montage 通知轨上，使用原生 Branching Point 上下文核对 MontageInstanceID。动画序列中的普通声音／特效通知继续使用 UE 机制。后续支持通用序列玩法通知时要证明归属可解析，解析失败不能随意绑定新动作。

### 7.2 去重作用域

- 默认 PerOccurrence：每次 Notify／State 进入独立记录，同段每目标最多一次。
- ExecutionGroup：显式非空组名，整次 GA 共享。
- AttackPhase：显式 PhaseId + Group，让同一刀多个来源共享，下一刀使用另一个 PhaseId。

移除 HitGroupScope.TriggerTime 和开始时间浮点位的组键。作者没有多来源共享需求时不填 PhaseId；新功能不依赖事件轨道开始时间相等才能运行。

循环／跳段同一通知再次实际进入时分配新的 occurrence；相同进入回调重发不会重复创建会话。是否允许每次循环重新受伤由明确组政策决定。

### 7.3 顺序与采样

原生引擎负责通知触发和事件时序，不增加替代原生通知的 StartTime Scheduler。功能顺序通过轨道上的实际先后位置表达；需要必然先完成的资源由具体接口保证。

新增业务可以使用派生 Notify／普通 GA 请求现有服务，不要求再经过旧 Definition 标签绑定。基础执行账本只提供资源生命周期，不添加与时间排序相关的调度权限。

同帧窗口已进入又退出时，关闭请求在 PostPhysics 首次／末次采样完成后再释放；如果是 GA 取消，则立刻失效而不补伤害。检测等待来源组件骨骼更新，不能把动画更新前的第一帧采样假装成更新后的真实手部位置。

低帧率必须测试通知触发和短区间；事件可消费不等于保存了所有历史骨骼姿势。保留已有连续 Sweep 能力，暂不承诺曲线／骨骼完整回放。

## 8. Montage 生命周期与现有限制

当前 ASC.ApplyDefinitionMontageSettings 强制 bEnableAutoBlendOut=false。迁移后恢复原生自动退出，按照 Definition.NaturalBlendOut 配置自然混出；取消使用 StopBlendOut。GA 用原生 Montage 结束／混出／中断委托与当前实例身份结束执行，覆盖播放失败、重放、换 Montage 和手动停止。

普通攻击从开始自然混出后停止玩法窗口与伤害，按明确的能力结束政策结束／进入清理；不把“到达旧 Timeline 总时长”作为自然结束条件。对特殊保持末帧能力必须显式设计结束逻辑，不能依赖一个不会发生的 Completed 回调。

移除旧调度器导致的“只能一个不循环 Section”强制限制。首轮回归仍使用现有五段线性 Montage；随后验证多 Section、跳段、循环和播放倍率变化，证明通知区间与生命周期可正确清理。未验证的复杂组合不宣称已支持。

同一 Montage／序列被多个 Definition 共用时，通知位置也被共用，差异伤害由默认值／显式覆盖解决；确有不同命中区间时复制 Montage 或选择明确 Section，避免不知情地改动多个技能。

## 9. 取消、解绑和并发安全

GA.EndAbility 先使 ExecutionId／Montage 身份失效，再封存资源账本，停止采样并释放句柄。清理包含：检测会话、命中组、窗口标签贡献、作用域 GE、手持请求、委托、骨骼租用、运行锚点／目标。

NotifyEnd 正常释放自己的资源，迟到 End、重复 End 或别的角色的 End 都应无害。清理不能等待 NotifyEnd，不清零别人添加的标签，不用 Notify UObject 的成员存储运行状态。

命中 GE 或状态变化可能同步取消能力／销毁目标；消费每批结果时再次检查来源执行、目标 ASC 当前 Avatar 和句柄。旧批次不能给新 Pawn 或下一次激活补伤害。

World EndPlay、死亡、Pawn 更换、装备卸装、GameFeature 卸载走同一幂等回收契约。默认来源失效后停止伤害，保持当前没有独立攻击 Actor 的边界。

## 10. 服务器、预测和骨骼更新

服务器通过自己的通知和会话产生最终命中／GE。拥有端可以预测动画、状态窗口和手持；服务器验证 Combo 转换和移动取消。客户端不发送“通知刚触发所以我命中了这些目标”的可信名单。

关键战斗通知配置 TriggerOnDedicatedServer=true，适当的触发权重／过滤，首版以 Montage Branching Point 处理时序；这些设置需要对应测试而不是口头保证可靠。

在服务器播放攻击前就持有角色 Mesh 的动画／骨骼更新租用，确保没有可见渲染时也能触发通知并更新 Socket。手持之外的 Body 攻击同样需要此租用。

当前 WeaponInstance 只在手持请求期间覆盖 VisibilityBasedAnimTickOption；迁移时把此能力并入现有 Pawn 战斗入口的统一租用计数，WeaponInstance 与 GA 都使用相同接口，防止各自恢复旧值互相覆盖。不为此新增常驻组件。

预测拒绝回收拥有端状态／手持，服务器确认的执行不被旧预测撤销。模拟代理的旋转／移动约束继续复用现有复制入口，不依靠没有执行 GA 的观察者给服务器提交状态。

测试驱动客户端动作仍使用原生世界 Tick／真实输入，避免 Editor Python 的 GAllowActorScriptExecutionInEditor 改变 RPC 调用空间。

## 11. 最终归档与重构清单

### 11.1 归档旧调度类型

- Source/Hodgepodge/Public 与 Private/Data/HodgeAbilityTimeline.h/.cpp。
- Source/Hodgepodge/Public 与 Private/AbilitySystem/HodgeTimelineEvaluator.h/.cpp。
- Source/Hodgepodge/Public 与 Private/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h/.cpp。
- HodgeAbilityTimelineTests.cpp、HodgeTimelineEvaluatorTests.cpp 及其他夹具里依赖旧任务的测试代码。
- FHodgeTimelineTaskConfig、Timeline 专用 StopReason／节点枚举、OnIndexedPoint／WindowEntered 等旧回调与 include。

不删除引擎自己的 Timeline 蓝图节点或第三方插件中无关的时间机制。

### 11.2 重构当前类

- HodgeGameplayAbility_Definition：保留授予、身份和 GAS 路由；移除 TimelineTask、OnTimelineFinished、GetExecutionTimeline、RefreshExecutionClock，建立通知资源账本与 Montage 生命周期。
- HodgeGameplayAbility_Melee：接收中性请求，取消按标签扫描 HitWindows／HitPoints；共享键使用 occurrence／显式 phase，不读动画开始时间。
- HodgeAbilityTask_WaitHitResults：保留 PostPhysics 调度，取消 Timeline 存活／IsWindowActive 查询。
- HodgeCombatComponentBase：会话身份不使用 EventIndex；Combo 读取当前执行的作用域状态，TimelineEvent 改为中性的业务消息入口；移除时钟推进。
- HodgeAbilityDefinition：删除旧绑定字段与覆盖校验，保持实际动作配置和可选默认效果。
- HodgePawnData：校验实际 Montage 通知与动作契约，不从 Timeline 推导节点权限。
- HodgeAbilitySystemComponent：保留既有预测、授予和动作设置复制；修正自动 BlendOut 与结束分类。
- HodgeWeaponInstance：手持请求继续复用，骨骼策略与公共租用入口统一。

Blueprint 可覆盖的命中处理函数需迁移到中性配置参数并编译全部引用；不能只在 C++ 把 WindowTag 忽略而让旧引脚继续存在。

### 11.3 专用编辑器

归档 SHodgeAbilityTimeline、旧 HodgeAbilityEditorToolkit 的 Timeline／Tasks／Selection／事件调度预览、只服务此 Toolkit 的 SHodgeAbilityPreview 和旧 DefinitionWorkflow 测试。

Definition 使用标准属性编辑，动画时机使用 UE Montage Editor。保留现有 HodgeAbilityEditor 模块中的通用动画工具／原生 PIE 验证工具，清理不再使用的依赖和自定义 AssetTypeActions；不删除与 Timeline 无关的工具，不新增专门的时机编辑器。

通知内局部参数可在 Montage 编辑器调整、复制、拖动。只在真正影响 Runtime 正确性的地方校验：骨骼／维度、有效 GE、所属执行、合法目标与权限，不把功能不同当成全局禁止重叠的规则。

### 11.4 资产、脚本与文档

迁移后移除：

- Main/Character/Hero/Ability/BasicAttack/Timeline 的五个正式时间轴。
- CodexText/DefinitionCombo 的旧时间轴。
- CodexText/CombatHitWindows 的测试时间轴与旧绑定／夹具引用。
- CodexText/SkillHitVolumes 的 TL_VolumePoints／Windows／OnceWindow，并迁移或替换相应例子。
- CodexText/Timeline 的 GA／DA／GE 专用演示和监听测试。
- CodexText/Backups 中仍引用被删除脚本类的旧备份资产，先复制到项目外备份，再移出活动 Content，避免后续扫描／Cook 加载失效类。
- Tools/BasicAttack、Tools/Combat 及其他工具中生成／验证旧 Timeline 的路径；更新可继续使用的动作／检测脚本，退役纯时间轴脚本。

完整名单由引用审计生成，不仅按文件名删除。先迁移真正使用的资产，再处理无人引用的旧夹具／重定向器；禁止盲目递归删除整个 CodexText。

当前手册、README、知识库和自动索引改为通知配置。旧设计／验收移入或标记 History，当前入口不再要求 Timeline。Config 只删除经引用审计确认专属于旧调度的标签／重定向，不删除仍被 Combo 使用的正常状态与业务消息。

## 12. 资产迁移方法

1. 备份 Source、Config、Docs、待改资产及引用闭包，记录 SHA 和恢复说明；基线分支／提交固定。
2. 读取五段及测试 Definition 的实际 Montage、播放倍率、窗口／Point、GE、手持和连段权限；标记共享 Montage 和人工配置差异。
3. 在现有 Montage 通知轨上按实际源时间生成新通知，保持区间、顺序意图和倍率；命中绑定合并成通知参数，身体来源转换为 Mesh Socket 直接引用。
4. 冲突／未解析来源／隐藏 GE 契约输出人工复核列表，不跳过有损数据、不自动发明默认伤害。
5. 迁移工作用项目外 manifest 记录新旧对应，不在最终 Runtime 保存旧 Timeline 类型或 EventIndex。
6. Compile／Validate 所有相关 BP，确认引用已替换，再保存、移除旧资产和类型。
7. 冷启动重新加载、执行引用审计和目标范围 Cook，证明活动资产没有旧类残留；最后清理兼容中间代码。

一次性迁移工具可以在实施期间存在，最终只保留不会依赖已删除类的审计／恢复记录。旧字段在资产重存前不能先删除；若临时保留 UHT 读取字段，最终验收必须删除。

手持和命中原先存在非法覆盖、Profile 配对错误时，记录作者意图并修正到合法通知配置。当前 AM_Attack04 的左手需求按第 3 节落地，不继续要求补 Body 组件注册。

## 13. 实施顺序

### 阶段 A：一个真实动作的完整闭环

先做 AM_Attack04 左手 HitCheck State，跑通 Montage → 归属执行 → 单独检测句柄 → GE → 正常 End／取消回收。左手只填骨骼、半径、区间和效果，证明没有 SourceTag／Experience 子类／外部 Timeline 配对成本。

中间提交可暂时保留旧类供其他资产读取，但新的动作不得启动旧 Timeline；这种状态只是迁移过程，不能算最终完成。

### 阶段 B：迁移五段普攻及业务窗口

迁移手持、旋转锁、前后摇、移动取消／接段和消息。解除 Combo 与时钟／EventIndex 关系，恢复原生 Montage 自然结束。保留用户五段顺序、输入、当前 GE 和连段记忆。

### 阶段 C：多段／独立技能与并发

迁移 Pulse／ConfirmedTarget／固定中心例子，验证重复激活、多个来源共享 phase、预测拒绝和资源泄漏。复杂 Section／循环在中性执行身份成立后单独验证。

### 阶段 D：彻底清除旧系统

移除所有旧 Runtime／Editor 类型、字段、资产和脚本依赖，更新手册，执行构建／引用／Cook 和回归。`dev-AN` 最终不保留二选一执行模式或一个幕后旧 Timeline。

## 14. 验收标准

### 作者体验

- 左手检测不需要改 Experience 或新建组件子类。
- 一刀只需编辑一个通知区间，改时机不访问外部 DA。
- 普通多段不注册逐段标签、不复制绑定数组；终结参数可在对应通知覆盖。
- 新功能能通过专用 Notify／GA 调用现有服务，不修改中央调度模型。

### Runtime 与生命周期

- 动画和通知时机一致；正确执行 NormalEnd／Interrupted／Cancelled／播放失败。
- 所有取消路径清理检测、标签、GE、手持和骨骼租用；旧 End 不影响新动作。
- 同一 GA 再激活、同一 Montage 两个角色同时播放、短区间跨帧、同标签重叠均不串状态。
- 两种来源共享一刀只扣一次，多段独立扣血，取消后不补剩余段。
- 角色 Mesh 隐藏、纯左手且无手持请求、未被渲染时服务器仍有正确 Socket 更新。

### 保留功能

- 五段顺序与末段后摇重开、提前输入缓存、移动取消保持。
- 被独立技能打断后连段记忆保留并正常到期。
- 武器攻击保持在手部检测，回背／悬浮／消隐与连段衔接不被破坏。
- 旋转锁结束、取消、预测拒绝后恢复正确。

### 联机与工程

- Editor／Game Win64 Development 常规构建、所有相关 BP Compile／Validate。
- 单人／Listen Server 双人 100ms 延迟，主机与客户端激活／取消／拒绝／观察者一致，最终仅服务器提交伤害。
- 独立进程客户端／Dedicated Server PIE 模式验证服务器骨骼与通知；未构建独立 Server Target 时不能宣称专服可执行文件通过。
- 旧类／字段／资产的活动引用为零；冷加载和目标范围 Cook 通过。
- 代码仍按主 .cpp 组织，Runtime 不依赖 Editor，不改变引擎／插件／构建版本。

## 15. 最终交付清单与本轮边界

实施后的交付须包含新通知字段手册、迁移资产清单、外部备份恢复路径、实际构建／测试／Cook 命令和结果、未验证场景，以及旧系统清除审计。

本轮仅新增本设计并加入文档入口，检查本地链接和工作区差异。没有修改 C++、Blueprint、资产、GameplayTag、构建配置或分支，没有执行构建、PIE、联机或 Cook。

相关现状源码：[Definition GA](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp)、[Melee GA](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp)、[采样 Task](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.cpp)、[战斗组件](../../Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp)、[当前配置模型](../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)。这些链接用于现状审计，实施后应随删除／重构更新，不能长期保留失效路径。

## 16. 归档修订（用户确认）

旧 C++ 快照统一存入 `Archive/Timeline/Cpp`，保持原 Source 相对目录；包括改写类的原始实现，便于核对与恢复。纯 Timeline 实现退出 Source，不参与 UBT/UHT。旧蓝图与 Timeline 数据资产统一移入 `Archive/Timeline/Blueprints`，保持 Content 相对目录，移出 Content 后不参与加载及 Cook；这是原始 uasset 归档，不是当前编辑器可用资产。旧反射类型退役后不能在当前版本直接打开这些资产；恢复需同时恢复对应 C++ 与依赖资产，不能单独把 uasset 放回 Content。归档文件附 SHA256 清单，迁移前另做项目外备份。正式 Montage／Definition／连段表先替换引用并保存，审计依赖后再归档，禁止用缺失类或静默丢字段代替迁移。

## 17. 原生通知上下文核对修订

UE 5.5 的 `FAnimNotifyMontageInstanceContext` 可从 Queued 回调的 EventReference 读取原始 Montage 实例 ID。采用 Queued 默认值，仍支持显式 Branching Point 回调；同一时刻多个 Branching Point 引擎明确会跳过其中通知，不能把同帧接段、后摇与移动取消都设置为 Branching Point。状态 End 使用引擎事件副本，账本键使用资产上的 NotifyState UObject 加本次执行／Montage 身份，不使用事件副本地址或编辑器 GUID。

## 18. 最终派发修订：原生 Branching Point

联机重复测试发现远端角色的服务器 Queued 事件会漏发。UE CharacterMovement 的 ServerMove 与动画更新可能在同一帧多次 TickPose，AnimInstance.PreUpdateAnimation 重置 NotifyQueue；正常帧的单次通过不能保证派发。最终使用原生 Branching Point，直接在引擎 Montage 推进中接收 Payload.MontageInstanceID，不再把 Queued 当作正式玩法后端；保留其回调签名只作来源校验，不维持另一个执行模式。原生分支通知绕开动画图队列、姿势权重、LOD 与概率过滤，仍由当前 GA 归属验证保证安全。

同一精确时刻的点标记存在引擎限制，单次 Hit Notify 提供可选 AdditionalHits，多个来源共用一个作者时刻；每项仍使用独立 occurrence 或显式 Group＋Phase。Definition 校验拒绝点命中与另一分支标记重叠，避免静默少段。持续状态同边界会经引擎活动状态检查进入／退出，验证包含重叠标签、手持、命中与短窗口。第 17 节为弃用的过渡尝试，不能作为最终配置指南。

## 19. 实施交付

旧系统已集中归档，正式五段与多段样例改为原生 Branching Point。实际工程与玩法结论见 [迁移验证报告](../Validation/anim-notify-migration-2026-10-07.md)，作者逐字段说明见 [攻击配置手册](../Guides/attack-ability-configuration.md)。第 15 节的“本轮仅文档”记录属于实施授权之前的历史边界。
