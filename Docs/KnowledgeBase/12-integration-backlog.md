# 当前状态、断点与接通顺序

[返回首页](README.md) · [上一轮变更](24-update-2026-09-28.md) · [本轮变更](25-update-2026-09-29.md)

> 核对日期：2026-09-29。对象：HEAD `abb9224`，工作区有 2 个 `CodexText` 资产改动。已执行 Editor 构建（UBT 报 `Target is up to date`）；**未执行 PIE、蓝图 Compile、联机、打包**。保留 KB 编号便于追踪，但"源码已接入"与"运行验收通过"严格区分。

## 旧缺口现已在源码接入

### KB-01：Pawn 生成与数据注入——源码已接入

GameMode 使用延迟构造，在 FinishSpawning 前查找 PawnExtension 并调用 SetPawnData（未注释）；另一条路径是 PlayerState::OnExperienceLoaded → GameMode::GetPawnDataForController → SetPawnData。默认 PawnClass 回退仍为 CharacterBase，BP_Hero_Pover 的实际 PawnClass 待编辑器确认。

### KB-02：Hero 协调组件——ASC 接入已收敛到单一入口

HeroComponent 在 PawnExtension 到达 DataInitialized 后调用 PawnExtension::InitializeAbilitySystem；HeroCharacter 的 PossessedBy / OnRep_PlayerState 已退化为只调用 Super。PlayerState::PreInitializeComponents 仍用 GetPawn() 做占位首绑。剩余工作是换 Pawn / 重生时的清理顺序运行验证。

### KB-03：输入绑定与消费——主链已接入，条件仍待验证

HodgeInputComponent 与 HodgeLocalPlayerBase 已配置；Hero 绑定原生和技能输入；HodgePlayerController::PostProcessInput 调用 ProcessAbilityInput。额外输入句柄已持久化到 AdditionalInputConfigHandles，`RemoveAdditionalInputConfig` 已实现，EndPlay 统一解绑。`Input_Move` 记录"移动意图"，`Input_MoveStopped` 绑到 `Completed`/`Canceled` 清零，供"移动取消后摇"消费（`HasMoveIntent` / `GetMoveIntent` / `OnMoveIntentChanged`）。仍需关注：DefaultInputMappings 与软引用加载、AddMappingContext 被 bRegisterWithSettings 条件包住、ClearAllMappings 可能清除其他来源映射、Ready 事件不保证绑定成功、AutoRun 切换调用仍注释。⚠️ **Crouch 的输入绑定本轮被临时屏蔽**（见 KB-20）。

### KB-04：相机入口——源码已接入，资产值待验证

Hero 绑定 DetermineCameraModeDelegate；模式选择优先技能临时覆盖，否则 PawnData.DefaultCameraMode。控制器已选 HodgePlayerCameraManager。模式资产为 `CM_ThirdPerson` / `CM_ThirdPerson_Death`；`Content/Characters/Cameras/` 另有 `ThirdPersonOffsetCurve` / `ThirdPersonDeathOffsetCurve` 两条曲线资产（C++ 默认 `TargetOffsetCurve` 已置空，改由资产 / 蓝图 RuntimeFloatCurves 提供）。PawnData 实际指向待编辑器确认。

### KB-05：基础 AbilitySet 授予——已在源码接入

PlayerState::SetPawnData 现于权威端遍历 PawnData->AbilitySets，对每个非空集合调用 `GiveToAbilitySystem(ASC, nullptr)`，随后发送 NAME_HodgeAbilityReady 并 ForceNetUpdate。注意**未记录 GrantedHandles**：没有可撤销/防重的句柄，防重仅靠"已有 PawnData 时提前返回"。

### KB-06：自定义 EffectContext——代码与配置已接入

HodgeAbilitySystemGlobals::AllocGameplayEffectContext 返回 Hodge Context；DefaultGame.ini 已选择该类。运行时实际类型仍需核对。

### KB-12：Receiver 与 ASC 退出顺序——Receiver 已修正，退出顺序待验证

CharacterBase 在 PreInitializeComponents 注册 Receiver、BeginPlay 发送 GameActorReady、EndPlay 成对调用 RemoveGameFrameworkComponentReceiver，生命周期已对称。新控制器 OnUnPossess 先清空 ASC Avatar，可能使 PawnExtension 依赖 Avatar 匹配的清理分支跳过；仍需按真实生命周期验证。

### KB-13：LocalPlayer 与 Feature Policy——配置已补齐

