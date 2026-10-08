# 攻击能力配置手册：动画通知驱动

本手册适用于 dev-AN。旧自建 Timeline 已退役；动画相关时机在 Montage 原生通知轨编辑。设计与迁移结果见 [迁移设计](../Design/anim-notify-combat-migration.md)，旧文件恢复见 [归档说明](../../Archive/Timeline/README.md)。

## 1. 正式资产入口与制作步骤

正式五段动作：`/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1～5`；各 Definition 引用对应 `AM_Attack01～05_Montage`，当前连段顺序 01 → 02 → 03 → 05 → 04。

1. 创建 Melee GA 蓝图，父类 `HodgeGameplayAbility_Melee`；普通技能配置 `ExecutionRoute=Standalone`，连段技能配置 `ComboCoordinated`。
2. 在 Definition 填技能身份、GA、Montage 和默认伤害；由 AbilitySet／装备／PawnData 授予，不能仅创建资产就期望自动响应输入。
3. 打开 Montage，在通知轨右键添加 `Hodge 持续命中` 或 `Hodge 单次命中`，编辑来源、形状与伤害。持续区间拖动两端调整时机，单次通知放在具体帧。
4. 按动作需要添加状态区间、武器手持区间；声音、粒子等继续使用 UE 自带通知。
5. 保存 Montage、Definition，测试服务器目标扣血、重复攻击与取消后清理。

首版玩法通知放在 **Montage 本身的通知轨**，不要放到其引用动画序列上。系统用原生 Montage 实例上下文核对当前 GA，无法认领的通知不执行玩法。玩法通知使用原生 Branching Point，避免远端角色多次姿势更新清空 Queued 事件。单次通知不能与另一个 Branching Point 标记在同一精确时间；同一时刻多来源使用单次命中通知的 AdditionalHits，手持等先决区间提前开始。持续状态的同帧开始／结束由引擎活动状态检查补齐，仍需对应验证。

## 2. Definition：身份、动作、默认伤害

`AbilityTag`：已授予技能身份，连段表用它找到唯一 Definition，重复授予同身份不能任意选择一个。

`AbilityClass`：执行 GA，命中技能用 Melee 或其子类；当前契约为 InstancedPerActor、LocalPredicted。蓝图可扩展准备目标和效果处理。

`ExecutionConfig.Montage`：本次动作蒙太奇，通知时机也在它上面。共用 Montage 会共用通知位置，默认伤害允许各 Definition 不同；需要不同区间时复制 Montage。

`PlayRate`：播放倍率，必须大于 0；通知时间按源动画秒填写，不随倍率改写。

`BlendIn`／`NaturalBlendOut`／`StopBlendOut`：开始、自然混出与取消混出。`Time` 为秒，`Mode` 当前要求 Standard，`Curve` 为混合曲线类型，Custom 时 `CustomCurve` 必填。自然退出恢复原生自动 BlendOut；自然曲线来自 Montage 的 BlendOut，迁移时已按配置写入，不同默认曲线的技能不要不知情地共用同一 Montage。

`ExecutionRoute`：ComboCoordinated 由 CombatComponent 的连段图授权；Standalone 走普通 GAS 激活，与连段图分开。

`InputTag`：仅 Standalone 使用，授予时写入 Spec；还需配置 InputConfig、IA／IMC 和输入处理。连段技能由连段图接收输入，不能在这里同时配置。

`DefaultHitConfig.DamageEffect`：默认伤害 GE。项目生命伤害必须为 Instant 且包含 HodgeDamageExecution；正式普攻使用 `/Game/Main/Character/Hero/Ability/BasicAttack/GE_MeleeDamage_Instant`。没有 GE 就不能产生伤害，运行时不会创建隐式替代品。

`DefaultHitConfig.DamageMultiplier`：默认倍率，最终还受捕获属性、来源衰减和目标规则影响；通知 DamageScale 继续乘在这个值上。

`DefaultHitConfig.DamageType`：写入伤害 Spec 的动态资产标签，用于效果分类，不会增加一次独立伤害。

旧 `TimelineTaskConfig`、`HitWindows`、`HitPoints`、`WeaponUseWindowTag` 不再是作者入口；不需要另建数据资产同步通知标签和时间。

## 3. 持续命中／单次命中的 Hit 字段

