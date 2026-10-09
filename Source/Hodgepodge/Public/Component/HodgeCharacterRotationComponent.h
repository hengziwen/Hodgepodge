#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "GameplayTagContainer.h"
#include "Combat/HodgeCharacterFacingTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "HodgeCharacterRotationComponent.generated.h"

class UHodgeAbilitySystemComponent;

/** 可复制的旋转约束；Actor Transform 仍由 CharacterMovement 同步。 */
USTRUCT(BlueprintType)
struct FHodgeCharacterRotationState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bYawLocked = false;

	UPROPERTY(BlueprintReadOnly)
	bool bRecoveringFacing = false;

	UPROPERTY(BlueprintReadOnly)
	float LockedYaw = 0.f;

	UPROPERTY(BlueprintReadOnly) EHodgeCharacterFacingDriver BaseDriver = EHodgeCharacterFacingDriver::Movement;
	UPROPERTY(BlueprintReadOnly) EHodgeCharacterFacingDriver Driver = EHodgeCharacterFacingDriver::Movement;
	UPROPERTY(BlueprintReadOnly) EHodgeLocomotionStyle Style = EHodgeLocomotionStyle::FreeDirectional;
	UPROPERTY(BlueprintReadOnly) float ActionYaw = 0.f;
	UPROPERTY(BlueprintReadOnly) FGuid ActionRequestId;
	UPROPERTY(BlueprintReadOnly) int32 StateVersion = 0;
	UPROPERTY() uint32 RequestSequence = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHodgeFacingStateChanged, FHodgeCharacterRotationState, Previous, FHodgeCharacterRotationState, Current);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHodgeActionFacingApplied, FHodgeFacingRequestHandle, Handle, EHodgeFacingRequestStatus, Status);

/**
 * 解析 Pawn 的旋转约束，控制器与移动组件负责执行。
 * ASC 由 PawnExtension 生命周期注入；组件不修改共享 ASC 的标签。
 */
