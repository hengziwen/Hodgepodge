# Pivot 滑步修复与验证

日期：2026-10-10；UE 5.5.4。对应[设计 v1.4](../Design/dash-sprint-system.md)及[配置指南](../Guides/dash-sprint-configuration.md)。[原始结果摘要](pivot-foot-slide-results-2026-10-10.json)。

## 原因

Run_Turnback 原素材长 1.6 秒，Root 前向轨迹在约 0.5 秒达到 216 cm，再反向起步。上一版复制后关闭根运动，按固定时间播放；角色却由普通 CMC 先完成反向刹停并加速。动画接地阶段与胶囊位移不同步，因此即使解决慢放／卡帧，也会滑步。

Sprint_F 本身是原地循环，Root 位移为零。其近地脚部步幅速度估算约 599.07 cm/s；固定 1 倍播放搭配 720 cm/s 实际移动，也会在掉头结束混回循环时产生滑动。

## 实现

- 新建 A_Hero_Sprint_Pivot_RootMotion 与 AM_Hero_Sprint_Pivot，位于 /Game/Main/Character/Hero/Anim/Movement；原有 Turn 及第三方原素材保留。
- 新资源启用根运动、AnimFirstFrame 根锁定、FullBody Slot，BlendIn／Out 为 0.08／0.12 秒。正式 Main AnimBP 使用 RootMotionFromMontagesOnly。
- Sprint GA 检测近反向输入，以当前 Sprint 的 SessionId／SequenceId 与起手方向、Yaw 快照发送预测 TargetData。服务器复检，GAS／Character 根运动复制驱动其他客户端。
- 拥有者保留当前起手朝向，服务器对齐通过验证的快照；CMC 只在这个 Pivot 根运动蒙太奇活跃时让它提供唯一旋转增量，其他攻击逻辑保留。
- 新路径启用时关闭旧原地 Pivot 准入，避免两套掉头叠加。正常完成回 Sprint；松开、攻击、伤害／控制会沿能力结束流程停止自己持有的蒙太奇。
- 原地循环按实际平面速度／参考速度匹配播放倍率；默认参考速度 599.07 cm/s、基准倍率 1、上下限 0.5～1.5，720 cm/s 下约 1.20 倍。参考值从原地素材近地、垂直速度较小的脚部区间估算，并非伪造 Root 曲线。

默认 PivotTriggerAngle=150°、PivotMinimumSpeed=250 cm/s、PivotReentryDelay=0.25 秒。重入间隔是防重复动作，不是 Dash／Sprint 冷却。未新增体力、无敌、完整锁定或镜头功能。

## 接地数据对照

同一正式 Hero、平地、同一反向输入，对比旧原地路径与新根运动路径。动画时间取 0.18～1.2 秒；脚踝高度低于该段最小值＋5 cm，且垂直速度小于 40 cm/s，统计脚踝世界空间 XY 运动速度。

- 左脚：约 580.99 → 77.26 cm/s，下降 86.70%；旧／新近地样本 19／21。
- 右脚：约 567.41 → 44.62 cm/s，下降 92.14%；旧／新近地样本 12／5。

这是脚踝接地代理指标，不是鞋底严格锁定测量。转身时脚踝绕前脚掌运动本身可以非零，且样本包含落地／离地边缘；不能把这些数值解释成鞋底每一点的实际滑动速度或“全场景绝对零滑移”。数据证明动画／胶囊的主要错配显著减少。

本次没有新增足底锁定 IK、任意地形适配或多角度镜像转身。现有骨骼混合与 IK 保留。

## 实际验证

最终 EditorBuild4.log、GameBuild2.log 退出 0；日志在 Saved/PivotFootSlide。常规构建前经插件确认两个项目编辑器均无脏包，停止用户运行的 PIE 后正常关闭，没有强制杀进程或使用 Live Coding。

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
```

Native2：32 项通过，23 无警告、9 带既有警告，失败／未运行均为 0。新增根运动 Pivot 触发角度、速度／零输入／非有限值、TargetData 序号／朝向序列化与循环速度匹配测试，回归 Movement、Facing、Rotation、Combo、Combat、HitReaction。

```text
UnrealEditor-Cmd.exe Hodgepodge.uproject -unattended -NullRHI -nosplash
-ExecCmds="Automation RunTests Hodge.Movement+Hodge.Facing+Hodge.Rotation+Hodge.Combo+Hodge.Combat+Hodge.HitReaction"
-TestExit="Automation Test Queue Empty"
-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/PivotFootSlide/Native2
```

真实 PIE 合计 37 场景：

- 单人移动回归 10 项：Dash 短按、B／侧向、取消后摇、长按／窗口配置、Sprint 退出、根运动掉头。
- 单人对照与取消 4 项：旧路径、根运动路径、掉头中松开、掉头中攻击。
- 额外独立根运动播放采样 1 项。
- 双客户端＋PIE 专用服务器，0ms／100ms 各 10 项；拥有者、服务器、观察者均实际播放至恢复尾段并退出。
- 100ms 下掉头期间真实 Melee 正伤害、零生命伤害强控制各 1 项，均确认停止自己的根运动蒙太奇并结束 Sprint。

工具：run_e2e.py、run_pivot_contact.py、run_pivot_hit_exit.py；均通过本地插件执行。错误为空且场景数量完整后才计通过。

没有执行本轮覆盖率采集、Cook／打包、独立进程网络或高延迟／丢包压力测试。v1.3 的覆盖率为历史范围，不能用于本次代码。

## 工作区与收尾

Profile 与指定 Main AnimBP 为预期修改，新增两个根运动资产。对本轮开始已有的其他 23 份 Main 资产核对 hash，一致，包含已有伤害 GE、攻击蒙太奇、Hero BP 和固定动画层。

测试覆盖值已还原，只重载临时 Profile／探针包；NetEmulation.Off 已发送，PIE 世界 0、脏包 0。编辑器重新打开并保留可配置状态。未回退、未自动提交。
