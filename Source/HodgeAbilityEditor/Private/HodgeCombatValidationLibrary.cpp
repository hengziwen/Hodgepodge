#include "HodgeCombatValidationLibrary.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Editor.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "Character/HodgeEnemyCharacter.h"
#include "Engine/StaticMeshActor.h"
#include "Animation/Skeleton.h"
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

namespace
{
	bool AddAuthoringNotify(UAnimMontage* Montage, UObject* Notify, float Start, float End)
	{
		if (!Montage || !Notify || Start < 0.f || Start >= Montage->GetPlayLength() || End > Montage->GetPlayLength() || End < Start) { return false; }
		Montage->Modify();
		FAnimNotifyEvent Event;
		Event.Notify = Cast<UAnimNotify>(Notify);
		Event.NotifyStateClass = Cast<UAnimNotifyState>(Notify);
		if (Event.NotifyStateClass && End <= Start) { return false; }
		Event.Guid = FGuid::NewGuid();
		Event.NotifyName = Notify->GetFName();
		Event.TrackIndex = Montage->AnimNotifyTracks.Num();
		Event.TriggerWeightThreshold = 0.f;
		Event.bTriggerOnDedicatedServer = true;
		Event.Link(Montage, Start);
		Event.TriggerTimeOffset = GetTriggerTimeOffsetForType(Montage->CalculateOffsetForNotify(Start));
		if (Event.NotifyStateClass)
		{
			Event.SetDuration(End - Start);
			Event.EndLink.Link(Montage, End);
			Event.EndTriggerTimeOffset = GetTriggerTimeOffsetForType(Montage->CalculateOffsetForNotify(End));
		}
		FAnimNotifyTrack Track;
		Track.TrackName = Notify->GetFName();
		Montage->AnimNotifyTracks.Add(Track);
		if (Event.Notify) { Event.Notify->OnAnimNotifyCreatedInEditor(Event); }
		if (Event.NotifyStateClass) { Event.NotifyStateClass->OnAnimNotifyCreatedInEditor(Event); }
		Montage->Notifies.Add(Event);
		Montage->RefreshCacheData();
		Montage->MarkPackageDirty();
		return true;
	}
}

bool UHodgeCombatValidationLibrary::AddHitNotify(UAnimMontage* Montage, float Start, float End, const FHodgeAnimHitConfig& Config, bool bSingle)
{
	if (!Montage) { return false; }
	if (bSingle)
	{
		auto* Notify = NewObject<UHodgeAnimNotify_Hit>(Montage, NAME_None, RF_Transactional);
		Notify->Hit = Config;
		return AddAuthoringNotify(Montage, Notify, Start, Start);
	}
	auto* Notify = NewObject<UHodgeAnimNotifyState_HitCheck>(Montage, NAME_None, RF_Transactional);
	Notify->Hit = Config;
	return AddAuthoringNotify(Montage, Notify, Start, End);
}

bool UHodgeCombatValidationLibrary::AddStateNotify(UAnimMontage* Montage, float Start, float End, FGameplayTag Tag)
{
	if (!Montage || !Tag.IsValid()) { return false; }
	auto* Notify = NewObject<UHodgeAnimNotifyState_GameplayTag>(Montage, NAME_None, RF_Transactional);
	Notify->StateTag = Tag;
	return AddAuthoringNotify(Montage, Notify, Start, End);
}

bool UHodgeCombatValidationLibrary::AddWeaponHandNotify(UAnimMontage* Montage, float Start, float End)
{
	if (!Montage) { return false; }
	return AddAuthoringNotify(Montage, NewObject<UHodgeAnimNotifyState_WeaponHand>(Montage, NAME_None, RF_Transactional), Start, End);
}

FGameplayTagContainer UHodgeCombatValidationLibrary::InspectAbilityWindows(UHodgeGameplayAbility_Definition* Ability)
{
	return Ability ? Ability->GetExecutionWindows() : FGameplayTagContainer();
}
int32 UHodgeCombatValidationLibrary::InspectHitSessions(AActor* Avatar)
{
	const auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(Avatar);
	return Combat ? Combat->GetDetectionSessionCount() : 0;
}
int32 UHodgeCombatValidationLibrary::InspectPoseLeases(AActor* Avatar)
{
	const auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(Avatar);
	return Combat ? Combat->GetPoseLeaseCount() : 0;
}

