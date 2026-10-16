# 共鸣解放配置与调参

更新：2026-10-15。适用于已实现的GA_Resonance_Liberation；设计见[当前实施规格](../Design/resonance-liberation.md)，验证见[本轮验收](../Validation/resonance-liberation-2026-10-15.md)。

## 1. 直接运行

当前正式玩家PawnData已追加`AS_Resonance_Liberation`，输入配置已追加`IA_Resonance_Liberation`，IMC_Default映射R。出生充能0，普通攻击实际命中回能，满100且不在12秒CD时按R；冷却中仍可回能。死亡清空当前充能，持久PlayerState原CD继续计时。

大招GA继承`HodgeGameplayAbility_Melee`，按Standalone Definition运行，不需要蓝图再次播放角色蒙太奇或再次CommitAbility。相机只影响本地释放者，其他客户端仍看角色动作与翼。

### 开发时直接充满

在运行中的游戏窗口按`~`打开控制台，输入`HodgeFillResonance`并回车；当前玩家能量回满到实际容量（首版100），右下条同步刷新。Output Log选择当前PIE世界后也可执行；双客户端建议分别使用各自游戏窗口的控制台，避免选错玩家。

命令只补能量，不移除12秒CD，不自动释放R；角色死亡或资源未初始化时拒绝并提示。单人／Listen服务器本地执行，远端客户端通过当前Controller既有ServerCheat RPC转交服务器，服务器通过正式SetByCaller增量GE修改PlayerState属性。Shipping及Test构建中没有调试效果。

Lyra参考为`LyraCheatManager::God`的客户端转发和`ApplySetByCallerHeal`的权威GE；项目现有完整CheatManager尚未迁入，因此只增补Controller Exec及ServerCheat的精确白名单，未开放任意远端控制台命令或CheatAll。

## 2. 技能与资源

目录：`/Game/Main/Character/Hero/Ability/ResonanceLiberation`。

- `DA_Resonance_Liberation`：ExecutionConfig.Montage为本目录AM_Burst01；ExecutionBodyTag为`State.Combat.Body.Vajra`，DefaultHitConfig.Reaction.AttackJudgementTag为`Combat.Attack.Judgement.Vajra`。初始伤害倍率2，Impacts暂留空。
- `GA_Resonance_Liberation` Class Defaults：AdditionalCosts只有一个HodgeAbilityCost_ResonanceEnergy，内部CostGameplayEffectClass为GE_ResonanceEnergy_Cost。GA原生CostGameplayEffectClass保持空；CooldownGameplayEffectClass为本目录GE_Resonance_Liberation_Cooldown。
- `GE_Resonance_Liberation_Cooldown`：12秒，独立`Cooldown.Hero.ResonanceLiberation`。
- `GE_ResonanceEnergy_Init`：共享初始化父类只将MaxResonanceEnergy设为100；不补充当前能量。AS不再授予第二份ResonanceSet。
- 普攻 `DA_Attack_1～5` 和 `DA_Resonance_Skill`：在 `AttackRules` 中选择 `HodgeResonanceAttackRuleConfig`，`GainPerPhase` 分别初值 5 和 20，`GainEffect` 为 GE_ResonanceEnergy_Gain。大招不配置该命中收益规则。旧 Definition 同名能量字段已迁移，不能继续作为正式参数编辑。

回能依赖真实生命扣除，不由命中提示或Overlap数量决定。相同执行、相同HitGroup＋AttackPhase只回一次；新增多段伤害时给真正独立阶段不同Phase，同刀多个检测源保持相同Group／Phase。

金刚体是控制等级，不是无敌；同级可以通过比较；空Impacts不会自动添加僵直、击退或挑飞。Dash中断沿用既有Tag Relationship Mapping，不因金刚体自动豁免。

## 3. 镜头构图

打开本目录`DA_Resonance_Liberation_Camera`，调整实际存在的字段：

1. `Shots`按Start／End连续覆盖0～1；初版Acquire、FaceHands、WingReveal。时间是相对于电影通知窗口归一化，不是整个蒙太奇比例。
2. `Anchor`定位相机参考点，`LookAt`定位观察点；现用真实`Bip001Head`。空名称明确表示角色根部；缺失名称只有配置合法FallbackAnchor时允许回退。
3. `StartOffset/EndOffset`单位厘米，使用角色水平朝向：X前、Y右、Z上；头骨只提供位置，不继承骨骼旋转。`LookAtOffset`调画面焦点。
4. `FOV`为起止水平角，越小越近景；PositionCurve是该段0～1的绝对局部位置，FOVCurve是角度。曲线为空时使用缓入缓出的起止插值。
5. `BlendFraction`控制本段开始与上一段末帧的混合占比；0为硬切。`BlendFractions=(0.16,0.20)`分别控制整个电影窗口的进入和返回比例。`CancelBlendSeconds=0.25`仅用于中断后的实时间返回。
6. `bSuppressLookInput=true`只抑制电影窗口内的玩法Look输入；不禁用移动意图，不重置其他系统输入限制。
7. `ProbeRadius=8`、`MinLookAtDistance=35`厘米，`PitchLimits=(-70,70)`；严重遮挡退出电影，正常玩法与伤害继续。

