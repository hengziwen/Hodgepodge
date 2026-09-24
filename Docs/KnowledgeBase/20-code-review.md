# 代码评审顺序与检查清单

> 本文是**评审规范**，不是对当前代码的验证结论。文中举的例子是"要在哪里看"，不代表这些位置已经正确或已经运行通过。
[返回首页](README.md)

## 为什么固定这个顺序

代码评审的顺序决定了先发现哪类问题。UE 项目里，最贵的 bug 是**多人不同步、生命周期错位、GC 悬空**，它们上线后极难复现；命名和 include 排序几乎不影响正确性。因此：

- **先看代码会不会在多人、生命周期和 GC 上炸，Style 永远最后看。**
- 前面几层没通过时，不要用后面的层"补分"：注释齐全、命名漂亮的代码同样可以每帧泄漏 Ability 或让客户端改权威状态。
- 每层只回答一个问题，判断不了就明确写"未确认"，不要默认通过。

## 固定顺序

| 层级 | Review 什么 | UE 项目最常见的问题 |
| --- | --- | --- |
| 1. 需求正确性 | 代码到底解决了什么问题 | 做出了不需要的系统、需求理解错 |
| 2. 架构职责 | 逻辑放的位置对不对 | Character 越写越大、GA/Component/GameMode 职责混乱 |
| 3. UE 生命周期 | 初始化/销毁时机对不对 | Constructor/BeginPlay/PostInit/OnRep/EndPlay 用错 |
| 4. UObject/GC | UObject 引用是否合法 | 裸 `UObject*`、Delegate 没解绑、异步回调悬空 |
| 5. 网络 | Authority/Owner/Avatar/Replication/RPC | 客户端修改权威状态、重复 RPC、预测与服务器分歧 |
| 6. GAS | Ability 生命周期与预测 | Ability 不 End、Task 残留、PredictionKey 用错、Tag 泄漏 |
| 7. 数据与资产 | DataAsset/SoftRef/加载策略 | 输入时异步加载、硬引用链爆炸、资产没预加载 |
| 8. 性能 | Tick/遍历/加载/分配 | Tick 里 `GetAllActorsOfClass`、频繁读表/Load |
| 9. 可维护性 | API/命名/依赖/扩展性 | magic number、状态重复、接口边界不清 |
| 10. Style | Epic Coding Standard | 命名、const、include、类布局 |

1～6 层任一发现**阻塞级**问题，先提出并停止深入第 7 层之后的细节；除非用户明确要求只做样式检查。

## 逐层检查点

### 1. 需求正确性

- 这段代码对应哪个已确认的需求？是否只是"看起来该有"的系统？
- 是否与现有机制重复（例如又造了一套输入/属性/初始化入口，而 Experience、PawnData、Init State 已有路径）？
- 改动范围是否超出需求：顺手重构、顺手改拼写、顺手动第三方资产？
- 需求边界是否被偷偷扩大（"先做个小功能"实际改了复制和 GAS 行为）？
- 无法确认时按 [接通清单](12-integration-backlog.md) 的"现状证据 / 未接通"口径描述，而不是假设需求已明确。

### 2. 架构职责

- 逻辑是否放在该放的对象上？参考 [架构职责与对象所有权](02-architecture.md)。
- `Character` 是否在膨胀：移动、相机、输入、GAS 初始化、UI 是否都能塞进 Character？
- GameMode / GameState / PlayerController / PlayerState / Pawn / Component 的职责是否与既有的 Lyra 式组件协作一致？
- 是否绕过既有扩展点（GameFeature、Experience、ActionSet、Init State）另起一套。
- 新增公开头文件是否让模块依赖变重；Runtime 模块是否引入了 Editor-only 依赖。
- 是否符合 AGENTS.md：单一 `Hodgepodge` Runtime 模块、`Public`/`Private` 对应、`Hodge` 命名，不新增模块来"整理目录"。

### 3. UE 生命周期

- 构造、`BeginPlay`、`PostInitializeComponents`、`OnRep_*`、`EndPlay`、`Unpossess`、`Destroyed` 的时机是否用对？
- 初始化动作是否**幂等**，能否承受重复调用（组件重挂载、Pawn 重建、Experience 重载）？
- 数据依赖是否在依赖就绪后才读取，而不是在构造里读还没加载的资产/组件？
- 状态推进是否可能卡死或越过前置条件；两个 Init State 是否互相等待。
- 清理是否与创建对称：`EndPlay`/`Unpossess` 是否解绑了所有绑定与委托。
- 容器/句柄/弱引用是否在销毁路径上被复位，避免下次初始化读到脏状态。
- 详细核对位置见 [PawnExtension 与 Hero 初始化](04-pawn-initialization.md)、[从启动到生成玩家的调用链](03-runtime-startup.md)。

### 4. UObject / GC

- 成员是否使用 `TObjectPtr<T>` / `TWeakObjectPtr<T>` / `TSoftObjectPtr<T>`，而非裸 `UObject*`？
- 非 UObject 持有的 UObject 引用是否加 `UPROPERTY()`（否则被 GC 回收）？
- 委托绑定是否有对应的解绑；异步回调（Timer、Latent、AsyncLoad、HTTP、流式加载）是否用弱引用/句柄防止回调悬空？
- Lambda 捕获 `this` 是否保证同步执行或已做失效判断？
- 长生命周期对象（PlayerState、GameState、Subsystem、AssetManager、全局 AbilitySystem）是否持有短命 Pawn/Component 的强引用。
- 互相持有/静态容器是否构成隐性泄漏；GC 持有关系见 AGENTS.md 约定。

### 5. 网络

