# GAS、AbilitySet 与技能生命周期

> 最近源码核对：2026-09-17。源码接入状态与运行验收分开记录。
[返回首页](README.md) · [伤害系统](08-combat-health.md) · [本轮变更](21-update-2026-09-17.md)

## ASC 的项目职责

UHodgeAbilitySystemComponent 在原生 GAS 上增加 Tag 输入缓存、激活组、关系映射、全局能力注册、失败通知和动态 Tag GE 辅助方法。它并不是某个普攻或闪避技能的实现。

玩家 ASC 在 PlayerState 构造函数创建，开启复制并设置 Mixed。ASC 接入已收敛：HeroComponent 在 PawnExtension 到达 DataInitialized 后调用 PawnExtension::InitializeAbilitySystem，由它 InitAbilityActorInfo、设置 TagRelationshipMapping 并广播 OnAbilitySystemInitialized。Hero 的 PossessedBy/OnRep_PlayerState 已退化为只调用 Super；PlayerState::PreInitializeComponents 的 GetPawn() 占位首绑仍在。剩余是验证换 Pawn 的清理顺序。

ASC 新 Avatar 处理包括通知能力实例、注册 GlobalAbilitySystem、给 HodgeAnimInstance 初始化 Tag 映射，以及尝试 OnSpawn 能力。因此“角色侧 ASC 查询断开”和“ASC 内部完全没初始化”是两种不同问题。

## 激活策略

- OnInputTriggered：按下时尝试激活一次，适合离散操作。
- WhileInputActive：持有时继续尝试激活，适合持续输入驱动的行为；需考虑技能结束后持有键是否重新激活。
- OnSpawn：获得 Avatar 或能力授予等相关时机尝试自动激活，适合被动能力；也需要满足网络执行策略和激活条件。

激活策略和网络 NetExecutionPolicy 不是同一个概念。项目基类构造当前默认 InstancedPerActor 和 LocalPredicted；派生技能仍可覆盖。输入触发本身不决定网络策略，应在具体 Ability 中选择并验收。

## 激活组

Independent 不参与独占互斥；Exclusive_Replaceable 可被其他独占能力替换；Exclusive_Blocking 阻止其他独占能力。ASC 在激活和结束通知中维护计数，并取消可替换组。

把不可取消的能力配置成可替换组会冲突。排查“技能第二次不触发”时，应确认前一次是否调用 EndAbility、是否被正确取消、组计数是否恢复，而不是只检查键位。

## 能力授予与回收

AbilitySet 为每个技能创建 FGameplayAbilitySpec，写入等级、SourceObject 和 InputTag，再调用 GiveAbility。技能实例和 Spec 是不同对象：Spec 保存授予信息和运行句柄；Ability CDO 保存类默认配置；实际 Ability 实例执行技能。

玩家永久技能现已由 PlayerState::SetPawnData 授予：权威端遍历 PawnData->AbilitySets，对每个非空集合调用 `GiveToAbilitySystem(ASC, nullptr)`，随后发送 NAME_HodgeAbilityReady 并 ForceNetUpdate。注意这里**未传 GrantedHandles**，没有任何句柄可用于撤销或防重复；SetPawnData 在已有 PawnData 时提前返回，是当前唯一的防重手段。GameFeature_AddAbilities 也有有效授予实现，可作为扩展入口；实际配置与触发需要资产与运行证据。

装备、临时 Buff 或玩法授予应保留 GrantedHandles，卸载时调用 TakeFromAbilitySystem。不能依赖销毁某个蓝图变量来自动撤销所有 GAS 状态。

## Cost、Tag 与失败通知

HodgeGameplayAbility 扩展 AdditionalCosts、激活组检查和 TagRelationshipMapping 条件，并提供失败文本/动画映射字段。字段存在并不证明 UI 已接入；项目没有完整 CommonUI/消息展示系统。

