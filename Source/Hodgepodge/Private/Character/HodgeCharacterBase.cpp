// 111屎山代码来袭


#include "Character/HodgeCharacterBase.h"
#include "Core/PlayState/HodgePlayerStateBase.h"
void AHodgeCharacterBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AHodgeCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called when the game starts or when spawned
void AHodgeCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Initialize AbilitySystemComponent from PlayerState (PlayerState owns ASC)
	if (APlayerState* PS = GetPlayerState())
	{
		if (AHodgePlayerStateBase* HPS = Cast<AHodgePlayerStateBase>(PS))
		{
			HPS->InitializeAbilitySystemForCharacter(this);
		}
	}
}


