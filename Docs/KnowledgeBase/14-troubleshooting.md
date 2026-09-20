# 按症状定位的排障手册

> 最近源码核对：2026-09-19。源码接入状态与运行验收分开记录。
[返回首页](README.md)

## 启动加载失败或看不到玩家

先查实际地图、GameMode 和 `Identified experience`。没有日志则看 Experience 分配是否执行或被专服登录分支截断；有 ID 但失败则核对 PrimaryAsset 类型、扫描目录和资源类；Loaded 后没有 Pawn 则看 RestartPlayer 条件、PlayerStart、PawnClass 和生成碰撞。

不要先修改输入：没有正确 Pawn，输入修复也没有承载对象。

## Init State 永远停在 Spawned

在 PawnExtension::CanChangeInitState 看 PawnData 是否为空，再看 Controller。GameMode 的数据注入已经启用，检查是否实际执行、Pawn 是否含扩展组件、数据是否非空。客户端单独失败时查 PawnData 是否复制到达及 OnRep 是否重试。

## 显示 GameplayReady 但角色不能操作

当前状态链并不全面校验输入和相机。看实际 HeroComponent 是否启用并挂载；看本地 InputComponent 类型；查看 IMC 和绑定；检查 Move/Look 回调。不要用状态名代替操作验证。

## PlayerState 有 ASC，但角色返回空

旧直接 InitAbilityActorInfo 不会保存 PawnExtension 指针；但当前 HeroComponent 已有有效接入调用。出现为空时优先检查 Hero 的 Init State 是否到达 DataInitialized、实际 Pawn 类是否含 HeroComponent，而非再次添加相同调用。

## 按键回调进来了，技能不激活

按顺序看 Spec 是否授予、动态 InputTag 精确匹配、Pressed/Held 缓存、ProcessAbilityInput 是否每帧执行，再看 CanActivateAbility 的失败标签、Cost、Cooldown、ActivationGroup。若第一次成功第二次失败，重点看 EndAbility 和组计数。

## 攻击时间轴不推进、窗口标签残留，或 Ability 迟迟不结束

`UHodgeAbilityTimeline` 与 `UHodgeAbilityTask_PlayTimeline` **已按统一事件模型重新实现**（单一 `Events[]`，`Kind = Window / Point`），详见 [第一阶段设计](../Design/ability-timeline-stage1.md)。本节的症状按发生率排列：

**Task 建了但不推进**：先确认 `ReadyForActivation()` 被调用。C++ 侧工厂只创建、**必须自己调**，漏掉时既不报错也不推进；蓝图侧用 latent 节点（`K2Node_LatentAbilityCall`）则自动补。再搜日志 `[Hodge] PlayTimeline 参数非法`，对应 `Timeline` 为空 / `Duration<=0` / `InitialPlayRate<=0` / `StartOffset` 越界。

**窗口标签残留（`Status.Attack.*` 计数不为 0）**：`WindowTag` 由 `ExitWindow` 与 `ClearAllWindowState` 配对撤销，两处都有反直觉约束：

- `ExitWindow` **不能**加 `if (bStopped) return;` —— 它已经先把该下标从账本摘掉，重入进来的清理不会再管这个窗口，提前返回会让计数**永久多 1**；
- `EnterWindow` 正相反：写完 tag **之后**必须先查 `bStopped || bCleanedUp` 再施加 GE —— 只靠"施加后补偿"撤不掉已经发生过的 `OnActive` / Cue / 属性变化。

也就是说"给所有 ASC 调用之后统一加守卫"这个做法是错的，判断依据是**这个操作是否已经被记进账本**。

**Ability 远超预期时长一直挂着**：查 `TickTask` 的 `WorldDelta <= 0` 守卫是否在**累加 `LogicalElapsed` 之前**。放到后面拦不住 —— 负差值已经把逻辑时间污染成负数，`ElapsedTime >= Duration` 要等它爬回来才成立，且早期节点会被重新跨过（Point 重复派发）。

**事件没到客户端**：按 `EHodgeTimelineEventNetPolicy` 区分端，不要假设 `HandleGameplayEvent` 会自动 RPC。特别注意 `PointEffectClass + LocallyControlledOnly` 已被校验定为 **Error**：该组合下 listen server 主机自己控制的角色会施加 GE，而远程客户端不会。

