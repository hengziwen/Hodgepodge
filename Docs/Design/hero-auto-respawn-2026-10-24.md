# Hero 自动重生实施规范

日期：2026-10-24。引擎：UE 5.5.4。状态：**首版已实现并完成本轮验证**；范围和未验证项见[验证报告](../Validation/hero-auto-respawn-2026-10-24.md)。

本文记录本轮已批准的实现范围、Lyra 复用关系、生命周期约束与验收要求。源码、资产导出和历史测试用于确定实现依据；实际构建、PIE 与联机结果见本轮验证报告。正式接口和配置见第 3 节。

## 1. 目标与范围

首版实现正式玩家 Hero 死亡后的自动重生：保留 Controller、PlayerState、ASC、角色成长和库存身份，完整执行原死亡流程，解除旧 Avatar 后，通过现有 GameMode 生成新 Pawn 并完成正常初始化。

| 项目 | 本轮约束 |
| --- | --- |
| 目标角色 | 仅玩家 Hero；普通敌人、Dummy、Boss、AI Bot 不接入 |
| 触发方式 | 服务器确认的完整死亡流程；不新增手动复活输入 |
| 出生位置 | 使用当前 GameMode 的默认 PlayerStart 流程 |
| 死亡表现 | 保留现有死亡 GA 及其时序，不提前强制结束死亡能力 |
| 重生前置 | 死亡完成、旧 Avatar 已解绑、原 Controller/PlayerState 身份仍匹配 |
| 额外等待 | 从死亡完成且旧 Avatar 已解绑后起算；默认 0 秒，可配置；0 秒仍在下一有效 Tick 执行 |
| 生成失败 | 默认包含首次共 3 次生成尝试，即首次失败后最多再试 2 次；次数与默认 0.5 秒间隔可配置，耗尽后明确停止 |
| 基础架构 | 现有 Experience、PawnData、PlayerState ASC、Init State、属性协调器和库存/装备链 |
| 网络 | 服务器调度，复用现有 Pawn、ASC、属性和装备复制；不增加重生 RPC 或另一个复制状态机 |

额外等待不替代死亡表现时长。额外等待与失败重试是两个独立参数，重试不能重新触发死亡流程。参数保留在现有配置体系内，不能把手感数值散落在 GameMode 或测试代码中；实际字段和合法范围随实现核验记录。

## 2. Lyra 核查依据与适配差异

本机参考项目：`E:/Project/UProject/study/Epic/LyraStarterGame`。本轮同时查阅源码和实际资产导出的 T3D，不以同名类或磁盘字符串代替调用链确认。

### 2.1 资产证据

只读导出保存在本机 `Saved/PlayerRespawn_20261023/Inspection/LyraStarterGame/`；该目录为本地证据，不纳入源码提交。

| Lyra 资产/导出 | 已核实内容 | 本项目处理 |
| --- | --- | --- |
| `/ShooterCore/Game/Respawn/GA_AutoRespawn`；`GA_AutoRespawn.t3d`、`report.json` | OnSpawn；死亡监听/解除、`K2_OnPawnAvatarSet`、Avatar EndPlay、`RequestPlayerRestartNextFrame`；默认等待 5 秒 | 复用持久被动能力调度、换 Avatar 重绑和清理职责；等待采用本轮批准值 |
| 同一 GA 的实际默认值 | `NetExecutionPolicy=LocalPredicted`，`NetSecurityPolicy=ServerOnly`；二者不是同一个属性 | 本轮明确使用原生 `ServerOnly` 执行策略，将调度限制在服务器；不能声称是逐字段原样复制 |
| 同一 GA 的消息/UI 分支 | GameplayMessage、UIExtension、重生消息与倒计时界面；还包含即时 Reset 分支和强制结束死亡能力的路径 | 首版不迁入这组依赖，不增加倒计时 UI、即时 Reset 或强制截断死亡 |
| `/ShooterCore/Elimination/AbilitySet_Elimination` | 实际授予 `GA_AutoRespawn` | 通过当前 PlayerState 的基础 AbilitySet 授予原生重生能力 |
| `GA_SpawnEffect.t3d`、`GE_SpawnIn.t3d` | 出生效果属于独立资产链 | 不随重生调度一起迁入，不增加出生特效或保护效果 |

