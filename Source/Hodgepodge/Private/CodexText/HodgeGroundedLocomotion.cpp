#include "CodexText/HodgeGroundedLocomotion.h"
#include "CodexText/HodgeLocomotionLab.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimSequence.h"
#include "Curves/CurveFloat.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGroundedLocomotion)

void UHodgeGroundedLocomotion::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    if (!OverlayPose) OverlayPose = GetClass()->GetDefaultObject<UHodgeGroundedLocomotion>()->OverlayPose;
    if (PoseMesh.IsValid()) PoseMesh->UnregisterOnBoneTransformsFinalizedDelegate(FinalizedPoseHandle);
    PoseMesh = GetSkelMeshComponent();
    FinalizedPoseHandle.Reset();
    if (PoseMesh.IsValid())
        FinalizedPoseHandle = PoseMesh->RegisterOnBoneTransformsFinalizedDelegate(
            FOnBoneTransformsFinalizedMultiCast::FDelegate::CreateUObject(this, &ThisClass::CacheFinalizedFootPose));
    bHavePose = false; bHadMovement = false; ActionIndex = INDEX_NONE; GroundActionTime = 0.f;
    FootAlphaL = FootAlphaR = 0.f; TurnDelay = 0.f;
    GroundActionPlayRate = 1.f; bStopPlanted = false; bStopAirborne = true;
    TerrainAlpha = 0.f;
    StrideAlpha = 0.f; StrideScale = 1.f; StrideDirection = FVector(0.f, 1.f, 0.f);
    HandGripComponent.Reset(); HandGripWeight = 0.f; HandIKAlpha = 0.f; OverlayAlpha = 0.f; OverlayTarget = 0.f;
    for (int32 I=0; I<2; ++I) { bLocked[I]=false; PreviousLockCurve[I]=0.f; }
}

FName UHodgeGroundedLocomotion::GetGroundedState()
{
    const int32 Index = GetStateMachineIndex(TEXT("Grounded"));
    return Index == INDEX_NONE ? NAME_None : GetCurrentStateName(Index);
}

void UHodgeGroundedLocomotion::SetLeftHandGrip(USceneComponent* Component, FName Socket, float Weight)
{
    HandGripComponent = Component; HandGripSocket = Socket; HandGripWeight = FMath::Clamp(Weight, 0.f, 1.f);
}

void UHodgeGroundedLocomotion::NativeUninitializeAnimation()
{
    if (PoseMesh.IsValid()) PoseMesh->UnregisterOnBoneTransformsFinalizedDelegate(FinalizedPoseHandle);
    FinalizedPoseHandle.Reset();
    PoseMesh.Reset();
    bHavePose = false;
    Super::NativeUninitializeAnimation();
}

void UHodgeGroundedLocomotion::CacheFinalizedFootPose()
{
    if (auto* Mesh = PoseMesh.Get())
    {
        // PostEvaluate runs before the component swaps its pose buffers; sockets are valid here.
        // Cache the final solved feet; virtual foot bones are also used as terrain IK targets.
        PreviousRawFeet[0] = Mesh->GetSocketTransform(TEXT("Bip001LFoot"), RTS_World);
        PreviousRawFeet[1] = Mesh->GetSocketTransform(TEXT("Bip001RFoot"), RTS_World);
        bHavePose = Mesh->DoesSocketExist(TEXT("Bip001LFoot")) && Mesh->DoesSocketExist(TEXT("Bip001RFoot"));
    }
}

