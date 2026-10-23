/** @file HodgeGameplayAbility_HavocRules.cpp @brief 暗涌会话、服务器收益和周期GE归属实现。 */
#include "AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/AttributeSet/HodgeHavocSet.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "AbilitySystem/AttributeSet/HodgeResonanceSet.h"
#include "AbilitySystem/Effects/HodgeHavocEffects.h"
#include "AbilitySystem/Effects/HodgeResonanceEnergyEffect.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Engine/World.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_HavocRules)

UHodgeGameplayAbility_HavocRules::UHodgeGameplayAbility_HavocRules()
{
	ActivationPolicy = EHodgeAbilityActivationPolicy::OnSpawn;
	ActivationGroup = EHodgeAbilityActivationGroup::Independent;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	bAllowWhileHitReacting = true;
	FGameplayTagContainer Tags = GetAssetTags();
	Tags.AddTag(HodgeGameplayTags::Ability_Behavior_SurvivesDeath);
	SetAssetTags(Tags);
	StateEffect = UHodgeDarkSurgeEffect::StaticClass();
	DrainEffect = UHodgeDarkFlowDrainEffect::StaticClass();
	GainEffect = UHodgeDarkFlowEffect::StaticClass();
}

UHodgeGameplayAbility_HavocRules* UHodgeGameplayAbility_HavocRules::Find(UAbilitySystemComponent* ASC)
{
	if (!ASC) { return nullptr; }
	UHodgeGameplayAbility_HavocRules* Found = nullptr;
	FScopedAbilityListLock Lock(*ASC);
	for (auto& Spec : ASC->GetActivatableAbilities())
	{
		auto* Rules = Cast<UHodgeGameplayAbility_HavocRules>(Spec.GetPrimaryInstance());
		if (Rules && Rules->IsActive())
		{
			if (Found) { return nullptr; }
			Found = Rules;
		}
	}
	return Found;
}

bool UHodgeGameplayAbility_HavocRules::ValidateEffects() const
{
	if (!StateEffect || !DrainEffect || !GainEffect || !StateEffect->IsChildOf(UHodgeDarkSurgeEffect::StaticClass()) ||
		!DrainEffect->IsChildOf(UHodgeDarkFlowDrainEffect::StaticClass()) || !GainEffect->IsChildOf(
			UHodgeDarkFlowEffect::StaticClass())) { return false; }
	const auto* State = StateEffect.GetDefaultObject();
	const auto* Drain = DrainEffect.GetDefaultObject();
	const float Period = Drain->Period.GetValueAtLevel(1.f);
	return State->DurationPolicy == EGameplayEffectDurationType::Infinite && State->Period.GetValueAtLevel(1.f) == 0.f
		&&
		GainEffect.GetDefaultObject()->DurationPolicy == EGameplayEffectDurationType::Instant &&
		Drain->DurationPolicy == EGameplayEffectDurationType::Infinite && FMath::IsFinite(Period) && Period > 0.f &&
		Drain->Modifiers.IsEmpty() &&
		Drain->Executions.Num() == 1 && Drain->Executions[0].CalculationClass && Drain->Executions[0].CalculationClass->
		IsChildOf(UHodgeDarkFlowDrainExecution::StaticClass()) &&
		!Drain->bExecutePeriodicEffectOnApplication && FMath::IsFinite(DrainPerSecond) && DrainPerSecond > 0.f;
}

bool UHodgeGameplayAbility_HavocRules::IsCurrentSource(const UHodgeGameplayAbility_Definition* Source) const
{
	const auto* ASC = GetAbilitySystemComponentFromActorInfo();
	const auto* Health = ASC ? ASC->GetSet<UHodgeHealthSet>() : nullptr;
	return IsActive() && CurrentActorInfo && CurrentActorInfo->IsNetAuthority() && ASC && BoundAvatar.IsValid() && ASC->
		GetAvatarActor() == BoundAvatar.Get() &&
		Source && Source->IsActive() && !Source->IsExecutionEnding() && Source->GetExecutionId().IsValid() &&
		Source->GetAbilitySystemComponentFromActorInfo() == ASC && Source->GetAvatarActorFromActorInfo() == BoundAvatar.
		Get() &&
		!ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) && (!Health || Health->GetHealth() > 0.f);
}

