#pragma once

#include "Components/PawnComponent.h"
#include "Combat/HodgeMovementActionTypes.h"
#include "GameplayEffectTypes.h"
#include "HodgeLocomotionPolicyComponent.generated.h"

class UHodgeAbilitySystemComponent;
class UHodgeSprintAbilityProfile;
class UGameplayAbility;
DECLARE_MULTICAST_DELEGATE(FHodgeSprintAuthorityEnded);

/** 只拥有速度和步态，朝向仍由 RotationComponent 仲裁。 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeLocomotionPolicyComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	UHodgeLocomotionPolicyComponent(const FObjectInitializer& Initializer = FObjectInitializer::Get());
	friend struct FHodgeMovementTestAccess;
	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
	void UninitializeFromAbilitySystem();
	FGuid AcquireSprint(UGameplayAbility* Source, const UHodgeSprintAbilityProfile* Profile, int32 ActivationKey);
	void ReleasePolicy(FGuid Handle);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) FGuid AcquireSpeedModifier(UObject* Source, float Multiplier);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void ReleaseSpeedModifier(FGuid Handle);
	void SetMoveReplayState(const FHodgeLocomotionState& State);
	void ClearMoveReplayState();
	void BeginServerMove(int32 ActivationKey);
	void EndServerMove();
	UFUNCTION(BlueprintPure) FHodgeLocomotionState GetResolvedPolicy() const;
	UFUNCTION(BlueprintPure) const UHodgeSprintAbilityProfile* GetProfile() const;
	UFUNCTION(BlueprintCallable) void SetSprintInput(int32 SessionId, bool bHeld);
	bool IsSessionHeld(int32 SessionId) const;
	float SessionHeldTime(int32 SessionId) const;
	void AuthorizeHandoff(int32 DashKey, int32 SessionId, float ValidFor);
	bool CanHandoff(int32 SessionId) const;
	void ConsumeHandoff();
	void RecordDashExitVelocity(FName Source, float MaximumSpeed);
	bool GetDashExitVelocity(FName Source, float& OutMaximumSpeed) const;
	FHodgeSprintAuthorityEnded OnSprintAuthorityEnded;
	virtual void TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* Function) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UHodgeSprintAbilityProfile> OverrideProfile;
private:
	struct FDashExitVelocity { float MaximumSpeed = 0.f; double ExpiresAt = 0.; };
	TMap<FName, FDashExitVelocity> DashExitVelocities;
	UFUNCTION(Server, Reliable) void ServerSetSprintInput(AActor* Avatar, int32 SessionId, bool bHeld);
	UFUNCTION() void OnRep_State();
	void SetInputInternal(int32 SessionId, bool bHeld);
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
	UPROPERTY(Transient) TWeakObjectPtr<UGameplayAbility> SprintSource;
	UPROPERTY(Transient) TObjectPtr<const UHodgeSprintAbilityProfile> SprintProfile;
	UPROPERTY(ReplicatedUsing=OnRep_State) FHodgeLocomotionState State;
	FHodgeLocomotionState ReplayState;
	FHodgeLocomotionState PredictedState;
	bool bReplay = false;
	bool bServerOverride = false;
	FGuid SprintHandle;
	struct FSpeedModifier { TWeakObjectPtr<UObject> Source; float Multiplier = 1.f; };
	TMap<FGuid, FSpeedModifier> SpeedModifiers;
	void RefreshSpeedScale();
	int32 InputSessionId = 0;
	bool bInputHeld = false;
	double InputPressedAt = 0.;
	int32 HandoffSessionId = 0;
	double HandoffExpiresAt = 0.;
};
