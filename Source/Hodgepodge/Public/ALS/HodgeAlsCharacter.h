#pragma once

#include "GameFramework/Character.h"
#include "ALS/State/HodgeAlsLocomotionState.h"
#include "ALS/State/HodgeAlsMantlingState.h"
#include "ALS/State/HodgeAlsMovementBaseState.h"
#include "ALS/State/HodgeAlsRagdollingState.h"
#include "ALS/State/HodgeAlsRollingState.h"
#include "ALS/State/HodgeAlsViewState.h"
#include "ALS/Utility/HodgeAlsGameplayTags.h"
#include "HodgeAlsCharacter.generated.h"

struct FHodgeAlsMantlingParameters;
struct FHodgeAlsMantlingTraceSettings;
class UHodgeAlsCharacterMovementComponent;
class UHodgeAlsCharacterSettings;
class UHodgeAlsMovementSettings;
class UHodgeAlsAnimationInstance;
class UHodgeAlsMantlingSettings;

UCLASS(AutoExpandCategories = ("Settings|HodgeAls Character", "Settings|HodgeAls Character|Desired State"))
class HODGEPODGE_API AHodgeAlsCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "HodgeAls Character")
	TObjectPtr<UHodgeAlsCharacterMovementComponent> HodgeAlsCharacterMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|HodgeAls Character")
	TObjectPtr<UHodgeAlsCharacterSettings> Settings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|HodgeAls Character")
	TObjectPtr<UHodgeAlsMovementSettings> MovementSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|HodgeAls Character|Desired State",
		ReplicatedUsing = "OnReplicated_DesiredAiming")
	uint8 bDesiredAiming : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|HodgeAls Character|Desired State", Replicated)
	FGameplayTag DesiredRotationMode{HodgeAlsRotationModeTags::ViewDirection};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|HodgeAls Character|Desired State", Replicated)
	FGameplayTag DesiredStance{HodgeAlsStanceTags::Standing};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|HodgeAls Character|Desired State", Replicated)
	FGameplayTag DesiredGait{HodgeAlsGaitTags::Running};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|HodgeAls Character|Desired State", Replicated)
	FGameplayTag ViewMode{HodgeAlsViewModeTags::ThirdPerson};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|HodgeAls Character|Desired State",
		ReplicatedUsing = "OnReplicated_OverlayMode")
	FGameplayTag OverlayMode{HodgeAlsOverlayModeTags::Default};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient, Meta = (ShowInnerProperties))
	TWeakObjectPtr<UHodgeAlsAnimationInstance> AnimationInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient)
	FGameplayTag LocomotionMode{HodgeAlsLocomotionModeTags::Grounded};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient)
	FGameplayTag RotationMode{HodgeAlsRotationModeTags::ViewDirection};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient)
	FGameplayTag Stance{HodgeAlsStanceTags::Standing};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient)
	FGameplayTag Gait{HodgeAlsGaitTags::Walking};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient)
	FGameplayTag LocomotionAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient)
	FHodgeAlsMovementBaseState MovementBase;

	// Replicated raw view rotation. Depending on the context, this rotation can be in world space, or in movement
	// base space. In most cases, it is better to use FHodgeAlsViewState::Rotation to take advantage of network smoothing.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient,
		ReplicatedUsing = "OnReplicated_ReplicatedViewRotation")
	FRotator ReplicatedViewRotation{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient)
	FHodgeAlsViewState ViewState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient, Replicated)
	FVector_NetQuantizeNormal InputDirection{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character",
		Transient, Replicated, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float DesiredVelocityYawAngle{0.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient)
	uint8 bHasDesiredVelocity : 1 {false};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient)
	FHodgeAlsLocomotionState LocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient)
	FHodgeAlsMantlingState MantlingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient, Replicated)
	FVector_NetQuantize RagdollTargetLocation{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient)
	FHodgeAlsRagdollingState RagdollingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|HodgeAls Character", Transient)
	FHodgeAlsRollingState RollingState;

	FTimerHandle BrakingFrictionFactorResetTimer;

