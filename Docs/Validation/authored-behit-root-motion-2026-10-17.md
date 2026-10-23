# 现有 Behit 与分阶段根运动实施记录

日期：2026-10-17；UE 5.5.4；工作分支 `dev-Combat`。本批在原工作区继续开发，未提交、切分支或覆盖用户已有修改。对应[总设计第 15 节](../Design/act-combat-animation-feedback-design.md)、[当前配置手册第 12 节](../Guides/hit-reaction-configuration.md)。

## 1. 任务与当前实现

用户要求停止程序生成受击，复用现有 S/B、Fly/Push，按实际阶段选择动画根运动或 CMC 弹道，并保留旧原型备份。实现覆盖以下六处：

1. **退出程序原型正式引用**：Hero／假人 Profile 的强动作、变体和轻反馈均改用现有 Behit 的项目副本。旧八个 FullBody 原型与旧四向局部原型仍保留，未删除或继续生成骨骼关键帧。
2. **完整 S/B 地面动作**：小幅 S_L/R、大幅 B_L/R，按力度及左右选片。Sequence、Montage 保留完整片长与原曲线，地面动画根运动负责胶囊移动，不同时提交外部 RMS。Knockback 使用 B，不再拿空中 Push 凑地面效果。
3. **Fly 与 Push 三阶段**：Launch→Fly_Start，AirHit→Push_Start；观察真实下落后进入对应 Loop，真实落地后进入 Fall。空中使用 CMC 重力与速度，Fall 恢复动画根运动。Slam 暂借 Push；GetUp 接现有 StandUp。
4. **连续命中策略**：密集同级接触保留起播时间，独立新受力允许重入，重反馈不被较轻接触压掉。左右由服务器解析并复制，前后缺少专属素材时使用明确回退，不称四向素材齐全。
5. **控制与根运动尾部**：自然解控后可完成原片段剩余位移；尾部按具体 Montage 实例管理，保持服务器姿势更新租约。新动作、输入、死亡、取消、解绑、离地均结束旧运动。控制长于原片段时只保持末姿态，不重复根位移。
6. **空中限流与兼容**：普通空中 HitStun 转为 Push 表现但不新增 Launch，不消耗续空次数；显式 AirHit 才占 MaxAirHits。旧目标默认 Impact 路线，Guardian 未强行迁移不兼容骨架。

## 2. 接口、来源与资产

- `EHodgeReactionMotionSource`：Impact／AnimationRootMotion。Profile 每条的 MotionSource、LandingMotionSource、GetUpMotionSource 明确阶段所有者；Runtime State 保留已解析结果并通过原 State 复制。
- `EHodgeHitSide`：Automatic／Left／Right，放在攻击 Reaction 表现请求；不改变 Body 或攻击等级。
- Profile `bUseLateralVariants`、`GroundReplayInterval`、`AirLoopTransitionTime`、`bAllowRootMotionRecoveryTail`，含默认兼容和有限值校验。
- `HodgeGameplayAbility_HitReaction::Synchronize/EndAbility` 仍负责 GAS Montage 与当前运动，组件仍负责权威计划、实际落地和生命周期，不新增并行状态机。
- `HodgeAnimationAuthoringLibrary::InspectBehitAnimation` 提取完整 60Hz 根轨迹与骨架差异；`ConfigureBehitMontage` 仅允许指定项目目录，不改 Sequence 关键帧。骨架适配调用既有 ReplaceSkeleton 路线。

本机 Lyra 对照：`Source/LyraGame/Animation/LyraAnimInstance.cpp`、`Character/LyraCharacterMovementComponent.cpp`，保留 GAS／AnimInstance／CMC 的职责；Lyra 未提供可直接复制的 Behit 多阶段受击系统。UE5.5.4 `CharacterMovementComponent::ApplyRootMotionToVelocity/ConstrainAnimRootMotionVelocity` 核实动画根运动优先级和 Falling 重力约束；GAS PlayMontage／复制继续沿用现有 ASC，不自行同步每帧胶囊位置。

正式新增目录：`/Game/Main/Character/Hero/Anim/HitReactions/Behit`，共 26 个 Sequence／Montage。11 个现有完整动作各一对，另有左右局部反馈各一对。来源均为 `/Game/Wuwa/Anim/BaseAnim`；11 个源包 SHA 校验未变，骨架无缺骨或父层级差异，参考姿态误差接近浮点精度。

