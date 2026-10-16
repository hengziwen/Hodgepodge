# Lyra Inventory 与现有 Equipment 集成设计

初稿：2026-10-11。更新：2026-10-17。版本：v0.3 数据层实现。适用：UE 5.5.4。

**状态：数据层与单武器接入代码、正式配置已落地，最终验证状态见 [本轮验收](../Validation/inventory-equipment.md)。背包 UI、拾取和存档仍后置。下文设计契约配合第 9 节的实际接口阅读。**

2026-10-16 范围决定：先搭建背包数据层及其与现有 Equipment 的连接，背包界面暂不制作。设计记录阶段只更新文档；随后用户授权按本文编码和测试，进入当前实现阶段。此前讨论中“首批加入可见背包页面”的建议不再适用。

### 当前提交与暂缓决定（2026-10-17）

用户决定提交当前已完成的数据层、单武器装备接入、正式配置、工具与测试，剩余工作暂缓。2026-10-16 的 Editor／Game 构建、139 项原生回归和 67 条 PIE 集成断言结果保留在验收记录中；本次只整理文档与提交，不重新执行构建、PIE 或视觉校验。

- **功能暂缓：** 背包页面、图标／详情／按钮／焦点与输入接入、拾取／掉落、物品使用 GA、QuickBar、多装备位、仓库／交易、存档、跨地图／跨 Experience 持久化、多角色共享背包及每武器独立连段。保留当前数据接口，不提前创建空实现。
- **验证缺口暂缓：** Cook／打包、独立专服构建、独立进程联机、重连／更换 PlayerState 持久化、延迟／丢包压力和 Iris 运行模式。原生组件生命周期测试不替代这些集成结论；按原需求未做额外视觉校验。
- **恢复条件：** 用户明确安排下一阶段后，先核对本文与当前源码、资产，再按具体需求补充设计、实现和验收；本轮提交不自动启动后续阶段。

## 1. 背景与目标

将 Lyra 的 Inventory（物品容器／背包）设计接入 Hodgepodge，让玩家能够持有物品，并从背包选择武器装备。复用现有 Equipment、属性协调器、GAS 和 Experience，不重写已完成的武器表现与能力授予系统。

参考工程：`E:/Project/UProject/study/Epic/LyraStarterGame`。编码前基线已有 EquipmentDefinition／Instance／Manager、FastArray 与子对象复制、装备属性、能力撤销、武器表现，以及 GameplayTagStack；当时尚无 Inventory 核心和 QuickBar。本轮 Inventory 实现见第 9 节，QuickBar 继续后置。

首版范围：

- 物品定义、实例与 Fragment；服务器添加、移除、查询、数量消耗。
- 装备类物品独立实例，普通物品支持配置堆叠上限。
- 玩家背包复制、变更通知及一个当前武器装备位。
- 默认武器入包、从背包装备、卸装及 Pawn 更换后的重新装备。

暂不包含背包 UI、图标／详情／按钮制作、背包打开输入、拾取／掉落交互、仓库／交易、存档、跨地图持久化、多装备位、QuickBar 多槽位、物品使用 GA，以及每武器独立连段／动画体系。普通怪物继续直接使用 Equipment，不强制拥有背包。攻击吸附、受击改造、翅膀和伤害跳字均不在本次修改范围，保持各自当前实现。

背包核心不以完整角色属性系统为前置条件。本轮仅沿用现有装备等级与加成，不新增暴击、元素、随机词条、稀有度成长或新的伤害公式。物品分类、图标和扩展片段按实际消费需求后续增加，避免为尚未设计的玩法预建配置。

## 2. 核心概念与职责

**ItemDefinition** 是物品类型的静态配置；**ItemInstance** 是玩家实际持有的一件物品或一个堆叠；**EquipmentInstance** 是该物品在当前 Pawn 上生效时的临时运行对象。卸装不删除背包物品，删除物品也不能只销毁武器模型。

