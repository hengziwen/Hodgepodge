---
name: ue-navigating-engine-source
description: 从本机 UE5.5.4 源码定位并核实函数签名、默认值、头文件、模块依赖及弃用标记；适用于 API 核查和版本适配。
metadata:
  engine: 5.5.4
  source: kevinpbuckley/unreal-engine-skills
  commit: b22b11d0a46bac2d55eae8d3273d2ef22c08a95d
  upstream_skill: ue-navigating-engine-source
  adaptation: source-guided-workflow
---

# Navigating the Unreal Engine source — Hodgepodge UE 5.5.4

处理 `ue-navigating-engine-source` 对应的任务，执行下面的本地流程。

1. 读取 [项目契约](../../ue55-policy.md) 和当前工程事实。
2. 读取 [引擎源码定位的 5.5.4 流程](../../ue55-reference/source.md)，再查当前需求涉及的源码、Lyra 调用链和配置。
3. 用本机精确签名和实际资产支持范围形成改动，按项目要求验证并区分未执行项。

## 本条目保留的专题范围

Source tree on this machine、Source tree organization、Module structure (Public / Private / Classes)、Naming prefixes as navigation hints、The *_API macro tells you the module、Repeatable "find X" workflow、Reading reflection specifiers、The *.generated.h contract、Verified examples (UE 5.8)。

## 来源与深度参考

- [上游原文](../../skill-sources/kevinpbuckley/skills/core/ue-navigating-engine-source/SKILL.md)：只在需要本专题细节时读取相关章节，其链接的 references/scripts/assets 均完整保留。
- 上游内容针对其他引擎/插件版本，代码块不直接执行；每次采用前按本地领域流程核对 5.5.4。
- 上游：`kevinpbuckley/unreal-engine-skills@b22b11d0a46bac2d55eae8d3273d2ef22c08a95d`，原路径 `skills/core/ue-navigating-engine-source`。
