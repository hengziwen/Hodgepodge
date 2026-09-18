# Timeline 与 PlayTimeline AbilityTask：第一阶段设计（统一事件模型）

> **文档状态：设计草案，尚未实现；数据模型已定型（经五轮评审）。** 最近更新 2026-09-18。
> 本文件覆盖 `UHodgeAbilityTimeline` 与消费它的 `UHodgeAbilityTask_PlayTimeline`。
> 统一事件模型（单一 `Events[]`，`Kind = Window / Point`）**已定稿，不再回到 `Phases[] + Events[]`**。
> 文中所有类型都还**不存在于工作区**：工作区里另有一版按 `Phase + Event` 双数组写的实现，**待按本文件改写**
> （逐项 delta 见 [§9 对现有实现的影响](#9-对现有实现的影响)）。
> 未编译、未 PIE；不得把本文件描述成"已实现 / 已验证"。

**相关文档**

- 母文档：[AbilityTimeline 设计方案](ability-timeline.md)（全量设计草案，含连击 / Montage / 网络 / CombatComponent）
- [GAS、AbilitySet 与技能生命周期](../KnowledgeBase/07-gas.md)
- [当前状态、断点与接通顺序](../KnowledgeBase/12-integration-backlog.md)
- [AI 开发与验证流程](../AI_DEVELOPMENT.md)
- [项目开发约定](../../AGENTS.md)

---

## 0. 本文件的范围

### 0.1 本阶段做

```text
UHodgeAbilityTimeline（数据资产）
    Duration
    Events[]                        ← 唯一的时间轴数组
        Kind = Window              Timeline Window（对应动画里的 AnimNotifyState）
            EventID / Priority
            StartTime / EndTime
            WindowTag / WindowEffectClass
        Kind = Point               Timeline Point Event（对应动画里的 AnimNotify）
            EventID / Priority
            StartTime
            PointEventTag / PointEffectClass
            NetPolicy
            ↓
UHodgeAbilityTask_PlayTimeline（运行时驱动）
    PlayTimeline() / Activate()
    InitializeTimeline()            ← 起点状态 + 起点事件
    TickTask() → AdvanceTimeline()
        └── 单一时间节点序列：WindowEnd → WindowBegin → Point
    StopTimeline(Reason) → 清理（tag + GE）→ 广播 → EndTask()
    OnDestroy() → 清理（兜底）
```

外加：编辑期校验（`IsDataValid` / `PostEditChangeProperty`）、一组标签、一条测试 Timeline、一个最小消费方 GA。

### 0.2 本阶段不做

| 不做 | 为什么不是必要条件 |
|---|---|
| Montage / `MontageSection` / `PlayMontageAndWait` | 本阶段只做"时间 → 语义信号"。不播动画时反而没有"两套时钟漂移"问题，**更容易验证本层算法是否正确** |
| `AttackID` 载荷 | 载荷身份是"多条 Timeline 并行"才需要解决的问题。单条 Timeline 时 `OptionalObject` 已足够 |
| ComboSet / `Transitions` / 连击段数 | 属于"节点之间怎么衔接"，与本层的"节点内部时间结构"是两件事 |
| `UHodgeCombatComponent` / `NextAttackID` | 同上 |
| 输入闭环（`WaitInputPress`） | 本阶段不消费连击输入 |
| 攻击段执行身份（Execution / Serial） | 只在"旧段回调误伤新段"的场景才需要；本阶段每段独立起停，无段间重叠 |
| Bundle / 预加载 | 本阶段不引用大体积资产（无 Montage） |
| 循环窗口 / 变速 / HitStop | 会让时间语义复杂一倍，等真有需求再加 |
| 联机验收 | `NetPolicy` 的分派代码会写，但"listen server + 2 人客户端"的验证不在本阶段 |

### 0.3 为什么改成"统一事件模型"

这一节是本文件的立论依据，不是术语调整。

**Timeline 的本质，是把动画资产里的逻辑信息搬出来：**

| 动画侧 | 语义 | Timeline 侧 |
|---|---|---|
| `AnimNotifyState`（区间） | 一段时间内持续生效 | **Timeline Window**：`[StartTime, EndTime)` 内自动加/减 `WindowTag`，并按需施加 `WindowEffectClass`（Infinite） |
| `AnimNotify`（瞬时） | 某一刻触发一次 | **Timeline Point Event**：`StartTime` 时刻派发一次 `PointEventTag`（GameplayEvent），并按需施加 `PointEffectClass` |

**双数组方案（`Phases` + `Events`）的两个实际问题：**

1. **作者看到的不是一条时间轴，而是两张表。** 想在 0.35s 处插一个事件，必须同时核对"它落在哪个 Phase 里""和哪个 Phase 的边界撞了"。更糟的是两张表的**相对顺序不可见**——作者得在脑子里手工归并，才能知道 0.30 处到底谁先执行。
   > ⚠️ 但要分清因果：**"同刻顺序需要定义"这件事与数组怎么放无关。** 只要一条时间轴同时含区间与时刻，就必须回答"0.30 谁先执行"，
   > 换成单一数组后这条规则（`WindowEnd → WindowBegin → Point`）依然存在，见 §3.4.2。
   > 所以统一数组的收益**落在这条规则之外**，见下面四项。
2. **一个区间只能表达"一个标签"或"一个标签 + 若干额外标签"，无法表达"这段区间同时驱动一个状态标签和一个游戏效果"。** 而 `AnimNotifyState` 天然就是"这段时间里我既有状态、又有附加效果"。旧版曾用 `AdditionalGrantedTags` 打补丁，又因为"半授予"重入问题把它删掉，本质是**模型没给这个需求留正确的位置**（见 §3.11）。

**统一后的收益**（全部落在那条排序规则之外）：

- **一个 Authoring List**：作者只维护一张表，每条 Entry 在一处配完；
- **统一身份与 Metadata**：所有条目共有 `EventID` / `Priority`，编辑器里能按同一套规则展示、排序、定位；
- **一套校验、一套 Scheduler 输入**：`IsDataValid` 与运行期节点收集都只扫一个数组（`CollectNodes` / `SortNodes`）；
- **迁移直观**：看到 `ANS_X` 就配一条 Window，看到 `AN_X` 就配一条 Point，搬完删掉动画里的 Notify 即可（§0.3.1）。

> 准确的说法是：**一个列表描述全部 Timeline Entry；运行期把 Window 展开成 Begin / End 节点，再统一排序消费。**
> 不要写成"编辑器里列表顺序即执行顺序"，那是错的：

```text
Events:  Window A [0.1, 0.9)
         Point  B @0.5

列表顺序：A, B
真实节序：A.Begin @0.1 → B.Point @0.5 → A.End @0.9
```

#### 0.3.1 从 Montage 迁移过来的对照流程

第二阶段接入 Montage 时，迁移动作是机械的：

```text
打开 AM_Attack01
  ├── 找到 ANS_ComboWindow [0.30, 0.60)  → 在 Timeline 加一条 Window：
  │      EventID=ComboWindow  WindowTag=Status.Attack.ComboWindow  [0.30, 0.60)
  ├── 找到 AN_HitCheck @0.35             → 在 Timeline 加一条 Point：
  │      EventID=HitCheck     PointEventTag=GameplayEvent.Attack.HitCheck  @0.35
  └── 删除动画里这两条 Notify（逻辑 Notify 一律不留，见 §7.1）
```

**"逻辑与表现分离"就体现在这一步**：迁移完成后，改窗口时间不需要动画师动资产，换动画不需要重新对位逻辑。

---

## 1. 职责与判据

### 1.1 Timeline（资产）

| 做 | 不做 |
|---|---|
| 声明总时长 `Duration` | 伤害数值 / 倍率公式 |
| 声明一段时间内生效的语义与效果（Window） | 打谁、打几个、怎么选目标 |
| 声明某一刻触发的语义（Point） | 事件触发后干什么 |
| 声明 Point 在哪些端派发（`NetPolicy`） | 事件之间的业务依赖 |

### 1.2 Task（运行时）

**做四件事**：

1. 按世界时间推进逻辑时间；
2. 在 Window 进入 / 退出时授予 / 撤销 `WindowTag`（non-replicated loose tag），并按需施加 / 移除 `WindowEffectClass`；
3. 在 Point 时刻向 ASC 派发 `GameplayEvent`；
4. 时间走完时自动结束自己，并保证**任何销毁路径都清理干净**（tag 与 GE 都要清）。

**不做**：播 / 停 Montage、推进连击段数、结束 Ability、决定事件含义、持有业务状态。

### 1.3 判据（沿用母文档 §3.1）

> **Timeline 回答"什么时候发出什么语义信号"，不执行业务逻辑。**

| 允许 | 不允许 |
|---|---|
| `0.35s → GameplayEvent.Attack.HitCheck`（发信号） | `SphereTrace(...)`（执行检测） |
| `[0.30, 0.60) → Status.Attack.ComboWindow`（发状态） | `SetNextComboIndex(3)`（改战斗状态） |
| `[0.20, 0.50) → GE_Armor`（声明这段时间带霸体） | `if (血量 < 50%)`（做条件判断） |

---

## 2. 数据模型

> 本阶段类型**尚未实现**。字段以本文件为准。

### 2.1 `EHodgeTimelineEventKind`

```cpp
/**
 * 时间轴事件类型。
 *
 * 与动画资产里的两类 Notify 一一对应：
 *   Window  ← AnimNotifyState（区间，持续生效）
 *   Point   ← AnimNotify（瞬时，触发一次）
 */
UENUM(BlueprintType)
enum class EHodgeTimelineEventKind : uint8
{
    // 时间区间：[StartTime, EndTime) 内保持生效。
    Window,

    // 时间点：只在 StartTime 触发一次。
    Point
};
```

### 2.2 `FHodgeTimelineEvent`（统一结构）

```cpp
/**
 * 时间轴上的一个条目（Timeline Entry）：区间或时刻。
 *
 * 与动画资产的两类 Notify 一一对应：Window ← AnimNotifyState，Point ← AnimNotify。
 * 所有条目放在同一个数组里（统一 Authoring List / 统一身份 / 统一校验 / 统一 Scheduler 输入）；
 * 但**不为"字段统一"而牺牲语义**——两种 Kind 的字段是分开的，用 EditConditionHides 互相隐藏。
 *
 * 为什么 Tag 与效果类都要按 Kind 拆开：
 *   Window 的 tag 走 AddLooseGameplayTag / RemoveLooseGameplayTag，是"状态"；
 *   Point  的 tag 走 HandleGameplayEvent，是"消息"。
 *   两者的运行期语义、Tag 命名空间、GE 生命周期契约都不同，共用一个属性只会把差异藏起来。
 */
USTRUCT(BlueprintType)
struct FHodgeTimelineEvent
{
    GENERATED_BODY()

    // 事件类型。决定哪些字段生效。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EHodgeTimelineEventKind Kind = EHodgeTimelineEventKind::Window;

    // 资产内唯一标识。
    // 只用于：编辑器显示与查找、日志、Trace、跨端断言。
    // **禁止在 Gameplay 逻辑里按它分支** —— 那会造出第二套业务身份（见 §2.6）。
    // 它是列表的显示名，所以必须起得可读（ComboWindow / HitCheck / SuperArmor）。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName EventID;

    // 区间起点（秒）；Point 用它表示触发时刻。闭区间。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.0, UIMin=0.0, Units="s"))
    float StartTime = 0.f;

    // 区间终点（秒），开区间：EndTime 时刻本事件已经退出。仅 Window 有效。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        meta=(ClampMin=0.0, UIMin=0.0, Units="s",
              EditCondition="Kind==EHodgeTimelineEventKind::Window", EditConditionHides))
    float EndTime = 0.f;

    // 同一时刻的执行次序，数值小的先执行。
    // 只在"同一时刻、同一类节点"之间比较；不覆盖 WindowEnd → WindowBegin → Point 的固有顺序（§3.4.6）。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 Priority = 0;

    // ───────── Window 专用：区间内持续生效的"状态" ─────────

    // 区间内自动 Add / Remove 的 loose tag。
    // Categories 只是编辑器 Tag Picker 的层级过滤（不参与校验、不影响运行期）：
    // Window 表达的是"状态"，所以默认限定在 Status 下；需要驱动别的命名空间时去掉这个 metadata。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        meta=(Categories="Status",
              EditCondition="Kind==EHodgeTimelineEventKind::Window", EditConditionHides))
    FGameplayTag WindowTag;

    // 区间内施加、退出时移除的 GE。**必须是 Infinite**，否则这段区间的语义会出现空洞（§4.1）。
    // 只在权威端施加并依赖 GE 复制。留空表示这段区间只驱动 WindowTag。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        meta=(EditCondition="Kind==EHodgeTimelineEventKind::Window", EditConditionHides))
    TSubclassOf<UGameplayEffect> WindowEffectClass;

    // ───────── Point 专用：某一刻派发的"消息" ─────────

    // 随 GameplayEvent 广播的事件标签。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        meta=(Categories="GameplayEvent",
              EditCondition="Kind==EHodgeTimelineEventKind::Point", EditConditionHides))
    FGameplayTag PointEventTag;

    // 时刻施加一次的 GE。只在拥有权威时施加；**Timeline 此后不再追踪它的生命周期**。
    // 允许 Instant / HasDuration，禁止 Infinite（§4.1）。留空表示这个时刻只派发 PointEventTag。
    //
    // 固定顺序（写死在实现里，不依赖调用方）：**先派发 PointEventTag，再施加本 GE**。
    // 因此事件的处理者若结束了 Ability / 停止了 Timeline，本 GE 就不会被施加（§3.9）。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        meta=(EditCondition="Kind==EHodgeTimelineEventKind::Point", EditConditionHides))
    TSubclassOf<UGameplayEffect> PointEffectClass;

    // Point 在哪些端派发（Window 的 tag 各端各自维护、Window 的 GE 固定权威端，都不受它控制）。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        meta=(EditCondition="Kind==EHodgeTimelineEventKind::Point", EditConditionHides))
    EHodgeTimelineEventNetPolicy NetPolicy = EHodgeTimelineEventNetPolicy::LocalAndAuthority;
};
```

**字段说明与取舍**

| 字段 | 取舍 | 理由 |
|---|---|---|
| `Kind` | 显式枚举，不做"EndTime > StartTime 就是区间"的隐式推断 | 隐式推断会让 `EndTime == StartTime` 的意图不明；而且"是否派发 GameplayEvent""是否按 NetPolicy 分派"这些**运行时行为差异**必须显式可见 |
| `EventID` | `FName`，资产内唯一；**只做定位，不做业务判定** | 用于编辑器显示与查找、日志、Trace、跨端断言。Gameplay 逻辑一律按 Tag 分支，否则会同时存在 `EventID == "HitCheck"` 与 `PointEventTag == GameplayEvent.Attack.HitCheck` 两套业务身份，改一个忘一个 |
| ~~`DisplayName`~~ | **本阶段删除** | `TitleProperty` 不会自动回退到别的字段，留空就是一片空白，而本阶段明确不做自定义条目显示。直接用 `TitleProperty=EventID`，等有 Timeline Editor 再补中文名 |
| `WindowTag` / `PointEventTag` | **按 Kind 拆成两个字段**，不共用一个 `EventTag` | ① `Categories` 会真的过滤 Tag Picker 的层级，共用一个字段时 Point 根本选不到 `GameplayEvent.*`；② 两者运行期语义不同（loose tag vs `HandleGameplayEvent`），共用一个属性等于把差异藏起来。至于"填了一个忘另一个"，由 `EditConditionHides` + 按 Kind 校验兜住，**不构成理由** |
| `Priority` | 显式配置，但不覆盖语义顺序 | 见 §3.4.6：语义顺序被覆盖会让"某一刻的状态"依赖作者配置，跨端一致与区间定义都会破 |
| `WindowEffectClass` / `PointEffectClass` | **按 Kind 拆成两个字段** | 生命周期契约不同：Window 成对施加 / 移除且必须 Infinite；Point 只施加一次、此后不归 Timeline 管。挤在一个字段里只能写成一段注释，而这两条契约的性质完全不同 |

### 2.3 `EHodgeTimelineEventNetPolicy`

```cpp
/**
 * Point 事件的本端执行策略。
 *
 * 三态直接决定事件走哪条执行通道，因此必须在派发时真实分派。
 */
UENUM(BlueprintType)
enum class EHodgeTimelineEventNetPolicy : uint8
{
    // 服务器与本地控制端各自执行。用于两端都需要的事件。
    LocalAndAuthority,

    // 仅服务器执行。用于命中判定、施加伤害等游戏事实。
    AuthorityOnly,

    // 仅本地控制端执行。用于相机抖动、本地特效、本地 UI。
    LocallyControlledOnly
};
```

### 2.4 `UHodgeAbilityTimeline`

```cpp
/**
 * 技能逻辑时间轴。
 *
 * 只描述"什么时候发生什么"，不包含任何业务判断。
 * 由 UHodgeAbilityTask_PlayTimeline 在运行时驱动。
 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeAbilityTimeline : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // 逻辑总时长（秒）。逻辑时间的唯一基准。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.01, Units="s"))
    float Duration = 1.0f;

    // 时间轴条目列表（Window 与 Point 混排）。
    // 保存后按 StartTime 稳定排序；TitleProperty 直接指向 EventID（本阶段不引入 DisplayName）。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty=EventID))
    TArray<FHodgeTimelineEvent> Events;

    // 本阶段**不提供** `GetActiveWindows` 之类的查询 API：
    // 它原本唯一的消费者是 InitializeTimeline，现在初始化改走与 Tick 共用的节点收集（§3.5）；
    // 而"此刻处于哪些窗口"要判断时，直接查 ASC 上的 loose tag 更准（那才是真实运行态）。
    // 等出现真实查询需求再加。

#if WITH_EDITOR
    // 编辑器校验：时长、区间合法性、标签与 ID 有效性、同标签窗口重叠等。
    virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

    // 属性变化后自动按 StartTime 排序 Events，不修改配置语义。
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
```

### 2.5 时间语义（三条约定）

| 约定 | 内容 | 为什么 |
|---|---|---|
| 区间 | Window 是左闭右开 `[StartTime, EndTime)`；Point 只在 `StartTime` 这一刻 | 相邻窗口可以在同一时刻无缝衔接，且同一时刻的状态由数学定义唯一确定（见 §3.4） |
| 时刻消费 | 推进使用左开右闭 `(PreviousTime, CurrentTime]` | 保证每个时刻恰好被消费一次，且起点时刻不丢（见 §3.5） |
| 单位 | 一律绝对秒，不用归一化比例 | 连击窗口等"物理时长"是手感参数，换动画不能改变它的绝对长度 |

### 2.6 身份职责的边界（`EventID` vs Tag）

两套身份必须分工死，否则一定会腐烂成"改一个忘一个"：

| 载体 | 用途 | 禁止 |
|---|---|---|
| `EventID`（`FName`） | 编辑器显示与查找、日志、`-LogGameplayEvents` 之类的 Trace、跨端断言（"两端处理的是同一条 Entry"） | **Gameplay 逻辑禁止按它分支**：不要写 `if (EventID == FName("HitCheck"))`。要判断"现在能不能接招"就看 `WindowTag`，要判断"收到的是哪条消息"就看 `PointEventTag` |
| `WindowTag` / `PointEventTag` | Gameplay 语义：状态查询、互斥判定、事件订阅 | 不要用 Tag 当"这条 Entry 的唯一编号"（同一个 Tag 可以出现在多条 Entry 上） |

`EventID` 因为同时充当列表显示名，所以命名要面向人：`ComboWindow` / `HitCheck` / `SuperArmor`。

### 2.7 本阶段刻意不引入的字段

`Montage` / `MontageSection` / `LengthMismatchTolerance` / `AttackID` / `SyncDurationFromMontage()` / 循环窗口 / 窗口内的变速曲线 / `DisplayName`（自定义条目显示）—— 全部留到第二阶段。

`StartOffset` 保留为 Task 的运行参数（不是资产字段）："从某一秒起播"是 Task 的能力，与是否播动画无关。

---

## 3. 运行时：`UHodgeAbilityTask_PlayTimeline`

### 3.1 接口

先定义停止原因。它决定要不要广播停止事件，由调用方**显式传入**，不允许由回调反推：

```cpp
/**
 * 时间轴的停止原因。
 *
 * 区分它们不是为了日志好看：只有"被外力真正抢占"才广播 Interrupted。
 */
UENUM(BlueprintType)
enum class EHodgeTimelineStopReason : uint8
{
    // 尚未声明。本阶段的 Task 不保存停止原因，该值留给后续"攻击段执行"使用。
    None,

    // 时间走完（ElapsedTime >= Duration），由 Task 自动产生，广播 Timeline.End。
    NaturalEnd,

    // 被外力真正抢占，广播 Interrupted。
    Interrupted,

    // Ability 取消 / Task 随 Ability 销毁。不广播任何事件。
    AbilityCancelled
};
```

```cpp
/**
 * 把一条时间轴展开之后的"节点"。
 *
 * 枚举值顺序 = 同一时刻的执行顺序。这一点是设计的一部分，不是巧合：
 * 见 §3.4.2（区间 [Start, End) 的数学定义要求 WindowEnd 先于 WindowBegin、Point 最后）。
 *
 * 放在头文件（而不是 .cpp 的匿名 namespace）里，是因为 CollectNodes / SortNodes
 * 要作为成员函数被 InitializeTimeline 与 AdvanceTimeline 共用 —— 那正是"统一 Scheduler"的落点。
 */
enum class EHodgeTimelineNodeKind : uint8
{
    WindowEnd = 0,
    WindowBegin = 1,
    PointFire = 2
};

struct FHodgeTimelineNode
{
    // 节点发生的时刻。恢复"起点处已生效的窗口"时取该 Window 的 StartTime（§3.5）。
    float Time = 0.f;
    EHodgeTimelineNodeKind Kind = EHodgeTimelineNodeKind::PointFire;
    int32 EventIndex = INDEX_NONE;
};

UCLASS()
class HODGEPODGE_API UHodgeAbilityTask_PlayTimeline : public UAbilityTask
{
    GENERATED_BODY()

public:
    UHodgeAbilityTask_PlayTimeline(const FObjectInitializer& ObjectInitializer);

    /**
     * 创建时间轴驱动。
     *
     * 注意：工厂只创建并赋值，不会触发 Task。C++ 调用方拿到返回值后必须自己调用
     * ReadyForActivation()，否则 Activate() 永远不会执行（且不会有任何报错）。
     */
    UFUNCTION(BlueprintCallable, Category="Hodge|Ability|Tasks",
        meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility",
              BlueprintInternalUseOnly="TRUE"))
    static UHodgeAbilityTask_PlayTimeline* PlayTimeline(
        UGameplayAbility* OwningAbility,
        UHodgeAbilityTimeline* Timeline,
        float StartOffset = 0.f,
        float InitialPlayRate = 1.f);

    // 停止时间轴。必须带原因，它决定要不要广播停止事件。
    UFUNCTION(BlueprintCallable, Category="Hodge|Ability|Tasks")
    void StopTimeline(EHodgeTimelineStopReason Reason);

    // 是否已经停止推进。已销毁必然是已停止，反之不成立。
    UFUNCTION(BlueprintPure, Category="Hodge|Ability|Tasks")
    bool IsTimelineStopped() const { return bStopped; }

protected:
    virtual void Activate() override;
    virtual void TickTask(float DeltaTime) override;
    virtual void OnDestroy(bool bInOwnerFinished) override;

    // 建立起点状态：恢复起点处已在生效的 Window，并消费起点时刻的节点。
    // 与 AdvanceTimeline 共用同一套节点收集与排序，保证"需要恢复的窗口"与"恰好落在起点的 Point"
    // 之间的**调度顺序**遵循同一套规则 —— 这是接续语义，不是历史重放（§3.5）。
    void InitializeTimeline(float InStartOffset);

    // 消费 (PreviousTime, CurrentTime] 内的全部时间节点。
    void AdvanceTimeline(float PreviousTime, float CurrentTime);

    // 收集时间节点。bInitializing 为 true 时只收集"起点恢复"与"恰好落在起点"的节点。
    // PreviousTime 在 bInitializing 为 true 时被忽略。
    void CollectNodes(float PreviousTime, float CurrentTime, bool bInitializing,
                      TArray<FHodgeTimelineNode>& OutNodes) const;

    // 节点排序：时间 → 节点种类 → Priority → EventIndex。
    // InitializeTimeline 与 AdvanceTimeline 共用，是"统一 Scheduler"的唯一入口。
    void SortNodes(TArray<FHodgeTimelineNode>& Nodes) const;

    // Window 进入 / 退出：WindowTag 加减 + WindowEffectClass 施加 / 移除。
    void EnterWindow(int32 EventIndex);
    void ExitWindow(int32 EventIndex);

    // Point：按 NetPolicy 派发一次 GameplayEvent，并按需施加 PointEffectClass。
    void FirePointEvent(const FHodgeTimelineEvent& Event);

    // 施加一条 Entry 配置的 GE（Window 与 Point 共用），返回句柄；失败时返回无效句柄。
    // 名字不叫 ApplyWindowEffect：Point 也在用它。
    FActiveGameplayEffectHandle ApplyTimelineEffect(UAbilitySystemComponent* ASC,
                                                    TSubclassOf<UGameplayEffect> EffectClass);

    // 派发 Task 自身产生的系统事件（Timeline.End / Interrupted）。
    void FireSystemEvent(const FGameplayTag& EventTag);

    // 撤销全部已授予的 tag 与已施加的 GE。必须幂等，且对"改 ASC 引发的重入"安全。
    void ClearAllWindowState();

private:
    UPROPERTY()
    TObjectPtr<UHodgeAbilityTimeline> TimelineAsset;

    float StartOffset = 0.f;
    float InitialPlayRate = 1.f;
    float LastUpdateWorldTime = 0.f;
    float LogicalElapsed = 0.f;
    float ElapsedTime = 0.f;

    // tag 账本：当前处于激活状态的 Window 下标。tag 由配置推出，所以记账即状态。
    TArray<int32> ActiveWindowIndices;

    // GE 账本：Window 下标 → 已施加的 Infinite GE 句柄。没有配 GE 的窗口不在此表里。
    TMap<int32, FActiveGameplayEffectHandle> WindowEffectHandles;

    // 是否已停止推进。StopTimeline 与 OnDestroy 都会置位，它同时是重入防护的唯一哨兵。
    bool bStopped = false;

    // 清理是否已经完成。用于 GE 的"施加后才拿到句柄"这一特殊路径做补偿判断（见 3.11）。
    bool bCleanedUp = false;
};
```

构造函数：

```cpp
UHodgeAbilityTask_PlayTimeline::UHodgeAbilityTask_PlayTimeline(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 不设置它，TickTask 不会被调用。
    bTickingTask = true;
}
```

### 3.2 工厂与 `Activate()` 的分工

```cpp
UHodgeAbilityTask_PlayTimeline* UHodgeAbilityTask_PlayTimeline::PlayTimeline(
    UGameplayAbility* OwningAbility, UHodgeAbilityTimeline* Timeline,
    float StartOffset, float InitialPlayRate)
{
    UHodgeAbilityTask_PlayTimeline* Task = NewAbilityTask<UHodgeAbilityTask_PlayTimeline>(OwningAbility);

    Task->TimelineAsset = Timeline;
    Task->StartOffset = StartOffset;
    Task->InitialPlayRate = InitialPlayRate;

    return Task;
}
```

工厂**只赋值**，不访问 World、不校验、不做初始化。

**C++ 调用方必须自己调 `ReadyForActivation()`**，这不是可选项：

```cpp
// C++ 里的正确用法
UHodgeAbilityTask_PlayTimeline* Task =
    UHodgeAbilityTask_PlayTimeline::PlayTimeline(this, Timeline, 0.f, 1.f);

Task->ReadyForActivation();   // ← 触发 Activate()；漏掉这行 = 时间轴静默不跑
```

依据（已核实）：

- `ReadyForActivation()` 是 `UGameplayTask` 的公开入口（注释原文 "Called to trigger the actual task once the delegates have been set up"，`GameplayTask.h:155-157`），它内部才会走到 `Activate()`；`Activate()` 是 protected virtual（`GameplayTask.h:162`）。
- 引擎自己的工厂也不代劳：`UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy()` 只做 `NewAbilityTask` + 赋值 + `return`（`AbilityTask_PlayMontageAndWait.cpp:111-125`）。
- 蓝图侧的 latent AbilityTask 节点会自动补这一步，所以**只有 C++ 手写调用才需要自己调**；也正因如此工厂里**不能**代为调用（否则蓝图侧会激活两次）。
- `NewAbilityTask` 是 `UAbilityTask` 的静态模板成员，只负责创建（`AbilityTask.h:126-139`）。

```cpp
void UHodgeAbilityTask_PlayTimeline::Activate()
{
    Super::Activate();

    // 校验集中在唯一入口：C++ 与蓝图调用最终都会经过这里（C++ 需先 ReadyForActivation）。
    const bool bParamsValid =
        IsValid(TimelineAsset) &&
        TimelineAsset->Duration > 0.f &&
        InitialPlayRate > 0.f &&
        StartOffset >= 0.f &&
        StartOffset < TimelineAsset->Duration;

    if (!bParamsValid)
    {
        UE_LOG(LogTemp, Error,
               TEXT("[Hodge] PlayTimeline 参数非法：Timeline=%s Duration=%.3f StartOffset=%.3f PlayRate=%.3f"),
               *GetNameSafe(TimelineAsset),
               IsValid(TimelineAsset) ? TimelineAsset->Duration : -1.f,
               StartOffset, InitialPlayRate);

        // 不留一个"活着但不动"的 Task。
        EndTask();
        return;
    }

    const UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("[Hodge] PlayTimeline 拿不到 World，Task 结束。"));
        EndTask();
        return;
    }

    LogicalElapsed = 0.f;
    ElapsedTime = StartOffset;
    LastUpdateWorldTime = World->GetTimeSeconds();

    InitializeTimeline(StartOffset);
}
```

> **为什么校验放在 `Activate()` 而不是让工厂返回 `nullptr`**：工厂在蓝图里是表达式节点，返回 `nullptr`
> 需要调用方额外分支；而 `Activate()` 是所有路径必经的生命周期起点。语义统一为
> **无效参数 ⇒ 立刻结束，不产生空转 Task**。

### 3.3 内部状态

| 成员 | 含义 | 约束 |
|---|---|---|
| `TimelineAsset` | 被驱动的时间轴 | 必须是 `UPROPERTY`，否则会被 GC |
| `StartOffset` / `InitialPlayRate` | 逻辑时间起点 / 速率 | `[0, Duration)`；`> 0`，启动后不变 |
| `LastUpdateWorldTime` / `LogicalElapsed` / `ElapsedTime` | 时间推进三件套 | `ElapsedTime` 单调不减且夹在 `Duration` 以内 |
| `ActiveWindowIndices` | 处于激活状态的 Window 下标 | **tag 账本**：与 `WindowTag` 的计数严格一一对应。一个窗口只驱动一个 tag，所以"下标集合"本身就是账本，不需要按 tag 计数 |
| `WindowEffectHandles` | 已施加的 Infinite GE 句柄 | **GE 账本**：只记录"确实施加成功过"的句柄；施加失败（`Handle` 无效）不入账 |
| `bStopped` | 已停止推进 | `StopTimeline` 与 `OnDestroy` 都会置位；重入防护的唯一哨兵 |
| `bCleanedUp` | **账本已封存**（`ClearAllWindowState` 把两个账本 `MoveTemp` 走之后就置位，不是等函数返回） | 只给 GE 的补偿路径用（见 §3.11）：从这一刻起新施加的 GE 不再入账，必须由 `EnterWindow` 自补偿 |

两个账本都是**按下标精确对应**的，不需要"按 tag 计数"的通用账本——因为一个 Window 只驱动一个 `WindowTag` 和至多一个 GE（见 §3.4.5）。

### 3.4 核心算法：单一时间节点序列

#### 3.4.1 问题

`AdvanceTimeline` 里**不能**把"窗口状态更新"和"事件派发"当成两个互不相关的步骤。低帧率下一次 Tick 可能跨过多个边界：

```text
Window Active = [0.30, 0.40)
Point  HitCheck         = 0.35

上一帧 PreviousTime = 0.28
这一帧 CurrentTime  = 0.42
```

三种写法的结果：

| 写法 | 0.35 的 `HitCheck` 看到 `Status.Attack.Active` | 结论 |
|---|---|---|
| `UpdateWindows(CurrentTime)` → `FirePoints(...)` | `false`（0.42 时 Active 已退出） | 错误 |
| `FirePoints(...)` → `UpdateWindows(CurrentTime)` | `false`（窗口还没进入） | 错误 |
| 按时间顺序消费全部节点 | `true` | 正确 |

如果事件处理里要读 `Status.Attack.*` 做判断（例如"命中判定只在 Active 期间生效"），前两种写法会**静默给出错误结果**，且只在卡顿、低帧率、断点调试时出现。

#### 3.4.2 三种节点与同一时刻的顺序

一次推进中可能出现的节点：

| 节点 | 来源 | 时刻 |
|---|---|---|
| `WindowEnd` | `Kind == Window` 的 `EndTime` | `EndTime` |
| `WindowBegin` | `Kind == Window` 的 `StartTime` | `StartTime` |
| `Point` | `Kind == Point` 的 `StartTime` | `StartTime` |

同一时刻 `T` 上的执行顺序固定为：

```text
WindowEnd  →  WindowBegin  →  Point
```

依据是 Window 的区间定义 `[StartTime, EndTime)`：

- `EndTime == T` 的窗口在 `T` 时刻**已经不生效** ⇒ 先 Exit；
- `StartTime == T` 的窗口在 `T` 时刻**已经生效** ⇒ 后 Enter；
- Point 在 `T` 时刻执行，应当看到 **`T` 时刻的完整状态** ⇒ 最后派发。

于是 `T` 时刻的状态恰好等于 `{ W | W.Start <= T < W.End }`，与数学定义一致。

#### 3.4.3 为什么"合并排序"优于"每帧查询差集"

考虑一个**短于一帧**的窗口：

```text
Window Active = [0.30, 0.31)
Point  Test             = 0.305

上一帧 0.28 → 这一帧 0.42
```

| 写法 | 结果 |
|---|---|
| 每帧查询一次"此刻生效的窗口集合"，再与 `ActiveWindowIndices` 做差集 | `CurrentTime = 0.42` 时窗口既不生效也从未生效 ⇒ **整个窗口被跳过**，0.305 的 Point 看到 `false` |
| 合并时间节点序列 | `WindowBegin@0.30 → Point@0.305 → WindowEnd@0.31` ⇒ Point 看到 `true`，tag 与 GE 进出配对 |

所以窗口状态维护也用**时间节点序列**，不用"每帧查询差集"。唯一例外是**启动瞬间**——那时没有"上一帧"，于是按接续语义收集此刻仍生效的 Window，再经过**同一套 Scheduler**（`CollectNodes` → `SortNodes` → `EnterWindow`）建立初始状态（见 §3.5）。那里不需要另立一套规则，也不需要查询 API。

#### 3.4.4 算法

```cpp
namespace
{
    // 左开右闭，与推进区间一致。
    FORCEINLINE bool IsInAdvanceInterval(float Time, float PreviousTime, float CurrentTime)
    {
        return Time > PreviousTime && Time <= CurrentTime;
    }
}

void UHodgeAbilityTask_PlayTimeline::CollectNodes(float PreviousTime, float CurrentTime,
                                                 bool bInitializing, TArray<FHodgeTimelineNode>& OutNodes) const
{
    OutNodes.Reset();

    const TArray<FHodgeTimelineEvent>& Events = TimelineAsset->Events;
    OutNodes.Reserve(Events.Num() * 2);

    for (int32 EventIndex = 0; EventIndex < Events.Num(); ++EventIndex)
    {
        const FHodgeTimelineEvent& Event = Events[EventIndex];

        if (Event.Kind == EHodgeTimelineEventKind::Window)
        {
            if (bInitializing)
            {
                // 起点恢复：只补"起点时仍然生效"的窗口（Start <= T < End），且节点时刻取 StartTime。
                // 取 StartTime 是为了**排序**：这样它落在与"正常推进时该窗口进入"相同的相对位置上，
                // 同一个比较器就能排出与真实运行一致的先后。
                // 注意这只影响调度顺序 —— 窗口 GE 的实际施加时刻仍然是 StartOffset（§3.5）。
                if (Event.StartTime <= CurrentTime && CurrentTime < Event.EndTime)
                {
                    OutNodes.Add({Event.StartTime, EHodgeTimelineNodeKind::WindowBegin, EventIndex});
                }
            }
            else
            {
                if (IsInAdvanceInterval(Event.EndTime, PreviousTime, CurrentTime))
                {
                    OutNodes.Add({Event.EndTime, EHodgeTimelineNodeKind::WindowEnd, EventIndex});
                }

                if (IsInAdvanceInterval(Event.StartTime, PreviousTime, CurrentTime))
                {
                    OutNodes.Add({Event.StartTime, EHodgeTimelineNodeKind::WindowBegin, EventIndex});
                }
            }
        }
        else
        {
            // Point：起点只消费"恰好落在起点"的那一条；历史（StartTime < StartOffset）一律不补发。
            const bool bCollect = bInitializing
                ? (Event.StartTime == CurrentTime)
                : IsInAdvanceInterval(Event.StartTime, PreviousTime, CurrentTime);

            if (bCollect)
            {
                OutNodes.Add({Event.StartTime, EHodgeTimelineNodeKind::PointFire, EventIndex});
            }
        }
    }
}

void UHodgeAbilityTask_PlayTimeline::SortNodes(TArray<FHodgeTimelineNode>& Nodes) const
{
    const TArray<FHodgeTimelineEvent>& Events = TimelineAsset->Events;

    Nodes.Sort([&Events](const FHodgeTimelineNode& A, const FHodgeTimelineNode& B)
    {
        if (A.Time != B.Time)
        {
            return A.Time < B.Time;
        }

        if (A.Kind != B.Kind)
        {
            return static_cast<uint8>(A.Kind) < static_cast<uint8>(B.Kind);
        }

        const int32 PriorityA = Events[A.EventIndex].Priority;
        const int32 PriorityB = Events[B.EventIndex].Priority;

        if (PriorityA != PriorityB)
        {
            return PriorityA < PriorityB;
        }

        // 同刻同类同优先级：用下标兜底，保证跨端严格确定。
        return A.EventIndex < B.EventIndex;
    });
}

void UHodgeAbilityTask_PlayTimeline::AdvanceTimeline(float PreviousTime, float CurrentTime)
{
    // 时钟不前进或倒退（关卡切换、世界时间重置）时不做区间消费。
    if (CurrentTime <= PreviousTime)
    {
        return;
    }

    const TArray<FHodgeTimelineEvent>& Events = TimelineAsset->Events;

    // 1) + 2) 收集与排序：与 InitializeTimeline 共用同一套规则（"统一 Scheduler"的落点）
    TArray<FHodgeTimelineNode> Nodes;
    CollectNodes(PreviousTime, CurrentTime, /*bInitializing=*/false, Nodes);
    SortNodes(Nodes);

    // 3) 严格按序消费
    for (const FHodgeTimelineNode& Node : Nodes)
    {
        // 三种节点都会写 ASC（加 tag / 施 GE / 派发事件），都可能同步结束 Ability 并销毁本 Task。
        if (bStopped)
        {
            return;
        }

        const FHodgeTimelineEvent& Event = Events[Node.EventIndex];

        switch (Node.Kind)
        {
        case EHodgeTimelineNodeKind::WindowEnd:
            ExitWindow(Node.EventIndex);
            break;

        case EHodgeTimelineNodeKind::WindowBegin:
            EnterWindow(Node.EventIndex);
            break;

        case EHodgeTimelineNodeKind::PointFire:
            FirePointEvent(Event);
            break;
        }

        // 关键：上面的调用返回后可能已经销毁，游标必须立刻失效。
        if (bStopped)
        {
            return;
        }
    }
}
```

**关于复杂度**：一条 Timeline 通常 3~8 条 Window + 3~10 条 Point，每帧收集 + 排序约 20 个节点，代价可以忽略。

> **本阶段不使用"事件游标"。** 游标方案隐含"数组运行期必须有序"这一前提，一旦乱序就永久漏触发；
> 合并扫描 + 排序不依赖数组有序。等真的有性能压力时再引入游标，并在那时补"必须有序"的校验。

#### 3.4.5 窗口的进入 / 退出：两个账本

```cpp
void UHodgeAbilityTask_PlayTimeline::EnterWindow(int32 EventIndex)
{
    // UE 5.5 的 UAbilityTask::AbilitySystemComponent 是 TWeakObjectPtr，取一次局部指针并判空。
    UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
    if (!ASC)
    {
        return;
    }

    // 防御：同一窗口只允许进入一次，否则 tag 会多加、GE 会多施。
    if (ActiveWindowIndices.Contains(EventIndex))
    {
        return;
    }

    const FHodgeTimelineEvent& Event = TimelineAsset->Events[EventIndex];

    // 先记账，再改 ASC。引擎是先更新标签计数、再广播回调，
    // 所以重入进来的清理一定看得见这一条并配对移除（理由见 §3.10 / §3.11）。
    ActiveWindowIndices.Add(EventIndex);
    ASC->AddLooseGameplayTag(Event.WindowTag, 1);

    // ── 防线一：Tag 回调可能已经结束了 Ability 并完成清理 ──
    // 此时**绝不能**再去施加 GE。"施加后立刻移除"只能撤销最终状态，
    // 撤销不了 Apply 期间已经发生的副作用（属性变化、Cue、OnActive、Tag 变化、能力阻断重算）。
    if (bStopped || bCleanedUp)
    {
        return;
    }

    // GE 无法"先记账"：句柄要等施加完才拿得到。这条路径的补偿规则见 §3.11。
    if (Event.WindowEffectClass && ASC->GetOwnerActor() && ASC->GetOwnerActor()->HasAuthority())
    {
        const FActiveGameplayEffectHandle Handle =
            ApplyTimelineEffect(ASC, Event.WindowEffectClass);

        // 施加失败（Tag 阻断 / 免疫 / 类无效）：不入账，否则账本会失真。
        if (!Handle.IsValid())
        {
            return;
        }

        // ── 防线二：Apply 内部发生重入 ──
        // 账本里没有这个句柄，不补偿就永久残留。
        if (bCleanedUp)
        {
            ASC->RemoveActiveGameplayEffect(Handle);
            return;
        }

        WindowEffectHandles.Add(EventIndex, Handle);
    }
}

void UHodgeAbilityTask_PlayTimeline::ExitWindow(int32 EventIndex)
{
    // 先记账移除，再改 ASC：重入进来的清理不会重复处理这一条。
    if (!ActiveWindowIndices.Remove(EventIndex))
    {
        return;
    }

    UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
    if (!ASC)
    {
        return;
    }

    const FHodgeTimelineEvent& Event = TimelineAsset->Events[EventIndex];

    // GE 先移除（可能触发属性变化与回调），再减 tag。
    if (const FActiveGameplayEffectHandle* Found = WindowEffectHandles.Find(EventIndex))
    {
        const FActiveGameplayEffectHandle Handle = *Found;
        WindowEffectHandles.Remove(EventIndex);

        if (Handle.IsValid())
        {
            ASC->RemoveActiveGameplayEffect(Handle);
        }
    }

    // ⚠️ 这里**绝对不能**加 `if (bStopped) return;`，和 EnterWindow 正相反：
    //    EnterWindow 是"先记账再改 ASC"，所以重入清理会把这一条一起撤掉，提前返回是安全的；
    //    ExitWindow 已经先把账本条目摘掉了，清理不会再管这个窗口的 tag，
    //    在这里提前返回 = WindowTag 计数永久多 1。
    //    换句话说："给所有 ASC 调用后面统一加守卫"这个做法是错的，必须按账本归属分别判断。
    //
    // 用**入参时取到的那个局部 ASC 指针**（而不是重新 AbilitySystemComponent.Get()）：
    // 当前调用栈已经接手了这个窗口的清理责任，就不该因为 Task 内部引用状态变化而半途甩手。
    // 保留 IsValid 检查只为对付"ASC 自身已被销毁"这一种情况 —— 那时它的 tag 容器也一起没了，
    // 跳过是正确行为；同时也避免拿一个悬垂裸指针去调成员函数。
    if (IsValid(ASC))
    {
        ASC->RemoveLooseGameplayTag(Event.WindowTag, 1);
    }
}
```

**为什么 Window 的 tag 与 GE 各用一个账本**

- tag：`WindowTag` 可以从配置推出，所以"这个窗口激活过"这件事本身（`ActiveWindowIndices`）就是账本，不需要额外计数表。
- GE：句柄是运行时才产生、且可能与配置不一致（施加失败就没有句柄），所以必须单独存句柄。
- 两者都**按下标**索引，因此不需要通用账本；这也是"一条 Entry 只驱动一个 tag + 至多一个 GE"这条约束的价值（§3.11）。

#### 3.4.6 为什么 `Priority` 不覆盖语义顺序

排序键是 `时间 → 节点种类 → Priority → Index`，也就是**语义顺序优先于作者配置**。原因：

- 若让 `Priority` 覆盖语义顺序，就会出现"某个 Point 在 `T` 时刻看到的状态取决于作者有没有调优先级"——**同一份时间轴在不同资产配置下语义不同**，`[Start, End)` 这条数学定义当场失效；
- 跨端一致依赖"相同输入 + 相同规则"，规则里混入可配置项会让对齐更难；
- 真正需要"点在窗口内执行"时，把 Point 的 `StartTime` 放到 `Window.EndTime` 之前即可，**在编辑器里一眼可见**，比藏在优先级里更好读。

`Priority` 的定位因此是：**同一时刻、同一类节点之间的执行次序**（例如 0.35s 上有 `CameraShake` / `HitCheck` / `VFX` 三条 Point）。

### 3.5 初始化语义

**先定义语义边界**（这一节最容易产生误解的地方）：

> **`StartOffset` 是"状态恢复 / 接续"语义，不是"历史重放"语义。**
>
> ```text
> 历史 Point（StartTime < StartOffset）            → 不补发
> 已经结束的 Window（EndTime <= StartOffset）       → 不重放
> 当前仍生效的 Window（Start <= StartOffset < End） → 在 StartOffset 重新建立
> ```
>
> 因此 `WindowEffectClass` 的 `OnActive` / `GameplayCue` / 周期 Tick 的**实际发生时刻是 `StartOffset`**，
> 而不是它名义上的 `StartTime`：

```text
Window [0.10, 0.50)，GE = Infinite + Period=0.1

从 0 跑      ：0.10 Apply → 0.20 Period → 0.30 Period
从 0.30 接入 ：0.30 Apply（此前的周期没有发生过）
```

> 换句话说，本节的算法解决的是 **"起点状态重建的确定性顺序"**，
> 而不是 **"与从 0 跑完全等价"** —— 后者在数学上也不可能，除非重放历史副作用，而那正是我们拒绝的做法。

```cpp
void UHodgeAbilityTask_PlayTimeline::InitializeTimeline(float InStartOffset)
{
    // 关键：初始化**不自己发明一套顺序**，而是走与 Tick 完全相同的收集 + 排序 + 消费。
    // 否则"接续运行时"的调度顺序会与"正常推进"不一致（早先版本就是这个 bug）。
    //
    // 语义边界：这只保证**当前状态重建的调度顺序**一致，
    // 不保证"从 0 跑到这里"的历史副作用等价 —— 见本节开头的语义说明。
    TArray<FHodgeTimelineNode> Nodes;
    CollectNodes(/*PreviousTime=*/0.f, InStartOffset, /*bInitializing=*/true, Nodes);
    SortNodes(Nodes);

    for (const FHodgeTimelineNode& Node : Nodes)
    {
        // 前一个节点可能已经把 Ability 结束掉了（见 §3.10）。
        if (bStopped)
        {
            return;
        }

        switch (Node.Kind)
        {
        case EHodgeTimelineNodeKind::WindowBegin:
            EnterWindow(Node.EventIndex);
            break;

        case EHodgeTimelineNodeKind::PointFire:
            FirePointEvent(TimelineAsset->Events[Node.EventIndex]);
            break;

        case EHodgeTimelineNodeKind::WindowEnd:
            // 不会出现：EndTime <= StartOffset 的窗口在起点就已经退出，
            // CollectNodes 的 bInitializing 分支不会收集它。
            break;
        }
    }
}
```

语义是：

| 条件 | 行为 |
|---|---|
| `StartTime < StartOffset` 的 Point | **跳过**，不补发 |
| `StartTime == StartOffset` 的 Point | **触发**。起点时刻属于本次执行 |
| 跨过 `StartOffset` 的 Window（`Start <= StartOffset < End`） | 启动瞬间就进入，tag 与 GE 立刻生效 |
| `EndTime == StartOffset` 的 Window | 视为未进入（区间开区间语义） |

**三条必须知道的限制**

1. `Point.StartTime == StartOffset` 用的是精确比较，因此 `StartOffset` 与 `StartTime` 应当**同源**（资产字面量）。若来自运行时计算，调用方需要自行对齐，否则可能漏触发。
2. **"起点状态用一次查询得出"并不等于"与从 0 跑一致"。** 早先的设计用 `GetActiveWindows(StartOffset)` 直接逐个 `EnterWindow`，拿到的**集合**是对的，但**顺序**是数组顺序：同刻多个窗口、以及同刻 `Priority` 不同的 Point，都会与正常 Tick 的顺序不一致。集合相同而顺序不同时，GE 之间的相互阻断、Tag 回调的先后都可能不同 —— 所以顺序是**可观测的**，必须一视同仁。现在两者共用 `CollectNodes` / `SortNodes`，就是为堵住这个缺口。
3. **不要用"从 `-∞` 重放到 `StartOffset`"这个更省事的写法。** 它会让已经结束的窗口先真施加 GE 再真移除：最终状态虽对，但 `OnActive` / Cue / 属性变化这些副作用撤不回来，比"跳过历史事件"更脏。所以起点只恢复状态、只消费起点时刻的节点，历史 Point 一律不补发。

### 3.6 `TickTask`

```cpp
void UHodgeAbilityTask_PlayTimeline::TickTask(float DeltaTime)
{
    Super::TickTask(DeltaTime);

    if (bStopped)
    {
        return;
    }

    const UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const float Now = World->GetTimeSeconds();
    const float WorldDelta = Now - LastUpdateWorldTime;

    // 先更新采样点，再判方向。
    // 顺序很重要：若把它放到 `if (WorldDelta <= 0) return;` 之后，
    // 后续每一帧都会重新算出同一个负差值 → Timeline 会被永久冻死。
    LastUpdateWorldTime = Now;

    // ⚠️ 负差值必须在**累加之前**拦掉。
    // 只靠 AdvanceTimeline 里的 `CurrentTime <= PreviousTime` 是拦不住的：那时
    // LogicalElapsed / ElapsedTime 已经被污染成负数，而下一帧起 CurrentTime 又开始单调增长，
    // 于是早期节点会被重新跨过（Point 重复派发），
    // 并且 ElapsedTime 要花几百秒才能爬回 Duration —— Ability 会远超预期时长一直挂着。
    if (WorldDelta <= 0.f)
    {
        return;
    }

    // 用世界时间差累加，而不是用 TickTask 的 DeltaTime：卡顿后能一次性把区间追上。
    LogicalElapsed += WorldDelta * InitialPlayRate;

    const float PreviousTime = ElapsedTime;

    // 时钟不允许越过 Duration。
    ElapsedTime = FMath::Min(StartOffset + LogicalElapsed, TimelineAsset->Duration);

    // 先把区间内的全部节点消费完……
    AdvanceTimeline(PreviousTime, ElapsedTime);

    // ……其间任意一个节点都可能已经结束了 Ability（Task 已被 OnDestroy，bStopped 已置位）。
    if (bStopped)
    {
        return;
    }

    // ……再判定自然结束，这样 StartTime == Duration 的 Point 不会被提前吃掉。
    if (ElapsedTime >= TimelineAsset->Duration)
    {
        StopTimeline(EHodgeTimelineStopReason::NaturalEnd);
    }
}
```

### 3.7 `StopTimeline` 与 `OnDestroy`

```cpp
void UHodgeAbilityTask_PlayTimeline::StopTimeline(EHodgeTimelineStopReason Reason)
{
    if (bStopped)
    {
        return;
    }

    bStopped = true;

    // 先清理本端状态，再广播，避免订阅者读到即将失效的旧状态。
    ClearAllWindowState();

    switch (Reason)
    {
    case EHodgeTimelineStopReason::NaturalEnd:
        FireSystemEvent(HodgeGameplayTags::GameplayEvent_Attack_Timeline_End);
        break;

    case EHodgeTimelineStopReason::Interrupted:
        FireSystemEvent(HodgeGameplayTags::GameplayEvent_Attack_Interrupted);
        break;

    default:
        // AbilityCancelled / None 不广播任何事件。
        break;
    }

    // 停止必须真正结束 Task，否则会留下一个"不再推进但仍然活着"的对象。
    // 重复 EndTask 由引擎的 TaskState 守卫，这里不需要额外判断。
    EndTask();
}

void UHodgeAbilityTask_PlayTimeline::OnDestroy(bool bInOwnerFinished)
{
    // 头等规则：从这一刻起，任何仍在调用栈里的时间轴推进都必须立刻停止。
    bStopped = true;

    // 无条件兜底：Ability 被外部取消、Pawn 销毁等路径都不会经过 StopTimeline。
    ClearAllWindowState();

    Super::OnDestroy(bInOwnerFinished);
}
```

`OnDestroy` 有两条来源，两条都必须被 `bStopped` 覆盖（已核实引擎实现）：

| 来源 | 进入方式 | `bInOwnerFinished` |
|---|---|---|
| Task 自己结束 | `EndTask()` → `OnDestroy(false)`（`GameplayTask.cpp:167-181`，带 `TaskState != Finished` 守卫） | `false` |
| Ability 结束 | `UGameplayTask::TaskOwnerEnded()` → `OnDestroy(true)`（`GameplayTask.cpp:146-165`） | `true` |

- **`EndTask()` 是幂等的**：第二次调用会因为 `TaskState == Finished` 直接返回。
- `bInOwnerFinished` **只用于日志与诊断**，不要用它改变清理行为。
- 引擎自己在 `UAbilityTask::OnDestroy` 的注释里写着 "**If we have already been destroyed this is being called recursively**"（`AbilityTask.cpp:115`），说明"递归销毁"是引擎承认的正常路径。

清理实现：

```cpp
void UHodgeAbilityTask_PlayTimeline::ClearAllWindowState()
{
    UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();

    // 先搬走并清空两个账本，再动 ASC。
    // 原因：移除 tag / 移除 GE 都会触发回调，回调可能在调用栈上重入本函数；
    // 若此时外层还在遍历账本，内层的 Reset() 会让外层读到已释放的内存。
    TArray<int32> IndicesToRemove = MoveTemp(ActiveWindowIndices);
    TMap<int32, FActiveGameplayEffectHandle> HandlesToRemove = MoveTemp(WindowEffectHandles);
    ActiveWindowIndices.Reset();
    WindowEffectHandles.Reset();

    if (ASC)
    {
        // 先撤 GE，再撤 tag：GE 可能带属性变化与更强的回调。
        for (const TPair<int32, FActiveGameplayEffectHandle>& Pair : HandlesToRemove)
        {
            if (Pair.Value.IsValid())
            {
                ASC->RemoveActiveGameplayEffect(Pair.Value);
            }
        }

        for (int32 EventIndex : IndicesToRemove)
        {
            const FHodgeTimelineEvent& Event = TimelineAsset->Events[EventIndex];
            ASC->RemoveLooseGameplayTag(Event.WindowTag, 1);
        }
    }

    // 标记清理完成：给"施加 GE 期间发生重入"的补偿路径用（见 3.11）。
    bCleanedUp = true;
}
```

四条硬性要求：

1. **幂等。** `StopTimeline` 调一次，随后的 `EndTask()` 会同步触发 `OnDestroy` 再调一次；第二次必须是空操作（账本已空）。
2. **对重入安全。** 见上面"先搬走再清理"的写法。
3. **严格配对。** 不要用 `SetLooseGameplayTagCount(Tag, 0)`（会清掉别处叠加的次数），不要用"按 GE 类反查并移除"（会误删别人的实例），一律按句柄移除。
4. **tag 用 non-replicated loose tag，不要用 `AddDynamicTagGameplayEffect`。** 两端各跑一条时间轴时，GE 方案会产生双实例并残留。需要"真实的、要复制的战斗状态"时，用 Window 的 `WindowEffectClass`（权威端施加，见 §3.9）。

`IsTimelineStopped()` 的语义是"已停止推进"。由于 `OnDestroy` 也会置位，所以**"已销毁"必然是"已停止"**，反之不成立。

### 3.8 清理与广播的顺序

`StopTimeline` 里固定为：

```text
ClearAllWindowState()  →  FireSystemEvent(...)  →  EndTask()
```

理由：`Interrupted` 的语义是"这一段结束了"，订阅方拿到通知时应当看到一个**干净的**状态（tag 与 GE 都已撤销），而不是"通知到达时状态还挂着"。若先广播再清理，订阅方在回调里查询状态会得到即将失效的旧值。

### 3.9 事件派发与 `NetPolicy`（Point）

```cpp
void UHodgeAbilityTask_PlayTimeline::FirePointEvent(const FHodgeTimelineEvent& Event)
{
    // 先算出"本端是否该执行"，再派发一次。
    // listen server 上本地玩家的 Ability 会同时满足 HasAuthority 与 IsLocallyControlled，
    // 写成两个独立 if 会让同一个事件执行两次。
    const AActor* Avatar = GetAvatarActor();
    const bool bIsAuthority = Avatar && Avatar->HasAuthority();
    const bool bIsLocallyControlled = IsLocallyControlled();

    bool bShouldFire = false;
    switch (Event.NetPolicy)
    {
    case EHodgeTimelineEventNetPolicy::LocalAndAuthority:
        bShouldFire = bIsAuthority || bIsLocallyControlled;
        break;

    case EHodgeTimelineEventNetPolicy::AuthorityOnly:
        bShouldFire = bIsAuthority;
        break;

    case EHodgeTimelineEventNetPolicy::LocallyControlledOnly:
        bShouldFire = bIsLocallyControlled;
        break;
    }

    if (!bShouldFire)
    {
        return;
    }

    UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
    if (!ASC)
    {
        return;
    }

    // 先派发语义事件，再由（可选的）GE 施加效果。
    // 载荷带身份：OptionalObject 指向本 Task，便于订阅方确认"这是我那条 Timeline"。
    FGameplayEventData Payload;
    Payload.EventTag = Event.PointEventTag;
    Payload.Instigator = Avatar;
    Payload.OptionalObject = this;
    Payload.EventMagnitude = Event.StartTime;

    // 注意：这是同步调用。广播对象可以在这里 EndAbility，从而销毁本 Task。
    ASC->HandleGameplayEvent(Event.PointEventTag, &Payload);

    if (bStopped)
    {
        return;
    }

    // Point 的 GE 只在拥有权威时施加：客户端预测施加 + 服务器施加会得到两份实例，
    // 一旦取消时序错开就会残留。第一阶段不做预测，因此固定权威端。
    //
    // 契约：Point **只负责 Apply 这一次**。此后 GE 的生命周期（Instant 立即结算、
    // HasDuration 自行到期）完全不属于 Timeline —— 不记账、也不移除它。
    // 正因为"没人会来收"，PointEffectClass 禁止配 Infinite（§4.1）。
    if (Event.PointEffectClass && bIsAuthority)
    {
        ApplyTimelineEffect(ASC, Event.PointEffectClass);
    }
}
```

**Point 内部的固定顺序：Event → Effect**

```text
HandleGameplayEvent(PointEventTag)   ← ① 派发语义事件（同步，可能结束 Ability / 停止 Timeline）
        ↓
if (bStopped) return                 ← ② 处理者已经结束了这一帧，则不再往下走
        ↓
Apply PointEffectClass               ← ③ 施加本 Entry 的效果
```

> 也就是说：**Event 的处理者若结束了 Ability，这一条 Point 的效果就不会产生。**
> 这是契约而不是 bug —— 触发者已经表示"这一击到此为止"，这时再补一个 Buff 反而是错的。
> 它必须写进字段说明（已在 §2.2 标注），否则半年后看资产的人会默认"同一时刻配的两样都会执行"。

**系统事件**（`Timeline.End` / `Interrupted`）由 Task 自己产生，复用同一套派发，按 `LocalAndAuthority`：

```cpp
void UHodgeAbilityTask_PlayTimeline::FireSystemEvent(const FGameplayTag& EventTag)
{
    // 系统事件没有资产条目，构造临时事件复用同一条派发路径，避免出现第二套逻辑。
    // 它是"消息"而不是"状态"，所以只填 Point 侧的字段。
    FHodgeTimelineEvent Event;
    Event.Kind = EHodgeTimelineEventKind::Point;
    Event.StartTime = ElapsedTime;
    Event.PointEventTag = EventTag;
    Event.NetPolicy = EHodgeTimelineEventNetPolicy::LocalAndAuthority;

    FirePointEvent(Event);
}
```

> `FGameplayEventData` 没有 `FGameplayAbilitySpecHandle` 与 `PredictionKey` 字段，所以身份只能靠
> `OptionalObject` 携带。`AttackID` 属于第二阶段（多条 Timeline 并行时才需要）。

**Window 的 `WindowTag` 与 `WindowEffectClass` 的端范围是固定的，不受 `NetPolicy` 控制：**

| 内容 | 端范围 | 理由 |
|---|---|---|
| `WindowTag`（loose tag） | 两端各自维护 | 本地运行态，用于动画层与本地互斥判定；loose tag 不复制，两端各加减不可能产生跨机残留 |
| `WindowEffectClass` | **仅权威端施加，依赖 GE 复制** | 两端各施一个 Infinite GE 会产生两个实例，取消时序错开就永久残留（这正是旧版删掉 `AdditionalGrantedTags` 的同一个理由，见 §3.11） |

### 3.10 重入防护（头等规则）

> **任何向 ASC 修改 Tag、施加/移除 GE、或派发 GameplayEvent 的调用，都是潜在重入点；
> 返回后必须确认 Task 仍然有效、未被停止。**

这条链路**全部同步**，每一环都在引擎源码里核实过：

```text
TickTask
  └─ AdvanceTimeline
       └─ (EnterWindow / ExitWindow / FirePointEvent)
            └─ ASC->HandleGameplayEvent        ← 同步广播（AbilitySystemComponent_Abilities.cpp:2521-2562）
            └─ ASC->ApplyGameplayEffectToSelf  ← 同步（可能改属性 → 广播 → 死亡 → 取消 Ability）
                 └─ 回调里 EndAbility()
                      └─ TaskOwnerEnded()      ← GameplayTask.cpp:146-165，同步
                           └─ 本 Task 的 OnDestroy(true)
                                └─ ClearAllWindowState()
            ← 返回：如果不检查，循环会继续消费剩余节点
```

引擎侧也有两处间接印证：

```cpp
// HandleGameplayEvent 内部：Make a copy in case it changes due to callbacks
TArray<TPair<FGameplayTagContainer, FGameplayEventTagMulticastDelegate>> LocalGameplayEventTagContainerDelegates = GameplayEventTagContainerDelegates;
```

```cpp
// UAbilityTask::OnDestroy 注释：If we have already been destroyed this is being called recursively
```

**不检查的后果**（不是"多跑一次"这种轻微问题）：

```text
OnDestroy → ClearAllWindowState()（账本已清空、tag 与 GE 已撤）
    ↓ 循环继续
EnterWindow(后面某个 Window) → 重新 AddLooseGameplayTag / ApplyGameplayEffectToSelf
    ↓
再没有任何人会清理它  →  Status.Attack.* 永久残留  →  角色卡死
```

**四处必须带守卫**：

| 位置 | 守卫写法 |
|---|---|
| `AdvanceTimeline` 的消费循环 | 每轮**调用前**与**调用后**各判一次 `bStopped`，成立即 `return` |
| `InitializeTimeline` 的两个循环 | 同上（起点事件同样能 EndAbility） |
| `TickTask` 的 `AdvanceTimeline` 之后 | 判 `bStopped`，成立即 `return`（不要再去判 `NaturalEnd`） |
| `OnDestroy` | **入口第一件事就是 `bStopped = true`**（整道防线的前提） |

`EnterWindow` / `ExitWindow` / `FirePointEvent` **内部**不需要额外的循环守卫，因为它们各自只做固定几步、且在最后一步之后立即返回。
但**它们内部每一步之后都要按 §3.11 的规则处理"已经清理过"的情况**。

**其余约束**

| 约束 | 原因 |
|---|---|
| `TickTask` 开头判 `bStopped` | 停止后仍在同一帧被回调时不要继续推进 |
| **`TickTask` 在累加 `LogicalElapsed` 之前 `return` 掉非正的 `WorldDelta`** | **这才是时钟倒退的真正防线**（见 §3.6）：必须在污染逻辑时间之前拦。`AdvanceTimeline` 里那句 `CurrentTime <= PreviousTime` 只是廉价的不变式校验，**它拦不住倒退** |
| `EnterWindow` 判重、`ExitWindow` 判存在 | 防止 tag 多加、GE 多施 |
| `ClearAllWindowState` 幂等 **且重入安全** | 会被调用两次；且撤 tag / 撤 GE 本身会触发可能重入的回调 |
| `StopTimeline` 内部 `return` 早退 | 重复停止不得重复广播 |
| 销毁后不得再调用依赖 `Ability` 的辅助函数（如 `IsLocallyControlled()`） | `UAbilityTask::OnDestroy` 会把 `Ability` 置空（`AbilityTask.cpp:130-135`），之后这些函数静默返回错误结果而不是崩溃（`IsLocallyControlled()` 实现为 `return Ability && ...`，`AbilityTask.cpp:212-215`）。**这正是必须靠 `bStopped` 立刻停止的原因：不能依赖辅助函数的容错** |
| `EndTask()` 可以放心重复调用 | 引擎用 `TaskState != Finished` 守卫 |

### 3.11 GameplayEffect 的重入、补偿与时长契约

Window 的 tag 可以"先记账再改 ASC"，因为账本本身就是"我要加这个 tag"。**GE 不行** —— 句柄要等施加完成才拿得到。这带来三条专门规则，缺一条就会要么留下残留、要么留下已经发生过的副作用。

**规则一：Tag 写入之后、施加 GE 之前，先检查 `bStopped || bCleanedUp`。**

```cpp
ActiveWindowIndices.Add(EventIndex);
ASC->AddLooseGameplayTag(Event.WindowTag, 1);      // ← 潜在重入点

if (bStopped || bCleanedUp)
{
    return;                                        // ← 防线一：干脆不要施加
}
```

缺少这道检查会怎样：Tag 回调里结束 Ability → `OnDestroy` 清空账本与全部状态 → 返回 `EnterWindow` → **仍然去 Apply 了一次 GE** → 紧接着又 Remove 掉。

最终状态确实干净，但这次 Apply 已经真实发生过，它可能触发：

```text
Attribute 变化 / GameplayCue / Tag 变化 / Delegate / OnActive / 其它 GE 的阻断重算
```

**这些副作用是 Remove 撤不回来的。** 所以"施加后补偿"不能替代"事前检查"，两道防线都要。

**规则二：施加后立刻入账；若发现已经清理过，立刻补偿移除。**

```cpp
const FActiveGameplayEffectHandle Handle = ApplyTimelineEffect(ASC, Event.WindowEffectClass);

if (Handle.IsValid())
{
    if (bCleanedUp)
    {
        // 施加期间 Ability 已结束、清理已跑过：账本里没有它 → 不补偿就永久残留
        ASC->RemoveActiveGameplayEffect(Handle);
        return;
    }

    WindowEffectHandles.Add(EventIndex, Handle);
}
```

这段是**可证明安全**的：`ApplyTimelineEffect` 返回之后，到 `bCleanedUp` 判断与入账之间**没有任何调用进入外部代码**（单线程、同步），所以不存在"检查完又被清理"的窗口期。

**规则三：施加失败（`Handle` 无效）不入账。**

`ApplyGameplayEffectSpecToSelf` 可能因为 Tag 阻断、免疫、能力未激活等原因失败并返回无效句柄。此时**不能记账**，否则 `ClearAllWindowState` 会去移除一个从未存在的实例（按句柄移除虽然是安全的空操作，但账本会失真、日志会误导）。入账条件就是 `Handle.IsValid()`。

**`ApplyTimelineEffect` 的实现**

```cpp
FActiveGameplayEffectHandle UHodgeAbilityTask_PlayTimeline::ApplyTimelineEffect(
    UAbilitySystemComponent* ASC, TSubclassOf<UGameplayEffect> EffectClass)
{
    if (!ASC || !EffectClass)
    {
        return FActiveGameplayEffectHandle();
    }

    // 只建一个"干净"的 Context：**不要** AddSourceObject(this)。
    // UHodgeAbilityTask_PlayTimeline 是调度实现细节，不是战斗语义上的"来源"；
    // 而且 PointEffectClass 允许 HasDuration —— GE 完全可能在本 Task 销毁后还活着，
    // 让一个已经结束的调度 Task 当 SourceObject 只会误导后续读代码的人。
    // 需要 Instigator / EffectCauser / Ability 时，应由 GA 侧提供 Context（见 §3.9 与附录 B）。
    FGameplayEffectContextHandle Context = ASC->MakeEffectContext();

    const FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(EffectClass, /*Level=*/1.f, Context);
    if (!Spec.IsValid())
    {
        // 类无效 / 未加载：当作施加失败，不入账。
        UE_LOG(LogTemp, Error, TEXT("[Hodge] ApplyTimelineEffect 构造 Spec 失败：%s"), *GetNameSafe(EffectClass));
        return FActiveGameplayEffectHandle();
    }

    return ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
}
```

> `Level` 本阶段固定为 1：Window 的等级语义（按连击段数提升？）属于第二阶段话题。
>
> 关于"GE 的来源身份"：本阶段**刻意不填 `SourceObject`**。调试用的 Timeline 身份已经有
> `EventID` / `Payload.OptionalObject` / 日志三条通道，不必污染 GE Context。
> 接入正式 GA 后，正确做法是让 GA 侧提供 Context（把 Ability 的 `Instigator` / `EffectCauser` / Ability 信息带进去），
> 见附录 B 第 8 项。

**时长契约（与 §4.1 的校验一一对应）**

| 位置 | 允许 | 理由 |
|---|---|---|
| `WindowEffectClass` | **必须 Infinite** | Window 的语义是 `[Start, End)` 内状态持续成立，而它靠 `ExitWindow` 撤销。若配 `HasDuration` 且短于窗口，GE 会在窗口中途自行到期 —— 这段时间的状态空洞直接违反 Window 的数据语义。**定 Error，不是 Warning** |
| `PointEffectClass` | Instant / HasDuration；**禁止 Infinite** | Point 只负责 Apply 这一次，此后生命周期不归 Timeline 管（见 §3.9）。`Instant` 立即结算、`HasDuration` 自行到期，都不会留下"没人收"的状态；`Infinite` 会，所以定 Error |

> 若只想让 GA 自己决定施加什么效果，`PointEffectClass` 留空、只配 `PointEventTag` 即可 ——
> 对伤害一类业务这也是更推荐的路径，因为"打谁、算多少"属于 GA / GE 的职责。

**关于 `ExitWindow` 的相反要求**

`EnterWindow` 在 Tag 之后要提前返回，`ExitWindow` **正相反**：它已经先把账本条目摘掉了，重入进来的清理不会再管这个窗口的 tag，所以无论 `bStopped` 是什么都必须自己把 tag 减掉。详见 §3.4.5 的注释 —— **"给所有 ASC 调用之后统一加守卫"这个做法是错的**，判断依据是"这个操作是否已经被记进账本"。

**为什么一条 Entry 只驱动一个 `WindowTag` + 至多一个 GE**

这是账本能够"按下标精确对应"的前提，也是我们把"一段区间授予多个标签"这条路封掉的原因：

- tag 侧：多标签会让 `EnterWindow` 变成"分批写入 ASC"，产生半授予窗口——第一批写成功、回调结束 Ability、清理把**尚未写入**的标签也一并撤销（吃掉别人的计数），返回后外层又把后半批加回去，而 Task 已经销毁没人清理；
- GE 侧：多 GE 需要多条句柄记录，且每一条都可能失败或触发重入，补偿逻辑会成倍复杂。

真出现"一段区间要驱动多个标签"的需求时，正确做法不是加回多标签字段，而是**再加一条同区间的 Window**（`Priority` 已经能表达同刻执行顺序）。这条约束让模型保持"一条 Entry = 一个语义 + 一个效果"的可读性。

---

## 4. 编辑器校验

### 4.1 `IsDataValid` 清单

| 级别 | 检查 |
|---|---|
| Error | `Duration <= 0` |
| Error | `EventID` 为空 |
| Error | `EventID` 在资产内重复 |
| Error | `StartTime < 0` **或** `StartTime > Duration`（所有 Kind 统一，见下面的时间范围） |
| Error | Window：`EndTime <= StartTime` |
| Error | Window：`EndTime > Duration` |
| Error | Window：`WindowTag` 无效（`!IsValid()`） |
| Error | Window：同一 `WindowTag` 的窗口之间区间重叠（tag 按计数维护，重叠会让计数语义含混） |
| Error | Window：`WindowEffectClass` 的 `DurationPolicy != Infinite`（见 §3.11 的时长契约） |
| Error | Point：`PointEventTag` 无效（`!IsValid()`） |
| Error | Point：`PointEffectClass` 的 `DurationPolicy == Infinite`（Timeline 不会再移除它，等于配出了一个永久状态） |
| Error | Point：配了 `PointEffectClass` 但 `NetPolicy == LocallyControlledOnly`（这个组合在多人下没有一致语义，见下） |
| Warning | 两条 Point 的 `StartTime`、`Priority` 完全相同（执行顺序只能靠数组下标决定，建议显式区分） |
| Warning | 同一 `WindowTag` 的两个窗口**首尾相接**（`Prev.EndTime == Next.StartTime`）：边界处该 Tag 会走 `1 → 0 → 1`，订阅方会真的收到一次 Removed / Added；若本意是一段连续区间，请合并为一条 Window。（相同 `WindowEffectClass` 相邻同理，会重新触发 Cue / `OnActive`） |
| Warning | `Events` 未按 `StartTime` 升序（运行期已不依赖有序，但排序便于人工核对）。⚠️ UE 5.5 的 `FDataValidationContext` 只有 `AddError` / `AddWarning` 两级、**没有 `AddInfo`**，所以这条只能落在 Warning |

> **时间范围（三条，干净且不依赖其它 Error 间接发现）**：
>
> ```text
> Point :  0 <= StartTime <= Duration
> Window:  0 <= StartTime < EndTime <= Duration
> ```
>
> `StartTime > Duration` 必须是 **Error**：推进被 `ElapsedTime = Min(StartOffset + LogicalElapsed, Duration)` 截断，
> 越界的 Entry **永远不可能被消费** —— 这是不可达配置，不是"可能配得不太合理"。
> 而 `Point.StartTime == Duration` 是**合法**的：自然结束的判定排在区间消费之后（§3.6），所以落在终点那一条会被消费到。
>
> **`PointEffectClass + LocallyControlledOnly` 为什么是 Error 而不是 Warning**：这个组合在多人下没有一致语义 ——
>
> ```text
> 远程玩家的 Owning Client ：LocallyControlled = true , Authority = false
>                          → bShouldFire = true（事件执行）
>                          → 但 GE 的闸门是 bIsAuthority → 不施加
>
> 服务器（专用服）        ：Authority = true , LocallyControlled = false
>                          → bShouldFire = false → 直接 return → 也不施加
>
> listen server 主机自己控制的角色：两者同时为真
>                          → 事件执行 + GE 施加
> ```
>
> 结果是 **Host 有、Remote 没有**。合法组合只有 `AuthorityOnly` 与 `LocalAndAuthority`（两者在专用服与 listen server 上行为一致）。
>
> 校验必须**按 `Kind` 分流**，只校验当前 Kind 的字段。隐藏字段（例如 `Kind == Point` 时的 `WindowTag`）可能残留切换前填过的值，
> 拿它报错只会误导。
>
> 两条**刻意不做**的事：① **不强制 Tag 命名空间**，不在校验里要求 `WindowTag` 必须属于 `Status.*`
> —— `Categories` 只是 Tag Picker 的层级过滤，不是契约，而一段区间完全可能合法地驱动 `Status.Movement.Dodging` 这类状态；
> ② 不检查"Point 是否落在某个 Window 区间内"（见下）。
>
> `EndTime > Duration` 与 `EndTime <= StartTime` 属于非法配置：前者永远无法正常退出，后者区间为空。

**刻意不加的一条**：不检查"Point 是否落在某个 Window 区间内"。
Window 与 Point 是两个独立概念，Point 完全可以合法地落在所有窗口之外：

```text
Window Active [0.20, 0.50)
Point  CameraShake @0.90      ← 合法：不需要任何窗口处于激活状态
```

### 4.2 `PostEditChangeProperty`

**只做一件事：把 `Events` 按 `StartTime` 稳定排序。**

> ⚠️ 必须用 **`TArray::StableSort`**（或 `Algo::StableSort`），不能用 `TArray::Sort`。
> 因为 `Index` 是运行期节点排序的最终兜底键：同刻、同 `Priority` 的两条 Entry，谁在数组里靠前谁先执行。
> `TArray::Sort` 是不稳定排序，会在"整理列表"时把这类条目的相对顺序悄悄换掉 —— **执行顺序会真的变**。
> 稳定排序只把不同时刻的条目归位，同刻条目维持原有先后 —— 作者在列表里的先后**就是**这个 tie-breaker 的表达方式
> （不需要额外字段），所以自动整理无权改动它。

不自动交换 `StartTime` / `EndTime`、不自动把 `EndTime` 钳到 `Duration`、不自动补 `EventID`。理由：

- 它与 §4.1 把这些情况定为 **Error** 自相矛盾——同一份数据不能既非法又被自动改对；
- 自动"猜意图"会把配置错误悄悄改掉，Author 永远不知道原始值写错了；
- 本设计的整体理念是**发现错误，不替作者猜意图**。

分工因此很干净：

```text
PostEditChangeProperty  → 排序（纯整理，不改语义）
IsDataValid             → 报告所有配置错误
```

### 4.3 不在本阶段做

自定义 Asset Editor、可视化时间轴、`SyncDurationFromMontage()`（无 Montage）、按 Notify 自动导入。编辑器依赖：`WITH_EDITOR` 分支需要 `UnrealEd`，项目 `Hodgepodge.Build.cs` 已经具备。

---

## 5. 前置：本阶段需要的 GameplayTag

按项目既有做法声明：`HodgeGameplayTags.h` 的 `namespace HodgeGameplayTags` 内用
`HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(...)`，`.cpp` 用 `UE_DEFINE_GAMEPLAY_TAG(...)` 定义。

常量名沿用项目现有写法（点号换成下划线）：`Status.Attack.Active` → `Status_Attack_Active`，
与既有的 `Status_Death_Dying`、`GameplayEvent_MeleeHit` 一致。

**Window 用的状态标签（位于 `Status.Attack.*` 下）**

```text
Status.Attack.Windup
Status.Attack.Active
Status.Attack.Recovery
```

**Point 用的事件标签（位于 `GameplayEvent.Attack.*` 下）**

```text
GameplayEvent.Attack.Test            // 本阶段验证用
GameplayEvent.Attack.Timeline.End    // Task 自动产生
GameplayEvent.Attack.Interrupted     // 仅当验证中断路径时需要
```

**父标签**（可选，但显式声明后能在编辑器的 Tag 列表里直接搜到；中间层父级会随子标签隐式建立，
项目现有做法也不会单独声明 `Status` / `GameplayEvent` 这类根标签）：

```text
Status.Attack
GameplayEvent.Attack
```

> 现状核对：`HodgeGameplayTags.h` 中搜不到 `Attack`（0 命中）；`Config/DefaultGameplayTags.ini`
> 目前只有 `a` 与 `Ability.Attack` 两条。这两组标签**都不存在**，必须作为第 ① 步补上。

---

## 6. 开发顺序

### 6.1 七步

| 步骤 | 内容 | 出口条件 |
|---|---|---|
| ① | 标签：上表全部标签，声明到 `HodgeGameplayTags.h` / `.cpp` | 编译通过；编辑器 GameplayTag 列表里可见；无重复定义 |
| ② | 数据类型：`EHodgeTimelineEventKind` / `FHodgeTimelineEvent` / `EHodgeTimelineEventNetPolicy` / `UHodgeAbilityTimeline` | 资产可在内容浏览器创建；`Events` 列表按 `EventID` 显示、按 `Kind` 隐藏无关字段 |
| ③ | 编辑期：`IsDataValid`（§4.1 全量）+ `PostEditChangeProperty` 排序 | 故意配错时间 / 空 `EventID` / 重复 `EventID` / 越界区间能报出 |
| ④ | Task：构造函数 / `PlayTimeline` 工厂 / `Activate` / `InitializeTimeline` / `TickTask` / `CollectNodes` / `SortNodes` / `AdvanceTimeline` / `EnterWindow` / `ExitWindow` / `FirePointEvent` / `ApplyTimelineEffect` / `StopTimeline` / `ClearAllWindowState` / `OnDestroy` | 编译通过 |
| ⑤ | 测试 Timeline 资产（§6.2）+ 一个测试用 Infinite GE 资产 | 校验无 Error |
| ⑥ | 最小消费方 GA（§6.3） | 能在 PIE 里打印事件顺序与窗口状态 |
| ⑦ | 暴力测试（§6.4） | 每个场景结束后 `Status.Attack.*` 的 Count 均为 0、无遗留 Active GE |

### 6.2 测试 Timeline 配置

```text
Duration = 1.00

#1 Window  EventID=Windup     WindowTag=Status.Attack.Windup      [0.00, 0.20)   Priority=0
#2 Window  EventID=Active     WindowTag=Status.Attack.Active      [0.20, 0.50)   Priority=0
#3 Window  EventID=Recovery   WindowTag=Status.Attack.Recovery    [0.50, 1.00)   Priority=0
#4 Point   EventID=Test       PointEventTag=GameplayEvent.Attack.Test   t = 0.35   Priority=0
```

> 每条 Entry 只需要 `EventID`（列表显示名）+ 当前 Kind 的字段，`DisplayName` 已删除。

**另外加两条专门测边界的配置**（用第二个测试资产，别混在同一条里）：

```text
A) 同刻顺序：
   Window EventID=A WindowTag=Status.Attack.Active       [0.20, 0.35)
   Point  EventID=B PointEventTag=GameplayEvent.Attack.Test   t = 0.35
   → 期望顺序：WindowEnd → Point；Point 收到时 Active == false

B) 带 GE 的窗口 + 同刻 Priority：
   Window EventID=Armor WindowTag=Status.Attack.Active  [0.20, 0.50)
          WindowEffectClass=GE_TestInfinite                 （必须是 Infinite）
   Point  EventID=P1 PointEventTag=GameplayEvent.Attack.Test   t = 0.30  Priority=1
   Point  EventID=P2 PointEventTag=GameplayEvent.Attack.Test2  t = 0.30  Priority=0
   → 期望：Active 窗口进入时 GE 施加；0.30 处 P2 先于 P1 执行；
          窗口退出时 GE 被移除（该 GE 的 Active 实例数归零）

C) 初始化顺序（验证 §3.5 的修正）：
   Window EventID=Armor WindowTag=Status.Attack.Active  [0.20, 0.50)
   Point  EventID=P1 PointEventTag=GameplayEvent.Attack.Test   t = 0.30  Priority=1
   Point  EventID=P2 PointEventTag=GameplayEvent.Attack.Test2  t = 0.30  Priority=0
   → 期望：PlayTimeline(StartOffset=0.30) 的**调度顺序**与"从 0 跑到该时刻"一致
          （Active 恢复 → P2 → P1），而不是数组顺序 P1 → P2
          ⚠️ 只要求调度顺序一致；不要求此前的 GE / Cue / Period 等历史副作用一致（§3.5）
```

### 6.3 最小消费方 GA

目的只是"让 Task 有调用者、让事件可见"，不实现任何攻击逻辑：

```text
UHodgeGameplayAbility 的最简子类（测试用，后续可删）
    ActivateAbility
      ├── 订阅 GameplayEvent.Attack（父标签 + OnlyMatchExact = false）
      ├── 打印窗口标签的 Count
      ├── PlayTimeline(Timeline, StartOffset = 0)
      │     └── 立即 ReadyForActivation()          ← 漏了这句 Task 不跑，且没有任何报错
      └── 收到事件 → 打印 EventTag + EventMagnitude + 当前 Status.Attack.* 状态
```

```cpp
// C++ 侧必须显式激活（见 3.2）
UHodgeAbilityTask_PlayTimeline* Task =
    UHodgeAbilityTask_PlayTimeline::PlayTimeline(this, Timeline, 0.f, 1.f);
Task->ReadyForActivation();
```

- **订阅必须先于 `PlayTimeline`**：`Activate()`（由 `ReadyForActivation()` 触发）会立刻派发起点事件，订阅晚了就丢。
- 订阅要监听**父标签**并关闭精确匹配，否则收不到子标签。
- 这个测试 Ability 是否长期保留、叫什么名字，属于[附录 B](#附录-b待确认事项)的待确认项。

### 6.4 验收场景与判定

每个场景结束后都检查：**ASC 上 `Status.Attack.*` 的 Count 全部为 0，且没有遗留的测试 GE 实例。**

| 场景 | 操作 | 判定 |
|---|---|---|
| 正常走完 | 主配置，`StartOffset = 0` | Point 按时间各触发一次；窗口 tag 进出配对；`Timeline.End` 恰好一次；结束后状态全清 |
| 同刻顺序 | 配置 A（`EndTime == Point.StartTime`） | 顺序为 WindowEnd → Point；Point 内读到 Active == false |
| 同刻 Priority | 配置 B 的两条 0.30 Point | `Priority` 小的先执行 |
| 窗口 GE 配对 | 配置 B | 进入窗口时 GE 施加（Active GE 数 +1）；退出时移除（归零）；取消 Ability 时同样归零 |
| 中途取消 | 在窗口激活期间 `CancelAbility` | `OnDestroy` 把 tag 与 GE 都清到 0；不广播 `Timeline.End` |
| 起点接续 | `StartOffset = 0.25`（落在 Active 中间） | 启动瞬间 Active tag 已存在、窗口 GE 已施加；`0.25` 之前的 Point **没有补发**。⚠️ 这里断言的是"**状态已重建 + 调度顺序正确**"，**不是**"与从 0 跑等价"：GE 的 Apply / Period / Cue 都从 `StartOffset` 才开始（§3.5） |
| 起点恰好等于事件时刻 | `StartOffset = 0.35` | `0.35` 的 Point 触发一次，之前的 Point 不触发 |
| 跨帧消费 | 制造一次跨过窗口边界与 Point 的大 Δt | 三个节点按时间顺序消费；Point 看到正确的窗口状态 |
| 时钟倒退 | PIE 里不易自然触发。两种可行做法：① 在 Replay 里向后拖动时间轴（项目已有 `AHodgeReplayPlayerController`）；② 临时加一个 `#if !UE_BUILD_SHIPPING` 的调试入口把 `LastUpdateWorldTime` 推到未来，再继续 Tick | 该帧**不消费任何节点**（`Status.Attack.*` 保持原状、无事件派发）；**已触发过的 Point 不会被重复派发**；随后按新时钟继续推进并正常结束。若出现"Point 执行两遍"或"Ability 迟迟不结束"，说明守卫又被挪到累加之后了 |
| 连续 20 次 | 反复激活 / 取消 | 每次结束都归零，无累积残留、无 GE 泄漏 |

**最低放行门槛**：上表里**前四个 + 中途取消 + 起点接续**必须先单独跑死，再进下一阶段接 Montage：

```text
正常结束
同刻顺序（EndTime == Point 时刻）
窗口 GE 施加 / 移除配对
CancelAbility 中断
t.MaxFPS = 5 跨多个窗口与 Point
        ↓ 每次结束
Status.Attack.* Count == 0 且 Active GE 数 == 0
```

**关于"跨帧消费"怎么制造**：优先用**降低帧率**（例如临时把 `t.MaxFPS` 调到 5）而不是依赖断点。断点造成的 Δt 是否被引擎的时间步长上限裁剪取决于当前设置；帧率降低能稳定产生"一帧跨多个节点"的情形。两种方式判定标准相同：Point 看到的状态必须与"该时刻的区间定义"一致。

### 6.5 文件落位（建议）

| 文件 | 位置 |
|---|---|
| `HodgeAbilityTimeline.h/.cpp`（含事件结构、kind 与 NetPolicy 枚举） | `Source/Hodgepodge/Public|Private/Data/` |
| `HodgeAbilityTask_PlayTimeline.h/.cpp` | `Source/Hodgepodge/Public|Private/AbilitySystem/Abilities/` |

理由：项目现有布局按"性质"切目录 —— 数据资产类型集中在 `Data/`（`HodgeAbilitySet` / `HodgePawnData` / `HodgeGameData` 都在此），Ability 相关代码在 `AbilitySystem/Abilities/`。
母文档 §4.6 把 Timeline 放在 `AbilitySystem/Timeline/`、把同样性质的 `UHodgeComboSet` 放在 `Data/`，两者不一致；本文件按项目现状统一。若最终决定沿用母文档的目录，仅需改这两个路径。

---

## 7. 与 Montage 的分工（第二阶段的前置说明）

本阶段不播动画，但接口是按"接管 AN / ANS"设计的，提前把边界写清以免第二阶段返工。

### 7.1 迁移后 Montage 里应该只剩什么

```text
分离前：ANS_ComboWindow → 发 GameplayEvent → GA 接
分离后：Timeline 的 Window → Task 发 GameplayEvent → GA 接

Montage 里只保留：
  - 纯表现类 Notify：脚步声、武器拖尾、特效挂点、材质参数
  - 动画层自身需要的标记：ALS 的状态切换、转身标记
```

**逻辑 Notify 一律移除。** 同时保留两套时间源会得到"双时间源互相打架"的更糟状态，排查时也不知道以哪个为准。

### 7.2 本阶段不解决的两个问题（第二阶段必须重审）

| 问题 | 说明 |
|---|---|
| 逻辑时间与动画时间漂移 | Timeline 走 `GetWorld()->GetTimeSeconds()` 差值，Montage 走 AnimInstance 播放位置。两者是两套时钟；`InitialPlayRate` 相同**不能**保证同步。接入 Montage 时必须明确可接受漂移范围，并加一个调试观察值（`ElapsedTime` vs Montage 位置） |
| Window 与 Montage 的中断联动 | Montage 被抢占后 Timeline 是否继续跑，是第二阶段的"中断策略"话题。当前 Task 提供 `StopTimeline(Interrupted)` 作为通道，但不自己决定 |

---

## 8. 已知边界与风险

| 项 | 说明 |
|---|---|
| **Tick 的宿主** | AbilityTask 由 GAS / GameplayTasks 体系调度，对 AbilityTask 来说它的 tasks component **就是 ASC 自己**：`UAbilitySystemComponent` 继承自 `UGameplayTasksComponent`（`AbilitySystemComponent.h:113`），`UGameplayAbility::GetGameplayTasksComponent()` 也直接返回 `ActorInfo->AbilitySystemComponent`（`GameplayAbility.cpp:1510-1513`）。**不要**写成"Avatar 上的 `GameplayTasksComponent`"（本项目 ASC 在 PlayerState 上）。实现后第一件事是打日志确认 `TickTask` 每帧真的进入 |
| **暂停语义** | 推进用世界时间差，因此游戏暂停时 Timeline 不前进（预期行为，但要写进验收） |
| **世界时钟倒退 → 冻结一帧** | 关卡切换、世界时间重置、Replay 拖动时间轴都会让 `GetTimeSeconds()` 倒退。此时 Timeline **冻结一帧**（不修改逻辑时间、不消费节点），下一帧按新时钟继续；由于 `ElapsedTime` 单调不减，已消费的节点**不会重放、也不会重复派发**（§3.6）。代价是**不支持时间轴跟着回放倒退** —— 若将来 Replay 真需要，必须显式设计，不能只删掉这道守卫 |
| **卡顿会补放事件** | 见 §3.6。若将来需要"卡顿后跳过表现类 Point"，那是独立策略，不要靠缩小区间来修 |
| **两套时钟（第二阶段风险）** | 见 §7.2 |
| **模拟代理没有窗口 tag** | loose tag 不复制，远端玩家身上不会有 `Status.Attack.*`。项目 `UHodgeAnimInstance` 通过 `FGameplayTagBlueprintPropertyMap` 绑定 Tag 变化（`HodgeAnimInstance.cpp:24`），**接动画层前必须实测远端是否依赖这些标签**；若需要，正确做法是另开"观察者面向"的复制状态，而不是把本地 tag 改成复制的 |
| **Window GE 只在权威端** | 远端模拟代理通过 GE 复制拿到该状态；若某状态需要客户端预测，需要单独设计，不要改成两端各自施加 |
| **`NetPolicy` 无法在本阶段完整验证** | 单进程下"服务器 / 本地控制端"两个条件同时为真，三种策略都会派发。真正的分派验证需要 listen server + 第二个客户端，列为未验证 |
| **`Interrupted` 目前没有触发者** | 本阶段没有 Montage，中断只能由外部主动调 `StopTimeline(Interrupted)` 验证；Ability 被取消走 `OnDestroy`，不经过 `StopTimeline` |
| **`EndTask()` 后不要访问成员** | `EndTask()` 可能同步触发 `OnDestroy` |

---

## 9. 对现有实现的影响

工作区里已有一版按 `Phase + Event` 双数组写的实现（`HodgeAbilityTimeline.*`、`HodgeAbilityTask_PlayTimeline.*`）。
采用本模型后需要按下面清单改写；**时间推进、区间语义、重入防护、清理幂等这些结论全部保留**，改的是数据模型与节点收集。

| 文件 / 位置 | 改动 |
|---|---|
| `HodgeAbilityTimeline.h` | 删除 `FHodgeTimelinePhase`；`FHodgeTimelineEvent` 改为统一结构（新增 `Kind` / `EventID` / `Priority` / `WindowTag` / `PointEventTag` / `WindowEffectClass` / `PointEffectClass`，`Time` 改名 `StartTime`）；新增 `EHodgeTimelineEventKind`；删除 `TArray<FHodgeTimelinePhase> Phases`；**不再提供 `GetActiveWindows`** |
| `HodgeAbilityTimeline.cpp` | `IsDataValid` 按 §4.1 重写（按 `Kind` 分流 + `EventID` 唯一性 + 同 `WindowTag` 窗口重叠 + Window GE 必须 Infinite / Point GE 禁止 Infinite + `LocallyControlledOnly` 配置陷阱）；`PostEditChangeProperty` 改按 `StartTime` **稳定**排序 |
| `HodgeAbilityTask_PlayTimeline.h` | `EnterPhase/ExitPhase` → `EnterWindow/ExitWindow`；`FireEvent` → `FirePointEvent`；`ClearAllPhaseTags` → `ClearAllWindowState`；新增 `WindowEffectHandles` / `bCleanedUp` / `ApplyTimelineEffect` / `CollectNodes` / `SortNodes`；`FHodgeTimelineNode` 与 `EHodgeTimelineNodeKind` 移到头文件；`ActivePhaseIndices` → `ActiveWindowIndices` |
| `HodgeAbilityTask_PlayTimeline.cpp` | 节点收集改为单数组扫描（每个 Window 最多 2 个节点）；排序键加 `Priority`；**`InitializeTimeline` 改为复用 `CollectNodes` / `SortNodes`**（修正早先"从起点接入不遵守 `Priority`"的缺陷）；**`TickTask` 把 `WorldDelta <= 0` 的判定提到累加 `LogicalElapsed` 之前**（修正时钟倒退污染逻辑时间的缺陷，§3.6）；`EnterWindow` 增加"Tag 之后先查 `bStopped`"这道防线；窗口进入 / 退出增加 GE 施加与移除（含 §3.11 的补偿规则）；清理同时处理两个账本 |
| `HodgeGameplayTags.*` | 不需要改（本阶段标签集不变） |

**代码尚未按本文件改写**（见 §10）。

---

## 10. 验证边界

- 本文件是**设计草案**，编写时**未编译、未生成工程、未运行编辑器、未做 PIE、未联机、未打包**。
- 文中"已核实"的引擎行为都给出了文件与行号；其余"未知 / 待确认"条目一律按未确认处理。
- 交付汇报必须**分别标记** C++ 构建 / 蓝图编译 / PIE / 联机 / 打包的"通过 / 失败 / 未执行"（见 [AI 开发与验证流程](../AI_DEVELOPMENT.md) 与 [项目开发约定](../../AGENTS.md)）。
- 新增 C++ 文件后按需刷新 IDE 工程，不手工修改 `.sln` 与 `Intermediate` 生成物。

---

## 附录 A：与旧版（`Phases` + `Events` 双数组）的差异

| # | 旧版 | 本版 | 原因 |
|---|---|---|---|
| 1 | `Phases[]`（区间，`PhaseTag`）与 `Events[]`（时间点，`EventTag`）两个平行数组 | 单一 `Events[]`，用 `Kind` 区分 Window / Point | 两张表的相对顺序不可见，作者得手工归并；统一后是"**一个列表描述全部 Entry，运行期展开成节点再统一排序**"（**不等于**列表顺序即执行顺序，见 §0.3） |
| 2 | 旧版为跨数组同刻顺序发明了"`Exit → Enter → Event`"规则 | 该规则变为"三种节点的固有顺序"，不再是补丁 | 数据模型给对了位置，约定自然成立 |
| 3 | 一段区间只能驱动一个 `PhaseTag`（曾用 `AdditionalGrantedTags` 打补丁，后又因重入问题删除） | Window 同时驱动 `WindowTag` + `WindowEffectClass` | ANS 天然就是"状态 + 附加效果"；旧模型没给这个需求留位置 |
| 4 | 区间只能发状态，无法表达"这段时间带霸体"这类需要复制的战斗事实 | 新增 `WindowEffectClass`（Window 权威端施加并复制） | 补上"本地表现（loose tag）"与"战斗事实（GE）"的分工 |
| 5 | 无事件身份，只能靠下标 | `EventID`（`FName`，资产内唯一，兼作列表显示名） | 日志可定位、跨端可断言；**不用于业务判定**（§2.6）。因此不再引入 `DisplayName` —— 其"留空回退"并不存在 |
| 6 | 无显式顺序控制，同刻同类只能靠数组下标 | `Priority`（同刻同类比较） | 作者意图可表达；同时保留下标兜底保证跨端确定 |
| 7 | `Time` | `StartTime` / `EndTime` | 与 Window 语义统一；Point 只用 `StartTime` |
| 8 | 校验清单 9 条 | 按 `Kind` 分流 + `EventID` 空 / 重复 + 同 `WindowTag` 窗口重叠 + Window GE 必须 Infinite + Point GE 禁止 Infinite + `LocallyControlledOnly` 陷阱 | 与新的记账模型（tag 下标账本、GE 句柄账本）对齐 |
| 9 | 清理只处理 tag 账本 | 清理处理 tag **与** GE 两个账本，并新增 GE 的"施加后补偿"规则（§3.11） | GE 句柄"施加后才有"，无法先记账 |
| 10 | `EnterWindow` 只有"施加后补偿"一层防护 | 增加"Tag 之后、施加之前先查 `bStopped \|\| bCleanedUp`"（§3.11 防线一） | 补偿只能撤销最终状态，撤不掉已经发生过的 `OnActive` / Cue / 属性变化 |
| 11 | `InitializeTimeline` 用 `GetActiveWindows` 逐个 `EnterWindow` | 初始化与 Tick 共用 `CollectNodes` / `SortNodes` | 原实现只保证**集合**正确、**顺序**会与"从 0 跑"不一致，而顺序可观测（§3.5） |
| 12 | 编辑器排序用普通 `TArray::Sort` | 改为 `StableSort` | `Index` 是运行期最终兜底键，不稳定排序会静默改掉同刻同优先级条目的执行顺序 |
| 13 | 单一 `EventTag` + 单一 `GameplayEffectClass` | 拆成 `WindowTag`/`PointEventTag` 与 `WindowEffectClass`/`PointEffectClass` | ① `Categories` 会过滤 Tag Picker，单字段下 Point 选不到 `GameplayEvent.*`；② 两种 Kind 的运行期语义与 GE 生命周期契约都不同 |
| 14 | `StartTime > Duration` 记为 Warning | 升为 **Error**（所有 Kind 统一） | 推进被 `Min(..., Duration)` 截断，越界 Entry **永远不可能被消费** —— 这是不可达配置，不是风格建议 |
| 15 | `PointEffectClass + LocallyControlledOnly` 记为 Warning | 升为 **Error** | 这个组合在多人下没有一致语义：Host 会施加、Remote 不会（见 §4.1 的三段推导） |
| 16 | 校验只列"非法项"，没显式给出合法范围 | 显式写出 `Point: 0 <= StartTime <= Duration`、`Window: 0 <= StartTime < EndTime <= Duration` | 让"什么算合法"从一张错误清单变成一条可读规则，不必靠别的 Error 间接推出来 |
| 17 | 未定义 Point 内部 Event 与 Effect 的先后 | 写死 **Event → Effect**；事件处理者结束 Ability 时 Effect 不施加（§2.2 / §3.9） | 不写清就会被默认成"同一时刻配的两样都会执行"，而实际不是 |
| 18 | `ApplyTimelineEffect` 用 `AddSourceObject(this)` | **不再把 Task 当 `SourceObject`** | Task 是调度实现细节；`PointEffectClass` 允许 `HasDuration`，GE 可能比 Task 活得更久 |
| 19 | §3.5 声称与"从 0 跑"语义一致 | 收窄为"**当前状态重建的调度顺序**一致，**不保证历史副作用等价**"，并给出 `StartOffset` 的正式定义 | 窗口 GE 的 `OnActive` / Cue / 周期在接续时以 `StartOffset` 为实际发生时刻，历史不等价 |
| 20 | 校验没有覆盖"同 `WindowTag` 首尾相接" | 新增 Warning | 边界处 Tag 会 `1 → 0 → 1`，订阅方真的会看到一次 Removed/Added；相同 GE 相邻还会重触发 Cue / `OnActive` |

**直接继承、未改动的部分**：Timeline 不做业务判断的判据（§1.3）、绝对秒而非归一化（§2.5）、
`[Start, End)` 与 `(Previous, Current]` 的区间定义、`NaturalEnd` 必须在最后一次 Advance 之后（§3.6）、
`OnDestroy` 无条件清理与严格配对（§3.7）、loose tag 而非 GE 来驱动本地状态（§3.9）、
`NetPolicy` 必须只派发一次（§3.9）、`ReadyForActivation()` 要求（§3.2）、重入防护（§3.10）。

---

## 附录 B：待确认事项

| # | 事项 | 说明 |
|---|---|---|
| 1 | 文件落位 | §6.5 按项目现状给出建议；若沿用母文档 §4.6 的目录，需要明确取舍理由 |
| 2 | 测试用消费方 GA 的形态 | 单独一个 C++ 测试子类（可重复、可删），还是直接给 `Content/Main/Character/Hero/GA_Attack` 指定 C++ 父类并在蓝图里接线 |
| 3 | Window 的 GE 是否允许"值/曲线"配置 | 当前只存 `TSubclassOf<UGameplayEffect>`；若要按窗口调参，需要额外字段（SetByCaller 或曲线） |
| 4 | Point 是否需要"延迟到下一帧"选项 | 当前 Point 在到达时刻立即同步派发；若某类表现需要延后一帧，应另开机制而不是复用 Point |
| 5 | `NetPolicy` 验证是否纳入本阶段 | 需要 listen server + 第二个客户端；不纳入则本阶段结束时该项仍是"未验证" |
| 6 | 是否需要"跳过表现类 Point"的降级策略 | 当前行为是"卡顿后补放全部节点"。若有观感问题再引入分级策略 |
| 7 | ~~`PointEffectClass` 是否再收成只允许 Instant~~ **已定：不收** | 保留 `Instant` / `HasDuration`，禁 `Infinite`。理由：真正的安全边界是"没人回收"（Infinite），而 `HasDuration` 自终止。`Point @0.35 → GE_MoveSpeedUp_5s` 是合理需求 —— Timeline 只负责"何时 Apply"，GE 自己负责"何时结束"，职责不冲突 |
| 8 | `ApplyTimelineEffect` 的 Context 由谁提供 | **已定：Task 不作为 `SourceObject`**（它是调度细节，且 Point GE 可能比 Task 活得久）。本阶段用 `ASC->MakeEffectContext()` 不填来源；接入正式 GA 后改由 GA 侧提供携带 `Instigator` / `EffectCauser` / Ability 的 Context。仅剩的开放点是"GA 侧怎么暴露这个 Context"（`UGameplayAbility::MakeEffectContext` 的访问级别待确认） |
| 9 | `WindowTag` 的 `Categories` 是否保留 | 当前设 `Categories="Status"` 只为编辑器过滤。若出现"窗口驱动非 `Status.*` 状态"的合法用法，去掉该 metadata 即可（校验不依赖它） |

---

## 附录 C：变更记录

| 日期 | 内容 |
|---|---|
| 2026-09-17 | 首次拆出；确定第一阶段范围、时间节点序列算法、初始化措辞修正与实现细节 |
| 2026-09-17 | 第二轮补强：补 `EHodgeTimelineStopReason` 定义；加入重入防护（`OnDestroy` 置 `bStopped`、四处守卫、清理幂等）；更正 Tick 宿主描述；`PostEditChangeProperty` 收窄为只排序；删除"事件未被 Phase 覆盖"告警 |
| 2026-09-18 | 第三轮补强：删除 `AdditionalGrantedTags`；明确 C++ 侧必须 `ReadyForActivation()`；增加最低放行门槛 |
| 2026-09-18 | **数据模型重写**：`Phases` + `Events` 双数组 → 单一 `Events` 数组（`Kind = Window / Point`）；新增 `EventID` / `DisplayName` / `Priority` / `GameplayEffectClass`；`EnterPhase/ExitPhase` → `EnterWindow/ExitWindow`；清理扩展为 tag + GE 两个账本；新增 §3.11 GE 重入与失败补偿；新增 §7 与 Montage 的分工；补充 §9 对现有实现的影响 |
| 2026-09-18 | **第四轮（评审修正）**：① Tag 与效果类按 `Kind` 拆成 `WindowTag` / `PointEventTag` 与 `WindowEffectClass` / `PointEffectClass`（`Categories` 会真的过滤 Tag Picker，单字段下 Point 选不到 `GameplayEvent.*`）；② 删除 `DisplayName`，改为 `TitleProperty=EventID`；③ `EventID` 用途收窄为"编辑器 / 日志 / Trace / 跨端断言"，禁止 Gameplay 分支（新增 §2.6）；④ **`InitializeTimeline` 改为复用 `CollectNodes` / `SortNodes`**，修正"从起点接入不遵守 `Priority`"的缺陷；⑤ `EnterWindow` 增加"Tag 之后先查 `bStopped`"这道防线，并写明 `ExitWindow` 的相反要求；⑥ Window GE 必须 Infinite 由 Warning 升为 **Error**，同时 Point GE 禁止 Infinite；⑦ `ApplyWindowEffect` → `ApplyTimelineEffect` 并补进接口；⑧ 纠正 §0.3 的论证（同刻顺序与数组形态无关）与"列表顺序即执行顺序"的错误表述；⑨ `PostEditChangeProperty` 明确用 `StableSort` |
| 2026-09-18 | **第五轮（评审修正，模型定型）**：① `StartTime > Duration` 由 Warning 升为 **Error**（所有 `Kind` 统一）并显式给出合法时间范围；② `PointEffectClass + LocallyControlledOnly` 由 Warning 升为 **Error**（Host 施加 / Remote 不施加，没有一致语义）；③ §3.5 收窄为"**接续语义 ≠ 历史重放**"，并给出 `StartOffset` 的正式定义；④ `ApplyTimelineEffect` **不再 `AddSourceObject(this)`**；⑤ 写死 Point 内部 **Event → Effect** 顺序，以及"事件终止 Ability 时 Effect 不施加"；⑥ 新增"同 `WindowTag` 首尾相接"的 Warning；⑦ `ExitWindow` 改用入参时取到的局部 ASC 指针 + `IsValid` 守卫（不再重新 `Get()`，也避免悬垂裸指针）；⑧ 附录 B 第 7 项（Point GE 是否收成只允许 Instant）判定为**不收**。**至此数据模型定型，下一步是按本文件重写代码** |
| 2026-09-18 | **第六轮（实现 Bug 修正）**：`TickTask` 的时钟倒退防线原本写在 `AdvanceTimeline` 里（`CurrentTime <= PreviousTime` 才 return），**拦不住** —— 负的 `WorldDelta` 已经把 `LogicalElapsed` / `ElapsedTime` 污染成负数，此后 `CurrentTime` 重新单调增长，早期节点会被再次跨过（Point 重复派发），且 `ElapsedTime` 要几百秒才能爬回 `Duration`（Ability 远超预期时长挂着）。改为在累加**之前** `if (WorldDelta <= 0.f) return;`，并**先更新 `LastUpdateWorldTime`** 以免永久冻死；这同时让 §3.3 声明的"`ElapsedTime` 单调不减"真正成立。另修两处旧文案：§3.4.3 的"用查询建立初始状态"改为"接续语义 + 统一 Scheduler"；§6.2 用例 C 的"与从 0 跑完全一致"收窄为"调度顺序一致" |
