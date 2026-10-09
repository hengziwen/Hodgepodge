# 受击 PIE／联机补测

日期：2026-10-08，UE 5.5.4，Win64。承接[核心验证](hit-reaction-2026-10-08.md)。本轮执行正常 RHI 的真实编辑器 PIE，使用原生世界 Tick、当前攻击 GA、实际动画通知和伤害 GE；不是原生预览世界测试替代 PIE。

## 测试环境与资源

地图 `/Game/ThirdPerson/Maps/ThirdPersonMap`，当前 Experience／正式 Hero／PlayerState ASC 路径。单人 PIE 中创建第二个本地玩家作为目标；联网使用两玩家 Listen Server，同一编辑器进程内存在真实服务器世界和客户端世界，分别核对 Authority、AutonomousProxy、SimulatedProxy。

隔离资源在 `/Game/CodexText/HitReactionValidation`：

- `DA_ReactionProfile`：六种 Impact 的真实蒙太奇、空中／落地／起身衔接；所有条目 `bAllowWithoutMontage=false`。
- `DA_ProbeAttack`、`AM_ProbeAttack`：原生 Melee、独立攻击判定、一次 0.3 秒 Hit Notify、现有 GE，每次实际伤害 5。
- `DA_ProbeHold`、`AM_Hold`：目标长动作及四级 Body，核对是否实际中断。
- `AS_ReactionProbe`、`BP_ReactionGrant`：通过现有 EquipmentManager／AbilitySet 授予测试能力，玩家 ASC 所有权保持 PlayerState。
- `DA_EnemyPawnData`、`BP_EnemyProbe`：原生 HodgeEnemyCharacter、Pawn 所有权 ASC、AIController、Combat、当前模型／AnimBP，以及正常属性初始化与复制。
- 动画副本关闭根运动，由 Impact／CharacterMovement 负责胶囊移动；不改第三方源动画、正式 Skeleton、正式技能或地图。

测试脚本见 [Tools/HitReaction](../../Tools/HitReaction/README.md)。当前资源用于验证运行链路，不代表正式各类怪物的受击动画已经配置完成。

## 已执行结果

以下基础组每组 7 项：Skill 同级中断、低于 SuperArmor 仅轻反馈、Vajra 同级中断、击退、挑飞、倒地起身、腾空过程中取消恢复。

- 单人 PIE：7 项通过，`Saved/HitReactionPIE/single.json`。
- Listen Server，远端拥有者目标，默认延迟：7 项通过，`network_owner_0ms.json`。
- Listen Server，客户端发起攻击／模拟代理观察目标，默认延迟：7 项通过，`network_simulated_0ms.json`。
- 上述模拟代理路径，100ms 包延迟：7 项通过，`network_simulated_100ms.json`。
- 远端拥有者目标，100ms 包延迟：7 项通过，`network_owner_100ms.json`。
- 客户端攻击原生怪物，100ms 包延迟：7 项通过，`network_enemy_100ms.json`。

配置命令是两 PIE 世界的 `NetEmulation.PktLag 100`，含义是各 NetDriver 模拟包延迟；不将它描述为实测 RTT。拥有者延迟样本中，同级受击控制由服务器约 0.317 秒开始，客户端约 0.450 秒收到，证明测试确实经过延迟复制。

每项采样真实生命、控制阶段、蒙太奇播放与位置、胶囊位置／速度及角色网络 Role。强受击核对目标 Hold GA 被取消；低判定核对 Hold 仍激活、胶囊没有受击位移、OnLightFeedback 到达。联网核对恢复后的控制解除、生命一致性和位置收敛；击退／挑飞另核对客户端确实出现对应位移。默认延迟拥有者组为最初基础断言；客户端轻反馈和更完整的位置断言已在后续拥有者 100ms 组核对。

扩展组 4 项通过，`network_enemy_extended_100ms.json`：Launch → AirHit 实际追加一次托举、两次伤害合计 10，最高上升约 499 厘米；Launch → Slam 实际向下运动并衔接 Downed／GettingUp；墙体将预期 150 厘米击退限制到约 69 厘米；天花板将自由挑飞约 250 厘米限制到约 157 厘米。模拟代理同步动画、阶段、生命及最终位置。