FHodgeMontageValidationState UHodgeCombatValidationLibrary::InspectMontageState(AActor* Avatar, UAnimMontage* Montage)
{
	FHodgeMontageValidationState Result;
	const auto* Character = Cast<ACharacter>(Avatar);
	UAnimInstance* Anim = Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (!Anim) { return Result; }
	const int32 Machine = Anim->GetStateMachineIndex(TEXT("LocomotionSM"));
	if (Machine != INDEX_NONE) { Result.LocomotionState = Anim->GetCurrentStateName(Machine); }
	for (int32 Index = Anim->MontageInstances.Num() - 1; Index >= 0; --Index)
	{
		const FAnimMontageInstance* Instance = Anim->MontageInstances[Index];
		if (!Instance || Instance->Montage != Montage) { continue; }
		Result.InstanceId = Instance->GetInstanceID();
		Result.Position = Instance->GetPosition();
		Result.Weight = Instance->GetWeight();
		Result.DesiredWeight = Instance->GetDesiredWeight();
		Result.BlendTime = Instance->GetBlendTime();
		Result.bPlaying = Instance->IsPlaying();
		Result.bStopped = Instance->IsStopped();
		break;
	}
	return Result;
}

bool UHodgeCombatValidationLibrary::ConfigureValidationPIE(int32 Players, bool bDedicated)
{
	if (!GEditor || GEditor->PlayWorld || Players < 1 || Players > 2) { return false; }
	auto* Settings = GetMutableDefault<ULevelEditorPlaySettings>();
	Settings->SetPlayNumberOfClients(Players);
	Settings->SetRunUnderOneProcess(true);
	Settings->SetPlayNetMode(bDedicated ? PIE_Client : (Players > 1 ? PIE_ListenServer : PIE_Standalone));
	return true;
}

bool UHodgeCombatValidationLibrary::ConfigureValidationSections(UAnimMontage* Montage)
{
	if (!Montage || !Montage->GetPathName().StartsWith(TEXT("/Game/CodexText/AnimNotifyCombat/")) || Montage->GetPlayLength() <= .5f) { return false; }
	Montage->Modify();
	Montage->CompositeSections.Reset();
	FCompositeSection First;
	First.SectionName = TEXT("Default");
	First.NextSectionName = TEXT("SecondHalf");
	First.Link(Montage, 0.f);
	FCompositeSection Second;
	Second.SectionName = TEXT("SecondHalf");
	Second.Link(Montage, .5f);
	Montage->CompositeSections.Add(First);
	Montage->CompositeSections.Add(Second);
	Montage->MarkPackageDirty();
	return true;
}

bool UHodgeCombatValidationLibrary::NormalizeCombatNotifies(UAnimMontage* Montage)
{
	if (!Montage) { return false; }
	Montage->Modify();
	for (FAnimNotifyEvent& Event : Montage->Notifies)
	{
		UAnimNotify* Point = Event.Notify.Get();
		UAnimNotifyState* State = Event.NotifyStateClass.Get();
		if (Point && (Point->IsA<UHodgeAnimNotify_Hit>() || Point->IsA<UHodgeAnimNotify_GameplayEvent>())) { Point->OnAnimNotifyCreatedInEditor(Event); }
		if (State && (State->IsA<UHodgeAnimNotifyState_HitCheck>() || State->IsA<UHodgeAnimNotifyState_GameplayTag>() || State->IsA<UHodgeAnimNotifyState_WeaponHand>())) { State->OnAnimNotifyCreatedInEditor(Event); }
	}
	Montage->RefreshCacheData();
	Montage->MarkPackageDirty();
	return true;
}

bool UHodgeCombatValidationLibrary::ConfigureHitReactionValidationMontage(UAnimMontage* Montage, FName SlotName)
{
	if (!Montage || SlotName.IsNone() || !Montage->GetPathName().StartsWith(TEXT("/Game/CodexText/HitReactionValidation/"))) { return false; }
	Montage->Modify();
	Montage->Notifies.Reset();
	for (auto& Track : Montage->SlotAnimTracks) { Track.SlotName = SlotName; }
	Montage->RefreshCacheData();
	Montage->MarkPackageDirty();
	return true;
}

AActor* UHodgeCombatValidationLibrary::SpawnHitReactionValidationActor(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FVector Location)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	if (!World || World->WorldType != EWorldType::PIE || World->GetNetMode() == NM_Client || !ActorClass || Location.ContainsNaN()) { return nullptr; }
	const bool bFixtureEnemy = ActorClass->IsChildOf(AHodgeEnemyCharacter::StaticClass()) &&
		ActorClass->GetPathName().StartsWith(TEXT("/Game/CodexText/HitReactionValidation/"));
	if (!bFixtureEnemy && ActorClass != AStaticMeshActor::StaticClass()) { return nullptr; }
	FActorSpawnParameters Parameters;
	Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Actor = World->SpawnActor<AActor>(ActorClass, Location, FRotator::ZeroRotator, Parameters);
	if (Actor) { Actor->SetReplicates(true); }
	return Actor;
}
