// Copyright Epic Games, Inc. All Rights Reserved.
#include "AbilitySystem/Abilities/HodgeGameplayAbility_AutoRespawn.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Character/HodgeHeroCharacter.h"
#include "Component/HodgeHealthComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Core/GameMode/HodgeGameModeBase.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Data/HodgePawnData.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_AutoRespawn)

UHodgeGameplayAbility_AutoRespawn::UHodgeGameplayAbility_AutoRespawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Lyra 的客户端倒计时消息不在首版范围，生成调度只保留权威执行端。
	ActivationPolicy = EHodgeAbilityActivationPolicy::OnSpawn;
	ActivationGroup = EHodgeAbilityActivationGroup::Independent;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;
	FGameplayTagContainer Tags = GetAssetTags();
	Tags.AddTag(HodgeGameplayTags::Ability_Behavior_SurvivesDeath);
	Tags.AddTag(HodgeGameplayTags::Ability_Type_Passive_AutoRespawn);
	SetAssetTags(Tags);
}

void UHodgeGameplayAbility_AutoRespawn::ActivateAbility(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event)
{
	Super::ActivateAbility(Handle, Info, ActivationInfo, Event);
	BindAvatar();
}

void UHodgeGameplayAbility_AutoRespawn::OnPawnAvatarSet()
{
	if (IsActive()) { BindAvatar(); }
	Super::OnPawnAvatarSet();
}

void UHodgeGameplayAbility_AutoRespawn::BindAvatar()
{
	// 与 Lyra/Havoc 被动能力一致，重新绑定前先清除本实例上一代资源。
	Unbind();
	auto* Pawn = Cast<AHodgeHeroCharacter>(GetAvatarActorFromActorInfo());
	auto* PS = Cast<AHodgePlayerState>(GetOwningActorFromActorInfo());
	auto* Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(Pawn);
	const auto* Data = Extension ? Extension->GetPawnData<UHodgePawnData>() : nullptr;
	auto* ASC = GetHodgeAbilitySystemComponentFromActorInfo();
	auto* PC = PS ? Cast<APlayerController>(PS->GetOwner()) : nullptr;
	auto* Health = UHodgeHealthComponent::FindHealthComponent(Pawn);
	if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority() || !Pawn || !PS || !PC || !Health ||
		!ASC || ASC != PS->GetHodgeAbilitySystemComponent() || !Data || !Data->bEnableAutoRespawn) { return; }
	FString Error;
	if (!Data->ValidateAutoRespawn(Error))
	{
		UE_LOG(LogTemp, Error, TEXT("AutoRespawn invalid config %s: %s"), *GetPathNameSafe(Data), *Error);
		return;
	}
	BoundAvatar = Pawn;
	BoundPlayerState = PS;
	BoundController = PC;
	BoundASC = ASC;
	BoundHealth = Health;
	BoundExtension = Extension;
	BoundData = Data;
	Delay = Data->AutoRespawnDelay;
	MaxAttempts = Data->AutoRespawnMaxAttempts;
	RetryInterval = Data->AutoRespawnRetryInterval;
	Health->OnDeathFinished.AddUniqueDynamic(this, &ThisClass::OnDeathFinished);
	Pawn->OnEndPlay.AddUniqueDynamic(this, &ThisClass::OnAvatarEndPlay);
	Extension->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(
		this, &ThisClass::OnAvatarUninitialized, BoundAvatar));
	// 晚绑定必须补查已完成的死亡，不能只等待下一次事件。
	if (Health->GetDeathState() == EHodgeDeathState::DeathFinished) { OnDeathFinished(Pawn); }
}

void UHodgeGameplayAbility_AutoRespawn::OnDeathFinished(AActor* Actor)
{
	if (!BoundAvatar.HasSameIndexAndSerialNumber(TWeakObjectPtr<APawn>(Cast<APawn>(Actor))) || bDeathCompleted) { return; }
	bDeathCompleted = true;
	TryQueueRespawn();
}

void UHodgeGameplayAbility_AutoRespawn::OnAvatarUninitialized(TWeakObjectPtr<APawn> Avatar)
{
	if (!BoundAvatar.HasSameIndexAndSerialNumber(Avatar)) { return; }
	// 此广播仍处于旧组件清理栈，实际生成必须延迟到后续世界 Tick。
	bTeardownObserved = true;
	TryQueueRespawn();
}

void UHodgeGameplayAbility_AutoRespawn::OnAvatarEndPlay(AActor* Actor, EEndPlayReason::Type Reason)
{
	if (!BoundAvatar.HasSameIndexAndSerialNumber(TWeakObjectPtr<APawn>(Cast<APawn>(Actor)))) { return; }
	if (Reason != EEndPlayReason::Destroyed || !bDeathCompleted)
	{
		// 切图、退出、正常换角色与意外销毁均不能被误当成一次死亡重生。
		Unbind();
		return;
	}
	bTeardownObserved = true;
	TryQueueRespawn();
}

