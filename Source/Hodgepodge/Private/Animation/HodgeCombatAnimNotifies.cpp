#include "Animation/HodgeCombatAnimNotifies.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/ActiveMontageInstanceScope.h"
#include "Animation/AnimNotifyQueue.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCombatAnimNotifies)

namespace
{
	UHodgeGameplayAbility_Definition* ResolveExecution(const FBranchingPointNotifyPayload& Payload)
	{
		if (!Payload.SkelMeshComponent || !Payload.NotifyEvent) { return nullptr; }
		auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Payload.SkelMeshComponent->GetOwner());
		auto* GA = ASC ? Cast<UHodgeGameplayAbility_Definition>(ASC->GetAnimatingAbility()) : nullptr;
		UE_LOG(LogTemp, Verbose,
		       TEXT("[HodgeNotify] Resolve %s Authority=%d Instance=%d Source=%s Ability=%s Accept=%d"),
		       *GetNameSafe(Payload.NotifyEvent->Notify ? static_cast<UObject*>(Payload.NotifyEvent->Notify.Get()) :
			       static_cast<UObject*>(Payload.NotifyEvent->NotifyStateClass.Get())),
		       Payload.SkelMeshComponent->GetOwner()->HasAuthority(), Payload.MontageInstanceID,
		       *GetNameSafe(Payload.SequenceAsset), *GetNameSafe(GA), GA && GA->AcceptsNotify(Payload));
		return GA && GA->AcceptsNotify(Payload) ? GA : nullptr;
	}
}

FHodgeAnimHitConfig::FHodgeAnimHitConfig()
{
	WeaponSourceTag = HodgeGameplayTags::Combat_Source_Weapon_MainHand;
}

bool FHodgeAnimHitConfig::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	auto Error = [&Errors](const TCHAR* Message) { Errors.Add(FText::FromString(Message)); };
	if (Source == EHodgeAnimHitSource::CharacterMeshSocket && BoneOrSocket.IsNone())
	{
		Error(TEXT("CharacterMeshSocket requires BoneOrSocket."));
	}
	if (Source == EHodgeAnimHitSource::EquippedWeapon && !WeaponSourceTag.IsValid())
	{
		Error(TEXT("EquippedWeapon requires WeaponSourceTag."));
	}
	if (Source == EHodgeAnimHitSource::NamedComponent && ComponentName.IsNone())
	{
		Error(TEXT("NamedComponent requires ComponentName."));
	}
	if (Source == EHodgeAnimHitSource::ExecutionAnchor && AnchorKey.IsNone())
	{
		Error(TEXT("ExecutionAnchor requires AnchorKey."));
	}
	if ((Source == EHodgeAnimHitSource::ExecutionTarget || TargetPolicy != EHodgeHitTargetPolicy::AnyInVolume) &&
		TargetKey.IsNone()) { Error(TEXT("Selected targets require TargetKey.")); }
	if (!AttackPhase.IsNone() && HitGroup.IsNone()) { Error(TEXT("AttackPhase requires an explicit HitGroup.")); }
	if (LocalTransform.ContainsNaN() || !LocalTransform.GetRotation().IsNormalized() || !LocalTransform.GetScale3D().
		Equals(FVector::OneVector)) { Error(TEXT("LocalTransform must be finite, normalized and unit scale.")); }
	if (!FMath::IsFinite(Radius) || Radius <= 0.f || BoxHalfExtent.ContainsNaN() || BoxHalfExtent.GetMin() <= 0.f ||
		!FMath::IsFinite(CapsuleHalfHeight) || (Shape == EHodgeHitShape::Capsule && CapsuleHalfHeight < Radius) ||
		!FMath::IsFinite(DamageScale) || DamageScale < 0.f || !FMath::IsFinite(RepeatHitInterval) || RepeatHitInterval <
		0.f ||
		!FMath::IsFinite(MaxTargetDistance) || MaxTargetDistance <= 0.f || !FMath::IsFinite(MaxAnchorDistance) ||
		MaxAnchorDistance <= 0.f)
	{
		Error(TEXT("Invalid hit dimensions, multiplier, interval or distance."));
	}
	if (Profile) { Profile->Validate(Errors); }
	return Errors.Num() == Before;
}

UHodgeAnimNotifyState_HitCheck::UHodgeAnimNotifyState_HitCheck() { bIsNativeBranchingPoint = true; }

UHodgeAnimNotify_Hit::UHodgeAnimNotify_Hit()
{
	bIsNativeBranchingPoint = true;
	Hit.Source = EHodgeAnimHitSource::AvatarRoot;
}

UHodgeAnimNotifyState_GameplayTag::UHodgeAnimNotifyState_GameplayTag() { bIsNativeBranchingPoint = true; }
UHodgeAnimNotifyState_WeaponHand::UHodgeAnimNotifyState_WeaponHand() { bIsNativeBranchingPoint = true; }
UHodgeAnimNotify_GameplayEvent::UHodgeAnimNotify_GameplayEvent() { bIsNativeBranchingPoint = true; }

