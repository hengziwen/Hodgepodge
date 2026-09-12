# 按症状定位的排障手册

[返回首页](README.md)

## 启动加载失败或看不到玩家

先查实际地图、GameMode 和 `Identified experience`。没有日志则看 Experience 分配是否执行或被专服登录分支截断；有 ID 但失败则核对 PrimaryAsset 类型、扫描目录和资源类；Loaded 后没有 Pawn 则看 RestartPlayer 条件、PlayerStart、PawnClass 和生成碰撞。

不要先修改输入：没有正确 Pawn，输入修复也没有承载对象。

## Init State 永远停在 Spawned

在 PawnExtension::CanChangeInitState 看 PawnData 是否为空，再看 Controller。GameMode 的数据注入当前注释，是第一观察点。客户端单独失败时查 PawnData 是否复制到达及 OnRep 是否重试。

## 显示 GameplayReady 但角色不能操作

当前状态链并不全面校验输入和相机。看实际 HeroComponent 是否启用并挂载；看本地 InputComponent 类型；查看 IMC 和绑定；检查 Move/Look 回调。不要用状态名代替操作验证。

## PlayerState 有 ASC，但角色返回空

这是当前明确存在的双路径问题。直接 InitAbilityActorInfo 不会给 PawnExtension::AbilitySystemComponent 赋值。检查 InitializeAbilitySystem 是否真正从有效代码调用，不能只看注释草稿命中搜索结果。

## 按键回调进来了，技能不激活

按顺序看 Spec 是否授予、动态 InputTag 精确匹配、Pressed/Held 缓存、ProcessAbilityInput 是否每帧执行，再看 CanActivateAbility 的失败标签、Cost、Cooldown、ActivationGroup。若第一次成功第二次失败，重点看 EndAbility 和组计数。

## 相机在原点、视角不跟随或旋转异常

检查模式委托是否绑定、返回模式是否非空、栈内是否有模式、GetCameraView 是否覆写了空视图。再核对默认相机模式配置、目标 Pawn、PlayerCameraManager 类型。只改 CameraComponent 相对位置通常不能修复模式选择入口。

## 创建 GE Spec 触发 check

若在 HodgeGameplayAbility::MakeEffectContext 的 check，检查实际 Context ScriptStruct；需要自定义 Globals 分配 Hodge Context。若是其他 check，记录完整调用栈，不要把所有 GAS 崩溃归为同一原因。

## 血量属性存在，但攻击不扣血

确认服务器命中目标、目标 ASC、GE 是否应用、GE 修改哪个 Attribute。HealthSet::PostGameplayEffectExecute 主体目前注释，写入一个伤害量不会自动按注释里的公式扣 Health。先用最小 GE 验证属性，再接伤害转换。

## 远端血量归零，但服务器没有死亡

OnRep 可以在客户端触发耗尽事件；服务器权威处理不能依赖 OnRep。检查服务器 GE 结算后的耗尽广播与角色健康/死亡组件绑定，两者目前都不完整。

## 动画不播放

先确认角色位置真的在动，再查 Mesh、Skeleton、AnimClass、ABP 状态机变量、GroundDistance 和 ASC Tag 映射。技能动画另查 Montage Slot、Ability 是否存活和网络执行端。

## 重生后重复输入、技能数量增加

检查重复绑定句柄、重复授予 AbilitySet、旧 Pawn 未解绑、输入 Held 缓存未清。记录每次重生前后 ASC 地址/Owner/Avatar、Spec 数量和输入组件身份。

## GameFeature 停用后仍有内容

查授予句柄是否按上下文保存，Actor Receiver 是否正常移除，输入额外配置是否有实际 Remove 实现。CharacterBase EndPlay 事件不对称和 Hero 额外输入移除 TODO 是现有观察点。

## 日志取证模板

记录：复现步骤、实际地图与 Experience、单机/监听/专服、角色类型、服务器与客户端分别观察到的结果、首个异常日志和调用栈、最近修改。先定位最早断点，再处理后续连锁空指针。

推荐搜索：

```powershell
rg -n 'Identified experience|EXPERIENCE|PawnData|Ability|Error|Fatal' Saved/Logs
rg -n 'ProcessAbilityInput|InitializeAbilitySystem|SetPawnData|DetermineCameraModeDelegate' Source/Hodgepodge
```

第二条搜索会包含注释命中，必须打开上下文复核。