拟议类型与职责（均留在现有业务模块内）：

- `UHodgeInventoryItemDefinition`：沿用 Lyra 的 UObject 蓝图定义形式，保存名称、最大堆叠数与 Fragment。
- `UHodgeInventoryItemFragment`：可内联配置的静态片段。首版提供可装备片段和初始物品 Tag 数值片段；图标片段随 UI 阶段加入。
- `UHodgeInventoryItemInstance`：保存稳定 ItemId、Definition、物品等级和 StatTags；不拥有装备 Actor，也不向 ASC 授予能力。
- `UHodgeInventoryManagerComponent`：管理持有列表、数量、服务器修改与拥有者复制。
- `UHodgeEquipmentSelectionComponent`：保存当前武器 ItemId，协调装备切换与 Avatar 绑定；不复制 EquipmentManager 的授予循环。
- 现有 `UHodgeEquipmentManagerComponent`：继续创建 EquipmentInstance、授予／撤销能力与属性、管理装备 Actor。
- 现有 `UHodgeAttributeCoordinator`：继续协调角色属性与生命资源策略，不成为第二个背包。

数据关系：

```text
PlayerState
  ├─ InventoryManager：持有物品
  ├─ EquipmentSelection：当前选择的 ItemId
  └─ ASC／AttributeCoordinator：能力与属性
                ↓ 当前 Avatar 就绪后
Pawn → EquipmentManager → EquipmentInstance → 武器 Actor／能力／属性 GE
```

## 3. 数据结构与所有权

以下字段为建议契约，不是最终 C++ 声明。

- `ItemDefinition`：DisplayName、MaxStackCount、Fragments。可装备物品的 MaxStackCount 必须为 1。
- `EquippableFragment`：引用现有 EquipmentDefinition。
- `ItemInstance`：ItemId（FGuid）、ItemDefinition、ItemLevel、StatTags。物品等级沿用现有装备属性配置的合法范围。
- `InventoryEntry`：ItemInstance、StackCount；列表采用 FastArray，实例采用 UObject 子对象复制。物品数量与 StatTags 的数值不混用。
- `EquipmentSelection`：SelectedItemId、状态修订号；实际已装备实例由当前 Pawn 的 EquipmentManager 持有。
- `OperationResult`：成功／失败原因、受影响 ItemId、实际数量与必要诊断。预期失败不使用 check 崩溃。

首版采用 InventoryManager 与 EquipmentSelection 放在 PlayerState、通过 Experience 注入的方案；基础配置失效时停止装备操作。PlayerState 未更换的死亡／重生保留持有物品和选择，Pawn 上的装备实例重新创建。组件被移除或 Experience 停用时解绑并撤销本组件管理的装备；跨 Experience 的背包保留策略另行设计。这不等于退出游戏、重新连接、更换 PlayerState 或跨地图后已经具有持久化。

背包列表、实例数据和选择信息仅向拥有者复制；已装备武器的必要表现仍按现有 Pawn／Equipment 复制给观察者。远端不能依赖私人 ItemInstance 的引用可解析，也不复制整个背包来满足武器显示。

同一类型的两把武器可以拥有不同 ItemId 和等级。堆叠只合并 Definition、等级与可变数据兼容的普通物品；首版拒绝合并带不同实例状态的物品。

### 3.1 数据不变量与可调边界

