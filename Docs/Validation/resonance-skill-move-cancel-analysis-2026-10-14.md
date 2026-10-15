# AM_Resonance_Skill1 移动取消后摇排查

日期：2026-10-14；本文为修复前只读排查记录：根因已通过源码、当前资产及单人/Listen Server PIE 复现确认，当时尚未编码。后续实现见 [Definition 移动取消设计](../Design/definition-move-cancel.md)；本节保留历史复现事实。

## 1. 当前配置与事实

`/Game/Main/Character/Hero/Ability/ResonanceSkill/DA_Resonance_Skill` 实际引用 `AM_Resonance_Skill1`，ExecutionRoute=Standalone，PlayRate=1，ExecutionBodyTag=State.Combat.Body.Normal。旧 AM_Resonance_Skill 路径加载后也解析到同一新资产，不是实际运行另一份模板。

技能蒙太奇源时长约 4.3333 秒。Recovery 通知源时间约 1.0816～4.3333 秒；Status.Attack.Cancel.Move 通知约 1.0989～4.3333 秒，使用 HodgeAnimNotifyState_GameplayTag/BranchingPoint。末次命中约在 0.7654 秒结束，取消窗口位于伤害之后。Definition StopBlendOut.Time=0.1 秒，NaturalBlendOut.Time=0.2 秒。

AbilityTags 为 Ability.Type.Action.Melee 和 Ability.Type.Action.Melee.ResonanceSkill。没有为该技能新增会阻止取消的特殊体状态；正常服务器技能取消已实际成功，不能将此问题归因为 CanBeCanceled=false 或 CD 标签。

## 2. 根因与调用链

HodgeGameplayAbility_Definition::StartExecutionMontage 只在 IsComboCoordinated 时调用 Combat.ExecutionStarted。CombatComponent 的 CurrentAbility 是连段执行对象，Standalone 技能不会登记。

CombatComponent::TickComponent 的移动取消分支在 CurrentAbility 为空时直接返回。NotifyWindowsChanged 也只向连段路由转发。ServerMoveCancel/ProcessServerMoveCancel、IsMoveCancelPredicted 和 ClientMoveCancelResult 同样依赖 CurrentAbility/ExecutionKey 的连段身份。仅将一个入口改成查找 Standalone 不足以完成联机链路。

Hero.Input_Move 先记录原始意图，随后在 ASC.HasMatchingGameplayTag(Status.Attack) 且没有预测移动取消时返回。CMC.ConstrainInputAcceleration 有相同保护。因此窗口通知可以正常授予标签，却不会自行取消技能；动作未结束时行走输入仍被拦截。

Status.Attack.Recovery 与 Status.Attack.Cancel.Move 都是 Status.Attack 的子标签。拥有取消许可并不意味着 ASC 的攻击状态消失，移除某个标签或直接放开输入不能替代完整技能结束。

## 3. 实际复现与状态机证据

复现脚本位于 Saved/ResonanceMoveCancel。使用真实 IA_Resonance_Skill 和 IA_Move；持续移动在窗口前开始，记录窗口、当前 GA、加速度、速度与底层 Locomotion 状态。随后通过既有原生游戏 Tick 队列请求服务器取消同一个真实技能实例，验证退出链路，不修改资产或产品逻辑。

- 单人：窗口内位置约 2.0000 秒，移动意图=true，取消标签=true，Recovery/Cancel.Move 执行窗口均存在，GA 和 Montage 仍活动、蒙太奇权重=1，行走加速度=0、Locomotion=Idle，Gameplay.MovementStopped=false。
- Listen Server：拥有者位置约 1.6226 秒，服务器约 1.6060 秒，两端 GA/窗口均活动；拥有者移动意图=true、加速度=0、Locomotion=Idle。
- 服务器正常取消后：两端技能结束、蒙太奇退出、攻击及窗口标签清理；拥有者进入 Cycle，移动加速度=2400、速度=420。观察到的窗口内非零速度来自动作运动，不能据此认为行走输入已放行。

这证明动画状态机具备进入移动的正常路径；主要故障发生在 GAS/Combat 的取消权限消费之前。本次不需要重做 Idle→Cycle 过渡或扩大蒙太奇取消窗口。

Listen Server 的服务器 Hero 原始移动意图为 false 属于本地输入职责差异；服务器接收取消请求后应校验请求载荷及自身窗口，不能要求远端 Pawn 的 Hero 本地输入缓存为 true。

