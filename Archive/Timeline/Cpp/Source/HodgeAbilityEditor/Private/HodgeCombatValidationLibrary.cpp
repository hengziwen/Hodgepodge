#include "HodgeCombatValidationLibrary.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCombatValidationLibrary)

bool UHodgeCombatValidationLibrary::QueueAbilityAction(UHodgeAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle Handle, bool bCancel)
{
	if (!IsValid(ASC) || !Handle.IsValid() || !ASC->GetWorld() || ASC->GetWorld()->WorldType != EWorldType::PIE) { return false; }
	TWeakObjectPtr<UHodgeAbilitySystemComponent> WeakASC = ASC;
	TWeakObjectPtr<AActor> Avatar = ASC->GetAvatarActor();
	ASC->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakASC, Avatar, Handle, bCancel]()
	{
		if (!WeakASC.IsValid() || !Avatar.IsValid() || WeakASC->GetAvatarActor() != Avatar.Get()) { return; }
		if (bCancel)
		{
			FGameplayAbilitySpec* Spec = WeakASC->FindAbilitySpecFromHandle(Handle);
			UGameplayAbility* Instance = Spec ? Spec->GetPrimaryInstance() : nullptr;
			if (Instance && Instance->IsActive())
			{
				Instance->CancelAbility(Handle, WeakASC->AbilityActorInfo.Get(), Instance->GetCurrentActivationInfo(), true);
			}
		}
		else
		{
			const bool bActivated = WeakASC->TryActivateAbility(Handle, true);
			UE_LOG(LogTemp, Display, TEXT("HodgeValidation native activation: World=%s Authority=%d Result=%d"),
				*WeakASC->GetWorld()->GetName(), WeakASC->IsOwnerActorAuthoritative(), bActivated);
		}
	}));
	return true;
}
