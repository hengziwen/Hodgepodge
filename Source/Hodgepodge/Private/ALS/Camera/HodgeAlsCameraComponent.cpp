#include "ALS/Camera/HodgeAlsCameraComponent.h"

#include "ALS/Camera/HodgeAlsCameraSettings.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "GameFramework/WorldSettings.h"
#include "ALS/Camera/Utility/HodgeAlsCameraConstants.h"
#include "ALS/Utility/HodgeAlsDebugUtility.h"
#include "ALS/Utility/HodgeAlsMacros.h"
#include "ALS/Utility/HodgeAlsRotation.h"
#include "ALS/Utility/HodgeAlsUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsCameraComponent)

UHodgeAlsCameraComponent::UHodgeAlsCameraComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	bTickInEditor = false;
	bHiddenInGame = true;
}

void UHodgeAlsCameraComponent::PostLoad()
{
	Super::PostLoad();

	// This is required for the camera to work properly, as its mesh is never rendered.
	// We change the tick option here to override the value that comes from the config file.
	VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

void UHodgeAlsCameraComponent::OnRegister()
{
	Character = Cast<ACharacter>(GetOwner());

	Super::OnRegister();
}

void UHodgeAlsCameraComponent::RegisterComponentTickFunctions(const bool bRegister)
{
	Super::RegisterComponentTickFunctions(bRegister);

	// Tick after the owner to have access to the most up-to-date character state.

	AddTickPrerequisiteActor(GetOwner());
}

void UHodgeAlsCameraComponent::Activate(const bool bReset)
{
	if (bReset || ShouldActivate())
	{
		TickCamera(0.0f, false);
	}

	Super::Activate(bReset);
}

void UHodgeAlsCameraComponent::InitAnim(const bool bForceReinitialize)
{
	Super::InitAnim(bForceReinitialize);

	AnimationInstance = GetAnimInstance();
}

void UHodgeAlsCameraComponent::BeginPlay()
{
	HODGEALS_ENSURE(IsValid(GetAnimInstance()));
	HODGEALS_ENSURE(IsValid(Settings));
	HODGEALS_ENSURE(IsValid(Character));

	Super::BeginPlay();
}

void UHodgeAlsCameraComponent::TickComponent(float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (IsValid(Settings) && Settings->bIgnoreTimeDilation)
	{
		// Use the previous global time dilation, as this frame's delta time may not yet be affected
		// by the current global time dilation, and thus unscaling will produce the wrong delta time.

		const auto TimeDilation{PreviousGlobalTimeDilation * GetOwner()->CustomTimeDilation};

		DeltaTime = TimeDilation > UE_SMALL_NUMBER ? DeltaTime / TimeDilation : GetWorld()->DeltaRealTimeSeconds;
	}

	PreviousGlobalTimeDilation = GetWorld()->GetWorldSettings()->GetEffectiveTimeDilation();

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Skip camera tick until parallel animation evaluation completes.

	if (!IsRunningParallelEvaluation())
	{
		TickCamera(DeltaTime);
	}
}

void UHodgeAlsCameraComponent::CompleteParallelAnimationEvaluation(const bool bDoPostAnimationEvaluation)
{
	Super::CompleteParallelAnimationEvaluation(bDoPostAnimationEvaluation);

	TickCamera(GetAnimInstance()->GetDeltaSeconds());
}

FVector UHodgeAlsCameraComponent::GetFirstPersonCameraLocation() const
{
	return Character->GetMesh()->GetSocketLocation(Settings->FirstPerson.CameraSocketName);
}

FVector UHodgeAlsCameraComponent::GetThirdPersonPivotLocation() const
{
	const auto* Mesh{Character->GetMesh()};

	FVector FirstPivotLocation;

	if (!IsValid(Mesh->GetAttachParent()) && Settings->ThirdPerson.FirstPivotSocketName == UHodgeAlsConstants::RootBoneName())
	{
		// The root bone location usually remains fixed when the mesh is detached, so use the capsule's bottom location here as a fallback.

		FirstPivotLocation = Character->GetRootComponent()->GetComponentLocation();
		FirstPivotLocation.Z -= Character->GetRootComponent()->Bounds.BoxExtent.Z;
	}
	else
	{
		FirstPivotLocation = Mesh->GetSocketLocation(Settings->ThirdPerson.FirstPivotSocketName);
	}

	return (FirstPivotLocation + Mesh->GetSocketLocation(Settings->ThirdPerson.SecondPivotSocketName)) * 0.5f;
}

FVector UHodgeAlsCameraComponent::GetThirdPersonTraceStartLocation() const
{
	return Character->GetMesh()->GetSocketLocation(bRightShoulder
		                                               ? Settings->ThirdPerson.TraceShoulderRightSocketName
		                                               : Settings->ThirdPerson.TraceShoulderLeftSocketName);
}

void UHodgeAlsCameraComponent::GetViewInfo(FMinimalViewInfo& ViewInfo) const
{
	ViewInfo.Location = CameraLocation;
	ViewInfo.Rotation = CameraRotation;
	ViewInfo.FOV = CameraFieldOfView;

	ViewInfo.PostProcessBlendWeight = IsValid(Settings) ? PostProcessWeight : 0.0f;

	if (ViewInfo.PostProcessBlendWeight > UE_SMALL_NUMBER)
	{
		ViewInfo.PostProcessSettings = Settings->PostProcess;
	}
}

void UHodgeAlsCameraComponent::TickCamera(const float DeltaTime, bool bAllowLag)
{
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UHodgeAlsCameraComponent::TickCamera"), STAT_UHodgeAlsCameraComponent_TickCamera, STATGROUP_HodgeAls)
	TRACE_CPUPROFILER_EVENT_SCOPE(__FUNCTION__);

	if (!IsValid(GetAnimInstance()) || !IsValid(Settings) || !IsValid(Character))
	{
		return;
	}

	HODGEALS_ENSURE_MESSAGE(!IsRunningParallelEvaluation(),
	                   TEXT("UHodgeAlsCameraComponent::TickCamera() should not be called during parallel animation")
	                   TEXT(" evaluation, because accessing animation curves causes the game thread to wait")
	                   TEXT(" for the parallel task to complete, resulting in performance degradation"));

#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebugCameraShapes{
		UHodgeAlsDebugUtility::ShouldDisplayDebugForActor(GetOwner(), UHodgeAlsCameraConstants::CameraShapesDebugDisplayName())
	};
#else
	const auto bDisplayDebugCameraShapes{false};
#endif

	// Refresh movement base.

	const auto& BasedMovement{Character->GetBasedMovement()};
	const auto bMovementBaseHasRelativeRotation{BasedMovement.HasRelativeRotation()};

	auto MovementBaseLocation{FVector::ZeroVector};
	auto MovementBaseRotation{FQuat::Identity};

	if (bMovementBaseHasRelativeRotation)
	{
		MovementBaseUtility::GetMovementBaseTransform(BasedMovement.MovementBase, BasedMovement.BoneName,
		                                              MovementBaseLocation, MovementBaseRotation);
	}

	if (BasedMovement.MovementBase != MovementBasePrimitive || BasedMovement.BoneName != MovementBaseBoneName)
	{
		MovementBasePrimitive = BasedMovement.MovementBase;
		MovementBaseBoneName = BasedMovement.BoneName;

		if (bMovementBaseHasRelativeRotation)
		{
			const auto MovementBaseRotationInverse{MovementBaseRotation.Inverse()};

			PivotMovementBaseRelativeLagLocation = MovementBaseRotationInverse.RotateVector(PivotLagLocation - MovementBaseLocation);
			CameraMovementBaseRelativeRotation = MovementBaseRotationInverse * CameraRotation.Quaternion();
		}
		else
		{
			PivotMovementBaseRelativeLagLocation = FVector::ZeroVector;
			CameraMovementBaseRelativeRotation = FQuat::Identity;
		}
	}

	const auto CameraTargetRotation{Character->GetViewRotation()};

	const auto PreviousPivotTargetLocation{PivotTargetLocation};

	PivotTargetLocation = GetThirdPersonPivotLocation();

	const auto FirstPersonOverride{
		UHodgeAlsMath::Clamp01(GetAnimInstance()->GetCurveValue(UHodgeAlsCameraConstants::FirstPersonOverrideCurveName()))
	};

	if (FAnimWeight::IsFullWeight(FirstPersonOverride))
	{
		// Skip other calculations if the character is fully in first-person mode.

		PivotLagLocation = PivotTargetLocation;
		PivotLocation = PivotTargetLocation;

		CameraLocation = GetFirstPersonCameraLocation();
		CameraRotation = CameraTargetRotation;

		CameraFieldOfView = bOverrideFieldOfView ? FieldOfViewOverride : Settings->FirstPerson.FieldOfView;
		return;
	}

	// Force disable camera lag if the character was teleported.

	bAllowLag &= Settings->TeleportDistanceThreshold <= 0.0f ||
		FVector::DistSquared(PreviousPivotTargetLocation, PivotTargetLocation) <= FMath::Square(Settings->TeleportDistanceThreshold);

	// Calculate camera rotation.

	if (bMovementBaseHasRelativeRotation)
	{
		CameraRotation = (MovementBaseRotation * CameraMovementBaseRelativeRotation).Rotator();

		CameraRotation = CalculateCameraRotation(CameraTargetRotation, DeltaTime, bAllowLag);

		CameraMovementBaseRelativeRotation = MovementBaseRotation.Inverse() * CameraRotation.Quaternion();
	}
	else
	{
		CameraRotation = CalculateCameraRotation(CameraTargetRotation, DeltaTime, bAllowLag);
	}

	const FQuat CameraYawRotation{FVector::ZAxisVector, FMath::DegreesToRadians(CameraRotation.Yaw)};

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraShapes)
	{
		UHodgeAlsDebugUtility::DrawSphereAlternative(GetWorld(), PivotTargetLocation, CameraYawRotation.Rotator(), 16.0f, FLinearColor::Green);
	}
