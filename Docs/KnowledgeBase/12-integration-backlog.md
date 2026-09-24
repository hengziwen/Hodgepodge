# 当前状态、断点与接通顺序

[返回首页](README.md) · [上一轮变更](22-update-2026-09-19.md) · [本轮变更](23-update-2026-09-22.md)

> 核对日期：2026-09-22（沿用 2026-09-17 的 KB 编号）。保留 KB 编号便于追踪，但“源码已接入”与“运行验收通过”严格区分。

## 旧缺口现已在源码接入

### KB-01：Pawn 生成与数据注入——源码已接入

GameMode 使用延迟构造，在 FinishSpawning 前查找 PawnExtension 并调用 SetPawnData（未注释）；另一条路径是 PlayerState::OnExperienceLoaded → GameMode::GetPawnDataForController → SetPawnData。默认 PawnClass 回退仍为 CharacterBase，BP_Hero_Pover 的实际 PawnClass 待编辑器确认。

### KB-02：Hero 协调组件——ASC 接入已收敛到单一入口

HeroComponent 在 PawnExtension 到达 DataInitialized 后调用 PawnExtension::InitializeAbilitySystem；HeroCharacter 的 PossessedBy / OnRep_PlayerState 已退化为只调用 Super。PlayerState::PreInitializeComponents 仍用 GetPawn() 做占位首绑。剩余工作是换 Pawn / 重生时的清理顺序运行验证。

### KB-03：输入绑定与消费——主链已接入，条件仍待验证

HodgeInputComponent 与 HodgeLocalPlayerBase 已配置；Hero 绑定原生和技能输入；HodgePlayerController::PostProcessInput 调用 ProcessAbilityInput。额外输入句柄已持久化到 AdditionalInputConfigHandles，`RemoveAdditionalInputConfig` 已实现，EndPlay 统一解绑。`Input_Move` 现在额外记录"移动意图"（原始输入量），并新增 `Input_MoveStopped` 绑到 `Completed`/`Canceled` 清零，供"移动取消后摇"消费（`HasMoveIntent` / `GetMoveIntent` / `OnMoveIntentChanged`）；该意图已在 PIE 用 `simulate_input` 实测。仍需关注：DefaultInputMappings 与软引用加载、AddMappingContext 被 bRegisterWithSettings 条件包住、ClearAllMappings 可能清除其他来源映射、Ready 事件不保证绑定成功、AutoRun 切换调用仍注释。

### KB-04：相机入口——源码已接入，资产值待验证

Hero 绑定 DetermineCameraModeDelegate；模式选择优先技能临时覆盖，否则 PawnData.DefaultCameraMode。控制器已选 HodgePlayerCameraManager。模式资产换为 `CM_ThirdPerson` / `CM_ThirdPerson_Death`，旧 `CM_Default` 已删除；PawnData 实际指向待编辑器确认。

### KB-05：基础 AbilitySet 授予——已在源码接入

PlayerState::SetPawnData 现于权威端遍历 PawnData->AbilitySets，对每个非空集合调用 `GiveToAbilitySystem(ASC, nullptr)`，随后发送 NAME_HodgeAbilityReady 并 ForceNetUpdate。注意**未记录 GrantedHandles**：没有可撤销/防重的句柄，防重仅靠“已有 PawnData 时提前返回”。后续若需要撤销或换装，应引入按来源保存的句柄。

### KB-06：自定义 EffectContext——代码与配置已接入

HodgeAbilitySystemGlobals::AllocGameplayEffectContext 返回 Hodge Context；DefaultGame.ini 已选择该类。运行时实际类型仍需核对。

### KB-12：Receiver 与 ASC 退出顺序——Receiver 已修正，退出顺序待验证

CharacterBase 在 PreInitializeComponents 注册 Receiver、BeginPlay 发送 GameActorReady、EndPlay 成对调用 RemoveGameFrameworkComponentReceiver，生命周期已对称（此前的不对称问题已修复）。新控制器 OnUnPossess 先清空 ASC Avatar，可能使 PawnExtension 依赖 Avatar 匹配的清理分支跳过；仍需按真实生命周期验证。

### KB-13：LocalPlayer 与 Feature Policy——配置已补齐

LocalPlayerClassName 和 GameFeaturesManagerClassName 已指向项目类型。Policy 的具体观察者注册仍是另一个环节，见 KB-07。

## 当前仍应优先处理

### KB-07：Cue——管理器配置已有，路径注册与预加载不完整

