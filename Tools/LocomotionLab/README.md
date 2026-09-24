# CodexText ALS 式地面运动实验

入口地图：`/Game/CodexText/Locomotion/L_LocomotionLab`。Play 后点击游戏视口。

- WASD 移动，鼠标沿用 Hodge 视角输入。
- 默认跑步 420 cm/s；按住 Shift 行走 180 cm/s。
- V 切换镜头相对面向 / 移动方向面向。
- O 开关 Stand2 上半身姿态覆盖（实验预览；不触发 GAS 技能或装备武器）。
- Shift+F1 释放鼠标，Esc 结束 PIE。

## 2026-09-21 精修接入

仍使用 `ABP_ALS_Grounded`，没有改动 Main/Wuwa 原始资产。图中保留 Grounded 与六方向循环，增加以下实验层：

- 停步读取 Feet_Position 与接触曲线，已落地支撑脚立即开始锁定。由于现有收脚片段与循环姿态不一致，身体使用短混合，不能把文章中的立即锁脚直接等同于整个人物零时间切换。
- 转身角度决定播放速度，镜头反向或切出镜头面向时中断旧转身；动作末帧完整输出，避免90度动作少转最后几度。
- 十二个循环播放器的 StartPosition 按与站立腿部姿态的差异采样选取，保留同步标记。低速通过步幅与步频共同匹配，方向混合补偿合成步幅；使用 UE5.5 Stride Warping 的 Manual 模式。
- Foot Placement 处理地面法线、脚底与骨盆高度，再由 Two Bone IK 解算腿链。该引擎节点在5.5标记为 Experimental；它的额外脚锁关闭，继续由现有 FootLock 管理停步和转身。实验地图新增15度斜坡与15厘米高差台阶，Actor 标签以 CodexTerrain_ 开头。
- Stand2 从 Bip001Spine 开始覆盖上半身，控制曲线保留基础层；DefaultSlot 仍位于覆盖层之后，原全身技能可覆盖最终姿态。不是把现有全身攻击强行裁成上半身攻击，也尚未实现完整 ALS 多武器 Overlay 系统。
- `SetLeftHandGrip(Component, Socket, Weight)` 接受装备组件上的握持目标，使用右手骨空间驱动左手位置和旋转。传入空组件解除；组件失效、下落或技能蒙太奇期间停用。没有装备目标时权重为0，不强行改变当前单手近战动作。

最后验证结果与限制见 `grounded/refinement-verification.md`。构建、运行验证和完整战斗/联机/打包验收必须分开看待。

## 2026-09-20 基线：FootLock、停步与原地转身

实验角色现在使用 `/Game/CodexText/AnimInstance/ALS/ABP_ALS_Grounded`，父类为 `UHodgeGroundedLocomotion`，继承第一阶段的 Hodge/GAS 动画初始化及六方向运动逻辑。打开 AnimGraph → Grounded → Move → ALS_Locomotion 查看六方向循环层。Grounded 外层包含 Idle、Move、四种左右脚走跑停步、左右转身及 Fall；根图包含 DefaultSlot、脚踝姿态缓存、双腿 Two Bone IK 和脚踝旋转修正。

- 十二个循环动画副本在 `/Game/CodexText/Anim/Grounded`，补齐 L/R 同步标记、FootContact_L/R 和 Feet_Position 曲线；保留第一阶段 FeetCrossing 与速度校准。
- 停步沿用 CharacterMovement 制动，速度降到阈值后，按支撑脚选择四个裁剪后的收脚动画。Walk_L 取原片段 1.0–1.9 秒，Walk_R 取 1.0–2.6 秒，Run_L 取 0.8–2.5 秒，Run_R 取 0.8–2.633 秒。它们负责制动后的收脚，不是完整距离匹配刹停系统。
- 镜头面向模式静止时身体保持朝向。视角与身体相差超过70°并持续0.2秒后，播放对应90°转身；CF_TurnYaw_Left/Right 根据素材根旋转驱动角色朝向。动画副本根轨道归零，避免重复位移/旋转。
- FootLock 根据低速落脚区间的曲线缓存世界空间脚踝位置与旋转，再由蓝图 IK 应用。原虚拟脚骨实际指向脚尖，因此图中先将正确脚踝姿态复制到虚拟骨，再修改腿链；没有更改源 Skeleton。移动、腾空、蒙太奇和大幅瞬移解除锁定。
- 更新顺序明确为移动组件 → 面向组件 → Mesh 动画，避免旋转之后脚锁目标仍使用上一帧坐标系。Two Bone IK 最多允许3%伸展，补偿转身混合姿态触及腿长上限时的末端误差；这不代替坡地骨盆修正。
- 保留六方向换向及髋部偏好。不启用 crouch；不增加启动动画、Lean 或基于 Lean 的 Pivot。Run_Turnback 独立急转动作尚未接入。

当前范围是单人平地实验。尚未做坡地射线/骨盆高度修正、移动平台相对锁定、步幅修正、完整武器/GAS战斗回归、联机复制或打包验证。脚部曲线来自素材轨迹采样，仍可以逐帧人工微调。

### 验证与复现

2026-09-15 已完成常规 Editor 构建、蓝图接入和 PIE 测试；2026-09-20 继续核对当前资产并复测。测试脚本在本目录 `grounded/`，每次只运行一个，需要先打开实验地图并启动单人 PIE、等待角色生成。脚本会移动实验角色和调整测试时的面向；不要与手动操作同时进行。原始运行输出在 `D:/CodexUE55Lab/grounded-*.json`，验证摘要见 `grounded/verification.json`。

- 走跑×两种面向×八方向：32/32；经原 Hodge Enhanced Input 的 IA_Move：8/8。
- 四种停步状态均触发并回到 Idle；左右90°转身及移动中断、腾空/瞬移解除脚锁通过。连续满权重转身锁脚区间的累计位移低于1 cm。
- DefaultSlot 动态蒙太奇播放期间脚锁释放，结束后恢复；角色不能蹲伏。此检查不等于完整 GAS 技能回归。
- 左右快速换向仍经过对应反向髋部状态。完整逐帧数据在上述临时输出目录。
- Game 构建的历史阻断是原 `Private/Character/HodgeEnemyCharacter.cpp:53` 未以 WITH_EDITOR 保护的 PostEditChangeProperty 实现；本轮新增运行时代码曾编译通过，未修改该无关问题。最新构建结果见 grounded/verification-notes.md。

改造前备份：`D:/CodexUE55Lab/BeforeFootLock-20260915-095637`；编辑器中保存的蓝图备份 `/Game/CodexText/Backups/BeforeFootLock/ABP_ALS_Lab`。下面保留第一阶段记录，旧 ABP_ALS_Lab 不是当前实验角色的主输出。

## 第一阶段实现记录

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
