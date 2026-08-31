#pragma once

#include "Animation/AnimInstance.h"
#include "Engine/World.h"
#include "ALS/State/HodgeAlsControlRigInput.h"
#include "ALS/State/HodgeAlsCrouchingState.h"
#include "ALS/State/HodgeAlsDynamicTransitionsState.h"
#include "ALS/State/HodgeAlsFeetState.h"
#include "ALS/State/HodgeAlsGroundedState.h"
#include "ALS/State/HodgeAlsInAirState.h"
#include "ALS/State/HodgeAlsLayeringState.h"
#include "ALS/State/HodgeAlsLeanState.h"
#include "ALS/State/HodgeAlsLocomotionAnimationState.h"
#include "ALS/State/HodgeAlsLookState.h"
#include "ALS/State/HodgeAlsMovementBaseState.h"
#include "ALS/State/HodgeAlsPoseState.h"
#include "ALS/State/HodgeAlsRagdollingAnimationState.h"
#include "ALS/State/HodgeAlsRotateInPlaceState.h"
#include "ALS/State/HodgeAlsSpineState.h"
#include "ALS/State/HodgeAlsStandingState.h"
#include "ALS/State/HodgeAlsTransitionsState.h"
#include "ALS/State/HodgeAlsTurnInPlaceState.h"
#include "ALS/State/HodgeAlsViewAnimationState.h"
#include "ALS/Utility/HodgeAlsGameplayTags.h"
#include "HodgeAlsAnimationInstance.generated.h"

class UHodgeAlsLinkedAnimationInstance;
class AHodgeAlsCharacter;

UCLASS()
class HODGEPODGE_API UHodgeAlsAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()

	friend UHodgeAlsLinkedAnimationInstance;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UHodgeAlsAnimationInstanceSettings> Settings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	TObjectPtr<AHodgeAlsCharacter> Character;

	// Used to indicate that the animation instance has not been updated for a long time
	// and its current state may not be correct (such as foot location used in foot lock).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	uint8 bPendingUpdate : 1 {true};

	// Time of the last teleportation event.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (ClampMin = 0))
	double TeleportedTime{0.0f};

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	uint8 bDisplayDebugTraces : 1 {false};

	mutable TArray<TFunction<void()>> DisplayDebugTracesQueue;
#endif

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag ViewMode{HodgeAlsViewModeTags::ThirdPerson};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag LocomotionMode{HodgeAlsLocomotionModeTags::Grounded};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag RotationMode{HodgeAlsRotationModeTags::ViewDirection};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag Stance{HodgeAlsStanceTags::Standing};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag Gait{HodgeAlsGaitTags::Walking};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag OverlayMode{HodgeAlsOverlayModeTags::Default};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag LocomotionAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag GroundedEntryMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsMovementBaseState MovementBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsLayeringState LayeringState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsPoseState PoseState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsViewAnimationState ViewState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsSpineState SpineState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsLookState LookState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsLocomotionAnimationState LocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsLeanState LeanState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsGroundedState GroundedState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsStandingState StandingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsCrouchingState CrouchingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsInAirState InAirState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsFeetState FeetState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsTransitionsState TransitionsState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsDynamicTransitionsState DynamicTransitionsState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsRotateInPlaceState RotateInPlaceState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsTurnInPlaceState TurnInPlaceState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FHodgeAlsRagdollingAnimationState RagdollingState;

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaTime) override;

	virtual void NativePostUpdateAnimation();

protected:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;

	// Core

protected:
	UFUNCTION(BlueprintPure, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe, ReturnDisplayName = "Setting"))
	UHodgeAlsAnimationInstanceSettings* GetSettingsUnsafe() const;

	UFUNCTION(BlueprintPure, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe, ReturnDisplayName = "Rig Input"))
	FHodgeAlsControlRigInput GetControlRigInput() const;

public:
	void MarkPendingUpdate();

	void MarkTeleported();

private:
	void RefreshMovementBaseOnGameThread();

	void RefreshLayering();

	void RefreshPose();

	// View

private:
	void RefreshViewOnGameThread();

	void RefreshView(float DeltaTime);

public:
	virtual bool IsSpineRotationAllowed();

private:
	void RefreshSpine(float SpineBlendAmount, float DeltaTime);

protected:
	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeLook();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshLook();

	// Locomotion

private:
	void RefreshLocomotionOnGameThread();

protected:
	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeLean();

	// Grounded

public:
	void SetGroundedEntryMode(const FGameplayTag& NewGroundedEntryMode);

protected:
	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void ResetGroundedEntryMode();

