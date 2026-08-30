// 111屎山代码来袭


#include "Core/PlayState/HodgePlayerState.h"

#include "AbilitySystem/HodgeAbilitySystemComponentBase.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "Component/HodgeExperienceManagerComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Core/GameMode/HodgeGameModeBase.h"
#include "Core/PlayerController/HodgePlayerControllerBase.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgePlayerState)

const FName AHodgePlayerState::NAME_HodgeAbilityReady("HodgeAbilitiesReady");

AHodgePlayerState::AHodgePlayerState(const FObjectInitializer& ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UHodgeAbilitySystemComponentBase>(
		this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	HealthSet = CreateDefaultSubobject<UHodgeHealthSet>(TEXT("HealthSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);

	MyTeamID = FGenericTeamId::NoTeam;
	MySquadID = INDEX_NONE;
}

AHodgePlayerControllerBase* AHodgePlayerState::GetHodgePlayerController() const
{
	return Cast<AHodgePlayerControllerBase>(GetOwner());
}

UAbilitySystemComponent* AHodgePlayerState::GetAbilitySystemComponent() const
{
	return GetHodgeAbilitySystemComponent();
}

void AHodgePlayerState::SetPawnData(const UHodgePawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		// UE_LOG(LogTemp, Error,
		//        TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."),
		//        *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	// for (const UHodgeAbilitySet* AbilitySet : PawnData->AbilitySets)
	// {
	// 	if (AbilitySet)
	// 	{
	// 		AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
	// 	}
	// }

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_HodgeAbilityReady);

	ForceNetUpdate();
}

void AHodgePlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		UHodgeExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<
			UHodgeExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(
			FOnHodgeExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

void AHodgePlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AHodgePlayerState::Reset()
{
	Super::Reset();
}

void AHodgePlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	// if (UHodgePawnExtensionComponent* PawnExtComp = UHodgePawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	// {
	// 	PawnExtComp->CheckDefaultInitialization();
	// }
}

void AHodgePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	//@TODO: Copy stats
}

void AHodgePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyPlayerConnectionType, SharedParams)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyTeamID, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MySquadID, SharedParams);

	SharedParams.Condition = ELifetimeCondition::COND_SkipOwner;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReplicatedViewRotation, SharedParams);

	DOREPLIFETIME(ThisClass, StatTags);
}

void AHodgePlayerState::OnDeactivated()
{
	bool bDestroyDeactivatedPlayerState = false;

	switch (GetPlayerConnectionType())
	{
	case EHodgePlayerConnectionType::Player:
	case EHodgePlayerConnectionType::InactivePlayer:
		//@TODO: Ask the experience if we should destroy disconnecting players immediately or leave them around
		// (e.g., for long running servers where they might build up if lots of players cycle through)
		bDestroyDeactivatedPlayerState = true;
		break;
	default:
		bDestroyDeactivatedPlayerState = true;
		break;
	}

	SetPlayerConnectionType(EHodgePlayerConnectionType::InactivePlayer);

	if (bDestroyDeactivatedPlayerState)
	{
		Destroy();
	}
}

void AHodgePlayerState::OnReactivated()
{
	Super::OnReactivated();
	if (GetPlayerConnectionType() == EHodgePlayerConnectionType::InactivePlayer)
	{
		SetPlayerConnectionType(EHodgePlayerConnectionType::Player);
	}
}

void AHodgePlayerState::SetPlayerConnectionType(EHodgePlayerConnectionType NewType)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyPlayerConnectionType, this);
	MyPlayerConnectionType = NewType;
}

void AHodgePlayerState::SetSquadID(int32 NewSquadID)
{
	if (HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MySquadID, this);

		MySquadID = NewSquadID;
	}
}

void AHodgePlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void AHodgePlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 AHodgePlayerState::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool AHodgePlayerState::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

FRotator AHodgePlayerState::GetReplicatedViewRotation() const
{
	return ReplicatedViewRotation;
}

void AHodgePlayerState::SetReplicatedViewRotation(const FRotator& NewRotation)
{
	if (NewRotation != ReplicatedViewRotation)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedViewRotation, this);
		ReplicatedViewRotation = NewRotation;
	}
}

void AHodgePlayerState::OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience)
{
	if (AHodgeGameModeBase* HodgeGameMode = GetWorld()->GetAuthGameMode<AHodgeGameModeBase>())
	{
		if (const UHodgePawnData* NewPawnData = HodgeGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogTemp, Error,
			       TEXT(
				       "AHodgePlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"
			       ), *GetNameSafe(this));
		}
	}
}

void AHodgePlayerState::OnRep_PawnData()
{
}

void AHodgePlayerState::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	//ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void AHodgePlayerState::OnRep_MySquadID()
{
	//@TODO: Let the squad subsystem know (once that exists)
}
