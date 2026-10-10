#include "Component/HodgeDefenseComponent.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Combat/HodgeMovementActionTypes.h"
#include "Data/HodgeSprintAbilityProfile.h"
#include "Abilities/GameplayAbility.h"
#include "TimerManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeDefenseComponent)

void UHodgeDefenseComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)
{
	if (ASC == InASC && InASC && InASC->GetAvatarActor() == GetOwner()) { return; }
	UninitializeFromAbilitySystem();
	if (InASC && InASC->GetAvatarActor() == GetOwner()) { ASC = InASC; }
}
void UHodgeDefenseComponent::UninitializeFromAbilitySystem() { Windows.Reset(); ASC.Reset(); }
FGuid UHodgeDefenseComponent::RegisterDodgeWindow(UGameplayAbility* Source, FGuid Execution, const UHodgeSprintAbilityProfile* Profile, double StartTime)
{
	if (!GetOwner()->HasAuthority() || !ASC.IsValid() || ASC->GetAvatarActor() != GetOwner() || !Source ||
		!Source->IsActive() || Source->GetAvatarActorFromActorInfo() != GetOwner() || !Execution.IsValid() ||
		!Profile || !FMath::IsFinite(StartTime)) { return {}; }
	const FGuid Handle = FGuid::NewGuid(); FWindow& Window = Windows.Add(Handle);
	Window.Source = Source; Window.Profile = Profile; Window.Execution = Execution; Window.StartTime = StartTime;
	return Handle;
}
void UHodgeDefenseComponent::UnregisterDodgeWindow(FGuid Handle) { Windows.Remove(Handle); }
EHodgeIncomingHitOutcome UHodgeDefenseComponent::EvaluateWindow(const UHodgeSprintAbilityProfile& Profile, double Age, bool bRewarded, bool bCanBeDodged, bool bPerfectEligible)
{
	if (!bCanBeDodged || !FMath::IsFinite(Age) || Age < Profile.InvulnerabilityStart || Age >= Profile.InvulnerabilityEnd) { return EHodgeIncomingHitOutcome::Allowed; }
	return !bRewarded && bPerfectEligible && Age >= Profile.PerfectStart && Age < Profile.PerfectEnd ? EHodgeIncomingHitOutcome::PerfectDodge : EHodgeIncomingHitOutcome::Dodged;
}
EHodgeIncomingHitOutcome UHodgeDefenseComponent::ResolveIncomingHit(AActor* Source, FGuid AttackExecution, int32 HitIndex, bool bCanBeDodged, bool bCanTriggerPerfect)
{
	if (!GetOwner()->HasAuthority() || !ASC.IsValid() || ASC->GetAvatarActor() != GetOwner() || !bCanBeDodged) { return EHodgeIncomingHitOutcome::Allowed; }
	if (ASC->HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) || ASC->HasMatchingGameplayTag(HodgeGameplayTags::Cheat_GodMode))
	{ return EHodgeIncomingHitOutcome::Allowed; }
	const double Time = GetWorld()->GetTimeSeconds();
	for (auto It = Windows.CreateIterator(); It; ++It)
	{
		FWindow& Window = It.Value(); const UHodgeSprintAbilityProfile* Profile = Window.Profile.Get();
		if (!Window.Source.IsValid() || !Window.Source->IsActive() || !Profile) { It.RemoveCurrent(); continue; }
		const double Age = Time - Window.StartTime;
		const auto Result = EvaluateWindow(*Profile, Age, Window.bRewarded, bCanBeDodged,
			bCanTriggerPerfect && AttackExecution.IsValid() && HitIndex >= 0 && IsValid(Source) && Source != GetOwner());
		if (Result == EHodgeIncomingHitOutcome::Allowed) { continue; }
		if (Result == EHodgeIncomingHitOutcome::Dodged) { return Result; }
		Window.bRewarded = true;
		const TWeakObjectPtr<UHodgeAbilitySystemComponent> WeakASC = ASC;
		const TWeakObjectPtr<AActor> Avatar = GetOwner(), Attacker = Source;
		const TSubclassOf<UGameplayEffect> Reward = Profile->PerfectRewardEffect;
		// 完美奖励延迟到命中事务结束，避免事件回调重入结算。
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakASC, Avatar, Attacker, Reward]()
		{
			if (!WeakASC.IsValid() || !Avatar.IsValid() || WeakASC->GetAvatarActor() != Avatar.Get()) { return; }
			if (Reward)
			{
				const FGameplayEffectSpecHandle Spec = WeakASC->MakeOutgoingSpec(Reward, 1.f, WeakASC->MakeEffectContext());
				if (Spec.IsValid()) { WeakASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get()); }
			}
			FGameplayEventData Event; Event.EventTag = HodgeMovementTags::PerfectDodge; Event.Instigator = Attacker.Get(); Event.Target = Avatar.Get();
			WeakASC->HandleGameplayEvent(Event.EventTag, &Event);
			FGameplayCueParameters Cue; Cue.Instigator = Avatar.Get(); Cue.Location = Avatar->GetActorLocation();
			WeakASC->ExecuteGameplayCue(HodgeMovementTags::PerfectDodgeCue, Cue);
		}));
		return EHodgeIncomingHitOutcome::PerfectDodge;
	}
	return EHodgeIncomingHitOutcome::Allowed;
}
void UHodgeDefenseComponent::EndPlay(EEndPlayReason::Type Reason) { UninitializeFromAbilitySystem(); Super::EndPlay(Reason); }