void UHodgeGroundedLocomotion::NativeUpdateAnimation(float Dt)
{
    Super::NativeUpdateAnimation(Dt);
    auto* Character = Cast<ACharacter>(TryGetPawnOwner());
    auto* Mesh = GetSkelMeshComponent();
    if (!Character || !Mesh || Dt <= 0.f) return;
    const auto* Movement = Character->GetCharacterMovement();
    const auto* Lab = Character->FindComponentByClass<UHodgeLocomotionLabComponent>();
    const bool HasInput = Movement->GetCurrentAcceleration().SizeSquared2D() > 1.f || !Character->GetPendingMovementInputVector().IsNearlyZero();
    const bool Interrupted = bLabFall || HasInput || IsAnyMontagePlaying();
    const bool Teleported = FVector::DistSquared(Character->GetActorLocation(), PreviousActorLocation) > FMath::Square(150.f);
    // Floor settlement can move the capsule with zero reported velocity on the first grounded frame.
    const bool AnchorMoved = FVector::DistSquared(Character->GetActorLocation(), PreviousActorLocation) > FMath::Square(0.5f);
    PreviousActorLocation = Character->GetActorLocation();
    if (Teleported || Interrupted)
    {
        ActionIndex = INDEX_NONE; GroundActionTime = 0.f; TurnDelay = 0.f;
        if (Teleported || bLabFall || IsAnyMontagePlaying()) bHadMovement = false;
    }
    if (LabSpeed > 80.f) bLastWalk = Lab && Lab->bWalking;
    if (LabSpeed > 15.f) bHadMovement = true;
    if (ActionIndex != INDEX_NONE)
    {
        if (!GroundActions.IsValidIndex(ActionIndex) || !GroundActions[ActionIndex] || GroundActionTime >= GroundActions[ActionIndex]->GetPlayLength())
        { ActionIndex=INDEX_NONE; }
        else GroundActionTime = FMath::Min(GroundActionTime + Dt * GroundActionPlayRate, GroundActions[ActionIndex]->GetPlayLength());
    }
    if (ActionIndex >= 4)
    {
        const float Delta = FRotator::NormalizeAxis(Character->GetBaseAimRotation().Yaw - Character->GetActorRotation().Yaw);
        // 镜头反向或退出镜头面向时停止旧转身，重新等待稳定方向。
        if (!Lab || !Lab->bCombatFacing || (ActionIndex == 4 ? Delta > 45.f : Delta < -45.f))
        { ActionIndex = INDEX_NONE; TurnDelay = 0.f; }
    }
    if (!Interrupted && ActionIndex == INDEX_NONE && LabSpeed < 10.f)
    {
        if (bHadMovement)
        {
            const float L = GetCurveValue(TEXT("FootContact_L"));
            const float R = GetCurveValue(TEXT("FootContact_R"));
            const float FeetPosition = GetCurveValue(TEXT("Feet_Position"));
            bSupportLeft = FMath::Abs(FeetPosition) > 0.05f ? FeetPosition > 0.f : L >= R;
            bStopPlanted = FMath::Max(L, R) >= 0.8f;
            bStopAirborne = !bStopPlanted;
            StopEntryContact[0] = L; StopEntryContact[1] = R;
            ActionIndex = (bLastWalk ? 0 : 2) + (bSupportLeft ? 0 : 1);
            GroundActionTime = 0.f; GroundActionPlayRate = 1.f; bHadMovement = false;
        }
        else if (Lab && Lab->bCombatFacing)
        {
            const float Delta = FRotator::NormalizeAxis(Character->GetBaseAimRotation().Yaw - Character->GetActorRotation().Yaw);
            TurnDelay = FMath::Abs(Delta) > TurnThreshold ? TurnDelay + Dt : 0.f;
            if (TurnDelay >= 0.2f)
            {
                ActionIndex = Delta < 0.f ? 4 : 5; GroundActionTime=0.f; TurnStartYaw=Character->GetActorRotation().Yaw; TurnDelay=0.f;
                GroundActionPlayRate = FMath::GetMappedRangeValueClamped(FVector2D(TurnThreshold, 180.f), FVector2D(1.f, 1.6f), FMath::Abs(Delta));
            }
        }
    }
    bGroundStopWalkL = ActionIndex==0; bGroundStopWalkR = ActionIndex==1;
    bGroundStopRunL = ActionIndex==2; bGroundStopRunR = ActionIndex==3;
    bGroundTurnL = ActionIndex==4; bGroundTurnR = ActionIndex==5;
    bGroundMove = !bLabFall && ActionIndex == INDEX_NONE && (LabSpeed >= 5.f || HasInput);
    bGroundIdle = !bLabFall && ActionIndex == INDEX_NONE && !bGroundMove;
    const bool bWarpMoving = bGroundMove && !IsAnyMontagePlaying();
    StrideAlpha = bLabFall || IsAnyMontagePlaying() ? 0.f : FMath::FInterpTo(StrideAlpha, bWarpMoving ? 1.f : 0.f, Dt, 12.f);
    const FVector NewStrideDirection = Mesh->GetComponentTransform().InverseTransformVectorNoScale(Character->GetVelocity()).GetSafeNormal2D();
    if (!NewStrideDirection.IsNearlyZero()) StrideDirection = NewStrideDirection;
    const float ReferenceSpeed = FMath::Lerp(Lab ? Lab->WalkSpeed : 180.f, Lab ? Lab->RunSpeed : 420.f, LabGait);
    const float SpeedRatio = LabSpeed / FMath::Max(ReferenceSpeed, 1.f);
    // 低速缩短步幅，同时保留自然步频；斜向混合按合成速度补偿。
    const float BaseStride = FMath::Clamp(SpeedRatio / 0.8f, 0.35f, 1.f);
    LabPlayRate = FMath::Clamp(SpeedRatio / BaseStride, 0.05f, 1.4f);
    const float WeightLength = FVector2D(LabForward - LabBackward, LabRight - LabLeft).Size();
    DiagonalScale = FMath::Clamp(1.f / FMath::Max(WeightLength, 0.7071f), 1.f, 1.4143f);
    StrideScale = FMath::FInterpTo(StrideScale, bWarpMoving ? BaseStride * DiagonalScale : 1.f, Dt, 12.f);
    TerrainAlpha = bEnableTerrainIK && bHavePose && Movement->IsMovingOnGround() && !IsAnyMontagePlaying() && !Teleported ? 1.f : 0.f;
    OverlayAlpha = FMath::FInterpTo(OverlayAlpha, OverlayPose && !bLabFall ? OverlayTarget : 0.f, Dt, 10.f);
    HandIKAlpha = 0.f;
    if (auto* Grip = HandGripComponent.Get(); Grip && Grip->IsRegistered() && (HandGripSocket.IsNone() || Grip->DoesSocketExist(HandGripSocket))
        && Mesh->DoesSocketExist(TEXT("Bip001RHand")) && !IsAnyMontagePlaying() && !bLabFall)
    {
        // 右手骨空间避免持物随角色转向时产生一帧世界空间滞后。
        const FTransform Target = Grip->GetSocketTransform(HandGripSocket).GetRelativeTransform(Mesh->GetSocketTransform(TEXT("Bip001RHand")));
        if (Target.GetLocation().SizeSquared() < FMath::Square(100.f))
        { HandGripLocation = Target.GetLocation(); HandGripRotation = Target.Rotator(); HandIKAlpha = HandGripWeight; }
    }
    RotationTarget = Character->GetActorRotation().Yaw;
    if (IsTurningInPlace())
    {
        const UCurveFloat* Curve = bGroundTurnL ? TurnYawLeft.Get() : TurnYawRight.Get();
        if (Curve) RotationTarget = TurnStartYaw + Curve->GetFloatValue(GroundActionTime);
    }
    const FTransform ComponentTransform = Mesh->GetComponentTransform();
    const bool AllowLock = bHavePose && !AnchorMoved && !Interrupted && !bGroundMove && LabSpeed < 15.f;
    for (int32 I=0; I<2; ++I)
    {
        float Curve = AllowLock ? FMath::Clamp(GetCurveValue(I==0 ? TEXT("FootLock_L") : TEXT("FootLock_R")),0.f,1.f) : 0.f;
        // 停步首帧沿用已落地的支撑脚，随后交给收脚动画自身的解锁曲线。
        if (AllowLock && ActionIndex >= 0 && ActionIndex < 4 && bStopPlanted && I == (bSupportLeft ? 0 : 1))
            Curve = FMath::Max(Curve, StopEntryContact[I] * FMath::Clamp(1.f - GroundActionTime / 0.1f, 0.f, 1.f));
        if (!AllowLock || Curve < 0.01f) bLocked[I]=false;
        // During increasing weights the contact target can settle; a full or falling weight holds it.
        if (AllowLock && Curve > 0.f && (!bLocked[I] || (Curve > PreviousLockCurve[I] && Curve < 0.99f)))
        { LockedFeet[I]=PreviousRawFeet[I]; bLocked[I]=true; }
        float Alpha = Curve;
        if (bLocked[I] && FVector::Dist(LockedFeet[I].GetLocation(), PreviousRawFeet[I].GetLocation()) > 45.f)
        { Alpha=0.f; bLocked[I]=false; }
        const FTransform Local = LockedFeet[I].GetRelativeTransform(ComponentTransform);
        if (I==0) { FootAlphaL=Alpha; FootTargetL=Local.GetLocation(); FootRotationL=Local.Rotator(); }
        else { FootAlphaR=Alpha; FootTargetR=Local.GetLocation(); FootRotationR=Local.Rotator(); }
        PreviousLockCurve[I]=Curve;
    }
}