GlobalGameplayCueManagerClass 已配置；Policy 观察者的添加路径回调主体已启用，但 InitGameFeatureManager 创建 AddGameplayCuePaths 观察者的一行仍注释。注销移除主体也停用；AssetManager 初始化钩子仍占位，常驻 GameplayCueNotifyPaths 未启用。不能把配置类名等同于路径注册和预加载成功。

### KB-08：Health 结算——主体已有，角色死亡衔接仍缺

HealthSet 已有 Damage/Healing 元属性、转换、免疫/调试条件和耗尽广播。CombatCharacter 的 HealthComponent 创建与事件绑定仍注释；CombatComponentBase 无命中/连招主体。⚠️ 时间轴已重新实现并通过实测（窗口标签、窗口 GE 施加/移除、Point 与 `Timeline.End` 派发、中途取消的清理），但**连招（ComboSet）与预加载仍不存在**（见 KB-15），且时间轴的**重入类时序**（`EnterWindow` 两道防线 / `ExitWindow` 不对称 / GE 失败补偿）与跨端、时钟倒退尚未验证，所以攻击侧仍**不能**当成"只差接线"。剩余工作是具体攻击 Ability → 技能命中→GE→耗尽→角色死亡/重生。

### KB-09：敌人 ASC——仍缺完整初始化

EnemyCharacter 现在只设置 AI 自动控制和 `AutoPossessAI`；自身旋转/移动参数已注释、继承 Combat 基类，没有完整的敌人 ASC 创建与关联。先明确 ASC 所有者，再验证服务器伤害目标和远端状态。

### KB-10：专服启动——仍有提前返回风险

TryDedicatedServerLogin 在默认地图条件满足时返回 true，实际登录和续接回调仍停用；还没有独立 Server Target。PIE 服务端与独立服务器构建必须分开记录。

### KB-14：GameFeature 输入——撤销闭环已补，运行验证待做

AddInputBinding、AddInputContextMapping 的扩展添加分支均已启用。Hero::RemoveAdditionalInputConfig 已实现（按 InputConfig 解绑并移除记录），EndPlay 统一解绑，绑定句柄已持久化。ContextMapping 的 ControllersAddedTo 记录路径仍需复核。验收必须覆盖激活、停用、再次激活和多世界。

### KB-15：攻击时间轴与 ComboSet——时间轴已重新实现并部分验证；连击仍不存在

> **状态更新（2026-09-19）**：`UHodgeAbilityTimeline` 与 `UHodgeAbilityTask_PlayTimeline` 已按**统一事件模型**（单一 `Events[]`，`Kind = Window / Point`）**重新实现并编译通过**；数据校验规则、调度器（窗口进入/退出、自然结束清理、起点接续不重放历史），以及**窗口 GE 的施加/移除**与 **Point / `Timeline.End` 事件派发**都已在编辑器与 PIE 实测通过。完整证据与验证边界见 [本轮记录](22-update-2026-09-19.md)。
>
> **仍然不存在**：`UHodgeComboSet`、`UHodgeAssetManager::PreloadPrimaryAssetBundles`、`HodgeGameplayAbility::PreloadPrimaryAssetsOnGrant`，以及旧设计的 `Attack.Entry.*` / `Attack.Transition.*` / `Status.AttackMode.*`。
>
> ✅ **"没有 Attack 标签"的旧结论已作废**：`HodgeGameplayTags.h/.cpp` 已随 HEAD `77b7dba` 集中声明 `Status.Attack`（`.Windup` / `.Active` / `.Recovery`）与 `GameplayEvent.Attack`（`.Test` / `.Timeline.End` / `.Interrupted`）——这些是时间轴调度所依赖的标签账本。
>
> ✅ **已通过的关键路径**（同日补测）：**窗口 GE 的施加 / 移除**（`get_gameplay_effect_count` `0 → 1 → 0`，无泄漏）、**Point 事件派发**、**系统事件 `Timeline.End` 派发**（观测手段：事件触发能力 + `ActivationOwnedTags` 同名标签，见 [排障手册](14-troubleshooting.md)）。
>
> ✅ **取消路径的清理也已通过**：`clear_ability(handle)` 打断后 `Status.Attack.*` 与窗口 GE 均归零、时间轴不再推进（此后不再出现后续窗口或 Point）。`Interrupted` 不派发是设计约定（`AbilityCancelled` 不广播；`Interrupted` 的触发者要等第二阶段）。
>
> ⚠️ **仍未验证的是重入类时序**：`EnterWindow` 两道防线、`ExitWindow` 不对称约束、`ClearAllWindowState` 幂等、GE 施加失败补偿，以及 `NetPolicy` 跨端分派、时钟倒退。也就是说**"正常走完"与"被中途打断"都跑通了，但"在回调里重入"这一类最刁钻的时序零运行证据**—— 而那正是这套设计里最费笔墨的地方（§3.11 两道防线、`ExitWindow` 不对称约束、时钟倒退守卫）。V13 因此从"未执行"变为"**主路径 + 取消路径通过、重入类待补**"（见 [验收清单](16-validation.md)），V14（连击）仍不适用。
>
> 🆕 **状态更新（2026-09-22）**：新增**取消窗口标签** `Status.Attack.Cancel` / `.Move`（授权语义，与"处于后摇"的 `Status.Attack.Recovery` 分开配 Window）与 AbilityTask **`UHodgeAbilityTask_WaitMoveCancel`**（窗口 + 移动意图同时成立时广播 `OnMoveCancel` 一次），`UHodgeHeroComponent` 增加**移动意图**信号（`HasMoveIntent` / `GetMoveIntent` / `OnMoveIntentChanged`，输入层原始输入量，不依赖位移）。这是"移动取消后摇"的消费方，把 Timeline 的"授权"与输入层的"意图"分层。⚠️ **但新任务本身零运行证据**（本轮未编译、未 PIE，无法确认已编入 DLL），且**当前没有正式攻击 Ability 消费它**——"取消后摇"端到端仍未接通。详见 [本轮记录](23-update-2026-09-22.md)。
>
> 旧的**双数组版**（`Phases[]` + `Events[]`）以及当时的"已回退"结论见 [21-update-2026-09-17.md](21-update-2026-09-17.md)，仅作历史记录。

