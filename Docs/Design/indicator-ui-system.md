# Indicator UI 系统：职责边界与完整流程

> **文档类型：学习笔记 + 源码核实说明**（不是设计草案，也不是"已实现功能"的声明）。
> 基线 2026-09-26，代码位置：`Source/Hodgepodge/{Public,Private}/UI/IndicatorSystem/`，共 11 个文件（**全部在编**）。
> 本文所有行号均指向本项目当前工作区；引用上游处会明确标注。

**相关文档**

- [UMG 与 Slate：最小心智模型](umg-slate-mental-model.md)（**建议先读**：UMG↔Slate 两层关系、5 个概念、两条树与绑定链）
- [Lyra UI 迁移文件导览](lyra-ui-file-guide.md)（全局分级）
- [Lyra UI 架构迁移执行文档](lyra-ui-migration-plan.md)

---

## 0. 一句话结论

> **Gameplay 系统不要自己管"世界跟随 UI"。它只创建一个 `UIndicatorDescriptor` 表达"我要一个什么样的世界 Indicator"，注册给当前玩家的 `UHodgeIndicatorManagerComponent`；后面的统一 UI 层（`SActorCanvas`）根据 Descriptor 负责创建 Widget、世界转屏幕、排序、贴边、画箭头和销毁。**

## 1. 先纠正一个容易犯的错

> ❌ "`UIndicatorDescriptor` 负责坐标转换。"
> ✅ **`UIndicatorDescriptor` 不是坐标转换器**，它是"一条 Indicator 显示请求的描述数据"。真正的坐标转换在 `FIndicatorProjection::Project(...)`。

```
UIndicatorDescriptor      =  "我要显示什么 Indicator？"      （描述数据 / 工单）
FIndicatorProjection      =  "这个目标世界坐标对应屏幕哪里？"  （数学 / 转换器）
UHodgeIndicatorManagerComponent = "这个玩家现在有哪些 Indicator？" （清单 / 注册表）
SActorCanvas              =  "我怎么把所有 Indicator 真正画出来？"（渲染与布局）
```

Descriptor 只给 `Project()` **提供"应该怎么转"的参数**（跟哪个组件、哪个 Socket、哪种投影模式、偏移多少、要不要贴边），它自己一行投影数学都没有。

## 2. 四个角色的职责边界

| 角色 | 类型 | 在哪 | 职责 | 明确不负责 |
|---|---|---|---|---|
| `UIndicatorDescriptor` | `UObject` | `IndicatorDescriptor.h:88` | 描述"要什么"：目标组件/Socket、控件软类、业务数据对象、投影模式、对齐、偏移、贴边、箭头、优先级 | 不转坐标、不创建 Widget、不持有 Widget 生命周期 |
| `UHodgeIndicatorManagerComponent` | `UControllerComponent`（挂 Controller） | `HodgeIndicatorManagerComponent.h:18` | 每个玩家一份清单：`AddIndicator` / `RemoveIndicator` / `GetIndicators`，并广播增删事件 | 不做任何 UI 创建或布局 |
| `FIndicatorProjection` | 普通 struct | `IndicatorDescriptor.h:35` | 世界空间 → 屏幕像素 + 深度（`Project()`） | 不知道 Widget 存在 |
| `SActorCanvas` | `SPanel` + `FGCObject` | `SActorCanvas.h` | 监听清单增删、按需创建控件、每帧投影、排序、贴边、箭头、回收 | 不认识任何 Gameplay 系统 |
| `UIndicatorLayer` | `UWidget` | `IndicatorLayer.h:14` | 把 `SActorCanvas` 包装成 UMG 可摆放的控件 | 不参与逻辑 |

## 3. 完整数据流

```
              Gameplay Systems（锁定 / 任务 / 队伍 / 交互 / Ping / 掉落）
                        │
                        │  只做一件事：NewObject<UIndicatorDescriptor>() + 填字段
                        ▼
                UIndicatorDescriptor（一张"UI 工单"）
                        │
                        │  Manager->AddIndicator(Descriptor)
                        ▼
        UHodgeIndicatorManagerComponent（该玩家的清单）
                        │
                        ├─ SetIndicatorManagerComponent(this)   建立归属
                        ├─ OnIndicatorAdded.Broadcast(Descriptor)   通知 UI 层
                        └─ Indicators.Add(Descriptor)            保存
                        │
                        ▼
                  SActorCanvas（统一 UI 层，每个 LocalPlayer 一份）
                        │
                        ├─ 首次拿到 Manager 时补播已有指示器     SActorCanvas.cpp:270-309
                        ├─ OnIndicatorAdded → 异步加载控件类 → 控件池取实例 → 加槽位  :889-1010
                        └─ 每帧 UpdateCanvas()：
                              FIndicatorProjection::Project()   SActorCanvas.cpp:397
                                    ↓
                              World → Screen X/Y + Depth
                                    ↓
                              Set/判定可见性、脏标记、无效时移除
                        │
                        ▼
                  OnArrangeChildren()  SActorCanvas.cpp:505+
                        ├─ StableSort（Priority 升序，同优先级按 Depth 降序）  :541
                        ├─ 屏幕矩形内缩出 ClampRect（含箭头尺寸的 FixedPadding） :597-602
                        ├─ 超出则 4 平面 + 线段求交得到贴边点与方向          :620-636
                        ├─ 在框内但目标在摄像机背后 → 按象限钉到最近边        :638-690
                        ├─ 贴边则取箭头并算旋转/偏移                        :692-760
                        └─ 最终决定每个槽位的位置与可见性
```