Lyra `GA_AutoRespawn` 图内仍有临近重生取消死亡可能导致 Pawn 初始化不完整的 TODO。本轮采用“完整死亡并解绑后生成”，并验证重复请求和旧回调边界，不把示例图当作所有异常场景均已解决的保证。

当前 Hero 只读导出位于同目录的 `Hodgepodge/`：`GA_Hero_Death` 的导出默认 `Duration=2.5`、`ServerInitiated`、`bAutoStartDeath=true`，图中包含死亡等待与能力结束路径。该数值描述导出快照，首版不因新增重生调度修改它；最终资产读回结果仍须记录。

### 2.2 源码复用链

| Lyra 文件/函数 | 保留机制 | Hodge 对应与必要适配 |
| --- | --- | --- |
| `Source/LyraGame/AbilitySystem/Abilities/LyraGameplayAbility_Death.cpp`：`ActivateAbility`、`EndAbility`、`StartDeath`、`FinishDeath` | 死亡事件触发、取消普通能力、结束时完成死亡 | 已迁入 `UHodgeGameplayAbility_Death`，继续使用 |
| `Source/LyraGame/Character/LyraHealthComponent.cpp`：死亡阶段及委托 | 死亡状态推进、服务器事实与客户端表现同步 | 使用当前 `UHodgeHealthComponent`，保留项目资源清理规则 |
| `Source/LyraGame/Character/LyraCharacter.cpp`：死亡结束、解绑销毁 | 旧身体完成清理后退出控制 | 使用当前 `AHodgeCombatCharacter::OnDeathFinished`、`DestroyDueToDeath`、`UninitAndDestroy` |
| `Source/LyraGame/Character/LyraPawnExtensionComponent.cpp`：`InitializeAbilitySystem`、`UninitializeAbilitySystem` | 持久 Owner 与可替换 Avatar 分离、SurvivesDeath 能力保留、输入清理 | 使用当前 PawnExtension；其旧 Pawn 资源通知适配继续保留 |
| `Source/LyraGame/AbilitySystem/LyraAbilitySystemComponent.cpp`：`InitAbilityActorInfo`；`LyraGameplayAbility`：`OnPawnAvatarSet`、`TryActivateAbilityOnSpawn` | 新 Avatar 到达通知与 OnSpawn 激活 | 使用当前 Hodge ASC/GA 入口，不从新 Pawn 重复授予能力 |
| `Source/LyraGame/GameModes/LyraGameMode.cpp`：`RequestPlayerRestartNextFrame`、`PlayerCanRestart`、`FailedToRestartPlayer`、`SpawnDefaultPawnAtTransform` | 服务器资格检查、GameMode 生成及 PawnData 注入 | 增加窄范围的受检单次 Restart，失败由当前重生能力有界调度 |
| `Source/LyraGame/Equipment/LyraEquipmentManagerComponent.cpp`：`UninitializeComponent`、装备移除 | 旧 Pawn 实际装备清理 | 使用当前 Hodge 装备的授予 ASC/句柄账本和库存选择适配 |

本轮不引入 Lyra PlayerSpawningManager、团队出生点评分、CommonGame、CommonUser、GameplayMessageRuntime 或整个 ShooterCore。Hodge 已有属性协调器能区分首次出生、普通换绑和明确重生，继续使用其最终生命恢复逻辑，不复制 Lyra 绑定时补满生命的临时处理。

## 3. 职责与配置组织

### 3.1 原生自动重生能力

`UHodgeGameplayAbility_AutoRespawn` 继承现有 Hodge GA，配置为 `OnSpawn`、`ServerOnly`、`Independent`，带现有 `Ability.Behavior.SurvivesDeath` 标签。由 PlayerState 基础 AbilitySet 授予一次，随持久 ASC 跨 Pawn 继续工作。

现有 `UHodgePawnData` 增加以下配置，正式只在 `/Game/Main/Data/PawnData/DA_Dafult_PawnData` 启用：

