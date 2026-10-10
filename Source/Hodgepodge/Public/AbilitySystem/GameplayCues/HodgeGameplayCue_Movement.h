#pragma once

#include "GameplayCueNotify_Actor.h"
#include "HodgeGameplayCue_Movement.generated.h"
class UNiagaraSystem;
class UNiagaraComponent;

/** 可配置的移动表现；不修改体力、防御、Actor 变换或速度。 */
UCLASS(Blueprintable)
class HODGEPODGE_API AHodgeGameplayCue_Movement : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
public:
	AHodgeGameplayCue_Movement();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UNiagaraSystem> Effect;
	virtual bool OnActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
	virtual bool WhileActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
private:
	UPROPERTY(Transient) TObjectPtr<UNiagaraComponent> ActiveEffect;
};