## 4. 逐类精确说明

### 4.1 `UIndicatorDescriptor`（394 行头文件 / 276 行实现，`UCLASS(BlueprintType)`）

**它描述什么**（全部是 `UFUNCTION(BlueprintCallable)` 的 getter/setter，可在蓝图里配）：

| 分组 | 字段 / API | 说明 |
|---|---|---|
| 目标 | `SetSceneComponent` / `SetComponentSocketName` | 跟随的组件与 Socket（`NAME_None` 表示不用 Socket，走组件原点） |
| 数据 | `SetDataObject(UObject*)` | **业务数据**，与定位用的组件相互独立。Widget 通过它拿要做展示的数据 |
| 控件 | `SetIndicatorClass(TSoftClassPtr<UUserWidget>)` | 用软类引用，避免 Descriptor 创建时同步加载 |
| 投影 | `SetProjectionMode(EActorCanvasProjectionMode)` | 5 种模式，见 4.3 |
| 布局 | `SetHAlign` / `SetVAlign` / `SetWorldPositionOffset`（投影前）/ `SetScreenSpaceOffset`（投影后）/ `SetBoundingBoxAnchor`（包围盒锚点，默认 `(0.5,0.5,0.5)`） | |
| 贴边 | `SetClampToScreen` / `SetShowClampToScreenArrow` | 是否钉到屏幕边缘、要不要画方向箭头 |
| 排序 | `SetPriority(int32)` | 深度排序**之后**再用它调前后关系 |
| 可见性 | `SetDesiredVisibility(bool)` / `GetIsVisible()` | `GetIsVisible()` = 组件有效 **且** `bVisible`（:171） |
| 自动清理 | `SetAutoRemoveWhenIndicatorComponentIsNull(bool)` / `CanAutomaticallyRemove()` | 组件失效后由画布自动摘除（:157-161） |
| 归属 | `SetIndicatorManagerComponent` / `GetIndicatorManagerComponent` / `UnregisterIndicator()` | 见 4.2 |

**两个关键设计细节**：

1. `IndicatorWidget`、`Content`、`CanvasHost` 全是**弱引用**（:142、:389、:393）——Descriptor 不维持 Widget 生命周期，谁创建谁负责。
2. `friend class SActorCanvas`（:361）——上游明确表达"真正干活的 Slate 层会直接读这些数据"，所以 `SActorCanvas` 直接访问 `Indicators`/`Priority`/`HAlign` 等。

### 4.2 `UHodgeIndicatorManagerComponent`（`UControllerComponent`）

构造里设 `bAutoRegister = true` + `bAutoActivate = true`（:19-23），所以**挂在 Controller 上就自动生效**，不需要手动 `RegisterComponent`。

**`AddIndicator` 的确切顺序**（`HodgeIndicatorManagerComponent.cpp:43-55`）：

```cpp
IndicatorDescriptor->SetIndicatorManagerComponent(this);   // 1. 建立归属（内部 ensure 只能被设置一次）
OnIndicatorAdded.Broadcast(IndicatorDescriptor);           // 2. 广播（注意：此时还没进列表）
Indicators.Add(IndicatorDescriptor);                       // 3. 保存
```

**`RemoveIndicator`**（:58-74）：`ensure(Descriptor->GetIndicatorManagerComponent() == this)` → `OnIndicatorRemoved.Broadcast` → `Indicators.Remove`。

`UIndicatorDescriptor::SetIndicatorManagerComponent` 里 `ensure(ManagerPtr.IsExplicitlyNull())`（`IndicatorDescriptor.cpp:259`）——**一个 Descriptor 只能归一个 Manager**，重复注册会被断言拦住。

**主动注销**：`Descriptor->UnregisterIndicator()`（`IndicatorDescriptor.cpp:267-276`）就是 `Manager->RemoveIndicator(this)` 的转发。所以 Gameplay 侧**不需要保存 `UUserWidget*`，只保存 `UIndicatorDescriptor*` 即可**。