void UHodgeGameplayAbility_AutoRespawn::TryQueueRespawn()
{
	if (!IsActive() || !bDeathCompleted || !bTeardownObserved || bRequestStarted) { return; }
	bRequestStarted = true;
	UE_LOG(LogTemp, Display, TEXT("AutoRespawn queued PS=%s generation=%llu delay=%.3f attempts=%d interval=%.3f"),
		*GetNameSafe(BoundPlayerState.Get()), Generation, Delay, MaxAttempts, RetryInterval);
	ScheduleAttempt(Delay);
}

void UHodgeGameplayAbility_AutoRespawn::ScheduleAttempt(float Seconds)
{
	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown) { Unbind(); return; }
	const FTimerDelegate Callback = FTimerDelegate::CreateUObject(this, &ThisClass::AttemptRespawn, Generation);
	if (Seconds > 0.f) { World->GetTimerManager().SetTimer(RespawnTimer, Callback, Seconds, false); }
	else { RespawnTimer = World->GetTimerManager().SetTimerForNextTick(Callback); }
}

bool UHodgeGameplayAbility_AutoRespawn::IsRequestCurrent(uint64 ForGeneration) const
{
	const auto* PC = BoundController.Get();
	const auto* PS = BoundPlayerState.Get();
	const auto* ASC = BoundASC.Get();
	const auto* Data = BoundData.Get();
	const UWorld* World = GetWorld();
	return IsActive() && Generation == ForGeneration && bRequestStarted && bDeathCompleted && bTeardownObserved &&
		World && !World->bIsTearingDown && World->GetNetMode() != NM_Client && IsValid(PC) && IsValid(PS) && ASC &&
		PC->GetWorld() == World && PC->GetPlayerState<AHodgePlayerState>() == PS && PS->GetOwner() == PC &&
		PS->GetHodgeAbilitySystemComponent() == ASC && ASC->GetOwnerActor() == PS && !PC->GetPawn() &&
		!ASC->GetAvatarActor() && Data && Data->bEnableAutoRespawn && PS->GetPawnData<UHodgePawnData>() == Data;
}

void UHodgeGameplayAbility_AutoRespawn::AttemptRespawn(uint64 ForGeneration)
{
	// 旧调用不能清新代际的 Timer；身份变化只退休仍属于本代的请求。
	if (Generation != ForGeneration) { return; }
	RespawnTimer.Invalidate();
	if (!IsRequestCurrent(ForGeneration)) { Unbind(); return; }
	auto* PC = BoundController.Get();
	auto* GameMode = GetWorld()->GetAuthGameMode<AHodgeGameModeBase>();
	if (!GameMode || !GameMode->PlayerCanRestart(PC))
	{
		UE_LOG(LogTemp, Warning, TEXT("AutoRespawn stopped: restart no longer allowed PS=%s generation=%llu"),
			*GetNameSafe(BoundPlayerState.Get()), ForGeneration);
		Unbind();
		return;
	}
	++Attempts;
	UE_LOG(LogTemp, Display, TEXT("AutoRespawn attempt PS=%s generation=%llu attempt=%d/%d"),
		*GetNameSafe(BoundPlayerState.Get()), ForGeneration, Attempts, MaxAttempts);
	const bool bSpawned = GameMode->TryRestartPlayerForAutoRespawn(PC);
	// Possess 可以同步绑定新 Avatar；返回的旧栈不能清理新绑定或再次重试。
	if (Generation != ForGeneration || !IsActive()) { return; }
	if (bSpawned || !IsRequestCurrent(ForGeneration)) { Unbind(); return; }
	if (Attempts >= MaxAttempts)
	{
		UE_LOG(LogTemp, Error, TEXT("AutoRespawn exhausted PS=%s generation=%llu attempts=%d"),
			*GetNameSafe(BoundPlayerState.Get()), ForGeneration, Attempts);
		Unbind();
		return;
	}
	ScheduleAttempt(RetryInterval);
}

void UHodgeGameplayAbility_AutoRespawn::Unbind()
{
	++Generation;
	if (UWorld* World = GetWorld()) { World->GetTimerManager().ClearTimer(RespawnTimer); }
	if (auto* Health = BoundHealth.Get()) { Health->OnDeathFinished.RemoveDynamic(this, &ThisClass::OnDeathFinished); }
	if (auto* Pawn = BoundAvatar.Get()) { Pawn->OnEndPlay.RemoveDynamic(this, &ThisClass::OnAvatarEndPlay); }
	if (auto* Extension = BoundExtension.Get()) { Extension->UnregisterAbilitySystemDelegates(this); }
	BoundAvatar.Reset();
	BoundPlayerState.Reset();
	BoundController.Reset();
	BoundASC.Reset();
	BoundHealth.Reset();
	BoundExtension.Reset();
	BoundData.Reset();
	Attempts = 0;
	bDeathCompleted = false;
	bTeardownObserved = false;
	bRequestStarted = false;
}

void UHodgeGameplayAbility_AutoRespawn::EndAbility(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled)
{
	Unbind();
	Super::EndAbility(Handle, Info, ActivationInfo, bReplicate, bCancelled);
}

void UHodgeGameplayAbility_AutoRespawn::OnRemoveAbility(const FGameplayAbilityActorInfo* Info, const FGameplayAbilitySpec& Spec)
{
	Unbind();
	Super::OnRemoveAbility(Info, Spec);
}
