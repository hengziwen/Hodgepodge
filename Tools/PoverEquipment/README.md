# PoverEquipment 导入工具

针对本次指定的 PMX 装备包；根目录、源文件夹和输出路径使用本机明确路径。不会改动源文件或自动提交 Git。

- `convert_pmx.py`：校验同名目标源文件内容，保留目录复制原文件，使用隔离 Blender/MMD Tools 转为四个同名 FBX，并记录骨骼、材质和源贴图映射。
- `import_and_repair.py`：在 UE 编辑器中导入贴图和骨骼网格，重建源材质并修复全部引用，只操作 `/Game/Wuwa/PoverEquipment`。
- `validate_import.py`：验证资产类型、原始骨骼、材质槽、贴图参数、透明设置、shader 统计和源文件 SHA-256。
- `write_report.py`：生成包含全部资产和全部修复引用的中文 Markdown、CSV 和 JSON。

转换需要 `Saved/PoverEquipmentImport/Runtime/python` 中的 `bpy==4.2.0`、`numpy==1.26.4`、`opencc-python-reimplemented==0.1.7`，以及 `Runtime/blender_mmd_tools-main` 中的 MMD Tools 4.5.14。依赖和缓存位于 Saved，没有改动项目插件。首次安装及分析产物也位于该目录。

导入脚本通过项目已有 `Tools/HitReaction/mcp_execute.py` 和本地 UE 网关执行。执行前保存其他工作、停止 PIE；脚本拒绝在其他资产或地图有未保存改动时导入。重新运行会重建本次导入材质的节点图，不应用于已由用户重新编辑的材质。

最终验证必须在独立命令行进程启用渲染，并设置 `HODGE_IMPORT_FRESH=1`；详见 `Docs/Validation/pover-equipment-import-2026-10-11.md` 的实际命令。普通无渲染 commandlet 无法读取有效材质 shader 统计。当前编辑器的全新包也不保证可被 ReloadPackages 直接重载。

原 PMX/贴图与 FBX 导入源保留在 Content 目标目录；压缩包只是原文件备份，不再导入其贴图副本。所有中间数据、日志、隔离依赖和截图留在 Saved。
