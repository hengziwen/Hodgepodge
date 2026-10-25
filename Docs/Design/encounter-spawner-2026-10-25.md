# 双模式刷怪点：实现方案与决策记录

日期：2026-10-25。范围：交互开关与区域触发的多波普通敌人遭遇战。

本文记录已核实的项目能力、已批准的玩法规则及最终实现。两种模式均已完成；实际构建、原生回归与 PIE 结果及未验证范围见 [验证记录](../Validation/encounter-spawner-2026-10-25.md)。

## 1. 目标与边界

两种入口共用一套波次、敌人成员登记、服务器结算和运行身份管理。交互型在清怪后等待领奖；区域型清怪后自动完成。单个刷怪点在世界中共享一轮，不为每位玩家复制一套敌人。

沿用现有 Experience、普通敌人 PawnData、EnemyInit、GAS、HealthComponent 和 PlayerState 库存。不新增业务模块，不修改引擎、普通敌人战斗参数、玩家死亡或重生规则，不引入对象池、全局副本框架或新的货币体系。

### 1.1 已确认规则

- 首位触发玩家的 PlayerState 记录本轮身份；Pawn 更换不改变奖励归属。
- 交互型仅该玩家可领取一次配置奖励。首版奖励为单种现有库存物品的定义、数量、等级；示例为 `ID_TestMaterial × 1`。
- 入库失败时保持待领取，不提前标记领取成功，不部分发放多项奖励。
- 区域型完成清怪后自动完成，不设额外领奖步骤。
- 最小交互入口沿用 Hero 输入与 GAS 接线，新增 F 键。当前正式 IMC 中 F 未被占用。
- 开关显示“启动挑战／领取奖励”文字；服务器复核当前 Pawn、轮次、距离和可见性。
- 示例交互距离 250cm；区域 Box 半尺寸 `(300, 300, 150)cm`；初始化等待上限 10 秒，均可配置。
- 交互距离由现有 PawnData 的 `InteractionDistance` 保存；正式 Hero 入口为 `/Game/Main/Data/PawnData/DA_Dafult_PawnData`。Controller 组件与刷怪目标复核读取同一值，原生 Controller 无须额外制作蓝图配置类。
- 复用现有 Guardian 资产，不修改其战斗参数。
- 交互型领奖真实成功后立即恢复未激活；区域的一次性／重复触发可配置，示例采用重复触发与 30 秒冷却。
- 区域每轮结束后必须先观察全员离开，再接受新的配置边沿；示例为离开后重新进入。进入／离开触发边沿可配置，不以冷却结束时仍在区内的 Pawn 自动启动。
- 清怪以 Health 的 DeathStarted 计一次合法击杀，不等待死亡表现完成；其后正常销毁不重复计数。
- 仍存活成员意外销毁、生成失败、初始化或行为树失败／等待超时均终止本轮，清理剩余存活怪物且不发奖。
- 战斗中触发者死亡或断线终止当前轮；普通离开区域继续运行。
- 待领取期间归属者死亡仍保留 PlayerState 资格，重生换 Pawn 后可领取；断线则放弃该轮。
- 本轮失败后交互型恢复未激活；区域型进入冷却与重新进入门禁，不立即重复刷怪。

### 1.2 已批准的首次区域补触发

- 仅首轮尚未接受启动时，服务器在刷怪点自身初始化完成后查询一次当前 Box 重叠。
- 玩家已在区域内，或首次进入时尚未完成 Experience／当前 Avatar 属性初始化，在真实就绪且仍在 Box 内时补齐首次资格；不要求先离开再进入。
- Enter 配置补一次启动；Exit 配置只补合法进入见证，之后仍须真实 EndOverlap 才能启动。
- 通过 PawnExtension GameplayReady、PlayerState 属性就绪及 Experience 低优先级完成事件等待，不使用持续 Tick 或无限 Timer 重试；先登记监听再查询现态。
- 每个候选使用独立身份。离开、死亡或 Pawn 结束生命周期时配对解绑，旧候选回调不能消费重新进入后的资格。
- 首轮真正接受启动或进入失败出口后，永久退休首次补触发资格并清理候选监听；后续仍严格遵守冷却、全员离开与新边沿。
- Experience 的现有加载回调入口不返回解绑句柄，使用弱对象、来源 Manager 与退休 epoch 共同阻止迟到执行；GameState、Pawn Init State、属性及 Actor 生命周期监听均配对撤销。

