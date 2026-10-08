> 2026-10-07：本文保留为旧方案／验证历史。dev-AN 已改为 Montage 动画通知，当前配置请看 [攻击能力配置手册](../Guides/attack-ability-configuration.md)，旧文件见 [集中归档](../../Archive/Timeline/README.md)。

# Timeline 命中窗口接入

> 2026-10-06 状态同步：命中框架和测试夹具已有实现与伤害验证；正式五段 DA_Attack 的 HitWindows 仍为空，需配置后补实际伤害回归。 当前项目事实见 [本轮更新](../KnowledgeBase/26-update-2026-10-06.md)。

> 后续更正：本文所述原生 HodgeMeleeDamageEffect 和角色默认 CombatComponent 均已移除。当前配置显式引用既有 GameplayEffectParent_Damage_Basic，并由 Experience 的 AddComponents 添加判定组件；下文仅保留历史事实。

> 2026-10-01：本文已转为上一版实现与测试资产的历史记录。后续编码以 [近战命中检测与 GA 效果应用：重构设计](melee-detection-ga-effects.md) 为依据。本文中“CombatComponent 构建并提交伤害 GE”、组件驱动 Timeline 时钟及组件持有攻击去重规则的方案已被替代；下方旧配置步骤不作为新设计的接入验收依据。原生职责重构已编码，蓝图迁移和运行验证待执行，具体步骤见新文档第 13 节。

状态：2026-09-30 已通过 UnrealMCP 在当前编辑器运行 8 项 Timeline / Combat 自动化测试，全部成功；新建的 8 个 CombatHitWindows 示例资产全部通过数据校验。本次未执行构建、蓝图编译、PIE、联机或打包，尚未验证实际扣血。下文 Main/Data/Combat 路径仍为建议；实际创建的示例及操作步骤见文末。

## 职责与生命周期

- Timeline 仍只负责时间、WindowTag、窗口 GE 和 Point；新增带 EventIndex 的窗口进入/退出通知。
- Definition GA 每次激活生成独立 FGuid，将命中窗口绑定转交 Pawn 的 CombatComponent。
- CombatComponent 每个窗口保存独立会话：来源、上下文、几何历史、命中记录和伤害参数；不通过 ASC 的聚合 Tag 计数创建会话。
- 检测策略只返回几何命中；组件统一过滤目标、遮挡、角度和重复接触，再向目标 ASC 提交 GE。
- 默认 HodgeMeleeDamageEffect 使用 HodgeDamageExecution，公式为 BaseDamage × DamageMultiplier × 原有距离/材质倍率。
- HealthSet 继续处理无敌、自毁例外、生命值扣减及死亡通知；本次没有实现格挡、暴击或新的受击表现。

AHodgeCombatCharacter 已创建名为 CombatComponent 的原生组件，不需要再在蓝图或 GameFeature 中添加一份。它不复制会话，只有服务器执行判定与伤害。

进入窗口立即检测；活跃窗口在 PostPhysics 更新，正常退出补最后一次采样；取消、Task 销毁、GA 结束、死亡开始、ASC 解绑、Pawn 销毁只清理，不补伤害。组件已对角色 Mesh 建立 Tick 前置依赖。第一次进入和边界采样使用当时可用的姿态，不主动重新计算动画。

每个窗口默认对同一目标 ASC 提交一次伤害。RepeatHitInterval > 0 时，按世界时间间隔允许重复伤害；这是重复命中间隔，不是几何采样间隔。免疫或拒绝的 GE 也消耗本次接触机会。暂停动画不会暂停世界时间，因此仍可对窗口内持续接触按此间隔提交伤害。

HitGroup 留空表示窗口独立去重。同次 GA 执行中相同 HitGroup 共享记录，直到该次执行结束；同组的 RepeatHitInterval 必须一致。旧执行结束不会清理其他执行的会话。

## 1. 建立基础伤害属性

从 Experience 实际使用的 PawnData 找到 AbilitySets；磁盘上已有 `/Game/Main/Data/AbilitySet/DA_Pover`，但需确认它是否仍是当前玩家使用的资产。

1. 在角色 AbilitySet 的 GrantedAttributes 中添加 `HodgeCombatSet`，只授予一份；如果已存在则不重复添加。
2. 建议创建 `/Game/Main/Data/Combat/GE_CombatBaseAttributes`，父类 GameplayEffect，DurationPolicy = Infinite。
3. 添加 Modifier：Attribute = HodgeCombatSet.BaseDamage，Modifier Op = Add，Magnitude = Scalable Float 20。
4. 将这个 GE 添加到同一个角色 AbilitySet 的 GrantedGameplayEffects，EffectLevel = 1。

角色主动技能与基本属性由 PawnData 的 AbilitySet 提供；不要再让每件武器添加一份 CombatSet。武器属性 GE 可以修改已经存在的属性。

