# Hodgepodge 本地知识库

> 人工核对日期：2026-09-17。对象：本仓库当前工作区，包含未提交的修改。此知识库是项目本地文档，不依赖在线服务。

## 从这里开始

Hodgepodge 是 UE 5.5 的个人动作 RPG 框架工程，正在将角色、GAS、输入和相机迁移到 Lyra 式组件协作架构。当前最重要的事情是完成玩家初始化与可操作闭环，而不是扩展开放世界或后端。

这套文档区分四类内容：

- **源码已实现**：有效 C++ 或配置存在，仍不代表已经经过本轮运行验证。
- **未接通 / 草稿**：调用入口、配置、挂载或实际函数体缺失；整段注释不算实现。
- **待编辑器验证**：二进制资产内部字段、蓝图逻辑、实际地图覆盖、运行效果。
- **建议 / 目标**：后续开发设计，不能当成已有功能。

本轮没有编译、启动 PIE、修改游戏代码或修改资产。自动索引是静态导航工具，不是 C++ 编译器或蓝图解析器。

## 阅读路线

### 第一次接触项目

1. [项目定位、目录与文档可信度](01-project-map.md)
2. [架构职责与对象所有权](02-architecture.md)
3. [从启动到生成玩家的调用链](03-runtime-startup.md)
4. [PawnExtension 与 Hero 初始化](04-pawn-initialization.md)
5. [当前状态、断点与接通顺序](12-integration-backlog.md)

### 按系统查询

- [数据资产与 AssetManager](05-data-assets.md)
- [Enhanced Input 与技能输入](06-input.md)
- [GAS、AbilitySet 与技能生命周期](07-gas.md)
- [属性、伤害、战斗与死亡](08-combat-health.md)
- [相机、移动与动画](09-camera-animation.md)
- [Experience 与 GameFeature 扩展](10-game-features.md)
- [网络、复制与 Dedicated Server](11-network.md)

### 动手开发与排查

- [本地环境、构建与配置](13-build-config.md)
- [按症状定位的排障手册](14-troubleshooting.md)
- [新增角色、技能、玩法的操作手册](15-development-recipes.md)
- [验收场景与调试观察点](16-validation.md)
- [术语、决策记录与维护规范](17-maintenance-glossary.md)
- [常见问题](18-faq.md)
- [2026-09-13 更新记录与验证边界](19-update-2026-09-13.md)
- [代码评审顺序与检查清单](20-code-review.md)
- [2026-09-17 更新记录与验证边界](21-update-2026-09-17.md)
- [项目 AI 开发流程](../AI_DEVELOPMENT.md)
- [项目开发约定](../../AGENTS.md)

### 精确查代码与资源

- [源码文件与有效定义索引](Reference/source-index.md)
- [源码子目录参考入口](Reference/README.md)
- [GameplayTag 注册索引](Reference/gameplay-tags.md)
- [Content/Main 资产清单](Reference/assets.md)
- [配置逐节索引](Reference/config.md)
- [插件描述与启用声明](Reference/plugins.md)
- [扫描快照与源码漂移报告](Reference/snapshot.md)
- [原始项目 README](../../README.md)
- [Lyra 学习参考](../../LYRA_LEARNING_GUIDE.md)
- [Lyra 运行参考](../../LYRA_RUNTIME_FLOW.md)
- [原始架构目标方案](<../../UE5 开放世界动作 RPG 架构方案 V2.md>)

## 最新状态提示

2026-09-17 本轮已核对：ASC 接入收敛到 HeroComponent→PawnExtension 单一入口（Hero 的 PossessedBy/OnRep_PlayerState 只调 Super）；CharacterBase Receiver 生命周期成对；PlayerState::SetPawnData 已在权威端授予 PawnData->AbilitySets（未记录句柄）；HeroComponent 的额外输入移除已实现并在 EndPlay 统一解绑；新增 AbilityTimeline / ComboSet / PlayTimeline 数据与 Task（有实现、无 C++ 调用点）与 PreloadPrimaryAssetBundles 同步预加载；相机资产换为 CM_ThirdPerson / CM_ThirdPerson_Death；新增 CodexText 独立实验模块与 Editor-only MCP 插件。以上均为源码/配置/磁盘核对，不代表编译、PIE 或联机通过。剩余重点：技能授予句柄与撤销、Cue 路径注册与预加载、死亡衔接、敌人 ASC、专服启动、Timeline 的 BP 接线。详见 [本次更新记录](21-update-2026-09-17.md)。

不要把 Lyra 参考文档里的 ShooterCore、FrontEnd 地图、CommonUI 等当成本项目现有资源。

## 本地使用和维护

用 VS Code、Rider 的 Markdown 预览或任意本地 Markdown 阅读器打开本页。文档采用相对链接，仓库移动后仍可阅读；源码链接优先指向文件，符号与行号另列供 IDE 搜索。

在仓库根目录运行：

```powershell
python Docs/KnowledgeBase/tools/kb.py check
python Docs/KnowledgeBase/tools/kb.py search "PawnData"
python Docs/KnowledgeBase/tools/kb.py refresh
```

`check` 检查文档链接和快照漂移；`search` 搜索本知识库；`refresh` 仅重建 Reference 自动索引和扫描快照，不会更新人工章节的结论。代码变化后必须人工复核对应章节，不能只刷新快照就宣布知识库已更新。

工具行为、退出码、扫描范围与解析限制见 [维护工具说明](tools/README.md)。
