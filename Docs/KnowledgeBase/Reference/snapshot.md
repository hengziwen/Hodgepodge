# 扫描快照

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

## 生成信息

- UTC：2026-10-08T07:44:56.487898+00:00
- Git HEAD：`300d2a90108b620db7d0327cf911a9fa67d31ba8`
- 源文件（h/cpp/cs）：304
- Main 与选定 CodexText 正式依赖文件：148
- 原生标签注册条目：207
- 漂移跟踪文件：547

## 生成时已有的受 Git 跟踪修改

```text
M .gitignore
 M Config/DefaultEngine.ini
 M Config/DefaultGame.ini
 M Config/DefaultGameplayTags.ini
 M Config/DefaultInput.ini
 M Content/Main/Experiences/Exp_HodgeDefaultExperience.uasset
 M Docs/Design/hodge-ui-architecture.md
 M Docs/Design/lyra-migration-status.md
 M Docs/Design/lyra-ui-file-guide.md
 M Docs/Design/lyra-ui-migration-plan.md
 M Docs/KnowledgeBase/10-game-features.md
 M Docs/KnowledgeBase/12-integration-backlog.md
 M Docs/KnowledgeBase/16-validation.md
 M Docs/KnowledgeBase/README.md
 M Docs/KnowledgeBase/Reference/README.md
 M Docs/KnowledgeBase/Reference/assets.md
 M Docs/KnowledgeBase/Reference/config.md
 M Docs/KnowledgeBase/Reference/gameplay-tags.md
 M Docs/KnowledgeBase/Reference/snapshot.json
 M Docs/KnowledgeBase/Reference/snapshot.md
 M Docs/KnowledgeBase/Reference/source-abilitysystem.md
 M Docs/KnowledgeBase/Reference/source-component.md
 M Docs/KnowledgeBase/Reference/source-core.md
 M Docs/KnowledgeBase/Reference/source-gamefeatures.md
 M Docs/KnowledgeBase/Reference/source-index.md
 M Docs/KnowledgeBase/Reference/source-module.md
 M Docs/KnowledgeBase/Reference/source-tests.md
 M Docs/KnowledgeBase/Reference/source-ui.md
 M Docs/KnowledgeBase/tools/kb.py
 M Docs/README.md
 M README.md
 M Source/HodgeAbilityEditor/HodgeAbilityEditor.Build.cs
 M Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp
 M Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp
 M Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp
 M Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp
 M Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerController.cpp
 M Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddWidget.cpp
 M Source/Hodgepodge/Private/UI/Extension/UIExtensionPointWidget.cpp
 M Source/Hodgepodge/Private/UI/HodgeGameViewportClient.cpp
 M Source/Hodgepodge/Private/UI/HodgeHUDLayout.cpp
 M Source/Hodgepodge/Private/UI/Subsystem/HodgeUIManagerSubsystem.cpp
 M Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h
 M Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h
 M Source/Hodgepodge/Public/Component/HodgeHeroComponent.h
 M Source/Hodgepodge/Public/Core/GameInstance/HodgeGameInstanceBase.h
 M Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h
 M Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddWidget.h
 M Source/Hodgepodge/Public/UI/Extension/UIExtensionPointWidget.h
 M Source/Hodgepodge/Public/UI/HodgeActivatableWidget.h
 M Source/Hodgepodge/Public/UI/HodgeGameViewportClient.h
 M Source/Hodgepodge/Public/UI/HodgeHUDLayout.h
 M Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h
```

这里只记录已跟踪路径状态，未跟踪知识库自身不包含在此列表。HEAD 不足以还原 dirty 工作区；[snapshot.json](snapshot.json) 保存扫描范围的 SHA-256。

## 验证边界

索引刷新本身没有运行 Unreal 编译、PIE、打包或蓝图数据解析。资产页包括 Main 与选定 CodexText 正式依赖目录；路径存在不证明内部引用正确。人工章节记录的独立只读资产解析和历史运行验收不由本工具执行。函数与宏索引是导航候选，不做 C++ 语义解析。人工章节核对日期不会由 refresh 自动更新。

运行 `python Docs/KnowledgeBase/tools/kb.py check` 检查相对链接和跟踪文件漂移。发现变化后，先复核人工章节，再刷新快照。
