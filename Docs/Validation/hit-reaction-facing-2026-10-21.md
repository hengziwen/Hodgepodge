# 受击自动朝向：分阶段实施与验证

创建：2026-10-21；最后核对：2026-10-23，Asia/Shanghai。工程：Hodgepodge；引擎：UE5.5.4。

当前结论：含尾重复ACK修复的EditorBuild-13／GameBuild-09、240项原生及59项Python断言已通过；唯一最终FullFinal01十格全部通过，共100个正式场景、170次正式命中，详情见第6.7节。运行发生于2026-10-22，本次仅复核已有证据及同步文档，没有重跑构建或PIE。W05画面、W06真实生命周期余项与W07操作／碰撞评审仍未完成，不能宣布首版整体验收；后续职责和具体入口见第6.8～6.9节。以下历史切片和失败报告继续保留。

## 1. 配置切片阶段范围与授权（此前已完成）

本文第1～5节保留此前独立配置与校验切片的真实记录。当时来源映射、迟到校正及同步契约尚未确认，因此只完成该独立切片；用户后续已批准设计第15.2～15.7节，执行与资产工作按第6节继续。旧209项原生及冷加载证据不能算作新执行代码已经通过。

该阶段已确认首批玩家与普通人形敌人、Boss不启用、有限补转、同级保持／升级接管／空中不重开／硬锁永久冻结、分目标参数及校验兼容；该阶段仅落实独立配置基础及检查。

配置切片阶段未更改PlanBuilder、受击提交／复制、GA、Rotation、CMC／SavedMove及编辑器Details定制，未保存或启用正式Profile、蓝图或Montage，未增加模块／插件。该阶段的范围审计不自动覆盖第6节后续修改；用户已有工作保留，不自动提交或推送。

## 2. 配置切片实现内容

- `Source/Hodgepodge/Public/Data/HodgeHitReactionProfile.h`：新增`FHodgeHitReactionFacingPreset`、`FHodgeHitReactionFacingSettings`及`UHodgeHitReactionProfile.Facing`。配置属于目标Profile，和Compact／兼容动画布局独立。
- `Source/Hodgepodge/Private/Data/HodgeHitReactionProfile.cpp`：同一主文件实现默认值及两个无副作用`Validate`，完整资产`Profile.Validate`纳入`Facing.Validate`；既有`IsDataValid`、PawnData资产检查和现有检查工具可沿原入口取得诊断。未接入运行时`ValidatePlanPresentation`。
- `Source/Hodgepodge/Private/Tests/HodgeHitReactionFacingTests.cpp`：当时新增四项原生配置回归，不加载或保存正式资产，不创建World；后续新增候选测试另按第6节验收。

默认`Facing.bEnabled=false`、`DeadZoneAngle=3°`。玩家默认Light为30°／0.08s，Heavy和首次Launch为60°／0.12s，三组角速率均720°/s。敌人45°／0.08s与90°／0.14s不会自动覆盖旧资产；用户后续已批准仅为Dummy显式配置，备份／修改／保存结果按第6节记录。

配置契约：

1. 启用时，三组最大角必须有限且在(0°,180°]；窗口和角速率必须有限且大于0，不附加未获批准的上限或最小正值。
2. 死区必须有限、非负且不大于三组最大角的最小值，等于边界合法。
3. 禁用时跳过所有休眠数值，不修正或清空它们，也不删除已有错误。
4. 校验返回值仅表示本次是否追加诊断；调用方已有错误不使有效配置返回失败。完整配置错误携带`Facing.Light`、`Facing.Heavy`、`Facing.Launch`或`Facing.DeadZoneAngle`路径。
5. 运行时“某组配置非法只停止可选转向，不取消合法受击”的规则已确认；该阶段仅接入完整资产检查，后续运行时接线及验证属于第6节。

## 3. 配置切片实际验证状态

首轮仅配置字段的Editor常规构建退出0，日志`Saved/HitReactionFacing_20261021/EditorBuild-config.log`；随后追加用户批准的校验和测试，需以最终构建为准。

- 最终Editor和Game常规构建均退出0，日志分别为`Saved/HitReactionFacing_20261021/EditorBuild-validation.log`与`GameBuild-validation.log`；未使用Live Coding。
- 全量`Hodge`原生自动化本轮实际完成209项：123项成功、86项带警告成功，失败／未运行／进行中均0，进程退出0。四项新增配置回归均成功且零警告，报告`Saved/HitReactionFacing_20261021/FullAutomation/index.json`；不是沿用旧205项结果。
- 冷加载旧资产退出0，脚本报告`error=null`及`HODGE_FACING_CONFIG_COLD_AUDIT=PASS`。实际CDO和3份旧Profile均为默认关闭及玩家保守数值，5份PawnData的Profile引用存在，5份角色蓝图的生成类和CDO可加载。新增脏包为空，13份受检资产前后SHA全部一致；未编译或保存蓝图，未启动PIE。日志`ColdProfileAudit.log`及报告`cold-profile-audit.json`位于本轮Saved目录。
- 静态／范围审计通过：16份受保护源码／资产SHA不变，去除新增声明、配置实现及完整检查钩子后，两份Profile旧源码文本与备份完全一致；`git diff --check`退出0，只有既有LF／CRLF提示。工作区从本配置实施前245条变为247条，仅新增测试与本文两条路径，无移除路径。最终文档链接及脚本语法结果另见`config-slice-audit.json`。

四项新增回归的真实过滤器为`Hodge.HitReaction.FacingConfig`，分别覆盖预设数值边界、死区及休眠兼容、诊断追加、完整Profile检查入口。原生通过不证明实际Yaw、动画或联机已经通过。

## 4. 配置切片命令与证据位置

