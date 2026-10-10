# 玩家 Dash 技能中断配置

## 目标与接入

当前玩家使用既有 Tag Relationship Mapping：Dash 激活时按标签取消所有攻击类技能，Death 标签与死亡状态优先豁免。不依赖攻击子类、蒙太奇 Slot、旧后摇窗口或旋转锁。死亡状态时禁止启动 Dash。

当前资产：

- `/Game/Main/Data/TagRelationship/DA_Hero_TagRelationshipMapping`
- `/Game/Main/Data/PawnData/DA_Dafult_PawnData` → `TagRelationshipMapping` 指向上述资产。
- `/Game/Main/Character/Hero/Ability/GA_Hero_Dash` → `Ability.Type.Action.Dash`，激活组为 `Independent`。
- `/Game/Main/Character/Hero/Ability/BasicAttack/GA_Attack_1`～`GA_Attack_5` → `Ability.Attack`、各自 `Ability.Attack.Light.01`～`.05` 和 `Ability.Type.Action.Melee`。
- `/Game/Main/Character/Hero/Ability/GA_Hero_Death` → `Ability.Type.StatusChange.Death`。

PawnExtension 按既有初始化链把 PawnData 的映射交给 PlayerState 上的 ASC，服务器和拥有者客户端各自执行相同规则。Pawn 更换继续使用既有初始化流程，无需新建额外组件或在 BeginPlay 手动再次授予能力。

## 映射内容

打开映射资产的 `Ability Tag Relationships` 数组。Dash 条目配置：

- `Ability Tag`：`Ability.Type.Action.Dash`。
- `Ability Tags To Cancel` 和 `Ability Tags To Block`：`Ability.Attack`、`Ability.Type.Action.Melee`、`Ability.Type.Action.WeaponFire`、`Ability.Type.Action.Grenade`。
- `Ability Tags To Cancel Exceptions`：`Ability.Type.StatusChange.Death`、`Status.Death`。
- `Force Cancel`：开启，允许取消攻击的临时不可取消阶段。
- `Activation Blocked Tags`：`Status.Death`，同时包含其子标签 `.Dying`／`.Dead`。

Death 条目使用 `Ability.Type.StatusChange.Death`，`Ability Tags To Block` 为 `Ability.Type.Action.Dash`。

取消目标采用层级匹配：父标签 `Ability.Attack` 匹配所有攻击子标签。目标只需在 GA 的 **Ability Tags** 中带有攻击分类，不能仅配置 Activation Owned Tags。ASC 也检查已授予 Spec 的 DynamicSpecSourceTags，兼容既有 Definition 授予的细分攻击标签。

这里中断的是同一玩家 ASC 上正在执行的技能。`State.Combat.Body.*` 属于受击强度判定，与技能分类标签用途不同，不能拿来代替 `Ability.Attack`；本轮不改变打中敌人时的受击规则。

## 优先级与取消过程

1. Dash 启动前检查角色死亡状态及活动死亡能力；死亡时拒绝启动。
2. ASC 汇总来源 GA 匹配的映射条目，应用 Block 和 Cancel 集合。
3. 对活动目标，先检查 Death 和映射中的取消豁免，再检查目标攻击标签。
4. 普通规则仍尊重 `CanBeCanceled`。只有匹配目标的映射条目显式开启 `Force Cancel`，才允许取消该不可取消阶段。
5. 通过 GAS 的 `CancelAbility` 正常退出，执行目标的 EndAbility、任务清理、窗口／命中检测／姿势租约释放及蒙太奇复制。
6. Dash 结束时移除自己施加的攻击阻塞。无关独立技能不因激活组替换而结束。

Death 保留底层不可移除的取消豁免，即使从映射删除 Death Exception，或某个活动能力同时带有攻击和 Death 标签，Dash 的映射取消仍不能结束它。该保护覆盖本次关系映射取消入口；直接调用其他系统的 CancelAbilities、EndAbility 或蒙太奇强制替换不属于此入口，调用者仍应遵守各自生命周期。

`Force Cancel` 会重新开放目标实例的可取消标志，然后调用正常取消流程，不直接强制 EndAbility。项目攻击 GA 必须在取消时释放自己的资源；不能用“不可取消”来代替正确清理。

联机中，服务器已死亡但拥有者尚未收到死亡状态时，本地可能短暂预测 Dash。服务器仍拒绝该动作；死亡激活／状态到达后本地撤销预测，之后再次按键也不能启动 Dash。Death 优先级保证服务器死亡流程不被取消，不能消除状态传播本身的延迟。

## 新攻击与例外

新增玩家攻击 GA 时，在 Ability Tags 加入 `Ability.Attack` 或其子标签，即可纳入当前 Dash 中断规则。原生 Melee 默认具有 `Ability.Type.Action.Melee` 分类；蓝图仍建议明确声明攻击标签，便于资产检查。

新增非死亡例外时，为目标配置独立 Ability Tag，再把该 Tag 加入 Dash 条目的 `Ability Tags To Cancel Exceptions`。任一匹配来源条目的豁免优先于所有取消条目。普通技能没有攻击标签则不被 Dash 中断；不能把一般的不可打断攻击误标为 Death。

Dash 当前占用主动作动画通道，活动期间阻止重新启动攻击；这是对动画所有权的约束。死亡能力应只声明死亡分类，不把 Death 能力本身配置成普通攻击分类。已活动的攻击／Death 双标签仍受取消豁免保护。

## 与原有取消机制的关系

`SprintAbilityProfile.AttackCancelWindow` 保留资产兼容，Dash 不再消费。普通移动取消攻击仍由 CombatComponent 的原有窗口控制；移动取消 Dash 后摇、Dash→Sprint 手动时间配置和 Root Motion Pivot 规则保持既有行为。当前旋转攻击不使用旋转锁，本系统不新增旋转锁交接。

Dash 无冷却，本轮不恢复体力系统。死亡时不会通过强制取消、激活组替换或后摇判断绕过限制。

## 复现与验证入口

打开默认地图 `/Game/ThirdPerson/Maps/ThirdPersonMap`，在各段普攻起手阶段点按 Shift，应立即中断攻击并进入 Dash；Dash 结束后攻击可正常启动。死亡中按 Shift，不得出现 Dash 或取消死亡流程。

资产配置与重载检查：`Tools/DashSprint/configure_interrupt_mapping.py`、`verify_interrupt_configuration.py`；运行检查：`run_interrupt_integration.py`、`run_combat_integration.py`。编辑器脚本由 `Tools/HitReaction/mcp_execute.py` 通过已认证的本地网关执行；不在 PIE 中重写正式映射资产。

自动化测试组 `Hodge.Relationship` 验证标签层级、豁免、强制授权和真实 GAS 取消。运行测试应另外检查拥有者、服务器与观察者的动画，以及攻击结束后的命中会话和姿势租约清理。实际结果以本轮验证报告为准。