TagRelationshipMapping 适合集中描述某类技能阻断/取消其他标签、需要或禁止哪些状态。PawnData 字段现已启用，PawnExtension 设置到 ASC 的调用已启用，配置后仍要检查 ASC 实际引用。

## EffectContext 配套缺口

HodgeGameplayAbility::MakeEffectContext 调用父类创建句柄后，尝试提取 FHodgeGameplayEffectContext，并 check 结果。当前新增 HodgeAbilitySystemGlobals::AllocGameplayEffectContext 返回 FHodgeGameplayEffectContext，DefaultGame.ini 也已选择该 Globals，旧类型分配缺口已在代码和配置层补齐。

无需重复新增 Globals；现在应核对运行时实际使用的 Globals 和 Context 类型。验收应检查实际 ScriptStruct 类型，再创建 GE Spec，不能通过删除 check 掩盖数据管线不一致。

当前自定义 Context 的 NetSerialize 复用父类，额外本地字段不会因此自动复制。Iris 路径也转发父类序列化；将来加入必须联网的自定义字段时，需要同步设计序列化和验证。

## 攻击时间轴与 ComboSet（已实现，尚无 C++ 调用点）

`UHodgeAbilityTimeline`（UPrimaryDataAsset）描述阶段区间（`FHodgeTimelinePhase`）、时间点事件（`FHodgeTimelineEvent`，带 `EHodgeTimelineEventNetPolicy`）和 Montage 软引用，提供 `GetActivePhases` 与编辑器校验/Bundle 收集。`UHodgeComboSet`（UPrimaryDataAsset）持有 `FHodgeAttackNode` 数组和入口表，`FindNode` 按 AttackID 查找。

`UHodgeAbilityTask_PlayTimeline` 是驱动 Task：`bTickingTask = true`，用世界时间差累加逻辑时间；进入/退出阶段时对 ASC 加/减 **non-replicated loose tag**；到时间点用 `HandleGameplayEvent` 派发 `GameplayEvent.Attack.*`；`ElapsedTime >= Duration` 时 `StopTimeline(NaturalEnd)` → `EndTask()`。`OnDestroy` 无条件 `ClearAllPhaseTags()`，广播前有 `ShouldBroadcastAbilityTaskDelegates()` 守卫；事件按 `NetPolicy` 决定在哪些端执行，不使用 PredictionKey（`FGameplayEventData` 本身没有该字段）。

配套的 `HodgeGameplayTags` 分组：`Attack.Entry.*` / `Attack.Transition.*`（数据层入口与转移键）、`GameplayEvent.Attack.*`（ComboWindow Open/Close、HitCheck、JumpSection、Timeline.End、Interrupted、Phase Enter/Exit）、`Status.Attack.*`（阶段 loose tag，含 ComboWindow/Invincible/SuperArmor）、`Status.AttackMode.*`（服务器权威 GE 授予并复制）。

**当前完成度**：类、校验、Bundle 收集与 `UHodgeAssetManager::PreloadPrimaryAssetBundles` 均有有效实现，但源码内**没有任何 Ability 调用 `PlayTimeline`，也没有代码调用 `ComboSet::FindNode`**——即“有实现、无 C++ 调用点”，BP 侧是否接线未验证。设计依据见 [AbilityTimeline 设计方案](../Design/ability-timeline.md)。

## Cue 与全局能力

HodgeGameplayCueManager 实现了资源管理相关逻辑，DefaultGame.ini 已通过 GlobalGameplayCueManagerClass 选择它；但 AssetManager 初始化钩子仍占位，常驻 GameplayCueNotifyPaths 仍注释，Policy 未注册 Cue 路径观察者。不能宣称预加载和 Feature Cue 路径全部接通。

GlobalAbilitySystem 保存世界级授予，并对注册 ASC 应用。调用这些公共方法时仍需检查权威边界，不能把 WorldSubsystem 自动等同于仅服务器执行。

源码：[Ability](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp)、[ASC](../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp)、[Context](../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayEffectContext.cpp)、[AbilitySet](../../Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp)。