## 2. 当前代码事实与 Lyra 复用评估

### 2.1 Hodge 已有能力

- `UHodgeInteractionComponentBase` 原先仅有类声明，没有目标查询、输入执行或 RPC，不能直接当作可用交互系统。
- 普通敌人 `AHodgeEnemyCharacter` 持有自身 ASC；`UHodgeEnemyInitComponent` 协调 Experience、PawnData、控制器、属性、能力、装备与 Combat 初始化。
- `AHodgeEnemyAIController` 独立管理行为树启动。EnemyInit Ready 不保证 Brain Running，两项都须核对。
- `UHodgeHealthComponent` 区分 NotDead、DeathStarted、DeathFinished；Actor 销毁不等于合法击杀。
- `UHodgeInventoryManagerComponent::AddItemDefinition` 已实现单次预检及提交，可报告等待、背包满、配置错误等结果。
- Hero 自动重生会更换 Pawn，PlayerState 和其库存、ASC 保留。奖励身份不能仅保存 Pawn 指针。

编辑器只读核查已确认：正式装备 Guardian 的 PawnData 为 `/Game/Main/Character/Enemies/Guardian/DA_Guardian_EquippedPawnData`，AI 为 `AIC_Guardian`，行为树为 `AI/BT_Guardian`。默认 Experience 有 AddInputContextMapping 与 AddComponents Action；具体条目已核实包含双端 Combat、敌人 Equipment、玩家 Inventory／EquipmentSelection，原 Experience 与 Guardian 未重存。

### 2.2 本机 Lyra 参考

参考根目录：`E:/Project/UProject/study/Epic/LyraStarterGame`。以下路径相对该根目录。

- `Source/LyraGame/Interaction/IInteractableTarget.h` 的 `GatherInteractionOptions`：由目标按当前条件提供选项。
- `Interaction/Abilities/LyraGameplayAbility_Interact.cpp` 的 `ActivateAbility`、`UpdateInteractions`、`TriggerInteraction`：分离扫描、提示与执行请求。
- `Interaction/Tasks/AbilityTask_GrantNearbyInteraction.cpp`：服务器邻近查询、能力授予、Timer 清理。其授予缓存按 GA 类，结束任务不撤销缓存授予；不能原样用于持久 ASC 的 Pawn 切换。
- `Interaction/Tasks/AbilityTask_WaitForInteractableTargets_SingleLineTrace.cpp`：视线和范围筛选。缓存选项不能替代执行时的服务器业务校验。
- `Weapons/LyraWeaponSpawner.cpp` 的 `OnOverlapBegin`、`AttemptPickUpWeapon_Implementation`、`ResetCoolDown`、`OnRep_WeaponAvailability`、`EndPlay`：服务器受理、成功后切状态、复制显隐、冷却与 Timer 配对清理。
- `GameModes/LyraBotCreationComponent.cpp` 的 `OnExperienceLoaded`、`SpawnOneBot`：Experience 完成后服务器创建，Controller → PlayerState → RestartPlayer；适用于 AI 玩家，不适用于 Hodge 普通敌人自身 ASC。
- `GameModes/LyraGameMode.cpp` 的 Pawn 创建链：deferred Spawn 后先配置，再 FinishSpawning。

本次未找到上述原生代码中可直接复用的完整波次清怪与领奖系统。`LyraGamePhaseSubsystem` 使用 GameState ASC 的全局层级阶段，同级阶段可能互相取消，不作为各刷怪点的实例状态机。`IPickupable::AddPickupToInventory` 逐项添加、无聚合失败或回滚，且库存的实例 AddEntry 仍有 unimplemented；奖励继续使用本项目已有库存事务。

最小适配保留目标查询与玩家请求划分，通过玩家拥有的组件发出可靠请求，在服务器唯一入口重验。不开关 ASC，不搬入 Lyra IndicatorManager／UIExtension，不用 RestartPlayer 生成普通敌人。

### 2.3 两种模式的选择理由