打开`AM_Burst01`通知轨，找到`Hodge Cinematic Camera`，Profile为上述资产；窗口当前0～2.266667源秒。移动通知条即可调开始／结束，必须不超过主蒙太奇前半段，且首版只允许一个连续电影窗口。

`PostProcess/Weight`是本地可选后处理覆盖，当前权重0.2，轻微暗角和曝光降低；不永久修改地图／相机设置。`LocalEvents`可填唯一EventId、窗口归一化Time、可选非单例CameraShake和非循环短Sound。未填素材就没有对应正式效果；事件不是服务器伤害触发器。世界Niagara仍使用现有动画通知／GameplayCue。

调参顺序建议：先关闭PP在正式地图检查脸／手／翼构图，再调FOV和切点，最后补灯光、特效与音效。验证地图的单向灯光会使正面特写呈暗剪影，不能仅通过进一步压暗曝光解决。

## 4. 翼、武器与命中

主蒙太奇引用已有`/Game/Wuwa/Anim/AM_Burst01`序列，长度约4.533333源秒，不更名或重做源动作。

`WingAction`状态窗口全程，WingMontage指向`/Game/Main/Character/Hero/Wing/Montages/AM_Wing_Burst01`，其来源为`/Game/Wuwa/Wing/BaseAnim/G_Burst01`。翼由主蒙太奇实际源时间同步；正常结束或取消走既有淡出。武器窗口使用FadeAtHand。

单次Hit通知初值3.082667源秒（0.68T），需要你结合实际挥击姿势调至冲击帧。通知的具体配置：

- Source=AvatarRoot，Shape=Box，TransformPolicy=SnapshotOnEventEnter。
- BoxHalfExtent=(600,450,200)厘米，即全尺寸12×9×4米；LocalTransform.Translation=(400,0,0)，中心在触发时角色前方4米。
- HitGroup=ResonanceLiberation、AttackPhase=Burst、RepeatHitInterval=0。
- Profile=DA_Resonance_Liberation_HitProfile，Overlap／OnceOnEnter；保持敌人对象类型、友方过滤与视线过滤。
- bUseDefaultDamage和bUseDefaultReaction均开启，分别继承伤害与金刚体输出。

这是一次空间查询，多目标每个一次；触发后玩家再移动、转向或新敌人进入均不会再次命中。需要持续范围时应另行设计，不把当前单次通知拉成长窗口冒充持续攻击。

Recovery／MoveCancel窗口从0.82T到末尾，沿用Standalone后摇取消机制；电影窗口结束不提前结束GA或移除自身体状态。

## 5. 右下方技能图标与充能环

2026-10-15 已替换早期的宽条占位控件。`/Game/Main/UI/HUD/WBP_HodgeHUD`中的`ReferenceHUDCanvas/ResonanceEnergy`：锚点和对齐均(1,1)，偏移(-48,-24)，尺寸76×100；左侧`ResonanceSkill`偏移(-196,-24)，尺寸相同。调整Canvas Slot可改位置；图标大小还需同步子类默认值`IconSize`。

子类路径仍为`/Game/Main/UI/HUD/WBP_Resonance_Energy`，保留已有HUD引用。共用`UHodgeAbilityIconWidget`：中央图标从下向上恢复冷却亮色，倒计时向上取整；大招额外以金色环显示真实能量，12点起顺时针增长。能量不足或冷却未结束时不能释放，两个百分比互不替代。

纹样、材质、暗度和环颜色通过子类Class Defaults配置；两个WBP已经保存完整Designer树，数字字体、尺寸和子项排版直接在Designer修改，运行时不再生成布局。`WBP_HodgeHUD`里的`ResonanceEnergy`现为`UIExtensionPointWidget`，由`EAS_HodgeGameplayUI`注入真实Widget。旧`EnergyFill`／`EnergyLabel`／`LiberationButton`不再使用。完整资产路径、参数和预览说明见[共鸣图标配置](resonance-ability-icons.md)。

Widget读共享UI数据源，显示真实能量、CD和按钮状态。依赖尚未到达时透明等待，不显示假的满条，不停止必要刷新。按钮与R共用`InputTag.Ability.ResonanceLiberation`，不直接调用服务器扣资源。

## 6. 自动化工具与边界

- `Tools/CinematicUltimate/configure_assets.py`：首次创建和幂等装配；交付标记保护已有用户调参。正式调参后不要删除标记并反复初始化。
- `validate_assets.py`：只读配置断言及限定蓝图原生编译诊断；报告在Saved/CinematicUltimate/assets-results.json。
- `run_batch.py single listen dedicated listen-lag`：真实输入、源实例、资源与世界伤害的PIE测试，会临时隔离验证地图敌人和调整性能设置，结束需运行本目录restore_editor.py。

正式Niagara／音效／震屏素材、近墙与各画幅的最终构图、独立专服可执行程序及Cook／打包仍需后续验证。当前验收不声称复刻了原作所有美术与分镜。
