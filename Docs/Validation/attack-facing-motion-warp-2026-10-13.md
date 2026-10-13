# 攻击前摇旋转 Motion Warping 实现与验证

日期：2026-10-13；UE 5.5.4；Windows x64 Development；项目 Hodgepodge。

## 1. 交付范围

依据 [实施契约](../Design/attack-facing-motion-warp.md) 完成 AW-01～07 的首版实现。用户确认：自由状态无移动输入时按镜头水平朝向出招。方向优先级为有效锁定目标、有效移动意图、镜头水平朝向；无控制器才使用角色朝向。

已实现配置与窗口校验、即时播放、固定方向 TargetData、服务器验证、有界等待、根运动旋转执行、动作朝向占用、预测取消暂停／恢复、回放快照与资源身份清理。

只为 DA_Attack_1 开启。其余四段保持关闭，五段连击完整回归；不启用平移吸附、自动选敌、持续追踪目标、攻击旋转锁或空中追击。未改锁定镜头与 Dash RMS 业务设计。

没有自动提交。原有工作区包含大量其他未提交代码和资产，本轮开始状态和目标文件备份保存在 `Saved/attack-warp-implementation-status-before.txt`、`Saved/AttackWarp/Before`。不能把整个 Git diff 都归入本轮。

## 2. 代码与接口

- `Public/Private/Combat/HodgeAttackFacingTypes`：执行策略、方向来源、配置、固定 Yaw 限幅、GAS TargetData 序列化。
- `Public/Private/Component/HodgeAttackMotionWarpingComponent`：正常和 SavedMove 根运动求值前准备目标；按本次执行身份清理专用目标及修正器。
- `Public/Private/Animation/HodgeRootMotionModifier_AttackFacing`：Skew Warp 的旋转执行，禁用 Translation，实际倍率换算，强约束检查。
- `HodgeCharacterRotationComponent`：MontageWarp 占用、暂停、恢复、权威状态退役；驱动枚举 Movement／Controller／ActionDirection 保留。
- `HodgeCharacterMovementComponent`：Warp 占用期间排除物理动作转向；移动数据、SavedMove 合并与回放保持执行身份。
- `HodgeGameplayAbility_Definition`：方向采样、上传、缓存消费、校验、超时、原蒙太奇启动、Recovery 与取消／结束资源清理。
- `HodgeAbilitySystemComponent`：固定方向一次消费及有界退役记录，阻止重复、非法和迟到上传；保留其他 TargetData 流程。
- `HodgeCombatComponentBase`：方向未就绪时保留同次连段授权；服务器发起事件携带同一方向载荷。
- `HodgeAbilityDefinition`：FacingWarp 参数与唯一原生通知窗口校验。
- `HodgeCombatCharacter`：原生默认 AttackMotionWarpingComponent。
- `HodgeAnimationAuthoringLibrary`：指定资产的 Warp 通知制作与配置校验。
- `HodgeCombatValidationLibrary`：仅编辑器 PIE 的取消恢复故障注入；游戏 Tick 执行，避免 Slate Tick 直接触发跨世界 GAS 调用。

模块仍为既有 Runtime 与 Editor 工具模块，增加原生 MotionWarping 依赖和插件启用项，没有新建业务模块。ASC 文件保留原有 UTF-16 编码。

新测试代码：`Source/Hodgepodge/Private/Tests/HodgeAttackWarpTests.cpp`、`HodgeAttackWarpTestTypes.h/.cpp`。运行工具见 [工具说明](../../Tools/AttackWarp/README.md)。

## 3. 正式资产与调整入口

已保存两份资产：

- `/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1`
- `/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage`

在 DA_Attack_1 的 FacingWarp 中调整：bEnabled=true、WarpTargetName=HodgeAttackFacing、MaxCorrectionAngle=90°、MaxRotationRate=1080°/s、RotationTimeMultiplier=1、TargetDataTimeout=1s、StartYawTolerance=45°、TargetYawTolerance=30°。

Montage 通知轨唯一 Motion Warping 通知使用 `Hodge Attack Facing Warp`；源时间 **0～0.1 秒**。首次命中约 0.1333 秒，Recovery 约 0.6 秒。窗口末尾只结束 Warp，动作占用持续到 Recovery 或退出。不得将窗口延伸到首刀之后或与 Rotation.Locked 重叠。角色仍保持 `bAllowPhysicsRotationDuringAnimRootMotion=false`。

Hero 与首招 GA 蓝图已执行编译，冷启动确认原生 Warp 组件存在；其余四段配置只读确认关闭。参数导出见 `Saved/AttackWarp/configuration-final.json`，初次 `configuration.json` 的 0.0001 起点是历史记录，已被最终 0 起点替代。

## 4. 实际构建和测试结果

Editor、Game 常规构建均退出 **0**；编辑器关闭后构建，再冷启动。没有使用 Live Coding。构建日志：`Saved/AttackWarp/editor-build.log`、`game-build.log`。

原生自动化共 **49/49 Success**，其中新增攻击 Warp **9/9**。UE 报告分类为 23 项成功、26 项成功伴警告，失败 0、未运行 0。不能将其表述为零警告。覆盖攻击 Warp、Combo、Facing、Rotation、LockOn、Movement 与 UI 相关回归。

真实 PIE 攻击专项：单人 **72/72**；Listen Server 双玩家 **83/83**；PIE Dedicated 双玩家 **83/83**；Listen Server 加 `Net PktLag=100` **83/83**。共 **321** 条实际断言，检查所有断言结果而非只检查网关返回成功。

五段真实攻击输入回归：四种模式各 **1/1**，验证五段 Montage 均出现。

