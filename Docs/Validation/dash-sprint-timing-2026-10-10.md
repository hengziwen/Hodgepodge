# Dash／Sprint v1.3：时序、体力移除与 Pivot 验证

日期：2026-10-10。引擎：UE 5.5.4。工作目录：E:/Project/Git/Hodgepodge。

本轮对应[设计 v1.3](../Design/dash-sprint-system.md)与[配置指南](../Guides/dash-sprint-configuration.md)，测试数据见[机器可读结果](dash-sprint-timing-results-2026-10-10.json)。此前 v1.2 的体力与测试结论是历史记录，不代表当前实现。

## 1. 已完成的行为

- 短按 Shift 不取消 Dash。F／B 1.333333 秒蒙太奇按 MontagePlayRate=1 播放，Duration=0.35 秒只控制 RMS 位移；正常结束等待本次蒙太奇 OnCompleted。
- 默认 0.5 秒开放移动取消后摇；没有移动意图就完整播完。只结束自己的动作、RMS、朝向、防御和 Cue，使用既有蒙太奇混出，不清全体动作或写死 Idle。
- Sprint 窗口默认 0.5～1.2 秒，长按阈值 0.22 秒，移动阈值 0.1，网络等待／授权余量 0.5 秒，均能在 DA_Hero_SprintAbility 配置。长按阈值可以晚于窗口开启，不会被提前消费。
- 移除 PlayerState 体力默认子对象、体力属性／GE／Tag、初始化与回滚、Dash 消耗、Sprint 资源门槛与持续消耗、恢复、恢复阻塞、预测资源预算、SavedMove 预算字段和相关 API。
- Sprint Pivot 使用时间推进与独立倍率，相关节点统一重置，移出 Locomotion 同步组；主图 Pivot→Cycle 补齐按长度／倍率／状态经过时间的出口。原八向资源和普通距离匹配分支保留。
- Sprint 原地循环使用 SprintCyclePlayRate，不再按零根运动位移反推速度，消除每帧无法调整 playrate 的警告。

当前攻击不使用旋转锁；没有加入攻击锁交接。B 默认启用，保持角色后向锥后撤语义。

## 2. 问题与修复依据

原 Dash 把 1.33 秒动画按 Length／0.35 加速，再在位移结束时结束 GA；动画、位移和恢复段被错误绑定。修订分离位移与动画时间，以完成回调收尾，以显式窗口主动取消恢复段。

原衔接只在窗口开启时尝试一次，长按尚未成立会消费会话；另有代码内固定等待时间。修订在窗口内等到长按、方向与授权同时成立，再只尝试一次；异步等待与授权余量纳入配置。

原 Pivot 资源替换后仍调用距离匹配，并使用原素材通知／状态出口。测试先发现时间推进异常，再发现末帧保持。修订为单一 Turn 的独立时钟，并补齐时间出口。早期仅检验时间推进的断言不够，最终额外验证在 Shift 释放之前已回到循环。

调试还发现节点不相关后 GetCachedBlendWeight 会保留上一次权重。最终采样联合主状态机当前状态，避免把不再求值的节点缓存误判为仍在播放。

首次修订的 PIE 暴露了新增准入检查误拒绝自身 DashPreparing 的回归，已修正并加入原生断言；最终运行依据是修正后的 Native3 和完整 E2E，早期失败不计为通过。

## 3. HUD 与体力事实

最终磁盘配置已复读；物理映射为 LeftShift／Gamepad_FaceButton_Right。正式 Main 没有体力条资产或显示绑定。运行时记录为 HP 150／150，生命显示比例 1，两个能力入口；查找 /Script/Hodgepodge.HodgeStaminaSet 为不存在。

生命面板位于 WBP_HodgeHUD 的左下 PlayerVitalsPoint，OverlaySlot 左／下对齐、Padding=32。WBP_PlayerVitals 的 SizeBox 宽 280，Border 内边距 16，纵排 HealthText／HealthBar。这些是 UMG 逻辑单位，受 DPI 缩放影响。右下为 AbilityBarPoint。未改动 Main UI 资产。

CodexText Survivor 示例中带 Stamina 名字的控件显示等级／经验等示例数据，没有绑定本次体力属性，保留该独立示例。

## 4. 构建、蓝图与原生测试

