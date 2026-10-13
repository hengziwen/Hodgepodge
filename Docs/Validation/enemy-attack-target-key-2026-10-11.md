# 敌人攻击目标键回归修复 · 2026-10-11

UE 5.5.4。用户反馈统一战斗请求后测试怪物不攻击；本轮修复目标键契约，保留共享请求架构、用户已修复的武器插槽及现有角色／HUD。

## 原因与证据

原样复现用户当前地图 L_EnemyAIValidation 与 Play As Client 双客户端模式。服务器上的徒手 Guardian 已 Ready，Brain 为 Running，黑板 CanAct／HasLineOfSight／InAttackRange 均为 true。8 秒被动采样没有攻击蒙太奇或共享请求事件。

实际 BT_Guardian 的攻击任务 TargetActor.SelectedKeyName 为 SelfActor，模板与运行实例一致；BB_Guardian.TargetActor 的 Object BaseClass 为 UObject。任务 AddObjectFilter 要求 AActor，故 TargetActor 被类型过滤排除。UE 5.5 BlackboardSelectorDetails::InitKeyFromProperty 对不在候选中的选择回退到首个可选键，本例为 SelfActor。ExecuteTask 读到怪物自己，IsValidEnemyTarget 拒绝，在 RequestStandaloneAbility 之前退出。

所以本次不是“没有授予 GA”，也不是必须撤掉统一请求层。早期脚本直接制作／运行树而未覆盖打开编辑器后的黑板类型筛选，漏掉了该回归。

## 修复

- 正式 BB_Guardian 和 Combo 验证 BB_EnemyCombo 的 TargetActor Object BaseClass 改为 Actor；只修复对应任务的 None／SelfActor 自动回退为 TargetActor，不重建树或修改其他合法目标键。
- ConfigureEnemyBehaviorTree 生成 Object 键时明确 BaseClass=AActor，避免制作工具再次生成不匹配配置。
- UHodgeBTTask_ExecuteAbility 默认选择 TargetActor，InitializeFromAsset 解析 ID／类型；Actor／Pawn 键合法，UObject、非 Object、SelfActor 和缺失黑板拒绝，初始化给出诊断。配置无效时不会进入请求提交。
- 新增 Hodge.Enemy.AI.TargetKeyContract：覆盖 Actor 筛选、原 UObject 被排除、自定义 Pawn 键保留、正确 ID、错误类型／自身／缺失黑板。
- 迁移脚本 Tools/CombatActionRequests/repair_target_keys.py 只处理两份生成树；运行采样脚本 test_enemy_attack_regression.py 使用自然 BT 攻击，观察实例与伤害，不直接激活能力。

## 构建与原生测试

常规 EditorBuild2.log、GameBuild.log 均退出 0；未用 Live Coding。首次 EditorBuild.log 因编辑器尚未退出出现 LNK1104，之后确认进程退出重新构建。

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Enemy+Hodge.ActionRequests' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/AIRequestRegression/Native' '-abslog=E:/Project/Git/Hodgepodge/Saved/AIRequestRegression/Native.log'
```

24/24 通过，0 失败、0 未运行；1 Success，23 SuccessWithWarnings，含预期非法配置诊断及既有日志，不声称零警告。报告和失败日志留在 Saved/AIRequestRegression。

## 实际 PIE

关闭编辑器后重新打开，确认磁盘加载的 BaseClass=Actor、SelectedKeyName=TargetActor。单人两种正式怪物各自然播放三个不同攻击实例，并造成真实伤害；徒手怪首次阶段生命 150→118，装备怪阶段 102→70，间隔均短于任务 8 秒超时，不以强制激活代替行为树执行。

Listen Server、双客户端 PIE 专服均通过，两种怪物分别观察到三个不同攻击实例、实际扣血及客户端动画。三种模式合计六个怪物攻击场景通过，最终 JSON 均 error=null。Listen／PIE 专服中的相邻实例起手间隔约 3.2～3.6 秒，小于 8 秒任务超时。

额外复跑既有共享 Combo 端到端脚本：单人 9/9，通过，包含 AI 无玩家输入映射提交意图与玩家真实输入衔接；原生共享请求八项也包含在上述 24 项中。最终结果见 [摘要 JSON](enemy-attack-target-key-results-2026-10-11.json)。

联机验证脚本曾提前持有尚未完成旅行的客户端 World，以及仅在服务器移动测试 Pawn；现已等待远端就绪、每帧查询当前世界并按 PlayerId 同步测试位置。没有放宽游戏攻击范围／取消规则来满足测试。结束后恢复原 L_EnemyAIValidation 地图与 Play As Client 双客户端设置，保留原后台限速关闭偏好。

## 验证工具问题与边界

首次额外观测脚本把 Python callable 绑定到 const-reference Blueprint 委托，触发 UE Python 插件 FFrame::StepExplicitProperty 访问异常。已保留崩溃日志，移除该测试绑定，改为 Slate Tick 的只读蒙太奇／生命采样；不修改生产委托签名来绕过测试工具问题。

未运行 Cook／打包、独立进程专服、丢包压力或代码覆盖率插桩；PIE 专服不等于 Server Target。未提交 Git。
