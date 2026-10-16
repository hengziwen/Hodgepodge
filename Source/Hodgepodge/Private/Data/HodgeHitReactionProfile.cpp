/** @file HodgeHitReactionProfile.cpp
 * @brief 实现目标受击动画与恢复配置相关的类型、接口与现有处理流程。
 */
#include "Data/HodgeHitReactionProfile.h"
#include "Animation/AnimMontage.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHitReactionProfile)

/** 查找动画。 */
const FHodgeHitReactionAnimation* UHodgeHitReactionProfile::FindAnimation(EHodgeImpactType Type) const
{
	return Animations.FindByPredicate([Type](const auto& Entry) { return Entry.Type == Type; });
}

/** 选用局部来源最主要的水平轴；退化方向按正面处理。 */
UAnimMontage* UHodgeHitReactionProfile::FindLightFeedback(FVector LocalSourceDirection) const
{
	const auto Direction = FMath::Abs(LocalSourceDirection.X) >= FMath::Abs(LocalSourceDirection.Y) ?
		(LocalSourceDirection.X >= 0.f ? EHodgeHitPresentationDirection::Front : EHodgeHitPresentationDirection::Back) :
		(LocalSourceDirection.Y >= 0.f ? EHodgeHitPresentationDirection::Right : EHodgeHitPresentationDirection::Left);
	const auto* Entry = DirectionalLightFeedback.FindByPredicate([Direction](const auto& Value) { return Value.Direction == Direction; });
	return Entry ? Entry->Montage.Get() : LightFeedbackMontage.Get();
}

/** 检查当前配置并收集诊断。 */
bool UHodgeHitReactionProfile::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	TSet<EHodgeImpactType> Types;
	for (const auto& Entry : Animations)
	{
		const bool bGroundControl = Entry.Type == EHodgeImpactType::HitStun || Entry.Type == EHodgeImpactType::Knockback;
		if (uint8(Entry.Type) > uint8(EHodgeImpactType::Slam) || Types.Contains(Entry.Type) ||
			(bGroundControl && (!FMath::IsFinite(Entry.DefaultDuration) || Entry.DefaultDuration <= 0.f)) || (!Entry.Montage && !Entry.bAllowWithoutMontage))
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
	TSet<EHodgeHitPresentationDirection> Directions;
	for (const auto& Entry : DirectionalLightFeedback)
	{
		if (uint8(Entry.Direction) > uint8(EHodgeHitPresentationDirection::Right) || Directions.Contains(Entry.Direction) ||
			!Entry.Montage || Entry.Montage->HasRootMotion() || !Entry.Montage->IsValidAdditive() || Entry.Montage->GetPlayLength() <= 0.f)
		{ Errors.Add(FText::FromString(TEXT("Directional feedback requires unique directions and valid in-place additive montages."))); }
		Directions.Add(Entry.Direction);
	}
	return Before == Errors.Num();
}

#if WITH_EDITOR
/** 在编辑器校验当前配置并收集诊断。 */
EDataValidationResult UHodgeHitReactionProfile::IsDataValid(FDataValidationContext& Context) const
{
	const auto Parent = Super::IsDataValid(Context);
	TArray<FText> Errors;
	Validate(Errors);
	for (const auto& Error : Errors) { Context.AddError(Error); }
	return Errors.IsEmpty() && Parent != EDataValidationResult::Invalid ? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}
#endif
