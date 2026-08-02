// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HodgeCharacterBase.generated.h"

UCLASS()
class HODGEPODGE_API AHodgeCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	//~ Begin AActor Interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface
};
