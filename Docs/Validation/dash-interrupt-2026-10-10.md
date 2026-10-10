# 当前玩家 Dash 标签中断：实施与验证

日期：2026-10-10。引擎：UE 5.5.4。目录：E:/Project/Git/Hodgepodge。

## 目标与阶段

使用既有 Tag Relationship Mapping，让 Dash 中断当前玩家所有攻击类技能，Death 状态或标签具有优先取消豁免。交付包含已保存的玩家资产、运行代码、原生／PIE／双客户端测试与配置文档。执行人是本任务代理；用户未指定硬性截止时间，以构建、配置、运行验证和交付为阶段节点。

原有 Dash／Sprint／Pivot、体力移除和旋转改动是此前进度，不从头重做。本轮备份位于 Saved/DashInterrupt/baseline，新增涉及的五个攻击蓝图也另行备份。

## 实现

- 关系行新增取消豁免和 `bForceCancel`，修正取消查询的父子标签匹配方向，提供蓝图可查询接口。
- ASC 的映射取消检查目标 AssetTags 与 DynamicSpecSourceTags，先判 Death／配置豁免，再判匹配和强制权限。正常 GAS 取消完成能力、任务、窗口、姿势与命中会话清理。
- Dash 移除旧攻击窗口准入和 ExclusiveReplaceable 切换，保持 Independent；攻击范围由映射定义，不通过定义类强转或激活组替换判断。
- 移动能力准入检查死亡状态与活动死亡能力；原生 Melee／Death 添加基础分类。
- 玩家 PawnData 绑定新映射，五个 Main 普攻蓝图补齐攻击分类。蓝图编译、保存和重载检查均执行，未批量重存无关资产。
- 编辑器验证辅助增加原生下一帧 GameplayEvent 入口，仅用于 PIE，避免编辑器脚本保护影响死亡事件网络路径。

资产路径、规则字段及新攻击接入见[配置指南](../Guides/dash-interrupt-configuration.md)。设计和原配置手册已同步取消窗口的新含义。

## 发现并修复的问题

1. PawnData 原先没有关系映射，五个攻击 GA 的 AbilityTags 为空，Dash 仍依赖后摇窗口。已配置分类与映射，移除 Dash 的旧窗口限制。
2. 普通 GAS 的标签取消没有逐目标豁免表达能力。已新增映射取消豁免，并保留不可删除的 Death 底层保护。
3. Editor 第一次构建发现新增测试复制 FNativeGameplayTag，以及通用 ASC 指针调用项目方法的编译错误。已改用 FGameplayTag 和正确类型转换。
4. Game 第一次构建暴露 Editor 合并编译掩盖的 GameplayTags 头文件缺失。已补显式 include，最终 Game 构建通过。
5. 首轮死亡探针直接激活需要事件数据的蓝图，并在 Pawn 销毁后仍访问组件，报告未落盘。已改为正式死亡 GameplayEvent，收尾支持已销毁 Pawn；早期探针不计入通过结果。
6. 首轮 100ms 正式死亡测试在服务器死亡通知到达拥有者之前采到本地 Dash 预测，约 40ms 后撤销；服务器从未批准 Dash，Death 保持活动。复测分别断言服务器始终否决、拥有者已知死亡后不能预测、未知状态下的预测最终撤销，并在收到死亡后再次按键。保留首轮报告 `network-100ms-interrupt-first.json`，不把它计为通过。
7. 复跑探针暴露其按技能编号判断连段前进的错误假设：当前正式连段表的顺序为 1→2→3→5→4。探针已改为读取表内窗口，每个蒙太奇实例只发送一次续段输入；正式连段表未修改。保留失败报告 `network-100ms-interrupt-order-probe.json`。

## 构建与原生验证

