# 项目文档

**dev-AN 通知驱动迁移：[彻底弃用自建 Timeline，迁移到动画通知](Design/anim-notify-combat-migration.md)。** 旧文件集中归档，当前配置参阅通知驱动手册；实际验证见迁移报告。

[客户端连段／移动取消窗口修复](Validation/client-combo-window-2026-10-07.md)：首帧服务器授权竞态、现有输入缓存与正式连段客户端回归。

2026-10-06 同步。先看 [项目 README](../README.md)、[本地知识库](KnowledgeBase/README.md)、[当前状态与待办](KnowledgeBase/12-integration-backlog.md)。精确代码与资产见 [参考索引](KnowledgeBase/Reference/README.md)。

**攻击配置统一入口：[攻击能力配置手册](Guides/attack-ability-configuration.md)。** 查询 Definition、AnimNotify、HitCheck、Profile、连段表、能力授予和输入时，优先引用手册对应章节；设计文档作为背景。

[Lyra → Hodgepodge 迁移现状与检查清单](Design/lyra-migration-status.md)：九个子系统的当前进度、缺口、建议顺序与后续验收复选框。

## 属性成长与装备加成

- [角色属性初始化、等级成长与装备加成](Design/character-attribute-growth.md)：首版已实现；曲线、服务器初始化顺序、升级/装备资源策略及实际验证见第 13 节。

## 已实施方案与配置

- [配置检测体与多段独立技能](Design/skill-hit-volumes-usage.md)：形状、锚点、Window／Point、命中组、指定目标、输入授予和示例资产；[设计与后续 Actor 范围](Design/skill-hit-volumes-proposal.md)。
- [命中检测调试绘制](Design/hit-detection-debug-draw.md)：Weapon、Body、HitBox 默认绘制 5 秒，控制台开关与验证范围。
- [Timeline](Design/ability-timeline-stage1.md)、[Definition/连段](Design/ability-definition-combo-graph.md)、[技能编辑器](Design/ability-definition-editor.md)。
- [统一检测与 Melee GE](Design/melee-detection-ga-effects.md)、[命中窗口](Design/combat-hit-windows.md)、[装备基础](Design/equipment-weapon-system.md)。
- [旋转约束](Design/character-rotation-policy.md)、[连段记忆与末段重开](Design/combo-retention.md)、[武器显现/插槽回背](Design/weapon-presentation.md)。
- [正式动画说明](../Content/Main/Character/Hero/Anim/README.md)、[Lyra 动画研究过程](Design/lyra-animation-inspection-20261004.md)。

Design 并非全是未实现草案：各篇开头区分当前实现、历史设计和后续目标；历史示例不覆盖当前事实。

## UI 迁移与学习

- [迁移计划](Design/lyra-ui-migration-plan.md)、[文件导览](Design/lyra-ui-file-guide.md)、[当前架构](Design/hodge-ui-architecture.md)。
- [UMG/Slate](Design/umg-slate-mental-model.md)、[UIExtension](Design/ui-extension-system.md)、[Indicator](Design/indicator-ui-system.md)。

CommonUI 已引入，但完整前端/HUD、GameViewport 和部分停用依赖仍需接通。

## 验证与维护

- [检测体与多段攻击验证](Validation/skill-hit-volumes-2026-10-06.md)：构建、原生、单人、100ms 联机、原技能保护与测试入口边界。
- [跳跃衔接与死亡能力验证](Validation/jump-death-2026-10-06.md)：跳跃过渡修正；死亡事件/阶段已测试，互斥计数与 PIE 清理仍有错误。
- [39 个正式资产迁入 Main](Validation/main-assets-migration-2026-10-06.md)：来源、目标、引用修正、备份与本次验证。
- [实际验证范围](KnowledgeBase/16-validation.md)、[本轮状态](KnowledgeBase/26-update-2026-10-06.md)。
- Validation 和 Design 中带日期的验收记录保留当时结果；不得据此宣称当前正式伤害、重生或打包全部通过。
- [开发流程](AI_DEVELOPMENT.md)、[开发约定](../AGENTS.md)、[评审标准](../CODE_REVIEW.md)。
- [旧长版 README](History/README-before-20261006.md)、[旧扫描快照](KnowledgeBase/History/snapshot-before-20261006.json) 保留历史。