| 字段 | 默认值／合法范围 |
| --- | --- |
| bEnableAutoRespawn | 默认 false，正式 Hero 为 true |
| AutoRespawnDelay | 0 秒；必须有限且非负 |
| AutoRespawnMaxAttempts | 3；包含首次，必须至少 1 |
| AutoRespawnRetryInterval | 0.5 秒；必须有限且大于 0 |

Runtime 与 `IsDataValid` 共用 `ValidateAutoRespawn`。禁用配置不因休眠值阻止旧资产加载，启用但非法时明确报告并拒绝监听，不取消原本的死亡流程。

能力只负责以下事项：

1. 激活及 `OnPawnAvatarSet` 时识别当前正式 Hero、PlayerState 和 Controller，只监听当前生命组件的 `DeathFinished`，绑定时补查该状态；通过 PawnExtension 反初始化/Avatar EndPlay 确认清理。
2. 记录本次生命/重生请求身份，用弱引用和代际校验隔离旧回调；重复通知不能新开并行重生任务。
3. 完整死亡和旧 Avatar 解绑均满足后才开始额外等待，按资格与次数限制提交单次生成尝试；不以 DeathStarted 开始倒计时。
4. 成功接管新 Avatar 后清理旧请求并绑定新身体；能力撤销、结束、对象失效或新身体已由其他合法路径接管时使旧任务失效。
5. 按来源清理本能力持有的委托和 Timer，不清空其他能力、装备或系统持有的资源。

能力不直接 Spawn/Possess Pawn，不手工调用属性回血，不复制库存，不重新授予基础能力，不清理所有 GE，也不替代旧 Pawn 的死亡和战斗清理。

### 3.2 GameMode 单次生成入口

`AHodgeGameModeBase::TryRestartPlayerForAutoRespawn` 是本轮调度的受检单次入口。进入前核对服务器、Controller/PlayerState、Experience、当前 Pawn/Avatar 与启用配置；能力负责本次请求代际检查，不允许旧任务重置已有的新 Pawn。

实际生成继续走现有 `RestartPlayer`。由它保留 `ExpectRespawn` 标记、默认 PlayerStart、PawnData 注入、Possess 和现有初始化链。

该入口只在**当前 Controller 的当前单次调用作用域**内抑制 `FailedToRestartPlayer` 自行安排额外下一帧重试，使失败次数由重生能力唯一管理。作用域结束后恢复原逻辑；其他 Controller、首次进入游戏及其他现有生成入口不受影响。不能用全局布尔值永久关闭 GameMode 原重试，也不能在一次失败中同时排入 GameMode 和能力两条重试链。

“已提交请求”“成功生成 Pawn”“属性与装备已就绪”分别记录。生成失败不得报告成功，已生成但初始化失败不得反复生成新 Pawn 掩盖配置错误。

### 3.3 资产接线

按现有正式引用链核对：

- `/Game/Main/Experiences/Exp_HodgeDefaultExperience`。
- `/Game/Main/Data/PawnData/DA_Dafult_PawnData`。
- `/Game/Main/Data/AbilitySet/DA_Pover`。
- `/Game/Main/Character/Hero/BP_Hero_Pover`。
- `/Game/Main/Character/Hero/Ability/GA_Hero_Death`。

先读引用及现值，备份明确目标后修改基础 AbilitySet 的重生能力授予与必要配置，再保存并读回。保留现有库存开关、默认物品、装备、受击 Profile 和 Experience 的 Inventory/Selection 组件注入。基础、装备、GameFeature 的能力授予句柄保持各自归属，禁止把重生能力同时放入多个来源。

## 4. 完整流程与身份约束

