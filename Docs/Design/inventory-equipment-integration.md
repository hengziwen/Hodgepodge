# Lyra Inventory 与现有 Equipment 集成设计

日期：2026-10-11。版本：v0.1 草案。适用：UE 5.5.4。

**状态：仅设计，尚未实现。下文新增类型、字段和接口均为拟议内容，不能按现有编辑器配置使用。**

## 1. 背景与目标

将 Lyra 的 Inventory（物品容器／背包）设计接入 Hodgepodge，让玩家能够持有物品，并从背包选择武器装备。复用现有 Equipment、属性协调器、GAS 和 Experience，不重写已完成的武器表现与能力授予系统。

参考工程：`E:/Project/UProject/study/Epic/LyraStarterGame`。当前项目已有 EquipmentDefinition／Instance／Manager、FastArray 与子对象复制、装备属性、能力撤销、武器表现，以及 GameplayTagStack；Inventory 核心和 QuickBar 尚未迁入。

首版范围：

- 物品定义、实例与 Fragment；服务器添加、移除、查询、数量消耗。
- 装备类物品独立实例，普通物品支持配置堆叠上限。
- 玩家背包复制、变更通知及一个当前武器装备位。
- 默认武器入包、从背包装备、卸装及 Pawn 更换后的重新装备。

暂不包含背包 UI、拾取／掉落交互、仓库／交易、存档、跨地图持久化、多装备位、QuickBar 多槽位、物品使用 GA，以及每武器独立连段／动画体系。普通怪物继续直接使用 Equipment，不强制拥有背包。攻击吸附不在本次范围，翅膀继续保持禁用。

## 2. 核心概念与职责

**ItemDefinition** 是物品类型的静态配置；**ItemInstance** 是玩家实际持有的一件物品或一个堆叠；**EquipmentInstance** 是该物品在当前 Pawn 上生效时的临时运行对象。卸装不删除背包物品，删除物品也不能只销毁武器模型。

拟议模块：

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

首版建议将 InventoryManager 与 EquipmentSelection 放在 PlayerState，通过 Experience 注入；基础配置失效时停止装备操作。PlayerState 未更换的死亡／重生保留持有物品和选择，Pawn 上的装备实例重新创建。组件被移除或 Experience 停用时解绑并撤销本组件管理的装备；跨 Experience 的背包保留策略另行设计。

背包列表、实例数据和选择信息仅向拥有者复制；已装备武器的必要表现仍按现有 Pawn／Equipment 复制给观察者。远端不能依赖私人 ItemInstance 的引用可解析，也不复制整个背包来满足武器显示。

同一类型的两把武器可以拥有不同 ItemId 和等级。堆叠只合并 Definition、等级与可变数据兼容的普通物品；首版拒绝合并带不同实例状态的物品。

## 4. 接口与数据流

拟议的 Inventory 接口：

- `CanAddItemDefinition(Definition, Count)`：只验证，不修改数据。
- `AddItemDefinition(Definition, Count)`：返回结果与受影响物品，按堆叠规则创建／合并。
- `RemoveItemById(ItemId)`：移除整个实例；当前装备物品必须先成功卸装。
- `ConsumeItemsByDefinition(Definition, Count)`：预检总数量，再扣除实际数量；不足时不部分扣除。
- `GetItems()`、`FindItemById(ItemId)`、`GetTotalItemCountByDefinition(Definition)`：只读查询，数量按 StackCount 求和。
- `OnInventoryChanged`：发布添加、数量变化与移除，包含 ItemId、旧／新数量和修订号。

拟议的装备选择接口：

- `CanEquipItem(ItemId)`：验证持有权、Fragment、装备配置、Avatar／ASC 就绪与战斗限制。
- `RequestEquipItem(ItemId)`、`RequestUnequip()`：返回操作结果；已有相同选择且实际装备有效时幂等成功。
- `BindAvatar(Pawn)`、`UnbindAvatar(Pawn)`：按 Pawn 生命周期绑定和撤销，忽略旧 Pawn 的迟到通知。
- `OnEquipmentSelectionChanged`：通知实际结果；UI 后续读取快照并订阅变化。

