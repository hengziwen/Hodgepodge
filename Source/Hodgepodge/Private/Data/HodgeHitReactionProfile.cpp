#include "Data/HodgeHitReactionProfile.h"
#include "Animation/AnimMontage.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHitReactionProfile)

const FHodgeHitReactionAnimation* UHodgeHitReactionProfile::FindAnimation(EHodgeImpactType Type) const
{
	return Animations.FindByPredicate([Type](const auto& Entry) { return Entry.Type == Type; });
}

bool UHodgeHitReactionProfile::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	TSet<EHodgeImpactType> Types;
	for (const auto& Entry : Animations)
	{
		if (uint8(Entry.Type) > uint8(EHodgeImpactType::Slam) || Types.Contains(Entry.Type) ||
			!FMath::IsFinite(Entry.DefaultDuration) || Entry.DefaultDuration <= 0.f || (!Entry.Montage && !Entry.bAllowWithoutMontage))
		{
			Errors.Add(FText::FromString(TEXT("Reaction entries require unique valid types, a positive duration and a montage or explicit animation-free permission.")));
		}
		Types.Add(Entry.Type);
		for (UAnimMontage* Montage : {Entry.Montage.Get(), Entry.AirLoopMontage.Get(), Entry.LandingMontage.Get(), Entry.GetUpMontage.Get()})
		{
			if (Montage && (Montage->GetPlayLength() <= 0.f || Montage->HasRootMotion()))
			{
				Errors.Add(FText::FromString(TEXT("V1 reaction montages must have a positive length and be in-place; Impact owns capsule movement.")));
			}
		}
	}
	for (auto Type : FallbackToHitStun)
	{
		if (uint8(Type) > uint8(EHodgeImpactType::Slam) || Type == EHodgeImpactType::HitStun || !FindAnimation(EHodgeImpactType::HitStun))
		{ Errors.Add(FText::FromString(TEXT("Fallback requires a valid effect type and a configured HitStun entry."))); }
	}
	if (!FMath::IsFinite(DownedDuration) || DownedDuration <= 0.f || !FMath::IsFinite(MaxControlDuration) || MaxControlDuration <= 0.f ||
		!FMath::IsFinite(MaxAirborneDuration) || MaxAirborneDuration <= 0.f || !FMath::IsFinite(MaxLaunchHeight) || MaxLaunchHeight <= 0.f ||
		!FMath::IsFinite(MaxLaunchSpeed) || MaxLaunchSpeed <= 0.f || MaxAirHits < 0)
	{ Errors.Add(FText::FromString(TEXT("Reaction bounds must be finite and positive; MaxAirHits must be nonnegative."))); }
	if (LightFeedbackMontage && LightFeedbackMontage->HasRootMotion())
	{ Errors.Add(FText::FromString(TEXT("Light feedback cannot contain root motion."))); }
	return Before == Errors.Num();
}

#if WITH_EDITOR
EDataValidationResult UHodgeHitReactionProfile::IsDataValid(FDataValidationContext& Context) const
{
	const auto Parent = Super::IsDataValid(Context);
	TArray<FText> Errors;
	Validate(Errors);
	for (const auto& Error : Errors) { Context.AddError(Error); }
	return Errors.IsEmpty() && Parent != EDataValidationResult::Invalid ? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}
#endif