- **交互型适合明确开始的副本挑战、竞技场和可反复领奖的遭遇。** 玩家主动确认后才隐藏开关，节奏可控；代价是需要输入、视线与距离复核，以及清场后的独立库存提交。待领取死亡保留资格，避免死亡表现或换 Pawn 丢失已经完成的挑战。
- **区域型适合路径遭遇、伏击和关卡区域门禁。** 无需按键或领奖步骤，但必须处理真实重叠、多组件、初始化时机、冷却与新边沿。离开见证与冷却分开记录，可避免站在区内持续刷怪，也避免冷却结束伪造进入事件。
- **公共运行使用一个 Actor 和两份模式配置。** 生成、成员身份、清波、失败、归属及复制集中维护，入口只决定启动和结算方式，避免两套蓝图状态机逐渐出现行为差异；同时继续保留单一 Hodgepodge Runtime 模块。
- **首版采用世界共享的一轮，而非每玩家各自副本。** 同时触发由服务器首个合法请求获胜，首位触发者独占该轮奖励；其他玩家可参与已有 AI 战斗，但不另外发奖。该权衡是已批准范围，不隐含队伍奖励或参与者分账。
- **沿用普通敌人的完整初始化与 AI。** 刷怪点不修改其仇恨、目标、攻击、受击或死亡表现，也不把区域当作 AI 隔离墙；NPC 是否追出区域遵循原 AI。普通离开继续战斗，符合本轮规则。
- **采用事件和 Timer 管理，不做全世界逐帧查怪。** 同步批量出生和存活成员回查仍有成本；DeathStarted 清波时上一批的死亡动画可能与下一批重叠，性能预算需同时考虑存活与死亡表现，不只看 AliveCount。大批量场景应先采样再决定是否需要额外生成预算。

## 3. 主要职责

### 3.1 交互底座

- 目标接口：提供当前文本及运行身份，执行已授权的交互业务。
- `UHodgeInteractionComponentBase`：由 PlayerController 持有；按键时局部物理查询最近候选，检查距离和 Visibility 视线，不逐帧遍历世界 Actor。
- `UHodgeGameplayAbility_Interact`：接收现有 GAS 输入，仅发起交互并结束，不持有刷怪或奖励状态。
- 服务器请求同时核对 PlayerController 当前 Pawn、PlayerState、ASC Owner／Avatar、Health 存活、目标、距离、视线及选项轮次／修订。
- 刷怪点在最终执行入口再次校验本轮状态和奖励资格。隐藏表现或客户端缓存不能授予权限。

### 3.2 公共刷怪运行职责

- 配置保存波次、普通敌人 PawnData、等级、相对刷怪点的出生变换、波次等待与初始化超时。
- 实例持有服务器状态、轮次身份、当前波号、正在生成标记、待就绪成员及存活成员。
- 只推进当前轮次的已登记成员；重复死亡、迟到回调、旧轮次 Timer 不得扣减新轮次或结算奖励。
- 触发适配只决定何时提出启动请求；波次推进与结算不复制两套实现。
- 复制运行快照供客户端显隐与文字展示。客户端不自行生成怪物、不计清怪、不发奖励。

## 4. 状态与流程

公共状态使用以下含义；状态事实由服务器实例持有，客户端快照不授予启动或领奖权限。

1. **未激活**：接受合法启动请求。交互开关可见；区域保留触发检测。
2. **激活**：服务器已接受一位触发者，建立新轮次并检查依赖；再次启动请求拒绝。
3. **刷怪中**：按配置生成当前批，等待该批真实清空，再进入下一批。开关隐藏且不可交互。
4. **待领取**：仅交互型进入；开关显示领取文字，只有本轮归属者可以领取。库存失败继续停留。
5. **已完成**：奖励已成功提交，或区域型已完成。交互型成功领奖后立即恢复未激活；区域一次性配置保持完成，重复配置同时满足冷却结束和全员离开后才恢复可触发。失败的区域轮次仍遵守冷却与离开门禁。

区域碰撞范围与开关表现须分离。不能通过隐藏整个管理 Actor 或禁用区域碰撞来丢弃离开事件；否则无法可靠判断重复触发门禁。多组件重叠要按 Pawn 身份去重，非玩家 Actor 和不满足条件的 Pawn 不触发。

### 4.1 批次生成的关键顺序