public:
	explicit AHodgeAlsCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* Property) const override;
#endif

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreRegisterAllComponents() override;

	virtual void PostRegisterAllComponents() override;

	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& ViewInfo) override;

public:
	virtual void PostNetReceiveLocationAndRotation() override;

	virtual void OnRep_ReplicatedBasedMovement() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void Restart() override;

public:
	const UHodgeAlsCharacterSettings* GetSettings() const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "HodgeAls Character", Meta = (ReturnDisplayName = "Handled"))
	bool OnCalculateCamera(float DeltaTime, FMinimalViewInfo& ViewInfo);

private:
	void RefreshMeshProperties() const;

	void RefreshMovementBase();

	// View Mode

public:
	const FGameplayTag& GetViewMode() const;

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Character", Meta = (AutoCreateRefTerm = "NewViewMode"))
	void SetViewMode(const FGameplayTag& NewViewMode);

private:
	void SetViewMode(const FGameplayTag& NewViewMode, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetViewMode(const FGameplayTag& NewViewMode);

	UFUNCTION(Server, Reliable)
	void ServerSetViewMode(const FGameplayTag& NewViewMode);

	// Locomotion Mode

public:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode = 0) override;

public:
	const FGameplayTag& GetLocomotionMode() const;

protected:
	void SetLocomotionMode(const FGameplayTag& NewLocomotionMode);

	virtual void NotifyLocomotionModeChanged(const FGameplayTag& PreviousLocomotionMode);

	UFUNCTION(BlueprintImplementableEvent, Category = "HodgeAls Character")
	void OnLocomotionModeChanged(const FGameplayTag& PreviousLocomotionMode);

	// Desired Aiming

public:
	bool IsDesiredAiming() const;

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Character")
	void SetDesiredAiming(bool bNewDesiredAiming);

