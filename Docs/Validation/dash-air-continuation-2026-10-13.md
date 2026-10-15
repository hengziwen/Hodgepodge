# Dash 离地续冲实现与验证

日期：2026-10-13；UE 5.5.4；Win64 Development；设计版本 v1.9。

## 1. 已确认需求与范围

- DA-AIR-01：Dash 只能地面起手；准备阶段仍受地面限制。
- DA-AIR-02：已经提交的 Dash 冲出台阶进入 Falling 时，继续原定剩余水平位移并正常下落，不因失去地面结束。
- DA-AIR-03：RMS 结束或清理不清零负的下落 Z；落地恢复地面收尾，正常移动交接保留既有策略。
- DA-AIR-04：落地不重播或延长 Dash；死亡、强控制、移动停止、解绑和其他不支持模式仍限制执行。
- DA-AIR-05：用户本轮确认，Dash 仍活动、有效窗口内落地、Shift 未松开且有移动意图时允许 Sprint。空中不提交或消费请求，动作／窗口过期不自动补进 Sprint。
- DA-AIR-06：保留无冷却／无体力、F／B 语义、既有动画时序、后摇取消和旋转架构；不新增空中起手能力，不修改正式资产。

## 2. Lyra 对照与适配

先前只读检查了本机 Lyra 的 `/ShooterCore/Game/Dash/GA_Hero_Dash`、`GE_HeroDash_Cooldown` 和 `GCNL_Dash`，蓝图图连接和默认对象导出保存在 `Saved/DashAirAudit`；本轮继续核对默认值和 Character 的移动模式标签更新入口。

Lyra 使用 LocalPredicted／Independent GA，分离动画和 ConstantForce RMS；加法式 RMS 默认 Strength=1850、Duration=0.25s，AbilityDuration=0.55s，结束 ClampVelocity=1000。Dash 图没有持续地面检查；它有1.5秒冷却，与本项目契约不同。

本项目复用现有 GAS、原生根运动任务和引擎限速清理机制，保留覆盖式水平位移和 Distance／Duration 配置。使用原生任务 `bEnableGravity=true` 对应 IgnoreZAccumulate，CMC 继续计算垂直重力；不迁入四方向动画、加法式速度或冷却。

## 3. 根因与代码改动

原先 `MovementAction::IsMovementValid()` 与起手条件都要求 `IsMovingOnGround()`，0.01s 定时检查在 Falling 时调用默认取消 Finish；Dash.EndAbility 随即 EndTask 移除 RMS。位移因此提前终止，默认 SetVelocity=ZeroVector 也可能清零下落速度。

- `HodgeGameplayAbility_MovementAction.h/.cpp`：新增执行模式虚接口，公共身份／约束检查保留；默认仍要求地面，CanActivateAbility 不放宽。
- `HodgeGameplayAbility_Dash.h/.cpp`：已提交后允许 Falling；原生 RMS 开启重力兼容，登记本次来源速度记录；仅地面打开／提交 Sprint 衔接，CompleteHandoff 再检查地面。
- `HodgeLocomotionPolicyComponent.h/.cpp`：活动来源记录包括速度上限、位移时长加有限回放余量、仅空中限速标记。读取当前物理模式选择停止／限速；正常成功交接覆盖活动记录。
- `HodgeCharacterMovementComponent.cpp`：在 Super.PerformMovement 清理前对活动与待加入 RMS 应用来源策略；地面保留原 SetVelocity 零速度，Falling 用 ClampVelocity 保留负 Z。已有保存帧与校正入口继续查询同一来源记录。
- `Private/Tests/HodgeDashAirTests.cpp`：四项新原生测试，使用真实原生策略组件和 CMC，不创建重复组件。
- `Tools/DashAir`：真实输入、临时平台、批量模式启动及报告导出工具。

记录按本次 Spec／Session 的 RMS 名称区分，有限时间过期，解绑清空；不修改不匹配的来源。沿用同一 Runtime 模块，成员实现保留在各自主 `.cpp`，新增逻辑有中文注释。

## 4. 实际结果与需求追溯

最终常规 Editor、Game 构建均退出 **0**，关闭编辑器后构建并冷启动，无 Live Coding。日志分别为 `Saved/DashAirImplementation/editor-build.log`、`game-build.log`。

相关原生自动化 **45/45 Success**：17项成功、28项成功伴警告，失败0、未运行0。包含新增四项和 Movement／Facing／Rotation／LockOn／AttackWarp 回归。自动化进程退出0并不能单独证明用例通过，本轮另外检查了报告。

