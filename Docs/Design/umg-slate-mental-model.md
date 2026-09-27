# UMG 与 Slate：读懂 Indicator 系统所需的最小心智模型

> **文档类型：入门说明 / 学习笔记**（不是设计草案，也不是功能声明）。
> 基线 2026-09-26。所有引擎侧对应关系均在 `E:\UE\UE_5.5` 源码中核实，项目侧结论带文件与行号。
> 目的：把"只会写 `UUserWidget`"和"看懂 `SPanel / Slot / OnArrangeChildren`"之间的那道坎填上。

**相关文档**

- [Indicator UI 系统：职责边界与完整流程](indicator-ui-system.md)（本文的下一步）
- [Lyra UI 迁移文件导览](lyra-ui-file-guide.md)

---

## 0. 一句话

> **`SActorCanvas` 是一个真正存在于 UI Widget Tree 里的 Slate 容器（自定义 `SPanel`），不是管理器。** 它出生在 `UIndicatorLayer::RebuildWidget()` 里；`UIndicatorLayer` 才是你要放进 UMG 蓝图的那个东西。

最容易犯的错：把 `SActorCanvas` 当成"IndicatorManager 那一类的东西"，然后去找它挂在哪。

## 1. UMG 与 Slate 的两层关系

| 层 | 世界 | 基类 | 你平时接触到 |
|---|---|---|---|
| 上层 | UObject / Blueprint | `UWidget` / `UUserWidget` | 蓝图里拖控件、写 `UPROPERTY(meta=(BindWidget))`、配 Details |
| 下层 | C++ UI Framework | `SWidget` | 需要自定义布局或自绘时才会碰到 |

对应关系（**均已在引擎源码中核实**）：

| UMG | Slate | 证据（`Engine/Source/Runtime/UMG/Private/Components/`） |
|---|---|---|
| `UUserWidget` | `SObjectWidget`（外加 `UUserWidget` 生成的控件树） | `UserWidget.cpp` 里大量 `MyGCWidget` |
| `UCanvasPanel` | `SConstraintCanvas` | `CanvasPanel.cpp`：`MyCanvas = SNew(SConstraintCanvas);` |
| `UTextBlock` | `STextBlock`（外层还套了 `SInvalidationPanel` 做失效优化） | `TextBlock.cpp`：`SNew(SInvalidationPanel) ... SNew(STextBlock)` |
| `UButton` | `SButton` | `Button.cpp`：`MyButton = SNew(SButton);` |
| `UImage` | `SImage` | `Image.cpp`：`MyImage = SNew(SImage);` |
| `UProgressBar` | `SProgressBar` | `ProgressBar.cpp`：`MyProgressBar = SNew(SProgressBar);` |
| `UOverlay` | `SOverlay` | `Overlay.cpp`：`MyOverlay = SNew(SOverlay);` |
| `UHorizontalBox` | `SHorizontalBox` | `HorizontalBox.cpp` |
| `UBorder` | `SBorder` | `Border.cpp` |
| **`UIndicatorLayer`** | **`SActorCanvas`（项目自定义）** | `IndicatorLayer.cpp:36`：`MyActorCanvas = SNew(SActorCanvas, ...)` |

**所以「这个按钮占多大、放哪、鼠标点中没有、怎么 Paint」这些问题的答案，最终都在 Slate 那一层。** UMG 负责的是把它包成 UObject / 蓝图可用的样子（属性同步、GC、序列化、`BindWidget` 等）。

## 2. 为什么平时完全感觉不到 Slate

因为 `UWidget` 已经把 `RebuildWidget()` 这层封装做完了：你在蓝图里放控件 → UMG 构造对应的 Slate 控件 →

```text
UCanvasPanel    ──RebuildWidget()──→  SConstraintCanvas
UButton         ──RebuildWidget()──→  SButton
UTextBlock      ──RebuildWidget()──→  STextBlock
```

**你一直都在"Slate 之上的封装层"工作**，只是不需要关心下面。Indicator 之所以让你突然掉到 Slate，是因为它的布局规则**无法用现成的 UMG 容器表达**（见 §9）。

## 3. 读懂这套代码只需要 5 个概念

