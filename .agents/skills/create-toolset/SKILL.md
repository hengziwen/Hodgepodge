---
name: create-toolset
description: 为 UE5.5.4 扩展项目编辑器工具或 MCP 能力，复用 HodgeAbilityEditor 与现有桥接协议；不依赖 5.8 ToolsetRegistry。
metadata:
  engine: 5.5.4
  source: EpicGames/unreal-engine-skills-for-claude-code-plugin
  commit: a6aa73ada02a9fb1f5415bec491f9942c519b297
  upstream_skill: create-toolset
  adaptation: source-guided-workflow
license: MIT
---

# Create Toolset — Hodgepodge UE 5.5.4

处理 `create-toolset` 对应的任务，执行下面的本地流程。

1. 读取 [项目契约](../../ue55-policy.md) 和当前工程事实。
2. 读取 [UE5.5 编辑器工具扩展的 5.5.4 流程](../../ue55-reference/epic-toolset.md)，再查当前需求涉及的源码、Lyra 调用链和配置。
3. 用本机精确签名和实际资产支持范围形成改动，按项目要求验证并区分未执行项。

## 本条目保留的专题范围

Principles、Before You Write、Shared Conventions、Documentation、C++ Specifics、Python Specifics、Testing Your Work、Reviewing Your Work。

## 来源与深度参考

- [上游原文](../../skill-sources/epicgames/skills/create-toolset/SKILL.md)：只在需要本专题细节时读取相关章节，其链接的 references/scripts/assets 均完整保留。
- 上游内容针对其他引擎/插件版本，代码块不直接执行；每次采用前按本地领域流程核对 5.5.4。
- 上游：`EpicGames/unreal-engine-skills-for-claude-code-plugin@a6aa73ada02a9fb1f5415bec491f9942c519b297`，原路径 `skills/create-toolset`。
- 可用性：依赖缺失或协议不同的能力已改成前置核查/本地替代流程；安装本条目不安装商业资产或新的引擎 API。
