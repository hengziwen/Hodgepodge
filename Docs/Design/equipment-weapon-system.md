# 武器 / 装备系统实施文档

> **文档状态：实施参考，尚未完成实现。** 最近更新 2026-09-29。
> 本文档用于指导实现，不代表功能已经跑通。文中"已落地"仅指源码编译通过与静态核对，"未验证"指尚未执行 PIE、蓝图编译或联机。
>
> 部分一（最小步骤）与部分二（完整容器系统）是**两个独立里程碑**，可以先只做部分一。

**相关文档**

- [项目开发约定](../../AGENTS.md)
- [AI 开发与验证流程](../AI_DEVELOPMENT.md)
- [当前状态、断点与接通顺序](../KnowledgeBase/12-integration-backlog.md)
- [属性、伤害、战斗与死亡](../KnowledgeBase/08-combat-health.md)

**外部参考**

```text
Lyra 源码：E:\Project\UProject\study\Epic\LyraStarterGame
本机引擎：E:\UE\UE_5.5
项目：    e:\Project\Git\Hodgepodge
```

---

## 0. 范围与边界

### 0.1 本文档覆盖

| 部分 | 目标 | 验收标准 |
|---|---|---|
| 一、最小步骤 | 进 PIE 能看到武器挂在角色右手，且能取到武器实例 | 服务器与客户端都能看到武器 Actor；`GetFirstInstanceOfType` 非空 |
| 二、完整容器系统 | 换武器时属性/被动随之变化，外观随之切换 | 装备 → 加成生效；卸下 → 加成撤销、外观销毁；重复装备不重复授予 |

### 0.2 本文档**不**覆盖

- 近战命中判定（角色原点扇形 / 武器 Socket / 手部碰撞体）
- 伤害结算与 `HodgeDamageExecution` 的三个断点
- 命中表现（GameplayCue、停帧、震屏、浮字）
- 背包 / 快捷栏（`InventoryItemInstance`、`QuickBarComponent`）

### 0.3 设计前提（与 Lyra 的关键差异）

Lyra 的武器是**能力载体**：每把武器带 `AbilitySetsToGrant`，装上就多出开火/装弹技能，卸下就撤销。

本项目的武器是**数值与标签的来源**：

- 换武器**不改变**普攻、大招等有表现技能的动作
- 武器最多提供属性加成（攻击力、暴击）和无表现的被动效果（"释放技能后叠一层加成"）
- 普攻、大招由角色（`PawnData`）授予，永久存在

因此：**机制照抄 Lyra，内容语义相反。** `AbilitySetsToGrant` 这个字段照用，但往里放的是被动 GA 与加成 GE，不放带 Montage 的技能。

---

## 1. 现状盘点

### 1.1 已落地（源码编译通过）

| 项 | 位置 | 说明 |
|---|---|---|
| `UHodgeEquipmentDefinition` | `Source/Hodgepodge/Public\|Private/Equipment/HodgeEquipmentDefinition.*` | `Abstract`，需派生蓝图使用 |
| `UHodgeEquipmentInstance` | 同目录 `HodgeEquipmentInstance.*` | `UObject`，承载运行时状态 |
| `UHodgeEquipmentManagerComponent` | 同目录 `HodgeEquipmentManagerComponent.*` | `UPawnComponent`，装备容器 |
| `UHodgeWeaponInstance` | 同目录 `HodgeWeaponInstance.*` | 武器子类；Cosmetics 部分已注释，设备属性保留 |
| CoreRedirects | `Config/DefaultEngine.ini` | 4 条 `Lyra*` → `Hodge*` 重定向 |

**均未挂载、未建资产、未接线。** 目前项目中没有任何代码创建 `UHodgeEquipmentManagerComponent`。

### 1.2 可直接复用的既有地基

