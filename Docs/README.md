# 项目文档

**开发入口：[项目开发准则](Development/development-standards.md)**。新增代码必须有简洁中文注释；讨论或设计具体实现前核查本机 Lyra，已有成熟详细实现直接复用。任务启动同时阅读 [AGENTS.md](../AGENTS.md) 与 [AI 开发与验证流程](AI_DEVELOPMENT.md)。

**dev-AN 通知驱动迁移：[彻底弃用自建 Timeline，迁移到动画通知](Design/anim-notify-combat-migration.md)。** 旧文件集中归档，当前配置参阅通知驱动手册；实际验证见迁移报告。

[客户端连段／移动取消窗口修复](Validation/client-combo-window-2026-10-07.md)：首帧服务器授权竞态、现有输入缓存与正式连段客户端回归。

**2026-10-15 同步**。先看 [项目 README](../README.md)、[本地知识库](KnowledgeBase/README.md)、[当前任务与待办复核](KnowledgeBase/29-current-work-and-backlog.md)、[当前状态与待办](KnowledgeBase/12-integration-backlog.md)。精确代码与资产见 [参考索引](KnowledgeBase/Reference/README.md)。

**本轮新增主线**：[共鸣／暗涌技能体系](Design/havoc-dark-surge.md)（[10-15 验收](Validation/havoc-dark-surge-2026-10-15.md)）、[共鸣解放](Design/resonance-liberation.md)（[10-15 验收](Validation/resonance-liberation-2026-10-15.md)）、[目标锁定](Design/lock-on-system.md)、[软锁定镜头](Validation/soft-lock-camera-2026-10-13.md)、[Dash／Sprint](Design/dash-sprint-system.md)、[角色朝向](Design/character-facing-modes.md)、[受击系统](Design/hit-reaction-system.md)、[敌人 AI](Design/enemy-ai-system.md)、[参考 HUD](Validation/main-hud-reference-2026-10-11.md)、[独立技能移动取消](Design/definition-move-cancel.md)；以及[架构审查](Design/architecture-refactor-review-2026-10-15.md)（**待实施**）。完整汇总见 [10-15 更新](KnowledgeBase/30-update-2026-10-15.md)。

**攻击配置统一入口：[攻击能力配置手册](Guides/attack-ability-configuration.md)。** 查询 Definition、AnimNotify、HitCheck、Profile、连段表、能力授予和输入时，优先引用手册对应章节；设计文档作为背景。

[Lyra → Hodgepodge 迁移现状与检查清单](Design/lyra-migration-status.md)：九个子系统的当前进度、缺口、建议顺序与后续验收复选框。

## 属性成长与装备加成

- [角色属性初始化、等级成长与装备加成](Design/character-attribute-growth.md)：首版已实现；曲线、服务器初始化顺序、升级/装备资源策略及实际验证见第 13 节。
- [Lyra Inventory 与 Equipment 集成设计](Design/inventory-equipment-integration.md)：v0.1 草案，背包核心、单武器位、物品身份、复制与现有装备衔接；仅设计，尚未实现。

## 已实施方案与配置

- [翅膀通知与蒙太奇配置](Guides/wing-notify-montage-configuration.md)：通知内直接选择翼蒙太奇，窗口控制显隐，保留淡出、取消和 Box 采样；[现行设计](Design/attack-wing-presentation.md)、[本轮验证](Validation/wing-notify-montage-2026-10-14.md)。

