# ACT 战斗与受击反馈实现验证

日期：2026-10-17；UE 5.5.4；分支 `dev-Combat`，起始 `891fd70`。对应[总设计](../Design/act-combat-animation-feedback-design.md)及[工作包](../Design/act-combat-animation-feedback-plan.md)。保留工作区原有修改，本轮不提交、不切分支。

**交付状态：技术切片已实现、构建和下述回归通过，整体战斗体验尚未最终验收。** 未受击客户端后续连段仍有独立失败，按用户此前决定暂缓；不能将本轮结论写成所有战斗功能均正常。

> 后续 Behit 替换与分阶段根运动已实施，见[当前实现验证](authored-behit-root-motion-2026-10-17.md)。下文八个程序动作、原地限制和旧测试数量保留为对应批次的历史证据，不代表当前正式 Profile 仍引用这些原型。

## 1. 原始需求与范围

继承封存分支已有 R1／R3、短方向 Additive、接触音画和正式五段配置；改善强受击延长后的姿态保持、强弱覆盖、地面受击恢复和击退速度节奏。五段仍为 01→02→03→05→04，不改伤害、判定等级、攻击通知时间、现有技能消耗或正式连段授权。

用户确认暂缓跑攻和切手。玩家地面 HitStun 原型采用 0.2 秒闪避、0.3 秒明确标记的普攻、0.4 秒移动恢复；参数在 Profile 可调。完整破韧、HitStop、额外镜头、公共动画默认引用迁移不属于本轮必做项。Client 共鸣回路前连段窗口故障继续独立保留。

## 2. 实现与接入

- **裁决与转换**：地面击退不被纯 HitStun 撤销；倒地／起身不被站立击退拉回。实际扣血和公共接触不随动画抑制而丢失；已接受的致死伤害仍产生接触，已死亡目标不重复产生反馈。同物理帧 Launch 后再命中保留 PendingLaunch 和空中链。
- **表现选择与持续**：原 Profile 条目增加方向、力度阈值变体和可选 Sustain／Recover Section；保留原默认 Montage 回退及运行时已解析的网络引用。重复同级或更轻 HitStun 继续原动画，但重置最新受击恢复时钟，不免费免控。
- **纯视觉尾部**：配置显式开启后，自然结束保留原地动画的具体实例；移动、新动作、死亡、解绑和期限到达精确停止旧实例。尾部不持有 GAS 控制、方向或检测租约，不停新动作 Montage。
- **玩家恢复**：复用 GAS CanActivate／Commit、连段授权、现有 Dash／Definition 方向链。只有服务器在费用和准入成功后解除当前 Sequence；客户端等待权威解锁，不自行删控制。独立旋转锁与移动禁令仍有效。异步等待有执行身份、Avatar 和超时约束，结束配对清理。
- **击退曲线**：沿用 CMC RootMotionSource_ConstantForce；派生 RMS 仅以当前模拟步的曲线积分替代点采样。目标 Profile 曲线积分归一化，峰值限制保留；原生序列化／Clone／状态匹配沿用父类。无曲线时沿用原行为。
- **独立素材**：`/Game/Tests/Combat/ACTPrototype` 内 8 个方向×中重强度 Sequence 和 8 个 FullBody Montage，0.8 秒，Impact／Sustain／Recover 分段，Sustain 边界闭合；曲线 `CF_ACT_Knockback`。基于现有 Hero 素材制作临时脊柱／头部失衡样例，不冒充成熟退步、摔倒或动作捕捉素材。原短轻 Additive 不重做。
- **正式接线**：Hero 与独立假人 Profile 使用新变体、持续段、自然尾部与曲线；Hero 开启恢复，假人不开启。五段仅新增明确允许恢复普攻的标记，原伤害／执行／Body 配置逐字段对照备份一致。Guardian Profile 保留原有素材。

所有改动前的 Profile／五段副本位于 ACTPrototype/Backups；源动画不覆盖。脚本集中在 `Tools/ACTCombat`，审计、制作、验证、网络夹具和离线采集职责分离，运行覆盖不保存共享探针资产。

## 3. 参考与差异

