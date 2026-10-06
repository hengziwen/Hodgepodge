#pragma once

#include "CoreMinimal.h"
#include "HodgeAttributeTypes.generated.h"

class APawn;
class UHodgeEquipmentDefinition;

UENUM(BlueprintType)
enum class EHodgeAttributeUpdateReason : uint8
{
	FirstSpawn,
	Respawn,
	Rebind,
	LevelUp,
	EquipmentChange,
	Restore
};

USTRUCT(BlueprintType)
struct FHodgeCharacterProgression
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FGuid CharacterId;
	UPROPERTY(BlueprintReadOnly) int32 Level = 1;
	UPROPERTY(BlueprintReadOnly) int32 Revision = 0;
};

USTRUCT(BlueprintType)
struct FHodgeAttributeReadyState
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) TObjectPtr<APawn> Avatar = nullptr;
	UPROPERTY(BlueprintReadOnly) int32 LifeGeneration = 0;
	UPROPERTY(BlueprintReadOnly) int32 Revision = 0;
	UPROPERTY(BlueprintReadOnly) bool bReady = false;
};

USTRUCT(BlueprintType)
struct FHodgeOwnedEquipmentState
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FGuid InstanceId;
	UPROPERTY(BlueprintReadOnly) TSubclassOf<UHodgeEquipmentDefinition> Definition;
	UPROPERTY(BlueprintReadOnly) int32 Level = 1;
};
