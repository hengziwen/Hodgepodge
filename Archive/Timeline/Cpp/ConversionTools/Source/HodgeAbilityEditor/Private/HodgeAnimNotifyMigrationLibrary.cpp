#include "HodgeAnimNotifyMigrationLibrary.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Data/HodgeAbilityTimeline.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "Animation/AnimMontage.h"
#include "GameplayEffect.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAnimNotifyMigrationLibrary)

namespace
{
	FHodgeAnimHitConfig ConvertHit(const FHodgeHitEffectConfig& Old, TSubclassOf<UGameplayEffect> Fallback, int32 Phase)
	{
		FHodgeAnimHitConfig New;
		New.Profile = Old.Profile;
		New.Damage = Old;
		New.bUseDefaultDamage = false;
		if (!New.Damage.DamageEffect) { New.Damage.DamageEffect = Fallback; }
		New.TargetPolicy = Old.TargetPolicy;
		New.TargetKey = Old.TargetKey;
		New.MaxTargetDistance = Old.MaxTargetDistance;
		New.RepeatHitInterval = Old.RepeatHitInterval;
		New.HitGroup = Old.HitGroup;
		New.AttackPhase = Old.HitGroupScope == EHodgeHitGroupScope::TriggerTime ? FName(*FString::Printf(TEXT("Phase_%d"), Phase)) : NAME_None;
		New.bAllowFriendlyFire = Old.bAllowFriendlyFire;
		New.Shape = Old.Volume.Shape;
		New.Radius = Old.Volume.Shape == EHodgeHitShape::Capsule ? Old.Volume.CapsuleRadius : Old.Volume.SphereRadius;
		New.BoxHalfExtent = Old.Volume.BoxHalfExtent;
		New.CapsuleHalfHeight = Old.Volume.CapsuleHalfHeight;
		New.LocalTransform = Old.Volume.LocalTransform;
		New.TransformPolicy = Old.Volume.TransformPolicy;
		New.AnchorKey = Old.Volume.AnchorKey;
		New.MaxAnchorDistance = Old.Volume.MaxAnchorDistance;
		if (Old.SourceTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Combat.Source.Weapon"), false)))
		{ New.Source = EHodgeAnimHitSource::EquippedWeapon; New.WeaponSourceTag = Old.SourceTag; }
		else if (Old.SourceTag == FGameplayTag::RequestGameplayTag(TEXT("Combat.Source.Body.LeftHand"), false))
		{ New.Source = EHodgeAnimHitSource::CharacterMeshSocket; New.BoneOrSocket = TEXT("Bip001LHand"); New.Radius = 15.f; }
		else if (Old.Volume.AnchorKind == EHodgeHitAnchorKind::ExecutionTransform) { New.Source = EHodgeAnimHitSource::ExecutionAnchor; }
		else if (Old.Volume.AnchorKind == EHodgeHitAnchorKind::ExecutionTarget) { New.Source = EHodgeAnimHitSource::ExecutionTarget; }
		else { New.Source = EHodgeAnimHitSource::AvatarRoot; }
		return New;
	}
	void AddEvent(UAnimMontage* Montage, UObject* Notify, float Start, float End, int32 Track)
	{
		FAnimNotifyEvent Event;
		Event.Notify = Cast<UAnimNotify>(Notify);
		Event.NotifyStateClass = Cast<UAnimNotifyState>(Notify);
		Event.NotifyName = Notify->GetFName();
		Event.TrackIndex = Track;
		Event.MontageTickType = EMontageNotifyTickType::Queued;
		Event.bTriggerOnDedicatedServer = true;
		Event.TriggerWeightThreshold = 0.f;
		Event.Guid = FGuid::NewGuid();
		Event.Link(Montage, Start);
		Event.TriggerTimeOffset = GetTriggerTimeOffsetForType(Montage->CalculateOffsetForNotify(Start));
		if (Event.NotifyStateClass)
		{
			Event.SetDuration(End - Start);
			Event.EndLink.Link(Montage, End);
			Event.EndTriggerTimeOffset = GetTriggerTimeOffsetForType(Montage->CalculateOffsetForNotify(End));
		}
		Montage->Notifies.Add(Event);
	}
}

bool UHodgeAnimNotifyMigrationLibrary::ConvertLegacyDefinition(UHodgeAbilityDefinition* Definition, TSubclassOf<UGameplayEffect> MissingDamageEffect)
{
	if (!Definition || !Definition->ExecutionConfig.Montage || !Definition->ExecutionConfig.TimelineTaskConfig.Timeline) { return false; }
	auto* Montage = Definition->ExecutionConfig.Montage.Get();
	const auto* Timeline = Definition->ExecutionConfig.TimelineTaskConfig.Timeline.Get();
	for (const auto& Event : Timeline->Events)
	{
		if (Event.WindowEffectClass || Event.PointEffectClass) { return false; }
		if (Event.StartTime >= Montage->GetPlayLength()) { return false; }
	}
	Definition->Modify();
	Montage->Modify();
	// 保留用户原有的动画通知，仅移除本工具上次生成的通知。
	Montage->Notifies.RemoveAll([](const FAnimNotifyEvent& Event)
	{ return (Event.Notify && Event.Notify->GetName().StartsWith(TEXT("ANMigration_"))) || (Event.NotifyStateClass && Event.NotifyStateClass->GetName().StartsWith(TEXT("ANMigration_"))); });
	TMap<float, int32> Phases;
	int32 Track = Montage->AnimNotifyTracks.Num();
	for (int32 Index = 0; Index < Timeline->Events.Num(); ++Index)
	{
		const auto& Event = Timeline->Events[Index];
		if (!Phases.Contains(Event.StartTime)) { Phases.Add(Event.StartTime, Phases.Num() + 1); }
		const float End = FMath::Min(Event.EndTime, Montage->GetPlayLength());
		const FName Name(*FString::Printf(TEXT("ANMigration_%d_%s"), Index, *FGuid::NewGuid().ToString(EGuidFormats::Digits)));
		UObject* Notify = nullptr;
		if (Event.Kind == EHodgeTimelineEventKind::Window)
		{
			if (const auto* Binding = Definition->HitWindows.FindByPredicate([&](const auto& Entry) { return Entry.WindowTag == Event.WindowTag; }))
			{
				auto* Hit = NewObject<UHodgeAnimNotifyState_HitCheck>(Montage, Name, RF_Transactional);
				Hit->Hit = ConvertHit(*Binding, MissingDamageEffect, Phases[Event.StartTime]);
				Notify = Hit;
				if (!Definition->DefaultHitConfig.DamageEffect)
				{
					Definition->DefaultHitConfig = Hit->Hit.Damage;
					Hit->Hit.bUseDefaultDamage = true;
				}
			}
			else if (Event.WindowTag == Definition->WeaponUseWindowTag)
			{ Notify = NewObject<UHodgeAnimNotifyState_WeaponHand>(Montage, Name, RF_Transactional); }
			else
			{
				auto* Tag = NewObject<UHodgeAnimNotifyState_GameplayTag>(Montage, Name, RF_Transactional);
				Tag->StateTag = Event.WindowTag;
				Notify = Tag;
			}
		}
		else
		{
			if (const auto* Binding = Definition->HitPoints.FindByPredicate([&](const auto& Entry) { return Entry.PointEventTag == Event.PointEventTag; }))
			{
				auto* Hit = NewObject<UHodgeAnimNotify_Hit>(Montage, Name, RF_Transactional);
				Hit->Hit = ConvertHit(*Binding, MissingDamageEffect, Phases[Event.StartTime]);
				Notify = Hit;
			}
			else
			{
				auto* Message = NewObject<UHodgeAnimNotify_GameplayEvent>(Montage, Name, RF_Transactional);
				Message->EventTag = Event.PointEventTag;
				Notify = Message;
			}
		}
		FAnimNotifyTrack NotifyTrack;
		NotifyTrack.TrackName = Event.EventID;
		Montage->AnimNotifyTracks.Add(NotifyTrack);
		AddEvent(Montage, Notify, Event.StartTime, End, Track++);
	}
	Montage->BlendOut = Definition->ExecutionConfig.NaturalBlendOut.MakeBlend();
	Montage->bEnableAutoBlendOut = true;
	Montage->RefreshCacheData();
	Montage->MarkPackageDirty();
	Definition->ExecutionConfig.TimelineTaskConfig.Timeline = nullptr;
	Definition->HitWindows.Reset();
	Definition->HitPoints.Reset();
	Definition->WeaponUseWindowTag = FGameplayTag();
	Definition->MarkPackageDirty();
	return true;
}
