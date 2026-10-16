# 受击 PIE 验证入口

## 2026-10-16 表现切片

本节对应[修订计划](../../Docs/Design/hit-reaction-system.md#17-2026-10-16-受击表现实施计划修订)与[验证记录](../../Docs/Validation/hit-presentation-2026-10-16.md)。下面历史夹具说明不代表新工具不修改正式资产。

- `audit_presentation.py`：只读导出正式五段、通知、Profile、AnimBP 和轻反馈骨骼采样，结果在 `Saved/HitPresentation/Before`。复跑会覆盖该审计目录的同名报告，应先保留已有证据。
- `create_presentation_assets.py`：新建四方向短 Additive 原型，修改已备份的正式五段、Hero／假人 Profile 和 AnimBP。已有原型时停止，不覆盖用户修改；它是资产制作工具，不能作为只读检查运行。
- `capture_presentation.py`：通过正式 SubmitCombatIntent 连段链采集实际渲染帧和时间线，不代表物理输入。`CAPTURE_STAGE` 指定输出分组；`CAPTURE_FIXED_STEP=True` 使用测试专用 60Hz 游戏步长，在截图等待期间暂停测试世界并恢复设置。这是离线采样，不是实时 60fps 性能测量，也不是生产 HitStop。
- `validate_presentation.py`：`VALIDATION_MODE` 选择 Standalone／Listen／Dedicated，逐项运行方向、六类 Impact、倒地接触、高抗及跨端表现次数／实际动画检查。检查输出 JSON 的实际通过数量与 error，不把 Started 当作通过。
- `configure_hit_stun_exit.py`：现在只读输出控制时长、动画长度和 BlendOut，不裁剪、不保存正式动画。底层显式裁剪函数仍保留。
- `verify_presentation_assets.py`：只读核对正式伤害／执行契约、短动作基准和正式预留削韧配置。
- `validate_dense_contacts.py`：在已准备且基础用例完成的联机夹具中，用临时第二个真实 Notify 检查 0.12 秒间隔连续命中；不保存夹具资产，结束后须重载测试 Montage／Definition／AbilitySet。
- `render_comparison.py`：将两个完整 60Hz 截图组制作成 H.264 正常／慢速对照，不补帧、不添加伪造实况音轨。所需 imageio-ffmpeg 安装在忽略的 `Saved/HitPresentation/MediaDeps`；没有修改全局 Python 安装。

`CAPTURE_BASELINE_ASSETS=True` 只在内存恢复备份配置，要求开始时没有未保存内容包；结束只重载本次覆盖包，不将临时基线保存回正式资产。左右仍是相同最新运行时代码。收尾错误独立保留，不作为截图不完整的借口删除原证据。

备份保存在 `/Game/Tests/Combat/HitPresentationBackups/B20261016`，不是可直接删除的缓存。旧轻反馈素材保留，新增方向原型位于 `/Game/Main/Character/Hero/Anim/HitReactions/PresentationPrototype`；其制作质量仍需画面验收。

## 历史核心／运动夹具

需要交互式长时间观察当前实现时，使用[高血量假人手册](../../Docs/Guides/hit-reaction-dummy.md)。新地图和资源独立保存，正式攻击配置及现有验证夹具不被修改。

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
