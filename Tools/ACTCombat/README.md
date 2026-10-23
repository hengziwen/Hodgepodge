# ACT 切片工具边界

## 受击有限补转验证

`validate_hit_facing_runtime.py` 对应[受击补转设计](../../Docs/Design/hit-reaction-facing-design-2026-10-21.md)与[本轮验证记录](../../Docs/Validation/hit-reaction-facing-2026-10-21.md)。采集只能在带 `-HodgeCombatValidationExclusive` 的新建独占编辑器进程内执行，启动器必须传入该进程的精确 `HIT_FACING_EXCLUSIVE_EDITOR_PID`；不得向用户正在使用的编辑器注入脚本。

启动配置为 `HIT_FACING_MODE`（`single`／`listen`／`dedicated`）、`HIT_FACING_LAG_MS`（0／100）、`HIT_FACING_PLAYER_TARGET` 和新的 `HIT_FACING_REPORT_DIR`。`HIT_FACING_CASES` 可缩小诊断范围，完整验收使用默认十项。角色与 Profile 必须已按批准清单配置；脚本不会替测试自动启用正式配置。

真实攻击经过既有意图／能力队列、Montage Notify 和 GE。采集器在内存配置探针并于结束时恢复、重载，记录 Actor／Mesh、控制段、真实结算与所有移动／纠正／回放诊断；不得将请求 `Started`、轮询结果或编辑器退出成功代替命中和验收成功。全链场景始终使用同一目标，覆盖接触、轻重受击、首次击飞、真实落地、倒地起身及释放。原生 `HitFacingResolved` 诊断须通过启动参数 `-LogCmds="LogHodgeHitPresentation Verbose"` 打开，默认不影响日常日志。

正式场景前分别执行轻僵直、击退、击飞三次真实预热，等待各端自然释放；完整原始证据保存至 `warmup.json` 和 `runtime.warmup`，不计入正式场景通过。每端样本末尾记录完整引擎帧及身份的 `HitFacingSample`，离线按相邻标记的日志行序关联移动事件，并核对建立、释放和取消边界。新报告缺失标记即失败；旧报告只能按原采集范围读取，不补造因果证据。冷启动卡顿可能耗尽既定世界时间窗口，原失败须保留，不能以预热结果覆盖。

用户已批准仅在 Hero 的 `SameLevel`／`Upgrade` 两个一秒控制窗专项中，临时在内存关闭 `bEnableGroundRecovery`。每项结束及异常清理立即恢复、读回原值，结束 PIE 后重载明确改动的 Profile，不保存；`ground_recovery_scopes` 与逐帧配置读数供离线核对。预热、`FullChain` 和其他场景继续使用正式恢复配置，这两个专项不证明正式 0.4 秒恢复时序。

普通补转、一次 Final 校正和原生朝向纠正分别验收。只有实际接受新 ACK 且上下文仍匹配的原生 RMS／显式 Rotation 才产生 `NativeCorrection`，允许其瞬时朝向变化并完整留证。`NativeRejectedRestore` 仅抵消重复或拒绝回包造成的未授权朝向写入，前后净变化必须为零，不能作为新转向或 Final 消费的依据。

系统 Python 只负责断言回归和离线审计，不启动 Unreal：

```powershell
python -m unittest discover -s Tools/ACTCombat/tests -p test_hit_facing_assertions.py
python Tools/ACTCombat/validate_hit_facing_runtime.py --report <本批runtime.json> --native-log <本批完整UE日志> --output <不存在的audit.json>
```

离线输出拒绝覆盖既有证据。当前批次启动命令、逐项结果和历史失败均见验证记录；原生回归、Actor 数值及 Mesh 组件朝向不能替代动画画面、脚滑、操作手感或独立进程专服验收。

## 当前分段击飞路线

