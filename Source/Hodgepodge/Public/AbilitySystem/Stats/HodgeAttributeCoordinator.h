#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AbilitySystem/Stats/HodgeAttributeTypes.h"
#include "HodgeAttributeCoordinator.generated.h"

class AHodgePlayerState;
class UHodgeAbilitySystemComponent;
class UHodgeHealthSet;
class UHodgePawnData;
class UHodgeCharacterStatProfile;

/** PlayerState 所属服务器协调器，不 Tick，不拥有武器显示或存档 IO。 */
UCLASS()
class HODGEPODGE_API UHodgeAttributeCoordinator : public UObject
{
	GENERATED_BODY()
public:
	bool PrepareAvatar(APawn* Avatar, const UHodgePawnData* Data);
	bool CompleteAvatarInitialization();
	void DetachAvatar(APawn* Avatar);
	bool SetCharacterLevel(int32 Level);
	bool RestoreHealth(float Health);
	bool BeginEquipmentUpdate();
	void FinishEquipmentUpdate(bool bSuccess);
	void ExpectRespawn();
	bool SetInitialSavedHealth(float Health);
	bool IsUpdating() const { return bUpdating; }
	bool IsInitializing() const { return bUpdating && bBootstrap; }
	bool IsBoundTo(const APawn* Avatar) const;
	bool IsReadyFor(const APawn* Avatar) const;
	FHodgeOwnedEquipmentState GetOrCreateDefaultEquipment(TSubclassOf<UHodgeEquipmentDefinition> Definition);
	void RememberEquipment(FGuid Id, TSubclassOf<UHodgeEquipmentDefinition> Definition, int32 Level);
private:
	AHodgePlayerState* GetPlayerState() const;
	bool IsCurrentAvatar() const;
	bool ValidateCoreSets() const;
	bool ApplyCharacterBase(int32 Level);
	bool BeginUpdate(EHodgeAttributeUpdateReason Reason);
	bool CommitUpdate(bool bSuccess);
	void PublishReady(bool bReady);
	UPROPERTY(Transient) TWeakObjectPtr<APawn> BoundAvatar;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> AbilitySystem;
	UPROPERTY(Transient) TObjectPtr<const UHodgePawnData> PawnData;
	UPROPERTY(Transient) TObjectPtr<const UHodgeCharacterStatProfile> Profile;
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeHealthSet> HealthSet;
	bool bUpdating = false;
	bool bBootstrap = false;
	bool bWasReady = false;
	bool bExpectRespawn = false;
	bool bHasSavedHealth = false;
	float SavedHealth = 0.f;
	float OldHealth = 0.f;
	float OldMaxHealth = 1.f;
	float OldBaseHealth = 1.f;
	float OldBaseDamage = 0.f;
	int32 LifeGeneration = 0;
	int32 AppliedProfileVersion = 0;
	EHodgeAttributeUpdateReason UpdateReason = EHodgeAttributeUpdateReason::FirstSpawn;
};
