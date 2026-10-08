#pragma once

#include "GameplayEffect.h"
#include "HodgeEquipmentStatEffect.generated.h"

/** 每件装备独立持有的非周期加成。 */
UCLASS()
class HODGEPODGE_API UHodgeEquipmentStatEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UHodgeEquipmentStatEffect();
};