- 谁有权修改状态：是否只在 `HasAuthority()` 下改动，客户端只发请求？
- `OwnerActor` / `AvatarActor` 是否正确（玩家 ASC 归 PlayerState，Avatar 是当前 Pawn）；换 Pawn 后是否重绑定？
- 属性复制、`OnRep` 是否补了表现；`ReplicatedUsing` 的回调是否处理初始值。
- RPC 的 `Server`/`Client`/`NetMulticast` 标记与时机是否正确；`Reliable` 的使用是否必要，是否可能重复发送。
- 客户端预测与服务器结果是否一致；预测失败/回滚是否被处理。
- 是否依赖 `LocalPlayer` / `PlayerController` / 输入，从而在 Simulated Proxy 或 Dedicated Server 上失效。
- 参考 [网络、复制与 Dedicated Server](11-network.md)；不要用 PIE 单机结果宣称联机通过。

### 6. GAS

- Ability 是否一定走到 `EndAbility`；被取消、被打断、Actor 销毁的路径是否也结束。
- `GameplayTask` / `AbilityTask` 是否在结束时正确 `EndTask`，有没有残留回调。
- `PredictionKey` 使用是否正确；预测窗口外的状态改动是否只在服务器执行。
- 授予的 Ability/Effect/Set/Attribute 是否记录句柄，卸载或来源消失时是否按来源撤销。
- Tag 是否泄漏：独占组、激活组、loose tag、被阻断后的标签是否正确清除。
- Cost/Cooldown 失败分支是否有明确结果，而不是静默无效。
- 自定义 `EffectContext` / `AbilitySystemGlobals` 的网络序列化字段是否完整。
- 参考 [GAS、AbilitySet 与技能生命周期](07-gas.md)、[属性、伤害、战斗与死亡](08-combat-health.md)。

### 7. 数据与资产

- 配置放在 DataAsset（PawnData / InputConfig / AbilitySet 等），还是硬编码在 C++？优先数据驱动。
- 引用策略：能用软引用/`PrimaryAssetId` 的地方是否改成了硬引用，导致加载链膨胀。
- 资产是否在**需要之前**预加载；是否在输入或 Tick 里触发同步加载。
- 资产路径、GameplayTag 是否真实存在，是否和蓝图引用一致；改路径是否处理 CoreRedirects。
- 打包时是否可达（SoftObjectPath 不能编译期校验；Cook 是否包含该资产）。
- 参考 [数据资产与 AssetManager](05-data-assets.md)；资产内部字段需在编辑器确认，磁盘存在不等于配置正确。

### 8. 性能

- Tick 是否存在，能否降到事件驱动或设置 `PrimaryActorTick` 开关。
- 是否在 Tick/高频回调里做 `GetAllActorsOfClass`、遍历全部 Actor/Component、频繁查表或 `LoadObject`。
- 是否每帧分配（`FString` 拼接、临时数组、`TArray` 反复扩容、Lambda 拷贝大对象）。
- 大量生成/销毁的对象是否考虑对象池；复制频率和属性数量是否可控。
- 加载是同步还是异步；是否在游戏线程做重活。

### 9. 可维护性

- API 边界是否清晰：公开接口是否最小、命名是否表达意图、参数是否过多。
- 是否有 magic number / 魔法字符串，是否应提为常量或配置。
- 状态是否重复存在于多个地方，是否会产生不一致。
- 重复代码是否值得抽公共函数；抽象是否过度（只有一处调用就别抽）。
- 日志是否可定位（包含类名/上下文），是否误用 `Log`/`Warning`/`Error`。
- 新增/修改的 GameplayTag、类名、资产路径是否同步更新相关文档。

### 10. Style（Epic Coding Standard）

- 命名：类型前缀、布尔 `b` 前缀、驼峰、变量/函数一致。
- `const` 正确性；按值/按引用传参是否合适。
- Include 规则：`.generated.h` 放最后、优先前向声明、按需 include，不引入未使用头文件。
- 类布局：`UCLASS`/`UPROPERTY` 宏与访问修饰符顺序符合 Epic 规范与相邻代码风格。
- 反射实现沿用 `UE_INLINE_GENERATED_CPP_BY_NAME`；不随意修改已有拼写（如 `Dafult`、`PlayState`）。
- 中文注释以“一行说明一件事”为原则：函数体内的 `//` 内联注释块是否超过 3 行（4 行及以上判不通过）；文件头/类头 `/** */` Doxygen 注释不受此限；是否逐行翻译代码或为排版凑行数。
- 不重构第三方（ALS、RiderLink）与二进制资产。

## 阻塞级判定参考

命中任一条，按阻塞处理并在评审结论中优先列出：

- 客户端可直接修改权威状态，或缺少权限校验的 RPC。
- Ability/Effect/Task 无结束路径，或换 Pawn/重生后重复授予、标签泄漏。
- UObject 引用可能被 GC 回收，或委托/异步回调在对象销毁后仍会触发。
- 初始化非幂等，或生命周期时机错误导致依赖未就绪就读写。
- 在 Tick/输入路径同步加载资产或全量遍历。

## 评审输出格式（建议）

每条结论包含：**层级 / 严重度 / 位置（文件:符号） / 问题 / 依据 / 建议**。

- 严重度：阻塞 / 高 / 中 / 低 / 待确认。
- 依据必须能指到代码或硬性规则，不写"感觉不太好"。
- 不确定的写"待确认 + 需要什么信息"，不猜。
- 不要在没有实际编译/PIE/联机时把结论写成"已验证通过"；验证口径见 [验收场景与调试观察点](16-validation.md) 与 [AI 开发与验证流程](../AI_DEVELOPMENT.md)。

## 与现有流程的衔接

- 评审前先看 `git status`，保留用户已有改动，不回退、不覆盖、不自动提交。
- 需要时用 `python Docs/KnowledgeBase/tools/kb.py search "<符号>"` 定位源码与解释；工具边界见 [维护工具说明](tools/README.md)。
- 代码改动后按 [术语、决策记录与维护规范](17-maintenance-glossary.md) 更新受影响章节并刷新索引。
- 新增结论写入对应人工章节，不要只刷新快照就宣布知识库已更新。