| 能力 | 位置 | 状态 |
|---|---|---|
| `UHodgeAbilitySet::GiveToAbilitySystem(ASC, OutHandles, SourceObject)` | `Public/Data/HodgeAbilitySet.h` | ✅ 第三参已支持 |
| `FHodgeAbilitySet_GrantedHandles::TakeFromAbilitySystem(ASC)` | 同上 | ✅ 含 Ability / GE / AttributeSet 三类句柄 |
| `GrantedGameplayEffects` / `GrantedAttributes` | 同上 | ✅ 可用于属性加成 |
| 运行时授予的 `OnSpawn` 能力自动激活 | `Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp` | ✅ `OnGiveAbility` → `TryActivateAbilityOnSpawn` |
| 时间轴 Window / Point + `Status.Attack.Active` | `AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.*` | ✅ 已 PIE 验证 |
| `UHodgeHealthComponent::FindHealthComponent` | `Component/HodgeHealthComponent.h` | ✅ `UHodgeWeaponInstance` 已在用 |

### 1.3 尚未具备（不在本阶段）

- 敌人没有 ASC（`AHodgeEnemyCharacter` 的 `GetAbilitySystemComponent()` 走 `PawnExtComponent`，AI 无 PlayerState 进不去）
- `UHodgeCombatSet` 从未挂到任何 ASC 上
- `HodgeDamageExecution` 的敌我倍率恒 `0.0f`
- 近战扫掠、伤害施加、命中表现

---

# 部分一：最小步骤（武器挂到玩家身上）

**目标**：进 PIE，武器 Actor 出现在角色右手，`EquipmentList` 有 1 条 Entry。
**不做**：判定、伤害、加成、切换。

## 1.1 前置清理（建议先做，非阻塞）

`HodgeWeaponInstance` 里还留着射击残留。它们能编译，但和近战无关，留着会让后面加字段时更难读。

| 残留 | 位置 | 建议 |
|---|---|---|
| `ApplicableDeviceProperties` | `HodgeWeaponInstance.h` | 手柄自适应扳机，删 |
| `ApplyDeviceProperties` / `RemoveDeviceProperties` / `DevicePropertyHandles` | `.h` + `.cpp` | 同上 |
| `UpdateFiringTime` / `GetTimeSinceLastInteractedWith` / `TimeLastFired` | 同上 | 服务"多久没开火播待机动画"，删 |
| `PickBestAnimLayer` | 同上 | 已空实现，删 |
| `OnDeathStarted` + 构造函数里的死亡绑定 | `.cpp` | 只为清理设备属性而存在，随设备属性一起删 |
| `OnEquipped` 里的 `TimeLastEquipped` 赋值 | `.cpp` | 同上 |

删完 `UHodgeWeaponInstance` 应该只剩：构造函数、`OnEquipped` / `OnUnequipped`（留给后面加判定参数与外观钩子）。

> 清理后必须重新执行 Editor 构建，不要在编辑器开着时依赖 Live Coding 通过。

## 1.2 把组件挂到 Pawn

### 硬约束

`UHodgeEquipmentManagerComponent` 继承 `UPawnComponent`，其 `GetPawn()` 是 `Cast<APawn>(GetOwner())`。

**挂到 PlayerState 上的后果**：

- `GetPawn()` 恒返回 `nullptr`
- `SpawnEquipmentActors` 里的 `if (APawn* OwningPawn = GetPawn())` 直接跳过 → **武器永远不生成，且无任何报错**
- 若有人调 `GetPawnChecked()`（例如 `UPawnComponent::GetPlayerState()`）→ **断言崩溃**

**结论：只能挂 Pawn（`ACharacter`）。**

### 三个方案

