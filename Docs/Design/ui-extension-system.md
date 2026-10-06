# UIExtension：职责边界与完整流程

> 2026-10-06 状态同步：UI 迁移/学习材料：CommonUI 已引入，部分 UIExtension/Indicator 代码有效；依赖 CommonGame/GameSettings/CommonUser 的停用内容与完整前端/HUD 尚未接通。本文不作为游戏 UI 已出画面的声明。 当前项目事实见 [本轮更新](../KnowledgeBase/26-update-2026-10-06.md)。

> **文档类型：子系统说明 + 学习笔记。**
> 迁移状态：**已并入 `Hodgepodge` 模块并编译通过**（2026-09-27，Editor / Game 双构建 EXIT=0）。
> **运行验证：未执行**（未开编辑器、未 PIE、未做蓝图编译）。
> 代码位置：`Source/Hodgepodge/{Public,Private}/UI/Extension/`。

**相关文档**

- [UMG 与 Slate：最小心智模型](umg-slate-mental-model.md)
- [Indicator UI 系统：职责边界与完整流程](indicator-ui-system.md)（另一个自建 UI 子系统，与本文的对比见 §11）
- [Lyra UI 迁移文件导览](lyra-ui-file-guide.md)（§5「四个正交的轴」）
- [Lyra UI 架构迁移执行文档](lyra-ui-migration-plan.md)（§12 是本文的落地计划）

---

## 0. 一句话

> **`UUIExtensionPointWidget` 是 UIExtension 在 UMG 世界里的"实体插槽"：它把 HUD 里的一个真实布局位置注册成 ExtensionPoint，把 Subsystem 匹配过来的 Extension 转换成实际 `UUserWidget`，并负责这些动态控件的创建、映射、移除与生命周期清理。**
> **`UUIExtensionSubsystem` 只是媒婆 / 路由器**——它自己**不创建任何 UI**。

## 1. 四个角色

| 角色 | 类型 | 存在形态 | 职责 |
|---|---|---|---|
| `FUIExtension` | 内部 struct | Subsystem 里的 `TSharedPtr` | "我有东西想显示"：`ExtensionPointTag` + `ContextObject` + `Data` + `Priority` |
| `FUIExtensionPoint` | 内部 struct | Subsystem 里的 `TSharedPtr` | "我这里可以放东西"：`ExtensionPointTag` + 匹配规则 + `AllowedDataClasses` + 回调 |
| `UUIExtensionSubsystem` | `UWorldSubsystem` | 每个 World 一份 | Tag / Context / DataClass 三条件匹配 + 广播 Added / Removed |
| `UUIExtensionPointWidget` | `UDynamicEntryBoxBase` 派生 | **真正在 UMG Widget Tree 里** | 注册坑位、接收结果、`CreateEntryInternal` 造控件、记账、移除、清理 |

```text
Extension                    ExtensionPoint
"我有东西想显示"              "我这里可以放东西"
       │                             │
       └──────────────┬──────────────┘
                      ▼
            UUIExtensionSubsystem         ← 只匹配 + 广播，不建 UI
                      ▼
            UUIExtensionPointWidget       ← 真正 CreateEntryInternal
```

## 2. 匹配要同时过三个条件

`FUIExtensionPoint::DoesExtensionPassContract`（`UIExtensionSystem.cpp`）：

| 条件 | 规则 |
|---|---|
| **Tag** | 扩展点会遍历自己的 Tag 及其父 Tag；`ExactMatch` 只看自身，`PartialMatch` 还能收到注册在**子 Tag** 上的扩展（Subsystem 侧的遍历逻辑） |
| **Context** | **必须严格相等**，或**两边都为 explicitly null**。`LocalPlayer` 注册的扩展**不会**被 `PlayerState` 的坑位收到 |
| **DataClass** | 扩展的 `Data`（若是 `UClass` 就取该类，否则取其实例类）必须 `IsChildOf` 或实现 `AllowedDataClasses` 中之一。槽位注册时**自动加入 `UUserWidget`**，所以 `RegisterExtensionAsWidget` 传任意 `UUserWidget` 子类都能通过 |

