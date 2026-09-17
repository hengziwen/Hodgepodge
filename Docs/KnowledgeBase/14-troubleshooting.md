# 按症状定位的排障手册

> 最近源码核对：2026-09-17。源码接入状态与运行验收分开记录。
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

## 攻击时间轴不推进或阶段标签残留

`UHodgeAbilityTask_PlayTimeline` 依赖 World 时间差累加：先确认 Task 是否被创建（源码内没有 C++ 调用 `PlayTimeline`，需确认 BP 是否接线）、Timeline 资产 Duration 是否大于 0、InitialPlayRate 是否大于 0。标签残留优先看 `OnDestroy` 是否执行到 `ClearAllPhaseTags`，以及是否有别的来源在加同名 loose tag。事件没到客户端时按 `EHodgeTimelineEventNetPolicy` 区分端，不要假设 `HandleGameplayEvent` 会自动 RPC。

## 首次进战斗卡顿或 Montage 未加载

`PreloadPrimaryAssetBundles` 是同步阻塞加载，正常只在 Ability 被授予时发生。若卡顿出现在输入或 Tick，检查是否把预加载放错位置；若 Montage 缺失，检查 HodgeAbilityTimeline / HodgeComboSet 的 `UpdateAssetBundleData` 是否收集了该 Montage，以及 DefaultGame.ini 是否登记了两个 PrimaryAssetType。

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

## 日志取证模板

记录：复现步骤、实际地图与 Experience、单机/监听/专服、角色类型、服务器与客户端分别观察到的结果、首个异常日志和调用栈、最近修改。先定位最早断点，再处理后续连锁空指针。

推荐搜索：

```powershell
rg -n 'Identified experience|EXPERIENCE|PawnData|Ability|Error|Fatal' Saved/Logs
rg -n 'ProcessAbilityInput|InitializeAbilitySystem|SetPawnData|DetermineCameraModeDelegate' Source/Hodgepodge
```

第二条搜索会包含注释命中，必须打开上下文复核。