- ItemId 在服务器创建，存续期间稳定；换 Pawn、卸装和重新装备不重新生成。整个堆叠移除后，该身份不再用于新物品。
- 已提交条目的 StackCount 为正整数且不超过 MaxStackCount；可装备物品固定为 1。数量变为 0 时统一移除条目并注销复制子对象。
- MaxStackCount 与最大条目数必须大于 0；容量按条目计算，不按物品总数量计算。入包优先填充兼容堆叠，再按需要创建条目，预检后空间不足则整体拒绝。
- StatTags 表示物品自身的整数状态，不代替条目数量、不直接写角色 AttributeSet。Fragment 初始化仅处理新实例，不在重复绑定或复制回调中再次初始化。
- 物品数量与等级只由库存的服务器修改入口维护；界面、装备实例和属性协调器读取结果。背包玩家的装备等级不能绕过库存单独修改。
- 首版不开放任意实例状态改写、手动拆分堆叠或物品转移。无不同状态的真实用例时，不新增可配置的复杂堆叠比较规则。
- 最大条目数放入现有玩家默认物品配置所依附的 PawnData；MaxStackCount 放 ItemDefinition。正式数值实施时结合测试资产确定，不将实验容量写成固定协议常量。

## 4. 接口与数据流

拟议的 Inventory 接口：

- `CanAddItemDefinition(Definition, Count)`：只验证，不修改数据。
- `AddItemDefinition(Definition, Count)`：返回结果与受影响物品，按堆叠规则创建／合并。
- `RemoveItemById(ItemId)`：移除整个实例；当前装备物品必须先成功卸装。
- `ConsumeItemsByDefinition(Definition, Count)`：预检总数量，再扣除实际数量；不足时不部分扣除。
- `GetItems()`、`FindItemById(ItemId)`、`GetTotalItemCountByDefinition(Definition)`：只读查询，数量按 StackCount 求和。
- `OnInventoryChanged`：发布添加、数量变化与移除，包含 ItemId、旧／新数量和修订号。

跨多个堆叠的添加／消耗返回所有受影响身份与数量变化，不能只返回第一件物品就宣称全部完成。查询使用稳定 ItemId，不把数组下标或对象地址当作对外身份。对“消耗到当前装备物品”的操作首版直接拒绝；显式移除整个已装备实例，必须通过选择模块成功卸装后才能删除，避免通用消耗入口隐式卸装武器。

拟议的装备选择接口：

- `CanEquipItem(ItemId)`：验证持有权、Fragment、装备配置、Avatar／ASC 就绪与战斗限制。
- `RequestEquipItem(ItemId)`、`RequestUnequip()`：返回操作结果；已有相同选择且实际装备有效时幂等成功。
- `BindAvatar(Pawn)`、`UnbindAvatar(Pawn)`：按 Pawn 生命周期绑定和撤销，忽略旧 Pawn 的迟到通知。
- `OnEquipmentSelectionChanged`：通知实际结果；UI 后续读取快照并订阅变化。

这些服务器修改接口不等于客户端可以直接调用。客户端经自身 PlayerController 的请求入口发送 ItemId，服务器重新验证所属背包和当前状态；不接受客户端指定的等级、装备定义或其他玩家实例。

客户端首版仅开放装备／卸装请求，不提供任意发物品、删除物品或修改数量的 RPC。发放和消耗由服务器业务或受控测试入口调用。操作结果区分成功、依赖等待和拒绝，并携带具体原因；至少覆盖非法数量／配置、容量或数量不足、不持有该物品、不可装备、动作限制、Avatar 未就绪、装备失败及恢复失败。

如果提供客户端结果通知，应携带请求关联标识，但结果 RPC 不替代库存和选择的状态复制；客户端不能把通知中的 UObject 引用当作已经完成复制。数据层不为此新增 Widget、背包页面或 UI 输入绑定。

初始化流程：

1. Experience、玩家 PawnData 与背包组件就绪。
2. 服务器按首版默认物品配置加入物品，每个 PlayerState 的初始化来源只执行一次；重生不再次发放。
3. 选择默认物品，等待当前 Avatar、ASC 和 EquipmentManager 就绪。
4. 使用 ItemId／ItemLevel 调用现有 `EquipItemWithState`，验证成功后发布实际选择结果。

