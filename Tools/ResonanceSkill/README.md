# 共鸣技能模板制作与验证

使用当前 UE 5.5.4。资产和调参入口见 [配置指南](../../Docs/Guides/resonance-skill-configuration.md)。

`configure_assets.py` 在编辑器无 PIE 时创建模板，补充 PawnData、输入配置和 IMC 的本技能条目。首次使用需确保 Config/Tags/ResonanceSkillTags.ini 已由冷启动加载。各新资产有版本元数据，成功后重跑保留调参；不要移除版本标识来强制覆盖用户资产。

制作工具包含早期错误 Key 文本的补偿分支：只修复本工具创建的无效 `(` 映射，正常用户改键保留。输入条目按实际 TagName 去重，不使用 Python Struct 对象身份比较标签。

运行前保存编辑器工作，按顺序执行：

```powershell
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/ResonanceSkill/configure_assets.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/ResonanceSkill/validate_assets.py
& E:/Python/python.exe Tools/ResonanceSkill/run_batch.py single listen dedicated listen-lag
```

`validate_assets.py` 只读检查资产、路由、引用、实际 E 键和冷却。蓝图 Status 是 Python 受保护字段，原生 MCP 的 blueprint.get_blueprint 用于核对 compileStatus／hasCompileErrors；不能仅以 Python Compile 返回完成代替诊断。

`run_e2e.py` 注入真实 Enhanced Input，检查拥有者／服务器唯一授予、实际动画与伤害、独立 ActiveGE、CD 内拒绝、8秒到期及 Dash 中断后保留 CD。查询效果按具体冷却 GE 类，避免误用只匹配 Effect Asset Tags 的查询判断 Granted Tags。

所有备份、日志和连续采样留在 Saved/ResonanceSkill。Dedicated 是本机 PIE 专服，未构建独立 Server。实际桌面键盘和手柄体验另行验收。

测试后停止 PIE，用现有 Tools/LockOn/restore_editor.py 显式传入绝对 LOCK_ON_WORK 恢复地图和性能。原始完整 Play Settings 未记录，恢复单人基线，不声称完全还原所有编辑器选项。