bool UHodgeGameplayAbility_HavocRules::CanEnterDarkSurge(const AActor* Avatar) const
{
	const auto* ASC = GetAbilitySystemComponentFromActorInfo();
	const auto* Set = ASC ? ASC->GetSet<UHodgeHavocSet>() : nullptr;
	const auto* Health = ASC ? ASC->GetSet<UHodgeHealthSet>() : nullptr;
	return IsActive() && Avatar && Avatar == BoundAvatar.Get() && ASC && ASC->GetAvatarActor() == Avatar && Set && Set->
		IsResourceReady() &&
		ASC->GetSet<UHodgeCombatSet>() &&
		Set->GetDarkFlow() >= Set->GetMaxDarkFlow() && !Session.Id.IsValid() && !ASC->HasMatchingGameplayTag(
			HodgeHavocTags::DarkSurge) &&
		!ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) && (!Health || Health->GetHealth() > 0.f) &&
		ValidateEffects();
}

bool UHodgeGameplayAbility_HavocRules::TryEnterDarkSurge(UHodgeGameplayAbility_Definition* Source)
{
	if (!IsCurrentSource(Source) || !CanEnterDarkSurge(Source->GetAvatarActorFromActorInfo())) { return false; }
	return BeginSession(Source->GetExecutionId());
}

bool UHodgeGameplayAbility_HavocRules::BeginSession(FGuid EntryExecutionId)
{
	if (!EntryExecutionId.IsValid() || !CanEnterDarkSurge(BoundAvatar.Get())) { return false; }
	auto* ASC = GetAbilitySystemComponentFromActorInfo();
	Session.Id = FGuid::NewGuid();
	Session.EntryExecutionId = EntryExecutionId;
	Session.Avatar = BoundAvatar;
	Session.Clock.Start(GetWorld()->GetTimeSeconds());
	const FGuid Id = Session.Id;
	auto Context = ASC->MakeEffectContext();
	Context.AddSourceObject(this);
	// 先准备两份Spec，再施加；失败只回滚本会话的已创建效果。
	auto State = ASC->MakeOutgoingSpec(StateEffect, 1.f, Context);
	auto Drain = ASC->MakeOutgoingSpec(DrainEffect, 1.f, Context);
	if (!State.IsValid() || !Drain.IsValid())
	{
		EndDarkSurge(Id);
		return false;
	}
	const auto StateHandle = ASC->ApplyGameplayEffectSpecToSelf(*State.Data.Get());
	if (Session.Id != Id)
	{
		ASC->RemoveActiveGameplayEffect(StateHandle);
		return false;
	}
	Session.State = StateHandle;
	if (!ASC->GetActiveGameplayEffect(StateHandle) || !ASC->HasMatchingGameplayTag(HodgeHavocTags::DarkSurge))
	{
		EndDarkSurge(Id);
		return false;
	}
	const auto DrainHandle = ASC->ApplyGameplayEffectSpecToSelf(*Drain.Data.Get());
	// 应用GE会同步广播；回调结束会话时，返回的句柄仍须独立回收。
	if (Session.Id != Id)
	{
		ASC->RemoveActiveGameplayEffect(DrainHandle);
		return false;
	}
	Session.Drain = DrainHandle;
	if (!ASC->GetActiveGameplayEffect(DrainHandle))
	{
		EndDarkSurge(Id);
		return false;
	}
	if (auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(BoundAvatar.Get()))
	{
		Combat->InvalidateModeContinuation(Id);
	}
	return true;
}

void UHodgeGameplayAbility_HavocRules::EndDarkSurge(FGuid Id)
{
	if (!Id.IsValid() || Id != Session.Id || bEndingSession) { return; }
	TGuardValue<bool> Guard(bEndingSession, true);
	const auto Old = Session;
	Session = {};
	if (auto* ASC = BoundASC.Get())
	{
		ASC->RemoveActiveGameplayEffect(Old.Drain);
		ASC->RemoveActiveGameplayEffect(Old.State);
	}
	if (auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(Old.Avatar.Get()))
	{
		Combat->InvalidateModeContinuation(Id);
	}
}