LocalPlayerClassName 和 GameFeaturesManagerClassName 已指向项目类型。Policy 的具体观察者注册仍是另一个环节，见 KB-07。

### KB-16：攻击 Ability 与连招——已换代为 Definition 链路，旧 BasicAttack 成孤儿

**当前主链路**（2026-09-29）：`UHodgeAbilityDefinition`（单段技能）+ `UHodgeComboDefinition`（DataTable 跳转表）+ `UHodgeComboComponent`（**已挂到 `AHodgePlayerState`**）+ `UHodgeGameplayAbility_Definition`。五段连击资产在 `/Game/CodexText/DefinitionCombo/`，`DA_Dafult_PawnData` 已配 `ComboDefinition`。详见 **KB-22** 与 [本轮记录](25-update-2026-09-29.md)。

**旧链路**：`UHodgeGameplayAbility_BasicAttack`（`TArray<FHodgeBasicAttackStep> AttackSteps`，每段 = Montage + Timeline；输入缓冲 `WaitInputPress`；窗口接段监听 `Status.Attack.Cancel.NextAttack` → `TryAdvance`；移动取消 `WaitMoveCancel`；后摇取消 `OnInterrupted → EndAbility`）。它有过 PIE 验证（`Docs/Validation/basic-attack-2026-09-24.md`，**不含命中与伤害**），但**现在 `DA_Pover` 已移除该授予、C++ 零引用** → 属待清理的孤儿，两套连招模型并存容易混淆。

### KB-17 🆕：生命与死亡——已接通

`UHodgeHealthComponent`（`UGameFrameworkComponent`）已由 `AHodgeCombatCharacter` 构造创建并绑定 `OnDeathStarted` / `OnDeathFinished`；监听 `HealthSet` 的 `OnHealthChanged` / `OnMaxHealthChanged` / `OnOutOfHealth`；`DeathState` 状态机（NotDead → DeathStarted → DeathFinished，ReplicatedUsing）→ 禁用移动与碰撞 → 下一帧销毁 → `UninitAndDestroy`。`HandleOutOfHealth` 派发 `GameplayEvent.Death`。

⚠️ 剩余：`HandleOutOfHealth` 里 Elimination / Verb Message 广播整段注释（淘汰 / 击杀提示链未接）。

### KB-18 🆕：UI 源码迁移——已落地编译，尚未出画面

`UI/` 81 个文件（41 `.h` + 40 `.cpp`）已落地并通过编译；`CommonUI` 插件启用；`Build.cs` 新增 `CommonUI` / `CommonInput` / `UMG` / `Slate` / `SlateCore`；`AHodgeHUDBase` → `AHodgeHUD`；`GameFeatureAction_AddWidget` 复活。

⚠️ **24 个文件"待复活"**（逐行注释 + `[UI-MIGRATION-PENDING]` 标记）：需 CommonGame / GameSettings / CommonUser。`_AddWidget` 的 `PushContentToLayer_ForPlayer` 仍注释 → Layout 不会创建。详见 [本轮记录](24-update-2026-09-28.md)。

## 当前仍应优先处理

### KB-08：伤害数值管线——Execution 已建，但倍率恒 0

`UHodgeCombatSet`（`BaseDamage` / `BaseHeal`）、`HodgeDamageExecution` / `HodgeHealExecution` 均已实现；`UHodgeGameData` 新增 `DamageGameplayEffect_SetByCaller` / `HealGameplayEffect_SetByCaller` / `DynamicTagGameplayEffect` 字段并在 `DA_Dafult_GameData` 里配置。HealthSet 的 Damage/Healing 元属性结算闭环已通（见 KB-17 的死亡衔接）。

🔴 **但 `HodgeDamageExecution` 里 TeamSubsystem 判敌我的整段被注释，`DamageInteractionAllowedMultiplier` 恒 `0.0f`** → 攻击能打出去、GE 也会执行，但**减血结果恒为 0**。这是当前"战斗闭环"最大的实际缺口，优先于其它工作。

补充缺口：无防御属性、无暴击、无 `DamageType.*` 分支（目前只有 `BaseDamage × 距离衰减 × 物理材质衰减`）。

### KB-07：Cue——管理器配置已有，路径注册与预加载不完整

GlobalGameplayCueManagerClass 已配置；Policy 观察者的添加路径回调主体已启用，但 InitGameFeatureManager 创建 AddGameplayCuePaths 观察者的一行仍注释。注销移除主体也停用；AssetManager 初始化钩子仍占位，常驻 GameplayCueNotifyPaths 未启用。

