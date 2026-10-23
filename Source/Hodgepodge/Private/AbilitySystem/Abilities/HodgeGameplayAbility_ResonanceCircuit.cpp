/** @file HodgeGameplayAbility_ResonanceCircuit.cpp @brief 灭音以合法攻击实例为提交边界，不另写伤害链路。 */
#include "AbilitySystem/Abilities/HodgeGameplayAbility_ResonanceCircuit.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h"
#include "AbilitySystem/AttributeSet/HodgeHavocSet.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_ResonanceCircuit)

bool UHodgeGameplayAbility_ResonanceCircuit::CanActivateAbility(FGameplayAbilitySpecHandle Handle,
                                                                const FGameplayAbilityActorInfo* Info,
                                                                const FGameplayTagContainer* SourceTags,
                                                                const FGameplayTagContainer* TargetTags,
                                                                FGameplayTagContainer* RelevantTags) const
{
	auto* ASC = Info ? Info->AbilitySystemComponent.Get() : nullptr;
	const auto* Resource = ASC ? ASC->GetSet<UHodgeHavocSet>() : nullptr;
	const auto* Character = Info ? Cast<ACharacter>(Info->AvatarActor.Get()) : nullptr;
	if (!Resource || !Resource->IsResourceReady() || Resource->GetDarkFlow() < Resource->GetMaxDarkFlow() || ASC->
		HasMatchingGameplayTag(HodgeHavocTags::DarkSurge) ||
		(bRequireGrounded && (!Character || !Character->GetCharacterMovement()->IsMovingOnGround()))) { return false; }
	if (const auto* Current = Cast<UHodgeGameplayAbility_Definition>(ASC->GetAnimatingAbility()); Current && Current->
		IsActive())
	{
		if (!EntryCancelWindow.IsValid() || !Current->CanBeCanceled() || !Current->GetExecutionWindows().
			HasTag(EntryCancelWindow)) { return false; }
	}
	return Super::CanActivateAbility(Handle, Info, SourceTags, TargetTags, RelevantTags);
}

void UHodgeGameplayAbility_ResonanceCircuit::OnExecutionReady()
{
	bCommitted = false;
	BurstOccurrence = INDEX_NONE;
	Super::OnExecutionReady();
}

bool UHodgeGameplayAbility_ResonanceCircuit::CommitDarkSurge(const FBranchingPointNotifyPayload& Payload,
                                                             const FHodgeAnimHitConfig& Burst)
{
	if (bCommitted || !AcceptsNotify(Payload) || !CurrentActorInfo || !CurrentActorInfo->IsNetAuthority())
	{
		return false;
	}
	auto* Rules = UHodgeGameplayAbility_HavocRules::Find(GetAbilitySystemComponentFromActorInfo());
	if (!Rules || !Rules->CanEnterDarkSurge(GetAvatarActorFromActorInfo())) { return false; }
	// 伤害查询之前锁存，回调重入也不能再次炸气；空挥不影响入态资格。
	bCommitted = true;
	BurstOccurrence = AllocateNotifyOccurrence();
	if (!BeginNotifyHit(BurstOccurrence, Burst, Payload.SkelMeshComponent, true))
	{
		UE_LOG(LogTemp, Error, TEXT("[Havoc] MieYin burst query failed: %s"), *GetPathName());
		return false;
	}
	return true;
}

void UHodgeGameplayAbility_ResonanceCircuit::ProcessMeleeHitResults_Implementation(const FHodgeHitDetectionBatch& Batch)
{
	if (!IsMeleeBatchCurrent(Batch) || Batch.OccurrenceId != BurstOccurrence) { return; }
	Super::ProcessMeleeHitResults_Implementation(Batch);
	// 空命中批次也完成正式提交；来源结束或死亡时Rules会拒绝迟到入态。
	BurstOccurrence = INDEX_NONE;
	if (auto* Rules = UHodgeGameplayAbility_HavocRules::Find(GetAbilitySystemComponentFromActorInfo()))
	{
		if (!Rules->TryEnterDarkSurge(this))
		{
			UE_LOG(LogTemp, Warning, TEXT("[Havoc] DarkSurge commit rejected after burst: %s"), *GetPathName());
		}
	}
}
