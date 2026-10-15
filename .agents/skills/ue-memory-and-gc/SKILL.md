---
name: ue-memory-and-gc
description: 选择 UE5.5.4 UObject 的 UPROPERTY/TObjectPtr、弱引用、软引用和 GC 保活方案，排查 GC 后崩溃及异步悬空引用。
metadata:
  engine: 5.5.4
  source: kevinpbuckley/unreal-engine-skills
  commit: b22b11d0a46bac2d55eae8d3273d2ef22c08a95d
  upstream_skill: ue-memory-and-gc
  adaptation: source-guided-workflow
---

# Memory & garbage collection — Hodgepodge UE 5.5.4

处理 `ue-memory-and-gc` 对应的任务，执行下面的本地流程。

1. 读取 [项目契约](../../ue55-policy.md) 和当前工程事实。
2. 读取 [对象生命周期与 GC的 5.5.4 流程](../../ue55-reference/gc.md)，再查当前需求涉及的源码、Lyra 调用链和配置。
3. 用本机精确签名和实际资产支持范围形成改动，按项目要求验证并区分未执行项。

## 本条目保留的专题范围

How UObject GC works、Pointer types — which to use when、The UObject-GC pattern — owned members、Validity checks、Holding UObjects from non-UObject classes、Creating, destroying, and the root set、Non-UObject memory — plain C++ smart pointers。

## 来源与深度参考

- [上游原文](../../skill-sources/kevinpbuckley/skills/core/ue-memory-and-gc/SKILL.md)：只在需要本专题细节时读取相关章节，其链接的 references/scripts/assets 均完整保留。
- 上游内容针对其他引擎/插件版本，代码块不直接执行；每次采用前按本地领域流程核对 5.5.4。
- 上游：`kevinpbuckley/unreal-engine-skills@b22b11d0a46bac2d55eae8d3273d2ef22c08a95d`，原路径 `skills/core/ue-memory-and-gc`。