**Context 严格相等这一条，是"为什么一个槽位要注册多个 ExtensionPoint"的根本原因。**

## 3. 一个槽位最多注册三个 ExtensionPoint

同一个 `UUIExtensionPointWidget`（比如 `Tag = HUD.Slot.Reticle`）实际注册了**三个接收渠道**，这也是头文件里必须是数组的原因：

```cpp
TArray<FUIExtensionPointHandle> ExtensionPointHandles;   // 不是单个 Handle
```

| # | 注册方式 | Context | 接收谁 |
|---|---|---|---|
| 1 | `RegisterExtensionPoint(...)` | `nullptr` | **全局** Extension（谁都能注册，不区分玩家） |
| 2 | `RegisterExtensionPointForContext(..., GetOwningLocalPlayer(), ...)` | `LocalPlayer` | 本机 UI：设置、输入设备提示、本地菜单/HUD |
| 3 | `RegisterExtensionPointForContext(..., PlayerState, ...)` | `PlayerState` | Gameplay 玩家维度：等级、队伍、英雄信息、比赛状态 |

第 3 个是**延迟且条件性**的：它通过 `HodgeLocalPlayerBase::CallAndRegister_OnPlayerStateSet` 注册——PlayerState 若已经存在会**立即回调**，否则等它出现。所以严格说是"**最多**三个"，PlayerState 永不出现的场景（例如纯 UI 场景）只有 2 个。

## 4. 不丢单：注册顺序无关

`RegisterExtensionPointForContext` 内部会调用 `NotifyExtensionPointOfExtensions(Entry)` —— **把已经存在的 Extension 立刻补发一遍 Added**。

所以不存在"Quest 的 GameFeature 比 HUD 先加载，所以 Quest UI 丢了"这种顺序问题：

```text
坑位后上线
   → 注册 ExtensionPoint
   → Subsystem 回查已有 Extension
   → 立即补发 Added
   → 槽位照样收到
```

## 5. 收到 Added 后的两条路径

`OnAddOrRemoveExtension`：

| 路径 | 条件 | 流程 |
|---|---|---|
| **A. 直接给控件类** | `Cast<UClass>(Data)` 成功（即 `RegisterExtensionAsWidget` 注册的） | → `CreateEntryInternal(WidgetClass)` → 记 `ExtensionMapping[Handle] = Widget` |
| **B. 数据 → 控件类** | Data 不是 `UClass`，且槽位配了 `DataClasses`，且蓝图绑定了 `GetWidgetClassForData` | → `GetWidgetClassForData.Execute(Data)` 得到控件类 → `CreateEntryInternal` → 记映射 → `ConfigureWidgetForData(Widget, Data)` 把数据灌进去 |

路径 B 就是"**数据驱动 UI**"：`Data → 决定 View 类型 → 创建 View → 灌数据`。调用方若判断这条数据不需要显示，`GetWidgetClassForData` 直接返回空类即可忽略。

## 6. 删除与生命周期

```text
创建时：ExtensionMapping.Add(Request.ExtensionHandle, Widget)
        ExtensionMapping
          Handle_A → WBP_Quest
          Handle_B → WBP_Team

卸载时：GF 卸载 → Handle_A.Unregister() → Subsystem 广播 Removed
        → OnAddOrRemoveExtension(Removed)
        → ExtensionMapping.FindRef(Handle_A) → RemoveEntryInternal(Widget)
```

**Handle 就是这条动态 UI 的身份证**，所以不会出现"Feature 卸载了，但不知道 HUD 里哪个控件是它建的"。

`ResetExtensionPoint()` 统一收尾：

```text
ResetExtensionPoint
 ├── ResetInternal()               清空动态 Entry（来自 UDynamicEntryBoxBase）
 ├── ExtensionMapping.Reset()      清空 Handle → Widget 账本
 ├── Handle.Unregister() × N       从 Subsystem 注销全部 ExtensionPoint
 └── ExtensionPointHandles.Reset() 清空本地 Handle
```