float UHodgeGameplayAbility_HavocRules::ApplyDarkFlowGain(const UHodgeGameplayAbility_Definition* Source, float Amount)
{
	if (!IsCurrentSource(Source) || !FMath::IsFinite(Amount) || Amount <= 0.f || !GainEffect || !GainEffect->IsChildOf(
		UHodgeDarkFlowEffect::StaticClass())) { return 0.f; }
	auto* ASC = GetAbilitySystemComponentFromActorInfo();
	const auto* Set = ASC->GetSet<UHodgeHavocSet>();
	if (!Set || !Set->IsResourceReady()) { return 0.f; }
	const float Before = Set->GetDarkFlow();
	auto Spec = ASC->MakeOutgoingSpec(GainEffect, 1.f, ASC->MakeEffectContext());
	if (!Spec.IsValid()) { return 0.f; }
	Spec.Data->SetSetByCallerMagnitude(HodgeHavocTags::DarkFlowDelta, Amount);
	ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	return FMath::Max(0.f, Set->GetDarkFlow() - Before);
}

void UHodgeGameplayAbility_HavocRules::HandleEnhancedNormalHit(const UHodgeGameplayAbility_Definition* Source,
                                                               float AcceptedDamage)
{
	if (!IsCurrentSource(Source) || !FMath::IsFinite(AcceptedDamage) || AcceptedDamage <= 0.f || !Session.Id.IsValid())
	{
		return;
	}
	auto* ASC = GetAbilitySystemComponentFromActorInfo();
	const auto* Resource = ASC->GetSet<UHodgeResonanceSet>();
	if (!ASC->HasMatchingGameplayTag(HodgeHavocTags::DarkSurge) || !Resource || !Resource->IsResourceReady() ||
		Resource->GetResonanceEnergy() >= Resource->GetMaxResonanceEnergy()) { return; }
	auto Spec = ASC->MakeOutgoingSpec(UHodgeResonanceEnergyEffect::StaticClass(), 1.f, ASC->MakeEffectContext());
	if (!Spec.IsValid() || !ProcGate.TryReserve(GetWorld()->GetTimeSeconds(), 1.)) { return; }
	Spec.Data->SetSetByCallerMagnitude(HodgeResonanceTags::EnergyDelta, 1.f);
	ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
}

float UHodgeGameplayAbility_HavocRules::SettleFuel(const FGameplayEffectSpec& Spec)
{
	auto* ASC = BoundASC.Get();
	const auto* Drain = ASC ? ASC->GetActiveGameplayEffect(Session.Drain) : nullptr;
	if (!IsActive() || !CurrentActorInfo || !CurrentActorInfo->IsNetAuthority() || !ASC || !Session.Id.IsValid() ||
		!Drain || &Drain->Spec != &Spec ||
		!Session.Avatar.IsValid() || ASC->GetAvatarActor() != Session.Avatar.Get() || Spec.GetContext().
		GetSourceObject() != this ||
		!ASC->GetActiveGameplayEffect(Session.State) || !ASC->HasMatchingGameplayTag(HodgeHavocTags::DarkSurge))
	{
		return 0.f;
	}
	return Session.Clock.Settle(GetWorld()->GetTimeSeconds(), DrainPerSecond);
}

void UHodgeGameplayAbility_HavocRules::OnFuelChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.f) { EndDarkSurge(Session.Id); }
}

void UHodgeGameplayAbility_HavocRules::OnEffectRemoved(const FActiveGameplayEffect& Effect)
{
	if (Session.Id.IsValid() && (Effect.Handle == Session.State || Effect.Handle == Session.Drain))
	{
		// 先结束会话再清空，避免属性委托在清理中重新进入旧会话。
		EndDarkSurge(Session.Id);
		ClearDarkFlow();
	}
}

void UHodgeGameplayAbility_HavocRules::OnDeathTagChanged(FGameplayTag Tag, int32 Count)
{
	if (Count > 0)
	{
		EndDarkSurge(Session.Id);
		ClearDarkFlow();
	}
}

