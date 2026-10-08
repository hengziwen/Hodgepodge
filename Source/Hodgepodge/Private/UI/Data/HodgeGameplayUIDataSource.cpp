#include "UI/Data/HodgeGameplayUIDataSource.h"
#include "UI/Subsystem/HodgeUIManagerSubsystem.h"
#include "Core/LocalPlayer/HodgeLocalPlayerBase.h"
#include "Core/PlayerController/HodgePlayerController.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Component/HodgeHealthComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Component/HodgeCombatComponentBase.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "Data/HodgeComboDefinition.h"
#include "Abilities/GameplayAbility.h"
#include "NativeGameplayTags.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayUIDataSource)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_SLOT_VITALS, "UI.Slot.PlayerVitals");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_SLOT_ABILITIES, "UI.Slot.AbilityBar");

void UHodgeGameplayUIDataSource::Initialize(UHodgeLocalPlayerBase* Player, APlayerController* InController)
{
	Shutdown();
	if (!Player || !InController || InController->GetLocalPlayer() != Player) { return; }
	bBound = true; LocalPlayer = Player; Controller = InController;
	PlayerStateDelegate = Player->CallAndRegister_OnPlayerStateSet(
		UHodgeLocalPlayerBase::FPlayerStateSetDelegate::FDelegate::CreateUObject(this, &ThisClass::StateChanged));
	PawnDelegate = Player->CallAndRegister_OnPlayerPawnSet(
		UHodgeLocalPlayerBase::FPlayerPawnSetDelegate::FDelegate::CreateUObject(this, &ThisClass::PawnChanged));
	Refresh();
}

void UHodgeGameplayUIDataSource::UnbindPawn()
{
	if (Health.IsValid())
	{
		Health->OnHealthChanged.RemoveAll(this); Health->OnMaxHealthChanged.RemoveAll(this);
		Health->OnDeathStarted.RemoveAll(this); Health->OnDeathFinished.RemoveAll(this);
	}
	if (Extension.IsValid()) { Extension->UnregisterAbilitySystemDelegates(this); }
	Health.Reset(); Extension.Reset(); Avatar.Reset();
}

void UHodgeGameplayUIDataSource::UnbindState()
{
	if (PlayerState.IsValid()) { PlayerState->OnAttributeReadinessChanged.Remove(ReadyDelegate); }
	ReadyDelegate.Reset(); PlayerState.Reset();
}

void UHodgeGameplayUIDataSource::Shutdown()
{
	bBound = false;
	if (LocalPlayer.IsValid())
	{
		LocalPlayer->OnPlayerPawnSet.Remove(PawnDelegate);
		LocalPlayer->OnPlayerStateSet.Remove(PlayerStateDelegate);
	}
	PawnDelegate.Reset(); PlayerStateDelegate.Reset();
	UnbindPawn(); UnbindState(); LocalPlayer.Reset(); Controller.Reset();
	Refresh(); OnVitalsChanged.Clear();
}

void UHodgeGameplayUIDataSource::StateChanged(UHodgeLocalPlayerBase* Player, APlayerState* State)
{
	UnbindState();
	PlayerState = Cast<AHodgePlayerState>(State);
	if (PlayerState.IsValid()) { ReadyDelegate = PlayerState->OnAttributeReadinessChanged.AddUObject(this, &ThisClass::Refresh); }
	Refresh();
}

void UHodgeGameplayUIDataSource::PawnChanged(UHodgeLocalPlayerBase* Player, APawn* Pawn)
{
	if (!bBound || Player != LocalPlayer.Get()) { return; }
	if (Avatar.Get() == Pawn && Health.IsValid()) { Refresh(); return; }
	UnbindPawn(); Refresh(); Avatar = Pawn;
	Health = Pawn ? UHodgeHealthComponent::FindHealthComponent(Pawn) : nullptr;
	if (Health.IsValid())
	{
		Health->OnHealthChanged.AddUniqueDynamic(this, &ThisClass::AttributeChanged);
		Health->OnMaxHealthChanged.AddUniqueDynamic(this, &ThisClass::AttributeChanged);
		Health->OnDeathStarted.AddUniqueDynamic(this, &ThisClass::DeathChanged);
		Health->OnDeathFinished.AddUniqueDynamic(this, &ThisClass::DeathChanged);
	}
	Extension = Pawn ? UHodgePawnExtensionComponent::FindPawnExtensionComponent(Pawn) : nullptr;
	if (Extension.IsValid())
	{
		Extension->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::Refresh));
		Extension->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::Refresh));
	}
	Refresh();
}

