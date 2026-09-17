# 常见问题

[返回首页](README.md)

> 最近源码核对：2026-09-17。

## 现在还要从零接 HeroComponent 吗？

不用。它已启用并在 HeroCharacter 构造挂载；状态就绪后接入 ASC、输入和相机。旧知识库“整份注释”是 9 月 10 日状态，现在已撤销。

## SetPawnData、ProcessAbilityInput 和相机委托还缺调用者吗？

都已有有效入口：GameMode 生成前注入 PawnData；HodgePlayerController::PostProcessInput 消费能力输入；Hero 在 DataInitialized 绑定相机模式委托。仍要验证实际生成类、状态到达和资产值。

## 为什么主链有代码仍可能不能移动？

检查 Hero DefaultInputMappings、InputConfig、IMC 软引用加载、Native Tag。当前 AddMappingContext 在 bRegisterWithSettings 条件内，false 会跳过；Ready 事件也可能在绑定未成功时发送。不要只看 GameplayReady。

## 为什么按键到了 ASC 但没有技能？

PlayerState::SetPawnData 现已在权威端遍历 PawnData->AbilitySets 授予技能（未记录 GrantedHandles）。先确认 Spec 确实被授予，再检查动态 InputTag 精确匹配、Cost、Cooldown 和激活组；重生或重复初始化时注意没有句柄可供撤销。

## PawnData 现在有哪些字段？

PawnClass、AbilitySets、TagRelationshipMapping、InputConfig、DefaultCameraMode 都存在。关系映射和输入/相机消费已接入；AbilitySets 的基础授予仍缺。字段存在不保证资产值正确。

## EffectContext 还缺 Globals 吗？

不缺类和配置了。新增 HodgeAbilitySystemGlobals 返回 Hodge Context，DefaultGame.ini 已选择它。如果仍有类型 check，检查运行时 Globals、实际 Context 类型和是否使用旧构建。

## 现在有伤害和死亡吗？

HealthSet 已实现 Damage/Healing 转换和耗尽广播；完整命中、敌人 ASC、角色 HealthComponent/死亡绑定仍不完整。不能继续说“结算主体全注释”，也不能说“完整战斗已通过”。

## GameFeature 和 Cue 是否全部好了？

输入 Action 添加分支已恢复，Hero 额外输入移除也已实现并在 EndPlay 统一解绑。CueManager 和 Policy 已配置，但 Cue 路径观察者创建仍注释，注销移除和预加载仍不完整。

## 为什么还要查重生？

ASC 接入已收敛到组件路径，不再是双路径并存；但控制器先清 Avatar 与 PawnExtension 条件清理的相对顺序仍可能相互影响。还需验证输入句柄、状态、Cue 和旧 Pawn 的解绑。

## 新的 BP 和相机模式资产可以证明资产都配好了？

只能证明文件存在。内部父类、Mesh、AnimClass、默认字段和实际地图使用仍需编辑器验证。本轮未解析二进制资产。相机模式现为 `CM_ThirdPerson` / `CM_ThirdPerson_Death`，旧的 `CM_Default` 已删除。

## 插件是否都跟 README 一样？

以描述文件为准：UNTLink 当前禁用；ALS 描述默认启用，即使主模块不再直接依赖；UnrealMCP 与 McpAutomationBridge 均为 Editor-only 插件。它们的加载结果、连接与工具调用本轮未验证。

## 攻击时间轴、ComboSet 现在能用了吗？

**不能 —— 它们当前根本不存在。** ⚠️ 早先的记录是"源码里类型、校验和 Task 实现都已存在，只是没有 C++ 调用点"，但那批改动属于当时工作区的**未提交改动，随后已被丢弃**：当前 `Source/Hodgepodge` 搜不到 `HodgeAbilityTimeline` / `HodgeComboSet` / `PlayTimeline`，`Config/DefaultGame.ini` 也没有对应 PrimaryAssetTypesToScan，`Attack.*` 标签未注册，`GA_Melee` 与 `Content/Main/Character/Hero/Ability/` 也不存在。攻击闭环需要**从零实现**。详见 [本轮变更](21-update-2026-09-17.md) 的回退标注与 [接通清单](12-integration-backlog.md) KB-15。

## 知识库是否验证了当前编译和运行？

本轮只验证文档、自动索引与文件漂移，未做 UE 构建、蓝图编译、PIE、联机或打包。项目约定要求文档修改只验证文档与差异。

## 怎么查最新代码？

打开 Reference 或运行 `python Docs/KnowledgeBase/tools/kb.py search "类名或标签"`。人工章节说明行为，自动参考提供导航；扫描工具不解析完整 C++ 或蓝图。
