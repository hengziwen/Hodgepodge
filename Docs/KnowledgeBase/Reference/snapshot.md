# 扫描快照

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

## 生成信息

- UTC：2026-09-22T01:52:07.722014+00:00
- Git HEAD：`993a9eb8bef49afc7c78b5081e681883de4c401a`
- 源文件（h/cpp/cs）：152
- Content/Main 文件：44
- 原生标签注册条目：162
- 漂移跟踪文件：212

## 生成时已有的受 Git 跟踪修改

```text
M Content/CodexText/Anim/Grounded/Run_B.uasset
 M Content/CodexText/Anim/Grounded/Run_F.uasset
 M Content/CodexText/Anim/Grounded/Run_LB.uasset
 M Content/CodexText/Anim/Grounded/Run_LF.uasset
 M Content/CodexText/Anim/Grounded/Run_RB.uasset
 M Content/CodexText/Anim/Grounded/Run_RF.uasset
 M Content/CodexText/Anim/Grounded/Stand1.uasset
 M Content/CodexText/Anim/Grounded/Stand1_Turn_L90D.uasset
 M Content/CodexText/Anim/Grounded/Stand1_Turn_R90D.uasset
 M Content/CodexText/Anim/Grounded/Stop_Run_L.uasset
 M Content/CodexText/Anim/Grounded/Stop_Run_R.uasset
 M Content/CodexText/Anim/Grounded/Stop_Walk_L.uasset
 M Content/CodexText/Anim/Grounded/Stop_Walk_R.uasset
 M Content/CodexText/Anim/Grounded/Walk_B.uasset
 M Content/CodexText/Anim/Grounded/Walk_F.uasset
 M Content/CodexText/Anim/Grounded/Walk_LB.uasset
 M Content/CodexText/Anim/Grounded/Walk_LF.uasset
 M Content/CodexText/Anim/Grounded/Walk_RB.uasset
 M Content/CodexText/Anim/Grounded/Walk_RF.uasset
 M Content/CodexText/AnimInstance/ALS/ABP_ALS_Grounded.uasset
 M Content/CodexText/Locomotion/BP_LocomotionHero.uasset
 M Content/CodexText/Locomotion/L_LocomotionLab.umap
 M Content/CodexText/Timeline/DA_TimelineTest.uasset
 M Content/CodexText/Timeline/GA_TimelineTest.uasset
 M Docs/KnowledgeBase/01-project-map.md
 M Docs/KnowledgeBase/07-gas.md
 M Docs/KnowledgeBase/09-camera-animation.md
 M Docs/KnowledgeBase/12-integration-backlog.md
 M Docs/KnowledgeBase/13-build-config.md
 M Docs/KnowledgeBase/14-troubleshooting.md
 M Docs/KnowledgeBase/16-validation.md
 M Docs/KnowledgeBase/17-maintenance-glossary.md
 M Docs/KnowledgeBase/18-faq.md
 M Docs/KnowledgeBase/20-code-review.md
 M Docs/KnowledgeBase/README.md
 M Docs/KnowledgeBase/Reference/README.md
 M Docs/KnowledgeBase/Reference/assets.md
 M Docs/KnowledgeBase/Reference/gameplay-tags.md
 M Docs/KnowledgeBase/Reference/snapshot.json
 M Docs/KnowledgeBase/Reference/source-abilitysystem.md
 M Docs/KnowledgeBase/Reference/source-codextext.md
 M Docs/KnowledgeBase/Reference/source-component.md
 M Docs/KnowledgeBase/Reference/source-index.md
 M Docs/KnowledgeBase/tools/kb.py
 M Source/Hodgepodge/Hodgepodge.Build.cs
 M Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayTags.cpp
 M Source/Hodgepodge/Private/CodexText/HodgeGroundedAuthoring.cpp
 M Source/Hodgepodge/Private/CodexText/HodgeGroundedLocomotion.cpp
 M Source/Hodgepodge/Private/CodexText/HodgeLocomotionLab.cpp
 M Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp
 M Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h
 M Source/Hodgepodge/Public/CodexText/HodgeGroundedLocomotion.h
 M Source/Hodgepodge/Public/Component/HodgeHeroComponent.h
 M Tools/LocomotionLab/README.md
```

这里只记录已跟踪路径状态，未跟踪知识库自身不包含在此列表。HEAD 不足以还原 dirty 工作区；[snapshot.json](snapshot.json) 保存扫描范围的 SHA-256。

## 验证边界

没有运行 Unreal 编译、PIE、打包或蓝图数据解析。Content/Main 之外只统计顶层文件数量，不做引用结论。函数与宏索引是导航候选，不做 C++ 语义解析。人工章节核对日期不会由 refresh 自动更新。

运行 `python Docs/KnowledgeBase/tools/kb.py check` 检查相对链接和跟踪文件漂移。发现变化后，先复核人工章节，再刷新快照。
