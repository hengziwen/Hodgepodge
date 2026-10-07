#include "Combat/HodgeHitDetection.h"
#include "Components/BoxComponent.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Engine/OverlapResult.h"
#include "Components/PrimitiveComponent.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHitDetection)

namespace
{
#if ENABLE_DRAW_DEBUG
	TAutoConsoleVariable<int32> CVarHitDebugWeapon(TEXT("Hodge.Combat.DebugDraw.Weapon"), 1,
		TEXT("Draw weapon hit detection geometry. 0: off, 1: on."));
	TAutoConsoleVariable<int32> CVarHitDebugBody(TEXT("Hodge.Combat.DebugDraw.Body"), 1,
		TEXT("Draw body hit detection geometry. 0: off, 1: on."));
	TAutoConsoleVariable<int32> CVarHitDebugHitBox(TEXT("Hodge.Combat.DebugDraw.HitBox"), 1,
		TEXT("Draw hitbox detection geometry. 0: off, 1: on."));
	TAutoConsoleVariable<float> CVarHitDebugDuration(TEXT("Hodge.Combat.DebugDraw.Duration"), 5.f,
		TEXT("Lifetime in seconds for each hit detection debug drawing."));

	struct FHitDebugSettings
	{
		bool bEnabled = false;
		FColor Color = FColor::White;
		float Duration = 5.f;
	};

	FHitDebugSettings GetHitDebugSettings(const UWorld* World, const FHodgeHitSource& Source)
	{
		FHitDebugSettings Result;
		if (!World || World->GetNetMode() == NM_DedicatedServer) { return Result; }
		static const FGameplayTag WeaponTag = FGameplayTag::RequestGameplayTag(TEXT("Combat.Source.Weapon"), false);
		static const FGameplayTag BodyTag = FGameplayTag::RequestGameplayTag(TEXT("Combat.Source.Body"), false);
		static const FGameplayTag HitBoxTag = FGameplayTag::RequestGameplayTag(TEXT("Combat.Source.Hitbox"), false);
		if (Source.SourceTag.MatchesTag(WeaponTag))
		{
			Result.bEnabled = CVarHitDebugWeapon.GetValueOnGameThread() != 0;
			Result.Color = FColor::Cyan;
		}
		else if (Source.SourceTag.MatchesTag(BodyTag))
		{
			Result.bEnabled = CVarHitDebugBody.GetValueOnGameThread() != 0;
			Result.Color = FColor::Yellow;
		}
		else if (Source.SourceTag.MatchesTag(HitBoxTag))
		{
			Result.bEnabled = CVarHitDebugHitBox.GetValueOnGameThread() != 0;
			Result.Color = FColor::Magenta;
		}
		const float Duration = CVarHitDebugDuration.GetValueOnGameThread();
		Result.Duration = FMath::IsFinite(Duration) ? FMath::Max(0.f, Duration) : 5.f;
		return Result;
	}

	void DrawHitPoints(const UWorld* World, const TArray<FHitResult>& Hits, float Duration)
	{
		for (const FHitResult& Hit : Hits)
		{
			DrawDebugPoint(World, Hit.ImpactPoint, 10.f, FColor::Green, false, Duration);
		}
	}

	void DrawSphereSweep(const UWorld* World, const FVector& Start, const FVector& End, float Radius,
		const FHitDebugSettings& Settings, const TArray<FHitResult>& Hits)
	{
		if (!Settings.bEnabled) { return; }
		const FColor Color = Hits.IsEmpty() ? Settings.Color : FColor::Green;
		const FVector Delta = End - Start;
		if (Delta.IsNearlyZero())
		{
			DrawDebugSphere(World, End, Radius, 12, Color, false, Settings.Duration, 0, 1.f);
		}
		else
		{
			const FQuat Rotation = FQuat::FindBetweenNormals(FVector::UpVector, Delta.GetSafeNormal());
			DrawDebugCapsule(World, (Start + End) * .5f, Delta.Size() * .5f + Radius, Radius,
				Rotation, Color, false, Settings.Duration, 0, 1.f);
			DrawDebugLine(World, Start, End, Color, false, Settings.Duration, 0, 1.f);
		}
		DrawHitPoints(World, Hits, Settings.Duration);
	}