---

# 附录：2026-09-17 本轮评审结论 — 普攻攻击链（GA_Attack / IA_Attack / DA_Pover / Attack Montage）

> **范围声明**：本附录是**针对一次具体改动的评审结论**，正文第 1–150 行的"规范"定位不变。
>
> - **评审对象**（当前工作区未提交改动）：
>   - 新增：`/Game/Main/Character/Hero/GA_Attack`、`/Game/Main/Data/DA_Pover`、`/Game/Main/Input/InputAction/IA_Attack`、`/Game/CodexText/Montage/AM_Attack01~05_Montage`、`Config/DefaultGameplayTags.ini`
>   - 修改：`/Game/Main/Character/Hero/BP_Hero_Pover`、`/Game/Main/Data/DA_Dafult_PawnData`、`/Game/Main/Input/DA_HodgeInputConfig`、`/Game/Main/Input/IMC_Default`、`/Game/CodexText/AnimInstance/ALS/ABP_ALS_Grounded`
> - **评审依据**：磁盘资产 + 本机编辑器（`127.0.0.1:55557` 蓝图桥，**只读**读取蓝图图 / 节点引脚默认值 / CDO）+ 现有 C++ 与配置。
> - **未执行**：C++ 构建、蓝图编译、PIE、联机、Cook/打包。所有"运行表现"结论均为**静态推导**，需按下文"建议验证方法"实测。
> - `.uasset` 在仓库中以 Git LFS 指针保存，本机未取到 LFS 对象，**无法对比改动前的资产内容**；下文中"这是本次新引入"的判断只对新增资产成立，对"修改过的二进制资产"只描述**当前状态**。

## A0. 已核实的事实（先建立事实基础，再谈问题）

| # | 事实 | 证据位置 |
| --- | --- | --- |
| F1 | `GA_Attack` 父类是 `UHodgeGameplayAbility`（CDO 上可见 `ActivationPolicy` / `ActivationGroup` / `FailureTagToUserFacingMessages` / `AdditionalCosts` 等 Hodge 专有属性） | 编辑器 CDO 读取；资产字符串含 `Hodgepodge.HodgeGameplayAbility` |
| F2 | `AbilityTags = Ability.Attack`；`ActivationPolicy = OnInputTriggered`；`ActivationGroup = Independent`；`InstancingPolicy = InstancedPerActor`；`NetExecutionPolicy = LocalPredicted`；`ReplicationPolicy = ReplicateNo`；`NetSecurityPolicy = ClientOrServer`；`Cost/Cooldown = None`；`CancelAbilitiesWithTag` / `BlockAbilitiesWithTag` / `ActivationOwnedTags` / `ActivationBlockedTags` **全为空** | 编辑器 CDO 读取 |
| F3 | 蓝图变量：`attackcount`(int，默认 0)、`AttackMontage`(`TMap<int32, AnimMontage>`，键 1→AM_Attack01、2→02、3→03、4→04，**硬引用**)、`NewVar`(`FTimerHandle`) | 编辑器 CDO + 蓝图图 |
| F4 | `ActivateAbility` 流程：`ClearAndInvalidateTimerHandle(NewVar)` → `attackcount = attackcount + 1` → `PlayMontageAndWait(Montage = AttackMontage[attackcount], Rate=1, StartSection=None, bStopWhenAbilityEnds=true, bAllowInterruptAfterBlendOut=false)` → `then` → `等待Gameplay事件` | 节点 `8C0AE685…` → `4E70A593…` → `85EDE0F7…` → `EAEF3DD0…` → `0D3CB3C6…` → `FAA89B9D…` |
| F5 | `OnCompleted` / `OnBlendOut` / `OnInterrupted` / `OnCancelled` **四个引脚全部接到同一个 `EndAbility`**（`61A7160F…`），没有区分取消原因，也没有 `bWasCancelled` 分支 | `0D3CB3C6…` 的四个输出引脚 |
| F6 | `等待Gameplay事件`（`FAA89B9D…`）：`EventTag` **为空**、`OnlyMatchExact=true`、`OnlyTriggerOnce=false`，且 `then` / `EventReceived` / `Payload` **全部未连线** → 死节点 | 节点引脚默认值 |
| F7 | `OnEndAbility` 流程：`SetTimerByEvent(Delegate = 自定义事件, Time = 1.2, bLooping = false)` → `Set NewVar` → `Branch(attackcount >= 4)` → `then` → 直接调用同一个 `自定义事件`；`自定义事件` 体 = `Set attackcount = 0` | `1985805745…` → `3F555457…` → `E928F645…` → `8020560A…`(B="4") → `111C1072…`；`8A83DEF1…` → `3DF22423…`(默认 0) |
| F8 | 语义结论：`attackcount` 是 **1 基段数**；连击重置有两条路径 —— ① 1.2 s 计时器到点；② 段数到达 4 时在本次 `EndAbility` 内**立即**重置 | F4 + F7 |
| F9 | 输入链**完整**：`Exp_HodgeDefaultExperience` 的 `GameFeatureAction_AddInputContextMapping` 挂载 `IMC_Default`（Priority=0，RegisterWithSettings=true）→ `IMC_Default` 有 `IA_Attack ← LeftMouseButton` → `DA_HodgeInputConfig.AbilityInputActions` 有 `IA_Attack ↔ InputTag.Ability.Melee` → `DA_Pover`（`HodgeAbilitySet`）以 `InputTag.Ability.Melee` 授予 `GA_Attack` → `HasTagExact` 精确匹配 | 编辑器读取；`HodgeAbilitySystemComponent.cpp:282-301`、`HodgeAbilitySet.cpp:146` |
| F10 | 资产链可达：`DefaultGame.ini:10` → `DA_Dafult_PawnData` → `AbilitySets = [DA_Pover]` → `GA_Attack` | 编辑器读取 + `Config/DefaultGame.ini` |
| F11 | 5 条 Montage 均使用槽位 `DefaultSlot`，`ABP_ALS_Grounded` 的 AnimGraph 中存在 `AnimGraphNode_Slot`（`SlotName = DefaultSlot`）→ **槽位匹配** | 资产字符串 + 编辑器 |
| F12 | 5 条 Montage **没有任何 Notify**（`AnimNotifyList` 为空，只有一条默认 `AnimNotifyTrack`）；Montage 时长分别约 3.17 / 3.43 / 3.40 / 4.67 / 4.07 s | 资产字符串 + `get_play_length()` |
| F13 | `BP_Hero_Pover` / `BP_HeroBase` 的 `HodgeHeroComponent.DefaultInputMappings` **为空**；IMC 由 Experience 侧 `GameFeatureAction_AddInputContextMapping` 在 `NAME_BindInputsNow` 之后追加，**不会被 `InitializePlayerInput` 开头的 `ClearAllMappings()` 清掉** | 编辑器 CDO + `HodgeHeroComponent.cpp:424-425`、`600-606` |
| F14 | `DA_Pover` 的资产类型是 **`HodgeAbilitySet`**，不是 PawnData；`AM_Attack05_Montage` **无任何引用者** | 编辑器读取 + AssetRegistry referencers |