工作目录为`E:/Project/Git/Hodgepodge`。本轮命令结果以第3节及原始日志为准，以下构建／测试命令在各步骤实际完成后记录结果：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/HitReactionFacing_20261021/FullAutomation' '-abslog=E:/Project/Git/Hodgepodge/Saved/HitReactionFacing_20261021/FullAutomation.log'
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecutePythonScript=E:/Project/Git/Hodgepodge/Saved/HitReactionFacing_20261021/inspect_profile_defaults.py' '-abslog=E:/Project/Git/Hodgepodge/Saved/HitReactionFacing_20261021/ColdProfileAudit.log'
& 'E:/Python/python.exe' './Saved/HitReactionFacing_20261021/audit_config_slice.py'
```

构建和两次UE命令均已退出0；审计命令实际结果见第3节。复现须使用新的报告路径，不能覆盖本批证据。两份只读Python脚本的AST语法检查通过。

备份位于`Saved/HitReactionFacing_20261021/ConfigBefore/`；实施前Profile和16份受保护源码／资产SHA分别保存在本轮Saved根目录的`config-before-sha256.csv`与`protected-before-sha256.csv`，冷加载13份资产基线为`cold-assets-before-sha256.csv`。日志、报告和临时只读检查脚本留在Saved，不纳入源码。

## 5. 配置切片之后的工作与依赖

- **P0，战斗程序＋移动／网络程序**：按已批准的来源、时间、独立身份、最小历史移动和量化验收契约补齐代码；常规构建与原生回归通过后才能将执行切片记为完成。已有批准项无需重复确认。
- **P1，战斗程序＋资产制作**：检查并备份Hero／Dummy两份Profile，按已批准参数启用并保存，读回和冷加载核实；其他Profile不启用。
- **P1，战斗程序＋动画／技术美术**：接入实际提交许可、唯一Yaw执行与阶段清理；保持旧选片快照和世界运动，审查玩家／普通敌人的侧背受击资源。
- **P1，QA＋程序**：新增实际Yaw采样，验证角度／速率／时窗、同级与升级、多来源、硬锁永久冻结、首次Launch与空中不重开、真实落地起身恢复，以及取消／死亡／换Pawn／旧回调；再执行Listen与PIE Dedicated三端0／100ms和真实纠正回放。

配置阶段没有执行新功能画面、PIE、联机、Cook／打包及独立Server验证；后续状态按第6节记录。默认关闭下的旧流程回归只能说明兼容性，不能证明新转向行为正确。旧历史退出异常保留，不能因配置阶段进程正常退出声称已修复其原因。

## 6. 后续执行切片：当前实施与待验证项

### 6.1 新增批准记录

2026-10-21用户明确选择并批准：

- 现有近战角色Source在BeginHit冻结双方水平位置，各现有输入模式按最终成功计划映射Light／Heavy／首次Launch，保留原选片与世界运动；无效补转不取消合法受击。
- 同身份的一次权威FinalYaw校正允许瞬时变化；过期转窗不重播，旧段不能作用于新动作、死亡或新Pawn。
- 世界时间截止及局部动画停顿规则；独立ChainId／Segment／Revision，同级首无源也不补开、升级才新段；服务器不回滚受击、Owner本地SavedMove历史重放，活动及边界不合并，不扩展MoveData／RPC。
- 正常补转`720°/s × 有效CMC步长 + 0.05°`、Owner首允许消费步误差≤1°、持续相关模拟代理收到同段冻结后0.25秒内误差≤2°；Mesh另行采样，所有纠正／回放证据保留，单人／Listen／PIE Dedicated的0／100ms及玩家／普通人形目标均属首版验证范围。
- 正式启用仅`/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction`和`/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction`；检查引用、备份后分别按玩家／敌人参数配置和保存，Guardian及其他未分类Profile不启用。

完整规范见设计第15.2～15.7节；此批准记录不代替实际实现或验证。

### 6.2 当前代码与进度

已完成来源水平快照、独立补转状态、纯候选计算、GA成功后提交、Rotation／CMC执行、冻结、一次校正及SavedMove历史回放。含尾重复ACK补丁的EditorBuild-13／GameBuild-09、240项原生及59项Python断言通过，三项源码契约保持不变。最终FullFinal01十格全部通过，正式与预热分别留证；最终两份Profile磁盘保护及六份蓝图保护均通过。既有通过／失败批次保留，画面和未覆盖集成边界未整体验收。

执行切片实施前源码备份位于`Saved/HitReactionFacing_20261021/Execution/Before/`，对应基线为`Execution/source-before-sha256.csv`。该目录与第4节旧配置切片证据分开，不能覆盖旧报告。

审查修复及其真实验证范围：

- **已进入EditorBuild-02及本批原生报告**：补转来源增加目标生命周期代，死亡、直接零生命及解绑使旧候选失效；正常受击完成不误废止合法的新命中。旧PendingHit只停止过期的可选补转，不改原伤害和受击裁决。
- **已在EditorBuild-03／GameBuild-02构建及后续原生中验证**：Legacy三入口的补转保护，以及Final状态首次Live绑定时保留已消费的一次校正身份。`FullAutomation-AfterLegacy`中`FacingMovement.LegacyNativeRotationProtection`与`FacingMovement.FinalCorrectionOnceAndPause`均成功；该批整体仍有另一项失败。
- **Mixed真实失败已修复并重验**：Mixed＋GoodMove回归发现当前朝向锚点覆盖新动作末步Native合法输出；程序仅收窄旧锚点恢复范围，保持原断言期望、参数、网络格式及容差。EditorBuild-04／GameBuild-03均退出0，`FullAutomation-AfterMixed`的228项全部成功，其中Mixed带4条警告成功；原始失败与新结果均保留在第6.3节。
- **RootMotion执行资格已构建并通过原生**：保留Final记录，新增明确退休事实；公开执行资格排除已退休、无目标及RootMotion计划。退休处理集中在`FinishFacingSegment`，`ChangePhase`落地／起身切入根运动时也退休，保留已有FinalYaw及硬锁记录。RootMotion切回同级Impact不能复活旧Final，只有升级才新建段；落地／起身两阶段及合法同级测试已在231项及后续232项批次中通过。
- **首绑复制时序已构建并通过最新原生**：State／Facing可先于ASC依赖到达，旧初始化会无条件解绑并清掉当前Pawn已复制的记录。对照本机`E:/Project/UProject/study/Epic/LyraStarterGame/Source/LyraGame/Character/LyraPawnExtensionComponent.cpp`第105／116行的`InitializeAbilitySystem`：同ASC返回，存在旧ASC才解绑。本项目最小适配为`!ASC.IsExplicitlyNull()`才清理旧绑定，保留真正首绑的权威记录，同时保留stale弱引用的清理语义。
- **十二项FacingMovement及六项FacingLifecycle均已通过原生**：包括真实延迟Extension首绑、首个`PerformMovement`只校正一次、同ASC重复初始化幂等及真实ASC切换／解绑；未通过强制GC伪造stale覆盖。同世界Ready新Pawn隔离已通过；新增`RejectedPlansDoNotOccupy`启用Facing并使用有效来源，证明空接触、阈值不足、霸体、不可中断及动画缺失均不建立新的受击控制或补转身份。引入该组合回归的`AfterRejectedPlanRegression`批次没有改生产Runtime逻辑，后续诊断改动另列。
- **Python命名与NPC入口已核实相应范围**：仅为枚举添加`ScriptName="HodgeHitReactionFacingPresetKind"`，C++类型名及资产路径不变，已验证原生日志中不再出现既有配置Struct的Python重名告警。NPC ExportText使用真实反射`import_text`，SinglePlayerFull02通过现有EnemyCombo意图入口完成10场景；不将其视为玩家物理输入或全部AI战斗验收。
- **PIE按版本逐格推进**：早期全部失败均保留；经历结算日志关联、硬锁缓存、因果标记、专项隔离及原生Yaw锚点修复后，最终冻结版本重新采集全部FullFinal01十格并通过，不用旧批替代最终格。Hero专项只在SameLevel／Upgrade隔离，FullChain保留正式恢复；画面和其他未覆盖集成边界仍待评审。

最新已验证二进制为EditorBuild-13／GameBuild-09，`AfterNativeRejectedRestore`实际240项全部成功，含真正尾重复ACK回归；独立复核未提出新阻断项，59项Python断言通过，未改变数量或验收标准。构建只在命令行使用`-MaxParallelActions=2`，未改配置。P0补丁与规定十格机械矩阵已闭环；W05画面、W06真实生命周期余项及W07表现未完成。

### 6.3 本批实际命令与证据

证据根目录为`Saved/HitReactionFacing_20261021/Execution/`。以下构建均为第4节所列Editor／Game Target的常规Build.bat命令，实际参数以各构建日志为准，未使用Live Coding；UBT日志总时长与外层进程JSON时长分别记录：

- `EditorBuild-01.log`：UBT 417.08秒；`EditorBuild-01.json`记录退出0、外层420.103秒。
- `EditorBuild-02.log`：UBT 124.07秒；`EditorBuild-02.json`记录退出0、外层124.714秒。后续`FullAutomation-BeforeLegacy`使用此Editor二进制。
- `GameBuild-01.log`：UBT 431.12秒；`GameBuild-01.json`记录退出0、外层431.556秒。不能将它视为后续两项生产修复及新增Mixed回归的最终构建结果。
- `EditorBuild-03.log`：UBT 264.96秒；`EditorBuild-03.json`记录退出0、外层265.512秒。后续`FullAutomation-AfterLegacy`使用此Editor二进制。
- `GameBuild-02.log`：UBT 248.03秒；`GameBuild-02.json`记录退出0、外层248.441秒。该构建早于下述Mixed失败的锚点恢复修复。
- `EditorBuild-04.log`：UBT 75.82秒；`EditorBuild-04.json`记录退出0、外层76.380秒。后续`FullAutomation-AfterMixed`使用此Editor二进制。
- `GameBuild-03.log`：UBT 82.71秒；`GameBuild-03.json`记录退出0、外层83.012秒。此构建与EditorBuild-04均早于最新RootMotion执行资格修复。
- `EditorBuild-05.json`记录退出1、外层395.498秒；这是主动中止，不能记为编译报错。`EditorBuild-05-aborted.json`保存中止原因、专属UBT进程PID 4624及完整命令，时刻为2026-10-21 21:33:42（Asia/Shanghai）。中止时GameBuild-04尚未启动，其后重新构建结果如下。
- `EditorBuild-06.log`：UBT 350.53秒；`EditorBuild-06.json`记录退出0、外层351.599秒。后续`FullAutomation-AfterRootMotionAndFirstBind`使用此Editor二进制。
- `GameBuild-04.log`：UBT 355.77秒；`GameBuild-04.json`记录退出0、外层356.071秒。
- `EditorBuild-07.log`：UBT 155.34秒；`EditorBuild-07.json`记录退出0、外层155.819秒，`max_parallel_actions=2`。
- `GameBuild-05.log`：UBT 182.20秒；`GameBuild-05.json`记录退出0、外层182.479秒，`max_parallel_actions=2`。
- `EditorBuild-08.log`：UBT 55.30秒；`EditorBuild-08.json`记录退出0、外层57.383秒，`max_parallel_actions=2`。
- `GameBuild-06.log`：UBT 54.44秒；`GameBuild-06.json`记录退出0、外层54.637秒，`max_parallel_actions=2`。
- `EditorBuild-09.log`：UBT 26.45秒；`EditorBuild-09.json`记录退出0、外层28.620秒，`max_parallel_actions=2`。
- `GameBuild-07.log`：UBT 34.99秒；`GameBuild-07.json`记录退出0、外层35.136秒，`max_parallel_actions=2`。
- `EditorBuild-10.log`：UBT 24.67秒；`EditorBuild-10.json`记录退出0、外层27.130秒，`max_parallel_actions=2`。
- `GameBuild-08.log`：UBT 28.96秒；`GameBuild-08.json`记录退出0、外层29.102秒，`max_parallel_actions=2`。
- `EditorBuild-11.log`：UBT 56.16秒；`EditorBuild-11.json`记录退出0、外层58.519秒，目的为新增真实RMS回放回归、生产修复前编译，不是修后验证。
- `EditorBuild-12.log`：UBT 132.08秒；`EditorBuild-12.json`记录退出0、外层132.527秒，包含原生Yaw锚点生产修复初版，早于尾重复ACK补丁。
- `EditorBuild-13.log`：UBT 87.17秒；`EditorBuild-13.json`记录退出0、外层87.585秒，包含尾重复ACK最小恢复分支与真实回归。
- `GameBuild-09.log`：UBT 79.15秒；`GameBuild-09.json`记录退出0、外层79.378秒，覆盖同一最终C++补丁。

实际全量原生命令：

```powershell
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash -nosound -HodgeCombatValidationExclusive '-abslog=E:/Project/Git/Hodgepodge/Saved/HitReactionFacing_20261021/Execution/FullAutomation-BeforeLegacy.log' '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/HitReactionFacing_20261021/Execution/FullAutomation-BeforeLegacy'
```

`FullAutomation-BeforeLegacy/index.json`实际共226项：132项成功、94项带警告成功，失败／未运行／进行中均0；`FullAutomation-BeforeLegacy-process.json`记录进程退出0、111.554秒。该报告只证明EditorBuild-02二进制当时覆盖的原生范围，不证明最新补丁、正式资产表现或PIE／联机通过。

后续同一原生命令通过`run_stage.ps1 -Stage Native -Suffix '-AfterLegacy'`使用独立日志与报告目录。`FullAutomation-AfterLegacy/index.json`实际共228项：132项成功、95项带警告成功、1项失败，未运行／进行中均0。`FullAutomation-AfterLegacy-process.json`记录UE进程退出0、79.230秒；包装器读取真实报告后拒绝将其计为通过，外层调用退出1。UE正常退出不能覆盖测试失败。

唯一失败为`Hodge.HitReaction.FacingMovement.MixedNewActionAndGoodMoveReplay`，其两条真实等价断言均失败：

- `AckAllOldFacing=0`：实际最终Yaw为25.760000°，Native对照为43.040001°。
- `AckAllOldFacing=1`：实际最终Yaw为25.760000°，Native对照为31.520000°。

原始断言及两侧采样完整保留在`FullAutomation-AfterLegacy.log`与报告中；该批失败当时阻止进入正式资产启用及新功能PIE验收，没有降低断言或覆盖失败报告。

收窄旧锚点恢复后，通过`run_stage.ps1 -Stage Native -Suffix '-AfterMixed'`以新二进制和独立路径重跑。`FullAutomation-AfterMixed/index.json`实际共228项：132项成功、96项带警告成功，失败／未运行／进行中均0；`FullAutomation-AfterMixed-process.json`记录进程退出0、105.632秒。Mixed用例状态为成功，保留4条警告及全部对照样本：`AckAllOldFacing=0`两侧均43.040001°，`AckAllOldFacing=1`两侧均31.520000°，原期望未修改。

该228项通过仅覆盖EditorBuild-04二进制，不能证明后来新增的RootMotion退休、执行资格及首绑复制时序修复通过，也不证明正式资产、PIE或联机表现通过。

`FullAutomation-AfterRootMotionAndFirstBind/index.json`实际共230项：131项成功、96项带警告成功、3项失败，未运行／进行中均0；对应`-process.json`记录UE进程退出0、97.137秒，包装器根据报告拒绝通过，外层调用退出1。三个失败及后续修订分别为：

- `FacingLifecycle.ExcludedPlanFreezesPrior`：用RootMotion Knockback之后的Light请求冒充同级交接，但原降级规则应返回NoImpact。测试改为合法的Knockback→RootMotion Knockback→恢复Impact Knockback→更高Launch，保留同级不复活及升级新段的断言，未修改原降级规则。
- `FacingMovement.ReplicatedFinalBeforeFirstASCBinding`：首次Final计数期望1、实际读到0；原始日志已有Correction事件且Actor朝向正确。
- `FacingMovement.RootMotionRetirementCorrectionAndNativeReplay`：接管前Final计数同样期望1、实际读到0；原始日志已有Correction事件。两项计数测试的输出设备已统一使用原子计数，`GetCount()`先`FlushThreadedLogs()`再读取，保持一次消费期望不变；后续重验结果如下。

三项失败日志和报告均保留，不将日志中的正确Actor朝向覆盖失败状态。新增第12项新Pawn隔离回归及上述测试修订、枚举ScriptName改动均晚于本报告，不能沿用该批结果。

`FullAutomation-AfterBoundaryRegression/index.json`实际共231项：132项成功、99项带警告成功，失败／未运行／进行中均0；对应`-process.json`记录UE进程退出0、69.458秒。全部12项FacingMovement、5项FacingLifecycle均成功，包含此前三项失败的重验和新Pawn隔离；本批日志中原Python重名告警为0。该结果属于EditorBuild-07二进制的原生范围，不能替代真实PIE、画面及联机验收。

`FullAutomation-AfterRejectedPlanRegression/index.json`实际共232项：132项成功、100项带警告成功，失败／未运行／进行中均0；对应`-process.json`记录UE进程退出0、51.543秒。新增`FacingLifecycle.RejectedPlansDoNotOccupy`使用启用配置与有效来源，经真实BeginHit／伤害／FinishHit核对五类拒绝结果及无占用；用例成功，保留1条警告。该批没有修改生产Runtime逻辑，不能将“动画依赖缺失被拒绝”写成“真实Montage同步失败已验证”。

增加同步诊断后，`FullAutomation-AfterNativeResolutionDiagnostics/index.json`重新完成232项：132项成功、100项带警告成功，失败／未运行／进行中均0；对应进程退出0、34.201秒。此批属于EditorBuild-09，仅证明当前C++及原生范围，不能把新增日志的编译／回归成功记为新PIE采集通过。

硬锁Final缓存同步修复后，`FullAutomation-AfterHardLockFinalCache/index.json`实际完成233项：132项成功、101项带警告成功，失败／未运行／进行中均0，逐项错误为0；对应进程退出0、29.363秒。新增`FacingMovement.HardLockKeepsAuthoritativeFinal`成功，原12项Movement及其余回归均完成；该结果属于EditorBuild-10，不代替修复后的PIE重采。

原生Yaw锚点修复初版后，`FullAutomation-AfterNativeAnchorBeforeDuplicate/index.json`实际完成239项：132项成功、107项带警告成功，失败／未运行／进行中均0，逐项错误为0；进程退出0、46.649秒。原修前失败的`NativeRotationActiveMultiMoveZeroStep`本批成功。该报告属于EditorBuild-12，早于交叉复核后新增的尾重复ACK补丁及回归，不覆盖尚未执行的后续构建和测试。

尾重复ACK补丁后，`FullAutomation-AfterNativeRejectedRestore/index.json`实际240项全部Success：132项成功、108项带警告成功，失败／未运行／进行中及逐项错误均0，UE退出0、54.843秒。它属于EditorBuild-13，包含原红测及真正尾重复ACK回归。该结果只关闭本次C++与原生范围，不能记为最终PIE十格通过。

`PythonAssertionTests-02.log`记录Python断言自测20／20成功，进程退出0：新增`test_hit_facing_assertions`十五项，既有`test_runtime_assertions`五项。该结果验证采样断言自身能接受合法证据并拒绝错误证据，不替代游戏中真实Yaw、Mesh和网络运行结果。

NPC ExportText入口增加两项回归后，`PythonAssertionTests-03.log`仅运行`test_hit_facing_assertions`模块，17／17成功；`PythonAssertionTests-04.log`重新运行全目录，22／22成功（该模块17项及既有运行时断言5项）。新增覆盖完整实录ExportText及完整GameplayTag的保留，以及空值、歧义和错误Schema的拒绝；不是22项实际游戏场景通过。

后续`PythonAssertionTests-07.log`仅补转模块47／47通过，`PythonAssertionTests-08.log`全`ACTCombat/tests`目录52／52通过（含另5项既有断言）；因果日志归帧、建立／释放边界、Replay作用域和BeforeYaw锚点均纳入断言。用户批准Hero专项夹具隔离后增加两项失败注入回归，`PythonAssertionTests-09.log`全目录54／54通过，口径为补转49＋既有5。三批均为离线断言自测，不是47／52／54个游戏场景通过，亦不能替代真实网络采样。

原生Yaw纠正边界批准后，`PythonAssertionTests-10.log`增加NativeCorrection schema解析、独立验收及三项回归，全目录57／57通过（补转52＋既有5）。这不证明尚未落地的生产修复或失败网络格已经通过。

随后`PythonAssertionTests-11.log`全目录59／59通过（补转54＋既有5），包含`NativeRejectedRestore`失败注入；这仍是离线断言自测。旧`ListenPlayer100Full01`原日志按新解析规则重审生成`audit-native-schema-before-fix.json`，结果仍`passed=false`，没有借新分类消除原真实失败。

`source-contract-audit-after-hardlock.json`静态检查三项均通过：原`FHodgeHitReactionPlanBuilder::Build`函数、`FMoveData`及`FMoveDataContainer`定义与执行前备份一致；该结果证明所查源码范围没有改动，不代替运行验证。

最终补丁后的`source-contract-audit-after-native-final.json`再次确认上述三项unchanged且`passed=true`，没有以新ACK处理为由扩展MoveData格式或重写原Build算法。

预启用保护报告`asset-pre-enable-protection.json`及Native之前的`asset-pre-enable-protection-final.json`均记录13份Profile／PawnData／蓝图仍与旧SHA256一致。它们是正式修改前的基线，不能称启用后的Hero／Dummy文件仍与旧哈希相同。

`asset-enable.json`记录`error=null`；检查两份Profile及引用后，先完成raw文件与Content资产两种备份，再仅为批准的Hero／Dummy启用及保存。raw备份位于`Execution/AssetBefore/`，Content备份为`/Game/Tests/Combat/HitFacing/Backups/DA_Hero_HitReaction_BeforeFacing`及`DA_DummyHitReaction_BeforeFacing`。两份Profile各27项旧配置精确保持不变，检查器均零警告，Guardian仍关闭，`dirty_after=[]`；`AssetEnable-process.json`记录进程退出0、25.357秒。

`asset-final-protection.json`本轮复核为`passed=true`：13份基线中11份未经批准修改的资产保持SHA不变，两份批准原件的raw备份匹配启用前SHA；六份蓝图磁盘及raw备份均未变，两份Content备份存在且LFS属性正确。保护脚本首轮错误推导`DA_Dummy_HitReaction`备份名，实际资产一直为`DA_DummyHitReaction`；首错保留于`asset-final-protection-attempt01-error.json`，改为读取`asset-enable.json`真实路径后通过，没有为此重命名或修改资产。

冷启动与蓝图验证保留三轮真实记录：

- 首轮`cold-runtime-audit.json`已正确冷读正式配置、5份PawnData引用及6份生成类／CDO，但读受保护的Blueprint.Status失败。首份HeroBase的编译调用已执行，状态未能确认，不能将该轮称为六份蓝图编译通过。
- `cold-runtime-audit-bridge.json`在编译前读取SaveOnCompile时，因Python未导出该枚举的0值而TypeError停止；该轮没有执行桥接编译。
- `cold-runtime-audit-bridge02.json`通过精确异常数字确认SaveOnCompile为Never（0），未改用户配置。现有`hodge_blueprints`的`bridge_status`确认工程后，六份蓝图均返回`compiled=true`、原生`blueprint_status=3`（UpToDate），原始返回保存在`cold-bridge02-compile-results.json`。编译前均有raw备份，六份蓝图磁盘SHA未变，`cold_new_dirty_packages=[]`、`compiled_memory_dirty_packages=[]`，未保存蓝图；`RuntimeColdAuditBridge02-process.json`记录退出0、76.568秒。

Bridge02同时读回Hero为玩家参数、Dummy为敌人参数且均启用，Guardian、Profile CDO及两份备份仍关闭，5份PawnData引用正确。冷读／内存编译成功不代表受击动画画面或联机通过。

`PIE/SingleEnemySmoke01/runtime.json`记录单人敌人目标、0ms、请求SideLight；采样读取Segment受保护属性失败，场景／样本／命中事件均为空，`cleanup_complete=true`、`dirty_after=[]`。对应`audit.json`为`passed=false`，`PIE-SingleEnemySmoke01-process.json`虽退出0、33.737秒，也不能计为通过。原runtime、audit及日志均保留，后续重跑没有覆盖该失败报告。

`PIE/SingleEnemySmoke02/runtime.json`及`audit.json`证明单人／敌人／0ms的SideLight实际完成并通过：1个case、188份样本、15条原生执行记录，`error=null`、`cleanup_complete=true`。这只关闭该Smoke覆盖，不代表玩家、完整单人链或联机矩阵通过。

`PIE/SingleEnemyFull01/runtime.json`实际完成SideLight、BackHeavy、BackLaunch、SameLevel四个case，随后在Upgrade的参数断言处失败，整批不能计为通过。Upgrade原始数据为Light从90°限幅到135°，Heavy新段从实际135°面向180°；剩余角度45°符合Heavy最多90°的契约，脚本误将最大角当作每次必须转满的角度。保留原失败及样本，脚本预期修订后的整批结果待另行记录。

Full01的SameLevel第二刀虽有正伤害，裁决为NoImpact／None，不能视为成功同级控制续播。BackLaunch原始样本已记录Start／Loop／Fall／StandUp、AIRBORNE／DOWNED／GETTING_UP和真实升高回落，可作为该子链机械证据；不能据此把逐case reset后的独立测试拼成连续整链，也不能把Mesh组件Yaw当作视觉验收。

`SingleEnemyIntegration01`实际命令如下，运行日期为2026-10-21；2026-10-22续作时重新核对其原始报告：

```powershell
& './Saved/HitReactionFacing_20261021/Execution/run_pie.ps1' -Batch SingleEnemyIntegration01 -Mode single -LagMs 0 -Cases 'SameLevel,Upgrade,AirFollow'
```

该批请求三项，实际case／样本／命中均为0，`audit.json`为`passed=false`。原生日志先记录`add_function`生成函数与`HodgeReactionResolved__DelegateSignature`的const引用签名不匹配；错误清理随后对未成功绑定的函数调用`remove_function`，再次抛异常，没有立即进入closing，下一帧继续执行探针。最终`runtime.json`中的error因此变成后续授予失败，不能将它当作本轮首因。报告虽有`cleanup_complete=true`、`dirty_after=[]`，也不能证明该轮所有清理入口正确；`PIE-SingleEnemyIntegration01-process.json`记录UE退出0、28.462秒，包装器据真实审计拒绝通过。

随后改用`add_callable`／`remove_callable`，仅在成功绑定后记录bound，并在清理开始即置closing、保留首错、单列清理异常。该修订进入Integration02／03重验；后续结果如下，未覆盖Integration01失败记录。

- `SingleEnemyIntegration02`（2026-10-22）：委托绑定及退出清理正常，实际case／样本／命中均为0。排查定位为ExplicitImpact探针仍携带20点ReservedPoiseDamage，被既有互斥校验拒绝装备授予。`runtime.json`保留授予失败，`cleanup_complete=true`、`dirty_after=[]`；进程退出0、68.225秒，不计通过。采集配置已修为仅PoiseDriven携带削韧，不改正式阈值或校验。
- `SingleEnemyIntegration03`（同日）：授予成功，真实近战Notify／GE进入FinishHit；随后在`OnReactionResolved`转接Python结构const引用时崩溃。栈为`InvokePythonCallableFromUnrealFunctionThunk`→`FFrame::StepExplicitProperty`，异常为读空地址的`EXCEPTION_ACCESS_VIOLATION`；进程退出3、25.827秒，没有`runtime.json`，没有正常清理完成结论，也没有场景通过。原日志／进程证据保留；它与此前历史`0xC0000409`不是同一个已确认问题。

为避免采集监听改变战斗调用链，在原`OnReactionResolved.Broadcast`前增加`HitFacingResolved`同步日志，仅Verbose启用时记录Actor、HitId、结算时间、阶段、空中状态、链／段／修订及动画实例，默认关闭；原委托、RPC和运行状态不变。Python采集取消该结构委托绑定，改用Actor＋HitId唯一关联轮询和原生日志。EditorBuild-09／GameBuild-07及上述原生已通过，实际PIE重验见下节；不修改引擎插件或声称修复PythonScriptPlugin本身。

### 6.4 2026-10-22新版采集的实际PIE结果

以下均使用独立批次、日志、runtime和离线audit；三份通过报告的`error=null`、`cleanup_complete=true`、`dirty_after=[]`，请求10场景与实际完成10场景一致。通过只覆盖相应模式／目标格及当时二进制，不覆盖之后的缓存修复。

| 批次 | 模式／目标 | runtime场景 | 样本 | 原生记录 | 真实GE | UE退出／外层秒数 | 离线审计 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| SingleEnemyFull02 | 单人／敌人／0ms | 10 | 2603 | 1102 | 17 | 0／104.546 | 通过 |
| SinglePlayerFull02 | 单人／玩家／0ms | 10 | 2261 | 878 | 17 | 0／88.185 | 通过 |
| ListenEnemy0Full01 | Listen／敌人／0ms | 10 | 4990 | 1164 | 17 | 0／95.248 | 通过 |
| ListenPlayer0Full01 | Listen／玩家／0ms | 10 | 4636 | 1925 | 17 | 0／93.031 | 失败，未计通过 |

10场景包括SideLight、BackHeavy、BackLaunch、ContactNoImpact、SameLevel、Upgrade、AirFollow、HardLock、Cancel与FullChain。FullChain在同一目标不reset地完成接触→Light→Heavy→首次Launch→真实落地→倒地起身→恢复，按实际正式时序记录自然释放／新链；不强行假定所有阶段始终共享同一控制链。SameLevel和Upgrade另用1秒专用ExplicitImpact探针验证控制窗内保持／接管；它们不证明正式0.4秒连打时序，未延长正式Profile或80～140ms补转窗。Mesh组件Yaw持续单独记录，不能代替骨骼画面或操作体验验收。

ListenPlayer0Full01的`runtime.json`虽为10场景完成、无错误且清理完成，`audit.json`仍为`passed=false`，不能据运行时完成宣称通过。当前分开处理三项事实：

1. 原生事件以WorldTime归入采样帧，但网络TickDispatch可早于当帧WorldTime递增，导致网络侧事件被错配到前一帧；拟增采样日志标记后重新采集，不删除或跳过现有失败样本。
2. HardLock存在真实Owner错误：从60°校正到服务器最终0°后，旧冻结缓存又恢复60°。CMC同身份权威Final缓存同步修复及`FacingMovement.HardLockKeepsAuthoritativeFinal`回归随后通过EditorBuild-10／GameBuild-08和233项原生；仍需实际Listen玩家PIE重采，不把原生通过等同网络通过。
3. 首个SideLight存在约125.66ms冷卡顿，超过80ms转窗，实际未产生补转；这是独立于事件归因的真实观测。计划预热既有路径后保持原参数重采，不放宽时窗、限速或误差标准。

其余Listen／PIE Dedicated的玩家／普通人形及0／100ms格尚无完整通过结论；PIE Dedicated不等于独立Server构建。此前所有失败批次与崩溃记录继续保留。

### 6.5 因果采样重验及Hero专项夹具决定

网络归帧不再仅依靠WorldTime，而以逐Actor采样日志标记的顺序限定原生事件区间；完整保存建立／释放、Replay作用域、BeforeYaw锚点和纠正样本，不跳过失败帧。三个真实独立预热攻击的`warmup.json`及`runtime.warmup`另外保存，不计正式场景或正式样本数。下表原生记录数来自整份日志，包含预热，不能当作纯正式记录数：

| 批次 | 模式／目标 | 正式场景 | 正式／预热样本 | 原生记录（含预热） | 正式GE | UE退出／外层秒数 | 离线审计 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| ListenPlayer0CausalSmoke01 | Listen／玩家／0ms | 3 | 1190／1618 | 802 | 3 | 0／54.606 | 通过，105项检查 |
| ListenPlayer0Full02 | Listen／玩家／0ms | 10 | 5832／1614 | 3072 | 17 | 0／104.493 | 通过，因果证据完整 |
| DedicatedEnemy0Full01 | PIE Dedicated／敌人／0ms | 10 | 8205／2205 | 1522 | 17 | 0／105.846 | 通过 |
| ListenEnemy100Full01 | Listen／敌人／100ms | 10 | 5472／1534 | 1765 | 17 | 0／105.530 | 通过 |
| DedicatedPlayer0Full01 | PIE Dedicated／玩家／0ms | 10 | 8748／1908 | 2908 | 17 | 0／108.480 | 失败，隔离前证据 |

前三个完整通过批次及三场景Smoke的runtime均无错误、清理完成且无脏包。ListenPlayer0Full02的`causal_marker_evidence_complete=true`，正式样本5832与预热1614分开，标记总数7446；硬锁旧缓存回拉及真实速率检查均按新样本审计，不能将旧ListenPlayer0Full01覆盖为通过。Dedicated敌人的Proxy Final组共30，其中正式24、预热6，全部`verdict=converged`；Listen敌人100ms共15组，其中正式12、预热3，全部收敛，预热组不计正式credit。

DedicatedPlayer0Full01实际10场景和清理完成，但唯一离线失败是Upgrade接管专项：两次真实Applied结算相隔0.400753秒，超过Hero原GroundRecovery约0.4秒，第二次合法建立新链，不能据此把生产逻辑判为未接管。该批失败完整保留，不修改正式参数或将新链伪写为同链。

用户随后批准只为Hero SameLevel／Upgrade两专项在内存临时关闭`bEnableGroundRecovery`，使已有1秒探针窗生效；每项结束／异常立即还原，不保存资产。FullChain及其他case保留正式控制。脚本实现和两项失败注入恢复回归已完成，全目录54项自测通过；隔离前的通过／失败批次不能自动成为隔离后的验证结论。

`DedicatedPlayer0RecoverySmoke01`随后真实完成SameLevel／Upgrade／FullChain三场景并通过因果及隔离审计：正式2508份样本、预热1755份样本、8次正式GE，整份日志1321条原生记录；进程退出0、84.491秒。两个覆盖scope均记录`before_enabled=true`、`override_enabled=false`及case_end恢复true；FullChain的1458份正式样本全部`ground_recovery_enabled=true`，清理完成且`dirty_after=[]`。该Smoke只证明这三项正常流程及恢复，异常恢复另由上述失败注入自测覆盖；完整10场景及其他网络格的新批次尚待独立结果。

### 6.6 隔离后的完整格与原生Yaw边界修前失败

`DedicatedPlayer0Full02`完成正式10场景并通过因果／隔离离线审计：正式7221份样本、预热1782份样本、17次正式GE，整份日志2496条原生记录；UE退出0、114.626秒。SameLevel、Upgrade两个scope均case_end恢复true，清理完成且无脏包。它关闭此批Dedicated玩家0ms格，不覆盖之后新增的原生纠正边界。

串行矩阵随后在`ListenPlayer100Full01`停止：运行时正式10场景、5148份正式样本、1262份预热样本及17次正式GE完成，清理成功且无脏包；整份日志4215条原生记录，UE退出0、114.355秒，但离线审计为失败。唯一失败组为预热WarmupKnockback的Owner历史／可见Yaw核对：原生实际Yaw由42.2383118°纠正至39.5095825°，ReplayEnd却回到21.3519287°，BeforeYaw锚点也错误。预热不计正式credit，但其真实物理失败不能忽略，稍后Final收敛亦不能抹掉先前错误。

用户明确批准设计第15.9节：仅对通过原生新ACK、实际应用Yaw的RMS／显式Rotation响应，在仍同Avatar／段／有效Impact driver时同步当前锚点；零步进同步锁缓存，每次原生纠正独立留证并允许跳变，与720°/s普通补转及唯一Final校正分开。RootAnim、换Pawn、新动作、Retired维持原机制，协议、参数与容差不改。

`NativeRotationRepro-BeforeFix`在未加生产修复的EditorBuild-11上只运行新项`Hodge.HitReaction.FacingMovement.NativeRotationActiveMultiMoveZeroStep`：实际1项失败、6条错误，未运行／进行中为0；UE虽退出0、34.062秒，`expected_failure=true`是复现目的，不能计为回归通过。原日志与报告保留，证明多SavedMove／零步进情况下原生锚点及后续锁参考会被旧值覆盖。初版修复后该项已在EditorBuild-12的239项中成功，后续尾重复问题及修后PIE仍单独验证。

交叉复核又发现尾重复ACK：合法ACK锚点消费完成、历史已空后，UE RMS对重复LastAck时间戳仍可执行`SetActorYaw`，不能将其记为新的NativeCorrection。最小补丁保留完整Super；仅同身份有效Impact把该未新ACK写入恢复至调用前Yaw，独立记录`NativeRejectedRestore`、Before≈After净变化0，不改变Anchor／Pending／Final／协议。随后该补丁及真正尾重复回归通过EditorBuild-13／GameBuild-09及240项原生，修后实际PIE结果仍须独立采集。

该串行任务中止时，余下Dedicated敌人100ms、Dedicated玩家100ms及计划中的单人双方／Listen双方新版回归尚无结果；此处保留当时状态，后续最终十格结果见第6.7节。

### 6.7 最终冻结版本的十格结果与保护证据

2026-10-23续作核对2026-10-22已完成的最终采集。`run_final_matrix.ps1`在EditorBuild-13二进制上按顺序重新执行全部十格；每格独立保存runtime、完整原生日志、warmup、离线audit和进程结果，没有使用旧通过批次补齐最终格。

| 最终批次 | 模式／目标 | 正式场景 | 正式／预热样本 | 正式GE | 原生记录（含预热及其他Actor） | UE退出／外层秒数 | 离线审计 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| ListenPlayer100FullFinal01 | Listen／玩家／100ms | 10 | 5356／1392 | 17 | 4615 | 0／114.926 | 通过 |
| DedicatedPlayer100FullFinal01 | PIE Dedicated／玩家／100ms | 10 | 7188／1731 | 17 | 4024 | 0／112.867 | 通过 |
| DedicatedEnemy100FullFinal01 | PIE Dedicated／敌人／100ms | 10 | 7104／1815 | 17 | 1577 | 0／114.290 | 通过 |
| SingleEnemy0FullFinal01 | 单人／敌人／0ms | 10 | 2375／799 | 17 | 1339 | 0／111.133 | 通过 |
| SinglePlayer0FullFinal01 | 单人／玩家／0ms | 10 | 2369／741 | 17 | 1252 | 0／111.110 | 通过 |
| ListenEnemy0FullFinal01 | Listen／敌人／0ms | 10 | 5272／1252 | 17 | 1431 | 0／113.174 | 通过 |
| ListenPlayer0FullFinal01 | Listen／玩家／0ms | 10 | 5330／1136 | 17 | 2714 | 0／115.817 | 通过 |
| DedicatedEnemy0FullFinal01 | PIE Dedicated／敌人／0ms | 10 | 6987／1755 | 17 | 1286 | 0／114.323 | 通过 |
| DedicatedPlayer0FullFinal01 | PIE Dedicated／玩家／0ms | 10 | 6057／1338 | 17 | 2138 | 0／117.266 | 通过 |
| ListenEnemy100FullFinal01 | Listen／敌人／100ms | 10 | 5168／1242 | 17 | 1571 | 0／117.041 | 通过 |

十格均为`causal_marker_evidence_complete=true`、`error=null`、`cleanup_complete=true`、`dirty_after=[]`，UE退出0；每格真实完成默认十场景和17次正式GE。合计100个正式场景、170次正式命中、53206份正式样本；13201份预热样本及30次预热攻击另外留证，不计正式credit。所有预热物理断言同样参与审计，没有跳过先前失败类型。

同一目标的FullChain覆盖接触→Light→Heavy→首次Launch→真实落地→倒地→起身→释放；按真实正式控制时序允许自然释放后进入新链，未强制所有阶段共享同一控制链。SameLevel、Upgrade才使用获批一秒专项夹具；Hero五格共10个scope均case_end恢复true，FullChain、预热及其他场景的正式恢复配置不变。异常恢复另由Python失败注入回归覆盖，不能把正常退出当作所有异常路径都在PIE实测。

网络及回放分别验收：

- 普通Step继续使用`720°/s × 有效CMC步长 + 0.05°`及不越界检查；同身份Final唯一性、Owner首允许消费步≤1°保持原要求，全部纠正和回放样本保留。
- 被测玩家100ms两格分别记录25、23次合法NativeCorrection，包含实际原生RMS跳变，按新ACK／同身份／实际Yaw独立验收。整份十格日志共58次，另外10次属于两个敌人100ms格中的其他Hero，不能写成Dummy目标走过Owner纠正。Native记录总数21947同样包含预热及其他Actor。
- 全部Proxy Final共96组正式、24组预热，均`verdict=converged`；按持续相关且无新动作接管的0.25秒／2°要求审计，预热组不算正式场景。
- 十格NativeRejectedRestore均为0，不能声称PIE自然命中过尾重复RMS分支。该分支由240项中的`NativeTailDuplicateAfterNoHistoryCannotTurnBody`等原生回归证明；RootAnim、退休、换Avatar等排除范围亦按各自原生证据报告。
- Actor与Mesh组件Yaw均采样；组件数值不能代替骨骼画面、脚滑或操作体验。PIE Dedicated不是独立专服可执行文件，也不是Cook或打包验证。

最终证据位于本机忽略目录`Saved/HitReactionFacing_20261021/Execution/`，不纳入源码：

- `final-matrix-summary.json`：逐格原始报告汇总，`passed=true`；原件为`PIE/<批次>/runtime.json`、`audit.json`、`warmup.json`及`PIE-<批次>.log`、`PIE-<批次>-process.json`。
- `final-execution-manifest.json`：两目标构建、240项原生、59项Python及十格结果；记录五项最终源码SHA与验证时冻结值一致。它不覆盖之后的主观表现或未测边界。
- `asset-final-protection-after-matrix.json`：11份其他受保护资产未变，两份批准原件的raw备份保存，六份蓝图磁盘及raw备份SHA保护通过，两份Content备份LFS符合约定。
- `profile-memory-override-disk-protection.json`：Hero、Dummy当前磁盘SHA与批准启用后的`asset-final-protection.json`逐字一致，PIE专项内存覆盖未保存至正式Profile。
- `source-contract-audit-after-native-final.json`：原PlanBuilder.Build及两份MoveData定义三项unchanged；既有旧失败、红测和修后结果全部保留。

实际运行入口与结果：Editor及Game的Build.bat命令见第6.3节，最终并行参数为2，均退出0；`run_stage.ps1 -Stage Native -Suffix '-AfterNativeRejectedRestore'`实际完成240项，132项成功、108项带警告成功，失败／未运行／进行中及逐项错误均0。`PythonAssertionTests-11.log`为全目录59项断言通过。`run_final_matrix.ps1`及`verify_post_matrix_assets.py`、`write_final_manifest.py`均完成并通过，未使用Live Coding。复跑须使用新批次和报告路径，现有包装器拒绝覆盖旧证据；本次续作仅核对文件与文档，不重复这些运行。

审计边界：NativeCorrection离线去重目前使用`(Avatar, MoveTimeStamp)`，现有每格采集小于170秒。超过UE时间戳重置周期的长跑须先扩充审计分代，再验证；生产SavedMove对象身份机制未受影响，当前有界结果不外推为长时间浸泡通过。历史`0xC0000409`与Python委托转接崩溃仍按各自记录处理，不因最终矩阵正常退出而宣称其原因已修复。

### 6.8 剩余优先级、依赖与职责

1. **已完成，P0代码与规定机械矩阵**：最终Editor／Game、240项原生、59项Python、十格100正式场景、资产保护及文档同步均已完成。原有工作区条目保留，当前252条，无本项目UE进程残留；没有提交或推送。本结论不关闭W05／W06／W07的剩余部分。
2. **P1，W05画面评审；主责动画／技术美术，战斗策划和QA配合**：依赖已通过的单人双方FullChain，核对有限补转与选片、脚部滑动、空中／落地衔接，准备后续双方差异评审材料。按第6.9节留录像与明确结论；具体团队人员尚未指定。
3. **P1，W06真实集成边界；主责QA，移动／网络和战斗程序配合**：可与W05画面复核并行。规定十格联机机械矩阵已完成，真实PlayerState ASC重生交接、多攻击者、真实动画暂停／零速／变速和Montage同步失败仍需专项。先定义每项的实际触发路径、允许结果与证据，复用现有规则；需要改变协议、时窗、转向或验收契约时先提交具体方案确认。原生对象生命周期或失败注入证据不代替这些真实PIE场景。
4. **P1，W07双方差异与操作评审；主责战斗策划，动画、程序和QA配合**：依赖W05画面及W06余项证据齐备，核对玩家镜头、锁定与真实恢复输入，以及双方受击、弱点和碰撞表现；完成后交项目负责人决定首版签收。
5. **后续长跑与发布验证；主责QA及移动／网络程序**：长跑依赖离线审计分代；独立专服、Cook／打包依赖明确发布目标和现有Target支持，尚未执行，不为此另加Target或模块。

### 6.9 画面与操作评审入口

此节是待执行评审步骤，不是通过记录，也不新增实现范围或参数：

1. 在UE5.5.4打开`/Game/Tests/Combat/L_HitReactionDummy`，单人PIE使用1名玩家、Play Standalone模式；确认正式Hero、Dummy及Experience初始化完成。正式输入按[制作流程第9节](../Guides/combat-production-workflow.md#end-to-end)核对当前IMC，不假设键位。该指南的手动制作路线与本功能自动探针结果分开。
2. 只读核对`/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction`及`/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction`的`Facing.bEnabled`、`Light/Heavy/Launch.MaxCorrectionAngle`、`WindowDuration`、`MaxYawSpeed`与`DeadZoneAngle`，预期为设计第15.1节批准值；Guardian及其他未批准Profile保持未启用。检查实际Profile选出的Montage，Hero原地动作位于`/Game/Main/Character/Hero/Anim/HitReactions/Behit/InPlace/`；不靠调整正式参数让评审画面通过。
3. 敌人作为目标时按正式攻击输入从正面、侧面、背后观察接触、轻／重受击和首次击飞；重击由真实削韧窗口决定，不要求每轮五段必出Heavy。玩家作为目标时复用已验证NPC现有意图链，具体攻击探针及十场景复现入口见[工具说明](../../Tools/ACTCombat/README.md)；自动采集必须使用新建独占编辑器和新报告目录，不能注入正在使用的编辑器。未另行执行和录像前，这些机械报告不算玩家画面已评审。
4. 在正常世界时间记录同一目标完整链，先看实速，再逐帧核对原选片、身体补转、脚部接地、Impact世界轨迹、真实触地后的Landing／Downed／GetUp及行动恢复。首版背击只作有限补转，不以最终完全面向攻击者作为通过条件；空中追加命中不得重新开转窗。组件Yaw与录像分别保存。
5. 玩家同时核对镜头／锁定没有被受击补转改写、恢复后真实移动／攻击输入可用；敌人核对方向动作与弱点／碰撞关系。网络画面复核应同时录Owner与观察者，区分获批NativeCorrection／唯一Final瞬转和普通补转；“允许权威跳变”不自动等于观感合格。
6. 每项评审记录目标、模式／延迟、来源侧向、实际Montage、对应命中／链段／修订、录像时点、预期与实际、评审人及结论。出现需改变动画资源、角度／时窗、原生纠正处理或其他批准契约的问题，先保存证据并明确提案；不覆盖旧失败或顺手修改正式资产。