#endif

	// Calculate pivot lag location. Get the pivot target location and interpolate using axis-independent lag for maximum control.

	if (bMovementBaseHasRelativeRotation)
	{
		PivotLagLocation = MovementBaseLocation + MovementBaseRotation.RotateVector(PivotMovementBaseRelativeLagLocation);

		PivotLagLocation = CalculatePivotLagLocation(CameraYawRotation, DeltaTime, bAllowLag);

		PivotMovementBaseRelativeLagLocation = MovementBaseRotation.UnrotateVector(PivotLagLocation - MovementBaseLocation);
	}
	else
	{
		PivotLagLocation = CalculatePivotLagLocation(CameraYawRotation, DeltaTime, bAllowLag);
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraShapes)
	{
		DrawDebugLine(GetWorld(), PivotLagLocation, PivotTargetLocation,
		              FLinearColor{1.0f, 0.5f, 0.0f}.ToFColor(true),
		              false, 0.0f, 0, UHodgeAlsDebugUtility::DrawLineThickness);

		UHodgeAlsDebugUtility::DrawSphereAlternative(GetWorld(), PivotLagLocation, CameraYawRotation.Rotator(), 16.0f, {1.0f, 0.5f, 0.0f});
	}
#endif

	// Calculate pivot location.

	const auto PivotOffset{CalculatePivotOffset()};

	PivotLocation = PivotLagLocation + PivotOffset;

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraShapes)
	{
		DrawDebugLine(GetWorld(), PivotLocation, PivotLagLocation,
		              FLinearColor{0.0f, 0.75f, 1.0f}.ToFColor(true),
		              false, 0.0f, 0, UHodgeAlsDebugUtility::DrawLineThickness);

		UHodgeAlsDebugUtility::DrawSphereAlternative(GetWorld(), PivotLocation, CameraYawRotation.Rotator(), 16.0f, {0.0f, 0.75f, 1.0f});
	}