**要主动打断时间轴做验证**：`asc.clear_ability(handle)`（用法见下面 MCP 一节）。取消后 `Status.Attack.*` 与窗口 GE 都应归零，且**此后不应再出现后续窗口标签或 Point 派发**（出现就是"停不下来还在 tick"）。注意 `GameplayEvent.Attack.Interrupted` **在取消时不会派发**，这是设计约定（`AbilityCancelled` 不广播；`Interrupted` 只给"被外力抢占"，本阶段没有触发者）——别把它当成漏派发的 bug 去查。

## ~~首次进战斗卡顿或 Montage 未加载~~（⚠️ 预加载实现不存在）

> `PreloadPrimaryAssetBundles` / `PreloadPrimaryAssetsOnGrant` 与 Timeline / ComboSet 的 Bundle 收集**当前不存在**，所以"预加载放错位置"这类排查暂不适用。
>
> 仍然有效的相关机制：`FHodgeBundles::Equipped` 存在，`UHodgeExperienceManagerComponent::StartExperienceLoad()` 会按端加载 Experience 的 `Equipped` Bundle；进战斗卡顿要先从 Experience Bundle 与 `-LogAssetLoads` 入手。

~~`PreloadPrimaryAssetBundles` 是同步阻塞加载，正常只在 Ability 被授予时发生。若卡顿出现在输入或 Tick，检查是否把预加载放错位置；若 Montage 缺失，检查 HodgeAbilityTimeline / HodgeComboSet 的 `UpdateAssetBundleData` 是否收集了该 Montage，以及 DefaultGame.ini 是否登记了两个 PrimaryAssetType。~~

## 相机在原点、视角不跟随或旋转异常

当前模式绑定已有实现，检查状态是否到达、模式委托是否绑定、返回模式是否非空、栈内是否有模式、GetCameraView 是否覆写了空视图。再核对默认相机模式配置、目标 Pawn、PlayerCameraManager 类型。只改 CameraComponent 相对位置通常不能修复模式选择入口。

## 创建 GE Spec 触发 check

若在 HodgeGameplayAbility::MakeEffectContext 的 check，检查实际 Context ScriptStruct；当前 Globals 类和分配配置均已存在；检查是否加载旧 DLL、配置是否被覆盖以及实际 Globals 类型。若是其他 check，记录完整调用栈，不要把所有 GAS 崩溃归为同一原因。

## 血量属性存在，但攻击不扣血

确认服务器命中目标、目标 ASC、GE 是否应用、GE 修改哪个 Attribute。HealthSet 现已实现 Damage/Healing 到 Health 转换。检查 GE 是否写入正确元属性、免疫/GodMode 是否拦截、目标是否真正有 HealthSet，再追踪命中和效果应用。

## 远端血量归零，但服务器没有死亡

OnRep 可以在客户端触发耗尽事件；服务器权威处理不能依赖 OnRep。服务器 GE 结算后的耗尽广播已经实现，下一观察点是角色健康/死亡组件订阅，后者仍停用。

## 动画不播放

先确认角色位置真的在动，再查 Mesh、Skeleton、AnimClass、ABP 状态机变量、GroundDistance 和 ASC Tag 映射。技能动画另查 Montage Slot、Ability 是否存活和网络执行端。

## 重生后重复输入、技能数量增加

检查重复绑定句柄、重复授予 AbilitySet、旧 Pawn 未解绑、输入 Held 缓存未清。记录每次重生前后 ASC 地址/Owner/Avatar、Spec 数量和输入组件身份。

## GameFeature 停用后仍有内容

查授予句柄是否按上下文保存，Actor Receiver 是否正常移除，输入额外配置是否解绑。CharacterBase Receiver 已配对、Hero 额外输入移除已实现并在 EndPlay 统一清理，所以先怀疑运行时顺序（Pawn 已换、记录被提前清空）和记录未覆盖的来源，而不是再看 TODO。

PlayerState::SetPawnData 授予 AbilitySet 时未记录句柄，GameFeature 或重生想撤销玩家基础能力时无可撤销对象——这属于设计缺口，不是运行时 bug。

## MCP 连不上、连上了却"什么都没发生"

两个 Editor-only 桥已实测可用（2026-09-19）：`UnrealMCP`（TCP 55557，前端 `Tools/UnrealMCP/server.py`）与 `McpAutomationBridge`（原生 MCP Streamable HTTP，`POST http://127.0.0.1:3016/mcp`，令牌头 `X-MCP-Capability-Token`，令牌在 `Saved/MCP/capability-token`，会话头 `Mcp-Session-Id`）。踩过的坑按症状列出：