1. 正常 Hero 通过 Experience、PawnData 和 PlayerState 基础能力完成初始化；重生能力在服务器进入待监听状态。
2. 伤害通过现有正式 GE/GAS 路径触发生命耗尽和死亡 GA。死亡开始立即执行既有战斗、移动、碰撞与资源处理，重生能力不监听 DeathStarted 或在此启动倒计时。
3. 死亡 GA 完成原表现后结束；生命组件推进至死亡完成，重生能力据此建立该生命的待处理身份。旧角色在既有后续 Tick 中解绑 Controller/ASC、清理装备并退出。
4. 确认完整死亡和旧 Avatar 解绑后开始额外等待。等待完成后，在安全的后续执行点检查请求仍有效，再调用 GameMode 单次入口；0 秒等待也只在下一有效 Tick 提交。
5. 生成失败按同一请求的次数与间隔重试；不支持的目标、失效的 Controller/PlayerState、权限或配置错误明确停止。上限耗尽停止并保留最后失败阶段。
6. 新 Pawn 经当前初始化链绑定同一 ASC，恢复库存所选装备；属性协调器按 Respawn 原因提交最终生命并发布当前 Avatar 的 Ready。
7. 新 Avatar 通知使旧代际的 Timer/委托回调失效，重生能力重新监听新身体。后续死亡开启新一轮；旧生命周期回调不能再次生成或清理新 Pawn。

必须区分“旧 Avatar 正常解绑”和“角色已被另一条合法路径接管”。前者是当前死亡重生的前置条件，后者终止旧请求。不能仅因为 `ASC->GetAvatarActor()` 为空就认为可以生成；仍需匹配死亡记录、Controller、PlayerState、世界和当前能力身份。

退出、切图、能力移除、Controller/PlayerState 失效应清理本能力请求。未经确认的异常销毁不能自动等同完整死亡；EndPlay 可参与清理，但不绕过死亡完成前置条件。异步回调和同步广播重入均执行同样的身份检查。

## 5. 数据与状态契约

| 数据/状态 | 本轮行为与验收含义 |
| --- | --- |
| Controller、PlayerState、ASC、核心属性集 | 同一次连接内保留原对象；只替换 Pawn/Avatar |
| CharacterId、角色等级 | 沿用 PlayerState 成长数据，不重新随机生成或回到默认等级 |
| Health | 使用现有协调器的 Respawn 分支；无新增伤害时等于装备完成后的最终 MaxHealth；属性重建期间的合法伤害仍按既有规则结算 |
| 库存 | 保留 ItemId、Definition、Level、Count、StatTags，不重复发默认物品；OwnedEquipmentStates 继续只是库存的兼容镜像 |
| 装备 | 保留所选物品身份；旧 Pawn 的实际装备、Actor、授予句柄撤销，新 Pawn 重建实际实例；显式卸装跨重生保持空手 |
| 共鸣能量 | 沿用 `UHodgeHealthComponent::StartDeath` 的既有服务器清零；重生绑定不另清空、不补满 |
| 冷却 | 沿用持久 ASC 已有 GE 和时间推进，不统一撤销、重施或延长；等待期间冷却可以自然到期 |
| 暗流/暗涌 | 沿用 HavocRules 的死亡/解绑清零、结束会话及移除自身 Drain/State GE |
| 其他 GE | 沿用各来源现有生命周期；装备自有 GE 可随卸装撤销并重建，不承诺所有 GE 跨死亡保留 |
| 战斗与表现状态 | 旧连段、输入缓存、命中窗口、受击、锁定、运动/朝向占用、回放和校正按现有执行身份清理，不复制到新身体 |
| HUD | 继续跟随现有 Pawn 切换与属性就绪监听；不创建另一份生命、装备或倒计时事实源 |

客户端新 Pawn 的生命组件绑定到当前 ASC、且自身 `DeathState=NotDead` 时，清除前一身体遗留的本地死亡 Loose Tags；这是对 Lyra 绑定清理的最小适配，不写生命值或服务器死亡阶段。死亡通知和死亡事件转发均核对 `ASC.Avatar == 本组件 Owner`，旧身体只能推进自己的表现，不能给新身体写标签、清资源或发死亡事件。已死亡的当前身体重新绑定时不会被这条清理复活。

本轮“沿用现有行为”不等于共鸣能量保留死亡前数值，也不等于重新装备时保留原运行时装备 UObject。跨 PlayerState、更换 Experience、断线重连和跨图的数据迁移不在本轮实现范围。

