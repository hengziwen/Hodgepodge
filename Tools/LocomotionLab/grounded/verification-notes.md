# Grounded 实验交付记录

本轮实现保存在 `CodexText` 实验资产及对应 C++ 中。当前动画蓝图是 `/Game/CodexText/AnimInstance/ALS/ABP_ALS_Grounded`，实验入口是 `/Game/CodexText/Locomotion/L_LocomotionLab`。原始 Main/Wuwa 资产没有由本轮重存。

## 构建

2026-09-20 常规构建命令：

```powershell
& D:\UE_5.5\Engine\Build\BatchFiles\Build.bat HodgepodgeEditor Win64 Development -Project=D:\Hodgepodge\Hodgepodge.uproject -WaitMutex -architecture=x64
& D:\UE_5.5\Engine\Build\BatchFiles\Build.bat Hodgepodge Win64 Development -Project=D:\Hodgepodge\Hodgepodge.uproject -WaitMutex -architecture=x64
```

Editor 目标通过，退出码0。Game 目标退出码6，仍被 `HodgeEnemyCharacter.cpp:53` 未加 WITH_EDITOR 保护的 PostEditChangeProperty 定义阻断；本轮实验代码编译通过。日志保存在 `D:/CodexUE55Lab/grounded-build-20260920.log` 与 `grounded-game-build-20260920.log`。未修改无关的 EnemyCharacter 错误，未声称打包通过。

动画蓝图及实验角色蓝图通过现有 C++ authoring helper 编译，检查 BS_Error 后保存。19个动画副本检查了脚锁/接触曲线、循环左右脚同步标记，以及起中末三个采样点的零根轨道。根图包含两组 Copy Bone、Two Bone IK、Modify Bone；原始 Skeleton 未修改。

## PIE 验证

`verification.json` 是逐帧结果的断言摘要。完整输出在 `D:/CodexUE55Lab`，不将数千帧日志提交到项目源码。

2026-09-20 最终结果：15项断言全部通过；主停步/脚锁测试5896帧、换向测试1341帧。方向32/32、输入8/8通过。默认帧率下转身锁脚区间最大累计位移0.421 cm，30 FPS下0.880 cm；左右转身最终角度约+90.015°、-90.015°。测试结束已停止 PIE，资产检查无未保存包。

覆盖走跑×两种面向×八方向、Hodge IA_Move 注入、左右髋部换向、四种支撑脚停步、停步中断、左右90°转身及移动中断、腾空/瞬移解锁、DefaultSlot动态蒙太奇解锁和蹲伏禁用。另以30 FPS检查转身及锁脚。测试以实际活动状态、速度、脚踝世界位置和脚锁权重判断，不仅检查 C++ 标志。

复测修正了面向组件与 Mesh 的 Tick 先后关系。腿链最多3%伸展，补偿混合姿态下略超出腿长的锁定目标；满权重转身区间的累计位移门槛为1 cm。该指标只覆盖本次平地素材与测试帧率，不是任意帧率、地形或动作组合的保证。

依次执行 `test_grounded.py`、`test_directions.py`、`test_hodge_input.py`、`test_reversals.py`、`test_interruptions.py`、`test_stop_interrupt.py`、`test_turn30.py`，每个完成后再运行下一个；之后用工程 Python 运行 `verify_results.py`。编辑器脚本需要桥接执行，不能直接用普通 Python 导入 unreal。`audit_assets.py` 在非 PIE 编辑器中执行。测试临时关闭后台节流，结束交付时恢复设置；30 FPS测试恢复原帧率上限。

## 开场脚部交叉修复（2026-09-21）

此前移动测试通过，但未覆盖无输入出生站立，遗漏了双脚锁在同一点的问题。NativePostEvaluateAnimation 发生在 Mesh 骨骼缓冲交换之前，GetSocketTransform 读到了旧姿势。改用 OnBoneTransformsFinalized 回调缓存脚部世界变换，并在动画实例重新初始化/销毁时解绑；角色落地修正位置超过0.5 cm时释放旧锚点，避免将双脚锁在出生时较高的位置。

新增 `test_idle_spawn.py`：在启动 PIE 前注册采样，不注入移动、不传送角色，检查开场静止脚距和与 IK 前姿势的偏差。实测双脚横向间距26.38 cm，脚踝与原姿势最大偏差低于0.04 cm。截图 `D:/CodexUE55Lab/idle-fixed.png` 已人工查看，双脚分开且贴地。

本次 Editor 常规构建退出码0，日志 `D:/CodexUE55Lab/idle-fix-build.log`；Game 仍被上述 EnemyCharacter 旧错误阻断，日志 `idle-fix-game-build.log`。不将旧的运动回归通过当作初始站姿已正确的证据。

## 保留的边界（未扩展）

没有专用启动动画、crouch 或 Lean/Pivot；独立 Run_Turnback 急转尚未接入。停步是移动组件完成制动后的收脚，并非 Distance Matching。尚未验证坡地/移动平台、联机、打包及完整武器/GAS技能组合。备份和第一阶段记录见上级 README。
