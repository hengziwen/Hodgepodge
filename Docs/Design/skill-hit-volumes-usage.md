# 配置检测体、多段攻击与独立技能：使用说明

完整字段、默认值、适用条件和通用排查统一见[攻击能力配置手册](../Guides/attack-ability-configuration.md)；本文保留配置检测体的简明操作说明。

本轮实现范围：配置形状、锚点、逐段 Window／Point、共享去重、指定目标、独立 Definition 技能入口。未实现独立技能 Actor、镜头演出、角色绕敌位移、凤凰特效或完整锁定系统。

## 1. 选择技能的执行入口

打开技能的 `HodgeAbilityDefinition`：

- 普攻连段保留 `ExecutionRoute=ComboCoordinated`，由 ComboDefinition 的输入和转换授权。
- 独立技能选择 `ExecutionRoute=Standalone`，`AbilityClass` 使用继承 `HodgeGameplayAbility_Melee` 的 GA 蓝图。
- 独立技能 `InputTag` 可以使用已注册的 `InputTag.Ability.Skill`；直接事件／代码激活可以留空。
- `AbilityTag` 表示技能身份，应为每种正式技能选择唯一标签。测试例子共用身份，只能选择一个例子授予，不应把全部例子同时安装。
- `ExecutionConfig.Montage` 和 `TimelineTaskConfig.Timeline` 指定本技能的动作和时间轴，Timeline 开启 `Use Montage Duration`。

将 Definition 放到角色 PawnData 已使用的 AbilitySet 的 `GrantedAbilityDefinitions`，或装备授予的 AbilitySet 中。不要再把同一个 GA 同时放入 `GrantedGameplayAbilities`，否则第二个授予没有 Definition 关联。

在实际 `InputConfig.AbilityInputActions` 中添加 InputAction → 同一个 InputTag，并确保对应 InputAction 在实际启用的 MappingContext 中绑定了按键。InputTag 写入 Spec 不会自动创建 InputAction／按键映射。选择一个不被当前 ComboDefinition.InputBindings 消费的标签；通用 Skill 标签已经登记。

独立技能仍执行正常 GAS 的能力检查、消耗／冷却、ActivationGroup 和属性初始化检查，死亡状态禁止激活。它不会写入 CurrentComboTag 或清空连段记忆；如果它中断普攻，普攻按原有结束流程保留记忆，记忆仍按原时间到期。

GA 默认允许远端结束独立技能。需要客户端取消同步到服务器时保持 GA 的 `Server Respects Remote Ability Cancellation=true`；若明确要求服务器独占结束，可关闭。普攻连段无论此默认值如何仍使用原有协调器授权流程。

## 2. 新的可复用 Profile

位于 `/Game/Main/Combat/HitProfiles/`：

- `DA_Hit_VolumeOverlap`：ShapeQueryStrategy、Overlap、OnceOnEnter、DetectionAnchor，默认需要视线。适合一次爆发和 Timeline Point。
- `DA_Hit_VolumeSweep`：ShapeQueryStrategy、Sweep、EveryFrame、DetectionAnchor，保留大位移传送保护。适合跟随角色／Socket 的持续窗口。
- `DA_Hit_VolumeDash`：与 Sweep 相同，但开启 `Continuous Motion`，覆盖真实连续位移路径。
- `DA_Hit_ConfirmedTarget`：一次采样、默认不要求视线，用于已经服务器确认的目标。它不进行范围查询，但保留目标距离与伤害资格检查。

修改 Profile 会影响所有引用它的技能；需要不同墙体／角度／采样政策时复制 Profile。已有 SocketSweep 和组件 BoxSweep 保持原用法，必须使用 QueryMode=Sweep。新 Overlap 使用配置形状。

## 3. 配置命中形状

在 Definition 的 `HitWindows` 或 `HitPoints` 条目中展开 `Volume`：

- `GeometryMode=ExistingSource`：旧行为，SourceTag 解析角色 HitSources 或武器 HitSources，现有组件／刀刃尺寸不变。
- `GeometryMode=ConfiguredShape`：本绑定提供形状；Profile 选择 ShapeQueryStrategy，通常使用上述 Volume Profile。
- `Shape=Sphere`：`SphereRadius` 是厘米半径。
- `Shape=Box`：`BoxHalfExtent` 是三个半尺寸，(200,100,150) 对应完整尺寸 400×200×300cm。
- `Shape=Capsule`：`CapsuleRadius` 和 `CapsuleHalfHeight`，半高包含端帽且不小于半径；默认沿锚点 Z 轴，通过 LocalTransform 旋转可调整。
- `LocalTransform`：相对锚点的位移、朝向。Scale 必须为 (1,1,1)，尺寸由上述字段控制；配置形状不继承角色／Mesh 的缩放。

锚点选择：

