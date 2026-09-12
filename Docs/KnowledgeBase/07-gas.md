# GAS、AbilitySet 与技能生命周期

[返回首页](README.md) · [伤害系统](08-combat-health.md)

## ASC 的项目职责

UHodgeAbilitySystemComponent 在原生 GAS 上增加 Tag 输入缓存、激活组、关系映射、全局能力注册、失败通知和动态 Tag GE 辅助方法。它并不是某个普攻或闪避技能的实现。

玩家 ASC 在 PlayerState 构造函数创建，开启复制并设置 Mixed。Hero 当前通过 PossessedBy 和 OnRep_PlayerState 给它设置 Owner/Avatar。这条直接路径能够触发 ASC 内部的新 Avatar 处理，却没有保存 PawnExtension 的 ASC 指针。

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

玩家永久技能计划由 PlayerState 的 PawnData 配置提供，但当前授予循环仍注释。GameFeature_AddAbilities 有有效授予实现，可作为扩展入口；是否存在实际配置并触发，需要资产与运行证据。

装备、临时 Buff 或玩法授予应保留 GrantedHandles，卸载时调用 TakeFromAbilitySystem。不能依赖销毁某个蓝图变量来自动撤销所有 GAS 状态。

## Cost、Tag 与失败通知

HodgeGameplayAbility 扩展 AdditionalCosts、激活组检查和 TagRelationshipMapping 条件，并提供失败文本/动画映射字段。字段存在并不证明 UI 已接入；项目没有完整 CommonUI/消息展示系统。

TagRelationshipMapping 适合集中描述某类技能阻断/取消其他标签、需要或禁止哪些状态。PawnData 字段现已启用，但 PawnExtension 设置到 ASC 的调用仍未启用，配置后要检查 ASC 实際引用。

## EffectContext 配套缺口

HodgeGameplayAbility::MakeEffectContext 调用父类创建句柄后，尝试提取 FHodgeGameplayEffectContext，并 check 结果。仓库未找到有效自定义 AbilitySystemGlobals 分配实现与对应配置，因此默认 Context 与项目期望类型不匹配。

需要补充 Globals 类型，重写分配入口返回自定义 Context，并配置引擎使用它。验收应检查实际 ScriptStruct 类型，再创建 GE Spec，不能通过删除 check 掩盖数据管线不一致。

当前自定义 Context 的 NetSerialize 复用父类，额外本地字段不会因此自动复制。Iris 路径也转发父类序列化；将来加入必须联网的自定义字段时，需要同步设计序列化和验证。

## Cue 与全局能力

HodgeGameplayCueManager 实现了资源管理相关逻辑，但 AssetManager 的 InitializeGameplayCueManager 仍占位，配置也未指向项目 CueManager。准确结论是“自定义 Cue 管理/预加载未接通”，不是“原生 GAS 在任何情况下都不能播放 Cue”。

GlobalAbilitySystem 保存世界级授予，并对注册 ASC 应用。调用这些公共方法时仍需检查权威边界，不能把 WorldSubsystem 自动等同于仅服务器执行。

源码：[Ability](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp)、[ASC](../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp)、[Context](../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayEffectContext.cpp)、[AbilitySet](../../Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp)。
