# UE5 开放世界动作 RPG 架构方案 V2

## 方案修订背景

原方案存在以下问题，V2 版做了针对性调整：

1. **BaseCharacter 塞太多东西** — ASC、Combat、Inventory、Interaction 不应全部放 BaseCharacter，需拆分角色层级

2. **StateComponent 冗余** — 使用 GAS 就不该再造状态系统，战斗状态优先 GameplayTag

3. **Dedicated Server 理解滞后** — 不能等单机完成再联机，从第一行代码就按 Server Authority 写

4. **ALS 定位偏差** — ALS 只是 Movement \+ Animation \+ Camera 方案，应吸收而非依赖其蓝图结构

5. **后端过早设计** — 当前目标是 UE Dedicated Server，LoginServer / Database 属于后期 MMO 架构

> **核心变化：从"功能模块划分"变成了"职责和生命周期划分"。**
> 
> 

---

## 一、项目目标重新定义

### 最终目标

```
UE5 Client
+
UE Dedicated Server
+
GAS Gameplay Framework
+
ALS Movement Framework
+
Data Driven Content System
+
Future Backend Support
```

### 方向参考

- 鸣潮

- 原神

- 崩坏：星穹铁道（战斗部分）

- MMO Action RPG

> 开发阶段目标不是做完整游戏，而是搭好可扩展的底层框架。
> 
> 

---

## 二、总体架构

```
Player Client
                     |
                    UE5
                     |
        --------------------------------
        |        Gameplay Layer        |
        |  Character / GAS / Combat    |
        |  Interaction / Inventory     |
        --------------------------------
        |      Presentation Layer      |
        |  Animation / Niagara / Camera|
        |  UI                          |
        --------------------------------
        |        Network Layer         |
        |  Replication / RPC / Prediction
        --------------------------------
        |          Data Layer          |
        |  DataTable / DataAsset       |
        --------------------------------
                     |
              Dedicated Server
                     |
              Backend Server
                     |
                 Database
```

---

## 三、代码模块设计

大型项目按**职责**划分，而非简单分类：

```
Source/OpenWorldRPG
│
├── Core              # GameInstance / GameMode / GameState
│                     # PlayerController / PlayerState
├── Characters        # Base / Combat / Player / Monster / NPC
├── Components        # Base / Combat / Interaction / Inventory / Weapon
├── AbilitySystem     # ASC / AttributeSet / Ability / Effect / Tags
├── Input             # InputComponent / InputConfig
├── Animation         # AnimInstance / Notify
├── AI                # Controller / BehaviorTree / EQS
├── World             # Streaming / Interaction
├── Data              # CharacterData / ItemData / SkillData
├── UI
├── Network
└── Utils
```

---

## 四、核心类关系

这是 V2 方案最重要的改动。

### 角色继承层级

```
ACharacter
    ↓
AOWBaseCharacter
    ↓
┌─────────────────────┬──────────────────┐
│                     │                  │
AOWCombatCharacter   AOWNPCCharacter
    │
┌─────────┬──────────────┐
│         │              │
AOWPlayerCharacter  AOWMonsterCharacter
```

> **为什么这样拆？** 玩家和怪物能力差异大，战斗单位才有 GAS / Combat / Weapon，NPC 等非战斗单位不需要这些。
> 
> 

---

## 五、各角色职责

### 1\. AOWBaseCharacter

所有角色共有，只放最基础的东西：

**身体：**

- Mesh

- Skeleton

- Animation

**基础移动：**

- MovementComponent

**公共接口：**

```cpp
GetCharacterName()
GetTeamID()
GetLevel()
```

**不放：**

- ❌ GAS

- ❌ Combat

- ❌ Inventory

- ❌ Interaction

### 2\. AOWCombatCharacter

所有战斗单位（玩家、怪物）的共同基类。

**拥有组件：**

- AbilitySystemComponent

- AttributeSet

- CombatComponent

- WeaponComponent

### 3\. AOWPlayerCharacter

玩家专属：

- Camera

- SpringArm

- Input

- Inventory

- InteractionScanner

### 4\. AOWMonsterCharacter

怪物专属：

- AIController

- LootComponent

- CombatComponent

- ASC

---

## 六、组件设计

### 基类组件

**OWActorComponentBase** 只提供基础能力，不放业务逻辑：

```cpp
GetOwnerActor()
HasAuthority()
GetASC()
```

### 组件继承结构

```
UActorComponent
      ↓
OWActorComponentBase
      ↓
┌─────────────────┬───────────────────┬──────────────────┬───────────────┐
CombatComponent   InventoryComponent  InteractionComponent  WeaponComponent
```

---

## 七、输入系统设计

采用 **Enhanced Input \+ GAS**

### 数据流

```
InputAction
    ↓
HodgeInputComponent
    ↓
PlayerController
    ↓
Character
    ↓
GAS
```

### 职责划分

**InputComponent — 绑定**

- IA\_Attack

- IA\_Jump

- IA\_Dodge

**PlayerController — 输入入口**

```cpp
Move()
Look()
AbilityInputPressed()
```

**Character — 执行**

---

## 八、GAS 架构

GAS 是整个系统的核心。

```
Character
    ↓
AbilitySystemComponent
    ↓
┌──────────────┬──────────────────┬──────────────────┬──────────────┐
AttributeSet   GameplayAbility    GameplayEffect     GameplayTag
```