1. 检查本轮和波次身份，设置正在生成闸门，登记待生成数量。
2. deferred Spawn 创建普通敌人。
3. FinishSpawning 前登记成员，绑定 Health 死亡、Actor EndPlay、EnemyInit 就绪变化；写入 EnemyPawnData 与 InitialLevel。
4. FinishSpawning 可能同步触发自动 AI 占有、初始化、能力或销毁；返回后立即复核身份及实际现态。
5. 完成整批生成并关闭闸门后，才允许评估清波。生成中即使存活数短暂为零，也不得提前开下一批。
6. 初始化等待、配置失败、行为树启动失败分别诊断；超时不能被视为已击杀。

### 4.2 奖励提交

交互请求带客户端观察到的轮次与修订。服务器核对归属、待领取状态及请求身份；以重入保护调用库存 AddItemDefinition，真实成功后才推进完成状态。重复、旧轮次或不符归属的请求不再次添加物品。运行期间不统一清空 GameplayEffect、能量或库存状态。

### 4.3 清波、失败与归属生命周期

- 仅当前 RunId、当前波次已登记成员的首次 DeathStarted 可以减少存活数。重复回调、已清理成员、旧轮次或死亡后的 EndPlay 均不再次清波。
- 出生与初始化仍在进行时，存活数为零不触发下一波；先完成本批登记并关闭生成闸门，再判断 PendingCount 和 AliveCount 是否都为零。
- 活成员意外 EndPlay、生成失败、EnemyInit 失败或超时、Brain 启动失败：先退休当前运行身份，再解绑、清 Timer 与销毁剩余活成员；不得用清理造成的死亡／销毁回调发奖。
- Active／Spawning 期间触发者 DeathStarted 或断线采用同一中止出口；普通移动离开区域不改变归属、不取消战斗。
- AwaitingReward 时仅保存 PlayerState 领奖资格，死亡不取消；真正执行交互时仍核对当前有效且存活的 Pawn／Avatar。断线放弃资格并通过该模式的失败出口恢复。

### 4.4 区域重复与边沿门禁

- 只有玩家 Pawn 的真实重叠事件可以提供边沿；多个身体组件的重复事件不得重复启动同一轮。
- 冷却使用服务器世界时间，截止值复制在 CooldownEndServerTime。冷却结束本身不制造进入／离开事件。
- 每轮结束后记录 bNeedsExit；结束时仍在区内的全部玩家必须先离开，门禁解除后才允许新的配置边沿启动下一轮。
- Enter 配置需要解除门禁后的新进入；Exit 配置同样先完成结束后的离开门禁，再由后续进入与离开形成新 Exit。用于解除旧轮次门禁的离开不能同时启动新轮次。
- bRepeatable=false 的正常完成不重新启动；失败仍按已批准的区域失败冷却与重新进入出口处理。首次已在区域内与依赖就绪后的补触发按第 1.2 节执行；它不能绕过后续轮次的重新进入门禁。

### 4.5 配置组织与静态校验

- `UHodgeEncounterDefinition` 保存 Mode、Waves 和 EnemyInitializationTimeout；每个 Wave 包含 DelayBeforeSpawn 与 Enemies，每个 Spawn 包含 PawnData、Level 与 RelativeTransform。
- 区域字段为 AreaHalfExtent、TriggerEdge、bRepeatable、CooldownSeconds，仅区域模式参与校验与执行；交互距离统一读取请求者 PawnData 的 InteractionDistance，默认 250cm，不在刷怪资产中维护第二份值。
- 交互奖励字段为 RewardDefinition、RewardCount、RewardLevel；区域模式不领奖，也不要求休眠奖励字段有效。
- Validate 与编辑器 IsDataValid 共用校验：至少一波且每波非空，延时与冷却为有限非负数，初始化超时为有限正数，区域半尺寸各轴为有限正数，出生变换各项有限。
- PawnData 的 PawnClass 必须派生自非抽象 AHodgeEnemyCharacter；等级与属性合法性复用其 StatProfile 的 Validate／Evaluate，不增加刷怪专用等级或波次数量上限。
- 奖励复用现有物品定义 Validate；数量与等级必须合法。静态校验通过不代表 Experience、EnemyInit、ASC、控制器或 Brain 已就绪，运行时仍须逐一核对。

### 4.6 制作入口

