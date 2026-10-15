# 扫描快照

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

## 生成信息

- UTC：2026-10-15T12:39:24.675583+00:00
- Git HEAD：`b702482967b7185de062392be83cc789f7d031db`
- 源文件（h/cpp/cs）：426
- Main 与选定 CodexText 正式依赖文件：300
- 原生标签注册条目：237
- 漂移跟踪文件：900

## 生成时已有的受 Git 跟踪修改

```text
M .agents/ue-project-context.md
 M .codex/config.toml
 M .gitattributes
 M AGENTS.md
 M Docs/Design/architecture-refactor-review-2026-10-15.md
 M Docs/Design/lyra-migration-status.md
 M Docs/Development/development-standards.md
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
 M Docs/KnowledgeBase/15-development-recipes.md
 M Docs/KnowledgeBase/16-validation.md
 M Docs/KnowledgeBase/17-maintenance-glossary.md
 M Docs/KnowledgeBase/18-faq.md
 M Docs/KnowledgeBase/29-current-work-and-backlog.md
 M Docs/KnowledgeBase/README.md
 M Docs/KnowledgeBase/Reference/README.md
 M Docs/KnowledgeBase/Reference/assets.md
 M Docs/KnowledgeBase/Reference/config.md
 M Docs/KnowledgeBase/Reference/gameplay-tags.md
 M Docs/KnowledgeBase/Reference/snapshot.json
 M Docs/KnowledgeBase/Reference/snapshot.md
 M Docs/KnowledgeBase/Reference/source-abilitysystem.md
 M Docs/KnowledgeBase/Reference/source-actor.md
 M Docs/KnowledgeBase/Reference/source-animation.md
 M Docs/KnowledgeBase/Reference/source-camera.md
 M Docs/KnowledgeBase/Reference/source-character.md
 M Docs/KnowledgeBase/Reference/source-codextext.md
 M Docs/KnowledgeBase/Reference/source-combat.md
 M Docs/KnowledgeBase/Reference/source-component.md
 M Docs/KnowledgeBase/Reference/source-core.md
 M Docs/KnowledgeBase/Reference/source-data.md
 M Docs/KnowledgeBase/Reference/source-equipment.md
 M Docs/KnowledgeBase/Reference/source-gamefeatures.md
 M Docs/KnowledgeBase/Reference/source-index.md
 M Docs/KnowledgeBase/Reference/source-input.md
 M Docs/KnowledgeBase/Reference/source-interface.md
 M Docs/KnowledgeBase/Reference/source-module.md
 M Docs/KnowledgeBase/Reference/source-tests.md
 M Docs/KnowledgeBase/Reference/source-ui.md
 M Docs/Learning/伤害链路的总流程.md
 M Docs/README.md
 M README.md
 M TagMigration/GameplayTags_对照表.md
```

这里只记录已跟踪路径状态，未跟踪知识库自身不包含在此列表。HEAD 不足以还原 dirty 工作区；[snapshot.json](snapshot.json) 保存扫描范围的 SHA-256。

## 验证边界

索引刷新本身没有运行 Unreal 编译、PIE、打包或蓝图数据解析。资产页包括 Main 与选定 CodexText 正式依赖目录；路径存在不证明内部引用正确。人工章节记录的独立只读资产解析和历史运行验收不由本工具执行。函数与宏索引是导航候选，不做 C++ 语义解析。人工章节核对日期不会由 refresh 自动更新。

运行 `python Docs/KnowledgeBase/tools/kb.py check` 检查相对链接和跟踪文件漂移。发现变化后，先复核人工章节，再刷新快照。
