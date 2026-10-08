# 通知驱动战斗验证

当前配置入口是 [攻击能力配置手册](../../Docs/Guides/attack-ability-configuration.md)。旧 Timeline 生成与测试脚本位于 `Archive/Timeline/Cpp/Tools`，不再对正式资产运行。

测试蓝图 `/Game/CodexText/SkillHitVolumes/GA_VolumeExample` 仍保留，七个测试 Definition 已改为独立 Montage 原生通知，路径 `/Game/CodexText/AnimNotifyCombat/AM_*`；原 `TL_*` 数据资产已归档。`DA_VolumeSharedTrigger` 的名称保留兼容原测试入口，但运行参数已经转换为显式 `HitGroup + AttackPhase`，不再按触发时间去重。

编辑器测试必须通过 `HodgeCombatValidationLibrary.QueueAbilityAction` 的原生下一帧入口或真实 EnhancedInput 激活客户端能力，不能从 Editor Python 回调直接触发客户端 Server RPC。

运行日志与结果：`Saved/AnimNotifyMigration`。这些日志是本次验证证据，不纳入源码。测试 Editor helper 只存在于既有 Editor 模块，不进入 Game 目标。

客户端正式连段回归：先运行 `start_network.py` 或 `start_dedicated.py`，等待两个玩家初始化完成，再执行 `test_client_combo_windows.py`。它通过真实 EnhancedInput 提前 0.15 秒输入，连续两轮 01→02→03→05→04→01，同时核对拥有端与权威服务器的全部 11 次激活；结束后断言检测会话与骨骼租用归零。结果写入 `Saved/ClientComboFix/authoritative-combo.json`，运行下一项前须等待该文件生成并核对 error，避免重叠测试互相注入输入。

`test_client_move_cancel.py` 在第一段窗口前持续注入移动，检查两端在自然结束前确认取消；`test_client_expired_input.py` 在 0.1 秒输入下一次攻击，验证缓存先于 0.6 秒窗口过期后没有接段。输出分别为同目录 move-cancel.json、early-input.json。启动时 error 为 RUNNING，完成后应为 null；不要把旧收据或运行中状态视为通过。三项脚本依次执行，间隔须让上次动作和连段记忆结束。
