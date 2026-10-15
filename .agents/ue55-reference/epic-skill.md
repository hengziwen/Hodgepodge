# UE5.5 项目工作流 Skill 编写：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 保留 Epic 的简洁、明确触发范围、可复用工作流原则；本机没有 UAgentSkill/FAgentSkillDetails/AgentSkillToolset。
2. 在仓库 .agents/skills/<name>/SKILL.md 编写标准 Agent Skill，声明 name/description 并链接领域参考；不创建 /Game/Skills 的不存在资产类型。
3. 读取已有 skill 避免同职责重复，项目长期规范继续由 AGENTS.md 与开发准则维护，skill 只描述特定操作流程。
4. 源码/配置以 5.5.4 为准；校验 frontmatter、命名、链接及真实能力边界，不能用安装成功证明编辑器注册成功。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- 本领域不依赖 5.8 官方引擎设施；使用下面的项目桥接/标准 skill 文件路径。

## 本地桥接边界

- `Tools/ChiR24MCP/server.py`：stdio 转本机 HTTP MCP，工具入口为 `unreal`，流程为 search→describe→execute。
- `Tools/UnrealMCP/server.py`：普通蓝图 EventGraph 桥；`bridge_status` 核验项目身份。
- `Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.h`、`HodgeAnimationAuthoringLibrary.h`：检查已有项目编辑接口。
- `.codex/config.toml` 已绑定当前 E 盘工作区与 `Saved/UESkillsRuntime`，HODGE_PROJECT 限制蓝图桥身份，NO_PROXY 仅绕过回环代理。真实握手、工具列表和蓝图桥项目身份已核实；环境重建步骤见 [安装记录](../UE55_SKILLS.md)。
- 不执行上游 ModelContextProtocol.StartServer，不依赖 UAgentSkill 或 ToolsetDefinition，不自动开启 Python Remote Execution。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