Hero、假人和五段迁移前完整资产备份共 7 个，位于 `/Game/Tests/Combat/BehitMigration/Backups`。共鸣 R 的旧备份保留在 `/Game/Tests/Combat/ResonanceLaunch/Backups`。不重跑一次性工具覆盖手调。

正式五段顺序 01→02→03→05→04，PresentationSide 为 L/R/L/R/L；力度维持 0.65/0.8/1.15/1.0/1.6。01／02 新增 0.16／0.18 秒 HitStun，其余沿用原 Impact。五个攻击的伤害、ExecutionConfig、ExecutionBodyTag 校验未变。R 保留已加入的 Launch 180/800cm/s 与原伤害／消耗／通知。

## 3. 构建、正确性与测试证据

本批生成证据均在忽略的 `Saved/ACTCombat/Behit`，不纳入源代码。以下命令已执行：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash -nosound '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/ACTCombat/Behit/Automation-Final' '-abslog=E:/Project/Git/Hodgepodge/Saved/ACTCombat/Behit/Automation-Final.log'
```

Editor 最终构建退出 0，83.68 秒；Game 退出 0，115.79 秒。日志为 `Saved/ACT-Editor-Behit-Final-20261017.log`、`Saved/ACT-Game-Behit-Final-20261017.log`。既有 IncludeOrder／弃用提示不在本批清理。

末次注释同步后的 Editor／Game 常规构建也退出 0，分别 26.50／38.04 秒，日志 `Saved/ACT-Editor-Behit-Delivery-20261017.log`、`Saved/ACT-Game-Behit-Delivery-20261017.log`。此轮只更新中文注释，生产执行逻辑未再变化。

原生 Hodge 全套 161 项：96 Success、65 SuccessWithWarnings、0 Fail/NotRun，命令退出 0。新增阶段根运动／左右选择契约、空中接触不占续空次数；保留原取消、死亡、受击转换、R 判定调试等回归。警告不改称纯 Success。

冷启动 `validate_authored_behit_assets.py` 通过：两份 Profile 合法、五段非 Reaction 字段未变、11 个源包未改、片长与阶段根运动标记一致、主 ABP_Pover_Base 编译 ERRORS=0。AnimBP 使用 RootMotionFromMontagesOnly；未声称全部 Linked Layer 重编通过。

最终逻辑的单人 11 案例／48 检查、Listen 11 案例／85 检查、PIE Dedicated 双客户端 11 案例／85 检查、100ms Dedicated 双客户端 11 案例／85 检查通过；各报告 error=null，均已确认 PIE 清理。Listen／Dedicated／100ms 的各案例最终位置误差最大均小于 0.007cm，25cm 门槛未放宽。100ms 通过日志确认三个世界的 `PktLag set to 100`，结束恢复 0；不是仅改报告标签。对应 `motion-single.json`、`motion-listen.json`、`motion-dedicated.json`、`motion-dedicated-100ms.json`。

11 案例包含 S_L、S_R、B_L、B_R、0.12 秒内密集续播、独立再受力、地面 B 击退、倒地 StandUp、Fly→Push 三段、Slam 落地起身、根运动中取消。核查真实伤害、选片、根运动实际移动、外部 RMS 互斥、真实落地、控制／运动清理和多端最终收敛；不将脚本内存配置的探针称为正式五段输入。正式 R 与玩家输入结果另录。

`validate_resonance_launch.py` 本批真实 Enhanced Input R 的 9 项检查通过，`r-input-smoke.json` error=null、root_motion_migration=true：正常消耗、一次实际伤害 62、Fly 三段实际权重均为 1、离地约 323.68cm、真实接地后 Fall、最终无控制残留。未使用直接调用 BeginHit 替代 R 能力。

`validate_action_loop.py` 的正式输入闭环通过，`action-loop.json` error=null，4 项：真实 01→02→03→05→04、移动退出攻击、方向闪避提交、闪避后回到普攻。使用 Enhanced Input 注入，不称物理键鼠测试或复杂客户端连段故障已修复。该报告独立归档，不覆盖原 ACT 结果。

`prepare_player_fixture.py`→`validate_player_recovery.py`→`finish_player_fixture.py` 的 PIE 专服双客户端玩家恢复 7 案例通过，`player-recovery-0ms.json` error=null：过早闪避、合法闪避、过早普攻、合法普攻、服务器拒绝闪避／普攻、普通移动恢复。合法普攻同时检查拥有者／服务器连段节点；观察端无控制残留。普攻有输入缓存，过早输入可能在合法窗口稍后执行，测试要求禁止期不提前执行，并非保证丢弃所有早按。当前资产根运动下重复验证的玩家输入为 0ms；100ms 本批覆盖的是上方假人动作网络组，不混称 100ms 玩家恢复全套已重跑。

所有异步脚本均读取最终报告并确认清理，不保存测试覆盖。Python AST 21 个文件与 `git diff --check` 通过，相关文档本地链接检查通过；二进制新增资产继续由既有 Git LFS 规则管理。测试、构建日志和运行报告仍在忽略的 Saved 内，未提交。

## 4. 失败记录与解释

首轮密集测试错误地通过两次独立探针能力触发，受 0.3 秒起手影响，两次接触相隔约 0.35 秒；不满足 0.16 秒续播前提。改为同一真实 Montage 的两个 Notify，相隔 0.12 秒；生产阈值未放宽。失败报告保留为 `motion-single-first-gap-failed.json`。

首次网络采样误以 World.get_name 区分 PIE 世界，两个名称相同，已改为完整路径；原报告保留 `motion-listen-world-name-failed.json`。随后在大幅受击 1.5 秒中途记录到约 93cm 瞬时胶囊差异，当时原片段仍在高速退步。将观察延长到完整动作结束，保留 25cm 最终门槛并增加根运动无残留检查后，旧 DLL 的 9 案例 69 检查通过。该结果证明最终收敛，不证明高速运动途中没有延迟／校正。最终 DLL 增加倒地起身和 Slam 后另行回归。

测试只在内存改探针配置、追加密集 Notify，完成恢复字段并重载指定测试包，不保存测试覆盖。正常关闭本任务启动的编辑器前检查脏包，仅清理本任务明确修改的测试包，没有强制结束用户编辑器。

100ms 首轮停在第二次重授予：日志明确 `InternalServerTryActivateAbility: ... invalid SpecHandle`。工具 `Tools/HitReactionDummy/control.py::probe` 原先只按 Definition 选拥有者能力，可能选到尚未复制撤销的旧句柄。现改为等待本次服务器新句柄精确匹配后激活，保留 10 秒上限，不修改生产授予或 GAS 拒绝逻辑。首轮中止证据为 `motion-dedicated-100ms-stale-probe-handle.json`，不把此轮算通过。补测前同步了一条恢复尾部中文注释并再次常规构建；生产执行逻辑与已通过 161 项测试的版本相同。

## 5. 当前限制及后续手调

- 完整 S/B 约 3.37～4.33 秒，Fall 原轨迹约 3.5～3.94 米水平位移；完整保留供观察，不擅自改变素材曲线。这些尺度可能不适合最终战斗距离，技术正确不代表观感合格。
- B 用于地面 Knockback 是现有素材样例；Slam 暂借 Push，地面 Knockdown 暂借 Fall。没有新制作专门的踉跄、倒下起始或下砸动作，不能宣称素材缺口已全部解决。
- 局部反馈为原 S 左右片段的 Additive 转换，不是程序旋转骨骼，也不是已精修的四方向局部受力。方向／切换混出／0.16 秒重入间隔仍需用户调试评分。
- 没有修改伤害公式、暴击、属性、技能消耗、跑攻、切手、HitStop、破韧或新的镜头框架。既有 Client 共鸣回路前连段故障依用户要求保持暂缓。
- 未执行 Cook、打包、独立进程专服、斜坡／边缘／多人推挤、丢包、真实键鼠手感或持续帧率测量。未把用户七项主观评分填成通过，未声称达到悟空参考项目观感。

当前可调字段、真实资产位置及含义见配置手册第 12 节；保持控制时长、原片段内容、混出和运动来源分别维护。
