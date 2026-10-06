#include "AbilitySystem/Stats/HodgeAttributeCoordinator.h"
#include "AbilitySystem/HodgeGameplayTags.h"

#include "Core/PlayState/HodgePlayerState.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "Component/HodgeHealthComponent.h"
#include "Data/HodgeCharacterStatProfile.h"
#include "Data/HodgePawnData.h"
#include "Data/HodgeEquipmentStatProfile.h"
#include "Equipment/HodgeEquipmentManagerComponent.h"
#include "Equipment/HodgeEquipmentDefinition.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAttributeCoordinator)

AHodgePlayerState* UHodgeAttributeCoordinator::GetPlayerState() const { return GetTypedOuter<AHodgePlayerState>(); }
bool UHodgeAttributeCoordinator::IsBoundTo(const APawn* Avatar) const { return Avatar && BoundAvatar.Get() == Avatar; }
bool UHodgeAttributeCoordinator::IsCurrentAvatar() const { return BoundAvatar.IsValid() && AbilitySystem.IsValid() && AbilitySystem->GetAvatarActor() == BoundAvatar.Get(); }
bool UHodgeAttributeCoordinator::IsReadyFor(const APawn* Avatar) const
{
	const auto* PS = GetPlayerState();
	return PS && IsBoundTo(Avatar) && IsCurrentAvatar() && PS->AttributeReadyState.bReady;
}

bool UHodgeAttributeCoordinator::ValidateCoreSets() const
{
	if (!AbilitySystem.IsValid()) { return false; }
	int32 HealthCount = 0, CombatCount = 0;
	for (const UAttributeSet* Set : AbilitySystem->GetSpawnedAttributes())
	{
		if (Set && Set->IsA<UHodgeHealthSet>()) { ++HealthCount; }
		if (Set && Set->IsA<UHodgeCombatSet>()) { ++CombatCount; }
	}
	return HealthCount == 1 && CombatCount == 1;
}

void UHodgeAttributeCoordinator::PublishReady(bool bReady)
{
	auto* PS = GetPlayerState();
	if (!PS || !PS->HasAuthority()) { return; }
	PS->AttributeReadyState.Avatar = BoundAvatar.Get();
	PS->AttributeReadyState.LifeGeneration = LifeGeneration;
	PS->AttributeReadyState.bReady = bReady;
	PS->AttributeReadyState.Revision = PS->AttributeReadyState.Revision == MAX_int32 ? 1 : PS->AttributeReadyState.Revision + 1;
	PS->ForceNetUpdate();
	if (auto* World = PS->GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(PS, [PS]() { PS->NotifyAttributeReadiness(); }));
	}
}

bool UHodgeAttributeCoordinator::BeginUpdate(EHodgeAttributeUpdateReason Reason)
{
	if (bUpdating || !GetPlayerState() || !GetPlayerState()->HasAuthority() || !IsCurrentAvatar() || !HealthSet.IsValid()) { return false; }
	bWasReady = GetPlayerState()->AttributeReadyState.bReady;
	bUpdating = true;
	UpdateReason = Reason;
	OldHealth = HealthSet->GetHealth();
	OldMaxHealth = FMath::Max(1.f, HealthSet->GetMaxHealth());
	OldBaseHealth = AbilitySystem->GetNumericAttributeBase(UHodgeHealthSet::GetMaxHealthAttribute());
	OldBaseDamage = AbilitySystem->GetNumericAttributeBase(UHodgeCombatSet::GetBaseDamageAttribute());
	HealthSet->BeginAttributeRebuild();
	PublishReady(false);
	return true;
}

bool UHodgeAttributeCoordinator::ApplyCharacterBase(int32 Level)
{
	float Health, Damage;
	if (!Profile || !Profile->Evaluate(Level, Health, Damage) || !IsCurrentAvatar()) { return false; }
	auto Context = AbilitySystem->MakeEffectContext();
	Context.AddSourceObject(Profile);
	auto Spec = AbilitySystem->MakeOutgoingSpec(Profile->InitializationEffect, Level, Context);
	if (!Spec.IsValid()) { return false; }
	Spec.Data->SetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_Stat_MaxHealth, Health);
	Spec.Data->SetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_Stat_BaseDamage, Damage);
	return AbilitySystem->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get()).WasSuccessfullyApplied();
}

