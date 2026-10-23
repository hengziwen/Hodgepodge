# 受击表现切片实施与验证（2026-10-16）

对应 [设计 §17](../Design/hit-reaction-system.md#17-2026-10-16-受击表现实施计划修订)。本次已落地 P0 的代码、四方向动作原型、正式五段配置与可回放画面，以及 P1 的字段／校验／工具保护。**工程验证通过不等于主观手感验收通过；P2 公共动画迁移和 P3 完整运动打磨仍未完成。**

最后密集复测曾发现 **0.12 秒同级轻反馈在混出开始时重启实例**。已修正组件，使用包含混出实例的 `GetInstanceForMontage` 及实际权重判断；用户保存关闭占用文件的编辑器后，已完成常规 Editor／Game 构建、冷启动和完整 151 项回归。拥有者／模拟代理两方向各三项密集用例均通过，实际实例编号证明同级不重启、加强反馈可替换，连续强受击保留起播时间。失败证据仍保留，不把先前失败删除后称一次通过。

## 1. 按优先级追踪未完成工作

1. **P0：五段演出及画面验收，已交付可评审原型，尚未整体验收。** 输出为正式资产、四方向短动作、接触事件和正常／慢速对照视频。代码、正式资产核对及关键 PIE 场景已执行；剩余是物理输入下的完整操作体验、四方向／密集命中／高抗出招的可视化验收、实况收音，以及用户七项手感评分。约束：不提高伤害／攻击判定去强行制造演出，不将离线采样或测试入口等同于实际 Client 输入。完成标准：关键场景无重复结算、无明显抽搐，七项主观评分各至少 4/5；当前不代用户评分。

   混出重启修复的工程验证已完成：两次相隔 0.12 秒的同级接触保留同一 Montage 实例，更强接触切换实例，新强受击更新 Sequence 但保持 StartedAt。各端分别按事件到达时刻观察，两次独立扣血／接触计数成立。`dense-contacts-preblendfix.json` 保留修复前实例 504→506 的失败证据；最终结果为 `dense-owner.json`、`dense-simulated.json`。
2. **P1：配置与工具，逻辑实现已完成，Details 视觉检查待验。** 输出为按 Impact 隐藏字段、按类型校验／使用、计时说明及不自动裁动画的工具。已通过 UHT、Editor／Game 构建和相关原生测试；尚未在 UE 5.5.4 原生 Details 数组面板逐项目视验收。约束：保留六种 Impact、合法空 Impact 与未配置的区别、ReservedPoiseDamage 序列化及 IsConfigured 契约。完成标准：隐藏字段不造成无关校验错误，类型切换后的可见字段与真实计时一致。
3. **P2：公共 AirLoop／Landing／GetUp 默认值迁移，未实施，受 P0 验收门槛约束。** 输出应为 Hero／Guardian 各特殊路径解析对照、迁移工具及兼容记录。剩余为逐路径盘点、引入默认值并保留条目覆盖、资产迁移和多人行为对比。约束：保留旧 UPROPERTY、备份和 FHodgeHitReactionState 已解析动画及复制结构。完成标准：普通落地、落地倒地、空中 Knockdown、Slam、重新受击和起身的表现／控制／运动不变。不能在 P0 未验收时以精简字段替代演出工作。
4. **P3：完整运动演出，未实施。** 输出应为击退速度／位移曲线和退步动画、Launch → AirHit → Landing／GetUp 连续动作。剩余为素材补齐、曲线积分归一化、碰撞／网络／脚步配合、正常与慢速画面对比。约束：保留 CharacterMovement／RootMotionSource 权威移动，不靠放大距离或延长控制掩盖素材问题。完成标准：位移与姿态同步，服务端／拥有者／模拟代理无明显分歧。

可选 HitStop／镜头冲击须在无顿帧版本验收后另作实验。共振度／破韧另立设计；用户暂缓的 Client 共鸣回路前普攻连段窗口问题继续独立保留，本次没有修复。上述顺序是当前设计的依赖关系，不将全部未完成项笼统写成“已完成”。

## 2. 当前实现与数据边界

### 接触结算与表现

- 继续使用 Melee → BeginHit → GE／HealthSet → FinishHit → 目标受击 GA；没有新建第二条伤害或控制链。
- 新增 FHodgeHitPresentation，在同一结算 Id 上携带执行身份、FName AttackPhase、方向、力度、位置／法线及可靠性标记。目标局部来源方向在命中时取得，避免 RPC 到达后目标转身改变选片。
- 只对真实、非初始穿透的 blocking Sweep 使用精确接触；Overlap／ConfirmedTarget 明确退回目标附近，缺物理材质时不声称知道表面类型。表面标志已传递，尚未制作材质分类音色表。
- “精确”指本次碰撞查询返回的几何接触点；碰到 Capsule 并不等于已定位肩部骨骼或获得蒙皮表面材质，不能用此标志夸大精度。
- 服务器确认合法接触后发送 Unreliable 表现事件；强受击成功、低判定及合法无 Impact 均可反馈。客户端使用有界 64 项 HitId 记录去重；独立接触不会按 AttackPhase 合并。没有拥有者预测火花。
- Niagara／声音与主 Montage 分开执行。同级轻反馈仍播放时不重启，更强反馈可覆盖；强受击和倒地／起身有优先级约束。原轻反馈 Cue 路径保留，当前正式 Profile 未配置该旧 Cue，已检查现有 GE／通知／武器的接触来源。
- 重复地面 HitStun 在原 Montage 尚有有效内容且无运动变更时保留 StartedAt；新 Sequence、EndsAt 和资源更新仍执行。受击 GA 据此续播，而不是忽略整个新控制计划。真实状态替换仍可切换动画。
- 倒地／起身中的纯 HitStun 不突然跳回站立；保留接触、伤害与原倒地计时。解绑只混出本组件拥有的局部 Montage，并清理表现身份和引用。

主要代码：[类型](../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)、[Profile](../../Source/Hodgepodge/Public/Data/HodgeHitReactionProfile.h)、[组件](../../Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp)、[受击 GA](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.cpp)、[Melee](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp)。R1 检测调度和 R3 资源收益未因本切片重写。

### 动作与音画原型

- 新资源目录：`/Game/Main/Character/Hero/Anim/HitReactions/PresentationPrototype`。四个 `A_Hit_Front/Back/Left/Right` 与四个对应 `AM_Hit_*`，每个 0.2 秒、60Hz 原型动作。
- 沿用当前目标骨架、Local Space Additive、自身第 0 帧基准，无根运动。骨盆保持，胸／脊柱与头部有错开的受力峰值并恢复基准。没有用四个全身普通 Montage 冒充 Additive。
- 原约 3.367 秒 LightFeedback 源序列与 Montage **未裁剪／覆盖**，仍作旧资产回退。源骨骼审计发现其动作贯穿较长区间，并非可以直接删除的纯静止尾段。
- 修正 ABP_Pover_Base Additive 分骨混合节点的未连接暴露权重引脚默认值为 0.35，使其与节点存储值一致；原编译输入为 1。没有重建整张 AnimBP。
- 接触原型复用 `/Game/Assets/Niagara/Impact/NS_BlockImpact` 与 `/Game/Assets/Sounds/Weapons/Axe/SW_HitBody1`。资源本体未修改；力度调节缩放／音量，尚不是三套独立音色及完整表面方案。
- 画面检查显示火花偏大、带明显火焰风格，存在遮挡动作的风险。方向动作是关键帧原型，缺少正式 Medium／Heavy、倒地局部反馈、配套退步恢复等精修素材，不能视作最终美术交付。

### 正式五段的实际改动

播放顺序保持 **01 → 02 → 03 → 05 → 04**。原始最新资产仅 01 配有 HitStun；本轮正式配置变为：

- 01：Normal 判定、合法空 Impact，局部接触，力度 0.65；**移除了原先的 HitStun 请求**。
- 02：Normal 判定、合法空 Impact，局部接触，力度 0.8。
- 03：Normal 判定、HitStun，沿用目标默认控制时长，力度 1.15。
- 05：Normal 判定、HitStun，沿用目标默认控制时长，力度 1。
- 04：Normal 判定、Knockback，使用现有 Impact 默认 120cm／0.2s，力度 1.6。

这属于经过授权的 Reaction 配置变更，**不是玩法行为完全不变**。五个 Definition 的伤害 GE／倍率／类型／元素、执行配置、Montage、Body、闪避和友伤规则均已逐项对照备份；没有改攻击窗口或提高判定。04 的位移仍通过 Body／目标支持规则裁决；高抗目标不会为了演出被强行推走。

01 和 02 不保证必然选两个不同方向：选片依据真实接触来源。实测首轮两刀均选 Front，不能声称已完成“第二刀必然反向”的演出目标。

### 配置与作者工具

- DefaultDuration／ControlDuration 仅地面 HitStun、Knockback 使用；MoveDuration／距离仅 Knockback；水平／竖直速度仅相应空中类型。编辑条件、Validate 与 BuildPlan 一起调整，隐藏的无关旧值不再阻断其他类型。
- PresentationStrength 有效范围 0.1～2，仅表现，不作为伤害或打断等级。
- ReservedPoiseDamage 保留序列化、Blueprint 与 IsConfigured／结果契约，从常规编辑面板移除；审计 16 个正式 Definition 及命中通知覆盖，未发现非零配置。没有实现扣韧或破韧。
- `configure_hit_stun_exit.py` 默认现在只读报告控制时长、Montage 长度和 BlendOut；不裁动画、不保存。底层显式裁剪函数保留。
- 公共动画字段未合并；运行时 FHodgeHitReactionState 的字段及复制结构保留。

## 3. 资产保护与收尾

正式写入前的备份目录：`/Game/Tests/Combat/HitPresentationBackups/B20261016`。包含五段 Definition、Hero／假人 Profile、AnimBP、测试地图；`PrototypeV1` 保留最初方向动作版本。修正 Rotator 时使用 UE Python 的命名参数，避免 roll／pitch／yaw 位置顺序误用；测试地图假人姿态也已纠正。

既有工作区包括 R9／R5／R1／R3 大量未提交代码和资产，全部保留，没有 reset、自动提交或顺手清理第三方。测试数据、构建日志、画面输出和本地视频依赖位于忽略的 Saved，不纳入源码。

最终核对五段／16 个正式 Definition、36 个 Python 文件语法、11 个本地文档链接、结果 JSON 与 `git diff --check` 均通过；PIE 世界已结束，无未保存内容／地图包，PIE 模式恢复 Client／2 玩家，随后关闭自动化测试编辑器以释放文件占用。

资产 A/B 只在内存恢复原配置，运行时代码始终相同。收尾第一次试图重存恢复后的资产，遇到另一个编辑器的文件锁；已停止回调，**仅重载本次七个临时覆盖包**，随后五段／16 个 Definition 核对通过、Dirty Package 清零。原失败保留在 BeforeReplay60/capture.json，标记 cleanup_recovered，未删除或伪称从未失败。工具随后改为不保存基线覆盖包，并在收尾异常时停止，避免访问已销毁 PIE World／重复重试。

## 4. 实际测试与证据

### 常规构建与原生回归

已按项目流程关闭本轮编辑器后常规构建，再冷启动加载新反射布局；不是用 Live Coding 代替。

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash -NoSound '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/HitPresentation/ReleaseTests' '-abslog=E:/Project/Git/Hodgepodge/Saved/HitPresentation/release-tests.log'
```

Editor／Game 均退出 0；混出修复后的最终日志 `editor-build-blendfix.log`、`game-build-blendfix.log`（24.00s／91.62s）。完整 Hodge 回归再次执行，报告目录 `BlendFixTests`、日志 `blendfix-tests.log`，**151 项：86 Success、65 SuccessWithWarnings、0 Failed、0 NotRun**，不将带警告测试描述为无警告。HitReaction 组 10 项，覆盖相关参数校验、方向、去重、倒地不跳站立、结算、取消等；低判定保持正在运行的 GAS 动作另有断言。整体回归同时覆盖之前的重构成果。

上述命令中的 ReleaseTests／release-tests.log 为前一轮报告名称；最终复跑相同命令，仅输出改为 BlendFixTests／blendfix-tests.log。最终源码及摘要见[机器可读结果](hit-presentation-2026-10-16-results.json)。

一次 GUI 与测试进程同时启动时在 usd_ar.dll 发生启动访问异常；保留 `Hodgepodge-backup-2026.10.16-04.18.27.log`。顺序启动恢复，不能据此断言 USD 根因已修复或所有启动路径稳定；未禁用插件规避。

### 真实 PIE 与网络

`validate_presentation.py` 已执行 Standalone／Listen／PIE Dedicated，**各 12 项通过**：四方向、HitStun、Knockback、Launch、Knockdown、Launch→AirHit、Launch→Slam、倒地再命中、高抗反馈。真实装备授予、正式 Melee、Notify、GE／扣血，非直接伪造结果。

混出修复、最终构建与冷启动后，以上三种模式各 12 项再次执行通过，当前 pie-Standalone／Listen／Dedicated.json 均为最后版本。

检查了最终恢复、真实方向 Montage、各世界表现次数一致、低判定不控制。Dedicated 为三个 PIE 世界；远端攻击等待能力复制，并由实际拥有者客户端发起。启动夹具先等各端绑定，空中追击先等实际离地，长起身用有界实际时限等待，未靠放宽玩法规则通过。

另用正式 Hero Profile 补测玩家目标两个方向：服务端攻击远端拥有者、客户端攻击服务端并观察模拟代理。各包含 EqualSkill 与 LowerAgainstArmor，检查真实动作中断／保留、局部动画重叠、HP 一致、最终位置误差 <5cm。使用组件实际计数断言每次接触各端为 1，不以 Python 委托数量代替去重。详细结果和密集命中结果见同目录 results.json 及 Saved 原始报告。

旧夹具原先额外授予一份受击 GA，触发运行时唯一性守卫 NotReady；已在“使用正式 Profile”的夹具分支中取消重复补授予，正式生产守卫未改。重跑前解绑旧监听器，避免夹具重复监听误报两次反馈。失败报告保留。

新增 `validate_dense_contacts.py` 在测试 Montage 中临时增加 0.42 秒 Notify，与原 0.3 秒形成 0.12 秒双接触。拥有者与模拟代理两方向各执行同级轻反馈、增强轻反馈、连续强硬直，共六项通过；每项两次实际 GE 扣血，各端接触增量均为 2。同级实例保持、增强实例切换、强硬直 Sequence 更新且 StartedAt 不变。普通 Montage 查询会忽略混出实例，测试使用现有原生 InspectMontageState 读取真实实例编号／权重，不用位置归零误报重播。

### 可回放画面

输出位于 `Saved/HitPresentation/`：

- `comparison-normal.mp4`：7 秒，60Hz 离线采样正常播放。
- `comparison-quarter.mp4`：28 秒，同样 420 个真实画面以 0.25 倍速回放。
- `comparison-contact.png`：同一仿真时刻的左右接触对照。
- `contact-sound.wav`：实际使用 SoundWave 的独立导出，**不是实况收音**。
- `AfterBlendFix60/` 和 `BeforeReplayBlendFix60/`：最终修复后各 420 个实际 960×540 截图与时间／身份／HP／阶段日志，仿真间隔 1/60s，无补帧。修订版采集墙钟约 59.48 秒，不能用输出 60fps 证明实时性能。额外保留原先 After60／BeforeReplay60 作为排查记录。

左右两侧使用**同一最新运行时**，左侧只恢复原资产配置，不能冒充旧代码版本全量前后对照。连段通过正式 SubmitCombatIntent 入口重复发出，不是物理按键；7 秒覆盖完整 01→02→03→05→04 后下一轮前两刀，共观察到七次修订版接触。视频没有录音；并未完成实况音画同步验收。

最终采集补充了 AttackRank／BodyRank、AttackPhase、力度、ExecutionId、ImpactPoint 和实际 Montage 实例／位置／权重；视频叠加显示其中主要诊断，完整身份和运动数据在逐帧 JSON。Montage 权重不等于最终分骨 BlendWeight，后者仍须 AnimBP 调试核实。

离线采集在截图等待期间暂停测试世界，逐步推进仿真，退出后恢复设置；它不是生产 HitStop，也不是实时性能测试。旧 Before／After 非均匀采样目录仅保留排查，不作为本轮 60Hz 对照素材。

## 5. 仍不能宣称通过的项目

- 原生 Details 数组条件显示的目视检查、物理输入与 Client 五段窗口、延迟／丢包／相关性重入、独立进程网络、独立 Server 可执行文件、Cook／打包。
- 正式 Guardian 正在出招的画面对比；当前主动动作保持证据来自正式玩家动画＋原生 GAS 断言和高抗假人。
- 最终音色／火花／方向性动作质量、每刀声音变化、实况音画同步、纯动作版本的主观对比和七项评分。
- 击退仍沿用 ConstantForce，完整浮空／落地／起身素材未重新制作；没有 HitStop、CameraShake 或共振度。

因此当前结论是：**已有可运行、可比较、可回退的第一批切片，关键工程场景已执行；整个受击手感重做尚未验收完成。** 下一批应围绕动作与接触资源的画面问题继续收敛，不抢先删除公共动画序列化字段。
