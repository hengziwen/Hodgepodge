/** @file HodgeGameplayCue_Movement.cpp
 * @brief 实现玩法GameplayCue移动相关的类型、接口与现有处理流程。
 */
#include "AbilitySystem/GameplayCues/HodgeGameplayCue_Movement.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayCue_Movement)

/** 构造玩法GameplayCue移动并初始化其默认状态。 */
AHodgeGameplayCue_Movement::AHodgeGameplayCue_Movement()
{
	bAutoDestroyOnRemove = true; PrimaryActorTick.bCanEverTick = false;
}
/** 处理活动。 */
bool AHodgeGameplayCue_Movement::OnActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{ return WhileActive_Implementation(Target, Parameters); }
/** 在GameplayCue活动期间更新对应表现。 */
bool AHodgeGameplayCue_Movement::WhileActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Effect || !IsValid(Target) || !Target->GetRootComponent() || GetNetMode() == NM_DedicatedServer) { return true; }
	if (!IsValid(ActiveEffect))
	{
		ActiveEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(Effect, Target->GetRootComponent(), NAME_None,
			FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
	}
	return true;
}
/** 处理移除。 */
bool AHodgeGameplayCue_Movement::OnRemove_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (IsValid(ActiveEffect)) { ActiveEffect->Deactivate(); ActiveEffect->DestroyComponent(); }
	ActiveEffect = nullptr; return true;
}
/** 处理执行。 */
bool AHodgeGameplayCue_Movement::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Effect && IsValid(Target) && GetNetMode() != NM_DedicatedServer)
	{ UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Effect, Target->GetActorLocation(), Target->GetActorRotation()); }
	return true;
}