首版拟在 PawnData 增加玩家默认物品配置，引用 ItemDefinition 并指定数量；其中一件指定为默认武器。保留现有 DefaultWeaponDefinition 给怪物与未迁移玩家使用，启用背包的玩家不得同时走两条默认装备入口。

切换流程：

1. 校验新物品与装备依赖，保存旧物品和当前 Avatar 身份。
2. 首版在攻击执行／待处理战斗请求、Dash／Sprint、强受击或死亡期间拒绝手动换装备，不自动取消这些动作。
3. 走现有卸装入口，再按新物品身份装备；成功后提交选择并通知。
4. 失败时尝试恢复旧装备。若恢复也失败，明确报告并清空实际装备状态，不显示虚假的成功；阻止依赖该武器的动作。

上述恢复是补偿操作，不承诺任意 GA、Instant GE 或蓝图回调的副作用都能回滚。存在不可逆失败时停止本次操作并输出诊断，不无限重试。无 Avatar 时保留选择，绑定新 Pawn 后恢复实际装备，不将“等待 Avatar”报告为已装备成功。

显式卸装成功后清空选择；单纯解绑旧 Pawn 则保留物品选择。SelectedItemId 表示期望恢复的物品，实际生效装备仍以当前 Pawn 的 EquipmentManager 为准；通知必须能够区分“有选择但等待绑定”和“已经装备”，不新增一套平行装备列表。

## 5. 与现有实现的衔接

需要调整的主要位置：

- `HodgeCombatCharacter::InitializeDefaultEquipment`：背包玩家改由选择模块驱动；旧链路保留给怪物及未迁移配置。
- PlayerState 的 `OwnedEquipmentStates` 与 AttributeCoordinator 的装备记录：收敛物品身份和等级的事实来源到 Inventory；旧接口改为兼容适配，不能独立写两份等级。
- `EquipmentManager::EquipItemWithState`：复用现有授予与失败校验；允许从物品取身份与等级，不重新生成 GUID。
- EquipmentInstance 的来源关系：关联 ItemInstance，但远端表现与攻击不能依赖私人来源对象。设置来源的时机须保证需要来源的装备回调能够读取它。
- 武器来源解析与攻击准备：确认只选择当前生效武器。现有 ComboDefinition 仍来自 PawnData，不随物品切换自动替换。
- Experience 与初始化就绪：组件晚到、重复通知、解绑、重生均可安全推进。数据就绪与实际装备成功分开判断；所有新增类留在现有 Hodgepodge Runtime 模块。

首版使用组件委托与完整快照通知，不引入 GameplayMessageRouter、CommonGame 或新的业务模块。服务器修改与客户端复制通知都要覆盖；晚订阅者必须读取已有快照，不能只等待一次事件，订阅顺序和修订号见下文。

### 5.1 旧身份与等级记录的迁移

1. 背包启用之前盘点默认 EquipmentDefinition 与拟议 ItemDefinition 的显式映射；不凭武器名称或蓝图路径猜测。
2. 初始化库存时，对需要接续的旧装备记录保留 InstanceId／Level，以该身份创建物品；重复执行命中同一身份时只校验一致性，不重复发放。映射缺失、身份冲突或等级非法时报告错误，不静默生成替代物品。
3. 背包启用后，物品实例成为拥有身份和等级的事实来源；OwnedEquipmentStates 仅在确需兼容旧消费者时作为派生记录。AttributeCoordinator 的 RememberEquipment 不得反向创建另一份独立物品或覆盖库存等级。
4. 背包玩家现有 SetEquipmentLevel 入口需要路由或限制到统一库存修改链，再刷新已装备属性；本轮不扩展升级玩法。未启用背包的玩家和怪物保留原装备路径。
5. 迁移的是当前运行时数据契约，不是磁盘存档升级。保留旧字段与正式资产引用，直到全部调用者和回归场景核查后再决定删除，不通过直接移除 UPROPERTY 清空旧数据。