### 4.3 `FIndicatorProjection::Project`（`IndicatorDescriptor.cpp:21-251`）

签名：

```cpp
bool Project(const UIndicatorDescriptor& IndicatorDescriptor,
             const FSceneViewProjectionData& InProjectionData,
             const FVector2f& ScreenSize,
             FVector& OutScreenPositionWithDepth);   // X/Y = 屏幕像素，Z = 到摄像机的距离
```

**5 种投影模式**（`EActorCanvasProjectionMode`，`IndicatorDescriptor.h:48-64`）：

| 模式 | 取点方式 | 实现位置 |
|---|---|---|
| `ComponentPoint` | 组件（或 Socket）的世界点 | :55-105 |
| `ComponentBoundingBox` / `ActorBoundingBox` | 世界包围盒按 `BoundingBoxAnchor` 取锚点 | :186-245 |
| `ComponentScreenBoundingBox` / `ActorScreenBoundingBox` | 投影成屏幕二维包围盒后按锚点插值 | :108-183 |

**里面那些"容易漏但很关键"的处理**：

1. **摄像机前后判断**：`ULocalPlayer::GetPixelPoint` / `GetPixelBoundingBox` 的返回值表示是否在摄像机前方（:65、:131、:216）。
2. **屏幕空间偏移在背后时反转 X**（:71-72、:146-147、:221）——否则目标转到背后时指示器会左右乱跳。
3. **摄像机背后但投影结果落在屏幕内时，把它沿"屏幕中心 → 该点"方向推到屏幕外**（:80-90、:163-176、:228-237）。这是为了让后续贴边逻辑能算出正确的方向——不推出去的话，背后的目标会诡异地停在屏幕中央。
4. **深度用 `ViewOrigin` 到投影点的距离**（:96-97、:156、:241），供排序使用。

### 4.4 `SActorCanvas`（602 有效行 / 1184 总行，`SPanel` + `FGCObject`）

> 它是整套 Indicator 系统的**客户端 UI Runtime**：前面所有类到这里才串起来。完整职责清单与运行链见 §12。

**`Construct`**（:207-252）：`SetCanTick(false)`（改用 ActiveTimer 驱动，见下）、`SelfHitTestInvisible`，并**预先建 10 个箭头控件**放在 `ArrowChildren` 里备用——箭头是复用的，不是每次新建。

**`UpdateCanvas`（ActiveTimer 驱动，:253-482）**——每帧的流程：

1. 没有 `OptionalPaintGeometry` 就直接返回（:260-263）——**它由 `OnPaint` 写入（:804）**，这个"数据依赖绘制"的耦合很反直觉，见 §12.3。
2. 懒获取 Manager：拿不到就一直重试；拿到就**订阅 `OnIndicatorAdded/Removed` 并补播已有清单**（:270-309）。这是"Gameplay 先注册、UI 后创建"场景下的关键补偿。
3. 取 `LocalPlayer->GetProjectionData(...)`（:322）——这里就是 `UHodgeLocalPlayerBase::GetProjectionData` 被覆写的那个入口。
4. 遍历槽位：组件失效且允许自动移除 → 摘除（:342）；不可见 → 跳过；然后调 `Project()`（:397）；失败则标记无有效位置，成功则写屏幕位置（:429）与深度（:433，**该行有待验证标记，见 §10.1**）。
5. **只在 `IndicatorsChanged` 时 `Invalidate(Paint)`**（:449）——避免无谓重绘。
6. **清单为空时 `TickHandle.Reset()` 并返回 `Stop`**（:467-475）——没有指示器时系统自动停摆，值得学的省电细节。

**`OnArrangeChildren`（:505-794）**——把所有指示器一次统一处理：

- `StableSort`：`Priority` 相同时按 `Depth` 降序，否则 `Priority` 升序（:541）。
- 尺寸与对齐：`GetOffsetAndSize`（:1092-1166）用 `CanvasHost->GetDesiredSize()` + `HAlign/VAlign` 算出 slot 尺寸、投影点对齐偏移和 clamp padding。
- 贴边：屏幕内缩出 `FixedPadding`（含箭头尺寸）得到 `ClampRect`（:597-602）；超出则用 **4 个平面 + 线段求交**得到贴边点与方向（:620-636）；在框内但目标是**摄像机背后**时改按象限判断钉到哪条边（:638-690）。
- 箭头：需要时从预建箭头里取一个、设旋转、按 `VAlignment` 做居中补偿（:692-760）。

**`OnPaint`（:795-854）**：存下 `AllottedGeometry` → `ArrangeChildren()` → 按排列结果逐个 child `Paint`，并可选择严格绘制顺序（`:838`）。

