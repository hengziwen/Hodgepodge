# Main 主角受击动画与能力接入

日期：2026-10-08，UE 5.5.4。按本轮授权配置正式主角目标侧资产，保留现有攻击 Impact 和通知。开始时五份 DA_Attack 与五份攻击 Montage 已有用户修改；修改前备份 14 个相关文件到 `Saved/HeroHitReactionSetup/Before`，附 SHA-256 清单。没有回退或提交。

## 已配置的正式资产

- 主角网格 `/Game/Main/Character/Hero/Anim/Model/SKM_Pover_LyraLab` 当前使用 `/Game/Main/Character/Hero/Anim/ABP_Pover_Base`。
- `/Game/Main/Character/Hero/Anim/Model/SK_Pover_LyraLab`：FullBody 保持 DefaultGroup；AdditiveHitReact 放入独立 HodgeHitFeedback Group。
- `ABP_Pover_Base`：原 AdditiveHitReact 在 FullBody 之前，本轮移到 FullBody、Inertialization、RotateRootBone 之后。主姿势缓存为 HodgeHitReactionBase；Identity Pose → AdditiveHitReact → Apply Additive，再与原主姿势做 Layered Blend Per Bone，最后接回原 FullBody_SkeletalControls。骨骼分支 Bip001Spine，初始权重 0.35，Mesh Space Rotation Blend=true，Curve Blend=UseBasePose。原 FullBody 及移动／骨骼控制路径保留。
- `/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction`：六类目标动作和空中／落地／起身衔接，强动画使用 FullBody，无动画回退关闭。DownedDuration=1、MaxControlDuration=6、MaxAirborneDuration=4、MaxLaunchHeight=600、MaxAirHits=6、自动起身开启。
- 同目录 `A_Hero_*`／`AM_Hero_*`：现有 Wuwa 受击素材的项目自有副本，动画骨架替换为 SK_Pover_LyraLab，关闭根运动。LightFeedback 为 Local Space Additive，以当前动画第 0 帧为基准，蒙太奇使用 AdditiveHitReact。强动作使用 FullBody，默认控制时长 0.4 秒。后续可在 Profile 替换动作并调整时长。
- `/Game/Main/Character/Hero/Ability/GA_Hero_HitReaction`：HodgeGameplayAbility_HitReaction 蓝图子类，继承 ServerInitiated／ServerOnly 和受击期间允许激活设置。
- `/Game/Main/Data/AbilitySet/DA_Pover`：普通 GrantedGameplayAbilities 增加上述 GA，等级 1，InputTag 空；Jump／Death 原授予保留，不重复授予受击能力。
- `/Game/Main/Data/PawnData/DA_Dafult_PawnData`：HitReactionProfile 指向正式主角 Profile。
- `/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1～5`：ExecutionBodyTag 均为 State.Combat.Body.Normal。保存前后逐份断言 DefaultHitConfig 文本相同，未修改攻击判定、Impacts、倍率或 DamageType。

本轮未改五份正式攻击 Montage；相对修改前备份的 SHA-256 全部相同。当前第一段已有 Normal 判定和默认 HitStun；2～5 的 Reaction 仍为空，按用户要求保留。空 Reaction 不触发强受击。

## 原生轻反馈播放修复

`MulticastLightFeedback_Implementation` 原先调用 `Montage_Play(Montage)`。UE 5.5 默认 `bStopAllMontages=true`，仅分开 Slot Group 仍会停止主动作。本轮显式传入 false，轻反馈只替换自身 Group；已有同 Group 拦截继续生效。

编辑器辅助接线、Skeleton Group 和动画副本骨架替换放在已有 HodgeAnimationAuthoringLibrary 主 `.h/.cpp`，仅 Editor 模块，不增加模块或修改依赖。常规编译前确认无未保存内容并正常关闭编辑器；没有使用 Live Coding。

## 实际验证

