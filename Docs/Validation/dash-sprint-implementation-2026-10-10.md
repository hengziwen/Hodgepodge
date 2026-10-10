# Dash／Sprint 实现与验证

日期：2026-10-10。UE 5.5.4、Win64 Development。设计：[Dash／Sprint v1.2](../Design/dash-sprint-system.md)；实际配置：[配置与接口](../Guides/dash-sprint-configuration.md)；机器可读结果：[测试与覆盖率 JSON](dash-sprint-results-2026-10-10.json)。

## 1. 实施范围

- 默认启用角色后向锥 B 后撤，半角 45°、包含边界；保持起手面向，位移沿角色反向。其他方向用 F，关闭 B 后全部方向用 F。
- 当前旋转攻击未使用旋转锁；不添加锁交接接口。Dash 在真实攻击取消窗口内准备并提交，窗口外拒绝且不扣体力。
- MovementAction／Dash／Sprint GA、按键会话、共享体力、初始化与恢复阻塞、速度修正、CMC 预算／回放、防御窗口和 GameplayCue 已接入 Main。
- StaminaSet 属于 PlayerState ASC；Pawn 组件经既有初始化解绑，重复绑定不回满。StatProfile 新增最大体力曲线与默认关闭的重生补满选项。
- F／B 源素材是带根运动的 AnimSequence；制作 Main 原地副本及 FullBody 蒙太奇，使用 RMS 单一主位移。Source 动画、现有攻击／受击配置保留。
- 正式主图／固定层新增 3 个 Cycle 入口、3 个 Pivot 入口、1 个准入节点，保留 Free／ReservedStrafe、原八向资源、轻反馈 Group 和 IK。
- 真实 Melee 伤害提交前消费 Defense 决议；正伤害接受与强控制分别结束 Sprint。完美判断在伤害／Impact 前，奖励与表现不拥有防御权限。
- 拥有者、服务器和观察者沿用 GAS／CMC；没有新建 Runtime 模块、旋转多播或第二个移动数据容器。

本轮不自动提交 Git。完整锁定目标服务、锁定 CameraMode、空中 Dash、全局慢动作及打包继续排除。

## 2. 构建与蓝图