| 方案 | 步骤 | 优点 | 代价 |
|---|---|---|---|
| **A. 角色蓝图手加** | ① 给 `UHodgeEquipmentManagerComponent` 的 `UCLASS` 加 `meta=(BlueprintSpawnableComponent)` ② 打开 Hero 蓝图 → Add Component | 最快，改完即可手加 | 当前 UCLASS 是 `UCLASS(BlueprintType, Const)`，**没有该 meta，蓝图面板里搜不到**；每个角色蓝图都要加，漏了静默失效 |
| **B. C++ 挂**（推荐） | 在 `AHodgeCombatCharacter` 构造函数 `CreateDefaultSubobject<UHodgeEquipmentManagerComponent>` | 不会漏；Hero 与 Enemy 自动都有；蓝图仍可见 | 需要重编译 |
| **C. GameFeature**（Lyra 原版） | GameFeatureData 里加 `GameFeatureAction_AddComponents`，`ActorClass` = Pawn 类，`ComponentClass` = 装备组件 | 数据驱动，可按 Experience 控制 | 配置链路长，调试绕 |

**Lyra 的实际做法是 C。** 核对方式：在 `Plugins/GameFeatures/ShooterCore/Content/ShooterCore.uasset` 中可探测到 `GameFeatureAction_AddComponents`、`ComponentClass`、`EquipmentManagerComponent` 三个字符串共存。

**建议顺序**：先用 A 验证（记得加 meta），确认武器能挂上；跑通后挪到 B。

### 挂载位置

挂 `AHodgeCombatCharacter`（Hero 与 Enemy 的公共基类）还是只挂 `AHodgeHeroCharacter`，取决于敌人是否也要拿武器。第一版建议只挂 Hero，避免把敌人 ASC 的问题提前引进来。

## 1.3 建武器 Actor 蓝图

```
AActor 蓝图（命名建议 B_Weapon_Sword，放 Content/Main/Weapons/）
└── StaticMeshComponent          ← 设为 Root Component
       Static Mesh = SM_Sword
       Collision Enabled = No Collision     ← 必须关
       Component Tick = 关闭
```

要点：

- **必须勾选 `Replicates`**（Class Defaults → Replication → Replicates）。`AActor::bReplicates` 默认是 `false`（`Actor.cpp:197`），而 `UHodgeEquipmentInstance::SpawnEquipmentActors` 里**没有任何 `SetReplicates` 调用**。不勾的话武器只在服务器上存在，客户端看不到，且客户端的 `SpawnedActors` 会是空引用。
- **必须关闭碰撞**。否则武器会挡住角色移动，也会污染后续近战扫掠的 Trace 结果。
- **里面不要放逻辑**。这是 Lyra 的设计：武器 Actor 是纯表现，逻辑在 `UHodgeWeaponInstance` 上。
- 如果需要"武器 Socket"判定（部分三再展开），在 StaticMesh 资源上（或骨骼上）加插槽，例如 `weapon_tip` / `weapon_base`。**现在加好，后面不用返工。**
- 如果手上还要挂刀鞘、灯笼等配件，可以建**多个** Actor，`ActorsToSpawn` 支持数组。

## 1.4 建装备定义与武器实例蓝图

### 1.4.1 武器实例蓝图（可选）

`UHodgeWeaponInstance` 的蓝图子类（命名建议 `BP_WeaponInstance_Sword`）。

第一版可以**不建**——`InstanceType` 留空时 `AddEntry` 会回退到 `UHodgeEquipmentInstance::StaticClass()`，武器一样能挂上。等要往武器上放判定参数时再建。

### 1.4.2 装备定义蓝图（必须）

`UHodgeEquipmentDefinition` 是 `UCLASS(..., Abstract, ...)`，**不能直接实例化，必须派生蓝图**。

命名建议 `DA_Equipment_Sword`，放 `Content/Main/Equipments/`。

配置三个字段：

| 字段 | 填什么 | 说明 |
|---|---|---|
| `InstanceType` | `BP_WeaponInstance_Sword`，或留空 | 指向武器实例类 |
| `AbilitySetsToGrant` | 第一版留空 | 部分是后面的事 |
| `ActorsToSpawn` | 一条 Entry | 见下 |

