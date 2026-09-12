# Experience 与 GameFeature 扩展

[返回首页](README.md)

## 概念边界

Experience 是玩法配置，不是独立插件。ActionSet 是可复用的动作组合，不是另一套正在运行的 Experience。GameFeature 是可加载/激活的插件单元，GameFeatureAction 是激活期间执行的扩展行为。

Experience 可以直接配置 Actions，也可组合 ActionSets；因此没有自定义 GameFeature 插件实例，并不意味着所有 Action 都无法使用。反过来，Action 类存在也不说明某个 Experience 已经配置它。

## WorldActionBase

WorldActionBase 为需要参与世界生命周期的 Action 提供上下文和 World 加入入口。多 PIE 世界下，不应把所有临时句柄放在单个无上下文全局数组里。项目 Action 实现常按 FGameFeatureStateChangeContext 保存运行状态。

## AddAbilities

有有效的能力、属性、AbilitySet 授予和回收代码。它通过组件管理器观察 Actor 扩展事件，寻找或请求 ASC，记录授予句柄供停用撤销。具体入口是否触发依赖目标 Actor Receiver、Action 的 ActorClass 配置及加载流程。

配置前确定目标应该是 PlayerState 还是 Pawn。给 Pawn 动态新增另一个 ASC 可能与玩家 PlayerState ASC 分裂；不要为了让查询不为空而临时生成第二套能力系统。

## AddInputContextMapping

存在 LocalPlayer/设置注册逻辑和 AddMappingContext 函数体，但 Controller 扩展中的添加分支被注释。注册用户设置里的映射上下文与把映射加入当前活动输入不是同一操作。应验证每个入口，而不是全文搜索出现 AddMappingContext 就标记完成。

## AddInputBinding

依赖 HeroComponent 的 Ready 状态和额外 InputConfig 增删。当前扩展添加分支、查找 Hero 和实际调用都被注释；HeroComponent 又整体停用，所以这条扩展链不完整。PawnsAddedTo 记录存在也不能证明绑定已发生。

## AddGameplayCuePath 与 Policy

Cue 路径 Action 当前提供目录配置和编辑器校验，实际增删路径依赖 Policy 观察者；观察者内部的路径增删和资源库刷新仍被注释。另有自定义 CueManager 未配置、启动钩子空实现。加载插件资源、注册 Cue 搜索路径、预加载 Cue 是不同步骤，应分别验收。

Policy 中保留了上游迁移注释；检查实际激活类配置及有效函数体。不要照搬依赖 CommonGame、CommonUI、GameSettings 等未引入模块的代码。

## AddWidget 与 SplitscreenConfig

AddWidget 当前仍是注释实现，不能视为完整 HUD 注入。SplitscreenConfig 的职责是分屏策略，而非输入和玩家生成。项目使用专用 HUD 基类不等于已经接入 Lyra UIExtension。

## Receiver 生命周期问题

CharacterBase 在 PreInitializeComponents 注册 Receiver，在 BeginPlay 发送 GameActorReady。但 EndPlay 当前又发送 GameActorReady，未像 PlayerStateBase 那样移除 Receiver。这是需要修复的生命周期不对称，可能影响扩展撤销。

## 新增扩展的验收原则

激活前记录目标组件/能力/输入数量；激活一次增加预期项；重复通知不重复授予；停用恢复基线；再次激活仍正常；双 PIE 世界互不污染。扩展不仅要“加得上”，也要“卸得掉”。

源码入口：[GameFeatures 索引](Reference/source-index.md)、[CharacterBase](../../Source/Hodgepodge/Private/Character/HodgeCharacterBase.cpp)、[PlayerStateBase](../../Source/Hodgepodge/Private/Core/PlayState/HodgePlayerStateBase.cpp)。