这些服务器修改接口不等于客户端可以直接调用。客户端经自身 PlayerController 的请求入口发送 ItemId，服务器重新验证所属背包和当前状态；不接受客户端指定的等级、装备定义或其他玩家实例。

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

## 5. 与现有实现的衔接

需要调整的主要位置：

- `HodgeCombatCharacter::InitializeDefaultEquipment`：背包玩家改由选择模块驱动；旧链路保留给怪物及未迁移配置。
- PlayerState 的 `OwnedEquipmentStates` 与 AttributeCoordinator 的装备记录：收敛物品身份和等级的事实来源到 Inventory；旧接口改为兼容适配，不能独立写两份等级。
- `EquipmentManager::EquipItemWithState`：复用现有授予与失败校验；允许从物品取身份与等级，不重新生成 GUID。
- EquipmentInstance 的来源关系：关联 ItemInstance，但远端表现与攻击不能依赖私人来源对象。设置来源的时机须保证需要来源的装备回调能够读取它。
- 武器来源解析与攻击准备：确认只选择当前生效武器。现有 ComboDefinition 仍来自 PawnData，不随物品切换自动替换。
- Experience 与初始化就绪：组件晚到、重复通知、解绑、重生均可安全推进。数据就绪与实际装备成功分开判断；所有新增类留在现有 Hodgepodge Runtime 模块。

首版使用组件委托与完整快照通知，不引入 GameplayMessageRouter、CommonGame 或新的业务模块。服务器修改与客户端复制通知都要覆盖；UI 晚订阅时先读取快照，不能只等待一次事件。

## 6. 必须补齐的 Lyra 示例限制

本地 Lyra 的 AddEntry(ItemInstance) 尚未实现，CanAddItemDefinition 未检查容量／唯一性；总数量按条目计数，ConsumeItemsByDefinition 按整条移除，不能直接作为项目的堆叠实现。

首版只提供按 Definition 创建物品，暂不开放外部 ItemInstance 转移接口；实现正数量、堆叠上限、数量溢出与总数预检，统一移除时的复制子对象注销。容量采用可配置最大条目数，达到上限时不部分入包。已有装备定义和第三方资源不批量重命名或重存。

## 7. 验收与开发顺序

所有验收均待实现后执行，本次文档不表示构建或运行通过。

1. 背包核心：合法添加／合并／移除；非法定义、负数、零、溢出、容量不足拒绝；消耗不足不修改；同类型两把武器身份和等级独立。
2. 装备衔接：默认物品只发放一次；切换后属性正确、旧技能撤销、无重复 Actor；装备失败及恢复失败状态明确；战斗期间请求被拒绝。
3. 生命周期与复制：晚注册、重复回调、换 Pawn 和组件移除正确清理；重生保留物品且不重复装备；拥有者获得正确背包，观察者仅看到装备表现。
4. 回归：常规 Editor／Game 构建，原生测试，单人与 Listen Server 双玩家 PIE；回归已有属性、装备、Combat、敌人初始化及死亡。Cook／独立进程按后续发布阶段验收。

实施顺序为背包核心 → 单武器位与旧身份记录适配 → 生命周期／联机验证；完整 UI、拾取、QuickBar 与存档后置。前述迁移量估计为首版约 6～10 人日，仅供排期参考，接口细化与测试发现的问题可能调整范围。

待评审：PlayerState 归属是否满足后续角色切换；最大条目数／普通物品堆叠默认值；是否确需普通物品消耗首版落地；旧装备记录迁移方案。当前采用上述建议作为草案假设，编码前确认。

参考：[现有装备设计](equipment-weapon-system.md)、[属性与装备加成](character-attribute-growth.md)、[共享战斗请求](combat-action-requests.md)。
