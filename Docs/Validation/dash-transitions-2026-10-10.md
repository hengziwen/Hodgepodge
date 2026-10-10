# Dash 到 Idle／移动／Sprint 的连续过渡

日期：2026-10-10。引擎：UE 5.5.4。工作目录：E:/Project/Git/Hodgepodge。

## 问题与定位

修复前使用正式 Main Hero、默认地图和真实 Enhanced Input 注入，分别采集静止短按、短按后移动、长按后移动、尾段输入移动、尾段长按衔接和持续长按六种场景。

碰撞体 Z、Mesh 相对 Z 和模型 Root 高度始终稳定。Dash 尾段骨盆平均高度约 84.44cm，Idle 约 80.32cm，视觉下沉约 4.13cm。素材原始 Dash／Idle 末帧高度相同，差异来自动画层：LegIK 和 FootPlacement 使用 `1-FullBodyWeight`，全身 Dash 时关闭贴地，结束后恢复地面修正并插值降低骨盆。

默认 RMS 位移 0.35 秒结束，后摇取消和 Sprint 开放原为 0.5 秒，且 RMS 结束策略把速度设为零。持续移动长按的实际 Sprint 提交约 0.55 秒，衔接采样最低速度为 0。自然动画混出时仍由 Dashing 状态阻塞移动，直到能力完成回调。

## 修复

- HodgeAnimInstance 在 NativeUpdateAnimation 汇总 FullBody 蒙太奇实例（包括混出实例），缓存 LocomotionFootIKAlpha。Dash 权重保留贴地，其他全身动作照常抑制；替换动作时按权重平滑退出。
- 正式动画层仅替换 LegIK Alpha 和 FootPlacement 选择器的 Alpha 来源。UseFootPlacement／DisableLegIK 条件保持；没有给胶囊或 Mesh 增加 Z 补偿，也没有改 Root／骨盆轨迹、骨架或第三方素材。
- Main Profile 和原生默认 MoveCancelOpenTime／HandoffOpenTime 改为 0.35 秒，与位移结束对齐。仍可手动配置更晚的恢复等待；无输入短按仍完整播放，不以位移结束结束 GA。
- 主动移动退出或 Sprint 交接调整本次 RMS 结束策略为 ClampVelocity，沿用已有速度并限制到下一步态上限；无输入结束、取消和死亡路径保留停止规则。
- RMS 名称采用复制一致的 SpecHandle＋SessionId，保持拥有者与服务器来源匹配，避免修改其他技能的来源。
- LocomotionPolicy 暂存本次来源的成功退出速度策略，CMC 在保存帧／网络来源恢复后、引擎清理 RMS 前重施加。记录一秒后过期、解绑／换 Pawn 时清空；死亡和强控制不应用。这避免重放较早的归零结束策略导致速度回退，不直接写入角色速度。
- Packed 移动响应已包含完整权威速度，但 UE 的旧 RMS 纠正桥接仅传递垂直速度。CMC 在服务器纠正包含本次已成功退出的 Dash 来源时，将包内完整速度交给原 ClientAdjustPosition；保留时间戳确认、位置校正和后续重放。动画根运动、相对基座速度、无关 RMS、死亡／控制继续使用原入口，不添加客户端猜测速度或新网络字段。
- 正常蒙太奇混出开始释放自己的移动限制和动作朝向，能力继续等待动画完成；已释放的朝向句柄不再误判为拒绝。
- 回归发现原 Sprint Pivot 定时检查可能晚于 CMC 制动／转向，错过起手速度。Hero 在提交移动前通知活动 Sprint，以来向速度和起手朝向检查；原 TargetData 验证、序号、根运动与生命周期继续沿用。

正式资产只修改 `/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase` 与 `/Game/Main/Character/Hero/Ability/Sprint/DA_Hero_SprintAbility`。标签映射、五个攻击 GA、死亡豁免和其他动画资产未改动。

## 验证方式与命令

