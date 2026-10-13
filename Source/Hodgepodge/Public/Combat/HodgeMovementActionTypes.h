/** @file HodgeMovementActionTypes.h
 * @brief 声明Dash/Sprint动作数据相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "Combat/HodgeCharacterFacingTypes.h"
#include "NativeGameplayTags.h"
#include "HodgeMovementActionTypes.generated.h"

namespace HodgeMovementTags
{
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dashing);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(DashPreparing);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sprinting);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SprintAbility);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(PerfectDodge);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SprintCue);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(PerfectDodgeCue);
}

/** 定义Dash冲刺Variant的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeDashVariant : uint8 { /* 前向选项。 */ Forward, /* 后向选项。 */ Backward };

/** 定义Sprint奔跑输入状态的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeSprintInputState : uint8 { /* Idle选项。 */ Idle, /* Dash冲刺Requested选项。 */ DashRequested, /* Dash冲刺活动选项。 */ DashActive, /* Sprint奔跑活动选项。 */ SprintActive, /* ConsumedUntil释放选项。 */ ConsumedUntilRelease };

/** 一个物理按键会话只允许一次 Dash 与一次 Sprint 衔接。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeSprintInputSession
{
	GENERATED_BODY()
	// 本次会话的唯一身份。
	UPROPERTY(BlueprintReadOnly) int32 SessionId = 0;
	// 用于拒绝旧Pawn回调的初始化代号。
	UPROPERTY(BlueprintReadOnly) int32 AvatarGeneration = 0;
	// 当前状态数据。
	UPROPERTY(BlueprintReadOnly) EHodgeSprintInputState State = EHodgeSprintInputState::Idle;
	// 控制是否按住的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bHeld = false;
	// 按下时刻的数值或阈值。
	UPROPERTY(BlueprintReadOnly) double PressedAt = 0.;
	/** 开始本轮处理。
	 * @param Time 当前处理使用的时间值。
	 * @param Generation 初始化代，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool Begin(double Time, int32 Generation);
	/** 释放当前对象持有的资源或句柄。 */
	void Release();
	/** 消费当前缓存或事件。 */
	void Consume();
	/** 检查当前状态是否满足操作资格。
	 * @param Time 当前处理使用的时间值。
	 * @param Threshold 阈值，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool Qualifies(double Time, float Threshold) const;
};

/** 同一 Dash 的位移与面向独立，B 只修正位移到角色正后方。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeDashDirections
{
	GENERATED_BODY()
	// Variant的枚举配置或当前取值。
	UPROPERTY(BlueprintReadOnly) EHodgeDashVariant Variant = EHodgeDashVariant::Forward;
	// 移动方向的几何或变换数据。
	UPROPERTY(BlueprintReadOnly) FVector MoveDirection = FVector::ZeroVector;
	// 朝向方向的几何或变换数据。
	UPROPERTY(BlueprintReadOnly) FVector FacingDirection = FVector::ZeroVector;
	/** 按当前约束选择所需候选。
	 * @param Desired 期望，由调用方传入。
	 * @param ActorForward Actor对象前向，由调用方传入。
	 * @param bEnableBackward 是否启用后向的控制标记。
	 * @param BackwardHalfAngle 后向Half角度，由调用方传入。
	 * @param Out 输出输出。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	static bool Select(FVector Desired, FVector ActorForward, bool bEnableBackward, float BackwardHalfAngle, FHodgeDashDirections& Out);
};

/** 已授权 Sprint 激活键；速度参数来自配置，不接收客户端任意速度。 */
USTRUCT(BlueprintType)
struct FHodgeLocomotionState
{
	GENERATED_BODY()
	// 本次激活的关联键。
	UPROPERTY(BlueprintReadOnly) int32 ActivationKey = 0;
	// 版本的数值或阈值。
	UPROPERTY(BlueprintReadOnly) int32 Version = 0;
	// 控制是否Sprinting的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bSprinting = false;
	// 最大速度的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float MaxSpeed = 0.f;
	// 加速度的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float Acceleration = 0.f;
	// Braking的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float Braking = 0.f;
	// 转向速率的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float TurnRate = 0.f;
	// 控制是否掉头允许的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bPivotAllowed = false;
	// 速度缩放的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float SpeedScale = 1.f;
};