目标必须有有效 ASC、HealthSet、正数 Health，并能通过 Actor 查询到 ASC。当前 EnemyCharacter 的 ASC 初始化缺口没有在本任务中重写。可先使用现有拥有 PlayerState ASC 的 Hero 作为受击目标验证；普通静态 Actor 或只有网格的敌人不会自动获得受击能力。

默认禁止自伤及同一个 ASC 之间的伤害。队伍从 Actor 或 Pawn 的 Controller 的 IGenericTeamAgentInterface 读取；双方都有有效且相同队伍 ID 时默认禁止友伤，绑定的 AllowFriendlyFire 可以放开。当前未接入该接口的角色按无队伍处理，彼此可伤害；这不代表项目已完成队伍系统。

## 2. 创建检测配置

创建 Data Asset，选择 `HodgeHitDetectionProfile`。

建议创建 `/Game/Main/Data/Combat/DA_Hit_SocketSweep`：

- Strategy = HodgeSocketSweepStrategy。
- ObjectTypes = Pawn；若目标使用自定义 Object Type，要加入该类型。
- RequireLineOfSight = true，ObstructionChannel = Visibility。
- HalfAngleDegrees = 180 表示不限制朝向；前方扇形可用 60。
- MaxSweepDistance = 300 cm，超过后视为传送，仅检测当前位置。

建议创建 `/Game/Main/Data/Combat/DA_Hit_BoxSweep`：Strategy = HodgeBoxSweepStrategy，其余按需要配置。RotationSubsteps 默认 8，仅盒体策略使用。

策略在类默认对象上无状态执行。新增算法时派生 UHodgeHitDetectionStrategy，实现 Capture 和 Detect；可变状态继续放在会话的 FHodgeHitGeometry 中，不写入 Profile 或策略默认对象。Detect 只产生候选命中，不施加伤害、不触发技能切换。

## 3. 配置来源

SourceTag 采用精确匹配。角色来源与所有已装备武器来源合计只能匹配一条；重复或缺失时窗口拒绝启动并输出日志。

### 武器

打开已有 `/Game/Main/Weapon/BP_WeaponInstance_Sword`，在 Hodge → Combat → HitSources 添加：

- SourceTag = Combat.Source.Weapon.MainHand。
- WeaponActorIndex = 0，对应 EquipmentDefinition.ActorsToSpawn 的实际生成顺序。
- ComponentName：刀剑网格组件的对象名；若网格就是根组件则留空。
- Sockets：填写武器网格上实际存在的刀根、刀尖 Socket，例如 weapon_base、weapon_tip；示例名不是自动创建的插槽。
- SegmentSamples = 5，沿两个 Socket 的连线建立 5 个采样点。
- Radius = 10 cm，LocalOffset = 0。

每个采样点执行上一帧到当前帧的球扫掠。多个 Socket（超过两个）按各自位置独立采样；空列表使用组件原点。找不到 Socket 会报错，绝不悄悄改为本体攻击。武器外观碰撞仍可保持 NoCollision。

### 本体 / 脚部

从实际 PawnData.PawnClass 打开角色蓝图，选择原生 CombatComponent，在 HitSources 配置：

- 本体：SourceTag = Combat.Source.Body.Origin，ComponentName = CollisionCylinder，Sockets 留空，LocalOffset = (100, 0, 0)，Radius = 60。当前 BP_Hero_Pover 原生胶囊对象名已通过编辑器核实为 CollisionCylinder；这些参数表示身体前方一个球形区域。
- 脚部：SourceTag = Combat.Source.Body.RightFoot，ComponentName 留空（使用角色 Mesh），Sockets 填实际右脚骨骼或 Socket 名，Radius 例如 15。

两种来源均可使用 SocketSweep 配置。本体的局部偏移跟随指定组件变换，角色 Mesh 与胶囊的坐标朝向可能不同，应按所选组件配置。

### 指定碰撞盒

在角色蓝图添加 BoxComponent，建议命名 `HitBox_Chest`，附着到需要的骨骼或组件，调整 BoxExtent 和相对变换。盒体可设为 NoCollision，它只提供查询形状，不依赖 OnBeginOverlap 或 GenerateOverlapEvents。

在 CombatComponent.HitSources 添加：SourceTag = Combat.Source.Hitbox.Chest，ComponentName = 该盒体实际对象名。绑定使用 BoxSweep 配置。此策略读取盒体世界变换和缩放后的 BoxExtent，不使用来源的 Sockets、Radius 或 LocalOffset。

## 4. 连接 Definition 与 Timeline

项目中已有 `/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1` 及 `/Game/CodexText/DefinitionCombo/DA_Attack_1_Timeline`。先从当前 AbilitySet 的 GrantedDefinitions 确认实际引用，再修改对应资产，不修改备份目录。