`ActorsToSpawn` 单条 Entry 的三个字段（来自 `FHodgeEquipmentActorToSpawn`）：

| 字段 | 值 | 说明 |
|---|---|---|
| `ActorToSpawn` | `B_Weapon_Sword` | 1.3 建的 Actor 蓝图 |
| `AttachSocket` | 角色右手武器插槽名 | **最容易翻车的一项，见下** |
| `AttachTransform` | 相对插槽的偏移/旋转 | 握持角度不对就调这里 |

### ⚠️ `AttachSocket` 是本步骤最大的坑

`UHodgeEquipmentInstance::SpawnEquipmentActors` 里**没有任何插槽校验**。插槽名写错时：

- `AttachToComponent` 返回 false，但**不报错、不打日志**
- Actor 会停在角色 Mesh 原点（通常在脚底或胯下）
- 现象看起来像"武器挂错了地方"，很难联想到是插槽名问题

**确定插槽名的办法**：打开角色骨骼网格资源 → Skeleton Tree 里找右手武器插槽；或打开角色蓝图 → 选中 Mesh → Details → 查看 Sockets。Lyra 用的是 `hand_r_weapon` 这类命名，你的骨架可能不同。

**排查技巧**：如果武器位置不对，先在角色蓝图上临时打印 `GetMesh()->DoesSocketExist("你的插槽名")`，比猜快。

## 1.5 调用 `EquipItem`

### API

```cpp
UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
UHodgeEquipmentInstance* EquipItem(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition);
```

两个注意点：

1. 参数是 **`TSubclassOf`（类）**，不是资产实例——因为 `EquipmentDefinition` 是 Abstract UObject
2. 带 **`BlueprintAuthorityOnly`**，**只有服务器能调**，客户端调用会被静默忽略

### 时序是第二个大坑

`FHodgeEquipmentList::AddEntry` 内部执行顺序：

```
1. NewObject<UHodgeEquipmentInstance>(Pawn, InstanceType)      ← 总是执行
2. AbilitySetsToGrant → GiveToAbilitySystem(ASC, &Handles, 实例) ← 需要 ASC 有效
3. SpawnEquipmentActors(ActorsToSpawn)                          ← 总是执行
4. MarkItemDirty(Entry)
```

第 2 步取 ASC 的方式是 `UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn)`。若此时 ASC 尚未初始化：

- 走 `else` 分支（原 Lyra 代码那里只有一句 `//@TODO: Warning logging?`，**不报错**）
- **结果是：武器外观正常挂上，但武器授予的能力与加成一个都没生效**

这个组合症状（"外观对了但能力没生效"）非常难排查，务必让 `EquipItem` 发生在 ASC 就绪之后。

### 各调用时机是否可用

| 时机 | 是否可用 | 原因 |
|---|---|---|
| 角色蓝图 `BeginPlay` | ❌ | 玩家的 ASC 要等 `PawnExtension` 到达 `DataInitialized` |
| `AHodgeCombatCharacter::OnAbilitySystemInitialized()` | ✅ 推荐 | `PawnExtComponent` 的初始化委托会回调它 |
| `UHodgeHeroComponent` 处理完 `DataInitialized` 之后 | ✅ 可以 | 同上，时机等价 |
| `AHodgePlayerState::SetPawnData` 之后 | ⚠️ | Pawn 可能还没生成，`GetPawn()` 会失败 |

**推荐**：在 `AHodgeCombatCharacter::OnAbilitySystemInitialized()` 末尾调 `EquipItem`，武器资产配在角色上（`TSubclassOf` 型 `EditDefaultsOnly` 属性）。

## 1.6 验证清单

按顺序卡，第 4 条专门抓插槽名问题。