| # | 概念 | 一句话 | 在项目里的实例 |
|---|---|---|---|
| 1 | `SWidget` | Slate 世界的控件基类，地位相当于 UMG 的 `UWidget` | 一切 Slate 控件 |
| 2 | `SPanel` | **能容纳多个 Child 并负责它们的布局**的 Slate 控件 | `SActorCanvas` |
| 2b | `SLeafWidget` | 没有 Child、自己画自己的 Slate 控件 | `SCircumferenceMarkerWidget` |
| 3 | `Slot` | Panel 为每个 Child 保存的布局 / 运行时数据 | `SActorCanvas::FSlot`、`FArrowSlot` |
| 4 | `OnArrangeChildren()` | Panel 决定每个 Child「放哪、多大」 | `SActorCanvas.cpp:505-794` |
| 5 | `OnPaint()` | 布局完成之后，真正把 Child 画出来 | `SActorCanvas.cpp:795-854` |

## 4. 两种 Slate 自绘范式（项目里各有一个实例）

```text
SWidget
   │
   ├── SLeafWidget                      ← 没有 Child，自己画自己
   │       └── SCircumferenceMarkerWidget（环形标记，60/88 行）
   │
   └── SPanel                           ← 有多个 Child，负责摆它们
           └── SActorCanvas             ← Indicator 容器（212/1184 行）
```

**判断标准很简单**：
- 你要"画一个东西"（圆环、箭头、格子）→ `SLeafWidget`
- 你要"装一堆别人的东西并决定它们的位置"→ `SPanel`

## 5. Slot：你其实一直在用它

在 UMG 里给 `CanvasPanel` 放一个 `Image`，蓝图 Details 里的这些：

```text
Position / Size / Anchors / Alignment / ZOrder
```

**大部分就是 Slot 数据**（`UCanvasPanelSlot`）。结构上永远是：

```text
Panel
 ├── Slot ── Child A
 ├── Slot ── Child B
 └── Slot ── Child C
```

`SActorCanvas` 自己定义了 `FSlot : TSlotBase<FSlot>`（`SActorCanvas.h`），因为普通 Canvas Slot 不够用，它还要存：

| 字段 | 用途 |
|---|---|
| `ScreenPosition` | 投影得到的屏幕位置 |
| `Depth` | 排序次键（**注意 §10 的疑点**） |
| `Priority` | 排序主键 |
| `bIsIndicatorVisible` | Descriptor 与组件有效性合成后的可见性 |
| `bInFrontOfCamera` | 是否在摄像机前方（决定 clamp 走哪个分支） |
| `bHasValidScreenPosition` | 是否可用于布局 |
| `bDirty` | 是否变化（决定是否 `Invalidate(Paint)`） |
| `bWasIndicatorClamped` | 上一帧是否被贴边（供箭头/状态变化检测） |

于是 `OnArrangeChildren()` 可以这样说话：

```text
Boss Slot：屏幕位置 (850, 320)，Widget 100x30，居中 → 放到 (800, 305)
```

## 6. 三阶段分工（与一个反直觉点）

| 阶段 | 谁驱动 | 干什么 |
|---|---|---|
| **数据** | `ActiveTimer`（`RegisterActiveTimer(0, ...)`，`:1182`） | `FIndicatorProjection::Project()` 算出每个 slot 的屏幕位置 / 深度 / 优先级 / 可见性；有变化才 `Invalidate(Paint)`（`:449`） |
| **布局** | `OnPaint` 内部调用 `ArrangeChildren()` | 用 DesiredSize + HAlign/VAlign + Clamp + Arrow 算出每个 Child 的最终 Geometry |
| **绘制** | Slate Paint 流水线 | 按 Geometry 逐个 Child `Paint` |

**反直觉点**：三者不是一条直线。`OnPaint` 会先把当前几何存下来（`:804` `OptionalPaintGeometry = AllottedGeometry;`），而 `UpdateCanvas` 正是**读它**来做投影（`:260` 守卫、`:322` 使用）。所以：

1. `UpdateCanvas` **依赖 `OnPaint` 至少跑过一次**；
2. 当帧布局用的屏幕位置来自**上一次** `UpdateCanvas`，存在**一帧延迟**（上游同样如此，是设计不是缺陷）。

详细时序见 [indicator-ui-system.md §12.3](indicator-ui-system.md)。

## 7. 两条树，以及把它们接起来的那条链

这个概念一旦建立，整个系统就通了。

```text
Gameplay / Actor Tree                      UI Tree
────────────────────                       ───────
LocalPlayer A
   │
PlayerController A                         WBP_GameHUD
   │                                            │
   └── IndicatorManager A                 UIndicatorLayer
           ├── Descriptor Boss                  │
           ├── Descriptor Quest         RebuildWidget() → SNew(SActorCanvas, ...)
           └── Descriptor Teammate              │
                                          SActorCanvas A
                                                ├── WBP_BossIndicator
                                                ├── WBP_QuestIndicator
                                                └── WBP_TeammateIndicator
```