最终常规 EditorBuild8.log、GameBuild4.log 均退出 0；日志位于 Saved/DashTimingRefinement。实际命令：

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
```

关闭编辑器后常规编译，没有用 Live Coding 替代。保留原有插件依赖、IncludeOrder、UI 弃用等提示。新增 Editor 工具明确依赖 AnimGraphRuntime；早期工具编译／链接问题已修复。

refine_timing.py 只保存本轮指定 Profile 和两份 AnimBP。最终两份动画蓝图均 ERRORS=0、WARNINGS=0；FootPlacement 实验性信息与未使用引脚信息保留。迁移结果为 PivotEvaluators=2、TimingExits=1。

Native3/index.json：31 项，通过 31（22 无警告、9 带既有日志警告），失败 0，未运行 0。

Hodge.Movement 覆盖 F／B 与锥形边界、输入会话、目标数据序列化、已移除字段、准备阶段提交、移动键授权、多来源速度修正、回放、衔接与取消窗口、Pivot 时间推进／倍率／完成／非法值、防御窗口。回归 Facing、Rotation、Combo、Combat、HitReaction。

```text
UnrealEditor-Cmd.exe Hodgepodge.uproject -unattended -NullRHI -nosplash
-ExecCmds="Automation RunTests Hodge.Movement+Hodge.Facing+Hodge.Rotation+Hodge.Combo+Hodge.Combat+Hodge.HitReaction"
-TestExit="Automation Test Queue Empty"
-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/DashTimingRefinement/Native3
```

## 5. 真实 PIE 与网络

地图 /Game/ThirdPerson/Maps/ThirdPersonMap，正式 BP_Hero_Pover／PawnData／Main 动画图，真实 Enhanced Input 注入。攻击与受击使用已有隔离探针授予；未保存探针修改。

- 单人：10 场景通过。
- 双客户端 Play As Client＋PIE 专用服务器，0ms：10 个移动与动画、3 个攻击交互、2 个实际伤害／控制场景通过。
- 同样模式，各验证世界 NetEmulation.PktLag 100：上述 15 场景通过。
- 合计 40 场景；7 份报告 error 为空，场景数量完整。

移动场景：前／后／侧短按完整动画、默认长按、松开退出、停步退出、移动取消后摇、延后长按阈值、延后衔接时间、实际掉头 Pivot。短按验证拥有者与服务器完整播放，联机还检查观察客户端蒙太奇位置。

攻击场景：窗口外拒绝 Dash 取消、窗口内允许、Sprint 后攻击退出。受击场景：真实正生命伤害导致退出、零生命伤害的强控制导致退出。

默认窗口拥有者约 0.55 秒进入 Sprint。HoldThreshold=0.85 时约 0.884 秒进入；HandoffOpenTime=0.85 时约 0.917 秒进入。数值包含起手准备和帧量化，配置窗口自身从成功播放计时。

0／100ms 场景均验证拥有者、服务器、观察客户端的 Turn 求值时间正常增长，并在按键释放前离开 Pivot。100ms 下存在正常传输与状态到达的相位差；不声明帧级零延迟同步。

结束已发送 NetEmulation.Off、停止 PIE、还原临时 Profile 字段及探针配置，只重载测试涉及的脏包。最终 PIE 世界 0、脏包 0。

## 6. 覆盖率与边界

OpenCppCoverage 0.9.9.0，MSVC PDB 行覆盖；合并 Native3 和最终单人 E2E。相对本轮开始时备份，本轮 Runtime 新增／修改的可测行覆盖 92／102，90.20%。逐文件未覆盖行见结果 JSON，原始 XML／HTML 在 Saved/DashTimingRefinement 的 native-coverage.xml、e2e-single-coverage.xml、NativeCoverage、E2ECoverage。

该分母只包含 PDB 提供的新增／修改可执行行，不包含删除的体力代码；不是全项目覆盖率，也不能与 v1.2 的另一范围百分比直接比较。没有采集分支百分比、蓝图百分比或网络运行的覆盖率。网络交互有真实 E2E 场景结果。

summarize_timing_results.py 核对全部结果、三个视角的 Pivot 播放／退出和原有资产 hash，再合并覆盖率。

## 7. 交付状态与未执行项

7 份本轮开始已修改的伤害 GE、Hero BP、AM_Attack01～05 均与开始备份 hash 一致。指定两份 AnimBP 与 Sprint Profile 为本轮预期资产修改；没有批量保存其他资产，没有回退或自动提交。

本轮功能与自动化验收已完成。未执行 Cook／打包、独立 Dedicated Server 可执行文件构建、独立进程网络测试、丢包／高于 100ms 延迟压力测试。动画动作的审美、脚步贴地和模型姿态仍需团队按素材标准进行视觉验收；本次没有新增多角度／镜像 Turn 或根运动转向架构。

调参入口均已存在并接好：DA_Hero_SprintAbility 的 MontagePlayRate、MoveCancelOpenTime、HandoffOpenTime／CloseTime、HoldThreshold、MoveIntentThreshold、HandoffNetworkGrace、SprintTurnPlayRate／BlendOutTime、SprintCyclePlayRate。无需用户补接缺失代码。