**GC 与资源**：`FGCObject::AddReferencedObjects` 手动上报 `AllIndicators`（:881-887）；`FUserWidgetPool` 复用实际 UMG 控件；析构时取消未完成的异步加载（:857-872，见 [迁移执行文档 §11](lyra-ui-migration-plan.md)）。

## 5. 一张"工单"的完整生命周期（以锁定系统为例）

```cpp
// ── 第一步：Gameplay 只描述"要什么"，不碰 UI ──────────────────
UIndicatorDescriptor* Descriptor = NewObject<UIndicatorDescriptor>();
Descriptor->SetDataObject(Enemy);                    // 业务数据
Descriptor->SetSceneComponent(Enemy->GetMesh());     // 跟随目标
Descriptor->SetComponentSocketName(TEXT("head"));    // 头顶 Socket
Descriptor->SetIndicatorClass(WBP_LockOnIndicator);  // 软类引用
Descriptor->SetProjectionMode(EActorCanvasProjectionMode::ComponentPoint);
Descriptor->SetClampToScreen(true);
Descriptor->SetShowClampToScreenArrow(true);         // 出屏后显示方向箭头
Descriptor->SetPriority(10);                         // 高于血条
Descriptor->SetAutoRemoveWhenIndicatorComponentIsNull(true);

// ── 第二步：找到该玩家的 Manager 并注册 ───────────────────────
UHodgeIndicatorManagerComponent* Manager =
    UHodgeIndicatorManagerComponent::GetComponent(PlayerController);   // 或走 UIndicatorLibrary
Manager->AddIndicator(Descriptor);

// ── 第三步（引擎侧自动发生） ─────────────────────────────────
//   Manager 保存 + 广播 → SActorCanvas 收到后加载/取控件 → 放进槽位
//   每帧 Project() → 布局 → 排序 → 贴边 → 画箭头

// ── 第四步：解除锁定，只注销，不管 Widget ─────────────────────
Descriptor->UnregisterIndicator();   // Manager 广播移除 → 画布摘除并回收控件到池
```

**注意**：锁定系统全程没有 `CreateWidget`、没有 `WorldToScreen`、没有 `Tick`、没有保存 `UUserWidget*`。它只知道**意图**。

## 6. 为什么要绕这一圈

| | 没有 Descriptor | 有 Descriptor |
|---|---|---|
| Gameplay 需要知道 | Widget 怎么创建、放哪层、怎么 WorldToScreen、怎么跟随、出屏怎么办、何时销毁、多人给谁、怎么排序、边缘怎么处理 | 只填一张工单 |
| 依赖方向 | `QuestSystem` → `WBP_QuestIndicator` → UI 实现细节 | `QuestSystem` → `IndicatorDescriptor`（数据契约） |
| N 个系统 | N 份 `CreateWidget + Tick + Clamp + Remove` | 1 份统一实现在 `SActorCanvas` |
| 统一优化 | 几乎不可能 | 改一处，所有系统受益 |

对照项目里可能出现的使用者：任务目标、锁定框、敌人血条、队友标记、可交互物提示、掉落物名称、世界事件图标、Ping 标点。

## 7. 与 GAS 的类比（以及类比的边界）

你会自然地把它和 `GameplayEffectSpec` 联系起来——**这个类比成立，而且很有用**：

```
Ability            →  GameplayEffectSpec      →  ASC                  →  GAS 统一处理
Gameplay 系统       →  UIndicatorDescriptor    →  IndicatorManager      →  SActorCanvas 统一处理
```

两者都是：**把"要发生什么"描述成一个数据对象，交给一个统一系统去执行**，从而切断"发起方 → 具体实现"的依赖。

**边界（哪里不像）**：

| | `GameplayEffectSpec` | `UIndicatorDescriptor` |
|---|---|---|
| 生命周期 | 通常一次性（应用后即结束），由 Spec 复制/预测体系管理 | **长驻**，可能存活整场战斗，直到显式注销 |
| 归属 | 由 ASC 应用 | 由 `ManagerPtr` 弱引用反向持有（`ensure` 保证唯一） |
| 谁最终消费 | GAS 的属性/GE 执行管线 | 一个 Slate 面板，**没有网络复制**（UI 是本地表现） |

所以更准确的说法是：**Descriptor 是"UI Indicator 系统里的 GE Spec"——描述对象 + 统一执行者，但它是长命的、单机的。**

## 8. 这套架构带来的可优化点（现状对照）

