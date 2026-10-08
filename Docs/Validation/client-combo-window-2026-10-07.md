# 2026-10-07：客户端连段和移动取消窗口修复

## 原因与证据

Listen Server 远端玩家及 Play As Client：拥有端预测接段，服务器拒绝并回退；移动取消也受影响。用户日志多次出现 ClientActivateAbilityFailed，原日志保留在 `Saved/ClientComboFix/user-acceptance.log`。

受控复现中，窗口打开后下一帧点击成功；提前输入缓存于 NotifyBegin 首帧消耗失败。拥有端预测第二段时，服务器第一段 Montage 位置为 **0.599960327 秒**，尚未到 **0.6 秒**通知起点；下一帧服务器窗口已经打开，客户端却已被拒绝。原因是 RPC 处理早于动画更新，以及两端动作时钟偏差，不是通知配置缺失。

首版仅延后一帧，六次输入通过，但连续第二轮仍误拒绝。最终处理在现有输入缓存期限内等待权威通知实际开启。

进一步连续十轮暴露累计动画进度偏差：通知回调内预测切段会额外推进本帧动画，连续到第六轮时两端起点差约 0.4 秒，超过 0.3 秒缓存范围。最终增加已确认连段的拥有端动画时钟校正，避免只扩大等待期限掩盖漂移。

## 最终处理

- ASC 核对 Avatar、来源节点及激活键、Definition、输入消息类型、目标授予句柄与状态资格。目标边有效但当前 GA 窗口尚未开启时保存请求，在 PostPhysics 之后的 TimerManager 阶段重查。
- 缓存期限采用 ComboDefinition.InputBufferSeconds；不推算通知时间、不提前写窗口标签。窗口已满足时直接处理，避免无条件每段延迟一帧。超时拒绝，实际激活仍经过原 PrepareServerActivation、优先级选边及 CanActivateAbility。
- 载荷由 ASC 的 Transient UPROPERTY 持有，回调弱引用组件与 Avatar；换 Pawn、EndPlay 清空，最多 256 个挂起请求。源身份变化或状态禁用不能继续等待并获得授权。
- 移动取消在动画更新后的 TimerManager 阶段核对窗口、SpecHandle、激活键与 Avatar，处理后回复客户端。
- 服务器成功激活后确认本次预测键及实际等待秒数；拥有端按本地激活时刻扣除等待时间校正多推进的 Montage 进度，保留正常网络预测。只校正仍在执行的同一 Avatar／Spec／预测键／Montage，过时确认忽略；不创建或提前授予窗口标签。

业务修改为 HodgeAbilitySystemComponent.h/.cpp、HodgeCombatComponentBase.h/.cpp；负向断言补入已有 HodgeAbilityDefinitionTests.cpp。没有改 Montage、连段表或伤害配置，没有新增角色组件。

## 验证

原始收据在 `Saved/ClientComboFix`；四个业务源码备份在该目录的 Before 中，约 143 KB，没有再次创建全项目备份。

客户端脚本 `Tools/AnimNotifyCombat/test_client_combo_windows.py` 用真实 EnhancedInput 提前 0.15 秒输入，连续两轮攻击，同时核对拥有端与权威服务器全部 11 次激活，结束检查检测会话和骨骼租用归零。Python 不直接调用客户端 Server RPC。

最终构建：Editor-clock-build.log 退出 0，Game-clock-build.log 退出 0；AutomationClockFinal/index.json 的 18 项全部 Success。Retention 中新增断言覆盖关闭窗口不授权、其他激活键、错误消息类型／源节点、无效目标句柄和输入禁用。

已完成的正式连段网络回归：

- Play As Client、100ms Packet Lag，第一个客户端连续十轮 **51 次**激活，两端顺序完全一致，结束后检测会话与骨骼租用均为 0。原始记录 stress-client-clock-lag100-final.json；最后一次拥有端／服务器激活时间约 41.971／42.098 秒，未再累计到缓存期限之外。
- 同模式第二个客户端连续两轮 **11 次**激活，两端一致并清理完成，记录 second-client-combo.json。
- Listen Server 远端玩家，0ms 和 100ms 两种 Packet Lag，各连续两轮 **11 次**激活，两端一致并清理完成，记录 combo-listen-clock-zero-final.json、combo-listen-clock-lag100-final.json。
- 每轮顺序为 01→02→03→05→04→01，提前输入缓存消费、末段后摇直接回第一段均包含在上述测试中；每帧检查两端至多一个连段 GA 活动。
- Play As Client 100ms 移动取消：服务器约 0.770 秒结束，拥有端约 0.891 秒确认，早于自然结束；窗口开启前没有取消，记录 move-client-clock-lag100-final.json。
- Listen Server 100ms 移动取消：服务器约 0.758 秒结束，拥有端约 0.867 秒确认，同样不在关闭窗口中取消，记录 move-listen-clock-lag100-final.json。
- 过期输入：在 0.1 秒点击，缓存早于 0.6 秒窗口过期，两端均只执行第一段，early-input.json 保留最近一次模式结果。

以上延迟为 NetEmulation.PktLag=100 的配置值，不声称真实网络 RTT 恰为 100ms。完整十轮脚本是已发布两轮脚本的同逻辑扩展，原始版本和收据保存在 Saved/ClientComboFix/stress-combo.py 与对应结果中。

最终编辑器会话日志未出现 ClientActivateAbilityFailed。文档 refresh／check 通过，53 份 Markdown 检查为 0 链接／快照错误与 0 漂移；git diff --check 退出 0，保留既有 LF／CRLF 提示。收尾检查无未保存资产／地图，关闭 PIE，清除测试 Packet Lag，恢复双人 Listen 设置与正常前后台节流。没有提交代码。

实际工程命令（UE 5.5.4）：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/ClientComboFix/AutomationClockFinal' '-abslog=E:/Project/Git/Hodgepodge/Saved/ClientComboFix/AutomationClockFinal.log'
python Docs/KnowledgeBase/tools/kb.py refresh
python Docs/KnowledgeBase/tools/kb.py check
git diff --check
```

## 历史范围与未验证项

[通知迁移报告](anim-notify-migration-2026-10-07.md) 的网络八项主要覆盖独立命中技能，正式连段六次输入主要在单人执行。它没有充分覆盖远端客户端缓存于 NotifyBegin 首帧消耗的正式连段，本报告补充该缺口，历史命中网络结果保持原范围。

本次没有单独重新编译／重存蓝图，PIE 使用正式 GA 与 Montage。未做独立进程、独立 Server Target、丢包／抖动压力、Cook 或完整打包。PIE Client 使用编辑器内服务器，不等于独立专服可执行文件通过。

已确认时钟校正针对当前单 Section、固定倍率的正式五段普攻；显式多 Section 跳转／循环或运行中变速不会被按线性时间重置，保留原生 Montage 行为。这些扩展连段组合未做本轮网络压力验证。
