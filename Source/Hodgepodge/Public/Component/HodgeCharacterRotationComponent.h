#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "GameplayTagContainer.h"
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
};

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
	float GetRecoveryTurnRate() const { return RecoveryTurnRate; }

	void SetMoveReplayState(const FHodgeCharacterRotationState& State);
	void ClearMoveReplayState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void HandleConstraintTagChanged(FGameplayTag Tag, int32 NewCount);
	void RefreshLocalState();
	void PublishAuthorityState();
	bool IsReplayingMove() const;

	UFUNCTION()
	void OnRep_RotationState();

	UPROPERTY(EditDefaultsOnly, Category = "Hodge|Rotation", Meta = (ClampMin = "1", Units = "deg/s"))
	float RecoveryTurnRate = 360.f;

	UPROPERTY(Transient)
	TWeakObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(ReplicatedUsing = OnRep_RotationState)
	FHodgeCharacterRotationState ReplicatedState;

	FHodgeCharacterRotationState LocalState;
	FHodgeCharacterRotationState ReplayState;
	FDelegateHandle RotationTagHandle;
	FDelegateHandle MovementStoppedTagHandle;
	FDelegateHandle HitReactionTagHandle;
	bool bHasReplayState = false;
};
