# 普攻连段记忆

> 2026-10-06 状态同步：方案已实施；当前 1→2→3→5→4，前四段取消后默认保留 1 秒，末段后摇窗口可直接接 1，末段结束清记忆。 当前项目事实见 [本轮更新](../KnowledgeBase/26-update-2026-10-06.md)。

## 行为约定

- 当前动作与连段记忆由 CombatComponent 分别持有。CurrentComboTag 表示执行节点，ComboMemory.Node 表示最近成功启动的普攻节点。
- 每段成功开始后记录节点；自然结束、移动取消或其他技能打断后，立即清理当前动作及其标签，开始 ComboRetentionSeconds 倒计时。默认 1 秒。
- 其他技能不刷新、不暂停倒计时；输入缓存 InputBufferSeconds 仍为 0.3 秒，只保存一个意图。
- 动作还在播放时，原 RequiredWindowTags 条件继续生效。动作结束后，只允许 bAllowAfterExecutionEnded=true 的输入边续接，RequiredSourceTags、BlockedSourceTags 和技能激活条件继续校验。
- 最后一段在 NextAttack 窗口内可直接接第一段；此跳转不允许动作结束后续接，因此末段结束仍立即清空记忆。时间到期、死亡、换 Pawn、组件卸载及显式回到 Entry 均清空记忆。
- 成功启动即占用该段，允许在出手前被打断后继续下一段；不以动画完成或实际命中为推进条件。

## 网络与失败处理

- 使用 GameState 的服务器时间计算期限，无 GameState 的测试环境使用 World 时间。
- 客户端预测选段，服务器根据自己的记忆、期限、当前执行身份和表中条件重新选择。客户端不能自行声明当前窗口已发生。
- 动作结束后续接请求仍携带上次成功执行的 PredictionKey，记忆过期后才归零；同名节点的旧请求不能绕过当前执行身份校验。
- 仅动作执行期间授予节点标签；连段记忆不授予 Status.Attack，也不维持旋转锁或命中窗口。
- 结束后的权威记忆只复制给拥有者，不把输入/期限交给观察者。预测中的新动作不被上一段的迟到记忆覆盖。
- 激活失败不推进节点；准备切段后激活失败恢复之前的记忆。服务器拒绝请求不清除已确认进度。客户端收到 GAS 激活失败后请求服务器校正记忆，在校正期间暂停选段。

## 当前资产

默认 Experience 使用 /Game/Main/Data/PawnData/DA_Dafult_PawnData，连段定义为 /Game/Main/Data/Combo/DA_LightCombo，表为同目录 DT_LightCombo。

表内现有顺序 1→2→3→5→4 保持不变，前四个输入跳转显式允许动作结束后续接。末段 Combo.Light.04 的输入跳转直接指向 Combo.Light.01，RequiredWindowTags=Status.Attack.Cancel.NextAttack，bAllowAfterExecutionEnded=false，Priority=10。目标为第一段技能节点，不经过需要额外一次输入的 Entry 节点。

末段 Timeline 的 NextAttack、Recovery、MoveCancel 现为 1.2 秒至动画结束；重新开始仍受 NextAttack 窗口约束。五段蒙太奇、Timeline、旋转锁区间、命中配置均不因本任务改变。

## 验证记录

实施前备份：E:/Project/Backups/Hodgepodge/ComboMemory/20261005，含 Source、DefinitionCombo 资产及工作区状态。保留先前动画与旋转任务改动；未自动提交。