## A1. 分层逐项核对

| 层级 | 结论 |
| --- | --- |
| 1 需求正确性 | **有问题**：动画表现成立，但"连击"语义与"命中"闭环都不成立，见 R1 / R2 |
| 2 架构职责 | **有问题**：生产资产反向依赖实验目录，加载策略绕过既有软引用/Bundle 约定，见 R3；攻击无状态标签与互斥配置，见 R4 |
| 3 UE 生命周期 / UObject / GC | 未发现阻塞级问题：Timer 用动态委托绑定 UObject，实例销毁后 `FTimerManager` 会失效清理；AbilityTask 随 Ability 销毁解绑。仅 R7 一条低影响项 |
| 4 Server / Owning Client / Simulated Proxy | 结构正确（`LocalPredicted` + 复制 Montage，模拟代理不跑 Task、不跑蓝图）。仅 R8 一条潜在预测分歧 |
| 5 GAS / AbilityTask / GameplayTag | 有问题：Tag 注册方式偏离项目约定（R5）；`WaitGameplayEvent` 死节点（R6） |
| 6 Replication / RPC | 无自定义 RPC，方向正确；`bReplicateInputDirectly=false` 与 `HodgeAbilitySystemComponent` 注释一致 |
| 7 DataAsset / SoftReference / AssetManager | 有问题：见 R3；另外 `HodgeAbilitySet` 没有对应的 `PrimaryAssetTypesToScan` 类型 |
| 8 Tick / 查询 / 分配 / 网络性能 | 未发现影响项：无 Tick；每帧仅 `ProcessAbilityInput` 的既有遍历；无每帧分配 |
| 9 可维护性 | 有问题：见 R9 / R10 |
| 10 Style | 见 R11 |

## A2. 问题清单

### R1 — MAJOR（层级 1 需求正确性）

**位置**：`/Game/Main/Character/Hero/GA_Attack` 事件图（`0D3CB3C6…` PlayMontageAndWait、无输入监听节点）；对照 `Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp:384-389`。

**为什么有问题**：Ability 处于激活状态期间，后续攻击按键在 `ProcessAbilityInput` 里只会走 `AbilitySpecInputPressed()`（发一个 `InputPressed` 复制事件），**不会重新激活、也不会推进段数**；而 `GA_Attack` 既没有 `WaitInputPress`，也没有任何 `InputPressed` 监听，这些按键被直接丢弃。因此 `attackcount` 只能在"上一段 Montage 播完 → `EndAbility` → 玩家再按一次"这一条路径上推进：它是一套"顺次播放"，不是"窗口内接段"。再叠加 F7 的 1.2 s 重置窗口，玩家真正的可接续时间只有"整段动画播完后的 1.2 秒"。

**什么场景会触发**：PIE 里连续点击左键。预期"连点出第 2、3、4 段"，实际第 1 段播完前所有点击都被吞；`AM_Attack01` 时长 3.17 s，即 3 秒内的连点全部无效。若"攻击中被打断后允许在窗口内接段"是需求，则当前实现完全不满足。

**推荐修改方式**（二选一，先确认需求）：
1. 若目标是"连击接段"：按 [AbilityTimeline 设计方案](../Design/ability-timeline.md) 6.2 / 8.5 增加 `ComboBufferWindow` 阶段 + `WaitInputPress` 做输入缓冲，并在 `ComboTransition` 时刻决定是否 `StopTimeline` + 起下一段；**两端都要创建 `WaitInputPress`**。
2. 若目标就是"顺次播放"：把 4 段合并进一条 Montage 的 4 个 Section，用 Notify/事件驱动 `Montage_JumpToSection`，避免"必须等整段播完才响应"的手感，并把这个取舍写进文档。

> 注：本条是否算缺陷取决于需求口径。若需求只写"能播普攻动画"，请将本条降级并在交付说明中写明"连击暂不支持窗口内接段"。

### R2 — MAJOR（层级 1 需求正确性）

**位置**：`GA_Attack` 节点 `FAA89B9D…`（等待Gameplay事件，`EventTag` 为空、所有输出未连线）；`/Game/CodexText/Montage/AM_Attack01~05_Montage`（无 Notify，F12）；`Source/Hodgepodge` 中无任何 `GameplayEvent.MeleeHit` 派发点。

**为什么有问题**：本次改动只产出"播放动画"，没有任何命中判定、伤害、Cue 或事件输出。`WaitGameplayEvent` 是一个空标签的死节点，说明这条链**原本打算接、但没接完**。对照 [接通清单](12-integration-backlog.md) KB-08（"剩余工作是具体攻击 Ability → 技能命中 → GE → 耗尽 → 角色死亡/重生"），本次交付没有缩小这个缺口。