void UHodgeGameplayUIDataSource::Refresh()
{
	FHodgeUIVitalsSnapshot Next;
	Next.bReady = bBound && Controller.IsValid() && Avatar.IsValid() && Controller->GetPawn() == Avatar.Get()
		&& PlayerState.IsValid() && PlayerState->AreAttributesReadyFor(Avatar.Get()) && Health.IsValid() && Health->GetMaxHealth() > 0;
	if (Next.bReady)
	{
		Next.Health = Health->GetHealth(); Next.MaxHealth = Health->GetMaxHealth();
		Next.Percent = FMath::Clamp(Next.Health / Next.MaxHealth, 0.f, 1.f);
		Next.bDead = Health->IsDeadOrDying();
	}
	if (Next.bReady != Vitals.bReady || Next.bDead != Vitals.bDead || Next.Health != Vitals.Health || Next.MaxHealth != Vitals.MaxHealth)
	{ Vitals = Next; OnVitalsChanged.Broadcast(Vitals); }
}

void UHodgeGameplayUIDataSource::AttributeChanged(UHodgeHealthComponent* Component, float Old, float New, AActor* Instigator) { Refresh(); }
void UHodgeGameplayUIDataSource::DeathChanged(AActor* Actor) { Refresh(); }

bool UHodgeGameplayUIDataSource::CanUseGameplay() const
{
	return bBound && Controller.IsValid() && Avatar.IsValid() && Controller->GetPawn() == Avatar.Get()
		&& PlayerState.IsValid() && PlayerState->AreAttributesReadyFor(Avatar.Get()) && Health.IsValid()
		&& !Health->IsDeadOrDying() && UHodgeUIManagerSubsystem::AllowsGameplayInput(Controller.Get());
}

FHodgeUIAbilityDisplayState UHodgeGameplayUIDataSource::GetAbilityDisplayState(FGameplayTag InputTag) const
{
	FHodgeUIAbilityDisplayState State;
	auto* PC = Cast<AHodgePlayerController>(Controller.Get());
	auto* ASC = PC ? PC->GetHodgeAbilitySystemComponent() : nullptr;
	if (!bBound || !ASC || !InputTag.IsValid()) { return State; }
	if (auto* Combat = Avatar.IsValid() ? Avatar->FindComponentByClass<UHodgeCombatComponentBase>() : nullptr)
	{
		if (auto* Combo = Combat->GetComboDefinition())
		{ for (const auto& Binding : Combo->InputBindings) { if (Binding.InputTag == InputTag) { State.bGranted = true; } } }
	}
	bool bCanActivate = State.bGranted;
	FScopedAbilityListLock Lock(*ASC);
	for (const auto& Spec : ASC->GetActivatableAbilities())
	{
		if (!Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag) || !Spec.Ability) { continue; }
		State.bGranted = true;
		bCanActivate = Spec.Ability->CanActivateAbility(Spec.Handle, ASC->AbilityActorInfo.Get());
		if (const auto* Tags = Spec.Ability->GetCooldownTags(); Tags && !Tags->IsEmpty())
		{
			for (float Remaining : ASC->GetActiveEffectsTimeRemaining(FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(*Tags)))
			{ State.CooldownRemaining = FMath::Max(State.CooldownRemaining, Remaining); }
		}
		break;
	}
	State.bInputAllowed = bCanActivate && CanUseGameplay() && ASC->GetAvatarActor() == Avatar.Get()
		&& !ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked) && State.CooldownRemaining <= 0;
	return State;
}

bool UHodgeGameplayUIDataSource::SubmitInput(FGameplayTag InputTag)
{
	if (!GetAbilityDisplayState(InputTag).bInputAllowed) { return false; }
	if (auto* PC = Cast<AHodgePlayerController>(Controller.Get()))
	{ if (auto* ASC = PC->GetHodgeAbilitySystemComponent())
	  { ASC->AbilityInputTagPressed(InputTag); ASC->AbilityInputTagReleased(InputTag); return true; } }
	return false;
}