旧记录接续是服务器内部受控初始化路径，可以使用已有身份创建新库存实例；它不等于开放外部 ItemInstance 接管。默认物品发放与旧记录接续须在同一初始化计划内去重，不能先接续默认武器，再按默认配置新增另一把。

### 5.2 复制、通知与资源生命周期

- InventoryList、选择信息及 ItemInstance 的子对象复制均限制拥有者；PlayerState 可能对其他连接相关，不能仅限制数组就假定子对象也不会外发。注册子对象和传统 ReplicateSubobjects 路径按项目实际启用方式分别实现，不能以 SetupIrisSupport 认定已启用 Iris。
- 创建、提交、注册复制子对象和删除、注销必须配对；库存条目用反射持有实例，待提交对象在同步回调期间也须保活。移除后不再由旧条目或待处理回调恢复引用。
- FastArray 的条目引用和 UObject 字段可能分批到达。客户端只在物品身份与定义可解析后发布可读变化；后续复制到达应继续推进就绪，不能将暂时空引用误判为永久删除，也不使用无限轮询。
- 服务器本地修改也发布变更，覆盖 Standalone／Listen Server 主机；客户端由复制变化通知，避免只依赖不会在服务器本地执行的复制回调。
- 修订号用于本地快照与变更一致性，不承诺跨机器每次中间修改都有一条通知。复制可能合并多次修改，通知是状态变化而非可靠逐操作账本。
- 后续 UI 或其他消费者在同一游戏线程先绑定监听、读取带修订号快照，再忽略已包含的通知；读取快照和订阅不能跨异步等待而不做补读。当前阶段只提供数据契约和测试消费者，不制作 UI。
- 同步委托、能力激活与蓝图装备回调可能重入。修改过程中拒绝重入写请求，提交完整状态后发布变化；回调后再次核对当前 Avatar 和本次操作身份。
- 默认发放标记只有整批必需物品成功提交后才完成；配置／容量失败不能留下半批物品或“已初始化”假状态。换 Pawn 不重新发放；组件停用／重新注入不承诺跨 Experience 持久保留库存。
- 手动装备限制不阻碍解绑清理；死亡或 Pawn 销毁时仍必须撤销当前实例、委托及能力句柄。旧 Pawn 迟到通知不得卸掉新 Pawn 的武器。

## 6. 必须补齐的 Lyra 示例限制

本地 Lyra 的 AddEntry(ItemInstance) 尚未实现，CanAddItemDefinition 未检查容量／唯一性；总数量按条目计数，ConsumeItemsByDefinition 按整条移除，不能直接作为项目的堆叠实现。

首版只提供按 Definition 创建物品，暂不开放外部 ItemInstance 转移接口；实现正数量、堆叠上限、数量溢出与总数预检，统一移除时的复制子对象注销。容量采用可配置最大条目数，达到上限时不部分入包。已有装备定义和第三方资源不批量重命名或重存。

### 6.1 已核查的 Lyra 参考与适配差异

以下路径相对于本机 `E:/Project/UProject/study/Epic/LyraStarterGame`。本次为源码核查，没有打开 ShooterCore 蓝图确认其实际组件注入和初始物品资产配置，不将这些配置认定为已经迁入 Hodge。