证据：Saved/ResonanceMoveCancel/assets.json、single-reproduction.json、listen-reproduction.json、editor-restored.json。

## 4. 推荐修复内容

保持 Standalone 路由及独立 CD。沿用已有 CombatComponent、Definition 的预测停止/恢复和服务器确认 RPC，不新增专用 Resonance Skill 取消系统。

1. 在 CombatComponent 内增加共用的“当前可移动取消执行”查询：以 ASC 当前动画技能/匹配活动 Spec 为事实来源，限定当前 Avatar 的有效 HodgeGameplayAbility_Definition 执行，覆盖 ComboCoordinated 与 Standalone；保留 CurrentAbility 的连段职责。
2. 将移动取消检查与 IsComboReady、连段转换分支分离。输入超过阈值、当前执行 CanBeCanceled、该执行拥有 MoveCancel 窗口且无死亡/受击/输入禁用时才请求取消。持续输入应在窗口开启时自动生效，不能只监听一次 OnMoveIntentChanged。
3. 复用 BeginPredictedMoveCancel 让拥有者先退出姿势并行走；IsMoveCancelPredicted 以匹配的动作身份判断，不再强制 CurrentAbility==预测对象。
4. ProcessServerMoveCancel 按 Avatar、SpecHandle、Activation/PredictionKey 和当前有效执行查找目标，校验服务器该执行的窗口及有限输入向量，然后直接结束对应执行。不要依赖连段节点或要求远端 Hero 有本地移动意图。
5. ClientMoveCancelResult 按请求身份接受或恢复，兼容 Standalone，防止旧回调影响新技能；新动作、解绑和销毁须清理待处理请求。同一次预测被拒绝后保持原有抑制规则。
6. 结束沿用 FinishExecution/EndAbility 和 StopDefinitionMontage 的 0.1 秒取消混出，释放命中任务、翼窗口、朝向、体状态、通知标签与 PoseLease。保持已提交的 CD GE，不退款或重置冷却。

主要修改文件：HodgeCombatComponentBase.h/.cpp。Definition 已有通用预测取消/拒绝恢复与 EndAbility 清理，可直接复用；如果增加通用窗口变化回调，应明确与现有连段 WindowsChanged 分开，不移除所有 IsComboCoordinated 条件。

参数建议：继续使用当前 Status.Attack.Cancel.Move 和 0.1 移动意图阈值；连段保留既有 ComboDefinition 配置。独立技能的通用取消参数若需要单独配置，应在实现中明确归属，不让其隐式依赖某份普攻连段图，也不让用户配置尚不存在的字段。

## 5. 不推荐的处理

- 把本技能改为 ComboCoordinated：会改变授权、输入和节点生命周期，掩盖独立技能通用能力缺口。
- 删除 Status.Attack 或修改 CMC 让攻击期间直接行走：不能结束蒙太奇/命中，也可能放开前摇和伤害阶段。
- 只在 AnimBP 添加 Idle 过渡：完整权重的攻击蒙太奇和 GAS 执行仍然存在。
- 在技能蓝图另写一次移动检测并随意 Montage_Stop：容易漏掉服务器确认、CD 与资源清理。

## 6. 修复后验收

必须新增独立技能回归：窗口前不取消；窗口内点按移动取消；窗口前持续移动在窗口开启时取消；零/阈值以下输入不取消；CD 保留；命中/翼和攻击标签清理；取消后进入真实移动状态。Listen Server/客户端需检查拥有者立即预测、服务器确认、观察者混出，以及窗口不同步拒绝后的恢复；普攻连段移动取消需回归。

当前仅完成排查与复现，没有执行修复后的构建或测试。复现结束已停止 PIE、恢复原地图/性能选项并打开当前技能蒙太奇，无未保存包；原完整 PIE 设置未记录，恢复单人基线。没有提交或推送。

Lyra 对应核查：LyraHeroComponent::Input_Move 提供移动输入；LyraAbilitySystemComponent::CancelAbilitiesByFunc 使用活动实例、CanBeCanceled 和 GAS CancelAbility。Lyra 源码没有本项目这种攻击后摇通知窗口/连段预测取消实现，应沿用已经迁入的 GAS 基础和 Hodge 当前预测链路，而不是另造第二套。

## 6. 后续修复完成

上述为修复前排查记录。现已按方案完成代码、常规构建、原生测试和四种 PIE 模式验证；实际结果与命令见 [修复验证](definition-move-cancel-2026-10-14.md)，调参入口见 [Resonance Skill 第6节](../Guides/resonance-skill-configuration.md)。
