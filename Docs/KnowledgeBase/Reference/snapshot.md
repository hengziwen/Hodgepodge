# 扫描快照

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

## 生成信息

- UTC：2026-10-05T17:05:40.569816+00:00
- Git HEAD：`925a13ef353b3e2586696ab981e1553ac8e43b6a`
- 源文件（h/cpp/cs）：292
- Main 与选定 CodexText 正式依赖文件：143
- 原生标签注册条目：202
- 漂移跟踪文件：509

## 生成时已有的受 Git 跟踪修改

```text
M CODE_REVIEW.md
 M Content/CodexText/LyraAnimation/README.md
 M Content/Main/Character/Hero/Anim/README.md
 M Docs/AI_DEVELOPMENT.md
 M Docs/Design/ability-definition-combo-graph.md
 M Docs/Design/ability-definition-combo-implementation-plan.md
 M Docs/Design/ability-definition-editor.md
 M Docs/Design/ability-timeline-stage1.md
 M Docs/Design/ability-timeline.md
 M Docs/Design/character-rotation-policy.md
 M Docs/Design/combat-component-unification-validation-20261001.md
 M Docs/Design/combat-hit-windows.md
 M Docs/Design/combo-retention.md
 M Docs/Design/equipment-weapon-system.md
 M Docs/Design/hodge-ui-architecture.md
 M Docs/Design/indicator-ui-system.md
 M Docs/Design/lyra-animation-inspection-20261004.md
 M Docs/Design/lyra-ui-file-guide.md
 M Docs/Design/lyra-ui-migration-plan.md
 M Docs/Design/melee-detection-ga-effects.md
 M Docs/Design/melee-experience-ge-validation-20261001.md
 M Docs/Design/melee-pie-validation-20261001.md
 M Docs/Design/ui-extension-system.md
 M Docs/Design/umg-slate-mental-model.md
 M Docs/Design/weapon-presentation.md
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
 M Docs/KnowledgeBase/19-update-2026-09-13.md
 M Docs/KnowledgeBase/20-code-review.md
 M Docs/KnowledgeBase/21-update-2026-09-17.md
 M Docs/KnowledgeBase/22-update-2026-09-19.md
 M Docs/KnowledgeBase/23-update-2026-09-22.md
 M Docs/KnowledgeBase/24-update-2026-09-28.md
 M Docs/KnowledgeBase/25-update-2026-09-29.md
 M Docs/KnowledgeBase/README.md
 M Docs/KnowledgeBase/Reference/README.md
 M Docs/KnowledgeBase/Reference/assets.md
 M Docs/KnowledgeBase/Reference/config.md
 M Docs/KnowledgeBase/Reference/gameplay-tags.md
 M Docs/KnowledgeBase/Reference/snapshot.json
 M Docs/KnowledgeBase/Reference/snapshot.md
 M Docs/KnowledgeBase/Reference/source-abilitysystem.md
 M Docs/KnowledgeBase/Reference/source-animation.md
 M Docs/KnowledgeBase/Reference/source-character.md
 M Docs/KnowledgeBase/Reference/source-component.md
 M Docs/KnowledgeBase/Reference/source-core.md
 M Docs/KnowledgeBase/Reference/source-data.md
 M Docs/KnowledgeBase/Reference/source-equipment.md
 M Docs/KnowledgeBase/Reference/source-index.md
 M Docs/KnowledgeBase/Reference/source-module.md
 M Docs/KnowledgeBase/Reference/source-tests.md
 M Docs/KnowledgeBase/tools/README.md
 M Docs/KnowledgeBase/tools/kb.py
 M "Docs/Learning/\344\274\244\345\256\263\351\223\276\350\267\257\347\232\204\346\200\273\346\265\201\347\250\213.md"
 M Docs/README.md
 M Docs/Validation/basic-attack-2026-09-24.md
 M Docs/Validation/definition-combo-2026-09-28.md
 M Docs/Validation/timeline-2026-09-24.md
 M LYRA_LEARNING_GUIDE.md
 M LYRA_RUNTIME_FLOW.md
 M README.md
 M "TagMigration/GameplayTags_\345\257\271\347\205\247\350\241\250.md"
 M Tools/ChiR24MCP/README.md
 M Tools/LocomotionLab/README.md
 M Tools/LocomotionLab/grounded/refinement-verification.md
 M Tools/LocomotionLab/grounded/verification-notes.md
 M Tools/ModelRepair/README.md
 M Tools/UnrealMCP/README.md
 M Tools/Weapon/README.md
 M "UE5 \345\274\200\346\224\276\344\270\226\347\225\214\345\212\250\344\275\234 RPG \346\236\266\346\236\204\346\226\271\346\241\210 V2.md"
```

这里只记录已跟踪路径状态，未跟踪知识库自身不包含在此列表。HEAD 不足以还原 dirty 工作区；[snapshot.json](snapshot.json) 保存扫描范围的 SHA-256。

## 验证边界

索引刷新本身没有运行 Unreal 编译、PIE、打包或蓝图数据解析。资产页包括 Main 与选定 CodexText 正式依赖目录；路径存在不证明内部引用正确。人工章节记录的独立只读资产解析和历史运行验收不由本工具执行。函数与宏索引是导航候选，不做 C++ 语义解析。人工章节核对日期不会由 refresh 自动更新。

运行 `python Docs/KnowledgeBase/tools/kb.py check` 检查相对链接和跟踪文件漂移。发现变化后，先复核人工章节，再刷新快照。