| 优化 | 现在有吗 | 依据 |
|---|---|---|
| 控件池复用（避免反复创建 Widget） | ✅ 有 | `FUserWidgetPool IndicatorPool` |
| 统一排序（Priority + Depth） | ✅ 有 | `SActorCanvas.cpp:336-339` |
| 无指示器时自动停止更新 | ✅ 有 | `UpdateCanvas` 末尾 `TickHandle.Reset()` + `Stop`（:289-293） |
| 仅在变化时重绘 | ✅ 有 | `if (IndicatorsChanged) Invalidate(...)`（:274-277） |
| 箭头控件复用 | ✅ 有 | `Construct` 预建 10 个（:145-157） |
| 组件失效自动摘除 | ✅ 有 | `CanAutomaticallyRemove()`（:215-223） |
| **按距离剔除**（远处不更新） | ❌ 没有 | `UpdateCanvas` 里没有任何距离判断 |
| **屏幕外不创建 Widget** | ❌ 没有 | 注册即创建控件；出屏靠贴边处理 |
| **按 Priority 分帧/降频** | ❌ 没有 | 每帧全量投影 |
| **距离/可见性 LOD** | ❌ 没有 | — |

即：**当前实现已经具备"统一处理"的骨架，但还没有做规模化优化**。开放世界（100 敌人 + 50 掉落 + 20 任务目标）时要补的是右列那四项，而且**只需改 `SActorCanvas` 一处，Gameplay 系统一行都不用动**——这正是这套架构的价值所在。

## 9. 项目当前状态与差距（重要）

| 项 | 状态 |
|---|---|
| IndicatorSystem 11 个文件 | ✅ **全部在编**（已随 Editor / Game 双构建 EXIT=0） |
| Gameplay 侧调用点 | ❌ **0 个**：全项目没有任何 `AddIndicator` 调用（只有系统内部定义） |
| `UIndicatorLayer` 使用者 | ❌ 没有任何 Widget 蓝图使用它 |
| 运行证据 | ❌ 未打开编辑器、未 PIE、未做蓝图编译 |

**要跑起来还缺什么**：

1. 一个继承 `UIndicatorLayer` 的 Widget 蓝图（作为 HUD 一部分）；
2. 一个实现 `UIndicatorWidgetInterface`（`BindIndicator`/`UnbindIndicator`）的标记控件蓝图；
3. 一个 Gameplay 调用点（`NewObject<UIndicatorDescriptor>` + `AddIndicator`）；
4. 在 Controller 上挂 `UHodgeIndicatorManagerComponent`——**注意：上游是把它作为 GameFeature 的附加组件挂上去的**（`GameFeatureAction_AddWidget`/AddComponents 那条链），本项目 A 组的 `GameFeatureAction_AddWidget` 仍是注释状态，所以这一步要么补 GameFeature 链，要么临时在 Controller 构造里手动加。

## 10. 待核实的疑点与"看着像 bug 其实不是"的点

| 项 | 位置 | 说明 |
|---|---|---|
| ⚠️ `SetDepth(ScreenPositionWithDepth.X)` | `SActorCanvas.cpp:433`（**已在 `:431-432` 标记 `TODO(待验证)`**） | 投影输出里 **Z 才是到摄像机的距离**，但这里用 **X**（屏幕横坐标）当深度。**上游 Lyra 同一行也是 `.X`**（上游 `SActorCanvas.cpp:260`），所以**不是移植写错**。影响：`StableSort`（:541）的次排序键实际是屏幕横坐标而非真实远近。**未做运行验证；不要当成已确认 bug，也不要顺手改上游**。验证步骤见 §10.1 |
| ⚠️ `InactiveIndicators` 只写不读 | `SActorCanvas.h:446`；写点 `:896`、`:913`、`:995` | 全项目**没有任何读取点**。语义上它表示"已收到 Descriptor 但 Widget 还没进 Canvas"，当前是**死状态**。**上游 Lyra 同样只写不读**（上游 `:549`、`:559`、`:588`），不是移植缺失 |
| ⚠️ 摄像机背后象限钉边的边界条件 | `:638-690` | 用 `ScreenXNorm/ScreenYNorm` 比较决定贴哪条边；正好落在对角线时的取边结果未验证 |
| ℹ️ 4 平面求交"取最后一个命中" | `:620-636` | 循环遍历 4 个平面**没有 break**。正常情况下射线从矩形内部出发只穿出一次，所以只有一次命中；仅在正好穿过角点（浮点精度）时才可能多次命中，此时按 Left→Top→Right→Bottom 的顺序**最后一个**生效 |
| ℹ️ 重入安全性 | `:889-1010` | `OnIndicatorAdded` → `AddIndicatorForEntry` 期间若 Descriptor 被移除，已有 `AllIndicators.Contains()` 守卫（:968）；但**异步加载窗口内的并发移除未做运行验证** |
| ℹ️ `GetOffsetAndSize` 里的 `AllottedSize` 恒为 0 | `:1101` | 上游注释写明 "This might get used one day"。因此 `HAlign_Center` 时 `OutOffset = -OutSize/2`，这正是"以投影点为中心"的实现方式（:1127、:1154） |

