# PawnExtension 与 Hero 初始化

[返回首页](README.md) · [输入](06-input.md) · [GAS](07-gas.md)

> 最近源码核对：2026-09-13。两个组件现在都已启用，以下描述有效源码；本轮未执行 PIE。

## 创建与所有权

AHodgeCombatCharacter 构造创建 PawnExtension；AHodgeHeroCharacter 构造新增 HeroComponent。GameMode 延迟生成 Pawn，在 FinishSpawning 前查找 PawnExtension 并调用 SetPawnData。PlayerState 仍拥有 ASC，Pawn 是 Avatar。新 GameMode 默认选择 AHodgePlayerController，默认 PawnClass 回退仍是 CharacterBase，实际 PawnData.PawnClass 待资产确认。

SetPawnData 只接受权威端首次赋值，随后 ForceNetUpdate 并检查状态；客户端 OnRep_PawnData 也检查状态。这个 setter 不支持随意重复切换配置，换职业/换 Pawn 需定义生命周期。

## 两个 Feature 如何配合

PawnExtension OnRegister 注册 Feature；BeginPlay 监听其他 Feature，进入 Spawned 并尝试推进。HeroComponent 也注册 Feature，监听 PawnExtension 的 DataInitialized 通知。

1. PawnExtension 的 Spawned→DataAvailable 要求 PawnData；权威端或本地控制角色还需要 Controller。
2. Hero 的 Spawned→DataAvailable 要求 HodgePlayerState。非模拟代理还要求 Controller 与 PS 的 Owner 配对；真实本地玩家还要求 InputComponent、HodgePlayerController 和 LocalPlayer。
3. PawnExtension 等全部 Feature 到达 DataAvailable 后进入 DataInitialized。
4. Hero 等 PawnExtension 到达 DataInitialized，再进入自己的 DataInitialized，实际初始化 ASC、输入和相机。
5. 两者最后进入 GameplayReady；Hero 末端没有完整能力初始化校验，因此不能据此证明技能已授予。

这个依赖方向避免互相等待更晚状态。新增 Feature 时要画清楚依赖，不要让双方都等待对方的 DataInitialized。

## Hero 的实际装配动作

HandleChangeInitState 从 PawnExtension 获取 PawnData，调用 InitializeAbilitySystem(HodgePS ASC, HodgePS)。如果控制器类型和输入组件满足条件，调用 InitializePlayerInput；随后绑定 CameraComponent.DetermineCameraModeDelegate。

初始化输入需要 HodgeLocalPlayerBase 和 EnhancedInput 子系统；DefaultEngine.ini 已选择这个 LocalPlayer 类。网络服务器没有 LocalPlayer，必须验证当前分支不会在无本地输入的 Pawn 上进入本地初始化。

角色旧 PossessedBy / OnRep_PlayerState 直接 InitAbilityActorInfo 仍保留，所以现在是“组件入口已接入，但旧路径未收敛”，不再是“InitializeAbilitySystem 零调用者”。两条路径对动画、OnSpawn 能力和回调的先后仍需运行核对。

## PawnExtension 的 ASC 接入

InitializeAbilitySystem 处理已有 ASC 和旧 Avatar，保存指针，调用 InitAbilityActorInfo，并把 PawnData.TagRelationshipMapping 设置到 ASC，再广播 OnAbilitySystemInitialized。相同 ASC 的早返回体现幂等策略，但需要与换 Pawn、先直接初始化后组件初始化的顺序一起检查。

UninitializeAbilitySystem 在当前 Avatar 仍属于自己时取消不保留的技能，调用 ClearAbilityInput、移除 Cue、解除 Avatar/ActorInfo 并广播；最后清空保存的指针。SurvivesDeath 标签定义部分能力可以保留，但不自动决定属性和 GE 的重生策略。

## 解除与重生的剩余风险

新 HodgePlayerController::OnUnPossess 会先把匹配的 ASC Avatar 清空，再调用父类。PawnExtension 解除主体又要求 Avatar 等于自己，因此应核对实际执行顺序：提前清空 Avatar 可能使取消技能、清输入、移 Cue 和广播分支被跳过。此处是静态条件风险，本轮未复现。

Hero::EndPlay 只注销 Feature 并调用父类；额外输入句柄持久化、额外输入移除和相机覆盖清理仍需独立验证。不要把“有 Uninitialize 函数”当成所有退出路径都会走到它。

## 已有调试入口

当前用户修改加入 HODGE-DBG 日志，能观察 PlayerState 空、Controller/PS 未配对、InputComponent/LocalPlayer 未就绪、PawnExtReady 和相机委托绑定。日志代码存在不代表当前运行已产生这些日志。

调试记录至少同时保存 World、NetMode、角色名、实际控制器类型、PawnData、ASC Owner/Avatar。静态计数日志跨对象累加不能直接解释成单 Pawn 的重试次数。

## 验收重点

分别检查服务器、拥有客户端和模拟代理；确认角色查询到的 ASC 与 PlayerState 相同，Owner 是 PS、Avatar 是当前 Pawn；重复初始化不重复绑定；换 Pawn 后旧身体不再收到输入、技能或回调。运行证据补齐前，状态保持“源码已接入，运行待验证”。

源码：[PawnExtension](../../Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp)、[HeroComponent](../../Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp)、[HeroCharacter](../../Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp)、[PlayerController](../../Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerController.cpp)。
