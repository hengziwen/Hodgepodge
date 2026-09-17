# 扫描快照

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

## 生成信息

- UTC：2026-09-17T02:13:37.446563+00:00
- Git HEAD：`2606b0cf9f54178c778a4e4c96e317c9f048ddd4`
- 源文件（h/cpp/cs）：152
- Content/Main 文件：41
- 原生标签注册条目：185
- 漂移跟踪文件：208

## 生成时已有的受 Git 跟踪修改

```text
M Config/DefaultGame.ini
 M Content/Main/Character/Hero/BP_Hero_Pover.uasset
 M Docs/Design/ability-timeline.md
 M Docs/KnowledgeBase/01-project-map.md
 M Docs/KnowledgeBase/02-architecture.md
 M Docs/KnowledgeBase/03-runtime-startup.md
 M Docs/KnowledgeBase/04-pawn-initialization.md
 M Docs/KnowledgeBase/05-data-assets.md
 M Docs/KnowledgeBase/06-input.md
 M Docs/KnowledgeBase/07-gas.md
 M Docs/KnowledgeBase/08-combat-health.md
 M Docs/KnowledgeBase/09-camera-animation.md
 M Docs/KnowledgeBase/10-game-features.md
 M Docs/KnowledgeBase/11-network.md
 M Docs/KnowledgeBase/12-integration-backlog.md
 M Docs/KnowledgeBase/13-build-config.md
 M Docs/KnowledgeBase/14-troubleshooting.md
 M Docs/KnowledgeBase/16-validation.md
 M Docs/KnowledgeBase/17-maintenance-glossary.md
 M Docs/KnowledgeBase/18-faq.md
 M Docs/KnowledgeBase/README.md
 M Docs/KnowledgeBase/Reference/README.md
 M Docs/KnowledgeBase/Reference/assets.md
 M Docs/KnowledgeBase/Reference/config.md
 M Docs/KnowledgeBase/Reference/gameplay-tags.md
 M Docs/KnowledgeBase/Reference/plugins.md
 M Docs/KnowledgeBase/Reference/snapshot.json
 M Docs/KnowledgeBase/Reference/source-abilitysystem.md
 M Docs/KnowledgeBase/Reference/source-camera.md
 M Docs/KnowledgeBase/Reference/source-character.md
 M Docs/KnowledgeBase/Reference/source-component.md
 M Docs/KnowledgeBase/Reference/source-core.md
 M Docs/KnowledgeBase/Reference/source-data.md
 M Docs/KnowledgeBase/Reference/source-gamefeatures.md
 M Docs/KnowledgeBase/Reference/source-index.md
 M Docs/KnowledgeBase/tools/kb.py
 M Docs/README.md
 M Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp
 M Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayTags.cpp
 M Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp
 M Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h
 M Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h
 M Source/Hodgepodge/Public/Data/HodgeAssetManager.h
```

这里只记录已跟踪路径状态，未跟踪知识库自身不包含在此列表。HEAD 不足以还原 dirty 工作区；[snapshot.json](snapshot.json) 保存扫描范围的 SHA-256。

## 验证边界

没有运行 Unreal 编译、PIE、打包或蓝图数据解析。Content/Main 之外只统计顶层文件数量，不做引用结论。函数与宏索引是导航候选，不做 C++ 语义解析。人工章节核对日期不会由 refresh 自动更新。

运行 `python Docs/KnowledgeBase/tools/kb.py check` 检查相对链接和跟踪文件漂移。发现变化后，先复核人工章节，再刷新快照。
