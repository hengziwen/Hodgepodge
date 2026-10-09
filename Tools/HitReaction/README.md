# 受击 PIE 验证入口

使用 UE 5.5.4 的真实 PIE 世界、当前 Hero／AnimBP、原生攻击 GA、实际 Hit Notify 和 GE。报告写入忽略的 `Saved/HitReactionPIE`。测试资源仅保存到 `/Game/CodexText/HitReactionValidation`，不保存正式 Hero、地图、Skeleton 或技能资产。

`mcp_execute.py` 使用项目本地已启用的 MCP 网关，在内部读取 `Saved/MCP/capability-token`，不输出凭据。先打开项目编辑器，确保网关运行。

PowerShell 调用格式：

```powershell
& 'E:/Python/python.exe' Tools/HitReaction/mcp_execute.py Tools/HitReaction/create_validation_assets.py
& 'E:/Python/python.exe' Tools/HitReaction/mcp_execute.py Tools/HitReaction/create_enemy_fixture.py
```

资源创建应在 PIE 开始前执行。动画副本移除根运动，蒙太奇使用 FullBody；ProbeAttack 在 0.3 秒发出一次真实命中通知。Profile 禁止无动画回退。

单人依次执行 `start_pie.py`、`setup_single.py`、`setup_actors.py`、`run_suite.py`。第二个本地玩家作为受击目标，验证当前完整攻击／伤害链。结束后执行 `stop_pie.py`，等待 PIE 世界关闭。

联机依次执行 `start_network.py`、`setup_network.py`、`setup_actors.py`、`run_suite.py`，先验证远端拥有者角色。等待报告完成，再执行 `switch_network_direction.py`、`run_suite.py`，验证客户端发起攻击与模拟代理。`set_network_lag.py` 为两世界设置 `NetEmulation.PktLag 100`；之后可分别用 `switch_network_direction.py` 和 `switch_network_owner.py` 选择方向并复跑。

每次 `run_suite.py` 返回“Started”只代表已启动。等待对应 JSON 出现并检查 `error == null`、基础组 `cases` 数量为 7；不要同时启动两组测试。延迟单位是每个 NetDriver 的包延迟，不是声明真实网络 RTT 为 100 毫秒。

怪物联机需要事先创建 Enemy 资源，在上述联机配置后执行 `setup_enemy.py`，等待客户端生成怪物，再执行 `setup_enemy_observer.py`、`run_suite.py`。使用原生 HodgeEnemyCharacter、Pawn 所有权 ASC、AIController 和复制后的模拟代理。`select_extended.py` 后再运行测试会选择 4 项额外用例：Launch → AirHit、Launch → Slam、撞墙击退、碰顶挑飞。

基础组检查等级、扣血、目标动作中断／保留、真实蒙太奇播放、位移、倒地起身及取消恢复；联机额外检查拥有者／观察者控制、动画、生命一致性、最终位置误差小于 5 厘米。轻反馈使用保留的 UObject 监听器，结束 PIE 时解绑；不持有指向角色成员内存的 Python delegate wrapper。

结束测试必须执行 `stop_pie.py`。测试中的 Definition／Body 变更只在内存，不要对正式资产或全部 Dirty Package 执行保存。常规构建、结果和未覆盖范围见 [本次验证记录](../../Docs/Validation/hit-reaction-pie-network-2026-10-08.md)。