最新契约见[攻击分段决定击飞](../../Docs/Design/attack-segment-launch.md)。`configure_segment_launch.py` 备份并只迁移终结刀／R 的命中分段开关；`validate_poise_runtime.py` 现已改为验证轻重削韧、任意大削韧不击飞、分段续播 Fly、未开启开关的空中命中正常落地起身，以及续飞次数上限。当前输出为 `Saved/SegmentLaunch`，旧 `Saved/PoiseReaction` 报告属于上一版累计破韧路线，不等于本轮通过。

## 2026-10-18 削韧路线

当前普通攻击路线见[目标侧削韧设计](../../Docs/Design/poise-driven-hit-reactions.md)。`migrate_poise_reactions.py` 先备份当前正式配置，再创建原地 Behit 副本、精简 Profile、启用 PawnData 削韧并迁移五段；已有批次时停止，不能日常重跑覆盖手调。`validate_poise_assets.py` 核对互斥入口、原字段保留、伤害／执行／通知不变、源包哈希及 AnimBP 编译。`validate_poise_runtime.py` 复用真实装备探针、Notify 和 GE，覆盖轻重／破韧、连续阈值、不同 RMS 距离、取消和高抗目标；`POISE_MODE` 可选 single／listen／dedicated，`POISE_LAG_MS=100` 模拟延迟，输出 `Saved/PoiseReaction`。

以下 Behit 根运动套件属于上一批历史基线，其“地面必须播放动画根运动”和按攻击表现力度选 S/B 的断言已不适用于精简布局。不要通过重跑旧迁移恢复旧作者结构。当前 R／动作闭环套件仍可复用，并用独立输出目录保留本轮证据。

对应[设计](../../Docs/Design/act-combat-animation-feedback-design.md)与[验证记录](../../Docs/Validation/act-combat-animation-feedback-2026-10-17.md)。执行环境为 UE5.5.4 Editor Python，除编码器外不在系统 Python 中运行。生成报告和媒体写入忽略的 Saved/ACTCombat，不自动提交。

- `audit_assets.py`：只读正式五段、Hero／Guardian／假人、E／R、Dash、Combo 和 AnimBP，记录反射数据及源文件哈希。
- `create_reaction_slice.py`：历史程序动作制作记录，已停止使用；用户明确要求复用现有 Behit，不再运行此脚本生成骨骼关键帧。
- `configure_player_recovery.py`：备份后接入 Hero Profile 和正式五段的恢复标记，伤害／执行配置必须保持一致；已有备份时停止，避免覆盖用户手调。
- `calibrate_knockback_curve.py`：保存初版曲线副本后调整样例端点；不是每次构建后必须运行的自动配置。
- `validate_assets.py`：旧 ACTPrototype 阶段的历史检查，含旧 Reaction 全量相等及程序动作持续段前提；Behit 迁移后使用下方新资产检查，不用旧前提判定新配置失败。
- `prepare_player_fixture.py`／`finish_player_fixture.py`：建立两个真实玩家客户端及 PIE 专服，复用现有测试装备授予，结束后恢复偏好并重载明确的共享探针包；不保存运行覆盖。
- `validate_player_recovery.py`：夹具就绪后注入 Enhanced Input，检查过早输入、合法闪避／普攻、服务器准入拒绝和移动恢复。0ms 时要求自然释放前权威提交；真实 Montage 起播单独确认，不把两次网络往返当成零耗时。100ms 设置沿用现有 `Tools/HitReaction/set_network_lag.py`。不是物理键鼠测试。
- `validate_control_chain.py`：复用高抗出招、击退、浮空、倒地和取消套件；仅在内存关闭玩家主动恢复以检查 0.9 秒持续姿态，完成或退出 PIE 自动恢复。该隔离组不替代正式 0.4 秒玩家恢复测试。
- `validate_action_loop.py`：正式 Enhanced Input 五段、移动退出、真实闪避及闪避后普攻；不是物理键鼠或复杂多人混战测试。
- `check_reports.py`：聚合实际报告、接触次数、持续姿态、伤害和位移证据，不将启动成功代替断言通过。
- `capture_slice.py`：复用正式连段入口采集旧／新资产的 60Hz 离线帧，`ACT_CAPTURE_BASELINE` 控制内存配置，不回退代码；DLL 身份由验证记录关联，结束恢复，不保存旧配置。
- `encode_comparison.py`：将两组完整帧编码成正常／四分之一速度对照。媒体明确标注离线模拟、无实况音轨，不作为实时 60fps 性能证明；不下载游戏素材。