本机 Lyra 的 `LyraGameplayAbility::CanActivateAbility/EndAbility`、`LyraAbilitySystemComponent::ProcessAbilityInput`、`LyraPawnExtensionComponent` 与 `LyraAnimInstance` 是已有 GAS／输入／Owner-Avatar／TagPropertyMap 的参考链，继续复用 Hodge 已迁入实现。本机引擎 `AbilityTask_PlayMontageAndWait::StopPlayingMontage/OnDestroy` 用于核对动画归属清理，`FRootMotionSource_ConstantForce::PrepareRootMotion/NetSerialize` 用于核对移动与复制。Lyra 未在已核查链内提供完整的方向强受击与本项目恢复协议，未迁入另一套同职责框架。BMWukong 只读参考见[审查](../Design/bmwukong-combat-reference-review-2026-10-17.md)，不直接复制 5.7 资产。

## 4. 验证记录（持续更新）

### 已完成的阶段验证

- 常规 Editor／Game 构建：`Saved/ACT-Editor-Verified-20261017.log`、`Saved/ACT-Game-Verified-20261017.log`，退出均为 0。随后真实输入发现恢复朝向顺序问题，正在追加修复验证；这些旧构建不能替代修复后的结果。
- 全部 `Hodge` 原生自动化：`Saved/ACTCombat/Automation-Verified/index.json`，158 项，93 Success、65 SuccessWithWarnings、0 Fail、0 NotRun。警告保留，不称为无警告通过。
- 资产检查：`Saved/ACTCombat/asset-validation.json`，5 个攻击原伤害／执行配置一致，Hero／Guardian／假人 Profile 合法，8 个 Montage 持续段闭合；主 `ABP_Pover_Base` 编译 0 错误／0 警告。既有 Linked Layer 未因作者工具路径限制而强行修改或宣称编译通过。
- 实际 Melee 12 场景 PIE Dedicated：`Saved/ACTCombat/pie-Dedicated-integrated.json`，四向轻接触、HitStun、Knockback、Launch、Knockdown、Launch→AirHit／Slam、倒地接触、高抗均通过。150cm 请求实测最大 X 位移约 150.0054cm。PIE Dedicated 不等于独立专服可执行文件测试。
- Standalone／Listen 各 12 场景的早期曲线版本通过；归一化点采样仍出现 Dedicated 138.36cm／Standalone 147.75cm 的偏差，因此改成逐物理步积分。旧通过不作为最终积分 RMS 的距离证据。

### 测试发现与修正

1. 强受击持续段使用闭合 Section，防止控制延长时动作提前混出；恢复时钟独立于动画起始时钟。
2. 初版曲线点采样实测存在步长偏差，首轮最大位移约 161.3cm；端点校准仍受步长影响，改用积分 RMS，保留原始失败记录。
3. 真实 Enhanced Input 恢复测试的 EarlyDodge 通过、AllowedDodge 失败：旧流程先申请方向，而方向组件仍被 Controlled 锁定。修正为服务器提交并解除控制，拥有者等待复制解锁后再申请原方向链；不放开全局旋转锁。原失败保存为 `player-recovery-first-failed.json`。
4. 修复后普攻服务器在约 0.353 秒解除控制，拥有者约 0.375 秒播放，而服务器采样首次观察到 Montage 为 0.401 秒。旧断言误将“0.4 秒前提交恢复”写成“0.4 秒前服务器必已采到 Montage”，已分开检查提交与真实动作，仍要求实际攻击成功；原记录保存在 `player-recovery-handoff-sampling-failed.json`，不隐藏失败或用解除控制单独判定成功。
5. 100ms 首轮在服务器受击后 0.23 秒替客户端按闪避，但拥有者尚未收到自己的恢复窗口，因此合法用例失败。延迟用例改为等拥有者实际窗口后按键，0ms 门限断言不变；保留 `player-recovery-100ms-before-owner-window.json`。这是到达时间与测试前提问题，没有修改生产时间同步或扩张服务器免控窗口。
6. 密集接触旧测试要求专服播放轻 Montage，而生产代码明确跳过专服轻表现；两客户端实际实例均正确续播。修正测试按渲染端检查轻动画、按三端检查接触和强受击，并加强每个渲染端的播放位置断言。原错误保存在 `dense-dedicated-assertion-failed.json`，未为通过测试而启用专服特效。
7. 控制链旧测试同样要求专服触发轻表现委托，已按上述职责修正，原结果保存在 `control-dedicated-assertion-failed.json`。扩展空中组又发现测试夹具只在服务器设置 Pawn Overlap、客户端仍 Block，导致拥有者在角色碰撞后产生约 70cm 位置差；原结果保存在 `extended-fixture-collision-failed.json`。隔离测试改为三端使用相同临时 Pawn 碰撞响应，保持世界物理障碍有效；不修改正式角色碰撞资产，不能将隔离用例当成生产角色互相推挤测试。
8. 碰撞统一后 AirHit 两端位置一致，但旧扩展断言假定两刀共扣 10 血。正式玩家属性下实为每刀约 3.1，修正为两次独立结算 HitId 和 AcceptedDamage 总和匹配生命差，未改 GE 或属性；保留 `extended-fixed-damage-assumption-failed.json`。
9. Python `str(Guid)` 返回包装器地址，不能用于结算去重。套件改为 `Guid.export_text()` 比较真实值，原中间错误保存为 `extended-python-guid-repr-failed.json`；不将每帧新包装器计为新伤害。
10. 最终接口复查及定向 PIE 确认：恢复普攻播放期间拥有者仍为 `Combo.Entry`。等待解锁时没有标记 FacingDataWaiting，协调器提前收回授权；异步续接又错误重置“激活已返回”标记，无法完成准备阶段。修正为保留等待授权、只在 Activate 起点重置返回标记，并用原弱 Combat 引用检查 Avatar 身份。新增真实节点断言，原失败见 `recovery-combo-before-fix.json`；此前仅检查 Montage 的七项通过不能替代此项回归。
11. 修复后追加“第二刀也必须被服务器接受”的检查仍失败：拥有者推进到 02，服务器未推进。另用 `probe_client_combo.py` 完全不施加受击，只输入普通客户端攻击，也观察到拥有者 01／02／03、服务器仅 01；见 `plain-client-combo.json`。该现象与用户先前明确暂缓的 Client 连段窗口问题重合，本轮不修改连段协议；失败保存在 `recovery-followup-existing-combo-failed.json`，不算入通过的恢复七项。恢复测试另统一临时控制器朝向，避免上一用例镜头方向和直接复位 ActorYaw 冲突；完整恢复转向角度尚未覆盖。

