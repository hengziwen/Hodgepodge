# UE5.5 编辑器工具扩展：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 保留 Epic 的职责清晰、接口可组合、读写对称和错误显式原则；5.5 不使用 ToolsetRegistry/ToolsetDefinition/tool_call 装饰器。
2. 先查现有 AuthoringLibrary 与桥接 schema，已有接口直接复用；缺失的项目工具优先放 HodgeAbilityEditor 的独立类。
3. 工具使用真实 UE5.5 UFUNCTION/Python 暴露接口；若任务确需网关扩展，遵循现有 server.py/插件注册协议，不假装两个 MCP 协议兼容。
4. 新 C++ 工具按常规 Editor 构建验证，Runtime 依赖变化补 Game；测试覆盖资源不存在、非法参数、重复调用和保存结果。
5. 不把 skill 安装变成对第三方 McpAutomationBridge 的自动重构；后续确需修改时明确任务关系及验证办法。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Editor/UnrealEd/Public/Subsystems/EditorAssetSubsystem.h:277`：`SaveLoadedAsset`。

## 本地桥接边界

- `Tools/ChiR24MCP/server.py`：stdio 转本机 HTTP MCP，工具入口为 `unreal`，流程为 search→describe→execute。
- `Tools/UnrealMCP/server.py`：普通蓝图 EventGraph 桥；`bridge_status` 核验项目身份。
- `Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.h`、`HodgeAnimationAuthoringLibrary.h`：检查已有项目编辑接口。
- `.codex/config.toml` 已绑定当前 E 盘工作区与 `Saved/UESkillsRuntime`，HODGE_PROJECT 限制蓝图桥身份，NO_PROXY 仅绕过回环代理。真实握手、工具列表和蓝图桥项目身份已核实；环境重建步骤见 [安装记录](../UE55_SKILLS.md)。
- 不执行上游 ModelContextProtocol.StartServer，不依赖 UAgentSkill 或 ToolsetDefinition，不自动开启 Python Remote Execution。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
