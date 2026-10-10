#include "AbilitySystem/GameplayCues/HodgeGameplayCue_Movement.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayCue_Movement)

AHodgeGameplayCue_Movement::AHodgeGameplayCue_Movement()
{
	bAutoDestroyOnRemove = true; PrimaryActorTick.bCanEverTick = false;
}
bool AHodgeGameplayCue_Movement::OnActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{ return WhileActive_Implementation(Target, Parameters); }
bool AHodgeGameplayCue_Movement::WhileActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	if (!Effect || !IsValid(Target) || !Target->GetRootComponent() || GetNetMode() == NM_DedicatedServer) { return true; }
	if (!IsValid(ActiveEffect))
	{
		ActiveEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(Effect, Target->GetRootComponent(), NAME_None,
			FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
	}
	return true;
}
bool AHodgeGameplayCue_Movement::OnRemove_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	if (IsValid(ActiveEffect)) { ActiveEffect->Deactivate(); ActiveEffect->DestroyComponent(); }
	ActiveEffect = nullptr; return true;
}
bool AHodgeGameplayCue_Movement::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	if (Effect && IsValid(Target) && GetNetMode() != NM_DedicatedServer)
	{ UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Effect, Target->GetActorLocation(), Target->GetActorRotation()); }
	return true;
}
