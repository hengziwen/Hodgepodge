// 111屎山代码来袭


#include "Core/GameMode/HodgeGameModeBase.h"

#include "Character/HodgeCharacterBase.h"
#include "Core/GameState/HodgeGameStateBase.h"
#include "Core/HUD/HodgeHUDBase.h"
#include "Core/PlayerController/HodgePlayerControllerBase.h"
#include "Core/PlayState/HodgePlayerStateBase.h"

AHodgeGameModeBase::AHodgeGameModeBase()
{
	PlayerControllerClass = AHodgePlayerControllerBase::StaticClass();
	DefaultPawnClass = AHodgeCharacterBase::StaticClass();
	GameStateClass = AHodgeGameStateBase::StaticClass();
	PlayerStateClass = AHodgePlayerStateBase::StaticClass();
	HUDClass = AHodgeHUDBase::StaticClass();
}

void AHodgeGameModeBase::InitGameState()
{
	Super::InitGameState();
}

void AHodgeGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

bool AHodgeGameModeBase::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}

bool AHodgeGameModeBase::ControllerCanRestart(AController* Controller)
{
	return true;
}
