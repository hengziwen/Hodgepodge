#pragma once
#include "CoreMinimal.h"
#include "CodexText/HodgeALSLocomotion.h"
#include "HodgeGroundedLocomotion.generated.h"

/** Flat-ground stop, turn and foot-lock layer for the CodexText experiment. */
UCLASS()
class HODGEPODGE_API UHodgeGroundedLocomotion : public UHodgeALSLocomotion
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float Dt) override;
	virtual void NativePostEvaluateAnimation() override;
	UFUNCTION(BlueprintPure)
	FName GetGroundedState();
	UFUNCTION(BlueprintPure)
	float GetGroundedRotationTarget() const { return RotationTarget; }

	UFUNCTION(BlueprintPure)
	bool IsTurningInPlace() const { return bGroundTurnL || bGroundTurnR; }

	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundIdle = true;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundMove = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundStopWalkL = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundStopWalkR = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundStopRunL = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundStopRunR = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundTurnL = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundTurnR = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	float GroundActionTime = 0.f;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	float FootAlphaL = 0.f;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	float FootAlphaR = 0.f;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	FVector FootTargetL = FVector::ZeroVector;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	FVector FootTargetR = FVector::ZeroVector;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	FRotator FootRotationL = FRotator::ZeroRotator;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	FRotator FootRotationR = FRotator::ZeroRotator;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bSupportLeft = true;
	UPROPERTY(EditDefaultsOnly, Category="Grounded")
	TArray<TObjectPtr<class UAnimSequence>> GroundActions;
	UPROPERTY(EditDefaultsOnly, Category="Grounded")
	TObjectPtr<class UCurveFloat> TurnYawLeft;
	UPROPERTY(EditDefaultsOnly, Category="Grounded")
	TObjectPtr<class UCurveFloat> TurnYawRight;
	UPROPERTY(EditDefaultsOnly, Category="Grounded")
	float TurnThreshold = 70.f;

private:
	FTransform PreviousRawFeet[2];
	FTransform LockedFeet[2];
	bool bHavePose = false;
	bool bHadMovement = false;
	bool bLastWalk = false;
	bool bLocked[2] = {false, false};
	float PreviousLockCurve[2] = {0.f, 0.f};
	float RotationTarget = 0.f;
	float TurnStartYaw = 0.f;
	float TurnDelay = 0.f;
	int32 ActionIndex = INDEX_NONE;
	FVector PreviousActorLocation = FVector::ZeroVector;
};

UCLASS()
class HODGEPODGE_API UHodgeGroundedAuthoring : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
	static bool AddGroundedLayer(UObject* Blueprint, const TArray<UObject*>& Actions, UObject* Idle, UObject* Fall,
	                             UObject* PreviewMesh);
};