外部编译前确认编辑器无 PIE、无脏包并正常关闭。最终 Editor／Game 构建采用以下命令，日志为 Saved/DashTransitions/EditorBuild11.log 和 GameBuild7.log。

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Relationship+Hodge.Movement+Hodge.Facing+Hodge.Rotation+Hodge.Combo+Hodge.Combat+Hodge.HitReaction' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/DashTransitions/Native8' '-abslog=E:/Project/Git/Hodgepodge/Saved/DashTransitions/Native8.log'
```

配置、编译与测试通过已认证本地网关执行：

```powershell
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/configure_dash_transitions.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/prepare_validation_arena.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/run_dash_transitions.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/run_e2e.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/run_combat_integration.py
& E:/Python/python.exe Tools/DashSprint/summarize_dash_transitions.py
```

过渡测试检查角色／Mesh Z、尾段与 Idle 骨盆高度差、持续长按在 RMS→Sprint 区间的最低速度、较早与尾段移动输入的响应，以及后撤动画。普通回归覆盖 F／B／侧向、完整短按、释放／停步、移动取消、可配置延迟窗口和 Pivot；攻击交互验证标签中断和攻击退出 Sprint。

实际数据和最终数量见[机器可读结果](dash-transitions-results-2026-10-10.json)。新原生用例 Hodge.Movement.DashFootIKContinuity 检查 Idle、Dash 入／出混合、非 Dash 全身动作、动作替换和非法权重；其余 34 项原生一并回归。

## 最终结果

- EditorBuild11／GameBuild7 常规构建均退出 0。原生 Native8 共 35 项通过：26 项 Success、9 项 Success with warnings，0 失败、0 未运行；现有警告保留。
- 正式动画层编译为 ERRORS=0、WARNINGS=0，配置结果保存在 Saved/DashTransitions/configuration.json。FootPlacement 的实验性节点说明仍存在。
- 同一最终构建、同一编辑器进程下复跑单人、双客户端 0ms、双客户端 100ms，每种条件 7 项过渡＋10 项状态／动画＋3 项攻击交互，共 60 项，全部通过。异步执行开始不计为通过；最终 JSON 的场景数量、error 和 passed 均由汇总脚本校验。
- Dash 尾段减 Idle 的骨盆高度差：单人 −0.10cm、0ms 联机 −0.08cm、100ms 联机 −0.12cm；修复前为 +4.13cm。胶囊 Z 和 Mesh 相对 Z 连续，未增加 Z 补偿。
- 三种条件持续长按衔接的拥有者最低速度均为 720cm/s，服务器速度连续性断言也通过。早期和尾段移动输入均无需等待整段蒙太奇完成；无方向短按仍完整播放。
- 可配置的延后 HoldThreshold／HandoffOpenTime、服务器批准、释放／停步、正常 Pivot 播放及回循环、标签攻击取消和攻击退出 Sprint 均通过。
- 最终停止 PIE、关闭网络模拟、重载临时 Profile；PIE 世界和脏包均为 0。保存的 Duration／MoveCancelOpenTime／HandoffOpenTime 均为 0.35，NoMoveIntentGrace=0.08、BlockedExitDelay=0.4 未调大。最终状态记录在 Saved/DashTransitions/final-state.json。
- 与任务开始的 33 份 Main 资产 hash 对照，仅两份关联资产改变，其他 31 份保持一致。未提交 Git；生成文件、运行日志与失败探针均留在 Saved，不纳入源码交付。

最终串行运行记录：Saved/DashTransitions/final-runtime-arena.log。配置指南与设计文档已同步默认窗口、IK 混合和网络纠正规则。本轮无剩余实现步骤；发布验证与全地形视觉验收见下方边界。

## 调试中发现的问题

首次构建把项目朝向句柄误当作 FGuid 调用了 Invalidate，已改为清空结构体。第二次链接仍有编辑器占用 DLL，确认无脏包后再次正常关闭，后续常规构建通过。补充纠正测试时访问了 UE 受保护的响应容器接口，改为独立测试响应容器，未扩大 Runtime 的公开接口。新增纠正测试的移动组件最初未激活，修正夹具后复跑通过；Native6 的失败记录保留，最终使用 Native8（在移除临时检测宽限后复跑）。

读取动画图时，导出工具传入不存在的资产触发 UnrealExporter 的 Object 断言；没有用户未保存内容，重新打开编辑器后继续。之后作者／检查脚本对目标资产做有效性检查。

第一次普通回归的九项移动场景通过，但 Pivot 未启动，报告保留为 single-0ms-pivot-timing-first.json。已修复输入检查晚于物理制动的问题，早期报告不计入最终通过数量。

网络探针首次使用固定响应时间，未区分 Slate 帧末注入与游戏处理帧，改为记录实际注入时刻并计入处理帧与网络到达。专用服务器未持续计算骨骼，静止骨盆读数不作为高度验收；客户端高度与服务器位置／速度分别检查。

原探针捕获拥有者在成功交接后从 800cm/s 回退到约 97cm/s，服务器已维持 720cm/s。该探针当时尚未隔离第二名玩家，不能作为重放导致降速的独立证据。针对源码中的保存帧来源恢复路径，已补仅匹配本次 RMS 的成功退出策略恢复；最终以隔离场景复跑验收。fixed-network-0ms-replay-first.json 保留，不计入通过数量。

100ms 的延后衔接场景遇到约 0.4 秒长帧，客户端在窗口内请求，但服务器下次更新已经越过关闭点，未建立授权。远端服务器改用 `[HandoffOpenTime, HandoffCloseTime+HandoffNetworkGrace]` 的有限授权窗口，拥有者仍只在原配置窗口触发；活动 Dash、会话、按住、方向和状态验证不变。原生用例检查不能提前授权和宽限到期。首次报告保留为 network-100ms-handoff-hitch-first.json。

100ms 下部分速度探针和 Pivot 失败实际来自测试角色碰撞：第二名玩家位于冲刺路线，服务器探针只设置 Pawn 通道为 Overlap，客户端胶囊仍阻挡。另有长跑超过测试地面边界的问题。新增 prepare_validation_arena.py，将第二名玩家在三种世界中移至路线外 25m，并扩大测试地面至 100m。此前临时加入的 Sprint 停步／阻挡宽限已移除，正常退出阈值保留；临时参数覆盖重载丢弃，相关旧报告不计入最终数量。

速度复核期间检查了 UE RMS 纠正入口只传垂直分量的行为，并补充完整权威速度处理；本次速度失败同时受上述探针碰撞影响，不能全部归因于纠正入口。状态回归通过时仍不能视为连续过渡通过；失败报告保留为 fixed-network-100ms-planar-correction-first.json。已针对包内完整速度增加入口处理，并补原生测试验证同名退出来源、过期纠正拒绝及无关 RMS 不受影响。

## 边界

脚部贴地修正与原素材自身的蹲伏、起身、步幅变化不同；本轮处理过渡时重新启用修正造成的整体高度跳变，不抹掉动作本身。手动调大后摇／衔接时间会有意保留等待，未取消这些配置能力。

没有恢复体力或增加冷却，没有更改旋转锁／锁敌服务／CameraMode。未执行 Cook／打包、独立进程联机、丢包或超过 100ms 的网络压力、全地形与全视角视觉验收；本轮没有采集代码行／分支或蓝图覆盖率百分比。
