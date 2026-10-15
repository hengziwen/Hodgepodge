---
name: ue-mass-entity
description: UE5.5.4：UE Mass Entity。用于对应领域的开发与问题排查。
metadata:
  engine: 5.5.4
  source: quodsoler/unreal-engine-skills
  commit: f3742d7b688690810df369802b90430324e380b9
  upstream_skill: ue-mass-entity
  adaptation: source-guided-workflow
license: MIT
---

# UE Mass Entity — Hodgepodge UE 5.5.4

处理 `ue-mass-entity` 对应的任务，执行下面的本地流程。

1. 读取 [项目契约](../../ue55-policy.md) 和当前工程事实。
2. 读取 [Mass Entity的 5.5.4 流程](../../ue55-reference/mass.md)，再查当前需求涉及的源码、Lyra 调用链和配置。
3. 用本机精确签名和实际资产支持范围形成改动，按项目要求验证并区分未执行项。

## 本条目保留的专题范围

Modules and Build.cs、Fragments, Tags and Archetypes、FMassEntityManager、UMassProcessor、FMassEntityQuery、Deferred Commands、UMassObserverProcessor、Single-Entity Access and FEntityBuilder、Relations and Archetype Groups、Traits, Config Assets and Spawning、Representation and LOD、Crowd, Navigation and Behavior。

## 来源与深度参考

- [上游原文](../../skill-sources/quodsoler/skills/ue-mass-entity/SKILL.md)：只在需要本专题细节时读取相关章节，其链接的 references/scripts/assets 均完整保留。
- 上游内容针对其他引擎/插件版本，代码块不直接执行；每次采用前按本地领域流程核对 5.5.4。
- 上游：`quodsoler/unreal-engine-skills@f3742d7b688690810df369802b90430324e380b9`，原路径 `skills/ue-mass-entity`。
