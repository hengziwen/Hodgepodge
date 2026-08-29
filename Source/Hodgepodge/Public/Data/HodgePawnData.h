// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HodgePawnData.generated.h"

/**
 * Pawn data asset for storing pawn-specific information.
 */
UCLASS()
class HODGEPODGE_API UHodgePawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UHodgePawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from ALyraPawn or ALyraCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Abilities")
	//TArray<TObjectPtr<ULyraAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Abilities")
	//TObjectPtr<ULyraAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Input")
	//TObjectPtr<ULyraInputConfig> InputConfig;

	// Default camera mode used by player controlled pawns.
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Camera")
	//TSubclassOf<ULyraCameraMode> DefaultCameraMode;
};
