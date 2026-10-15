# UE5.5 编辑器 MCP 操作：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 5.5.4 没有上游 ModelContextProtocol/AllToolsets/ToolsetRegistry，删除默认已连接的假设，不调用 list_toolsets/describe_toolset/call_tool。
2. 发现实际已加载的 hodge_unreal 或 hodge_blueprints。ChiR24 网关走 unreal 的 search→describe→execute，参数以返回 schema 为准。
3. 普通蓝图桥先 bridge_status 核对 Hodgepodge 项目；它限定普通 EventGraph，不能代表 AnimGraph/UMG/材质编辑能力。
4. 读取状态后串行修改目标资产，编译并核实保存；超时后先读回结果，避免重复写入。不以 Live Coding 代替常规 Editor/Game 构建。
5. 连接不可用时继续源码/离线排查并报告缺失工具或旧路径；不自动启用 Remote Execution、启动其他引擎、升级插件或编造工具响应。

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
