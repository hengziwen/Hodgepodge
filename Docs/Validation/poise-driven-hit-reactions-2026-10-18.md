# 削韧受击与原地运动验证

日期：2026-10-18；分支 `dev-Combat`；UE5.5.4 CL40574608。对应[实现设计](../Design/poise-driven-hit-reactions.md)。保留开始时已有的未提交工作，不提交、不切分支。

## 实现范围

普通攻击使用互斥 PoiseDriven 输入，服务器 FinishHit 一次消费正值削韧；目标 PawnData 保存滑动窗口、轻重破韧阈值、保护和位移预设。既有 HitReactionComponent／GA／CMC 执行并复制结果，保留显式 Impact 与 Legacy。Profile 以 Compact 作为唯一动画来源，旧字段及备份保留。

正式五段使用 15／20／25／25／80 演示削韧；R 保留显式 Launch。Hero／假人轻重使用原地 S/B 左右动作；Guardian 保留自己的骨架，破韧结果为 Knockdown。Hero、假人和三份 Guardian PawnData 共五份目标规则已接入。

创建 22 个原地 Sequence／Montage 副本、14 个迁移前备份，以及终结刀本轮削韧校准前的独立备份，共 15 个备份；原 Wuwa 动画与上一批完整 Behit 副本保留。B_L 使用前 1.10s，不经过原片约 1.18～1.27s 的异常区间；这属于排除坏区间，不是修复完整原始骨骼关键帧。

## 本轮变更文件

- Runtime：`Combat/HodgeHitReactionTypes.h/.cpp`、`Component/HodgeHitReactionComponent.h/.cpp`、`Data/HodgeHitReactionProfile.h/.cpp`、`Data/HodgePawnData.h/.cpp`、`AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.cpp`。均位于 `Source/Hodgepodge/Public`／`Private`，同类仍集中主 cpp。
- 原生测试：`Source/Hodgepodge/Private/Tests/HodgeHitReactionTests.cpp`，新增八项削韧用例并沿用原受击测试。
- 工具：`Tools/ACTCombat/migrate_poise_reactions.py`、`validate_poise_assets.py`、`validate_poise_runtime.py`；更新正式动作闭环、R 回归、工具 README 与 `Tools/HitReactionDummy/control.py` 的诊断。
- 正式资产：Hero／假人／Guardian 三份 Profile，五份相关 PawnData，五段 Definition 和 R Definition；新素材位于 `/Game/Main/Character/Hero/Anim/HitReactions/Behit/InPlace`，备份位于 `/Game/Tests/Combat/PoiseMigration/Backups`。
- 文档：当前设计、本文、受击设计／配置手册、ACT 总设计和 Docs 索引。没有修改伤害公式、攻击 Notify、技能消耗、主 AnimBP 根运动模式或第三方源资产。

## 构建与原生正确性

