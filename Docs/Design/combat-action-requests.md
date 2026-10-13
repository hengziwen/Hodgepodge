# 玩家与 AI 共享战斗请求层

日期：2026-10-11。适用 UE 5.5.4。实施基于 Enemy AI v0.1 和既有玩家 Combo／GAS 链路。验证结果由本轮报告单独记录，设计取舍不自动等于已通过网络验证。

## 1. 目标与范围

玩家输入和 AI 决策提交同一类战斗请求，共享准备检查、连段编排和执行结果；实际激活、预测、消耗、互斥、蒙太奇、命中和伤害仍由既有 GAS／Combat 执行。

本轮覆盖 Standalone Definition 技能、Combo 意图、玩家 InputTag 到 IntentTag 的适配、当前敌人 BTTask 的两种请求模式及完整请求生命周期。不将普通怪物接入 Enhanced Input／PlayerController，不改变 ASC 所有权。

普通非 Definition GA 继续沿用 ASC 输入策略。Death／受击的事件入口、Dash／Sprint 的按住会话、完整 AI 连段选择策略、仇恨、动作队列、目标吸附和 Experience 热切换不在本轮范围。新增的请求接口是现有 CombatComponent 的功能，不新增业务模块或第二套能力系统。

## 2. 输入、处理和输出

输入为 AbilityTag 或 IntentTag，二者由明确的 RequestType 区分。AbilityTag 指向已授予的 Standalone Definition；IntentTag 提交给当前 ComboDefinition，不允许 AI 指定下一段后绕过 Transition。

玩家普通攻击：InputTag → 既有 InputBindings → SubmitCombatIntent。玩家独立 Definition：ASC.ProcessAbilityInput → RequestStandaloneAbility。AI：行为树取得有效目标、停止追击并准备朝向 → 选择独立技能或意图请求。

共享处理：核对当前 ASC／Avatar 和战斗准备 → 解析已授予技能或连段图 → 遵守标签／窗口／缓存／授权 → GAS 原有激活入口 → 监听真实结束。目标、距离和朝向仍由适配器处理；请求不会保证攻击命中目标。

输出是带 RequestId 的状态快照与变化通知。Queued 仅表示等待，Started 表示本地执行已开始，不表示玩家预测已经获得服务器确认。Completed／Cancelled／Expired／Rejected 是终结状态，不能被迟到通知恢复为活动状态。

## 3. 模块与接口

### 3.1 数据类型

`Combat/HodgeCombatActionRequest.h` 定义：

```cpp
enum class EHodgeCombatActionRequestType : uint8
{ StandaloneAbility, ComboIntent };

enum class EHodgeCombatActionRequestStatus : uint8
{ Queued, Started, Completed, Cancelled, Expired, Rejected };

struct FHodgeCombatActionRequestState
{
    FGuid RequestId;
    EHodgeCombatActionRequestType Type;
    FGameplayTag ActionTag;
    EHodgeCombatActionRequestStatus Status;
    FGameplayAbilitySpecHandle AbilityHandle;
    FGuid ExecutionId;
    FString Reason;
};
```

### 3.2 CombatComponent

```cpp
FHodgeCombatActionRequestState RequestStandaloneAbility(FGameplayTag AbilityTag);
FHodgeCombatActionRequestState SubmitCombatIntent(FGameplayTag IntentTag);
bool GetCombatActionRequest(FGuid RequestId, FHodgeCombatActionRequestState& OutState) const;
bool CancelCombatActionRequest(FGuid RequestId);
bool IsCombatReadyForActions() const;
```

Blueprint 可以直接调用提交、查询和取消；OnCombatActionRequestChanged 为 BlueprintAssignable 动态通知，OnActionRequestChanged 为原生通知。调用方先绑定通知再提交，并检查返回快照，补偿同步结束／拒绝时尚未得到 RequestId 的情况。

请求记录保存状态、弱 Ability 实例和激活中标志。与原 Combo 单槽缓存关联的 BufferedActionRequest 表示当前待消费请求；成功选择 Transition 时消费缓存，不把本次请求误报为取消。

独立请求不允许激活 ComboCoordinated Definition。连段意图使用原 SelectTransition／PrepareTransition／InternalTryActivateAbility，保留窗口、权限、服务器验证和记忆逻辑。映射配置中的意图或图中存在的意图可提交；没有合法当前边时等待，缓存过期后明确 Expired。

### 3.3 ASC 与 Definition GA

ASC 的 Pressed／Held／Released 策略和 PlayerController.PostProcessInput 保留。独立 Definition 的待激活 Spec 转交共享请求入口，非 Definition GA 保持原 TryActivateAbility。

Definition.CanActivateAbility 使用共享 IsCombatReadyForActions，随后继续执行原 GAS、ActivationGroup、受击及授权检查。Hero 准备来源为 PlayerState 属性协调，敌人准备来源为 EnemyInit.GameplayReady 和当前依赖；生命为零或死亡时拒绝普通战斗技能。

Death 等生命周期 GA 不经过此 Definition 战斗门控。当前某些技能允许受击期间使用的规则仍由其 GA 配置决定，不以共享请求层覆盖全部技能策略。

