> 2026-10-07 更新：本文中旧 Timeline／HitWindows 的字段与制作步骤仅保留为历史；当前攻击入口以 [通知配置手册](../Guides/attack-ability-configuration.md) 和 [通知迁移更新](27-update-2026-10-07-anim-notify.md) 为准。
> **2026-10-15 更新：Timeline 已彻底退役** —— `UHodgeAbilityTimeline` / `HodgeAbilityTask_PlayTimeline` / `HodgeTimelineEvaluator` 全部移出 `Source/`（搜索 0 命中）并迁入 `Archive/Timeline/`；攻击窗口与事件改由 **Montage 动画通知**驱动；新增共鸣（Resonance）与暗涌（Havoc）体系。详见 [本轮更新](30-update-2026-10-15.md)。

# GAS、AbilitySet 与技能生命周期

[返回知识库](README.md) · [本轮更新](30-update-2026-10-15.md) · [接通清单](12-integration-backlog.md)

> 当前核对：**2026-10-15**（HEAD `b702482`）；运行结论仅限 `Docs/Validation/` 里对应日期报告的范围。

## 玩家 ASC 与输入

PlayerState 创建玩家 ASC、HealthSet/CombatSet，PawnExtension 以 PS 为 Owner、当前身体为 Avatar 接入；HeroComponent 绑定输入和默认相机。PlayerController.PostProcessInput 消费 ASC 输入，换 Pawn 要同时考虑解绑、重绑及输入清理。

PawnData 的 AbilitySets 在权威端授予，但 PlayerState 仍未保存完整 GrantedHandles 撤销账本。EquipmentManager 为装备来源保存句柄，卸装可精确撤销，不能混同两条路径。

## 现有普攻主链

Input → CombatComponent 选择跳转 → GAS 激活 Definition/Melee GA → 播放对应 FullBody Montage → 原生通知直接请求状态、手持与命中 → PostPhysics 采样 → Montage 生命周期／EndAbility 统一清理。

当前协调者是 Pawn 上的 HodgeCombatComponentBase，Experience 注入；旧 HodgeComboComponent 已移除，不在 PlayerState 另挂连段组件。旧 BasicAttack 类／Timeline 夹具已集中归档，退出活动 Source 和 Content。

CurrentComboTag 表示当前动作，ComboMemory.Node 记最近成功启动段。前四段自然结束/中断/移动取消后开始默认 1 秒保留计时，其他技能不刷新也不暂停它；记忆不保留攻击、旋转锁或命中标签。动作结束后仅允许 bAllowAfterExecutionEnded 的输入边续接；动作播放中继续检查窗口条件。末段后摇直接接第一段，末段结束不保留记忆。

输入缓存仍为单槽 0.3 秒，与连段记忆不同。客户端预测与服务器重新选边；服务器不接受客户端直接指定目标段，也不做历史窗口补偿。失败不得推进已确认节点，拥有者通过权威记忆校正恢复。

## 窗口与生命周期

Status.Rotation.Locked 由角色旋转组件消费，Status.Weapon.Hand 由 Definition 的 WeaponUseWindowTag 消费。手持请求用独立句柄，EndAbility 只释放自己的执行；命中窗口清理先于手持释放。

**命中窗口现在是 AnimNotify（不再是 HitWindows 资产字段）**：`UHodgeAnimNotifyState_HitCheck` / `UHodgeAnimNotify_Hit` 直接携带 `FHodgeAnimHitConfig`（来源、形状、偏移、倍率、去重 `HitGroup + AttackPhase`），手持窗口必须覆盖实际命中并更早进入。

⚠️ 正式五段普攻的**通知配置是否逐段填全**仍只能在编辑器确认（旧结论"五个 DA_Attack 的 HitWindows 均为空"属 Timeline 时代，不能照抄）；但**命中链路本身已通** —— `HodgeGameplayAbility_Melee.cpp:406` 构造并提交伤害，`HodgeDamageExecution` 倍率已启用（见 [08](08-combat-health.md)），共鸣解放 / 暗涌的验证报告都含命中项。

## 攻击命中检测

- `UHodgeHitDetectionProfile` 选策略，三种：`UHodgeSocketSweepStrategy`（刀身扫掠）、`UHodgeBoxSweepStrategy`（盒体扫掠）、`UHodgeShapeQueryStrategy`（形状重叠查询）
- 检测体用 `FHodgeHitVolumeConfig` 配置（Sphere / Box / Capsule + 锚点 AvatarRoot / RegisteredSource / ExecutionTransform / ExecutionTarget）
- 调试绘制 CVar：`Hodge.Combat.DebugDraw.Weapon` / `.Body` / `.HitBox`（默认 5 秒，**仅服务器绘制**）
- 检测挂在 `HodgeCombatComponentBase`（由 Experience `AddComponents` 注入 Pawn，**不是** Pawn 的 C++ 默认子对象）

> ⚠️ 该职责即将被抽离：[架构审查](../Design/architecture-refactor-review-2026-10-15.md) 的 R1 建议独立为 `UHodgeHitDetectionComponent`（`HodgeCombatComponentBase.cpp` 已达 1525 行）。

## 共鸣（Resonance）与暗涌（Havoc）

两者都**复用既有 Melee 模板 + 动画通知**搭出，没有引入新战斗框架：

| 组成 | 说明 |
|---|---|
| `UHodgeResonanceSet` | `ResonanceEnergy` / `MaxResonanceEnergy`，**资源挂在 PlayerState 的 ASC 上**（`HodgePlayerState.h:314,316`） |
| `UHodgeAbilityCost_ResonanceEnergy` | 能量作为可插拔消耗项（复用 `UHodgeAbilityCost` 体系） |
| `UHodgeHavocSet` / `UHodgeHavocRules` | 暗流燃料（DarkFlow）与灭音 / 强化五段 / 破命 / 湮灭增伤规则 |
| `UHodgeAnimNotify_DarkSurgeCommit` | 暗涌提交（通知驱动） |
| `UHodgeAnimNotifyState_CinematicCamera` | 共鸣解放前半段的电影镜头窗口（配合 `HodgeCinematicCameraProfile/Shots` + `UHodgeCameraMode_Cinematic` + `HeroComponent::RefreshCinematicCamera()`） |

配置入口：[`resonance-skill-configuration.md`](../Guides/resonance-skill-configuration.md)、[`resonance-liberation-configuration.md`](../Guides/resonance-liberation-configuration.md)、[`havoc-dark-surge.md`](../Design/havoc-dark-surge.md)。⚠️ 参数是**原型初值**；`HodgeFillResonance` / `HodgeFillDarkFlow` 是**调试填充命令**，不是正式获取途径。

## Context 与效果

HodgeAbilitySystemGlobals 分配 FHodgeGameplayEffectContext；Melee GA 每次命中独立构建 Spec，写来源、HitResult、Origin、SetByCaller 倍率等，再由服务器施加。关系映射、Cost、激活组、失败标签沿用 GAS 主体系；GameplayCue 仍有注册/预加载缺口。

源码：[CombatComponent](../../Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp)、[Definition GA](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp)、[Melee GA](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp)、[ASC](../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp)。
