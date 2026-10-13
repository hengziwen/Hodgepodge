# 共享战斗请求实施与验证

日期：2026-10-11。UE 5.5.4。[设计与完整接口](../Design/combat-action-requests.md)。本轮最终原生测试为 65 项通过、0 失败（37 项带警告，包含预期失败诊断与既有调试输出）；Saved/CombatActionRequests 保留实际命令日志、失败尝试和原始采样，不纳入源码。

## 0. 已执行结果

- Editor 常规构建：EditorBuild8.log，退出 0。
- Game 常规构建：GameBuild5.log，退出 0。
- Native5：65/65，通过；8 项请求／兼容测试与 57 项既有回归。
- 共享连段：单人 9/9、Listen Server 11/11，通过；原始报告 shared-single.json／shared-network.json。
- 独立技能闭环：单人 14/14、Listen Server 18/18，通过，包括无自动混出死亡收尾。
- Guardian 与验证蓝图 9 份编译通过，均无错误／警告。
- [机器可读验证摘要](combat-action-request-results-2026-10-11.json)。PIE 已结束，无未保存内容／地图，节流设置已恢复。

## 1. 本轮实现

- 新增 [请求类型和状态](../../Source/Hodgepodge/Public/Combat/HodgeCombatActionRequest.h)。
- [CombatComponent](../../Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp) 提供独立技能请求、语义意图、查询、精确取消、准备检查和原生／蓝图结果通知；沿用同一组件和同一连段执行逻辑。
- [ASC](../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp) 将玩家独立 Definition 的输入激活转交共享入口，其他普通 GA 保持原输入策略。
- [Definition GA](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp) 统一查询玩家／敌人战斗准备，不改变 GAS 激活、消耗、互斥或客户端预测。
- [BTTask](../../Source/Hodgepodge/Private/AI/HodgeBTTask_ExecuteAbility.cpp) 支持 StandaloneAbility 和 ComboIntent；通过请求结果等待及取消，保留目标、朝向、Timeout、Attempt 与 Possess 生命周期。
- 请求身份覆盖缓存替换、过期、同步结束、授予撤销、解绑、旧取消和移动取消确认后的缓存恢复。结果 FIFO 通知避免重入取消导致监听者看到倒序状态。
- 死亡回归发现当前 AM_Death 关闭自动混出；[EnemyDeath GA](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.cpp) 增加实际播放末帧检测，保留资产设置与原 Death 事件／Health 生命周期。

## 2. 资源与可运行配置

既有正式 Guardian 默认保持独立模式，玩家 Combo 表和技能配置不修改。新增 `/Game/Main/Character/Enemies/Guardian/Validation` 的 BP_EnemyCombo、AIC_EnemyCombo、BT／BB、PawnData、AbilitySet、ComboDefinition、DataTable 和 ComboCoordinated 攻击 Definition。

新 Combo 图为 Combo.Entry → Combo.Light.01，无玩家 InputBindings，使用既有 InputIntent.Attack.Light。控制器通过行为树 Task 的 ComboIntent 模式提交，验证不需要玩家键位映射。

可在 `/Game/Main/Maps/L_EnemyAIValidation` 放置 BP_EnemyCombo，或者运行 [验证资产脚本](../../Tools/CombatActionRequests/configure_validation_assets.py)与[共享连段测试](../../Tools/CombatActionRequests/run_shared_e2e.py)。先等待 PIE 的服务器和远端 Pawn／ASC 就绪，再启动检查。

## 3. 验证层次

[原生用例](../../Source/Hodgepodge/Private/Tests/HodgeCombatActionRequestTests.cpp) 覆盖：

- Standalone 实际蒙太奇激活、完成、重复拒绝与精确取消。
- Combo 授权、玩家 InputTag 映射、AI 无 InputBindings 的意图提交。
- 无窗口时缓存、替换通知、过期及不取消当前源动作。
- 同步播放失败、开始通知中重入取消、所有监听者的事件顺序。
- 无效配置／就绪、零生命、技能撤销、历史上限及 Shutdown。
- 移动取消确认时缓存 RequestId 保留并启动后续动作。
- 死亡蒙太奇保留末帧且无自动混出时仍完成 Health 死亡生命周期。

同时回归 Enemy、Combo、Attributes、WeaponPresentation、HitReaction、Combat、Rotation、Facing、Movement 和 Relationship。

真实 PIE 覆盖新 Combo AI 的就绪、节点授权、重复攻击与伤害；玩家真实 Enhanced Input 进入第一段、在缓存覆盖窗口内续接第二段。Listen Server 从远端 AutonomousProxy 输入，验证本地表现、服务器两段授权及实际伤害，并观察远端 AI 动画。

独立回归通过共享 RequestStandaloneAbility 提交玩家测试攻击，覆盖原 Guardian 追击、攻击任务结束、受击限制／恢复、重复初始化不回血、死亡停机／销毁和远端表现。

## 4. 实际命令

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.ActionRequests+Hodge.Enemy+Hodge.Combo+Hodge.Attributes+Hodge.WeaponPresentation+Hodge.HitReaction+Hodge.Combat+Hodge.Rotation+Hodge.Facing+Hodge.Movement+Hodge.Relationship' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/CombatActionRequests/Native5' '-abslog=E:/Project/Git/Hodgepodge/Saved/CombatActionRequests/Native5.log'
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/CombatActionRequests/configure_validation_assets.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/start_validation.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/CombatActionRequests/run_shared_e2e.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/stop_validation.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/start_network_validation.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/CombatActionRequests/run_shared_e2e.py
# 分别在新的单人／Listen Server 会话运行独立技能回归，等待最终 JSON 后再结束。
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/CombatActionRequests/run_standalone_e2e.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/stop_validation.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/restore_validation.py
git diff --check
```

新增 cpp 后使用过一次 -NoUBTMakefiles 使 UBT 重新收集源文件，没有修改项目构建设置。Native1 只有 57 项旧回归，不能作为新增测试证据；Native2／3 含新增用例，Native4 的新死亡用例没有推进足够的 Timer 帧，改为真实逐帧 latent 测试后通过，最终以 Native5 为准。

## 5. 实现边界与问题记录

- RequestId 为本地跟踪身份，玩家预测仍由现有 PredictionKey／Combo Payload 关联服务器；Started 是本地执行开始，不能宣传为服务器最终确认。
- 本轮不新增任意普通 GA 的通用请求框架，不把 Dash／Sprint 或 Death 事件改成 Combo 输入。AI 距离、目标和朝向仍由适配器负责。
- 最多 16 个活动／待处理、64 个历史记录；未知／已裁剪身份查询返回 false。过期的输入不会为了“保证连招”被放宽窗口。
- 回调中嵌套提交被拒绝；同一请求终结后不再改变状态。客户端已开始的 Combo 取消继续走原服务器确认协议。
- 测试脚本曾在客户端未完成复制时过早访问 Pawn，调整为就绪后运行；已清理失败尝试的临时授予。第一次输入注入类型错误改用项目已有向量注入方式；第二次按键早于缓存可覆盖窗口，按正确时间重跑，不改玩法窗口来迎合测试。
- AM_Death 当前关闭自动混出造成死亡完成事件缺失，已增加进度检测并单独验证；未重写用户的蒙太奇配置。
- 未执行 Dedicated Server、延迟／丢包压力、Cook／打包或行／分支覆盖率插桩。未自动提交 Git，保留原有未提交工作和装备插槽修改。
