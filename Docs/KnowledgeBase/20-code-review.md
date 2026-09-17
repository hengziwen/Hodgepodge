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