### 最终检查与证据

修复后的常规 Editor 构建 `Saved/ACT-Editor-RecoveryHandoff-20261017.log` 退出 0（192.45 秒）；Game 构建 `Saved/ACT-Game-RecoveryHandoff-20261017.log` 退出 0（212.34 秒）。`Automation-RecoveryHandoff/index.json` 再跑 158 项：93 Success、65 SuccessWithWarnings、0 Fail、0 NotRun，命令退出 0；新增独立旋转／移动禁令断言通过。已有 `UButton::IsFocusable` 弃用与构建 IncludeOrder 提示未顺手修改。

玩家恢复 `player-recovery-0ms.json` 和 `player-recovery-100ms.json` 各 7 项通过，均 error=null：过早闪避／普攻、合法闪避／普攻、服务器单独拒绝二者、普通移动恢复。0ms 合法闪避约 0.281 秒提交释放，合法普攻约 0.355 秒提交释放；输入缓存可在窗口打开后合法提交。100ms 下合法动作最终完成，但自然释放已先到，不能宣称仍然实现同样的提前恢复响应；实际起播延迟保留在样本，后续手感调优需考虑这一差异。服务器拒绝未提前解除控制，观察端最终无控制残留。输入由 Enhanced Input 注入，未测试物理键鼠。

100ms 双玩家／PIE 专服密集接触 `Saved/ACTCombat/dense-100ms.json` 三项通过：LightSameStrength、LightEscalation、StrongContinue。两次接触间隔 0.12 秒；三端接触数均为 2，第二次独立扣血。两个渲染端分别验证同级沿用实例、更强替换实例、强受击更新 Sequence 且保留 StartedAt。专服不播放轻表现，不以 -1 实例比较冒充动画通过。

100ms 高抗／控制链 `Saved/ACTCombat/control-chain-100ms.json` 7 项通过：同级 Skill、低判定高抗、同级 Vajra、击退、挑飞、倒地、取消挑飞。低判定目标的既有能力保持活动，两客户端轻反馈与出招 Montage 同时播放；强受击结束后各端无残留控制。150cm 击退测得 150.034cm，结束位置误差最大约 0.00365cm。另从实际样本确认，受击后超过 0.8 秒原型长度时三端仍在 Sustain 内播放，控制结束才进入恢复；不是仅检查“曾经播放过动画”。