	void DrawBoxSweep(const UWorld* World, const FVector& Start, const FVector& End, const FVector& Extent,
		const FQuat& Rotation, const FHitDebugSettings& Settings, const TArray<FHitResult>& Hits)
	{
		if (!Settings.bEnabled) { return; }
		const FColor Color = Hits.IsEmpty() ? Settings.Color : FColor::Green;
		DrawDebugBox(World, Start, Extent, Rotation, Color, false, Settings.Duration, 0, 1.f);
		if (!Start.Equals(End))
		{
			DrawDebugBox(World, End, Extent, Rotation, Color, false, Settings.Duration, 0, 1.f);
			// 每个旋转子步绘制端点盒体与八个顶点的扫掠连线。
			for (int32 Corner = 0; Corner < 8; ++Corner)
			{
				const FVector Offset = Rotation.RotateVector(FVector(
					(Corner & 1) ? Extent.X : -Extent.X,
					(Corner & 2) ? Extent.Y : -Extent.Y,
					(Corner & 4) ? Extent.Z : -Extent.Z));
				DrawDebugLine(World, Start + Offset, End + Offset, Color, false, Settings.Duration, 0, 1.f);
			}
		}
		DrawHitPoints(World, Hits, Settings.Duration);
	}
#endif

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

bool FHodgeHitVolumeConfig::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	if (GeometryMode == EHodgeHitGeometryMode::ExistingSource) { return true; }
	if (LocalTransform.ContainsNaN() || !LocalTransform.GetRotation().IsNormalized() ||
		!LocalTransform.GetScale3D().Equals(FVector::OneVector) || !FMath::IsFinite(MaxAnchorDistance) || MaxAnchorDistance <= 0.f)
	{
		Errors.Add(FText::FromString(TEXT("Hit volume requires a finite unit-scale transform and positive anchor distance.")));
	}
	if ((Shape == EHodgeHitShape::Sphere && (!FMath::IsFinite(SphereRadius) || SphereRadius <= 0.f)) ||
		(Shape == EHodgeHitShape::Box && (BoxHalfExtent.ContainsNaN() || BoxHalfExtent.GetMin() <= 0.f)) ||
		(Shape == EHodgeHitShape::Capsule && (!FMath::IsFinite(CapsuleRadius) || !FMath::IsFinite(CapsuleHalfHeight) ||
			CapsuleRadius <= 0.f || CapsuleHalfHeight < CapsuleRadius)))
	{
		Errors.Add(FText::FromString(TEXT("Hit volume dimensions are invalid; capsule half height must include its radius.")));
	}
	if (AnchorKind == EHodgeHitAnchorKind::ExecutionTransform && AnchorKey.IsNone())
	{
		Errors.Add(FText::FromString(TEXT("ExecutionTransform requires an AnchorKey supplied by the active server ability.")));
	}
	if (AnchorKind != EHodgeHitAnchorKind::RegisteredSource && !AnchorSocket.IsNone())
	{
		Errors.Add(FText::FromString(TEXT("AnchorSocket requires RegisteredSource.")));
	}
	return Before == Errors.Num();
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
#if ENABLE_DRAW_DEBUG
	const FHitDebugSettings Debug = GetHitDebugSettings(World, Source);
#endif
	for (int32 Index = 0; Index < Current.Points.Num(); ++Index)
	{
		const FVector End = Current.Points[Index];
		FVector Start = Previous.Points.IsValidIndex(Index) ? Previous.Points[Index] : End;
		if (!Profile->bContinuousMotion && FVector::DistSquared(Start, End) > FMath::Square(Profile->MaxSweepDistance)) { Start = End; }
		TArray<FHitResult> Hits;
		World->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, Objects,
			FCollisionShape::MakeSphere(Source.Radius), Params);
#if ENABLE_DRAW_DEBUG
		DrawSphereSweep(World, Start, End, Source.Radius, Debug, Hits);
#endif
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
	const bool bTeleported = !Profile->bContinuousMotion && FVector::DistSquared(Previous.Transform.GetLocation(), Current.Transform.GetLocation()) >
		FMath::Square(Profile->MaxSweepDistance);
	const FTransform Start = bTeleported ? Current.Transform : Previous.Transform;
	const FCollisionObjectQueryParams Objects = MakeObjectQuery(Profile);
#if ENABLE_DRAW_DEBUG
	const FHitDebugSettings Debug = GetHitDebugSettings(World, Source);
#endif
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
#if ENABLE_DRAW_DEBUG
		DrawBoxSweep(World, PreviousPosition, Position, Current.BoxExtent, Rotation, Debug, Hits);
#endif
		OutHits.Append(Hits);
		PreviousPosition = Position;
	}
}