**什么场景会触发**：PIE 中角色播放攻击动画撞向敌人 —— 无伤害、无受击、无音效/特效、无相机反馈。

**推荐修改方式**：
- 若本次交付边界**就是**动画表现：把这条写进交付说明与 [接通清单](12-integration-backlog.md)，并**删除死节点**（见 R6）。
- 若要闭环：在 Montage 的正确帧放 `AN`/`ANS` 派发 `GameplayEvent.MeleeHit`（或按设计方案用 Timeline 的 `HitCheck`），由**服务器**（`AuthorityOnly`）生成判定、应用伤害 GE，并补 `GameplayCue`。

### R3 — MAJOR（层级 2 架构职责 + 层级 7 数据与资产）

**位置**：`Content/Main/Character/Hero/GA_Attack.uasset` 的 `AttackMontage` 变量（`TMap<int32, AnimMontage>`，硬引用 `/Game/CodexText/Montage/AM_Attack01~04_Montage`）；`Content/Main/Character/Hero/BP_Hero_Pover.uasset` 的 `SkeletalMesh.AnimClass = /Game/CodexText/AnimInstance/ALS/ABP_ALS_Grounded`。

**为什么有问题**：
1. **生产资产直接依赖"实验目录"**。`Docs/KnowledgeBase/01-project-map.md` / `2026-09-17 更新记录`记录 `CodexText` 是独立实验模块（"主体系无 C++ 引用"）。现在 `Main` 的普攻链把 4 条攻击动画（且 Hero 的 AnimClass）压在了 `CodexText` 上，任何人按"实验内容可整体丢弃"的既有约定清理该目录，都会直接打断普攻。
2. **加载策略绕过既有约定**。`TMap<int32, AnimMontage>` 是硬引用：`PawnData → AbilitySet → GA_Attack 类 → 4 条攻击动画`全部绑死，玩家 `PawnData` 一加载就把攻击动画全部拉进内存/Cook 依赖链。项目既有做法（见 [数据资产与 AssetManager](05-data-assets.md)）是"配置用硬引用、大体积动画用软引用 + 主资产 `UpdateAssetBundleData()` 收集 Bundle"。
3. 用 `int32` 下标当段标识（`AttackMontage[attackcount]`）无法表达"跳段 / 技能接段 / 闪避派生"，设计方案第 9.1 节已明确要用 `AttackID`（GameplayTag）代替下标。

**什么场景会触发**：清理或移动 `CodexText`；把 `CodexText` 加入 pack 排除；段数/跳段需求出现后需要改数据结构。

**推荐修改方式**：
- 把 4 条攻击 Montage 移到 `/Game/Main/Character/Hero/Anim/Montage/`（用 Asset Move 保留重定向），或**先决策**把 `CodexText` 转正并同步更新知识库对它的定位。
- 短期：`AttackMontage` 至少改为按语义命名并用 `AttackID`（或 DataAsset 节点表）索引；中期：把段配置下沉到 `UHodgeComboSet` 风格的 DataAsset + 软引用 Montage + Bundle 预加载（并在 `DefaultGame.ini` 补对应 `PrimaryAssetTypesToScan`，当前 `HodgeAbilitySet` 也不在扫描列表中）。

### R4 — MAJOR（层级 2 架构职责 + 层级 5 GAS / Tag）

**位置**：`GA_Attack` CDO 的 `AbilityTags = Ability.Attack`，而 `ActivationOwnedTags` / `CancelAbilitiesWithTag` / `BlockAbilitiesWithTag` **全为空**；`DA_Dafult_PawnData.TagRelationshipMapping = None`。

**为什么有问题**：项目为攻击定义的核心契约是"攻击期间角色身上要有可查询的状态标签"（设计文档 12.2 / 6.4 铁律 3：`Status.Attack.*`），动画层与互斥判定都靠它工作。当前实现只有一条 `AbilityTags`（仅用于激活关系），**运行期角色身上没有任何"正在攻击"的可见状态**；同时没有 `CancelAbilitiesWithTag` / `BlockAbilitiesWithTag`，攻击既不打断别的技能，也不阻止别的技能打断它（`ActivationGroup = Independent`）。设计文档 7.3 明确写了"ALS 会打断 Montage（落地状态切换、TurnInPlace）"，而本改动没有任何抑制手段。

**什么场景会触发**：移动/落地/转身过程中按攻击（`DefaultSlot` 是全骨架槽位，会被 ALS 状态机抢占）→ `OnInterrupted` → 直接 `EndAbility`，攻击在中途被取消且无补偿动画；将来加入第二个技能后，攻击可被任意独占技能顶掉。

**推荐修改方式**：
1. 先在 `HodgeGameplayTags.h/.cpp` 声明 `Status.Attack.*`（及需要的 `GameplayEvent.Attack.*`），并在 `GA_Attack` 上配 `ActivationOwnedTags = Status.Attack`（或按段用 GE 授予），让动画层与互斥系统能查询。
2. 明确攻击与移动/其它技能的关系：至少在 `ActivationGroup` 或 `BlockAbilitiesWithTag` / `CancelAbilitiesWithTag` 上表达"攻击期间禁止转身/落地打断"或"允许被闪避取消"。
3. 标签落地后同步更新 [GAS、AbilitySet 与技能生命周期](07-gas.md) 与 [相机、移动与动画](09-camera-animation.md)。

### R5 — MINOR（层级 5 GAS / Tag）

**位置**：`Config/DefaultGameplayTags.ini`（新增文件）：`+GameplayTagList=(Tag="a",…)`、`+GameplayTagList=(Tag="Ability.Attack",…)`；`HodgeGameplayTags.h/.cpp` 中无 `Ability.Attack`，但有既有的 `Ability.Type.Action.Melee`、`InputTag.Ability.Melee`。