- `AvatarRoot`：角色根组件／胶囊坐标。角色根通常在胶囊中心，地面区域需要根据角色实际高度设置 Z 偏移。
- `RegisteredSource`：SourceTag 对应的来源组件，可通过单个 `AnchorSocket` 取骨骼／插槽。与刀根刀尖的 Sockets 列表分开。
- `ExecutionTransform`：通过 `AnchorKey` 读取当前服务器 GA 提供的世界变换。
- `ExecutionTarget`：通过本绑定的 `TargetKey` 读取服务器确认目标的当前变换。

`TransformPolicy=Follow` 每次采样解析当前锚点；`SnapshotOnEventEnter` 在该段开始时固定。对一个大招的多个 Point，Snapshot 是每段单独固定；若要所有段使用同一个施法位置，在 Prepare 事件中一次记录 ExecutionTransform，之后不要更新该键。

`MaxAnchorDistance` 限制运行锚点离角色的距离，默认 5000cm。缺失运行锚点／Socket 会失败，不会退回角色位置或世界零点。

SourceTag 只在 ExistingSource／RegisteredSource 路径必需。AvatarRoot、ExecutionTransform 和 ExecutionTarget 不要求在角色新增 HitSources 或 BoxComponent。

## 4. 配置时间与多段次数

### 挥刀／突进：HitWindows

Timeline 添加 `Kind=Window`，设置 StartTime／EndTime 和 WindowTag。Definition.HitWindows 的 WindowTag 必须精确相同。

三次同类型挥击可以放三个不重叠的同标签 Window，复用一个绑定；每个条目拥有自己的 EventIndex、会话和命中记录。每个 EventID 必须唯一，但它只是时间轴条目身份，不用来决定伤害类型。

Profile.SampleMode：

- EveryFrame：整个窗口持续查询，之后进入范围的敌人也可以被命中。
- OnceOnEnter：窗口开始时只查询一次，迟到敌人不被命中，关闭窗口不会补一次查询。

### 明确的爆发时刻：HitPoints

Timeline 添加 `Kind=Point`，PointEventTag 例如 `GameplayEvent.Attack.Hit.Pulse`，StartTime 设置爆发时刻，NetPolicy 选择 AuthorityOnly 或 LocalAndAuthority。PointEffectClass 留空。

Definition.HitPoints 添加同一个 PointEventTag，配置 Volume／Profile／DamageEffect。同标签的多个 Point 复用一个绑定，但逐条独立执行，一帧跨过多个 Point 不会把它们合成一次。

终结伤害需要不同倍率／范围时，使用另一个消息标签，例如已注册的 `GameplayEvent.Attack.Hit.Finisher`，另加一个绑定。不要在同一个绑定数组中重复同一个标签。

Point 只采样一次，无论 Profile.SampleMode 的值如何。Point 的源 GE 不能代替目标范围伤害，因此命中 Point 禁止组合 PointEffectClass，也禁止 LocallyControlledOnly。

## 5. 伤害和去重

- `DamageEffect`：显式指定 Instant GE，包含 HodgeDamageExecution。可以使用当前正式普攻的 GE，或制作对应技能的子类。
- `DamageMultiplier`：写入 SetByCaller.DamageMultiplier；最终伤害仍包含 GE 对捕获属性的修正及来源衰减，并非一定等于裸 BaseDamage × 倍率。
- `RepeatHitInterval=0`：该记录范围内每个目标最多命中一次；正数允许最短间隔后再命中，但不等于固定脉冲调度。
- `HitGroup` 留空：每个 Window／Point 独立去重，同一目标可以在下一段再次受伤。
- 非空组 + `HitGroupScope=Execution`：同一次 GA 执行共享记录。间隔为 0 时整个组总共只命中该目标一次，保留原有语义。
- 非空组 + `HitGroupScope=TriggerTime`：同一组、相同 StartTime 的条目共享一段记录；下一个不同 StartTime 的攻击段重新获得机会。时间需精确相同。

例：同一刀同时开启“刀刃”和“辅助盒体”两个不同标签窗口，两者 StartTime 相同，HitGroup=Slash、Scope=TriggerTime。任意来源命中后本刀不重复扣血；下一刀复用绑定并改变开始时刻，可以再次命中。

共享组的 RepeatHitInterval 必须一致。组只属于本次 GA 执行，不跨其他 GA；取消和再次激活会重新创建记录。

## 6. 指定目标与世界位置的蓝图接线

在继承 HodgeGameplayAbility_Melee 的 GA 中覆盖返回 bool 的 `PrepareHitExecutionContext`。该事件在服务器调用，位于 Timeline 开始之前，不能把它当作客户端输入事件。

固定世界中心：

1. 获取角色／目标的世界位置，构造单位缩放的 Transform。
2. 调用 `SetHitAnchor(Key="Center", WorldTransform=...)`，检查返回值。
3. 返回 true。
4. 绑定选择 ExecutionTransform，AnchorKey=Center。