| 症状 | 真实原因 | 做法 |
|---|---|---|
| `429 Native MCP session limit reached`，随后报 `Missing Mcp-Session-Id header` | 桥有**并发会话上限**，而客户端每轮 `initialize` 新建会话却从不关闭；额度耗尽后连握手都做不成 | 用完必须 `DELETE /mcp` 带 `Mcp-Session-Id` 关闭（清理动作不能依赖 `initialize`） |
| 写字段报 `cannot be edited on instances` | 该属性是 `EditDefaultsOnly`（= `CPF_DisableEditOnInstance`），UE Python 的 `set_editor_property` 明确拒写 | 走桥的 `inspect.set_property`（走编辑器属性系统），它能写、且会自动存盘 |
| 校验返回 `isValid: true`，坏数据也不报错 | `manage_asset.validate` 与 `system_control.validate_assets` **名字骗人**：只检查"资产能否加载"（后者描述里写着 `manage_asset exists fallback`），**不跑 `IsDataValid`** | 真入口是 `UEditorValidatorSubsystem::IsAssetValid(AssetData, DataValidationUsecase)`；Python 侧返回三元组 `(result, errors, warnings)`，可直接读消息而不必扒日志 |
| `is_asset_valid() required argument 'validation_usecase'` | 少传枚举 | `unreal.DataValidationUsecase.MANUAL`（另有 `NONE` / `COMMANDLET` / `SAVE` / `PRE_SUBMIT` / `SCRIPT`） |
| PIE 内 `EditorAssetLibrary.load_asset` / `find_asset_data` 返回 `None` | PIE 里按 `/Game` 路径加载会失效 | 改用 `AssetRegistryHelpers.get_asset_registry().get_assets_by_path(...)` 拿 `AssetData` 再 `get_asset()`；蓝图取类用 `bp.generated_class()`（在 Python 里是**方法**，要带括号） |
| PIE 里 `PlayerState` 是普通类、`PAWN=None`、`ASC_COUNT=0` | PIE 起在 `EditorStartupMap`（当前是 `L_MainMenu`），那里面没有 Hodge 链 | 先用 `control_editor.open_asset` 把编辑器关卡切到 `ThirdPersonMap` 再 `play` |
| Python 造不出 `FGameplayTag` | `TagName` 只读、构造函数不收 `TagName`、`make_literal_gameplay_tag` 收 tag、`add_gameplay_tag(_to_container)` 收 container | 直接从数据资产里取现成的 tag 对象（如 `Events[i].WindowTag`），用 `GameplayTagLibrary.get_tag_name()` 读名字 |
| 想让桥编译 C++ 或触发 Live Coding | 桥**没有构建入口**（只有 `blueprint.compile`、`material.compile_material`） | C++ 变更走编辑器外构建或 Ctrl+Alt+F11；新增 `UCLASS` 还必须**重启编辑器**才会进反射 |
| 调完 `manage_gas.add_ability_task(taskType="PlayTimeline")` 读图表只有两个事件节点 | 它会建出真正的 `K2Node_LatentAbilityCall`（自动补 `ReadyForActivation`），但**不是立刻**出现在 `get_blueprint_graph` 结果里 | 别急着判"只加了变量"；重新读一次图表再下结论 |

桥暴露的是**单工具网关**（只有一个 `unreal` 工具，四个操作 `search` → `describe` → `execute` → `configure`），并且明令**不许猜** capability / tool / action / 参数名：先用 `search` 拿行、再用行里的 `nextCall` 原样 `describe` 拿契约。父工具名与能力前缀**不一致**是常见坑：`blueprint.*` 的父工具是 `manage_blueprint`、`asset.validate` 的父工具是 `manage_asset`，猜错只会得到空的 `CAP None`。

采样类验收要留意的时序问题：MCP 调用之间天然相隔数秒，而时间轴通常只有 1 秒。把被驱动节点的 `InitialPlayRate` 调小（实测用 `0.05`，即放慢 20 倍）就能在运行过程中从容采样；且**不要在 `run_python_in_unreal` 里 `sleep`** —— 它跑在游戏线程上，sleep 会把 PIE 一起冻住。

**PIE 内访问资产与对象**（比 `load_asset` 更隐蔽的一类坑）：