| # | 检查 | 期望 |
|---|---|---|
| 1 | 服务器上 `EquipmentList.Entries.Num()` | = 1 |
| 2 | `SpawnedActors.Num()` | = 1 |
| 3 | 武器 Actor 的 `GetAttachParentActor()` | = 角色 |
| 4 | 武器 Actor 的**实际世界位置** | 在手上，不在脚底 / 原点 |
| 5 | PIE Listen Server 客户端 | 客户端也能看到武器 |
| 6 | `GetFirstInstanceOfType<UHodgeWeaponInstance>()` | 非空 |

第 1、2 条可在 `EquipItem` 返回值上断点，或加临时日志。

---

# 部分二：完整武器容器系统

**目标**：换武器 → 属性/被动变化 + 外观切换，且可重复、可撤销、可联网。

## 2.1 三类职责边界

| 类 | 类型 | 职责 | 生命周期 |
|---|---|---|---|
| `UHodgeEquipmentDefinition` | `UObject`（`Abstract`） | **配置**：实例类、要授予的能力/GE、要生成的 Actor | 静态资产，运行期只读 |
| `UHodgeEquipmentInstance` | `UObject` | **运行时状态**：`Instigator`、`SpawnedActors`、`OnEquipped`/`OnUnequipped` | 随装备/卸下创建与销毁 |
| `UHodgeEquipmentManagerComponent` | `UPawnComponent` | **容器**：持有列表、授予/撤销、查询 | 随 Pawn |

**不要**把运行时状态放进 Definition，也不要把配置写进 Instance。

## 2.2 武器实例的数据字段

`UHodgeWeaponInstance` 是承载"这把武器有什么"的地方。建议分两组。

### 组 A：判定参数（为后续近战判定预留）

| 字段（建议名） | 类型 | 用途 |
|---|---|---|
| `bPreferWeaponSocket` | `bool` | 优先用武器 Socket 还是角色原点。大剑配 false，匕首配 true |
| `TraceSocketName` | `FName` | 如 `weapon_tip`；仅 `bPreferWeaponSocket` 为 true 时使用 |
| `TraceDistance` | `float` | 向前最大距离（cm） |
| `TraceRadius` | `float` | 扫掠球半径，代表刀身宽容度 |
| `HalfAngleDegrees` | `float` | 扇形半角（度） |
| `HeightOffset` | `float` | 角色原点抬高；Socket 不可用时的回退参数 |
| `bRequireLineOfSight` | `bool` | 是否要求无遮挡 |

**为什么参数放武器而不是技能**：大剑与匕首的判定范围不同，但用的是同一个普攻技能。放武器上 = 换武器即换手感，技能零改动。

### 组 B：被动的数值与标签

| 字段（建议名） | 类型 | 用途 |
|---|---|---|
| `WeaponLevel` | `int32` | 供加成 GE 的 Level 或 SetByCaller 使用 |
| `DamageBonusTag` | `FGameplayTag` | 伤害类型标签（项目已有 `GameplayEffect.DamageType.Melee`） |
| 武器自身属性 | — | 若走属性路线，可加一个武器专用 AttributeSet |

**第一版建议只做组 A**，组 B 等到"加成怎么算"定下来再加。

## 2.3 属性加成与被动能力

这是本项目与 Lyra 分道扬镳的地方，但**机制完全复用**。

### 两条通道

| 通道 | 用哪个字段 | 例子 | 有表现吗 |
|---|---|---|---|
| **属性加成** | `AbilitySetsToGrant` 里的 `GrantedGameplayEffects`（Infinite GE） | +50 攻击力、+10% 暴击 | 无 |
| **被动效果** | `AbilitySetsToGrant` 里的 `GrantedGameplayAbilities`（`OnSpawn` 策略的无表现 GA） | "每次释放技能后叠 1 层攻击加成" | 无（内部逻辑） |

两者都由 `AddEntry` 里的这一句统一授予、由 `RemoveEntry` 里的 `TakeFromAbilitySystem` 统一撤销：

```cpp
AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
```

### 已有保障