**为什么有问题**：项目约定是**原生标签集中声明**（`HodgeGameplayTags.h` 用 `UE_DECLARE_GAMEPLAY_TAG_EXTERN`、`.cpp` 用 `UE_DEFINE_GAMEPLAY_TAG`，见 [GAS](07-gas.md) 12.1 与设计文档 12.1）。用 ini 注册一个 `Ability.*` 新标签，会让它无法在 C++ 里引用（`TagRelationshipMapping`、`CanActivateAbility`、失败反馈等都用不到），也让"`Ability.Type.Action.Melee` 与 `Ability.Attack` 谁代表攻击"这件事变成两套并行语义。`Tag="a"` 是一条无意义的脏数据。

**什么场景会触发**：任何需要用 C++ 按标签切断/取消攻击、或按 `Categories=Ability` 做编辑器分组时；清理 ini 时会误删/漏删。

**推荐修改方式**：删除 `Tag="a"`；把 `Ability.Attack` 改为在 `HodgeGameplayTags.h/.cpp` 集中声明的原生标签（或直接复用 `Ability.Type.Action.Melee` 并在文档中统一口径），保留 ini 只做非代码标签的补充。

### R6 — MINOR（层级 5 GAS / AbilityTask）

**位置**：`GA_Attack` 节点 `FAA89B9D46FA779ACCA7CEB0FD802505`（"等待Gameplay事件"），`EventTag` 空、`OnlyTriggerOnce=false`、`OnlyMatchExact=true`，四个输出引脚全未连线。

**为什么有问题**：每次激活都会创建一个 `UAbilityTask_WaitGameplayEvent` 并以**空标签**注册到 `ASC->GenericGameplayEventCallbacks`（`AbilityTask_WaitGameplayEvent.cpp:28-44`）。功能上无害（能力结束时随能力销毁并解绑），但它是一条永远不会触发、也没有输出接线的死路径，掩盖了"命中事件本来该接在这里"的设计意图，后续排查时会被误认为已接通。叠加 F12（Montage 无 Notify），这条链实际上从头到尾没有任何事件源。

**什么场景会触发**：不触发运行错误；触发的是**误判**——后续维护者看到"等待Gameplay事件"会以为命中链已经写好。

**推荐修改方式**：删除该节点；等真正接命中链时再按设计方案补 `EventTag`（父标签 + `OnlyMatchExact=false`）并接线，或改走 `AuthorityOnly` 的 `HitCheck` 通道。

### R7 — MINOR（层级 3 生命周期）

**位置**：`GA_Attack` 变量 `attackcount`（`InstancedPerActor` 实例变量）；实例所属 ASC 挂在 `AHodgePlayerState`（`HodgePlayerState.cpp:40-47`）。

**为什么有问题**：玩家 ASC 的所有者是 `PlayerState`，换 Pawn / 死亡重生时 ASC 与 Ability 实例都保留，`attackcount` 会跨身体延续；而连击状态按设计（决策 Q4，见 [AbilityTimeline 设计方案](../Design/ability-timeline.md) 9.5）应属于"当前身体的短期状态"，死亡/换身体应清零。

**什么场景会触发**：连击到第 3 段时被换下/死亡，1.2 s 内重生并立刻攻击 → 第一刀直接是第 4 段。**影响有限**：F7 的 1.2 s 重置计时器会让它在多数情况下自行归零，所以只在"换 Pawn 后立刻攻击"这一窄窗口可见；同时换 Pawn 时若 Ability 仍在激活（Montage 播放中），也没有任何清理路径。

**推荐修改方式**：短期在接受范围内可不改，但应在交付说明里标注；中期把段数移到挂在 Pawn 上的战斗组件（设计文档 9.5），或监听死亡/`InitState_DataInitialized` 复位。

### R8 — MINOR（层级 4 网络 / 层级 6 预测）

**位置**：`GA_Attack` 的 `attackcount`（未复制，蓝图变量）+ `NetExecutionPolicy = LocalPredicted`；`HodgeAbilitySystemComponent` 未对 `ClientActivateAbilityFailed` 做段数修正。

**为什么有问题**：两端各自执行同一套蓝图来保持段数一致，这是设计文档 8.3 认可的"两端确定性推导"路线；但 `attackcount` 是**自定义 `int32` 状态，不在 GAS 的自动回滚范围内**。一旦客户端预测激活成功而服务器激活失败（`ServerTryActivateAbility` 被拒），客户端已经自增并播了动画，服务器没有，之后每一段都错开一位，且没有任何修正路径。

**什么场景会触发**：当前 `GA_Attack` 没有 Cost / Cooldown / 激活阻断标签，服务器侧几乎不会拒绝，因此**今天是潜在问题**；一旦按 KB-08 补上激活条件（死亡、受击硬直、Cost），分歧就会真实发生。

**推荐修改方式**：短期记录为已知风险；补激活条件之前先决定修正机制 —— 让客户端在 `ClientActivateAbilityFailed` / `ScriptOnAbilityFailedToActivate` 时回退段数，或把段数改为服务器权威下发 + 客户端只做表现（参考设计文档 8.3 / 9.5 的"客户端无法判定的服务器事实要单独设计修正机制"）。

### R9 — MINOR（层级 9 可维护性）

**位置**：`GA_Attack` 图内的 `3F555457…`（`Time = 1.2`、硬编码）、`8020560A…`（`B = 4`、硬编码）；变量名 `attackcount`、`NewVar`；自定义事件 `8A83DEF1…` 使用默认名"自定义事件"且**被两处调用**（`3F555457…` 的 Delegate 与 `111C1072…` 的直接调用）。

**为什么有问题**：连击重置窗口（1.2 s）与段数上限（4）是**手感与玩法参数**，硬编码在蓝图里意味着改参数必须打开蓝图图找到引脚；`NewVar` 是自动生成的变量名，`attackcount` 大小写与语义都不清晰（实际是 1 基段号）；同一个自定义事件既是定时器回调又被直接调用，"连击重置"发生几次、由谁触发，在图上读不出来（F7 表明 `attackcount>=4` 时会立即重置一次，1.2 s 后再重置一次，第二次是冗余的）。