- Editor／Game Win64 Development 常规构建退出 0，日志 `Saved/HeroHitReactionSetup/EditorBuild.log`、`GameBuild.log`。
- 主 AnimBP 原生编译诊断 ERRORS=0、WARNINGS=0。蓝图受击子类编译保存、默认值读取及一次授予核对通过；重新打开后确认 Profile 引用、五份 Body 和 0.35 混合权重。
- 18 项 HitReaction／Combat／Combo／Rotation 原生回归全部成功，日志 `Editor3.log`，报告目录 `NativeAutomation`。
- 实际 `Play As Client`、玩家数 2、Run Under One Process：一个 PIE Dedicated Server 世界及两个客户端世界，Authority／AutonomousProxy／SimulatedProxy 已读取核对。不是独立 Dedicated Server 可执行文件构建。
- 各 NetDriver 配置 `NetEmulation.PktLag 100`。7 项实际命中用例全部通过：同级 Skill、低于 SuperArmor、同级 Vajra、击退、挑飞、倒地起身、空中取消恢复。
- 目标使用正式 PawnData Profile、正式受击 GA 蓝图及正式 AnimBP；测试授予只增加两份独立攻击／长动作 Definition，没有第二份受击 GA，也没有覆盖目标 Profile。攻击端使用隔离探针，不改正式 DA_Attack 的 Impact。
- 轻反馈用例中，服务器、拥有者客户端和另一客户端都实际同时播放主 Hold 与 LightFeedback 蒙太奇，Hold GA 仍激活，生命正常减少，无控制和胶囊受击位移。
- 轻反馈重叠期间采集骨骼局部旋转，并与同时间主动画原姿势比较：三个视角的 Spine2 最大偏差约 30°，Pelvis 最大偏差小于 0.001°，证明上身实际参与混合、骨盆保持原姿势。权重 0.35 是初始配置，可按实际手感调小。
- 强受击结束后三个视角控制解除、生命一致，最大最终位置误差约 0.0048 厘米。
- PIE 已停止，监听器已解绑并执行后续 GC；测试 Definition 的临时变更通过只重载对应 CodexText 包清除。主 AnimBP 和目标 Profile 已在编辑器打开。

逐帧结果 `Saved/HitReactionPIE/hero_two_clients_100ms.json`；骨骼指标 `Saved/HeroHitReactionSetup/bone-metrics.json`；最终资产配置 `configured.json`；图接线前后 `animgraph-before.json`／`animgraph-after.json`。

实际构建与测试命令：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=2
& 'E:/Python/python.exe' Tools/HitReaction/mcp_execute.py Tools/HitReaction/configure_hero.py
& 'E:/Python/python.exe' Tools/HitReaction/mcp_execute.py Tools/HitReaction/create_hero_test_grant.py
& 'E:/Python/python.exe' Tools/HitReaction/mcp_execute.py Tools/HitReaction/start_two_clients.py
& 'E:/Python/python.exe' Tools/HitReaction/mcp_execute.py Tools/HitReaction/setup_two_clients.py
& 'E:/Python/python.exe' Tools/HitReaction/mcp_execute.py Tools/HitReaction/setup_actors.py
& 'E:/Python/python.exe' Tools/HitReaction/mcp_execute.py Tools/HitReaction/set_network_lag.py
& 'E:/Python/python.exe' Tools/HitReaction/mcp_execute.py Tools/HitReaction/run_suite.py
& 'E:/Python/python.exe' Tools/HitReaction/mcp_execute.py Tools/HitReaction/stop_pie.py
```

原生回归命令在普通编辑器执行：`Automation RunTests Hodge.HitReaction+Hodge.Combat+Hodge.Combo+Hodge.Rotation`。`configure_hero.py` 为本轮初始化脚本，会设置 Profile 初始参数；后续手工调整资产后不要自动重跑覆盖。

本轮未执行跨机器网络、模拟丢包、Cook／打包或全部正式五段攻击逐刀受击验收；未改造 Impact 系统，也未实现削韧或自动追击。动画素材的最终美术观感需要按实际角色手感继续调整。

2026-10-09 补充：LightFeedback 蒙太奇原来缺少与加法基准一致的 PreviewBasePose，独立预览会错误叠加到 Skeleton Reference Pose。现增加第 0 帧静态非加法 PreviewBase，189 条动画轨在起止帧保持相同；旋转误差小于 0.001°、位置误差 0。预览基准修正不改变运行时上身范围／权重。后续受击退出与联机取消审查见[网络动画审查](animation-network-review-2026-10-09.md)，其结果覆盖本报告原先未验收的混出连续性和取消时序。

## 字段解释与消费位置

**HitAcceptancePolicy** 在 HodgeHitReactionComponent::FinishHit 使用。AcceptedDamage 要求 HealthSet 接受本次 Damage 输出；无输出时不控制。ExplicitControl 允许无 Damage 输出时尝试控制，但明确拒绝、免疫／GodMode、死亡和等级检查仍有效。普通攻击保留 AcceptedDamage。

**DamageType** 在 HodgeGameplayAbility_Melee::MakeMeleeHitSpec 写入 Spec 的动态资产 Tag；UE 同时加入 CapturedSourceTags，HodgeDamageExecution 传入属性条件求值和来源衰减。当前没有按 Melee／Pistol 等类型分支计算专属伤害或执行控制的业务。它不等于攻击判定，不选择受击动作。

**命中通知 Reaction** 在 HodgeGameplayAbility_Melee::BeginNotifyHit 解析。bUseDefaultReaction=true 使用当前 Definition.DefaultHitConfig.Reaction；false 使用通知 ReactionOverride 整份配置，并非逐字段补齐。bUseDefaultDamage 独立控制 GE／倍率／DamageType 的来源。当前五份正式攻击 Montage 的命中通知均使用默认 Reaction；本轮未修改这些开关。