单次命中的 `AdditionalHits` 是可选附加来源列表：同一瞬间多个来源放在一个通知内，每项使用相同 Hit 字段，默认独立去重；同一刀应填写相同 Group＋Phase。它不增加第二份时机配置。

两种通知使用相同 `FHodgeAnimHitConfig`；持续通知在区间内由 PostPhysics Task 采样，单次通知只采样一次，然后释放。

### 3.1 来源与几何

`Source` 有六种：CharacterMeshSocket、EquippedWeapon、AvatarRoot、NamedComponent、ExecutionAnchor、ExecutionTarget。

`BoneOrSocket`：CharacterMeshSocket 直接读取通知所在角色 Mesh 的骨骼或插槽，例如 `Bip001LHand`；不存在时拒绝该检测，不退回角色原点。NamedComponent 可留空取组件原点。

`WeaponSourceTag`：EquippedWeapon 匹配当前装备 WeaponInstance.HitSources，默认剑为 `Combat.Source.Weapon.MainHand`。只在武器来源使用，身体攻击不需要来源标签。

`ComponentName`：NamedComponent 读取当前 Avatar 的组件对象名；它适合确实已有的组件，不要求每个技能在角色上常驻一个 Box。

`Shape`：Sphere、Box、Capsule。武器来源沿用其来源采样点；其余来源使用通知配置的虚拟形状。

`Radius`：球或胶囊半径，单位厘米，大于 0。`BoxHalfExtent` 是盒体三个轴的半尺寸，实际总尺寸为两倍。`CapsuleHalfHeight` 包含上下端帽，不小于 Radius。

`LocalTransform`：相对来源的位移和旋转；Scale 必须为 1，尺寸用上述字段调整，避免骨骼缩放悄悄改变伤害区域。

`TransformPolicy`：Follow 每次采样跟随锚点；SnapshotOnEventEnter 在通知开始时固定位置／朝向，之后角色移动不会拖走区域。

`AnchorKey`：ExecutionAnchor 使用服务器 GA.SetHitAnchor 提供的运行变换；缺失不会退回世界零点。更新同 Key 可推动 Follow 区域，Snapshot 保持已捕获的锚点。

`MaxAnchorDistance`：锚点离施法者允许的最大距离，创建与采样均检查，单位厘米。

### 3.2 目标

`TargetPolicy`：AnyInVolume 对范围内合格目标结算；LockedTargetInVolume 只对指定目标且仍需实际空间命中；ConfirmedTarget 直接对服务器确认目标结算，不依赖该目标碰撞，但仍检查距离、死亡、ASC Avatar 与伤害资格。

`TargetKey`：对应服务器 GA.SetHitTarget(Key, Actor)；ExecutionTarget 也使用这个键获得区域锚点。客户端填入一个 Actor 不能替代服务器确认。

`MaxTargetDistance`：指定目标离施法者允许的最大距离，单位厘米；ConfirmedTarget 也受此限制。

### 3.3 效果与命中次数

`bUseDefaultDamage`：开启时使用 Definition.DefaultHitConfig 的 GE／倍率／类型；关闭时使用通知的 `Damage` 显式效果参数。通知自己的几何与去重参数总是生效。

`Damage`：显式 DamageEffect、DamageMultiplier、DamageType；与 DefaultHitConfig 同一效果契约，仅在关闭默认伤害时使用。

`DamageScale`：再乘一次倍率，普通段为 1，终结段可以为 2。它不是基础攻击力，不能拿它替代属性配置。

`RepeatHitInterval`：0 表示本记录内每个目标 ASC 只处理一次；正数按世界秒允许再次处理。同一目标多个碰撞组件会先归并到 ASC，不会多扣血。

`HitGroup`：留空让每次通知进入独立去重。非空且 AttackPhase 为空，整次 GA 共享；非空且 AttackPhase 非空，仅相同 Group＋Phase 共享。

`AttackPhase`：显式标识“同一刀”，如 Slash01。左右手属于同一刀时填相同 Group＋Phase，下一刀填 Slash02；不要求通知开始时间相等。没有共享需求时两个字段都留空。

`bAllowFriendlyFire`：允许同队目标，但仍不允许自身、同 ASC、无效或已经死亡目标。

