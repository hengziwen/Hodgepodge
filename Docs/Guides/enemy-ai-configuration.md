# 普通怪物 AI v0.1 配置与开发手册

本实现对应 [敌人 AI 设计](../Design/enemy-ai-system.md)。实现和验证结果见 [实施报告](../Validation/enemy-ai-implementation-2026-10-11.md)。本手册的资产均为新增正式 Guardian 或独立验证资源，不要求修改旧 Hero 攻击资产。

## 1. 可运行资源

正式资源根目录为 `/Game/Main/Character/Enemies/Guardian`：

- `BP_Enemy_Guardian`：徒手近战怪，基础 AbilitySet 提供攻击。
- `BP_Enemy_GuardianEquipped`：装备近战怪，基础包提供受击／死亡，武器包提供攻击。
- `AIC_Guardian`：`AHodgeEnemyAIController` 子类，明确设置 BehaviorTree。
- `AI/BT_Guardian`、`AI/BB_Guardian`：最小追击、攻击、等待行为树及黑板。
- `DA_Guardian_PawnData`、`DA_Guardian_EquippedPawnData`：各自引用属性、基础能力及受击配置；装备版另设 DefaultWeaponDefinition。
- `DA_Guardian_Stats`：350 最大生命、15 基础伤害。
- `DA_Guardian_Attack`：Standalone 路由的近战 Definition，使用 `Ability.Type.Action.Melee`。
- `DA_Guardian_HitReaction`、`GA_Guardian_Death`：敌人自身的受击、恢复与死亡资源。
- `Anim/ABP_Guardian`、`Model/SKM_Guardian`、`Model/SK_Guardian`：独立复制的骨架、网格和动画，不修改导入素材。

测试地图为 `/Game/Main/Maps/L_EnemyAIValidation`，包含地面、两个 PlayerStart、导航边界、徒手怪和装备怪。使用既有 ThirdPerson GameMode 与 Default Experience。

## 2. 放置与验证步骤

1. 打开测试地图，确认 World Settings 的 GameMode 为 `/Game/ThirdPerson/Blueprints/BP_ThirdPersonGameMode`。
2. 两种敌人蓝图的 Class Defaults：EnemyPawnData 指向各自 PawnData，AI Controller Class 指向 AIC_Guardian，Auto Possess AI 为 Placed in World or Spawned。
3. Default Experience 的 AddComponents 保留原有条目，并增加 EnemyCharacter 的 EquipmentManager（服务器、客户端均启用）。Combat 使用既有通用注入条目。
4. 单人 PIE：怪物就绪后发现有效玩家 Hero，导航追击到攻击范围，GAS 播放自身攻击蒙太奇并执行命中。
5. 两人 Play As Listen Server：远端观察怪物移动、攻击、受击及死亡；客户端不运行怪物决策、不授予基础能力。
6. 调试时查询 EnemyInit.GetInitializationResult／GetInitializationDiagnostic。Ready 只表示初始化完成；CanExecuteAIAction 才包含当前死亡、控制等限制。

## 3. 初始化模块与接口

`UHodgeEnemyInitComponent` 是 EnemyCharacter 的原生默认组件；无需在蓝图重复添加。TryInitialize 是唯一协调入口，可重复调用。它依次推进自己的 Spawned、DataAvailable、DataInitialized、GameplayReady。

进入服务器 DataAvailable 需要 Experience Loaded、敌人 PawnData 和 Controller。随后等待 PawnExtension 的 DataInitialized，再绑定本怪物 ASC，初始化属性、授予基础能力并准备默认装备。初始化不在 CanChangeInitState 内执行副作用。

公开接口：

```cpp
EHodgeEnemyInitializationResult TryInitialize();
bool IsGameplayReady() const;
bool HasValidCombatDependencies() const;
bool CanExecuteAIAction() const;
FString GetInitializationDiagnostic() const;
EHodgeEnemyInitializationResult GetInitializationResult() const;
```

结果枚举为 Waiting、Ready、Failed、Invalidated。Waiting 由依赖事件推进，不使用固定频率重试 Timer。Failed 为不可自动恢复的配置／执行失败；Invalidated 为解绑、组件移除或本生命周期失效。

