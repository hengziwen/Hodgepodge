#pragma once

#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "Combat/HodgeMovementActionTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "HodgeGameplayAbility_MovementAction.generated.h"

class UHodgeSprintAbilityProfile;
class UHodgeLocomotionPolicyComponent;
class UHodgeCharacterRotationComponent;

/** GAS 激活关联的方向快照，客户端不能提交速度、费用或防御时间。 */
USTRUCT()
struct FHodgeMovementActionTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	FVector2D Input = FVector2D::ZeroVector;
	float ControlYaw = 0.f;
	float ActorYawAtStart = 0.f;
	int32 SessionId = 0;
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bSuccess);
};
template<> struct TStructOpsTypeTraits<FHodgeMovementActionTargetData> : TStructOpsTypeTraitsBase2<FHodgeMovementActionTargetData>
{ enum { WithNetSerializer = true, WithCopy = true }; };

/** 两份移动能力复用目标数据及生命周期，不继承攻击 Definition。 */
UCLASS(Abstract, Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_MovementAction : public UHodgeGameplayAbility
{
	GENERATED_BODY()
public:
	UHodgeGameplayAbility_MovementAction();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UHodgeSprintAbilityProfile> Profile;
	UFUNCTION(BlueprintPure) bool HasCommittedMovement() const { return bCommitted; }
	UFUNCTION(BlueprintPure) int32 GetInputSessionId() const { return SessionId; }
	FGuid GetMovementExecutionId() const { return ExecutionId; }
	const UHodgeSprintAbilityProfile* ResolveProfile(const FGameplayAbilityActorInfo* Info) const;
	virtual bool CheckCooldown(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayTagContainer* Tags = nullptr) const override { return true; }
	virtual void ApplyCooldown(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo) const override {}
	virtual bool CheckCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayTagContainer* Tags = nullptr) const override { return true; }
	virtual void ApplyCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo) const override {}
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* RelevantTags = nullptr) const override;
protected:
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
	virtual void BeginMovement(const FHodgeMoveIntentSnapshot& Intent) PURE_VIRTUAL(UHodgeGameplayAbility_MovementAction::BeginMovement, );
	virtual void UpdateMovement(float DeltaTime) PURE_VIRTUAL(UHodgeGameplayAbility_MovementAction::UpdateMovement, );
	virtual void HandleMovementFollowUp(const FGameplayAbilityTargetDataHandle& Data) {}
	void Finish(bool bCancelled = true);
	bool IsMovementValid() const;
	void SetMovementTag(FGameplayTag Tag, bool bAdd);
	void StartCue(FGameplayTag Tag);
	void StopCue();
	UPROPERTY(Transient) TObjectPtr<const UHodgeSprintAbilityProfile> ExecutionProfile;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeLocomotionPolicyComponent> Policy;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeCharacterRotationComponent> Rotation;
	FGuid ExecutionId;
	int32 SessionId = 0;
	float ActorYawAtStart = 0.f;
	bool bCommitted = false;
	double PreparingAt = 0.;
	double CommittedAt = 0.;
	FHodgeFacingRequestHandle FacingHandle;
	FGameplayTag MovementTag;
private:
	void OnTargetData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag Tag);
	void TickMovement();
	void OnConstraintChanged(FGameplayTag Tag, int32 Count);
	FDelegateHandle ConstraintHandles[3];
	FDelegateHandle DataHandle;
	FTimerHandle TickHandle;
	FGameplayTag ActiveCue;
	bool bReceivedData = false;
};