**什么场景会触发**：调手感需要改动画时长或连击上限时；排查"段数为什么被重置"时。

**推荐修改方式**：把 1.2 s 与 4 提到命名的变量/DataAsset（或至少用 `Make Literal` 命名引脚）；重命名为 `ComboIndex` / `ComboResetTimerHandle`；把定时器回调单独命名（如 `ResetCombo`）并去掉多余的立即调用路径；补一段说明连击规则的事件注释。

### R10 — MINOR（层级 9 可维护性 / 层级 7 资产）

**位置**：`/Game/Main/Data/DA_Pover`（实际类型 `HodgeAbilitySet`）；`/Game/CodexText/Montage/AM_Attack05_Montage`（AssetRegistry 无任何 referencer）。

**为什么有问题**：`DA_Pover` 的命名（`DA_` 前缀 + 角色名）与它的真实类型/职责（AbilitySet）不符，后续按名字找 PawnData 会找错对象；`AM_Attack05` 是随本次改动一起提交的孤儿资产，会进入 Cook 且没有任何用途，也不清楚它是否本该是第 5 段。

**什么场景会触发**：在 `/Game/Main/Data` 下按命名挑选 PawnData/AbilitySet；打包体积审计。

**推荐修改方式**：`DA_Pover` 重命名为 `AS_Pover`（或 `DA_Pover_AbilitySet`）并保留重定向；确认 `AM_Attack05` 的用途，若不是第 5 段则删除或补进 `AttackMontage` 并同步上限。

### R11 — NIT（层级 10 Style）

**位置**：同上蓝图；`Config/DefaultGameplayTags.ini`。

- 蓝图变量全小写无前缀（`attackcount`）、自动名未改（`NewVar`）；Epic/蓝图惯例应为 `ComboIndex`、`ComboResetTimerHandle`。
- 自定义事件保持默认名"自定义事件"，无法从节点标题判断语义。
- `Ability.Attack` 与既有 `Ability.Type.Action.Melee` 并存，命名空间语义重复（见 R5）。
- `Config/DefaultGameplayTags.ini` 保留 `Tag="a"`。
- 不修改既有拼写（`DA_Dafult_PawnData` 的 `Dafult`）—— 符合 AGENTS.md，不作为问题。

## A3. 结论

**是否建议合入**：**不建议按现状合入。** 本次改动把"普攻能播动画"这一层接通了（输入链、资产链、槽位均已核实成立，见 F9/F10/F11/F13），但 R1（连击不可接段）、R2（无命中闭环）决定了它是否算"功能完成"，R3（生产资产依赖实验目录 + 硬引用动画）会在后续清理时变成事故。R1/R2 若被明确判定为"本次范围之外"，则需要改的是**交付口径与文档**，而不是代码。

**必须修改项（合入前）**

1. **R3** —— 决定 4 条攻击 Montage 与 `ABP_ALS_Grounded` 的归属：迁到 `Main`，或把 `CodexText` 转正并更新知识库定位。这是纯资产改动，成本最低、收益最高，且越晚改引用越难。
2. **R2 或明确降级** —— 要么补上 `GameplayEvent.MeleeHit` → 服务器命中判定 → 伤害 GE 的最小闭环；要么在交付说明与 [接通清单](12-integration-backlog.md) 中显式写明"本次只有动画表现，无命中判定"，并删除 R6 的死节点。
3. **R1 确认口径** —— 明确"连击"是否要求窗口内接段。若要求，必须先做输入缓冲；若不要求，把取舍写进文档并记录为已知限制。

**可延后项**

- R4（`Status.Attack.*` 标签与互斥配置）：在没有第二个技能、也没有动画层消费方之前，可以稍后补；但补命中链时应一起做。
- R5（`Ability.Attack` 改为原生标签）、R6（删除死节点）、R9（参数外提与重命名）、R10（`DA_Pover` 改名、`AM_Attack05` 去留）：不阻塞合入，建议同一批处理。
- R7（`attackcount` 跨 Pawn）、R8（预测分歧修正）：在换 Pawn/死亡流程与激活条件落地前不会真实发生，随 KB-08/KB-12 一起做。
- 与本次改动同链但**不属于本次改动**的两处：`HodgeHeroComponent.cpp` 中大量 `[HODGE-DBG]` 临时日志（源码注释已写"定位后删除"，本次普攻输入验证完成后应清理）；`IMC_Default` 里残留的 `/Game/ThirdPerson/Input/Actions/IA_Move` 模板映射（与 `Main/Input/InputAction/IA_Move` 重复，实际只有后者会被绑定）。

**建议验证方法**

前置：`Saved/MCP/diagnostics` 无异常；按 [AI 开发与验证流程](../AI_DEVELOPMENT.md) 关闭编辑器做一次常规构建（本节结论不依赖 C++ 改动，但仍需确认蓝图父类未失效）。

