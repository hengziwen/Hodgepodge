# 分段击飞验证

日期：2026-10-18；分支 dev-Combat；UE5.5.4。实现约定见[设计](../Design/attack-segment-launch.md)。

本轮仅修改攻击分段击飞开关、受击裁决、相关校验和测试，保留开始时工作区改动；不提交、不切分支。

## 变更文件

- `HodgeCombatAnimNotifies.h/.cpp`：分段作者字段及默认／覆盖反应解析。
- `HodgeGameplayAbility_Melee.cpp`：转交唯一分段开关。
- `HodgeHitReactionTypes.h/.cpp`：运行快照，隐藏旧破韧配置，轻重阈值校验。
- `HodgeHitReactionComponent.h/.cpp`：取消累计击飞、分段 Fly、空中续飞和接地恢复。
- `HodgePawnData.cpp`：削韧不再依赖破韧动画家族。
- `HodgeHitReactionTests.cpp`：原生回归及新分段用例。
- `configure_segment_launch.py`、`validate_poise_assets.py`、`validate_poise_runtime.py`、动作闭环断言与工具说明；两份正式 Montage 及对应备份。

## 实际命令

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -nosplash -nosound "-ExecCmds=Automation RunTests Hodge" "-TestExit=Automation Test Queue Empty" -ReportExportPath=E:/Project/Git/Hodgepodge/Saved/SegmentLaunch/Automation -abslog=E:/Project/Git/Hodgepodge/Saved/SegmentLaunch/Automation.log
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -nosplash -nosound "-ExecCmds=py E:/Project/Git/Hodgepodge/Saved/SegmentLaunch/run_batch.py" -abslog=E:/Project/Git/Hodgepodge/Saved/SegmentLaunch/Editor-PIE.log
```

## 当前结果

- Editor 常规构建通过；最后一次 `Editor-Build-Tests.log` 总耗时 123.22s，含等待 Game 构建互斥锁，实际编译阶段 30.47s；此前功能版本常规构建 208.79s，最终裁决确认构建 32.61s。
- Game 常规构建通过，`Game-Build.log` 总耗时 214.26s。保留既有 CodexText `UButton::IsFocusable` 弃用告警，没有顺手改动构建设置。
- 原生 `Hodge.*` 共 172 项：106 Success、66 SuccessWithWarnings、0 Failed、0 NotRun，`Automation/index.json`。新增三项分段用例；更新原有空中接触、阈值上限与严格 Fly 支持测试，未使用旧通过结果替代复测。
- 资产检查 91 项通过，`assets-validated.json`；正式六份 Definition 磁盘哈希保持不变。只保存 `AM_Attack04_Montage` 与 `AM_Burst01` 的分段开关，保存前分别复制到 `/Game/Tests/Combat/SegmentLaunch/Backups`。01／02／03／05 只读核实保持关闭。
- 最终 PIE 单人 0ms：12 类、89 项检查通过；Listen 0ms：12 类、101 项通过；Dedicated PIE 0ms／100ms：各 12 类、113 项通过。合计 48 个场景执行、416 项检查，四份 `runtime-*.json` 均为 `error=null`。同时记录服务器、拥有者和观察者状态及最终位置收敛。
- 十二类包括轻受击左侧、重受击左右、大额削韧不击飞、独立 150cm 击退、取消、高抗、空中允许续飞、空中禁止续飞、续飞次数上限、密集同级不刷新、密集轻重推进。四种模式中，允许续飞都观察到两个 Fly_Start 实例；禁止续飞及次数封顶都只有一个，无 Push 替代 Fly；接地后均观察到 StandUp，并完成控制清理。
- 正式五段／移动／闪避闭环 7 项通过，`action-loop.json`：01→02→03→05→04 各实际扣血 31，前四刀为普通接触／轻硬直／重硬直／接触，只有第 04 刀由分段开关得到 Launch；该刀 ReactionLevel 为 None，不再伪报累计破韧。移动退出、闪避提交及闪避后普攻正常。
- 正式 R 冒烟 9 项通过，`r-input-smoke.json`：原消耗与释放、真实扣血和一次 Launch、Fly 三段实际起播、真实接地以及恢复清理通过。
- `batch.json` 记录六组套件全部完成，`summary.json` 保存最终 Editor DLL／Game EXE 哈希。测试编辑器正常 LogExit；旧 Saved/PoiseReaction 的通过结果不计入本轮。

## 测试夹具修复

首轮新增 ReplayAndLanding 用例直接 Tick 了未注册 WorldContext 的 EditorPreview World，触发引擎断言；日志保留为 `Automation-FirstFixtureCrash.log`。已改为只推进被测组件，不让孤立原生夹具模拟完整游戏 World；真实物理运动由 PIE 测试承担。重建后的 172 项全部完成，未通过修改生产逻辑回避测试崩溃。

## 边界

开始时 177 个已有未提交文件已备份；最终 165 个原样保留、12 个与本任务相关文件有意更新，0 个意外变化或丢失。新修改的原干净文件（通知结构／实现、Melee、两份 Montage）及本轮新增工具、文档和备份单独列入任务范围。资产命中记录按值比较，除 Can Launch 外其余字段完全相同；六份正式 Definition 哈希未变。

Python 工具语法检查、文档本地链接检查和 `git diff --check` 通过；只保留已有 MovementAction.h 的 LF／CRLF 提示。不提交、不切分支，不把 Saved 报告加入源码。

运行时代码共用于玩家与敌人受击组件；真实 PIE 目标为现有敌人假人，Formal Hero 五段／R 提供攻击输入。Guardian 没有兼容的 Fly 动画家族时会明确返回 Unsupported，本轮没有跨骨架套用 Hero 动作；不声称所有现有怪物已补齐 Fly 素材。缺素材边界不改变 Body、免疫与次数限制。

本轮不进行额外视觉验收，不声称专业动画手感已完成；独立进程专服、Cook／打包未执行。
