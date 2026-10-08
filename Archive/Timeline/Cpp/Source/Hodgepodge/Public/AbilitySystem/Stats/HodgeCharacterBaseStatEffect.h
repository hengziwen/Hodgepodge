#pragma once

#include "GameplayEffect.h"
#include "HodgeCharacterBaseStatEffect.generated.h"

/** 一次性写角色基础值，不保留可撤销的活动效果。 */
UCLASS()
class HODGEPODGE_API UHodgeCharacterBaseStatEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UHodgeCharacterBaseStatEffect();
};