### KB-09：敌人 ASC——仍缺完整初始化

EnemyCharacter 现在只设置 AI 自动控制和 `AutoPossessAI`；自身旋转/移动参数已注释、继承 Combat 基类，没有完整的敌人 ASC 创建与关联。先明确 ASC 所有者，再验证服务器伤害目标和远端状态。**在 KB-08 修好前，"打敌人"无法端到端验证。**

### KB-10：专服启动——仍有提前返回风险

TryDedicatedServerLogin 在默认地图条件满足时返回 true，实际登录和续接回调仍停用；还没有独立 Server Target。

### KB-14：GameFeature 输入——撤销闭环已补，运行验证待做

AddInputBinding、AddInputContextMapping 的扩展添加分支均已启用。Hero::RemoveAdditionalInputConfig 已实现，EndPlay 统一解绑。ContextMapping 的 ControllersAddedTo 记录路径仍需复核。验收必须覆盖激活、停用、再次激活和多世界。

### KB-15：时间轴与 ComboSet——时间轴已验证，**ComboSet 缺口已由 ComboDefinition 补上** ✅

`UHodgeAbilityTimeline` + `UHodgeAbilityTask_PlayTimeline` + **`UHodgeTimelineEvaluator`**（纯静态求值器，运行时与编辑器共用，配套 `Hodge.Timeline.SharedEvaluator` 测试）已实现并**编译 + PIE 实测通过**（窗口 GE 施加/移除 `0→1→0`、Point / `Timeline.End` 派发、取消清理幂等，见 `Docs/Validation/timeline-2026-09-24.md`）；另有 `HodgeAbilityTimelineTests.cpp` 与新增的 `HodgeTimelineEvaluatorTests.cpp`。`Status.Attack.*` / `GameplayEvent.Attack.*` 原生标签已集中声明。

**2026-09-29 更新**：连招数据不再是缺口 —— `UHodgeComboDefinition`（DataTable 跳转表）已落地并挂载验证，见 **KB-22**；旧的 `UHodgeComboSet` 命名不再使用。

**仍然存在**：`UHodgeAssetManager::PreloadPrimaryAssetBundles`、`HodgeGameplayAbility::PreloadPrimaryAssetsOnGrant`（Bundle 预加载）、旧设计的 `Attack.Entry.*` / `Attack.Transition.*` / `Status.AttackMode.*`。

⚠️ 未验证的是**重入类时序**：`EnterWindow` 两道防线、`ExitWindow` 不对称约束、GE 施加失败补偿，以及 `NetPolicy` 跨端分派、时钟倒退。

### KB-19 🆕：UI 出画面——依赖插件未引入

缺 `PrimaryGameLayout` / `GameUIPolicy` / `UGameUIManagerSubsystem` / `UCommonLocalPlayer`（都来自 **CommonGame**）。引入后按 [迁移计划](../Design/lyra-ui-migration-plan.md) 复活 24 个文件，并解注释 `_AddWidget` 的 `PushContentToLayer_ForPlayer`。另外 ini 里没有 CommonUI 按键映射（如 `UI.Action.Escape`），也未配 `GameViewportClientClassName`。

### KB-20 🆕：下蹲被临时屏蔽

`HodgeHeroComponent` 里 Crouch 的 `BindNativeAction` 被注释（`Input_Crouch` 函数体、`ToggleCrouch`、`bCanCrouch=true` 均保留）；`HodgeLocomotionLab.cpp` 另有显式 `bCanCrouch=false`。动画 / UI 就绪后接回。

### KB-21 🆕：`GetBlendInfo` 返回的不是栈顶层（待统一修缺陷）

> 记录日期 2026-09-29，对象为本页 KB 条目之外的**既有缺陷**。核对此条时的实际状态：HEAD `dc3fb9f`，工作区**有未提交修改**（本条目只读核对相机源码，未改动任何源码）。

`UHodgeCameraModeStack::GetBlendInfo` 的形参名（`OutWeightOfTopLayer` / `OutTagOfTopLayer`）与函数注释都声明返回**栈顶**层，但实现取的是 `CameraModeStack.Last()`，即**栈底**：