必须配置 StatProfile。PawnExtension 绑定其他 ASC、PawnData 冲突、非法属性值、空／非法 AbilitySet 都不能进入 Ready。客户端等待复制数据与本地 ASC 绑定，不验证服务器完整 AbilitySpec。

属性、基础能力和装备各有已提交标记，重复回调不重新应用属性、不恢复损失生命、不重复授予。失败或失效清理本入口拥有的基础句柄及默认装备；初始化 Instant GE 的效果不能被宣称可以无损撤销。对象池重置和 Experience 热切换不在首版范围。

## 4. 受检授予与装备协议

AbilitySet 保留旧 GiveToAbilitySystem 兼容入口，新协调路径使用 ValidateBatchForGrant 和 TryGiveToAbilitySystem，实际授予循环共享。

```cpp
struct FHodgeAbilityGrantResult
{
    bool bSucceeded;
    bool bHasIrreversibleEffects;
    FString Error;
};
```

授予前校验类、资源、等级、Definition、重复身份和已有 AttributeSet 冲突。首版初始化包禁止 OnSpawn 自动激活。授予中发生解绑／销毁则撤销当前可安全清理的句柄；存在 Instant GE 等不可逆副作用时报告并停止，不整包重试。

装备 AddEntry 在候选阶段完成属性效果、受检能力授予及装备 Actor 生成。全部成功后才提交 Entry。空 AbilitySet、授予失败、生成／挂接失败均返回空装备实例，并清理候选能力、效果和 Actor。GetLastEquipError 与 DidEquipHaveIrreversibleEffects 提供诊断。

PrepareDefaultEquipment／IsDefaultEquipmentReady 由 CombatCharacter 提供。徒手怪无 DefaultWeaponDefinition 时不等待武器；装备怪同时验证装备实例及其授予依赖。玩家保留 PlayerState 属性协调链路，敌人 ASC Owner／Avatar 均为自身 Pawn。

## 5. 控制器、感知和黑板

AHodgeEnemyAIController 不创建 PlayerState。原生 EnemyCharacter 默认绑定该类；具体 Blueprint 子类必须配置 BehaviorTree，Guardian 已显式完成配置。

每次 Possess 先失效旧周期、解绑旧通知，再订阅并立即查询新 EnemyInit。只有服务器、初始化完成、依赖有效、生命为正且未死亡时尝试启动；RunBehaviorTree 成功才计入 Running。每个 Possess 周期成功启动一次。OnUnPossess 停止树、移动、Focus，清理旧委托并增加周期身份。

默认参数：AttackRange 180、AttackExitRange 220、SightRadius 1400、TargetMemorySeconds 3。退出范围大于进入范围，减少边界反复切换。决策每 0.2 秒更新，感知事件也可立即更新。这是决策频率，不是初始化轮询。

黑板数据：

- TargetActor：当前可见有效玩家 Hero，Object。
- TargetActor 的 Object Base Class 必须为 Actor（或其子类）；攻击任务的 TargetActor 选择该键。不要选 SelfActor。若保留默认 UObject，编辑器的 Actor 类型过滤会隐藏此键并可能回退到 SelfActor，导致任务在提交 GAS 请求前失败。
- LastKnownLocation：最后可见位置，Vector。
- CanAct：当前能否接受动作请求，Bool。
- HasLineOfSight／HasLastKnownLocation：追击或搜索分支条件，Bool。
- InAttackRange：带迟滞的攻击范围结果，Bool。

保留当前可见目标；无有效当前目标时选择最近可见 Hero，并通过现有 DamageRules 过滤。失去视线后只走最后已知位置，不追踪隐藏目标的实时坐标。记忆过期清除目标。完整阵营、仇恨和目标优先级以后扩展。

受击 Controlled 标签使 CanAct 为假，Decorator Abort 当前分支并停止移动，整棵树仍运行。死亡或生命为零停止正常 AI，死亡 GA 自己完成表现与销毁。必要依赖失效时停止决策，首版不承诺自动恢复被撤销的 GameFeature。

## 6. 共享战斗请求任务

