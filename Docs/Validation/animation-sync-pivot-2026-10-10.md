# Dash／Sprint 标记同步与 Pivot 提前恢复

日期：2026-10-10。UE 5.5.4。工作目录：E:/Project/Git/Hodgepodge。

## 定位与修复

原 Sprint 循环属于 Locomotion，Dash F／B 和根运动 Pivot 的 SyncGroup 为 None，序列没有左右脚标记。Pivot 一直等整段 1.6 秒蒙太奇结束，再追加 0.25 秒重入间隔；素材后部已经在跑步时仍被视为活动掉头。

- 四份 Main 移动序列在 HodgeSprintSync 轨道添加 Hodge.LeftFoot／Hodge.RightFoot 标记，三个蒙太奇使用 HodgeSprint 同步组。蒙太奇领导同步，根运动和动作速率保持各自时间；循环跟随脚步相位。
- 共享层 FullBody_CycleState 增加独立 Sprint 播放器和 Bool 混合，原 Locomotion／AlwaysFollower 播放器保留。Sprint 进入／退出混合 0.12／0.16 秒，Dash 混出 0.18 秒，Pivot 入／出混合 0.12／0.16 秒。Sync Group 与 FullBody 的 Slot Group 是不同配置。
- 新增 Profile.PivotRecoveryEndTime，Main 为素材时间 0.9 秒。素材旋转约 0.3 秒完成，但仍需保留随后制动与反向起步，恢复点后混回循环，未裁剪或重采样原始 Root 轨迹。
- 重入间隔从 Pivot 起手计算；控制阶段仍阻止重复起手，恢复后不追加尾段等待。旧任务回调清理后再启动新实例，避免旧混出完成影响新状态。远端下一序号可有限等待恢复点，届时重新验证会话、速度、方向、朝向与状态。
- Dash 和根运动 Pivot 均保持脚部贴地，避免 Pivot 混出时才突然启用腿部修正；攻击等其他全身动作继续沿用原权重规则。
- 100ms 回归发现另一处真实问题：拥有者保持输入、按住和地面状态时，收到服务器旧 Dash 动作朝向快照而退出 Sprint。复制朝向状态补充动作 SpecHandle／PredictionKey，拒绝已本地结束或激活不符的预测动作；服务器专用技能及组件来源继续接受权威回退，没有调大停步／阻挡阈值。

相关配置已直接保存到 Main，见[配置指南](../Guides/dash-sprint-configuration.md)和[设计 v1.7](../Design/dash-sprint-system.md)。仅九份关联 Main 资产修改：四个序列、三个蒙太奇、DA_Hero_SprintAbility 和 ABP_Pover_LocomotionBase；第三方素材、骨架、Main 主蓝图及其他已有资产保留。

## 实际命令与验收