- `PushCameraMode` 以 `Insert(CameraMode, 0)` 约定 `CameraModeStack[0]` 为栈顶（`HodgeCameraMode.cpp:453`），每次压栈后还会强制 `CameraModeStack.Last()->SetBlendWeight(1.0f)`（`:456`），所以栈底恒为"权重 100% 的基础模式"。
- `GetBlendInfo` 取 `Last()` 后返回该基础模式的 `CameraTypeTag` 与 `BlendWeight`（`HodgeCameraMode.cpp:630-656`），注释自己写的是"获取栈底的 CameraMode"。因此返回的标签是基础模式（默认第三人称）的标签、权重恒为 `1.0`；`CM_ThirdPerson_Death`、技能临时覆盖模式等**上层模式在当前主导时不会被反映**。

**当前影响：低（无消费方）**。`UHodgeCameraComponent::GetBlendInfo` 声明处没有 `UFUNCTION`（`HodgeCameraComponent.h:57`），C++ 侧除定义外无调用者，蓝图也调不到，所以现在是**潜伏缺陷**，不影响现有画面；但它一旦被动画 / UI 用来判断"当前镜头类型"，就会拿到过期层（恒为基础模式）。

**来源**：与上游 Lyra 同名接口的写法一致（按讨论结论记录；本次**未**核对本机 Lyra 源码）。因此本条按"继承自上游的既有问题"处理，不计入本项目引入缺陷。

**最小修复方向**：改为取 `CameraModeStack[0]` 以与压栈 / 更新（`UpdateStack` 从栈顶向下遍历）的约定一致；若真正想要的是"当前主导视图"，需要按各层权重从栈底向上求出实际主导层，只换下标并不等价。

**验收方法**：先确认没有蓝图 / 动画依赖旧行为（`GetBlendInfo` 当前不是 `UFUNCTION`，改动不会破坏蓝图调用），再在 PIE 中打印镜头切换前后的返回值，确认压入死亡 / 技能模式后 tag 随之变化。

### KB-22 🆕：技能 Definition 与连击系统——已挂载并验证（当前主链路）

`UHodgeAbilityDefinition`（单段技能数据资产）+ `UHodgeComboDefinition`（连招跳转表，终于是 `ComboSet` 的正解）+ `UHodgeComboComponent`（**已由 `AHodgePlayerState` 构造创建**，PawnExtension 初始化时 Configure）+ `UHodgeGameplayAbility_Definition`。

- **输入缓存**：单槽、0.3 秒、新输入覆盖旧输入
- **切段**：先做预检查，失败保留当前动作；实际切换失败回 `Entry`
- **标签账本**：拥有端 / 权威端各维护本地账本，模拟代理收 `skip-owner` 快照（不重复计数）
- **事件链上限 32**：超过报错并清理回 `Entry`（防零时间循环）
- **网络**：输入切段走 GAS `LocalPredicted` 激活 RPC，**服务器按当前执行窗口重新选边**，不接受客户端指定目标、不做历史窗口补偿
- **时间轴时长**：支持 `Use Montage Duration`（从 Montage 派生），窗口用"源动画秒数"

资产：`/Game/CodexText/DefinitionCombo/`（`GA_Attack_1~5` + `DA_Attack_1~5` + 对应 Timeline + `DT_LightCombo` + `DA_LightCombo` + `AS_LightCombo`）；`DA_Dafult_PawnData` 已存 `ComboDefinition = DA_LightCombo`。

验证（[`Docs/Validation/definition-combo-2026-09-28.md`](../Validation/definition-combo-2026-09-28.md)）：Editor + Game 两目标构建退出码 0；单人 PIE 14 项、Listen Server（拥有端 23 / 主机 19）、网络模拟 15 项、原生自动化 7/7。

🔴 **缺口**：不含命中判定与伤害，且 `HodgeDamageExecution` 倍率恒 0（见 KB-08）→ 连招能跑完但**打不掉血**。另有：组合触发 B、Section 跳转 / 循环、图形编辑器未包含。

### KB-23 🆕：独立编辑器模块 `HodgeAbilityEditor`——已可用

`Source/HodgeAbilityEditor/`（9 文件，`Type: Editor`），已登记进 `HodgepodgeEditor.Target.cs`。内容浏览器双击 `UHodgeAbilityDefinition` 即打开（注册了 `FHodgeAbilityDefinitionActions`），六个停靠页：Definition / Preview / Timeline / Ability Tasks / Selected Event / Validation；`SHodgeAbilityPreview`（AdvancedPreviewScene + Montage）、`SHodgeAbilityTimeline`（`SLeafWidget` 自绘轨道，编辑走 `FScopedTransaction`）；`Hodge.Editor.DefinitionWorkflow` 测试。

