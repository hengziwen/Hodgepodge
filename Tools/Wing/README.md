# 残象之翼程序驱动与验证

本目录描述旧 PMX 程序化模式及其历史禁用验证。新 Anzhu 攻击限定表现使用 `Tools/WingAttack`；旧条目仍关闭。“无任何 WingComponent”断言只适用于此前完全禁用状态，不用于验证现在的新模式。

当前正式角色的翅膀已暂停启用。`disable_assets.py` 关闭客户端/服务器注入并移除翼尖通知，保留全部资源。`configure_assets.py` 默认拒绝运行，防止后续制作脚本误启用；重新制作需要明确设置 ENABLE_POVER_WING=True，并恢复 Experience 注入选项。

先完成 UE 5.5.4 常规 Editor/Game 构建，再使用项目已有 `Tools/HitReaction/mcp_execute.py` 在编辑器执行资源脚本。保存其他工作并停止 PIE，脚本拒绝覆盖其他未保存资产。

1. 本机 Python 运行 `create_sockets.py`：调用网关原生 `animation_physics.create_socket`，只补充缺失的三个翅膀检测挂点。
2. 网关运行 `configure_assets.py`：按真实五段连段表定位末段，创建独立 AnimBP、Profile、注入组件蓝图与命中通知。保留用户的 WingAttach。
3. `start_validation.py`：在已有 AI 验证地图启动 PIE；全局 WING_PLAYERS=2 启动两个玩家，WING_DEDICATED=True 使用 PIE 专服模式。
4. `run_e2e.py`：真实 Enhanced Input 驱动五段攻击，验证独立翼骨骼变形、各端运动、服务器取消和检测清理。
5. 单人 PIE 中 `test_damage.py`：在挥翼窗口暂停源蒙太奇，检查固定翼尖的服务器伤害与持续重叠去重。
6. 多人 PIE 中 `test_client_cancel_rejection.py`：确认客户端结束本地连段不能结束服务器实例。
7. 停止 PIE 后执行 `restore_validation.py`，恢复地图与性能选项。

网络取消使用已有 QueueAbilityAction，延迟到原生世界 Tick 执行；不要直接在编辑器脚本中停止服务器蒙太奇后据此判断 RPC 复制是否正确。

工具不会自动提交 Git，输出和备份位于 Saved/WingImplementation。完整配置说明位于 Docs/Guides/pover-wing-configuration.md。
