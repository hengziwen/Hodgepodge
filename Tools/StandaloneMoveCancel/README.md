# 独立技能移动取消验证

针对 `AM_Resonance_Skill1`，使用真实 EnhancedInput E／移动／普攻输入，不删除冷却 GE、不修改资产。报告需求编号对应 `Docs/Design/definition-move-cancel.md`。

1. 正常关闭已保存的编辑器，按项目准则完成 Editor／Game 构建。
2. 用 UE 5.5.4 冷启动项目；确认无未保存内容及运行中的 PIE。
3. 将当前地图写入 `Saved/StandaloneMoveCancel/editor-before.json`，格式为 `{"map":"/Game/ThirdPerson/Maps/ThirdPersonMap"}`；地图应取实际值。
4. 执行 `E:/Python/python.exe Tools/StandaloneMoveCancel/run_batch.py single listen dedicated listen-lag`。
5. 通过现有 `Tools/LockOn/restore_editor.py` 恢复地图与性能设置，执行时传入 `LOCK_ON_WORK=E:/Project/Git/Hodgepodge/Saved/StandaloneMoveCancel`。

每种模式依次覆盖窗口前松键、低阈值输入、提前按住、窗口内再按、冷却中再次发动、清理以及五段普攻回归。联机模式额外为服务器 PIE Actor 添加不复制的输入禁止标签，构造真实 RPC 拒绝，并验证拥有者恢复、松键重试。`listen-lag` 使用单向 100ms 延迟模拟。

原生测试位于 `Source/Hodgepodge/Private/Tests/HodgeCombatActionRequestTests.cpp`；运行 `Hodge.ActionRequests` 和 `Hodge.Combo`。日志、逐帧状态、需求断言与历史失败报告留在 Saved；交付报告只导出实际通过的检查及摘要。

测试限定编辑器 PIE；没有独立服务器可执行文件、Cook／打包或人工手感评审的通过声明。限制标签只注入测试世界并配对移除，不在产品运行时添加测试开关。