- [Resonance Skill 配置](Guides/resonance-skill-configuration.md)：独立 Melee 技能模板，E 键、8 秒冷却、动画／伤害／输入调参入口；[创建与验证](Validation/resonance-skill-setup-2026-10-13.md)。当前 Skill1 后摇移动取消见 [设计](Design/definition-move-cancel.md) 与 [修复验证](Validation/definition-move-cancel-2026-10-14.md)。
- [共鸣解放设计与当前实现](Design/resonance-liberation.md)：R键满充消耗、12秒CD、阶段回能、右下充能HUD、前半段电影镜头、全程翼与金刚体固定Box；[配置指南](Guides/resonance-liberation-configuration.md)说明实际资产／调参，[2026-10-15实施验收](Validation/resonance-liberation-2026-10-15.md)记录81项原生测试、35项资产检查及四种PIE模式结果。正式Niagara／音效／震屏素材仍待补齐；[相机原评审](Design/resonance-liberation-cinematic-review-2026-10-14.md)保留历史取舍依据。
- [攻击前摇旋转 Motion Warping](Design/attack-facing-motion-warp.md)：首招立即播放、固定方向 TargetData、前摇根运动旋转、Recovery 释放与预测取消恢复；[实现和验证结果](Validation/attack-facing-motion-warp-2026-10-13.md)。
- [配置检测体与多段独立技能](Design/skill-hit-volumes-usage.md)：形状、锚点、Window／Point、命中组、指定目标、输入授予和示例资产；[设计与后续 Actor 范围](Design/skill-hit-volumes-proposal.md)。
- [命中检测调试绘制](Design/hit-detection-debug-draw.md)：Weapon、Body、HitBox 默认绘制 5 秒，控制台开关与验证范围。
- [Timeline](Design/ability-timeline-stage1.md)、[Definition/连段](Design/ability-definition-combo-graph.md)、[技能编辑器](Design/ability-definition-editor.md)。
- [统一检测与 Melee GE](Design/melee-detection-ga-effects.md)、[命中窗口](Design/combat-hit-windows.md)、[装备基础](Design/equipment-weapon-system.md)。
- [旋转约束](Design/character-rotation-policy.md)、[连段记忆与末段重开](Design/combo-retention.md)、[武器显现/插槽回背](Design/weapon-presentation.md)。
- [受击核心设计](Design/hit-reaction-system.md)、[受击配置手册](Guides/hit-reaction-configuration.md)：四级 Body／独立判定、Impact、目标动画与恢复、敌人 PawnData 初始化已接入；[核心验证](Validation/hit-reaction-2026-10-08.md)、[真实 PIE／联机补测](Validation/hit-reaction-pie-network-2026-10-08.md)。
- [Main 主角受击接入](Validation/hero-hit-reaction-setup-2026-10-08.md)：正式 AnimBP／独立轻反馈 Group／Profile／GA 授予，以及两个客户端模式的真实播放和骨骼混合验证。
- [受击退出与联机动作审查](Validation/animation-network-review-2026-10-09.md)：取消复制语义、移动取消预测／拒绝恢复、网络模拟残留、受击素材时长及前后实测时序。
- [正式动画说明](../Content/Main/Character/Hero/Anim/README.md)、[Lyra 动画研究过程](Design/lyra-animation-inspection-20261004.md)。
- [旋转模式与动画隔离](Design/character-facing-modes.md)、[接口指南](Guides/character-facing-configuration.md)、[实现与测试报告](Validation/character-facing-implementation-2026-10-10.md)：Free／预留八向分支、动作覆盖、输入快照、预测与实际覆盖率；不含完整锁定目标／CameraMode。

- [Dash 与 Sprint 设计](Design/dash-sprint-system.md)、[配置指南](Guides/dash-sprint-configuration.md)：v1.4 加入[根运动 Pivot 滑步修复](Validation/pivot-foot-slide-2026-10-10.md)，并保留之前将位移与动画时长分开，配置后摇取消／长按衔接与 Pivot 倍率，暂时移除体力系统；[本轮验证](Validation/dash-sprint-timing-2026-10-10.md)包括 31 项原生、40 项真实 PIE 场景及本轮可测行覆盖率。此前 v1.2 的结果仅见[历史实施报告](Validation/dash-sprint-implementation-2026-10-10.md)。
- [Dash 标签映射中断配置](Guides/dash-interrupt-configuration.md)：当前玩家映射、攻击分类、强制取消授权和 Death 豁免；Dash 不再依赖旧攻击后摇窗口。[本轮验证](Validation/dash-interrupt-2026-10-10.md)包含 34 项原生与 69 项单人／双客户端运行场景。
- [Dash 过渡修复](Validation/dash-transitions-2026-10-10.md)：v1.6 保持脚部贴地、对齐位移与衔接开放、恢复 RMS 交接与重放速度策略，并提前检查 Pivot 起手。
- [动画同步与 Pivot 恢复](Validation/animation-sync-pivot-2026-10-10.md)：v1.7 独立标记同步、恢复点混出、尾段再次掉头，以及预测动作朝向的迟到快照处理。

