# Dash 离地续冲验证

使用 UE 5.5.4 和本机已构建的 HodgepodgeEditor。设计见 [Dash／Sprint v1.9](../../Docs/Design/dash-sprint-system.md)，结果见 [本轮报告](../../Docs/Validation/dash-air-continuation-2026-10-13.md)。

## 运行

编辑器冷启动且无未保存工作，在项目根目录执行：

```powershell
& E:/Python/python.exe Tools/DashAir/run_batch.py single listen dedicated listen-lag
& E:/Python/python.exe Tools/LockOn/run_validation_batch.py --kind lock --work E:/Project/Git/Hodgepodge/Saved/DashAirImplementation single listen dedicated listen-lag
& E:/Python/python.exe Tools/DashAir/export_results.py
```

`run_batch.py` 复用现有锁定测试启动器，在验证地图等待 Experience 和玩家就绪，读取实际逐项断言，再结束各 PIE 模式。网关凭据只由既有执行工具私下读取，不写入报告。

`run_e2e.py` 在各 PIE 世界建立相同的临时 Cube 碰撞平台，通过真实 Enhanced Input 发起 Dash，自然运动冲出平台；测试结束不保存地图或资产。仅“空中起手拒绝”用例主动设置 Falling，其他离地用例均依靠平台边缘的实际地面检测。

用例覆盖平地短按、位移早期／末期离地、窗口内落地衔接、落地过晚不补 Sprint、空中拒绝起手。采样包含拥有者／服务器的 GA、速度、移动模式与 Montage；模拟代理能力 Spec 不一定可见，按其位移和 Montage 验证表现。

Listen／Dedicated 使用两个玩家；Dedicated 是本机 PIE 专服，未构建独立 Server 可执行文件。`listen-lag` 施加 `Net PktLag=100`。

## 结束与报告

输出位于 `Saved/DashAirImplementation`，失败记录保留到 History。结束后先停止 PIE，再通过现有 `Tools/LockOn/restore_editor.py` 显式传入绝对 `LOCK_ON_WORK` 恢复地图和性能设置；原始完整 Play Settings 未记录，只恢复单人基线。

`export_results.py` 检查实际报告并生成仓库摘要。原始日志、采样、缓存和备份留在 Saved，不提交源码仓库。它不会启动新构建或新测试，构建退出码应与本轮真实命令记录一起核对。
