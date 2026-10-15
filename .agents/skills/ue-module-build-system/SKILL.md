---
name: ue-module-build-system
description: 处理 UE5.5.4 Build.cs、Target.cs、模块依赖、UHT/编译/链接错误及 Editor/Game 常规构建；保持项目已有模块和构建版本。
metadata:
  engine: 5.5.4
  source: quodsoler/unreal-engine-skills
  commit: f3742d7b688690810df369802b90430324e380b9
  upstream_skill: ue-module-build-system
  adaptation: source-guided-workflow
license: MIT
---

# UE Module & Build System — Hodgepodge UE 5.5.4

处理 `ue-module-build-system` 对应的任务，执行下面的本地流程。

1. 读取 [项目契约](../../ue55-policy.md) 和当前工程事实。
2. 读取 [模块与 UBT 构建的 5.5.4 流程](../../ue55-reference/build.md)，再查当前需求涉及的源码、Lyra 调用链和配置。
3. 用本机精确签名和实际资产支持范围形成改动，按项目要求验证并区分未执行项。

## 本条目保留的专题范围

Build.cs Anatomy、Target.cs、.uproject File、Creating a New Module、Creating a Plugin、Which Module Provides This Type、Resolving Build Errors。

## 来源与深度参考

- [上游原文](../../skill-sources/quodsoler/skills/ue-module-build-system/SKILL.md)：只在需要本专题细节时读取相关章节，其链接的 references/scripts/assets 均完整保留。
- 上游内容针对其他引擎/插件版本，代码块不直接执行；每次采用前按本地领域流程核对 5.5.4。
- 上游：`quodsoler/unreal-engine-skills@f3742d7b688690810df369802b90430324e380b9`，原路径 `skills/ue-module-build-system`。