新测试对应：

- Eligibility：DA-AIR-01／02，地面准备、未提交离地拒绝、已提交 Falling 继续、Sprint 空中拒绝以及 None／Flying／Swimming 拒绝。
- Constraints：DA-AIR-04，Death／Controlled／MovementStopped 约束和 Avatar 解绑失效。
- FinishPolicy：DA-AIR-03，地面／空中／再次落地切换、成功交接覆盖、非法时长、来源隔离和解绑。
- Physics：DA-AIR-02／03，真实 CMC 覆盖式 RMS 保持水平速度、重力下降、实际源收尾及清理；测得 Z 从 −409.8 到 −419.6，水平速度限幅。

Dash 离地专项 PIE：single **36/36**、listen **36/36**、dedicated **41/41**、listen-lag **36/36**，共 **149** 条断言。

用例为平地短按、早期／末期走出台阶、有效窗口内落地衔接、落地过晚不补 Sprint、空中拒绝起手。前五种通过真实输入和实际碰撞平台运动，只有空中拒绝用例主动设置 Falling。Dedicated 两客户端额外确认模拟代理的位移和 Dash 动画；拥有者／权威角色检查真实活动 GA。

既有锁定／Dash／Sprint 回归四模式各 **20/20**，共 **80** 条，覆盖目标获取／切换／解除、自由朝向、Dash、长按 Sprint 和目标死亡。专项及回归合计 **229** 条 PIE 断言。

原始报告与采样位于 `Saved/DashAirImplementation/*-dash-air-e2e.json`、`*-e2e.json`；原生报告 `Automation/index.json`；仓库摘要与代码指纹见 [结果 JSON](dash-air-continuation-2026-10-13-results.json)。未运行行覆盖率工具，不提供虚构覆盖百分比。

## 5. 实施过程中的问题与处理

初次构建因测试初始化列表推导为不可复制的 FNativeGameplayTag 失败，改为显式 FGameplayTag 数组后修复。

物理测试夹具最初重复创建策略组件，记录与 CMC 查找的组件不同；改用角色原生组件。RMS 提交存在引擎复制，断言改为读取实际源；随后检查覆盖活动和待加入数组，保证首个物理步也取得正确收尾策略。保留失败日志，最终四项均通过。

Python 不公开内部 SpawnActor 蓝图入口的方法名，测试改用已有 UFunction 反射调用，在各 PIE 世界创建相同临时平台；没有为测试新增 Runtime 类或保存地图。

既有重叠 Branching Point、工具／服务连接等警告保留，未通过删除无关插件或批量重写资产掩盖它们。

## 6. 边界与未验证项

- Dedicated 为本机多世界 PIE 专服，未构建独立 Server，也未运行跨机器、丢包、Cook 或打包。
- 强约束和解绑由原生测试覆盖；本轮未专项复跑空中真实受击／死亡的完整联机流程。
- 准备阶段离地仍会拒绝提交；仅已提交 Dash 可续冲。真实高延迟下的准备期边缘竞争需按后续网络专项评估，不能直接信任客户端自报地面。
- 不保证碰墙仍走满配置距离，继续服从 CMC 碰撞；续冲不等于穿墙或飞行。
- 有效落地衔接要求 Dash 仍活动。蒙太奇或动作已结束，即使配置的关闭时间数值更晚，也不会重新激活 Sprint。

测试后停止 PIE，恢复原 `/Game/ThirdPerson/Maps/ThirdPersonMap`、性能设置与原 t.MaxFPS=0；未保存包为0。原始完整 Play Settings 没有记录，恢复单人基线。正式 Content 无差异，已有开发规则改动保留，未自动提交。

## 7. 实际命令与复跑

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Movement+Hodge.Facing+Hodge.Rotation+Hodge.LockOn+Hodge.AttackWarp' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/DashAirImplementation/Automation' '-abslog=E:/Project/Git/Hodgepodge/Saved/DashAirImplementation/automation.log'
& E:/Python/python.exe Tools/DashAir/run_batch.py single
& E:/Python/python.exe Tools/DashAir/run_batch.py listen dedicated listen-lag
& E:/Python/python.exe Tools/LockOn/run_validation_batch.py --kind lock --work E:/Project/Git/Hodgepodge/Saved/DashAirImplementation single listen dedicated listen-lag
& E:/Python/python.exe Tools/DashAir/export_results.py
```

构建前关闭已保存编辑器，PIE 工具前启动本机已构建编辑器。路径和恢复说明见 [工具说明](../../Tools/DashAir/README.md)。