它被两处调用：`ReleaseSlateResources`（Widget 释放时）与 `RebuildWidget` 的运行时分支开头（**保证可重复调用**）。

## 7. 槽位的"形态"来自 `UDynamicEntryBoxBase`

这一点容易被忽略：`UUIExtensionPointWidget` 继承 `UDynamicEntryBoxBase`，所以**槽位内部怎么排由父类属性决定**（Details 面板可配）：

| 属性 | 作用 |
|---|---|
| `EntryBoxType` | 容器类型：`Horizontal` / `Vertical` / `Wrap` / `VerticalWrap` / `Radial` / `Overlay`（引擎 `DynamicEntryBoxBase.h:14-22`） |
| `EntrySizeRule` | 条目尺寸规则（横/竖排列时用） |
| `EntryHorizontalAlignment` / `EntryVerticalAlignment` | 条目对齐 |
| `SpacingPattern` / `MaxElementSize` | 间距与最大尺寸（Wrap / Radial 相关） |

也就是说：**同一个插槽既能做成横向一条技能图标，也能做成径向排列**，不需要另写控件。

## 8. 设计期占位

`RebuildWidget` 里 `IsDesignTime()` 分支**不注册**扩展点，而是返回一个 `SOverlay` + 居中 `STextBlock`，显示：

```text
┌────────────────────────┐
│    Extension Point     │
│      HUD.Slot.Reticle  │
└────────────────────────┘
```

作用：让 UI 设计者在 Designer 里能看出"这块不是普通 Panel，是一个动态扩展区域"。**设计期不注册 → 别在编辑器里找效果，效果只在 PIE 出现。**

## 9. 项目迁移记录（2026-09-27）

| 项 | 内容 |
|---|---|
| 落点 | `Public/UI/Extension/` + `Private/UI/Extension/`（扁平，与 `IndicatorSystem/` 一致） |
| 文件 | `UIExtensionSystem.h` 438 行 / `.cpp` 534 行；`UIExtensionPointWidget.h` 143 行 / `.cpp` 280 行（含中文注释） |
| 未拷贝 | 上游 `UIExtensionModule.cpp`（并模块后无意义）、`LogUIExtension.h/.cpp` |
| 改动 | `UIEXTENSION_API` → `HODGEPODGE_API`（共 6 处）、`UCommonLocalPlayer` → `UHodgeLocalPlayerBase`（5 处，含头文件前向声明与签名）、include 路径改 `UI/Extension/...`、`LogUIExtension` 改用**就地定义**的日志类别（`.h` DECLARE + `.cpp` DEFINE，14 处调用不动） |
| 构建 | Editor **EXIT=0**（0 error / 0 warning）；Game **EXIT=0** |
| 构建配置 | `Hodgepodge.Build.cs` **零改动**（`UMG`/`Slate`/`SlateCore`/`GameplayTags` 已在依赖里） |

## 10. 坑与待验证

| 项 | 说明 |
|---|---|
| ⚠️ **`GetOwningLocalPlayer<UHodgeLocalPlayerBase>()` 没有空检查** | `RebuildWidget` 里直接 `->CallAndRegister_OnPlayerStateSet(...)`。**若该 Widget 没有 OwningPlayer（例如没经 `CreateWidget(PC, ...)` 创建），会直接崩**。上游 Lyra 同样没有检查（上游 `UIExtensionPointWidget.cpp:38`）。临时做法是保证 HUD 用 `CreateWidget(PC, ...)` 创建；要彻底稳妥就在项目版加一行空判断（属对上游的有意偏离，需单独记录） |
| ⚠️ `FDelegateHandle Handle = ...` 未保存 | 上游原样。它导致**无法在销毁时手动解绑**，但不崩的原因是用 `CreateUObject` 绑的弱 UObject 委托——Widget 销毁后回调自动失效。理解这一点即可，不必专门"修" |
| ℹ️ PlayerState 永不出现时只有 2 个 ExtensionPoint | 见 §3 |
| ℹ️ 设计期不注册 | 见 §8 |
| ℹ️ 未运行验证 | 迁移只证明了"能编过、链接过"。**"槽位能显示内容""Handle 注销能移除"都没有运行证据**。最小验证路径见 [迁移执行文档 §12.4](lyra-ui-migration-plan.md) Step 4：2 张蓝图 + `K2_RegisterExtensionAsWidget(HUD.Slot.Reticle, ...)`（`HUD.Slot.*` 标签项目已注册，无需新增） |