1. 打开 `/Game/Main/Maps/L_EncounterValidation`；`EncounterSwitch` 和 `EncounterArea` 分别引用 `/Game/Main/Data/Encounters/DA_Encounter_Interaction`、`DA_Encounter_Area`。
2. 在 Definition 的 Waves 数组添加批次；Enemies 中每个条目生成一只敌人，分别填写 PawnData、Level 和 RelativeTransform。示例两批为一只、两只，使用正式装备 Guardian，等级 1。
3. RelativeTransform 相对刷怪 Actor，包括 Actor 自身的平移、旋转和缩放；使用分离的出生点，避开墙体和其他胶囊。DelayBeforeSpawn 是每批生成前的等待，第一批同样适用；零等待在下一 Tick 开始。
4. 交互型配置单种奖励定义、数量、等级。示例为 ID_TestMaterial ×1；角色交互距离在正式 Hero 的 DA_Dafult_PawnData 中调整，输入沿用 IA_Interact → InputTag.Ability.Interact → 原生交互 GA。
5. 区域型填写 Box 半尺寸、Enter／Exit 边沿、是否重复和冷却。示例半尺寸为 300／300／150cm，完整尺寸为 600／600／300cm，重复冷却 30 秒。区域不设置奖励。
6. 将 `AHodgeEncounterSpawner` 放入其他关卡时只需指定 Definition；所在 Experience 仍须提供普通敌人所需 Combat、Equipment 等既有注入。开关样式可调整原有组件；替换提示 Widget 时应继承 UHodgeEncounterPromptWidget，保留文字更新契约。


## 5. 验收清单

### 5.1 原生与配置

- 合法与非法波次配置、敌人类、等级、出生位置、范围、时间参数。
- FinishSpawning 中同步死亡／销毁、首个和中途生成失败、Waiting 超时、Ready 但 Brain Failed。
- 最后一怪重复回调、旧轮次回调、主动清理不计击杀、多人同时启动只建立一轮。
- 过期交互、旧 Avatar、错误 PlayerState、超距、遮挡、死亡玩家均被拒绝。
- 背包满不丢奖励，成功后重复领取不增加库存；归属者换 Pawn 后不误用旧 Avatar。

### 5.2 真实编辑器与联机

- 独立验证场景内放置两类刷怪点，至少两批 Guardian，验证显隐、文本、真实死亡和波次递进。
- 单人、Listen、PIE Dedicated；拥有者和旁观客户端的状态、波号、存活数与开关表现一致。
- 至少覆盖零延迟与 100ms 模拟延迟、并发触发／领取、玩家死亡重生和断线。
- 区域配置、重复门禁、冷却、退出边沿及失败出口以最终批准的规则验收。
- Editor 与 Game 常规构建；不以编译替代蓝图、输入、PIE 或网络通过。

## 6. 已识别风险与待办

- 首次补触发必须验证候选仍处于当前 Box、同一有效 Pawn／PlayerState／ASC Avatar 且真实属性就绪；接受首轮后旧就绪事件不得重新刷怪。
- 初始实现不做大规模敌人性能承诺；批量生成、骨骼更新、感知及 AI 决策开销须在实际数量下采样。
- 关卡卸载、世界退出与异常 Destroy 不能伪装击杀。清理应先退休旧轮次、解绑和清 Timer，再销毁存活成员。
- 玩家奖励归属使用 PlayerState；RPC 和瞬时动作仍核对当前 Pawn，二者职责不同。
- 通用交互、刷怪管理及库存均需防止委托回调重入和重复结算。
- 区域按三维 Box 重叠判断边沿，跳跃或传送进入／离开也遵循同一规则；从区域外直接传送到另一侧区域外不制造虚拟经过事件。
- 普通离开不会取消战斗，首版没有最长战斗时限或离开自动放弃。需要取消时由服务器调用 ResetEncounter；一次性正常完成不会被该取消入口重置。
- 奖励幂等和刷怪状态属于当前服务器世界，不提供跨存档领奖账本或关卡重载后进度持久化。

本次已依次完成：已确认规则的数据与公共运行 → 交互、波次、归属及失败原生回归 → 首次区域补触发与候选退休回归 → 独立示例配置与资产校验 → Editor／Game 构建及原生测试 → 单人和联机端到端验证 → 交付记录。剩余工作是上述明确未验证的规模性能、独立进程与持久化等后续议题，不阻塞当前已批准首版。

实施与本次范围内验证由本次开发任务完成，玩法规则均已获用户确认。后续玩法扩展由用户确定范围，团队评审人员尚未指定，不虚设姓名或职责归属。
