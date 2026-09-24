# 2026-09-21 动画精修验证

范围：CodexText 实验角色及 ABP_ALS_Grounded。保留原 Hodge 输入、移动、GAS/PlayerState 初始化。没有启用蹲伏、Lean 或添加独立起步动画。

## 构建与接入

- `Build.bat HodgepodgeEditor Win64 Development -Project=D:/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64`：退出0；最新日志 `D:/CodexUE55Lab/refined-final-editor-build.log`。关闭编辑器后常规构建，再重新打开验证。
- `Build.bat Hodgepodge Win64 Development -Project=D:/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64`：退出6；最新日志 `D:/CodexUE55Lab/refined-final-game-build.log`。本轮运行时代码编译通过，整个 Game 目标仍阻断于原 `HodgeEnemyCharacter.cpp:53` 的 PostEditChangeProperty 条件编译问题。未改动该文件。
- ABP_ALS_Grounded 通过项目原生 authoring helper 编译并保存，实际 PIE 使用该类。曲线修改仅在 Anim/Grounded 的19个副本中；保留原 Skeleton。
- 新增步幅、地形与分层图使用本机 UE5.5.4 API。AnimationWarping 已由项目启用，本次仅为编辑器建图补充 Build.cs 编辑器条件依赖，没有升级或重新启用插件。

## 实际运行验证

`refinement-verification.json` 保存19项最终断言，全部通过。原始帧数据在 `D:/CodexUE55Lab`，不能把状态断言当成所有素材逐帧美术验收。

- 无输入新生角色：Idle 正常，双脚最小横向间距约26.24厘米；同时检查地形 IK 开启后的脚趾离地距离，不能再要求 IK 后脚踝高度与原动画完全相同。
- 走跑 × 两种面向 × 八方向：32/32。
- 地面过渡：13种情况，包括四种停步、左右90度转身、移动中断、下落和瞬移。测试中的停步首帧最大脚位移约1.72厘米；修复了步幅权重瞬间退出造成约20厘米跳位的问题。
- 低速：通过半幅移动输入达到90 cm/s，步幅约0.625、播放速率0.8；斜向切换能触发步幅补偿。测试不再用编辑器属性写入强行覆盖 CharacterMovement 的 MaxWalkSpeed。
- 30 FPS 左右转身：最终角度误差小于1度，满权重锁脚窗口最大累计位移约0.74厘米。
- 静止地形对比：平地、15度斜坡、15厘米台阶开启/关闭 IK。开启后两侧脚趾高度误差范围约 -0.14～1.27厘米；关闭时台阶悬空一侧约19厘米。这是脚趾骨与射线命中面的度量，不等于鞋底网格的精确接触距离。
- Overlay：站立与移动期间权重达到1；原 DefaultSlot 保留。验证用 Stand2 姿态覆盖上半身，默认关闭，O键可预览。
- 手部 IK：使用角色右前臂骨作为临时测试目标，左手稳态位置误差最大约0.021厘米。实际装备可调用 SetLeftHandGrip 指定自己的握持挂点；没有有效目标时不开启。
- 近战 Attack01 经 DefaultSlot 播放，动画更新后解除手部 IK。该检查是蒙太奇节点与 IK 的协作测试，不是完整 GAS 技能/网络回归。

## 尚未验收的范围

- 当前接入的是基础姿态覆盖和手部 IK 目标接口，不是完整 ALS 多武器 Overlay、AimOffset 或所有现有技能的逐一适配。
- 没有装备双手武器资产或创建虚构握持挂点；当前近战动作仍沿用其原有动画。未来每件需要双手握持的装备应提供真实目标，并在装备/卸装流程绑定和解绑。
- 地形 Foot Placement 在 UE5.5 标记为 Experimental；本轮完成上述静态地形对比，尚未覆盖各种楼梯连续行走、移动平台、极端坡度、不同 LOD/帧率组合的完整验收。
- 尚未验证联机、Cook/打包及完整战斗能力回归。独立 Run_Turnback 片段、完整起步动作与全套 ALS 细节不在本轮完成项中。

## 复测方法

打开 `/Game/CodexText/Locomotion/L_LocomotionLab`，启动单人 PIE，等待角色生成。不要在脚本运行期间手动控制角色。

1. `test_idle_spawn.py` 应在新 PIE 开始前注册，验证出生站姿。
2. 用项目虚拟环境运行 `run_refined_suite.py`，它顺序执行6组脚本，约3分钟；每组等待自己的结果文件更新后才启动下一组。
3. 运行 `verify_refinement.py` 检查断言；失败时退出1。脚本依赖本轮的 idle-spawn-test.json。
4. 完成后停止 PIE，并将 EditorPerformanceSettings.bThrottleCPUWhenNotForeground 恢复为 true。

`apply_stride.py` 与 `apply_terrain.py` 是一次性建图脚本，已有节点时拒绝重复插入；不要用它们覆盖用户后来手动调整的图。`apply_combat.py` 可对已存在的分层进行过渡参数和默认姿态核对。

初始源码、蓝图及地图备份：`D:/CodexUE55Lab/BeforeRefine-20260921-090915`。未自动提交或回退已有用户修改。