既有锁定回归：四种模式各 **20/20**，共 **80** 条，覆盖获取、保持、鼠标切换、解除、Dash、Sprint、朝向恢复、目标死亡。Pivot 的原生根运动与时序测试通过；本轮未新增人工观察 Pivot 动画的视觉验收。

原生报告：`Saved/AttackWarp/Automation/index.json`；日志 `automation.log`。E2E 报告按模式保存在 `Saved/AttackWarp/*-attack-warp-e2e.json`、`*-combo-regression.json`、`*-e2e.json`，包含逐项结果和连续采样。仓库内摘要和代码 hash 见 [机器可读结果](attack-facing-motion-warp-2026-10-13-results.json)。

## 5. 需求到测试的追溯

- AW-01 即时播放：PIE 自由／锁定输入检查，按游戏帧数验证启动；不以首轮资源加载墙钟耗时判断功能延迟。
- AW-02 固定方向：`FixedYaw`、`PayloadValidation`；PIE 镜头 ±45°、有效移动输入、锁定和 120°目标限幅到90°。
- AW-03 窗口与占用：`WindowContract`、`RootExecutor`；PIE 窗口后不继续转向、Recovery 释放、0.5／2 倍率。
- AW-04 身份与强约束：`LeaseAndSuspension`、`ReplayAndOwnership`、`PayloadValidation`、`LateAuthorityDoesNotRevive`；PIE Dash 取消与锁定回归。
- AW-05 网络：`TargetDataRetirement`、`PayloadValidation`、`ReplayAndOwnership`；双玩家权威／拥有者／模拟代理及延迟场景。
- AW-06 取消恢复：`LeaseAndSuspension`、`ReplayAndOwnership`；PIE 移动取消与编辑器注入的拒绝恢复，分别恢复到窗口内／窗口后。
- AW-07 回归：九项新原生测试、49项完整相关组、四模式连段和锁定回归。

这属于需求与用例覆盖记录，未使用编译器行覆盖率工具，不提供虚构的代码覆盖百分比。

## 6. 测试中发现并修复的问题

1. 服务器在等待方向数据时被基础移动提前转身：增加当前姿态占用，验证方向后替换为固定目标。
2. 首帧低帧率可直接跨过非零 Warp 起点：正式首招窗口从 0 开始；保留首刀前结束约束。
3. 低帧率或网络 Montage 位置修正可能跳过 Recovery 通知：增加源位置检查，补偿释放本次朝向占用。
4. Recovery 已释放但 GA 尚活动时，迟到权威快照重新激活旧朝向：增加本次执行退役标识，原生回归和网络实测通过。
5. 延迟方向消费和普通连段结束混用授权完成逻辑：只在本次延迟启动／启动失败时完成准备流程，五段四模式回归通过。

构建阶段修复了依赖可见性、Editor 对 MotionWarping 的直接链接及原生 API 名称差异。测试工具中还修正了 Rotator 参数顺序、Slate 调用上下文、连续采样重复计数和文件报告写入竞态。这些属于测试／集成问题，不据此声称项目原有玩法错误。

仍存在既有 Montage 同时刻 Branching Point 重叠警告（含0.6秒、3.166667秒），以及已有工具／项目警告。本次没有批量重写旧通知；朝向恢复已做源位置补偿，其他通知语义仍应按独立任务评估。

## 7. 验证边界与后续事项

- 取消拒绝测试使用真实预测取消／恢复函数和真实权威 Montage 位置，但**拒绝结果由 Editor 探针注入**，不宣称已测真实网络拒绝 RPC 的全部路径。
- SavedMove 历史快照、位置回退、所有权清理由原生集成测试覆盖；没有对网络丢包导致的每次真实移动纠正做逐事件统计。
- 仅测试本机多世界 PIE。未构建独立 Server 目标，未做跨机器联机、严重丢包、长时间压力、Cook 或打包。
- 输入通过 Enhanced Input 注入，证明实际能力和动画链路；最终手感仍需玩家人工调试，90°／1080°/s 是可配置基线，不保证所有动作的大角度都完全对准。
- DA_Attack_2～5 独立 Warp 窗口尚未制作；尤其第2段首刀前摇短，不能照搬首招窗口。
- 平移攻击吸附、自动目标选择和空中追击按既定范围保持后续阶段。
- 退役记录容量与测试探针动作名称属于协议／测试约束；正式玩法方向和窗口没有依赖测试地图或探针。

测试完成已停止 PIE、恢复原地图和性能设置、t.MaxFPS=0，检查未保存地图／资产均为0。原始完整 Play Settings 未记录，恢复单人测试基线；无自动提交。

## 8. 本机复跑命令

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.AttackWarp+Hodge.Combo+Hodge.Facing+Hodge.Rotation+Hodge.LockOn+Hodge.Movement+Hodge.UI' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/AttackWarp/Automation' '-abslog=E:/Project/Git/Hodgepodge/Saved/AttackWarp/automation.log'
& E:/Python/python.exe Tools/AttackWarp/run_batch.py single listen dedicated listen-lag
$env:HODGE_WARP_RUNNER='combo'
& E:/Python/python.exe Tools/AttackWarp/run_batch.py single listen dedicated listen-lag
Remove-Item Env:HODGE_WARP_RUNNER
& E:/Python/python.exe Tools/LockOn/run_validation_batch.py --kind lock --work E:/Project/Git/Hodgepodge/Saved/AttackWarp single listen dedicated listen-lag
```

运行 PIE 工具前按工具说明启动本机已构建编辑器；常规构建前关闭已保存编辑器。复跑命令中的绝对引擎路径以本机为准。