bool UHodgeAttributeCoordinator::PrepareAvatar(APawn* Avatar, const UHodgePawnData* Data)
{
	auto* PS = GetPlayerState();
	if (!PS || !PS->HasAuthority() || !IsValid(Avatar) || !Data || bUpdating) { return false; }
	if (IsReadyFor(Avatar) && !bExpectRespawn && Profile == Data->StatProfile && AppliedProfileVersion == Profile->ConfigurationVersion) { return true; }
	if (IsReadyFor(Avatar) && HealthSet.IsValid()) { SavedHealth = HealthSet->GetHealth(); }
	if (BoundAvatar.IsValid() && BoundAvatar.Get() != Avatar) { DetachAvatar(BoundAvatar.Get()); }
	const bool bNewLife = BoundAvatar.Get() != Avatar || bExpectRespawn;
	BoundAvatar = Avatar;
	AbilitySystem = PS->GetHodgeAbilitySystemComponent();
	PawnData = Data;
	Profile = Data->StatProfile;
	HealthSet = const_cast<UHodgeHealthSet*>(AbilitySystem->GetSet<UHodgeHealthSet>());
	if (bNewLife) { LifeGeneration = LifeGeneration == MAX_int32 ? 1 : LifeGeneration + 1; }
	PublishReady(false);
	TArray<FText> Errors;
	float Health, Damage;
	if (!IsCurrentAvatar() || !ValidateCoreSets() || !Profile || !Profile->Validate(Errors) || !Profile->Evaluate(PS->CharacterProgression.Level, Health, Damage))
	{
		UE_LOG(LogTemp, Error, TEXT("Attribute initialization rejected for %s: invalid profile, core sets or level"), *GetNameSafe(Avatar));
		return false;
	}
	if (!PS->CharacterProgression.CharacterId.IsValid()) { PS->CharacterProgression.CharacterId = FGuid::NewGuid(); }
	const auto Reason = bExpectRespawn ? EHodgeAttributeUpdateReason::Respawn
		: (bHasSavedHealth ? EHodgeAttributeUpdateReason::Rebind : EHodgeAttributeUpdateReason::FirstSpawn);
	if (!BeginUpdate(Reason)) { return false; }
	bBootstrap = true;
	if (!ApplyCharacterBase(PS->CharacterProgression.Level)) { CommitUpdate(false); return false; }
	return true;
}

bool UHodgeAttributeCoordinator::CompleteAvatarInitialization()
{
	if (!bUpdating) { return IsReadyFor(BoundAvatar.Get()); }
	if (!bBootstrap || !PawnData || !IsCurrentAvatar()) { return CommitUpdate(false); }
	bool bHasEquipment = true;
	if (PawnData->DefaultWeaponDefinition)
	{
		const auto* Manager = BoundAvatar->FindComponentByClass<UHodgeEquipmentManagerComponent>();
		bHasEquipment = Manager && Manager->FindInstanceOfDefinition(PawnData->DefaultWeaponDefinition);
	}
	return CommitUpdate(bHasEquipment);
}

bool UHodgeAttributeCoordinator::CommitUpdate(bool bSuccess)
{
	if (!bUpdating || !HealthSet.IsValid()) { return false; }
	bSuccess &= IsCurrentAvatar();
	if (!bSuccess && AbilitySystem.IsValid())
	{
		AbilitySystem->SetNumericAttributeBase(UHodgeHealthSet::GetMaxHealthAttribute(), OldBaseHealth);
		AbilitySystem->SetNumericAttributeBase(UHodgeCombatSet::GetBaseDamageAttribute(), OldBaseDamage);
	}
	bool bReachedZero = false;
	const float Delta = HealthSet->EndAttributeRebuild(bReachedZero);
	float Target = OldHealth;
	if (bSuccess)
	{
		if (UpdateReason == EHodgeAttributeUpdateReason::FirstSpawn || UpdateReason == EHodgeAttributeUpdateReason::Respawn)
		{
			Target = HealthSet->GetMaxHealth();
			if (!bReachedZero)
			{
				if (auto* Component = UHodgeHealthComponent::FindHealthComponent(BoundAvatar.Get())) { Component->ResetForSpawn(); }
			}
		}
		else if (UpdateReason == EHodgeAttributeUpdateReason::LevelUp) { Target = OldHealth / OldMaxHealth * HealthSet->GetMaxHealth(); }
		else if (UpdateReason == EHodgeAttributeUpdateReason::Rebind && bHasSavedHealth) { Target = SavedHealth; }
	}
	Target += Delta;
	if (bReachedZero || ((UpdateReason != EHodgeAttributeUpdateReason::FirstSpawn && UpdateReason != EHodgeAttributeUpdateReason::Respawn) && OldHealth <= 0.f)) { Target = 0.f; }
	if (IsCurrentAvatar()) { bSuccess &= HealthSet->SetHealthForAttributeCommit(Target); }
	const bool bRestoreReady = !bSuccess && !bBootstrap && bWasReady && IsCurrentAvatar();
	bUpdating = false;
	bBootstrap = false;
	if (bSuccess)
	{
		bHasSavedHealth = true;
		SavedHealth = HealthSet->GetHealth();
		bExpectRespawn = false;
		if (Profile && UpdateReason != EHodgeAttributeUpdateReason::EquipmentChange) { AppliedProfileVersion = Profile->ConfigurationVersion; }
	}
	PublishReady(bSuccess || bRestoreReady);
	return bSuccess;
}