- `FHodgeAbilitySet_GrantedHandles` 同时保存 Ability / GameplayEffect / AttributeSet 三类句柄，撤销是完整的
- `UHodgeGameplayAbility::OnGiveAbility` 会调 `TryActivateAbilityOnSpawn`，所以**换武器时新授予的 `OnSpawn` 被动会立即激活**，旧的在 `TakeFromAbilitySystem` 时被撤销

### 设计规则（建议用 `IsDataValid` 钉死）

| 规则 | 理由 |
|---|---|
| 武器的 `GrantedGameplayAbilities` 只允许 `ActivationPolicy == OnSpawn` 或纯事件驱动 | 排除"按键触发有表现技能" |
| 武器的 GA 禁止配 Montage / `AttackSteps` | 动作由角色决定 |
| 角色的主动技能一律由 `PawnData->AbilitySets` 授予，**永不**放进武器 | 换武器不改变操作 |
| 武器要影响主动技能伤害，只走"属性加成"或"判定参数"，**不换技能** | 数值变化，动作不变 |

第 1、2 条建议在 `UHodgeEquipmentDefinition::IsDataValid` 里做编辑器校验——用报错把设计规则钉死，比写在文档里管用。

## 2.4 卸下与切换

### 卸下

`UnequipItem` 的执行顺序：

```
1. RemoveReplicatedSubObject(实例)          ← 注册式复制路径
2. 实例->OnUnequipped()                      ← 蓝图钩子 K2_OnUnequipped
3. EquipmentList.RemoveEntry(实例)
     ├─ GrantedHandles.TakeFromAbilitySystem(ASC)   ← 撤销能力与加成
     ├─ 实例->DestroyEquipmentActors()              ← 销毁武器 Actor
     ├─ RemoveCurrent()
     └─ MarkArrayDirty()
```

### 切换武器

**必须先卸后装**，不要直接 `EquipItem` 新的然后期望旧的自动消失：

```
现在武器 = GetFirstInstanceOfType<UHodgeWeaponInstance>()
if (现在武器) UnequipItem(现在武器)
EquipItem(新武器定义)
```

`EquipmentList` 是数组，允许同时装备多件（武器 + 饰品 + 防具）。如果做"同时只能有一把武器"，这个约束要自己加在调用侧，容器不管。

### 重复装备

当前 `AddEntry` **没有去重**。同一个 `EquipmentDefinition` 调两次 `EquipItem` 会生成两个 Entry、两个实例、两套授予。

如果属性加成是 `Additive` 的 Infinite GE，会造成**加成翻倍**。需要在调用侧或 `EquipItem` 里加去重判断。

## 2.5 网络复制要点

### 两套子对象复制路径（重要）

`EquipItem` 里有一段条件：

```cpp
if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
{
    AddReplicatedSubObject(Result);
}
```

同时组件还实现了 `ReplicateSubobjects`（老的通道式路径）。

**引擎侧的事实**（已核对 UE 5.5 源码）：

| 事实 | 位置 |
|---|---|
| `IsUsingRegisteredSubObjectList()` 直接返回 `bReplicateUsingRegisteredSubObjectList` | `ActorComponent.h:531` |
| 该标志的初值不是硬编码，而是 `GDefaultUseSubObjectReplicationList` | `ActorComponent.cpp:469`（组件）、`Actor.cpp:200`（Actor） |
| `GDefaultUseSubObjectReplicationList` **默认为 `false`** | `ActorComponent.cpp:99`，可由 CVar `net.SubObjects.DefaultUseSubObjectReplicationList` 覆盖 |
| **`AActor` 上也有同名标志，且两者必须同时为 true** 才走注册式路径 | `ActorComponent.h:529` 注释："the owning actor of this component must also have its `bReplicateUsingRegisteredSubObjectList` flag set to true" |
| `AActor` 的那个标志是 `UPROPERTY(Config, EditDefaultsOnly)`，**可在蓝图/编辑器里改，也能被 ini 覆盖** | `Actor.h:553` |
| 启用 Iris 时**强制要求为 true**，否则 `ensure` 失败 | `EngineReplicationBridge.cpp:137` |

