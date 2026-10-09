#include "Combat/HodgeHitReactionTypes.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/Skeleton.h"
#include "Engine/SkeletalMesh.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHitReactionTypes)

namespace HodgeHitReactionTags
{
	UE_DEFINE_GAMEPLAY_TAG(Body_Normal, "State.Combat.Body.Normal");
	UE_DEFINE_GAMEPLAY_TAG(Body_Skill, "State.Combat.Body.Skill");
	UE_DEFINE_GAMEPLAY_TAG(Body_SuperArmor, "State.Combat.Body.SuperArmor");
	UE_DEFINE_GAMEPLAY_TAG(Body_Vajra, "State.Combat.Body.Vajra");
	UE_DEFINE_GAMEPLAY_TAG(Judgement_Normal, "Combat.Attack.Judgement.Normal");
	UE_DEFINE_GAMEPLAY_TAG(Judgement_Skill, "Combat.Attack.Judgement.Skill");
	UE_DEFINE_GAMEPLAY_TAG(Judgement_SuperArmor, "Combat.Attack.Judgement.SuperArmor");
	UE_DEFINE_GAMEPLAY_TAG(Judgement_Vajra, "Combat.Attack.Judgement.Vajra");
	UE_DEFINE_GAMEPLAY_TAG(Controlled, "State.Combat.HitReaction.Controlled");
}

int32 HodgeHitReaction::BodyRank(FGameplayTag Tag)
{
	using namespace HodgeHitReactionTags;
	if (Tag == Body_Normal) { return 1; }
	if (Tag == Body_Skill) { return 2; }
	if (Tag == Body_SuperArmor) { return 3; }
	if (Tag == Body_Vajra) { return 4; }
	return 0;
}

int32 HodgeHitReaction::JudgementRank(FGameplayTag Tag)
{
	using namespace HodgeHitReactionTags;
	if (Tag == Judgement_Normal) { return 1; }
	if (Tag == Judgement_Skill) { return 2; }
	if (Tag == Judgement_SuperArmor) { return 3; }
	if (Tag == Judgement_Vajra) { return 4; }
	return BodyRank(Tag);
}

int32 HodgeHitReaction::ResolveBody(const UAbilitySystemComponent* ASC, FGameplayTag& OutTag)
{
	OutTag = FGameplayTag();
	if (!ASC) { return 0; }
	FGameplayTagContainer Tags;
	ASC->GetOwnedGameplayTags(Tags);
	int32 Rank = 0;
	for (FGameplayTag Tag : Tags)
	{
		const int32 Value = BodyRank(Tag);
		if (Value > Rank) { Rank = Value; OutTag = Tag; }
	}
	return Rank;
}

bool HodgeHitReaction::CanImpact(int32 AttackRank, int32 BodyRank)
{
	return AttackRank >= 1 && AttackRank <= 4 && BodyRank >= 0 && BodyRank <= 4 && AttackRank >= BodyRank;
}

bool HodgeHitReaction::CanUseMontage(const UAnimMontage* Montage, const USkeletalMesh* Mesh)
{
	const USkeleton* Skeleton = Montage ? Montage->GetSkeleton() : nullptr;
	return Skeleton && Mesh && (Skeleton == Mesh->GetSkeleton() || Skeleton->IsCompatibleMesh(Mesh));
}

bool FHodgeImpactSpec::IsMovement() const
{
	return Type == EHodgeImpactType::Knockback || Type == EHodgeImpactType::Launch ||
		Type == EHodgeImpactType::AirHit || Type == EHodgeImpactType::Slam;
}

bool FHodgeImpactSpec::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	if (uint8(Type) > uint8(EHodgeImpactType::Slam) || uint8(Direction) > uint8(EHodgeImpactDirection::WorldDirection) ||
		!FMath::IsFinite(ControlDuration) || ControlDuration < 0.f || !FMath::IsFinite(KnockbackDistance) || KnockbackDistance < 0.f ||
		!FMath::IsFinite(MoveDuration) || MoveDuration <= 0.f || !FMath::IsFinite(HorizontalSpeed) || HorizontalSpeed < 0.f ||
		!FMath::IsFinite(VerticalSpeed) || VerticalSpeed < 0.f || WorldDirection.ContainsNaN() ||
		(IsMovement() && Direction == EHodgeImpactDirection::WorldDirection && WorldDirection.GetSafeNormal2D().IsNearlyZero()))
	{
		Errors.Add(FText::FromString(TEXT("Impact requires valid enums, finite nonnegative magnitudes, positive move duration and a usable direction.")));
	}
	return Before == Errors.Num();
}

bool FHodgeHitReactionConfig::IsConfigured() const
{
	return AttackJudgementTag.IsValid() || !Impacts.IsEmpty() || ReservedPoiseDamage != 0.f;
}

bool FHodgeHitReactionConfig::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	if (!IsConfigured()) { return true; }
	if (HodgeHitReaction::JudgementRank(AttackJudgementTag) == 0)
	{
		Errors.Add(FText::FromString(TEXT("Reaction requires one exact Normal/Skill/SuperArmor/Vajra judgement tag.")));
	}
	if (!FMath::IsFinite(ReservedPoiseDamage) || ReservedPoiseDamage < 0.f ||
		uint8(HitAcceptancePolicy) > uint8(EHodgeHitAcceptancePolicy::ExplicitControl))
	{
		Errors.Add(FText::FromString(TEXT("Invalid reaction acceptance policy or reserved poise magnitude.")));
	}
	int32 MovementCount = 0;
	TSet<EHodgeImpactType> Types;
	for (const auto& Impact : Impacts)
	{
		Impact.Validate(Errors);
		MovementCount += Impact.IsMovement() ? 1 : 0;
		if (Types.Contains(Impact.Type)) { Errors.Add(FText::FromString(TEXT("Duplicate Impact type in one hit."))); }
		Types.Add(Impact.Type);
	}
	if (MovementCount > 1 || (MovementCount && Types.Contains(EHodgeImpactType::Knockdown)))
	{
		Errors.Add(FText::FromString(TEXT("One phase supports one movement Impact; use bKnockdownOnLanding for airborne-to-down transitions.")));
	}
	return Before == Errors.Num();
}