每次 GA 激活生成新执行身份，记录不跨连段或再次激活保留。需要持续伤害时显式设间隔，而不是复制一个相同标签的绑定。

## 4. 查询 Profile：可选共享过滤参数

Profile 留空使用原生默认 Pawn、Sweep、逐帧、视线检查。可复用资产位于 `/Game/Main/Combat/HitProfiles`；算法由真实来源与形状决定，不再要求手工配 Strategy。

`ObjectTypes`：要查询的碰撞对象类型，默认 Pawn；这是碰撞类型，不是伤害类型。目标至少需要可查询的对应碰撞。

`bRequireLineOfSight`／`ObstructionChannel`：是否从过滤原点向目标检查遮挡，以及使用哪个 Trace Channel，默认 Visibility。

`HalfAngleDegrees`：水平扇区半角，180 为全方向。`FilterFrame` 选择施法者或检测区域的原点／朝向作为过滤参考。

`QueryMode`：Sweep 查询相邻采样之间的运动，Overlap 查询当前位置。默认剑沿刀身多个采样点做球扫；虚拟球、盒、胶囊可使用 Sweep 或 Overlap。

`SampleMode`：EveryFrame 持续采样；OnceOnEnter 在持续通知第一次 PostPhysics 采样后不再采样。单次命中通知始终仅一次。

`MaxSweepDistance`：普通来源超过阈值视为传送，只查新位置；`bContinuousMotion` 仅对确实需要覆盖长距离连续运动的技能开启。瞬移后调用 ResetHitGeometryHistory 丢弃旧轨迹。

`RotationSubsteps`：盒体／胶囊旋转时的有限插值次数（1～32），不是精确连续旋转碰撞，数值越大查询开销越高。

## 5. 三种常用命中配置

### 左手划击（AM_Attack04）

1. 在 `/Game/Main/Character/Hero/Anim/Montages/AM_Attack04_Montage` 添加 Hodge 持续命中，覆盖实际从上向下划击区间。
2. Hit.Source=CharacterMeshSocket，BoneOrSocket=Bip001LHand，Shape=Sphere，Radius=15 作为初值，Profile 可留空。
3. bUseDefaultDamage=true；DA_Attack_4.DefaultHitConfig 配置有效伤害 GE，DamageScale=1，RepeatHitInterval=0，HitGroup／AttackPhase 留空。
4. 保存并攻击有 ASC、HealthSet 和 Pawn 查询碰撞的目标，观察黄色身体检测区域与目标扣血。

不需要 Timeline，不需要 Definition.HitWindows，不需要 Combat.Source.Body.LeftHand 注册，不需要 Experience 换 Combat 子类，也不需要手持窗口覆盖身体命中。

### 默认剑挥砍

Hit.Source=EquippedWeapon，WeaponSourceTag=Combat.Source.Weapon.MainHand。`/Game/Main/Weapon/BP_WeaponInstance_Sword` 的 HitSources 配置检测组件 SkeletalMesh、ActorIndex=0、刀根刀尖 Sockets、Radius 和 SegmentSamples。

`SegmentSamples=5` 指两个 Socket 之间包含两端的五个球扫点，范围 2～32，不是五段攻击。武器检测 Mesh 保持手部，可见 WeaponVisualMesh 专门负责回背和消隐。

在 Montage 加 Hodge 武器手持区间，覆盖拔刀与挥砍表现；它不直接开启伤害。武器命中另由持续／单次命中通知控制。

### 大范围与多段斩击

Source=AvatarRoot，Shape=Sphere／Box／Capsule，配置偏移与尺寸即可，不在角色上堆常驻碰撞盒。以连续八段为例，放八个持续区间或八个单次通知，默认每段独立命中；最后一段可以增加范围和 DamageScale。

若同一刀同时使用左手与剑，两项通知填 HitGroup=Slash、AttackPhase=01；第二刀使用 02。即使两项通知相差几帧也共享当前刀的命中记录。

远处固定区域用服务器 SetHitAnchor＋ExecutionAnchor；追踪确认目标用 SetHitTarget＋ExecutionTarget。独立技能 Actor 尚未实现，不能把这套角色执行生命周期当成已支持脱离角色存活的飞行物。

## 6. 状态、手持和玩法消息通知