protected:
	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeGrounded();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshGrounded();

private:
	FVector3f GetRelativeVelocity() const;

	FVector2f GetRelativeAccelerationAmount() const;

	void RefreshVelocityBlend();

	void RefreshGroundedLean();

protected:
	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshGroundedMovement();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void SetHipsDirection(EHodgeAlsHipsDirection NewHipsDirection);

private:
	void RefreshMovementDirection(float ViewRelativeVelocityYawAngle);

	void RefreshRotationYawOffsets(float ViewRelativeVelocityYawAngle);

protected:
	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeStandingMovement();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshStandingMovement();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void ActivatePivot();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void ResetPivot();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshCrouchingMovement();

	// In Air

public:
	void Jump();

private:
	void RefreshInAirOnGameThread();

protected:
	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshInAir();

	void RefreshGroundPrediction();

	void RefreshInAirLean();

	// Feet

private:
	void RefreshFeetOnGameThread();

	void RefreshFeet(float DeltaTime);

	void RefreshFoot(FHodgeAlsFootState& FootState, const FName& IkCurveName, const FName& LockCurveName,
	                 const FTransform& ComponentTransformInverse, float DeltaTime) const;

	void ProcessFootLockTeleport(float IkAmount, FHodgeAlsFootState& FootState) const;

	void ProcessFootLockBaseChange(float IkAmount, FHodgeAlsFootState& FootState, const FTransform& ComponentTransformInverse) const;

	void RefreshFootLock(float IkAmount, FHodgeAlsFootState& FootState, const FName& LockCurveName,
	                     const FTransform& ComponentTransformInverse, float DeltaTime) const;

	// Transitions

public:
	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void PlayQuickStopAnimation();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void PlayTransitionAnimation(UAnimSequenceBase* Sequence, float BlendInDuration = 0.2f, float BlendOutDuration = 0.2f,
	                             float PlayRate = 1.0f, float StartTime = 0.0f, bool bFromStandingIdleOnly = false);

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void PlayTransitionLeftAnimation(float BlendInDuration = 0.2f, float BlendOutDuration = 0.2f, float PlayRate = 1.0f,
	                                 float StartTime = 0.0f, bool bFromStandingIdleOnly = false);

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void PlayTransitionRightAnimation(float BlendInDuration = 0.2f, float BlendOutDuration = 0.2f, float PlayRate = 1.0f,
	                                  float StartTime = 0.0f, bool bFromStandingIdleOnly = false);

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void StopTransitionAndTurnInPlaceAnimations(float BlendOutDuration = 0.2f);

protected:
	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshDynamicTransitions();

private:
	void RefreshTransitions();

	void PlayQueuedTransitionAnimation();

	void StopQueuedTransitionAndTurnInPlaceAnimations();

	// Rotate In Place

public:
	virtual bool IsRotateInPlaceAllowed();

protected:
	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshRotateInPlace();

	// Turn In Place

public:
	virtual bool IsTurnInPlaceAllowed();

protected:
	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeTurnInPlace();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshTurnInPlace();

private:
	void PlayQueuedTurnInPlaceAnimation();

	// Ragdolling

private:
	void RefreshRagdollingOnGameThread();

public:
	FPoseSnapshot& SnapshotFinalRagdollPose();

	// Utility

public:
	float GetCurveValueClamped01(const FName& CurveName) const;
};

inline UHodgeAlsAnimationInstanceSettings* UHodgeAlsAnimationInstance::GetSettingsUnsafe() const
{
	return Settings;
}

inline void UHodgeAlsAnimationInstance::MarkPendingUpdate()
{
	bPendingUpdate |= true;
}

inline void UHodgeAlsAnimationInstance::MarkTeleported()
{
	TeleportedTime = GetWorld()->GetTimeSeconds();
}

inline void UHodgeAlsAnimationInstance::SetGroundedEntryMode(const FGameplayTag& NewGroundedEntryMode)
{
	GroundedEntryMode = NewGroundedEntryMode;
}

inline void UHodgeAlsAnimationInstance::ResetGroundedEntryMode()
{
	GroundedEntryMode = FGameplayTag::EmptyTag;
}

inline void UHodgeAlsAnimationInstance::SetHipsDirection(const EHodgeAlsHipsDirection NewHipsDirection)
{
	GroundedState.HipsDirection = NewHipsDirection;
}

inline void UHodgeAlsAnimationInstance::ResetPivot()
{
	StandingState.bPivotActive = false;
}

inline void UHodgeAlsAnimationInstance::Jump()
{
	InAirState.bJumpRequested = true;
}