## 编辑器资产检查步骤

1. 打开 `/Game/Main/Data/DA_Dafult_PawnData`，确认 PawnClass 是目标 Hero 蓝图、InputConfig、DefaultCameraMode 指向 `Main/Camera/CM_ThirdPerson`（旧 CM_Default 已删除）。AbilitySets 现在会被授予，可以实际填写。
2. 打开 `/Game/Main/Character/Hero/BP_Hero_Pover`，检查父类/继承组件、HeroComponent 的 DefaultInputMappings。选择 `/Game/Main/Input/IMC_Default`，核对优先级和 RegisterWithSettings；当前代码 false 会跳过添加，不能误当成只关闭设置注册。
3. 打开 InputConfig，确认 NativeInputActions 的 Move/Look 等标签匹配；打开 IMC 检查键位、轴类型和修饰器。
4. 打开 `/Game/Main/Character/Hero/GA_Attack`（已随 HEAD `993a9eb` 提交），确认父类与 Montage，并确认是否已改用时间轴 / `WaitMoveCancel`。⚠️ `GA_Melee`、`Main/Character/Hero/Ability/` 目录与 ComboSet 资产**不存在**，不必再核对。时间轴的测试件在实验区 `/Game/CodexText/Timeline/`（`DA_TimelineTest` + `GA_TimelineTest`）；`Main/Character/Hero/DA_TimeLineText` 是**旧字段模型**的废数据（加载后只剩一条空条目，跑校验会报 3 条 Error），需要重配或删除。
5. PIE 观察 HODGE-DBG 的 CONFIG、IMC entry、BLOCKED、CALLBACK 和相机绑定日志。期望实际映射存在、回调有值、相机跟随；本轮未执行这些操作。

## 当前开发顺序

先验收玩家输入/相机主链并确认 AbilitySet 授予；再补时间轴的**重入类用例**（造一个"收到窗口标签/事件就结束 Ability"的测试能力，撞两道防线与 `ExitWindow` 不对称；主路径与取消路径已通过）；接着把 **`UHodgeAbilityTask_WaitMoveCancel` 编进构建并接线验证**（新任务零运行证据，需先 Editor 常规构建让新 `UCLASS` 进反射）；然后做第一个攻击 Ability（C++ 子类 + 命中判定；**连击仍需从零实现**）；随后连接敌人 ASC、命中和死亡；再完善 Cue 路径与预加载；进入联机/热卸载前覆盖 KB-10、KB-12、KB-14。

本页只更新知识，不修改游戏实现。所有未执行的构建、蓝图编译、PIE、联机、Cook/打包保持“未验证”。