## 6. 风险处理与约束

- **双重重生**：同一生命只允许一个待处理请求；死亡、完成、解绑、Timer 重入不能各自生成。新 Avatar 到达后旧请求永久失效。
- **重试相乘**：能力的有界重试必须独占本次尝试的重试责任；GameMode 的局部抑制不得影响其他玩家。
- **重复发放**：基础能力和库存继续复用幂等入口；不通过重新 SetPawnData 或重建 PlayerState 实现普通重生。
- **旧装备误清理**：使用授予时 ASC/句柄和所属 Pawn 校验；旧 Pawn 的迟到解绑不能撤销新身体装备或将新 Ready 改回 false。
- **网络顺序**：分别验证服务器、拥有者与模拟代理的 Pawn/Avatar 收敛；不能仅以服务器生成成功判定客户端可操作。库存和资源继续 OwnerOnly/Mixed 的既有可见性。
- **失败掩盖**：缺少能力、非法参数、缺失依赖、没有可生成 Pawn 与初始化失败分开诊断；无无限轮询或静默成功。
- **性能**：沿用事件、Timer 和原生成链；不为监听死亡新增每帧全世界扫描。多人同时死亡产生的生成/装备峰值需要实测，不作未测容量承诺。
- **工程边界**：保持单一 Hodgepodge Runtime 模块、UE 5.5.4、同类主 `.cpp` 和中文注释；测试/编辑器帮助代码不得无条件进入 Runtime 依赖。

## 7. 执行顺序与验收

优先顺序为：确认正式资产接线与配置 → 原生能力及 GameMode 窄入口 → 身份/重试测试 → Editor/Game 常规构建 → 冷启动蓝图检查与资产保存读回 → 真实死亡重生集成 → 验证报告。涉及反射布局时先正常关闭编辑器再构建，Live Coding 不替代常规构建。

### 7.1 原生与配置验收

| 编号 | 检查内容 | 通过标准 |
| --- | --- | --- |
| R01 | 能力策略与授予 | Hero 基础来源仅授予一份，OnSpawn/ServerOnly/Independent/SurvivesDeath 与批准方案一致 |
| R02 | 配置 | 默认等待 0 秒、含首次共 3 次尝试、间隔 0.5 秒读回一致；非法配置明确拒绝，不产生无界 Timer |
| R03 | 死亡前置 | 死亡开始不启动倒计时；DeathFinished 与旧 Avatar 解绑均满足才计额外等待；0 秒在下一有效 Tick 生成 |
| R04 | 去重与换绑 | 同代际重复通知/Timer 只保留一条请求；新 Avatar 及旧回调交错不再次生成 |
| R05 | 失败重试 | 次数受配置上限约束，间隔符合配置；耗尽后不再生成，诊断包含请求和失败阶段 |
| R06 | 局部影响范围 | 当前尝试不叠加 GameMode 重试；其他 Controller 和原生成入口保持原流程 |
| R07 | 清理 | 结束/移除能力、退出、失效对象后无有效旧 Timer 或监听；重复解绑安全 |
| R08 | 基础回归 | 复跑 Hodge.Attributes、Hodge.Inventory、Hodge.Resonance 及受影响死亡/战斗生命周期用例 |

已有 `Hodge.Inventory.EquipmentIdentityLifecycleAndGuards` 覆盖库存身份、换 Pawn、旧解绑与显式卸装保留；`Hodge.Attributes.AvatarAndBootstrapFailure` 覆盖普通换绑与明确重生的生命差异；共鸣测试已有死亡清能量断言。这些测试提供回归基础，不替代新增自动调度验证。

### 7.2 真实死亡重生集成

在正式 Hero、正式 Experience 和恢复后的正式 PawnData 上通过真实伤害入口致死。禁止用手动 `UnPossess/Destroy/RestartPlayer` 代替本轮自动死亡链的成功证据。既有 `Tools/Inventory/run_e2e.py` 的手动重启段可保留作库存回归，但须在报告中区分。

