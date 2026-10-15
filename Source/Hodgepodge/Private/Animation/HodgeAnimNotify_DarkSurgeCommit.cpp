/** @file HodgeAnimNotify_DarkSurgeCommit.cpp @brief 复用服务器虚拟Box查询和近战去重。 */
#include "Animation/HodgeAnimNotify_DarkSurgeCommit.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_ResonanceCircuit.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAnimNotify_DarkSurgeCommit)

UHodgeAnimNotify_DarkSurgeCommit::UHodgeAnimNotify_DarkSurgeCommit()
{
	bIsNativeBranchingPoint=true;
	Burst.Source=EHodgeAnimHitSource::AvatarRoot; Burst.Shape=EHodgeHitShape::Box;
	Burst.TransformPolicy=EHodgeHitTransformPolicy::SnapshotOnEventEnter;
	Burst.HitGroup=TEXT("MieYin"); Burst.AttackPhase=TEXT("Burst");
}

void UHodgeAnimNotify_DarkSurgeCommit::BranchingPointNotify(FBranchingPointNotifyPayload& Payload)
{
	auto* ASC=Payload.SkelMeshComponent ? UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Payload.SkelMeshComponent->GetOwner()) : nullptr;
	auto* GA=ASC ? Cast<UHodgeGameplayAbility_ResonanceCircuit>(ASC->GetAnimatingAbility()) : nullptr;
	if (GA) { GA->CommitDarkSurge(Payload,Burst); }
}