#endif

	// Calculate target camera location.

	const auto CameraTargetLocation{PivotLocation + CalculateCameraOffset()};

	// Trace for an object between the camera and character to apply a corrective offset.

	const auto CameraFinalLocation{CalculateCameraTrace(CameraTargetLocation, PivotOffset, DeltaTime, bAllowLag, TraceDistanceRatio)};

	if (!FAnimWeight::IsRelevant(FirstPersonOverride))
	{
		CameraLocation = CameraFinalLocation;
		CameraFieldOfView = Settings->ThirdPerson.FieldOfView;
	}
	else
	{
		CameraLocation = FMath::Lerp(CameraFinalLocation, GetFirstPersonCameraLocation(), FirstPersonOverride);
		CameraFieldOfView = FMath::Lerp(Settings->ThirdPerson.FieldOfView, Settings->FirstPerson.FieldOfView, FirstPersonOverride);
	}

	if (bOverrideFieldOfView)
	{
		CameraFieldOfView = FieldOfViewOverride;
	}

	CameraFieldOfView = FMath::Clamp(CameraFieldOfView + CalculateFovOffset(), 5.0f, 175.0f);
}

FRotator UHodgeAlsCameraComponent::CalculateCameraRotation(const FRotator& CameraTargetRotation,
                                                      const float DeltaTime, const bool bAllowLag) const
{
	if (!bAllowLag)
	{
		return CameraTargetRotation;
	}

	const auto RotationLag{GetAnimInstance()->GetCurveValue(UHodgeAlsCameraConstants::RotationLagCurveName())};

	return UHodgeAlsRotation::ExponentialDecayRotation(CameraRotation, CameraTargetRotation, DeltaTime, RotationLag);
}

