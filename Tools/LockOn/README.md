# 锁定系统制作与验证工具

这些脚本通过本机 UE 编辑器 gateway 执行，不是运行时游戏逻辑。配置与验收见 [配置手册](../../Docs/Guides/lock-on-configuration.md) 和 [验证报告](../../Docs/Validation/lock-on-implementation-2026-10-12.md)。

## 使用前提

UE 5.5.4 编辑器打开本项目、gateway 可用、停止 PIE、保存所有资产。执行器读取本机 capability 文件，不把令牌写入日志。`Saved/LockOn` 保存备份、结果和截图，不提交日志或缓存。

```powershell
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/LockOn/validate_assets.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/LockOn/start_test.py
```

默认 `single`。其他模式在单独脚本中设置 `LOCK_ON_MODE='listen'`、`'dedicated'` 或 `'listen-lag'`，再用 `exec` 执行 `start_test.py`，不可在已有 PIE 中再次启动。`dedicated` 是专服 PIE，不是独立 Server Target；`listen-lag` 给各 PIE 网络驱动设置 `Net PktLag=100`。

启动脚本先等待玩家 Experience／PawnData／ASC／LockOn 就绪，再自动执行 `run_e2e.py`。服务端只暂停 PIE 中的敌人决策并摆放测试角色，不更改正式行为树或怪物蓝图。结果 `<mode>-e2e.json` 必须 `passed=true` 且 20 条检查均通过。测试超时和异常写入结果，不能把 gateway 接受脚本当成玩法通过。

中键测试经 `PlayerController::InputKey` 排入下一游戏帧，覆盖真实 IMC、Started 绑定与 Hero→GA 路径；鼠标移动和 Dash／Sprint 使用 EnhancedInput 注入。此方式不验证 Windows 物理鼠标按键、DPI 或人工手感。`<mode>-hud.png` 来自所属玩家 Slate 根窗口；普通 viewport 截图可能捕获服务器或编辑器窗口，不用作 HUD 证据。

## 停止与恢复

```powershell
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/LockOn/stop_test.py
# 等待 PIE 实际停止，再执行恢复。
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/LockOn/restore_editor.py
```

恢复脚本拒绝覆盖未保存包，并恢复首次记录的地图、性能设置与历史截图。最初没有记录完整 PIE 选项，因此恢复为单人独立模式，不能声称完全还原所有原始播放设置。测试网络驱动随 PIE 结束销毁。

## 资产制作与调试

- `configure_assets.py`：创建缺失资产、追加本功能的 AbilitySet／输入／Profile／HUD 层，修改前备份；不要用它覆盖已人工调好的 Profile。
- `configure_enemy_anchors.py`：只在未配置时选择已知真实胸口骨骼，保存 Guardian 的锚点；不修理武器插槽或行走动画。
- `repair_key_mapping.py`：只修复本功能键映射；正确 FKey 文本是 `MiddleMouseButton`。
- `validate_assets.py`：核对 10 项正式资产引用、唯一授予／输入映射、标记和骨骼锚点。
- `configure_soft_camera.py`：备份正式 Profile 后只配置本轮软镜头，不重复授予能力或制作 HUD。
- `run_camera_e2e.py`：核对真实 CameraMode 栈／POV、环绕、停手宽限、角色面向、技能覆盖／恢复和解除连续性。
- `export_results.py`：本机 Python 直接运行，校验实际结果后导出轻量 JSON 与 SHA256；固定本轮报告日期和检查数量，扩展用例时应同步更新。
- `Log LogTemp Verbose`：开启候选拒绝原因与评分日志；完成诊断后 `Log LogTemp Log` 恢复正常输出。
- `Hodge.LockOn`：14 个原生测试；综合回归使用 `Hodge.LockOn+Hodge.Facing+Hodge.Rotation+Hodge.Movement+Hodge.UI`。

脚本路径、测试坐标、动作幅值与 100ms 延迟是验证夹具配置，不写入生产战斗逻辑。换英雄或地图时先调整夹具资源，再独立验收。

## 历史 v0.4 软镜头专项

在编辑器执行包装脚本，通过 `LOCK_ON_RUNNER` 指定 `run_camera_e2e.py`，通过 `LOCK_ON_WORK` 指定独立证据目录，模式仍用 `LOCK_ON_MODE`。所有输出应分别检查 `passed` 和逐项断言，不能只看脚本启动成功。

```python
from pathlib import Path
exec(Path('E:/Project/Git/Hodgepodge/Tools/LockOn/start_test.py').read_text(encoding='utf-8-sig'), {
    'LOCK_ON_MODE': 'listen',
    'LOCK_ON_WORK': 'E:/Project/Git/Hodgepodge/Saved/LockOnCamera',
    'LOCK_ON_RUNNER': 'E:/Project/Git/Hodgepodge/Tools/LockOn/run_camera_e2e.py',
})
```

专项的 `CameraOverride` 编辑器动作在 PIE 本地 Hero 上调用实际 SetAbilityCameraMode／ClearAbilityCameraMode，以已授予命令 Spec 标识验证归属；它验证相机接口链，不是制作了一份正式技能演出 GA。最终恢复也需给 restore_editor.py 传入相同 LOCK_ON_WORK，避免拿历史测试的地图／设置恢复本轮环境。

## v0.5 获取与屏幕区域跟随

当前验收入口为 `run_tracking_e2e.py`，每种模式 22 项断言。中键仍走 InputKey／IMC，鼠标和移动／Dash 走 Enhanced Input。整圈环绕与敌人横移使用受控 PIE 位置轨迹，另有实际移动／Dash/Sprint 集成检查；不是全部地形与动作的物理测试。

```powershell
& E:/Python/python.exe Tools/LockOn/run_validation_batch.py --kind tracking --work E:/Project/Git/Hodgepodge/Saved/LockOnTracking
& E:/Python/python.exe Tools/LockOn/run_validation_batch.py --kind lock --work E:/Project/Git/Hodgepodge/Saved/LockOnTracking
```

每个批次先备份 `editor-before.json`（原地图）到工作目录；start_test 保存性能设置。完成后通过相同 LOCK_ON_WORK 调用 restore_editor。构建需先关闭无未保存工作的编辑器，冷启动后再验证新反射字段。configure_soft_camera.py 现已更新为 v0.5 参数，不应用历史角度死区。

纯镜头测试对应 `CameraAcquireEase`、`CameraScreenZones`、`CameraInputRecoveryAndFrameRate`、`SoftCameraManualAndAssist` 和 `CameraModePriorityAndContinuity`。历史旧专项仍保留，但 `.75s` 硬宽限假设不作为当前验收。最新报告位于 `Docs/Validation/soft-lock-tracking-2026-10-13.md`。
