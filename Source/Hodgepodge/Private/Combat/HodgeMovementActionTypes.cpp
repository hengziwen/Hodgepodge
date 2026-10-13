/** @file HodgeMovementActionTypes.cpp
 * @brief 实现Dash/Sprint动作数据相关的类型、接口与现有处理流程。
 */
#include "Combat/HodgeMovementActionTypes.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeMovementActionTypes)

namespace HodgeMovementTags
{
	UE_DEFINE_GAMEPLAY_TAG(Dashing, "State.Movement.Dashing");
	UE_DEFINE_GAMEPLAY_TAG(DashPreparing, "State.Movement.DashPreparing");
	UE_DEFINE_GAMEPLAY_TAG(Sprinting, "State.Movement.Sprinting");
	UE_DEFINE_GAMEPLAY_TAG(SprintAbility, "Ability.Type.Action.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(PerfectDodge, "Event.Combat.PerfectDodge");
	UE_DEFINE_GAMEPLAY_TAG(SprintCue, "GameplayCue.Character.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(PerfectDodgeCue, "GameplayCue.Character.PerfectDodge");
}

/** 开始本轮处理。 */
bool FHodgeSprintInputSession::Begin(double Time, int32 Generation)
{
	if (bHeld || Generation <= 0 || !FMath::IsFinite(Time)) { return false; }
	SessionId = SessionId == MAX_int32 ? 1 : SessionId + 1;
	AvatarGeneration = Generation; PressedAt = Time; bHeld = true; State = EHodgeSprintInputState::DashRequested;
	return true;
}

/** 释放当前对象持有的资源或句柄。 */
void FHodgeSprintInputSession::Release() { bHeld = false; State = EHodgeSprintInputState::Idle; }
/** 消费当前缓存或事件。 */
void FHodgeSprintInputSession::Consume() { State = bHeld ? EHodgeSprintInputState::ConsumedUntilRelease : EHodgeSprintInputState::Idle; }

/** 检查当前状态是否满足操作资格。 */
bool FHodgeSprintInputSession::Qualifies(double Time, float Threshold) const
{
	return bHeld && State == EHodgeSprintInputState::DashActive && FMath::IsFinite(Time) &&
		FMath::IsFinite(Threshold) && Threshold >= 0.f && Time - PressedAt >= Threshold;
}

/** 按当前约束选择所需候选。 */
bool FHodgeDashDirections::Select(FVector Desired, FVector ActorForward, bool bEnableBackward, float BackwardHalfAngle, FHodgeDashDirections& Out)
{
	Out = {};
	if (Desired.ContainsNaN() || ActorForward.ContainsNaN()) { return false; }
	FVector Forward;
	if (!HodgeFacing::NormalizeDirection(ActorForward, Forward) || !FMath::IsFinite(BackwardHalfAngle) ||
		BackwardHalfAngle < 0.f || BackwardHalfAngle >= 90.f) { return false; }
	if (Desired.IsNearlyZero()) { Desired = Forward; }
	FVector Move;
	if (!HodgeFacing::NormalizeDirection(Desired, Move)) { return false; }
	const float Cosine = FMath::Cos(FMath::DegreesToRadians(BackwardHalfAngle));
	const bool bBack = bEnableBackward && FVector::DotProduct(Move, -Forward) + KINDA_SMALL_NUMBER >= Cosine;
	Out.Variant = bBack ? EHodgeDashVariant::Backward : EHodgeDashVariant::Forward;
	Out.MoveDirection = bBack ? -Forward : Move;
	Out.FacingDirection = bBack ? Forward : Move;
	return true;
}
