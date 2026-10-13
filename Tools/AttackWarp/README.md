# 攻击前摇旋转 Warp 验证工具

使用当前项目 UE 5.5.4，设计见 `Docs/Design/attack-facing-motion-warp.md`，结果见对应 Validation 报告。

## 前提

常规构建成功并冷启动编辑器；打开 `/Game/Main/Maps/L_EnemyAIValidation`，Experience 和正式玩家配置可用。启动测试前保存自己的工作，不关闭或覆盖未保存资产。

`mcp_execute.py` 使用项目已配置的本机编辑器网关，读取本地能力凭据，脚本不打印或存储凭据。脚本成功只表示进入编辑器，必须读取结果 JSON 的实际断言。

## 执行

在项目根目录用本机 Python 执行：

```powershell
& E:/Python/python.exe Tools/AttackWarp/run_batch.py single listen dedicated listen-lag
$env:HODGE_WARP_RUNNER='combo'
& E:/Python/python.exe Tools/AttackWarp/run_batch.py single listen dedicated listen-lag
Remove-Item Env:HODGE_WARP_RUNNER
& E:/Python/python.exe Tools/LockOn/run_validation_batch.py --kind lock --work E:/Project/Git/Hodgepodge/Saved/AttackWarp single listen dedicated listen-lag
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/AttackWarp/inspect_configuration.py
```

`run_e2e.py` 注入真实 Enhanced Input，记录 Montage、角色 Yaw、驱动模式和各网络副本。覆盖镜头方向、移动输入、锁定、角度限幅、0.5／2 倍率、后摇释放、Dash 与移动取消。网络模式另验证取消拒绝后的恢复；拒绝结果由 Editor 专用探针注入，恢复位置取实际权威 Montage。

`run_combo_regression.py` 持续注入攻击输入，验证五段真实蒙太奇均出现。两份测试只改变 PIE 副本，测试用倍率改在动画实例上，不写正式 Definition。

`run_batch.py` 启动各 PIE 模式，等待完整 JSON，校验每个断言，失败停止。旧报告移入 `Saved/AttackWarp/History`。此处 Dedicated 是 PIE 专服，未构建独立 Server 可执行文件。

`configure_assets.py` 是资产制作工具，会为 DA_Attack_1 添加或更新源时间 0～0.1 秒的原生 Warp 通知，并保存指定 Definition／Montage。它不是只读测试；已有配置时通常无需重跑。

`inspect_configuration.py` 是只读检查，输出 `configuration-final.json`。

## 结束与恢复

停止 PIE 后再调用现有 `Tools/LockOn/restore_editor.py`，显式传入绝对 `LOCK_ON_WORK`，恢复启动测试前记录的地图与性能设置。原始 PIE 选项没有完整快照，因此恢复单人基线，不能声称完全还原所有 Play Settings。验证日志与采样留在 Saved，不提交源码仓库。
