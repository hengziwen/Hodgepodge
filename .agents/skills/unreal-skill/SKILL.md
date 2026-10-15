---
name: unreal-skill
description: 在 UE5.5.4 项目创建、修改或检查文件型 Agent Skill 工作流；使用 SKILL.md，不创建不存在的 UAgentSkill
  资产。
metadata:
  engine: 5.5.4
  source: EpicGames/unreal-engine-skills-for-claude-code-plugin
  commit: a6aa73ada02a9fb1f5415bec491f9942c519b297
  upstream_skill: unreal-skill
  adaptation: source-guided-workflow
license: MIT
---

# Unreal Skill — Hodgepodge UE 5.5.4

处理 `unreal-skill` 对应的任务，执行下面的本地流程。

1. 读取 [项目契约](../../ue55-policy.md) 和当前工程事实。
2. 读取 [UE5.5 项目工作流 Skill 编写的 5.5.4 流程](../../ue55-reference/epic-skill.md)，再查当前需求涉及的源码、Lyra 调用链和配置。
3. 用本机精确签名和实际资产支持范围形成改动，按项目要求验证并区分未执行项。

## 本条目保留的专题范围

Principles、Before You Write、Structure、Python Skills、UAsset Skills、Reviewing Your Work。

## 来源与深度参考

- [上游原文](../../skill-sources/epicgames/skills/unreal-skill/SKILL.md)：只在需要本专题细节时读取相关章节，其链接的 references/scripts/assets 均完整保留。
- 上游内容针对其他引擎/插件版本，代码块不直接执行；每次采用前按本地领域流程核对 5.5.4。
- 上游：`EpicGames/unreal-engine-skills-for-claude-code-plugin@a6aa73ada02a9fb1f5415bec491f9942c519b297`，原路径 `skills/unreal-skill`。
- 可用性：依赖缺失或协议不同的能力已改成前置核查/本地替代流程；安装本条目不安装商业资产或新的引擎 API。
