# 目标侧削韧受击与原地运动

> 后续约定：累计削韧仅保留 Light／Heavy；Break 不再生成击飞，改由每个命中分段的 Can Launch 唯一决定。见[分段击飞设计](attack-segment-launch.md)。下文破韧阈值与保护描述保留为上一版实现记录。

日期：2026-10-18。基线：本地 `dev-Combat`，保留此前未提交的 ACT／Behit 实现。用户已授权本轮编码、资产迁移与测试。本设计取代普通攻击通过表现力度选择 S/B、由完整受击根轨迹决定位移的方案；不删除特殊 Impact 执行器。

## 范围与现有链路

沿用 Melee → BeginHit → GE／HealthSet.RecordDamage → FinishHit → HitReactionComponent → 目标 HitReaction GA → CMC。伤害公式、GAS 所有权、攻击 Notify 时机、技能消耗和既有网络入口保持原链路。没有新模块、AttributeSet、组件或平行受击状态机。

本机 Lyra 参考：`Source/LyraGame/Animation/LyraAnimInstance.cpp::InitializeWithAbilitySystem`、`AbilitySystem/Attributes/LyraHealthSet.cpp::PostGameplayEffectExecute`、`AbilitySystem/Executions/LyraDamageExecution.cpp::Execute_Implementation`。复用其 ASC 驱动、服务器属性结算及动画职责边界；本机 Lyra Source／GameFeatures 未发现可直接迁入的连续削韧裁决实现。新增机制在现有 Hodge 受击事务内最小适配；不声称 Lyra 已实现本方案。

本轮不做完整 Boss 共振度、破韧 UI、跑攻／切手、HitStop、相机重做或此前搁置的 Client 共鸣前连段故障。

## 数据与唯一来源

- `FHodgeHitReactionConfig.InputMode`：Legacy（旧资产兼容）、PoiseDriven（普通攻击）、ExplicitImpact（特殊技能）。新模式互斥，PoiseDriven 禁止 Impacts，要求 AcceptedDamage；ExplicitImpact 禁止非零削韧。
- `ReservedPoiseDamage`：保留反射名及序列化，常规界面显示 Poise Damage；这是唯一削韧数值来源。Legacy 仍只透传，不消费。
- `ImpulseScale`：攻击的运动倾向 0～4，不改变生命伤害和削韧。`PresentationStrength` 仅控制接触表现，不选择普通攻击的轻重等级。
- `UHodgePawnData.Poise`：嵌入轻量 `FHodgePoiseSettings`。保存窗口、阈值、破韧结果、保护及目标运动预设；不再塞进动画 Profile。
- `FHodgeHitReactionState.ReactionLevel`：解析后的 None／Light／Heavy／Break，与 Phase 和六种 Impact 正交。原有已解析 Montage 字段及复制职责保留。

默认目标样例：窗口 1.5s，阈值 20／55／100；破韧控制释放后保护 1s；普通重受击基础退步 65cm／0.25s，目标位移系数 1，最大地面距离 200cm；击飞速度水平 180、垂直 650cm/s。均为 Hodge 原型参数，不是参考游戏实测值。Guardian 使用自己的 Knockdown 破韧结果，不强行接入 Hero 骨架的 Fly。

## 裁决规则

1. 服务器在 FinishHit 移除本次 PendingHit 后消费一次削韧。多个 GE 只汇总生命损失，重复 Finish 不再消费。普通削韧必须实际生命损失大于零。
2. 使用滑动时间窗：新命中先移除到期样本，再追加正值；零削韧不追加、不延长窗口。队列上限默认 128、可配 1～1024；超限显式诊断并拒绝本次削韧，保留实际生命伤害和既有样本。
3. 每轮只消费更高的阈值；一次大额命中只提交最高等级。分数下降再跨同一阈值不重播；窗口完全清空才开始下一轮，清空不提前取消受控状态。
4. Light → HitStun，Heavy → Knockback，Break → 目标选择的 Launch／Knockdown／HitStun。提交失败不消费成功等级，下次新的正值命中可以重试；Timer 不触发受击。
5. 成功破韧清空队列，保护覆盖破韧控制、落地和恢复，再从控制释放起延长指定秒数；期间只扣血／接触，不预存下一轮削韧。
6. 默认低判定不积累且不打断。高级选项允许霸体中积累，但不绕过 BodyRank；解除霸体不自动发作，仅下一次合格新命中重新裁决。
7. Airborne／Downed／GettingUp 的普通攻击只扣血与反馈，不累积、不延长控制。显式 AirHit／Slam 仍受原次数和总控制上限约束。
8. 免疫、闪避、拒绝、死亡与零实际伤害不积累。特殊合法零伤害控制继续使用独立 ExplicitControl 资格，不混为普通削韧。
9. 死亡标签或生命归零、ASC 解绑、PawnData 变化和对象销毁清空队列／保护／Timer；计时器检查弱 Avatar 与代次，不作用于旧 Pawn。攻击者或观察者断线不清空活目标的全局进度。
10. 客户端不计算、不复制完整时间窗，仅接收现有受击状态、GAS Montage 和 CMC 运动结果。