指定敌人：

1. 从服务器目标选择逻辑取得候选 Actor；没有锁定系统时可先用 SphereOverlapActors 筛选候选、按距离选择。
2. 调用 `SetHitTarget(Key="Enemy", Target=...)`，检查返回值。
3. 返回 true。
4. 绑定 TargetKey=Enemy。

TargetPolicy：

- AnyInVolume：范围内所有合法目标。
- LockedTargetInVolume：仍做几何检测，仅保留已选择目标；目标躲开范围就不受伤。
- ConfirmedTarget：仅对确认目标结算，不依赖碰撞是否开启；仍检查目标存活／有效、MaxTargetDistance 和 Profile 的过滤政策。适合确认捕获后的连续演出。

TargetKey 同时用于 ExecutionTarget 锚点。`MaxTargetDistance` 默认 2000cm，创建及采样都检查；超出后当前段无效。返回 false 可取消本次技能，不应将无效目标换成自身或世界原点。

SetHitAnchor／SetHitTarget 都要求当前服务器执行有效，客户端调用不会安装命中上下文。移动的 Follow 世界锚点可由服务器技能逻辑再次调用 SetHitAnchor 更新；Snapshot 会话不受后续更新影响。目标跟随优先使用 ExecutionTarget + Follow。

本次没有实现一个通用的客户端目标数据 RPC 或锁定组件。位置来源须由服务器选择／验证；未来接入客户端候选目标时需另接 GAS TargetData 验证，不应把客户端坐标直接当成可信结果。

## 7. 高速突进、瞬移和表现

Continuous Motion 适合真正连续的突进，可扫过上一位置到当前位置的大位移路径。默认不开启时，超过 MaxSweepDistance 的变化视作传送，只检测新位置。

同一个连续窗口内发生瞬移时，在服务器完成位置变更后调用 GA 的 `ResetHitGeometryHistory`；它只重置几何轨迹，不恢复已经消耗的命中机会。或者将瞬移前后拆成两个窗口，按需要选择共享去重组。

当前采样覆盖相邻实际采样点间的直线路径，旋转形状使用有限子步；不包含任意曲线的历史还原或完整骨骼姿势回放。高速曲线运动应由位移系统提供足够的路径更新，不能仅靠扩大阈值声称还原整个动画。

凤凰仅是角色表现时，使用角色或服务器锚点检测，Niagara 不决定伤害位置。可以用几段突进 Window 加一个落地 Point；凤凰独立离开角色后继续运行属于后续技能 Actor 阶段。

`RequiresWeaponInHand=false` 可以让远程／领域命中不要求落在手持窗口内。WeaponUseWindowTag 仍控制拔刀表现；需要握剑的条目保持 true 并让手持窗口覆盖命中条目。

## 8. 示例资产与调试

示例位于 `/Game/CodexText/SkillHitVolumes/`，属于测试／教学资产，不自动安装到正式角色：

- `GA_VolumeExample`：基础 GA。
- `DA_VolumeThreePulse`：三个 Point，一条绑定，每段倍率 0.5。
- `DA_VolumeThreeWindows`：三个 Window，一条绑定。
- `DA_VolumeOnceWindow`：长窗口只在开始查询一次。
- `DA_VolumeFixedCenter`：三个 Point 读取 Center，需要服务器设置锚点。
- `DA_VolumeConfirmed`：三个 Point 使用 Enemy，需要服务器设置目标。
- `DA_VolumeSharedExecution`／`DA_VolumeSharedTrigger`：对比整次执行与每段共享去重。
- `AS_VolumeLab`／`BP_VolumeLabGrant`：通过原有装备授予链在 PIE 中安装一个例子；测试时未新增武器 Actor 或常驻组件。
- `GE_VolumeLabHeal`：测试之间恢复血量，使用 HodgeHealthSet.Healing，正式玩法不依赖它。

FixedCenter／Confirmed 例子的基础 GA 没有完整目标选择蓝图。复制 GA 并按第 6 节覆盖 Prepare 事件后使用；自动化验证是在服务器执行开始后、第一段命中前显式提供了上下文。

配置形状沿用 `Hodge.Combat.DebugDraw.HitBox=1` 和 `Hodge.Combat.DebugDraw.Duration=5`，服务端／Listen Server 绘制；客户端不复制查询调试线。绿色表示几何查询有结果，不保证通过资格／去重后实际扣血。

Overlap 的命中点是碰撞最近点或目标位置，不是刀刃真实接触点；ResultKind 可区分 Sweep／Overlap／ConfirmedTarget，缺失的骨骼和物理材质不伪造。

实施设计和范围见 [设计提案](skill-hit-volumes-proposal.md)。实际构建和测试结果见本轮验证记录，不能仅根据示例资产存在判断功能通过。
