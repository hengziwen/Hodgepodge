# UI 基础闭环回归工具

仅在 UE 5.5.4 编辑器 Python／MCP 中执行；需要已经编译的 HodgeAbilityEditor。脚本在 Slate 后续帧等待真实游戏运行，不用 RPC 回调里的 Python 直接激活 GAS。不会重写角色、蒙太奇或战斗配置。

## 入口与顺序

- `start_single.py`：正式 ThirdPersonMap 的单人 PIE。执行之前必须没有 PIE。
- `test_single.py`：12 个阶段，检查真实属性、等级变化、菜单／Modal、HUD 卸载／再激活、根布局重建、异步取消／失败。需冷启动编辑器且未提前加载菜单 Blueprint，以实际覆盖未完成加载的取消；已加载菜单是同步完成请求，无法再按请求 ID 取消。
- `test_network.py`：先用 `Tools/AnimNotifyCombat/start_network.py` 启动两人 Listen PIE，再执行，检查服务器玩家／客户端 UI 与输入隔离。
- `test_dedicated.py`：先用 `Tools/AnimNotifyCombat/start_dedicated.py` 启动两客户端 PIE，再执行同样隔离检查；服务器世界没有本地 UI。不代表支持独立 Server Target。
- `test_menu_input.py`：无活动技能时执行；向 EnhancedInput 注入攻击与移动，验证菜单期间均被阻止、退出无缓存重放。
- `test_death_respawn.py`：服务器施加致死 GE，观察死亡／Pawn 销毁，测试工具调用原生 RestartPlayer，等待新 Pawn 的复制数值稳定 2 秒，再观察血条与新伤害；过程中的重绑／属性变化也会记入报告。只验证 UI 重绑，不实现自动重生玩法。
- `test_owned_modal.py`：从生产菜单打开确认页面，停用 HUD 后检查只撤销该菜单拥有的 Modal，再激活不残留输入阻断。
- 停止 PIE 使用 `Tools/AnimNotifyCombat/stop_pie.py`；停止完成后才再次启动其他模式。

报告保存在 `Saved/UIFoundation`；只在 JSON `error=null` 时计为通过。原生测试使用 `Automation RunTests Hodge.UI`，全部原生回归使用 `Automation RunTests Hodge`。100ms 战斗回归复用 `Tools/AnimNotifyCombat/test_client_combo_windows.py` 与 `test_client_move_cancel.py`。

Esc 验证通过游戏原生视口，测试调用期间暂时绕过编辑器 StopPlaySession；不会更改用户快捷键、运行时配置或插件。InspectPlayerUI 读取实际根布局／Extension Entry，而不把对象池中旧控件当作活动 HUD。

当前正式界面使用 Designer 树和 Graph，Inspect 同时读取实际文字／进度和共享数据源。确认框测试通过真实 CommonButton 的点击处理进入 Blueprint 绑定，六个原生业务控件类型已归档。

生产配置参阅 [UI 基础配置指南](../../Docs/Guides/ui-foundation-configuration.md)，实际结果见 [2026-10-08 验证](../../Docs/Validation/ui-foundation-2026-10-08.md)。
