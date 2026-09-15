#pragma once

#include "CoreMinimal.h"
#include "Animation/HodgeAnimInstance.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HodgeALSLocomotion.generated.h"

UENUM(BlueprintType)
enum class EHodgeALSDirection : uint8 { Forward, Backward, LeftForward, LeftBackward, RightForward, RightBackward };

/** Isolated ground locomotion experiment. Gameplay input and GAS remain owned by Hodge. */
UCLASS()
class HODGEPODGE_API UHodgeALSLocomotion : public UHodgeAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	UFUNCTION(BlueprintPure, Category="ALS Lab")
	FName GetLabActiveState();
	UFUNCTION(BlueprintCallable, Category="ALS Lab")
	void SetHipBias(float Bias) { HipOrientationBias = FMath::Clamp(Bias, -1.f, 1.f); }

	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	EHodgeALSDirection LabDirection = EHodgeALSDirection::Forward;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabForward = 1.f;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabBackward = 0.f;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabLeft = 0.f;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabRight = 0.f;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabGait = 0.f;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabPlayRate = 1.f;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabSpeed = 0.f;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabViewAngle = 0.f;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabYawOffset = 0.f;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabFeetCrossing = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALS Lab", meta=(ClampMin="-1", ClampMax="1"))
	float HipOrientationBias = 0.f;
	UPROPERTY(EditDefaultsOnly, Category="ALS Lab")
	TObjectPtr<class UCurveFloat> ForwardBackwardYawCurve;
	UPROPERTY(EditDefaultsOnly, Category="ALS Lab")
	TObjectPtr<class UCurveFloat> LeftYawCurve;
	UPROPERTY(EditDefaultsOnly, Category="ALS Lab")
	TObjectPtr<class UCurveFloat> RightYawCurve;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabIdle = true;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabFall = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabF = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabB = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabLF = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabLB = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabRF = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabRB = false;

private:
	float DirectionHoldRemaining = 0.f;
};

UCLASS()
class HODGEPODGE_API UHodgeALSAuthoring : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Only accepts a new, empty CodexText animation blueprint. Never rewrites another graph. */
	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
	static bool BuildLocomotionGraph(UObject* Blueprint, const TArray<UObject*>& WalkRunAnimations, UObject* Idle,
	                                 UObject* Fall);
};
