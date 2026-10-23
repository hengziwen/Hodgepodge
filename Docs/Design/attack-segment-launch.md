# 攻击分段决定击飞

日期：2026-10-18；取代[上一版削韧设计](poise-driven-hit-reactions.md)中的累计破韧击飞规则。

## 接口与职责

唯一作者字段为 `FHodgeAnimHitConfig.bCanLaunch`，位于 Montage 的单次命中、持续命中及 AdditionalHits 各分段中，默认关闭。每段可以独立选择，不从 Definition 默认反应、伤害大小或累计削韧隐式推导。

`FHodgeAnimHitConfig::ResolveReaction` 先解析默认／覆盖反应，再覆盖本分段开关。Melee 将解析结果传入现有检测批次和受击事务；`FHodgeHitReactionConfig.bCanLaunch` 是 Transient 运行快照，不是第二个可保存的配置入口。

沿用 Melee→GE／HealthSet→FinishHit→HitReactionComponent／GA→CMC。服务器确认伤害与权限，客户端沿用现有 State、GAS Montage 和 CMC 复制，不增加 RPC、模块或平行状态机。

## 裁决规则

- 削韧窗口、轻硬直和重硬直规则保留；累计超过旧破韧阈值也只到 Heavy。旧 BreakThreshold／BreakImpact／BreakProtection 保留序列化但隐藏，不参与校验或执行；ReactionLevel.Break 保留枚举兼容，不再由此链路产生。
- 分段开关开启：通过现有伤害接受、BodyRank、目标动画与运动支持检查后，直接生成 Launch／Fly，不需要达到削韧阈值；同一命中不同时结算一份削韧控制和一份击飞。
- 空中再次命中且开关开启：重新播放 Fly_Start，继续 Fly_Loop，再在真实接地时播放 Fly_Fall；不以 Push 代替 Fly。续飞消耗 MaxAirHits，沿用首次 ChainStartedAt／AirStartedAt 和总控制上限，不能无限续空。
- 空中再次命中且开关关闭：保持当前动画身份、弹道与排队速度，不重播 Fly／Push，不立即在空中播放地面动作；标记落地倒地，实际接地后由现有 Downed→GettingUp 路线恢复。
- 无开关的旧 Launch／AirHit 参数不能绕过资格；参数只提供允许击飞后的运动强度。开关开启时统一选择 Fly，旧 Impact 不能改成 Push 或 Slam 来覆盖这个选择。
- 免疫、闪避拒绝、死亡和普通零实际伤害不能触发击飞；低判定不能绕过霸体。目标不支持 Fly 时明确返回 Unsupported，不伪报成功或把 Fly 静默换成地面受击。
- 轻重僵直的动画、RMS 距离、滑动到期、同级不刷新、死亡和 ASC 清理保留。

## 正式资产

迁移只在正式第 04 刀（连段终结刀）和共鸣解放 R 的现有命中分段开启 Can Launch，保留原来的击飞测试入口；01／02／03／05 保持关闭。先复制对应 Montage 到 `/Game/Tests/Combat/SegmentLaunch/Backups`，只保存这两份正式 Montage。Definition、伤害、削韧数值、消耗、通知时刻和判定框保持原值。

既有 R Launch 参数继续提供原速度；无显式参数时采用目标 PawnData 中的 Launch 速度与抵抗系数。目标参数不授予击飞资格，也不依赖 Poise.bEnabled 或旧 BreakImpact。Guardian 不支持 Hero 的 Fly 骨架，保持 Unsupported 边界，不强行替换素材。

## Lyra 核查与验证

本机 Lyra `LyraDamageExecution.cpp::Execute_Implementation` 使用服务器效果结算并从 EffectContext 获取命中信息，`LyraAnimInstance.cpp::InitializeWithAbilitySystem` 绑定 ASC 标签；继续沿用项目已迁入的这些边界。检索 Lyra Source／GameFeatures 未找到完整的近战分段击飞／空中续飞裁决，因此在 Hodge 现有事务内适配，不引入同职责系统或新依赖。

原生测试覆盖分段优先权、默认／覆盖反应、旧 Launch 无开关、任意大削韧不击飞、零削韧带开关击飞、空中重播／不开关不重播、真实接地阶段、重复 Finish、Body、接受条件及次数上限。真实 PIE 使用正式装备探针和 Notify／GE 检查动画实例与落地起身，再回归正式五段、R、Listen／Dedicated PIE 与 100ms。

实际命令、变更清单和测试结果见[验证记录](../Validation/attack-segment-launch-2026-10-18.md)。
