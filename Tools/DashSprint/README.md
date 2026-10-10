# Dash／Sprint 验证工具

适用当前 UE 5.5.4 项目。实际配置见 Docs/Guides/dash-sprint-configuration.md，验证范围见 Docs/Validation/dash-sprint-implementation-2026-10-10.md。

通过既有 Tools/HitReaction/mcp_execute.py 执行脚本，内部读取本地网关令牌；不打印凭据。

configure_hero.py 仅创建／配置指定 Main 资源、输入、授予与两份动画图。图有人工作业后先审查，再运行作者脚本；不要无检查覆盖。

单人准备：CharacterFacing/start_single.py → setup_single.py → setup_actors.py → DashSprint/run_e2e.py。

双客户端准备：CharacterFacing/start_network.py → HitReaction/setup_two_clients.py → setup_actors.py → DashSprint/run_e2e.py。另运行 run_combat_integration.py 与 run_hit_exit.py。

每个场景脚本注册 Slate 回调后返回，Started 不是通过。必须读取 Saved/DashSprintImplementation 的 JSON，检查 error 为空、所有 passed 为 True、场景数量完整。set_network_lag.py 对全部世界设置 100ms 包延迟；复跑前确认 lag_ms 标记和实际网络配置一致。

run_hit_exit.py 只修改隔离探针 DefaultHitConfig，完成恢复；不要保存运行时探针。每组结束运行 HitReaction/stop_pie.py，清理网络模拟与回调。

原生测试组 Hodge.Movement 与 Hodge.Facing.ActivationPair，加原有旋转／战斗回归。最终报告 Native6/index.json 与原始 XML 位于 Saved；summarize_results.py 校验原生／31 项 E2E 并计算 Runtime 改动行覆盖率，含未跟踪新增文件。统计只有 PDB 可测行，不提供分支或蓝图覆盖率。

## 标签映射中断（v1.5）

`configure_interrupt_mapping.py` 配置当前玩家的关系映射、PawnData 绑定和五个普攻 GA 分类；`verify_interrupt_configuration.py` 重载已保存资产并检查规则。执行前要求无 PIE、无未保存内容；作者脚本保留无关标签和映射条目。

按上述准备启动 PIE 后，将测试上下文 `movement_output_dir` 设为 `DashInterrupt`，顺序执行 `run_e2e.py`、`run_combat_integration.py`、`run_interrupt_integration.py`。后一组测试包含真实死亡事件，必须最后运行；结束后重新启动 PIE 才能复跑新的网络条件。不要在前一组回调尚未完成时启动下一组。

中断测试覆盖五段攻击在旧取消窗口之前启动 Dash、窗口内取消、三种死亡状态及实际 Death 能力。读取 `Saved/DashInterrupt/*-interrupt.json`，确认 error 为空、10 项场景完整。网络组同时采样拥有者、服务器和观察者蒙太奇，并检查取消后的命中会话和姿势租约清理。原生 `Hodge.Relationship` 另验证不可取消阶段、攻击／Death 双标签和无关技能。

## Dash 过渡（v1.6）

`configure_dash_transitions.py` 仅接好正式层的连续脚部 IK，并将 Main 默认后摇／衔接开放与 Duration 对齐。它不修改骨架、蒙太奇素材或角色 Z。

准备角色后先执行 `prepare_validation_arena.py`，扩大地面并将第二名玩家在服务器和客户端移至移动路线外，避免胶囊阻挡和越过地面边界污染过渡指标。测试上下文设置 `movement_output_dir='DashTransitions'`、`transition_phase='fixed'` 后，顺序执行 `run_dash_transitions.py`（7 项高度／响应／速度连续性场景）、`run_e2e.py`（10 项含 Pivot）、`run_combat_integration.py`（3 项攻击交互），每组完成后再运行下一组。单人、双客户端 0／100ms 分别复跑；结束停止 PIE 并重载临时 Profile 修改。

最终 `summarize_dash_transitions.py` 核对 Native8、全部运行 JSON、骨盆高度差、RMS→Sprint 最低速度及未关联资产 hash。结果位于 Docs/Validation/dash-transitions-results-2026-10-10.json；异常探针保留在 Saved/DashTransitions，不算通过。

## 标记同步与 Pivot 恢复（v1.7）

`configure_animation_sync.py` 只修改四份 Main 移动序列的专用标记轨道、三个蒙太奇、Profile 恢复点和正式循环层分支；执行前要求无 PIE、无脏包。不要用它覆盖尚未评审的手工动画图。

准备隔离地面后，将输出目录设为 AnimationSync，顺序运行 `run_animation_sync.py`（6 项）、`run_e2e.py`（10 项）、`run_dash_transitions.py`（7 项）、`run_combat_integration.py`（3 项）。单人、双客户端 0／100ms 均需完成；后续组不能与前一组回调重叠。新探针检查实际标记组、第二个实例的播放起点／恢复点及转身结果，不只检查 IsPivoting 布尔值。

`montage_listener.py` 记录真实自然结束／中断事件，避免长帧跳过蒙太奇尾段时误报短按提前结束；每个场景组结束必须解绑。高度探针依据实际 BlendOut 计算尾段采样窗口。

`summarize_animation_sync.py` 校验最终原生报告 Native3、全部 78 项 PIE 与非关联 Main 资产 hash。`Hodge.Movement.DebugSprint 1` 可打印退出条件，默认关闭；测试结束恢复临时 CVar、停止 PIE 并清理配置。