void UHodgeAnimNotifyState_HitCheck::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = Cast<UHodgeGameplayAbility_Melee>(ResolveExecution(Payload)))
	{
		const int32 Occurrence = GA->BeginNotifyResource(Payload);
		if (Occurrence != INDEX_NONE) { GA->BeginNotifyHit(Occurrence, Hit, Payload.SkelMeshComponent, false); }
	}
}

void UHodgeAnimNotifyState_HitCheck::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = ResolveExecution(Payload)) { GA->EndNotifyResource(Payload); }
}

void UHodgeAnimNotify_Hit::BranchingPointNotify(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = Cast<UHodgeGameplayAbility_Melee>(ResolveExecution(Payload)))
	{
		const int32 Occurrence = GA->AllocateNotifyOccurrence();
		UE_LOG(LogTemp, Verbose, TEXT("[HodgeNotify] Point %s World=%s Authority=%d Occurrence=%d"), *GetName(),
		       *GA->GetWorld()->GetName(), GA->GetAvatarActorFromActorInfo()->HasAuthority(), Occurrence);
		GA->BeginNotifyHit(Occurrence, Hit, Payload.SkelMeshComponent, true);
		const FGuid Execution = GA->GetExecutionId();
		for (const auto& Config : AdditionalHits)
		{
			if (!GA->IsActive() || GA->GetExecutionId() != Execution) { break; }
			GA->BeginNotifyHit(GA->AllocateNotifyOccurrence(), Config, Payload.SkelMeshComponent, true);
		}
	}
}

void UHodgeAnimNotifyState_GameplayTag::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = ResolveExecution(Payload))
	{
		const int32 Occurrence = GA->BeginNotifyResource(Payload);
		if (Occurrence != INDEX_NONE) { GA->AcquireNotifyTag(Occurrence, StateTag); }
	}
}

void UHodgeAnimNotifyState_GameplayTag::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = ResolveExecution(Payload)) { GA->EndNotifyResource(Payload); }
}

void UHodgeAnimNotifyState_WeaponHand::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = ResolveExecution(Payload))
	{
		const int32 Occurrence = GA->BeginNotifyResource(Payload);
		if (Occurrence != INDEX_NONE) { GA->AcquireNotifyWeapon(Occurrence); }
	}
}

void UHodgeAnimNotifyState_WeaponHand::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = ResolveExecution(Payload)) { GA->EndNotifyResource(Payload); }
}

void UHodgeAnimNotify_GameplayEvent::BranchingPointNotify(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = ResolveExecution(Payload)) { GA->SendExecutionEvent(EventTag); }
}

namespace
{
	FBranchingPointNotifyPayload MontagePayload(USkeletalMeshComponent* Mesh,
	                                            const FAnimNotifyEventReference& Reference)
	{
		const auto* Context = Reference.GetContextData<UE::Anim::FAnimNotifyMontageInstanceContext>();
		return FBranchingPointNotifyPayload(
			Mesh, const_cast<UAnimSequenceBase*>(Cast<UAnimSequenceBase>(Reference.GetSourceObject())),
			const_cast<FAnimNotifyEvent*>(Reference.GetNotify()), Context ? Context->MontageInstanceID : INDEX_NONE);
	}
}

void UHodgeAnimNotifyState_HitCheck::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                 float TotalDuration, const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotifyBegin(Payload);
}

void UHodgeAnimNotifyState_HitCheck::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                               const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotifyEnd(Payload);
}

void UHodgeAnimNotifyState_GameplayTag::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                    float TotalDuration, const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotifyBegin(Payload);
}

void UHodgeAnimNotifyState_GameplayTag::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                  const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotifyEnd(Payload);
}

void UHodgeAnimNotifyState_WeaponHand::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                   float TotalDuration, const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotifyBegin(Payload);
}

void UHodgeAnimNotifyState_WeaponHand::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                 const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotifyEnd(Payload);
}

void UHodgeAnimNotify_Hit::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                  const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotify(Payload);
}

void UHodgeAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                            const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotify(Payload);
}

#if WITH_EDITOR
namespace
{
	void ConfigureGameplayNotify(FAnimNotifyEvent& Event)
	{
		// 玩法时机不随姿势混合、LOD、随机概率或动画图事件过滤消失。
		Event.MontageTickType = EMontageNotifyTickType::BranchingPoint;
		Event.TriggerWeightThreshold = 0.f;
		Event.NotifyTriggerChance = 1.f;
		Event.NotifyFilterType = ENotifyFilterType::NoFiltering;
		Event.bCanBeFilteredViaRequest = false;
		Event.bTriggerOnDedicatedServer = true;
		Event.bTriggerOnFollower = true;
	}
}

void UHodgeAnimNotifyState_HitCheck::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)
{
	ConfigureGameplayNotify(Event);
}

void UHodgeAnimNotify_Hit::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) { ConfigureGameplayNotify(Event); }

void UHodgeAnimNotifyState_GameplayTag::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)
{
	ConfigureGameplayNotify(Event);
}

void UHodgeAnimNotifyState_WeaponHand::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)
{
	ConfigureGameplayNotify(Event);
}

void UHodgeAnimNotify_GameplayEvent::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)
{
	ConfigureGameplayNotify(Event);
}
#endif