在 Timeline 增加一条 Window：

- EventID = MeleeHit，仅作条目身份和显示。
- StartTime = 0.30、EndTime = 0.42，需落在该 Montage 的有效时长内。
- WindowTag = Status.Attack.HitCheck.Weapon。
- WindowEffectClass 留空。

在对应 AbilityDefinition 的 Hodge → Combat → HitWindows 增加：

- WindowTag = Status.Attack.HitCheck.Weapon。
- SourceTag = Combat.Source.Weapon.MainHand。
- Profile = DA_Hit_SocketSweep。
- DamageEffect 留空，使用内置 HodgeMeleeDamageEffect。
- DamageMultiplier = 1.5。
- DamageType = GameplayEffect.DamageType.Melee。
- RepeatHitInterval = 0，HitGroup 留空，AllowFriendlyFire = false。

上述属性初值 20、倍率 1.5 且无其他修正时，每个目标本窗口预期受 30 点伤害。

本体与盒体的 WindowTag 分别可用 Status.Attack.HitCheck.Body / Status.Attack.HitCheck.HitBox，SourceTag 和 Profile 改为相应配置。一个标签只绑定一次，同标签的多个顺序窗口复用配置但拥有独立会话。

伤害在命中时创建 Spec，BaseDamage 在该时刻快照；本段倍率等请求参数在窗口开始时复制进会话。DamageType 作为动态 AssetTag 进入 Spec，目前不自动派生元素倍率。

如需自定义 DamageEffect，它必须为 Instant 并包含 HodgeDamageExecution 或其子类。可从 HodgeMeleeDamageEffect 派生蓝图配置 GameplayCue；不要另外用 Modifier 重复写入 Damage，也不要重复添加相同 Execution。Context 保留技能来源，并为每个命中独立复制 HitResult；武器窗口的 SourceObject 指向对应 WeaponInstance。

## 边界与后续验证

- 时间轴能报告一帧跨过的短窗口，但没有历史姿态缓存或服务器回溯；首次及末次检测使用当前可取得的姿态，是近似结果。
- 刀身采样点越稀疏、单帧转角越大，越可能漏掉中间圆弧；盒体旋转采用有限次插值，同样不保证连续旋转的精确覆盖。
- 服务器上相关骨骼必须实际刷新。未渲染 Mesh 的 VisibilityBasedAnimTickOption、动画更新优化和武器骨骼更新策略由项目根据性能需求配置，本次没有全局修改它们。
- 当前遮挡检查为攻击者到目标 Actor 中心的 Visibility 射线，不是按武器弯曲路径判断；复杂掩体可能需要后续替换遮挡策略。
- 当前只做服务器权威伤害，没有客户端命中预测、回溯验证或 Niagara/CueNotify 资产。
- 构建后重点验证：武器/脚/盒体各命中一次，多组件受击不重复扣血，同组去重，连续窗口独立，取消无尾伤，卸武器、换 Pawn、重生后无残留，服务器与客户端血量一致。

## 2026-09-30 MCP 验证与手动验收

通过项目 UnrealMCP 的 run_python_in_unreal 调用编辑器控制台命令：

```text
Automation RunTests Hodge.Timeline.+Hodge.Combat.
```

编辑器日志在 UTC 15:12:06 记录以下 8 项全部成功，随后记录 `Automation Test Queue Empty 8 tests performed`：

- Hodge.Combat.HitGeometry
- Hodge.Combat.HitProfileValidation
- Hodge.Timeline.CleanupAndOrdering
- Hodge.Timeline.RejectBeforeSideEffects
- Hodge.Timeline.SharedEvaluator
- Hodge.Timeline.Validation
- Hodge.Timeline.WindowCallbackReentry
- Hodge.Timeline.WindowIdentityAndCrossFrame

证据见 Saved/Logs/Hodgepodge.log；日志可能随下次启动轮换。这些测试覆盖几何采样、配置校验和 Timeline 的窗口通知/清理，不覆盖完整的攻击命中、GE 扣血或网络复制。后台限帧原先阻止自动化启动，测试期间临时关闭，结束已恢复原值 true。

### 已创建的示例

以下资产均位于 `/Game/CodexText/CombatHitWindows`，没有替换现有玩法引用：

- DA_Hit_SocketSweep、DA_Hit_BoxSweep：两种检测配置。
- DA_Test_Body、DA_Test_Weapon、DA_Test_HitBox：三种来源的攻击定义。
- DA_Test_Body_Timeline、DA_Test_Weapon_Timeline、DA_Test_HitBox_Timeline：各自独立的 Timeline。

