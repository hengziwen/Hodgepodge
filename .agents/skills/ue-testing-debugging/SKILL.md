---
name: ue-testing-debugging
description: 为 UE5.5.4 变更选择自动化测试、日志与调试方法，执行实际验证并区分 C++、蓝图、PIE、联机和打包结果。
metadata:
  engine: 5.5.4
  source: quodsoler/unreal-engine-skills
  commit: f3742d7b688690810df369802b90430324e380b9
  upstream_skill: ue-testing-debugging
  adaptation: source-guided-workflow
license: MIT
---

# UE Testing, Logging & Profiling — Hodgepodge UE 5.5.4

处理 `ue-testing-debugging` 对应的任务，执行下面的本地流程。

1. 读取 [项目契约](../../ue55-policy.md) 和当前工程事实。
2. 读取 [自动化、调试和验证的 5.5.4 流程](../../ue55-reference/testing.md)，再查当前需求涉及的源码、Lyra 调用链和配置。
3. 用本机精确签名和实际资产支持范围形成改动，按项目要求验证并区分未执行项。

## 本条目保留的专题范围

Logging、Assertions、Automation Tests、CQTest、Functional Tests、Profiling、Debug Visualization、Console Commands and CVars。

## 来源与深度参考

- [上游原文](../../skill-sources/quodsoler/skills/ue-testing-debugging/SKILL.md)：只在需要本专题细节时读取相关章节，其链接的 references/scripts/assets 均完整保留。
- 上游内容针对其他引擎/插件版本，代码块不直接执行；每次采用前按本地领域流程核对 5.5.4。
- 上游：`quodsoler/unreal-engine-skills@f3742d7b688690810df369802b90430324e380b9`，原路径 `skills/ue-testing-debugging`。
