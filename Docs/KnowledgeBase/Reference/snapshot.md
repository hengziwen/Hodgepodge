# 扫描快照

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

## 生成信息

- UTC：2026-10-06T16:22:43.688301+00:00
- Git HEAD：`7f4e8e4806b0114d243e862fe07705f7bfbcc00b`
- 源文件（h/cpp/cs）：311
- Main 与选定 CodexText 正式依赖文件：171
- 原生标签注册条目：205
- 漂移跟踪文件：566

## 生成时已有的受 Git 跟踪修改

```text
M Config/DefaultGameplayTags.ini
 M Content/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1.uasset
 M Content/Main/Character/Hero/Ability/BasicAttack/DA_Attack_2.uasset
 M Content/Main/Character/Hero/Ability/BasicAttack/DA_Attack_3.uasset
 M Content/Main/Character/Hero/Ability/BasicAttack/DA_Attack_4.uasset
 M Content/Main/Character/Hero/Ability/BasicAttack/DA_Attack_5.uasset
 M Content/Main/Character/Hero/Ability/BasicAttack/Timeline/DA_Attack01_Timeline.uasset
 M Content/Main/Character/Hero/Ability/BasicAttack/Timeline/DA_Attack02_Timeline.uasset
 M Content/Main/Character/Hero/Ability/BasicAttack/Timeline/DA_Attack03_Timeline.uasset
 M Content/Main/Character/Hero/Ability/BasicAttack/Timeline/DA_Attack04_Timeline.uasset
 M Content/Main/Character/Hero/Ability/BasicAttack/Timeline/DA_Attack05_Timeline.uasset
 M Content/Main/Character/Hero/Ability/GA_Hero_Death.uasset
 M Content/Main/Character/Hero/Anim/ABP_Pover_Base.uasset
 M Content/Main/Weapon/BP_WeaponInstance_Sword.uasset
 M Content/Wuwa/Weapon/R5Sword506Md20001_LOD0_Skeleton_Skeleton.uasset
 M Content/Wuwa/Weapon/Sword_Qiuyuan.uasset
 M Docs/AI_DEVELOPMENT.md
 M Docs/Design/lyra-migration-status.md
 M Docs/KnowledgeBase/08-combat-health.md
 M Docs/KnowledgeBase/README.md
 M Docs/KnowledgeBase/Reference/README.md
 M Docs/KnowledgeBase/Reference/assets.md
 M Docs/KnowledgeBase/Reference/config.md
 M Docs/KnowledgeBase/Reference/gameplay-tags.md
 M Docs/KnowledgeBase/Reference/snapshot.json
 M Docs/KnowledgeBase/Reference/snapshot.md
 M Docs/KnowledgeBase/Reference/source-abilitysystem.md
 M Docs/KnowledgeBase/Reference/source-combat.md
 M Docs/KnowledgeBase/Reference/source-component.md
 M Docs/KnowledgeBase/Reference/source-data.md
 M Docs/KnowledgeBase/Reference/source-index.md
 M Docs/KnowledgeBase/Reference/source-input.md
 M Docs/KnowledgeBase/Reference/source-module.md
 M Docs/KnowledgeBase/Reference/source-tests.md
 M Docs/KnowledgeBase/tools/kb.py
 M Docs/README.md
 M README.md
 M Source/HodgeAbilityEditor/HodgeAbilityEditor.Build.cs
 M Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.cpp
 M Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.cpp
 M Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp
 M Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp
 M Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp
 M Source/Hodgepodge/Private/Combat/HodgeHitDetection.cpp
 M Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp
 M Source/Hodgepodge/Private/Data/HodgeAbilityDefinition.cpp
 M Source/Hodgepodge/Private/Data/HodgeCharacterStatProfile.cpp
 M Source/Hodgepodge/Private/Data/HodgeEquipmentStatProfile.cpp
 M Source/Hodgepodge/Private/Data/HodgePawnData.cpp
 M Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h
 M Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h
 M Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h
 M Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h
 M Source/Hodgepodge/Public/Combat/HodgeHitDetection.h
 M Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h
 M Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h
 M Source/Hodgepodge/Public/Data/HodgeAbilitySet.h
 M Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h
 M Source/Hodgepodge/Public/Data/HodgeComboDefinition.h
 M Source/Hodgepodge/Public/Data/HodgePawnData.h
 M Source/Hodgepodge/Public/Input/HodgeInputConfig.h
```

这里只记录已跟踪路径状态，未跟踪知识库自身不包含在此列表。HEAD 不足以还原 dirty 工作区；[snapshot.json](snapshot.json) 保存扫描范围的 SHA-256。

## 验证边界

索引刷新本身没有运行 Unreal 编译、PIE、打包或蓝图数据解析。资产页包括 Main 与选定 CodexText 正式依赖目录；路径存在不证明内部引用正确。人工章节记录的独立只读资产解析和历史运行验收不由本工具执行。函数与宏索引是导航候选，不做 C++ 语义解析。人工章节核对日期不会由 refresh 自动更新。

运行 `python Docs/KnowledgeBase/tools/kb.py check` 检查相对链接和跟踪文件漂移。发现变化后，先复核人工章节，再刷新快照。
