# Definition 攻击技能移动取消

日期：2026-10-14；范围：现有 ComboCoordinated 与 Standalone Definition 技能的通知窗口取消。沿用 Combat、GAS、Hero、CMC 和蒙太奇退出链路，不新增取消组件，也不更改冷却提交语义。

## 1. 背景与目标

AM_Resonance_Skill1 的取消通知有效，但 Combat 移动取消原先仅查询连段 CurrentAbility，因此独立技能无法消费许可。目标是在后摇窗口执行移动输入时结束当前技能，恢复移动，并在联机中保留拥有者预测、服务器确认及拒绝恢复。

Lyra 参考：`Source/LyraGame/AbilitySystem/LyraAbilitySystemComponent.cpp::CancelAbilitiesByFunc` 校验活动 Spec 和实际能力实例；`LyraHeroComponent::Input_Move` 保留相机空间移动输入。Lyra 没有本项目通知窗口与预测蒙太奇恢复流程，继续复用 Hodge 的 BeginPredictedMoveCancel／ResolvePredictedMoveCancel，不另造 GA。

## 2. 需求及验收编号

- MC00：真实 E 激活现有 Standalone 技能；保持当前资产、输入、伤害与转向。
- MC01：窗口前不能取消；窗口前按过但已松开、无输入或低于阈值不能在后摇误取消。
- MC02：提前按住移动在窗口开启后可取消；窗口内新按移动同样可取消。
- MC03：拥有者预测停止蒙太奇并允许移动，服务器确认后两端完整结束技能。
- MC04：冷却仍从 CommitAbility 开始，移动取消不删除／退款冷却；冷却内再次 E 被拒绝。
- MC05：完整 EndAbility 清理标签、命中任务、翼显示、转向和姿势资源；观察者同步退出。
- MC06：服务器拒绝时恢复原蒙太奇，重新收回预测移动权限；同一持续输入不重复请求，松开重按后允许重试。
- MC07：连段仍使用 ComboDefinition 取消配置、授权、输入缓存和节点记忆；五段攻击和移动取消均可运行。
- MC08：无 Combo 配置也能更新独立取消；非法阈值、Avatar 解绑、死亡、旧执行、旧回执均不得误放行。

## 3. 配置与数据

`UHodgeAbilityDefinition` 新增两个仅 Standalone 使用的字段：

- `StandaloneMoveCancelWindowTag`：默认 Status.Attack.Cancel.Move；留空显式关闭移动取消。窗口时间仍以蒙太奇 Hodge GameplayTag 通知状态为唯一来源。
- `StandaloneMoveIntentThreshold`：原始二维输入长度，默认 0.1，合法范围 (0,1]；不是速度。非法／非有限值使配置校验失败。

ComboCoordinated 继续使用 ComboDefinition.MoveCancelWindowTag／MoveIntentThreshold。取消混合仍使用 ExecutionConfig.StopBlendOut；Resonance 当前为 0.1 秒。没有新增时间轴或复制一份 Recovery 配置。

取消请求复用组件 RPC，载荷是 Avatar、SpecHandle、有符号 ActivationPredictionKey、RequestId 与原始二维 Intent。RequestId 是客户端生成并由服务器原样回传的请求身份，不作为取消权限。服务器延后一帧处理时额外携带自己的 ExecutionId；客户端 PendingMoveExecution 验证自己的执行。两端 ExecutionId 各自生成，禁止直接跨网络比较。

## 4. 模块及接口职责

Combat 的 `GetMoveCancelAbility()` 从 ASC.GetAnimatingAbility 查询活动 Definition，验证 Avatar、活动 Spec 和实际实例；不将 Standalone 写入连段 CurrentAbility。`GetMoveCancelPolicy()` 按路由读取配置。`UpdateMoveCancel()` 在连段转换处理后独立检查当前输入，支持持续按住跨越窗口。

`IsMoveCancelPredicted()` 只为匹配 PendingMoveExecution／Spec／Key 的活动执行授予临时移动权限。Hero 和 CMC 原有攻击移动锁保持；预测权限失效后立即恢复保护。

`ServerMoveCancel` 延后一帧等当前通知更新；`ProcessServerMoveCancel` 校验服务器执行身份、窗口、可取消标记、输入禁止标签，以及有限且合法幅度的 RPC Intent。服务器不要求远端 Hero 本地输入缓存存在。取消通过该实例 FinishExecution／EndAbility 完整清理。

`ClientMoveCancelResult` 先匹配 Avatar／Spec／Key／RequestId，再核对本地执行身份；确认则结束预测技能，拒绝则调用原有恢复方法，按服务器位置／时间／倍率恢复蒙太奇。连段输入缓存身份保持原语义。

组件 Shutdown 配对清理待确认请求；更新时遇到结束、换技能、解绑或新执行则作废旧请求。Tick 开启条件只要求有效 ASC 与权限，不依赖 ComboDefinition 是否存在。

## 5. 流程与边界

真实移动输入 → Hero 保存 Intent → Combat 查当前执行 → 检查当前通知窗口 → 拥有者预测退出 → RPC → 服务器等待动画帧后验证 → EndAbility 或拒绝回执 → 客户端确认清理或恢复。

沿用现有 Status.Attack／Controlled 对移动输入的保护，不新增全时长攻击移动锁。不能直接移除 Status.Attack、全局放开 CMC 输入、把 Standalone 改成 Combo 或退款冷却。死亡优先阻止取得候选；技能替换、解绑和结束后旧确认不能结束新动作。

输入保持有限，二维幅度上限沿用原 RPC 2.01 的平方范围，允许对角键盘输入；独立取消阈值必须为正；连段继续允许零阈值，沿用 Hero 对非零输入的判断。不新增组件 Tick 或全 Ability 列表遍历，只查询当前 ASC 动画能力。

## 6. 验证与范围

原生测试覆盖路由策略、配置关闭／非法值、窗口前拒绝、边界阈值、非法输入、Avatar／执行身份、输入阻止、死亡与过期确认；既有动作请求和连段测试作回归。

真实 PIE 使用当前技能、E 和移动输入，覆盖单人、Listen、PIE Dedicated 及 Listen 100ms 延迟；网络拒绝通过服务器 PIE Actor 上的不复制输入禁止标签构造，调用既有 GAS 接口并配对移除，不修改生产资产或添加产品硬编码开关。所有断言标注 MC 编号，实际结果见本轮验证报告。

本次不包含技能动画重做、冷却重生策略、独立 Server Target、Cook／打包和手柄适配。动画视觉手感仍需用户实际游玩确认；自动化验证的是时序、状态和资源归属。