FVector UHodgeAlsCameraComponent::CalculatePivotLagLocation(const FQuat& CameraYawRotation, const float DeltaTime, const bool bAllowLag) const
{
	if (!bAllowLag)
	{
		return PivotTargetLocation;
	}

	const auto RelativePivotInitialLagLocation{CameraYawRotation.UnrotateVector(PivotLagLocation)};
	const auto RelativePivotTargetLocation{CameraYawRotation.UnrotateVector(PivotTargetLocation)};

	const auto LocationLagX{GetAnimInstance()->GetCurveValue(UHodgeAlsCameraConstants::LocationLagXCurveName())};
	const auto LocationLagY{GetAnimInstance()->GetCurveValue(UHodgeAlsCameraConstants::LocationLagYCurveName())};
	const auto LocationLagZ{GetAnimInstance()->GetCurveValue(UHodgeAlsCameraConstants::LocationLagZCurveName())};

	return CameraYawRotation.RotateVector({
		UHodgeAlsMath::ExponentialDecay(RelativePivotInitialLagLocation.X, RelativePivotTargetLocation.X, DeltaTime, LocationLagX),
		UHodgeAlsMath::ExponentialDecay(RelativePivotInitialLagLocation.Y, RelativePivotTargetLocation.Y, DeltaTime, LocationLagY),
		UHodgeAlsMath::ExponentialDecay(RelativePivotInitialLagLocation.Z, RelativePivotTargetLocation.Z, DeltaTime, LocationLagZ)
	});
}

FVector UHodgeAlsCameraComponent::CalculatePivotOffset() const
{
	return Character->GetMesh()->GetComponentQuat().RotateVector(
		FVector{
			GetAnimInstance()->GetCurveValue(UHodgeAlsCameraConstants::PivotOffsetXCurveName()),
			GetAnimInstance()->GetCurveValue(UHodgeAlsCameraConstants::PivotOffsetYCurveName()),
			GetAnimInstance()->GetCurveValue(UHodgeAlsCameraConstants::PivotOffsetZCurveName())
		} * Character->GetMesh()->GetComponentScale().Z);
}

