---
name: ue-metahuman-creator
description: 核查 UE5.5.4 MetaHuman Creator 的外部服务、已有角色资产和插件依赖；不提供 5.8 内置 Creator 编辑
  API。
metadata:
  engine: 5.5.4
  source: kevinpbuckley/unreal-engine-skills
  commit: b22b11d0a46bac2d55eae8d3273d2ef22c08a95d
  upstream_skill: ue-metahuman-creator
  adaptation: source-guided-workflow
---

# ue-metahuman-creator — UE5.5.4 依赖核查

处理 `ue-metahuman-creator` 对应的任务，执行下面的本地流程。

1. 读取 [项目契约](../../ue55-policy.md) 和当前工程事实。
2. 读取 [MetaHuman 可用性与旧版流程的 5.5.4 流程](../../ue55-reference/metahuman.md)，再查当前需求涉及的源码、Lyra 调用链和配置。
3. 用本机精确签名和实际资产支持范围形成改动，按项目要求验证并区分未执行项。

## 本条目保留的专题范围

Mental model、Core workflow、Subsystem API map (BlueprintCallable, so also Python)、C++ pattern (editor module)、Python worked example (Epic's exposed surface only)、Cloud dependencies and offline behaviour、Optional content ("MetaHuman Creator Core Data")。

## 来源与深度参考

- [上游原文](../../skill-sources/kevinpbuckley/skills/metahuman/ue-metahuman-creator/SKILL.md)：只在需要本专题细节时读取相关章节，其链接的 references/scripts/assets 均完整保留。
- 上游内容针对其他引擎/插件版本，代码块不直接执行；每次采用前按本地领域流程核对 5.5.4。
- 上游：`kevinpbuckley/unreal-engine-skills@b22b11d0a46bac2d55eae8d3273d2ef22c08a95d`，原路径 `skills/metahuman/ue-metahuman-creator`。
- 可用性：依赖缺失或协议不同的能力已改成前置核查/本地替代流程；安装本条目不安装商业资产或新的引擎 API。