`Hodge 状态区间.StateTag`：配置一个作用域状态，例如 Status.Rotation.Locked、Status.Attack.Recovery、Status.Attack.Cancel.NextAttack、Status.Attack.Cancel.Move。同标签区间可重叠，退出只撤销自己的贡献；GA 取消会兜底清理，不清零其他能力的标签。

攻击开头可调整方向，之后要锁朝向：把 Rotation.Locked 通知区间从需要锁定的帧开始，结束放在可恢复转向的帧。移动取消与接段权限分别配置对应状态区间，时间不用覆盖整个动作。

`Hodge 武器手持区间`：没有额外字段，使用当前装备 WeaponInstance 的手部 Socket 和表现配置；Begin 申请，End 释放。最后一个请求释放后回 WeaponOnBack、悬浮再消隐。连续连段的新请求会打断回收，GA 取消和预测拒绝同样释放。

`Hodge 玩法消息.EventTag`：发送给 ASC 和当前连段协调器，适用于实际需要消息的业务边。普通命中、手持和状态不需要额外事件标签。

自然混出和中断由 Montage 生命周期产生 GameplayEvent.Attack.Completed／Interrupted，无需在动作末尾手工添加 End 通知。

## 7. 连段、输入与授权

连段仍由 PawnData.ComboDefinition → DA_LightCombo → DT_LightCombo 定义节点和转换边，AbilitySets 授予五段 Definition。节点 AbilityTag 必须解析到唯一已授予 Definition。

边的 RequiredWindowTags 读取 **当前 GA 自己的状态区间**；其他能力添加同名 ASC 标签不会冒充当前动作权限。RequiredSourceTags／BlockedSourceTags 用于全局状态资格。输入缓存、节点记忆、取消后的保留和到期策略继续由 CombatComponent 管理，与动画通知对象分开。

末段后摇重开需要末段有 NextAttack 区间且连段图有回首段输入边；通知仅提供动作权限，不隐式重写图结构。技能打断和移动取消不会因为删除 NotifyState 对象而清空连段记忆。

## 8. 生命周期、网络与调试

命中最终在服务器执行，拥有端可预测动画、状态与武器表现。编辑器创建时自动设为 Branching Point、TriggerOnDedicatedServer=true、零触发权重且关闭姿势图／LOD／概率过滤；不要重新打开这些过滤来控制伤害概率。客户端不上传一个命中名单作为可信伤害结果。

GA 播放前租用角色骨骼更新，身体攻击即使没有手持请求也可在未渲染的服务器上读取骨骼。GA 与 WeaponInstance 共用 CombatComponent 的租用计数，最后一个租用退出后恢复原策略。

客户端连段请求可能早于服务器当前帧 NotifyBegin。来源和目标边有效的请求会在 ComboDefinition.InputBufferSeconds 内等待服务器实际窗口，随后仍完整授权；过期、错误来源或状态禁用会被拒绝。移动取消在动画更新后检查。无需延长 Montage 窗口或恢复 Timeline；见 [客户端窗口修复报告](../Validation/client-combo-window-2026-10-07.md)。

已确认的单 Section、固定倍率连段会校正拥有端额外推进的 Montage 进度，防止连续切段累积漂移；校正只作用于匹配的 Avatar／激活键／Montage，不直接派发通知或赋予权限。

检测在 PostPhysics 等待来源更新；同帧进入／退出保留一次末次查询，GA 取消立刻停止且不补伤害。低帧率下只使用实际更新的姿态，不宣称回放了所有丢失的骨骼帧。

默认调试开关均为 1，持续 5 秒：Hodge.Combat.DebugDraw.Weapon、Body、HitBox；Hodge.Combat.DebugDraw.Duration 控制秒数。在控制台设对应值为 0 可关闭。专服不绘制调试图形。

有动画无扣血：检查 GA 是否有服务器执行、有效伤害 GE、来源 Socket、目标 ASC／Avatar／HealthSet／碰撞对象类型、友伤规则、LOS，以及通知是否放在 Montage 本身。不要通过增加旧 Timeline 绑定解决。

取消后持续锁定：检查是否还有其他状态／GE 来源，或当前 GA 未结束；本执行只负责撤销自己持有的计数。完整验证结果与尚未验证项以迁移报告为准，编译通过不等于联机／打包通过。
