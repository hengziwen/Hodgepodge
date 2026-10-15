---
name: ue-game-features
description: 实现或排查 UE5.5.4 GameFeature、Experience、PawnData、组件注入和 Init State，包括加载、激活、停用及资源撤销。
metadata:
  engine: 5.5.4
  source: quodsoler/unreal-engine-skills
  commit: f3742d7b688690810df369802b90430324e380b9
  upstream_skill: ue-game-features
  adaptation: source-guided-workflow
license: MIT
---

# UE Game Features and Modular Gameplay — Hodgepodge UE 5.5.4

处理 `ue-game-features` 对应的任务，执行下面的本地流程。

1. 读取 [项目契约](../../ue55-policy.md) 和当前工程事实。
2. 读取 [GameFeature、Experience 与 Init State的 5.5.4 流程](../../ue55-reference/gamefeatures.md)，再查当前需求涉及的源码、Lyra 调用链和配置。
3. 用本机精确签名和实际资产支持范围形成改动，按项目要求验证并区分未执行项。

## 本条目保留的专题范围

Plugin Layout and Descriptor、Plugin States and the Subsystem、Writing a UGameFeatureAction、Engine-Provided Actions、Component Injection、Init State System、Modular Component Bases、Project Policies and Observers。

## 来源与深度参考

- [上游原文](../../skill-sources/quodsoler/skills/ue-game-features/SKILL.md)：只在需要本专题细节时读取相关章节，其链接的 references/scripts/assets 均完整保留。
- 上游内容针对其他引擎/插件版本，代码块不直接执行；每次采用前按本地领域流程核对 5.5.4。
- 上游：`quodsoler/unreal-engine-skills@f3742d7b688690810df369802b90430324e380b9`，原路径 `skills/ue-game-features`。
