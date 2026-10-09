# 角色朝向模式接口与配置

适用 UE 5.5.4。实现与实际测试范围见[验证报告](../Validation/character-facing-implementation-2026-10-10.md)；完整锁定目标服务和锁定 CameraMode 不在本轮实现中。

## 1. 默认与预留模式

Main 主角默认 Movement 驱动、FreeDirectional 风格：角色朝运动方向旋转，相机 Look 保留。Controller 驱动选择 ReservedStrafe，消费已有有效控制 Yaw，不自动选择敌人。

旋转组件位于当前 Pawn，通过 PawnExtension 绑定现有 ASC。不要分别修改 Actor 的 ControllerYaw 和 CMC 的 OrientToMovement；调用请求接口，由组件解析并应用。驱动开关互斥，冻结由原 Tag 约束叠加。

## 2. 基础请求

`AcquireBaseFacingMode(Driver, Source)` 返回带 AvatarGeneration 的句柄。Driver 允许 Movement 或 Controller；Source 为当前 Pawn、其拥有的对象或当前活动 GA。客户端动作改向另有活动 GA 验证，不能从任意 UObject 改向。

Controller 模式要求 Controller 存在，且组件默认配置 bAllowControllerFacingRequests 允许。默认开放的是模式能力，不是完整锁定操作；后续功能应通过自己的授权上下文调用。

保存句柄并在使用结束调用 `ReleaseFacingRequest`。同一来源可申请多份请求，后申请的同类请求获选；释放一份后重新解析剩余来源。不要每帧申请，不恢复缓存的布尔值。

`GetResolvedState`／`GetFacingPresentationSnapshot` 读取有效结果；`OnFacingStateChanged` 是本地通知，网络使用组件复制及移动协议，不将 Delegate 当 RPC。

## 3. 动作请求

`RequestActionFacing(WorldDirection, Source, ExecutionId, bInstantAtStart)` 的方向必须为合法水平向量，执行 ID 有效。默认平滑准备；只有明确的动作策略才传入瞬时定向 True。

Source 为活动 GA 时，客户端 RPC 使用 AbilitySpecHandle／PredictionKey 验证；Definition 的本地 GUID 不跨端共享，服务器映射自己的执行身份。旧执行、失去 Avatar 或当前旋转冻结不能获得新改向权限。

句柄创建不表示已应用。通过 `GetFacingRequestStatus` 或 `OnActionFacingApplied` 等待 Applied，再执行依赖起手方向的运动／锁定；Pending、Blocked、Rejected、Released、Invalid 均要处理。

GA 结束会释放自己拥有的请求。需要提前结束覆盖时显式释放；其他来源的旋转锁、减速和受击不会因此消失。新的基础模式可以在动作期间更新，动作结束恢复最新状态。

## 4. 输入语义

`GetMoveIntent()` 保留原始二维输入；`HasMoveIntent()` 仍用于移动取消，不检查速度。

`GetMoveIntentSnapshot()` 包含原始输入、采样控制 Yaw、世界方向、幅值、时间和 AvatarGeneration；`GetWorldMoveIntent()` 返回世界水平单位方向。无输入或无有效 Controller 时不制造前向移动。

OnMoveIntentChanged 只表示有／无输入翻转，不承担持续方向事件。攻击阻止实际移动之前仍需采集意图；输入结束或 UI／生命周期取消需走现有清理。

## 5. 动画层

正式资产是 `/Game/Main/Character/Hero/Anim/ABP_Pover_Base` 和 `/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase`。

原方向数据与资源保留。资源入口以 bUseStrafeLocomotion 区分 Free 的 Forward 和 ReservedStrafe 的原方向；OrientationWarping 消费 StrafeLocomotionWeight。序列切换继续使用已有惯性混合。

FacingModeBlendTime 默认 0.12 秒，控制权重恢复；RootYawOffset 在不可见权重归零后才重置。FullBody、轻反馈 Group、脚 IK 与现有抑制保护保留。当前 EnablePivot=False 不因本轮自动启用。

禁止将 LocalVelocityDirection 全局写死 Forward、运行时替换整个主动画实例或清空全部蒙太奇。

## 6. 参数与异常

组件 RecoveryTurnRate 默认 360°/s，ActionTurnRate 默认 720°/s；有限且大于零，编辑器验证拒绝无效值，运行时保留安全回退。

最多 32 个活动请求，结束结果和服务器已授权历史保持有限容量。重复释放无副作用；旧初始化代句柄无效；过时 RPC 不能改变新的执行。解绑恢复原驱动开关，保留共享 ASC 的其他 Tag。

模拟代理只使用服务器结果。动态模式进入 SavedMove 和自定义移动数据；未知序号不是授权，服务器使用当前合法状态。回放结束重新应用正常模式，不保留历史开关。

测试入口 QueueFacingMode／QueueActionFacing 延迟到实际世界 Tick；SetFacingValidationControllerPermission 仅允许编辑器 PIE 的 Authority Pawn，用于拒绝用例，不是 Runtime 游戏接口。
