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
    bHavePose = false; bHadMovement = false; ActionIndex = INDEX_NONE; GroundActionTime = 0.f;
    FootAlphaL = FootAlphaR = 0.f; TurnDelay = 0.f;
    for (int32 I=0; I<2; ++I) { bLocked[I]=false; PreviousLockCurve[I]=0.f; }
}

FName UHodgeGroundedLocomotion::GetGroundedState()
{
    const int32 Index = GetStateMachineIndex(TEXT("Grounded"));
    return Index == INDEX_NONE ? NAME_None : GetCurrentStateName(Index);
}

void UHodgeGroundedLocomotion::NativePostEvaluateAnimation()
{
    Super::NativePostEvaluateAnimation();
    if (auto* Mesh = GetSkelMeshComponent())
    {
        // Virtual foot bones retain the input pose; the IK modifies the deforming leg chain.
        PreviousRawFeet[0] = Mesh->GetSocketTransform(TEXT("VB ik_foot_left"), RTS_World);
        PreviousRawFeet[1] = Mesh->GetSocketTransform(TEXT("VB ik_foot_right"), RTS_World);
        bHavePose = Mesh->DoesSocketExist(TEXT("VB ik_foot_left")) && Mesh->DoesSocketExist(TEXT("VB ik_foot_right"));
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
    PreviousActorLocation = Character->GetActorLocation();
    if (Teleported || Interrupted)
    {
        ActionIndex = INDEX_NONE; GroundActionTime = 0.f; TurnDelay = 0.f;
        if (Teleported) bHadMovement = false;
    }
    if (LabSpeed > 80.f) bLastWalk = Lab && Lab->bWalking;
    if (LabSpeed > 15.f) bHadMovement = true;
    if (ActionIndex != INDEX_NONE)
    {
        GroundActionTime += Dt;
        if (!GroundActions.IsValidIndex(ActionIndex) || !GroundActions[ActionIndex] || GroundActionTime >= GroundActions[ActionIndex]->GetPlayLength())
        { ActionIndex=INDEX_NONE; }
    }
    if (!Interrupted && ActionIndex == INDEX_NONE && LabSpeed < 10.f)
    {
        if (bHadMovement)
        {
            // Contact information selects the first support foot, not the FeetCrossing curve.
            const float L = GetCurveValue(TEXT("FootContact_L"));
            const float R = GetCurveValue(TEXT("FootContact_R"));
            bSupportLeft = L >= R;
            ActionIndex = (bLastWalk ? 0 : 2) + (bSupportLeft ? 0 : 1);
            GroundActionTime = 0.f; bHadMovement = false;
        }
        else if (Lab && Lab->bCombatFacing)
        {
            const float Delta = FRotator::NormalizeAxis(Character->GetBaseAimRotation().Yaw - Character->GetActorRotation().Yaw);
            TurnDelay = FMath::Abs(Delta) > TurnThreshold ? TurnDelay + Dt : 0.f;
            if (TurnDelay >= 0.2f)
            {
                ActionIndex = Delta < 0.f ? 4 : 5; GroundActionTime=0.f; TurnStartYaw=Character->GetActorRotation().Yaw; TurnDelay=0.f;
            }
        }
    }
    bGroundStopWalkL = ActionIndex==0; bGroundStopWalkR = ActionIndex==1;
    bGroundStopRunL = ActionIndex==2; bGroundStopRunR = ActionIndex==3;
    bGroundTurnL = ActionIndex==4; bGroundTurnR = ActionIndex==5;
    bGroundMove = !bLabFall && ActionIndex == INDEX_NONE && (LabSpeed >= 5.f || HasInput);
    bGroundIdle = !bLabFall && ActionIndex == INDEX_NONE && !bGroundMove;
    RotationTarget = Character->GetActorRotation().Yaw;
    if (IsTurningInPlace())
    {
        const UCurveFloat* Curve = bGroundTurnL ? TurnYawLeft.Get() : TurnYawRight.Get();
        if (Curve) RotationTarget = TurnStartYaw + Curve->GetFloatValue(GroundActionTime);
    }
    const FTransform ComponentTransform = Mesh->GetComponentTransform();
    const bool AllowLock = bHavePose && !Teleported && !Interrupted && !bGroundMove && LabSpeed < 15.f;
    for (int32 I=0; I<2; ++I)
    {
        const float Curve = AllowLock ? FMath::Clamp(GetCurveValue(I==0 ? TEXT("FootLock_L") : TEXT("FootLock_R")),0.f,1.f) : 0.f;
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