两边通过 `FLocalPlayerContext` 接起来，完整链接链（每一跳都有代码）：

```text
WBP_GameHUD                              CreateWidget(PlayerControllerA, WBP_GameHUD)
   ↓ OwningPlayer
PlayerController A
   ↓
LocalPlayer A
   ↓  UIndicatorLayer::RebuildWidget() 里 GetOwningLocalPlayer()     IndicatorLayer.cpp:29-36
FLocalPlayerContext(LocalPlayerA)  ──→  SNew(SActorCanvas, ...)       IndicatorLayer.cpp:36
   ↓  SActorCanvas::UpdateCanvas 里 LocalPlayerContext.GetPlayerController()   SActorCanvas.cpp:276
PlayerController A
   ↓  UHodgeIndicatorManagerComponent::GetComponent(...)             SActorCanvas.cpp:276
   ↓  → Controller->FindComponentByClass<...>()                      HodgeIndicatorManagerComponent.cpp:35
IndicatorManager A
```

**含义**：`SActorCanvas` 从出生起就知道"我是玩家 A 的 Canvas"，不需要任何人告诉它。

## 8. `UIndicatorLayer` 是 UMG → Slate 的 Adapter

它整个类只有 20 行有效代码，看起来"什么都没干"，其实职责就是把 Slate 画布塞进 UMG 世界（`IndicatorLayer.cpp:29-43`）：

```text
UIndicatorLayer::RebuildWidget()
   ├─ IsDesignTime() ? → 返回 SNew(SBox)（设计器里给个占位）
   └─ 否则：
        GetOwningLocalPlayer()            ← 拿到"我属于哪个玩家"
        MyActorCanvas = SNew(SActorCanvas, FLocalPlayerContext(LocalPlayer), &ArrowBrush)
        return MyActorCanvas.ToSharedRef()
```

另外一半在 `ReleaseSlateResources` 里：`MyActorCanvas.Reset()`（`:22-27`）——**UMG 对象销毁 / 重建时主动放掉 Slate 侧引用**。所以你在 UMG 蓝图里放的是：

```text
WBP_GameHUD
├── Overlay
│   ├── WBP_PlayerHUD
│   ├── WBP_SkillBar
│   ├── WBP_QuestHUD
│   └── UIndicatorLayer      ← 放这个（UWidget），不是 SActorCanvas
```

## 9. 为什么不能直接用普通 `CanvasPanel`

因为普通 CanvasPanel 的布局规则是**"你告诉我坐标，我放那里"**，它不知道：

```text
这个 Widget 对应哪个 Actor？        Actor 世界坐标在哪里？
怎么 WorldToScreen？                Actor 跑出屏幕怎么办？
要不要 Clamp？Clamp 到哪条边？       箭头朝哪里？
多个 Indicator 谁盖谁？              每帧怎么更新？
```

而 `SActorCanvas` 的规则是：

> **"你告诉我世界目标，我来决定你每一帧在屏幕的哪里。"**

**这就是这个类最核心的一句话**，也是它必须自定义 `SPanel` 的原因——不是"为了 Slate 而 Slate"，而是"世界 UI 的位置"本身就是这个容器的布局规则。

## 10. 它为什么是 UI 而不是 Component，为什么不是单例

**① 为什么不是挂在 PlayerController 上的 Component？** 因为职责与生命周期完全不同：

| | `UHodgeIndicatorManagerComponent` | `SActorCanvas` |
|---|---|---|
| 所在层 | Gameplay / 状态层（挂在 `PlayerController`） | **UI 树**（挂在某张 HUD UserWidget 下） |
| 回答的问题 | "这个玩家现在有哪些 Indicator？" | "这些 Indicator 该怎么画出来？" |
| 内容 | Descriptor 列表 + 增删广播 | Widget、槽位、投影、排序、贴边 |

**② 一个 LocalPlayer 一套，不是"一个角色一套"**：

```text
PlayerController
   ├─ Possess Character A   ← 死亡
   └─ Possess Character B
```

`IndicatorManager` 与 `SActorCanvas` **不需要跟着 Pawn 重建**——它们属于"玩家 / 视图 / HUD"，而不是 Pawn。这也是 Manager 放在 PlayerController 而不是 Character 上的原因。

