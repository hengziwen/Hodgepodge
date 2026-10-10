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

UENUM(BlueprintType)
enum class EHodgeDashVariant : uint8 { Forward, Backward };

UENUM(BlueprintType)
enum class EHodgeSprintInputState : uint8 { Idle, DashRequested, DashActive, SprintActive, ConsumedUntilRelease };

/** 一个物理按键会话只允许一次 Dash 与一次 Sprint 衔接。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeSprintInputSession
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) int32 SessionId = 0;
	UPROPERTY(BlueprintReadOnly) int32 AvatarGeneration = 0;
	UPROPERTY(BlueprintReadOnly) EHodgeSprintInputState State = EHodgeSprintInputState::Idle;
	UPROPERTY(BlueprintReadOnly) bool bHeld = false;
	UPROPERTY(BlueprintReadOnly) double PressedAt = 0.;
	bool Begin(double Time, int32 Generation);
	void Release();
	void Consume();
	bool Qualifies(double Time, float Threshold) const;
};

/** 同一 Dash 的位移与面向独立，B 只修正位移到角色正后方。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeDashDirections
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) EHodgeDashVariant Variant = EHodgeDashVariant::Forward;
	UPROPERTY(BlueprintReadOnly) FVector MoveDirection = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly) FVector FacingDirection = FVector::ZeroVector;
	static bool Select(FVector Desired, FVector ActorForward, bool bEnableBackward, float BackwardHalfAngle, FHodgeDashDirections& Out);
};

/** 已授权 Sprint 激活键；速度参数来自配置，不接收客户端任意速度。 */
USTRUCT(BlueprintType)
struct FHodgeLocomotionState
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) int32 ActivationKey = 0;
	UPROPERTY(BlueprintReadOnly) int32 Version = 0;
	UPROPERTY(BlueprintReadOnly) bool bSprinting = false;
	UPROPERTY(BlueprintReadOnly) float MaxSpeed = 0.f;
	UPROPERTY(BlueprintReadOnly) float Acceleration = 0.f;
	UPROPERTY(BlueprintReadOnly) float Braking = 0.f;
	UPROPERTY(BlueprintReadOnly) float TurnRate = 0.f;
	UPROPERTY(BlueprintReadOnly) bool bPivotAllowed = false;
	UPROPERTY(BlueprintReadOnly) float SpeedScale = 1.f;
};
