# 客户端普攻偶发拒绝调查 · 2026-10-08

## 结论与边界

已确认“发生一次预测拒绝后，客户端可能连续预测第一段并被服务器再次拒绝”的恢复缺口，并用受控网络测试复现。用户日志中第一次 `GA_Attack_5` 被拒绝的具体条件尚不能确定：原日志没有记录该请求的服务器来源节点、执行键、窗口及拒绝分支。

本次仅调查和运行临时 PIE 测试，没有修改 C++、Montage、连段表、配置或正式 UI 资产。UMG 父类精简方案仍待审查。

## 用户原始事件

原日志已复制到 `Saved/ComboIncident-20261008/Hodgepodge-user.log`，约 702 KiB，避免后续编辑器日志追加／轮转影响原证据。日志时间为 UTC，北京时间加 8 小时。

- 10:14:03.518：PredictionKey 109，`GA_Attack_5` 被拒绝。
- 10:14:03.734、03.950、04.150、04.600：PredictionKey 110～113，`GA_Attack_1` 又被拒绝四次。
- 10:14:07.418 及之后继续有普攻输入，原日志没有再出现激活拒绝。

这些记录证明五次拒绝集中出现，不能仅解释成“按键没触发”。`GA_Attack_5` 当前使用 AM_Attack05_Montage；不要把资产编号直接解释成五段顺序中的最后一段。

## 恢复缺口的代码路径

1. [ASC 的拒绝入口](../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp) 调用 GAS 原生拒绝处理，结束与失败预测键匹配的客户端能力，然后调用 CombatComponent.HandlePredictionRejected。
2. [HandlePredictionRejected／ServerSynchronizeComboMemory](../../Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp) 设置客户端等待校正，并让服务器只返回 ComboMemory。
3. 服务器的上一段可能仍在执行。这时 ComboMemory.Node 是上一段，但 ExpiresAt=0；0 在这里表示“执行中，尚未开始结束后的保留计时”。
4. ClientCorrectComboMemory 清除等待标记并复制此记忆。客户端已因拒绝而没有 CurrentAbility。
5. 同文件 GetRememberedComboTag 仅在客户端有 CurrentAbility 或 ExpiresAt 大于当前时间时承认记忆。服务器发来的执行中状态因此在空闲客户端被判为无效。
6. TransitionSourceNode 回到客户端的 Combo.Entry，ExecutionKey 也按无记忆返回 0；后续点击预测 GA_Attack_1。
7. 服务器 ValidateServerRequestIdentity 仍要求来源为自己的当前动作节点及执行键。客户端请求身份不一致，未进入正常的窗口等待就被拒绝。

因此，当前恢复消息没有表达“服务器仍有当前执行、客户端已经没有预测执行”这一状态。暂停预测的标记又过早解除，使一次拒绝扩大成连续回退。

只有窗口尚未满足而身份正确的请求，才会进入现有的权威窗口等待。扩大 InputBufferSeconds 无法解决这个来源身份不一致的问题。

## 受控复现

使用正式 ThirdPersonMap、两人 Listen PIE、现有 GA／Montage 和真实 EnhancedInput 客户端普攻。

为单独验证恢复路径，测试在第一段两端已经运行且接段窗口已开时，短暂向服务器 Pawn 的 ASC 添加不复制的 Gameplay.AbilityInputBlocked。客户端不知道这个临时阻断，预测第二段；服务器合法拒绝一次。约 0.12 秒后移除该阻断，再持续点击客户端普攻。

这次人为条件只用于制造一次拒绝，不代表用户原始事件也是该 Tag 导致。

实测结果：

- 首次拒绝 GA_Attack_2，随后又连续拒绝 GA_Attack_1 十二次。
- 移除临时阻断后，某帧客户端为空闲、节点为 Combo.Entry、记忆为空；服务器仍在 GA_Attack_1、节点 Combo.Light.01，Montage 位置约 0.881 秒，接段／移动取消窗口均已打开。
- 同样存在“服务器当前动作、客户端无当前动作”的状态约 120 个采样帧。
- 此后服务器上一段结束，正常结束后的记忆可在客户端使用，约 2.25 秒后两端重新接上第二段。

数据与脚本：`Saved/ComboIncident-20261008/controlled-refusal.json`、`controlled-refusal-session.log`、`refusal-summary.json`、`test-controlled-refusal.py`。结果 JSON 的 error=null 表示测试采集完成，不能解释为恢复逻辑没有问题；上述异常是本次采集的目的。

另用 0.18 秒间隔持续点击，未注入阻断，分别在 0ms 和 `NetEmulation.PktLag 100` 下各采集约 18 秒（881／862 个采样帧），没有新增 ClientActivateAbilityFailed。记录为 `rapid-zero.json`、`rapid-lag100.json`。这两次没有复现首次拒绝，不能据此排除低频时间边界、丢包／抖动或其他中断条件。

## 首次拒绝仍需定位

原事件的来源节点／执行键变化、实际窗口不足、窗口等待超时以及后续 GAS 激活失败都可能走到客户端同一拒绝消息。目前各分支没有足够的服务端诊断，不能从原文件选定其中一项。

当前资产只读检查确认：InputBufferSeconds=0.3、ComboRetentionSeconds=1；五个普攻 Montage 都有接段和移动取消窗口，配置 PlayRate=1。这能排除当前资产整体没有窗口的情况，但不能还原事件瞬间服务器账本。

## 修复方向与验收建议

- 校正消息必须区分“权威动作仍执行”“动作已结束且记忆有效”“入口／记忆已过期”，并携带匹配的 Avatar／执行身份。
- 客户端不能把权威执行中状态直接按空闲记忆解释，也不能在尚未建立一致来源时重新开放从入口预测。
- 过期拒绝／校正不能覆盖后续已确认的执行；恢复仍遵守服务器真实窗口和既有缓存期限，不提前授予窗口 Tag。
- 补服务端拒绝分支诊断：预测键、请求目标、请求与权威来源节点／执行键、CurrentAbility、窗口、缓存等待／超时、相关阻断状态及 GAS 失败标签。先记录事实，再判断首次拒绝是否来自时间边界。
- 回归需覆盖一次拒绝后的恢复、执行中／结束后／过期三种校正、快速点击、移动取消与再次攻击、连续预测和过期回复。此前连续成功的连段测试未覆盖受控拒绝恢复。

本次没有实现上述修复，也未执行新构建、蓝图重编译、Cook 或打包。后续复现数据与临时环境恢复结果保存在同一 Saved 目录；正式改动需要另行按项目约定构建验证。
