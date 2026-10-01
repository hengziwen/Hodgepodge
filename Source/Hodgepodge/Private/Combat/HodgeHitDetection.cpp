#include "Combat/HodgeHitDetection.h"
#include "Components/BoxComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHitDetection)

namespace
{
	FCollisionObjectQueryParams MakeObjectQuery(const UHodgeHitDetectionProfile* Profile)
	{
		FCollisionObjectQueryParams Result;
		for (ECollisionChannel Channel : Profile->ObjectTypes)
		{
			Result.AddObjectTypesToQuery(Channel);
		}
		return Result;
	}
}

UHodgeHitDetectionProfile::UHodgeHitDetectionProfile()
{
	Strategy = UHodgeSocketSweepStrategy::StaticClass();
	ObjectTypes.Add(ECC_Pawn);
}

bool UHodgeHitDetectionProfile::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	if (!Strategy || Strategy->HasAnyClassFlags(CLASS_Abstract) || ObjectTypes.IsEmpty())
	{
		Errors.Add(FText::FromString(TEXT("Hit profile requires a concrete strategy and object channels.")));
	}
	for (ECollisionChannel Channel : ObjectTypes)
	{
		if (Channel >= ECC_MAX || !FCollisionObjectQueryParams::IsValidObjectQuery(Channel))
		{
			Errors.Add(FText::FromString(TEXT("Hit profile contains an invalid object channel.")));
		}
	}
	if (!FMath::IsFinite(HalfAngleDegrees) || HalfAngleDegrees < 0.f || HalfAngleDegrees > 180.f ||
		!FMath::IsFinite(MaxSweepDistance) || MaxSweepDistance <= 0.f || RotationSubsteps < 1 || RotationSubsteps > 32 ||
		ObstructionChannel >= ECC_MAX)
	{
		Errors.Add(FText::FromString(TEXT("Hit profile geometry limits are invalid.")));
	}
	return Before == Errors.Num();
}

#if WITH_EDITOR
EDataValidationResult UHodgeHitDetectionProfile::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult Parent = Super::IsDataValid(Context);
	TArray<FText> Errors;
	Validate(Errors);
	for (const FText& Error : Errors) { Context.AddError(Error); }
	return Errors.IsEmpty() && Parent != EDataValidationResult::Invalid
		? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}
#endif

bool UHodgeSocketSweepStrategy::Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
	FHodgeHitGeometry& OutGeometry) const
{
	if (!IsValid(Component) || !FMath::IsFinite(Source.Radius) || Source.Radius <= 0.f ||
		Source.LocalOffset.ContainsNaN() || Source.SegmentSamples < 2 || Source.SegmentSamples > 32)
	{
		return false;
	}
	OutGeometry = FHodgeHitGeometry();
	OutGeometry.Transform = Component->GetComponentTransform();
	for (FName Socket : Source.Sockets)
	{
		if (!Component->DoesSocketExist(Socket)) { return false; }
		OutGeometry.Points.Add(Component->GetSocketTransform(Socket).TransformPosition(Source.LocalOffset));
	}
	if (OutGeometry.Points.IsEmpty())
	{
		OutGeometry.Points.Add(OutGeometry.Transform.TransformPosition(Source.LocalOffset));
	}
	else if (OutGeometry.Points.Num() == 2)
	{
		const FVector Start = OutGeometry.Points[0];
		const FVector End = OutGeometry.Points[1];
		OutGeometry.Points.Reset();
		for (int32 Index = 0; Index < Source.SegmentSamples; ++Index)
		{
			OutGeometry.Points.Add(FMath::Lerp(Start, End, float(Index) / (Source.SegmentSamples - 1)));
		}
	}
	return !OutGeometry.Transform.ContainsNaN() &&
		!OutGeometry.Points.ContainsByPredicate([](const FVector& Point) { return Point.ContainsNaN(); });
}

void UHodgeSocketSweepStrategy::Detect(UWorld* World, const FHodgeHitSource& Source,
	const UHodgeHitDetectionProfile* Profile, const FHodgeHitGeometry& Previous,
	const FHodgeHitGeometry& Current, const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const
{
	const FCollisionObjectQueryParams Objects = MakeObjectQuery(Profile);
	for (int32 Index = 0; Index < Current.Points.Num(); ++Index)
	{
		const FVector End = Current.Points[Index];
		FVector Start = Previous.Points.IsValidIndex(Index) ? Previous.Points[Index] : End;
		if (FVector::DistSquared(Start, End) > FMath::Square(Profile->MaxSweepDistance)) { Start = End; }
		TArray<FHitResult> Hits;
		World->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, Objects,
			FCollisionShape::MakeSphere(Source.Radius), Params);
		OutHits.Append(Hits);
	}
}

bool UHodgeBoxSweepStrategy::Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
	FHodgeHitGeometry& OutGeometry) const
{
	const UBoxComponent* Box = Cast<UBoxComponent>(Component);
	if (!IsValid(Box)) { return false; }
	OutGeometry = FHodgeHitGeometry();
	OutGeometry.Transform = Box->GetComponentTransform();
	OutGeometry.BoxExtent = Box->GetScaledBoxExtent();
	return !OutGeometry.Transform.ContainsNaN() && !OutGeometry.BoxExtent.ContainsNaN() &&
		OutGeometry.BoxExtent.GetMin() > 0.f;
}

void UHodgeBoxSweepStrategy::Detect(UWorld* World, const FHodgeHitSource& Source,
	const UHodgeHitDetectionProfile* Profile, const FHodgeHitGeometry& Previous,
	const FHodgeHitGeometry& Current, const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const
{
	const bool bTeleported = FVector::DistSquared(Previous.Transform.GetLocation(), Current.Transform.GetLocation()) >
		FMath::Square(Profile->MaxSweepDistance);
	const FTransform Start = bTeleported ? Current.Transform : Previous.Transform;
	const FCollisionObjectQueryParams Objects = MakeObjectQuery(Profile);
	const int32 Steps = FMath::Clamp(Profile->RotationSubsteps, 1, 32);
	FVector PreviousPosition = Start.GetLocation();
	for (int32 Index = 1; Index <= Steps; ++Index)
	{
		const float Alpha = float(Index) / Steps;
		const FVector Position = FMath::Lerp(Start.GetLocation(), Current.Transform.GetLocation(), Alpha);
		const FQuat Rotation = FQuat::Slerp(Start.GetRotation(), Current.Transform.GetRotation(), Alpha);
		TArray<FHitResult> Hits;
		World->SweepMultiByObjectType(Hits, PreviousPosition, Position, Rotation, Objects,
			FCollisionShape::MakeBox(Current.BoxExtent), Params);
		OutHits.Append(Hits);
		PreviousPosition = Position;
	}
}
