# CodexText ALS 式地面运动实验

入口地图：`/Game/CodexText/Locomotion/L_LocomotionLab`。Play 后点击游戏视口。

- WASD 移动，鼠标沿用 Hodge 视角输入。
- 默认跑步 420 cm/s；按住 Shift 行走 180 cm/s。
- V 切换镜头相对面向 / 移动方向面向。
- Shift+F1 释放鼠标，Esc 结束 PIE。

## 查看实现

新动画蓝图：`/Game/CodexText/AnimInstance/ALS/ABP_ALS_Lab`。
打开 AnimGraph，双击 `ALS_Locomotion`，可查看 Idle、Forward、Backward、LeftForward、LeftBackward、RightForward、RightBackward、Fall。
每个方向状态内部有四个走跑姿态混合及 VelocityBlend 权重；所有运动 SequencePlayer 使用 ALS_Locomotion 同步组。输出通过 DefaultSlot。

实验角色 `BP_LocomotionHero` 已使用新蓝图。以前 `AnimInstance/ABP_Pover_Base` 中的 Speed×Direction BlendSpace 方案不再是实验角色的主动画输出，旧资产保留，未删除。不要打开旧 Pover 混合空间来查看本轮实现。

## 本轮实现

C++ `UHodgeALSLocomotion : UHodgeAnimInstance` 保留 Hodge 原生动画基类的 GAS 初始化。C++ 更新方向、权重与换向条件，蓝图实际执行状态转换、播放和混合。

- F/B/LF/LB/RF/RB 六种姿态。经脚部支撑轨迹核实，LF/LB 都是向左约90度侧移，RF/RB 都是向右约90度侧移，不是四个斜向样本。
- 镜头空间方向选择有滞回，避免边界抖动；角色旋转修正不反过来改变方向分类。
- 左右反向优先 LF→RB、RF→LB（以及对应反向），保留髋部姿态；短暂保持后再按 HipOrientationBias 恢复。
- FeetCrossing < 0.2 才允许同侧前后姿态切换。十二个动画副本已添加曲线，依据33个采样点的脚部横向间距生成，带安全余量。这是素材适配的初版曲线，尚非逐帧人工动画精修，也不保证任意混合姿态绝不穿腿。
- VelocityBlend 按角色空间速度的绝对分量和归一化，指数平滑；YawOffset 使用复制到本实验目录的 ALS 曲线，校正镜头相对角色朝向，使稳定斜向运动收敛至主导姿态。
- 动画在 `CodexText/Anim/ALS`，没有改动 BaseAnim 或原 Wuwa 动画。根据支撑脚速度估算，走路素材原速约82–90、跑步约350–381 cm/s；副本 RateScale 已校准到实验速度，配合运行时 PlayRate。同步混合、加减速中的滑步仍需后续步幅/脚锁精调。
- 原 Hodge 输入、CharacterMovement、Experience、PawnData、PlayerState ASC 保留。LabComponent 自定义旋转遵守移动组件禁止旋转的返回值。

这是按提供文章的核心机制适配 Hodge 和现有骨架的第一阶段，不是完整移植 ALSv4。未接入原武器动画层、完整起停/转身、步幅与斜向 IK 修正、脚锁、Lean 或联机复制；Fall 仅为基础下落姿态，DefaultSlot 未做战斗能力回归。HipOrientationBias 为可调参数，尚未由瞄准/武器动画曲线自动驱动。

## 已执行验证（2026-09-15，UE 5.5.4）

- `Build.bat HodgepodgeEditor Win64 Development -Project=D:/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64`：退出码0，日志 `D:/CodexUE55Lab/als-build.log`。
- 新动画蓝图编译通过。节点审计：根图含状态机、Slot、输出；运动机8状态、56条显式转换；每个方向图含四组走跑混合及八个播放器。见 graph-audit.json。
- 32/32 PIE 组合通过：走跑×两种面向×8方向。检查实际速度、角色局部方向、主导权重及蓝图实际活动状态，见 direction-results.json。测试使用原生函数控制走跑和面向，暂停键盘轮询但保留旋转 Tick。
- 8/8 Enhanced Input IA_Move 注入通过，经 HodgeHeroComponent，在镜头90度时世界移动方向正确，见 input-results.json。不是硬件长按测试。
- 1566帧换向记录：LF→RB→RF、RF→LB→LF、正负髋偏好选择通过；观察到同侧恢复时 FeetCrossing 均低于0.2。见 reversal-results.json。完整轨迹 `D:/CodexUE55Lab/als-reversals.json`。
- PIE 截图确认角色正常显示并播放新动画。`D:/CodexUE55Lab/als-live.png`。截图期间引擎等待资源加载导致桥接超时，随后完成，未重复截图写入。
- 原 Main/Character/Hero 与 Wuwa 共687个文件 SHA-256 与任务前一致。实验资产已保存，PIE已停止。
- Game 构建再次执行，新增两个实验 cpp 均编译通过；整个目标仍因原 `HodgeEnemyCharacter.cpp:53` 的 PostEditChangeProperty 条件编译错误失败，日志 `D:/CodexUE55Lab/als-game-build.log`。未打包、未联机测试。

## 备份和测试

改造前备份：`D:/CodexUE55Lab/PreALS-20260914-234004`。更早副本备份与原文件哈希：`D:/CodexUE55Lab/LocomotionPhase1Backup`。

三个 test_*.py 是当前地图单人 PIE 中执行的编辑器 Python 脚本，必须等待角色初始化完成，逐个运行，不要并行。它们会移动测试角色、切换状态并写测试结果。方向测试临时关闭编辑器后台节流；本轮结束已恢复原值。一次性建图脚本位于 D:/CodexUE55Lab/als_create.py，已有蓝图时会拒绝重建，不要用旧 phase1 脚本覆盖本轮资产。