Definition 复制自 DA_Attack_1，保留其 Montage、AbilityClass 和 AbilityTag；ExecutionConfig → TimelineTaskConfig → Timeline 已改为对应的新资产。每条 Timeline 只有一个 0.30–0.42 秒命中窗口；HitWindows 倍率 1.5，RepeatHitInterval 为 0，HitGroup 留空，DamageEffect 留空使用原生效果。这是单次攻击示例，没有复制连招过渡窗口。

EditorValidatorSubsystem.validate_assets_with_settings 实测：8 requested、8 checked、8 valid、0 invalid、0 warnings、0 skipped。配置合法不代表来源组件、伤害属性和受击目标已在运行时接好。

另一个已核实的引用：原 `/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1` 当前实际引用 `/Game/Main/Character/Hero/Ability/BasicAttack/Timeline/DA_Attack01_Timeline`，不是同目录下的 DA_Attack_1_Timeline。修改前始终从 ExecutionConfig 展开实际引用。

### 先跑通本体扣血

以下为用户接入和运行步骤，尚未代为执行。可先复制涉及的角色、PawnData、AbilitySet 和 Experience 到测试目录，再在测试地图引用副本；不要同时授予原定义与同 AbilityTag 的测试定义。

1. 当前 `/Game/Main/Data/PawnData/DA_Dafult_PawnData` 使用 BP_Hero_Pover，AbilitySets 包含 DA_Pover 和 AS_LightCombo。以实际 Experience 的 DefaultPawnData 为准确认这条链。
2. 在玩家使用的 AbilitySet 的 GrantedAttributes 添加一份 HodgeCombatSet。当前 DA_Pover 的 GrantedAttributes 和 GrantedGameplayEffects 都为空。按本文第 1 节创建 GE，将 BaseDamage 加到 20，并通过 GrantedGameplayEffects 授予；若其他来源已有属性/加成，先核对运行时最终值。
3. 在角色蓝图原生 CombatComponent 的 HitSources 添加本体来源：SourceTag=Combat.Source.Body.Origin，ComponentName=CollisionCylinder，LocalOffset=(100,0,0)，Radius=60，Sockets 留空。当前 BP_Hero_Pover 的 HitSources 实测为空。
4. 在实际使用的 AS_LightCombo 的 **GrantedAbilityDefinitions** 中，将第一项 DA_Attack_1 替换为 DA_Test_Body，其余项保留。三个测试 Definition 继承同一个 AbilityTag，只能选一个替换，不能同时添加。这里只测试单击第一段，不连续按键推进连招。
5. 编译并保存自己修改的蓝图/资产。打开可正常生成玩家的测试地图，以 2 Players、Play As Listen Server 启动 PIE。使用第二个已被控制的玩家作为目标；仅拖入一个 Hero 蓝图并不保证它有 PlayerState/ASC。
6. 让攻击者面向目标，目标中心位于前方约 120 cm，无遮挡。在调试器中查看目标 HodgeHealthComponent.GetHealth，或目标 PlayerState ASC 的 HodgeHealthSet.Health；初始设为或确认是 100。可用 `showdebug abilitysystem` 辅助查看属性，但要确认当前调试对象是目标。
7. 单击一次攻击，BaseDamage=20 且没有其他伤害修正时，目标应从 100 变 70；同一窗口持续接触不能继续扣血。等待技能完全结束后再攻击一次，应从 70 变 40。
8. 目标移到攻击范围外再打，应不扣血；在两者间放置阻挡 Visibility 的墙，应不扣血。交换攻击者与目标，客户端发起攻击也应由服务器扣血，两端看到相同 Health。

### 再切换来源与验证清理

1. 武器：按第 3 节配置 WeaponInstance.HitSources，使用实际网格组件名和 Socket；将 GrantedAbilityDefinitions 第一项替换为 DA_Test_Weapon。命中应每窗口扣 30，刀身多个采样点接触同一角色仍只能扣一次。
2. 碰撞盒：角色添加 BoxComponent，确认运行时对象名后配置 Combat.Source.Hitbox.Chest；将第一项替换为 DA_Test_HitBox。NoCollision 也应能判定，因为这里主动查询盒体几何。
3. 窗口进入前取消技能，应没有伤害；已命中后取消并留在范围内，应没有尾随伤害。卸武器、死亡、ASC 解绑或换 Pawn 后不应继续造成旧窗口伤害。
4. 重生后再次攻击，默认武器和判定应恢复，不应一次攻击重复扣血。最后再验证两个独立窗口各扣一次，以及同 HitGroup 的两个窗口合计只扣一次。

若不扣血，先检查日志里的来源缺失/重复、Socket 无效、CombatSet 缺失；再核对真实 Timeline 引用、目标 ASC/HealthSet、BaseDamage、查询 ObjectTypes、Visibility 遮挡和无敌标签。不要先调大伤害掩盖初始化或引用问题。