实际执行常规构建（Win64 Development），Editor 与 Game 退出码均为 0：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64
```

原生自动化运行命令：

```powershell
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Combo' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/LyraAnimationWork/ComboMemory-20261005/Automation'
```

5 项均为 Success，报告 0 error、1 warning。新 Retention 测试覆盖结束后保留、清理自己的攻击标签、保留其他来源标签、动作中窗口约束、结束后续接、失败恢复、到期重置、末段、零保留时间、同步回调结束、重复结束及组件卸载。既有两处测试蒙太奇路径更新为已迁移的 Main 路径。

第一次编译因测试使用 auto 推导为 FNativeGameplayTag 而非 FGameplayTag 失败，已明确类型。第一次 Retention 断言错误地忽略输入测试 Tag 的父级聚合计数，已改用独立的 InputTag 测试；最终重新构建、测试通过。

临时其他 GA 测试未成功激活，不能作为技能互斥打断通过的依据。该测试修改了内存中的测试默认值，已导出保留现场并重载受影响的 7 个测试包；未保存这些临时修改。五段 Timeline 的已保存 Duration 与窗口配置已恢复，GA_BasicAttack 磁盘哈希与测试前一致。常规构建前编辑器通过 QuitEditor 正常退出。

最终版本单人 PIE 采集 631 条记录：实际 Enhanced Input 移动取消第一段后续第二段；对第二段正式 GA 实例调用 K2_CancelAbility 后续第三段；停止第三段蒙太奇后等待超时回第一段；完整播放 1→2→3→5→4，末段结束后再次点击回第一段。取消通过实例入口执行，未修改正式 GA、Timeline 或类默认值。

最终版本两玩家 Listen Server + 100ms PktLag 采集 1266 条记录：服务器本地玩家与自治客户端均完成移动取消→续第二段→服务器中断→续第三段→记忆超时→回第一段，权威节点与本地节点一致。延迟参数已恢复为 0。

预测拒绝测试采集 34 条记录：只在服务器给远端 Pawn 添加 Gameplay.AbilityInputBlocked，客户端预测续第二段被服务器拒绝；记忆校正后仍保留第一段进度，撤销阻塞后客户端和服务器均启动第二段。测试只撤销自己添加的标签。

最终资产检查：四个续接标记与 1 秒配置正确；五段 Timeline Duration 和窗口未因本任务保存修改；GA_BasicAttack 磁盘哈希与测试前相同，重新打开后从磁盘恢复原来的 5 段配置。编辑器保留正式地图、单人 Standalone，PIE 已停止，未保存内容/地图均为 0。

原始记录位于 Saved/LyraAnimationWork/ComboMemory-20261005，不纳入源码。未执行独立进程、Dedicated Server、Cook/打包；后续冲刺、闪避等正式 GA 的具体取消规则需随其实现验证。

## 调整与验收

1. 打开 /Game/Main/Data/Combo/DA_LightCombo，调整 ComboRetentionSeconds（秒）。0 表示不保留；InputBufferSeconds 仍只控制提前输入。
2. 打开 DT_LightCombo，在相应输入跳转中设置 bAllowAfterExecutionEnded。播放中仍按 RequiredWindowTags 判断，结束后仍检查角色状态与技能可激活性。
3. 第一段进入后摇后移动取消，1 秒内再次点击应播放第二段。等待超过 1 秒再点击应播放第一段。
4. 第二段被中断后在保留时间内再次攻击应接第三段；Attack 4 的后摇接招窗口内点击应直接播放 Attack 1。窗口前最多使用现有 0.3 秒输入缓存，不允许提前跳过末段攻击。
5. Attack 4 自然结束或被取消后，再点击应从 Attack 1 开始；最后一段不保留结束后记忆。
6. 蓝图可查询 GetRememberedComboTag 与 GetComboMemoryRemainingTime；GetCurrentComboTag 仍只表示当前动作节点，结束后回到 Entry。

## 末段后摇重开修复（2026-10-05）

问题为 DT_LightCombo 的末段节点没有 Transitions：Timeline 已打开窗口，但没有目标节点。仅更新该表的一条跳转即可复用现有预测激活、服务器窗口/执行身份校验与输入缓存，无需修改 C++。

修改前已备份连段表、本文和编辑器读取的实际配置，目录 E:/Project/Backups/Hodgepodge/ComboLoop/20261005-194519；源表与备份 SHA256 一致。本次只保存 DT_LightCombo 的 Combo.Light.04 一条跳转和更新本文，未修改 C++、蒙太奇、Timeline 或蓝图图表。

执行命令为 `python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/fix_combo_loop.py combo-loop-fix-receipt.json`，编辑器导入后逐字段比较表的 JSON，并保存该表。通过 `start_pie.py` / `verify_combo_loop.py` 运行实际 Enhanced Input 采集，再用 `stop_pie.py` 和 `restore_main_pie_settings.py` 恢复环境。无需执行本次 C++ 构建或蓝图编译。

单人 PIE 531 条记录：保持 1→2→3→5→4 顺序连续循环两轮；末段 0.5 秒输入未跳过攻击，0.983 秒输入缓存在 1.200 秒接回第一段；第二轮 1.350 秒后摇输入在约 1.367 秒接回第一段。未等 4.667 秒完整动画结束，也未使用移动取消。

两玩家 Listen Server + 100ms PktLag 1338 条记录：两位玩家在两轮末段后摇直接点击均接回第一段，两个服务器权威 Pawn 也完整记录了相同的 11 个执行节点。前几段测试输入留有 0.25 秒窗口余量，末段在约 1.35 秒点击。

边界限制：输入紧贴前几段窗口的网络测试曾出现客户端预测第四段、服务器未确认该段的情况，原始记录 `network-window-edge.json` 保留；未将该测试计为通过，也未调整现有服务器时钟校验。本次通过的网络范围为后摇窗口内直接点击，不能据此声称延迟下所有窗口边界预测均通过。第一版采集还误将未参与游戏的预览 Pawn 纳入两玩家完成条件，已过滤无 PlayerState 的角色。

最终审计只有 Combo.Light.04 行变化，PIE 已停止，正式地图保留单人 Standalone，延迟恢复 0，内容/地图未保存包为 0。原始记录位于 Saved/LyraAnimationWork/ComboLoop-20261005。未执行独立进程、Dedicated Server、Cook/打包；旧验证记录对应原有末段结束行为。
