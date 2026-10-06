# Hodgepodge 本地知识库

> 人工核对：2026-10-06。依据当前源码、uproject、Config、只读资产解析与已保存验收证据。本次只更新文档和参考元数据，没有编译 C++、执行 PIE 或修改资产。

## 从这里开始

项目已接入 Experience/Init State、PlayerState ASC、输入相机、Main 固定移动动画层、统一 CombatComponent、默认剑、旋转锁、连段记忆与武器显隐。正式普攻仍未填写 HitWindows；测试伤害链成立不等于正式五段已造成伤害。

- [最新完整更新](26-update-2026-10-06.md)：当前路径、职责、调参及验证边界。
- [当前接通状态](12-integration-backlog.md)、[验证与未验证](16-validation.md)。
- [项目 README](../../README.md)、[设计文档目录](../README.md)。

## 按系统阅读

- [项目地图](01-project-map.md)、[所有权](02-architecture.md)、[启动](03-runtime-startup.md)、[Pawn 初始化](04-pawn-initialization.md)。
- [数据资产](05-data-assets.md)、[输入](06-input.md)、[GAS 与连段](07-gas.md)、[命中伤害和装备](08-combat-health.md)。
- [相机/动画/旋转](09-camera-animation.md)、[GameFeature](10-game-features.md)、[复制和服务器](11-network.md)。
- [构建配置](13-build-config.md)、[排障](14-troubleshooting.md)、[开发步骤](15-development-recipes.md)。
- [维护术语](17-maintenance-glossary.md)、[FAQ](18-faq.md)、[评审材料](20-code-review.md)。
- [源码、Tag、资产、配置参考](Reference/README.md)、[扫描快照](Reference/snapshot.md)、[工具说明](tools/README.md)。

## 文档可信度与历史

当前状态以 01～18 章节、本轮更新和实际代码/资产为准；Reference 是静态索引。Design 中既有规划也有已实施方案，状态在每篇开头注明。历史评审和验证只说明当时的基线，不能当作当前缺陷清单或当前测试通过证据。

- [09-13](19-update-2026-09-13.md)、[09-17](21-update-2026-09-17.md)、[09-19](22-update-2026-09-19.md)、[09-22](23-update-2026-09-22.md)、[09-28](24-update-2026-09-28.md)、[09-29](25-update-2026-09-29.md)。
- [更新前快照](History/snapshot-before-20261006.json) 保留原日期/hash；当前 snapshot.json 由维护工具重新生成。
- [更新前长版 README](../History/README-before-20261006.md) 保留旧架构解说。

check → 核对变化 → 修改人工章节 → refresh → check。不要清理默认玩法仍引用的 CodexText 资产，也不要把本轮静态核对说成新的游戏回归。