常规构建前无 PIE、无脏包，并正常关闭编辑器；未使用 Live Coding。

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Relationship+Hodge.Movement+Hodge.Facing+Hodge.Rotation+Hodge.Combo+Hodge.Combat+Hodge.HitReaction' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/AnimationSync/Native3' '-abslog=E:/Project/Git/Hodgepodge/Saved/AnimationSync/Native3.log'
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/configure_animation_sync.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/run_animation_sync.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/run_e2e.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/run_dash_transitions.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/run_combat_integration.py
& E:/Python/python.exe Tools/DashSprint/summarize_animation_sync.py
```

最终构建日志为 Saved/AnimationSync/EditorBuild9.log 和 GameBuild3.log。原生测试增加 Hodge.Facing.AuthorityActionLifecycle，实际覆盖匹配的活动激活、不同激活、结束后迟到数据、服务器技能和组件来源；运动测试补恢复点边界、非法输入及普通／Sprint 分组选择。

新增 PIE 六项：Dash→Sprint 标记同步、Pivot 提前混出、旧尾段时间内再次掉头、控制阶段提前反向输入、松键中断、攻击中断。读取实际组标记状态和蒙太奇实例，拥有者与服务器均需启动第二实例；第二实例必须从起点播放到自身恢复点并完成朝向变化。采集恢复区间脚部位移，检查突发跳变；另复跑十项状态／动画、七项高度／速度和三项攻击交互。

短按完整播放通过 AnimInstance 的真实 OnMontageEnded 事件检查，要求拥有者、服务器、观察者均自然完成，不能因 Slate 长帧跳过末段采样而只凭最大采样位置判定。监听器仅存在于 PIE 验证脚本，结束解绑，不增加 Runtime 玩法字段。

单人、双客户端 0ms、双客户端 100ms 的最终数量与指标由[机器可读结果](animation-sync-pivot-results-2026-10-10.json)核验。每组 Started 不是通过，必须核对 error、场景数量和 passed。测试地面与第二名玩家隔离，避免碰撞污染指标。

## 最终结果

- EditorBuild9 和 GameBuild3 均退出 0；Native3 共 36 项通过（26 项 Success、10 项带测试警告），0 失败、0 未运行。新 AuthorityActionLifecycle 测试无警告。
- 正式动画层编译 ERRORS=0、WARNINGS=0，FootPlacement 的实验性节点说明仍保留。九份关联资产已保存。
- 单人、双客户端 0ms 和 100ms 每种条件 26 项，共 78 项全部通过。最终动画事件确认三个网络视角的无方向短按自然结束，移动取消和 Sprint 交接允许主动混出。
- 实际组记录确认循环使用 HodgeSprint 并参与标记同步。三种条件均执行两个不同 Pivot 实例，第二次启动距第一次起手约 1.00～1.03 秒，早于原 1.6 秒尾段结束；第二实例从起点播放、达到恢复点并转回正确朝向。
- 恢复区间采样的最大脚部速度：单人约 738cm/s、0ms 约 755cm/s、100ms 约 727cm/s，未触发突发跳变探针。此数值是骨骼空间的过渡指标，不是角色移动速度或全视角视觉保证。
- 高度／移动／Sprint 速度、正常 Pivot、可配置 Hold／Handoff、松键／停步、攻击取消和攻击退出均通过。最终 Profile 的恢复点 0.9、混出 0.16、停步宽限 0.08、阻挡时长 0.4 已核对。
- PIE 与脏包均为 0，网络模拟已关闭，诊断 CVar 关闭，Slate 节流恢复原值。与 33 份 Main 基线对照，九份关联资产变化，其余 24 份内容 hash 相同；没有提交 Git。

最终运行与清理记录：Saved/AnimationSync/final-runtime-last.log、final-state.json。机器可读结果已由 summarize_animation_sync.py 完整校验。

## 调试记录与限制

- const／非 const 的 GetLinkedAnimInstances 可见性不同；CollectMarkers 没有导出给项目模块，改用公开 PostEditChange 触发刷新。
- CycleA／B 是主状态，实际叶播放器在共享层 FullBody_CycleState。UE 5.5 的折叠同步组属性未提供所需动态引脚，因此使用独立播放分支，未强改引擎或改变普通分支角色。
- 新测试 C++ 类名与旧 Lifecycle 测试重名，已单独命名，原测试保留。
- 原 100ms 失败样本与退出诊断保留为 sync-network-100ms-first.json、sync-network-100ms-facing-first.json。修复朝向来源生命周期后重新执行，不计早期失败为通过。
- 编辑器出现连续约 0.4 秒长帧，使服务器提交动画显著迟于客户端结束；该样本保留为 network-100ms-long-frame-first.json。测试期间临时关闭 Slate 节流并复跑，结束恢复原值。较长混出下，旧固定尾段采样点会被长帧跳过，采样改为依据实际 BlendOut 开始点计算；缺样本报告 fixed-network-100ms-sampling-first.json 不计为通过。
- 本轮不采集代码／分支或蓝图覆盖率百分比。未执行 Cook／打包、独立进程联机、丢包与超过 100ms 的压力、全地形和全视角人工视觉验收；不把常规帧率下通过扩大为任意长帧下完整动画保证。

保留所有用户已有改动，不自动提交 Git。日志、缓存和失败探针位于 Saved，不属于源码交付。