UCLASS(BlueprintType, Meta = (BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeCharacterRotationComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UHodgeCharacterRotationComponent(const FObjectInitializer& ObjectInitializer);

	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
	void UninitializeFromAbilitySystem();

	/** 预留侧移只消费控制朝向，不搜索目标或调整相机。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Rotation")
	FHodgeFacingRequestHandle AcquireBaseFacingMode(EHodgeCharacterFacingDriver Driver, UObject* Source);
	UFUNCTION(BlueprintCallable, Category="Hodge|Rotation")
	FHodgeFacingRequestHandle RequestActionFacing(FVector WorldDirection, UObject* Source, FGuid ExecutionId, bool bInstantAtStart = false);
	UFUNCTION(BlueprintCallable, Category="Hodge|Rotation")
	bool ReleaseFacingRequest(FHodgeFacingRequestHandle Handle);
	void ReleaseRequestsForSource(const UObject* Source);
	UFUNCTION(BlueprintPure, Category="Hodge|Rotation")
	EHodgeFacingRequestStatus GetFacingRequestStatus(FHodgeFacingRequestHandle Handle) const;
	UFUNCTION(BlueprintPure, Category="Hodge|Rotation")
	FHodgeFacingPresentationSnapshot GetFacingPresentationSnapshot() const;
	UFUNCTION(BlueprintPure, Category="Hodge|Rotation")
	int32 GetAvatarGeneration() const { return AvatarGeneration; }
	UFUNCTION(BlueprintPure, Category="Hodge|Rotation")
	bool IsFacingSystemReady() const;
	UPROPERTY(BlueprintAssignable) FHodgeFacingStateChanged OnFacingStateChanged;
	UPROPERTY(BlueprintAssignable) FHodgeActionFacingApplied OnActionFacingApplied;

	void ApplyResolvedMode();
	bool BeginServerMove(uint32 Sequence);
	void EndServerMove();

	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
	bool IsYawLocked() const;

	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
	bool IsRecoveringFacing() const;

	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
	float GetLockedYaw() const;

	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
	FHodgeCharacterRotationState GetResolvedState() const;

	FRotator FilterControlRotation(const FRotator& DesiredRotation, float DeltaSeconds) const;
	void NotifyFacingApplied(float DesiredYaw);
	float GetRecoveryTurnRate() const { return FMath::IsFinite(RecoveryTurnRate) && RecoveryTurnRate > 0.f ? RecoveryTurnRate : 360.f; }
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

	void SetMoveReplayState(const FHodgeCharacterRotationState& State);
	void ClearMoveReplayState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void HandleConstraintTagChanged(FGameplayTag Tag, int32 NewCount);
	void RefreshLocalState();
	void PublishAuthorityState();
	bool IsReplayingMove() const;
	void ResolveRequests();
	bool IsValidSource(const UObject* Source, bool bAction) const;
	FHodgeFacingRequestHandle AddRequest(EHodgeCharacterFacingDriver Driver, UObject* Source, FGuid ExecutionId, float Yaw, bool bInstant);
	void SendRequest(const FGuid& Id, bool bRelease);
	void SetRequestStatus(const FGuid& Id, EHodgeFacingRequestStatus Status);
	bool HasPendingPrediction() const;

	UFUNCTION(Server, Reliable)
	void ServerFacingRequest(AActor* Avatar, uint32 Sequence, FGuid Id, bool bRelease, EHodgeCharacterFacingDriver Driver,
		float Yaw, bool bInstant, FGameplayAbilitySpecHandle AbilityHandle, int32 PredictionKey, FGuid ExecutionId);
	UFUNCTION(Client, Reliable)
	void ClientFacingResult(uint32 Sequence, FGuid Id, bool bAccepted, FHodgeCharacterRotationState State);

	UFUNCTION()
	void OnRep_RotationState();

	UPROPERTY(EditDefaultsOnly, Category = "Hodge|Rotation", Meta = (ClampMin = "1", Units = "deg/s"))
	float RecoveryTurnRate = 360.f;
	UPROPERTY(EditDefaultsOnly, Category="Hodge|Rotation", meta=(ClampMin="1", Units="deg/s"))
	float ActionTurnRate = 720.f;
	UPROPERTY(EditDefaultsOnly, Category="Hodge|Rotation")
	bool bAllowControllerFacingRequests = true;

	UPROPERTY(Transient)
	TWeakObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(ReplicatedUsing = OnRep_RotationState)
	FHodgeCharacterRotationState ReplicatedState;

	FHodgeCharacterRotationState LocalState;
	FHodgeCharacterRotationState ReplayState;
	FDelegateHandle RotationTagHandle;
	FDelegateHandle MovementStoppedTagHandle;
	FDelegateHandle HitReactionTagHandle;
	FDelegateHandle DeathTagHandle;
	bool bHasReplayState = false;
	bool bServerMoveOverride = false;
	bool bResolvingRequests = false;
	bool bOriginalControllerYaw = false;
	bool bOriginalOrientToMovement = true;
	bool bOriginalControllerDesired = false;
	int32 AvatarGeneration = 0;
	uint64 NextRequestOrder = 0;
	uint32 LocalRequestSequence = 0;
	uint32 AcknowledgedSequence = 0;
	uint32 LastServerRequestSequence = 0;
	FHodgeCharacterRotationState ServerMoveState;
	struct FRequest
	{
		TWeakObjectPtr<UObject> Source;
		FGuid ExecutionId;
		EHodgeCharacterFacingDriver Driver = EHodgeCharacterFacingDriver::Movement;
		EHodgeFacingRequestStatus Status = EHodgeFacingRequestStatus::Pending;
		float Yaw = 0.f;
		uint64 Order = 0;
		bool bInstant = false;
		bool bRemoteClient = false;
	};
	TMap<FGuid, FRequest> Requests;
	TMap<FGuid, EHodgeFacingRequestStatus> CompletedRequests;
	TMap<uint32, FHodgeCharacterRotationState> ServerRequestHistory;
};