bool UHodgeShapeQueryStrategy::Capture(const USceneComponent*, const FHodgeHitSource&, FHodgeHitGeometry&) const
{
	// 配置形状由会话解析世界坐标，不允许误用场景组件作为形状配置。
	return false;
}

void UHodgeShapeQueryStrategy::Detect(UWorld* World, const FHodgeHitSource& Source,
	const UHodgeHitDetectionProfile* Profile, const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
	const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const
{
	const FCollisionShape Shape = Current.Shape == EHodgeHitShape::Box ? FCollisionShape::MakeBox(Current.BoxExtent)
		: Current.Shape == EHodgeHitShape::Capsule ? FCollisionShape::MakeCapsule(Current.Radius, Current.HalfHeight)
		: FCollisionShape::MakeSphere(Current.Radius);
	const auto Objects = MakeObjectQuery(Profile);
	const FVector End = Current.Transform.GetLocation();
	TArray<FHitResult> Results;
	if (Profile->QueryMode == EHodgeHitQueryMode::Overlap)
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, End, Current.Transform.GetRotation(), Objects, Shape, Params);
		for (const FOverlapResult& Overlap : Overlaps)
		{
			UPrimitiveComponent* Component = Overlap.Component.Get();
			AActor* Actor = Overlap.GetActor();
			if (!IsValid(Actor) || !IsValid(Component)) { continue; }
			FVector Point = Actor->GetActorLocation();
			FVector Closest;
			if (Component->GetClosestPointOnCollision(End, Closest) >= 0.f) { Point = Closest; }
			FHitResult Hit(Actor, Component, Point, FVector::ZeroVector);
			Hit.TraceStart = End;
			Hit.TraceEnd = End;
			Results.Add(Hit);
		}
	}
	else
	{
		const bool bTeleport = !Profile->bContinuousMotion && FVector::DistSquared(Previous.Transform.GetLocation(), End) > FMath::Square(Profile->MaxSweepDistance);
		const FTransform Start = bTeleport ? Current.Transform : Previous.Transform;
		const int32 Steps = Current.Shape == EHodgeHitShape::Sphere ? 1 : Profile->RotationSubsteps;
		FVector PreviousPosition = Start.GetLocation();
		for (int32 Index = 1; Index <= Steps; ++Index)
		{
			const float Alpha = float(Index) / Steps;
			const FVector Position = FMath::Lerp(Start.GetLocation(), End, Alpha);
			const FQuat Rotation = FQuat::Slerp(Start.GetRotation(), Current.Transform.GetRotation(), Alpha);
			TArray<FHitResult> Hits;
			World->SweepMultiByObjectType(Hits, PreviousPosition, Position, Rotation, Objects, Shape, Params);
			Results.Append(Hits);
			PreviousPosition = Position;
		}
	}
#if ENABLE_DRAW_DEBUG
	FHodgeHitSource DebugSource = Source;
	DebugSource.SourceTag = FGameplayTag::RequestGameplayTag(TEXT("Combat.Source.Hitbox.Chest"), false);
	const auto Debug = GetHitDebugSettings(World, DebugSource);
	const FVector Start = Profile->QueryMode == EHodgeHitQueryMode::Overlap ||
		(!Profile->bContinuousMotion && FVector::DistSquared(Previous.Transform.GetLocation(), End) > FMath::Square(Profile->MaxSweepDistance))
		? End : Previous.Transform.GetLocation();
	if (Current.Shape == EHodgeHitShape::Sphere) { DrawSphereSweep(World, Start, End, Current.Radius, Debug, Results); }
	else if (Current.Shape == EHodgeHitShape::Box) { DrawBoxSweep(World, Start, End, Current.BoxExtent, Current.Transform.GetRotation(), Debug, Results); }
	else if (Debug.bEnabled)
	{
		DrawDebugCapsule(World, End, Current.HalfHeight, Current.Radius, Current.Transform.GetRotation(),
			Results.IsEmpty() ? Debug.Color : FColor::Green, false, Debug.Duration, 0, 1.f);
		DrawDebugLine(World, Start, End, Debug.Color, false, Debug.Duration, 0, 1.f);
		DrawHitPoints(World, Results, Debug.Duration);
	}
#endif
	OutHits.Append(Results);
}
