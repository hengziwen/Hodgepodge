---
name: ue-sequencer-cinematics
description: UE5.5.4：UE Sequencer and Cinematics。用于对应领域的开发与问题排查。
metadata:
  engine: 5.5.4
  source: quodsoler/unreal-engine-skills
  commit: f3742d7b688690810df369802b90430324e380b9
  upstream_skill: ue-sequencer-cinematics
  adaptation: source-guided-workflow
license: MIT
---

# UE Sequencer and Cinematics — Hodgepodge UE 5.5.4

处理 `ue-sequencer-cinematics` 对应的任务，执行下面的本地流程。

1. 读取 [项目契约](../../ue55-policy.md) 和当前工程事实。
2. 读取 [Sequencer 与渲染队列的 5.5.4 流程](../../ue55-reference/sequencer.md)，再查当前需求涉及的源码、Lyra 调用链和配置。
3. 用本机精确签名和实际资产支持范围形成改动，按项目要求验证并区分未执行项。

## 本条目保留的专题范围

Build.cs Modules、Runtime vs Editor、Playing a Level Sequence、Binding Actors at Runtime、Sequencer Event Tracks、MovieScene Data Model、Cine Cameras and Camera Rigs、Movie Render Graph。

## 来源与深度参考

- [上游原文](../../skill-sources/quodsoler/skills/ue-sequencer-cinematics/SKILL.md)：只在需要本专题细节时读取相关章节，其链接的 references/scripts/assets 均完整保留。
- 上游内容针对其他引擎/插件版本，代码块不直接执行；每次采用前按本地领域流程核对 5.5.4。
- 上游：`quodsoler/unreal-engine-skills@f3742d7b688690810df369802b90430324e380b9`，原路径 `skills/ue-sequencer-cinematics`。