- `Source/LyraGame/Inventory/LyraInventoryItemDefinition.h`：定义类、内联 Fragments 与 OnInstanceCreated；复用其 UObject 蓝图定义机制，补充实际需要的堆叠上限。
- `Source/LyraGame/Inventory/LyraInventoryItemInstance.h/.cpp`：定义引用、StatTags 与网络 UObject；在 Hodge 增加稳定 ItemId 和已有装备等级，复用项目已迁入的 GameplayTagStack。
- `Source/LyraGame/Inventory/LyraInventoryManagerComponent.cpp`：AddEntry、FastArray 复制回调、ReadyForReplication 和 ReplicateSubobjects；复用成熟容器与子对象机制，补齐服务器校验、真实堆叠数量和成批预检。
- 同文件 GetTotalItemCountByDefinition 按条目递增；ConsumeItemsByDefinition 按整条删除，数量不足前可能已删除部分记录，且该分支直接操作列表，未走管理器统一子对象注销入口。Hodge 不照搬这三种行为。
- `Source/LyraGame/Inventory/InventoryFragment_EquippableItem.h`：物品到 EquipmentDefinition 的静态映射；保留此接缝，不在 ItemInstance 内复制能力授予逻辑。
- `Source/LyraGame/Equipment/LyraQuickBarComponent.cpp`：EquipItemInSlot／UnequipItemInSlot；提取单武器选择职责，不迁移三槽快捷栏及其 UI。Hodge 的 PlayerState／Pawn 分工不同，不照搬其 ControllerComponent 生命周期。
- `Source/LyraGame/Equipment/LyraGameplayAbility_FromEquipment.cpp`：能力通过 Spec.SourceObject 找装备，再通过 Instigator 找物品。Hodge 仍以 EquipmentInstance 作为装备授予的来源，并审查需要读取物品的实际回调；普通观察者不依赖私人物品对象。
- `Source/LyraGame/Inventory/IPickupable.cpp`：包含模板和已有实例入包分支，但后者通向未实现入口；本轮不迁移拾取流程，也不把接口存在视为拾取事务完整。
- `Source/LyraGame/LyraGame.Build.cs`：Lyra 变更消息依赖 GameplayMessageRuntime；Hodge 使用委托／快照适配，不为库存新增该依赖。沿用现有 NetCore、GameplayTags、ModularGameplay 和 GAS 相关依赖，实施时复核公开头文件可见性。

Hodge 的主要接入文件为 `Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp`、`Private/Equipment/HodgeEquipmentManagerComponent.cpp`、`Private/Equipment/HodgeWeaponInstance.cpp`、`Private/AbilitySystem/Stats/HodgeAttributeCoordinator.cpp`、`Private/Core/PlayState/HodgePlayerState.cpp`，以及各自 Public 声明和 PawnData。现有 ResolvePresentationWeapon 在来源不明确且同时存在多把有效武器时返回空；单武器选择必须维持清晰来源，不能把全部背包武器都装到 Pawn 上解决查询。

## 7. 验收与开发顺序

以下为验收要求，实际执行结果单独记录在验收文档，不把设计条目本身当作通过证据。

### 阶段 A：容器与物品数据，最高优先级

输出：Definition／Fragment／Instance、InventoryManager、结果类型、快照和变更通知。新类采用 `Public/Inventory` 与对应 `Private/Inventory`；每个类的非内联实现集中在主 cpp，不引入额外 Runtime 模块。

完成标准：

- 同类两把武器拥有独立身份；合法堆叠按真实数量添加、合并、消耗和清零移除，武器不堆叠。
- 覆盖多个堆叠时总数正确；非法定义、重复关键 Fragment、非正数量、数量溢出、非法堆叠上限和容量不足有明确失败，库存保持原状。
- 容量已满但现有堆叠仍可容纳时允许添加；需要新增条目且超容量时整体失败。
- 消耗不足不修改；未装备物品移除正确，已装备物品的保护契约可被选择模块接管。
- 原生测试通过公开操作和快照验证结果，不以读取 FastArray 私有实现替代功能断言；不依赖背包 Widget。

约束：Fragment 初始化仅允许初始化物品自身数据，不授予能力、不写角色属性或触发外部不可逆效果；提交前失败不广播半成品变化。普通物品消耗只提供数量原语，不实现“使用药品”等 GameplayAbility。

### 阶段 B：单武器选择与旧数据接续，高优先级

输出：EquipmentSelection、PlayerState／PawnData 接线、默认物品配置、旧身份记录适配、服务器装备请求入口。

完成标准：