### 3.4 行为树任务

`UHodgeBTTask_ExecuteAbility` 保留 AbilityTag、TargetActor、Timeout。新增 RequestType 和 IntentTag；原资产默认 StandaloneAbility，保持兼容。

- 独立模式填写 AbilityTag，提交 RequestStandaloneAbility。
- 连段模式填写 IntentTag，当前 PawnData 必须有合法 ComboDefinition，技能包必须授予图中引用的 ComboCoordinated Definition。
- Task 在 Queued／Started 期间等待；Completed 成功，其他终结状态失败。
- Abort／超时先解绑通知，再取消自己的 RequestId，释放自身朝向句柄。

Task 保留 AttemptId 和 PossessGeneration；共享组件管理 Spec／实例／ExecutionId。旧 Task 回调、旧请求取消不能影响另一轮动作。

## 4. 状态与身份规则

请求可同步拒绝或完成，不保证调用者一定观察到独立的 Started 通知。例如动画播放失败会在提交返回前进入 Cancelled。调用者不能仅用 TryActivateAbility 的布尔返回值猜测最终执行结果。

监听 ASC.OnAbilityEnded 必须在激活前建立。激活期间绑定 Spec，实际开始后记录实例和 ExecutionId。Definition 正常结束会先清空 ExecutionId，允许这一正常终结；若实例已有有效的新 ExecutionId，则旧请求不得消费新一轮结束。

缓存被新输入替换：旧请求 Cancelled。清理输入／解绑／组件卸载：关闭所属请求。缓存过期：Expired。未知标签、未授予或重复身份、错误路由、已有活动实例、初始化未完成或 GAS 拒绝：Rejected，并携带 Reason。

通知按 FIFO 派发。监听者在 Started 中同步取消动作时，所有监听者仍按 Started → Cancelled 收到事件。通知重入最多处理 64 项，超限记录诊断，状态查询仍以记录为准。提交期间再次提交会立即拒绝，防止清理／通知回调重入破坏正在建立的请求。

## 5. 网络与取消边界

RequestId 是本机请求跟踪身份，不另建网络 RPC 或复制请求账本。玩家继续使用既有 GAS PredictionKey 和 Combo Payload 关联服务器；普通怪物在服务器请求技能，模拟客户端通过既有蒙太奇／状态／Cue 复制表现。

仅服务器或当前本地控制方可提交。客户端模拟代理不能把表现状态当成完整授予记录使用。不得以请求来源枚举代替服务器权限检查。

取消只作用于本请求的 Spec、实例和执行身份，且尊重 CanBeCanceled。客户端可取消本地缓存和独立预测请求；已开始的 Combo 请求不提供绕过服务器确认的直接取消，仍使用既有移动取消等协议。服务器 AI 可取消自己本次 Combo 请求。

解绑时关闭跟踪并清理可取消的所属执行，不强行取消 Death 或其他不可取消技能。终结记录只能保留有限历史；查询已裁剪 RequestId 返回 false，不能将其当作新执行。

## 6. 依赖与兼容性

沿用 Hodgepodge Runtime 内的 GAS、GameplayTags、PawnExtension、EnemyInit、PlayerState 属性协调和 CombatDefinition。编辑器作者／测试工具位于既有 HodgeAbilityEditor，无新 Runtime → Editor 依赖。

原 GiveAbility／AbilitySet 授予归属不变。原 InputPressed 的 bool 仍表示输入已消费；新调用方用状态区分缓存与执行。InputBindings 中合法但当前没有边的输入继续保留单槽缓存语义。

不更名现有类、Tag、Slot 或资产，不修改玩家 Combo 表。现有 Standalone BT 资产无需填写新字段；新增 AI Combo 配置使用独立验证资产，不覆盖正式 Guardian 的 PawnData。

## 7. 性能与验证约束

所有提交与通知在 Game Thread 执行。每个 CombatComponent 最多 16 个活动／待处理请求、64 个历史记录；裁剪只移除终结记录。状态无持续复制，不新增普通 Standalone 请求 Tick；Combo 缓存使用现有组件 Tick。匹配结束事件扫描至多 64 项。

原生测试覆盖独立激活／完成／取消、重复拒绝、旧取消隔离、玩家映射、AI 无键位映射的意图、窗口缓存与替换、过期、同步结束、回调顺序、撤销、准备失败、历史上限与解绑。回归保留 Enemy、Combo、属性、装备、受击、旋转和移动组。

PIE 验证当前独立 AI 与新的 Combo AI；Listen Server 验证远端玩家通过共享意图进行本地预测、服务器授权和伤害，以及怪物攻击／受击／死亡复制。构建、原生、蓝图、PIE 和网络结论分别记录，不推断 Dedicated Server／Cook／打包结果。

## 8. 非自动混出死亡资源的兼容修复

本轮回归发现 AM_Death 关闭自动混出时，PlayMontageAndWait 不会收到末帧完成事件。EnemyDeath 保留原事件与 Health 链路，仅在此类资源上每 0.05 秒检查实际蒙太奇进度；实例已经停止在末帧或丢失时结束，退出时撤销 Timer。不以固定 Delay 猜测动作结束，不改写用户的资源设置。