void UHodgeGameplayAbility_HavocRules::ClearDarkFlow()
{
	auto* ASC = BoundASC.Get();
	const auto* Resource = ASC ? ASC->GetSet<UHodgeHavocSet>() : nullptr;
	if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority() || !Resource || Resource->GetDarkFlow() <= 0.f)
	{
		return;
	}
	// 生命周期清空使用原生增量GE，不受角色误配的收益GE影响。
	auto Spec = ASC->MakeOutgoingSpec(UHodgeDarkFlowEffect::StaticClass(), 1.f, ASC->MakeEffectContext());
	if (Spec.IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(HodgeHavocTags::DarkFlowDelta, -Resource->GetDarkFlow());
		ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}

void UHodgeGameplayAbility_HavocRules::OnAvatarUninitialized(TWeakObjectPtr<AActor> ForAvatar)
{
	if (ForAvatar != BoundAvatar) { return; }
	EndDarkSurge(Session.Id);
	ClearDarkFlow();
	BoundAvatar.Reset();
	ProcGate.Reset();
}

void UHodgeGameplayAbility_HavocRules::BindAvatar()
{
	if (BoundExtension.IsValid()) { BoundExtension->UnregisterAbilitySystemDelegates(this); }
	BoundAvatar = GetAvatarActorFromActorInfo();
	BoundExtension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(BoundAvatar.Get());
	if (BoundExtension.IsValid())
	{
		BoundExtension->OnAbilitySystemUninitialized_Register(
			FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAvatarUninitialized, BoundAvatar));
	}
}

void UHodgeGameplayAbility_HavocRules::ActivateAbility(FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* Info,
                                                       FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* Event)
{
	Super::ActivateAbility(Handle, Info, ActivationInfo, Event);
	Unbind();
	BoundASC = GetAbilitySystemComponentFromActorInfo();
	BoundAvatar = GetAvatarActorFromActorInfo();
	BindAvatar();
	if (auto* ASC = BoundASC.Get())
	{
		FuelChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(UHodgeHavocSet::GetDarkFlowAttribute()).
		                         AddUObject(this, &ThisClass::OnFuelChanged);
		RemovedHandle = ASC->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &ThisClass::OnEffectRemoved);
		DeathHandle = ASC->RegisterGameplayTagEvent(HodgeGameplayTags::Status_Death).AddUObject(
			this, &ThisClass::OnDeathTagChanged);
		// 晚授予规则能力也要补查持久死亡状态，不能仅依赖后续事件。
		OnDeathTagChanged(HodgeGameplayTags::Status_Death, ASC->GetTagCount(HodgeGameplayTags::Status_Death));
	}
}

void UHodgeGameplayAbility_HavocRules::OnPawnAvatarSet()
{
	if (IsActive() && BoundAvatar.Get() != GetAvatarActorFromActorInfo())
	{
		EndDarkSurge(Session.Id);
		if (BoundAvatar.IsValid()) { ClearDarkFlow(); }
		BindAvatar();
		ProcGate.Reset();
	}
	Super::OnPawnAvatarSet();
}

void UHodgeGameplayAbility_HavocRules::Unbind()
{
	if (auto* ASC = BoundASC.Get())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UHodgeHavocSet::GetDarkFlowAttribute()).Remove(FuelChangedHandle);
		ASC->OnAnyGameplayEffectRemovedDelegate().Remove(RemovedHandle);
		ASC->RegisterGameplayTagEvent(HodgeGameplayTags::Status_Death).Remove(DeathHandle);
	}
	if (BoundExtension.IsValid()) { BoundExtension->UnregisterAbilitySystemDelegates(this); }
	BoundExtension.Reset();
	DeathHandle.Reset();
	FuelChangedHandle.Reset();
	RemovedHandle.Reset();
	BoundASC.Reset();
	BoundAvatar.Reset();
	ProcGate.Reset();
}

void UHodgeGameplayAbility_HavocRules::EndAbility(FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* Info,
                                                  FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate,
                                                  bool bCancelled)
{
	EndDarkSurge(Session.Id);
	ClearDarkFlow();
	Unbind();
	Super::EndAbility(Handle, Info, ActivationInfo, bReplicate, bCancelled);
}
