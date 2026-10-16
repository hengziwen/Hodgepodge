# 扫描快照

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

## 生成信息

- UTC：2026-10-16T01:54:17.242216+00:00
- Git HEAD：`d028591d56ba402d000e6c251eb6ba9695279708`
- 源文件（h/cpp/cs）：440
- Main 与选定 CodexText 正式依赖文件：300
- 原生标签注册条目：237
- 漂移跟踪文件：919

## 生成时已有的受 Git 跟踪修改

```text
M Content/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1.uasset
 M Content/Main/Character/Hero/Ability/BasicAttack/DA_Attack_2.uasset
 M Content/Main/Character/Hero/Ability/BasicAttack/DA_Attack_3.uasset
 M Content/Main/Character/Hero/Ability/BasicAttack/DA_Attack_4.uasset
 M Content/Main/Character/Hero/Ability/BasicAttack/DA_Attack_5.uasset
 M Content/Main/Character/Hero/Ability/Havoc/DA_EnhancedAttack_1.uasset
 M Content/Main/Character/Hero/Ability/Havoc/DA_EnhancedAttack_2.uasset
 M Content/Main/Character/Hero/Ability/Havoc/DA_EnhancedAttack_3.uasset
 M Content/Main/Character/Hero/Ability/Havoc/DA_EnhancedAttack_4.uasset
 M Content/Main/Character/Hero/Ability/Havoc/DA_EnhancedAttack_5.uasset
 M Content/Main/Character/Hero/Ability/Havoc/DA_MieYin.uasset
 M Content/Main/Character/Hero/Ability/Havoc/DA_PoMing.uasset
 M Content/Main/Character/Hero/Ability/ResonanceSkill/DA_Resonance_Skill.uasset
 D Content/__ExternalActors__/ThirdPerson/Maps/ThirdPersonMap/C/QF/BY1DA5CRGQSAMOTK8RA8KR.uasset
 M Docs/Design/architecture-refactor-review-2026-10-15.md
 M Docs/Guides/attack-ability-configuration.md
 M Docs/Guides/hit-reaction-configuration.md
 M Docs/Guides/resonance-liberation-configuration.md
 M Docs/KnowledgeBase/05-data-assets.md
 M Docs/KnowledgeBase/07-gas.md
 M Docs/KnowledgeBase/08-combat-health.md
 M Docs/KnowledgeBase/10-game-features.md
 M Docs/KnowledgeBase/29-current-work-and-backlog.md
 M Docs/KnowledgeBase/README.md
 M Docs/KnowledgeBase/Reference/README.md
 M Docs/KnowledgeBase/Reference/assets.md
 M Docs/KnowledgeBase/Reference/gameplay-tags.md
 M Docs/KnowledgeBase/Reference/snapshot.json
 M Docs/KnowledgeBase/Reference/snapshot.md
 M Docs/KnowledgeBase/Reference/source-abilitysystem.md
 M Docs/KnowledgeBase/Reference/source-combat.md
 M Docs/KnowledgeBase/Reference/source-component.md
 M Docs/KnowledgeBase/Reference/source-core.md
 M Docs/KnowledgeBase/Reference/source-data.md
 M Docs/KnowledgeBase/Reference/source-gamefeatures.md
 M Docs/KnowledgeBase/Reference/source-index.md
 M Docs/KnowledgeBase/Reference/source-tests.md
 M Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.cpp
 M Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp
 M Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp
 M Source/Hodgepodge/Private/AbilitySystem/Executions/HodgeDamageExecution.cpp
 M Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp
 M Source/Hodgepodge/Private/Component/HodgeHealthComponent.cpp
 M Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp
 M Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp
 M Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp
 M Source/Hodgepodge/Private/Data/HodgeAbilityDefinition.cpp
 M Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp
 M Source/Hodgepodge/Private/Data/HodgeGameData.cpp
 M Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddAbilities.cpp
 M Source/Hodgepodge/Private/Tests/HodgeEnemyAITestTypes.cpp
 M Source/Hodgepodge/Private/Tests/HodgeEnemyAITestTypes.h
 M Source/Hodgepodge/Private/Tests/HodgeHavocTests.cpp
 M Source/Hodgepodge/Private/Tests/HodgeMeleeRoutingTests.cpp
 M Source/Hodgepodge/Private/Tests/HodgeResonanceEnergyTests.cpp
 M Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h
 M Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h
 M Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h
 M Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h
 M Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h
 M Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h
 M Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h
 M Source/Hodgepodge/Public/Data/HodgeAbilitySet.h
 M Source/Hodgepodge/Public/Data/HodgeGameData.h
 M Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddAbilities.h
 M Tools/CinematicUltimate/configure_assets.py
 M Tools/Havoc/README.md
 M Tools/Havoc/configure_assets.py
 M Tools/Havoc/validate_assets.py
 M Tools/HitReaction/README.md
```

这里只记录已跟踪路径状态，未跟踪知识库自身不包含在此列表。HEAD 不足以还原 dirty 工作区；[snapshot.json](snapshot.json) 保存扫描范围的 SHA-256。

## 验证边界

索引刷新本身没有运行 Unreal 编译、PIE、打包或蓝图数据解析。资产页包括 Main 与选定 CodexText 正式依赖目录；路径存在不证明内部引用正确。人工章节记录的独立只读资产解析和历史运行验收不由本工具执行。函数与宏索引是导航候选，不做 C++ 语义解析。人工章节核对日期不会由 refresh 自动更新。

运行 `python Docs/KnowledgeBase/tools/kb.py check` 检查相对链接和跟踪文件漂移。发现变化后，先复核人工章节，再刷新快照。