实际执行命令如下。日志在 Saved/DashInterrupt，初次失败日志保留，最终依据为 EditorBuild3、GameBuild2 和 Native2。

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Relationship+Hodge.Movement+Hodge.Facing+Hodge.Rotation+Hodge.Combo+Hodge.Combat+Hodge.HitReaction' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/DashInterrupt/Native2' '-abslog=E:/Project/Git/Hodgepodge/Saved/DashInterrupt/Native2.log'
```

Editor／Game 常规构建均退出 0。原生 34 项全部成功，其中 25 项无警告、9 项带既有警告；无失败、无未运行。新的 Hodge.Relationship 两组均零错误、零警告。

原生断言包含父标签匹配攻击子标签、源标签不匹配、空集合、配置豁免、Death 与攻击双标签、Death 状态子标签、强制取消权限、未授权强制时保留不可取消阶段、已授权后的实际取消、无关技能保留及死亡状态取消否决。原有移动、旋转、连招与受击测试一并回归。

## 运行验证范围

使用默认 ThirdPersonMap、正式 Main Hero／PawnData／动画图、真实 Enhanced Input 注入和原有隔离探针。按顺序执行移动回归、攻击交互、中断／死亡测试；最后一组有真实死亡，之后停止并重新启动 PIE。

每种条件检查五段普攻在旧窗口之前被 Dash 中断、窗口内取消、Status.Death／Dying／Dead 禁止 Dash、实际死亡能力保留。取消后检查命中会话和姿势租约清理。联机检查拥有者预测、服务器提交与观察者 Dash 蒙太奇；模拟代理上的 ASC 活动能力列表不作为远端动画判定依据。

最终场景计数和结果由 `Tools/DashSprint/summarize_interrupt_results.py` 校验并输出到[机器可读报告](dash-interrupt-results-2026-10-10.json)。只有 error 为空、passed 全部为 True 且数量完整才算通过，Started 输出不算通过。

最终共 69 项场景通过：单人 23 项、双客户端 Play As Client＋PIE 专用服务器正常网络 23 项、全部世界 100ms 包延迟 23 项。每组包含 10 项 Dash／Sprint／Pivot 回归、3 项攻击交互和 10 项中断／死亡检查。正式死亡网络检查以权威否决和已知死亡后的预测撤销为准，不宣称未知远端状态能立即阻止本地预测。

实际编辑器执行使用如下入口；启动、准备、网络模拟和停止脚本的完整顺序见 Tools/DashSprint/README.md。Saved/DashInterrupt/Editor2.log 保留每次执行的脚本 SHA 和输出。

```powershell
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/configure_interrupt_mapping.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/verify_interrupt_configuration.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/run_e2e.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/run_combat_integration.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/DashSprint/run_interrupt_integration.py
& E:/Python/python.exe Tools/DashSprint/summarize_interrupt_results.py
```

执行结束已关闭网络模拟、停止 PIE、重载临时 Profile 修改，并再次重载正式映射／攻击蓝图验证保存结果。最终 PIE 世界 0、脏内容与脏地图包 0。原有 27 个 Main 改动资产中，26 个 hash 保持不变，唯一预期改动为本轮绑定映射的 PawnData；五个原先干净的攻击蓝图和新映射属于本轮指定资产。未自动提交或回退工作区。

## 实现假设与边界

“所有攻击”按 GA 分类标签解释，包括普攻和其他带攻击分类的技能；不默认取消无关独立能力。当前玩家开启映射强制权限，因此临时 CanBeCanceled=false 不作为普通攻击的豁免。Death 是保留标签，不得给普通技能借用来躲避取消。

Dash 活跃期间阻止再次启动攻击，结束后移除自身阻塞，以维持主动作动画所有权。普通移动取消攻击仍受原窗口约束；Dash 不读旧 AttackCancelWindow。当前旋转攻击不使用旋转锁，本轮没有新增旋转锁交接、冷却或体力。

本轮没有采集代码行／分支或蓝图覆盖率百分比；原生断言和运行场景覆盖结果分别记录，不沿用前轮百分比。未执行 Cook／打包、独立进程联机、独立 Dedicated Server 可执行文件构建、丢包或高于 100ms 的延迟压力测试。PIE 专用服务器世界不能等同于独立服务器打包通过。