UHodgeBTTask_ExecuteAbility 配置 RequestType、AbilityTag／IntentTag、TargetActor 黑板键、Timeout（默认 8 秒）。默认 StandaloneAbility 保持已有 Guardian 资产兼容；ComboIntent 模式填写语义意图，PawnData 必须有合法 ComboDefinition，并授予图中引用的 ComboCoordinated Definition。

Task 验证 AI 就绪／目标并准备朝向，再绑定 Combat.OnActionRequestChanged，调用 RequestStandaloneAbility 或 SubmitCombatIntent。Queued／Started 等待，Completed 成功，Cancelled／Expired／Rejected 失败。同步结果以返回快照补偿。

Task 保存 RequestId、AttemptId、PossessGeneration、Pawn／Controller／ASC／Combat 和朝向句柄；共享组件管理 Spec／Ability／ExecutionId。Abort 先解绑通知，再取消自己的请求并释放朝向，不取消其他请求或 Death。

Task.InitializeFromAsset 解析并校验黑板目标键；缺少黑板、非 Actor 类型或 SelfActor 不能执行，初始化时输出诊断。新任务默认选择 TargetActor；显式配置的其他合法 Actor／Pawn 目标键保留。

2026-10-11 的目标键回归修复见 [报告](../Validation/enemy-attack-target-key-2026-10-11.md)。现有生成资产可使用 Tools/CombatActionRequests/repair_target_keys.py 修复目标类型和自动回退的选择；该脚本不重建行为树、武器或角色资产，也不覆盖其他合法目标键。

共享接口、取消权限和网络语义见 [共享战斗请求设计](../Design/combat-action-requests.md)。本轮验证资源在 `/Game/Main/Character/Enemies/Guardian/Validation`：BP_EnemyCombo、AIC_EnemyCombo、BT_EnemyCombo、DA_EnemyComboPawnData。该组合无玩家 InputBindings，直接提交 InputIntent.Attack.Light；正式 Guardian 配置保留原独立技能路由。

## 7. 动画与资源约束

ABP_Guardian 以 HodgeEnemyAnimInstance 的 bEnemyMoving 切换 Idle／Run，使用 0.15 秒混合；首版没有完整怪物运动匹配或 Pivot 系统。

FullBobySlot 与 UpperBobySlot 沿用已有拼写，分属 HodgeEnemyFullBody／HodgeEnemyLightFeedback 两个 Slot Group。轻反馈经过 spine 分支骨骼混合；强受击与死亡使用全身蒙太奇。动画复制后关闭 Root Motion 并锁定根骨；角色移动由 CharacterMovement／导航承担。

AM_Attack 的 0.35 秒处为一次 AvatarRoot Box 命中：半尺寸 (95,80,80)、前向偏移 (85,0,0)。默认判定 Normal，HitStun 0.35 秒。首版参数用于普通近战闭环，后续应结合武器和动画逐帧调校。

装备怪的攻击授予来自武器包，徒手怪来自基础包。两者共用该最小攻击 Definition，但授予归属不同。

DeathMontage 由 HodgeGameplayAbility_EnemyDeath 播放并等待完成；未配置动作时安全结束。Guardian Mesh 配置 AlwaysTickPoseAndRefreshBones，保证非可见时通知／蒙太奇能推进；真正 Dedicated Server 的骨骼和命中策略仍须单独验证。

## 8. 复现工具

Tools/EnemyAI 下的 configure_guardian.py、configure_validation_grant.py、configure_validation_map.py 用于受限资产作者配置，必须在非 PIE 状态运行。它们分别修改 Guardian 自有资源、独立玩家攻击测试包、独立验证地图；configure_guardian 还补充 Default Experience 的敌人装备组件条目。

start_validation.py 启动 PIE；run_e2e.py 注册随帧检查，不应将“Started”当作通过。查看 Saved/EnemyAIImplementation 的最终 JSON，error 必须为空且每个 case passed 为 true。stop_validation.py 结束 PIE，restore_validation.py 恢复本轮改动的后台节流设置。

示例：

```powershell
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/start_validation.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/run_e2e.py
# 等待报告实际完成后结束并恢复。
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/stop_validation.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/restore_validation.py
```

Listen Server 的包装脚本先设置 ENEMY_PLAYERS=2 再执行 start_validation。不要在生产地图运行自动测试；测试会调整测试 Pawn 位置并触发死亡。