void UHodgeAttributeCoordinator::DetachAvatar(APawn* Avatar)
{
	if (!IsBoundTo(Avatar)) { return; }
	if (bUpdating) { CommitUpdate(false); }
	if (HealthSet.IsValid()) { SavedHealth = HealthSet->GetHealth(); bHasSavedHealth = true; }
	PublishReady(false);
	BoundAvatar.Reset();
	AbilitySystem.Reset();
	HealthSet.Reset();
	PawnData = nullptr;
	Profile = nullptr;
}

bool UHodgeAttributeCoordinator::SetCharacterLevel(int32 Level)
{
	auto* PS = GetPlayerState();
	float Health, Damage;
	if (!PS || !PS->HasAuthority() || !IsReadyFor(BoundAvatar.Get()) || !Profile || !Profile->Evaluate(Level, Health, Damage)) { return false; }
	if (PS->CharacterProgression.Level == Level) { return true; }
	if (!BeginUpdate(EHodgeAttributeUpdateReason::LevelUp)) { return false; }
	const int32 PreviousLevel = PS->CharacterProgression.Level;
	const bool bApplied = ApplyCharacterBase(Level);
	if (bApplied) { PS->CharacterProgression.Level = Level; }
	if (!CommitUpdate(bApplied)) { PS->CharacterProgression.Level = PreviousLevel; return false; }
	PS->CharacterProgression.Revision = PS->CharacterProgression.Revision == MAX_int32 ? 1 : PS->CharacterProgression.Revision + 1;
	PS->ForceNetUpdate();
	return true;
}

bool UHodgeAttributeCoordinator::RestoreHealth(float Health)
{
	if (!GetPlayerState() || !GetPlayerState()->HasAuthority() || !IsReadyFor(BoundAvatar.Get()) || bUpdating || !FMath::IsFinite(Health) || Health < 0.f) { return false; }
	if (HealthSet->GetHealth() <= 0.f && Health > 0.f) { return false; }
	const bool bResult = HealthSet->SetHealthForAttributeCommit(Health);
	SavedHealth = HealthSet->GetHealth();
	return bResult;
}

bool UHodgeAttributeCoordinator::BeginEquipmentUpdate() { return IsReadyFor(BoundAvatar.Get()) && BeginUpdate(EHodgeAttributeUpdateReason::EquipmentChange); }
void UHodgeAttributeCoordinator::FinishEquipmentUpdate(bool bSuccess) { CommitUpdate(bSuccess); }
void UHodgeAttributeCoordinator::ExpectRespawn() { if (GetPlayerState() && GetPlayerState()->HasAuthority()) { bExpectRespawn = LifeGeneration > 0; } }

bool UHodgeAttributeCoordinator::SetInitialSavedHealth(float Health)
{
	if (LifeGeneration != 0 || bUpdating || !GetPlayerState() || !GetPlayerState()->HasAuthority() || !FMath::IsFinite(Health) || Health < 0.f) { return false; }
	SavedHealth = Health;
	bHasSavedHealth = true;
	return true;
}

FHodgeOwnedEquipmentState UHodgeAttributeCoordinator::GetOrCreateDefaultEquipment(TSubclassOf<UHodgeEquipmentDefinition> Definition)
{
	auto* PS = GetPlayerState();
	if (!PS || !PS->HasAuthority() || !Definition) { return {}; }
	if (const auto* Existing = PS->OwnedEquipmentStates.FindByPredicate([Definition](const auto& State) { return State.Definition == Definition; })) { return *Existing; }
	FHodgeOwnedEquipmentState State;
	State.InstanceId = FGuid::NewGuid();
	State.Definition = Definition;
	const UHodgeEquipmentStatProfile* ProfileData = Definition->GetDefaultObject<UHodgeEquipmentDefinition>()->StatProfile.Get();
	State.Level = ProfileData ? ProfileData->MinLevel : 1;
	PS->OwnedEquipmentStates.Add(State);
	PS->ForceNetUpdate();
	return State;
}

void UHodgeAttributeCoordinator::RememberEquipment(FGuid Id, TSubclassOf<UHodgeEquipmentDefinition> Definition, int32 Level)
{
	auto* PS = GetPlayerState();
	if (!PS || !PS->HasAuthority() || !Id.IsValid()) { return; }
	auto* State = PS->OwnedEquipmentStates.FindByPredicate([Id](const auto& Item) { return Item.InstanceId == Id; });
	if (!State) { State = &PS->OwnedEquipmentStates.AddDefaulted_GetRef(); State->InstanceId = Id; }
	State->Definition = Definition;
	State->Level = Level;
	PS->ForceNetUpdate();
}