100ms 扩展组 `Saved/ACTCombat/control-extended-100ms.json` 4 项通过：AirHit、Slam、WallKnockback、CeilingLaunch。墙体实际限制击退到约 70cm，顶棚限制上升约 157.1cm；两次空中攻击的独立结算总和匹配生命差，拥有者与服务器最终位置一致。

修复后再次冷启动资产检查通过，原 5 个攻击字段不变、3 个 Profile 合法、8 个闭合持续段及主 AnimBP 0 错误／0 警告均已读回。正式输入动作闭环 `action-loop.json` 通过五段 01→02→03→05→04、移动退出、真实 Dash 提交、闪避后普攻四项检查。

最终异步连段授权修复后：`Saved/ACT-Editor-Final-20261017.log` 退出 0（27.41 秒），`Saved/ACT-Game-Final-20261017.log` 退出 0（63.65 秒）；`Saved/ACTCombat/Automation-Final/index.json` 158 项再次全部通过，93 Success、65 SuccessWithWarnings、0 Fail／NotRun，命令退出 0。恢复 0／100ms 各七项再次通过，合法恢复普攻播放期间，拥有者及服务器均为 `Combo.Light.01`，不再停在 Entry；后续普通客户端连段问题仍按上节保留。

基线离线采集首轮在 180 秒期限写入 355 帧后停止，`ACT-Before` 保留未完成记录；提高采集期限后 `ACT-Before-Complete` 获得完整 420 帧／7 秒模拟、error=null。基线 DLL 记录在 `baseline-runtime.json`，它位于最后的异步授权修复之前；新采集将使用 `final-runtime.json` 对应最终构建。片段未使用恢复按键，两组不能标成“旧封存代码 vs 新代码”或严格同一二进制的纯资产 A/B。

`ACT-After` 也完成 420 帧，error=null；两组均显示完整 01→02→03→05→04，附带逐帧实际伤害、接触、动画实例和位置记录。编码器退出 0，生成[正常速度对照](../../Saved/ACTCombat/act-comparison-normal.mp4)（60fps 容器、7 秒）和[四分之一速度](../../Saved/ACTCombat/act-comparison-quarter.mp4)（15fps、28 秒）。它们是离线渲染采样，没有实况音轨；只用于姿态和时序比较，不能代替实时帧率、听感或用户评分。

最后一次 `validate_assets.py`、`check_reports.py` 均成功；28 项受击场景和单人动作闭环证据齐全，158 项原生测试无失败。17 个相关 Python 文件 AST 检查通过，`git diff --check` 通过；25 个原型／备份包哈希均未被后续测试改写，新增 uasset 由 Git LFS 规则覆盖。生成日志与媒体均在 Saved，未提交。

## 5. 未验收与内容缺口

物理键鼠手感、正式主动 Guardian 高抗行为、实况音色与火花遮挡、退步／浮空／落地／起身的最终素材质量、真实持续 60fps 性能和七项主观评分不能由自动化代填。8 个中重动作是可替换原型，尚不等同参考工程观感。未执行 Cook、打包或独立进程专服。跑攻／切手、HitStop、破韧和共鸣回路前窗口异常保持上述范围。

后续按优先级保留：

1. **独立 P0，用户暂缓**：Client 后续连段服务器未推进。输出应是单独的请求／窗口复现及修复回归，不修改受击参数掩盖；完成标准是正式普通攻击与恢复后攻击均在服务器和拥有者正确推进，原失败报告保留。
2. **体验 P1**：正式 Guardian 主动高抗和真实键鼠、多方向恢复转向、复杂混战验证；当前高抗用例是正式 Hero 的测试能力，不冒充正式 Guardian AI。需要适配目标骨架的素材与实际运行证据。
3. **素材 P1**：替换或精修临时 Medium／Heavy、退步／落地／起身，补分档音色和最终火花；保留原地 RMS 所有权、Body／Judgement 和伤害，按七项主观评分验收。
4. **后续 P2**：斜坡、真实角色推挤、丢包／独立专服／Cook／性能及 Details 面板补验；可选默认动画引用迁移、HitStop、破韧另按设计授权。跑攻与切手本轮明确不制作。

