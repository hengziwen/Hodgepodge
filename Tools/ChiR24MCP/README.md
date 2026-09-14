# ChiR24 MCP：Hodgepodge 本地接入

2026-09-13 完成。UE 5.5.4 / Win64；保留旧 hodge_blueprints。

## 来源与本地修复

- 上游：https://github.com/ChiR24/Unreal_mcp
- 固定提交：80430fd2862bfb43a6b0b1993f57033e45a9b98b。
- 上游版本 0.5.30，安装的是本地修复版，不是原样下载即可编译的版本。
- `ue55-local.patch` 包含 9 个文件的修改：8 个 UE 5.5 编译兼容修复，以及显式 Blueprint compile-and-save 绕过保存节流的修复。
- Fab 下载器公开 API 和部分 TEDS API 在 UE 5.5 不可用，对应功能禁用/返回不支持；未验证全插件所有能力。
- 源码位于 `D:/Hodgepodge/Plugins/McpAutomationBridge`。MIT 许可见 UPSTREAM-LICENSE。

## 使用

项目配置 `D:/Hodgepodge/.codex/config.toml` 新增 `hodge_unreal`，旧 `hodge_blueprints` 保留。
在 Hodgepodge 项目任务中使用；当前已运行的 Codex 任务如果没有新工具，需要刷新 MCP 连接或重新打开项目任务。

先打开 Hodgepodge 编辑器。新服务由 `server.py` 将 stdio 请求转给 ChiR24 原生 HTTP MCP，直接使用其工具 schema 和结果，不另写蓝图实现。
原生地址 `http://127.0.0.1:3016/mcp`；WebSocket 端口 8116；旧桥接端口 55557。
每次连接从项目 `Saved/MCP/capability-token` 读取认证信息，不把令牌写进配置或日志。不要手工设置另一个 CapabilityToken 覆盖它。

调用顺序：`unreal` search → describe → execute。按 describe 的 tool/action/params 和 consent 要求执行。
批量修改蓝图结束时必须调用 compile，参数 `saveAfterCompile=true`，并检查 compiled/saved。
单个节点操作的自动保存仍可能被节流；不能仅凭单次操作返回成功就认定最终资产已写盘。

只读连接检查：

```powershell
& 'D:\Hodgepodge\Tools\UnrealMCP\.venv\Scripts\python.exe' -X utf8 'D:\Hodgepodge\Tools\ChiR24MCP\verify_connection.py'
```

验证脚本读取实际项目 Codex 配置，完成 MCP initialize、tools/list、search、describe、execute，读取 WBP_Character，期望输出 PASS 和 widget_count=86。

## 已执行验证

- 独立工程 `D:/CodexUE55Lab/Test/Test.uproject`，UE 5.5.4；插件 RunUAT BuildPlugin Win64 成功，退出码 0。
- C++ UProbeWidget + 原生 MCP 创建 Widget Blueprint、Canvas、Button、事件/函数节点及连线；编译通过。
- PIE：实际 Button.OnClicked 广播 → C++ Increment → BlueprintImplementableEvent OnCountChanged → 蓝图更新 tooltip；计数 0→1→2，断言通过。
- 重启最初暴露保存节流丢失最后连线的问题。修复显式 compile-and-save 强制保存后，重新打包，再次 PIE、退出、冷启动 PIE 均通过。
- HodgepodgeEditor Win64 Development 构建成功，退出码 0。命令：`Build.bat HodgepodgeEditor Win64 Development -Project=D:\Hodgepodge\Hodgepodge.uproject -WaitMutex -architecture=x64`。
- Hodgepodge 原生 MCP 确认项目路径和 L_MainMenu，读取 WBP_Character 的 86 个控件；实际 Codex stdio 配置检查通过。
- 新插件启用后正常退出、重启通过；旧桥接仍连接 Hodgepodge。
- 未修改 Hodgepodge 的 Source 或 Content。测试 CppProbe 仅在独立测试工程中。

## 已知问题与验证边界

Game 构建执行命令：`Build.bat Hodgepodge Win64 Development -Project=D:\Hodgepodge\Hodgepodge.uproject -WaitMutex -architecture=x64`。
结果失败：`Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp:53` 的 PostEditChangeProperty 定义缺少 WITH_EDITOR 保护，非编辑器目标没有对应声明。该源码未被本次修改；没有通过修改业务代码掩盖此基线问题。Cook、打包、联机和全功能回归未执行。

2026-09-11 14:34:37 旧 Hodgepodge 编辑器 PID 41928 在 QUIT_EDITOR 后崩溃，调用栈在 AnimationBlueprintEditor；CrashContext 标记 EditorExit / IsRequestingExit=true。新插件目录创建于 14:35:18，崩溃进程尚未加载新插件。不能据此认定已修复动画编辑器内部根因。
恢复后禁用新插件的启动/退出通过，随后启用新插件的启动/退出/重启也通过。后续退出前检查未保存资产，并先关闭打开的资产编辑窗口；如再次复现需另查动画编辑器退出生命周期。

## 证据与回退

测试与构建日志在 `D:/CodexUE55Lab`：`PIE-restart-result.txt`、`Hodge-Editor-build.log`、`Hodge-Game-build.log`、`Hodge-exit-crash.log`、`Hodge-baseline.log`、`Hodge-integrated.log`、`Hodge-integrated-restart.log`。
接入前配置备份：`D:/CodexUE55Lab/HodgeBackup`。不要直接覆盖后续用户修改；回退时关闭编辑器，将 uproject 的 McpAutomationBridge Enabled 改为 false，并禁用/移除新增 hodge_unreal 配置。旧桥接未移除。

Codex 项目级配置依据：[OpenAI Docs MCP 配置](https://learn.chatgpt.com/docs/extend/mcp?surface=cli)。
