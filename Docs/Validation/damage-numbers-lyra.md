# Lyra 本地伤害跳字验证记录

日期：2026-10-16。基线：`main d028591d56ba402d000e6c251eb6ba9695279708`，UE 5.5.4 CL 40574608。工作开始时工作区干净；保留受击封存分支，不合入其代码。修改范围为跳字原生类、HealthComponent 转发、PlayerController 本地路由、六个迁入资源和一个新样式资产，以及对应测试/工具/文档。没有修改正式伤害公式、攻击/受击资源或 HUD Designer/Graph。没有自动提交。

## 实际命令与构建

在本项目编辑器退出后，以 PowerShell 执行：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
```

最终传输版本日志为 `Saved/DamageNumbers/build-editor-paced.log` 和 `build-game-paced.log`，分别成功完成，耗时 39.61s / 31.01s。保留原有 IncludeOrderVersion、ModularGameplay/SignificanceManager 插件声明及 IsFocusable 等警告，没有为消除警告更改无关插件或构建设置。

自动化命令：

```powershell
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.NumberPop+Hodge.HitReaction+Hodge.Havoc+Hodge.Resonance+Hodge.Combo' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/DamageNumbers/AutomationPaced' '-abslog=E:/Project/Git/Hodgepodge/Saved/DamageNumbers/automation-paced.log'
```

新增 FormattingAndValidation、AcceptedDamageAndLifecycle、SourceOwnershipAndDedup、TransportBatch，验证浮点边界、实际扣血、最后一击、零伤/免疫/治疗排除、PlayerState 归属、解绑重绑、旧 Avatar、身份去重和跨帧批次。TransportBatch 使用 Preview 世界的局部脚本执行保护，仅验证本地批次，联机权限与送达另在真实 PIE 验证。

最终 `AutomationPaced/index.json`：29 项执行，24 项成功、5 项带警告成功、0 失败、0 未执行。新增四项全部成功；TransportBatch 验证 72 笔独立事件跨批发送。带警告的是既有 Combo.DefinitionGrant、Combo.Retention、Havoc.AssetConfiguration、Havoc.VariantSelection、HitReaction.EnemyPawnDataInitialization。

## 资产与真实场景

`python Tools/DamageNumbers/migrate_lyra_assets.py` 迁入六个资源，SHA256 见 `Saved/DamageNumbers/lyra-migration.json`。编辑器执行 `setup_style.py` 后，AssetRegistry 确认项目内依赖闭合，六个资源全部可加载；二进制中的旧 Developer 源路径不在实际依赖集合中。新资产为 `/Game/Main/UI/Feedback/DA_DamageNumberStyle`。

真实 PIE 地图：`/Game/Main/Maps/L_EnemyAIValidation`。使用正式主角、两个正式 Guardian 的 PIE 实例，暂停 AI，并仅在实例上把生命补至 100000。输入经过 Enhanced Input，伤害由正式 Montage Notify 与 GAS 执行；正式连段顺序核对为 01→02→03→05→04，E/R 走同一跳字路径。压力与最后一击使用明确标记的权威 Damage GE 夹具，独立于正式攻击验收。

## 开发期间发现与处理

- 第一轮截图赶上首次 Niagara 着色器编译，不能证明可见性；预热后重新取图，已看见真实命中的 Lyra 数字。
- 测试曾在 NextAttack 窗口前 80ms 注入下一刀，远端只走到第二段。改为实际窗口开始后 30ms 注入后，正式五段走完。此次未更改连段代码，不据此宣布用户此前报告的 Client 共鸣回路前连段问题已修复。
- 40 次同帧逐笔 Unreliable RPC 曾只收到 25 笔；32+8 两个同帧批次曾只收到首批 32 笔。核查 UE5.5 `NetDriver.cpp::ProcessRemoteFunctionForChannel` 的非 Reliable 饱和拒发入口后，改为一次处理只发一批，最多 64 笔，余量延后，服务器总队列上限 256。每刀身份、数值和位置保持独立。
- 本地批次原生测试曾未初始化 Preview Actor 的 ProcessEvent 环境；在该测试作用域允许 Preview 事件后验证，真实网络测试不使用此保护。

历史失败报告和日志保留于 `Saved/DamageNumbers`，不提交生成物，不覆盖失败证据。

## 最终 PIE 结果与媒体

执行 `python Tools/DamageNumbers/run_batch.py listen dedicated listen-host listen-lag single`，五种模式全部成功：listen 20 项、dedicated 21 项、listen-host 20 项、listen-lag 20 项、single 20 项，共 101 次断言检查执行，包含重复的跨模式场景，不代表 101 种不同玩法。

每种模式最终均为服务器入队 58、本地接收 58、提交 Niagara 58；有其他玩家的场景，其接收数为 0。各模式均验证正式五段、E/R、两个目标分别扣血、同帧 40 笔事件全部收到、小数原值、AI 不归给玩家、共享一个 NiagaraComponent，以及最后 10 HP 承受 100 后本地收到 10。Dedicated 实例没有创建 Niagara 显示组件，也没有本地接收事件。这里证明的是 PIE Dedicated，不是独立 Server Target 构建。

报告：`Saved/DamageNumbers/{single,listen,dedicated,listen-host,listen-lag}-numbers-e2e.json`；汇总日志：`paced-network-console.log`。最后一次 single 报告来自额外离线录帧复测，之前使用真实帧时间的单人报告保留在 history 文件中；网络四种模式均使用实际帧时间。

执行 `python Tools/DamageNumbers/run_batch.py --capture single` 和 `python Tools/DamageNumbers/encode_capture.py`：449 张实际 PIE 世界画面编码为 960×540、60fps、约 7.48 秒的五段普攻演示，路径 `Saved/DamageNumbers/lyra-formal-combo-60fps-offline.mp4`。已检查第 30、180、360 帧，能看到真实命中数字出现、错开以及结束后消失。使用 `t.OverrideFPS 60` 固定仿真步长，属于离线录制，不是实时性能测量，不宣称逐帧达到《鸣潮》视觉。

结束核实：地图 `/Game/CodexText/L_MainMenu`，PIE 世界 0，脏包 0，`t.OverrideFPS=0`，Weapon/Body/HitBox 调试值均恢复 1。测试编辑器已正常退出，单人 PIE 基线与记录的性能值已恢复。迁入六资源再次通过 SHA256 校验。工作区仍为 main，未提交、未改动封存分支。

## 验证边界

视觉仍为原 Lyra Niagara，不宣称已还原《鸣潮》。正式战斗不伪造暴击，元素配色没有另行扩展。固定步长录帧属于离线演示，不代表实时 60fps 性能。分屏视图隔离、主动断线重连、完整 Pawn 重生交接、长时间压力、Unreal Insights 对照、Cook/打包和独立 Dedicated Server 可执行文件未在本轮证明。

没有将编译、原生测试或接收计数等同于资产美术验收。原 Lyra 字形较大且带蓝紫渐变，和此前《鸣潮》参考存在视觉差异，后续应以用户实际画面反馈决定是否调整。