## 6. 执行命令

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash -nosound '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/ACTCombat/Automation-Final' '-abslog=E:/Project/Git/Hodgepodge/Saved/ACTCombat/Automation-Final.log'
```

PIE 与资产通过已发现的原生 `system_control.execute_python` 执行项目相对路径脚本；启动成功与脚本断言结果分开核查。日志、报告、截图及视频保留在 Saved，不纳入源码提交。

## 7. 代码交付入口

- [Profile 与新配置](../../Source/Hodgepodge/Public/Data/HodgeHitReactionProfile.h)、[受击组件](../../Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp)：变体、转换、恢复资格与视觉尾部。
- [受击 GA](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.cpp)、[击退 RMS](../../Source/Hodgepodge/Private/Combat/HodgeHitReactionMotion.cpp)：姿态／运动最终执行。
- [Definition](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp)、[Dash](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.cpp)：现有动作链的受击恢复接入。
- [原生回归](../../Source/Hodgepodge/Private/Tests/HodgeHitReactionTests.cpp)、[工具与证据约束](../../Tools/ACTCombat/README.md)：制作、验证和数据边界。

## 8. 追加：共鸣解放 Fly、判定框命令与根运动复评

用户确认 Fly_Fall 从触地冲击开始。新增[根运动复评](../Design/hit-reaction-root-motion-evaluation-2026-10-17.md)：原地是当前实现约定，不是 UE 要求；地面/落地动画根运动值得接入，空中重力、实际落地和 RMS 互斥需明确。本批只完成评估，没有全量切换根运动，没有生成新动作。

正式 `DA_Resonance_Liberation` 只增加 Launch，180cm/s 水平、800cm/s 竖直，落地不追加 Knockdown。保留伤害/倍率/Vajra、执行配置和全部原通知；原资产副本在 `Tests/Combat/ResonanceLaunch/Backups`。配置报告 `Saved/ACTCombat/ResonanceLaunch/configured.json`，Burst Montage 的 SHA256 未变。Hero/假人复用已有 Fly 三段，Guardian 未新增不兼容骨架的 Launch。

命中几何新增 `Hodge.Combat.DebugDraw` 总开关和 `Hodge.Combat.DebugDraw.ConfiguredShape`，后者只控制配置判定体，不依赖默认武器来源 Tag。已有线条按生成时的 Duration 消失，不清除其他系统的线条，不复制调试几何。新增原生 `Hodge.Combat.ConfiguredShapeDebug` 使用真实物理查询及线批次，证明关闭/开启分类及关闭总开关只改变绘制，目标命中不变；武器分类关闭时配置 Box 仍可独立显示。

实际执行：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash -nosound '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/ACTCombat/ResonanceLaunch/Automation' '-abslog=E:/Project/Git/Hodgepodge/Saved/ACTCombat/ResonanceLaunch/Automation.log'
```

Editor 构建退出 0，47.21 秒；Game 退出 0，70.97 秒。日志为 `Saved/ACT-Editor-ResonanceLaunch-20261017.log`、`Saved/ACT-Game-ResonanceLaunch-20261017.log`。159 项原生测试：94 Success、65 SuccessWithWarnings、0 Fail/NotRun，命令退出 0；既有弃用/IncludeOrder 等提示未顺手修改。

冷启动配置首次失败：Python 反射结构是引用包装，检查副本时意外清空了内存中的新 Impacts，保存前断言拦截，正式 R 包未保存。改为构造独立结构后从文本复制；仅在正式配置仍与本轮备份一致时显式恢复制作，退出 0。失败日志保存在 `configure-first-failed.log`，未删除备份或静默重写已配置资产。

冷启动编辑器执行 `Tools/ACTCombat/validate_resonance_launch.py`，单人 PIE 注入正式 Enhanced Input R。最终 `r-input-smoke.json` error=null，9 项检查通过：正式授予/容量、正常消耗、一次 Launch 伤害、真实扣血 62、Fly_Start/Loop 进入、三段实际 Montage 权重均达到 1、胶囊最高离初始位置约 323.68cm、真实接地后 Fly_Fall、最终无控制残留。首轮八项报告保留为 `r-input-smoke-first.json`；补三段权重断言后重新运行并确认 PIE 清理结束。

Python AST 与 `git diff --check` 通过。未执行本批联机、Cook/打包、物理键鼠、音色或最终观感验收；不以单人 R 冒烟宣称动画根运动已经接入。用户已暂缓的 Client 连段问题不在本批修改范围。既有未提交修改保留，不提交、不切分支。