### 10.1 待验证实验：`Depth` 排序键到底用的是不是距离

**代码标记**：`Source/Hodgepodge/Private/UI/IndicatorSystem/SActorCanvas.cpp:431-432`，形如

```cpp
// TODO(待验证)：Project() 的输出是 X=屏幕横坐标、Z=到摄像机距离，
// 此处取 .X 会让排序次键变成屏幕横坐标；上游 Lyra 同样取 .X。见 indicator-ui-system.md §10。
CurChild.SetDepth(ScreenPositionWithDepth.X);
```

`grep -rn "TODO(待验证)" Source/` 可列出全部同类待验证点（当前只有这一处）。

**前提**：§9 里那 4 件事做完，屏幕上能同时显示至少 2 个指示器。

**排序规则回顾**（`SActorCanvas.cpp:541`）：同 `Priority` 时按 `Depth` **降序**排（`A.GetDepth() > B.GetDepth()` 在前）；Slate 中先排先画 → 后被覆盖。所以**前提是 `Depth` 真的是距离**，才能得到"近处遮挡远处"的直觉效果。不同 `Priority` 的指示器之间先按 `Priority` 排，看不出这个差异。

**实验设计（可证伪）**：造两个 `Priority` 相同、屏幕上互相重叠的指示器，但让**屏幕横坐标顺序与距离顺序相反**：

| | 距离摄像机 | 屏幕横坐标 X | 应有的绘制顺序 |
|---|---|---|---|
| A | **近**（如 5 m） | **大**（偏屏幕右侧） | 后画（压住 B） |
| B | **远**（如 20 m） | **小**（偏屏幕左侧） | 先画（被 A 压住） |

- 若 `Depth` = 真实距离（`.Z`）：B 的 Depth 大 → B 先画、A 后画 → **A 压住 B**（正确）
- 若 `Depth` = 屏幕横坐标（`.X`）：A 的 X 大 → A 先画、B 后画 → **B 压住 A**（远处盖住近处，异常）

**判据**：重叠处看到的是哪个指示器。**若结果是"远处那个压住近处那个"，则确认排序键取错。**

**更直接的确认方式**（不改逻辑，只加一行日志）：在 `:432` 之后临时打印两个值，观察它们是否不同：

```cpp
UE_LOG(LogTemp, Warning, TEXT("[Indicator] X=%.1f Z=%.1f"), ScreenPositionWithDepth.X, ScreenPositionWithDepth.Z);
```

两者数值不同 → 说明当前取的键与 `Project()` 的语义约定不符；再按上面的重叠用例确认是否影响观感。

**若确认要修**：把 `.X` 改成 `.Z`，并作为**对上游的有意偏离**单独提交（不要混在其他改动里），同时更新本文 §10 与本项目知识库的记录。改完需要重新验证一次"近处遮挡远处"。

---

## 11. 术语速查

| 名字 | 一句话 |
|---|---|
| `UIndicatorDescriptor` | 一张 UI 工单：要显示什么、跟谁、怎么转、什么规则 |
| `FIndicatorProjection` | 工单的执行者之一：世界 → 屏幕 + 深度 |
| `UHodgeIndicatorManagerComponent` | 某玩家的工单清单（增删 + 广播） |
| `SActorCanvas` | 统一的"施工队"：建控件、投影、排序、贴边、画箭头、回收 |
| `UIndicatorLayer` | 把施工队塞进 UMG 的插座 |
| `UIndicatorWidgetInterface` | 控件的接口：`BindIndicator` / `UnbindIndicator`，是数据与 UI 的接缝 |

---

## 12. `SActorCanvas` 深读：它到底是什么，以及三阶段分工

### 12.1 它的准确定位

`SActorCanvas` 不是"一个 Canvas"，也不只是"WorldToScreen"。它是**整套 Indicator 系统的客户端 UI Runtime**——前面所有类到这里才串起来：

| # | 职责 | 位置 |
|---|---|---|
| 1 | 连接 `IndicatorManager`（懒获取 + 失效重取） | `:270-309` |
| 2 | 订阅增删事件、补播已有清单 | `:289-300` |
| 3 | 接收 Descriptor 并保存 | `:889-900` |
| 4 | 异步加载 WidgetClass | `:917-951`（`FStreamableManager::RequestAsyncLoad`） |
| 5 | 从池创建/复用 Widget | `:975`（`IndicatorPool.GetOrCreateInstance`） |
| 6 | 绑定数据 | `:988`（`Execute_BindIndicator`） |
| 7 | 建立 Slate 槽位 | `:999-1009` + `AddActorSlot`（`:1046-1064`） |
| 8 | World → Screen 投影 | `:397`（`FIndicatorProjection::Project`） |
| 9 | 更新位置 / 深度 / 优先级 / 可见性 | `:420-445` |
| 10 | 排序（Priority + Depth） | `:541` |
| 11 | 尺寸与对齐 | `:1092-1166`（`GetOffsetAndSize`） |
| 12 | 屏幕边缘 Clamp | `:597-690` |
| 13 | 方向箭头（复用 10 个预建箭头） | `:692-760` |
| 14 | Slate 布局 / 绘制 | `OnArrangeChildren`（`:505-794`）/ `OnPaint`（`:795-854`） |
| 15 | 解绑与回收 | `:1013-1043`（`Execute_UnbindIndicator` + `Pool.Release`） |