private:
	void SetDesiredAiming(bool bNewDesiredAiming, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetDesiredAiming(bool bNewDesiredAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetDesiredAiming(bool bNewDesiredAiming);

	UFUNCTION()
	void OnReplicated_DesiredAiming(bool bPreviousDesiredAiming);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "HodgeAls Character")
	void OnDesiredAimingChanged(bool bPreviousDesiredAiming);

	// Desired Rotation Mode

public:
	const FGameplayTag& GetDesiredRotationMode() const;

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Character", Meta = (AutoCreateRefTerm = "NewDesiredRotationMode"))
	void SetDesiredRotationMode(const FGameplayTag& NewDesiredRotationMode);

private:
	void SetDesiredRotationMode(const FGameplayTag& NewDesiredRotationMode, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetDesiredRotationMode(const FGameplayTag& NewDesiredRotationMode);

	UFUNCTION(Server, Reliable)
	void ServerSetDesiredRotationMode(const FGameplayTag& NewDesiredRotationMode);

	// Rotation Mode

public:
	const FGameplayTag& GetRotationMode() const;

protected:
	void SetRotationMode(const FGameplayTag& NewRotationMode);

	virtual void NotifyRotationModeChanged(const FGameplayTag& PreviousRotationMode);

	UFUNCTION(BlueprintImplementableEvent, Category = "HodgeAls Character")
	void OnRotationModeChanged(const FGameplayTag& PreviousRotationMode);

	void RefreshRotationMode();

	// Desired Stance

public:
	const FGameplayTag& GetDesiredStance() const;

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Character", Meta = (AutoCreateRefTerm = "NewDesiredStance"))
	void SetDesiredStance(const FGameplayTag& NewDesiredStance);

private:
	void SetDesiredStance(const FGameplayTag& NewDesiredStance, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetDesiredStance(const FGameplayTag& NewDesiredStance);

	UFUNCTION(Server, Reliable)
	void ServerSetDesiredStance(const FGameplayTag& NewDesiredStance);

protected:
	virtual void ApplyDesiredStance();

	// Stance

public:
	virtual bool CanCrouch() const override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

public:
	const FGameplayTag& GetStance() const;

protected:
	void SetStance(const FGameplayTag& NewStance);

	UFUNCTION(BlueprintNativeEvent, Category = "HodgeAls Character")
	void OnStanceChanged(const FGameplayTag& PreviousStance);

	// Desired Gait

public:
	const FGameplayTag& GetDesiredGait() const;

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Character", Meta = (AutoCreateRefTerm = "NewDesiredGait"))
	void SetDesiredGait(const FGameplayTag& NewDesiredGait);

private:
	void SetDesiredGait(const FGameplayTag& NewDesiredGait, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetDesiredGait(const FGameplayTag& NewDesiredGait);

	UFUNCTION(Server, Reliable)
	void ServerSetDesiredGait(const FGameplayTag& NewDesiredGait);

	// Gait

public:
	const FGameplayTag& GetGait() const;

protected:
	void SetGait(const FGameplayTag& NewGait);

	UFUNCTION(BlueprintNativeEvent, Category = "HodgeAls Character")
	void OnGaitChanged(const FGameplayTag& PreviousGait);

private:
	void RefreshGait();

	FGameplayTag CalculateMaxAllowedGait() const;

	FGameplayTag CalculateActualGait(const FGameplayTag& MaxAllowedGait) const;

	bool CanSprint() const;

	// Overlay Mode

public:
	const FGameplayTag& GetOverlayMode() const;

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Character", Meta = (AutoCreateRefTerm = "NewOverlayMode"))
	void SetOverlayMode(const FGameplayTag& NewOverlayMode);

private:
	void SetOverlayMode(const FGameplayTag& NewOverlayMode, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetOverlayMode(const FGameplayTag& NewOverlayMode);

	UFUNCTION(Server, Reliable)
	void ServerSetOverlayMode(const FGameplayTag& NewOverlayMode);

	UFUNCTION()
	void OnReplicated_OverlayMode(const FGameplayTag& PreviousOverlayMode);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "HodgeAls Character")
	void OnOverlayModeChanged(const FGameplayTag& PreviousOverlayMode);

	// Locomotion Action

public:
	const FGameplayTag& GetLocomotionAction() const;

	void SetLocomotionAction(const FGameplayTag& NewLocomotionAction);

protected:
	virtual void NotifyLocomotionActionChanged(const FGameplayTag& PreviousLocomotionAction);

	UFUNCTION(BlueprintImplementableEvent, Category = "HodgeAls Character")
	void OnLocomotionActionChanged(const FGameplayTag& PreviousLocomotionAction);

	// Input

public:
	const FVector& GetInputDirection() const;

protected:
	void SetInputDirection(FVector NewInputDirection);

	virtual void RefreshInput(float DeltaTime);

	// View

public:
	virtual FRotator GetViewRotation() const override;

private:
	void SetReplicatedViewRotation(const FRotator& NewViewRotation, bool bSendRpc);

	UFUNCTION(Server, Unreliable)
	void ServerSetReplicatedViewRotation(const FRotator& NewViewRotation);

	UFUNCTION()
	void OnReplicated_ReplicatedViewRotation();

public:
	void CorrectViewNetworkSmoothing(const FRotator& NewTargetRotation, bool bRotationIsBaseRelative);

public:
	const FHodgeAlsViewState& GetViewState() const;

private:
	void RefreshView(float DeltaTime);

	void RefreshViewNetworkSmoothing(float DeltaTime);

	// Locomotion

public:
	const FHodgeAlsLocomotionState& GetLocomotionState() const;

private:
	void SetDesiredVelocityYawAngle(float NewVelocityYawAngle);

	void RefreshLocomotionEarly();

	void RefreshLocomotion();

	void RefreshLocomotionLate();

	UFUNCTION(Server, Reliable)
	void ServerSetInitialVelocityYawAngle(float NewVelocityYawAngle);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetInitialVelocityYawAngle(float NewVelocityYawAngle);

	// Jumping

public:
	virtual void Jump() override;

	virtual void OnJumped_Implementation() override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnJumpedNetworked();

	void OnJumpedNetworked();

	// Rotation

public:
	virtual void FaceRotation(FRotator Rotation, float DeltaTime) override final;

	void CharacterMovement_OnPhysicsRotation(float DeltaTime);

private:
	void RefreshGroundedRotation(float DeltaTime);

protected:
	virtual bool RefreshCustomGroundedMovingRotation(float DeltaTime);

	virtual bool RefreshCustomGroundedNotMovingRotation(float DeltaTime);

	float CalculateGroundedMovingRotationInterpolationSpeed() const;

	void RefreshGroundedAimingRotation(float DeltaTime);

	bool ConstrainAimingRotation(FRotator& ActorRotation, float DeltaTime, bool bApplySecondaryConstraint = false);

private:
	void ApplyRotationYawSpeedAnimationCurve(float DeltaTime);

	void RefreshInAirRotation(float DeltaTime);

protected:
	virtual bool RefreshCustomInAirRotation(float DeltaTime);

	void RefreshInAirAimingRotation(float DeltaTime);

	void SetRotationSmooth(float TargetYawAngle, float DeltaTime, float InterpolationSpeed);

	void SetRotationExtraSmooth(float TargetYawAngle, float DeltaTime, float InterpolationSpeed, float TargetYawAngleRotationSpeed);

	void SetRotationInstant(float TargetYawAngle, ETeleportType Teleport = ETeleportType::None);

	void RefreshTargetYawAngleUsingActorRotation();

	void SetTargetYawAngle(float TargetYawAngle);

	void SetTargetYawAngleSmooth(float TargetYawAngle, float DeltaTime, float RotationSpeed);

	void RefreshViewRelativeTargetYawAngle();

	// Rolling

public:
	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Character")
	void StartRolling(float PlayRate = 1.0f);

	UFUNCTION(BlueprintNativeEvent, Category = "HodgeAls Character")
	UAnimMontage* SelectRollMontage();

	bool IsRollingAllowedToStart(const UAnimMontage* Montage) const;

private:
	void StartRolling(float PlayRate, float TargetYawAngle);

	UFUNCTION(Server, Reliable)
	void ServerStartRolling(UAnimMontage* Montage, float PlayRate, float InitialYawAngle, float TargetYawAngle);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartRolling(UAnimMontage* Montage, float PlayRate, float InitialYawAngle, float TargetYawAngle);

	void StartRollingImplementation(UAnimMontage* Montage, float PlayRate, float InitialYawAngle, float TargetYawAngle);

	void RefreshRolling(float DeltaTime);

	void RefreshRollingPhysics(float DeltaTime);

	// Mantling

public:
	UFUNCTION(BlueprintNativeEvent, Category = "HodgeAls Character")
	bool IsMantlingAllowedToStart() const;

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Character", Meta = (ReturnDisplayName = "Success"))
	bool StartMantlingGrounded();

private:
	bool StartMantlingInAir();

	bool StartMantling(const FHodgeAlsMantlingTraceSettings& TraceSettings);

	UFUNCTION(Server, Reliable)
	void ServerStartMantling(const FHodgeAlsMantlingParameters& Parameters);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartMantling(const FHodgeAlsMantlingParameters& Parameters);

	void StartMantlingImplementation(const FHodgeAlsMantlingParameters& Parameters);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "HodgeAls Character")
	UHodgeAlsMantlingSettings* SelectMantlingSettings(EHodgeAlsMantlingType MantlingType);

	float CalculateMantlingStartTime(const UHodgeAlsMantlingSettings* MantlingSettings, float MantlingHeight) const;

	UFUNCTION(BlueprintNativeEvent, Category = "HodgeAls Character")
	void OnMantlingStarted(const FHodgeAlsMantlingParameters& Parameters);

private:
	void RefreshMantling();

	void StopMantling(bool bStopMontage = false);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "HodgeAls Character")
	void OnMantlingEnded();

	// Ragdolling

public:
	const FHodgeAlsRagdollingState& GetRagdollingState() const;

	bool IsRagdollingAllowedToStart() const;

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Character")
	void StartRagdolling();

private:
	UFUNCTION(Server, Reliable)
	void ServerStartRagdolling();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartRagdolling();

	void StartRagdollingImplementation();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "HodgeAls Character")
	void OnRagdollingStarted();

public:
	bool IsRagdollingAllowedToStop() const;

	UFUNCTION(BlueprintCallable, Category = "HODGEALS|Character", Meta = (ReturnDisplayName = "Success"))
	bool StopRagdolling();

private:
	UFUNCTION(Server, Reliable)
	void ServerStopRagdolling();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopRagdolling();

	void StopRagdollingImplementation();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "HodgeAls Character")
	UAnimMontage* SelectGetUpMontage(bool bRagdollFacingUpward);

	UFUNCTION(BlueprintNativeEvent, Category = "HodgeAls Character")
	void OnRagdollingEnded();