| 编号 | 场景 | 必须记录与断言 |
| --- | --- | --- |
| I01 | 正常死亡并重生 | DeathStarted、DeathFinished、旧 Avatar 解绑、尝试与新 Ready 的时间/身份顺序；完整死亡后才生成 |
| I02 | 连续多轮死亡与重复通知 | 每轮恰有一个新 Pawn；基础能力、物品、实际装备数量不增长 |
| I03 | 等级与装备保留 | CharacterId/Level、完整库存快照、选择 ItemId 保留；新装备 Level/Instigator 正确，旧装备 Actor/句柄清理 |
| I04 | 显式卸装后死亡 | 物品仍在库存，新身体保持空手，不静默恢复默认武器 |
| I05 | 生命与 Ready | 无新增伤害时 Health 等于最终 MaxHealth；属性和装备未就绪不能报告可操作；ASC Owner/Avatar 正确 |
| I06 | 共鸣、冷却与暗流 | 预置非零共鸣后死亡清零；长冷却按原截止时间推进；暗涌自身 GE/会话结束、暗流归零 |
| I07 | 战斗中死亡 | 攻击/冲刺/受击/击飞中的旧命中、方向、运动、动画、输入和相机资源按现有契约退出；新身体可正常行动 |
| I08 | 取消与迟到回调 | 倒计时中退出/换 Pawn/能力撤销及旧回调到达不生成额外身体、不清理新装备或新 Ready |
| I09 | 生成失败与多人交错 | 可控失败验证次数/间隔/停止；双玩家同时死亡各自独立，不互相吞掉或增加重试 |
| I10 | 网络收敛和隐私 | 同一 PlayerState 的新 Pawn/Avatar 各端一致；拥有者输入/HUD/选择恢复，模拟代理仍不可读私有库存 |

集成验证应分别记录单人、Listen 的主机/远端拥有者和 PIE Dedicated 结果，并覆盖本轮安排的延迟场景。具体执行模式、延迟、样本、退出码及未运行项进入后续报告；不把原生故障注入称为真实 RPC/网络测试，不把 PIE Dedicated 称为独立专服构建。

资产修改需有明确备份、保存与读回证据；测试夹具的内存改动退出时恢复，禁止保存临时测试值进正式资产。可调参数、技能授予和真实场景断言需一起验证，不能只验证 C++ 能编译。

## 8. 评估要求与交付对应

| 评估要求 | 实现范围 | 验收证据 |
| --- | --- | --- |
| 复用 Lyra 成熟职责 | 持久被动能力、Avatar 重绑、GameMode 生成；保留当前已迁入链路 | 源码/资产对应与 R01、I01 |
| 与当前架构兼容 | PlayerState ASC、基础授予、属性协调器、库存选择与装备清理 | R08、I03～I05 |
| 健壮性 | 完整死亡前置、弱引用、代际去重、有界重试、配对清理 | R03～R07、I02、I08～I09 |
| 性能与并发 | 无新增扫描 Tick，按 Controller 隔离一次尝试 | R06、I09；耗时与人数上限仅按实测报告 |
| 数据一致性 | 保留成长/库存事实，按原规则处理能量/冷却/暗流，各端初始化收敛 | I03～I06、I10 |
| 可审查与可复现 | 范围明确、合法参数和实际资产可读、真实死亡入口验证、失败留证 | 构建/资产/自动化/PIE 分项报告 |

本轮范围外：同一尸体原地复活、队友救援、重生 UI、出生特效/无敌、团队出生点策略、普通敌人/Boss/AI Bot 自动重生、统一 Buff/Debuff 清理策略、断线重连/跨 PlayerState/跨图存档、独立 Server Target、Cook 和打包。若验证发现必须改变这些契约才能完成首版，先提出具体差异，不用隐含扩展绕过边界。

本轮[验证报告](../Validation/hero-auto-respawn-2026-10-24.md)记录实际资产、参数、命令、252 项原生测试和 269 条集成断言，并保留失败证据与明确未验证项。后续范围扩展仍需独立安排，不把本次结果推导为所有网络条件或发布平台已通过。