### 12.2 一条 Indicator 的完整出生链

```text
LockOn System
   │ NewObject<UIndicatorDescriptor> + 填字段
   ▼
Manager->AddIndicator()                         HodgeIndicatorManagerComponent.cpp:43
   │  SetIndicatorManagerComponent → OnIndicatorAdded.Broadcast → Indicators.Add
   ▼
SActorCanvas::OnIndicatorAdded()                SActorCanvas.cpp:889
   │  AllIndicators.Add + InactiveIndicators.Add + AddIndicatorForEntry
   ▼
SActorCanvas::AddIndicatorForEntry()            :917
   │  FStreamableManager::RequestAsyncLoad(WidgetClass, CreateSP(...))
   ▼
SActorCanvas::OnIndicatorClassLoaded()          :954
   │  ① Descriptor 还活着吗（:960）
   │  ② 还在 AllIndicators 里吗（:968，加载期间被移除的守卫）
   │  ③ IndicatorPool.GetOrCreateInstance()（:975）
   │  ④ Execute_BindIndicator()（:988）
   │  ⑤ AddActorSlot() → CanvasChildren（:999）
   ▼
Widget 已在 Canvas 里 → 进入每帧更新循环
```

### 12.3 三阶段分工（以及一个反直觉点）

| 阶段 | 谁调用的 | 干什么 | 位置 |
|---|---|---|---|
| **数据更新** | `RegisterActiveTimer(0, ...)` 每帧 | 世界变化 → 每个 slot 的 `ScreenPosition` / `Depth` / `Priority` / 可见性；有变化才 `Invalidate(Paint)` | `UpdateCanvas` `:253-482` |
| **布局计算** | 由 `OnPaint` 内部调用 `ArrangeChildren()` | 有屏幕数据之后：DesiredSize + HAlign/VAlign → Clamp → Arrow → 最终 Geometry | `OnArrangeChildren` `:505-794` |
| **真正绘制** | Slate Paint 阶段 | 按算好的 Geometry 逐个 child `Paint` | `OnPaint` `:795-854` |

**反直觉的地方**（值得单独记）：三者**不是**"UpdateCanvas → OnArrangeChildren → OnPaint"这么一条直线。

```text
OnPaint() 每帧执行（Slate 绘制阶段）
   ├─ OptionalPaintGeometry = AllottedGeometry      ← 关键：把几何存下来  (:804)
   ├─ ArrangeChildren() → OnArrangeChildren()       ← 布局
   └─ 各 child Paint()                              ← 绘制

UpdateCanvas() 每帧执行（ActiveTimer 阶段，与上面交错）
   ├─ 读 OptionalPaintGeometry 拿 ScreenSize 做投影  (:260、:322)
   └─ 有变化 → Invalidate(Paint) → 下一帧触发 OnPaint
```

所以：
1. **`UpdateCanvas` 依赖 `OnPaint` 至少跑过一次**——这就是 `:260` 那个 `if (!OptionalPaintGeometry.IsSet()) return Continue;` 存在的原因。
2. 当帧 Paint 用的屏幕位置，实际是**上一次 `UpdateCanvas`** 算出来的，存在**一帧延迟**（上游同样如此，属于设计而非缺陷）。
3. 停止条件是"清单为空"，而 `UpdateActiveTimer` 在**还没找到 Manager 时也会继续 tick**（`:1175`：`AllIndicators.Num() > 0 || !IndicatorComponentPtr.IsValid()`）——否则 Manager 晚于 Canvas 创建时永远等不到。

### 12.4 Widget 池与"为什么接口必须是 Bind/Unbind"

创建与销毁都走池，不直接 `CreateWidget` / 不直接 `Destroy`：

```text
第一次需要 → 池里没有 → 创建 Widget → BindIndicator(A) → 使用
                                          ↓
                                      UnbindIndicator(A)      ← RemoveIndicatorForEntry (:1022)
                                          ↓
                                      IndicatorPool.Release() ← 回池，不是销毁 (:1029)
                                          ↓
下一次需要 → 从池复用 → BindIndicator(B)                       ← OnIndicatorClassLoaded (:988)
```