**所以当前的实际行为**：默认 `false` → 走 `ReplicateSubobjects` 那条老路。前提是 `AActor` 侧的标志也是 `false`（默认如此）。

**三种情形要分清**：

| 情形 | 组件标志 | Actor 标志 | 结果 |
|---|---|---|---|
| 默认（非 Iris） | false | false | 走 `ReplicateSubobjects`，**当前就是这条** |
| 只改组件 | true | false | ⚠️ 注册式不生效，`ReplicateSubobjects` 也不被调用 → **实例不上客户端，且不报错** |
| 都改 / 开 Iris | true | true | 走注册式；`ReplicateSubobjects` 失效但**不报错** |

中间那一行是最危险的组合。要切路径必须**同时**处理 Actor 侧（蓝图勾选或 `[SystemSettings] net.SubObjects.DefaultUseSubObjectReplicationList=1`）。

**排查顺序**：若"服务器正常、客户端收不到装备实例"，先确认这两个标志是否一致，再看 `AddReplicatedSubObject` 是否真的被调用（`IsReadyForReplication()` 为 false 时也会跳过）。

### 客户端回调

`FHodgeEquipmentList` 的回调负责在客户端补齐表现：

| 回调 | 时机 | 做的事 |
|---|---|---|
| `PostReplicatedAdd` | 客户端收到新 Entry | `Instance->OnEquipped()` |
| `PreReplicatedRemove` | 客户端收到移除 | `Instance->OnUnequipped()` |
| `PostReplicatedChange` | Entry 有字段变化 | 当前是空的（Lyra 里也全注释了） |

**注意**：客户端只跑 `OnEquipped` / `OnUnequipped`，**不跑 `SpawnEquipmentActors`**——武器 Actor 是独立复制的 Actor，客户端由引擎自动生成并复制过来。所以客户端上 `OnEquipped` 的时机可能早于武器 Actor 生成完毕。如果客户端表现依赖武器 Actor，要判空。

### Authority 边界

| 操作 | 只允许服务器 |
|---|---|
| `EquipItem` / `UnequipItem` | ✅ `BlueprintAuthorityOnly` |
| `AddEntry` | ✅ 内部 `check(HasAuthority())` |
| `GiveToAbilitySystem` | ✅ 内部 `IsOwnerActorAuthoritative()` 检查 |
| 读 `GetFirstInstanceOfType` | 两端都可以 |
| 读 `SpawnedActors` | 两端都可以（客户端靠复制） |

## 2.6 敌人侧（可选）

敌人要用武器，前置条件是**敌人得有 ASC**——这是当前项目的一个已知缺口（`AHodgeEnemyCharacter::GetAbilitySystemComponent()` 走 `PawnExtComponent`，AI 没有 PlayerState 与 PawnData，链路进不去）。

两个方向：

| 方向 | 做法 | 代价 |
|---|---|---|
| 敌人自持 ASC | `AHodgeEnemyCharacter` 上建 `UHodgeAbilitySystemComponent` + AttributeSet，覆写 `GetAbilitySystemComponent()`，`BeginPlay` 里 `InitAbilityActorInfo(this, this)` | 需要处理与 `PawnExtComponent` 初始化链的共存（该组件因缺 PawnData 会停在 `Spawned`，不冲突，但需验证） |
| 照 Lyra 用 Bot + PlayerState | 引入 BotController、PlayerState、PawnData | 重，且与"近战 A-RPG"的常见架构不符 |

**建议**：第一版只让玩家用装备系统；敌人等伤害闭环验证完再单独做。

---

# 部分三：坑清单

按"踩到的概率 × 排查难度"排序。