FVector UHodgeAlsCameraComponent::CalculateCameraOffset() const
{
	return CameraRotation.RotateVector(
		FVector{
			GetAnimInstance()->GetCurveValue(UHodgeAlsCameraConstants::CameraOffsetXCurveName()),
			GetAnimInstance()->GetCurveValue(UHodgeAlsCameraConstants::CameraOffsetYCurveName()),
			GetAnimInstance()->GetCurveValue(UHodgeAlsCameraConstants::CameraOffsetZCurveName())
		} * Character->GetMesh()->GetComponentScale().Z);
}

float UHodgeAlsCameraComponent::CalculateFovOffset() const
{
	return GetAnimInstance()->GetCurveValue(UHodgeAlsCameraConstants::FovOffsetCurveName());
}

FVector UHodgeAlsCameraComponent::CalculateCameraTrace(const FVector& CameraTargetLocation, const FVector& PivotOffset,
                                                  const float DeltaTime, const bool bAllowLag, float& NewTraceDistanceRatio) const
{
#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebugCameraTraces{
		UHodgeAlsDebugUtility::ShouldDisplayDebugForActor(GetOwner(), UHodgeAlsCameraConstants::CameraTracesDebugDisplayName())
	};
#else
	const auto bDisplayDebugCameraTraces{false};
#endif

	const auto MeshScale{UE_REAL_TO_FLOAT(Character->GetMesh()->GetComponentScale().Z)};

	static const FName MainTraceTag{FString::Printf(TEXT("%hs (Main Trace)"), __FUNCTION__)};

	auto TraceStart{
		FMath::Lerp(
			GetThirdPersonTraceStartLocation(),
			PivotTargetLocation + PivotOffset + FVector{Settings->ThirdPerson.TraceOverrideOffset},
			UHodgeAlsMath::Clamp01(GetAnimInstance()->GetCurveValue(UHodgeAlsCameraConstants::TraceOverrideCurveName())))
	};

	const auto TraceEnd{CameraTargetLocation};
	const auto CollisionShape{FCollisionShape::MakeSphere(Settings->ThirdPerson.TraceRadius * MeshScale)};

	auto TraceResult{TraceEnd};

	FHitResult Hit;
	if (GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, Settings->ThirdPerson.TraceChannel,
	                                     CollisionShape, {MainTraceTag, false, GetOwner()}))
	{
		if (!Hit.bStartPenetrating)
		{
			TraceResult = Hit.Location;
		}
		else if (TryAdjustLocationBlockedByGeometry(TraceStart, bDisplayDebugCameraTraces))
		{
			static const FName AdjustedTraceTag{FString::Printf(TEXT("%hs (Adjusted Trace)"), __FUNCTION__)};

			GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, Settings->ThirdPerson.TraceChannel,
			                                 CollisionShape, {AdjustedTraceTag, false, GetOwner()});
			if (Hit.IsValidBlockingHit())
			{
				TraceResult = Hit.Location;
			}
		}
		else
		{
			// Note that TraceStart may be changed even if TryAdjustLocationBlockedByGeometry() returned false.
			TraceResult = TraceStart;
		}
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraTraces)
	{
		UHodgeAlsDebugUtility::DrawSweepSphere(GetWorld(), TraceStart, TraceResult, CollisionShape.GetCapsuleRadius(),
		                                  Hit.IsValidBlockingHit() ? FLinearColor::Red : FLinearColor::Green);
	}
