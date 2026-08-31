#pragma once

#include "HodgeAlsViewSettings.generated.h"

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsViewSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HODGEALS")
	uint8 bEnableNetworkSmoothing : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HODGEALS")
	uint8 bEnableListenServerNetworkSmoothing : 1 {true};
};
