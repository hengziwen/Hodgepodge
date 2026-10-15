# 共鸣解放实现工具

配置、要求与运行结论见`Docs/Guides/resonance-liberation-configuration.md`及`Docs/Validation/resonance-liberation-2026-10-15.md`。所有脚本只使用已有本地MCP网关，认证文件留在Saved，不入库。

- 装配：`E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/CinematicUltimate/configure_assets.py`。
- 配置检查：同一网关运行`validate_assets.py`；蓝图编译可能使本任务包Dirty，脚本仅保存限定目标包。
- E2E：`E:/Python/python.exe Tools/CinematicUltimate/run_batch.py single listen dedicated listen-lag`。
- 回满调试命令验证：`E:/Python/python.exe Tools/CinematicUltimate/run_batch.py --debug single listen dedicated`；使用实际控制台与RPC，报告独立放在Saved/ResonanceDebug。
- 恢复：PIE停止后，网关运行`restore_editor.py`；恢复实际原地图和性能设置，PIE参数恢复单人基线，因为最初未记录所有Play设置。

运行条件：编辑器无未保存内容，验证地图与正式玩家资产存在；原生反射修改应先保存、关闭编辑器常规构建并冷启动。测试通过真实输入释放，不把服务器直接激活GA当作预测测试。

测试为了重复释放，在服务器夹具中使用正式增量GE补充资源，仍等待真实12秒CD；暂停／变速使用仅PIE、带执行身份校验的源实例探针。夹具不保存地图、改变生产数值或制造第二套玩法状态机。

报告保存在`Saved/CinematicUltimate`；已有失败报告带history时间戳保留。通过结论以报告断言为准，脚本启动成功不等于玩法通过。专服模式为PIE Dedicated，项目没有独立Server Target。
