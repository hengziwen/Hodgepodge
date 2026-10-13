# Anzhu 翅膀导入工具

只处理 `Content/Wuwa/Wing`，不启用玩家翅膀玩法，也不覆盖已有角色骨架。

1. `inspect_sources.py`、`inspect_uv.py`、`inspect_animation_channels.py`：用已有隔离 Blender 运行时读取 FBX 元数据、UV 与动画曲线，不转换源文件。
2. `copy_sources.py`：保留目录、文件名并验证 SHA256。
3. 经 `Tools/HitReaction/mcp_execute.py` 执行 `import_assets.py`：模型和动画统一使用 0.01 导入缩放，翅膀/镰刀分别绑定骨架。
4. 同样执行 `configure_materials.py`、`validate_assets.py`、`finalize_assets.py`：连接贴图、配置材质、校验并保存新资产。
5. 使用启用 `-AllowCommandletRendering` 的 UE Python commandlet 再执行校验，`HODGE_WING_FRESH=1` 写入磁盘重载报告。
6. `open_preview.py`、`open_animation.py` 打开原生资产编辑器；`screenshot_preview.py [filename]` 捕获编辑器窗口。
7. `check_gameplay_state.py` 只读核对旧翅膀仍禁用；`write_report.py` 将实际结果生成到 `Docs/Validation`。

中间文件和日志位于 `Saved/WingImport`。正式资产清单、材质参数和验证命令见 [导入记录](../../Docs/Validation/anzhu-wing-import-2026-10-12.md)。

重跑导入只复用已存在资产，不自动重导入或覆盖用户后来编辑的网格。重建材质会重建本工具创建的父材质与实例，用户修改后不要无条件重跑该步骤。
