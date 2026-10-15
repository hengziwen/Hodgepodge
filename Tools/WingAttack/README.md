# 翅膀通知蒙太奇制作与验证

现行入口见 [配置指南](../../Docs/Guides/wing-notify-montage-configuration.md)和 [设计](../../Docs/Design/attack-wing-presentation.md)。不再配置 AnimationBindings；不新增 Wing GA。

1. 保存并关闭编辑器，按项目准则常规构建 Editor 和 Game；冷启动编辑器。
2. `E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingAttack/configure_montages.py`：创建缺失的翼 AnimBP/蒙太奇，迁移当前通知，保留用户时间、挂接和命中配置。旧 configure_assets.py 禁止重建旧映射。
3. 同一网关执行 `validate_montages.py`：只读检查真实引用、骨架、时间与挂接；validate_assets.py 转到相同入口。
4. `E:/Python/python.exe Tools/WingAttack/run_montage_batch.py single listen dedicated`：依次运行表现与翼 Box 伤害，结果保存在 Saved/WingMontage。
5. 每次 PIE 结束调用 stop_test.py，全部完成后按记录恢复原地图与性能选项，不保存运行时地图。

本轮测试使用真实攻击输入，每段仅在连段许可窗口请求一次，避免高频输入不断替换客户端预测请求。表现检查真实翼蒙太奇实例、骨骼、时钟、淡出和 Dash 取消。伤害检查翼盒单独伤害、去重和范围外拒绝；其取消项通过原生游戏 Tick 队列发起服务器技能取消，以隔离伤害清理，不把 Dash 的地面激活条件混入 Box 验证。

`WING_SCENARIOS=presentation` 或 `damage` 可选择专项。必须检查报告 passed 和每项断言，脚本接入成功不等同于验收通过。

`configure_fade.py` 与已有材质副本继续复用。`configure_box.py` 在动作、缩放或窗口改变后重新校准；校准读取通知中的翼蒙太奇，支持翼单独攻击或与手部共用阶段。常规迁移不重新测量或覆盖用户盒体。

已有 validate_box.py/write_report.py/write_fade_report.py/write_box_report.py 属于 2026-10-12 历史流程，依赖当时快照；本轮结果见通知蒙太奇验证报告。导入模型、源材质、旧程序化资产及旧 Experience 禁用条目保持原配置。