Design 并非全是未实现草案：各篇开头区分当前实现、历史设计和后续目标；历史示例不覆盖当前事实。

## 敌人 AI（v0.1 已实施）

- [玩家与 AI 共享战斗请求层](Design/combat-action-requests.md)：独立 Definition 请求、Combo 意图、请求身份／结果、预测和取消边界；当前行为树任务支持两种模式。
- [普通怪物 AI 与 Experience／GAS 集成设计](Design/enemy-ai-system.md)：多轮讨论整合后的 v0.1 基线，含 Lyra 对照、方案取舍、EnemyInit 生命周期、受检授予、行为树与 GAS 协同、首版验收及后续计划。
- [普通怪物配置与开发手册](Guides/enemy-ai-configuration.md)、[实施验证报告](Validation/enemy-ai-implementation-2026-10-11.md)：正式 Guardian、初始化／授予接口、行为树、实际测试命令及未验证范围。
- [敌人不攻击的目标键修复](Validation/enemy-attack-target-key-2026-10-11.md)：Actor 类型过滤导致 SelfActor 回退、目标键初始化与正式资产修复，三种 PIE 模式连续攻击回归。
- [敌人现状与早期方案评估](Design/enemy-ai-development-assessment.md)、[审计结果](Validation/enemy-ai-audit-results-2026-10-10.json)：保留 2026-10-10 的源码／资产事实；首版范围和初始化契约以整合设计为准。

## 玩家锁定（v0.4 软镜头修正）

- [玩家目标锁定系统](Design/lock-on-system.md)、[配置手册](Guides/lock-on-configuration.md)、[实施验证](Validation/lock-on-implementation-2026-10-12.md)：中键获取／解除、鼠标甩动切换、短 GA 与 TargetData、服务器确认与拒绝恢复、锁定镜头／标记、Dash／Sprint 暂让与恢复；常规双目标构建、29 项原生回归、四种 PIE 模式各 20 项通过。正式美术、手感调优及独立进程／打包仍待验证。
- [软锁定镜头调研与修正](Design/soft-lock-camera-research-2026-10-13.md)、[新验收](Validation/soft-lock-camera-2026-10-13.md)：取消八度限制／横向缓存，手动环绕、停手宽限与柔和构图；修正 CameraMode 栈顶诊断、技能优先及混合期间输入。31 项原生回归、四模式镜头专项各 24 项、原锁定回归各 20 项通过；真实物理设备和复杂地形仍待验证。
- [锁定标记 WBP 与 Dash 即时起手](Validation/lock-on-indicator-dash-start-2026-10-13.md)：Designer 可编辑标记、画布恢复计时，正式 Profile 即时朝向；33 项原生、四模式九方向各 36 项及单人原流程 20 项验证。

## 战斗扩展与后续设计

- [灭音与暗涌／共鸣回路技能系统](Design/havoc-dark-surge.md)、[首版实施验证](Validation/havoc-dark-surge-2026-10-15.md)：已接入暗流燃料、灭音、强化五段、破命共享E冷却、20%湮灭增伤和HUD；[暗流回满调试命令](Validation/havoc-debug-fill-2026-10-15.md)。E换图标、重击和变奏仍暂缓，完整限制以验证报告为准。
- [共鸣解放设计](Design/resonance-liberation.md)、[配置指南](Guides/resonance-liberation-configuration.md)、[实施验证](Validation/resonance-liberation-2026-10-15.md)：R键、充能与独立冷却、金刚体、固定范围命中和前半段电影镜头。
- [共鸣技能配置](Guides/resonance-skill-configuration.md)、[共鸣图标与Layout接入](Guides/resonance-ability-icons.md)：E技能及E／R冷却、能量环与Experience扩展点配置。
- [近战索敌与攻击吸附](Design/melee-attack-assist.md)：软索敌、手动锁定、起手转向、有限接近、真实命中边界、预测与空中追击衔接。

