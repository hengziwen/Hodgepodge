#pragma once

#include "Animation/AnimInstance.h"
#include "HodgeAlsLinkedAnimationInstance.generated.h"

class AHodgeAlsCharacter;

UCLASS()
class HODGEPODGE_API UHodgeAlsLinkedAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, Category = "State", Transient)
	TWeakObjectPtr<UHodgeAlsAnimationInstance> Parent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	TObjectPtr<AHodgeAlsCharacter> Character;

public:
	UHodgeAlsLinkedAnimationInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;

protected:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;

protected:
	// Be very careful when using this function to read your custom variables using the property access system. It is
	// safe to use this function to read variables that change only inside UHodgeAlsAnimationInstance::NativeUpdateAnimation()
	// because it is guaranteed that this function will be called before parallel animation evaluation. Reading
	// variables that change in other functions can be dangerous because they can be changed in the game thread
	// at the same time as being read in the worker thread, which can lead to undefined behavior or even a crash.
	UFUNCTION(BlueprintPure, Category = "HODGEALS|Linked Animation Instance",
		Meta = (BlueprintThreadSafe, ReturnDisplayName = "Parent"))
	UHodgeAlsAnimationInstance* GetParent() const;

	UE_DEPRECATED(4.14, "Please use GetParent() instead")
	UFUNCTION(BlueprintPure, Category = "HODGEALS|Linked Animation Instance",
		Meta = (DeprecatedFunction, DeprecationMessage = "Please use GetParent() instead."))
	UHodgeAlsAnimationInstance* GetParentUnsafe() const;

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeLook();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshLook();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeLean();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeGrounded();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshGrounded();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void ResetGroundedEntryMode();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshGroundedMovement();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void SetHipsDirection(EHodgeAlsHipsDirection NewHipsDirection);

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeStandingMovement();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshStandingMovement();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void ResetPivot();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshCrouchingMovement();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshInAir();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshDynamicTransitions();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshRotateInPlace();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeTurnInPlace();

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshTurnInPlace();
};

inline UHodgeAlsAnimationInstance* UHodgeAlsLinkedAnimationInstance::GetParentUnsafe() const
{
	return Parent.Get();
}

inline UHodgeAlsAnimationInstance* UHodgeAlsLinkedAnimationInstance::GetParent() const
{
	return Parent.Get();
}
