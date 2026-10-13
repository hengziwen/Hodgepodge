/** @file HodgeGameplayUIDataSource.cpp
 * @brief 实现本地玩家共享UI数据源相关的类型、接口与现有处理流程。
 */
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

/** 解绑Pawn角色。 */
void UHodgeGameplayUIDataSource::UnbindPawn()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Health.IsValid())
	{
		Health->OnHealthChanged.RemoveAll(this); Health->OnMaxHealthChanged.RemoveAll(this);
		Health->OnDeathStarted.RemoveAll(this); Health->OnDeathFinished.RemoveAll(this);
	}
	if (Extension.IsValid()) { Extension->UnregisterAbilitySystemDelegates(this); }
	Health.Reset(); Extension.Reset(); Avatar.Reset();
}

/** 解绑状态。 */
void UHodgeGameplayUIDataSource::UnbindState()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (PlayerState.IsValid()) { PlayerState->OnAttributeReadinessChanged.Remove(ReadyDelegate); }
	ReadyDelegate.Reset(); PlayerState.Reset();
}

/** 停止当前对象的运行绑定并清理其资源。 */
void UHodgeGameplayUIDataSource::Shutdown()
{
	bBound = false;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (LocalPlayer.IsValid())
	{
		LocalPlayer->OnPlayerPawnSet.Remove(PawnDelegate);
		LocalPlayer->OnPlayerStateSet.Remove(PlayerStateDelegate);
	}
	PawnDelegate.Reset(); PlayerStateDelegate.Reset();
	UnbindPawn(); UnbindState(); LocalPlayer.Reset(); Controller.Reset();
	Refresh(); OnVitalsChanged.Clear();
}

/** 在本地玩家切换PlayerState时更新数据监听。 */
void UHodgeGameplayUIDataSource::StateChanged(UHodgeLocalPlayerBase* Player, APlayerState* State)
{
	UnbindState();
	PlayerState = Cast<AHodgePlayerState>(State);
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (PlayerState.IsValid()) { ReadyDelegate = PlayerState->OnAttributeReadinessChanged.AddUObject(this, &ThisClass::Refresh); }
	Refresh();
}

/** 在本地玩家切换Pawn时解绑旧数据并接入新角色。 */
void UHodgeGameplayUIDataSource::PawnChanged(UHodgeLocalPlayerBase* Player, APawn* Pawn)
{
	if (!bBound || Player != LocalPlayer.Get()) { return; }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
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

/** 重新采集当前依赖的数据并更新缓存状态。 */
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
		Next.CharacterLevel = PlayerState->GetCharacterLevel();
	}
	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	if (Next.bReady != Vitals.bReady || Next.bDead != Vitals.bDead || Next.Health != Vitals.Health || Next.MaxHealth != Vitals.MaxHealth || Next.CharacterLevel != Vitals.CharacterLevel)
	{ Vitals = Next; OnVitalsChanged.Broadcast(Vitals); }
}

/** 收到属性变化后刷新当前数据快照。 */
void UHodgeGameplayUIDataSource::AttributeChanged(UHodgeHealthComponent* Component, float Old, float New, AActor* Instigator) { Refresh(); }
/** 收到死亡阶段变化后刷新当前数据快照。 */
void UHodgeGameplayUIDataSource::DeathChanged(AActor* Actor) { Refresh(); }

/** 检查是否允许Use玩法。 */
bool UHodgeGameplayUIDataSource::CanUseGameplay() const
{
	return bBound && Controller.IsValid() && Avatar.IsValid() && Controller->GetPawn() == Avatar.Get()
		&& PlayerState.IsValid() && PlayerState->AreAttributesReadyFor(Avatar.Get()) && Health.IsValid()
		&& !Health->IsDeadOrDying() && UHodgeUIManagerSubsystem::AllowsGameplayInput(Controller.Get());
}

/** 获取能力显示状态。 */
FHodgeUIAbilityDisplayState UHodgeGameplayUIDataSource::GetAbilityDisplayState(FGameplayTag InputTag) const
{
	FHodgeUIAbilityDisplayState State;
	// 玩家控制器对象。
	auto* PC = Cast<AHodgePlayerController>(Controller.Get());
	// 后续GAS操作使用的能力系统组件。
	auto* ASC = PC ? PC->GetHodgeAbilitySystemComponent() : nullptr;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
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

/** 从当前本地玩家的数据源提交合法技能输入意图。 */
bool UHodgeGameplayUIDataSource::SubmitInput(FGameplayTag InputTag)
{
	if (!GetAbilityDisplayState(InputTag).bInputAllowed) { return false; }
	if (auto* PC = Cast<AHodgePlayerController>(Controller.Get()))
	{ if (auto* ASC = PC->GetHodgeAbilitySystemComponent())
	  { ASC->AbilityInputTagPressed(InputTag); ASC->AbilityInputTagReleased(InputTag); return true; } }
	return false;
}