**边界**：只支持单 Section 正向技能；不模拟 GAS 激活、命中、网络与位移。编辑器与运行时共用 `UHodgeTimelineEvaluator`，所以"编辑器看到的窗口"＝"运行时走进的窗口"。

### KB-24 🆕：装备系统四件套——编译通过但零挂载

`UHodgeEquipmentDefinition` / `UHodgeEquipmentInstance` / `UHodgeEquipmentManagerComponent` / `UHodgeWeaponInstance`（Lyra 移植）。

证据：无 `CreateDefaultSubobject<UHodgeEquipmentManagerComponent>`；非 `BlueprintSpawnableComponent`（蓝图搜不到）；`Docs/Design/equipment-weapon-system.md` 明写"均未挂载、未建资产、未接线"。

启用最小路径：① 定 ManagerComponent 挂在哪（建议 `AHodgeCombatCharacter`，Init State 初始化时把 ASC 交给它）→ ② 建 Definition 资产（`ActorsToSpawn` 填武器 Mesh + 手部 Socket）→ ③ `EquipItem` 验证挂上 / AbilitySet 授予 / `UnequipItem` 后 GE 与设备效果归零。

⚠️ 注意：ManagerComponent 的 AbilitySet 授予**保存了 `GrantedHandles` 并支持精确撤销**，比 KB-05 的 PlayerState 授予更完整 —— 想补 KB-05 的句柄可以直接参照它。

## 编辑器资产检查步骤

1. 打开 `/Game/Main/Data/DA_Dafult_PawnData`，确认 PawnClass 是目标 Hero 蓝图、InputConfig、DefaultCameraMode 指向 `Main/Camera/CM_ThirdPerson`。AbilitySets 会被授予，可以实际填写。
2. 打开 `/Game/Main/Character/Hero/BP_Hero_Pover`，检查父类 / 继承组件、HeroComponent 的 DefaultInputMappings（选 `IMC_Default`，注意 `bRegisterWithSettings=false` 会跳过添加）。
3. 打开 `DA_HodgeInputConfig`，确认 NativeInputActions / AbilityInputActions 的 Tag 与 `IA_*` 匹配；打开 IMC 检查键位与修饰器。
4. 打开 `/Game/CodexText/DefinitionCombo/` 下的 `DA_Attack_1~5`，确认每个的 `AbilityClass` 是 `UHodgeGameplayAbility_Definition` 子类、**Montage 与 Timeline 时长一致**（不一致会拒绝激活）；打开 `DT_LightCombo` 核对六行跳转表的 `RequiredWindowTags` / `RequiredSourceTags` / `BlockedSourceTags` / `TransitionPriority`。
5. 打开 `/Game/Main/Data/DA_Dafult_GameData`，确认 Damage / Heal / DynamicTag 三个 GE 引用是否已填。
6. PIE 观察 `HODGE-DBG` 的 CONFIG、IMC entry、BLOCKED、CALLBACK 与相机绑定日志。⚠️ 本轮未执行 PIE（连招的运行时结论看 `Docs/Validation/definition-combo-2026-09-28.md`）。

## 当前开发顺序

1. **修 `HodgeDamageExecution` 的敌我倍率**（当前恒 `0.0f`，是"连招能打完但不掉血"的根因）。
2. 编辑器确认 `DA_Attack_1~5` 的 Montage ↔ Timeline 配对与命中判定 → 打通"打中掉血"。
3. 引入 **CommonGame** → 复活 24 个 UI 文件 → 解注释 `PushContentToLayer_ForPlayer` → UI 出画面（可与 1~2 并行）。
4. AbilitySet 授予补 `GrantedHandles`（可直接参照 KB-24 里 ManagerComponent 的写法）。
5. 挂载 `UHodgeEquipmentManagerComponent` → 第一个装备 Definition → 武器挂手（装备 UI 与 Cosmetics 动画层仍是空壳）。
6. 敌人 ASC 初始化 → 验证"打敌人"完整闭环。
7. 清理孤儿 `UHodgeGameplayAbility_BasicAttack`（KB-16）；补时间轴**重入类用例**；完善 Cue 路径与预加载；进入联机 / 热卸载前覆盖 KB-10、KB-12、KB-14。

本页只更新知识，不修改游戏实现。未执行的构建、蓝图编译、PIE、联机、Cook/打包保持"未验证"。