#endif

	// Apply trace distance smoothing.

	if (!bAllowLag || !Settings->ThirdPerson.bEnableTraceDistanceSmoothing)
	{
		NewTraceDistanceRatio = 1.0f;
		return TraceResult;
	}

	const auto TraceVector{TraceEnd - TraceStart};
	const auto TraceDistance{TraceVector.Size()};

	if (TraceDistance <= UE_KINDA_SMALL_NUMBER)
	{
		NewTraceDistanceRatio = 1.0f;
		return TraceResult;
	}

	const auto TargetTraceDistanceRatio{UE_REAL_TO_FLOAT((TraceResult - TraceStart).Size() / TraceDistance)};

	NewTraceDistanceRatio = TargetTraceDistanceRatio <= TraceDistanceRatio
		                        ? TargetTraceDistanceRatio
		                        : UHodgeAlsMath::ExponentialDecay(TraceDistanceRatio, TargetTraceDistanceRatio, DeltaTime,
		                                                     Settings->ThirdPerson.TraceDistanceSmoothing.InterpolationSpeed);

	return TraceStart + TraceVector * TraceDistanceRatio;
}

bool UHodgeAlsCameraComponent::TryAdjustLocationBlockedByGeometry(FVector& Location, const bool bDisplayDebugCameraTraces) const
{
	// Based on ComponentEncroachesBlockingGeometry_WithAdjustment().

	const auto MeshScale{UE_REAL_TO_FLOAT(Character->GetMesh()->GetComponentScale().Z)};
	const auto CollisionShape{FCollisionShape::MakeSphere((Settings->ThirdPerson.TraceRadius + 1.0f) * MeshScale)};

	static TArray<FOverlapResult> Overlaps;
	check(Overlaps.IsEmpty())

	ON_SCOPE_EXIT
	{
		Overlaps.Reset();
	};

	static const FName OverlapMultiTraceTag{FString::Printf(TEXT("%hs (Overlap Multi)"), __FUNCTION__)};

	if (!GetWorld()->OverlapMultiByChannel(Overlaps, Location, FQuat::Identity, Settings->ThirdPerson.TraceChannel,
	                                       CollisionShape, {OverlapMultiTraceTag, false, GetOwner()}))
	{
		return false;
	}

	auto Adjustment{FVector::ZeroVector};
	auto bAnyValidBlock{false};

	FMTDResult MtdResult;

	for (const auto& Overlap : Overlaps)
	{
		if (!Overlap.Component.IsValid() ||
		    Overlap.Component->GetCollisionResponseToChannel(Settings->ThirdPerson.TraceChannel) != ECR_Block)
		{
			continue;
		}

		const auto* OverlapBody{Overlap.Component->GetBodyInstance(NAME_None, true, Overlap.ItemIndex)};

		if (OverlapBody == nullptr || !OverlapBody->OverlapTest(Location, FQuat::Identity, CollisionShape, &MtdResult))
		{
			return false;
		}

		if (!FMath::IsNearlyZero(MtdResult.Distance))
		{
			Adjustment += MtdResult.Direction * MtdResult.Distance;
			bAnyValidBlock = true;
		}
	}

	if (!bAnyValidBlock)
	{
		return false;
	}

	auto AdjustmentDirection{Adjustment};

	if (!AdjustmentDirection.Normalize() ||
	    ((GetOwner()->GetActorLocation() - Location).GetSafeNormal() | AdjustmentDirection) < -UE_KINDA_SMALL_NUMBER)
	{
		return false;
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraTraces)
	{
		DrawDebugLine(GetWorld(), Location, Location + Adjustment,
		              FLinearColor{0.0f, 0.75f, 1.0f}.ToFColor(true),
		              false, 5.0f, 0, UHodgeAlsDebugUtility::DrawLineThickness);
	}
#endif

	Location += Adjustment;

	static const FName FreeSpaceTraceTag{FString::Printf(TEXT("%hs (Free Space Overlap)"), __FUNCTION__)};

	return !GetWorld()->OverlapBlockingTestByChannel(Location, FQuat::Identity, Settings->ThirdPerson.TraceChannel,
	                                                 FCollisionShape::MakeSphere(Settings->ThirdPerson.TraceRadius * MeshScale),
	                                                 {FreeSpaceTraceTag, false, GetOwner()});
}