攻击吸附仍为设计草案，相关接口尚未创建；受击核心的当前字段见受击配置手册，削韧仍只预留数值。

## UI 迁移与学习

- [参考主 HUD 配置](Guides/main-hud-reference-configuration.md)、[实际验证](Validation/main-hud-reference-2026-10-11.md)：用户参考图存档、底部生命条、右上终焉入口与正式 UMG 资产；常规构建、冷启动单人／Listen Server 与实际布局截图。
- [当前任务与待办复核](KnowledgeBase/29-current-work-and-backlog.md)：HUD／AI／翅膀／锁定已完成范围、后续优先级、验收标准和用户暂缓事项。

- [UMG 可视化制作与父类精简方案](Design/umg-authoring-ui-refactor.md)：已授权实施：布局已迁入 Designer，六个业务原生控件类归档，采用共享数据源；[本轮回归结果](Validation/umg-authoring-refactor-2026-10-08.md)。
- [现行设计](Design/hodge-ui-foundation.md)、[配置指南](Guides/ui-foundation-configuration.md)、[实际验证](Validation/ui-foundation-2026-10-08.md)。

- [迁移计划](Design/lyra-ui-migration-plan.md)、[文件导览](Design/lyra-ui-file-guide.md)、[当前架构](Design/hodge-ui-architecture.md)。
- [UMG/Slate](Design/umg-slate-mental-model.md)、[UIExtension](Design/ui-extension-system.md)、[Indicator](Design/indicator-ui-system.md)。

基础 HUD、根布局、GameViewport 和菜单路由已接通；用户选择不引入 CommonGame／CommonUser／ModularGameplayActors。旧计划保留为历史，完整前端另行制作。

## 验证与维护

- [架构审查与分阶段重构方案](Design/architecture-refactor-review-2026-10-15.md)：本机Lyra对照、维护热点、目标依赖图、R1～R8问题和阶段0～5验收；仅完成评审，尚未实施结构重构。
- [项目开发准则](Development/development-standards.md)：中文注释、Lyra优先复用、单Runtime模块、生命周期、验证与提交约定。
- [检测体与多段攻击验证](Validation/skill-hit-volumes-2026-10-06.md)：构建、原生、单人、100ms 联机、原技能保护与测试入口边界。
- [跳跃衔接与死亡能力验证](Validation/jump-death-2026-10-06.md)：跳跃过渡修正；死亡事件/阶段已测试，互斥计数与 PIE 清理仍有错误。
- [39 个正式资产迁入 Main](Validation/main-assets-migration-2026-10-06.md)：来源、目标、引用修正、备份与本次验证。
- [实际验证范围](KnowledgeBase/16-validation.md)、[本轮状态](KnowledgeBase/30-update-2026-10-15.md)、[当前任务与待办](KnowledgeBase/29-current-work-and-backlog.md)。
- Validation 和 Design 中带日期的验收记录保留当时结果；不得据此宣称当前正式伤害、重生或打包全部通过。
- [开发流程](AI_DEVELOPMENT.md)、[开发约定](../AGENTS.md)、[评审标准](../CODE_REVIEW.md)。
- [旧长版 README](History/README-before-20261006.md)、[旧扫描快照](KnowledgeBase/History/snapshot-before-20261006.json) 保留历史。

- [锁定镜头 v0.5 获取与屏幕区域跟随验证](Validation/soft-lock-tracking-2026-10-13.md)：平滑获取、分轴软区、鼠标抑制／恢复、模式切换与 PIE 回归。