实际执行常规构建，未以 Live Coding 替代：

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=2
```

最终 EditorBuild20.log、GameBuild5.log 均退出 0，日志位于 Saved/DashSprintImplementation。原有插件依赖、IncludeOrder 与 UI 弃用提示保留，不通过升级引擎／插件或移除无关模块解决。

configure_hero.py 对两份正式 AnimBP 编译：ERRORS=0、WARNINGS=0；保留 FootPlacement 实验性信息与未用引脚信息。创建并保存指定能力、Profile、动画、Cue、输入与授予资产；未批量保存其他脏包。

## 3. 原生自动化

最终 Native6/index.json：**30 项，21 无警告成功、9 带日志警告成功，失败 0、未运行 0**。

新增 Hodge.Movement 五组覆盖：F／B 与锥形边界、Actor 相对方向、禁用 B／零／非有限输入、会话重试与释放、目标数据序列化、体力截断与最后预算区间、未知移动键、多来源速度修正、回放防重复收费、防御 `[Start, End)` 和完美奖励次数。

Hodge.Facing.ActivationPair 覆盖同一来源／执行／方向的两份记录、另一执行、另一来源及释放后状态；同时回归 Hodge.Facing、Rotation、Combo、Combat、HitReaction。

实际参数：

```text
UnrealEditor-Cmd.exe Hodgepodge.uproject -unattended -NullRHI -nosplash
-ExecCmds="Automation RunTests Hodge.Movement+Hodge.Facing+Hodge.Rotation+Hodge.Combo+Hodge.Combat+Hodge.HitReaction"
-TestExit="Automation Test Queue Empty"
-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/DashSprintImplementation/Native6
```

以 JSON 的 failed／notRun 判定，不能只看退出码。早期失败与夹具修复报告保留在 Saved，未用失败结果替代最终通过依据。

## 4. 真实 PIE 与联机

正式 Main Hero、真实 IA_Move／IA_Sprint、实际 GA、RMS、Montage、AnimBP 和服务器属性，不以普通移动位移替代 Dash 验证。

- 单人 PIE、双客户端 0ms、双客户端 100ms，各 7 个移动场景，共 **21 个通过**：F 点按、B 点按、侧向 F、长按 Sprint、松开退出、无移动输入退出、体力不足拒绝。
- 双客户端 0／100ms 各 3 个攻击集成场景，共 **6 个通过**：取消窗口外拒绝且不付费、窗口内真实 Dash 取消攻击并付一次体力、攻击结束 Sprint。允许取消时同时确认当前旋转没有锁定。
- 双客户端 0／100ms 各 2 个真实 Melee 场景，共 **4 个通过**：实际正伤害仅轻反馈时退出 Sprint、零伤害 ExplicitControl／HitStun 时退出 Sprint。

累计 **31 个场景通过**。移动测试检查拥有者／服务器／观察者；Dash 同时断言服务器实际提交、一次固定费用和 F／B 变体，Sprint 检查真实高速循环资源。受击测试只临时改隔离的 CodexText 探针，结束恢复配置且不保存探针包。

100ms 是每个 NetDriver 的模拟包延迟，不声明真实 RTT=100ms。双客户端使用 Play As Client 与同进程 PIE 服务器，不等于独立 Dedicated Server 可执行文件或跨机器测试。

可复跑脚本见 [Tools/DashSprint](../../Tools/DashSprint/README.md)。原始逐帧数据为 Saved/DashSprintImplementation 的 single／network／combat／hit-exit JSON；结束清理监听、输入、临时授予及 NetEmulation。

## 5. 覆盖率

复用已有 OpenCppCoverage 0.9.9.0，以 MSVC PDB 采集编译行；原生 Native6 与最终单人 E2E 合并。Runtime 本轮差异及新增文件可测行 **608／736，82.61%**。

统计包括新增文件，排除测试源码；不是整个项目覆盖率。未把未插桩的双客户端结果算入覆盖率，也不声称分支覆盖或蓝图节点覆盖率。优化／内联可能合并源行，数字仅适用于工具可测行。

原始 XML／HTML 位于 Saved/DashSprintImplementation/native-coverage.xml、e2e-single-coverage.xml、NativeCoverage、E2ECoverage。计算脚本 summarize_results.py 校验场景数量、错误为空并合并命中行，JSON 逐文件列出未覆盖行。

## 6. 发现的问题与修复

1. 原素材不是蒙太奇且启用了根运动：制作明确的原地副本及 FullBody 蒙太奇，不叠加根位移与 RMS。
2. Pivot 属性节点使用外部目标／保留旧变量 GUID：改为正确的 Self 引用、清 GUID、恢复输出连接；正式图随后编译通过。
3. 同帧方向与 Dash 回调先后不确定：Started 同步采样当前 Move Action 值，网络数据携带起手 Actor Yaw，保持 F／B 语义。
4. 客户端与服务器同一激活的朝向记录互相覆盖，服务器未提交：映射服务器 Movement 执行身份，并用 IsActionFacingApplied 按来源／执行／方向关联；另一执行或来源不能确认旧动作。
5. 双客户端测试仅看位移会把普通行走当作 Dash：加强断言服务器费用、提交与变体。测试复位改为通过模式请求对齐并等待，避免直接改 Actor 旋转后被旧移动回放覆盖。
6. 测试夹具错误 Outer／抽象组件、Game 公开头缺少 ASC 完整类型、DOREPLIFETIME 参数命名等：修正夹具与依赖，Editor／Game 和最终原生回归通过。
7. Python InputActionValue 注入不能可靠保留布尔值：测试改用引擎的向量注入包装，仍走真实 Enhanced Input Started／Completed 路径。

## 7. 实现假设与未覆盖项

- 首版采用恒定 RMS，暂不提供速度曲线、MontageRootMotion 切换、空中／转向中改道。参数与实际 API 以配置指南为准。
- 默认平滑起手；准备不无敌、不收费。跨端起手面向误差超过 90°拒绝，极端抖动／延迟的容错仍需压力测试。
- 持续体力由移动模拟前的有效预算结算，客户端是预算估计；尚未做大规模角色数下的 GE Spec 分配性能基准。
- Niagara 复用现有折射与 Hero Trail，可替换；没有把艺术观感、脚相位、镜像和所有角度素材宣称为最终品质验收。
- 防御窗口边界／奖励次数有原生验证，Melee 前置入口已接入；没有用端到端结果宣称所有未来投射物、范围伤害、减益入口已接入。新增入口必须消费公共防御结果。
- 没有跨机器、长期丢包／抖动、完整死亡／换 Pawn 的所有网络组合压力、Cook 或打包。没有 100% 异常路径／分支覆盖率。

## 8. 工作区保护

最初七份用户资产哈希全部保持不变：GE_MeleeDamage_Instant、五份攻击蒙太奇、BP_Hero_Pover。本轮只保存指定 Main 动画图、授予／PawnData／输入及新增资源。

最终清理确认 PIE 为空、脏包为空、NetEmulation Off；覆盖率编辑器正常退出。生成文件、缓存、原始日志及覆盖率工具不纳入源码；代码、文档与正式资源留在工作区供评审，未自动提交／推送。