**③ 感觉像单例，但绝不能是单例**：单 LocalPlayer 的游戏里通常只有一套，所以运行时"看起来像全局一个"。但 UE 支持分屏：

```text
LocalPlayer 0                           LocalPlayer 1
├── PC 0 → IndicatorManager 0           ├── PC 1 → IndicatorManager 1
└── HUD 0 → IndicatorLayer 0            └── HUD 1 → IndicatorLayer 1
        └── SActorCanvas 0                       └── SActorCanvas 1
```

两个人看同一个世界，但摄像机、屏幕坐标、HUD 都可以不同。所以**不要写成 `static SActorCanvas* GActorCanvas;` 这类全局单例**。

## 11. 和你以前写法的对照

**以前（每个血条自己管自己）**：

```cpp
void UEnemyIndicatorWidget::NativeTick(...)
{
    FVector2D ScreenPosition;
    UGameplayStatics::ProjectWorldToScreen(PC, Enemy->GetActorLocation(), ScreenPosition);
    SetPositionInViewport(ScreenPosition);
}
```

```text
N 个 Enemy Widget
   └── 各自 NativeTick → 各自找 PC → 各自 WorldToScreen → 各自 SetPosition
```

**现在（一个 Panel 统一管）**：

```text
SActorCanvas
   ├── 统一 UpdateCanvas（投影 + 脏标记）
   └── 统一 OnArrangeChildren（排序 + 对齐 + Clamp + 箭头）
        └── FSlot × N（每个 Indicator 一份屏幕数据）
```

好处：N 份重复逻辑变成 1 份；排序 / 池化 / 距离剔除 / 降频这类优化只需改这一个文件，Gameplay 侧一行不动。

## 12. `OnArrangeChildren` 到底在做什么（人话版）

它最后干的事情就是逐个 Child 说一句"这一帧你在这、这么大"。项目里的真实调用（`SActorCanvas.cpp:765-776`）：

```cpp
ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(
    CurChild.GetWidget(),          // 哪个 Child
    ScreenPosition + SlotOffset,   // 放哪（投影位置 + HAlign/VAlign 偏移）
    SlotSize,                      // 多大（Widget DesiredSize）
    1.f                            // Scale
));
```

粗略类比："把 `SetPositionInViewport()` 的工作**批量做成底层布局**"。

**但要清楚类比的边界**：Slate 并不会真的去调 `SetPositionInViewport`，它产出的是 `FArrangedWidget`（里面是 Geometry），交给后续的 Paint 使用。`SetPositionInViewport` 是"把一个 Widget 加到 viewport 并给坐标"，`MakeChild` 是"在既有几何体系里给一个 Child 分配子几何"——**机制不同，意图相似**。

## 13. 项目里的 Slate 谱系

| 文件 | 类型 | 属于 |
|---|---|---|
| `UIndicatorLayer` | `UWidget` | UMG ↔ Slate **Adapter** |
| `SActorCanvas` | `SPanel` + `FGCObject` | **原生 Slate 容器**（自定义布局） |
| `SCircumferenceMarkerWidget` | `SLeafWidget` | **原生 Slate 自绘** |
| `SHitMarkerConfirmationWidget` | Slate（⏸ 待复活） | 原生 Slate 自绘 |
| `UUIExtensionPointWidget` | `UDynamicEntryBoxBase`（⏸ 待引入） | UMG 容器（复用引擎的动态条目框） |
| 其余 70+ 文件 | `UUserWidget` / `UWidget` 派生 | 纯 UMG 层 |

## 14. 学习清单（吃透这些，`SActorCanvas.cpp` 至少 70% 不再是黑魔法）

```text
5 个概念：SWidget / SPanel / Slot / OnArrangeChildren / OnPaint
2 棵树：  Gameplay(Actor) 树  ↔  UI 树
1 条链：  HUD → OwningPlayer → LocalPlayer → GetOwningLocalPlayer()
          → FLocalPlayerContext → GetPlayerController() → IndicatorManager
```

剩下比较陌生的主要是 Slate 的 `FGeometry / Arrange / Paint / ActiveTimer` 细节——那些建议**在读具体代码时按需查**，不需要预先系统学完 Slate。

---

**变更记录**

| 日期 | 内容 |
|---|---|
| 2026-09-26 | 首版。整合"UMG↔Slate 两层模型 + 5 个概念 + 两条树 + 绑定链 + 为什么是 Panel 而不是 Component/单例"的学习笔记；UMG→Slate 映射均在引擎源码核实。未修改代码。 |