合计 **46 项真实 PIE／联机用例通过**。自由地面击退约 150 厘米、自由挑飞约 250 厘米；本轮联网强受击用例恢复后的最大位置误差小于 1 厘米，验收阈值为 5 厘米。最后怪物组及扩展组使用修正后的 UObject 监听器，在 `Editor5.log` 中记录，未再出现上述属性所属 ASC 断言或成员委托 GC 崩溃。

## 本轮修复与测试问题

1. 单人 PIE 首次发现动画 Skeleton 的严格指针比较会拒绝项目已配置的兼容骨架。改为与当前网格相同 Skeleton 或 `Skeleton.IsCompatibleMesh`，并加入原生回归；之后动画与运动用例通过。
2. 怪物联机首次命中触发 `HodgeHealthSet.OnRep_Health` 的 GAS 属性所属 ASC 断言。属性复制可早于 PawnData／PawnExtension 绑定，敌人 GAS 接口现在优先返回已有绑定，否则返回其原生 ASC，确保早期属性复制可解析所属组件；业务初始化仍由原入口负责。修复后怪物基础组通过。
3. 最初 Python callable 委托未被稳定持有，导致轻反馈计数漏记。随后保留成员委托包装器，又在销毁／重建怪物后触发 PythonScriptPlugin GC 调用栈崩溃。验证脚本改为持有独立 UObject 监听器，在结束 PIE 时解绑，不保留指向已销毁角色成员的 delegate wrapper。
4. 取消用例原先可能在服务器首次控制的同一帧取消，复制合法合并了中间状态。现在从首次进入控制起等待 0.35 秒再取消，核对客户端先收到控制，再收到解除；即时取消的原生 PendingLaunch 清理由原生测试覆盖。

中间失败及调用栈保留在 `Editor2.log`、`Editor3.log`、`Editor4.log`；最终测试使用修复后的编辑器。创建缺失测试资产时的首次 LoadPackage 警告和项目已有警告不隐藏。早期单人第二本地玩家曾触发已有 CommonUI 输入路由 ensure；测试场景把备用玩家放到地板外时也曾触发已有自毁 GE 缺失错误，随后修正测试摆放位置。

## 构建与回归命令

关闭原编辑器后完成常规构建，未使用 Live Coding。骨架兼容修复与敌人早期 ASC 修复均执行 Editor／Game 构建；最终两目标退出 0，日志 `Saved/HitReactionPIE/EditorBuildEnemyFix.log`、`GameBuildEnemyFix.log`。

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=2
```

最终构建后在普通编辑器执行 `Automation RunTests Hodge.HitReaction+Hodge.Combat+Hodge.Combo+Hodge.Rotation`，18 项全部成功，队列明确完成 18 项；日志 `Editor4.log`。新增检查覆盖兼容骨架、PawnData 到达前的敌人 GAS 接口及 AttributeSet 所属 ASC。

实际 PIE 脚本调用格式：

```powershell
& 'E:/Python/python.exe' Tools/HitReaction/mcp_execute.py Tools/HitReaction/create_validation_assets.py
& 'E:/Python/python.exe' Tools/HitReaction/mcp_execute.py Tools/HitReaction/create_enemy_fixture.py
& 'E:/Python/python.exe' Tools/HitReaction/mcp_execute.py Tools/HitReaction/start_pie.py
# 单人：setup_single.py → setup_actors.py → run_suite.py → stop_pie.py
# 双人：start_network.py → setup_network.py → setup_actors.py → run_suite.py
# 反向／延迟：switch_network_direction.py、set_network_lag.py、switch_network_owner.py 后 run_suite.py
# 怪物：setup_enemy.py → 等待复制 → setup_enemy_observer.py → run_suite.py
# 扩展：select_extended.py → run_suite.py
```

脚本返回 Started 只表示开始；通过以对应 JSON 的完整用例数及 `error=null` 为依据。日志和逐帧采样保存在忽略的 Saved 内，源码及隔离资产保留以便复现，没有提交或回退用户改动。

## 仍未覆盖

- 正式怪物资产的 Additive 轻抖、独立 HitSlot 混合和人工美术观感验收；本轮轻反馈验证的是通知到达、不中断和无控制／位移。
- 跨机器／独立进程网络、模拟丢包、Dedicated Server 构建、Cook 和打包。
- 死亡／复活／Pawn 更换的完整 PIE 联机流程；本轮已做原生生命拒绝、解绑与旧 Avatar 资源清理回归。
- 玩家升空追击、完整空中连招和吸附仍是后续功能；韧性仍只预留数值。