- 不止 `EditorAssetLibrary.load_asset`：**`load_blueprint_class` 在 PIE 里同样返回 `None`**，而这个失败会**伪装成别的问题**——引擎日志只留下 `[LogAbilitySystem] BuildAbilitySpecFromClass called with an invalid Ability Class.` / `K2_GiveAbility() called with an invalid Ability Class.`，表面症状是 `try_activate_ability` 返回 `False`、什么也没发生。一律改用 `AssetRegistryHelpers.get_asset_registry().get_assets_by_path(...)` + `AssetData.get_asset()`，蓝图再取 `.generated_class()`。
- PIE 世界用 `unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()`；`EditorLevelLibrary.get_pie_world` / `get_game_world` **都不存在**。`PlayerController.Pawn` 是 protected（Python 读不了），`UWorld::GetTimeSeconds` 也没暴露——用 `GameplayStatics.get_time_seconds(world)`。
- ASC 上这几个名字与直觉不同：`get_gameplay_effect_count(ge_class, optional_instigator_filter_component)`（**中间那个参数在 Python 绑定里是必需的**，传 `None`）、`get_active_effects(query)`、`activatable_abilities`（返回 `FGameplayAbilitySpecContainer`，**Python 不可迭代**）。Python 里也**没有** `GameplayTagLibrary.request_gameplay_tag`。

**写 `EditDefaultsOnly` 字段**（尤其是数组元素里的结构体字段）：

- Python `set_editor_property` 会报 `... cannot be edited on instances`；桥的 `inspect.set_property` 又**穿不过 `TArrayProperty`**（`Cannot traverse into property 'Events' of type 'ArrayProperty'`）。
- 可行路径是**结构体文本往返**：`evs = list(asset.get_editor_property("Events"))` → `txt = evs[i].export_text()` → 改文本 → `evs[i].import_text(txt)` → 再把**整个数组**写回（数组本身是可写的）。`FGameplayTag` / `FGameplayTagContainer` 也能这样造：`t = unreal.GameplayTag(); t.import_text('(TagName="GameplayEvent.Attack.Test")')`。这条通道绕过的是实例编辑守卫，不是 DPI。
- `inspect.set_property` 对**非数组**的嵌套路径（如 BP CDO / 资产的标量、结构体字段）仍然好用，失败时它的报错会指明卡在第几段。

**"事件到底有没有派发出去"怎么观测**：`HandleGameplayEvent` 唯一的订阅方是"带匹配 `AbilityTriggers` 的能力"。造一个测试能力，把事件标签写进 `AbilityTriggers`（`TriggerSource = GameplayEvent`），再把**同名标签**写进 `ActivationOwnedTags`——派发发生时该能力就被激活，标签计数 `0 → 1` 即证据，而且**标签会一直留着**，隔几次调用再采样也读得到。比改蓝图图表加 `WaitGameplayEvent` 节点省事得多。
（顺带记一个不一致：`manage_gas.add_ability_task(taskType="WaitGameplayEvent")` **只写配置变量、不建图节点**，与 `PlayTimeline` 的行为不同，别指望它。）

**PIE 启停**：`control_editor.play` / `control_editor.stop_pie`。

**怎么在 PIE 里主动"打断"一个正在跑的能力**：`asc.clear_ability(handle)`（返回 `None`，void）。它会连带取消活动中的实例 —— `CancelAbility` → Task `EndTask` → `OnDestroy` 清理，实测取消后窗口标签与 GE 都归零、且时间轴不再推进。跨调用传递句柄时注意 **`FGameplayAbilitySpecHandle.Handle` 是私有属性**（`get_editor_property("Handle")` 直接抛异常），用 `handle.export_text()` / `import_text()` 往返（文本形如 `(Handle=17)`）。

另外两点小坑：**编辑器进程的 CWD 不是项目根**，Python 里 `open("Saved/...")` 会 `FileNotFoundError`，写文件要用绝对路径（桥自己的 `file` 参数倒是吃项目相对路径）；`manage_blueprint` 的 `get_graph_details` 能一次读出全部节点与引脚（含 `linkedTo`），但要先用 `tool=manage_blueprint` + `action=get_graph_details`，**没有** `get_blueprint_graph`。

## 日志取证模板

记录：复现步骤、实际地图与 Experience、单机/监听/专服、角色类型、服务器与客户端分别观察到的结果、首个异常日志和调用栈、最近修改。先定位最早断点，再处理后续连锁空指针。

推荐搜索：

```powershell
rg -n 'Identified experience|EXPERIENCE|PawnData|Ability|Error|Fatal' Saved/Logs
rg -n 'ProcessAbilityInput|InitializeAbilitySystem|SetPawnData|DetermineCameraModeDelegate' Source/Hodgepodge
```

第二条搜索会包含注释命中，必须打开上下文复核。