实际执行：

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -nosplash -nosound "-ExecCmds=Automation RunTests Hodge" "-TestExit=Automation Test Queue Empty" -ReportExportPath=E:/Project/Git/Hodgepodge/Saved/PoiseReaction/Automation-Origin -abslog=E:/Project/Git/Hodgepodge/Saved/PoiseReaction/Automation-Origin.log
```

- Editor 常规构建退出码 0：最终日志 `Saved/PoiseReaction/Editor-Build-Origin.log`，45.16s。
- Game 常规构建退出码 0：`Game-Build-Origin.log`，62.51s。现有 CodexText `UButton::IsFocusable` 弃用告警仍存在，没有顺手改动。
- 最终 `Hodge.*`：169 项，103 Success、66 SuccessWithWarnings、0 Failed、0 NotRun；`Automation-Origin/index.json`。告警包括既有测试诊断和新增容量超限测试的预期拒绝日志。
- 新用例覆盖：阈值推进与破韧保护、滑动窗口／已消费等级、多 GE 与重复 Finish、零实际伤害／免疫、Body 与受击阶段约束、失败后可重试／解绑死亡、配置互斥／原地运动契约、有限队列与直接生命归零、攻击者位置快照及旧模式方向兼容。部分规则合并在同一用例内，八个用例不代表只有八个断言。
- 冷启动资产检查：82 项通过，含正式资产校验、旧字段保留、伤害／Execution／Body 不变、Notify 继续读取默认 Reaction、原片 SHA、根锁定与坏区间排除；Hero AnimBP `ERRORS=0`。日志和明细为 `assets-validated.json`。

## 真实 PIE 与网络

测试输入使用 Enhanced Input 注入或现有装备授予后的原生动作队列；实际经过 Notify、Melee、GE、HealthSet、目标受击 GA 与 CMC，不是直接写 State 或伪造扣血。不是物理键鼠操作。

本轮冷启动命令：

```powershell
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -nosplash -nosound "-ExecCmds=py E:/Project/Git/Hodgepodge/Saved/PoiseReaction/run_batch.py" -abslog=E:/Project/Git/Hodgepodge/Saved/PoiseReaction/Editor-Release.log
```

`run_batch.py` 是本轮忽略目录中的串行启动器，依次调用已纳入交付的资产检查、`validate_poise_runtime.py`、`validate_action_loop.py` 和 `validate_resonance_launch.py`；报告生成且 PIE 完全退出后才进入下一项，不保存内存中的探针覆盖。

- 最终单人 0ms：12 类、79 项检查通过；`runtime-single-0ms.json`。
- 最终 Listen 0ms：12 类、91 项检查通过；`runtime-listen-0ms.json`。
- 最终 Dedicated PIE 0ms：12 类、103 项检查通过；`runtime-dedicated-0ms.json`。
- 最终 Dedicated PIE 100ms：12 类、103 项检查通过；`runtime-dedicated-100ms.json`。
- 合计 48 个场景执行、376 项检查，所有报告 `error=null`，无失败断言。十二类为轻受击左侧、重受击左右、破韧、150cm 击退、取消、高抗、Fly→Push、Fly→Slam、空中碰撞隔离、密集同级不刷新、密集轻重破韧推进。网络场景同时观察服务器、拥有者和观察者的复制与最终位置收敛。
- 正式五段／移动／闪避闭环：7 项通过，`action-loop.json`。五刀均实际扣血 31，顺序 01→02→03→05→04；实际削韧窗口值依次为 15／35／60／70／105，等级为 None／Light／Heavy／None／Break。第 05 刀不刷新已消费的 Heavy，终结刀正常破韧；不把累计总输入直接当作当前滑动窗口值。
- 正式 R 冒烟：9 项通过，`r-input-smoke.json`。实际扣血 62、一次命中／一次 Launch，Fly_Start／Loop／Fall 均达到真实播放权重 1，最大离地高度约 323.68cm；Fall 在真实接地后起播，原资源消耗和释放、落地恢复及控制清理正常。
- `batch.json` 记录六组套件均完成、`error=null`；测试编辑器正常退出，`Editor-Release.log` 末尾为 LogExit，未遗留本轮 PIE。首轮、失败定位及旧 DLL 的报告分别保留于 `FirstPass`、`SecondPass`、`ThirdPass`、`BeforeOriginMatrix`，不与本节最终结果混用。

地面 Heavy 左右两组实测退步约 65cm，独立 Knockback 约 150cm；最终地面采样没有权威胶囊反向步进。保留碰撞的空中 Slam 会受到攻击者胶囊接触影响；只忽略这对运动碰撞的隔离对照中水平位移为零。此结果区分了 CMC 碰撞偏移与动画 Root 回跳，未修改生产碰撞规则，也不能据此承诺任意多人碰撞轨迹都单调。

## 失败历史与定位边界

1. 首次资产检查脚本误用了不存在的 NotifyState 类型名；改为项目实际 `HodgeAnimNotifyState_HitCheck`，随后 82 项通过。失败报告保留为 `assets-first-script-name-failed.json`。
2. 旧 R 测试读取兼容 Animations 数组，无法识别新 IP 家族并超时；其采样已记录真实扣血 62、实际 Fly 三段和离地。改为读取当前 Compact 家族，失败证据为 `r-first-legacy-expectation-failed.json`，最终结果另记。
3. 扩展下砸测试把地面退步单调性错误套用到空中碰撞场景而失败：记录中 Root 始终锁定、无动画根运动／RMS，但 CMC 水平速度发生偏转。增加保留正式碰撞的空中链检查，以及只在测试中忽略攻击者胶囊的隔离对照；不通过修改正式碰撞或动画掩盖问题。失败样本保留在 `SecondPass`。
4. 五段闭环测试固定向前攻击时，主角根运动越过假人，终结刀实际脱靶。改为通过正式 IA_LockOn 输入确认目标再连段，并逐刀记录实际扣血；没有瞬移跟随目标或伪造命中。失败证据保留在 `ThirdPass` 与 `action-before-position-capture-failed.json`。
5. 锁定后的真实五段验证发现，本轮初值 50 的终结刀与前段实际间隔约 1.37s，旧样本已离开 1.5s 窗口，最终仅累计 25＋50＝75。将新增削韧配置校准为 80，保留校准前资产；不改伤害、Notify 或原技能消耗。明细为 `rhythm-tuning.json`。
6. 普通削韧击退原先沿用检测体原点，刀刃在目标侧面或越过目标时可能产生意外侧移。已改为命中开始时的攻击者位置快照，接触表现继续保留真实采样；新增原生用例验证攻击者随后移动不会改变快照，Legacy／Explicit 仍使用原输入方向。
7. 一次忽略目录中的批处理启动脚本在加载地图的 Slate 回调重入时触发测试编辑器栈溢出；运行标记前置后重新冷启动完成测试。未修改运行时代码掩盖脚本问题，旧日志 `Editor-Smoke.log` 保留。

## 工作区与未验证项

开始时 127 个已有未提交文件均已复制到忽略目录 `Saved/PoiseReaction/WorkingBackup`。最终哈希核对：103 个未涉及文件原样保留、24 个与本任务相关文件有意更新、0 个意外变化。原片 SHA 与迁移备份另有明细；`summary.json` 汇总本轮实际报告、最终 Editor DLL／Game EXE 哈希和四组轨迹指标。

六个新增／更新 Python 工具 `python -m py_compile` 通过；`git diff --check` 通过，仅提示已有 MovementAction.h 的 LF／CRLF 转换警告。文档本地链接检查通过。没有自动提交、回退或切换分支，Saved 日志和报告未加入源码。

未执行独立进程专服、Cook／打包、长期压力／性能采样、断网重连实机测试或完整主观手感验收；不能据构建、自动化或最终位置收敛声称达到参考游戏观感。完整原片坏帧修复、专业脚步素材和共振度 UI 不属于本轮完成范围。