### 10.1 已记录的上游偏离

| # | 位置 | 上游行为 | 本项目行为 | 原因 | 状态 |
|---|---|---|---|---|---|
| 1 | `UIExtensionSystem.cpp:283`（`RegisterExtensionAsData`） | `if (ContextObject)` —— **有** Context 时输出**不带 Context** 的短格式日志；无 Context 时反而输出带 Context 的长格式（并打印 `GetNameSafe(nullptr)` = `None`） | `if (ContextObject == nullptr)` —— 与镜像函数 `UnregisterExtension`（`:427` 用 `IsExplicitlyNull()`）极性一致 | 上游判定与两个分支内容相反，属上游 bug。**功能无影响，但影响"槽位收不到内容"的排查**：带 Context 注册会被日志显示成全局注册 | ✅ 已修（2026-09-27），Editor / Game 双构建 EXIT=0；**未做运行验证** |

**还原方法**：把 `:283` 的 `== nullptr` 去掉即可恢复上游写法。

**判断依据（为什么确定是写反而不是"上游有意"）**：同一个文件里的镜像函数 `UnregisterExtension`（`:426-438`）对同一语义用的是 `IsExplicitlyNull()` → 短格式，`else` → 长格式 + 打印 Context 名。两个对称函数极性相反，日志格式与 `Unregister` 侧完全对应，故判定 Register 侧取反。上游 Lyra `UIExtensionSystem.cpp:170-177` 为同一写法，**不是移植引入**。

## 11. 速记，以及与 Indicator 的关系

```text
GameFeature / 任意模块
   │ RegisterExtensionAsWidget / RegisterExtensionAsDataForContext
   ▼
FUIExtension（内容）  ──┐
                        │ 三条件匹配：Tag / Context（严格相等）/ DataClass
UUIExtensionPointWidget ─┘
   │ 注册最多 3 个 ExtensionPoint（无 Context / LocalPlayer / PlayerState）
   ▼
UUIExtensionSubsystem（WorldSubsystem，只匹配 + 广播 + 补发）
   ▼
OnAddOrRemoveExtension → CreateEntryInternal → UUserWidget → HUD 显示
                        （Handle → Widget 记账，卸载时精确移除）
```

| | UIExtension | IndicatorSystem |
|---|---|---|
| 解决的问题 | **怎么挂**：把控件插进 HUD 的某处（Tag 坑位） | **画在哪**：控件跟随世界目标并在屏幕上定位 |
| 位置语义 | 屏幕空间，固定坑位 | 世界空间，每帧投影 |
| 谁创建控件 | 槽位 `UUIExtensionPointWidget` | `SActorCanvas`（内部有控件池） |
| 生命周期所有者 | `FUIExtensionHandle` + 槽位内 `ExtensionMapping` | `UIndicatorDescriptor` + `UIndicatorManager` |
| 两者的唯一交接点 | **同一张 HUD 蓝图里既放扩展点槽位、又放 `UIndicatorLayer`**（Lyra 实证见[文件导览 §5.3](lyra-ui-file-guide.md)） | 同左 |

---

**变更记录**

| 日期 | 内容 |
|---|---|
| 2026-09-27 | 首版。整合"实体插槽"的学习笔记；补齐三条件匹配（Context 严格相等）、三次注册的条件性、`UDynamicEntryBoxBase` 形态属性、`GetOwningLocalPlayer` 空指针风险等核实结论；记录迁移与双构建结果。未做运行验证。 |
| 2026-09-27 | 新增 §10.1「已记录的上游偏离」：修正 `RegisterExtensionAsData` 注册日志的判定极性（上游 bug，功能无影响但影响排查），双构建 EXIT=0。 |