---

## 九、ASC 初始化设计

### 推荐：PlayerState 持有 ASC

**原因：** 多人游戏标准模式。角色死亡后 Character 销毁，但玩家数据还在 PlayerState 里。

### 初始化流程（服务器）

```
Spawn Character
    ↓
Possessed By
    ↓
PlayerState 初始化 ASC
    ↓
ASC InitAbilityActorInfo
    ↓
Give Ability
    ↓
Apply Default Effect
```

### 结构关系

```
PlayerState          Character
    ↓                  ↓
AbilitySystemComponent  AvatarActor
    ↓
AttributeSet
```

---

## 十、GameplayTag 规划

**统一用 GameplayTag 管理状态，不再单独造 StateComponent。**

### State（状态）

```
State.Alive
State.Dead
State.Attacking
State.Casting
State.Stunned
State.Invincible
```

### Ability（技能）

```
Ability.Attack.Light
Ability.Attack.Heavy
Ability.Skill.One
Ability.Skill.Ultimate
```

### Weapon（武器）

```
Weapon.Sword
Weapon.Bow
Weapon.Spear
```

### Effect（效果）

```
Effect.Buff.Attack
Effect.Debuff.Burn
```

---

## 十一、Combat 系统设计

> **CombatComponent 不是技能系统，技能归 GAS 管。**
> 
> 

### CombatComponent 职责

**武器管理：**

- CurrentWeapon

**Hit 检测：**

```
Animation Notify
    ↓
CombatComponent
    ↓
Trace
    ↓
Hit Actor
    ↓
Apply GameplayEffect
```

**连招管理：**

- 保存 ComboIndex

- 例如：Attack1 → Attack2 → Attack3

---

## 十二、网络架构

**所有系统默认服务器权威（Server Authority）。**

### 输入流程

```
客户端：Attack Input
    ↓
TryActivateAbility
    ↓
Server 验证并执行
```

### 伤害处理

**错误写法：**

```cpp
HP -= 50;
```

**正确流程：**

```
Server
    ↓
GameplayEffect
    ↓
AttributeSet
    ↓
Replication（自动同步到客户端）
```

---

## 十三、Dedicated Server 开发路线

> 现在不要买服务器，用本地电脑即可。
> 
> 

### 本地测试结构

```
Editor
    ↓
Dedicated Server.exe
    ↙        ↘
Client1    Client2
```

### 测试阶段

**第一阶段 — 两个玩家：**

- Spawn

- 移动

- 动画同步

**第二阶段 — 战斗：**

- 技能同步

- 伤害同步

- 死亡同步

---

## 十四、开发阶段规划

### Phase 0：工程骨架（1 周）

**完成 C\+\+ 类：**

- GameInstance / GameMode / GameState

- PlayerController / PlayerState

- BaseCharacter / CombatCharacter / PlayerCharacter

**组件：**

- BaseComponent / CombatComponent

**目标：** 启动正常

### Phase 1：ALS 迁移（2 周）

**目标：** 玩家可以移动

**完成：**

- ALS Character 迁移

- AnimInstance 迁移

- Camera 迁移

- Enhanced Input 接入

**暂不做：** 技能

### Phase 2：GAS 基础（3 周）

**完成：**

- ASC 初始化

- AttributeSet

- GameplayAbility

**实现技能：**

- Q 技能

- 普通攻击

- 闪避

### Phase 3：联机框架（3 周）

**完成：** Dedicated Server

**测试：** Client A \+ Client B

**实现：**

- 移动同步

- Ability 同步

- Attribute 同步

### Phase 4：Combat 系统（1\~2 个月）

**攻击流程：**

```
Input → GA → Montage → Notify → Hit检测 → GE Damage → Death
```

### Phase 5：AI 系统

**怪物组成：** MonsterCharacter \+ AIController \+ BehaviorTree \+ GAS

**实现：**

- 感知

- 追击

- 攻击

- 死亡

### Phase 6：数据驱动

**所有配置走 DataTable。**

**Skill 数据：**

- SkillID / Damage / Cooldown / Montage / Effect

**Character 数据：**

- ID / Health / Attack / Defense

### Phase 7：背包装备

服务器保存

- InventoryComponent

- EquipmentComponent

### Phase 8：开放世界

**最后做。**

加入：

- World Partition

- Data Layer

- HLOD

- Level Streaming

### Phase 9：Backend

**最后做。**

```
LoginServer → GameServer → Database + Redis
```

---

## 十五、当前行动建议

**已完成：**

- ✅ C\+\+ 工程

- ✅ 基础类

- ✅ InputComponent

- ✅ BaseComponent

### 下一步顺序

**1\. 调整角色继承**

```
BaseCharacter → CombatCharacter → PlayerCharacter
```

**2\. 完善输入链路**

```
PlayerController → InputComponent
```

**3\. 接入 ALS**

- 让 PlayerCharacter 可以移动

**4\. 接入 GAS**

- 完成 ASC 初始化

- AttributeSet

- GameplayTag

**5\. 做第一个技能**

---

> 这套架构的核心思路：你现在不是在学 UE API，而是在学习大型游戏工程架构。遇到的"为什么这个类要存在"的问题，恰恰是最应该解决的问题——方向对了，后续才不会推倒重来。
> 
> 