- 默认物品只发放一次，接续既有默认武器身份和等级，无第二份默认装备；未迁移玩家与怪物旧链路继续工作。
- 装备、卸下、切换后物品仍按规则留在容器；同类型两把武器不会因按 Definition 查找而混淆。
- 只对选中武器创建 EquipmentInstance；旧能力、属性 GE 和武器 Actor 配对清理，新装备数值沿用现有计算。
- 失败前校验、失败后补偿恢复和恢复失败均有可观察结果；装备来源在相关授予／回调需要时已经可读。
- 战斗限制、死亡、非拥有者物品与重复请求得到正确处理；清理路径不被手动操作限制阻断。

约束：不复制装备授予循环，不自动取消攻击以换装，不引入新属性公式；含不可逆副作用的失败不能宣称已完整回滚。

### 阶段 C：生命周期与网络验证，首版交付必需

输出：自动化结果、运行日志和数据快照证据，以及单人／Listen Server 双玩家实测记录。可以使用受控测试入口打印或断言实际数据，不制作临时背包 UI 代替验证。

完成标准：

- 晚注册、重复回调、换 Pawn、死亡／重生、组件移除、Experience 停用正确清理或等待，不重复发放，不让旧回调卸掉新装备。
- PlayerState 未更换的重生保留物品与选择，重新绑定后恢复实际装备；显式卸装后不在重生时重新选回旧物品。
- 玩家 A／B 各自仅获得自己的库存实例与选择；观察者看到必要武器表现，无法读取另一方整个背包。
- Listen Server 主机与拥有者客户端均收到正确变化；结果通知先于实例数据到达时不发布无效物品，消费者晚加入仍能读取完整快照。
- 删除和批量消耗后复制子对象正确注销，GC 后无悬空引用；通知回调重入被拒绝，不留下半提交状态。
- 常规 Editor／Game 构建、相关原生测试、受影响蓝图编译、单人和 Listen Server 双玩家 PIE 分别记录；回归既有装备、属性、Combat、敌人初始化及死亡。

约束：已有报告不算本轮复跑；PIE Dedicated 如执行需单列，不表示已有独立 Server Target。Cook、独立进程和存档按后续阶段安排，未做如实记录。

### 后续阶段：按需求启动

背包页面、图标 Fragment、详情与输入接入、拾取／掉落、消耗品使用、QuickBar、多装备位和存档分别评审。当前数据层保留稳定身份、快照和操作结果，使后续界面无需直接修改 FastArray；不为这些暂缓功能提前创建空框架。

实施顺序为阶段 A → B → C。A 可独立验证，但仅完成容器不能宣称已完成可用背包与装备接入；A／B／C 全部满足后，交付名称为“背包数据层与单武器装备接入完成”，不宣称背包界面已经完成。旧 v0.1 的 6～10 人日仅为历史估算，本次不沿用为完成承诺。

实施前细化项：默认物品的具体 Definition 映射、容量／堆叠实验值、现有等级写入调用者盘点和正式 Experience 注入资产。这些不改变本轮已确定的“先数据层、UI 后置”范围；多角色共享背包与跨 Experience 持久策略后续另行决定，不作为本轮已支持功能。

## 8. 本次文档交付与验证边界

2026-10-16 设计记录阶段：根据用户决定更新范围、数据约束、接口语义、旧装备记录接续、Lyra 复用依据与分阶段验收，仅修改 Markdown。后续编码阶段继续保留已有工作区源码和伤害跳字改动，不自动提交，构建与运行结果见独立验收文档；按用户要求不执行视觉校验。

## 9. 实际模块、接口与正式配置