异步脚本返回 Started 仅表示已登记回调，需读取报告 error、用例数量、结束标记及 PIE 清理结果。夹具套件串行执行，禁止同时改同一探针 Definition／Montage；密集命中新增的临时 Notify 由收尾重载清除。

## 共鸣解放 Fly 与根运动复评

- `configure_resonance_launch.py`：备份正式 R Definition 后，只新增 Launch；通知、伤害与执行配置保持原样。已有备份或 Impact 时默认停止，不作为日常调参按钮。
- `validate_resonance_launch.py`：独立单人 PIE，以真实 Enhanced Input R 触发原能力消耗和正式判定框，检查实际扣血、Fly 三段 Montage、真实接地和控制清理；结束恢复 PIE 偏好，不保存测试状态。
- 结果输出到 `Saved/ACTCombat/ResonanceLaunch`。原型制作脚本保留为历史实现记录，后续不再通过程序旋转骨骼生成新动作；使用现有 Behit 素材。

上一批动画根运动复评见[设计](../../Docs/Design/hit-reaction-root-motion-evaluation-2026-10-17.md)：当时地面与真实落地接入了动画根运动。本轮普通削韧路线改用原地副本及 CMC／RMS，旧根运动断言仅用于历史回归，不能覆盖当前正式配置。

## 上一批 Behit 制作与回归

对应[实施记录](../../Docs/Validation/authored-behit-root-motion-2026-10-17.md)。不覆盖 `/Game/Wuwa/Anim/BaseAnim` 源包，不生成新的骨骼旋转曲线。

- `migrate_authored_behit.py`：一次性创建项目副本／Montage，先备份 Hero、假人和五段；发现已存在则停止，不是可重复的日常调参工具。
- `complete_authored_behit.py`：仅用于首轮迁移补齐原有 StandUp 和 Slam 的 Push 选片；已执行且有存在性保护，后续不重跑覆盖手调。
- `validate_authored_behit_assets.py`：正式两份 Profile、五段非 Reaction 字段、源包 SHA、完整片长、阶段根运动和主 AnimBP 编译；不保存资产。
- `validate_authored_behit.py`：真实装备探针和 Notify／GE 验证 S/B 左右、密集续播、独立重入、地面 Knockback、倒地起身、Fly→Push、Slam 和取消。`BEHIT_MODE` 为 single／listen／dedicated；`BEHIT_LAG_MS=100` 可增加网络延迟。每端独立采样世界身份、胶囊、Montage 和外部 RMS；完整恢复结束后比较位置，不能在原动作高速退步中途判定最终失步。输出在 `Saved/ACTCombat/Behit/motion-*.json`。
- `validate_resonance_launch.py` 可用 `R_REPORT_DIR='ACTCombat/Behit'` 保留本轮真实 R 输入证据，不覆盖上轮历史报告。

网络最终位置收敛不等于运动途中没有延迟／校正跳变，PIE 专服也不等于独立进程专服。原生与脚本通过不替代主观手感验收。共享探针的内存修改恢复后仍可能被编辑器标记为脏包，结束时仅重载本次明确修改的测试包，不保存到磁盘。

专服探针等待与本次服务器授予句柄精确一致的拥有者 Spec，不能只按相同 Definition 找尚未撤销的旧 Spec；`probe_queued` 记录服务器句柄，真实受击建立有 5 秒上限。
