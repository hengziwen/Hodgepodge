# 攻击能力配置手册

本文是 Hodgepodge 攻击配置问题的统一查询入口，按当前项目实现解释字段，而不是照搬 Lyra 或引擎通用示例。以后询问攻击配置时，优先引用本文对应章节；代码更新导致说明失效时先更新本文，再提供新的配置建议。

范围：Definition、Timeline、HitWindows／HitPoints、HitProfile、Volume、HitSources、连段 DataTable／ComboDefinition、AbilitySet、PawnData、输入、伤害 GE、武器手持和调试。所有示例时间、尺寸、倍率均为示例，需要按实际动画和模型调整。

## 1. 查阅导航与完整链路

- 动画、倍速、混合、独立技能入口：[第 2 章 Definition](#2-definition)。
- 命中时刻、前后摇、取消窗口、状态和消息：[第 3 章 Timeline](#3-timeline)。
- 命中绑定、伤害、多段与去重：[第 4 章 HitWindows 和 HitPoints](#4-hitwindows-和-hitpoints)。
- 碰撞策略、过滤、采样方式：[第 5 章 HitProfile](#5-hitprofile)。
- 无组件的球／盒／胶囊与锚点：[第 6 章 Volume](#6-volume)。
- 刀根刀尖、脚部和组件盒体：[第 7 章 HitSources](#7-hitsources)。
- 连段跳转、缓存、移动取消、结束后续接：[第 8 章 ComboDefinition 和 DataTable](#8-combodefinition-和-datatable)。
- 授予、出生配置和按键：[第 9 章 AbilitySet、PawnData 和输入](#9-abilitysetpawndata-和输入)。
- GA 蓝图、运行期锚点、GE 与手持：[第 10 章关联配置](#10-ga蓝图ge和武器手持)。
- 从零配置示例：[第 11 章配置配方](#11-配置配方)。
- 配了不触发、多段只扣一次等：[第 12 章排查](#12-排查与校验)。
- 执行句柄和输出字段：[第 13 章运行期字段](#13-运行期字段)。

正式玩家的主要链路：

```text
Experience.DefaultPawnData
  → PawnData.PawnClass / AbilitySets / ComboDefinition / InputConfig / DefaultWeaponDefinition
  → PlayerState 的 ASC 授予 AbilitySet 中的 Definition
  → 输入经 Combo 协调器或 Standalone 路由激活 GA
  → Definition 的 Montage + Timeline
  → WindowTag / PointEventTag 精确匹配命中绑定
  → CombatComponent 创建本次检测会话
  → Profile + 来源或 Volume 产生几何结果
  → Melee GA 校验目标、合并碰撞组件、检查命中记录
  → 构建 DamageEffect 的 Spec，写入倍率和上下文
  → HodgeDamageExecution → HealthSet.Damage → Health 减少
```

CombatComponent／EquipmentManager 当前通过 Experience 的 AddComponents 注入；不要在角色上再添加第二套。玩家 ASC 属于 PlayerState，命中会话属于当前 Pawn／GA 执行。最终伤害只在服务器提交，客户端可以预测动作和表现。

常用正式路径：

- Experience：`/Game/Main/Experiences/Exp_HodgeDefaultExperience`。
- PawnData：`/Game/Main/Data/PawnData/DA_Dafult_PawnData`。
- 能力包：`/Game/Main/Data/AbilitySet/DA_Pover`。
- 连段：`/Game/Main/Data/Combo/DA_LightCombo`、`DT_LightCombo`。
- 单段动作：`/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1～5`、`GA_Attack_1～5`。
- 时间轴：`/Game/Main/Character/Hero/Ability/BasicAttack/Timeline/DA_Attack01_Timeline～05`。
- 输入：`/Game/Main/Input/DA_HodgeInputConfig`。
- 武器实例：`/Game/Main/Weapon/BP_WeaponInstance_Sword`。

打开资产之前从实际 Experience／PawnData 向下核对引用；资产名称相似或文件存在不代表当前角色正在使用它。

## 2. Definition

类型：[UHodgeAbilityDefinition](../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)。描述一次动作执行；一个大招可以在一条 Timeline 中包含多次命中。

### 2.1 顶层字段

- **AbilityTag**：此动作的身份标签，默认空、必须填写。连段节点通过它查找已授予的 Definition。给不同正式动作分配不同身份，如 `Ability.Attack.Light.01`；同一 ASC 同时存在多个同身份授予时，按标签查找会拒绝任意选择。它不是按键、不是窗口，也不代替 GA 类的 AbilityTags。
- **AbilityClass**：实际执行的 GA 类，默认空、必须填写。要求继承 HodgeGameplayAbility_Definition、InstancedPerActor、LocalPredicted；配置命中默认使用 HodgeGameplayAbility_Melee 子类。跳跃／死亡等普通 GA 不应强行套进 Definition。
- **ExecutionConfig**：动作、倍率、混合、Timeline 的配置结构，展开后见下节。
- **ExecutionRoute**：默认 ComboCoordinated。ComboCoordinated 保留连段图授权，不能随意直接激活；Standalone 通过正常 GAS 激活和消耗／冷却检查，不写入连段节点。Standalone 不能作为 ComboTable 的动作节点。
- **InputTag**：默认空。只用于 Standalone，授予时写入 Spec 的动态输入标签；例如 `InputTag.Ability.Skill`。ComboCoordinated 应留空，其按键在 ComboDefinition.InputBindings 配置。此字段不创建 InputAction、MappingContext 或按键绑定。
- **HitWindows**：默认空，持续命中绑定数组。按精确 WindowTag 匹配 Timeline 的 Window；每个不同绑定标签只能出现一次。同标签的多个顺序窗口可以复用一项，数量不用与窗口条数相等。
- **HitPoints**：默认空，单次命中绑定数组。按精确 PointEventTag 匹配 Timeline 的 Point；同标签多个时刻可复用一项并独立执行，每个不同绑定标签只能出现一次。
- **WeaponUseWindowTag**：默认空。指定 Timeline 中控制武器手持的状态窗口，例如 `Status.Weapon.Hand`。留空时不占用手持表现；填写后必须存在对应窗口并能解析有效武器。它不是命中窗口，不能单靠它造成伤害。

### 2.2 ExecutionConfig 字段

- **Montage**：默认空、必填。本次动作播放的蒙太奇，也是 Definition 的总时长来源。当前要求长度大于 0、一个不循环的 Section、正 RateScale、Standard BlendModeIn；循环、跳段和反向播放不属于当前配置契约。
- **PlayRate**：默认 1，必须为有限正数。改变动作推进速度，实际推进也受 Montage.RateScale 等播放设置影响。Timeline 的 0.3 秒仍填动画源位置 0.3，而不是把所有窗口重新除以倍率。
- **BlendIn**：开始动作的混合设置，默认 Time=0.1 秒、Mode=Standard、Curve=Linear。
- **NaturalBlendOut**：正常完成动作时的退出混合，默认同上。
- **StopBlendOut**：取消或切换动作时的退出混合，默认同上。与 NaturalBlendOut 分开可减少连续动作衔接时的突兀。
- **TimelineTaskConfig**：当前只包含 Timeline 引用，没有额外可配置的检测任务数组。
- **TimelineTaskConfig.Timeline**：默认空、必填，选择本动作的 UHodgeAbilityTimeline；必须启用 bUseMontageDuration。多个 Definition 共用一个 Timeline 时，修改会同时影响它们，需要独立调参时先复制时间轴。

### 2.3 三组 Blend 内部字段

- **Time**：默认 0.1 秒，非负且有限；0 表示立即切换。是混合持续时间，不是 Timeline 的触发位置，也不决定伤害段数。
- **Mode**：默认 Standard；当前校验只接受 Standard。下拉框出现其他引擎枚举不代表项目已支持对应模式。
- **Curve**：默认 Linear，控制混合权重变化方式；按需要选择引擎已有曲线类型。
- **CustomCurve**：默认空；Curve=Custom 时必须提供 UCurveFloat，其他曲线类型下不需要填写。

配置上只播放动作、不产生伤害是合法的：HitWindows／HitPoints 都为空时没有检测任务。需要伤害时必须接通绑定、来源或 Volume、有效 GE。

## 3. Timeline

类型：[UHodgeAbilityTimeline / FHodgeTimelineEvent](../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)。时间轴描述何时发生什么；窗口几何和伤害倍率在 Definition 绑定中填写。

### 3.1 顶层字段

- **bUseMontageDuration**：声明默认 false，但供 Definition 使用的资产必须设 true。总时长／时钟关联实际 Montage 实例；它与 Standalone 执行路由是两回事，Standalone Definition 也必须开 true。
- **Duration**：默认 1 秒、必须为有限正数。仅供关闭 bUseMontageDuration 的独立 Timeline Task 手动计时；Definition 模式忽略它，不需要同步写入动画长度。
- **Events**：默认空，Window／Point 混排的条目数组。编辑器属性修改时按 StartTime 稳定排序。列表顺序不等于唯一执行规则，实际按时间、节点类型、Priority、条目下标排序。

### 3.2 Events 的公共字段

- **Kind**：默认 Window。Window 是 `[StartTime, EndTime)` 状态区间；Point 是 StartTime 的一次消息。切换 Kind 后另一组隐藏字段可能仍有旧值，但不参与当前类型的行为和校验。
- **EventID**：默认 None、必填，资产内唯一。是显示／调试身份，可用 Hit01、HandUse、Recovery；不需要注册为 GameplayTag，也不是命中绑定键。不能据其字符串决定业务分支。
- **StartTime**：默认 0，动画源秒，必须在 `[0, 动作时长]` 内且有限。Window 表示进入时刻，Point 表示触发时刻。输入前摇、刀刃进入敌人的时刻、取消权限开启时刻可以不同，不应全部重叠。
- **Priority**：默认 0，同一时刻、同类节点中数值小的先执行。同一时刻固定为“退出 Window → 进入 Window → Point”，Priority 不能反转这个顺序。相同类型和优先级才使用列表下标作为最后次序。

### 3.3 Window 专用字段

- **EndTime**：默认 0，必须比 StartTime 晚，不能越过动作总时长；0 不是“直到结束”的特殊值。结束端为开区间，到 EndTime 已退出。非常小的终点舍入误差会归一到总时长，起点和 Point 越界不会被自动修正。
- **WindowTag**：默认空、必填，区间内维护在 ASC 的 Loose GameplayTag。状态示例包括手持、攻击前摇、有效攻击、后摇、旋转锁定和取消权限；命中用 HitCheck 标签。父标签匹配不能代替 Definition 对绑定标签的精确匹配。
- **WindowEffectClass**：默认空，可选。给来源 ASC 施加的区间 GE，必须 Infinite 且 StackingType=None；权威端施加，退出时只移除本窗口的句柄。普通命中窗口通常留空，目标伤害在 Definition.DamageEffect。不要把 Instant 伤害 GE 填到这里。

常用状态标签的消费方式：

- `Status.Attack.Windup / Active / Recovery`：攻击阶段状态；只添加标签不等于开启碰撞检测。
- `Status.Attack.HitCheck.Weapon / Body / HitBox`：需要 Definition 的同标签 HitWindows 绑定才启动检测。
- `Status.Attack.Cancel.NextAttack`：只有 Combo 边的 RequiredWindowTags 等条件也满足时才能接段。
- `Status.Attack.Cancel.Move`：与 ComboDefinition.MoveCancelWindowTag 相同并达到移动意图阈值后，允许协调器取消当前连段动作。
- `Status.Rotation.Locked`：由现有旋转约束响应，锁定角色朝向；不等于禁止镜头转动，也不等于锁定移动。
- `Status.Weapon.Hand`：与 Definition.WeaponUseWindowTag 相同后占用手持表现。

同一个 Timeline 中，相同 WindowTag 的窗口不能重叠；首尾相接可能产生先退出再进入，应合并本来就要求连续生效的窗口。不同标签可以重叠，例如手持、旋转锁定和命中同时开启。

### 3.4 Point 专用字段

- **PointEventTag**：默认空、必填，是消息，不是持续状态。命中例子用 `GameplayEvent.Attack.Hit.Pulse`、终结用 `GameplayEvent.Attack.Hit.Finisher`。可重复出现在多个 Point 中；Definition 的同标签绑定仍只填一次。
- **PointEffectClass**：默认空，可选的来源 ASC 效果。只允许 Instant／HasDuration，禁止 Infinite；Timeline 只施加，不负责其后续移除。使用 HitPoints 的命中 Point 必须留空，给目标的伤害 GE 填在绑定的 DamageEffect。
- **NetPolicy**：默认 LocalAndAuthority。AuthorityOnly 只在服务器派发；LocalAndAuthority 在服务器与拥有端分别派发，Listen Server 本地玩家不会因此执行两次；LocallyControlledOnly 只用于本地消息。它不控制 Window 状态／GE，HitPoints 禁止仅本地派发。

Point 的实际顺序是先向 ASC 派发消息，再通知索引命中／连段消费者，最后施加可选来源 GE；前面的同步回调结束技能后，后面的操作不再执行。不要让同一个消息同时“立即结束技能”又期望随后产生命中。

### 3.5 三种容易混淆的时间

- StartTime／EndTime：动画源位置，随 Montage 时钟推进。
- RepeatHitInterval／InputBufferSeconds／ComboRetentionSeconds：世界秒，不随攻击播放倍率自动等比例缩放。
- Blend.Time：动作混合持续时间，不是时间轴上的偏移。

低帧率跨过多个 Point 时，调度器按顺序逐项执行；这保证事件身份不丢失，不等于还原了每个历史时刻的骨骼姿势或弯曲运动轨迹。

## 4. HitWindows 和 HitPoints

类型：[FHodgeHitEffectConfig 及其 Window／Point 子结构](../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)。两类绑定的几何和效果字段相同，触发字段不同。

### 4.1 触发字段

- **HitWindows.WindowTag**：默认空，必须精确对应至少一个 Timeline Window，同数组中唯一。
- **HitPoints.PointEventTag**：默认空，必须精确对应至少一个 Timeline Point，同数组中唯一。该 Point 必须到达权威端，且不能附带 PointEffectClass。

例如时间轴里三段窗口都叫 Status.Attack.HitCheck.Weapon，只需一条绑定。要为其中一刀配置不同来源、半径或倍率，需要不同绑定标签；先注册新 GameplayTag，再在时间轴和绑定两边使用它。

### 4.2 共同字段

- **SourceTag**：默认空。ExistingSource 或 RegisteredSource 必填，精确对应角色／武器 HitSources.SourceTag。AvatarRoot、ExecutionTransform、ExecutionTarget 等虚拟锚点可以留空；ConfirmedTarget 不要求注册来源。它不是窗口标签。
- **Profile**：默认空、必填，选择 UHodgeHitDetectionProfile 的查询和过滤设置。共享同一 Profile 就共享这些参数，不共享命中历史。
- **Volume**：检测体配置结构，默认 GeometryMode=ExistingSource。配置尺寸属于本绑定，不会写回角色组件或武器实例；细节见第 6 章。
- **TargetPolicy**：默认 AnyInVolume。AnyInVolume 处理范围内所有合法目标；LockedTargetInVolume 仍做几何查询但只保留已选择目标；ConfirmedTarget 对确认目标直接结算，不依赖目标是否开碰撞，仍检查距离、目标有效性、死亡与 Profile 过滤。
- **TargetKey**：默认 None。后两种目标策略以及 ExecutionTarget 锚点必填，与服务器 GA 的 SetHitTarget(Key) 一致。是 FName 键，不需要注册为 GameplayTag。
- **MaxTargetDistance**：默认 2000cm，有限正数。指定目标相对施法者的距离限制，创建与采样都检查；ConfirmedTarget 也受其限制。超出后当前会话失效，不自动改打其他目标。
- **RequiresWeaponInHand**：默认 true。Definition 已配置 WeaponUseWindowTag 时，要求该手持窗口覆盖此段；没有手持配置时它不会凭空创建手持请求。远处斩击可 false，让武器收回后范围仍在当前 GA 生命周期内完成。
- **DamageEffect**：默认空，默认 Melee 路径必填。要求 Instant 且 Executions 含 HodgeDamageExecution；没有隐式备用伤害 GE。常规可选择现有正式 GE 的合适子类，也可以在派生 GA 中明确实现不同效果契约。
- **DamageMultiplier**：默认 1，有限非负数。写入 SetByCaller.DamageMultiplier；0 不代表关闭检测，只代表伤害倍率为 0。最终值还受捕获属性修正、来源距离／材质衰减及目标交互规则影响。
- **DamageType**：默认空，可选。写入伤害 Spec 的动态资产标签，例如 GameplayEffect.DamageType.Melee；它自身不会另扣血、切换碰撞通道或筛选目标。
- **RepeatHitInterval**：默认 0，有限非负世界秒。0 为当前记录内每目标最多一次；正数允许间隔到期后的下一次有效查询再次命中。不会自动启动周期检测，Point 仍只采样一次。
- **HitGroup**：默认 None。空时每个条目拥有独立记录；非空时按下面的组作用域共享记录。组名是 FName，不是 GameplayTag，不需要全局注册。
- **HitGroupScope**：默认 Execution。Execution 让本次 GA 内同组共享记录；TriggerTime 让同组且 StartTime 精确相同的条目共享一段记录，下一个不同时间重新获得机会。它不会跨 GA／跨再次激活共享。
- **bAllowFriendlyFire**：默认 false。允许同队目标；仍拒绝自身、同 ASC、无有效 ASC／HealthSet、死亡或零血目标。当前无队伍接口的角色不会被自动视为友军。

### 4.3 多段去重的具体组合

- 三刀都能伤到同一个敌人：每段独立窗口／Point，HitGroup 留空、RepeatHitInterval=0。
- 整招无论多少检测来源只伤一次：相同非空 HitGroup、Scope=Execution、Interval=0。
- 每刀有剑刃与辅助范围，但每刀只能伤一次：两种不同触发标签，同一刀 StartTime 相同、同组、Scope=TriggerTime、Interval=0。
- 持续范围允许每 0.5 秒再次受伤：Window + EveryFrame + RepeatHitInterval=0.5；这是有资格时的最短受击间隔，不保证空挥也按节奏产生伤害。
- 严格固定三段伤害：用三个 Point／三个明确窗口，而不是用整个动作的一个长窗口加间隔估算刀数。

同一组绑定的 RepeatHitInterval 必须一致。多个碰撞组件命中同一目标会按目标 ASC 合并，免疫或同步回调也不会在同一次接触反复提交。

## 5. HitProfile

类型：[UHodgeHitDetectionProfile](../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)，一个可复用 DataAsset。

- **Strategy**：构造默认 SocketSweepStrategy，不能留空或选择抽象基类。SocketSweep 读采样点做球扫；BoxSweep 读实际 UBoxComponent；ShapeQuery 处理配置形状。ConfiguredShape 必须配 ShapeQuery，ExistingSource 必须配组件策略，ConfirmedTarget 不执行几何查询。
- **ObjectTypes**：构造默认 `[Pawn]`，不能空，也不能填不是 Object Channel 的 Trace Channel。目标受击组件需 Query Collision，并属于这些对象类型；世界有 ASC 不等于会被查询到。
- **bRequireLineOfSight**：默认 true。几何命中后检查原点至目标的遮挡；开启时不能把隔墙目标无条件纳入伤害。虚拟区域的原点由 FilterFrame 选择。
- **ObstructionChannel**：默认 Visibility，用于视线 LineTrace；这是 Trace Channel，和 ObjectTypes 的角色不同。
- **HalfAngleDegrees**：默认 180，有限值范围 `[0,180]`。是水平半角，90 对应前方总角度 180，180 为全方向。不能用它限制垂直高度／楼层，需要选择合适的形状尺寸或另设目标规则。
- **MaxSweepDistance**：默认 300cm，有限正数。bContinuousMotion=false 时，来源相邻采样位移超过此值按传送处理，只检测新位置；不是技能射程，也不是允许的目标距离。
- **RotationSubsteps**：默认 8，范围 `[1,32]`。控制盒体和配置胶囊的有限旋转插值查询；球体旋转无需同样子步。它不是刀身采样点数，也不保证数学上精确的连续旋转碰撞。
- **QueryMode**：默认 Sweep。Sweep 查询相邻位置间的路径；Overlap 只查询当前形状占用范围。旧 SocketSweep／组件 BoxSweep 在 Definition 中只允许 Sweep。
- **SampleMode**：默认 EveryFrame。Window 可持续逐帧或 OnceOnEnter；OnceOnEnter 不命中迟到目标，窗口结束不补一次。Point 不管选哪项都只采样一次。
- **FilterFrame**：默认 Caster。Caster 用施法角色的位置／朝向；DetectionAnchor 用实际检测区域的位置／朝向。远处爆发若仍使用 Caster，可能在正确范围查询后因错误方向或视线起点被剔除。
- **bContinuousMotion**：默认 false。true 明确允许连续高速移动扫过路径；不会自动区分瞬移，连续窗口内瞬移后需 ResetHitGeometryHistory，或拆开窗口。

项目已有可复用 Profile：

- `/Game/Main/Combat/HitProfiles/DA_Hit_VolumeOverlap`：ShapeQuery、Overlap、OnceOnEnter、DetectionAnchor、需要视线。
- `/Game/Main/Combat/HitProfiles/DA_Hit_VolumeSweep`：ShapeQuery、Sweep、EveryFrame、DetectionAnchor、保留传送保护。
- `/Game/Main/Combat/HitProfiles/DA_Hit_VolumeDash`：上述 Sweep 的连续位移版本。
- `/Game/Main/Combat/HitProfiles/DA_Hit_ConfirmedTarget`：一次采样，不要求视线，给确认目标路径使用。
- `/Game/CodexText/CombatHitWindows/DA_Hit_SocketSweep`、`DA_Hit_BoxSweep`：既有组件策略例子，先确认实际引用再修改。

需要不同过滤政策时复制 Profile。不要为了某个技能临时修改共享 Profile 或策略类默认对象的运行状态。

## 6. Volume

类型：[FHodgeHitVolumeConfig](../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)，嵌入在命中绑定中，不是需要另建的角色组件。

- **GeometryMode**：默认 ExistingSource，使用原 HitSources 及其策略。ConfiguredShape 开启本结构提供的虚拟几何，适合范围技能。
- **Shape**：默认 Sphere。Sphere／Box／Capsule 对应不同尺寸字段，未选形状的尺寸不参与本次几何。
- **AnchorKind**：默认 AvatarRoot。AvatarRoot 取角色根／胶囊变换；RegisteredSource 取 SourceTag 对应组件及可选 AnchorSocket；ExecutionTransform 取本次 GA 提供的世界变换；ExecutionTarget 取 TargetKey 对应目标的变换。
- **TransformPolicy**：默认 Follow，每次采样解析锚点。SnapshotOnEventEnter 只在该段开始时解析一次固定锚点。多个 Point 各自开始时固定，不自动共享同一个施法中心。
- **LocalTransform**：默认 Identity。相对锚点的位移／朝向偏移，Scale 必须为 (1,1,1)。锚点缩放不传给虚拟尺寸；旧组件盒体仍读其缩放后 BoxExtent。
- **SphereRadius**：默认 100cm，有限正数，Sphere 使用。
- **BoxHalfExtent**：默认 (100,100,100)cm，各轴有限且为正，Box 使用；完整尺寸是两倍，不要把完整刀气宽度直接当半尺寸。
- **CapsuleRadius**：默认 50cm，有限正数，Capsule 使用。
- **CapsuleHalfHeight**：默认 100cm，必须不小于 CapsuleRadius，包含两端半球；默认轴向为锚点 Z，可通过 LocalTransform 旋转。
- **AnchorSocket**：默认 None，仅 RegisteredSource 允许。是单个 Socket／骨骼名，留空取组件变换；不复用 HitSources.Sockets 的刀根刀尖列表。缺失名字会失败，不能悄悄回退。
- **AnchorKey**：默认 None，ExecutionTransform 必填，与 SetHitAnchor(Key) 一致。是 FName 键，不是 GameplayTag；资产不保存本次世界位置。
- **MaxAnchorDistance**：默认 5000cm，有限正数，限制解析锚点离施法者的位置。固定世界锚点也在后续采样时检查；玩家跑得太远会使该会话失效。不是形状半径，也不代替 MaxTargetDistance。

角色根通常在胶囊中心，地面爆发的 Z 偏移不能假定从脚底起算；Mesh／Socket 局部轴也可能与角色根轴不同。按所选锚点实际轴向调整，不要靠换正负数猜测。

确认目标路径忽略几何查询，但绑定的公共配置仍经过数据校验。要避免误解，ConfirmedTarget 通常保留 ExistingSource 默认几何，选择专用 Profile 并填写 TargetKey。

## 7. HitSources

类型：[FHodgeHitSource](../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)。武器来源放在实际 WeaponInstance 的 HitSources；身体／固定组件来源放在 Experience 实际注入的 CombatComponent 配置。

- **SourceTag**：默认空、注册来源必填。同一个 Pawn 当前角色和已装备武器范围内，精确匹配必须只有一个来源；重复标签会拒绝解析。常用 Weapon.MainHand、Body.Origin、Body.RightFoot、Hitbox.Chest。
- **ComponentName**：默认 None。填写实际组件对象名；角色留空使用 Character.Mesh，普通／武器 Actor 留空使用其根组件。变量显示名、资产名和组件对象名不是同一概念。
- **WeaponActorIndex**：默认 0，武器生成的 Actor 数组下标；只有武器来源使用。实际武器有多个 Actor 时明确选哪个，不是武器槽位或骨骼下标。
- **Sockets**：默认空。0 个使用来源原点；1 个使用该点；2 个在两点之间插值；多于 2 个逐个独立采样，不把每对点再插值。骨骼／插槽必须真实存在于所选组件。
- **SegmentSamples**：默认 5，合法范围 `[2,32]`。仅恰好两个 Socket 时决定包含两端的总采样点数；5 代表刀根、刀尖及中间 3 点。不是每秒 5 次查询，也不是伤害 5 段。即使当前列表不是两个点，字段仍需合法。
- **LocalOffset**：默认零，相对每个 Socket 或组件原点的采样偏移，受该变换的局部轴影响。SocketSweep 读取，组件 BoxSweep 不读取。
- **Radius**：默认 10cm，有限正数。每个 SocketSweep 采样点的球半径；不是整把剑长度。配置形状使用 Volume 的半径，不读取此值。

### 7.1 三类来源

武器：SourceTag=`Combat.Source.Weapon.MainHand`、ComponentName=`SkeletalMesh`、WeaponActorIndex=0、Sockets=[WeaponStart,WeaponEnd]。隐藏的 SkeletalMesh 用作手部检测，可见 WeaponVisualMesh 负责回背／消隐，不能把回背的可见模型误当挥刀路径。

本体：SourceTag=`Combat.Source.Body.Origin`、实际角色胶囊组件名、Sockets 空、LocalOffset=(100,0,0)、Radius=60 的例子，表示身体前方球体；踢腿可用右脚骨骼／Socket。数值须按实际角色调整。

旧组件 HitBox：SourceTag=`Combat.Source.Hitbox.Chest`、ComponentName 指向真实 UBoxComponent，Profile=BoxSweep。读取组件世界变换和缩放后 BoxExtent；Sockets、LocalOffset、Radius、SegmentSamples 不决定盒体几何。来源 Box 可以 NoCollision，因为它提供查询形状；被打目标的碰撞仍需可查询。

新技能需要很多不同大小区域时，优先用 ConfiguredShape，不为每个技能在角色上添加常驻 Box。连续独立飞出的攻击 Actor 属于后续功能，当前未实现。

## 8. ComboDefinition 和 DataTable

类型：[UHodgeComboDefinition / FHodgeComboRow / FHodgeComboTransition](../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)。动作内容属于 Definition；此处只协调节点、输入、跳转和记忆。

### 8.1 ComboDefinition 字段

- **ComboTable**：默认空、必填。选择 RowStruct=HodgeComboRow 的 DataTable。运行期按 ComboTag 名称查找行，不靠数组序号。
- **EntryComboTag**：默认空、必填，指向已存在入口行。入口代表无当前动作，不授予动作 AbilityTag，也不授予 GrantedTags。
- **InputBindings**：默认空，InputTag → IntentTag 数组。每个 InputTag 唯一；命中此映射后协调器先消费输入，不能再期望它同时激活 Standalone Spec。
- **InputBufferSeconds**：默认 0.3 世界秒，有限正数。容量 1，过早按下可在窗口开启时再次判断；新输入替换旧输入，过期不再接段。不是连段记忆持续时间。
- **ComboRetentionSeconds**：默认 1 世界秒，有限非负数。当前动作结束后保留可续接节点；0 为不保留。必须有合法的结束后输入边，不能仅把时间改大就期待续接。
- **MoveCancelWindowTag**：默认空。与当前动作 Timeline 的移动取消状态一致，例如 Status.Attack.Cancel.Move。仅协调当前连段动作，不会自动取消所有 Standalone GA。
- **MoveIntentThreshold**：默认 0.1，有限非负数，比较移动输入意图强度。不是 cm/s 移速，不是移动距离，也不自动检测角色是否被击退。

### 8.2 DataTable 每行字段

- **ComboTag**：默认空、必填，必须与行名完全相同。是节点身份，如 Combo.Light.01，不是 Definition 的 AbilityTag。
- **AbilityTag**：默认空。入口必须空，非入口必须填，并解析到 PawnData.AbilitySets 中唯一授予的 ComboCoordinated Definition。不要把 Montage 名或 GA 类名当作标签填入。
- **GrantedTags**：默认空。节点期间在 ASC 上维护的状态集合；离开节点撤销。入口禁止添加。不同于 Timeline 的局部窗口状态。
- **Transitions**：默认空，当前节点的出边。可同时有多条输入边和消息边，但每条边只选一种触发。

### 8.3 每条 Transition 字段

- **TriggerInputIntentTag**：默认空，来自 InputBindings.IntentTag；输入触发边填写此项。
- **TriggerEventTag**：默认空，时间轴权威 Point 或自然结束消息；消息边填写此项。与上一字段严格二选一，不能两者都空或都填。
- **TargetComboTag**：默认空、必填，必须有目标行。跳转入口表示回到无动作；跳转其他行表示尝试执行其 Definition。
- **RequiredWindowTags**：默认空。旧动作仍执行时，要求其自己 Timeline 的窗口同时全部存在；不能由别的技能或 ASC 上同名标签代替。入口没有动作窗口，不能要求此集合非空。
- **bAllowAfterExecutionEnded**：默认 false。true 允许在动作结束且记忆未过期时按输入继续；仅非入口的输入边可开。该恢复路径不依赖已经结束的旧窗口，但仍检查来源状态。事件边不能开。
- **RequiredSourceTags**：默认空，来源 ASC 要同时拥有全部标签。
- **BlockedSourceTags**：默认空，拥有其中任意标签就禁止跳转，不能与要求集合直接冲突。
- **TransitionPriority**：默认 0，多个合格出边中数值大者优先；同优先级取原列表先出现者。方向与 Timeline.Priority 的“小者先执行”不同。

### 8.4 InputBindings 的字段

- **InputTag**：实际输入标签，例如正式攻击 InputAction 对应的标签，必须与 InputConfig 相同。
- **IntentTag**：连段图使用的语义意图，必须与输入边的 TriggerInputIntentTag 精确相同；它可以与实际按键标签分开。

普攻末段后摇重开第一段，应配置从末段到第一段的输入边和合适 RequiredWindowTags；动作已结束后仍要续接则按设计开启 bAllowAfterExecutionEnded 并保留记忆。只发送 Timeline.End 或只增加 InputBufferSeconds 不会自动建立这条跳转。

## 9. AbilitySet、PawnData 和输入

### 9.1 AbilitySet 顶层字段

类型：[UHodgeAbilitySet](../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)，服务器批量授予并记录来源句柄。

- **GrantedGameplayAbilities**：默认空，直接授予普通 GA 的数组，适用于跳跃、死亡等。Definition GA 不能再重复放到这里，否则该 Spec 没有关联的 Definition。
- **GrantedAbilityDefinitions**：默认空，动作 Definition 数组，建立 Spec／Definition 关联。普攻和本框架独立动作技能使用此数组。
- **GrantedGameplayEffects**：默认空，安装能力包时给来源 ASC 施加的 GE；不是每刀的目标伤害，不按命中窗口触发。
- **GrantedAttributes**：默认空，能力包需要的属性集类型；现有核心属性链已有防重检查，不应为增加一种攻击重复创建 HealthSet／CombatSet。

数组元素：

- **GrantedAbilityDefinitions.Definition**：默认空、必填，对应动作资产。
- **GrantedAbilityDefinitions.AbilityLevel**：默认 1，至少 1；用于 GA Spec 等级，伤害 GE 构建会使用能力等级，不是 DamageMultiplier。
- **GrantedGameplayAbilities.Ability**：默认空、普通 GA 类。
- **GrantedGameplayAbilities.AbilityLevel**：默认 1，普通 GA 的 Spec 等级，通常从 1 开始。
- **GrantedGameplayAbilities.InputTag**：默认空，直接 GA 的输入标签；死亡由事件触发可以空。Definition 的独立输入在 Definition.InputTag 填写。
- **GrantedGameplayEffects.GameplayEffect**：默认空，包安装时施加的效果类。
- **GrantedGameplayEffects.EffectLevel**：默认 1，包 GE 的等级；Timeline 自身 GE 当前固定用等级 1，不会从这里读取等级。
- **GrantedAttributes.AttributeSet**：默认空，属性集类。

AbilitySet 卸装按记录撤销已授予能力、有效持续效果和新添加属性集；Instant 效果不会因为卸装而自动撤销过去已经发生的数值变化。

### 9.2 PawnData 字段

类型：[UHodgePawnData](../../Source/Hodgepodge/Public/Data/HodgePawnData.h)，通过实际 Experience.DefaultPawnData 选择。

- **PawnClass**：默认空，实际生成的 Pawn／Character 类。检查输入组件、Mesh 和检测组件都应从该类开始。
- **DefaultWeaponDefinition**：默认空，服务器在 ASC 就绪后自动装备的武器定义类；武器定义决定 InstanceType、Actor 及能力包。HitSources 编辑在它实际使用的 WeaponInstance 类中。
- **StatProfile**：默认空，角色基础属性与成长配置。本项目已接入初始化完成标记，要求属性的攻击 GA 不应绕过此门槛。详细数值来源见属性成长文档。
- **AbilitySets**：默认空，要安装的能力包列表；正确动作资产未放入其中或装备能力包就不会获得 Spec。
- **ComboDefinition**：默认空，角色连段协调配置，不替代每段 Definition。编辑器会检查其动作节点是否唯一解析到已授予的连段 Definition。
- **TagRelationshipMapping**：默认空，技能标签的阻塞、取消和激活要求映射，属于 GAS 能力关系，不是碰撞通道或敌人阵营规则。
- **InputConfig**：默认空，实际输入动作与项目标签的关联资产；按键还需要 MappingContext。
- **DefaultCameraMode**：默认空，角色默认相机模式，攻击镜头／朝向锁定由对应能力和状态窗口负责。

### 9.3 InputConfig 字段

类型：[UHodgeInputConfig / FHodgeInputAction](../../Source/Hodgepodge/Public/Input/HodgeInputConfig.h)。

- **NativeInputActions**：默认空，原生输入的查找列表，如移动／视角，需相应原生绑定逻辑；放入这里不会自动形成能力按下／释放事件。
- **AbilityInputActions**：默认空，能力输入动作列表，初始化时绑定按下和释放；连段映射先消费其登记的输入，剩余输入由 ASC 查找 Spec。
- **元素 InputAction**：默认空，选择真实 UInputAction。动作 ValueType、Triggers／Modifiers 会影响何时得到触发／完成事件，但不会自动选择 GA。
- **元素 InputTag**：默认空，与连段实际输入或独立 Spec 输入精确相同。编辑器 Categories 过滤只是选择器约束，不应误认为所有输入／状态命名都由运行期统一强校验。

MappingContext 的输入动作与按键映射应在实际启用的上下文里配置。当前 HeroComponent.DefaultInputMappings 是一个入口，Experience／GameFeature 也可能注入上下文；只在项目中新建 IMC 但没有启用它不会收到按键。

## 10. GA蓝图、GE和武器手持

### 10.1 GA 基本契约

Definition 的 GA 使用 InstancedPerActor + LocalPredicted。动作技能默认 Exclusive_Replaceable；独立技能仍受正常激活、属性就绪、消耗、冷却、组阻塞和死亡状态控制。不要通过修改原生校验去让未授予或未授权的技能随意启动。

`AbilityTag` 是 Definition 身份；GA 的 `AbilityTags` 是能力类型／关系标签。`CancelAbilitiesWithTag`、`BlockAbilitiesWithTag`、`ActivationRequiredTags`、`ActivationBlockedTags` 属于普通 GAS 配置；需要技能间取消／阻塞时另行设置，填写 Definition.InputTag 不会自动形成这些关系。

Standalone 想让客户端取消同步到服务器，保持 GA 的 Server Respects Remote Ability Cancellation=true；关闭时让服务器独占结束。普攻连段仍通过协调器验证。镜头转动与角色朝向锁定是两个不同输入／表现问题。

### 10.2 服务器锚点和目标入口

在 Melee 子类覆盖返回 bool 的 **PrepareHitExecutionContext**：服务器在 Timeline 激活前调用，当前上下文每次执行清空。函数内不能把 Latent Delay 当作准备完成；无法提供必需目标时返回 false 结束本次动作。

- **SetHitAnchor(Key, WorldTransform)**：当前服务器执行中安装世界变换；Key 与 Volume.AnchorKey 相同，Transform 有限、旋转规范化、Scale=1。更新同键会影响 Follow 的活动会话，Snapshot 会话保持原位。未提供键时不回退零点。
- **SetHitTarget(Key, Target)**：当前服务器执行中保存合法目标弱引用；Key 与 TargetKey 一致，目标须有有效伤害契约且非自身。绑定继续检查友伤、距离和死亡；返回成功不保证后续永远可以命中。
- **ResetHitGeometryHistory()**：在服务器完成瞬移后重置活动会话的上一采样位置，不清除命中记录，不让本刀重新伤害已经命中过的目标。

客户端直接调用 Setter 不建立权威上下文。当前没有通用的锁定组件或客户端 TargetData RPC；目标可由服务器现有选择逻辑提供，未来加入客户端候选时需验证位置／目标，不能直接信任客户端的半径、名单和倍率。

### 10.3 伤害 GE

默认攻击通过显式 DamageEffect 结算。检查 GE 的 Duration Policy=Instant、Executions.CalculationClass 包含 HodgeDamageExecution。

HodgeDamageExecution 捕获 Source CombatSet.BaseDamage，再读取 SetByCaller.DamageMultiplier，叠加 GE 对捕获属性的修正，并乘来源距离、物理材质和交互倍率；输出至 HealthSet.Damage，由 HealthSet 更新 Health。

需要注意：

- GE 的 Executions.CalculationModifiers 可以改变捕获值；“来源 BaseDamage=30”不代表任何 GE 都从 30 开始算。
- 已有测试中正式 GE 对捕获属性额外 +20，倍率 0.5 时每段为 (30+20)×0.5=25；这是特定配置例子，不是固定框架常量。
- 当前此 Execution 的伤害倍率入口是 SetByCaller.DamageMultiplier；资产名带 SetByCaller 不等于它自动消费 SetByCaller.Damage 作为伤害数值。
- Timeline.WindowEffectClass／PointEffectClass 是来源效果；目标伤害必须走命中绑定。
- 当前 Health 元属性／生命值的编辑可见性与伤害执行契约不同，不应直接改 CDO 默认 HP 来假装每次攻击伤害。

### 10.4 武器手持窗口

WeaponUseWindowTag 与 Timeline 的手持 Window 相同后，本次执行进入／退出时分别 Acquire／ReleaseHandUse。多个请求统一由 WeaponInstance 管理，连段衔接不用手动逐刀显示／隐藏模型。

要求手持的 HitWindow 必须完整落在手持区间内；同起点时手持 Window.Priority 比命中 Window 小，先完成手持绑定。要求手持的 Point 必须在手持开区间内，不能正好放在手持 EndTime。

RequiresWeaponInHand=false 只解除该命中条目的覆盖要求，不会取消整个技能的 WeaponUseWindowTag。手持、回背插槽、等待和消隐参数另见[武器表现设计](../Design/weapon-presentation.md)。攻击检测使用保持在手部的来源 Mesh，不受可见模型回背直接影响。

## 11. 配置配方

### 11.1 单刀武器攻击

1. 确认 PawnData 的默认装备实际使用 BP_WeaponInstance_Sword，给 HitSources 注册唯一 MainHand 来源。
2. 所选武器组件 SkeletalMesh 上存在 WeaponStart／WeaponEnd；Sockets 填两点、SegmentSamples=5、Radius 按刀身调整。
3. Timeline 例如 HandUse=[0,0.8)、Hit=[0.2,0.35)、Recovery=[0.5,0.9)、MoveCancel=[0.65,0.9)，全部在 Montage 有效长度内。
4. HandUse.WindowTag=Status.Weapon.Hand；Hit.WindowTag=Status.Attack.HitCheck.Weapon；Hit.Priority 在同起点时晚于手持。
5. Definition.WeaponUseWindowTag=Status.Weapon.Hand；HitWindows 一项匹配 WeaponTag、SourceTag=Combat.Source.Weapon.MainHand、Profile=SocketSweep、有效 DamageEffect、Multiplier=1、Interval=0、Group 空。
6. 普攻 Definition 选择 ComboCoordinated，填入 AbilitySet 与正确 ComboTable 节点。

### 11.2 踢腿或身体范围

在实际注入的 CombatComponent 子类默认值中配置 HitSources，角色／脚骨骼提供来源，不新增第二个组件。Body.WindowTag、Body.SourceTag、SocketSweep Profile 三者按各自职责连接。

纯拳脚可不配置 WeaponUseWindowTag；如果动作仍需要武器表现，则手持窗口与命中窗口分别设置。踢腿 Socket 必须在角色 Mesh 上，而不是武器 Skeleton 上。

### 11.3 无组件的大范围爆发

创建 Melee 子类独立 Definition：Standalone、唯一 AbilityTag、InputTag.Ability.Skill、对应 Montage／Timeline。

HitPoints 一项：Pulse 消息、Volume.ConfiguredShape、SphereRadius=800、AnchorKind=AvatarRoot、Overlap Profile、适当 GE 和倍率；Timeline 在 0.5 秒放 AuthorityOnly Point，同标签、PointEffectClass 空。

需要地面中心时按胶囊高度调整 LocalTransform.Z。不希望打楼上时用有限高度盒体，或增加明确的目标规则；半径并不自动限制楼层。

### 11.4 一个大招的三次命中

Timeline 在 0.3、0.6、0.9 秒各放 Pulse Point，EventID 分别 Hit01／02／03。HitPoints 只填一项，Group 空、Interval=0，每段独立记录。

终结在 1.2 秒放 Finisher，另加 Finisher 绑定，使用较大范围或较高倍率。所有时间须在实际 Montage 内。

### 11.5 次元斩固定在世界位置

Prepare 中获取服务器选定位置，SetHitAnchor("SlashCenter", Transform) 并返回 true。Volume 选择 ExecutionTransform、AnchorKey=SlashCenter、SnapshotOnEventEnter；在延迟时刻放 Point。

让全部伤害段固定于同一位置时只记录一次键，不在每段重新读取角色位置。若要跟随敌人则 SetHitTarget 后改用 ExecutionTarget + Follow。目标失效时明确取消或保留已固定位置，不能自动换到世界零点。

### 11.6 凤凰突进与落地

凤凰是角色表现时，每段突进开启配置胶囊／盒体的 Window，锚点跟随实际角色，Profile=VolumeDash；每段独立去重。落地用 Point 在 LandingCenter 做 Overlap。

如果中途是瞬移，拆分窗口或位置变更后 ResetHitGeometryHistory。当前没有任意曲线轨迹回放，必须让实际位移系统提供足够的采样；Niagara 粒子数量和位置不作为服务器命中数据。

### 11.7 锁定演出连续命中

服务器先确认 Target，SetHitTarget("Enemy", Target)；后续多个 Point 使用 ConfirmedTarget、TargetKey=Enemy 和有效 MaxTargetDistance。它不需要每一刀的视觉剑尖都精准碰撞，但每段仍要目标有效、未死亡且通过规则。

允许躲开时改为 LockedTargetInVolume，并选择相应武器／范围策略。目标死后是否换敌人是另一个显式策略，当前不会自动转移。

### 11.8 移动取消与结束后接段

Timeline 开移动取消窗口，ComboDefinition 的 MoveCancelWindowTag 使用同标签，MoveIntentThreshold 设置合理输入阈值；角色只有本次连段动作进入窗口才会取消。

在各动作节点增加有效输入出边，bAllowAfterExecutionEnded=true；ComboRetentionSeconds 保留例如 1 秒。动作结束、移动取消或被其他技能打断后，保留的节点在有效期内可以按输入续接。输入缓存与记忆各自到期，不能混用。

## 12. 排查与校验

### 12.1 按键没有动作

依次检查实际 Experience.DefaultPawnData、PawnClass、ASC 当前 Avatar 和属性就绪、AbilitySet 是否授予了 Definition、InputConfig 的 AbilityInputActions、已启用 MappingContext、精确 InputTag，以及路由与连段授权。资产有文件不等于已经授予。

### 12.2 有动作没有伤害

依次检查 Timeline 是否有有效命中 Window／Point，Definition 是否有同标签绑定，Profile 策略是否匹配 GeometryMode，来源标签是否唯一、组件／Socket 是否存在、服务器锚点／目标是否提供，以及 DamageEffect 是否有效。

随后检查目标 Query Collision／ObjectTypes、遮挡和半角、目标 ASC／HealthSet／存活、友伤规则、命中组是否已经消耗机会。只放 Status.Attack.Active 不会自动开启 HitCheck。

### 12.3 多段只扣一次或一刀扣多次

多段只扣一次：检查 HitGroup 是否用 Execution 共享且 Interval=0；每段独立时留空，或按需要用 TriggerTime。

一刀扣多次：检查是否多个不共享的窗口／来源都在打同一目标，是否设置了正 RepeatHitInterval。给同一段多来源配置相同开始时间、同组 TriggerTime，不要全局清空历史来解决。

### 12.4 迟到目标也受伤

EveryFrame 的长窗口会持续接纳新目标，即使 Interval=0。要只在开始判定，使用 OnceOnEnter 或 Point。不要把“不能重复打已命中过的敌人”误当“只查一次范围”。

### 12.5 远处范围不生效或随人跑

检查 FilterFrame 是否应该为 DetectionAnchor；检查锚点选择和 TransformPolicy、是否每段重新记录了中心；核对 MaxAnchorDistance／MaxTargetDistance。Fixed Snapshot 也有距离限制，SourceTag 不能充当运行期世界坐标。

### 12.6 高速突进漏沿途、瞬移扫整条路线

前者检查 MaxSweepDistance 是否触发传送保护、连续位移 Profile 是否正确；后者检查是否错误启用了 ContinuousMotion 且没有 ResetHitGeometryHistory。大曲线和高速骨骼动作需要实际路径／姿势采样，事件跨帧保证不等于几何回放保证。

### 12.7 内容校验方法

在 Definition 编辑器执行 Validate，或 Content Browser 的资产校验，分别校验 Timeline、Definition、PawnData。常见错误包括缺引用、错误路由、重复绑定、非法形状／尺寸、Point 仅本地、命中 Point 携带来源 GE、手持覆盖不足、缺目标键、连段行名不符和节点未唯一授予。

不要为了通过校验改变隐藏字段的无关值；按当前 Kind／GeometryMode 看实际生效字段。校验失败不会自动交换时间、自动注册标签、自动创建 Socket 或自动选择伤害 GE。

### 12.8 Debug 绘制

控制台：Hodge.Combat.DebugDraw.Weapon／Body／HitBox 默认 1，Duration 默认 5 秒；0 关闭对应类别。配置形状使用 HitBox 类别。实际查询在权威端，Listen Server 可看，客户端不复制调试线，Dedicated Server 不绘制。

绿色表示几何有结果，不保证已经通过目标资格、去重并扣血。相邻帧绘制累积 5 秒时看起来可能比实际单帧形状复杂，可减小持续时间帮助核对。详细见[调试绘制](../Design/hit-detection-debug-draw.md)。

## 13. 运行期字段

以下是源码中的运行请求、结果和回收记录，不是内容作者要填写的 DataAsset 参数。

### 13.1 FHodgeHitDetectionRequest

- **SourceTag / Profile / Volume**：由命中绑定复制而来，角色组件解析或虚拟几何的请求输入。
- **RuntimeAnchor**：已准备的世界变换，默认 Identity。
- **bHasRuntimeAnchor**：默认 false，区分“真实提供了零点坐标”和“根本没有提供键”。
- **RuntimeTarget**：目标弱引用，默认空，不延长目标生命周期。
- **TargetPolicy / MaxTargetDistance**：复制绑定的目标选择政策和距离约束。
- **IgnoredActors**：默认空，额外忽略列表；采样还会忽略来源角色和已装备表现 Actor。

### 13.2 FHodgeHitDetectionBatch

- **ExecutionId**：本次 GA 执行身份，换次激活后旧结果失效。
- **EventIndex**：Timeline 条目下标，默认 INDEX_NONE，区分同标签的多个时刻。
- **SessionHandle**：检测会话句柄，默认 0 为无效。
- **SampleSequence**：默认 0，实际采样递增，拒绝重复批次。
- **SampleTime**：世界秒，默认 0，用于命中间隔。
- **Hits**：原始几何结果，仍需经过 GA 目标／次数检查。
- **SourceComponent**：来源组件弱引用，配置区域可能没有组件。
- **Weapon**：武器实例弱引用，本体／范围可空。
- **SourceOrigin**：实际查询原点，进入效果上下文用于距离计算。
- **ResultKind**：Sweep／Overlap／ConfirmedTarget 的结果来源。Overlap 的点来自碰撞最近点或目标位置，后两种不应假装具有刀刃接触骨骼／物理材质。

### 13.3 FHodgeHitGeometry

**Points** 是 Socket 来源采样点；**Transform** 是本帧世界几何变换；**BoxExtent** 是盒半尺寸；**Shape** 是配置形状类型；**Radius** 是球／胶囊半径；**HalfHeight** 是胶囊半高。它们属于每个检测会话，策略默认对象不能保存上一次攻击的历史。

### 13.4 FHodgeAbilitySet_GrantedHandles

**AbilitySpecHandles** 用于回收已授予能力；**GameplayEffectHandles** 用于回收有效持续 GE；**GrantedAttributeSets** 用于移除本来源添加的集合。内容作者配置的是 AbilitySet 的授予数组，不填写这些运行期句柄。

## 14. 文档维护和验证边界

本手册依据本轮源代码核对，包含新检测体、多段和 Standalone 的当前实现；它不会把“未来独立攻击 Actor”“完整锁定系统”“演出镜头”“任意轨迹回放”描述成已完成。

本轮文档／注释工作不修改字段、默认值、声明顺序、include、结构、函数实现或资产配置。源码验证采用逐行插入检查和注释移除后等价检查；UHT／构建结果独立记录。此前功能验收见[多段技能验证](../Validation/skill-hit-volumes-2026-10-06.md)，不能当作本轮重新执行的 PIE／联机／打包。

相关资料：[配置体简明说明](../Design/skill-hit-volumes-usage.md)、[连段记忆](../Design/combo-retention.md)、[旋转约束](../Design/character-rotation-policy.md)、[角色成长](../Design/character-attribute-growth.md)。这些文档含各自历史与设计背景，攻击字段说明以本文和当前源码为准。