1. **蓝图编译**：打开 `/Game/Main/Character/Hero/GA_Attack`、`/Game/Main/Character/Hero/BP_Hero_Pover`、`/Game/CodexText/AnimInstance/ALS/ABP_ALS_Grounded`，Compile 后确认无错误/失效引脚，再保存。记录实际结果（无法用"能打开"代替"编译通过"）。
2. **输入与首段（单机 PIE）**：输出日志过滤 `HODGE-DBG`，点击左键应看到 `Hero input CALLBACK AbilityPressed InputTag.Ability.Melee`；角色应播放 `AM_Attack01_Montage`。这条同时验证 F9 的整条链。
3. **R1 的量化验证**：在 `GA_Attack` 上临时把 `attackcount` 用 `Print String` 打出，分别测三种节奏 —— ① 动画播完后 1.2 s 内再按；② 动画播完后超过 1.2 s 再按；③ 动画播放中连点。预期（按当前实现）：① 段数 2、3、4 递增；② 回到 1；③ 段数不变。③ 若不满足需求，R1 就是必须修改项。
4. **R2/R6 验证**：确认 Montage 内确实没有 Notify、`GA_Attack` 不派发任何 GameplayEvent（可用 `WaitGameplayEvent` 之外的日志或 GameplayDebugger 观察），据此决定是否降级交付口径。
5. **中断路径**：攻击过程中移动/落地/受击，观察 `PlayMontageAndWait` 的 `OnInterrupted`/`OnCancelled` → `EndAbility` 是否生效、1.2 s 后 `attackcount` 是否归零、角色能否再次攻击（验证无卡死）。
6. **联机（2 人 PIE，Play As Listen Server 或独立进程）**：`NetExecutionPolicy = LocalPredicted` 下核对 —— 服务器与客户端播放的是同一段 Montage、`attackcount` 两端一致、模拟代理能看到攻击动画。**单人 PIE 测不出 R8**；若要主动暴露 R8，可临时给 `GA_Attack` 加一个服务器侧才满足的 `ActivationBlockedTags` 观察段数是否错位（验证完必须移除）。
7. **R3 验证**：`git status` 确认 Montage 迁移后 `GA_Attack` 引用与 `BP_Hero_Pover` 的 AnimClass 指向新路径；跑一次 Cook 或 Content Browser 的 Reference Viewer 确认 `Main` 不再引用 `CodexText`（如果选择迁移）。

**验证边界**：本节所有结论来自磁盘资产与编辑器只读读取（`GA_Attack` 图 22 个节点、各节点引脚默认值、相关 CDO 与 Montage 属性均已核对），**没有**执行 C++ 构建、蓝图编译、PIE、联机或打包；`A3` 中的"预期表现"是静态推导，必须按上述步骤实测后才可写成"通过"。

---

## A4. 后续进展（2026-09-19 补记）

> A0–A3 是 2026-09-17 的评审快照，保留原样。以下是针对同一批资产与标签的**当前状态更新**。

- **R4（`Status.Attack.*` 标签与互斥配置）**：**源码侧已部分落地** —— `Status.Attack` / `.Windup` / `.Active` / `.Recovery` 与 `GameplayEvent.Attack` / `.Test` / `.Timeline.End` / `.Interrupted` 已在 `HodgeGameplayTags.h/.cpp` 集中声明（随 HEAD `77b7dba` 提交）。**仍未确认**：`GA_Attack` CDO 是否已配 `ActivationOwnedTags` / `CancelAbilitiesWithTag` / `BlockAbilitiesWithTag`（二进制资产未解析），以及是否已改用 `UHodgeAbilityTask_PlayTimeline`。
- **R5（`Ability.Attack` 注册方式）**：**未处理** —— `Config/DefaultGameplayTags.ini` 仍是 `Tag="a"` 与 `Tag="Ability.Attack"` 两条，`Ability.Attack` 仍是 ini 标签而非原生标签。
- **R1 / R2 / R6（连击接段、命中闭环、空 `WaitGameplayEvent` 死节点）**：**未处理** —— 本阶段只实现了 `UHodgeAbilityTimeline` + `UHodgeAbilityTask_PlayTimeline` 的调度器与标签账本（见 [2026-09-19 记录](22-update-2026-09-19.md)）；窗口 GE 施加/移除与 Point 派发已于 2026-09-19 补测通过（**重入类时序、跨端、时钟倒退仍未验证**）；`GA_Attack` 仍无命中判定。
- **R3 / R10（资产归属、命名、孤儿资产）**：**未处理** —— 攻击 Montage 仍在 `/Game/CodexText/Montage/`，`DA_Pover` 仍是 `HodgeAbilitySet`。

时间轴的实现与验证边界以 [2026-09-19 记录](22-update-2026-09-19.md) 为准。

---

## A5. 后续进展（2026-09-22 补记）：移动取消后摇消费方

> A0–A4 保留原样。以下是同一时间轴议题在本轮的进展与评审关注点，详见 [2026-09-22 记录](23-update-2026-09-22.md)。

- **新增**：`Status.Attack.Cancel` / `.Move` 标签、`UHodgeAbilityTask_WaitMoveCancel`、`UHodgeHeroComponent` 移动意图信号（`HasMoveIntent` / `GetMoveIntent` / `OnMoveIntentChanged`）。
- **评审关注点（新增代码，本轮未编译、未 PIE）**：
  1. **本地控制端语义**：移动意图**不复制**、只在本地控制端判定；确认没有把它当"两端一致"的复制状态来设计（头文件已声明理由）。
  2. **输入意图的采集点**：`Input_Move` 里记录意图必须在任何"屏蔽移动输入"之前；若后续用 `SetIgnoreMoveInput` / `DisableMovement` 实现攻击期间禁移，会**静默**让取消逻辑恒为 false。评审时检查是否违反了这条（头文件已写此约束）。
  3. **委托 / 事件解绑**：`WaitMoveCancel` 在 `OnDestroy` 解绑 `RegisterGameplayTagEvent` 与 `OnMoveIntentChanged`，用弱引用持有 ASC / HeroComponent；确认无残留回调。
  4. **广播期重入**：监听者常在 `OnMoveCancel` 回调里**同步** `EndAbility`，任务会在广播途中被销毁；确认 `bSucceeded` 先立、`bInOnDestroy` 兜底、`ShouldBroadcastAbilityTaskDelegates()` 守卫齐全。
  5. **`ReadyForActivation()`**：C++ 调用方必须自己调（与 `PlayTimeline` 同）；确认新任务的实际调用方没有漏。
- **仍未处理**：R1 / R2 / R6 不变；**当前没有正式攻击 Ability 消费 `OnMoveCancel`**，所以"取消后摇"尚未端到端接通。