- `Public/Inventory`／`Private/Inventory` 位于现有 Hodgepodge Runtime 模块，包含 ItemDefinition（及两个内联 Fragment）、ItemInstance、InventoryManager、EquipmentSelection；共享结果、发放项和快照条目位于 `HodgeInventoryTypes.h`。无新增业务模块、UI 框架或 GameplayMessageRuntime 依赖。
- `ItemDefinition::Validate(Level, Error)` 和编辑器校验共用配置规则；`ItemInstance` 的 ItemId／Definition／ItemLevel／StatTags 仅向拥有者复制，初始化后没有任意数值写入接口。
- `InventoryManager::InitializeFromPawnData(Data)` 原子发放默认批次并接续旧记录；`CanAddItemDefinition(Definition, Count, Level)` 只读预检；`AddItemDefinition`、`RemoveItemById`、`ConsumeItemsByDefinition` 均返回 `FHodgeInventoryOperation`，跨条目结果列出全部数量变化。数量查询使用 int64，写入时拒绝同类总数超过 int32 上限。
- `GetItems()` 返回可解析条目视图，配合 `GetRevision()` 与原生 `OnInventoryChanged` 使用；复制字段未到齐时等待后续网络回调收敛，不启动轮询。通知是本地状态变化，不保证跨网络逐操作回放。
- `EquipmentSelection::InitializeFromPawnData`／`BindAvatar`／`UnbindAvatar` 协调默认选择与 Avatar 生命周期，`RequestEquipItem`／`RequestUnequip` 为服务器修改接口，`GetSelectedItemId` 与 `IsSelectionApplied` 区分持有选择和实际装备。
- PlayerController 的公开 `RequestInventoryEquip(ItemId, RequestId)`／`RequestInventoryUnequip(RequestId)` 为普通原生门面，内部调用 Reliable Server RPC `ServerRequestInventoryEquip`／`ServerRequestInventoryUnequip`。结果经 Client RPC 返回，以 `GetLastInventoryResult`／`GetLastInventoryRequestId` 读取；不能用结果通知代替状态复制，也不开放客户端数量修改 RPC。
- Editor 模块的 `HodgeInventoryValidationLibrary` 仅用于 PIE 请求发送，局部处理 UE 5.5 编辑器脚本强制 Local Callspace 的保护，避免 Python 测试误报；服务器权限逻辑不变，Runtime 不依赖此工具。真实网络结果见验收，未执行视觉校验。
- `EquipmentManager::EquipItemWithState` 增加可选 SourceItem，在属性／能力授予及 OnEquipped 前建立来源；EquipmentInstance 的 Instigator 限拥有者复制，公开装备表现维持原链。库存玩家旧等级写入口拒绝旁路修改；OwnedEquipmentStates 按完整库存生成兼容镜像，删除物品时同步移除镜像。
- 角色在开启属性重建前检查库存配置与组件；库存玩家走选择组件，未迁移玩家与普通怪物继续旧默认装备路径。注销／重新注册组件配对注销和恢复复制子对象，重复回调不重复发物品。

正式配置为 `/Game/Main/Data/Inventory/ID_Sword`，其装备片段引用原 `/Game/Main/Data/Equipments/BP_Equipment_Sword`；`ID_TestMaterial` 为堆叠上限 20 的测试数据。`/Game/Main/Data/PawnData/DA_Dafult_PawnData` 启用 bUseInventory，容量初值 40，默认发放一把 1 级长剑；这些为可调原型配置，不改变既有伤害或成长公式。

`/Game/Main/Experiences/Exp_HodgeDefaultExperience` 为蓝图类，在原组件 Action 中增加 PlayerState 的 InventoryManager 和 EquipmentSelection。配置工具 `Tools/Inventory/setup_assets.py` 读取实际 CDO，并在 `Saved/Inventory/BeforeInventory` 保存首次修改前的原包；保留其他 Action 和 PawnData 字段，不制作页面或图标。后续手工更改已有物品定义时，工具不覆盖其配置。

参考：[现有装备设计](equipment-weapon-system.md)、[属性与装备加成](character-attribute-growth.md)、[共享战斗请求](combat-action-requests.md)。