## 运动与回弹修复

独立只读运行证据：`Saved/HitReactionReadOnlyAudit/isolation.json`。同一 B_L 实例、权重 1、无外部 RMS 的单机中采到单步约 60.43cm 向前跃迁；原始 Wuwa 与项目副本 Root 均在约 1.18～1.27s 出现回跳，Pelvis 也异常。不能归咎于网络或重复播放。

新建 `Behit/InPlace` 项目副本：关闭动画根运动并强制锁定首帧 Root。S 动作使用前 0.70s，B_R 前 1.0s，B_L 只使用前 1.10s，明确排除已证实的坏区间；不合成新骨骼动作、不改第三方源包，也不宣称修好了原始坏帧。完整旧片和正式引用迁移前备份保留。

地面运动由现有曲线归一化 RMS 执行：距离 = min(目标最大距离, 目标重受击距离 × 攻击 ImpulseScale × 目标 DisplacementScale)。空中继续 CMC 弹道、重力和真实落地。相同动作可以匹配有限范围内不同距离；过大距离仍可能滑步，需要动画师素材打磨。

普通削韧的远离来源位移使用命中瞬间攻击者位置快照；检测体原点仍用于精确接触／局部来源表现，不作为普通击退源。刀刃可能位于目标侧面或已经越过目标，直接用该点会产生错误的侧向退步。Legacy／Explicit 的已有方向配置保持兼容，实际接触点与法线没有被角色原点替代。

原地恢复尾部仅保留姿态，不保留普通受击胶囊运动。保留 AnimBP 的 RootMotionFromMontagesOnly，攻击和闪避仍正常使用自身根运动。

## Profile 精简与迁移

`bUseCompactAnimations` 显式选择唯一布局。常规配置为 `Compact.Light`／`Heavy` 左右动作与各自控制时长、`Fly`／`Push` 的 Start／Loop／Landing、公共 Downed／GetUp。Slam 复用 Push；Guardian 不填写不支持的空中家族。公共起身只配置一次，不改变 State 的已解析引用。

原 `Animations` 数组及全部 UPROPERTY 保留在兼容布局中，新布局不读取或校验隐藏旧条目。根运动恢复尾部和旧重播门槛在精简布局隐藏，控制／空中上限归入高级选项。接触音画、局部反馈和必要恢复限制继续保留。

正式五段按 01→02→03→05→04 设置演示削韧 15／20／25／25／80，每次真实命中分别计入滑动窗口；05→04 的实测命中间隔约 1.37s，因此终结刀从本轮初值 50 校准到 80，使窗口内的 25＋80 跨越 100。仅此终结刀独立命中为 80，仍不足以单刀破韧。不保证动作过慢、漏刀或旧样本到期后仍必然破韧。共鸣解放保留 ExplicitImpact Launch 供 Fly 测试。只切换受击配置，不修改原伤害、判定框、攻击时机或消耗。

迁移前在 `/Game/Tests/Combat/PoiseMigration/Backups` 保存当前 Profile、相关 PawnData、五段和 R。工具发现已有批次时停止，不覆盖手调。所有新原地素材在独立目录，完整旧动作与备份不删除。

## 验证与交付边界

原生测试覆盖阈值推进／最高等级、同轮防刷新、滑动到期、破韧保护、多 GE／重复 Finish、零伤害与免疫、Body／空中倒地、失败不消费、死亡解绑、容量及配置互斥。执行常规 Editor／Game 构建，再冷启动验证资产和 AnimBP。

实际 PIE 复用装备授予、原生动作队列、Notify、GE、目标 GA 与 CMC；记录每端胶囊／Root／Pelvis、Montage 身份及运动来源，覆盖原地轻受击、不同距离 RMS、Fly、连续阈值和取消。补正式五段／闪避／R 冒烟、Listen／PIE Dedicated 及 100ms 模拟延迟。

实际命令、通过／失败报告和未验证项见[本轮验证记录](../Validation/poise-driven-hit-reactions-2026-10-18.md)。PIE Dedicated 不等于独立进程专服；构建／断言通过不代表主观手感或高质量动画验收完成。