| # | 坑 | 症状 | 预防 |
|---|---|---|---|
| 1 | `AttachSocket` 写错 | 武器落在角色脚底/原点，**无任何日志** | 1.4 节的插槽名确认；加临时日志 |
| 2 | 武器 Actor 蓝图没勾 `Replicates` | 服务器上正常，**客户端完全看不到武器**；客户端的 `SpawnedActors` 为空 | 1.3 节 |
| 3 | `EquipItem` 早于 ASC 就绪 | 武器外观正常，但能力与加成**一个都没生效**，无报错 | 放在 `OnAbilitySystemInitialized` |
| 4 | 组件与 Actor 两个 `bReplicateUsingRegisteredSubObjectList` 不一致 | 实例不上客户端，**无报错** | 2.5 节的三情形表 |
| 5 | `NewObject` 的 outer 不是 Pawn | `GetWorld()` 返回 null → `SpawnActor` 崩溃 / `OnEquipped` 里的 `check(World)` 失败 | 照抄现有实现，别改 outer |
| 6 | `MarkItemDirty` 漏了 | 服务器正常，客户端永远收不到 | 不改 `AddEntry` 尾部 |
| 7 | 组件挂到 PlayerState | `GetPawn()` 恒 null → 武器不生成；或 `GetPawnChecked` 断言崩溃 | 1.2 节的硬约束 |
| 8 | 重复 `EquipItem` 同一把武器 | 加成翻倍（Additive Infinite GE 叠加） | 调用侧去重 |
| 9 | 武器 Actor 没关碰撞 | 挡住角色移动、污染后续扫掠 Trace | 1.3 节 |
| 10 | `TStructOpsTypeTraits` 漏特化 | FastArray 编译通过但复制静默失败 | 不动 `FHodgeEquipmentList` 的特化 |
| 11 | `UninitializeComponent` 里边迭代边删 | 崩溃 | 现有实现已先拷贝再删，别改 |

---

# 部分四：待决策项

实现前需要定下来的。

| # | 决策 | 影响 |
|---|---|---|
| 1 | 组件挂载方式（蓝图 A / C++ B / GameFeature C） | 决定 1.2 怎么做；影响是否要给 UCLASS 加 `BlueprintSpawnableComponent` |
| 2 | 挂 `AHodgeCombatCharacter` 还是只挂 `AHodgeHeroCharacter` | 决定敌人是否要装备 |
| 3 | `EquipItem` 的触发点（`OnAbilitySystemInitialized` / PawnData / 蓝图） | 决定 1.5 怎么接 |
| 4 | 武器实例的字段最终形态（组 A / 组 B） | 决定 2.2 加什么 |
| 5 | 属性加成走 `GrantedGameplayEffects` 还是自定义 AttributeSet | 决定 2.3 的资产配置方式 |
| 6 | "同时只能有一把武器"这个约束放哪一层 | 决定 2.4 的切换逻辑 |
| 7 | 武器是否需要有表现技能（你目前判断是"不需要"） | 若需要，本项目的定位需重新评估 |

---

# 附：与后续里程碑的接口

部分一、二完成后，接近战判定时**不需要改动本系统的任何代码**，只需要：

| 后续要做的事 | 从本系统取什么 |
|---|---|
| 近战扇形扫掠 | `GetFirstInstanceOfType<UHodgeWeaponInstance>()` → 读组 A 的判定参数 |
| 武器 Socket 判定 | 同上 → 读 `TraceSocketName`，再从 `Instance->GetSpawnedActors()` 取武器 Actor 的插槽世界变换 |
| 取武器模型位置给表现层 | `SpawnedActors`（注意：Lyra 里这个数组没有任何逻辑消费方，本项目会是第一个使用者） |
| 伤害数值来源 | 武器的组 B 字段 + `SetByCaller.Damage` |

**注意**：武器 Socket 判定这条路 Lyra 没有实现（`GetWeaponTargetingSourceLocation` 里留了 `//@TODO`，实际返回 Pawn 位置），没有可直接参照的代码，需要自己设计原点求解与回退链。
