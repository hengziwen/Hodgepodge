# Resonance Skill 配置指南

日期：2026-10-13；UE 5.5.4。已创建并授予当前玩家，首版 **E 键、8 秒 CD**。

UE 资产名使用 `GA_Resonance_Skill`，不含空格；说明和显示元数据保留 Resonance Skill。技能使用现有 `HodgeGameplayAbility_Melee` 与 Standalone Definition，没有新增攻击执行系统。

## 1. 已创建资产

技能目录：`/Game/Main/Character/Hero/Ability/ResonanceSkill`。

- `GA_Resonance_Skill`：Melee 蓝图子类，配置原生冷却 GE。
- `DA_Resonance_Skill`：技能身份、执行路由、动画、伤害、体状态与转向配置。
- `AM_Resonance_Skill`：独立蒙太奇模板，复制正式第1段普攻；不是新制作的共鸣技能动画。
- `GE_Resonance_Skill_Damage`：独立 Instant 伤害 GE，沿用项目伤害计算。
- `GE_Resonance_Skill_Cooldown`：HasDuration 冷却 GE，默认8秒，只授予本技能冷却标签。
- `AS_Resonance_Skill`：通过 GrantedAbilityDefinitions 授予该 Definition，等级1。
- `/Game/Main/Input/InputAction/IA_Resonance_Skill`：Boolean 输入动作。

接线资产已增补，原有条目保留：

- `/Game/Main/Data/PawnData/DA_Dafult_PawnData` 的 AbilitySets 加入 AS_Resonance_Skill。
- `/Game/Main/Input/DA_HodgeInputConfig` 的 AbilityInputActions 关联本技能输入标签。
- `/Game/Main/Input/IMC_Default` 将 E 映射至 IA_Resonance_Skill。

## 2. 标签

在 `Config/Tags/ResonanceSkillTags.ini` 注册：

- `Ability.Type.Action.Melee.ResonanceSkill`：Definition 独立身份，继承近战分类。
- `InputTag.Ability.ResonanceSkill`：输入语义，授予时写入 Spec。
- `Cooldown.Hero.ResonanceSkill`：独立冷却，不与普攻或 Dash 共用。

GA 资产标签保留 `Ability.Type.Action.Melee` 并加入本技能身份，沿用现有 Dash 中断和 Death 规则。不要把冷却标签加入技能的 ActivationOwnedTags，否则动画退出时可能产生错误语义；冷却由 GE 授予和到期移除。

## 3. 你主要调整哪些地方

**动画和时序**：打开 DA_Resonance_Skill，调整 `ExecutionConfig.Montage`、`PlayRate` 和混合配置。默认已指向 AM_Resonance_Skill；可在该蒙太奇中替换动画段并调整命中、Recovery、武器表现通知。

**伤害**：在 DA_Resonance_Skill 的 `DefaultHitConfig` 调整 DamageMultiplier、DamageType、Reaction 等参数；`DamageEffect` 指向独立 GE_Resonance_Skill_Damage。蒙太奇命中通知默认使用 Definition 伤害，通知备用伤害 GE 也已改为独立副本。

**范围和多段命中**：在 AM_Resonance_Skill 的 Hodge 命中通知调整检测来源、形状、尺寸、锚点、HitGroup／AttackPhase 和去重配置。当前保留首段普攻的装备武器检测，不代表最终技能范围。

**CD 时间**：打开 GE_Resonance_Skill_Cooldown 的 Class Defaults，在 Duration Magnitude 的 Scalable Float Value 调整8秒。DurationPolicy 保持 HasDuration，Target Tags 组件的独立冷却标签保持一致。

**CD GE 类型**：在 GA_Resonance_Skill 的 Class Defaults → Cooldowns → CooldownGameplayEffectClass 调整。无需在 ActivateAbility 图中重写计时或再次播放蒙太奇，执行链由父类负责。

**起手体状态**：在 DA_Resonance_Skill 调整 ExecutionBodyTag。模板沿用源普攻配置；需要技能硬体时可选择已有 `State.Combat.Body.Skill`。它与输出攻击判定的强度分开。

**转向**：FacingWarp 沿用首招配置，默认旋转窗口0～0.1秒。更换动画后必须按首次命中时间重新校验窗口；无需转向时可关闭 FacingWarp。不要把窗口延伸到首刀之后。

**按键**：在 IMC_Default 找到 IA_Resonance_Skill 的 E 映射修改；改键不必修改输入 Tag 或 Definition。手柄本轮未接。

## 4. 默认行为与边界

技能发动时由 Definition 的 CommitAbility 提交冷却。即使打空或被 Dash／受击中断，已提交的 CD 仍保留。若以后需要命中后才开始 CD、充能次数或返还冷却，需要另行明确行为。

这是可运行的近战攻击模板：仍使用现有主角骨架、FullBody Slot、武器与第一段普攻素材，不包含新动画、美术、音效、专用 GameplayCue 或技能 HUD 图标。当前主 HUD 血条／终焉入口不额外改造。

冷却持续在玩家 ASC 上，玩家 ASC 属于 PlayerState；重生后的冷却保留策略沿用现有框架，本轮未重做重生生命周期。

## 5. 制作和验证

制作工具见 [Tools/ResonanceSkill](../../Tools/ResonanceSkill/README.md)。已完成16项资产检查、三个蓝图原生编译状态检查和四种 PIE 模式各11项运行断言；实际结果见 [本轮验证](../Validation/resonance-skill-setup-2026-10-13.md)。

工具通过模板版本元数据避免重跑覆盖调参；不会恢复用户后来选择的正常键位。修改完资产后直接 Compile／Save，并在 PIE 测试新的动画、通知和参数。

## 6. 当前 Skill1 的移动取消（2026-10-14）

当前 DA_Resonance_Skill 实际引用 `/Game/Main/Character/Hero/Ability/ResonanceSkill/AM_Resonance_Skill1`；前文 AM_Resonance_Skill 是创建时的模板路径，不要重跑旧模板脚本覆盖用户后来编辑的动画。

打开 `/Game/Main/Character/Hero/Ability/ResonanceSkill/DA_Resonance_Skill` → Hodge → MoveCancel：

- `StandaloneMoveCancelWindowTag` 默认 `Status.Attack.Cancel.Move`，对应蒙太奇中的 Hodge GameplayTag 通知状态；留空关闭移动取消。
- `StandaloneMoveIntentThreshold` 默认 0.1，输入向量长度达到阈值才允许取消；取值 (0,1]。
- `ExecutionConfig.StopBlendOut.Time` 控制取消退出混合，目前 0.1 秒；自然退出仍使用 NaturalBlendOut。

打开 AM_Resonance_Skill1，调整现有 `Status.Attack.Cancel.Move` 通知状态的开始／结束位置即可控制后摇取消时段。当前窗口约为源时间 1.0989～4.3333 秒，末次命中结束约 0.7654 秒。Recovery 只表达动作阶段，不替代取消许可。无需扩大当前窗口来修复逻辑，也不需要在 GA 蓝图中另写取消。

移动提前按住时，窗口开启后取消；进入窗口后再移动也取消。窗口前松开、输入低于阈值、未配置通知窗口时仍正常收招。取消保留已提交的八秒 CD。联机预测被拒绝会恢复原蒙太奇，同一持续输入不重复请求，松开重按后可重试。

设计及需求编号见 [Definition 移动取消](../Design/definition-move-cancel.md)。