private:
	void SetRagdollTargetLocation(const FVector& NewTargetLocation);

	UFUNCTION(Server, Unreliable)
	void ServerSetRagdollTargetLocation(const FVector_NetQuantize& NewTargetLocation);

	void RefreshRagdolling(float DeltaTime);

	FVector RagdollTraceGround(bool& bGrounded) const;

	void ConstraintRagdollSpeed() const;

	// Debug

public:
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation) override;

private:
	static void DisplayDebugHeader(const UCanvas* Canvas, const FText& HeaderText, const FLinearColor& HeaderColor,
	                               float Scale, float HorizontalLocation, float& VerticalLocation);

	void DisplayDebugCurves(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

	void DisplayDebugState(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

	void DisplayDebugShapes(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

	void DisplayDebugTraces(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

	void DisplayDebugMantling(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;
};

inline const UHodgeAlsCharacterSettings* AHodgeAlsCharacter::GetSettings() const
{
	return Settings;
}

inline const FGameplayTag& AHodgeAlsCharacter::GetViewMode() const
{
	return ViewMode;
}

inline const FGameplayTag& AHodgeAlsCharacter::GetLocomotionMode() const
{
	return LocomotionMode;
}

inline bool AHodgeAlsCharacter::IsDesiredAiming() const
{
	return bDesiredAiming;
}

inline const FGameplayTag& AHodgeAlsCharacter::GetDesiredRotationMode() const
{
	return DesiredRotationMode;
}

inline const FGameplayTag& AHodgeAlsCharacter::GetRotationMode() const
{
	return RotationMode;
}

inline const FGameplayTag& AHodgeAlsCharacter::GetDesiredStance() const
{
	return DesiredStance;
}

inline const FGameplayTag& AHodgeAlsCharacter::GetStance() const
{
	return Stance;
}

inline const FGameplayTag& AHodgeAlsCharacter::GetDesiredGait() const
{
	return DesiredGait;
}

inline const FGameplayTag& AHodgeAlsCharacter::GetGait() const
{
	return Gait;
}

inline const FGameplayTag& AHodgeAlsCharacter::GetOverlayMode() const
{
	return OverlayMode;
}

inline const FGameplayTag& AHodgeAlsCharacter::GetLocomotionAction() const
{
	return LocomotionAction;
}

inline const FVector& AHodgeAlsCharacter::GetInputDirection() const
{
	return InputDirection;
}

inline const FHodgeAlsViewState& AHodgeAlsCharacter::GetViewState() const
{
	return ViewState;
}

inline const FHodgeAlsLocomotionState& AHodgeAlsCharacter::GetLocomotionState() const
{
	return LocomotionState;
}

inline const FHodgeAlsRagdollingState& AHodgeAlsCharacter::GetRagdollingState() const
{
	return RagdollingState;
}
