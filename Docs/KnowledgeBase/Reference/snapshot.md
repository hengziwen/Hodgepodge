# 扫描快照

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

## 生成信息

- UTC：2026-09-17T05:23:48.243462+00:00
- Git HEAD：`5847f9961eb5be1a859888af1cc176b861acda5a`
- 源文件（h/cpp/cs）：146
- Content/Main 文件：43
- 原生标签注册条目：152
- 漂移跟踪文件：205

## 生成时已有的受 Git 跟踪修改

```text
AM Config/DefaultGameplayTags.ini
 M Content/CodexText/AnimInstance/ALS/ABP_ALS_Grounded.uasset
A  Content/CodexText/Montage/AM_Attack01_Montage.uasset
A  Content/CodexText/Montage/AM_Attack02_Montage.uasset
A  Content/CodexText/Montage/AM_Attack03_Montage.uasset
A  Content/CodexText/Montage/AM_Attack04_Montage.uasset
A  Content/CodexText/Montage/AM_Attack05_Montage.uasset
 M Content/Main/Character/Hero/BP_Hero_Pover.uasset
AM Content/Main/Character/Hero/GA_Attack.uasset
 M Content/Main/Data/DA_Dafult_PawnData.uasset
A  Content/Main/Data/DA_Pover.uasset
 M Content/Main/Input/DA_HodgeInputConfig.uasset
 M Content/Main/Input/IMC_Default.uasset
A  Content/Main/Input/InputAction/IA_Attack.uasset
 M Docs/KnowledgeBase/01-project-map.md
 M Docs/KnowledgeBase/05-data-assets.md
 M Docs/KnowledgeBase/07-gas.md
 M Docs/KnowledgeBase/08-combat-health.md
 M Docs/KnowledgeBase/12-integration-backlog.md
 M Docs/KnowledgeBase/14-troubleshooting.md
 M Docs/KnowledgeBase/16-validation.md
 M Docs/KnowledgeBase/17-maintenance-glossary.md
 M Docs/KnowledgeBase/18-faq.md
 M Docs/KnowledgeBase/21-update-2026-09-17.md
 M Docs/KnowledgeBase/README.md
 M Docs/KnowledgeBase/Reference/README.md
 M Docs/KnowledgeBase/Reference/assets.md
 M Docs/KnowledgeBase/Reference/config.md
 M Docs/KnowledgeBase/Reference/gameplay-tags.md
 M Docs/KnowledgeBase/Reference/snapshot.json
 M Docs/KnowledgeBase/Reference/source-abilitysystem.md
 M Docs/KnowledgeBase/Reference/source-data.md
 M Docs/KnowledgeBase/Reference/source-index.md
 M Docs/KnowledgeBase/tools/kb.py
 M README.md
```

这里只记录已跟踪路径状态，未跟踪知识库自身不包含在此列表。HEAD 不足以还原 dirty 工作区；[snapshot.json](snapshot.json) 保存扫描范围的 SHA-256。

## 验证边界

没有运行 Unreal 编译、PIE、打包或蓝图数据解析。Content/Main 之外只统计顶层文件数量，不做引用结论。函数与宏索引是导航候选，不做 C++ 语义解析。人工章节核对日期不会由 refresh 自动更新。

运行 `python Docs/KnowledgeBase/tools/kb.py check` 检查相对链接和跟踪文件漂移。发现变化后，先复核人工章节，再刷新快照。