**这解释了 `IIndicatorWidgetInterface` 为什么设计成 `BindIndicator` / `UnbindIndicator` 而不是单个 `SetIndicator`**：控件实例会被复用，如果只有一个 setter，复用时"A 的数据"会残留在控件上，且没有"归还"时机让控件清理自己的绑定。成对的 Bind/Unbind 才是可复用的生命周期契约。

### 12.5 `AllIndicators` vs `InactiveIndicators`

| 集合 | 含义 | 写入点 |
|---|---|---|
| `AllIndicators` | Canvas 知道的**全部** Indicator（GC 上报也用它，`:885`） | `:892` 加、`:909` 删 |
| `InactiveIndicators` | 语义是"已收到 Descriptor，但 Widget 还没进 Canvas" | `:896` 加、`:913` / `:995` 删 |

**实测结论**：`InactiveIndicators` **只写不读**，当前是**死状态**；**上游 Lyra 同样如此**（上游 `:549/:559/:588`）。理解它的语义仍然有用（它是"异步加载窗口期"的意图表达），但**不要以为它在参与逻辑判断**。

### 12.6 Clamp 的两个分支（比 `Clamp(X, 0, W)` 高级得多）

第一分支——**目标投影点落在 `ClampRect` 之外**（`:620-636`）：

```text
        ScreenCenter
             ●
              \        从屏幕中心向目标投影点连线
               \       与 ClampRect 的 4 条边所在的平面求交
                ●  ← 交点 = 贴边位置，命中哪条边 = ClampDir
                 \      ClampDir 顺手决定 ArrowRotation
                  \                          (:694)
                   ● Target（可能在屏幕外很远处）
```

所以右上方的屏外目标会自然贴到**右上方向对应的屏幕边**，而不是分别把 X/Y 各自夹取。**同一份计算既给出位置、又给出箭头方向**，这是这个实现比朴素 clamp 优雅的地方。

第二分支——**投影点已经在 `ClampRect` 之内，但目标是摄像机背后**（`:638-690`）：改按归一化坐标比较（`ScreenXNorm` vs `ScreenYNorm` vs `-ScreenYNorm + 1`）决定钉到 Left / Top / Right / Bottom 哪条边。上游在 `FIndicatorProjection::Project` 里已经先把"背后但落在屏内"的点推到屏外（见 §4.3），所以两者配合才完整。

### 12.7 一页速记

```text
Gameplay
   │ 提交显示需求（不碰 UI）
   ▼
UIndicatorDescriptor     —— "显示什么、跟谁、怎么显示"
   ▼
UHodgeIndicatorManagerComponent —— "当前玩家有哪些 Indicator"
   ▼
SActorCanvas             —— "我把这些需求真正变成 UI"
   ├── Connect / Load / Pool / Bind
   ├── Project（FIndicatorProjection::Project）
   ├── Sort / Clamp / Arrow / Layout / Paint
   └── Unbind / Release
   ▼
Indicator Widget         —— "我负责具体长什么样"
   ▼
玩家最终看到的 UI
```

**这套架构值得搬进 Hodge 的原因**：Gameplay 只表达"我要显示一个世界 UI"，而世界跟随 UI 的所有脏活——资源加载、对象池、生命周期、投影、布局、排序、屏外处理——**全部被 `SActorCanvas` 吃掉了**。你的锁定框、怪物血条、任务点、交互提示、队友标记、Ping 可以直接共用同一套，而新增优化（距离剔除 / 屏外不创建 / 降频）只需改这一个文件。

---

**变更记录**

| 日期 | 内容 |
|---|---|
| 2026-09-26 | 首版。整理"Descriptor 是描述而不是转换器"的职责边界、完整数据流、逐类 API（带本项目行号）、GAS 类比与边界、优化现状、项目差距与疑点。未修改代码。 |
| 2026-09-26 | 在 `SActorCanvas.cpp:431-432` 加 `TODO(待验证)` 代码标记（仅注释，不改逻辑）；新增 §10.1「待验证实验：Depth 排序键」给出可证伪的实验设计；Editor 构建 EXIT=0。 |
| 2026-09-26 | 新增 §12「`SActorCanvas` 深读」：15 项职责清单、完整出生链、**三阶段分工与反直觉点（`UpdateCanvas` 依赖 `OnPaint` 存入的 `OptionalPaintGeometry`，且存在一帧延迟）**、WidgetPool 与 Bind/Unbind 的因果、`AllIndicators`/`InactiveIndicators` 实测语义、Clamp 两分支。同步把全文行号按当前代码（用户已补充中文注释，行号整体偏移）重新测量修正；§10 增加 3 条"像 bug 其实不是"的说明。未修改代码。 |
